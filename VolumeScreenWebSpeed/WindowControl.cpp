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
    WNDCLASS reg = { 0 };
    bool isReg = !!::GetClassInfo(hInst_, szWindowClass, &reg);
    if (isReg && (wcex.lpfnWndProc != reg.lpfnWndProc))
    {
        ::UnregisterClass(wcex.lpszClassName, hInstance);
    }
    RegisterClassEx(&wcex);
    return true;
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
        x, y, width, height, NULL, NULL, hInst_,this);
    return hWnd_;
}

void WindowControl::CreateParam(DWORD* styleEx, DWORD* style)
{

}

void WindowControl::SetVisible(bool visible)
{
    ShowWindow(hWnd_, visible? SW_SHOW : SW_HIDE);
}

LRESULT CALLBACK WindowControl::WndProcSta(
    HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    bool handle = false;
    LRESULT lResult = 0;
    if (message == WM_NCCREATE)
    {
        LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(
            lParam);
        WindowControl* thisWnd = reinterpret_cast<WindowControl*>(
            createStruct->lpCreateParams);
        thisWnd->hWnd_ = hWnd;
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(thisWnd));
    }
    else
    {
        WindowControl* thisWnd = reinterpret_cast<WindowControl*>(
            ::GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if (thisWnd)
        {
            handle = thisWnd->WndProc(message, wParam, lParam, &lResult);
        }
    }
    if (!handle)
        return DefWindowProc(hWnd, message, wParam, lParam);
    else
        return lResult;
}

bool WindowControl::WndProc(UINT message, WPARAM wParam,
                            LPARAM lParam, LRESULT* lResult)
{
    bool handle = false;  
    switch (message)
    {
        case WM_CREATE:
        {
            handle = OnCreate(hWnd_, message, wParam, lParam, lResult);
            break;
        }
        case WM_TIMER:
        {
            handle = OnTimer(hWnd_, message, static_cast<UINT_PTR>(wParam),
                             static_cast<DWORD>(lParam), lResult);
            break;
        }
        case WM_HOTKEY:
        {
            handle = OnHotKey(hWnd_, message, static_cast<int>(wParam),
                              lParam, lResult);
            break;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps = { 0 };
            HDC hdc = BeginPaint(hWnd_, &ps);
            // TODO: 在此添加任意绘图代码...
            EndPaint(hWnd_, &ps);
            handle = true;
            break;
        }
        default:
        break;
    }
    return handle;
}

bool WindowControl::OnCreate(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* result)
{
    return false;
}

bool WindowControl::OnTimer(
    HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime, LRESULT* result)
{
    return false;
}

bool WindowControl::OnHotKey(
    HWND hWnd, UINT uMsg, int idHotKey, LPARAM lParam, LRESULT* result)
{
    return false;
}
