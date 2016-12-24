#include "StdAfx.h"
#include "ToastWindow.h"
#include <assert.h>


ToastWindow::ToastWindow(void)
    : hWnd_(nullptr)
    , tips_()
{
}


ToastWindow::~ToastWindow(void)
{
    Close();
}

LRESULT ToastWindow::WndProc(
    HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_TIMER:
        {
            KillTimer(hWnd, wParam);
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps = { 0 };
            HDC hdc = BeginPaint(hWnd, &ps);
            RECT rect;
            GetClientRect(hWnd, &rect);
            DrawText(hdc, tips_.c_str(), 
                     tips_.length(), &rect,
                     DT_VCENTER | DT_CENTER);
            EndPaint(hWnd, &ps);
            break;
        }
        default:
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
            break;
        }
    }
    return 0;
}

void ToastWindow::CreateParam(DWORD* styleEx, DWORD* style)
{
    *styleEx |= WS_EX_TOOLWINDOW;
    *style |= ~(WS_BORDER | WS_CAPTION | WS_SYSMENU |
                WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
    *style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP;
}

void ToastWindow::Close()
{
    if (hWnd_)
    {
        SendMessage(hWnd_, WM_CLOSE, 0, 0);
        hWnd_ = nullptr;
    }
}

void ToastWindow::Show(int showTimeMs, const string& text)
{
    SetTimer(hWnd_, 1003, 5000, nullptr);
}
