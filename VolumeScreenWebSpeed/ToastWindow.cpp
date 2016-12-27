#include "StdAfx.h"
#include "ToastWindow.h"
#include <assert.h>
#include "WinDefine.h"


ToastWindow::ToastWindow(void)
    : tips_()
{
}


ToastWindow::~ToastWindow(void)
{
    SendMessage(hWnd_, WM_CLOSE, 0, 0);
}

bool ToastWindow::WndProc(
    UINT message, WPARAM wParam, LPARAM lParam, LRESULT* lResult)
{
    bool handle = false;
    switch (message)
    {
        case WM_CREATE:
        {
            int a = 10;
            break;
        }
        case WM_TIMER:
        {
            KillTimer(hWnd_, wParam);
            SetVisible(false);
            break;
        }
        case WM_CLOSE:
        {
            int b = 10;
            break;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps = { 0 };
            HDC hdc = BeginPaint(hWnd_, &ps);
            RECT rect;
            GetClientRect(hWnd_, &rect);
            DrawText(hdc, tips_.c_str(), 
                     tips_.length(), &rect,
                     DT_VCENTER | DT_CENTER |DT_SINGLELINE);
            EndPaint(hWnd_, &ps);
            handle = true;
        }
    }
    if (handle)
        return handle;

    return __super::WndProc(message, wParam, lParam, lResult);
}

void ToastWindow::CreateParam(DWORD* styleEx, DWORD* style)
{
    *styleEx |= WS_EX_TOOLWINDOW;
    *style &= ~(WS_BORDER | WS_CAPTION | WS_SYSMENU |
                WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME);
    *style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP;
}

void ToastWindow::Show(int showTimeMs, const string& text)
{
    SetTimer(hWnd_, WinDefine::TIMER_TOAST, showTimeMs, nullptr);
    tips_ = text;
    SetWindowPos(hWnd_, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    UpdateWindow(hWnd_);
}
