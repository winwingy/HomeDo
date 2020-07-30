#ifndef _VOLUME_SCREEN_WEB_SPEED_WIN_DEFINE_H_H
#define _VOLUME_SCREEN_WEB_SPEED_WIN_DEFINE_H_H
#include <windows.h>
#include <vector>
#include <string>

using std::vector;
using std::string;

typedef unsigned int uint;


#define CONFIG_SET "Set"
#define CONFIG_POWER_ON_START_PROGRESS  "PowerOnStartProgress"
#define CONFIG_SET_HOTKEY  "HotKey"
#define CONFIG_SET_PROGRESS_HOTKEY  "ProgressHotKey"
#define CONFIG_INF_FILENAME  "VolScrConfig.ini"
#define CONFIG_INF_FILENAME_JOB  "VolScrConfigJob.ini"

#define CONFIG_SET_KILLNAME_BEGIN  "[KillNameBegin]"
#define CONFIG_SET_KILLNAME_END  "[KillNameEnd]"

#define CONFIG_SET_NAMEWINDOW_BEGIN  "[NameWindowBegin]"
#define CONFIG_SET_NAMEWINDOW_END  "[NameWindowEnd]"

#define APP_NAME "wingy管理工具"

extern uint WM_TASKBARCREATED;

enum enWMMsg
{
	WM_TaskBarMsg = WM_USER + 2000,

};


enum enTray_menu
{
	enTray_timingClose,
	enTray_timingPopup,
	enTray_timingPopupNew,
	enTray_timingSleep,
	enTray_timingLogoff,
	enTray_timingRestart,
	enTray_closeNameWindow,
	enTray_changeWindowTitle,
	enTray_exit
};



class WinDefine
{
public:
    enum HOTKEY
    {
        HOTKEY_OPEN_CONFIG = 1000,
        HOTKEY_VOLUME_UP = 1001,
        HOTKEY_VOLUME_DOWN = 1002,
        HOTKEY_CLOSE_SCREEN = 1003,
        HOTKEY_KILL_PROCESS,
        HOTKEY_NOT_SCREEN_SAVE,
        HOTKEY_SHUT_DOWN,
		HOTKEY_TASK_LIST,
		HOTKEY_TASK_NEW,
        HOTKEY_MOUSESPEED_WEB,
        HOTKEY_MOUSESPEED_GAME,

        HOTKEY_PROGRESS_BEGIN = 5000,
        HOTKEY_PROGRESS_END = 6000,

        HOTKEY_PROGRESS_KILL_BEGIN = 7000,
        HOTKEY_PROGRESS_KILL_END = 8000,
    };

    enum TIMER_ID
    {
        TIMER_CLOSE_SCREEN = WM_USER + 100,
        TIMER_CLOSE_SCREEN_ADD,
        TIMER_INIT_VOLUME,
        TIMER_GET_WEB_TIME,
        TIMER_NOT_SCREEN_SAVE,
        TIMER_NOT_SCREEN_SAVE_MAX,
        TIMER_POWER_ON_START_PROGRESS,
		TIMER_MONITOR_BUSY_APP,
        TIMER_TOAST,
    };
    WinDefine(void);
    ~WinDefine(void);

    static int HotKeyStrigToVkKey(const string& hotKeyString);

private:
};

#endif