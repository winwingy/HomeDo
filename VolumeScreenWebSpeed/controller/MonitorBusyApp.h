#pragma once

class MonitorBusyApp
{
public:
	MonitorBusyApp();
	void init();
	void OnTimer(
		HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	
private:
	void checkBusyApp();
	bool MonitorBusyApp::checkAppCpuBusy(string appName, HANDLE hProcess);
	struct BusyAppItem
	{
		int busycnt = 0;
		int64_t last_system_time = 0;
		int64_t last_time = 0;
	};
	std::map<std::string, BusyAppItem> busyCntList_;
	std::map<std::string, int> restartAppList_;
};