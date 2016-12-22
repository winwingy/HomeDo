#ifndef _VOLUME_SCREEN_WEB_SPEED_WIN_DEFINE_H_H
#define _VOLUME_SCREEN_WEB_SPEED_WIN_DEFINE_H_H
#include <windows.h>
#include <vector>
#include <string>

using std::vector;
using std::string;

#define  TRACE_WW OutputDebugString

#define TRACE_ZZ(szText, strShow) \
{char szBuffer[1024]={0}; \
sprintf_s(szBuffer, _countof(szBuffer), szText, strShow);\
 OutputDebugString(szBuffer);}

const TCHAR* CONFIG_SET = "Set";
const TCHAR* CONFIG_POWER_ON_START_PROGRESS = "PowerOnStartProgress";
const TCHAR* CONFIG_SET_HOTKEY = "HotKey";
const TCHAR* CONFIG_SET_PROGRESS_HOTKEY = "ProgressHotKey";
const TCHAR* CONFIG_INF_FILENAME = "VolScrConfig.ini";
const TCHAR* CONFIG_INF_FILENAME_JOB = "VolScrConfigJob.ini";

const TCHAR* CONFIG_SET_KILLNAME_BEGIN = "[KillNameBegin]";
const TCHAR* CONFIG_SET_KILLNAME_END = "[KillNameEnd]";

enum HOTKEY
{
    HOTKEY_VOLUME_UP = 1001,
    HOTKEY_VOLUME_DOWN = 1002,
    HOTKEY_CLOSE_SCREEN = 1003,
    HOTKEY_KILL_PROCESS,
    HOTKEY_KILL_TTPLAYER,
    HOTKEY_NOT_SCREEN_SAVE,
    HOTKEY_NOT_SHUT_DOWN,

    HOTKEY_PROGRESS_BEGIN = 5000,
    HOTKEY_PROGRESS_END = 6000,

    HOTKEY_PROGRESS_KILL_BEGIN = 7000,
    HOTKEY_PROGRESS_KILL_END = 8000,
};




enum MSG_ID
{
    TIMER_CLOSE_SCREEN = WM_USER + 100,
    TIMER_CLOSE_SCREEN_ADD,
    TIMER_INIT_VOLUME,
    TIMER_GET_WEB_TIME,
    TIMER_NOT_SCREEN_SAVE,
    TIMER_NOT_SCREEN_SAVE_MAX,
    TIMER_POWER_ON_START_PROGRESS
};



class WinDefine
{
public:

    ~WinDefine(void);

    static WinDefine* GetInstance();
    int HotKeyStrigToVkKey(const string& hotKeyString);

    bool useJobConfig_;
    int initVolume_;
    int perVoulumeGap_;
    BOOL bFinishInitVolume_;
    int iGetWebTimeCnt_;

    int notScreenSavePerInputTime_;
    int  notScreenSaveCanTryCnt_;


private:
    WinDefine(void);
    static WinDefine* windefine_;
};

#endif