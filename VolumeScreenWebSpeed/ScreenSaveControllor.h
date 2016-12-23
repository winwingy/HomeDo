#pragma once
class ToastWindow;
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
    void ShowToastWindow(bool IsNotScreenSave);
    HWND hWnd_;
    HWND forcegroundHwnd_;
    Config* config_;
    std::unique_ptr<ToastWindow> toastWindow_;
};

