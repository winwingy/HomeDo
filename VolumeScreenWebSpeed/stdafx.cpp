// stdafx.cpp : 只包括标准包含文件的源文件
// VolumeScreenWebSpeed.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 "stdafx.h" 中
// 引用任何所需的附加头文件，而不是在此文件中引用
void traceFun(LPCTSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);

	const int bufLen = 1024;
	TCHAR buffer[bufLen];
	int len = _vstprintf_s(buffer, bufLen, lpszFormat, args);
	buffer[bufLen - 1] = 0;

	va_end(args);
	if (len > 0)
	{
		OutputDebugString(buffer);
	}
}