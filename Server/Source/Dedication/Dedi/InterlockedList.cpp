#include "pch.h"
#include <malloc.h>
#include "LogFile.h"
#include "InterlockedList.h"

CInterlockedList* g_pPhysicsResult = NULL;

CInterlockedList::CInterlockedList()
{ 
	m_pListHead = (PSLIST_HEADER)_aligned_malloc( sizeof(SLIST_HEADER), MEMORY_ALLOCATION_ALIGNMENT);
	if( NULL == m_pListHead )
	{
		DSERROR("[CInterlockedList] Memory allocation failed.\n");
        return;
    }
    InitializeSListHead( m_pListHead );
}
	
CInterlockedList::~CInterlockedList()
{
	RemoveAll();
}

UINT16	CInterlockedList::GetCount() const
{
	if( m_pListHead ) return QueryDepthSList( m_pListHead );
	else return 0;
}

BOOL	CInterlockedList::AddTail(PHYSICS_PACKET_HEADER* pHeader, N_PCINFO_HIT_DEDICATED2* pHit)
{
	PHYSICS_ITEM* pItem = (PHYSICS_ITEM*)_aligned_malloc(sizeof(PHYSICS_ITEM), MEMORY_ALLOCATION_ALIGNMENT);
	if( pItem == NULL )
	{
		DSERROR("[CInterlockedList] Memory allocation failed.\n");
		return FALSE;
	}
	
	pHeader->CopyTo( &pItem->header );
	pHit->CopyTo( &pItem->hit );

	SLIST_ENTRY* pFirstEntry = InterlockedPushEntrySList( m_pListHead, &(pItem->ItemEntry)); 
	if( pFirstEntry == NULL && GetCount() != 1)
	{
		DSERROR("[CInterlockedList] Add tail failure!\n");
		return FALSE;
	}

	return TRUE;
}

PHYSICS_ITEM*	CInterlockedList::RemoveHead()
{
	SLIST_ENTRY* pListEntry = InterlockedPopEntrySList( m_pListHead );
	if( pListEntry == NULL )
	{
		DSERROR("[CInterlockedList] List is empty.\n");
            return NULL;
	}
	
	return (PHYSICS_ITEM*)pListEntry;
}
void	CInterlockedList::DeleteEntry(SLIST_ENTRY* pListEntry)
{
	if( pListEntry ) _aligned_free( pListEntry );
}

void CInterlockedList::RemoveAll()
{
    UINT16 ui16Count = GetCount();
	SLIST_ENTRY* pListEntry = NULL;
	
	for( UINT16 i = 0; i < ui16Count; i++ )
	{
		pListEntry = InterlockedPopEntrySList( m_pListHead );
		 _aligned_free(pListEntry);
	}
    
	if( m_pListHead ) _aligned_free( m_pListHead );
}