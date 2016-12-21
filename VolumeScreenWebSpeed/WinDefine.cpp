#include "StdAfx.h"
#include "WinDefine.h"

WinDefine* WinDefine::windefine_ = NULL;

WinDefine::WinDefine(void)
	: useJobConfig_(false)
	, initVolume_(0)
	, initVolumeConst_(0)
	, perVoulumeGap_(10)
	, iInitTime_(1000)
	, iIsInitVolume_(1)
	, bFinishInitVolume_(FALSE)
	, iGetWebTimeCnt_(0)
	, notScreenSavePerInputTime_(0)
	, notScreenSaveCanTryCnt_(0)
{

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
