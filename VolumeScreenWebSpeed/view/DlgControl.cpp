#include "stdafx.h"
#include <Windows.h>
#include <sstream>
#include "DlgControl.h"
#include "../resource.h"
#include <time.h>
#include <assert.h>


DlgControl::DlgControl(void)
	: m_delOnClose(false)
{
}


DlgControl::~DlgControl(void)
{
}

bool DlgControl::DlgProc(UINT message, WPARAM wParam, LPARAM lParam,
	LRESULT* lResult)
{
	return false;
}

INT_PTR CALLBACK DlgControl::StaDlgProc(HWND hWnd, UINT message,
                                              WPARAM wParam, LPARAM lParam)
{
	DlgControl* pThis = nullptr;
	if (message == WM_INITDIALOG)
	{
		pThis = reinterpret_cast<DlgControl*>(lParam);
		if (pThis)
		{
			pThis->m_hWnd = hWnd;
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, lParam);
		}
	}
	else
	{
		pThis = reinterpret_cast<DlgControl*>(
			::GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}
	bool handled = false;
	LRESULT result = 0;
	if (pThis)
	{
		handled = pThis->DlgProc(message, wParam, lParam, &result);
	}
	if (WM_NCDESTROY == message && pThis && pThis->m_delOnClose)
	{
		delete pThis;
	}
	if (handled)
	{
		return result;
	}
	return 0;
}

bool DlgControl::CreateDlg(HWND hwnd, int dlgId)
{
	m_hWnd = CreateDialogParam((HINSTANCE)GetModuleHandle(NULL),
		MAKEINTRESOURCE(dlgId), hwnd, StaDlgProc, 
		reinterpret_cast<LPARAM>(this));
	assert(m_hWnd);
    return true;
}

void DlgControl::setVisible(bool vis)
{
	ShowWindow(m_hWnd, vis ? SW_SHOWNORMAL : SW_HIDE);
}

void DlgControl::activeWindow()
{
	HWND hCurWnd = NULL;
	DWORD dwMyID;
	DWORD   dwCurID;
	hCurWnd = ::GetForegroundWindow();
	dwMyID = ::GetCurrentThreadId();
	dwCurID = ::GetWindowThreadProcessId(hCurWnd, NULL);
	::AttachThreadInput(dwCurID, dwMyID, TRUE);
	::SetForegroundWindow(m_hWnd);
	::AttachThreadInput(dwCurID, dwMyID, FALSE);
}

bool DlgControl::isVisible()
{
	return !!::IsWindowVisible(m_hWnd);
}

void DlgControl::setDelteOnClose(bool del /*= false*/)
{
	m_delOnClose = del;
}

void DlgControl::MoveWindow(int x, int y)
{
	RECT rcThis;
	GetWindowRect(m_hWnd, &rcThis);
	int w = (rcThis.right - rcThis.left);
	int h = (rcThis.bottom - rcThis.top);
	::MoveWindow(m_hWnd, x, y, w, h, TRUE);
}

void DlgControl::showCenter()
{
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	int cx = rect.right - rect.left;
	int cy = rect.bottom - rect.top;

	RECT rcThis;
	GetWindowRect(m_hWnd, &rcThis);
	int w = (rcThis.right - rcThis.left);
	int h = (rcThis.bottom - rcThis.top);

	int x = (cx - w) / 2;
	int y = (cy - h) / 2;
	MoveWindow(x, y);
}
