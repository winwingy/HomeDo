#include "stdafx.h"
#include "VolumeScreenWebSpeed.h"

ATOM VolumeScreenWebSpeed::MyRegisterClass(HINSTANCE hInstance, 
                                           const TCHAR* szWindowClass)
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style			= CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc	= WndProc;
    wcex.cbClsExtra		= 0;
    wcex.cbWndExtra		= 0;
    wcex.hInstance		= hInstance;
    //任务切换时看到的图标（16*16）、 exe上的图标（32*32）
    wcex.hIcon			= (HICON)LoadImage(NULL,"icon_show.ico",
                                            IMAGE_ICON,0,0,LR_LOADFROMFILE);
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_VOLUMESCREENWEBSPEED);
    wcex.lpszClassName	= szWindowClass;
    //窗口右上角图标、 任务栏图标、 任务管理器看到的图标（16*16）
    wcex.hIconSm		= (HICON)LoadImage(NULL,"icon_show.ico",
                                            IMAGE_ICON,0,0,LR_LOADFROMFILE);
    //(HICON)LoadImage(NULL,"icon_show.ico",IMAGE_ICON,0,0,LR_LOADFROMFILE);

    return RegisterClassEx(&wcex);
}

HWND Create(HWND hWnd)
{   
    ATOM MyRegisterClass(HINSTANCE hInstance, const TCHAR* szWindowClass);
    HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
                        500, 300, 500, 300, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    string strValue = WinControlTool::GetInstance()->GetValueFromConfig(
        CONFIG_SET, "IsShow", "0", CONFIG_INF_FILENAME);
    ShowWindow(hWnd, atoi(strValue.c_str()));
    UpdateWindow(hWnd);
    return TRUE;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中 
   InitUseJobConfig();

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      500, 300, 500, 300, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   string strValue = WinControlTool::GetInstance()->GetValueFromConfig(
       CONFIG_SET, "IsShow", "0", CONFIG_INF_FILENAME); 
   ShowWindow(hWnd, atoi(strValue.c_str()));
   UpdateWindow(hWnd);
   return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_CREATE:
		{
			WinControlTool::GetInstance()->OnCreate(hWnd, message, wParam, lParam);					
			break;
		}
	case WM_HOTKEY:
		{
			WinControlTool::GetInstance()->OnHotKey(hWnd, message, wParam, lParam);
			break;			
		}
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
