#include "StdAfx.h"
#include "VolumeCtrlWrapper.h"
#include "config.h"
#include "WinDefine.h"
#include "MyVolumeCtrl.h"

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
        SetTimer(hWnd, TIMER_INIT_VOLUME, initTime, nullptr);
    }
    return true;
}

void VolumeCtrlWrapper::OnTimer(
    HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    if (uMsg == WM_TIMER && idEvent == TIMER_INIT_VOLUME)
    {
        WinDefine* winDefine = WinDefine::GetInstance();
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
    if (idHotKey == HOTKEY_VOLUME_UP || idHotKey == HOTKEY_VOLUME_DOWN)
    {
        if ((idHotKey == HOTKEY_VOLUME_DOWN) && !userChangedVolume_)
        {
            int initVolume = config_->GetValue(CONFIG_SET, "InitVolume", 30);
            myVolumeCtrl_->SetVolume(initVolume * 1 / 3);
            userChangedVolume_ = true;
        }
        int volumeNow = myVolumeCtrl_->GetVolume();   //得到的值偏小1
        if (idHotKey == HOTKEY_VOLUME_UP)
            volumeNow += perVolumeGap_;
        else
            volumeNow -= perVolumeGap_;

        if (volumeNow < 0)
            volumeNow = 0;

        myVolumeCtrl_->SetVolume(volumeNow); 
    }
}

