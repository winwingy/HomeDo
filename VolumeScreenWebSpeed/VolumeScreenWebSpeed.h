#pragma once

#include "WindowControl.h"
class VolumeScreenWebSpeedControl;

class VolumeScreenWebSpeed : public WindowControl
{
public:
    VolumeScreenWebSpeed();
    ~VolumeScreenWebSpeed();

    bool Init();
    void SetVisible();

private: 
    virtual bool OnCreate(HWND hWnd, UINT uMsg,
                          WPARAM wParam, LPARAM lParam, LRESULT* result) override;
    virtual bool OnTimer(HWND hWnd, UINT uMsg, UINT_PTR idEvent,
                         DWORD dwTime, LRESULT* result) override;
    virtual bool OnHotKey(HWND hWnd, UINT uMsg, int idHotKey,
                          LPARAM lParam, LRESULT* result) override;
    virtual bool WndProc(UINT message, WPARAM wParam, LPARAM lParam,
                         LRESULT* lResult) override;

    HANDLE mutexHandle_;
    std::unique_ptr<VolumeScreenWebSpeedControl> controller_;
};
