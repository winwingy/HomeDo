#pragma once
class ShutDownDlg
{
public:
    ShutDownDlg(void);
    ~ShutDownDlg(void);
    bool DoModal(HWND hwnd);

private:
    static bool RunShutDownCMD(INT64 sec, bool Cancel);
    static INT_PTR CALLBACK ShutDownWndProc(HWND hWnd, UINT message,
                                            WPARAM wParam, LPARAM lParam);
};

