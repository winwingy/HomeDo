#pragma once

#include "resource.h"
#include "WindowControl.h"
class WinControlTool;

class VolumeScreenWebSpeed : public WindowControl
{
public:
    VolumeScreenWebSpeed();
    ~VolumeScreenWebSpeed();

    bool Init();
    void Show();

private: 
    bool MyRegisterClass(HINSTANCE hInstance, const TCHAR* szWindowClass);
    virtual LRESULT WndProc(HWND hWnd, UINT message,
                            WPARAM wParam, LPARAM lParam) override;
//     LRESULT CALLBACK WndProcSta(HWND hWnd, UINT message,
//                                 WPARAM wParam, LPARAM lParam);

    HANDLE mutexHandle_;
    WinControlTool* winControlTool_;
};
