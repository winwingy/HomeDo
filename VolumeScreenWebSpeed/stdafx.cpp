// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// VolumeScreenWebSpeed.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO: �� "stdafx.h" ��
// �����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������
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