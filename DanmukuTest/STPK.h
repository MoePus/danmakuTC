#ifndef _STPK_
#define _STPK_
#include "windows.h"

struct mIDX
{
	DWORD HASH;
	DWORD OFFSET;
	DWORD SIZE;
};
struct STPK_ret
{

	DWORD SIZE;
	void* mem;
};

extern HANDLE STPK_HIDX, STPK_HBIN;
extern mIDX* IDXattr;
extern int STPK_filenum;
extern BOOL STPK_INIT;

void STPK_init(char* IDX);
DWORD STPK_dec(char* src, char* dest, DWORD srcSize, DWORD key);
STPK_ret STPK_read(DWORD HASH);


#endif