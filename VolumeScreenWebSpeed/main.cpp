// VolumeScreenWebSpeed.cpp : ����Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include "VolumeScreenWebSpeed.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    MSG msg;
    HACCEL hAccelTable = nullptr;
    VolumeScreenWebSpeed volumeScreenWebSpeed;
    volumeScreenWebSpeed.Init();
    volumeScreenWebSpeed.Show();
    // ����Ϣѭ��:
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}
