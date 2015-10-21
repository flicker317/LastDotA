
// LastAppDlg.h : ͷ�ļ�
//

#pragma once


typedef enum _FactionType
{
	FactionNone = 0,
	FactionBoth,
	FactionAutoMatch,
	FactionSentinel,
	FactionScourge
} FactionTypeId;

typedef struct _GAME_OPTION
{
	BOOL on;

	FactionTypeId factionType;
	BOOL hero;
	BOOL ward;
	BOOL bomb;
	BOOL rune;

	float fx;
	float fy;

} GAME_OPTION, *PGAME_OPTION;

// CLastAppDlg �Ի���
class CLastAppDlg : public CDialogEx
{
// ����
public:
	CLastAppDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_LASTAPP_DIALOG };

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
    
	// afx_msg void OnBnClickedNotifyChange();
    
	afx_msg void OnBnClickedBtnWinmax();
    afx_msg void OnBnClickedBtnWinnormal();
    afx_msg void OnBnClickedBtnInject();

	void ConfigGameOpt(GAME_OPTION &gameOpt);

    static DWORD ConnectThread(LPARAM lParam);
};

static BOOL ImprovePrivilege();
static BOOL InjectLibrary(HANDLE hRemoteProcess, TCHAR * szLibPath);