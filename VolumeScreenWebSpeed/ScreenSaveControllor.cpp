#include "StdAfx.h"
#include "ScreenSaveControllor.h"
#include <assert.h>
#include <sstream>
#include "WinDefine.h"
#include "config.h"
#include "ToastWindow.h"


namespace
{
    void PlaySoundHappy(int beg, int end)
    {
        unsigned FREQUENCY[] = { 392, 392, 440, 392, 523, 494,
            392, 392, 440, 392, 587, 523,
            392, 392, 784, 659, 523, 494, 440,
            689, 689, 523, 587, 523 };

        unsigned DELAY[] = { 375, 125, 500, 500, 500, 1000,
            375, 125, 500, 500, 500, 1000,
            375, 125, 500, 500, 500, 500, 1000,
            375, 125, 500, 500, 500, 1000 };
        for (int i = beg; i < end; i++)  //ÏÈÌî10¶ÌÐ©
        {
            Beep(FREQUENCY[i], DELAY[i]);
        }
    }
}

ScreenSaveControllor::ScreenSaveControllor(void)
    : hWnd_(nullptr)
    , forcegroundHwnd_(nullptr)
    , toastWindow_(nullptr)
    , config_(Config::GetShared())
{
}


ScreenSaveControllor::~ScreenSaveControllor(void)
{

}

void ScreenSaveControllor::ShowToastWindow(bool IsNotScreenSave, int timeMin)
{
    int showScreeenSaveToast = config_->GetValue(
        CONFIG_SET, "ShowScreeenSaveToast", 0);
    if (showScreeenSaveToast)
    {
        if (!toastWindow_)
        {
            toastWindow_.reset(new ToastWindow());
            int width = GetSystemMetrics(SM_CXSCREEN);
            toastWindow_->Create(nullptr, width - 500, 0, 200, 30);
        }
        int ShowScreeenSaveToastTimeMs = config_->GetValue(
            CONFIG_SET, "ShowScreeenSaveToastTimeMs", 2000);
        string ShowScreeenSaveToastTimeTextBeg = config_->GetValue(
            CONFIG_SET, "ShowScreeenSaveToastTimeTextBeg", 
            "Not Screen Save: ");
        string ShowScreeenSaveToastTimeTextEnd = config_->GetValue(
            CONFIG_SET, "ShowScreeenSaveToastTimeTextEnd", " Min");
        std::stringstream ss;
        if (IsNotScreenSave)
        {
            ss << ShowScreeenSaveToastTimeTextBeg << timeMin
                << ShowScreeenSaveToastTimeTextEnd;
        }
        else
        {
            ss << "Screen Save Normal";
        }
        toastWindow_->Show(ShowScreeenSaveToastTimeMs, ss.str());
    }
}

void ScreenSaveControllor::StopNotScreenSave(HWND hwnd, bool playSound)
{
    toastWindow_.reset();
    KillTimer(hwnd, WinDefine::TIMER_NOT_SCREEN_SAVE);
    KillTimer(hwnd, WinDefine::TIMER_NOT_SCREEN_SAVE_MAX);
    if (playSound)
    {
        PlaySoundHappy(0, 2);
    }
}

bool ScreenSaveControllor::InitControllor(HWND hWnd)
{
    return true;
}

void ScreenSaveControllor::OnTimer(
    HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    if (WinDefine::TIMER_CLOSE_SCREEN == idEvent ||
        WinDefine::TIMER_CLOSE_SCREEN_ADD == idEvent)
    {
        KillTimer(hwnd, idEvent);
        ::PostMessage(hwnd, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)1);
        ::PostMessage(hwnd, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2);
    }
    else if (WinDefine::TIMER_NOT_SCREEN_SAVE == idEvent)
    {
        INPUT input = { 0 };
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = VK_F24;
        SendInput(1, &input, sizeof(INPUT));
    }
    else if (WinDefine::TIMER_NOT_SCREEN_SAVE_MAX == idEvent)
    {
        StopNotScreenSave(hwnd, true);
    }
}

void ScreenSaveControllor::OnHotKeyNotScreenSave(
    HWND hwnd, UINT uMsg, int idHotKey, LPARAM lParam)
{
    Config* config = Config::GetShared();
    int notScreenSavePerInputTimeSec = 
        config->GetValue(CONFIG_SET, "notScreenSavePerInputTimeSec", 5 * 60);
    SetTimer(hwnd, WinDefine::TIMER_NOT_SCREEN_SAVE, 
             notScreenSavePerInputTimeSec * 1000, nullptr);
    int notScreenSavePerInputMAXTimeMin = config->GetValue(
        CONFIG_SET, "notScreenSavePerInputMAXTimeMin", 80);
    SetTimer(hwnd, WinDefine::TIMER_NOT_SCREEN_SAVE_MAX,
             notScreenSavePerInputMAXTimeMin * 60 * 1000, nullptr);
    ShowToastWindow(true, notScreenSavePerInputMAXTimeMin);
    PlaySoundHappy(0, 6);
}

void ScreenSaveControllor::OnHotKey(
    HWND hwnd, UINT uMsg, int idHotKey, LPARAM lParam)
{
    if (idHotKey == WinDefine::HOTKEY_CLOSE_SCREEN)
    {
        SetTimer(hwnd, WinDefine::TIMER_CLOSE_SCREEN, 1500, nullptr);
        SetTimer(hwnd, WinDefine::TIMER_CLOSE_SCREEN_ADD, 4000, nullptr);
        StopNotScreenSave(hwnd, false);
    }
    else if (idHotKey == WinDefine::HOTKEY_NOT_SCREEN_SAVE)
    {
        OnHotKeyNotScreenSave(hwnd, uMsg, idHotKey, lParam);
    }
}
