#include "pch.h"
#include "HashTable.h"

CHashTable::CHashTable()
{
	m_ppBinList		= NULL;
	m_ui32KeyCount	= 0;
}

CHashTable::~CHashTable()
{
	Destroy();
}

BOOL CHashTable::Create( UINT32 ui32KeyCount, COMPAREPROC pCmpProc )
{
	m_ppBinList = new i3BinList*[ ui32KeyCount ];
	if( NULL == m_ppBinList )
	{
		return FALSE;
	}
	
	for( UINT32 i = 0; i < ui32KeyCount; i++ )
	{
		m_ppBinList[i] = i3BinList::NewObject();
		if( NULL == m_ppBinList[i] )
		{
			return FALSE;
		}
		m_ppBinList[i]->SetCompareProc( pCmpProc );
	}
	
	m_ui32KeyCount = ui32KeyCount;

	return TRUE;
}

void CHashTable::Destroy()
{
	if( m_ppBinList )
	{
		for( UINT32 i = 0; i < m_ui32KeyCount ; i++ )
		{
			I3_SAFE_RELEASE( m_ppBinList[i] );
		}
		I3_SAFE_DELETE_ARRAY( m_ppBinList );
	}
}

BOOL CHashTable::Insert( T_UID TUID, void* p )
{
	UINT32 ui32Idx = TUID % m_ui32KeyCount;
	
	if (NULL == m_ppBinList[ui32Idx])
	{
		return FALSE; 
	}
	else
	{
		if (-1 ==  m_ppBinList[ui32Idx]->Add(p))
		{
			return FALSE;
		}
	}
	
	return TRUE;
	
}

void CHashTable::Delete( T_UID TUID, void* p )
{
	UINT32 ui32Idx = TUID % m_ui32KeyCount;
	INT32 i32FindIdx = m_ppBinList[ ui32Idx ]->FindItem( p );

	if( 0 <= i32FindIdx )
	{
		m_ppBinList[ ui32Idx ]->Delete( i32FindIdx );
	}

	return; 
}

i3BinList* CHashTable::Retrieve( T_UID TUID )
{
	UINT32 ui32Idx = TUID % m_ui32KeyCount;
	return m_ppBinList[ ui32Idx ];
}

#if defined BUILDMODE_DLG
void CHashTable::DumpCount(void)
{
	if ( NULL == m_ppBinList )
		return;

	INT32 i32Max = 0;
	INT32 i32Min = -1;

	UINT32 ui32OverflowCount = 0;

	for( UINT32 i = 0; i < m_ui32KeyCount; i++ )
	{
		UINT32 ui32Count = m_ppBinList[i]->GetCount();

		i32Min = MIN( i32Min, (signed)ui32Count );
		i32Max = MAX( i32Max, (signed)ui32Count );

		if ( 16 < ui32Count )
		{
			ui32OverflowCount++;
		}
	}

	//I3TRACE("MIN = %d, MAX = %d, OverflowCount = %d\n", nMin, nMax, nOverflowCount);
}
#endif
