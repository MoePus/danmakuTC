#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>


#include <unordered_map>
#include "mdl.h"
//#include<DxErr.h>
#include<xnamath.h>
#include <vector>
#include "thread"
#include <iterator> 
#include "mdl_Sprite.h"



#define d3dDevice_ g_pd3dDevice
#define d3dContext_ g_pImmediateContext
float oBECL[4] = { 0.0f, 0.0f, 0.0f, 0.0f };


XMMATRIX sprite_ex::GetWorldMatrix()
{
	XMMATRIX _translation = XMMatrixTranslation(position.x, position.y, 0.0f);
	XMMATRIX _rotationZ = XMMatrixRotationZ(rotation);
	XMMATRIX _scale = XMMatrixScaling(scale.x, scale.y, 1.0f);

	return _rotationZ * _scale* _translation;
}

sprite_ex::sprite_ex(DWORD s_h)
{
	scale = {1.0f, 1.0f };
	position = { 0.0f, 0.0f };
	rotation = 0.0f;
	sprite_handle = s_h;

	BECL = oBECL;
	OM = Ori_Alpha;
}

sprite_ex::~sprite_ex()
{
}


BOOL mdl_sprite::init()
{
	char* fxPath = "DT\\TextureMap.fx";

	ID3DBlob* vsBuffer = 0;

	DWORD fxHandle = SpecialFNVHash(fxPath,fxPath+strlen(fxPath),NULL);
	STPK_ret fxM = STPK_read(fxHandle);
	bool compileResult = CompileD3DShader((char*)fxM.mem,fxM.SIZE, fxPath, "VS_Main", "vs_4_0", &vsBuffer);
	if (compileResult == false)
	{
		//DXTRACE_MSG("Error compiling the vertex shader!");
		return FALSE;

	}

	HRESULT d3dResult;

	d3dResult = d3dDevice_->CreateVertexShader(vsBuffer->GetBufferPointer(),
		vsBuffer->GetBufferSize(), 0, &solidColorVS_);

	if (FAILED(d3dResult))
	{

		if (vsBuffer)
			vsBuffer->Release();
		//DXTRACE_MSG("Error creating the vertex shader!");
		return FALSE;

	}

	D3D11_INPUT_ELEMENT_DESC solidColorLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	unsigned int totalLayoutElements = ARRAYSIZE(solidColorLayout);

	d3dResult = d3dDevice_->CreateInputLayout(solidColorLayout, totalLayoutElements,
		vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &inputLayout_);

	vsBuffer->Release();

	if (FAILED(d3dResult))
	{
		//DXTRACE_MSG("Error creating the input layout!");
		return FALSE;

	}

	ID3DBlob* psBuffer = 0;

	
	compileResult = CompileD3DShader((char*)fxM.mem, fxM.SIZE, fxPath, "PS_Main", "ps_4_0", &psBuffer);
	free(fxM.mem);

	if (compileResult == false)
	{
		//DXTRACE_MSG("Error compiling pixel shader!");
		return FALSE;

	}

	d3dResult = d3dDevice_->CreatePixelShader(psBuffer->GetBufferPointer(),
		psBuffer->GetBufferSize(), 0, &solidColorPS_);

	psBuffer->Release();

	if (FAILED(d3dResult))
	{
		//DXTRACE_MSG("Error creating pixel shader!");
		return FALSE;
	}




	ID3D11BlendState* alphaBlendState_;
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.RenderTarget[0].BlendEnable = TRUE;

		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = 0xf;

	float* blendFactor = oBECL;

	d3dDevice_->CreateBlendState(&blendDesc, &alphaBlendState_);
	d3dContext_->OMSetBlendState(alphaBlendState_, blendFactor, 0xFFFFFFFF);
	alphaBlendState_->Release();




	D3D11_SAMPLER_DESC colorMapDesc;
	
	ZeroMemory(&colorMapDesc, sizeof(colorMapDesc));
	colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;

	d3dResult = d3dDevice_->CreateSamplerState(&colorMapDesc, &colorMapSampler_);

	if (FAILED(d3dResult))
	{
		//DXTRACE_MSG("Failed to create color map sampler state!");
		return false;
	}


	return TRUE;
}
mdl_sprite::mdl_sprite()
{
	this->init();
}
mdl_sprite::~mdl_sprite()
{
	if (solidColorVS_) solidColorVS_->Release();
	if (solidColorPS_) solidColorPS_->Release();
	if (inputLayout_) inputLayout_->Release();
	solidColorVS_ = 0;
	solidColorPS_ = 0;
	inputLayout_ = 0;
}

DWORD mdl_sprite::load_texture(char* filename)
{
	DWORD texture_handle = SpecialFNVHash(filename, filename + strlen(filename),NULL);
	HRESULT d3dResult;
	ID3D11ShaderResourceView* colorMap_;



	STPK_ret tbM=STPK_read(texture_handle);
	d3dResult = D3DX11CreateShaderResourceViewFromMemory(d3dDevice_, tbM.mem, tbM.SIZE, 0, 0, &colorMap_, 0);
	free(tbM.mem);

	if (FAILED(d3dResult))
	{
		//DXTRACE_MSG("Failed to load the texture image!");
		return false;
	}


	texturemap[texture_handle].colorMap_ = colorMap_;


	return texture_handle;
}
void mdl_sprite::unload_texture(DWORD texture_handle)
{
	
	if (texturemap[texture_handle].colorMapSampler_) 
		texturemap[texture_handle].colorMapSampler_->Release();
	if (texturemap[texture_handle].colorMap_) 
		texturemap[texture_handle].colorMap_->Release();
	texturemap.erase(texture_handle);
}

DWORD mdl_sprite::init_sprite(DWORD texture_handle)
{
	HRESULT d3dResult;

	ID3D11Buffer* vertexBuffer_;
	ID3D11Buffer* mvpCB_;
	XMMATRIX vpMatrix_;

	ID3D11Resource* colorTex;
	texturemap[texture_handle].colorMap_->GetResource(&colorTex);

	D3D11_TEXTURE2D_DESC colorTexDesc;
	((ID3D11Texture2D*)colorTex)->GetDesc(&colorTexDesc);
	colorTex->Release();
	float halfWidth = (float)colorTexDesc.Width / 2.0f;
	float halfHeight = (float)colorTexDesc.Height / 2.0f;


	VertexPos vertices[] =
	{
		{ XMFLOAT3(halfWidth, halfHeight, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(halfWidth, -halfHeight, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-halfWidth, -halfHeight, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-halfWidth, -halfHeight, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-halfWidth, halfHeight, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(halfWidth, halfHeight, 1.0f), XMFLOAT2(1.0f, 0.0f) },
	};

	D3D11_BUFFER_DESC vertexDesc;
	ZeroMemory(&vertexDesc, sizeof(vertexDesc));
	vertexDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.ByteWidth = sizeof(VertexPos) * 6;

	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(resourceData));
	resourceData.pSysMem = vertices;

	d3dResult = d3dDevice_->CreateBuffer(&vertexDesc, &resourceData, &vertexBuffer_);

	if (FAILED(d3dResult))
	{
		//DXTRACE_MSG("Failed to create vertex buffer!");
		return false;
	}





	D3D11_BUFFER_DESC constDesc;
	ZeroMemory(&constDesc, sizeof(constDesc));
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.ByteWidth = sizeof(XMMATRIX);
	constDesc.Usage = D3D11_USAGE_DEFAULT;

	d3dResult = d3dDevice_->CreateBuffer(&constDesc, 0, &mvpCB_);

	if (FAILED(d3dResult))
	{
		return false;
	}




	XMMATRIX view = XMMatrixIdentity();
	XMMATRIX projection = XMMatrixOrthographicOffCenterLH(0.0f, (float)w_width, 0.0f, (float)w_height, 0.1f, 100.0f);
	vpMatrix_ = XMMatrixMultiply(view, projection);
	

	


	DWORD spriteorder = spritemap.size();
	sprite* st = &spritemap[spriteorder];
	st->mvpCB_ = mvpCB_;
	st->vertexBuffer_ = vertexBuffer_;
	memcpy(&st->vpMatrix_, &vpMatrix_, sizeof(vpMatrix_));
	st->big = { (float)colorTexDesc.Width, (float)colorTexDesc.Height };
	st->texture_handle = texture_handle;
	return spriteorder;
}
void mdl_sprite::unload_sprite(DWORD sprite_handle)
{
	if (spritemap[sprite_handle].vertexBuffer_) 
		spritemap[sprite_handle].vertexBuffer_->Release();


	if (spritemap[sprite_handle].mvpCB_)
		spritemap[sprite_handle].mvpCB_->Release();

	spritemap.erase(sprite_handle);
}

BOOL mdl_sprite::add2RenList(DWORD sprite_handle)
{
	RenList.push_back(sprite_handle);
	return TRUE;
}
BOOL mdl_sprite::remove4RenList(DWORD RenN)
{
	if (RenList.size() - 1 >= RenN)
	{

			RenList.erase(RenList.begin() + RenN);
			return TRUE;
	}
	return FALSE;
}



void mdl_sprite::spriteRender()
{



	static DWORD cmpSpriteHandle = -1;
	unsigned int stride = sizeof(VertexPos);
	unsigned int offset = 0;
	DWORD size = RenList.size();

	d3dContext_->IASetInputLayout(inputLayout_);
	d3dContext_->VSSetShader(solidColorVS_, 0, 0);
	d3dContext_->PSSetShader(solidColorPS_, 0, 0);
	d3dContext_->PSSetSamplers(0, 1, &colorMapSampler_);
	d3dContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (DWORD i = 0; i < size;i++)
	{

		sprite* st = &spritemap[RenList[i].sprite_handle];

		if (cmpSpriteHandle != RenList[i].sprite_handle)
	{
		cmpSpriteHandle = RenList[i].sprite_handle;
	d3dContext_->IASetVertexBuffers(0, 1, &st->vertexBuffer_, &stride, &offset);
	d3dContext_->PSSetShaderResources(0, 1, &texturemap[st->texture_handle].colorMap_);
	}

		
		XMMATRIX world = RenList[i].GetWorldMatrix();
		XMMATRIX vpMatrix;
		memcpy(&vpMatrix, &st->vpMatrix_, sizeof(vpMatrix));
		XMMATRIX mvp = XMMatrixMultiply(world, vpMatrix);
		mvp = XMMatrixTranspose(mvp);

		d3dContext_->UpdateSubresource(st->mvpCB_, 0, 0, &mvp, 0, 0);
		d3dContext_->VSSetConstantBuffers(0, 1, &st->mvpCB_);
		d3dContext_->Draw(6, 0);

	}

	RenList.clear();
}

void MDL_spriteRender(mdl_sprite* sp)
{
	sp->spriteRender();

}