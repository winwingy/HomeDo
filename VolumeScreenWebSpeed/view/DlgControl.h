#pragma once
class DlgControl
{
public:
    DlgControl(void);
    virtual ~DlgControl(void);
	void setVisible(bool vis);
	bool isVisible();
	void setDelteOnClose(bool del = false);

protected:
	virtual bool CreateDlg(HWND hwnd, int dlgId, int x, int y,
		int width, int height);
	virtual bool CreateDlg(HWND hwnd, int dlgId, int width, int height);
	virtual bool DlgProc(UINT message, WPARAM wParam, LPARAM lParam,
		LRESULT* lResult);
	
    static INT_PTR CALLBACK StaDlgProc(HWND hWnd, UINT message,
                                            WPARAM wParam, LPARAM lParam);

protected:
	HWND m_hWnd;
	bool m_delOnClose;
};

