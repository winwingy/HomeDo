#pragma once

#include "resource.h"
class WinControlTool;

class VolumeScreenWebSpeed
{
public:
    VolumeScreenWebSpeed(HINSTANCE hInst);
    ~VolumeScreenWebSpeed();

    bool Init();

    HWND Create(HWND hWnd);
    void Show();

private: 
    bool MyRegisterClass(HINSTANCE hInstance, const TCHAR* szWindowClass);
    LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK WndProcSta(HWND hWnd, UINT message,
                                WPARAM wParam, LPARAM lParam);
    HINSTANCE hInst_;
    HANDLE mutexHandle_;
    HWND hWnd_;
    WinControlTool* winControlTool_;
};
