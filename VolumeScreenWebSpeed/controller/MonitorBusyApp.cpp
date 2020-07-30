#include "stdafx.h"
#include "MonitorBusyApp.h"
#include "tool\config.h"
#include <tlhelp32.h>
#include "tool\StringPathHelper.h"
#include <Psapi.h>
#include <shellapi.h>
#pragma comment (lib,"Psapi.lib")


MonitorBusyApp::MonitorBusyApp()
{

}

void MonitorBusyApp::init()
{
	
}

int GetProcessNumber()
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return (int)info.dwNumberOfProcessors;

}

bool MonitorBusyApp::checkAppCpuBusy(string appName, HANDLE hProcess)
{
	FILETIME creationTime;
	FILETIME exitTime;
	FILETIME kernelTime;
	FILETIME userTime;
	BOOL ret = GetProcessTimes(hProcess, &creationTime, &exitTime,
		&kernelTime, &userTime);
	if (FALSE == ret)
		return false;

	FILETIME now;
	GetSystemTimeAsFileTime(&now);
	int64_t now_time = StringPathHelper::toInt64(now);
	int64_t system_time = (StringPathHelper::toInt64(kernelTime) +
		StringPathHelper::toInt64(userTime)) /
		GetProcessNumber();
	auto iter = busyCntList_.find(appName);
	if (iter == busyCntList_.end())
	{
		BusyAppItem busyItem;
		busyItem .busycnt = 0;
		busyItem.last_system_time = system_time;
		busyItem.last_time = now_time;
		busyCntList_[appName] = busyItem;
		return false;
	}
	int64_t system_time_delta = system_time - iter->second.last_system_time;
	int64_t time_delta = now_time - iter->second.last_time;
	if (time_delta <= 0)
		return false;

	int cpu = (int)((system_time_delta * 100 + time_delta / 2) / time_delta);
	TRACEWW("use appname  %s  cpu %d", appName.c_str(), cpu);
	iter->second.last_system_time = system_time;
	iter->second.last_time = now_time;
	return cpu > 20;
}

void MonitorBusyApp::checkBusyApp()
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
			string appName = StringPathHelper::ToUpperString(pe.szExeFile);
			vector<string> listText;
			Config::GetShared()->GetList(_T("[MonitorBusyAppBeg]"),
				_T("[MonitorBusyAppEnd]"), &listText);
			for (auto&per : listText)
			{
				string upperText = StringPathHelper::ToUpperString(per);
				if (appName != upperText)
					continue;

				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE,
					pe.th32ProcessID);
				if (INVALID_HANDLE_VALUE == hProcess || !hProcess)
					continue;
				if (!checkAppCpuBusy(appName, hProcess))
				    continue;

				auto iter = busyCntList_.find(appName);
				if (iter == busyCntList_.end())
					continue;
				if (++(iter->second.busycnt) > 3)
				{
					iter->second.busycnt = 0;
					char strPath[1024] = { 0 };
					if (::GetModuleFileNameExA(hProcess, NULL,
						strPath, 1023 + 1))
					{
						restartAppList_[strPath] = 1;
					}
					TerminateProcess(hProcess, 0);
					CloseHandle(hProcess);
				}
			}
		} while (Process32Next(hSnapshot, &pe));
	} while (0);
	if (hSnapshot != INVALID_HANDLE_VALUE)
		CloseHandle(hSnapshot);
}

void MonitorBusyApp::OnTimer(HWND hWnd, UINT uMsg,
	UINT_PTR idEvent, DWORD dwTime)
{
	checkBusyApp();
	for (auto& per : restartAppList_)
	{
		ShellExecute(NULL, "open", per.first.c_str(),
			NULL, NULL, SW_SHOWNORMAL);
	}
	restartAppList_.clear();
}
