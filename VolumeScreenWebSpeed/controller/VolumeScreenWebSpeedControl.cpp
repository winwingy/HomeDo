#include "stdafx.h"
#include "VolumeScreenWebSpeedControl.h"
#include <algorithm>
#include <TlHelp32.h>
#include <shellapi.h>

#include "WinDefine.h"
#include "../view/ShutDownDlg.h"
#include "../tool/config.h"
#include "../tool/StringPathHelper.h"
#include "VolumeCtrlWrapper.h"
#include "ScreenSaveControllor.h"
#include "TaskDo.h"
#include "KugouTaskDo.h"
#include "TaskDoMgr.h"
#include "../view/TimingTaskDlg.h"
#include "view/ToastWindow.h"
#include "Resource.h"
#include "tool/windowTool.h"
#include "view/ChangeTitleDlg.h"
#pragma comment(lib, "Kernel32.lib")
using namespace std;

namespace
{
    void TranslateStringToVKKey(const string& stringIn, UINT* vkCtrl, UINT* vkKey)
    {
        vector<string> vklist;
        StringPathHelper::SplitStringBySign(stringIn, "+", &vklist);
        for (vector<string>::iterator it = vklist.begin(); it != vklist.end(); ++it)
        {
            int key = WinDefine::HotKeyStrigToVkKey(*it);
            if (key != -1)
            {
                *vkKey = key;
            }
            else if (it->length() == 1)
            {
                //VK_0 thru VK_9 are the same as ASCII '0' thru '9' (0x30 - 0x39)
                //VK_A thru VK_Z are the same as ASCII 'A' thru 'Z' (0x41 - 0x5A)
                *vkKey = StringPathHelper::ToUpperString(*it)[0];
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
            else
            {
                assert(0 && "no support key!!!");
            }
        }
    }

    bool RgeisterStringHotKey(const string& hotKeyString, HWND hWnd, int hotKeyId)
    {
        UINT vkCtrl = 0;
        UINT vkKey = 0;
        TranslateStringToVKKey(hotKeyString, &vkCtrl, &vkKey);
        return !!::RegisterHotKey(hWnd, hotKeyId, vkCtrl, vkKey);
    }

    void ShellExcuteProgress(const string& path)
    {
        if (path.empty())
            return;
        
        SHELLEXECUTEINFOA stExecInfo = { 0 };
        stExecInfo.cbSize = sizeof(stExecInfo);
        stExecInfo.fMask = SEE_MASK_FLAG_NO_UI;
        stExecInfo.lpVerb = "open";
        stExecInfo.lpFile = path.c_str();
        stExecInfo.nShow = SW_SHOW;
        BOOL bRet = ShellExecuteExA(&stExecInfo);
    }
}

VolumeScreenWebSpeedControl::VolumeScreenWebSpeedControl(void)
	: hWnd_(nullptr)
	, config_(Config::GetShared())
    , volumeCtrlWrapper_(new VolumeCtrlWrapper())
    , powerOnStartProgress_()
    , screenSaveControllor_(new ScreenSaveControllor())
	, timingTaskDlgList_()
	, shutDownDlg_(nullptr)
{

}

VolumeScreenWebSpeedControl::~VolumeScreenWebSpeedControl(void)
{
	
}

void VolumeScreenWebSpeedControl::InitProgressHotKey(HWND hWnd)
{
    int hotKeyCount = config_->GetValue(CONFIG_SET_PROGRESS_HOTKEY,
                                        "HotKeyCount", 10);
    BOOL bRet = FALSE;
	
    for (int i = 1; i <= hotKeyCount; ++i)
    {
        string progressName = "Progress" + StringPathHelper::IntToString(i);
        string path = config_->GetValue(CONFIG_SET_PROGRESS_HOTKEY, 
                                        progressName.c_str(), "");
        string hotKeyName = "ProgressHotKey" + StringPathHelper::IntToString(i);
        string hotkey = config_->GetValue(CONFIG_SET_PROGRESS_HOTKEY,
                                          hotKeyName.c_str(), ""); //ctrl+num3
        string killhotKeyName = "ProgressKillHotKey" + 
            StringPathHelper::IntToString(i);
        string killhotkey = config_->GetValue(CONFIG_SET_PROGRESS_HOTKEY,
                                              killhotKeyName.c_str(), ""); //ctrl+num3

        ProgressToIDHotKey idHotKey;
        idHotKey.path =StringPathHelper::ToUpperString(path);
        if (!hotkey.empty())
        {
            idHotKey.ID = WinDefine::HOTKEY_PROGRESS_BEGIN + i;
            TranslateStringToVKKey(hotkey, &idHotKey.vkCtrl, &idHotKey.vkKey);
            bRet = RegisterHotKey(hWnd, idHotKey.ID, idHotKey.vkCtrl, 
                                  idHotKey.vkKey);
        }

        if (!killhotkey.empty())
        {
            idHotKey.killID = WinDefine::HOTKEY_PROGRESS_KILL_BEGIN + i;
            TranslateStringToVKKey(killhotkey, &idHotKey.vkKillCtrl,
                                   &idHotKey.vkKillKey);
            bRet = RegisterHotKey(hWnd, idHotKey.killID, idHotKey.vkKillCtrl,
                                  idHotKey.vkKillKey);
        }
        if (!idHotKey.path.empty())
            progressToIDHotkeyList_.push_back(idHotKey);
    }
}

void VolumeScreenWebSpeedControl::InitGeneralHotKey(HWND hWnd)
{
    BOOL bRet = FALSE;
    string hotkey = config_->GetValue(CONFIG_SET_HOTKEY, "HotKeyOpenConfig", "");
    bRet = RgeisterStringHotKey(hotkey, hWnd, WinDefine::HOTKEY_OPEN_CONFIG);
    
    hotkey = config_->GetValue(CONFIG_SET_HOTKEY, "HotKeyVolumeUp", "");
    bRet = RgeisterStringHotKey(hotkey, hWnd, WinDefine::HOTKEY_VOLUME_UP);

    hotkey = config_->GetValue(CONFIG_SET_HOTKEY, "HotKeyVolumeDown", "");
    bRet = RgeisterStringHotKey(hotkey, hWnd, WinDefine::HOTKEY_VOLUME_DOWN);

    hotkey = config_->GetValue(CONFIG_SET_HOTKEY, "HotKeyCloseScreen", "");
    bRet = RgeisterStringHotKey(hotkey, hWnd, WinDefine::HOTKEY_CLOSE_SCREEN);

    hotkey = config_->GetValue(CONFIG_SET_HOTKEY, "HotKeyNotScreenSave", "");
    bRet = RgeisterStringHotKey(hotkey, hWnd, WinDefine::HOTKEY_NOT_SCREEN_SAVE);

    hotkey = config_->GetValue(CONFIG_SET_HOTKEY, "HotKeyKillProcess", "");
    bRet = RgeisterStringHotKey(hotkey, hWnd, WinDefine::HOTKEY_KILL_PROCESS);

    hotkey = config_->GetValue(CONFIG_SET_HOTKEY, "HotKeyShutDown", "");
    bRet = RgeisterStringHotKey(hotkey, hWnd, WinDefine::HOTKEY_SHUT_DOWN);

	hotkey = config_->GetValue(CONFIG_SET_HOTKEY, "HotKeyTaskList", "");
	bRet = RgeisterStringHotKey(hotkey, hWnd, WinDefine::HOTKEY_TASK_LIST);

	hotkey = config_->GetValue(CONFIG_SET_HOTKEY, "HotKeyTaskNew", "");
	bRet = RgeisterStringHotKey(hotkey, hWnd, WinDefine::HOTKEY_TASK_NEW);

    hotkey = config_->GetValue(CONFIG_SET_HOTKEY, "HotKeyMouseSpeedWeb", "");
    bRet = RgeisterStringHotKey(hotkey, hWnd, WinDefine::HOTKEY_MOUSESPEED_WEB);

    hotkey = config_->GetValue(CONFIG_SET_HOTKEY, "HotKeyMouseSpeedGame", "");
    bRet = RgeisterStringHotKey(hotkey, hWnd, WinDefine::HOTKEY_MOUSESPEED_GAME);

}

void VolumeScreenWebSpeedControl::InitHotKey(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    InitGeneralHotKey(hWnd);
    //程序Hotkey
    InitProgressHotKey(hWnd);
}

void VolumeScreenWebSpeedControl::InitPowerOnStartProgress(HWND hWnd)
{
    int progressTime = config_->GetValue(
        CONFIG_POWER_ON_START_PROGRESS, "PowerOnStartProgressDelayTime", 5000);

    int progressCount = config_->GetValue(
        CONFIG_POWER_ON_START_PROGRESS, "PowerOnStartProgressCount", 10);
    for (int i = 1; i <= progressCount; ++i)
    {
        stringstream ss;
        ss << "StartProgress" <<i;
        string path = config_->GetValue(
            CONFIG_POWER_ON_START_PROGRESS, ss.str().c_str(), "");
        if (!path.empty())
            powerOnStartProgress_.push_back(path);
    }
    SetTimer(hWnd, WinDefine::TIMER_POWER_ON_START_PROGRESS, progressTime, 
             nullptr);
}

void VolumeScreenWebSpeedControl::InitProgressSetting(HWND hWnd)
{
    volumeCtrlWrapper_->InitVolumeHotKey(hWnd);
    screenSaveControllor_->InitControllor(hWnd);
}

void VolumeScreenWebSpeedControl::InitTaskMgr(HWND hWnd)
{
	std::shared_ptr<KugouTaskDo> kugouTaskDo(new KugouTaskDo);
	kugouTaskDo->initTask();
	TaskDoMgr::instance()->addTaskDo(kugouTaskDo);

	// ... other task
}

void VolumeScreenWebSpeedControl::OnCreate(HWND hWnd, UINT uMsg,
                              WPARAM wParam, LPARAM lParam)
{
	hWnd_ = hWnd;

    RaiseToken();

	InitTaskBar(hWnd);

    InitPowerOnStartProgress(hWnd);

    InitHotKey(hWnd, uMsg, wParam, lParam);

    InitProgressSetting(hWnd);

	InitTaskMgr(hWnd);
}

BOOL CALLBACK EnumWindowsProc(
    HWND hwnd, LPARAM lParam)
{
    DWORD dwThisID = 0;
    GetWindowThreadProcessId(hwnd, &dwThisID);
    if (dwThisID == (DWORD)lParam)
    {
        PostMessage(hwnd, WM_CLOSE, 0, 0);
    }
    return TRUE;
}

void EnumKillProcess(const vector<string>& killName, bool tryExistFirst)
{
    PROCESSENTRY32 pe = { 0 };
    pe.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hSnapshot = INVALID_HANDLE_VALUE;
    do 
    {
        hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE)
            break;
        
        if (!Process32First(hSnapshot, &pe))
            break;

        do
        {
            string exeFile = StringPathHelper::ToUpperString(pe.szExeFile);
            for (vector<string>::const_iterator it = killName.begin();
                 it < killName.end(); ++it)
            {
                if (*it == exeFile)
                {
                    if (tryExistFirst)
                        EnumWindows(EnumWindowsProc, pe.th32ProcessID);

                    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE,
                                                  pe.th32ProcessID);
                    TerminateProcess(hProcess, 0);
                    CloseHandle(hProcess);
                }
            }
        } while (Process32Next(hSnapshot, &pe));
        
    } while (0);
    if (hSnapshot != INVALID_HANDLE_VALUE)
        CloseHandle(hSnapshot);

}

void VolumeScreenWebSpeedControl::KillProgressByNames(
    const vector<string>& nameList, bool tryExistFirst)
{
    if (nameList.empty())
        return;
    
    vector<string> vecUpName;
    std::transform(nameList.begin(), nameList.end(),
                   std::back_inserter(vecUpName),
                   StringPathHelper::ToUpperString);

    EnumKillProcess(vecUpName, tryExistFirst);
}

void  VolumeScreenWebSpeedControl::OnPowerOnStartProgressTimer(
    HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    if (!powerOnStartProgress_.empty())
    {
        ShellExcuteProgress(powerOnStartProgress_[0]);
        powerOnStartProgress_.erase(powerOnStartProgress_.begin());
    }

    int progressTime = config_->GetValue(
        CONFIG_POWER_ON_START_PROGRESS, "PowerOnStartProgressPerTime", 5000);
    SetTimer(hwnd, WinDefine::TIMER_POWER_ON_START_PROGRESS, 
             progressTime, nullptr);
    if (powerOnStartProgress_.empty())
        KillTimer(hwnd, idEvent);
}

void VolumeScreenWebSpeedControl::RaiseToken()
{
    HANDLE   hToken;
    TOKEN_PRIVILEGES   tkp;
    BOOL bRet(FALSE);
    static bool bPrivi(false);
    if (!bPrivi)
    {
        //定义变量   
        bRet = OpenProcessToken(GetCurrentProcess(),
                                TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
        char szRet[10] = { 0 };
        sprintf_s(szRet, _countof(szRet), "%d", bRet);
        TRACEWW(szRet);
        TRACEWW("\n");
        //OpenProcessToken（）这个函数的作用是打开一个进程的访问令牌   
        //GetCurrentProcess（）函数的作用是得到本进程的句柄   
        bRet = LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
        sprintf_s(szRet, _countof(szRet), "%d", bRet);
        TRACEWW(szRet);
        TRACEWW("\n");
        //LookupPrivilegeValue（）的作用是修改进程的权限   
        tkp.PrivilegeCount = 1;
        //赋给本进程特权   
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        bRet = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
                                     (PTOKEN_PRIVILEGES)NULL, 0);
        //AdjustTokenPrivileges（）的作用是通知Windows   NT修改本进程的权利  
        sprintf_s(szRet, _countof(szRet), "%d", bRet);
        TRACEWW(szRet);
        TRACEWW("\n");
        bPrivi = true;
    }
}

void VolumeScreenWebSpeedControl::InitTaskBar(HWND hWnd)
{
	int IsShowTray = config_->GetValue(CONFIG_SET, "IsShowTray", 0);
	if (!IsShowTray)
		return;
	
	WM_TASKBARCREATED = RegisterWindowMessage(_T("TaskbarCreated"));	
	nid_.cbSize = sizeof(nid_);
	nid_.hWnd = hWnd;
	nid_.uID = 0;
	nid_.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid_.uCallbackMessage = WM_TaskBarMsg;
	nid_.hIcon = (HICON)LoadIcon(GetModuleHandle(NULL),
		MAKEINTRESOURCE(IDI_ICON_SHOW));
	_tcscpy_s(nid_.szTip, APP_NAME);
	Shell_NotifyIcon(NIM_ADD, &nid_);
}

void VolumeScreenWebSpeedControl::OnKillProcess(HWND hWnd)
{
    vector<string> killNameList;
    config_->GetList(CONFIG_SET_KILLNAME_BEGIN, CONFIG_SET_KILLNAME_END,
                     &killNameList);
    if (killNameList.empty())
        return;

    KillProgressByNames(killNameList, false);
    KillProgressByNames(killNameList, false);
}

void VolumeScreenWebSpeedControl::OpenConfig()
{
    string configPath = config_->GetConfigPath();
    ShellExcuteProgress(configPath);
}

void VolumeScreenWebSpeedControl::SetMouseSpeed(bool isGameSpeed)
{
    int speed = 10;
    if (isGameSpeed)
    {
        speed = config_->GetValue(CONFIG_SET, "MouseSpeedGame", 10);
    }
    else
    {
        speed =config_->GetValue(CONFIG_SET, "MouseSpeedWeb", 8);
    }
    
    SystemParametersInfo(SPI_SETMOUSESPEED, 0,
                         reinterpret_cast<PVOID>(speed),
                         SPIF_UPDATEINIFILE);
}

void VolumeScreenWebSpeedControl::OnHotKey(HWND hWnd, UINT uMsg,
                                           int idHotKey, LPARAM lParam)
{
    volumeCtrlWrapper_->OnHotKey(hWnd, uMsg, idHotKey, lParam);
    screenSaveControllor_->OnHotKey(hWnd, uMsg, idHotKey, lParam);

    int iTime(0);
    BOOL bRet(FALSE);
    switch (idHotKey)
    {
        case WinDefine::HOTKEY_OPEN_CONFIG:
        {
            OpenConfig();
            break;
        }
        case WinDefine::HOTKEY_KILL_PROCESS:
        {
            OnKillProcess(hWnd);
            break;
        }
        case  WinDefine::HOTKEY_SHUT_DOWN:
        {
			ShowShutDownDlg();	
            break;
        }
		case  WinDefine::HOTKEY_TASK_LIST:
		{
			OnTaskList();
			break;
		}
		case  WinDefine::HOTKEY_TASK_NEW:
		{
			OnTaskNew();
			break;
		}
        case  WinDefine::HOTKEY_MOUSESPEED_WEB:
        {
            SetMouseSpeed(false);
            break;
        }
        case  WinDefine::HOTKEY_MOUSESPEED_GAME:
        {
            SetMouseSpeed(true);
            break;
        }
    }

    if (idHotKey >= WinDefine::HOTKEY_PROGRESS_BEGIN && 
        idHotKey <= WinDefine::HOTKEY_PROGRESS_END)
    {
        for (vector<ProgressToIDHotKey>::iterator it =
             progressToIDHotkeyList_.begin();
             it != progressToIDHotkeyList_.end(); ++it)
        {
            if (it->ID == idHotKey)
            {
				TaskDoMgr::instance()->beginLaunchTask(it->path);
                ShellExcuteProgress(it->path);		
				TaskDoMgr::instance()->afterLaunchTask(it->path);
                break;
            }
        }
    }
    else if (idHotKey >= WinDefine::HOTKEY_PROGRESS_KILL_BEGIN &&
             idHotKey <= WinDefine::HOTKEY_PROGRESS_KILL_END)
    {
        for (vector<ProgressToIDHotKey>::iterator it =
             progressToIDHotkeyList_.begin();
             it != progressToIDHotkeyList_.end(); ++it)
        {
            if (it->killID == idHotKey)
            {
                string killName(StringPathHelper::GetPathLastPart(it->path));
				if (!killName.empty())
				{
					TaskDoMgr::instance()->beginExitTask(it->path);
					KillProgressByNames(vector<string>(1, killName), true);
					TaskDoMgr::instance()->afterExitTask(it->path);
				}
                break;
            }
        }
    }
}

void VolumeScreenWebSpeedControl::OnTimer(
    HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    switch (idEvent)
    {
        case WinDefine::TIMER_POWER_ON_START_PROGRESS:
        {
            OnPowerOnStartProgressTimer(hWnd, uMsg, idEvent, dwTime);
            break;
        }
        default:
        break;
    }
    
    volumeCtrlWrapper_->OnTimer(hWnd, uMsg, idEvent, dwTime);
    screenSaveControllor_->OnTimer(hWnd, uMsg, idEvent, dwTime);
}

void VolumeScreenWebSpeedControl::OnTaskList()
{
	RemoveInvalidTimingTask();

	for (auto per : timingTaskDlgList_)
	{
		per->setVisible(true);
	}

	if (timingTaskDlgList_.empty())
	{
		ToastWindow* toastWindow = new ToastWindow();
		toastWindow->setDelOnClose(true);
		RECT rect;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
		toastWindow->Create(nullptr, rect.right - 200,
			rect.bottom - 30, 200, 30);
		toastWindow->Show(1000, _T("没有在运行的任务"));
		OnTaskNew();
	}
}

void VolumeScreenWebSpeedControl::OnTaskNew()
{
	RemoveInvalidTimingTask();
	TimingTaskDlg* pDlg = new TimingTaskDlg;
	pDlg->CreateDlgE(NULL);
	pDlg->setVisible(true);
	pDlg->activeWindow();
	timingTaskDlgList_.push_back(pDlg);
}


BOOL CALLBACK EnumWindowsProcCloseWindow(HWND hwnd, LPARAM lParam)
{
	tstring text = WindowTool::GetWindowText(hwnd);
	vector<string> nameList = *((vector<string>*)lParam);
	for (UINT i = 0; i < nameList.size(); ++i)
	{
		if (text == nameList[i])
		{
			PostMessage(hwnd, WM_CLOSE, 0, 0);
		}
	}
	return TRUE;
}

void VolumeScreenWebSpeedControl::OnCloseNameWindow()
{
	vector<string> nameList;
	config_->GetList(CONFIG_SET_NAMEWINDOW_BEGIN, CONFIG_SET_NAMEWINDOW_END,
		&nameList);
	if (nameList.empty())
		return;

	EnumWindows(EnumWindowsProcCloseWindow, (LPARAM)&nameList);
}

struct ChangeWindowTitleData
{
	tstring orcTitle;
	tstring toTitle;
	HWND hWnd;
};

BOOL CALLBACK EnumWindowsProcChangeWindowTitle(HWND hwnd, LPARAM lParam)
{
	tstring text = WindowTool::GetWindowText(hwnd);
	ChangeWindowTitleData titleData = *((ChangeWindowTitleData*)lParam);
	if (text.size() > 0 && text.find(titleData.orcTitle) != tstring::npos)
	{
		int ret = MessageBox(titleData.hWnd, text.c_str(), "是这个标题么",
			MB_YESNOCANCEL);
		if (IDYES == ret)
			SetWindowText(hwnd, titleData.toTitle.c_str());
		else if(IDCANCEL == ret)
			return FALSE;
	}
	return TRUE;
}

void VolumeScreenWebSpeedControl::OnChangeWindowTitle()
{
	auto changeFun = [this](const tstring& orcTitle,
		const tstring& toTitle)
	{
		ChangeWindowTitleData titleData;
		titleData.orcTitle = orcTitle;
		titleData.toTitle = toTitle;
		titleData.hWnd = hWnd_;
		EnumWindows(EnumWindowsProcChangeWindowTitle, (LPARAM)&titleData);
	};

	ChangeTitleDlg* pTitle = new ChangeTitleDlg();
	pTitle->CreateDlgE(hWnd_);
	pTitle->setDelteOnClose(true);
	pTitle->showCenter();
	pTitle->activeWindow();
	pTitle->setChangeEvent(changeFun);
}

void VolumeScreenWebSpeedControl::ShowShutDownDlg()
{
	if (!shutDownDlg_)
	{
		shutDownDlg_ = new ShutDownDlg;
		shutDownDlg_->CreateDlgE(nullptr);
	}
	shutDownDlg_->ShowDlg();
}

bool VolumeScreenWebSpeedControl::WndProc(HWND hWnd,
	UINT message, WPARAM wParam, LPARAM lParam, LRESULT* lResult)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		switch (wParam)
		{
		case enTray_timingClose:
		{
			ShowShutDownDlg();
			break;
		}		
		case enTray_timingPopup:
		{
			OnTaskList();
			break;
		}
		case enTray_timingPopupNew:
		{
			OnTaskNew();
			break;
		}
		case enTray_closeNameWindow:
		{
			OnCloseNameWindow();
			break;
		}
		case enTray_changeWindowTitle:
		{
			OnChangeWindowTitle();
			break;
		}
		case enTray_exit:
		{
			RemoveInvalidTimingTask();
			if (timingTaskDlgList_.size() > 0)
			{
				for (auto per : timingTaskDlgList_)
				{
					per->setVisible(true);
				}
				MessageBox(hWnd, _T("还有任务在运行，请先取消"), 
					_T("警告"), MB_OK);
				break;
			}

			if (shutDownDlg_ && shutDownDlg_->isTasking())
			{
				MessageBox(hWnd, _T("定时关机任务在运行，请先取消"),
					_T("警告"), MB_OK);
				break;
			}

			int res = MessageBox(hWnd, _T("真的要退出么？"),
				_T("警告"), MB_YESNO);
			if (res != IDYES)
			{
				break;
			}
			CloseApp(hWnd);
			break;
		}
		default:
			break;
		}
		break;
	}
	case WM_TaskBarMsg:
	{
		if (WM_LBUTTONDOWN == lParam || WM_RBUTTONDOWN == lParam)
		{
			SetActiveWindow(hWnd); //失去焦点时菜单自动消失
			POINT pt;
			GetCursorPos(&pt);
			HMENU hMenu;
			hMenu = CreatePopupMenu();
			AppendMenu(hMenu, MF_STRING, enTray_timingClose, "定时关机");
			AppendMenu(hMenu, MF_STRING, enTray_timingPopup, "定时任务列表");
			AppendMenu(hMenu, MF_STRING, enTray_timingPopupNew, "新建定时任务");
			//AppendMenu(hMenu, MF_STRING, enTray_closeNameWindow, "关闭弹窗");
			AppendMenu(hMenu, MF_STRING, enTray_changeWindowTitle,
				"改窗口标题");
			AppendMenu(hMenu, MF_STRING, enTray_exit, "退出");
			TrackPopupMenu(hMenu, TPM_LEFTBUTTON, pt.x, pt.y,
				NULL, hWnd, nullptr);
			DestroyMenu(hMenu);
			int a = 1;
		}
		break;
	}
	case WM_DESTROY:
	{
		break;
	}	
	default:
		break;
	}

	if (WM_TASKBARCREATED == message)
	{
		InitTaskBar(hWnd);
	}
	return false;
}

void VolumeScreenWebSpeedControl::RemoveInvalidTimingTask()
{
	for (auto iter = timingTaskDlgList_.begin();
		iter != timingTaskDlgList_.end();)
	{
		if (!(*iter)->isTasking() && !(*iter)->isVisible() )
		{
			delete *iter;
			iter = timingTaskDlgList_.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void VolumeScreenWebSpeedControl::CloseApp(HWND hWnd)
{
	Shell_NotifyIcon(NIM_DELETE, &nid_);
	PostQuitMessage(1);
}