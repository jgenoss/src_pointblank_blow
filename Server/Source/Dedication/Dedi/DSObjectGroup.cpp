#include "pch.h"
#include "DSObjectGroup.h"

CDSObjectGroupIdx::CDSObjectGroupIdx()
{
	m_i32ObjectCount = 0;
}

CDSObjectGroupIdx::~CDSObjectGroupIdx()
{
}

BOOL CDSObjectGroupIdx::InsertObject( INT32 i32Idx )
{
	if( DS_OBJECT_GROUP_COUNT <= m_i32ObjectCount )			return FALSE;

	for( INT32 i = 0 ; i < m_i32ObjectCount ; i++ )
	{
		if( m_i32ObjectIdx[i] == i32Idx )					return FALSE;
	}

	m_i32ObjectIdx[ m_i32ObjectCount ] = i32Idx;
	m_i32ObjectCount++;

	return TRUE;
}

CDSObjectGroup::CDSObjectGroup()
{
}

CDSObjectGroup::~CDSObjectGroup()
{
}

BOOL CDSObjectGroup::SetGroup( CDSObjectGroupIdx* pGroupIdx, CDSObject* pObject )
{
	m_eGroupType		= pGroupIdx->GetGroupType();
	m_i32ObjectCount	= pGroupIdx->GetObjectCount();

	for( INT32 i = 0 ; i < m_i32ObjectCount ; i++ )
	{
		m_pObject[ i ] = &pObject[ pGroupIdx->GetObjectList(i) ];
	}
	return TRUE;
}

void CDSObjectGroup::Reset()
{
	I3ASSERT( 0 < m_i32ObjectCount );

	CDSObject* pObject;

	// 모든 오브젝트 초기화
	for( INT32 i = 0 ; i < m_i32ObjectCount ; i++ )
	{
		m_pObject[i]->setShellCount( 0 );
		m_pObject[i]->SetRespawnTimer( 0.0f );
	}

	INT32 i32ObjectIdx = 0;
	if( 1 < m_i32ObjectCount )
	{
		// 랜덤하게 한개의 오브젝트를 활성화 시킨다.
		i32ObjectIdx = rand() % m_i32ObjectCount;
	}

	pObject = m_pObject[ i32ObjectIdx ];

	pObject->setShellCount( 1 );
	pObject->SetRespawnTimer( pObject->m_r32RespawnTime );
}

void CDSObjectGroup::Update()
{
	CDSObject* pObject;
	for( INT32 i = 0 ; i < m_i32ObjectCount ; i++ )
	{
		pObject = m_pObject[i];
		if( 0 < pObject->getShellCount() )	return;
	}

	for( INT32 i = 0 ; i < m_i32ObjectCount ; i++ )
	{
		pObject = m_pObject[ i ];
		// 박스가 비어 있고 타이머 시작
		if( 0 < pObject->getShellCount() )								continue;
		if( FALSE == pObject->m_bTimerEnable )							continue;

		// pick up 이후 경과 시간
		pObject->SetRespawnTimer( (REAL32)((GetTickCount() - pObject->m_ui32StartTime ) * 0.001) );

		// 타이머가 리스폰 타임을 넘었다면
		if( pObject->getRespawnTime() < pObject->m_r32RespawnTime )		continue;

		pObject->Reset();
		pObject->setShellCount( 1 );
		pObject->SetRespawnTimer( pObject->m_r32RespawnTime );
		pObject->m_bTimerEnable		= FALSE;
		pObject->m_bUDPSend			= TRUE;

		return;
	}
}