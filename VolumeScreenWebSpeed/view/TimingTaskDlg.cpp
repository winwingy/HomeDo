#include "stdafx.h"
#include <Windows.h>
#include <sstream>
#include "TimingTaskDlg.h"
#include "../resource.h"
#include <time.h>
#include <assert.h>
#include <iomanip>
#include "TaskRemindDlg.h"
#include "tool/windowTool.h"


namespace
{
	const int kTimerId_remain = 200;
	const int kTimerId_refresh = 100;
	const int kRefreshTime = 1000;
}

TimingTaskDlg::TimingTaskDlg(void)
{
	m_hRadioCountDown = nullptr;
	m_hRadioFixTime = nullptr;
	m_hEditHour = nullptr;
	m_hEditMin = nullptr;
	m_hEditSec = nullptr;

	m_hRadioRepeatYes = nullptr;
	m_hRadioRepeatNo = nullptr;

	m_hEditBoxText = nullptr;

	m_hStaRemainTime = nullptr;
	m_hBtnCancelTask = nullptr;
	m_hBtnOK = nullptr;

	m_remainTimeSec = 0;
	m_tasking = false;
}


TimingTaskDlg::~TimingTaskDlg(void)
{
	int a = 1;
}

void TimingTaskDlg::CreateDlgE(HWND hWnd)
{
	__super::CreateDlg(hWnd, IDD_DIALOG_TASK);
}

bool TimingTaskDlg::isTasking()
{
	return m_tasking;
}

void TimingTaskDlg::initControl()
{
	SendMessage(m_hRadioCountDown, BM_SETCHECK, BST_CHECKED, 0);
	SetWindowText(m_hEditHour, _T("1"));
	SetWindowText(m_hEditMin, _T(""));
	SetWindowText(m_hEditSec, _T(""));

	SendMessage(m_hRadioRepeatNo, BM_SETCHECK, BST_CHECKED, 0);

	SetWindowText(m_hEditBoxText, _T("1小时任务提醒"));
}


void TimingTaskDlg::getTimerText(int* hour, int* min, int* sec)
{
	HWND hWnd = m_hWnd;
	{
		const int len = 100;
		TCHAR szbuf[len] = { 0 };
		GetWindowText(GetDlgItem(hWnd, IDC_EDIT_HOUR), szbuf, len);
		*hour = atoi(szbuf);
	}

	{
		const int len = 100;
		TCHAR szbuf[len] = { 0 };
		GetWindowText(GetDlgItem(hWnd, IDC_EDIT_MIN), szbuf, len);
		*min = atoi(szbuf);
	}

	{
		const int len = 100;
		TCHAR szbuf[len] = { 0 };
		GetWindowText(GetDlgItem(hWnd, IDC_EDIT_SEC), szbuf, len);
		*sec = atoi(szbuf);
	}
}

bool TimingTaskDlg::isBtnChecked(HWND hWnd)
{
	if (SendMessage(hWnd, BM_GETCHECK, 0, 0)
		== BST_CHECKED)
		return true;

	return false;
}

bool TimingTaskDlg::onFixTimeTask()
{
	bool res = false;
	if (!isBtnChecked(m_hRadioFixTime))
		return res;

	HWND hWnd = m_hWnd;
	int hour = 0;
	int min = 0;
	int sec = 0;
	getTimerText(&hour, &min, &sec);
	INT64 totalSec = 0;
	if (isBtnChecked(m_hRadioRepeatYes))
	{
		SYSTEMTIME st;
		GetLocalTime(&st);

		tm t;
		t.tm_year = st.wYear - 1900;
		t.tm_mon = st.wMonth - 1;
		t.tm_mday = st.wDay;
		t.tm_hour = st.wHour;
		t.tm_min = min;
		t.tm_sec = sec;
		time_t sec = mktime(&t); // 带时区转换功能
		if (sec < time(nullptr))
		{
			sec += 1 * 60 * 60;
		}
		totalSec = sec - time(nullptr);
	}
	else
	{
		SYSTEMTIME st;
		GetLocalTime(&st);

		tm t;
		t.tm_year = st.wYear - 1900;
		t.tm_mon = st.wMonth - 1;
		t.tm_mday = st.wDay;
		t.tm_hour = hour;
		t.tm_min = min;
		t.tm_sec = sec;
		time_t sec = mktime(&t); // 带时区转换功能

		if (sec < time(nullptr))
		{
			assert(0);
			return res;
		}

		totalSec = sec - time(nullptr);
	}
	if (totalSec <= 0)
	{
		assert(totalSec);
		return res;
	}				 
	beginTimerTask(totalSec);
	return true;
}

void TimingTaskDlg::beginTimerTask(INT64 totalSec)
{
	if (totalSec <= 0)
	{
		assert(totalSec);
		return;
	}
	SetTimer(m_hWnd, kTimerId_refresh, kRefreshTime, nullptr);
	setRemainText(totalSec);
	m_remainTimeSec = totalSec;
	m_tasking = true;
}

void TimingTaskDlg::setRemainText(INT64 totalSec)
{
	if (totalSec < 0)
		totalSec = 0;
	
	std::stringstream ss;
	ss.fill('0');
	ss << setw(2) << totalSec / 60 / 60 << " 时 "
		<< setw(2) << totalSec / 60 % 60 << " 分 "
		<< setw(2) << totalSec % 60 << " 秒 ";
	SetWindowText(m_hStaRemainTime, ss.str().c_str());
}

bool TimingTaskDlg::onCountDownTask()
{
	HWND hWnd = m_hWnd;
	int hour = 0;
	int min = 0;
	int sec = 0;
	getTimerText(&hour, &min, &sec);
	INT64 totalSec = static_cast<INT64>(hour * 60 * 60) +
		static_cast<INT64>(min * 60) + static_cast<INT64>(sec);
	if (totalSec == 0)
		return false;

	beginTimerTask(totalSec);
	return true;
}


void TimingTaskDlg::onBtnOk()
{
	bool isOk = false;	
	if (isBtnChecked(m_hRadioFixTime))
	{
		isOk = onFixTimeTask();
	}
	else
	{
		isOk = onCountDownTask();
	}
	if (!isOk)
	{
		MessageBox(m_hWnd, "格式不对", "格式不对", MB_OK);
	}
	else
	{
		setControlEnable(false);
	}
}

void TimingTaskDlg::setHourState()
{
	if (isBtnChecked(m_hRadioRepeatYes))
	{
		if (isBtnChecked(m_hRadioCountDown))
		{
			ShowWindow(m_hEditHour, SW_SHOW);
		}
		else
		{
			ShowWindow(m_hEditHour, SW_HIDE);
		}
	}
	else
	{
		ShowWindow(m_hEditHour, SW_SHOW);
	}
}

void TimingTaskDlg::resetState()
{
	KillTimer(m_hWnd, kTimerId_refresh);
	m_tasking = false;
	setControlEnable(true);
}

void TimingTaskDlg::timerEnd(UINT message, WPARAM wParam,
	LPARAM lParam)
{
	TaskRemindDlg* pRemind = new TaskRemindDlg;
	pRemind->ShowDlg(nullptr);
	pRemind->setRemindText(WindowTool::GetWindowText(m_hEditBoxText));
	pRemind->setDelteOnClose(true);

	if (isBtnChecked(m_hRadioRepeatYes))
	{
		onBtnOk();		
	}
	else
	{
		resetState();
	}	
}

void TimingTaskDlg::onTimer(UINT message, WPARAM wParam,
	LPARAM lParam)
{
	switch (wParam)
	{
	case kTimerId_remain:
	{
		break;
	}
	case kTimerId_refresh:
	{
		m_remainTimeSec--;
		if (m_remainTimeSec <= 0)
		{
			m_remainTimeSec = 0;
			timerEnd(message, wParam, lParam);
		}
		setRemainText(m_remainTimeSec);		
		break;
	}
	default:
	{
		assert(0);
		break;
	}
	}
}

void TimingTaskDlg::close()
{
	if (m_tasking)
	{
		setVisible(false);
	}
	else
	{
		DestroyWindow(m_hWnd);
	}
}

bool TimingTaskDlg::DlgProc(UINT message, WPARAM wParam,
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
		// 分析菜单选择:
		switch (wmId)
		{
		case IDOK:
		{
			onBtnOk();
			break;
		}
		case ID_CancelTask:
		{
			resetState();
			break;
		}
		case IDCANCEL:
		{
			close();
			break;
		}
		case IDC_RADIO_FixTime:
		case IDC_RADIO_COUNTDOWN:
		case IDC_RADIO_RepeatYes:
		case IDC_RADIO_RepeatNo:
		{
			setHourState();
			break;
		}
		default:
			break;
		}
		break;
	}
	case WM_TIMER:
	{
		onTimer(message, wParam, lParam);
		break;
	}
	case WM_INITDIALOG:
	{
		m_hRadioCountDown = GetDlgItem(m_hWnd, IDC_RADIO_COUNTDOWN);
		m_hRadioFixTime = GetDlgItem(m_hWnd, IDC_RADIO_FixTime);
		m_hEditHour = GetDlgItem(m_hWnd, IDC_EDIT_HOUR);
		m_hEditMin = GetDlgItem(m_hWnd, IDC_EDIT_MIN);
		m_hEditSec = GetDlgItem(m_hWnd, IDC_EDIT_SEC);		

		m_hRadioRepeatYes = GetDlgItem(m_hWnd, IDC_RADIO_RepeatYes);
		m_hRadioRepeatNo = GetDlgItem(m_hWnd, IDC_RADIO_RepeatNo);

		m_hEditBoxText = GetDlgItem(m_hWnd, IDC_EDIT_TaskBoxText);

		m_hStaRemainTime = GetDlgItem(m_hWnd, IDC_STATIC_RemainTime);
		m_hBtnCancelTask = GetDlgItem(m_hWnd, ID_CancelTask);

		m_hBtnOK = GetDlgItem(m_hWnd, IDOK);

		initControl();
		break;
	}
	default:
		break;
	}
	return __super::DlgProc(message, wParam, lParam, lResult);
}

void TimingTaskDlg::setControlEnable(bool enable)
{
	EnableWindow(m_hRadioCountDown, enable);
	EnableWindow(m_hRadioFixTime, enable);
	EnableWindow(m_hEditHour, enable);
	EnableWindow(m_hEditMin, enable);
	EnableWindow(m_hEditSec, enable);
	EnableWindow(m_hRadioRepeatYes, enable);
	EnableWindow(m_hRadioRepeatNo, enable);
	EnableWindow(m_hEditBoxText, enable);
	EnableWindow(m_hBtnOK, enable);	
}
