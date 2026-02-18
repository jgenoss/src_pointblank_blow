#include "pch.h"
#include "HashTable.h"

I3_CLASS_INSTANCE(CHashTable, i3ElementBase);

CHashTable::CHashTable()
{	
	m_ppBinList = NULL;
	m_nKeyCount = 0;
}

CHashTable::~CHashTable()
{
	if (m_ppBinList)
	{
		for(UINT32 i = 0; i < m_nKeyCount; i++)
		{
			I3_SAFE_RELEASE(m_ppBinList[i]);
		}

		I3MEM_SAFE_FREE(m_ppBinList);
	}	
}

void CHashTable::Create(UINT32 ui32KeyCount, COMPAREPROC pCmpProc)
{
	I3ASSERT(NULL == m_ppBinList);
	I3ASSERT(ui32KeyCount);
	I3ASSERT(pCmpProc);
	
	m_ppBinList = (i3BinList**)i3MemAlloc(sizeof(i3BinList*) * ui32KeyCount);
	
	for(UINT32 i = 0; i < ui32KeyCount; i++)
	{
		m_ppBinList[i] = i3BinList::NewObject();
		m_ppBinList[i]->SetOptmizeEnableWhenClear(FALSE);
		m_ppBinList[i]->SetAllocationUnit( 64 ); 
		m_ppBinList[i]->SetCompareProc(pCmpProc);
	}

	m_nKeyCount = ui32KeyCount;

	return; 
}

BOOL CHashTable::Insert(UINT32 ui32Key, USER_NODE* pNode)
{
	I3ASSERT(m_ppBinList);
	UINT32 ui32idx = ui32Key % m_nKeyCount;
	//idx = 0;
	if (NULL == m_ppBinList[ui32idx])
	{
		I3BCLOG(LOCATION, "[%02d%02d%02d][%02d:%02d:%02d]",
			i3ThreadTimer::LocalTime().GetYear(),
			i3ThreadTimer::LocalTime().GetMonth(),
			i3ThreadTimer::LocalTime().GetDay(),
			i3ThreadTimer::LocalTime().GetHour(),
			i3ThreadTimer::LocalTime().GetMinute(),
			i3ThreadTimer::LocalTime().GetSecond() );
		I3BCLOG(LOCATION, "[ERROR] CHashTable::Insert() - BinList Alloc Null [nkey(%d),idx(%d)]", ui32Key, ui32idx);		
		return FALSE; 
	}
	else
	{
		if (-1 ==  m_ppBinList[ui32idx]->Add(pNode))
		{
			I3BCLOG(LOCATION, "[%02d%02d%02d][%02d:%02d:%02d]",
				i3ThreadTimer::LocalTime().GetYear(),
				i3ThreadTimer::LocalTime().GetMonth(),
				i3ThreadTimer::LocalTime().GetDay(),
				i3ThreadTimer::LocalTime().GetHour(),
				i3ThreadTimer::LocalTime().GetMinute(),
				i3ThreadTimer::LocalTime().GetSecond() );
			I3BCLOG(LOCATION, "[ERROR] CHashTable::Insert() - BinList Alloc Fail [nkey(%d),idx(%d)]", ui32Key, ui32idx);
			return FALSE;
		}
	}

	return TRUE; 
}

void CHashTable::Delete(UINT32 ui32Key, USER_NODE* pNode)
{
	I3ASSERT(m_ppBinList);

	//INT32	Loop = 0;
	UINT32	ui32idx = ui32Key % m_nKeyCount;
	//idx = 0;
	INT32	i32idxFind = m_ppBinList[ui32idx]->FindItem(pNode);

	if (0 <= i32idxFind)
	{
		m_ppBinList[ui32idx]->Delete(i32idxFind);
	}
	else
	{
		// 닉네임 변경된 유저의 경우 들어옴.
		// 해당 유저의 UID를 이용해 이전 닉네임을 삭제하여야 함.
		/*for(Loop = 0; Loop < MAX_CONNECT_USER_DIVISOR; Loop++)
		{
			idxFind = m_ppBinList[idx]->FindItem(pNode);
			if (0 <= idxFind)
			{
				m_ppBinList[idx]->Delete(idxFind);
				return;
			}
		}

		I3BCLOG(LOCATION, "[%02d%02d%02d][%02d:%02d:%02d]", g_pConfig->m_Year, g_pConfig->m_Month, g_pConfig->m_Day, g_pConfig->m_Hour, g_pConfig->m_Minute, g_pConfig->m_Second);		
		I3BCLOG(LOCATION, "[ERROR] CHashTable::Delete() - BinList Alloc Fail[idxFind(%d), idx(%d), nkey(%d), m_nKeyCount(%d), UID(%I64d)]", idxFind, idx, nKey, m_nKeyCount, pNode->_i64DbIdx);*/
	}

	return; 
}

i3BinList* CHashTable::Retrieve(UINT32 ui32Key)
{
	I3ASSERT(m_ppBinList);
	UINT32 ui32idx = ui32Key % m_nKeyCount;
	//idx = 0;
	return m_ppBinList[ui32idx];
}

UINT32 CHashTable::MakeKeyForINT(INT64 i64Value)
{
	UINT32 ui32key = (UINT32)i64Value;
	return ui32key;
}

UINT32 CHashTable::MakeKeyForString(const TTCHAR* pstrValue)
{	
	UINT32 ui32key = 0;
	UINT32 ui32Length = i3String::Length(pstrValue);
	UINT32 ui32Temp = 0;
	// SDBM hash function
	for(UINT32 i = 0; i < ui32Length; i++)
	{
		ui32Temp = pstrValue[i];
		if( 'A' <= pstrValue[i] && 'Z' >= pstrValue[i] ) //대문자를 소문자 값으로 변경
			ui32Temp += 32;
		ui32key = ui32Temp + (ui32key << 6) + (ui32key << 16) - ui32key;
	}

	return ui32key;
}

UINT32 CHashTable::MakeKeyForStream(const UINT8* pui8stream, INT32 i32streamSize)
{
	UINT32 ui32key = 0;	

	// SDBM hash function
	for(INT32 i = 0; i < i32streamSize; i++)
	{
		ui32key = pui8stream[i] + (ui32key << 6) + (ui32key << 16) - ui32key;
	}

	return ui32key;
}

#if defined I3_DEBUG
void CHashTable::DumpCount(void)
{
	if (NULL == m_ppBinList)
	{
//		I3PRINTF("HashTable is NULL\n");
		return;
	}

	INT32 i32Max = 0;
	INT32 i32Min = -1;

	UINT32 ui32OverflowCount = 0;

	for(UINT32 i = 0; i < m_nKeyCount; i++)
	{
		UINT32 ui32Count = m_ppBinList[i]->GetCount();

		i32Min = MIN(i32Min, (signed)ui32Count);
		i32Max = MAX(i32Max, (signed)ui32Count);

		if (16 < ui32Count)
		{
			ui32OverflowCount++;
		}
	}

//	I3PRINTF("MIN = %d, MAX = %d, OverflowCount = %d\n", nMin, nMax, nOverflowCount);
	return; 
}
#endif




	//// 32 bit Mix Functions
	//key = ~key + (key << 15);		// key = (key << 15) - key - 1;
	//key = key ^ (key >> 12);
	//key = key + (key << 2);
	//key = key ^ (key >> 4);
	//key = key * 2057;				// key = (key + (key << 3)) + (key << 11);
	//key = key ^ (key >> 16);

	//// Robert Jenkins' 32 bit integer hash function
	//key = (key+0x7ed55d16) + (key<<12);
	//key = (key^0xc761c23c) ^ (key>>19);
	//key = (key+0x165667b1) + (key<<5);
	//key = (key+0xd3a2646c) ^ (key<<9);
	//key = (key+0xfd7046c5) + (key<<3);
	//key = (key^0xb55a4f09) ^ (key>>16);
