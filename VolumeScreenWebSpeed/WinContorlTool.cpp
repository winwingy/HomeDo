#include "StdAfx.h"
#include "WinContorlTool.h"
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <TlHelp32.h>
#include <shellapi.h>
#include <assert.h>
#include <sstream>
#pragma comment(lib, "Kernel32.lib")

#include "MyVolumeCtrl.h"

#include "WinDefine.h"
#include "EasyWindow.h"
#include "ShutDownDlg.h"

using namespace std;

namespace
{
struct HotKeyData
{
    string keyString;
    int KeyValue;
};

HotKeyData HotKeyStringMap[] =
{
    {"F1", VK_F1},
    {"F2", VK_F2},
    {"F3", VK_F3},
    {"F4", VK_F4},
    {"F5", VK_F5},
    {"F6", VK_F6},
    {"F7", VK_F7},
    {"F8", VK_F8},
    {"F9", VK_F9},
    {"F10", VK_F10},
    {"F11", VK_F11},
    {"F12", VK_F12},
    {"num0", VK_NUMPAD0},
    {"num1", VK_NUMPAD1},
    {"num2", VK_NUMPAD2},
    {"num3", VK_NUMPAD3},
    {"num4", VK_NUMPAD4},
    {"num5", VK_NUMPAD5},
    {"num6", VK_NUMPAD6},
    {"num7", VK_NUMPAD7},
    {"num8", VK_NUMPAD8},
    {"num9", VK_NUMPAD9},
};

int HotKeyStrigToInt(const string& hotKeyString)
{
    for (int i = 0; i < sizeof(HotKeyStringMap)/sizeof(HotKeyStringMap[0]); ++i)
    {
        if (HotKeyStringMap[i].keyString == hotKeyString)
        {
            return HotKeyStringMap[i].KeyValue;
        }
    }
    return -1;
}



void PlaySoundHappy(int beg, int end)
{
    unsigned FREQUENCY[] = {392,392,440,392,523,494,  
        392,392,440,392,587,523,  
        392,392,784,659,523,494,440,  
        689,689,523,587,523};  

    unsigned DELAY[] = {375,125,500,500,500,1000,  
        375,125,500,500,500,1000,  
        375,125,500,500,500,500,1000,  
        375,125,500,500,500,1000};  
    for (int i = beg; i < end; i++)  //先填10短些
    {  
        Beep(FREQUENCY[i], DELAY[i]);  
    }  
}
}

// HWND GetFullScreenHwnd()
// {   
//     HWND hwnd = ::GetForegroundWindow();
//     Sleep(5000);
// 
//     RECT rect;
//     GetWindowRect(hwnd, &rect);
//     SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0); // 工作区大小
//     bool vis = ::IsWindowVisible(hwnd);
//     
//     do 
//     {
//         if (rect.right - rect.left > 700 && rect.bottom - rect.top > 500 && vis)
//         {
//             return hwnd;
//         }
// 
//     } while ();
// 
//     return NULL;
// }

WinControlTool* WinControlTool::winControlTool = NULL;

WinControlTool::WinControlTool(void)
	: myVolumeCtrl_(new CMyVolumeCtrl)
	, forcegroundWindowNotScreenSaveHwnd_(NULL)
    , notScreenSaveCanTryCntLeave_(0)
	, easyWindow_(new EasyWindow())
{
}

WinControlTool* WinControlTool::GetInstance()
{
	if(winControlTool == NULL)
	{
		winControlTool = new WinControlTool;
	}
	return winControlTool;
}


CMyVolumeCtrl* WinControlTool::GetMyVolumeCtrl()
{
	return myVolumeCtrl_;
}


WinControlTool::~WinControlTool(void)
{
	if (myVolumeCtrl_)
	{
		delete myVolumeCtrl_;
		myVolumeCtrl_ = NULL;
	}	
	if (easyWindow_)
	{
		delete easyWindow_;
	}
}


string WinControlTool::toupperString(const string& strLower)
{
	string strRet;
	for ( string::const_iterator iter = strLower.begin(); iter != strLower.end(); ++iter )
	{
		strRet.push_back(toupper(*iter));
	}
	return strRet;
}

string WinControlTool::GetValueFromConfig(const string& strAppName,
	const string& strKeyName, const string& strDefault, const string& strFileName)
{
	char szPath[2048] = {0};
	//char szFileName[MAX_PATH] = "\\VolScrConfig.ini";
	GetModuleFileName(NULL, szPath, sizeof(szPath));//GetModuleFileName	
	char* pDe = strrchr(szPath, '\\');
	string strPath;
	if ( pDe )
	{
		*pDe = 0;
		strPath.assign(szPath);
	}
	string strFullPath = strPath + "\\" + strFileName;//szFileName;			
	FILE* fp = NULL;
	fopen_s(&fp, strFullPath.c_str(), "r");
	if ( fp==NULL )
	{				
		UINT iPos = strPath.rfind("\\");
		strPath.erase(iPos);
		strFullPath = strPath + "\\" + strFileName;//szFileName;		
	}
	else 
		fclose(fp);

	char szVolume[2048] = {0};
	if (WinDefine::GetInstance()->useJobConfig_)
	{
		const string defaultValue = "UseJobConfig@#$#%@#$^$%%$&^#^%!!";
		GetPrivateProfileString(strAppName.c_str(), (strKeyName + "_job").c_str(), 
			defaultValue.c_str(), szVolume, sizeof(szVolume)-1, strFullPath.c_str() );	
		if (defaultValue != szVolume)
		{
			return szVolume;
		}
	}

	GetPrivateProfileString(strAppName.c_str(), strKeyName.c_str(), 
		strDefault.c_str(), szVolume, sizeof(szVolume)-1, strFullPath.c_str() );	
	return string(szVolume);
}

void WinControlTool::ReplaceString(string& orc, const string& findWhat, const string& replaceTo)
{
	string::size_type posBeg = 0;
	string::size_type posEnd = -1;
	while( (posEnd = orc.find(findWhat, posBeg)) != string::npos)
	{
		orc.replace(posEnd, findWhat.length(), replaceTo);
		posBeg = posEnd + findWhat.length();
	}
}

void WinControlTool::SplitStringBySign(vector<string>& result, const string& stringIn, const string& sign)
{
	string::size_type posBeg = 0;
	string::size_type posEnd = -1;
	while( (posEnd = stringIn.find(sign, posBeg)) != -1)//""  a++b++c a++b++ ++b++C++ a++++b
	{
		string child(stringIn, posBeg, posEnd - posBeg);		
		if (!child.empty())
		{
			result.push_back(child);
		}
		posBeg = posEnd + sign.length();
	}

	string childOut(stringIn, posBeg, stringIn.length() - posBeg);		
	if (!childOut.empty())
	{
		result.push_back(childOut);
	}
}
void WinControlTool::TranslateStringToVKKey(const string& stringIn, UINT* vkCtrl, UINT* vkKey)
{
	vector<string> vklist;
	SplitStringBySign(vklist, stringIn, "+");
	for (vector<string>::iterator it = vklist.begin(); it != vklist.end(); ++it)
	{
        int key = HotKeyStrigToInt(*it);
        if (key != -1)
        {
            *vkKey = key;
        }
        else if (!_stricmp(it->c_str(), "ctrl")) 
		{
			*vkCtrl |= MOD_CONTROL;
		}
		else if (!_stricmp(it->c_str(), "shift"))
		{
			*vkCtrl |= MOD_SHIFT;
		}
		else if (!_stricmp(it->c_str(), "alt"))
		{
			*vkCtrl |= MOD_ALT;
		}
        else if (!_stricmp(it->c_str(), "PageUp"))
        {
            *vkKey = VK_PRIOR;
        }
        else if (!_stricmp(it->c_str(), "PageDown"))
        {
            *vkKey = VK_NEXT;
        }
        else if (!_stricmp(it->c_str(), "Up"))
        {
            *vkKey = VK_UP;
        }
        else if (!_stricmp(it->c_str(), "Down"))
        {
            *vkKey = VK_DOWN;
        }
        else if (!_stricmp(it->c_str(), "left"))
        {
            *vkKey = VK_LEFT;
        }
        else if (!_stricmp(it->c_str(), "right"))
        {
            *vkKey = VK_RIGHT;
        }
		else if(it->length() == 1) 
		{
			*vkKey = toupperString(*it)[0];			
		}
		else
		{
			assert(0 && "no support key!!!");
		}
	}
}

void WinControlTool::InitProgressHotKey(HWND hWnd)
{
	string strValue = GetValueFromConfig(CONFIG_SET_PROGRESS_HOTKEY, "HotKeyCount", "10", CONFIG_INF_FILENAME); 	
	BOOL bRet = FALSE;
	for (int i = 1; i <= atoi(strValue.c_str()); ++i)
	{
		string progressName = "Progress";
		char szbuf[10];
		_itoa_s(i, szbuf, 10);
		progressName += szbuf;
		string path = GetValueFromConfig(CONFIG_SET_PROGRESS_HOTKEY, progressName.c_str(), "", CONFIG_INF_FILENAME); 
		string hotKeyName = "ProgressHotKey" + string(szbuf);
		string hotkey = GetValueFromConfig(CONFIG_SET_PROGRESS_HOTKEY, hotKeyName.c_str(), "", CONFIG_INF_FILENAME); //ctrl+num3
		string killhotKeyName = "ProgressKillHotKey" + string(szbuf);
		string killhotkey = GetValueFromConfig(CONFIG_SET_PROGRESS_HOTKEY, killhotKeyName.c_str(), "", CONFIG_INF_FILENAME); //ctrl+num3
		
		ProgressToIDHotKey idHotKey;
		idHotKey.path = path;		
		
		{
			idHotKey.ID = HOTKEY_PROGRESS_BEGIN + i;
			UINT vkCtrl = 0;
			UINT vkKey = 0; 
			TranslateStringToVKKey(hotkey, &vkCtrl, &vkKey);	
			idHotKey.vkCtrl = vkCtrl;
			idHotKey.vkKey = vkKey;
			bRet = RegisterHotKey(hWnd, idHotKey.ID, idHotKey.vkCtrl, idHotKey.vkKey);
		}

		if (!killhotkey.empty())
		{
			idHotKey.killID = HOTKEY_PROGRESS_KILL_BEGIN + i;
			UINT vkCtrl = 0;
			UINT vkKey = 0; 
			TranslateStringToVKKey(killhotkey, &vkCtrl, &vkKey);	
			idHotKey.vkKillCtrl = vkCtrl;
			idHotKey.vkKillKey = vkKey;
			bRet = RegisterHotKey(hWnd, idHotKey.killID, idHotKey.vkKillCtrl, idHotKey.vkKillKey);
		}		
		progressToIDHotkeyList_.push_back(idHotKey);		
	}
}


void WinControlTool::InitHotKey(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    BOOL bRet = FALSE;
    {
        string hotkey = GetValueFromConfig(CONFIG_SET_HOTKEY, "HotKeyVolumeUp", "", CONFIG_INF_FILENAME); 
        UINT vkCtrl = 0;
        UINT vkKey = 0; 
        TranslateStringToVKKey(hotkey, &vkCtrl, &vkKey);        
        bRet = RegisterHotKey(hWnd, HOTKEY_VOLUME_UP, vkCtrl, vkKey);
    }

    {
        string hotkey = GetValueFromConfig(CONFIG_SET_HOTKEY, "HotKeyVolumeDown", "", CONFIG_INF_FILENAME); 
        UINT vkCtrl = 0;
        UINT vkKey = 0; 
        TranslateStringToVKKey(hotkey, &vkCtrl, &vkKey);
        bRet = RegisterHotKey(hWnd, HOTKEY_VOLUME_DOWN, vkCtrl, vkKey);
    }

    {
        string hotkey = GetValueFromConfig(CONFIG_SET_HOTKEY, "HotKeyCloseScreen", "", CONFIG_INF_FILENAME); 
        UINT vkCtrl = 0;
        UINT vkKey = 0; 
        TranslateStringToVKKey(hotkey, &vkCtrl, &vkKey);
        bRet = RegisterHotKey(hWnd, HOTKEY_CLOSE_SCREEN, vkCtrl, vkKey);
    }

    {
        string hotkey = GetValueFromConfig(CONFIG_SET_HOTKEY, "HotKeyKillProcess", "", CONFIG_INF_FILENAME); 
        UINT vkCtrl = 0;
        UINT vkKey = 0; 
        TranslateStringToVKKey(hotkey, &vkCtrl, &vkKey);
        bRet = RegisterHotKey(hWnd, HOTKEY_KILL_PROCESS, vkCtrl, vkKey);
    }

    {
        string hotkey = GetValueFromConfig(CONFIG_SET_HOTKEY, "HotKeyNotScreenSave", "", CONFIG_INF_FILENAME); 
        UINT vkCtrl = 0;
        UINT vkKey = 0; 
        TranslateStringToVKKey(hotkey, &vkCtrl, &vkKey);
        bRet = RegisterHotKey(hWnd, HOTKEY_NOT_SCREEN_SAVE, vkCtrl, vkKey);
    }

    {
        string hotkey = GetValueFromConfig(CONFIG_SET_HOTKEY, "HotKeyShutDown", "", CONFIG_INF_FILENAME); 
        UINT vkCtrl = 0;
        UINT vkKey = 0; 
        TranslateStringToVKKey(hotkey, &vkCtrl, &vkKey);
        bRet = RegisterHotKey(hWnd, HOTKEY_NOT_SHUT_DOWN, vkCtrl, vkKey);
    }
	//程序Hotkey
	InitProgressHotKey(hWnd);
}

void WinControlTool::InitPowerOnStartProgress(HWND hWnd)
{
	string strValue = GetValueFromConfig(CONFIG_POWER_ON_START_PROGRESS, "PowerOnStartProgressCount", "10", CONFIG_INF_FILENAME); 	
	string strValueTime = GetValueFromConfig(CONFIG_POWER_ON_START_PROGRESS, "PowerOnStartProgressTime", "5000", CONFIG_INF_FILENAME); 	
	WinDefine::GetInstance()->powerOnStartProgressTime_ = atoi(strValueTime.c_str());
	BOOL bRet = FALSE;
	for (int i = 1; i <= atoi(strValue.c_str()); ++i)
	{
		string progressName = "StartProgress";
		char szbuf[10];
		_itoa_s(i, szbuf, 10);
		progressName += szbuf;
		string path = GetValueFromConfig(CONFIG_POWER_ON_START_PROGRESS, progressName.c_str(), "", CONFIG_INF_FILENAME); 	
		if (!path.empty())
		{
			WinDefine::GetInstance()->powerOnStartProgress_.push_back(path);
		}		
	}
	SetTimer(hWnd, TIMER_POWER_ON_START_PROGRESS, WinDefine::GetInstance()->powerOnStartProgressTime_, PowerOnStartProgressTimeProc);
}


void WinControlTool::OnCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RaiseToken();

	InitPowerOnStartProgress(hWnd);

	InitHotKey(hWnd, message, wParam, lParam);

	string strValue = GetValueFromConfig(CONFIG_SET, "InitVolume", "30", CONFIG_INF_FILENAME); 	
	WinDefine* winDefine = WinDefine::GetInstance();
	winDefine->initVolumeConst_ = winDefine->initVolume_ = atoi(strValue.c_str());
	strValue = GetValueFromConfig(CONFIG_SET, "PerVolumeGap", "3", CONFIG_INF_FILENAME); 	
	winDefine->perVoulumeGap_ = atoi(strValue.c_str());
	strValue = GetValueFromConfig(CONFIG_SET, "InitTime", "5000", CONFIG_INF_FILENAME); 	
	winDefine->iInitTime_ = atoi(strValue.c_str());
	strValue = GetValueFromConfig(CONFIG_SET, "IsInitVolume", "1", CONFIG_INF_FILENAME); 	
	winDefine->iIsInitVolume_ = atoi(strValue.c_str());	
	SetTimer(hWnd, TIMER_INIT_VOLUME, winDefine->iInitTime_, TimerProc);	

    //不屏保
    strValue = GetValueFromConfig(CONFIG_SET, "notScreenSavePerInputTime", "3", CONFIG_INF_FILENAME);
    winDefine->notScreenSavePerInputTime_ = atoi(strValue.c_str());
    strValue = GetValueFromConfig(CONFIG_SET, "notScreenSaveCanTryCnt", "3", CONFIG_INF_FILENAME); 
    winDefine->notScreenSaveCanTryCnt_ = atoi(strValue.c_str());

// 	strValue = GetValueFromConfig(CONFIG_SET, "GetWebTimeSpace", "20000", CONFIG_INF_FILENAME); 
// 	if ( strValue != "-1" )
// 	{
// 		SetTimer(hWnd, TIMER_GET_WEB_TIME, atoi(strValue.c_str()), TimerProc);	
// 	}	
}

string WinControlTool::W2A(wstring strWide)
{
	int iSize = (strWide.size()+1)*2;
	char* temp = new char[iSize];
	WideCharToMultiByte(CP_ACP, 0, strWide.c_str(), -1, (LPSTR)temp, iSize, NULL, NULL);
	return temp; 
}

BOOL CALLBACK WinControlTool::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	DWORD dwThisID = 0;
	GetWindowThreadProcessId(hwnd, &dwThisID);
	if (dwThisID == (DWORD)lParam)
	{
		PostMessage(hwnd, WM_CLOSE, 0, 0);
	}	
	return TRUE;
}	

void WinControlTool::KillProgressByNames(const vector<string>& nameList, bool tryExistFirst)
{
	vector<string> vecUpName;
	std::transform(nameList.begin(), nameList.end(), std::back_inserter(vecUpName), toupperString);

	PROCESSENTRY32 pe = {0};
	pe.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	BOOL bRet = Process32First(hSnapshot, &pe);
	TRACE_ZZ("%d\n", bRet);
	DWORD dwErr = GetLastError();
	TRACE_ZZ("%d\n", dwErr);
	do 
	{
		TRACE_WW(pe.szExeFile);
		TRACE_WW("\n");		
		_strupr_s(pe.szExeFile);		
		TRACE_WW(pe.szExeFile);
		TRACE_WW("\nGo TO Kill\n");
		for ( vector<string>::const_iterator it = vecUpName.begin(); it < vecUpName.end(); ++it )
		{
			if ( *it == pe.szExeFile )
			{				
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
				TRACE_WW(pe.szExeFile);
				TRACE_WW("\t\t");
#ifdef TRACE_WW
				char szProcess[50] = {0};
				sprintf_s(szProcess, _countof(szProcess), "%p", hProcess);
				TRACE_WW(szProcess);
				TRACE_WW("\n");
#endif
				if(tryExistFirst)
				{
					EnumWindows(EnumWindowsProc, pe.th32ProcessID);
					Sleep(3000);
				}				
				TerminateProcess(hProcess, 0);
				CloseHandle(hProcess);
			}
		}	
	} while (Process32Next(hSnapshot, &pe));
	CloseHandle(hSnapshot);
}



void WinControlTool::TerminateNameExe(string& strNameExe)
{
	if (strNameExe.empty())
	{
		return;
	}	
	ReplaceString(strNameExe, "\r\n", "\n");
	ReplaceString(strNameExe, "\r", "\n");
	vector<string> vecName;
	SplitStringBySign(vecName, strNameExe, "\n");
	KillProgressByNames(vecName, false);
}


VOID CALLBACK WinControlTool::PowerOnStartProgressTimeProc( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime )
{
	KillTimer(hwnd, idEvent);
	for (vector<string>::iterator it = WinDefine::GetInstance()->powerOnStartProgress_.begin();
		it !=  WinDefine::GetInstance()->powerOnStartProgress_.end(); ++it)
	{
		SHELLEXECUTEINFOA stExecInfo = {0};
		stExecInfo.cbSize = sizeof(stExecInfo);
		stExecInfo.fMask = SEE_MASK_FLAG_NO_UI ;
		stExecInfo.lpFile = it->c_str();
		stExecInfo.nShow = SW_HIDE;
		BOOL bRet = ShellExecuteExA(&stExecInfo);
	}
}

void WinControlTool::StopNotScreenSave(HWND hwnd, bool playSound)
{
	easyWindow_->Close();
    KillTimer(hwnd, TIMER_NOT_SCREEN_SAVE); 
    KillTimer(hwnd, TIMER_NOT_SCREEN_SAVE_MAX); 
    forcegroundWindowNotScreenSaveHwnd_ = nullptr;
    if (playSound)
    {
        PlaySoundHappy(0, 2);
    }
}

VOID CALLBACK WinControlTool::TimerProc( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime )
{
    WinControlTool* thisClass =  WinControlTool::GetInstance(); 
	if ( TIMER_CLOSE_SCREEN == idEvent || TIMER_CLOSE_SCREEN_ADD == idEvent )
	{
		KillTimer(hwnd, idEvent);
		::PostMessage(hwnd, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)1);		
		::PostMessage(hwnd, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2);			
	}
	else if ( TIMER_INIT_VOLUME==idEvent )
	{	
		WinDefine* winDefine = WinDefine::GetInstance();
		if ( FALSE == winDefine->bFinishInitVolume_ && winDefine->iIsInitVolume_ == 1)
		{	
			GetInstance()->GetMyVolumeCtrl()->SetVolume(winDefine->initVolume_);
			winDefine->initVolume_ = 0;
		}
		winDefine->bFinishInitVolume_ = TRUE;
		KillTimer(hwnd, idEvent);
	}
	else if (TIMER_GET_WEB_TIME == idEvent)
	{
		SHELLEXECUTEINFO stExecInfo = {0};
		stExecInfo.cbSize = sizeof(stExecInfo);
		stExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI ;
		stExecInfo.lpFile = "GetWebAndSetTime.exe";
		stExecInfo.nShow = SW_HIDE;
		BOOL bRet = ShellExecuteEx(&stExecInfo);
		DWORD dwErr = GetLastError();
		WaitForSingleObject(stExecInfo.hProcess, INFINITE);
		DWORD dwCode = -2;
		bRet = GetExitCodeProcess(stExecInfo.hProcess, &dwCode);
		if ( dwCode == 0 || ++WinDefine::GetInstance()->iGetWebTimeCnt_>5)
		{
			KillTimer(hwnd, idEvent);
		}
	}
	else if (TIMER_NOT_SCREEN_SAVE == idEvent)
	{
        TRACE_WW("TIMER_NOT_SCREEN_SAVE == idEvent\n");        
        bool notScreenContinue = false;
        if( thisClass->forcegroundWindowNotScreenSaveHwnd_ && 
            IsWindow(thisClass->forcegroundWindowNotScreenSaveHwnd_))
        {
            HWND fore = GetForegroundWindow();
            if (fore == thisClass->forcegroundWindowNotScreenSaveHwnd_)
            {
                TRACE_WW("fore == thisClass->forcegroundWindowNotScreenSaveHwnd\n");
                INPUT input = {0};
                input.type = INPUT_KEYBOARD; 
                input.ki.wVk = VK_F24; 			
                SendInput(1, &input, sizeof(INPUT));               
                thisClass->notScreenSaveCanTryCntLeave_ = WinDefine::GetInstance()->notScreenSaveCanTryCnt_;
                notScreenContinue = true;
            }
        }

        if (!notScreenContinue)
        {
            TRACE_WW(" if (!notScreenContinue)");
            if(--thisClass->notScreenSaveCanTryCntLeave_ <= 0)
            {
                thisClass->StopNotScreenSave(hwnd, true);
            }
        }
	}
    else if (TIMER_NOT_SCREEN_SAVE_MAX == idEvent)
    {
        thisClass->StopNotScreenSave(hwnd, false);
    }
}

void WinControlTool::RaiseToken()
{
	HANDLE   hToken;   
	TOKEN_PRIVILEGES   tkp;  
	BOOL bRet(FALSE);
	static bool bPrivi(false);
	if ( !bPrivi )
	{
		//定义变量   
		bRet = OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken);   
		char szRet[10] = {0};
		sprintf_s(szRet, _countof(szRet), "%d", bRet);
		TRACE_WW(szRet);
		TRACE_WW("\n");
		//OpenProcessToken（）这个函数的作用是打开一个进程的访问令牌   
		//GetCurrentProcess（）函数的作用是得到本进程的句柄   
		bRet = LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&tkp.Privileges[0].Luid);  
		sprintf_s(szRet, _countof(szRet), "%d", bRet);
		TRACE_WW(szRet);
		TRACE_WW("\n");
		//LookupPrivilegeValue（）的作用是修改进程的权限   
		tkp.PrivilegeCount =  1;   
		//赋给本进程特权   
		tkp.Privileges[0].Attributes   =   SE_PRIVILEGE_ENABLED;   
		bRet = AdjustTokenPrivileges(hToken,FALSE,&tkp,0,(PTOKEN_PRIVILEGES)NULL,0);   
		//AdjustTokenPrivileges（）的作用是通知Windows   NT修改本进程的权利  
		sprintf_s(szRet, _countof(szRet), "%d", bRet);
		TRACE_WW(szRet);
		TRACE_WW("\n");
		bPrivi = true;
	}
}

string WinControlTool::GetKillNameBuff()
{	
	char szPath[2048] = {0};
	GetModuleFileName(NULL, szPath, sizeof(szPath));
	string strPath(szPath);
	int iPos = strPath.rfind("\\");
	strPath.erase(iPos);

	string strAnotherPath(strPath);

	strPath += string("\\") + CONFIG_INF_FILENAME;
	FILE* fp = NULL;
	fopen_s(&fp, strPath.c_str(), "r");
	if ( fp == NULL )
	{
		iPos = strAnotherPath.rfind("\\");
		strAnotherPath.erase(iPos);
		strAnotherPath += string("\\") + CONFIG_INF_FILENAME;
		fopen_s(&fp, strAnotherPath.c_str(), "r");
		if(fp == NULL )
		{
			return "";
		}						
	}
	char szBuffer[2048] = {0};
	fread(szBuffer, 2047, 1, fp);
	fclose(fp);

	char* beg = strstr(szBuffer, CONFIG_SET_KILLNAME_BEGIN);
	char* end = strstr(szBuffer, CONFIG_SET_KILLNAME_END);
	if (beg && end)
	{
		beg += strlen(CONFIG_SET_KILLNAME_BEGIN);
		while ('\r' == *beg || '\n' == *beg)
		{
			beg++;
		}
		return string(beg, end);
	}
	return "";
}


void WinControlTool::OnKillProcess(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{	
	string killName = GetKillNameBuff();
	TerminateNameExe(killName);
	TerminateNameExe(killName);	
}

bool WinControlTool::ForcegroundWindowFullScreen(HWND forcegroundWindow)
{	
	if (forcegroundWindow)
	{
		RECT rect = {0};
		GetWindowRect(forcegroundWindow, &rect);
		int width = GetSystemMetrics(SM_CXSCREEN);
		int height = GetSystemMetrics(SM_CYSCREEN);
		if ((rect.right - rect.left >= width) && (rect.bottom - rect.top >= height))
		{
			return true;
		}
	}
	return false;
}

void WinControlTool::OnHotKeyNotScreenSave(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   // GetFullScreenHwnd();

    if (forcegroundWindowNotScreenSaveHwnd_) // 多按一次就关闭 （屏幕不屏保）
    {
        StopNotScreenSave(hWnd, true);
        return;
    }
	
    HWND foregroundWindow = GetForegroundWindow();
    if (foregroundWindow)
    {
        forcegroundWindowNotScreenSaveHwnd_ = foregroundWindow;
        SetTimer(hWnd, TIMER_NOT_SCREEN_SAVE, WinDefine::GetInstance()->notScreenSavePerInputTime_*1000, TimerProc);
        string strValue = GetValueFromConfig(CONFIG_SET, "notScreenSavePerInputMAXTime", "120", CONFIG_INF_FILENAME);
        SetTimer(hWnd, TIMER_NOT_SCREEN_SAVE_MAX, atoi(strValue.c_str())*60*1000, TimerProc);
        notScreenSaveCanTryCntLeave_ = WinDefine::GetInstance()->notScreenSaveCanTryCnt_;	
		strValue = GetValueFromConfig(CONFIG_SET, "notScreeenSaveMsgBox", "0", CONFIG_INF_FILENAME);
		if (strValue != "0")
		{
			easyWindow_->Create(nullptr, 600, 0, 200, 50);
		}
		PlaySoundHappy(0, 6);
    }
}

void WinControlTool::TipsSound()
{	
	Sleep(1000);
	::MessageBeep(0xFFFFFFFF);//API函数前加“::”符号，表示这是一个全局的函数，以与c++类的成员函数相区分
	Sleep(1000);
	::MessageBeep(MB_ICONASTERISK);
	Sleep(1000);
	::MessageBeep(MB_ICONEXCLAMATION);
	Sleep(1000);
	::MessageBeep(MB_ICONHAND);
	Sleep(1000);
	::MessageBeep(MB_ICONQUESTION);
	Sleep(1000);
	::MessageBeep(MB_OK);
	Sleep(1000);
}

void WinControlTool::OnHotKey(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int iVolume;
	char szVolume[256]={0};
	int iIDHotKey = (int)wParam;

	WinDefine* winDefine = WinDefine::GetInstance();

	int iTime(0);
	BOOL bRet(FALSE);
	switch(iIDHotKey)
	{
	case HOTKEY_VOLUME_UP:
		{
			iVolume = GetMyVolumeCtrl()->GetVolume();	//得到的值偏小1						
			sprintf_s(szVolume,sizeof(szVolume), "iVolume = %d\n", iVolume);
			iVolume += winDefine->perVoulumeGap_;
			GetMyVolumeCtrl()->SetVolume(iVolume);
			break;
		}
	case HOTKEY_VOLUME_DOWN:
		{			
			if ( FALSE == winDefine->bFinishInitVolume_ )
			{
				GetMyVolumeCtrl()->SetVolume(winDefine->initVolume_*1/3);
				winDefine->initVolume_ = 0;
				winDefine->bFinishInitVolume_ = TRUE;
			}			
			iVolume = GetMyVolumeCtrl()->GetVolume();	//得到的值偏小1						
			sprintf_s(szVolume,sizeof(szVolume), "iVolume = %d\n", iVolume);
			iVolume -= winDefine->perVoulumeGap_;
			if ( iVolume < 0 )
			{
				iVolume = 0;
			}
			GetMyVolumeCtrl()->SetVolume(iVolume);
			break;
		}
	case HOTKEY_CLOSE_SCREEN:
		{					
			SetTimer(hWnd, TIMER_CLOSE_SCREEN, 1500, TimerProc);
			SetTimer(hWnd, TIMER_CLOSE_SCREEN_ADD, 4000, TimerProc);
            StopNotScreenSave(hWnd, false);
			break;
		}
	case HOTKEY_KILL_PROCESS:
		{
			OnKillProcess(hWnd, message, wParam, lParam);	
			break;
		}
	case HOTKEY_NOT_SCREEN_SAVE:
		{
			OnHotKeyNotScreenSave(hWnd, message, wParam, lParam);
			break;
		}		
		case HOTKEY_NOT_SHUT_DOWN:
		{
            ShutDownDlg dlg;
            dlg.DoModal(NULL);
			break;
		}
    }

	if (iIDHotKey >= HOTKEY_PROGRESS_BEGIN && iIDHotKey <= HOTKEY_PROGRESS_END)
	{
		for (vector<ProgressToIDHotKey>::iterator it = progressToIDHotkeyList_.begin();
			it != progressToIDHotkeyList_.end(); ++it)
		{
			if (it->ID == iIDHotKey)
			{
				SHELLEXECUTEINFOA stInfo = {0};
				stInfo.cbSize = sizeof(stInfo);			
				stInfo.lpFile = it->path.c_str();
				stInfo.nShow = SW_SHOW;
				ShellExecuteExA(&stInfo);
			}
		}
	}
	else if (iIDHotKey >= HOTKEY_PROGRESS_KILL_BEGIN && iIDHotKey <= HOTKEY_PROGRESS_KILL_END)
	{
		for (vector<ProgressToIDHotKey>::iterator it = progressToIDHotkeyList_.begin();
			it != progressToIDHotkeyList_.end(); ++it)
		{
			if (it->killID == iIDHotKey)
			{
				string::size_type index = 0;
				if ( ((index = it->path.rfind("\\")) != -1) || 
					((index = it->path.rfind("/")) != -1) )
				{
					string killName(it->path, ++index, it->path.size() - index);
					KillProgressByNames(vector<string>(1, killName), true);
				}	
			}
		}
	}
}