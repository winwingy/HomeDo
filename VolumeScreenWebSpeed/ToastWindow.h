#pragma once
#include "WindowControl.h"

class ToastWindow : public WindowControl
{
public:
    ToastWindow(void);
    ~ToastWindow(void);
    void Show(int showTimeMs, const string& text);

private: 
    void Close();
    virtual LRESULT WndProc(HWND hWnd, UINT message,
                            WPARAM wParam, LPARAM lParam) override;
    virtual void CreateParam(DWORD* styleEx, DWORD* style) override;
    HWND hWnd_;
    string tips_;
};

