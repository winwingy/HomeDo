// PolyCtrl.cpp : CPolyCtrl ��ʵ��
#include "stdafx.h"
#include "PolyCtrl.h"


// CPolyCtrl

STDMETHODIMP CPolyCtrl::get_Sides(SHORT* pVal)
{
	// TODO: �ڴ����ʵ�ִ���
	*pVal = m_nSides;

	return S_OK;
}

STDMETHODIMP CPolyCtrl::put_Sides(SHORT newVal)
{
	// TODO: �ڴ����ʵ�ִ���

	if (newVal > 2 && newVal < 101)
	{
		m_nSides = newVal;
		return S_OK;
	}
	else
		return Error(_T("Shape must have between 3 and 100 sides"));
}
