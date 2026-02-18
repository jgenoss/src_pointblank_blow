#include "pch.h"
#include "i3BinListEx.h"

static INT32 _CompareProcMember(void* p1,void* p2)
{
	return *((INT32*)p1) - *((INT32*)p2);
}

I3_CLASS_INSTANCE( i3BinListEx, i3BinList );

i3BinListEx::i3BinListEx()
{
	SetCompareProc( _CompareProcMember );
}

i3BinListEx::~i3BinListEx()
{
}

void*	i3BinListEx::FindItemEx( UINT32 ui32Item )
{
	INT32 i32FindIdx = i3BinList::FindItem( &ui32Item );
	if( 0 > i32FindIdx ) return NULL;
	return i3List::GetItem( i32FindIdx );
}

void*	i3BinListEx::DeleteEx( UINT32 ui32Item )
{
	INT32 i32FindIdx = i3BinList::FindItem( &ui32Item );
	if( 0 > i32FindIdx ) return NULL;
	void* pData = i3List::GetItem( i32FindIdx );
	//I3_SAFE_DELETE( pData );
	i3List::Delete( i32FindIdx );
	return pData;
}


static INT32 _CompareProcMember64(void* p1,void* p2)
{
	if( *((INT64*)p1) > *((INT64*)p2))			return 1;
	else if( *((INT64*)p1) < *((INT64*)p2))		return -1;
	return 0;	
}

I3_CLASS_INSTANCE( i3BinListEx64, i3BinList );

i3BinListEx64::i3BinListEx64()
{
	SetCompareProc( _CompareProcMember64 );
}

i3BinListEx64::~i3BinListEx64()
{
}

void*	i3BinListEx64::FindItemEx( INT64 i64Item )
{
	INT32 i32FindIdx = i3BinList::FindItem( &i64Item );
	if( 0 > i32FindIdx ) return NULL;
	return i3List::GetItem( i32FindIdx );
}

void*	i3BinListEx64::DeleteEx( INT64 i64Item )
{
	INT32 i32FindIdx = i3BinList::FindItem( &i64Item );
	if( 0 > i32FindIdx ) return NULL;
	void* pData = i3List::GetItem( i32FindIdx );
	//I3_SAFE_DELETE( pData );
	i3List::Delete( i32FindIdx );
	return pData;
}