#pragma once
class Config;
class ScreenSaveControllor
{
public:
    ScreenSaveControllor(void);
    ~ScreenSaveControllor(void);

    bool InitControllor(HWND hWnd);
    void OnTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
    void OnHotKey(HWND hwnd, UINT uMsg, int idHotKey, LPARAM lParam);

private:
    void OnHotKeyNotScreenSave(HWND hwnd, UINT uMsg, 
                               int idHotKey, LPARAM lParam);
    HWND NotScreenSave(HWND hWndParent, bool showDlg);
    void StopNotScreenSave(HWND hwnd, bool playSound);
    void ShowNoScreenToast(bool IsNotScreenSave, int timeMin);
    HWND hWnd_;
    HWND forcegroundHwnd_;
    Config* config_;
};

