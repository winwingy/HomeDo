#include "stdafx.h"
#include "ChangeTitleDlg.h"
#include "Resource.h"
#include "tool\windowTool.h"

ChangeTitleDlg::ChangeTitleDlg(void)
{

}


ChangeTitleDlg::~ChangeTitleDlg(void)
{

}

void ChangeTitleDlg::CreateDlgE(HWND hWnd)
{
	__super::CreateDlg(hWnd, IDD_DIALOG_TITLE);
}


bool ChangeTitleDlg::DlgProc(UINT message, WPARAM wParam,
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
			tstring orc =
				WindowTool::GetWindowText(GetDlgItem(hWnd, IDC_EDIT_ORC_TITLE));
			tstring newTitle =
				WindowTool::GetWindowText(GetDlgItem(hWnd, IDC_EDIT_NEW_TITLE));
			if (changeEvent_)
				changeEvent_(orc, newTitle);
			
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
		break;
	}
	default:
		break;
	}
	return __super::DlgProc(message, wParam, lParam, lResult);
}

void ChangeTitleDlg::setChangeEvent(const ChangeEvent& changeEvent)
{
	changeEvent_ = changeEvent;
}
