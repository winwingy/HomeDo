#pragma once
class WindowControl
{
public:
    WindowControl();
    ~WindowControl();
    virtual HWND Create(HWND hWnd, int x, int y, int width, int height);
    virtual void Show(bool visible);

protected: 
    virtual LRESULT WndProc(HWND hWnd, UINT message,
                            WPARAM wParam, LPARAM lParam);
    virtual void CreateParam(DWORD* style, DWORD* styleEx);
    bool MyRegisterClass(HINSTANCE hInstance, const TCHAR* szWindowClass);
 
    static LRESULT CALLBACK WndProcSta(HWND hWnd, UINT message,
                                       WPARAM wParam, LPARAM lParam);
    HINSTANCE hInst_;
    HWND hWnd_;
};
