#include "windows.h"
#include "STPK.h"

HANDLE STPK_HIDX, STPK_HBIN;
mIDX* IDXattr;
int STPK_filenum;
BOOL STPK_INIT;

void STPK_init(char* IDX)
{
	if (!STPK_INIT)
	{
		char* tpackName = IDX + strlen(IDX);
		while (*--tpackName != '\\'){ if (*tpackName == '.') *tpackName = 0; }; tpackName++;
		char packName[32] = { 0 };
		strcpy(packName, tpackName);
		strcat(packName, ".IDX");
		STPK_HIDX = CreateFile(packName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
		strcpy(packName, tpackName);
		strcat(packName, ".BIN");
		STPK_HBIN = CreateFile(packName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);


		DWORD IDXSIZE = GetFileSize(STPK_HIDX, NULL);
		STPK_filenum = IDXSIZE / 3 / sizeof(DWORD);

		DWORD rb;
		IDXattr = (mIDX*)malloc(sizeof(mIDX)*STPK_filenum);
		ReadFile(STPK_HIDX, IDXattr, IDXSIZE, &rb, NULL);
		CloseHandle(STPK_HIDX);
		STPK_INIT = TRUE;
	}
}
DWORD STPK_dec(char* src, char* dest, DWORD srcSize, DWORD key)
{
	for (DWORD* i = (DWORD*)src; i < (DWORD*)src + srcSize / sizeof(DWORD); i++)
	{
		*i ^= key;

	}

	DWORD ORI_SIZE = NULL;
	memcpy(&ORI_SIZE, src, 4);
	DWORD roll = (srcSize - 4) / 0x40;
	for (unsigned int i = 0; i < roll; i++)
	{
		char* tmpDest = dest + i * 0x40;
		for (char* j = src + 4 + i * 0x40; j < src + 4 + 0x40 + i * 0x40; j += 2)
		{
			__asm
			{
					MOV EDX, j
					MOV AX, WORD ptr[EDX]
					SHL EAX, 0x10
					SHR EAX, 0xC
					SHL AX, 4
					SHL EAX, 4
					SHR AX, 8
					MOV EDX, j
					INC EDX
					MOV AH, BYTE ptr[EDX]
					SHL AX, 4
					SHR EAX, 8
					LEA EDX, tmpDest
					MOV EDX, DWORD ptr[EDX]
					MOV Byte ptr[EDX], AH
					MOV Byte ptr[EDX + 0x20], AL
			}
			tmpDest++;
		}
	}


	return ORI_SIZE;
}

STPK_ret STPK_read(DWORD HASH)
{
	for (int i = 0; i < STPK_filenum; i++)
	{
		if (IDXattr[i].HASH == HASH)
		{
			DWORD rb;
			SetFilePointer(STPK_HBIN, IDXattr[i].OFFSET, NULL, FILE_BEGIN);
			char* src = (char*)malloc(IDXattr[i].SIZE);
			char* dest = (char*)malloc(IDXattr[i].SIZE - 4);
			ReadFile(STPK_HBIN, src, IDXattr[i].SIZE, &rb, NULL);
			DWORD filesize = STPK_dec(src, dest, IDXattr[i].SIZE, ~0x99999999);
			free(src);
			return {filesize,dest};
		}
	}
	return {NULL,NULL};
}