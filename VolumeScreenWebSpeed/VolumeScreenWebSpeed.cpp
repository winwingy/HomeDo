#include "stdafx.h"
#include "VolumeScreenWebSpeed.h"
#include "config.h"
#include <assert.h>
#include "WinDefine.h"
#include "WinContorlTool.h"

VolumeScreenWebSpeed::VolumeScreenWebSpeed()
    : WindowControl()
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
    Config* config = Config::GetShared();
    bool ret = config->Init("VolScrConfig.ini", "VolScrConfig_job.int");
    assert(ret);
    mutexHandle_ = ::CreateMutex(nullptr, FALSE, "VolumeScreenWebSpeedMutex");
    return mutexHandle_ == INVALID_HANDLE_VALUE;
}

void VolumeScreenWebSpeed::Show()
{
    Config* config = Config::GetShared();
    int showValue = config->GetValue(CONFIG_SET, "IsShow", 0);
    __super::Show(!!showValue);
}

// LRESULT CALLBACK VolumeScreenWebSpeed::WndProcSta(
//     HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// {
//     if (message == WM_NCCREATE)
//     {
//         LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(
//             lParam);
//         VolumeScreenWebSpeed* thisWnd = reinterpret_cast<VolumeScreenWebSpeed*>(
//             createStruct->lpCreateParams);
//         ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(thisWnd));
//     }
//     else
//     {
//         VolumeScreenWebSpeed* thisWnd = reinterpret_cast<VolumeScreenWebSpeed*>(
//             ::GetWindowLongPtr(hWnd, GWLP_USERDATA));
//         if (thisWnd)
//         {
//             return thisWnd->WndProc(hWnd, message, wParam, lParam);
//         }
//     }
//     return DefWindowProc(hWnd, message, wParam, lParam);
// }

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
                break;
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
    }
    return __super::WndProc(hWnd, message, wParam, lParam);
}
