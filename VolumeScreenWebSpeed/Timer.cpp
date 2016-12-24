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
                  const Timer::TimerFun& TimerFunItem)
        {
            timerObj_ = TimerObj;
            timerId_ = TimerId;
            timerFunItem_ = TimerFunItem;
        }

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

}

VOID CALLBACK Timer::TimerProc(
    HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    KillTimer(hwnd, idEvent);
    IDAndTimerFun* idPer = reinterpret_cast<IDAndTimerFun*>(idEvent);
    if (idPer->GetTimerFun())
    {
        (idPer->GetTimerFun())();
    }
    Timer* obj = idPer->GetTimerObj();
    for (auto iter = (obj->idAndTimerFun_).begin();
         iter != obj->idAndTimerFun_.end(); ++iter)
    {
        if (iter->get() == idPer)
        {
            obj->idAndTimerFun_.erase(iter);
            break;
        }
    }
}

bool Timer::OneShotTimer(TimeMs timeMs, const TimerFun& timerFun)
{
    std::shared_ptr<IDAndTimerFun> idPer(new IDAndTimerFun());
    idPer->Init(this, reinterpret_cast<int>(idPer.get()), timerFun);
    idAndTimerFun_.push_back(idPer);
    SetTimer(hWnd_, idPer->GetTimerId(), timeMs, TimerProc);
    return true;
}
