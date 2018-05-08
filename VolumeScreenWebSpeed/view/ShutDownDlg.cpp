#include "stdafx.h"
#include <Windows.h>
#include <sstream>
#include "ShutDownDlg.h"
#include "../resource.h"
#include <time.h>
#include <assert.h>


ShutDownDlg::ShutDownDlg(void)
{
}


ShutDownDlg::~ShutDownDlg(void)
{
}


bool ShutDownDlg::RunShutDownCMD(INT64 sec, bool Cancel)
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

bool ShutDownDlg::DlgProc(UINT message, WPARAM wParam, LPARAM lParam,
	LRESULT* lResult)
{
	HWND hWnd = m_hWnd;
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
                    GetWindowText(GetDlgItem(hWnd, IDC_EDIT_HOUR), szbuf, len);
                    hour = atoi(szbuf);
                }
                int min = 0;
                {
                    const int len = 100;
                    TCHAR szbuf[len] = { 0 };
                    GetWindowText(GetDlgItem(hWnd, IDC_EDIT_MIN), szbuf, len);
                    min = atoi(szbuf);
                }
                int sec = 0;
                {
                    const int len = 100;
                    TCHAR szbuf[len] = { 0 };
                    GetWindowText(GetDlgItem(hWnd, IDC_EDIT_SEC), szbuf, len);
                    sec = atoi(szbuf);
                }
                INT64 totalSec = static_cast<INT64>(hour * 60 * 60) +
                    static_cast<INT64>(min * 60) + static_cast<INT64>(sec);
				if (totalSec != 0)
				{
					ShutDownDlg::RunShutDownCMD(totalSec, false);
					setControlState(false);
				}
                else
                    assert(totalSec);

                break;
            }
            case ID_CANCEL_SHUTDOW:
            {
                ShutDownDlg::RunShutDownCMD(0, true);
				setControlState(true);
                break;
            }
            case IDCANCEL:
				setVisible(false);
				break;
            default:
				break;
        }
    }
	else if (message == WM_CLOSE)
	{
		setVisible(false);
		*lResult = TRUE;
		return true;
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
        ::MoveWindow(hWnd, xPos, yPos, rect.right - rect.left, 
                   rect.bottom - rect.top, TRUE);
		setControlState(true);
    }
    return 0;
}

void ShutDownDlg::setControlState(bool enable)
{
	ShowWindow(GetDlgItem(m_hWnd, IDC_STATIC_ShutDowningTip), !enable);
	
	EnableWindow(GetDlgItem(m_hWnd, IDC_EDIT_HOUR), enable);
	EnableWindow(GetDlgItem(m_hWnd, IDC_EDIT_MIN), enable);
	EnableWindow(GetDlgItem(m_hWnd, IDC_EDIT_SEC), enable);

	EnableWindow(GetDlgItem(m_hWnd, IDOK), enable);
}

bool ShutDownDlg::CreateDlgE(HWND hwnd)
{
	return __super::CreateDlg(hwnd, IDD_DIALOG_SHUT_DOWN);
}

void ShutDownDlg::ShowDlg()
{
	setVisible(true);
	activeWindow();
}

bool ShutDownDlg::isTasking()
{
	return !IsWindowEnabled(GetDlgItem(m_hWnd, IDOK));
}
