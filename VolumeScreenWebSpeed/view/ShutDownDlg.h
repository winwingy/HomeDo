#pragma once
#include "DlgControl.h"
class ShutDownDlg : public DlgControl
{
public:
    ShutDownDlg(void);
    ~ShutDownDlg(void);
	bool CreateDlgE(HWND hwnd);
    void ShowDlg();
	bool isTasking();

private:
    static bool RunShutDownCMD(INT64 sec, bool Cancel);
	virtual bool DlgProc(UINT message, WPARAM wParam, LPARAM lParam,
		LRESULT* lResult) override;
	void setControlState(bool enable);
};

