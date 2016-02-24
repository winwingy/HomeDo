// SomeExeOpenFileDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SomeExeOpenFile.h"
#include "SomeExeOpenFileDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CSomeExeOpenFileDlg �Ի���




CSomeExeOpenFileDlg::CSomeExeOpenFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSomeExeOpenFileDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSomeExeOpenFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSomeExeOpenFileDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CSomeExeOpenFileDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BTN_EXEOPENFILE, &CSomeExeOpenFileDlg::OnBnClickedBtnExeopenfile)
END_MESSAGE_MAP()


// CSomeExeOpenFileDlg ��Ϣ�������

BOOL CSomeExeOpenFileDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CSomeExeOpenFileDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSomeExeOpenFileDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CSomeExeOpenFileDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL GetExeCommand(char *fullname,char* command);

void CSomeExeOpenFileDlg::OnBnClickedButton1()
{
	char command[256];
	GetExeCommand("D:\\KuGou\\By2 - ��֪��.mp3", command);
	TRACE("%s%d\n", command, strlen(command));
	m_strCommand = command;
	string strCommand;
	strCommand = command;//(command, command + strlen(command));
	TRACE("%s\n", strCommand.c_str());
	strCommand.erase(0, 1);
	string::size_type szPos = strCommand.find("\"");
	if( szPos != string::npos)
	{
		m_cstrExecute = string(strCommand, 0, szPos).c_str();
	}
	//"D:\Program Files\TTPlayer\TTPlayer.exe" ""D:\KuGou\By2 - ��֪��.mp3""
	//ShellExecute�� ���������� ���ո���ַ�����Ҫ������������		
	m_cstrFileName = string(strCommand, szPos + 3, strCommand.length() - (szPos+3) -1 ).c_str();	

}
//����ĺ�����ע���õ��������͵�open�����С�
////
BOOL GetExeCommand(char *fullname,char* command)
{// ""%1" 1*" for .bat file
	// "xxx.exe %1"
	// ""\a b\c\yyy.exe" /p %1"
	char ext[20];
	char pathname[MAX_PATH];
	char exename[MAX_PATH];
	char *at=0;
	strcpy(pathname,fullname);
	// must ".xxx"
	if((at=strrchr(fullname,'.'))==NULL) return FALSE;
	strcpy(ext,at);

	char open_key[MAX_PATH];
	DWORD openkeylen=MAX_PATH;
	char shellname[MAX_PATH]="shell";
	// under shell may be different "open","cplopen","Open with"...
	BYTE extfile[MAX_PATH];
	DWORD cbextfile=MAX_PATH;
	DWORD dwType=REG_SZ;
	HKEY hkey;
	HKEY hsubkey;
	// from ".xxx" get "xxxfile"
	DWORD dwErrorCode=RegOpenKeyEx(HKEY_CLASSES_ROOT,ext,0,KEY_READ,&hkey);
	if (dwErrorCode != ERROR_SUCCESS)
	{ // error not found
		command[0]=0;
		return FALSE;
	}
	else
	{ // ".xxx" OK
		// get "xxxfile"
		dwErrorCode=RegQueryValueEx(hkey,NULL,0,&dwType,extfile,&cbextfile);
		RegCloseKey(hkey);
		char tmp[MAX_PATH];
		sprintf(tmp,"%s\\%s",extfile,shellname);// "cppfile\\shell"
		if (dwErrorCode == ERROR_SUCCESS)  
		{// "cppfile\\shell"
			dwErrorCode=RegOpenKeyEx(HKEY_CLASSES_ROOT,tmp,0,KEY_READ,&hsubkey);
			if (dwErrorCode == ERROR_SUCCESS)  
			{// "cppfile\\shell"
				LONG enumErr=ERROR_SUCCESS;
				DWORD j=0;
				FILETIME ftLastWriteTime;
				while (enumErr==ERROR_SUCCESS || enumErr==ERROR_MORE_DATA)
				{
					cbextfile=260;// re-init
					openkeylen=MAX_PATH;// re-init
					enumErr=RegEnumKeyEx(hsubkey,j,open_key,&openkeylen,
						NULL,NULL,NULL,&ftLastWriteTime);
					strcpy(tmp,open_key);
					_strupr(tmp);
					if(strstr(tmp,"OPEN")) // any "open"
					{
						sprintf(tmp,"%s\\%s\\%s\\command",extfile,shellname,open_key);// "cppfile\\shell"
						break;
					}
					j++;
				}
				RegCloseKey(hsubkey);
				if(enumErr!=ERROR_SUCCESS) return FALSE;
			} // end with "shell" key
			else
			{ // no "shell" key
				return FALSE;
			}
			// "shell\\...open...\\command"	
			dwErrorCode=RegOpenKeyEx(HKEY_CLASSES_ROOT,tmp,0,KEY_READ,&hsubkey);
			cbextfile=260;// re-init
			//extfile="%SystemRoot%\\system32\\NOTEPAD32.EXE %1"
			dwErrorCode=RegQueryValueEx(hsubkey,NULL,0,&dwType,extfile,&cbextfile);
			//	if fullname contains space in its pathname
			if(strchr(fullname,' ')) sprintf(exename,"\"%s\"",fullname);
			else strcpy(exename,fullname);
			// replace "%SystemRoot%" to "c:\\winnt.400" //NT
			if(strstr((char *)extfile,"%SystemRoot%"))
			{  
				char windir[MAX_PATH];
				GetWindowsDirectory(windir,MAX_PATH);
				at=strchr((char *)extfile,'\\');// after "%SystemRoot%"

				strcat(windir,at);
				strcpy((char *)extfile,windir);
			}
			// parameterse	
			strcpy(tmp,(char*)extfile);
			if(at=strstr(tmp,"%1"))
			{
				*(at+1)='s';// "%s"  
				sprintf(command,tmp,exename);
			}
			else
			{
				sprintf(command,"%s %s",tmp,exename);
			}
			RegCloseKey(hsubkey);
			return TRUE;
		}
		else
		{
			command[0]=0;
			return FALSE;
		}
	}
}
void CSomeExeOpenFileDlg::OnBnClickedBtnExeopenfile()
{
	
	TRACE("%s\n", m_cstrFileName.GetBuffer());
	//���ո���ַ�����Ҫ������������
	//int iRet = (int)ShellExecute(0, "open", m_cstrExecute, "\"D:\\KuGou\\By2 - ��֪��.mp3\"",NULL, SW_SHOW);
	int iRet = (int)ShellExecute(0, "open", m_cstrExecute, m_cstrFileName,NULL, SW_SHOW);
}
