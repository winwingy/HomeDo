#include "stdafx.h"
#include <Windows.h>
#include <sstream>
#include "NotScreenSaveDlg.h"
#include "../resource.h"
#include <time.h>
#include <assert.h>


NotSCreenSaveDlg::NotSCreenSaveDlg(void)
    : hWnd_(NULL)
{
}


NotSCreenSaveDlg::~NotSCreenSaveDlg(void)
{
}


bool NotSCreenSaveDlg::RunShutDownCMD(INT64 sec, bool Cancel)
{
    bool ret = false;
    do
    {
        const int len = 1024;
        TCHAR buf[len] = { 0 };
        UINT size = GetSystemDirectory(buf, len);
        if (size > 0 && size >= len)
        {
            break;
        }
        std::string cmdExe = buf;
        cmdExe += _T("\\shutdown.exe");
        FILE* fp = NULL;
        fopen_s(&fp, cmdExe.c_str(), "r");
        if (!fp)
        {
            break;
        }

        std::stringstream ss;
        if (Cancel)
        {
            ss << cmdExe << " -a";
        }
        else
        {
            ss << cmdExe << " -s -t " << sec;
        }
        system(ss.str().c_str());
    } while (0);
    return ret;
}

INT_PTR CALLBACK NotSCreenSaveDlg::ScreenWndProc(HWND hWnd, UINT message,
                                                 WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    if (message == WM_COMMAND)
    {
        wmId = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // ·ÖÎö²Ëµ¥Ñ¡Ôñ:
        switch (wmId)
        {
            case IDOK:
            {
                int hour = 0;
                {
                    const int len = 100;
                    TCHAR szbuf[len] = { 0 };
                    GetWindowText(GetDlgItem(hWnd, IDC_EDIT_NOTSCREENSAVE_HOUR),
                                  szbuf, len);
                    hour = atoi(szbuf);
                }
                int min = 0;
                {
                    const int len = 100;
                    TCHAR szbuf[len] = { 0 };
                    GetWindowText(GetDlgItem(hWnd, IDC_EDIT_NOTSCREENSAVE_MIN),
                                  szbuf, len);
                    min = atoi(szbuf);
                }
                INT64 totalSec = static_cast<INT64>(hour * 60 * 60) +
                    static_cast<INT64>(min * 60);
                if (totalSec != 0)
                    NotSCreenSaveDlg::RunShutDownCMD(totalSec, false);
                else
                    assert(totalSec);

                break;
            }
            case ID_CANCEL_SHUTDOW:
            {
                NotSCreenSaveDlg::RunShutDownCMD(0, true);
                break;
            }
            case IDCANCEL:
            DestroyWindow(hWnd);
            break;
            default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    else if (message == WM_INITDIALOG)
    {
        SetFocus(GetDlgItem(hWnd, IDC_EDIT_HOUR));
        int x = GetSystemMetrics(SM_CXSCREEN);
        int y = GetSystemMetrics(SM_CYSCREEN);
        RECT rect;
        GetWindowRect(hWnd, &rect);
        int xPos = (x - (rect.right - rect.left)) / 2;
        int yPos = (y - (rect.bottom - rect.top)) / 2;
        MoveWindow(hWnd, xPos, yPos, rect.right - rect.left, rect.bottom - rect.top, TRUE);
    }
    return 0;
}

HWND NotSCreenSaveDlg::Create(HWND hWndParent)
{
    HWND dlg = CreateDialogParamA((HINSTANCE)GetModuleHandle(NULL),
                                  MAKEINTRESOURCE(IDD_DIALOG_NOT_SCREEN_SAVE), hWndParent,
                                  ScreenWndProc, 0);
    assert(dlg);
    UpdateWindow(dlg);
    return dlg;
}

void NotSCreenSaveDlg::ShowWindow(bool visible)
{
    assert(hWnd_);
    if (!hWnd_)
    {
        return;
    }
    if (visible)
    {
        ::ShowWindow(hWnd_, SW_SHOW);
        SetWindowPos(hWnd_, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE);
        SetWindowPos(hWnd_, HWND_NOTOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE);
    }
    else
    {
        ::ShowWindow(hWnd_, SW_HIDE);
    }
}
