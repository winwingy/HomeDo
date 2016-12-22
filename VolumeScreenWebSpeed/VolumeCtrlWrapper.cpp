#include "StdAfx.h"
#include "VolumeCtrlWrapper.h"
#include "config.h"
#include "WinDefine.h"

namespace
{
    struct TimerData
    {
        UINT_PTR TimerId;
        VolumeCtrlWrapper* VolumeCtrl;
        TimerData()
            : TimerId(-1)
            , VolumeCtrl(nullptr)
        {

        }
    };
}


VolumeCtrlWrapper::VolumeCtrlWrapper(void)
    : perVoulumeGap_(5)
    , config_(Config::GetShared())
{

}


VolumeCtrlWrapper::~VolumeCtrlWrapper(void)
{

}

VOID CALLBACK WinControlTool::VolumeTimerProc(
    HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    WinControlTool* pThis = reinterpret_cast<WinControlTool*>(
        GetWindowLongPtr(hwnd, GWLP_USERDATA));
    WinDefine* winDefine = WinDefine::GetInstance();
    int initVolume = pThis->config_->GetValue(CONFIG_SET, "InitVolume", 30);
    if (FALSE == winDefine->bFinishInitVolume_ && winDefine->iIsInitVolume_ == 1)
    {
        GetInstance()->GetMyVolumeCtrl()->SetVolume(winDefine->initVolume_);
        winDefine->initVolume_ = 0;
    }
    winDefine->bFinishInitVolume_ = TRUE;
    KillTimer(hwnd, idEvent);
}

bool VolumeCtrlWrapper::InitVolumeHotKey(HWND hWnd)
{
    perVoulumeGap_ = config_->GetValue(CONFIG_SET, "PerVolumeGap", 3);
    int isInitVolume = config_->GetValue(CONFIG_SET, "IsInitVolume", 1);
    if (isInitVolume)
    {
        int initTime = config_->GetValue(CONFIG_SET, "InitTime", 5000);
        TimerData* timerData = new TimerData(this, this);
        timerData
        SetTimer(hWnd, TIMER_INIT_VOLUME, winDefine->iInitTime_, VolumeTimerProc);
        int initVolume = config_->GetValue(CONFIG_SET, "InitVolume", 30);

        winDefine->initVolume_ = initVolume;
        int initTime = config_->GetValue(CONFIG_SET, "InitTime", 5000);
        winDefine->iInitTime_ = initTime;
    }
    winDefine->iIsInitVolume_ = atoi(strValue.c_str());
    SetTimer(hWnd, TIMER_INIT_VOLUME, winDefine->iInitTime_, TimerProc);
}
