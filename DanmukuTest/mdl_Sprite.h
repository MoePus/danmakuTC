#ifndef _m_s_H_
#define _m_s_H_

#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>

#include <unordered_map>
#include "mdl.h"
#include "STPK.h"
//#include<DxErr.h>
#include<xnamath.h>
#include <vector>
#include "thread"

enum MDL_SPRITE_OVERLAPP_MODE
{
	Ori_Alpha,
	All_Alpha,
	Add_Color,
	Sub_Color
};
struct texture
{
	ID3D11ShaderResourceView* colorMap_;
	ID3D11SamplerState* colorMapSampler_;

};

struct sprite
{
	XMMATRIX vpMatrix_;
	DWORD texture_handle;
	ID3D11Buffer* vertexBuffer_;
	ID3D11Buffer* mvpCB_;
	//ID3D11Buffer* indexBuffer_;//Slower
	XMFLOAT2 big;
};

struct VertexPos
{
	XMFLOAT3 pos;
	XMFLOAT2 tex0;
};


class sprite_ex
{
public:
	sprite_ex(DWORD sprite_handle);
	~sprite_ex();
	XMFLOAT2 scale;
	XMFLOAT2 position;
	float rotation;
	MDL_SPRITE_OVERLAPP_MODE OM;
	float* BECL;
	DWORD sprite_handle;
	XMMATRIX GetWorldMatrix();

};

class mdl_sprite
{
public:
	std::unordered_map<DWORD, texture> texturemap;
	std::unordered_map<DWORD, sprite> spritemap;
	std::vector<sprite_ex> RenList;
	BOOL init();
	mdl_sprite();
	~mdl_sprite();
	DWORD load_texture(char* filename);
	void unload_texture(DWORD texture_handle);

	DWORD mdl_sprite::init_sprite(DWORD texture_handle);
	void unload_sprite(DWORD sprite_handle);

	BOOL add2RenList(DWORD sprite_handle);
	BOOL remove4RenList(DWORD RenN);

private:
	friend void MDL_spriteRender(mdl_sprite* sp);
	void spriteRender();
	ID3D11VertexShader* solidColorVS_;
	ID3D11PixelShader* solidColorPS_;
	ID3D11InputLayout* inputLayout_;
	ID3D11SamplerState* colorMapSampler_;

};


void MDL_spriteRender(mdl_sprite* sp);


#endif