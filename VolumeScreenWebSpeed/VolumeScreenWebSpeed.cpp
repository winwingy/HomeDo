#include "stdafx.h"
#include "VolumeScreenWebSpeed.h"
#include "config.h"
#include <assert.h>
#include "WinDefine.h"
#include "VolumeScreenWebSpeedControl.h"

VolumeScreenWebSpeed::VolumeScreenWebSpeed()
    : WindowControl()
    , mutexHandle_(INVALID_HANDLE_VALUE)
    , controller_(new VolumeScreenWebSpeedControl())
{

}

VolumeScreenWebSpeed::~VolumeScreenWebSpeed()
{
    if (mutexHandle_ != INVALID_HANDLE_VALUE)
    {
        CloseHandle(mutexHandle_);
    }
}

bool VolumeScreenWebSpeed::Init()
{
    mutexHandle_ = ::CreateMutex(nullptr, FALSE, "VolumeScreenWebSpeedMutex");
    return !(mutexHandle_ == INVALID_HANDLE_VALUE);
}

void VolumeScreenWebSpeed::Show()
{
    Config* config = Config::GetShared();
    int showValue = config->GetValue(CONFIG_SET, "IsShow", 0);
    __super::Show(!!showValue);
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
