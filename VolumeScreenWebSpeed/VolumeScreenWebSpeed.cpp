// VolumeScreenWebSpeed.cpp : ����Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include "VolumeScreenWebSpeed.h"

#include "WinDefine.h"
#include "WinContorlTool.h"
#include <assert.h>


#define MAX_LOADSTRING 100

// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������


// �˴���ģ���а����ĺ�����ǰ������:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_VOLUMESCREENWEBSPEED, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_VOLUMESCREENWEBSPEED));

	// ����Ϣѭ��:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	//�����л�ʱ������ͼ�꣨16*16���� exe�ϵ�ͼ�꣨32*32��
	//wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VOLUMESCREENWEBSPEED));
	wcex.hIcon			= (HICON)LoadImage(NULL,"icon_show.ico",IMAGE_ICON,0,0,LR_LOADFROMFILE);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_VOLUMESCREENWEBSPEED);
	wcex.lpszClassName	= szWindowClass;
	//�������Ͻ�ͼ�ꡢ ������ͼ�ꡢ ���������������ͼ�꣨16*16��
	wcex.hIconSm		= (HICON)LoadImage(NULL,"icon_show.ico",IMAGE_ICON,0,0,LR_LOADFROMFILE);//(HICON)LoadImage(NULL,"icon_show.ico",IMAGE_ICON,0,0,LR_LOADFROMFILE);

	return RegisterClassEx(&wcex);
}

void InitUseJobConfig()
{
	WinDefine* winDefine = WinDefine::GetInstance();
	char userName[1024];
	DWORD nameLength = 1024;
	BOOL ret = GetUserName(userName, &nameLength);
	DWORD err = GetLastError();
	assert(ret);
	if (ret)
	{
		string congfigName = WinControlTool::GetInstance()->
			GetValueFromConfig(CONFIG_SET, 
			"JobConfigName", "NotConfigName@#$%!##", CONFIG_INF_FILENAME); 
		if (congfigName == userName)
		{
			WinDefine::GetInstance()->useJobConfig_ = true;
		}
	}
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ����� 
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

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	return (INT_PTR)FALSE;
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
		// �����˵�ѡ��:
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
		// TODO: �ڴ���������ͼ����...
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
