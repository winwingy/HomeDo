// SomeExeOpenFileDlg.h : ͷ�ļ�
//

#pragma once

#include <string>
using namespace std;
// CSomeExeOpenFileDlg �Ի���
class CSomeExeOpenFileDlg : public CDialog
{
// ����
public:
	CSomeExeOpenFileDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SOMEEXEOPENFILE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedBtnExeopenfile();
private:
	CString m_cstrExecute;
	CString m_cstrFileName;
	string m_strCommand;
};
