// HookThisProgress.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CHookThisProgressApp:
// �йش����ʵ�֣������ HookThisProgress.cpp
//

class CHookThisProgressApp : public CWinApp
{
public:
	CHookThisProgressApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CHookThisProgressApp theApp;