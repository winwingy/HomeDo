#ifndef _TIMER_H
#define _TIMER_H

#include <functional>
#include <memory>
#include <windows.h>
#include <list>

namespace
{
    class IDAndTimerFun;
}

class Timer
{
public:
    typedef int TimeMs;
    typedef std::function<void()> TimerFun;
    Timer(HWND hWnd);
    ~Timer();

    bool OneShotTimer(Timer::TimeMs timeMs, const TimerFun& timerFun);
  
private:
    static VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg,
                                   UINT_PTR idEvent, DWORD dwTime);

    HWND hWnd_;
    std::list<std::shared_ptr<IDAndTimerFun>> idAndTimerFun_;
};

#endif