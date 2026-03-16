#include "i3CommonType.h"
#include "i3BinList.h"
#include "i3Macro.h"

I3_CLASS_INSTANCE( i3BinList, i3List);

static INT32 _DefaultPointerCompareProc( void * p1, void * p2)
{
	return (INT32)((char *)p1 - (char *)p2);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Binary Search를 위한 리스트
i3BinList::i3BinList( INT32 allocunit)
: i3List( allocunit)
{
	m_pCompProc = _DefaultPointerCompareProc;

#if defined( _i3_DEBUGOBJECT_)
	SetObjectTypeName( "BinList");
#endif
}

i3BinList::~i3BinList(void)
{
}

INT32 i3BinList::GetAddPosition( void * p) const
{
	INT32 idx, startidx, endidx;

	if( m_Count == 0)
		return -1;

	if( m_pCompProc( p, Items[0]) <= 0)
		return 0;

	if( m_pCompProc( p,Items[m_Count-1]) >= 0)
		return -1;

	startidx = 0;
	endidx = m_Count - 1;
	idx = (startidx + endidx) >> 1;

	do {
		if( m_pCompProc( Items[idx], p) > 0) 
			endidx = idx - 1;
		else 
			startidx = idx + 1;	
		
		idx = (startidx + endidx) >> 1;
	} while( startidx < endidx);

	if( m_pCompProc( Items[idx], p) < 0)
		idx++;

	return idx;
}

INT32 i3BinList::Add( void * p)
{
	INT32 insertidx, idx = -1;

	insertidx = GetAddPosition( p);

	if( insertidx == -1) {
		idx = i3List::Add( p);
	}
	else {
		idx = i3List::Insert( insertidx, p);
	}

	return idx;
}

INT32 i3BinList::FindItem( void * p)
{
	INT32 start, end;
	INT32 idx, rv;

	if( m_Count <= 0)
		return -1;
		
	start = 0;
	end = m_Count - 1;

	while( start < end)
	{
		idx = (start + end) >> 1;
		
		rv = m_pCompProc( Items[idx], p);
		
		if( rv == 0)
			return idx;
		 
		if( rv > 0)
			end = idx - 1;
		else
			start = idx + 1;
	}

	if( start <= end)
	{	
		idx = (start + end) >> 1;
		
		if( m_pCompProc( Items[idx], p) == 0)
			return idx;
	}
	
	return -1;
}
