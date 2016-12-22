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
#include "MyVolumeCtrl.h"

#include "WinDefine.h"
#include "EasyWindow.h"
#include "ShutDownDlg.h"
#include "config.h"
#include "StringPathHelper.h"
#pragma comment(lib, "Kernel32.lib")
using namespace std;

namespace
{
    void PlaySoundHappy(int beg, int end)
    {
        unsigned FREQUENCY[] = { 392, 392, 440, 392, 523, 494,
            392, 392, 440, 392, 587, 523,
            392, 392, 784, 659, 523, 494, 440,
            689, 689, 523, 587, 523 };

        unsigned DELAY[] = { 375, 125, 500, 500, 500, 1000,
            375, 125, 500, 500, 500, 1000,
            375, 125, 500, 500, 500, 500, 1000,
            375, 125, 500, 500, 500, 1000 };
        for (int i = beg; i < end; i++)  //先填10短些
        {
            Beep(FREQUENCY[i], DELAY[i]);
        }
    }

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
        ::RegisterHotKey(hWnd, hotKeyId, vkCtrl, vkKey);
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
    , config_(Config::GetShared())
    , powerOnStartProgress_()
{
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

string RemoveLastExt(const string& fileName, string* ext)
{
    std::string ret(fileName);
    string::size_type pos = ret.rfind(".");
    if (pos != string::npos)
    {
        if (ext)
        {
            ext->assign(ret, pos, ret.size() - pos);
        }
        ret.erase(pos);
    }
    return ret;
}

void WinControlTool::ReplaceString(string& orc, const string& findWhat, const string& replaceTo)
{
    string::size_type posBeg = 0;
    string::size_type posEnd = -1;
    while ((posEnd = orc.find(findWhat, posBeg)) != string::npos)
    {
        orc.replace(posEnd, findWhat.length(), replaceTo);
        posBeg = posEnd + findWhat.length();
    }
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


    hotkey = config_->GetValue(CONFIG_SET_HOTKEY, "HotKeyNotScreenSave", "");
    bRet = RgeisterStringHotKey(hotkey, hWnd, HOTKEY_CLOSE_SCREEN);

    hotkey = config_->GetValue(CONFIG_SET_HOTKEY, "HotKeyNotScreenSave", "");
    bRet = RgeisterStringHotKey(hotkey, hWnd, HOTKEY_NOT_SCREEN_SAVE);

    hotkey = config_->GetValue(CONFIG_SET_HOTKEY, "HotKeyKillProcess", "");
    bRet = RgeisterStringHotKey(hotkey, hWnd, HOTKEY_KILL_PROCESS);

    hotkey = config_->GetValue(CONFIG_SET_HOTKEY, "HotKeyShutDown", "");
    bRet = RgeisterStringHotKey(hotkey, hWnd, HOTKEY_NOT_SHUT_DOWN);
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

VOID CALLBACK WinControlTool::VolumeTimerProc(
    HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    WinControlTool* pThis = reinterpret_cast<WinControlTool*>(
        GetWindowLongPtr(hwnd, GWLP_USERDATA));
    WinDefine* winDefine = WinDefine::GetInstance();
    int initVolume = pThis->config_->GetValue(CONFIG_SET, "InitVolume", 30);
    if (FALSE == winDefine->bFinishInitVolume_ && winDefine->iIsInitVolume_ == 1)
    {
        GetInstance()->GetMyVolumeCtrl()->SetVolume(winDefine->initVolume_);
        winDefine->initVolume_ = 0;
    }
    winDefine->bFinishInitVolume_ = TRUE;
    KillTimer(hwnd, idEvent);
}

void WinControlTool::InitProgressConfig(HWND hWnd)
{
    WinDefine* winDefine = WinDefine::GetInstance();
    int perVolumeGap = config_->GetValue(CONFIG_SET, "PerVolumeGap", 3);
    winDefine->perVoulumeGap_ = perVolumeGap;
 
    int isInitVolume = config_->GetValue(CONFIG_SET, "IsInitVolume", 1);
    if (isInitVolume)
    {
        int initVolume = config_->GetValue(CONFIG_SET, "InitVolume", 30);
        
        winDefine->initVolume_ = initVolume;
        int initTime = config_->GetValue(CONFIG_SET, "InitTime", 5000);
    winDefine->iInitTime_ = initTime;
    }
    winDefine->iIsInitVolume_ = atoi(strValue.c_str());
    SetTimer(hWnd, TIMER_INIT_VOLUME, winDefine->iInitTime_, TimerProc);

    //不屏保
    strValue = GetValueFromConfig(CONFIG_SET, "notScreenSavePerInputTime", "3", CONFIG_INF_FILENAME);
    winDefine->notScreenSavePerInputTime_ = atoi(strValue.c_str());
    strValue = GetValueFromConfig(CONFIG_SET, "notScreenSaveCanTryCnt", "3", CONFIG_INF_FILENAME);
    winDefine->notScreenSaveCanTryCnt_ = atoi(strValue.c_str());
}

void WinControlTool::OnCreate(HWND hWnd, UINT message,
                              WPARAM wParam, LPARAM lParam)
{
    RaiseToken();

    InitPowerOnStartProgress(hWnd);

    InitHotKey(hWnd, message, wParam, lParam);

    InitProgressConfig(hWnd);
}

string WinControlTool::W2A(wstring strWide)
{
    int iSize = (strWide.size() + 1) * 2;
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
    ReplaceString(strNameExe, "\r\n", "\n");
    ReplaceString(strNameExe, "\r", "\n");
    vector<string> vecName;
    SplitStringBySign(vecName, strNameExe, "\n");
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
        stExecInfo.lpFile = WinDefine::GetInstance()->
            powerOnStartProgress_[0].c_str();
        stExecInfo.nShow = SW_HIDE;
        BOOL bRet = ShellExecuteExA(&stExecInfo);
    }

    if (pThis->powerOnStartProgress_.empty())
        KillTimer(hwnd, idEvent);
}

VOID CALLBACK WinControlTool::TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    WinControlTool* thisClass = WinControlTool::GetInstance();
    if (TIMER_CLOSE_SCREEN == idEvent || TIMER_CLOSE_SCREEN_ADD == idEvent)
    {
        KillTimer(hwnd, idEvent);
        ::PostMessage(hwnd, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)1);
        ::PostMessage(hwnd, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2);
    }
    else if (TIMER_INIT_VOLUME == idEvent)
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
    else if (TIMER_NOT_SCREEN_SAVE == idEvent)
    {
        TRACE_WW("TIMER_NOT_SCREEN_SAVE == idEvent\n");
        bool notScreenContinue = false;
        if (thisClass->forcegroundWindowNotScreenSaveHwnd_ &&
            IsWindow(thisClass->forcegroundWindowNotScreenSaveHwnd_))
        {
            HWND fore = GetForegroundWindow();
            if (fore == thisClass->forcegroundWindowNotScreenSaveHwnd_)
            {
                TRACE_WW("fore == thisClass->forcegroundWindowNotScreenSaveHwnd\n");
                INPUT input = { 0 };
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
            if (--thisClass->notScreenSaveCanTryCntLeave_ <= 0)
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
        SetTimer(hWnd, TIMER_NOT_SCREEN_SAVE, WinDefine::GetInstance()->notScreenSavePerInputTime_ * 1000, TimerProc);
        string strValue = GetValueFromConfig(CONFIG_SET, "notScreenSavePerInputMAXTime", "120", CONFIG_INF_FILENAME);
        SetTimer(hWnd, TIMER_NOT_SCREEN_SAVE_MAX, atoi(strValue.c_str()) * 60 * 1000, TimerProc);
        notScreenSaveCanTryCntLeave_ = WinDefine::GetInstance()->notScreenSaveCanTryCnt_;
        strValue = GetValueFromConfig(CONFIG_SET, "notScreeenSaveMsgBox", "0", CONFIG_INF_FILENAME);
        if (strValue != "0")
        {
            easyWindow_->Create(nullptr, 600, 0, 200, 50);
        }
        PlaySoundHappy(0, 6);
    }
}

void WinControlTool::OnHotKeyNotScreenSaveCustom(hWnd, message, wParam, lParam)
{


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
    char szVolume[256] = { 0 };
    int iIDHotKey = (int)wParam;

    WinDefine* winDefine = WinDefine::GetInstance();

    int iTime(0);
    BOOL bRet(FALSE);
    switch (iIDHotKey)
    {
        case HOTKEY_VOLUME_UP:
        {
            iVolume = GetMyVolumeCtrl()->GetVolume();	//得到的值偏小1						
            sprintf_s(szVolume, sizeof(szVolume), "iVolume = %d\n", iVolume);
            iVolume += winDefine->perVoulumeGap_;
            GetMyVolumeCtrl()->SetVolume(iVolume);
            break;
        }
        case HOTKEY_VOLUME_DOWN:
        {
            if (FALSE == winDefine->bFinishInitVolume_)
            {
                GetMyVolumeCtrl()->SetVolume(winDefine->initVolume_ * 1 / 3);
                winDefine->initVolume_ = 0;
                winDefine->bFinishInitVolume_ = TRUE;
            }
            iVolume = GetMyVolumeCtrl()->GetVolume();	//得到的值偏小1						
            sprintf_s(szVolume, sizeof(szVolume), "iVolume = %d\n", iVolume);
            iVolume -= winDefine->perVoulumeGap_;
            if (iVolume < 0)
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
        case HOTKEY_NOT_SCREEN_SAVE_CUSTOM:
        {
            OnHotKeyNotScreenSaveCustom(hWnd, message, wParam, lParam);
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
                SHELLEXECUTEINFOA stInfo = { 0 };
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