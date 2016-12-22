#pragma once
class EasyWindow;
class VolumeCtrlWrapper
{
public:
	VolumeCtrlWrapper(void);
	~VolumeCtrlWrapper(void);

	HWND NotScreenSave(HWND hWndParent, bool showDlg);
	void Close();
	operator HWND ();
	HWND operator &();
    void StopNotScreenSave(HWND hwnd, bool playSound);
    static VOID CALLBACK ScreenTimerProcSta(
        HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD userDefine);
    void ScreenTimerProc(
        HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD userDefine);
private:
	HWND hWnd_;
    HWND forcegroundHwnd_;
    EasyWindow* easyWindow_;
    static VolumeCtrlWrapper* 
};

