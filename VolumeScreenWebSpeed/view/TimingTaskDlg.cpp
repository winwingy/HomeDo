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
#include "tool/config.h"
#include "tool/StringPathHelper.h"


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
	m_hComboRunCMD = nullptr;

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

void TimingTaskDlg::initControl(enTask_type taskType)
{
	SendMessage(m_hRadioCountDown, BM_SETCHECK, BST_CHECKED, 0);
	SetWindowText(m_hEditHour, _T("0"));
	tstring countTime = Config::GetShared()->GetValue(_T("TimingTask"),
		_T("TimeMinDefault"), _T("20"));
	SetWindowText(m_hEditMin, countTime.c_str());
	SetWindowText(m_hEditSec, _T(""));

	SendMessage(m_hRadioRepeatNo, BM_SETCHECK, BST_CHECKED, 0);

	tstring text =  Config::GetShared()->GetValue(_T("TimingTask"),
		_T("ShowTextDefault"), _T("任务"));
	SetWindowText(m_hEditBoxText, text.c_str());

	SendMessage(m_hComboRunCMD, CB_RESETCONTENT, 0, 0);
	m_itemDataList.clear();

	vector<string> listText;
	Config::GetShared()->GetList(_T("[RunCMDBegin]"), 
		_T("[RunCMDEnd]"), &listText);
	for (auto& per : listText)
	{
		vector<string> detailList;
		StringPathHelper::SplitStringBySign(per, "||", &detailList);
		if (detailList.size() < 2)
			continue;

		comBoItemData itemData;
		itemData.title = detailList[0];
		itemData.cmd = detailList[1];
		itemData.showText = detailList[2];
		itemData.totalSec = StringPathHelper::StringToInt(detailList[3]);
		m_itemDataList.push_back(itemData);
		
		std::string cmdStr = detailList[0] + "||" + detailList[1];
		SendMessage(m_hComboRunCMD, CB_ADDSTRING, 0, (LPARAM)cmdStr.c_str());		
	}
	SendMessage(m_hComboRunCMD, CB_SETCURSEL, taskType, 0);
	onComboSelChange();
}

TimingTaskDlg::comBoItemData TimingTaskDlg::getItemData(int index)
{
	if (index >= 0 && index < (int)m_itemDataList.size())
	{
		return m_itemDataList[index];
	}
	return comBoItemData();
}

TimingTaskDlg::comBoItemData TimingTaskDlg::getSelItemData()
{
	DWORD selIndex = SendMessage(m_hComboRunCMD, CB_GETCURSEL, 0, 0);
	if (selIndex != CB_ERR)
	{
		return getItemData(selIndex);
	}
	return comBoItemData();
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
	DWORD selIndex = SendMessage(m_hComboRunCMD, CB_GETCURSEL, 0, 0);
	if (selIndex != CB_ERR)
	{
		DWORD textLen = SendMessage(m_hComboRunCMD, CB_GETLBTEXTLEN, 
			selIndex, 0);
		tstring runCmd2(textLen+1, char(0));
		SendMessage(m_hComboRunCMD, CB_GETLBTEXT,
			selIndex, (LPARAM)runCmd2.data());	
		std::vector<string> runCmd2List;
		StringPathHelper::SplitStringBySign(runCmd2, "||", &runCmd2List);
		string runFinal = runCmd2;
		if (runCmd2List.size() >= 2)
		{
			runFinal = runCmd2List[1];
		}
		if (runFinal != "None")
		{
			system(runCmd2.c_str());
		}
	}

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
		__super::close();
	}
}

void TimingTaskDlg::onComboMsg(int wmId, int wmEvent, LPARAM lParam)
{
	switch (wmEvent)
	{
		case CBN_SELCHANGE:
		{
			onComboSelChange();
			break;
		}
	default:
		break;
	}

}

void TimingTaskDlg::onComboSelChange()
{
	comBoItemData itemData = getSelItemData();
	if (!itemData.cmd.empty())
	{
		SetWindowText(m_hEditBoxText, itemData.showText.c_str());
		setShowTime(itemData.totalSec);
	}
}

void TimingTaskDlg::setShowTime(int totalSec)
{
	int hour = totalSec / 60 / 60;
	int mininute = totalSec / 60 % 60;
	int sec = totalSec % 60;
	SetWindowText(m_hEditHour,
		StringPathHelper::IntToString(hour).c_str());
	SetWindowText(m_hEditMin,
		StringPathHelper::IntToString(mininute).c_str());
	SetWindowText(m_hEditSec,
		StringPathHelper::IntToString(sec).c_str());
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
		case IDC_COMBO_RUNCMD:
		{
			onComboMsg(wmId, wmEvent, lParam);
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
	case WM_CLOSE:
	{
		close();
		*lResult = TRUE;
		return true;
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
		m_hComboRunCMD = GetDlgItem(m_hWnd, IDC_COMBO_RUNCMD);

		m_hStaRemainTime = GetDlgItem(m_hWnd, IDC_STATIC_RemainTime);
		m_hBtnCancelTask = GetDlgItem(m_hWnd, ID_CancelTask);

		m_hBtnOK = GetDlgItem(m_hWnd, IDOK);

		initControl(enTask_type_normal);
		break;
	}
	case WM_INITSTATE:
	{
		initControl((enTask_type)wParam);
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

void TimingTaskDlg::setTaskType(enTask_type taskType)
{
	PostMessage(m_hWnd, WM_INITSTATE, (WPARAM)taskType, 0);
}
