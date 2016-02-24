// NoteBook.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <CommDlg.h>
#include "DefineRef.h"
#include "NoteBook.h"


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
	LoadString(hInstance, IDC_NOTEBOOK, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NOTEBOOK));

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
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
//  ע��:
//
//    ����ϣ��
//    �˴�������ӵ� Windows 95 �еġ�RegisterClassEx��
//    ����֮ǰ�� Win32 ϵͳ����ʱ������Ҫ�˺��������÷������ô˺���ʮ����Ҫ��
//    ����Ӧ�ó���Ϳ��Ի�ù�����
//    ����ʽ��ȷ�ġ�Сͼ�ꡣ
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NOTEBOOK));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_NOTEBOOK);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

BOOL GetClipFileName(LPTSTR szName)
{
	OPENFILENAME ofn={0};

	ofn.lStructSize       = sizeof(OPENFILENAME);
	//	ofn.hwndOwner         = ghApp;
	ofn.lpstrFilter       = NULL;
	//ofn.lpstrFilter       = "Video files (*.mpg; *.mpeg; *.mp4)\0*.mpg; *.mpeg; *.mp4\0\0";
	ofn.lpstrFilter       = L"Chess files (*.cnd)\0*.cnd\0\0";
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex      = 1;	
	ofn.lpstrFile         = szName;
	ofn.nMaxFile          = MAX_PATH;
	ofn.lpstrInitialDir   = NULL;
	ofn.lpstrTitle        = NULL;
	ofn.lpstrFileTitle    = NULL;
	//ofn.lpstrDefExt       = "MPG";
	ofn.lpstrDefExt       = NULL;
	//ofn.Flags             = OFN_FILEMUSTEXIST|OFN_READONLY|OFN_PATHMUSTEXIST;
	ofn.Flags             = OFN_PATHMUSTEXIST;
	//bn(GetOpenFileName((LPOPENFILENAME)&ofn));
	return GetOpenFileName((LPOPENFILENAME)&ofn);

} // GetClipFileName

LRESULT OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{	
	HDC hDC;
	PAINTSTRUCT ps;
	hDC = BeginPaint(hWnd, &ps);
// 	SetTextColor(hDC,RGB(255,0,0));
// 	SetBkColor(hDC,RGB(0,255,0));
// 	SetBkMode(hDC,TRANSPARENT);
// 	HFONT hFont = CreateFont(30,0,45,0,900,TRUE,
// 		TRUE,TRUE,0,0,0,0,0,"����");
// 	HFONT hOldFont = (HFONT)SelectObject(hDC,hFont);
// 
// 	CHAR szText[256] = "hello text LONG long long long long long longlong longl onglongl onglonglon glonglong";
// 	TextOut(hDC,100,100,szText,strlen(szText));
// 
// 	RECT rcRect = {0};
// 	rcRect.top = 150;
// 	rcRect.left = 100;
// 	rcRect.bottom = 200;
// 	rcRect.right = 200;
// 	Rectangle(hDC,100,150,200,200);
// 
// 	DrawText(hDC,szText,strlen(szText),&rcRect,
// 		DT_WORDBREAK|DT_RIGHT|DT_VCENTER|DT_NOCLIP);
// 	CHAR szExtText[] =  "A�й�����";
// 	rcRect.left = 100;
// 	rcRect.right = 200;
// 	rcRect.top = 300;
// 	rcRect.bottom = 350;
// 	INT nExtText[] = {20,0,20,0,20,0,20,0};
// 	ExtTextOut(hDC,100,300,ETO_OPAQUE,&rcRect,szExtText,
// 		strlen(szExtText),nExtText);
// 	SelectObject(hDC,hOldFont);
// 	DeleteObject(hFont);
// 	
	EndPaint(hWnd, &ps);

	return S_OK;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	
	
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
		case IDM_OPEN:
			{
				//��ϵͳѡ���
				GetClipFileName(NULL);

				break;
			}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_CREATE:
		{
			//����һ���༭��
			RECT rect;
			GetClientRect(hWnd, &rect);			
			CreateWindow(L"EDIT", L"", ES_AUTOHSCROLL|WS_CHILD|WS_VISIBLE|ES_AUTOVSCROLL|ES_LEFT|ES_MULTILINE,
				rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, hWnd, (HMENU)ID_NOTE_EDIT, hInst, 0);
		}
	case WM_PAINT:
		OnPaint(hWnd, message, wParam, lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
