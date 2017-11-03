#pragma once
#include "WindowControl.h"

class ToastWindow : public WindowControl
{
public:
    ToastWindow(void);
    ~ToastWindow(void);
    void Show(int showTimeMs, const string& text);

private: 
    virtual bool WndProc(UINT message, WPARAM wParam, LPARAM lParam,
                         LRESULT* lResult) override;
    virtual void CreateParam(DWORD* styleEx, DWORD* style) override;
    string tips_;
};

