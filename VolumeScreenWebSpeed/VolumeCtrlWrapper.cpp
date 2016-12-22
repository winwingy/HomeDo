#include "StdAfx.h"
#include "NotScreenSaveControllor.h"
#include <assert.h>
#include "EasyWindow.h"
#include "WinDefine.h"


VolumeCtrlWrapper::VolumeCtrlWrapper(void)
    : hWnd_(nullptr)
    , forcegroundHwnd_(nullptr)
{
}


VolumeCtrlWrapper::~VolumeCtrlWrapper(void)
{
}

LRESULT CALLBACK EasyWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
        {
            SetTimer(hWnd, 1003, 5000, nullptr);
            break;
        }
        case WM_TIMER:
        {
            KillTimer(hWnd, wParam);
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps = { 0 };
            HDC hdc = BeginPaint(hWnd, &ps);
            char* text = "Not Screen Save OK";
            RECT rect;
            GetClientRect(hWnd, &rect);
            DrawText(hdc, text, strlen(text), &rect,
                     DT_VCENTER | DT_CENTER | DT_SINGLELINE);
            EndPaint(hWnd, &ps);
            break;
        }
        default:
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
            break;
        }
    }
    return 0;

}


void VolumeCtrlWrapper::StopNotScreenSave(HWND hwnd, bool playSound)
{
    easyWindow_->Close();
    KillTimer(hwnd, TIMER_NOT_SCREEN_SAVE);
    KillTimer(hwnd, TIMER_NOT_SCREEN_SAVE_MAX);
    forcegroundWindowNotScreenSaveHwnd_ = nullptr;
    if (playSound)
    {
        PlaySoundHappy(0, 2);
    }
}

VOID CALLBACK VolumeCtrlWrapper::ScreenTimerProcSta(
    HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD userDefine)
{
    VolumeCtrlWrapper* pThis = reinterpret_cast < VolumeCtrlWrapper* >
        (userDefine);
    assert(pThis);
    if (pThis)
    {
        pThis->ScreenTimerProc(hwnd, uMsg, idEvent, userDefine);
    }
}

void VolumeCtrlWrapper::ScreenTimerProc(
    HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD userDefine)
{
    switch (uMsg)
    {
        case TIMER_NOT_SCREEN_SAVE:
        {

            break;
        }
        case TIMER_NOT_SCREEN_SAVE_MAX:
        {

            break;
        }
        default:
        break;
    }
}

HWND VolumeCtrlWrapper::NotScreenSave(HWND hWndParent, bool showDlg)
{
    if (forcegroundHwnd_) // 多按一次就关闭 （屏幕不屏保）
    {
        StopNotScreenSave(hWndParent, true);
        return;
    }

    HWND foregroundWindow = ::GetForegroundWindow();
    if (foregroundWindow)
    {
        forcegroundHwnd_ = foregroundWindow;
        SetTimer(hWndParent, TIMER_NOT_SCREEN_SAVE,
                 WinDefine::GetInstance()->notScreenSavePerInputTime_ * 1000,
                 ScreenTimerProcSta);
        string strValue = GetValueFromConfig(CONFIG_SET, "notScreenSavePerInputMAXTime", "120", CONFIG_INF_FILENAME);
        SetTimer(hWnd, TIMER_NOT_SCREEN_SAVE_MAX, atoi(strValue.c_str()) * 60 * 1000, TimerProc);
        notScreenSaveCanTryCntLeave_ = WinDefine::GetInstance()->notScreenSaveCanTryCnt_;
        strValue = GetValueFromConfig(CONFIG_SET, "notScreeenSaveMsgBox", "0", CONFIG_INF_FILENAME);
        if (strValue != "0")
        {
            easyWindow_->Create(nullptr, 600, 0, 200, 50);
        }
        PlaySoundHappy(0, 6);
    }
}
}

void VolumeCtrlWrapper::Close()
{
    SendMessage(hWnd_, WM_CLOSE, 0, 0);
}

VolumeCtrlWrapper::operator HWND()
{
    return hWnd_;
}

HWND VolumeCtrlWrapper::operator &()
{
    return hWnd_;
}
