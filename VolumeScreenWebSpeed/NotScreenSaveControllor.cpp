#include "StdAfx.h"
#include "NotScreenSaveControllor.h"
#include <assert.h>
#include "EasyWindow.h"
#include "WinDefine.h"


NotScreenSaveControllor::NotScreenSaveControllor(void)
	: hWnd_(nullptr)
    , forcegroundHwnd_(nullptr)
{
}


NotScreenSaveControllor::~NotScreenSaveControllor(void)
{
}

LRESULT CALLBACK EasyWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
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
			PAINTSTRUCT ps = {0};
			HDC hdc = BeginPaint(hWnd, &ps);
			char* text = "Not Screen Save OK";
			RECT rect;
			GetClientRect(hWnd, &rect);
			DrawText(hdc, text, strlen(text), &rect,
				DT_VCENTER|DT_CENTER|DT_SINGLELINE);
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


void NotScreenSaveControllor::StopNotScreenSave(HWND hwnd, bool playSound)
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

VOID CALLBACK NotScreenSaveControllor::ScreenTimerProcSta(
    HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD userDefine)
{
    NotScreenSaveControllor* pThis = reinterpret_cast<NotScreenSaveControllor*>
        (userDefine);
    assert(pThis);
    if (pThis)
    {
        pThis->ScreenTimerProc(hwnd, uMsg, idEvent, userDefine);
    }
}

void NotScreenSaveControllor::ScreenTimerProc(
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

HWND NotScreenSaveControllor::NotScreenSave(HWND hWndParent, bool showDlg)
{
    if (forcegroundHwnd_) // �ఴһ�ξ͹ر� ����Ļ��������
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

void NotScreenSaveControllor::Close()
{
	SendMessage(hWnd_, WM_CLOSE, 0, 0);
}

NotScreenSaveControllor::operator HWND()
{
	return hWnd_;
}

HWND NotScreenSaveControllor::operator &()
{
	return hWnd_;
}
