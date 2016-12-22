#include "StdAfx.h"
#include "Timer.h"

namespace
{
    class IDAndTimerFun
    {
    public:
        IDAndTimerFun()
            : timerObj_(nullptr)
            , timerId_(-1)
            , timerFunItem_()
        {

        }
        void Init(Timer* TimerObj, int TimerId,
                  const Timer::TimerFun& TimerFunItem);

        Timer* GetTimerObj(){ return timerObj_;}
        int GetTimerId(){ return timerId_; }
        Timer::TimerFun GetTimerFun(){ return timerFunItem_; }
    private:
        Timer* timerObj_;
        int timerId_;
        Timer::TimerFun timerFunItem_;

    };
}

Timer::Timer(HWND hWnd)
    : hWnd_(hWnd)
{

}

Timer::~Timer()
{
    for (auto iter = idAndTimerFun_.begin();
         iter != idAndTimerFun_.end(); ++iter)
    {
        delete *iter;
    }
}


VOID CALLBACK TimerProc(
    HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    IDAndTimerFun* idPer = reinterpret_cast<IDAndTimerFun*>(idEvent);

}

bool Timer::Begin(TimeMs timeMs, const TimerFun& timerFun)
{
    for (auto iter = idAndTimerFun_.begin(); 
         iter != idAndTimerFun_.end(); ++iter)
    {

    }
    IDAndTimerFun* idPer = new IDAndTimerFun();
    idPer->Init(this, reinterpret_cast<int>(idPer), timerFun);
    idAndTimerFun_.push_back(idPer);
    SetTimer(hWnd_, idPer->GetTimerId(), timeMs, TimerProc);
}
