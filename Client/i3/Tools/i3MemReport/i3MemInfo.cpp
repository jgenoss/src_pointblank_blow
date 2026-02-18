#include "stdafx.h"
#include "i3MemInfo.h"

static i3BinList		s_List;

static INT32		_CompProc( MemInfo * p1, MemInfo * p2)
{
	return strcmp( p1->m_szPath, p2->m_szPath);
}

static INT32	_CompareProcByCount( MemInfo * p1, MemInfo * p2)
{
	return p2->m_Count - p1->m_Count;
}

static INT32	_CompareProcBySize( MemInfo * p1, MemInfo * p2)
{
	return p2->m_Size - p1->m_Size;
}

void		MemInfo::SortByName()
{
	s_List.Sort( (COMPAREPROC)_CompProc);
}

void		MemInfo::SortByCount()
{
	s_List.Sort( (COMPAREPROC)_CompareProcByCount);
}

void		MemInfo::SortBySize()
{
	s_List.Sort( (COMPAREPROC)_CompareProcBySize);
}

void		MemInfo::DeleteAllInfo(void)
{
	INT32 i;
	MemInfo * pInfo;

	for( i = 0; i < s_List.GetCount(); i++)
	{
		pInfo = (MemInfo *) s_List.Items[i];

		delete pInfo;
	}

	s_List.Clear();
}

void		MemInfo::CalcCRC(void)
{
	INT32 len = strlen( m_szPath);

	m_CRC = CRC32( 0xFFFFFFFF, (UINT8 *) m_szPath, len);
	m_CRC = CRC32( m_CRC, (UINT8 *) &m_Line, 4);

	//I3TRACE( "%08X %s %d\n", m_CRC, m_szPath, m_Line);
}

INT32		MemInfo::GetCount(void)
{
	return s_List.GetCount();
}

MemInfo *	MemInfo::GetInfo( INT32 idx)
{
	return (MemInfo *) s_List.Items[idx];
}

INT32		MemInfo::Find( UINT32 crc)
{
	INT32 i;
	MemInfo * pInfo;

	for( i = 0; i < s_List.GetCount(); i++)
	{
		pInfo = (MemInfo *) s_List.Items[i];

		if( pInfo->m_CRC == crc)
			return i;
	}

	return -1;
}

void		MemInfo::AddInfo( MemInfo * pInfo)
{
	INT32 idx;
	MemInfo * pTarget;

	s_List.SetCompareProc( (COMPAREPROC) _CompProc);

	pInfo->CalcCRC();

	idx = Find( pInfo->m_CRC);

	//I3TRACE( "[%s] %d %0X : %d\n", pInfo->m_szPath, pInfo->m_Line, pInfo->m_CRC, idx);

	if( idx != -1)
	{
		pTarget = (MemInfo *) s_List.Items[idx];

		pTarget->m_Count++;
		pTarget->m_Size += pInfo->m_Size;
	}
	else
	{
		pTarget = new MemInfo;

		memcpy( pTarget, pInfo, sizeof(MemInfo));
		pTarget->m_Count = 1;

		s_List.Add( pTarget);
	}
}
