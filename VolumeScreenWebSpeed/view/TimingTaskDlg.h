#pragma once
#include "DlgControl.h"
class TimingTaskDlg : public DlgControl
{
public:
    TimingTaskDlg(void);
    ~TimingTaskDlg(void);
	void CreateDlgE(HWND hWnd);
	bool isTasking();

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
	void close();
	void resetState();
	void timerEnd(UINT message, WPARAM wParam, LPARAM lParam);

private:
	HWND m_hRadioCountDown;
	HWND m_hRadioFixTime;
	HWND m_hEditHour;
	HWND m_hEditMin;
	HWND m_hEditSec;

	HWND m_hRadioRepeatYes;
	HWND m_hRadioRepeatNo;

	HWND m_hEditBoxText;
	HWND m_hEditRunCMD;

	HWND m_hStaRemainTime;
	HWND m_hBtnCancelTask;
	HWND m_hBtnOK;

	INT64 m_remainTimeSec;
	bool m_tasking;

};

