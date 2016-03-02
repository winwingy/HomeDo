#include "StdAfx.h"
#include "EasyWindow.h"
#include <assert.h>


EasyWindow::EasyWindow(void)
	: hWnd_(nullptr)
{
}


EasyWindow::~EasyWindow(void)
{
}

LRESULT CALLBACK EasyWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_CREATE:
		{
			SetTimer(hWnd, 1003, 5000, nullptr);
			break;
		}
	case WM_TIMER:
		{
			KillTimer(hWnd, wParam);
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		}
	case WM_PAINT:
		{
			PAINTSTRUCT ps = {0};
			HDC hdc = BeginPaint(hWnd, &ps);
			char* text = "Not Screen Save OK";
			RECT rect;
			GetClientRect(hWnd, &rect);
			DrawText(hdc, text, strlen(text), &rect,
				DT_VCENTER|DT_CENTER|DT_SINGLELINE);
			EndPaint(hWnd, &ps);
			break;
		}
	default:
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		}
	}
	return 0;
	
}


HWND EasyWindow::Create(HWND hWndParent, int x, int y, int cx, int cy)
{
	WNDCLASSEX wcs = {0};
	wcs.cbSize = sizeof(wcs);
	wcs.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wcs.hInstance = GetModuleHandle(nullptr);
	wcs.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcs.lpfnWndProc = EasyWndProc;
	wcs.lpszClassName = "EasyWIndow";
	wcs.style = CS_HREDRAW | CS_VREDRAW;
	UnregisterClass(wcs.lpszClassName, wcs.hInstance);
	ATOM ret = RegisterClassEx(&wcs);
	assert(ret && L"RegisterClassEx Fail");
	if (!ret)
	{
		return nullptr;
	}
	HWND hwnd = CreateWindowEx(WS_EX_TOOLWINDOW, wcs.lpszClassName, "", 
		WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_POPUP, 
		x, y, cx, cy, hWndParent,
		nullptr, wcs.hInstance, nullptr);
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
		SWP_NOACTIVATE |SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
	assert(hwnd && L"CreateWindowEx Fail");
	UpdateWindow(hwnd);
	hWnd_ = hwnd;
	return hwnd;
}

void EasyWindow::Close()
{
	SendMessage(hWnd_, WM_CLOSE, 0, 0);
}

EasyWindow::operator HWND()
{
	return hWnd_;
}

HWND EasyWindow::operator &()
{
	return hWnd_;
}
