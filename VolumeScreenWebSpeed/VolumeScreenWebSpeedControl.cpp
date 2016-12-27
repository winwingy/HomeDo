#include "StdAfx.h"
#include "VolumeScreenWebSpeedControl.h"
#include <algorithm>
#include <iterator>
#include <TlHelp32.h>
#include <shellapi.h>
#include <assert.h>
#include <sstream>

#include "WinDefine.h"
#include "ShutDownDlg.h"
#include "config.h"
#include "StringPathHelper.h"
#include "VolumeCtrlWrapper.h"
#include "ScreenSaveControllor.h"
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
    : config_(Config::GetShared())
    , volumeCtrlWrapper_(new VolumeCtrlWrapper())
    , powerOnStartProgress_()
    , screenSaveControllor_(new ScreenSaveControllor())
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
        idHotKey.path = path;
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

void VolumeScreenWebSpeedControl::InitProgressConfig(HWND hWnd)
{
    volumeCtrlWrapper_->InitVolumeHotKey(hWnd);
    screenSaveControllor_->InitControllor(hWnd);
}

void VolumeScreenWebSpeedControl::OnCreate(HWND hWnd, UINT uMsg,
                              WPARAM wParam, LPARAM lParam)
{
    RaiseToken();

    InitPowerOnStartProgress(hWnd);

    InitHotKey(hWnd, uMsg, wParam, lParam);

    InitProgressConfig(hWnd);
}

BOOL CALLBACK EnumThreadWindows(
    HWND hwnd, LPARAM lParam)
{
    PostMessage(hwnd, WM_CLOSE, 0, 0);
    return TRUE;
}

void EnumCloseThreadWnd(DWORD pId)
{
    HANDLE hSnapshotThread = INVALID_HANDLE_VALUE;
    do 
    {
        hSnapshotThread = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pId);
        if (hSnapshotThread == INVALID_HANDLE_VALUE)
            break;

        THREADENTRY32 pe = { 0 };
        pe.dwSize = sizeof(THREADENTRY32);
        if (!Thread32First(hSnapshotThread, &pe))
            break;

        do
        {
            ::EnumWindows(EnumThreadWindows, pe.th32ThreadID);
        } while (Thread32Next(hSnapshotThread, &pe));
        Sleep(3000);
    } while (0);
    if (hSnapshotThread != INVALID_HANDLE_VALUE)
        CloseHandle(hSnapshotThread);
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
        
        if (Process32First(hSnapshot, &pe))
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
                        EnumCloseThreadWnd(pe.th32ProcessID);

                    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE,
                                                  pe.th32ProcessID);
                    TerminateProcess(hProcess, 0);
                    CloseHandle(hProcess);
                }
            }
        } while (Process32Next(hSnapshot, &pe));
        
    } while (0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
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
        TRACE_WW(szRet);
        TRACE_WW("\n");
        //OpenProcessToken（）这个函数的作用是打开一个进程的访问令牌   
        //GetCurrentProcess（）函数的作用是得到本进程的句柄   
        bRet = LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
        sprintf_s(szRet, _countof(szRet), "%d", bRet);
        TRACE_WW(szRet);
        TRACE_WW("\n");
        //LookupPrivilegeValue（）的作用是修改进程的权限   
        tkp.PrivilegeCount = 1;
        //赋给本进程特权   
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        bRet = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
                                     (PTOKEN_PRIVILEGES)NULL, 0);
        //AdjustTokenPrivileges（）的作用是通知Windows   NT修改本进程的权利  
        sprintf_s(szRet, _countof(szRet), "%d", bRet);
        TRACE_WW(szRet);
        TRACE_WW("\n");
        bPrivi = true;
    }
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
            ShutDownDlg dlg;
            dlg.DoModal(NULL);
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
                ShellExcuteProgress(it->path);
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
                    KillProgressByNames(vector<string>(1, killName), true);

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
