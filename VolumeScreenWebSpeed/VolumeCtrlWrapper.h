#pragma once
class EasyWindow;
class VolumeCtrlWrapper
{
public:
    VolumeCtrlWrapper(void);
    ~VolumeCtrlWrapper(void);

    bool InitVolumeHotKey(HWND hWnd);

private:
    int perVoulumeGap_;
    Config* config_;

};

