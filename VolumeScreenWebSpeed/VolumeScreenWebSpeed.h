#pragma once

#include "WindowControl.h"
class VolumeScreenWebSpeedControl;

class VolumeScreenWebSpeed : public WindowControl
{
public:
    VolumeScreenWebSpeed();
    ~VolumeScreenWebSpeed();

    bool Init();
    void Show();

private: 
    virtual bool OnCreate(HWND hWnd, UINT uMsg,
                          WPARAM wParam, LPARAM lParam, LRESULT* result) override;
    virtual bool OnTimer(HWND hWnd, UINT uMsg, UINT_PTR idEvent,
                         DWORD dwTime, LRESULT* result) override;
    virtual bool OnHotKey(HWND hWnd, UINT uMsg, int idHotKey,
                          LPARAM lParam, LRESULT* result) override;

    HANDLE mutexHandle_;
    std::unique_ptr<VolumeScreenWebSpeedControl> controller_;
};
