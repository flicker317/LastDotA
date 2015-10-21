
// LastAppDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LastApp.h"
#include "LastAppDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
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

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CLastAppDlg �Ի���




CLastAppDlg::CLastAppDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLastAppDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_MAIN);
}

void CLastAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLastAppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    /*
	ON_BN_CLICKED(IDC_RADIO_AUTO, &CLastAppDlg::OnBnClickedNotifyChange)
	ON_BN_CLICKED(IDC_RADIO_SENTINEL, &CLastAppDlg::OnBnClickedNotifyChange)
	ON_BN_CLICKED(IDC_RADIO_SCOURGE, &CLastAppDlg::OnBnClickedNotifyChange)
	ON_BN_CLICKED(IDC_RADIO_BOTH, &CLastAppDlg::OnBnClickedNotifyChange)
	ON_BN_CLICKED(IDC_CHECK_HERO, &CLastAppDlg::OnBnClickedNotifyChange)
	ON_BN_CLICKED(IDC_CHECK_WARD, &CLastAppDlg::OnBnClickedNotifyChange)
	ON_BN_CLICKED(IDC_CHECK_BOMB, &CLastAppDlg::OnBnClickedNotifyChange)
	ON_BN_CLICKED(IDC_CHECK_RUNE, &CLastAppDlg::OnBnClickedNotifyChange)
    */
	ON_BN_CLICKED(IDC_BTN_WINMAX, &CLastAppDlg::OnBnClickedBtnWinmax)
	ON_BN_CLICKED(IDC_BTN_WINNORMAL, &CLastAppDlg::OnBnClickedBtnWinnormal)
	ON_BN_CLICKED(IDC_BTN_INJECT, &CLastAppDlg::OnBnClickedBtnInject)
END_MESSAGE_MAP()


// CLastAppDlg ��Ϣ�������

BOOL CLastAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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
	((CButton *)GetDlgItem(IDC_RADIO_AUTO))->SetCheck(TRUE);
	((CButton *)GetDlgItem(IDC_CHECK_HERO))->SetCheck(TRUE);
	((CButton *)GetDlgItem(IDC_CHECK_WARD))->SetCheck(TRUE);
	((CButton *)GetDlgItem(IDC_CHECK_RUNE))->SetCheck(TRUE);

    HANDLE hConnectThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CLastAppDlg::ConnectThread, this, 0, NULL);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CLastAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CLastAppDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CLastAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*
void CLastAppDlg::OnBnClickedNotifyChange()
{
	GAME_OPTION gameOpt;
	DWORD bytesWritten;
	HANDLE hMailslot;

	hMailslot = CreateFile(
		TEXT("\\\\.\\Mailslot\\FF68535B-4AF0-72F6-0F5B-6A40141468E3"),
		GENERIC_WRITE,
		FILE_SHARE_READ, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (INVALID_HANDLE_VALUE == hMailslot)
	{
		MessageBox(TEXT("δ��⵽����ģ�飡"), TEXT("��ʾ"), MB_OK);
		return;
	}

	ConfigGameOpt(gameOpt);

	if(0 == WriteFile(hMailslot, &gameOpt, sizeof(gameOpt), &bytesWritten, NULL))
	{
		MessageBox(TEXT("���ó���"), TEXT("��ʾ"), MB_OK);
	}

	CloseHandle(hMailslot);
}
*/

void CLastAppDlg::ConfigGameOpt(GAME_OPTION & gameOpt)
{
    int cx, cy;

	ZeroMemory(&gameOpt, sizeof(gameOpt));

	gameOpt.on = TRUE;
	gameOpt.hero = ((CButton *)GetDlgItem(IDC_CHECK_HERO))->GetCheck() ? TRUE : FALSE;
	gameOpt.ward = ((CButton *)GetDlgItem(IDC_CHECK_WARD))->GetCheck() ? TRUE : FALSE;
	gameOpt.bomb = ((CButton *)GetDlgItem(IDC_CHECK_BOMB))->GetCheck() ? TRUE : FALSE;
	gameOpt.rune = ((CButton *)GetDlgItem(IDC_CHECK_RUNE))->GetCheck() ? TRUE : FALSE;

    cx = GetSystemMetrics(SM_CXSCREEN);
    cy = GetSystemMetrics(SM_CYSCREEN);

	gameOpt.fx = 1.0f * cx / 1366;
	gameOpt.fy = 1.0f * cy / 768;

	if (((CButton *)GetDlgItem(IDC_RADIO_AUTO))->GetCheck())
	{
		gameOpt.factionType = FactionAutoMatch;
	}
	else
	if (((CButton *)GetDlgItem(IDC_RADIO_SENTINEL))->GetCheck())
	{
		gameOpt.factionType = FactionSentinel;
	}
	else
	if (((CButton *)GetDlgItem(IDC_RADIO_SCOURGE))->GetCheck())
	{
		gameOpt.factionType = FactionScourge;
	}
	else
	if (((CButton *)GetDlgItem(IDC_RADIO_BOTH))->GetCheck())
	{
		gameOpt.factionType = FactionBoth;
	}
}

void CLastAppDlg::OnBnClickedBtnWinmax()
{
	int cx, cy, nStyle;
	HWND hGame;
	
	hGame = ::FindWindow(NULL, TEXT("Warcraft III"));
	if (NULL == hGame)
	{
		MessageBox(TEXT("δ��⵽ħ�޴��ڣ�"), TEXT("��ʾ"), MB_OK);
		return;
	}

	cx = GetSystemMetrics(SM_CXSCREEN);
	cy = GetSystemMetrics(SM_CYSCREEN);

	::ShowWindow(hGame, TRUE);
	::SetForegroundWindow(hGame);

	nStyle  = ::GetWindowLong(hGame, GWL_STYLE);
	nStyle &= ~WS_CAPTION;
	nStyle &= ~WS_THICKFRAME;
	::SetWindowLong(hGame, GWL_STYLE, nStyle);
	::SetWindowPos(hGame, CWnd::wndNoTopMost, 0, 0, cx, cy,0);
}

void CLastAppDlg::OnBnClickedBtnWinnormal()
{
	int cx, cy, nStyle;
	HWND hGame;

	hGame = ::FindWindow(NULL, TEXT("Warcraft III"));
	if (NULL == hGame)
	{
		MessageBox(TEXT("δ��⵽ħ�޴��ڣ�"), TEXT("��ʾ"), MB_OK);
		return;
	}

	cx = GetSystemMetrics(SM_CXSCREEN);
	cy = GetSystemMetrics(SM_CYSCREEN);

	::ShowWindow(hGame, TRUE);
	::SetForegroundWindow(hGame);

	nStyle  = ::GetWindowLong(hGame, GWL_STYLE);
	nStyle |= WS_CAPTION;
	nStyle |= WS_THICKFRAME;
	::SetWindowLong(hGame, GWL_STYLE, nStyle);
	::SetWindowPos(hGame, CWnd::wndNoTopMost, cx / 2 - 400, cy / 2 - 300, 800, 600, 0);
}

void CLastAppDlg::OnBnClickedBtnInject()
{
	HWND	hGameWnd;
	HANDLE	hGameHandle;
	DWORD	dwGamePid;
	WCHAR	wcsLibName[256];
	WCHAR * pLibPath;

	ImprovePrivilege();

	hGameWnd = ::FindWindow(NULL, TEXT("Warcraft III"));
	if (NULL == hGameWnd)
	{
		MessageBox(TEXT("δ��⵽ħ�޴��ڣ�"), TEXT("��ʾ"), MB_OK);
		return;
	}

	GetWindowThreadProcessId(hGameWnd, &dwGamePid);

	hGameHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwGamePid);
	if (NULL == hGameHandle)
	{
		MessageBox(TEXT("�򿪽��̳���"), TEXT("��ʾ"), MB_OK);
		return;
	}

	GetModuleFileName(NULL, wcsLibName, 256);
	pLibPath = wcsrchr(wcsLibName, '\\');
	pLibPath[0] = '\0';	// *pLibPath = '\0';

	wsprintf(wcsLibName, TEXT("%s\\%s"), wcsLibName, TEXT("LastTry.dll"));

	if (!InjectLibrary(hGameHandle, wcsLibName))
	{
		MessageBox(TEXT("ע��ģ��ʧ�ܣ�"), TEXT("��ʾ"), MB_OK);
	}

	CloseHandle(hGameHandle);
}

DWORD CLastAppDlg::ConnectThread(LPARAM lParam)
{
    CLastAppDlg *pLastAppDlg = (CLastAppDlg *)lParam;

    GAME_OPTION gameOpt;
    DWORD bytesWritten;
    HANDLE hMailslot;

    CString strLastError = L"";
    while (TRUE)
    {
        Sleep(500);

        hMailslot = CreateFile(
            TEXT("\\\\.\\Mailslot\\FF68535B-4AF0-72F6-0F5B-6A40141468E3"),
            GENERIC_WRITE,
            FILE_SHARE_READ, 
            NULL, 
            OPEN_EXISTING, 
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (INVALID_HANDLE_VALUE == hMailslot)
        {
            continue;
        }

        while (TRUE)
        {
            pLastAppDlg->ConfigGameOpt(gameOpt);

            if (0 == WriteFile(hMailslot, &gameOpt, sizeof(gameOpt), &bytesWritten, NULL))
            {
                if (ERROR_HANDLE_EOF != GetLastError())
                {
                    OutputDebugString(L"unknown error");
                }
                
                CloseHandle(hMailslot);
                break;
            }
            Sleep(500);
        }
    }
}


// ������Ȩ
static BOOL ImprovePrivilege()
{
	HANDLE	hToken	= NULL;										//���ƾ��
	BOOL	bRet	= FALSE;									//����ִ�н��
	TOKEN_PRIVILEGES tp = {1, {0, 0, SE_PRIVILEGE_ENABLED}};	//���Ȩ�����ƽṹ

	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);			//��ѯ�Ƿ���е���Ȩ��
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);	//�򿪽���Ȩ������
	AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof tp, 0, 0);					//Ϊ�������� DEBUG Ȩ��
	bRet = (GetLastError() == ERROR_SUCCESS);									//����Ƿ�ִ�гɹ�

	return bRet;
}

// ע��dll
static BOOL InjectLibrary(HANDLE hRemoteProcess, TCHAR * szLibPath)
{
	HANDLE hRemoteThread     = NULL;
	HANDLE hRemoteFunc       = NULL;
	PVOID  pRemoteParam      = NULL;
	DWORD  dwWriten          = 0;
	BOOL   bRet              = FALSE;

	TCHAR   szLibPathCopy[256] = {0};
	lstrcpyW(szLibPathCopy, szLibPath);

	if (hRemoteProcess == NULL)
	{
		return FALSE;
	}

	int iSize = (_tcslen(szLibPath) + 1) * sizeof(TCHAR);

	pRemoteParam = VirtualAllocEx(hRemoteProcess, NULL, iSize, MEM_COMMIT, PAGE_READWRITE);
	if (pRemoteParam == NULL)
	{
		return FALSE;
	}

	bRet = WriteProcessMemory(hRemoteProcess, pRemoteParam, (LPVOID)szLibPathCopy, iSize, &dwWriten);
	if (!bRet)
	{
		if (pRemoteParam)
		{
			VirtualFreeEx(hRemoteProcess, pRemoteParam, 0, MEM_RELEASE);
		}

		return FALSE; 
	}
#ifdef UNICODE
	hRemoteFunc = GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");
#else
	hRemoteFunc = GetProcAddress(GetModuleHandle("Kernel32"), "LoadLibraryA");
#endif
	hRemoteThread = CreateRemoteThread(hRemoteProcess, 0, 0, (LPTHREAD_START_ROUTINE)hRemoteFunc, pRemoteParam, 0, &dwWriten);

	// �ȴ��߳̽���
	if (hRemoteThread)
	{
		WaitForSingleObject(hRemoteThread, INFINITE);
		HMODULE g_hRemoteHandle;
		GetExitCodeThread(hRemoteThread, (DWORD*)&g_hRemoteHandle);
	}

	// ������
	if (pRemoteParam)
	{
		VirtualFreeEx(hRemoteProcess, pRemoteParam, 0, MEM_RELEASE);
	}

	CloseHandle(hRemoteThread);

	return TRUE;
}
