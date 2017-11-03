#include "stdafx.h"
#include "VolumeCtrlWrapper.h"
#include "../tool/config.h"
#include "WinDefine.h"
#include "../tool/MyVolumeCtrl.h"

namespace
{

}

VolumeCtrlWrapper::VolumeCtrlWrapper(void)
    : perVolumeGap_(5)
    , userChangedVolume_(false)
    , config_(Config::GetShared())
    , myVolumeCtrl_(new CMyVolumeCtrl)
{

}

VolumeCtrlWrapper::~VolumeCtrlWrapper(void)
{

}

bool VolumeCtrlWrapper::InitVolumeHotKey(HWND hWnd)
{
    perVolumeGap_ = config_->GetValue(CONFIG_SET, "PerVolumeGap", 3);
    int isInitVolume = config_->GetValue(CONFIG_SET, "IsInitVolume", 1);
    if (isInitVolume)
    {
		int initTime = config_->GetValue(CONFIG_SET, "InitVolumeTimeMs", 5000);
		SetTimer(hWnd, WinDefine::TIMER_INIT_VOLUME, initTime, nullptr);
    }
	else
	{
		 userChangedVolume_ = true;
	}
    return true;
}

void VolumeCtrlWrapper::OnTimer(
    HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    if (idEvent == WinDefine::TIMER_INIT_VOLUME)
    {
        int initVolume = config_->GetValue(CONFIG_SET, "InitVolume", 30);
        if (!userChangedVolume_)
            myVolumeCtrl_->SetVolume(initVolume);

        userChangedVolume_ = true;
        KillTimer(hwnd, idEvent);
    }
}

void VolumeCtrlWrapper::OnHotKey(
    HWND hwnd, UINT uMsg, int idHotKey, LPARAM lParam)
{
    if (idHotKey == WinDefine::HOTKEY_VOLUME_UP ||
        idHotKey == WinDefine::HOTKEY_VOLUME_DOWN)
    {
        if ((idHotKey == WinDefine::HOTKEY_VOLUME_DOWN) && !userChangedVolume_)
        {
            int initVolume = config_->GetValue(CONFIG_SET, "InitVolume", 30);
            myVolumeCtrl_->SetVolume(initVolume * 1 / 3);
            userChangedVolume_ = true;
        }
        int volumeNow = myVolumeCtrl_->GetVolume();   //得到的值偏小1
        if (idHotKey == WinDefine::HOTKEY_VOLUME_UP)
            volumeNow += perVolumeGap_;
        else
            volumeNow -= perVolumeGap_;

        if (volumeNow < 0)
            volumeNow = 0;

        myVolumeCtrl_->SetVolume(volumeNow); 
    }
}

