#pragma once

class EasyWindow
{
public:
	EasyWindow(void);
	~EasyWindow(void);
	HWND Create(HWND hWndParent, int x, int y, int cx, int cy);
	void Close();
	operator HWND ();
	HWND operator &();

private:
	HWND hWnd_;
};

