#include "stdafx.h"
#include <Windows.h>
#include <sstream>
#include "TaskRemindDlg.h"
#include "resource.h"
#include <assert.h>


TaskRemindDlg::TaskRemindDlg(void)
{
	m_hStaRemainText = nullptr;
	m_hBtnOK = nullptr;
}


TaskRemindDlg::~TaskRemindDlg(void)
{
}

void TaskRemindDlg::CreateDlg(HWND hWnd)
{
	__super::CreateDlg(hWnd, IDD_DIALOG_TASK_REMIND, 270, 60);
}

bool TaskRemindDlg::DlgProc(UINT message, WPARAM wParam,
	LPARAM lParam, LRESULT* lResult)
{
	HWND hWnd = m_hWnd;
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId, wmEvent;
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// ·ÖÎö²Ëµ¥Ñ¡Ôñ:
		switch (wmId)
		{
		case IDOK:
		{
			DestroyWindow(hWnd);
			break;
		}
		case IDCANCEL:
		{
			DestroyWindow(hWnd);
			break;
		}
		default:
			break;
		}
		break;
	}
	case WM_INITDIALOG:
	{
		m_hStaRemainText = GetDlgItem(m_hWnd, IDC_STATIC_RemainTime);
		m_hBtnOK = GetDlgItem(m_hWnd, IDOK);
		break;
	}
	default:
		break;
	}
	return __super::DlgProc(message, wParam, lParam, lResult);
}

void TaskRemindDlg::setRemindText(const tstring& text)
{
	SetWindowText(m_hStaRemainText, text.c_str());
}
