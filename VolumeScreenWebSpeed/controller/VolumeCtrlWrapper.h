#pragma once
class CMyVolumeCtrl;
class Config;
class VolumeCtrlWrapper
{
public:
    VolumeCtrlWrapper(void);
    ~VolumeCtrlWrapper(void);

    bool InitVolumeHotKey(HWND hWnd);
    void OnTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
    void OnHotKey(HWND hwnd, UINT uMsg, int idHotKey, LPARAM lParam);
private:
    int perVolumeGap_;
    bool userChangedVolume_;
    Config* config_;
    std::unique_ptr<CMyVolumeCtrl> myVolumeCtrl_;

};

