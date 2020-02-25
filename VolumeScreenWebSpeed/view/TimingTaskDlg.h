#pragma once
#include "DlgControl.h"
#include <map>
class TimingTaskDlg : public DlgControl
{
public:
    TimingTaskDlg(void);
    ~TimingTaskDlg(void);

	void CreateDlgE(HWND hWnd);
	enum 
	{
		WM_INITSTATE = WM_USER + 6780,
	};
	enum enTask_type
	{
		enTask_type_normal,
		enTask_type_sleep,
		enTask_type_showdown,
		enTask_type_loginoff,
	};
	void setTaskType(enTask_type taskType);
	bool isTasking();

protected:
	struct comBoItemData
	{
		std::string title;
		std::string cmd;
		std::string showText;
		int totalSec = 0;
	};
	virtual bool DlgProc(UINT message, WPARAM wParam, LPARAM lParam,
		LRESULT* lResult);

private:
	void initControl(enTask_type taskType);
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
	void onComboMsg(int wmId, int wmEvent, LPARAM lParam);
	void setShowTime(int totalSec);
	TimingTaskDlg::comBoItemData getItemData(int index);
	TimingTaskDlg::comBoItemData getSelItemData();
	void onComboSelChange();
private:
	HWND m_hRadioCountDown;
	HWND m_hRadioFixTime;
	HWND m_hEditHour;
	HWND m_hEditMin;
	HWND m_hEditSec;

	HWND m_hRadioRepeatYes;
	HWND m_hRadioRepeatNo;

	HWND m_hEditBoxText;
	HWND m_hComboRunCMD;

	HWND m_hStaRemainTime;
	HWND m_hBtnCancelTask;
	HWND m_hBtnOK;

	INT64 m_remainTimeSec;
	bool m_tasking;
	std::vector<comBoItemData> m_itemDataList;
};
