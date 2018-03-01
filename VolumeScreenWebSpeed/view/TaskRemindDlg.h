#pragma once
#include "DlgControl.h"
class TaskRemindDlg : public DlgControl
{
public:
    TaskRemindDlg(void);
    ~TaskRemindDlg(void);
	void CreateDlg(HWND hWnd);
	void setRemindText(const tstring& text);

protected:
	virtual bool DlgProc(UINT message, WPARAM wParam, LPARAM lParam,
		LRESULT* lResult);

private:
	void initControl();
	void setControlEnable(bool enable);
	void onBtnOk();
	void getTimerText(int* hour, int* min, int* sec);
	bool isBtnChecked(HWND hWnd);
	bool onFixTimeTask();
	bool onCountDownTask();
	void setHourState();
	void beginTimerTask(INT64 totalSec);
	void onTimer(UINT message, WPARAM wParam, LPARAM lParam);
	void setRemainText(INT64 totalSec);

	HWND m_hStaRemainText;
	HWND m_hBtnOK;
};

