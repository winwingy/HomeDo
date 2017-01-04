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
class ScreenSaveControllor;

class VolumeScreenWebSpeedControl
{
public:
    VolumeScreenWebSpeedControl(void);
    ~VolumeScreenWebSpeedControl(void);

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

    void OnCreate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnHotKey(HWND hWnd, UINT uMsg, int idHotKey, LPARAM lParam);
    void OnTimer(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

private:
    void RaiseToken();
    void InitHotKey(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void InitGeneralHotKey(HWND hWnd);
    void InitProgressHotKey(HWND hWnd);
    void InitPowerOnStartProgress(HWND hWnd);
    void InitProgressConfig(HWND hWnd);

    void OnKillProcess(HWND hWnd);
    void OnPowerOnStartProgressTimer(HWND hwnd, UINT uMsg,
                                     UINT_PTR idEvent, DWORD dwTime);

    void KillProgressByNames(const vector<string>& namelist, bool tryExistFirst);
    void OpenConfig();

    vector<ProgressToIDHotKey> progressToIDHotkeyList_;
    Config* config_;
    std::unique_ptr<VolumeCtrlWrapper> volumeCtrlWrapper_;
    vector<string> powerOnStartProgress_;
    std::unique_ptr<ScreenSaveControllor> screenSaveControllor_;
};
