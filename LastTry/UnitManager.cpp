#include "Common.h"

CUnitManager::CUnitManager(DWORD dwGameAddr) 
	: m_idAddrList(UNIT_TABLE_MAX), 
      m_dwGameAddr(dwGameAddr), 
	  m_nHerosCount(0), 
      m_nSentryWardsCount(0), 
      m_nObserverWardsCount(0), 
      m_nBombsCount(0), 
      m_runeType(RUNE_NONE)
{
}

CUnitManager::~CUnitManager(void)
{
}

//=================================================================
//  ��  ��: �ж��Ƿ�����Ϸ��
//  ��  ע: 
//  ��  ��: Flicker317
//  ��  ��: 1.0 2012/11/21
//=================================================================
BOOL CUnitManager::IsInGame()
{
	return (*(DWORD *)(m_dwGameAddr + 0xACAA38));
}

//=================================================================
//  ��  ��: �õ������������
//  ��  ע: ����ֵ��С 0~15
//  ��  ��: Flicker317
//  ��  ��: 1.0 2012/11/21
//=================================================================
DWORD CUnitManager::GetLocalPlayer()
{	
    WORD rt;
    DWORD dwGameAddr = m_dwGameAddr;
    __asm
    {
        mov  eax, dwGameAddr;
        mov  eax, dword ptr ds:[eax + 0xACD44C];
        test eax, eax;
        je   err;
        mov  eax, dword ptr ds:[eax + 0x28];
        mov  rt, ax;
    }
    return (DWORD)rt;
err:
    return 0xF;
}

//=================================================================
//  ��  ��: �����Ա�������Unit����
//  ��  ע: Game.dll+0xACB0D8  00000310		�ṹ���С
//			Game.dll+0xACB0DC  00000100		�����С
//			Game.dll+0xACB0E0  00000???		��λ������
//			Game.dll+0xACB0E4  ????????		��ά��̬����ָ��
//			Game.dll+0xACB0E8  ????????		�����ٵĵ�λ����
//			�ṹ��ͷΪ Game.dll+0x943A94		��ʾδ������
//  ��  ��: Flicker317
//  ��  ��: 1.0 2012/11/19
//=================================================================
void CUnitManager::TraversalUnitId()
{
	DWORD dwArraySize;			// �����С
	DWORD dwStructSize;			// �ṹ���С
	DWORD dwStructCount;		// �ṹ������
	DWORD dwCurrStoreAddr;		// ��ǰ��������ĵ�ַ����������ʱ��

	dwStructSize	= GetMemory(m_dwGameAddr + 0xACB0D8);
	dwStructCount	= GetMemory(m_dwGameAddr + 0xACB0E0);
	dwArraySize		= GetMemory(m_dwGameAddr + 0xACB0DC);

	dwCurrStoreAddr = m_dwGameAddr + 0xACB0E4;

	// ���
	m_idAddrList.clear();

	while ((dwCurrStoreAddr = GetMemory(dwCurrStoreAddr)))
	{
		m_idAddrList.push_back(dwCurrStoreAddr);
	}
}

//=================================================================
//  ��  ��: ˢ�±��浥λ��Ϣ������
//  ��  ע: ����Ϸ��ʵʱ���ã���hookס��EndScene���ã�Ƶ�ʿ��Խ��ͣ�
//  ��  ��: Flicker317
//  ��  ��: 1.0 2012/11/30
//=================================================================
void CUnitManager::RefreshUnitId(FactionTypeId factionType, BOOL bHero, BOOL bWard, BOOL bBomb)
{
	DWORD dwArraySize;			// �����С
	DWORD dwStructSize;			// �ṹ���С
	DWORD dwStructCount;		// �ṹ������
	DWORD dwCurrStruct;			// ��ǰ�����Ľṹ��ַ���� unit �� item �� id
	DWORD dwLocalIndex;			// �����������
	DWORD dwCurrIndex;			// ��ǰ��λ����

	dwStructSize	= GetMemory(m_dwGameAddr + 0xACB0D8);
	dwStructCount	= GetMemory(m_dwGameAddr + 0xACB0E0);
	dwArraySize		= GetMemory(m_dwGameAddr + 0xACB0DC);

	dwLocalIndex	= GetLocalPlayer();

	// ���
	m_nHerosCount			= 0;
	m_nSentryWardsCount		= 0;
	m_nObserverWardsCount	= 0;
	m_nBombsCount			= 0;

	for (unsigned int i = 0, totalCount = 0; i < m_idAddrList.size(); i++)
	{
		for (unsigned int j = 0; j < dwArraySize && totalCount < dwStructCount; j++)
		{
			dwCurrStruct = m_idAddrList[i] + 4 + j * dwStructSize;

			if (GetMemory(dwCurrStruct) == m_dwGameAddr + 0x943A94)
			{
				totalCount++;

HEROBLOCK:
				if (bHero)
				{
					switch ((GetMemory(dwCurrStruct + 0x30) >> 24))
					{
					case 'O': case 'U': case 'H': case 'N': case 'E':
						break;
					default:
						goto WARDBLOCK;
					}

					dwCurrIndex = GetMemory(dwCurrStruct + 0x24);

					switch (factionType)
					{
					case FactionNone:
						break;
					case FactionBoth:
						{
							if (m_nHerosCount < HERO_COUNT_MAX && 
								(dwCurrIndex > 0 && dwCurrIndex < 6) || (dwCurrIndex > 6 && dwCurrIndex < 12))
							{
								AddToHerosUnit(dwCurrStruct, dwCurrIndex);
							}
						}
						break;
					case FactionAutoMatch:
						{
							if (m_nHerosCount < HERO_COUNT_MAX && 
								((dwCurrIndex > 0 && dwCurrIndex < 6) && (dwLocalIndex > 6 && dwLocalIndex < 12)) || 
								((dwLocalIndex > 0 && dwLocalIndex < 6) && (dwCurrIndex > 6 && dwCurrIndex < 12)))
							{
								AddToHerosUnit(dwCurrStruct, dwCurrIndex);
							}
						}
						break;
					case FactionSentinel:
						{
							if (m_nHerosCount < HERO_COUNT_MAX && dwCurrIndex > 0 && dwCurrIndex < 6)
							{
								AddToHerosUnit(dwCurrStruct, dwCurrIndex);
							}
						}
						break;
					case FactionScourge:
						{
							if (m_nHerosCount < HERO_COUNT_MAX && dwCurrIndex > 6 && dwCurrIndex < 12)
							{
								AddToHerosUnit(dwCurrStruct, dwCurrIndex);
							}
						}
						break;
					default:
						break;
					}
				}
WARDBLOCK:
				if (bWard)
				{
					if (GetMemory(dwCurrStruct + 0x8) == 0)
						goto BOMBBLOCK;

					dwCurrIndex = GetMemory(dwCurrStruct + 0x24);

					switch(GetMemory(dwCurrStruct + 0x30))
					{
					case 0x6F657965:	// ����
						{
							switch (factionType)
							{
							case FactionNone:
								break;
							case FactionBoth:
								{
									if (m_nObserverWardsCount < WARD_COUNT_MAX && 
										(dwCurrIndex > 0 && dwCurrIndex < 6) || (dwCurrIndex > 6 && dwCurrIndex < 12))
									{
										AddToObserverWardsUnit(dwCurrStruct, dwCurrIndex);
									}
								}
								break;
							case FactionAutoMatch:
								{
									if (m_nObserverWardsCount < WARD_COUNT_MAX && 
										((dwCurrIndex  > 0 && dwCurrIndex  < 6) && (dwLocalIndex > 6 && dwLocalIndex < 12)) || 
										((dwLocalIndex > 0 && dwLocalIndex < 6) && (dwCurrIndex  > 6 && dwCurrIndex  < 12)))
									{
										AddToObserverWardsUnit(dwCurrStruct, dwCurrIndex);
									}
								}
								break;
							case FactionSentinel:
								{
									if (m_nObserverWardsCount < WARD_COUNT_MAX && dwCurrIndex > 0 && dwCurrIndex < 6)
									{
										AddToObserverWardsUnit(dwCurrStruct, dwCurrIndex);
									}
								}
								break;
							case FactionScourge:
								{
									if (m_nObserverWardsCount < WARD_COUNT_MAX && dwCurrIndex > 6 && dwCurrIndex < 12)
									{
										AddToObserverWardsUnit(dwCurrStruct, dwCurrIndex);
									}
								}
								break;
							}
						}
						break;
					case 0x6F303034:	// ����
						{
							switch (factionType)
							{
							case FactionNone:
								break;
							case FactionBoth:
								{
									if (m_nSentryWardsCount < WARD_COUNT_MAX && 
										(dwCurrIndex > 0 && dwCurrIndex < 6) || (dwCurrIndex > 6 && dwCurrIndex < 12))
									{
										AddToSentryWardsUnit(dwCurrStruct, dwCurrIndex);
									}
								}
								break;
							case FactionAutoMatch:
								{
									if (m_nSentryWardsCount < WARD_COUNT_MAX && 
										((dwCurrIndex > 0 && dwCurrIndex < 6) && (dwLocalIndex > 6 && dwLocalIndex < 12)) || 
										((dwLocalIndex > 0 && dwLocalIndex < 6) && (dwCurrIndex > 6 && dwCurrIndex < 12)))
									{
										AddToSentryWardsUnit(dwCurrStruct, dwCurrIndex);
									}
								}
								break;
							case FactionSentinel:
								{
									if (m_nSentryWardsCount < WARD_COUNT_MAX && dwCurrIndex > 0 && dwCurrIndex < 6)
									{
										AddToSentryWardsUnit(dwCurrStruct, dwCurrIndex);
									}
								}
								break;
							case FactionScourge:
								{
									if (m_nSentryWardsCount < WARD_COUNT_MAX && dwCurrIndex > 6 && dwCurrIndex < 12)
									{
										AddToSentryWardsUnit(dwCurrStruct, dwCurrIndex);
									}
								}
								break;
							}
						}
						break;
					}

				}
BOMBBLOCK:
				if (bBomb)
				{
					if (GetMemory(dwCurrStruct + 0x8) == 0)
						continue;

					dwCurrIndex = GetMemory(dwCurrStruct + 0x24);

					switch(GetMemory(dwCurrStruct + 0x30))
					{
					case 0x6E30304F:
					case 0x6E303050:
					case 0x6E303051:
					case 0x6E30304E:
					case 0x6F746F74:
					case 0x6F303138:
					case 0x6F303032:
					case 0x6F303042:
					case 0x6F303142:
						{
							switch (factionType)
							{
							case FactionNone:
								break;
							case FactionBoth:
								{
									if (m_nBombsCount < BOMB_COUNT_MAX && 
										(dwCurrIndex > 0 && dwCurrIndex < 6) || (dwCurrIndex > 6 && dwCurrIndex < 12))
									{
										AddToBombsUnit(dwCurrStruct, dwCurrIndex);
									}
								}
								break;
							case FactionAutoMatch:
								{
									if (m_nBombsCount < BOMB_COUNT_MAX && 
										((dwCurrIndex > 0 && dwCurrIndex < 6) && (dwLocalIndex > 6 && dwLocalIndex < 12)) || 
										((dwLocalIndex > 0 && dwLocalIndex < 6) && (dwCurrIndex > 6 && dwCurrIndex < 12)))
									{
										AddToBombsUnit(dwCurrStruct, dwCurrIndex);
									}
								}
								break;
							case FactionSentinel:
								{
									if (m_nBombsCount < BOMB_COUNT_MAX && dwCurrIndex > 0 && dwCurrIndex < 6)
									{
										AddToBombsUnit(dwCurrStruct, dwCurrIndex);
									}
								}
								break;
							case FactionScourge:
								{
									if (m_nBombsCount < BOMB_COUNT_MAX && dwCurrIndex > 6 && dwCurrIndex < 12)
									{
										AddToBombsUnit(dwCurrStruct, dwCurrIndex);
									}
								}
								break;
							}
						}
						break;
					}
				}
			}
		}
	}
}


//=================================================================
//  ��  ��: ��Ӣ����Ϣ���浽��Ա����
//  ��  ע: ��
//  ��  ��: Flicker317
//  ��  ��: 1.0 2012/11/19
//=================================================================
inline void CUnitManager::AddToHerosUnit(DWORD dwCurrStruct, DWORD dwCurrIndex)
{
	m_gHerosUnit[m_nHerosCount].id		= dwCurrStruct;
	m_gHerosUnit[m_nHerosCount].index	= dwCurrIndex;
	m_nHerosCount++;
}

//=================================================================
//  ��  ��: ��������Ϣ���浽��Ա����
//  ��  ע: ��
//  ��  ��: Flicker317
//  ��  ��: 1.0 2012/11/22
//=================================================================
inline void CUnitManager::AddToSentryWardsUnit(DWORD dwCurrStruct, DWORD dwCurrIndex)
{
	m_gSentryWardsUnit[m_nSentryWardsCount].id	  = dwCurrStruct;
	m_gSentryWardsUnit[m_nSentryWardsCount].index = dwCurrIndex;
	m_nSentryWardsCount++;
}
//=================================================================
//  ��  ��: ��������Ϣ���浽��Ա����
//  ��  ע: ��
//  ��  ��: Flicker317
//  ��  ��: 1.0 2012/11/22
//=================================================================
inline void CUnitManager::AddToObserverWardsUnit(DWORD dwCurrStruct, DWORD dwCurrIndex)
{
	m_gObserverWardsUnit[m_nObserverWardsCount].id	  = dwCurrStruct;
	m_gObserverWardsUnit[m_nObserverWardsCount].index = dwCurrIndex;
	m_nObserverWardsCount++;
}

//=================================================================
//  ��  ��: ������ը����Ϣ���浽��Ա����
//  ��  ע: ��
//  ��  ��: Flicker317
//  ��  ��: 1.0 2012/11/22
//=================================================================
inline void CUnitManager::AddToBombsUnit(DWORD dwCurrStruct, DWORD dwCurrIndex)
{
	m_gBombsUnit[m_nBombsCount].id	  = dwCurrStruct;
	m_gBombsUnit[m_nBombsCount].index = dwCurrIndex;
	m_nBombsCount++;
}

//=================================================================
//  ��  ��: ��ȡ��ǰӢ������
//  ��  ע: ����Ϸ��ʵʱ���ã���hookס��EndScene���ã�
//  ��  ��: Flicker317
//  ��  ��: 1.0 2012/11/19
//=================================================================
void CUnitManager::GetHerosLocation()
{
	// Ӧ������������� RefreshHerosId(factionType); 

	for (unsigned int i = 0; i < m_nHerosCount; i++)
	{			
		/*
		// ����ͨ����λ����ֵ�жϸõ�λ�Ƿ�����  ����ķ�����ֱ��ͨ���ڴ�ֵ�ж�
		if (GetUnitState(dwCurrStruct, UNIT_STATE_LIFE) == 0)
			continue;
		*/
		// RVA    ����	  ���
		// 064  00000000  00000F12
		// 0A8  00000000  ????????
		// 0B4  00000000  ????????
		// 194  00000004  00000000
		// 1C4  00000001  00000000
		// 25C  FFFFFFFF  000000??
		// 260  FFFFFFFF  000000??
		// �жϸõ�λ�Ƿ�����
		if (GetMemory(m_gHerosUnit[i].id + 0x64) == 0)
		{
			m_gHerosPoint[i].x = -10;
			m_gHerosPoint[i].y = -10;
		}
		else
		{
			m_gHerosPoint[i].x = *(float *)(m_gHerosUnit[i].id + 0x284);
			m_gHerosPoint[i].y = *(float *)(m_gHerosUnit[i].id + 0x288);

			m_gHerosPoint[i].x = ((7870 + m_gHerosPoint[i].x) * 0.0155f + 9) * m_fWidth;
			m_gHerosPoint[i].y = ((7910 - m_gHerosPoint[i].y) * 0.0115f + 572) * m_fHeight;
		}
	}
}

//=================================================================
//  ��  ��: ��ȡ���������
//  ��  ע: ����Ϸ��ʵʱ���ã���hookס��EndScene���ã�Ƶ�ʿ��Խ��ͣ�
//  ��  ��: Flicker317
//  ��  ��: 1.0 2012/11/22
//=================================================================
void CUnitManager::GetWardsLocation()
{
	for (unsigned int i = 0; i < m_nSentryWardsCount; i++)
	{
		m_gSentryWardsPoint[i].x = *(float *)(m_gSentryWardsUnit[i].id + 0x284);
		m_gSentryWardsPoint[i].y = *(float *)(m_gSentryWardsUnit[i].id + 0x288);

		m_gSentryWardsPoint[i].x = ((7870 + m_gSentryWardsPoint[i].x) * 0.0155f + 8) * m_fWidth;
		m_gSentryWardsPoint[i].y = ((7910 - m_gSentryWardsPoint[i].y) * 0.0115f + 572) * m_fHeight;
	}
	for (unsigned int i = 0; i < m_nObserverWardsCount; i++)
	{
		m_gObserverWardsPoint[i].x = *(float *)(m_gObserverWardsUnit[i].id + 0x284);
		m_gObserverWardsPoint[i].y = *(float *)(m_gObserverWardsUnit[i].id + 0x288);

		m_gObserverWardsPoint[i].x = ((7870 + m_gObserverWardsPoint[i].x) * 0.0155f + 8) * m_fWidth;
		m_gObserverWardsPoint[i].y = ((7910 - m_gObserverWardsPoint[i].y) * 0.0115f + 572) * m_fHeight;
	}

}

//=================================================================
//  ��  ��: ��ȡը������
//  ��  ע: ����Ϸ��ʵʱ���ã���hookס��EndScene���ã�Ƶ�ʿ��Խ��ͣ�
//  ��  ��: Flicker317
//  ��  ��: 1.0 2012/11/22
//=================================================================
void CUnitManager::GetBombsLocation()
{
	for (unsigned int i = 0; i < m_nBombsCount; i++)
	{
		m_gBombsPoint[i].x = *(float *)(m_gBombsUnit[i].id + 0x284);
		m_gBombsPoint[i].y = *(float *)(m_gBombsUnit[i].id + 0x288);

		m_gBombsPoint[i].x = ((7870 + m_gBombsPoint[i].x) * 0.0155f + 9) * m_fWidth;
		m_gBombsPoint[i].y = ((7910 - m_gBombsPoint[i].y) * 0.0115f + 572) * m_fHeight;
	}
}

//=================================================================
//  ��  ��: ��ȡ���ˢ����Ϣ
//  ��  ע: ����Ϸ��ʵʱ���ã���hookס��EndScene���ã�Ƶ�ʿ��Խ��ͣ�
//			Game.dll+0xACBA10  000000F8		�ṹ���С
//			Game.dll+0xACBA14  00000040		�����С
//			Game.dll+0xACBA18  00000???		������
//			Game.dll+0xACBA1C  ????????		��ά��̬����ָ��
//			Game.dll+0xACBA20  ????????		�����ٵĵ�λ����
//  		�ṹ��ͷΪ Game.dll+0x944214	��ʾδ������
//  ��  ��: Flicker317
//  ��  ��: 1.0 2012/11/22
//=================================================================
void CUnitManager::GetRuneType()
{
	std::vector<DWORD> itemList;

	DWORD dwArraySize;			// �����С
	DWORD dwStructSize;			// �ṹ���С
	DWORD dwStructCount;		// �ṹ������
	DWORD dwCurrStoreAddr;		// ��ǰ��������ĵ�ַ����������ʱ��
	DWORD dwCurrStruct;			// ��ǰ�����Ľṹ��ַ���� unit �� item �� id

	dwStructSize	= GetMemory(m_dwGameAddr + 0xACBA10);
	dwStructCount	= GetMemory(m_dwGameAddr + 0xACBA18);
	dwArraySize		= GetMemory(m_dwGameAddr + 0xACBA14);

	dwCurrStoreAddr = m_dwGameAddr + 0xACBA1C;

	while ((dwCurrStoreAddr = GetMemory(dwCurrStoreAddr)))
	{
		itemList.push_back(dwCurrStoreAddr);
	}

	for (unsigned int i = 0, totalCount = 0; i < itemList.size(); i++)
	{
		for (unsigned int j = 0; j < dwArraySize && totalCount < dwStructCount; j++)
		{
			dwCurrStruct = itemList[i] + 4 + j * dwStructSize;

			// �жϵ�λ�Ƿ�����
			if (GetMemory(dwCurrStruct) == m_dwGameAddr + 0x944214)
			{
				totalCount++;

				DWORD runeType = RUNE_TYPE_NONE;
				switch (GetMemory(dwCurrStruct + 0x30))
				{
				case 0x49303036:	// ����
					runeType = RUNE_TYPE_SPEEDUP;
					break;
				case 0x49303037:	// ����
					runeType = RUNE_TYPE_PHANTOM;
					break;
				case 0x49303038:	// �ָ�
					runeType = RUNE_TYPE_RECOVER;
					break;
				case 0x4930304A:	// ����
					runeType = RUNE_TYPE_INVISIBLE;
					break;
				case 0x4930304B:	// ˫��
					runeType = RUNE_TYPE_DOUBLE;
					break;
				default:
					continue;
				}

				// 0x58 != 0 && => 0x78 != 0xFFFFFFFF	��δ��
				if (GetMemory(dwCurrStruct + 0x58) != 0)
				{
					// itemx = [[itemid + 0x28] + 0x88]
					// itemy = [[itemid + 0x28] + 0x8C]
					if (GetMemory(GetMemory(dwCurrStruct + 0x28) + 0x88) == 0x453C0000)
					{
						// ˢ������·
                        SetRuneLower(runeType);
					}
                    else
                    {
                        // ˢ������·
                        SetRuneUpper(runeType);
                    }

					m_runeType = (RUNE_ENUM)runeType;

					return;
				}
			}
		}
	}

	m_runeType = RUNE_NONE;
}

/*������������������������������������������������������������������������������������������������������������������������������������
    ��������hook�е�������ĺ�����Ӧ�����û���������
*/

//=================================================================
//  ��  ��: ˢ�±���Ӣ����Ϣ������
//  ��  ע: ��������Ҫʱ���ã����û���������
//  ��  ��: Flicker317
//  ��  ��: 1.0 2012/11/19
//=================================================================
void CUnitManager::RefreshHerosId(FactionTypeId factionType)
{
    DWORD dwArraySize;			// �����С
    DWORD dwStructSize;			// �ṹ���С
    DWORD dwStructCount;		// �ṹ������
    DWORD dwCurrStruct;			// ��ǰ�����Ľṹ��ַ���� unit �� item �� id
    DWORD dwLocalIndex;			// �����������
    DWORD dwCurrIndex;			// ��ǰ��λ����

    dwStructSize	= GetMemory(m_dwGameAddr + 0xACB0D8);
    dwStructCount	= GetMemory(m_dwGameAddr + 0xACB0E0);
    dwArraySize		= GetMemory(m_dwGameAddr + 0xACB0DC);

    dwLocalIndex	= GetLocalPlayer();

    // ���
    m_nHerosCount = 0;

    for (unsigned int i = 0, totalCount = 0; i < m_idAddrList.size(); i++)
    {
        for (unsigned int j = 0; j < dwArraySize && totalCount < dwStructCount; j++)
        {
            dwCurrStruct = m_idAddrList[i] + 4 + j * dwStructSize;

            // �жϵ�λ�Ƿ�����
            if (GetMemory(dwCurrStruct) == m_dwGameAddr + 0x943A94)
            {
                totalCount++;

                // �ӵ�λtype�У���ȡ����ֽڵ��ַ�
                // ���ַ��ж��Ƿ���Ӣ��
                switch ((GetMemory(dwCurrStruct + 0x30) >> 24))
                {
                case 'O': case 'U': case 'H': case 'N': case 'E':
                    break;
                default:
                    continue;
                }

                // ��ȡӢ�۵��������
                dwCurrIndex = GetMemory(dwCurrStruct + 0x24);

                switch (factionType)
                {
                case FactionNone:
                    break;
                case FactionBoth:
                    {
                        if (m_nHerosCount < HERO_COUNT_MAX && 
                            (dwCurrIndex > 0 && dwCurrIndex < 6) || (dwCurrIndex > 6 && dwCurrIndex < 12))
                        {
                            AddToHerosUnit(dwCurrStruct, dwCurrIndex);
                        }
                    }
                    break;
                case FactionAutoMatch:
                    {
                        if (m_nHerosCount < HERO_COUNT_MAX && 
                            ((dwCurrIndex > 0 && dwCurrIndex < 6) && (dwLocalIndex > 6 && dwLocalIndex < 12)) || 
                            ((dwLocalIndex > 0 && dwLocalIndex < 6) && (dwCurrIndex > 6 && dwCurrIndex < 12)))
                        {
                            AddToHerosUnit(dwCurrStruct, dwCurrIndex);
                        }
                    }
                    break;
                case FactionSentinel:
                    {
                        if (m_nHerosCount < HERO_COUNT_MAX && dwCurrIndex > 0 && dwCurrIndex < 6)
                        {
                            AddToHerosUnit(dwCurrStruct, dwCurrIndex);
                        }
                    }
                    break;
                case FactionScourge:
                    {
                        if (m_nHerosCount < HERO_COUNT_MAX && dwCurrIndex > 6 && dwCurrIndex < 12)
                        {
                            AddToHerosUnit(dwCurrStruct, dwCurrIndex);
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }
}

//=================================================================
//  ��  ��: ˢ�±���ը����Ϣ������
//  ��  ע: ����Ϸ��ʵʱ���ã���hookס��EndScene���ã�Ƶ�ʿ��Խ��ͣ�
//  ��  ��: Flicker317
//  ��  ��: 1.0 2012/11/22
//=================================================================
void CUnitManager::RefreshBombsId(FactionTypeId factionType)
{
    DWORD dwArraySize;			// �����С
    DWORD dwStructSize;			// �ṹ���С
    DWORD dwStructCount;		// �ṹ������
    DWORD dwCurrStruct;			// ��ǰ�����Ľṹ��ַ���� unit �� item �� id
    DWORD dwLocalIndex;			// �����������
    DWORD dwCurrIndex;			// ��ǰ��λ����

    dwStructSize	= GetMemory(m_dwGameAddr + 0xACB0D8);
    dwStructCount	= GetMemory(m_dwGameAddr + 0xACB0E0);
    dwArraySize		= GetMemory(m_dwGameAddr + 0xACB0DC);

    dwLocalIndex	= GetLocalPlayer();

    // ���
    m_nBombsCount	= 0;

    for (unsigned int i = 0, totalCount = 0; i < m_idAddrList.size(); i++)
    {
        for (unsigned int j = 0; j < dwArraySize && totalCount < dwStructCount; j++)
        {
            dwCurrStruct = m_idAddrList[i] + 4 + j * dwStructSize;

            // �жϵ�λ�Ƿ�����
            if (GetMemory(dwCurrStruct) == m_dwGameAddr + 0x943A94)
            {
                totalCount++;

                // ��λ��ʧ��
                // +08  00000000
                // +0C  FFFFFFFF
                // +10  FFFFFFFF
                if (GetMemory(dwCurrStruct + 0x8) == 0)
                    continue;

                // ��ȡӢ�۵��������
                dwCurrIndex = GetMemory(dwCurrStruct + 0x24);

                switch(GetMemory(dwCurrStruct + 0x30))
                {
                // ը��
                // 1����
                // 6E30304F		1��
                // 6E303050		2��
                // 6E303051		3��
                // 6E30304E		4��
                // 
                // 2����
                // 6F746F74  	1��-4��
                // 
                // 4����
                // 6F303138		1��
                // 6F303032		2��
                // 6F303042		3��
                // 6F303142		A��
                case 0x6E30304F:
                case 0x6E303050:
                case 0x6E303051:
                case 0x6E30304E:
                case 0x6F746F74:
                case 0x6F303138:
                case 0x6F303032:
                case 0x6F303042:
                case 0x6F303142:
                    {
                        switch (factionType)
                        {
                        case FactionNone:
                            break;
                        case FactionBoth:
                            {
                                if (m_nBombsCount < BOMB_COUNT_MAX && 
                                    (dwCurrIndex > 0 && dwCurrIndex < 6) || (dwCurrIndex > 6 && dwCurrIndex < 12))
                                {
                                    AddToBombsUnit(dwCurrStruct, dwCurrIndex);
                                }
                            }
                            break;
                        case FactionAutoMatch:
                            {
                                if (m_nBombsCount < BOMB_COUNT_MAX && 
                                    ((dwCurrIndex > 0 && dwCurrIndex < 6) && (dwLocalIndex > 6 && dwLocalIndex < 12)) || 
                                    ((dwLocalIndex > 0 && dwLocalIndex < 6) && (dwCurrIndex > 6 && dwCurrIndex < 12)))
                                {
                                    AddToBombsUnit(dwCurrStruct, dwCurrIndex);
                                }
                            }
                            break;
                        case FactionSentinel:
                            {
                                if (m_nBombsCount < BOMB_COUNT_MAX && dwCurrIndex > 0 && dwCurrIndex < 6)
                                {
                                    AddToBombsUnit(dwCurrStruct, dwCurrIndex);
                                }
                            }
                            break;
                        case FactionScourge:
                            {
                                if (m_nBombsCount < BOMB_COUNT_MAX && dwCurrIndex > 6 && dwCurrIndex < 12)
                                {
                                    AddToBombsUnit(dwCurrStruct, dwCurrIndex);
                                }
                            }
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }
}

//=================================================================
//  ��  ��: ˢ�±����������Ϣ������
//  ��  ע: ����Ϸ��ʵʱ���ã���hookס��EndScene���ã�Ƶ�ʿ��Խ��ͣ�
//  ��  ��: Flicker317
//  ��  ��: 1.0 2012/11/19
//=================================================================
void CUnitManager::RefreshWardsId(FactionTypeId factionType)
{
    DWORD dwArraySize;			// �����С
    DWORD dwStructSize;			// �ṹ���С
    DWORD dwStructCount;		// �ṹ������
    DWORD dwCurrStruct;			// ��ǰ�����Ľṹ��ַ���� unit �� item �� id
    DWORD dwLocalIndex;			// �����������
    DWORD dwCurrIndex;			// ��ǰ��λ����

    dwStructSize	= GetMemory(m_dwGameAddr + 0xACB0D8);
    dwStructCount	= GetMemory(m_dwGameAddr + 0xACB0E0);
    dwArraySize		= GetMemory(m_dwGameAddr + 0xACB0DC);

    dwLocalIndex	= GetLocalPlayer();

    // ���
    m_nSentryWardsCount	  = 0;
    m_nObserverWardsCount = 0;

    for (unsigned int i = 0, totalCount = 0; i < m_idAddrList.size(); i++)
    {
        for (unsigned int j = 0; j < dwArraySize && totalCount < dwStructCount; j++)
        {
            dwCurrStruct = m_idAddrList[i] + 4 + j * dwStructSize;

            // �жϵ�λ�Ƿ�����
            if (GetMemory(dwCurrStruct) == m_dwGameAddr + 0x943A94)
            {
                totalCount++;

                // ��λ��ʧ��
                // +08  00000000
                // +0C  FFFFFFFF
                // +10  FFFFFFFF
                if (GetMemory(dwCurrStruct + 0x8) == 0)
                    continue;

                // ��ȡӢ�۵��������
                dwCurrIndex = GetMemory(dwCurrStruct + 0x24);

                switch(GetMemory(dwCurrStruct + 0x30))
                {
                case 0x6F657965:	// ����
                    {
                        switch (factionType)
                        {
                        case FactionNone:
                            break;
                        case FactionBoth:
                            {
                                if (m_nObserverWardsCount < WARD_COUNT_MAX && 
                                    (dwCurrIndex > 0 && dwCurrIndex < 6) || (dwCurrIndex > 6 && dwCurrIndex < 12))
                                {
                                    AddToObserverWardsUnit(dwCurrStruct, dwCurrIndex);
                                }
                            }
                            break;
                        case FactionAutoMatch:
                            {
                                if (m_nObserverWardsCount < WARD_COUNT_MAX && 
                                    ((dwCurrIndex > 0 && dwCurrIndex < 6) && (dwLocalIndex > 6 && dwLocalIndex < 12)) || 
                                    ((dwLocalIndex > 0 && dwLocalIndex < 6) && (dwCurrIndex > 6 && dwCurrIndex < 12)))
                                {
                                    AddToObserverWardsUnit(dwCurrStruct, dwCurrIndex);
                                }
                            }
                            break;
                        case FactionSentinel:
                            {
                                if (m_nObserverWardsCount < WARD_COUNT_MAX && dwCurrIndex > 0 && dwCurrIndex < 6)
                                {
                                    AddToObserverWardsUnit(dwCurrStruct, dwCurrIndex);
                                }
                            }
                            break;
                        case FactionScourge:
                            {
                                if (m_nObserverWardsCount < WARD_COUNT_MAX && dwCurrIndex > 6 && dwCurrIndex < 12)
                                {
                                    AddToObserverWardsUnit(dwCurrStruct, dwCurrIndex);
                                }
                            }
                            break;
                        }
                    }
                    break;
                case 0x6F303034:	// ����
                    {
                        switch (factionType)
                        {
                        case FactionNone:
                            break;
                        case FactionBoth:
                            {
                                if (m_nSentryWardsCount < WARD_COUNT_MAX && 
                                    (dwCurrIndex > 0 && dwCurrIndex < 6) || (dwCurrIndex > 6 && dwCurrIndex < 12))
                                {
                                    AddToSentryWardsUnit(dwCurrStruct, dwCurrIndex);
                                }
                            }
                            break;
                        case FactionAutoMatch:
                            {
                                if (m_nSentryWardsCount < WARD_COUNT_MAX && 
                                    ((dwCurrIndex > 0 && dwCurrIndex < 6) && (dwLocalIndex > 6 && dwLocalIndex < 12)) || 
                                    ((dwLocalIndex > 0 && dwLocalIndex < 6) && (dwCurrIndex > 6 && dwCurrIndex < 12)))
                                {
                                    AddToSentryWardsUnit(dwCurrStruct, dwCurrIndex);
                                }
                            }
                            break;
                        case FactionSentinel:
                            {
                                if (m_nSentryWardsCount < WARD_COUNT_MAX && dwCurrIndex > 0 && dwCurrIndex < 6)
                                {
                                    AddToSentryWardsUnit(dwCurrStruct, dwCurrIndex);
                                }
                            }
                            break;
                        case FactionScourge:
                            {
                                if (m_nSentryWardsCount < WARD_COUNT_MAX && dwCurrIndex > 6 && dwCurrIndex < 12)
                                {
                                    AddToSentryWardsUnit(dwCurrStruct, dwCurrIndex);
                                }
                            }
                            break;
                        }
                    }
                    break;
                }

            }
        }
    }
}
