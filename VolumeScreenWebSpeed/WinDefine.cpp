#include "StdAfx.h"
#include "WinDefine.h"

WinDefine* WinDefine::windefine_ = NULL;

WinDefine::WinDefine(void)
{
	g_initVolume = 0;
	g_initVolumeConst = 0;
	g_perVoulumeGap = 10;
	g_iInitTime = 1000;
	g_iIsInitVolume = 1;
	g_bFinishInitVolume = FALSE;
	g_iGetWebTimeCnt = 0;
	powerOnStartProgressTime_ = 5000;
    notScreenSavePerInputTime_ = 0;
    notScreenSaveCanTryCnt_ = 0;
}


WinDefine::~WinDefine(void)
{
}

WinDefine* WinDefine::GetInstance()
{
	if (windefine_ == NULL)
	{
		windefine_ = new WinDefine;
	}
	return windefine_;
}
