#include "Common.h"
#include <tchar.h>

#include <Windows.h>
#include <stdio.h>
int (WINAPIV * __snprintf)(char *, size_t, const char*...) = _snprintf;
int (WINAPIV * _sprintf)(char *, const char*...) = sprintf;
int (WINAPIV * _sscanf)(const char *, const char*...) = sscanf;

CCmdHandler::CCmdHandler(CUnitManager * pUnitManager, TCHAR * mailslotName)
    : m_pUnitManager(pUnitManager),
      m_bExitThread(FALSE),
      m_bRunService(FALSE),
      m_bHeroDetect(FALSE),
      m_bBombDetect(FALSE),
      m_bWardDetect(FALSE),
      m_bRuneDetect(FALSE),
      m_factionType(FactionNone),
      m_bConnected(FALSE),
      m_hListenThread(NULL)
{
	ZeroMemory(m_wcsSrvDesc, SRV_DESC_LENGTH);
    ZeroMemory(&m_localGameOpt, sizeof(GAME_OPTION));

    m_hMailslot = CreateMailslot(mailslotName, 0, 300, NULL);

	GAME_OPTION gameOpt;

	gameOpt.on = TRUE;
	gameOpt.factionType = FactionAutoMatch;
	gameOpt.hero = TRUE;
	gameOpt.ward = TRUE;
	gameOpt.bomb = FALSE;
	gameOpt.rune = TRUE;
	gameOpt.fx   = 1366 / 1366;
	gameOpt.fy   = 768 / 768;

	UnitConfig(&gameOpt);
}

CCmdHandler::~CCmdHandler()
{
    m_bExitThread = TRUE;

    DWORD dwRetn = WaitForSingleObject(m_hListenThread, 1000);

    if (WAIT_TIMEOUT == dwRetn)
    {
		FkDbgPrint((TEXT("Terminate Thread")));
        TerminateThread(m_hListenThread, 1);
    }

	FkDbgPrint((TEXT("Thread is exit")));

    CloseHandle(m_hMailslot);
}

BOOL CCmdHandler::Listen()
{
    if (NULL == m_hMailslot)
	{
		FkDbgPrint((TEXT("�Ͳ�δ�����ɹ�")));
        return FALSE;
    }

    m_hListenThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ListenThread, this, 0, NULL);

    if (NULL == m_hListenThread)
	{
		FkDbgPrint((TEXT("���������߳�ʧ��")));
        return FALSE;
    }

    CloseHandle(m_hListenThread);

    return TRUE;
}

void CCmdHandler::OnConnect(UCHAR * buffer, DWORD dwLength)
{
    if (dwLength == sizeof(GAME_OPTION))
    {
        UnitConfig((PGAME_OPTION)buffer);
		m_bConnected = TRUE;

		FkDbgPrint((TEXT("�пͻ�������")));
    }
}

void CCmdHandler::OnReceive(UCHAR * buffer, DWORD dwLength)
{
    if (dwLength == sizeof(GAME_OPTION))
    {
        UnitConfig((PGAME_OPTION)buffer);
    }
}

void CCmdHandler::OnClose()
{
    // UnitCleanup();
	m_bConnected = FALSE;

	FkDbgPrint((TEXT("���ӹر�")));
}

DWORD CCmdHandler::ListenThread(LPVOID lpThreadParameter)
{
    static DWORD nTimeOutCount = 0; // times of timeout

    CCmdHandler * pCmdHandler = (CCmdHandler *)lpThreadParameter;

    DWORD  dwLastError;
    BOOL   bReadFile;
    DWORD  dwReadByte;
    GAME_OPTION gameOpt;

    while (TRUE)    // read command length
    {
        bReadFile = ReadFile(pCmdHandler->m_hMailslot, &gameOpt, sizeof(gameOpt), &dwReadByte, NULL);

        if (pCmdHandler->m_bExitThread)
		{
			FkDbgPrint((TEXT("�˳�����ѭ��")));
            break;
        }
        
        if (!bReadFile)
        {
            dwLastError = GetLastError();

            // error
            if (ERROR_SEM_TIMEOUT != dwLastError)
			{
				FkDbgPrint((TEXT("δ֪�쳣 %d"), dwLastError));
                break;
            }

            // time out 
            if (nTimeOutCount != 6)
            {
                nTimeOutCount++;
            }
        }
        else
		{
			if (nTimeOutCount == 5 || nTimeOutCount == 6)
			{
				pCmdHandler->OnConnect((PUCHAR)&gameOpt, dwReadByte);
			}
			else
			{
				pCmdHandler->OnReceive((PUCHAR)&gameOpt, dwReadByte);
			}

			nTimeOutCount = 0;
        }

        // when count is 5, client is off
        // when count is 6, no client connect
        if (nTimeOutCount == 5)
        {
            // Close 
            pCmdHandler->OnClose();
        }
    }

    return 0;
}

void CCmdHandler::UnitConfig(PGAME_OPTION pGameOpt)
{
    if (memcmp(&m_localGameOpt, pGameOpt, sizeof(GAME_OPTION)) != 0)
    {
		FkDbgPrint((TEXT("�û����÷����ı�")));
		
        CopyMemory(&m_localGameOpt, pGameOpt, sizeof(GAME_OPTION));

        m_bRunService = m_localGameOpt.on;
        m_bHeroDetect = m_localGameOpt.hero;
        m_bBombDetect = m_localGameOpt.bomb;
        m_bWardDetect = m_localGameOpt.ward;
        m_bRuneDetect = m_localGameOpt.rune;
        m_factionType = m_localGameOpt.factionType;

		m_pUnitManager->SetScreenPixel(m_localGameOpt.fx, m_localGameOpt.fy);

		// ��������
		swprintf_s(
			m_wcsSrvDesc, 
			SRV_DESC_LENGTH,
			TEXT("Ӣ��(%s) ����(%s) ը��(%s) ���(%s) �ж�("), 
			m_bHeroDetect ? TEXT("����") : TEXT("�ر�"),
			m_bWardDetect ? TEXT("����") : TEXT("�ر�"),
			m_bBombDetect ? TEXT("����") : TEXT("�ر�"),
			m_bRuneDetect ? TEXT("����") : TEXT("�ر�"));

		switch (m_factionType)
		{
		case FactionNone:
			wcscat_s(m_wcsSrvDesc, SRV_DESC_LENGTH, TEXT("��)"));
			break;
		case FactionBoth:
			wcscat_s(m_wcsSrvDesc, SRV_DESC_LENGTH, TEXT("˫��)"));
			break;
		case FactionAutoMatch:
			wcscat_s(m_wcsSrvDesc, SRV_DESC_LENGTH, TEXT("�Զ�)"));
			break;
		case FactionSentinel:
			wcscat_s(m_wcsSrvDesc, SRV_DESC_LENGTH, TEXT("����)"));
			break;
		case FactionScourge:
			wcscat_s(m_wcsSrvDesc, SRV_DESC_LENGTH, TEXT("����)"));
			break;
		default:
			break;
		}
    }
}

void CCmdHandler::UnitCleanup()
{
    m_bConnected  = FALSE;

    m_bRunService = FALSE;
    m_bHeroDetect = FALSE;
    m_bBombDetect = FALSE;
    m_bWardDetect = FALSE;
    m_bRuneDetect = FALSE;
    m_factionType = FactionNone;

    ZeroMemory(&m_localGameOpt, sizeof(GAME_OPTION));
}


//=================================================================
//  ��  ��: ���������Ĺ���
//  ��  ע: ����Ϸ����ʾ
//  ��  ��: Flicker317
//  ��  ��: 1.0 2012/11/21
//=================================================================
void CCmdHandler::ServiceDesc(OUT wchar_t wcsSrvDesc[40])
{
	wcscpy_s(wcsSrvDesc, 40, m_wcsSrvDesc);
}