#pragma once
#include "DlgControl.h"
class TaskRemindDlg : public DlgControl
{
public:
    TaskRemindDlg(void);
    ~TaskRemindDlg(void);
	void ShowDlg(HWND hWnd);
	void setRemindText(const tstring& text);

protected:
	virtual bool DlgProc(UINT message, WPARAM wParam, LPARAM lParam,
		LRESULT* lResult);

private:
	HWND m_hStaRemainText;
	HWND m_hBtnOK;
};

