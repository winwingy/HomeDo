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
	m_hStaRemainText = nullptr;
	m_hBtnOK = nullptr;
}

void TaskRemindDlg::ShowDlg(HWND hWnd)
{
	m_hWnd = CreateDialogParam((HINSTANCE)GetModuleHandle(NULL),
		MAKEINTRESOURCE(IDD_DIALOG_TASK_REMIND), hWnd, StaDlgProc,
		reinterpret_cast<LPARAM>(this));
	RETURN_ASSERT(m_hWnd);

	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	int width = 428;
	int height = 140;

	MoveWindow(rect.right - width, rect.bottom - height);

	SetWindowPos(m_hWnd, 0, 0, 0, 0, 0, 
		SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE);
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
			close();
			break;
		}
		case IDCANCEL:
		{
			close();
			break;
		}
		default:
			break;
		}
		break;
	}
	case WM_INITDIALOG:
	{
		m_hStaRemainText = GetDlgItem(m_hWnd, IDC_STATIC_TaskText);
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
