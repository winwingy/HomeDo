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
            int key = WinDefine::GetInstance()->HotKeyStrigToVkKey(*it);
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

WinControlTool* WinControlTool::winControlTool = NULL;

WinControlTool::WinControlTool(void)
    : config_(Config::GetShared())
    , volumeCtrlWrapper_(new VolumeCtrlWrapper())
    , powerOnStartProgress_()
{

}

WinControlTool::~WinControlTool(void)
{

}

void WinControlTool::InitProgressHotKey(HWND hWnd)
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
            idHotKey.ID = HOTKEY_PROGRESS_BEGIN + i; 
            TranslateStringToVKKey(hotkey, &idHotKey.vkCtrl, &idHotKey.vkKey);
            bRet = RegisterHotKey(hWnd, idHotKey.ID, idHotKey.vkCtrl, 
                                  idHotKey.vkKey);
        }

        if (!killhotkey.empty())
        {
            idHotKey.killID = HOTKEY_PROGRESS_KILL_BEGIN + i;
            TranslateStringToVKKey(killhotkey, &idHotKey.vkKillCtrl,
                                   &idHotKey.vkKillKey);
            bRet = RegisterHotKey(hWnd, idHotKey.killID, idHotKey.vkKillCtrl,
                                  idHotKey.vkKillKey);
        }
        progressToIDHotkeyList_.push_back(idHotKey);
    }
}

void WinControlTool::InitGeneralHotKey(HWND hWnd)
{
    BOOL bRet = FALSE;
    string hotkey = config_->GetValue(CONFIG_SET_HOTKEY, "HotKeyVolumeUp", "");
    bRet = RgeisterStringHotKey(hotkey, hWnd, HOTKEY_VOLUME_UP);

    hotkey = config_->GetValue(CONFIG_SET_HOTKEY, "HotKeyVolumeDown", "");
    bRet = RgeisterStringHotKey(hotkey, hWnd, HOTKEY_VOLUME_DOWN);

    hotkey = config_->GetValue(CONFIG_SET_HOTKEY, "HotKeyCloseScreen", "");
    bRet = RgeisterStringHotKey(hotkey, hWnd, HOTKEY_CLOSE_SCREEN);

    hotkey = config_->GetValue(CONFIG_SET_HOTKEY, "HotKeyNotScreenSave", "");
    bRet = RgeisterStringHotKey(hotkey, hWnd, HOTKEY_NOT_SCREEN_SAVE);

    hotkey = config_->GetValue(CONFIG_SET_HOTKEY, "HotKeyKillProcess", "");
    bRet = RgeisterStringHotKey(hotkey, hWnd, HOTKEY_KILL_PROCESS);

    hotkey = config_->GetValue(CONFIG_SET_HOTKEY, "HotKeyShutDown", "");
    bRet = RgeisterStringHotKey(hotkey, hWnd, HOTKEY_SHUT_DOWN);
}

void WinControlTool::InitHotKey(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    InitGeneralHotKey(hWnd);
    //程序Hotkey
    InitProgressHotKey(hWnd);
}

void WinControlTool::InitPowerOnStartProgress(HWND hWnd)
{
    int progressTime = config_->GetValue(
        CONFIG_POWER_ON_START_PROGRESS, "PowerOnStartProgressTime", 5000);

    int progressCount = config_->GetValue(
        CONFIG_POWER_ON_START_PROGRESS, "PowerOnStartProgressCount", 10);
    for (int i = 1; i <= progressCount; ++i)
    {
        stringstream ss("StartProgress");
        ss << i;
        string path = config_->GetValue(
            CONFIG_POWER_ON_START_PROGRESS, ss.str().c_str(), "");
        if (!path.empty())
            powerOnStartProgress_.push_back(path);
    }
    SetTimer(hWnd, TIMER_POWER_ON_START_PROGRESS, progressTime,
             PowerOnStartProgressTimeProc);
}

void WinControlTool::InitProgressConfig(HWND hWnd)
{
    volumeCtrlWrapper_->InitVolumeHotKey(hWnd);
}

void WinControlTool::OnCreate(HWND hWnd, UINT message,
                              WPARAM wParam, LPARAM lParam)
{
    RaiseToken();

    InitPowerOnStartProgress(hWnd);

    InitHotKey(hWnd, message, wParam, lParam);

    InitProgressConfig(hWnd);
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

void WinControlTool::KillProgressByNames(
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

void WinControlTool::TerminateNameExe(string& strNameExe)
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

VOID CALLBACK WinControlTool::PowerOnStartProgressTimeProc(
    HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    WinControlTool* pThis = reinterpret_cast<WinControlTool*>(
        GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (!pThis->powerOnStartProgress_.empty())
    {
        SHELLEXECUTEINFOA stExecInfo = { 0 };
        stExecInfo.cbSize = sizeof(stExecInfo);
        stExecInfo.fMask = SEE_MASK_FLAG_NO_UI;
        stExecInfo.lpFile = pThis->powerOnStartProgress_[0].c_str();
        stExecInfo.nShow = SW_HIDE;
        BOOL bRet = ShellExecuteExA(&stExecInfo);
    }

    if (pThis->powerOnStartProgress_.empty())
        KillTimer(hwnd, idEvent);
}

VOID CALLBACK WinControlTool::TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    if (TIMER_INIT_VOLUME == idEvent)
    {

    }
    else if (TIMER_GET_WEB_TIME == idEvent)
    {
        SHELLEXECUTEINFO stExecInfo = { 0 };
        stExecInfo.cbSize = sizeof(stExecInfo);
        stExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
        stExecInfo.lpFile = "GetWebAndSetTime.exe";
        stExecInfo.nShow = SW_HIDE;
        BOOL bRet = ShellExecuteEx(&stExecInfo);
        DWORD dwErr = GetLastError();
        WaitForSingleObject(stExecInfo.hProcess, INFINITE);
        DWORD dwCode = -2;
        bRet = GetExitCodeProcess(stExecInfo.hProcess, &dwCode);
        if (dwCode == 0 || ++WinDefine::GetInstance()->iGetWebTimeCnt_ > 5)
        {
            KillTimer(hwnd, idEvent);
        }
    }   
}

void WinControlTool::RaiseToken()
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

string WinControlTool::GetKillNameBuff()
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

void WinControlTool::OnHotKey(HWND hWnd, UINT message, 
                              WPARAM wParam, LPARAM lParam)
{
    int idHotKey = (int)wParam;
    volumeCtrlWrapper_->OnHotKey(hWnd, message, idHotKey, lParam);
    WinDefine* winDefine = WinDefine::GetInstance();

    int iTime(0);
    BOOL bRet(FALSE);
    switch (idHotKey)
    {
        case HOTKEY_KILL_PROCESS:
        {
            OnKillProcess(hWnd, message, wParam, lParam);
            break;
        }
        case HOTKEY_SHUT_DOWN:
        {
            ShutDownDlg dlg;
            dlg.DoModal(NULL);
            break;
        }
    }

    if (idHotKey >= HOTKEY_PROGRESS_BEGIN && idHotKey <= HOTKEY_PROGRESS_END)
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
    else if (idHotKey >= HOTKEY_PROGRESS_KILL_BEGIN && idHotKey <= HOTKEY_PROGRESS_KILL_END)
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

void WinControlTool::OnTimer(HWND hWnd, UINT message, 
                             WPARAM wParam, LPARAM lParam)
{
    UINT_PTR idEvent = wParam;
    DWORD dwTime = lParam;
    volumeCtrlWrapper_->OnTimer(hWnd, message, idEvent, dwTime);
}

