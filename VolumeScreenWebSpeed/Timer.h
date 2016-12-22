#ifndef _TIMER_H
#define _TIMER_H

#include <functional>
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
    typedef std::function<void(TimeMs)> TimerFun;
    Timer(HWND hWnd);
    ~Timer();

    bool Begin(Timer::TimeMs timeMs, const TimerFun& timerFun);

private:
    
    HWND hWnd_;
    std::list<IDAndTimerFun*> idAndTimerFun_;
};

#endif