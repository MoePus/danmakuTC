#include "mdl.h"
#include "mdl_Sprite.h"
#include "windows.h"
#include "cmath"
#include <VECTOR>
#include <iostream>
#include <functional>
#include "thread"
#include <mutex>
#include <condition_variable>>

#include "libtcc\libtcc.h"
#pragma comment(lib,"libtcc.lib")
#include "immintrin.h"
#include <ppl.h>





mdl_sprite* gs;
long frameCount = 0;
long rendered = 0;

struct spritedes
{
	spritedes::spritedes(DWORD SH)
	{
		sprite_handle = SH;
	}


	BOOL rS = 1;
	XMFLOAT2 position;
	float rotation=0;
	
	DWORD sprite_handle;
	

};

class globalttime
{
public:
	globalttime::globalttime()
	{
		startTime = GetTickCount();
		lastTime = startTime;
		currentTime = lastTime;
	}
	void update()
	{
		lastTime = currentTime;
		currentTime = GetTickCount();
		interval = currentTime - lastTime;
	}
	DWORD getInter()
	{
		return 16;
	}
	DWORD getCurrent()
	{
		return currentTime;
	}
	DWORD getPassed()
	{
		return currentTime - startTime;
	}
private:
	DWORD startTime;
	DWORD lastTime;
	DWORD currentTime;
	DWORD interval;
}*gt;

class coordTime
{
public:
	coordTime::coordTime()
	{
		startTime = gt->getPassed();
	}
	DWORD getInter()
	{
		return 16;
	}
	DWORD getPassed()
	{
		return gt->getPassed()-startTime;
	}
private:
	DWORD startTime;

};

class msprite
{
public:
	msprite::msprite(){};
	msprite::msprite(char* filename)
	{
		teH = gs->load_texture(filename);
		spH = gs->init_sprite(teH);
		big = gs->spritemap[spH].big;
		
	}
	virtual void update()
	{
		
	}
	virtual void draw()
	{
		for (unsigned int i = 0; i < list.size(); i++)
		{
			int j = gs->RenList.size();
			gs->add2RenList(spH);
			gs->RenList[j].position = list[i].position;
			gs->RenList[j].rotation = list[i].rotation;
		}
	}

protected:
	XMFLOAT2 big;
	DWORD teH;
	DWORD spH;
	std::vector<std::vector<DWORD>> spHVector;
	std::vector < spritedes >  list;
};


class player :public msprite
{
public:
	player::player()
	{

	}
	player::~player()
	{

	}
	void update()
	{
		POINT mp;
		GetCursorPos(&mp);
		ScreenToClient(g_hWnd, &mp);
		//mp.y = w_height - mp.y;
		//if (mp.x > w_width)mp.x = w_width;
		//else if (mp.x < 0)mp.x = 0;

		//if (mp.y > w_height)mp.y = w_height;
		//else if (mp.y < 0)mp.y = 0;

		RECT rect;
		GetClientRect(g_hWnd, &rect);
		mp.x *= w_width / (float)(rect.right - rect.left);
		mp.y *= w_height / (float)(rect.bottom - rect.top);

		pos = XMFLOAT2(mp.x/2, mp.y/2);
	}
	void draw()
	{
		;
	}
	XMFLOAT2* getLpPos()
	{
		return &pos;
	}
private:
	XMFLOAT2 pos;
}*ziji;

class __coord
{
public:
	friend class danmu;
	__coord::__coord(XMFLOAT2 __pos, std::function<int(void* lp,int catalogue)> __proc, int __catalogue,BYTE* __lpmem,int __bn)
	{

		centre = __pos;
		proc = __proc;
		catalogue = __catalogue;
		lpmem = __lpmem;
		bn = __bn;
		mt = new coordTime;
		
	}
	__coord::~__coord()
	{
		delete mt;
	}

	XMFLOAT2* getCentre()
	{
		return &centre;
	}

	XMFLOAT2* getPos()
	{
		return &position;
	}



	byte* getLpmem()
	{
		return lpmem;
	}

	int update()
	{
		lastPos = position;
		int ret = proc(this, catalogue);
		float x = position.x - lastPos.x;
		float y = position.y - lastPos.y;
		roat = atan2f(-y, x);
		switch (ret)
		{
		case 0:
			break;
		default:
			break;
		};

		return ret;
	}
	float getRoat()
	{
		return roat;
	}

	friend DWORD ScriptMyPassed(__coord* lp);
	friend DWORD ScriptMyInter(__coord* lp);

	int bn;

protected:
	coordTime* mt;
	std::function<int(__coord* lp, int catalogue)> proc;
private:

	XMFLOAT2 position;
	XMFLOAT2 centre;
	float roat;

	XMFLOAT2 lastPos;

	int catalogue;
	BYTE* lpmem;
	
};


typedef void(*fun_create)(void* lp);
typedef int(*fun_proc)(void* lp,int catalogue);


 class danmu :public msprite
{
public:
	danmu::danmu(char* filename , char* cutScene="DT\\bullet1_Slicer.json")
	{
		idFile = "id.c";
		danmuLst.resize(16384);
		for (int i = 0; i < 16384; i++)
		{
			danmuLst[i].first = 0;
		}
		memPool = (BYTE*)_aligned_malloc(1024 * 1024,32);
		ZeroMemory(memPool, 1024 * 1024);
		
		teH = gs->load_texture(filename);
		//spH = gs->init_sprite(teH);
		spHVector = gs->init_cut_sprite(teH, cutScene);
		//big = gs->spritemap[spH].big;
		parseDid = parseScript();
	


		byte* _or = (byte*)_aligned_malloc(32, 32);
		memset(_or, 255, 32);
		o = _mm256_load_si256((__m256i*)_or);
		_aligned_free(_or);
		memUsage = (byte*)_aligned_malloc(2048, 32);
		memset(memUsage, 255, 2048);

		num = 0;


		std::thread iN(std::mem_fn(&danmu::NotifyFileChange),this);
		iN.detach();

		waitForIdChange = 0;

	}
	danmu::~danmu()
	{
		
		for (unsigned int i = 0; i < danmuLst.size(); i++)
		{
			delete danmuLst[i].first;
		}
		danmuLst.clear();
		danmuLst.~vector();
		_aligned_free(memPool);
		_aligned_free(memUsage);
		CloseHandle(Hfile);
		//gs->unload_sprite(spH);
		for (unsigned int i = 0; i < spHVector.size(); i++)
		{
			for (unsigned int j = 0; j < spHVector[i].size(); j++)
			{
				gs->unload_sprite(spHVector[i][j]);
			}
		}

		gs->unload_texture(teH);

	}
	void* initADanmu(XMFLOAT2 __initPos,int type, int catalogue)
	{

		if (size() == 16384)
		{
			return 0;
		}
		BYTE* lpmem = NULL; 
		int i,j,h;
		
		for (i = 0; i < 64; i++)
		{
			
			__m256i mmu = _mm256_load_si256((__m256i*)(memUsage + 32 * i));

			if (_mm256_testz_si256(o, mmu))
				continue;
		
			 BYTE tmp;
			for (j = 0; j < 32; j++)
			{
				if (memUsage[32 * i + j])
				{
					tmp = memUsage[32 * i + j];
					break;
				}
			}

			for (h = 0; h < 8; h++)
			{
				if (tmp & (1<<h) )
				{
					memUsage[32 * i + j] &=(0xff - (1 << h));
					lpmem = ((32 * i + j) * 8 + h) * 64 + memPool;
					break;
				}

			}
			break;

		}

		int bn = (32 * i + j) * 8 + h;
		__coord* lpCoord = new __coord(__initPos, fun_p, catalogue, lpmem, bn);
		


		danmuLst[bn].first = lpCoord;;
		danmuLst[bn].second = type;

		num++;

		return lpCoord;
	}
	void update()
	{
		if (waitForIdChange | !parseDid) return;
		fun_c(this);

		concurrency::parallel_for(0, 16384, [this](int i)
		{
			this->perUpdate(i);
		});


	}
	void draw()
	{
	
		for (unsigned int i = 0; i < danmuLst.size(); i++)
		{
			if (!danmuLst[i].first) continue;
			int j = gs->RenList.size();
			gs->add2RenList(spHVector[danmuLst[i].second>>8][danmuLst[i].second & 0xff]);
			XMFLOAT2 pos = *danmuLst[i].first->getPos();
			gs->RenList[j].position = XMFLOAT2(2.0f*pos.x,w_height-2.0f*pos.y);
			gs->RenList[j].rotation = danmuLst[i].first->getRoat()- XM_2PI / 4.0f;
			gs->RenList[j].scale = XMFLOAT2(1.3f,1.3f);
		}
	
	}
	DWORD size()
	{
		return num;
	}
	BOOL waitForIdChange;
private:
	std::string idFile;
	HANDLE Hfile;
	BOOL parseDid;
	void NotifyFileChange()
	{
		FILETIME writeTime;
		GetFileTime(Hfile, 0, 0, &writeTime);

		while (1)
		{
			Sleep(500);
			FILETIME lastWriteTime;
			GetFileTime(Hfile, 0, 0, &lastWriteTime);
			if (lastWriteTime.dwLowDateTime - writeTime.dwLowDateTime > 0)
			{
				waitForIdChange = 1;
				break;
			}
		}
	}


	BYTE* memPool;
	BYTE* memUsage;

	__m256i o;
	BOOL parseScript();
	std::vector<std::pair<__coord*,int>> danmuLst;
	std::function<void(void* lp)> fun_c;
	std::function<int(void* lp, int catalogue)> fun_p;

	int num;
	std::mutex numMutex;
	
	void perUpdate(int i)
	{
		
			if (danmuLst[i].first)
				if (danmuLst[i].first->update()<0)
				{
					ZeroMemory(danmuLst[i].first->getLpmem(), 64);
					int tmp = danmuLst[i].first->bn;

					memUsage[tmp / 8] |= (1 << (tmp % 8));
					delete danmuLst[i].first;
					danmuLst[i].first = 0;
					numMutex.lock();
					num--;
					numMutex.unlock();

				}
	}

};


std::vector<msprite*> spriteLst;


long ScriptGetPassed()
{

	//return gt->getPassed();
	return frameCount * 16;
}


DWORD ScriptMyPassed(__coord* lp)
{
	return lp->mt->getPassed();
}

DWORD ScriptMyInter(__coord* lp)
{
	return lp->mt->getInter();
}

XMFLOAT2* ScriptGetLpPos(__coord* lp)
{
	return lp->getPos();
}

XMFLOAT2* ScriptGetLpCentre(__coord* lp)
{
	return lp->getCentre();
}

BYTE* ScriptGetLpmem(__coord* lp)
{
	if (lp)
	return lp->getLpmem();
	else return 0;
}

XMFLOAT2*  ScriptGetLpZijiPos()
{
	return ziji->getLpPos();
}
std::mutex usageMutex;
void* ScriptInit(danmu* lp, XMFLOAT2 pos, int type, int catalogue)
{
	usageMutex.lock();
	void* ret = lp->initADanmu(pos, type, catalogue);
	usageMutex.unlock();
	return ret;
}//ScriptFunctions


BOOL danmu::parseScript()
{
	TCCState *s;
	//fun_create fun_c;
	//fun_proc fun_p;
	void *mem;
	int size;

	s = tcc_new();
	if (!s) {
		//fprintf(stderr, "Could not create tcc state\n");
		return 0;
	}

	
	tcc_set_output_type(s, TCC_OUTPUT_MEMORY);

	Hfile = CreateFile(idFile.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	if (GetLastError() != 0)
	{
		return 0;
	}

	DWORD fsize = GetFileSize(Hfile, 0);
	char* myDanmu = (char*)malloc(fsize+1);
	ZeroMemory(myDanmu, fsize + 1);
	DWORD rb = 0;
	ReadFile(Hfile, myDanmu, fsize, &rb, 0);

	if (tcc_compile_string(s, myDanmu) == -1)
		return 0;
	free(myDanmu);


	tcc_add_symbol(s, "getPassed", ScriptGetPassed);
	tcc_add_symbol(s, "getMyPassed", ScriptMyPassed);
	tcc_add_symbol(s, "getMyInter", ScriptMyInter);
	tcc_add_symbol(s, "getLpPos", ScriptGetLpPos);
	tcc_add_symbol(s, "getZijiLpPos", ScriptGetLpZijiPos);
	tcc_add_symbol(s, "getLpCentre", ScriptGetLpCentre);
	tcc_add_symbol(s, "getLpMem", ScriptGetLpmem);
	tcc_add_symbol(s, "init", ScriptInit);
	
	/* get needed size of the code */
	size = tcc_relocate(s, NULL);
	if (size == -1)
		return 0;

	/* allocate memory and copy the code into it */
	mem = malloc(size);
	tcc_relocate(s, mem);

	/* get entry symbol */
	fun_c = (fun_create)tcc_get_symbol(s, "create");
	fun_p = (fun_proc)tcc_get_symbol(s, "proc");

	/* delete the state */
	tcc_delete(s);

	return 1;

}

static danmu*id;



HANDLE renderLock;
std::mutex renderMutex;

extern HANDLE rExitLock,mExitLock;

void Render()
{

	while (1)
	{
	rendered++;
	WaitForSingleObject(renderLock, -1);
	renderMutex.lock();

	MDL_spriteRender(gs);

	renderMutex.unlock();

	if (!WaitForSingleObject(rExitLock, 0))
		break;

	MDL_RenderPresent();
	MDL_clear();

	}
	ReleaseSemaphore(mExitLock, 1, 0);
}

void renderInit()
{

	timeBeginPeriod(1);

	renderLock = CreateSemaphore(0, 0, 1, "RenderLock");

	gs = new mdl_sprite;
	ziji = new player;
	gs->RenList.reserve(16384);
	id = new danmu("DT\\bullet1.png");
	spriteLst.push_back(ziji);
	spriteLst.push_back(id);

	std::thread rl(Render);
	rl.detach();
	gt = new globalttime;
}


void Process(HANDLE frameUpdateNotify)
{

	while (1)
	{
	WaitForSingleObject(frameUpdateNotify, -1);
	frameCount++;

	gt->update();


	for (int i = 0; i < spriteLst.size(); i++)
	{
		spriteLst[i]->update();
	}


		if (id->waitForIdChange)
		{
			delete id;
			id = new danmu("DT\\bullet1.png");
			delete gt;
			gt = new globalttime;
			spriteLst.clear();
			spriteLst.push_back(id);
			continue;
		}
		if (renderMutex.try_lock())
		{
			for (int i = 0; i < spriteLst.size(); i++)
			{
				spriteLst[i]->draw();
			}
			renderMutex.unlock();
			ReleaseSemaphore(renderLock, 1, 0);
		}



	char danmuNum[88] = { 0 };
	sprintf_s(danmuNum, "skipped:%u num:%u",frameCount-rendered, id->size());
	SetWindowTextA(g_hWnd, danmuNum);


	}
}