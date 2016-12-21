#include "stdafx.h"
#include "VolumeScreenWebSpeed.h"
#include "config.h"
#include <assert.h>
#include "WinDefine.h"
#include "WinContorlTool.h"

namespace
{
    const TCHAR* ClassName = _T("VolumeScreenWebSpeedClass");
    const TCHAR* WindowsName = _T("VolumeScreenWebSpeed");
}

bool VolumeScreenWebSpeed::MyRegisterClass(HINSTANCE hInstance,
                                           const TCHAR* szWindowClass)
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    //任务切换时看到的图标（16*16）、 exe上的图标（32*32）
    wcex.hIcon = (HICON)LoadImage(NULL, "icon_show.ico",
                                  IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCE(IDC_VOLUMESCREENWEBSPEED);
    wcex.lpszClassName = szWindowClass;
    //窗口右上角图标、 任务栏图标、 任务管理器看到的图标（16*16）
    wcex.hIconSm = (HICON)LoadImage(NULL, "icon_show.ico",
                                    IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    return !!RegisterClassEx(&wcex);
}

VolumeScreenWebSpeed::VolumeScreenWebSpeed(HINSTANCE hInst)
    : hInst_(hInst)
    , mutexHandle_(INVALID_HANDLE_VALUE)
    , winControlTool_(new WinControlTool())
{

}

VolumeScreenWebSpeed::~VolumeScreenWebSpeed()
{
    if (mutexHandle_ != INVALID_HANDLE_VALUE)
    {
        CloseHandle(mutexHandle_);
    }
    if (winControlTool_)
    {
        delete winControlTool_;
        winControlTool_ = nullptr;
    }
}

bool VolumeScreenWebSpeed::Init()
{
    mutexHandle_ = ::CreateMutex(nullptr, FALSE, "VolumeScreenWebSpeedMutex");
    return mutexHandle_ == INVALID_HANDLE_VALUE;
}

HWND VolumeScreenWebSpeed::Create(HWND hWnd)
{
    Config* config = Config::GetShared();
    bool ret = config->Init("VolScrConfig.ini", "VolScrConfig_job.int");
    assert(ret);
    ret = MyRegisterClass(hInst_, ClassName);
    hWnd_ = ::CreateWindow(
        ClassName, WindowsName, WS_OVERLAPPEDWINDOW,
        500, 300, 500, 300, NULL, NULL, hInst_, NULL);

    if (!hWnd_)
    {
        return nullptr;
    }


    return hWnd_;
}

void VolumeScreenWebSpeed::Show()
{
    Config* config = Config::GetShared();
    int showValue = config->GetValue(CONFIG_SET, "IsShow", 0);
    ShowWindow(hWnd_, showValue);
    UpdateWindow(hWnd_);
}

LRESULT CALLBACK VolumeScreenWebSpeed::WndProcSta(
    HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_NCCREATE)
    {
        LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(
            lParam);
        VolumeScreenWebSpeed* thisWnd = reinterpret_cast<VolumeScreenWebSpeed*>(
            createStruct->lpCreateParams);
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(thisWnd));
    }
    else
    {
        VolumeScreenWebSpeed* thisWnd = reinterpret_cast<VolumeScreenWebSpeed*>(
            ::GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if (thisWnd)
        {
            return thisWnd->WndProc(hWnd, message, wParam, lParam);
        }
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT VolumeScreenWebSpeed::WndProc(
    HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
        case WM_COMMAND:
        {
            wmId = LOWORD(wParam);
            wmEvent = HIWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
                case IDM_EXIT:
                {
                    DestroyWindow(hWnd);
                    break;
                }
                default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        }

        case WM_CREATE:
        {
            winControlTool_->OnCreate(hWnd, message, wParam, lParam);
            break;
        }
        case WM_HOTKEY:
        {
            winControlTool_->OnHotKey(hWnd, message, wParam, lParam);
            break;
        }
        case WM_PAINT:
        {
            hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此添加任意绘图代码...
            EndPaint(hWnd, &ps);
            break;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
