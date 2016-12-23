#include "stdafx.h"
#include "WindowControl.h"

namespace
{
    const TCHAR* ClassName = _T("VolumeScreenWebSpeedClass");
    const TCHAR* WindowsName = _T("VolumeScreenWebSpeed");
}

bool WindowControl::MyRegisterClass(HINSTANCE hInstance,
                                    const TCHAR* szWindowClass)
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProcSta;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    //任务切换时看到的图标（16*16）、 exe上的图标（32*32）
    wcex.hIcon = (HICON)LoadImage(NULL, "icon_show.ico",
                                  IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = szWindowClass;
    //窗口右上角图标、 任务栏图标、 任务管理器看到的图标（16*16）
    wcex.hIconSm = (HICON)LoadImage(NULL, "icon_show.ico",
                                    IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    bool ret = false;
    if (RegisterClassEx(&wcex))
    {
        ::UnregisterClass(wcex.lpszClassName, hInstance);
        ret = !!RegisterClassEx(&wcex);
    }
    return ret;
}

WindowControl::WindowControl()
    : hInst_(reinterpret_cast<HINSTANCE>(GetModuleHandle(NULL)))
{

}

WindowControl::~WindowControl()
{

}

HWND WindowControl::Create(HWND hWnd, int x, int y, int width, int height)
{
    bool ret = MyRegisterClass(hInst_, ClassName);
    DWORD styleEx = 0;
    DWORD style = WS_OVERLAPPEDWINDOW;
    CreateParam(&styleEx, &style);
    hWnd_ = ::CreateWindowExA(
        styleEx, ClassName, WindowsName, style,
        x, y, x+width, y+height, NULL, NULL, hInst_,this);
    return hWnd_;
}

void WindowControl::CreateParam(DWORD* styleEx, DWORD* style)
{

}

void WindowControl::Show(bool visible)
{
    ShowWindow(hWnd_, visible? SW_SHOW : SW_HIDE);
}

LRESULT CALLBACK WindowControl::WndProcSta(
    HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_NCCREATE)
    {
        LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(
            lParam);
        WindowControl* thisWnd = reinterpret_cast<WindowControl*>(
            createStruct->lpCreateParams);
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(thisWnd));
    }
    else
    {
        WindowControl* thisWnd = reinterpret_cast<WindowControl*>(
            ::GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if (thisWnd)
        {
            return thisWnd->WndProc(hWnd, message, wParam, lParam);
        }
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT WindowControl::WndProc(
    HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hWnd, message, wParam, lParam);
}
