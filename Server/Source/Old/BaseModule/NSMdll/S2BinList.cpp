#include "stdafx.h"
#include "S2BinList.h"

static INT32 _DefaultPointerCompareProc( void* p1, void* p2 )
{
	return (INT32)((char*)p1 - (char*)p2);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Binary Search를 위한 리스트
S2BinList::S2BinList( INT32 i32Allocunit ) : S2List( i32Allocunit )
{
	m_pCompProc = _DefaultPointerCompareProc;

#if defined( _i3_DEBUGOBJECT_)
	SetObjectTypeName( "BinList");
#endif
}

S2BinList::~S2BinList(void)
{
}

INT32 S2BinList::GetAddPosition( void* p ) const
{
	INT32 i32Idx, i32Startidx, i32Endidx;
	
	if( m_i32Count == 0)
		return -1;
	
	if( m_pCompProc( p, Items[0]) <= 0 )
		return 0;
	
	if( m_pCompProc( p,Items[m_i32Count-1] ) >= 0 )
		return -1;
	
	i32Startidx = 0;
	i32Endidx = m_i32Count - 1;
	i32Idx = (i32Startidx + i32Endidx) >> 1;
	
	do {
		if( m_pCompProc( Items[i32Idx], p ) > 0 )
			i32Endidx = i32Idx - 1;
		else 
			i32Startidx = i32Idx + 1;
		
		i32Idx = (i32Startidx + i32Endidx) >> 1;
	} while( i32Startidx < i32Endidx );

	if( m_pCompProc( Items[i32Idx], p ) < 0 )
		i32Idx++;

	return i32Idx;
}

INT32 S2BinList::Add( void* p )
{
	INT32 i32Insertidx, i32Idx = -1;

	i32Insertidx = GetAddPosition( p );

	if( -1 == i32Insertidx )
	{
		i32Idx = S2List::Add( p );
	}
	else
	{
		i32Idx = S2List::Insert( i32Insertidx, p );
	}

	return i32Idx;
}

INT32 S2BinList::FindItem( void* p )
{
	INT32 i32Start, i32End;
	INT32 i32Idx, i32Rv;

	if( m_i32Count <= 0 )
		return -1;
	
	i32Start = 0;
	i32End = m_i32Count - 1;

	while( i32Start < i32End )
	{
		i32Idx = (i32Start + i32End) >> 1;
		
		i32Rv = m_pCompProc( Items[i32Idx], p );
		
		if( 0 == i32Rv )
			return i32Idx;
		 
		if( i32Rv > 0 )
			i32End = i32Idx - 1;
		else
			i32Start = i32Idx + 1;
	}

	if( i32Start <= i32End )
	{
		i32Idx = (i32Start + i32End) >> 1;
		
		if( 0 == m_pCompProc( Items[i32Idx], p ) )
			return i32Idx;
	}
	
	return -1;
}
