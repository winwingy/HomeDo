// VolumeAndScreen.cpp : ����Ӧ�ó������ڵ㡣
//
//
//#include "stdafx.h"
//#include "VolumeAndScreen.h"
//#include "MyVolume.h"
//#include <stdio.h>
//#include <Shellapi.h>
//
//
//#define MAX_LOADSTRING 100
//
//// ȫ�ֱ���:
//HINSTANCE g_hInst;								// ��ǰʵ��
//TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
//TCHAR szWindowClass[MAX_LOADSTRING];			// ����������
//
//// �˴���ģ���а����ĺ�����ǰ������:
//ATOM				MyRegisterClass(HINSTANCE hInstance);
//BOOL				InitInstance(HINSTANCE, int);
//LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
//
//
//
//
////�¼�ID
//const int SET_SCREEN = 5000;
//const int TIMER_SCREEN_LEFT = WM_USER + 96;
//const int CLOSE_MONITOR = 3000;
//const int WM_TASKMANAGE = 4000;
////Timer�¼�ID
//const int TIMER_CLOSE_SCREEN = 300;
//const int TIMER_CLOSE_SCREEN_ADD =301;
//
//char g_szShowTimer[256] = {0};
//int g_iShowSecond(0);
//const char* SHOWTIPS_BEGIN = "��Ļ�������� ";
//const char* SHOWTIPS_END = " ��";
//int g_iScreenTime(60 *15);
//const WCHAR* TIPS_CLOSESCREENSET = L"��ֱ�Ӱ���ݼ�����";
//
//BOOL g_bScreenFlag = FALSE;
//BOOL g_bCloseMoniter = FALSE;
//NOTIFYICONDATA m_SkyNID;
//LONG g_lOldProc;
//HWND g_hwndSttTipsCloseScreen;
//
//
//
//int APIENTRY _tWinMain(HINSTANCE hInstance,
//                     HINSTANCE hPrevInstance,
//                     LPTSTR    lpCmdLine,
//                     int       nCmdShow)
//{
//	UNREFERENCED_PARAMETER(hPrevInstance);
//	UNREFERENCED_PARAMETER(lpCmdLine);
//
// 	// TODO: �ڴ˷��ô��롣
//	MSG msg;
//	HACCEL hAccelTable;
//
//	// ��ʼ��ȫ���ַ���
//	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
//	LoadString(hInstance, IDC_VOLUMEANDSCREEN, szWindowClass, MAX_LOADSTRING);
//	MyRegisterClass(hInstance);
//
//	// ִ��Ӧ�ó����ʼ��:
//	if (!InitInstance (hInstance, nCmdShow))
//	{
//		return FALSE;
//	}
//
//	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_VOLUMEANDSCREEN));
//
//	// ����Ϣѭ��:
//	while (GetMessage(&msg, NULL, 0, 0))
//	{
//		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
//		{
//			TranslateMessage(&msg);
//			DispatchMessage(&msg);
//		}
//	}
//
//	return (int) msg.wParam;
//}
//
//
//
////
////  ����: MyRegisterClass()
////
////  Ŀ��: ע�ᴰ���ࡣ
////
////  ע��:
////
////    ����ϣ��
////    �˴�������ӵ� Windows 95 �еġ�RegisterClassEx��
////    ����֮ǰ�� Win32 ϵͳ����ʱ������Ҫ�˺��������÷������ô˺���ʮ����Ҫ��
////    ����Ӧ�ó���Ϳ��Ի�ù�����
////    ����ʽ��ȷ�ġ�Сͼ�ꡣ
////
//ATOM MyRegisterClass(HINSTANCE hInstance)
//{
//	WNDCLASSEX wcex;
//
//	wcex.cbSize = sizeof(WNDCLASSEX);
//
//	wcex.style			= CS_HREDRAW | CS_VREDRAW;
//	wcex.lpfnWndProc	= WndProc;
//	wcex.cbClsExtra		= 0;
//	wcex.cbWndExtra		= 0;
//	wcex.hInstance		= hInstance;
//	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VOLUMEANDSCREEN));
//	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
//	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
//	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_VOLUMEANDSCREEN);
//	wcex.lpszClassName	= szWindowClass;
//	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
//
//	return RegisterClassEx(&wcex);
//}
//
////
////   ����: InitInstance(HINSTANCE, int)
////
////   Ŀ��: ����ʵ�����������������
////
////   ע��:
////
////        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
////        ��������ʾ�����򴰿ڡ�
////
//BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
//{
//   HWND hWnd;
//
//   g_hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����
//
//
//   int iLeft = 0;
//   int iTop = 0;
//   int iWidth = 300;
//   int iHeight = 250;
//
//   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
//      iLeft, iTop, iWidth, iHeight, NULL, NULL, hInstance, NULL);
//
//   int iX = GetSystemMetrics(SM_CXSCREEN);
//   int iY = GetSystemMetrics(SM_CYSCREEN);
//
//   MoveWindow(hWnd, (iX - iWidth)/2, (iY - iHeight)/2, iWidth, iHeight, TRUE);
//
//
//   if (!hWnd)
//   {
//      return FALSE;
//   }
//
//  // ShowWindow(hWnd, nCmdShow);
//   UpdateWindow(hWnd);
//
//   m_SkyNID.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
//   m_SkyNID.hWnd = hWnd;
//   m_SkyNID.uID = IDI_ICON_TASK;
//   m_SkyNID.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP ;
//   m_SkyNID.dwState = NIS_SHAREDICON;
//   m_SkyNID.uCallbackMessage = WM_TASKMANAGE;
//   m_SkyNID.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_TASK));
//   wcscpy_s(m_SkyNID.szTip, _T("wingy������Ļ��ݼ�"));
//   Shell_NotifyIcon(NIM_ADD, &m_SkyNID);
//
//
//   return TRUE;
//}
//
//
//VOID CALLBACK TimerProc(          HWND hwnd,
//						UINT uMsg,
//						UINT_PTR idEvent,
//						DWORD dwTime
//						)
//{
//	if ( TIMER_CLOSE_SCREEN == idEvent  )
//	{
//		::SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2);	
//		KillTimer(hwnd, TIMER_CLOSE_SCREEN);
//	}
//
//// 	if ( g_iShowSecond > 0 )
//// 	{
//// 		--g_iShowSecond;
//// 	}	
//// 	else
//// 	{
//// 		KillTimer(hwnd, TIMER_SCREEN_LEFT);
//// 	}
//// 	sprintf_s(g_szShowTimer, sizeof(g_szShowTimer), "��Ļ�������� %d ��", g_iShowSecond);
//// 	InvalidateRect(hwnd, NULL, TRUE );
//}
//
//
//			
//
////�༭��ص�����
//LRESULT CALLBACK EditCloseScreenProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//		
//	switch( uMsg )
//	{
//	case WM_LBUTTONDOWN:
//		{
//			SetWindowText( g_hwndSttTipsCloseScreen, TIPS_CLOSESCREENSET );
//			OutputDebugString(L"WM_LBUTTONDOWN\n");
//			break;
//		}
//	case WM_KEYDOWN:
//		{
//			WCHAR szShowTip[1024] = {0};		
//			SHORT hState = GetKeyState(VK_CONTROL);		
//			if ( (hState & 0xFF00) )
//			{
//				OutputDebugString(L"VK_CONTROL\n");
//				wcscat( szShowTip, L"Control" );
//			}
//			hState = GetKeyState(VK_SHIFT);
//			if ( (hState & 0xFF00) )
//			{
//				OutputDebugString(L"VK_CONTROL\n");
//				if ( wcslen(szShowTip) > 0 )
//				{
//					wcscat( szShowTip, L" + " );
//				}
//				wcscat( szShowTip, L"Shift" );
//			}
//			hState = GetKeyState(VK_MENU);
//			if ( (hState & 0xFF00) )
//			{
//				OutputDebugString(L"VK_CONTROL\n");
//				if ( wcslen(szShowTip) > 0 )
//				{
//					wcscat( szShowTip, L" + " );
//				}
//				wcscat( szShowTip, L"ALT" );
//			}		
//			
//			if ( wcslen(szShowTip) > 0 )
//			{
//				wcscat( szShowTip, L" + " );
//			}				
//			int iLen = wcslen(szShowTip);
//			WCHAR szChar[2];
//			swprintf_s( szChar, sizeof(szChar)/sizeof(WCHAR), L"%c", wParam );
//			wcscat(szShowTip, szChar);
//			//swprintf_s( szShowTip + wcslen(szShowTip), sizeof(szShowTip)/sizeof(WCHAR), L"%c", wParam );
//
//			SetWindowText(hWnd, L"");
//
//			SetWindowText(hWnd, szShowTip);
//			break;
//		}
//	}
//
//	return CallWindowProc((WNDPROC)g_lOldProc, hWnd, uMsg, wParam, lParam);
//
//}
//
//LRESULT CALLBACK AccKeyProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//	switch(uMsg)
//	{
//	case WM_INITDIALOG:
//		{
//			g_hwndSttTipsCloseScreen = GetDlgItem(hWnd, IDC_STATIC_CLOSE_SCREEN_TIPS);
//
//			//�༭��ؼ�
//			HWND hEditCloseScreen = GetDlgItem(hWnd, IDC_EDIT_CLOSE_SCREEN);
//			//��ʼ���༭��ؼ��ص�����
//			g_lOldProc = SetWindowLong(hEditCloseScreen, GWL_WNDPROC, (LONG)EditCloseScreenProc);
//
//
//			//
//			int iLeft = 0;
//			int iTop = 0;
//			int iWidth = 500;
//			int iHeight = 250;
//
//			int iX = GetSystemMetrics(SM_CXSCREEN);
//			int iY = GetSystemMetrics(SM_CYSCREEN);
//
//			MoveWindow(hWnd, (iX - iWidth)/2, (iY - iHeight)/2, iWidth, iHeight, TRUE);
//			break;
//		}
//	case WM_COMMAND:
//		{
//
//			if ( LOWORD(wParam) == IDCANCEL )
//			{
//				EndDialog(hWnd, TRUE);//ֻ����ģʽ�Ի���
//				return TRUE;
//			}
//			break;
//		}
//	}
//	return FALSE;//��Ϣû�б��������������ڼ������� 
//
//}
//
//
////
////  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
////
////  Ŀ��: ���������ڵ���Ϣ��
////
////  WM_COMMAND	- ����Ӧ�ó���˵�
////  WM_PAINT	- ����������
////  WM_DESTROY	- �����˳���Ϣ������
////
////
//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	int wmId, wmEvent;
//	PAINTSTRUCT ps;
//	HDC hdc;
//
//	switch (message) 
//	{
//	case WM_COMMAND:
//		wmId    = LOWORD(wParam); 
//		wmEvent = HIWORD(wParam); 
//		// �����˵�ѡ��:
//		switch (wmId)
//		{
//		case IDM_EXIT:
//			UnregisterHotKey(hWnd, 1000);
//			DestroyWindow(hWnd);
//			break;
//		case ID_MENU_ACCKEY:
//			{
//				DialogBox(g_hInst, (LPCTSTR)(IDD_DLG_SET), hWnd, (DLGPROC)AccKeyProc);
//
//				break;
//			}
//		default:
//			return DefWindowProc(hWnd, message, wParam, lParam);
//		}
//		break;
//
//	case WM_DESTROY:
//		Shell_NotifyIcon(NIM_DELETE, &m_SkyNID);
//		PostQuitMessage(0);
//		break;
//	case WM_CREATE:
//		{
//			BOOL bRet = RegisterHotKey(hWnd, 1000, MOD_ALT, VK_PRIOR);
//			bRet = RegisterHotKey(hWnd, 1001, MOD_ALT, VK_NEXT);
//			//bRet = RegisterHotKey(hWnd, SET_SCREEN, MOD_ALT|MOD_CONTROL, ',');
//			bRet = RegisterHotKey(hWnd, CLOSE_MONITOR, MOD_ALT|MOD_CONTROL, 'P');
//
//			break;
//		}
//	case WM_TASKMANAGE:
//		{
//			switch(lParam)
//			{
//			case WM_LBUTTONDOWN:
//			case  WM_RBUTTONDOWN:
//				{
//					HMENU   hMenu   =   LoadMenu( NULL, MAKEINTRESOURCE(IDR_MENU_TASK)); 
//					hMenu=GetSubMenu(hMenu,0); 
//					POINT point;
//					GetCursorPos(&point);
//					SetForegroundWindow(hWnd);//ʹ�����������Ϊ�˽��˵���Ϊǰ�˴��ڣ��������뿪�˵�������˫���˵������Զ���ʧ������
//					TrackPopupMenu (hMenu,   TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RIGHTBUTTON,   point.x,   point.y,   0,   hWnd,   NULL);
//					break;
//				}
//			}		
//			//ShowWindow(hWnd, SW_SHOW);
//			break;
//		}
//	case WM_PAINT:
//		{
//			hdc = BeginPaint(hWnd, &ps);
//// 			RECT rt;
//// 			GetClientRect(hWnd, &rt);
//// 			DrawTextA(hdc, SHOWTIPS_BEGIN, strlen(SHOWTIPS_BEGIN), &rt, DT_CENTER);
//// 
//// 			SetTextColor(hdc, RGB(255, 0, 0));
//// 			HFONT hFont = CreateFontA(30,0,45,0,900,TRUE,
//// 				FALSE,FALSE,0,0,0,0,0,"����");
//// 			HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
//// 			char szShowTime[20];
//// 			sprintf_s(szShowTime, sizeof(szShowTime), "%d", g_iShowSecond);
//// 			TextOutA( hdc, 100, 20, szShowTime, strlen(szShowTime));
//// 
//// 			//��
//// 			SelectObject(hdc, hOldFont);
//// 			SetTextColor(hdc, RGB(0, 0, 0));
//// 			TextOutA( hdc, 140, 30, SHOWTIPS_END, strlen(SHOWTIPS_END));
//
//
//			EndPaint(hWnd, &ps);
//			break;
//		}
//	case  WM_LBUTTONDOWN:
//		{
//// 			int iTime = 60;//����Ϊ��λ�� ��С��60S
//// 			BOOL bRet = SystemParametersInfo( SPI_SETSCREENSAVETIMEOUT, g_iScreenTime, 0, SPIF_UPDATEINIFILE|SPIF_SENDCHANGE);
//// 			ShowWindow(hWnd, SW_HIDE);
//// 			break;
//		}
//	case WM_HOTKEY:
//		{
//		//	OutputDebugString("WM_HOTKEY");
//			int iVolume;
//			char szVolume[256];
//			//OutputDebugString(szVolume);
//			int iIDHotKey = (int)wParam;
//
//			int iTime(0);
//			BOOL bRet(FALSE);
//			switch(iIDHotKey)
//			{
//			case 1000:
//				{
//					iVolume = CMyVolume::GetVolume();	//�õ���ֵƫС1						
//					sprintf_s(szVolume,sizeof(szVolume), "iVolume = %d\n", iVolume);
//					iVolume += 31;
//					CMyVolume::SetValue(iVolume);
//					break;
//				}
//			case 1001:
//				{
//					iVolume = CMyVolume::GetVolume();	//�õ���ֵƫС1						
//					sprintf_s(szVolume,sizeof(szVolume), "iVolume = %d\n", iVolume);
//					iVolume -= 29;
//					CMyVolume::SetValue(iVolume);
//					break;
//				}
//			case SET_SCREEN:
//				{
//// 					if ( g_bScreenFlag == FALSE )
//// 					{
//// 						bRet = SystemParametersInfo( SPI_GETSCREENSAVETIMEOUT, 0, &g_iScreenTime, SPIF_UPDATEINIFILE|SPIF_SENDCHANGE);
//// 
//// 						iTime = 60;//����Ϊ��λ�� ��С��60S
//// 						bRet = SystemParametersInfo( SPI_SETSCREENSAVETIMEOUT, iTime, 0, SPIF_UPDATEINIFILE|SPIF_SENDCHANGE);
//// 						if ( bRet )
//// 						{						
//// 							ShowWindow(hWnd, SW_SHOW);
//// 							g_iShowSecond = 60;
//// 							sprintf_s(g_szShowTimer, sizeof(g_szShowTimer), "��Ļ�������� %d ��", g_iShowSecond);
//// 							SetTimer(hWnd, TIMER_SCREEN_LEFT, 1000, TimerProc);
//// 						}
//// 						g_bScreenFlag =! g_bScreenFlag;
//// 					}
//// 					else
//// 					{
//// 						ShowWindow(hWnd, SW_SHOW);
//// 						g_bScreenFlag =! g_bScreenFlag; 
//// 					}
//					break;
//				}			
//			case CLOSE_MONITOR:
//				{
//// 					if ( g_bCloseMoniter == FALSE )
//// 					{
//						SetTimer(hWnd, TIMER_CLOSE_SCREEN, 3000, TimerProc);
//								
//// 					}
//// 					else
//// 					{
//// 						::SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)1);
//// 					}
//// 					g_bCloseMoniter =! g_bCloseMoniter;
//					break;
//				}
//			}//end switch(iIDHotKey)
//		}//end case WM_HOTKEY:
//
//	default:
//		return DefWindowProc(hWnd, message, wParam, lParam);
//	}//end switch (message) 
//	return 0;
//}
//
//
