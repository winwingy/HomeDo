#include "stdafx.h"
#include "VolumeScreenWebSpeed.h"
#include "tool/config.h"
#include "controller/WinDefine.h"
#include "controller/VolumeScreenWebSpeedControl.h"
#include "view/WindowControl.h"

VolumeScreenWebSpeed::VolumeScreenWebSpeed()
    : WindowControl()
    , mutexHandle_(nullptr)
    , controller_(new VolumeScreenWebSpeedControl())
{

}

VolumeScreenWebSpeed::~VolumeScreenWebSpeed()
{
    if (mutexHandle_)
    {
        CloseHandle(mutexHandle_);
    }
}

bool VolumeScreenWebSpeed::Init()
{
	mutexHandle_ = ::CreateMutex(nullptr, FALSE, "VolumeScreenWebSpeed@#&*%#Mutex");
	if (!mutexHandle_)
		return false;

	DWORD err = GetLastError();
	if (ERROR_ALREADY_EXISTS == err)
		return false;

	return true;
}

void VolumeScreenWebSpeed::SetVisible()
{
    Config* config = Config::GetShared();
    int showValue = config->GetValue(CONFIG_SET, "IsShow", 0);
    __super::SetVisible(!!showValue);
}

bool VolumeScreenWebSpeed::OnCreate(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* result)
{
    controller_->OnCreate(hWnd, uMsg, wParam, lParam);
    return __super::OnCreate(hWnd, uMsg, wParam, lParam, result);
}

bool VolumeScreenWebSpeed::OnTimer(
    HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime, LRESULT* result)
{
    controller_->OnTimer(hWnd, uMsg, idEvent, dwTime);
    return __super::OnTimer(hWnd, uMsg, idEvent, dwTime, result);
}

bool VolumeScreenWebSpeed::OnHotKey(
    HWND hWnd, UINT uMsg, int idHotKey, LPARAM lParam, LRESULT* result)
{
    controller_->OnHotKey(hWnd, uMsg, idHotKey, lParam);
    return __super::OnHotKey(hWnd, uMsg, idHotKey, lParam, result);
}

bool VolumeScreenWebSpeed::WndProc(
    UINT message, WPARAM wParam, LPARAM lParam, LRESULT* lResult)
{
//     switch (message)
//     {
//         case WM_CLOSE:
//         {
//             int a = 10;
//             break;
//         }
//         case WM_QUIT:
//         {
//             int c = 10;
//             break;
//         }
//         case WM_DESTROY:
//         {
//             PostQuitMessage(0);
//             break;
//         }
//         default:
//         break;
//     }
	bool ret = controller_->WndProc(hWnd_, message, wParam, lParam, lResult);
	if (ret)
		return true;

    return __super::WndProc(message, wParam, lParam, lResult);
}
