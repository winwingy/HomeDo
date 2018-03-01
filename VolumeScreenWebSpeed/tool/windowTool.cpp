#include "stdafx.h"
#include "windowTool.h"

tstring WindowTool::GetWindowText(HWND hWnd)
{
	static TCHAR* szBuf = new TCHAR[5*1024];
	int len = ::GetWindowText(hWnd, szBuf, 5 * 1024);
	szBuf[len] = 0;
	return szBuf;
}
