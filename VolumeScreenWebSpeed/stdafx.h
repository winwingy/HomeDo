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
#include <functional>
using namespace std;

void traceFun(LPCTSTR lpszFormat, ...);

 #ifdef _DEBUG
 #define TRACEWW(formatStr, ...) do {traceFun(_T("%s, %d"), __FUNCTION__, __LINE__);\
 	traceFun(formatStr, __VA_ARGS__); OutputDebugString(_T("\r\n")); }while (0);
 #else
 #define TRACEWW 
 #endif

#ifdef _DEBUG
#define TRACEFUN  traceFun(_T("%s, %d\n"), __FUNCTION__, __LINE__);
#else
#define TRACEFUN 
#endif

#ifdef _UNICODE
#define  tstring std::wstring
#else
#define  tstring std::string
#endif

#define RETURN_ASSERT(x) if(!x) {assert(0); return;}