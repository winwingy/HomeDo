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
}

VolumeScreenWebSpeedControl::VolumeScreenWebSpeedControl(void)
    : config_(Config::GetShared())
    , volumeCtrlWrapper_(new VolumeCtrlWrapper())
    , powerOnStartProgress_()
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
    string hotkey = config_->GetValue(CONFIG_SET_HOTKEY, "HotKeyVolumeUp", "");
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
        CONFIG_POWER_ON_START_PROGRESS, "PowerOnStartProgressTime", 5000);

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
}

void VolumeScreenWebSpeedControl::OnCreate(HWND hWnd, UINT uMsg,
                              WPARAM wParam, LPARAM lParam)
{
    RaiseToken();

    InitPowerOnStartProgress(hWnd);

    InitHotKey(hWnd, uMsg, wParam, lParam);

    InitProgressConfig(hWnd);
}

BOOL CALLBACK VolumeScreenWebSpeedControl::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    DWORD dwThisID = 0;
    GetWindowThreadProcessId(hwnd, &dwThisID);
    if (dwThisID == (DWORD)lParam)
    {
        PostMessage(hwnd, WM_CLOSE, 0, 0);
    }
    return TRUE;
}

void VolumeScreenWebSpeedControl::KillProgressByNames(
    const vector<string>& nameList, bool tryExistFirst)
{
    vector<string> vecUpName;
    std::transform(nameList.begin(), nameList.end(),
                   std::back_inserter(vecUpName),
                   StringPathHelper::ToUpperString);

    PROCESSENTRY32 pe = { 0 };
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
        for (vector<string>::const_iterator it = vecUpName.begin(); it < vecUpName.end(); ++it)
        {
            if (*it == pe.szExeFile)
            {
                HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
                TRACE_WW(pe.szExeFile);
                TRACE_WW("\t\t");
#ifdef TRACE_WW
                char szProcess[50] = { 0 };
                sprintf_s(szProcess, _countof(szProcess), "%p", hProcess);
                TRACE_WW(szProcess);
                TRACE_WW("\n");
#endif
                if (tryExistFirst)
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

void VolumeScreenWebSpeedControl::TerminateNameExe(string& strNameExe)
{
    if (strNameExe.empty())
    {
        return;
    }
    StringPathHelper::ReplaceString(strNameExe, "\r\n", "\n");
    StringPathHelper::ReplaceString(strNameExe, "\r", "\n");
    vector<string> vecName;
    StringPathHelper::SplitStringBySign(strNameExe, "\n", &vecName);
    KillProgressByNames(vecName, false);
}

void  VolumeScreenWebSpeedControl::OnPowerOnStartProgressTimer(
    HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    if (!powerOnStartProgress_.empty())
    {
        SHELLEXECUTEINFOA stExecInfo = { 0 };
        stExecInfo.cbSize = sizeof(stExecInfo);
        stExecInfo.fMask = SEE_MASK_FLAG_NO_UI;
        stExecInfo.lpFile = powerOnStartProgress_[0].c_str();
        stExecInfo.nShow = SW_HIDE;
        BOOL bRet = ShellExecuteExA(&stExecInfo);
        powerOnStartProgress_.erase(powerOnStartProgress_.begin());
    }

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

string VolumeScreenWebSpeedControl::GetKillNameBuff()
{
    char szPath[2048] = { 0 };
    GetModuleFileName(NULL, szPath, sizeof(szPath));
    string strPath(szPath);
    int iPos = strPath.rfind("\\");
    strPath.erase(iPos);

    string strAnotherPath(strPath);

    strPath += string("\\") + CONFIG_INF_FILENAME;
    FILE* fp = NULL;
    fopen_s(&fp, strPath.c_str(), "r");
    if (fp == NULL)
    {
        iPos = strAnotherPath.rfind("\\");
        strAnotherPath.erase(iPos);
        strAnotherPath += string("\\") + CONFIG_INF_FILENAME;
        fopen_s(&fp, strAnotherPath.c_str(), "r");
        if (fp == NULL)
        {
            return "";
        }
    }
    char szBuffer[2048] = { 0 };
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


void VolumeScreenWebSpeedControl::OnKillProcess(HWND hWnd)
{
    string killName = GetKillNameBuff();
    TerminateNameExe(killName);
    TerminateNameExe(killName);
}

bool VolumeScreenWebSpeedControl::ForcegroundWindowFullScreen(HWND forcegroundWindow)
{
    if (forcegroundWindow)
    {
        RECT rect = { 0 };
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

void VolumeScreenWebSpeedControl::OnHotKey(HWND hWnd, UINT uMsg,
                                           int idHotKey, LPARAM lParam)
{
    volumeCtrlWrapper_->OnHotKey(hWnd, uMsg, idHotKey, lParam);

    int iTime(0);
    BOOL bRet(FALSE);
    switch (idHotKey)
    {
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
        for (vector<ProgressToIDHotKey>::iterator it = progressToIDHotkeyList_.begin();
             it != progressToIDHotkeyList_.end(); ++it)
        {
            if (it->ID == idHotKey)
            {
                SHELLEXECUTEINFOA stInfo = { 0 };
                stInfo.cbSize = sizeof(stInfo);
                stInfo.lpFile = it->path.c_str();
                stInfo.nShow = SW_SHOW;
                ShellExecuteExA(&stInfo);
            }
        }
    }
    else if (idHotKey >= WinDefine::HOTKEY_PROGRESS_KILL_BEGIN &&
             idHotKey <= WinDefine::HOTKEY_PROGRESS_KILL_END)
    {
        for (vector<ProgressToIDHotKey>::iterator it = progressToIDHotkeyList_.begin();
             it != progressToIDHotkeyList_.end(); ++it)
        {
            if (it->killID == idHotKey)
            {
                string::size_type index = 0;
                if (((index = it->path.rfind("\\")) != -1) ||
                    ((index = it->path.rfind("/")) != -1))
                {
                    string killName(it->path, ++index, it->path.size() - index);
                    KillProgressByNames(vector<string>(1, killName), true);
                }
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
}

