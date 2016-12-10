#pragma once
class NotSCreenSaveDlg
{
public:
    NotSCreenSaveDlg(void);
    ~NotSCreenSaveDlg(void);
    HWND Create(HWND hWndParent);
    void ShowWindow(bool visible);

private:
    static bool RunShutDownCMD(INT64 sec, bool Cancel);
    static INT_PTR CALLBACK ShutDownWndProc(
        HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    HWND hWnd_;
};

