// "stdafx.h" : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <memory>
#include <string>
#include <vector>
#include <assert.h>
#include <time.h>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <map>
using namespace std;

void traceFun(LPCTSTR lpszFormat, ...);

#ifdef _DEBUG
#define TRACE_WY(formatStr, ...) do {traceFun(_T("%s, %d"), __FUNCTION__, __LINE__);\
	traceFun(formatStr, __VA_ARGS__); OutputDebugString(_T("\r\n")); }while (0);
#else
#define TRACE_WY //
#endif

#define TRACE TRACE_WY
#define TRACE_WW TRACE_WY

#ifdef _UNICODE
#define  tstring std::wstring
#else
#define  tstring std::string
#endif