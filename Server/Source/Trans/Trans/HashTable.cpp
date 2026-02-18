#include "pch.h"
#include "HashTable.h"

I3_CLASS_INSTANCE(CHashTable, i3ElementBase);

CHashTable::CHashTable()
{	
	m_ppBinList = NULL;
	m_n32KeyCount = 0;
}

CHashTable::~CHashTable()
{
	Destroy();
}

bool CHashTable::Create( UINT32 nKeyCount, COMPAREPROC pCmpProc )
{
	m_ppBinList = (i3BinList**)i3MemAlloc(sizeof(i3BinList*) * nKeyCount);
	if( NULL == m_ppBinList )			return false;
	
	for(UINT32 i = 0; i < nKeyCount; i++)
	{
		m_ppBinList[i] = i3BinList::NewObject();
		if( NULL == m_ppBinList[i] )	return false;
		m_ppBinList[i]->SetCompareProc( pCmpProc );
	}
	
	m_n32KeyCount = nKeyCount;

	return true; 
}

void CHashTable::Destroy()
{
	if( m_ppBinList )
	{
		for( UINT32 i = 0; i < m_n32KeyCount ; i++ )
		{
			I3_SAFE_RELEASE( m_ppBinList[i] );
		}
		I3MEM_SAFE_FREE(m_ppBinList);
	}
}

BOOL CHashTable::Insert( T_UID TUID, USER_NODE* pNode )
{
	I3ASSERT(m_ppBinList);
	UINT32 ui32Idx = TUID % m_n32KeyCount;

	if (NULL == m_ppBinList[ui32Idx])
	{
		I3BCLOG(LOCATION, "[%02d%02d%02d][%02d:%02d:%02d]", i3ThreadTimer::LocalTime().GetYear(), i3ThreadTimer::LocalTime().GetMonth(), i3ThreadTimer::LocalTime().GetDay(), i3ThreadTimer::LocalTime().GetHour(), i3ThreadTimer::LocalTime().GetMinute(), i3ThreadTimer::LocalTime().GetSecond() );
		I3BCLOG(LOCATION, "[ERROR] CHashTable::Insert() - BinList Alloc Null [nkey(%I64d),idx(%d)]", TUID, ui32Idx);		
		return FALSE; 
	}
	else
	{
		if (-1 ==  m_ppBinList[ui32Idx]->Add(pNode))
		{
			I3BCLOG(LOCATION, "[%02d%02d%02d][%02d:%02d:%02d]", i3ThreadTimer::LocalTime().GetYear(), i3ThreadTimer::LocalTime().GetMonth(), i3ThreadTimer::LocalTime().GetDay(), i3ThreadTimer::LocalTime().GetHour(), i3ThreadTimer::LocalTime().GetMinute(), i3ThreadTimer::LocalTime().GetSecond() );
			I3BCLOG(LOCATION, "[ERROR] CHashTable::Insert() - BinList Alloc Fail [nkey(%I64d),idx(%d)]", TUID, ui32Idx);
			return FALSE;
		}
	}

	return TRUE; 
	
}

void CHashTable::Delete( T_UID TUID, USER_NODE* pNode )
{
	I3ASSERT(m_ppBinList);

	UINT32	ui32Idx = TUID % m_n32KeyCount;
	INT32	i32FindIdx = m_ppBinList[ ui32Idx ]->FindItem( pNode );

	if( 0 <= i32FindIdx )
	{
		m_ppBinList[ ui32Idx ]->Delete( i32FindIdx );
	}
	else
	{		
		I3TRACE("[ERROR] CHashTable::Delete() - Not find item, DataBase Idx(%d)", pNode->_i64UID);
	}

	return; 
}

i3BinList* CHashTable::Retrieve( T_UID TUID )
{
	I3ASSERT(m_ppBinList);
	UINT32 ui32Idx = TUID % m_n32KeyCount;
	return m_ppBinList[ ui32Idx ];
}

UINT32 CHashTable::MakeKeyForString(const TTCHAR* pstrValue)
{	
	UINT32 ui32key = 0;
	UINT32 ui32Temp = 0;

	while( '\0' != *pstrValue )
	{
		ui32Temp = *pstrValue;
		if( 'A' <= *pstrValue && 'Z' >= *pstrValue ) //대문자를 소문자 값으로 변경
			ui32Temp += 32;

		ui32key = ui32Temp + (ui32key << 6) + (ui32key << 16) - ui32key;
		pstrValue++;
	}

	return ui32key;
}

#if defined I3_DEBUG
void CHashTable::DumpCount(void)
{
	if (NULL == m_ppBinList)return;

	INT32 nMax = 0;
	INT32 nMin = -1;

	UINT32 nOverflowCount = 0;

	for(UINT32 i = 0; i < m_n32KeyCount; i++)
	{
		UINT32 nCount = m_ppBinList[i]->GetCount();

		nMin = MIN(nMin, (signed)nCount);
		nMax = MAX(nMax, (signed)nCount);

		if (16 < nCount)
		{
			nOverflowCount++;
		}
	}

	I3TRACE("MIN = %d, MAX = %d, OverflowCount = %d\n", nMin, nMax, nOverflowCount);
	return; 
}
#endif
