#include "pch.h"
#include "DSObjectManager.h"

CDSObjectManager::CDSObjectManager()
{
	m_i32ObjectCount = 0;
}

CDSObjectManager::~CDSObjectManager()
{
	Destroy();
}

BOOL CDSObjectManager::Create( INT32 i32Count )
{
	m_i32ObjectCount = i32Count;

	m_pList = new CDSObjectList[ i32Count ];
	if( NULL == m_pList )				return FALSE;

	return TRUE;
}

void CDSObjectManager::Destroy()
{
	I3_SAFE_DELETE_ARRAY( m_pList );
}

void CDSObjectManager::Reset()
{
	m_pFront		= NULL;
	m_ui32UpdateIdx	= 0;
}

BOOL CDSObjectManager::AddObject( INT32 i32Idx, CDSObject* pObject )
{
	if( i32Idx >= m_i32ObjectCount )	return FALSE;

	CDSObjectList* pList	= &m_pList[ i32Idx ];
	pList->m_pObject		= pObject;
	pList->m_ui32UpdateIdx	= m_ui32UpdateIdx;
	m_ui32UpdateIdx++;

	if( NULL != m_pFront )
	{
		m_pFront->m_pPrev	= pList;
	}
	pList->m_pPrev			= NULL;
	pList->m_pNext			= m_pFront;
	m_pFront				= pList;

	return TRUE;
}

void CDSObjectManager::RenewalObject( INT32 i32Idx )
{
	if (!m_pFront) return;

	I3ASSERT( i32Idx < m_i32ObjectCount );

	CDSObjectList* pList	= &m_pList[ i32Idx ];
	if (!pList) return;

	pList->m_ui32UpdateIdx	= m_ui32UpdateIdx;
	pList->m_pObject->ResetUDPSend();
	m_ui32UpdateIdx++;

	if( m_pFront == pList )	return;

	CDSObjectList* pPrev	= pList->m_pPrev;
	CDSObjectList* pNext	= pList->m_pNext;

	if( pPrev )
	{
		pPrev->m_pNext		= pList->m_pNext;
	}
	if( pNext )
	{
		pNext->m_pPrev		= pList->m_pPrev;
	}

	m_pFront->m_pPrev		= pList;
	pList->m_pNext			= m_pFront;
	m_pFront				= pList;
}

UINT32 CDSObjectManager::MakePacket( char* pBuffer, UINT32 ui32MaxSize, SEND_PACKET_TYPE ePacketType, UINT32& ui32UpdateTime )
{
	UINT16		ui32SendPos			= 0;
	UINT32		ui32WriteSize;

	CDSObjectList* pList = m_pFront;
	if( pList->m_ui32UpdateIdx == ui32UpdateTime )	return ui32SendPos;

	while( pList )
	{
		if( pList->m_ui32UpdateIdx <= ui32UpdateTime ) break;
		pList = pList->m_pNext;
	}
	
	while( pList )
	{
		if( FALSE == pList->m_pObject->MakePacket( &pBuffer[ ui32SendPos ], ui32MaxSize, ui32WriteSize, ePacketType ) )	return ui32SendPos;

		ui32MaxSize		-= ui32WriteSize;
		ui32SendPos		+= ui32WriteSize;

		ui32UpdateTime = pList->m_ui32UpdateIdx;
		pList = pList->m_pPrev;
	}
	return ui32SendPos;
}