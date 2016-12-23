#pragma once
#include <Windows.h>
#include <string>
#include <vector>

using std::string;
using std::wstring;
using std::vector;
class ToastWindow;
class Config;
class VolumeCtrlWrapper;

class WinControlTool
{
public:
    WinControlTool(void);
    ~WinControlTool(void);

    struct ProgressToIDHotKey
    {
        string path;
        UINT ID;
        UINT vkCtrl;
        UINT vkKey;

        UINT killID;
        UINT vkKillCtrl;
        UINT vkKillKey;
        ProgressToIDHotKey()
            : ID(0)
            , vkCtrl(0)
            , vkKey(0)
            , killID(0)
            , vkKillCtrl(0)
            , vkKillKey(0)
        {

        }
    };

    void OnCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void OnHotKey(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void OnTimer(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:

    void InitHotKey(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void InitGeneralHotKey(HWND hWnd);
    void InitProgressHotKey(HWND hWnd);
    void InitPowerOnStartProgress(HWND hWnd);

    string GetKillNameBuff();
    void OnKillProcess(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static VOID CALLBACK PowerOnStartProgressTimeProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
    static VOID CALLBACK GetWebTimeTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
    static VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
    static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
    void KillProgressByNames(const vector<string>& namelist, bool tryExistFirst);
    void TerminateNameExe(string& strNameExe);

    void RaiseToken();
    bool ForcegroundWindowFullScreen(HWND forcegroundWindow);
    void OnHotKeyNotScreenSave(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void TipsSound();
    void StopNotScreenSave(HWND hwnd, bool playSound);
    void InitProgressConfig(HWND hWnd);
    static VOID CALLBACK VolumeTimerProc(HWND hwnd, UINT uMsg,
                                         UINT_PTR idEvent, DWORD dwTime);
private:

    static WinControlTool* winControlTool;
    vector<ProgressToIDHotKey> progressToIDHotkeyList_;
    Config* config_;
    std::unique_ptr<VolumeCtrlWrapper> volumeCtrlWrapper_;
    vector<string> powerOnStartProgress_;
};

