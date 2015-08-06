#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include "ctime"
#include "mdl.h"
#include "STPK.h"
#include "thread"

extern void renderInit();
extern void Process(HANDLE state);



WORD MKreflect=0;

int w_width = 368*2;
int w_height = 432*2;

HANDLE rExitLock = NULL;
HANDLE mExitLock = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_LBUTTONUP:
		MKreflect |= 1;
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

void doRenderLoop()
{
	renderInit();
	HANDLE pplock= CreateSemaphore(0, 0, 1, "PPLock");

	std::thread p(Process, pplock);
	p.detach();


	while (1)
	{
	Sleep(16);
	ReleaseSemaphore(pplock, 1, 0);
	
	MKreflect = 0;
	}
}

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	srand(time(0));
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

	if (FAILED(InitWindow(hInstance, nCmdShow,"id-", "MDLClass_koishi",WndProc)))
        return 0;

    if( FAILED( InitDevice() ) )
    {
        CleanupDevice();
        return 0;
    }
	char IDXPATH[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,IDXPATH);
	strcat(IDXPATH, "\\dt.idx");
	STPK_init(IDXPATH);

	

    // Main message loop
    MSG msg = {0};
	

	rExitLock = CreateSemaphore(0, 0, 1, "RenderExitLock");
	mExitLock = CreateSemaphore(0, 0, 1, "mainExitLock");

	std::thread rl(doRenderLoop);
	rl.detach();
    while( WM_QUIT != msg.message )
    {
		
        if( GetMessage( &msg, NULL, 0, 0 )>0)
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
  
    }
	ReleaseSemaphore(rExitLock ,1, 0);
	rl.~thread();
	WaitForSingleObject(mExitLock, -1);

	
    CleanupDevice();

    return ( int )msg.wParam;
}


