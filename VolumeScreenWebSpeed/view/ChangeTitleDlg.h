#pragma once
#include "DlgControl.h"
class ChangeTitleDlg : public DlgControl
{
public:
    ChangeTitleDlg(void);
    ~ChangeTitleDlg(void);
	void CreateDlgE(HWND hWnd);
	typedef std::function < void(const tstring& orcTitle,
		const tstring& newTitle) > ChangeEvent;
	void setChangeEvent(const ChangeEvent& changeEvent);

protected:
	virtual bool DlgProc(UINT message, WPARAM wParam, LPARAM lParam,
		LRESULT* lResult);

private:
	ChangeEvent changeEvent_;
};

