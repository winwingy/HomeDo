// CircleDlg.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCircleDlgApp:
// �йش����ʵ�֣������ CircleDlg.cpp
//

class CCircleDlgApp : public CWinApp
{
public:
	CCircleDlgApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCircleDlgApp theApp;