// SomeExeOpenFile.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSomeExeOpenFileApp:
// �йش����ʵ�֣������ SomeExeOpenFile.cpp
//

class CSomeExeOpenFileApp : public CWinApp
{
public:
	CSomeExeOpenFileApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSomeExeOpenFileApp theApp;