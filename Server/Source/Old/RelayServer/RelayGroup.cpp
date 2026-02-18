#include "pch.h"
#include "LogFile.h"
#include "RelayGroup.h"
#include "RelayMember.h"

I3_CLASS_INSTANCE(CRelayGroup, i3ElementBase);

CRelayGroup::CRelayGroup()
{
	// ID를 초기화한다.
	m_eState		= RGS_INVALID;
	m_pRelayMember	= NULL;
	m_wEncryptKey	= 0xFFFFFFFF;
	m_RelayRoomInfo.Reset();
}

CRelayGroup::~CRelayGroup()
{
	// 멤버목록을 해제한다.
	I3MEM_SAFE_FREE( m_pRelayMember );
}

BOOL CRelayGroup::Create()
{
	I3MEM_SAFE_FREE( m_pRelayMember );	

	// 멤버목록을 준비한다.
	m_pRelayMember = (CRelayMember*) i3MemAlloc( sizeof( CRelayMember ) * SLOT_MAX_COUNT );
	if( m_pRelayMember == NULL ) return FALSE;
    
	m_eState = RGS_READY;

	return TRUE;
}

UINT32 CRelayGroup::Register_C( BATTLE_ROOM_INFO * pRelayRoomInfo )
{
	if( RGS_READY != m_eState )		return RELAY_ENCRYPTKEY_DEFAULT;

	m_wEncryptKey	= rand()%(0xFFFFFFFF);

	m_RelayRoomInfo.Copy( pRelayRoomInfo );
	
	// 멤버 정보 초기화
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		m_pRelayMember[i].Init();
	}

	m_eState		= RGS_USE;
	m_wCreateTime	= g_pConfig->GetServerTime();

	return m_wEncryptKey;
}

void CRelayGroup::Revoke_C(void)
{
	// 상태 전이
	m_eState = RGS_READY;

	// 초기화 상태가 아니면, 초기화 상태로 복귀
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		m_pRelayMember[i].Init();
	}
}

BOOL CRelayGroup::JoinGroup( INT32 i32Thread, RELAY_MEMBER_INFO* pRelayMember, UINT16 sPort )
{
	if( RGS_USE != m_eState )	
	{
		char szTemp[MAX_PATH];
		sprintf( szTemp, "[ERROR] JoinGroup State:%d, Slot:%d", m_eState, pRelayMember->_cSlotIdx );
		WRITE_LOG_RELAY( i32Thread, szTemp );

		return FALSE;
	}

	if( 0 > pRelayMember->_cSlotIdx || pRelayMember->_cSlotIdx >= SLOT_MAX_COUNT )
	{
		char szTemp[MAX_PATH];
		sprintf( szTemp, "[ERROR] JoinGroup SlotIdx:%d", pRelayMember->_cSlotIdx );
		WRITE_LOG_RELAY( i32Thread, szTemp );

		return FALSE;
	}
	
	// EncryptKey 유효성 검사
	if( FALSE == CheckEncryptKey( pRelayMember->_wEncryptKey ) )
	{
		char szTemp[MAX_PATH];
		sprintf( szTemp, "[ERROR] JoinGroup Encrypt OrgKey:%d, UserKey:%d, IP:%d, Port:%d", 
			m_wEncryptKey, pRelayMember->_wEncryptKey, pRelayMember->_wIp, pRelayMember->_sPort );
		WRITE_LOG_RELAY( i32Thread, szTemp );

		return FALSE;
	}

	m_wCreateTime = g_pConfig->GetServerTime();
	m_sPort = sPort;

	// 맴버를 등록한다. 맴버가 비어있지 않으면 FALSE;
	if( m_pRelayMember[ pRelayMember->_cSlotIdx ].InsertContext( pRelayMember ) )	return TRUE;

	return FALSE;
}

BOOL CRelayGroup::Withdraw( UINT8 cSlotIdx )
{
	if( RGS_INVALID == m_eState )						return FALSE;

	if( 0 > cSlotIdx || cSlotIdx >= SLOT_MAX_COUNT )	return FALSE;

	if( m_pRelayMember[ cSlotIdx ].DeleteContext() )	return TRUE;

	return FALSE;
}

BOOL CRelayGroup::Withdraw_C( UINT8 cSlotIdx )
{
	if( RGS_INVALID == m_eState )						return FALSE;

	if( 0 > cSlotIdx || cSlotIdx >= SLOT_MAX_COUNT )	return FALSE;

	m_pRelayMember[ cSlotIdx ].SetPlaying( FALSE );

	return TRUE;
}

BOOL CRelayGroup::CheckMember( UINT8 cSlotIdx )
{
	if( (0 == m_pRelayMember[ cSlotIdx ].GetIp()) || 
		(0 == m_pRelayMember[ cSlotIdx ].GetPort()) )		return FALSE;

	if( FALSE == m_pRelayMember[ cSlotIdx ].GetPlaying() )
	{
		m_pRelayMember[ cSlotIdx ].DeleteContext();
		return FALSE;
	}

	return TRUE;
}

BOOL CRelayGroup::GetRelayTarget( INT32 i32Thread, CUdpBuffer* pRecvBuffer )
{
	if( 0 > pRecvBuffer->GetSlotIdx() || pRecvBuffer->GetSlotIdx() >= SLOT_MAX_COUNT )
	{
		char szTemp[MAX_PATH];
		sprintf( szTemp, "[ERROR] GetRelayTarget Slot : %d", pRecvBuffer->GetSlotIdx() );
		WRITE_LOG_RELAY( i32Thread, szTemp );
		return FALSE;
	}
	if( 0 > pRecvBuffer->GetTargetIdx() || pRecvBuffer->GetTargetIdx() >= SLOT_MAX_COUNT )
	{
		char szTemp[MAX_PATH];
		sprintf( szTemp, "[ERROR] GetRelayTarget TargetIdx : %d", pRecvBuffer->GetSlotIdx() );
		WRITE_LOG_RELAY( i32Thread, szTemp );
		return FALSE;
	}
	if( FALSE == CheckEncryptKey( pRecvBuffer->GetEncryptKey() ) )
	{
		char szTemp[MAX_PATH];
		sprintf( szTemp, "[ERROR] GetRelayTarget Encrypt OrgKey:%d, SrcKey:%d, IP:%d, Port:%d", 
			m_wEncryptKey, pRecvBuffer->GetEncryptKey(), m_pRelayMember[ pRecvBuffer->GetSlotIdx() ].GetIp(), m_pRelayMember[ pRecvBuffer->GetSlotIdx() ].GetPort() );
		WRITE_LOG_RELAY( i32Thread, szTemp );
		return FALSE;
	}
	// 보낸 사람이 IP 와 Port 가 맞는지 확인합니다.
	if( FALSE == m_pRelayMember[ pRecvBuffer->GetSlotIdx() ].IsSameAddress( pRecvBuffer->GetIp(), pRecvBuffer->GetPort() ) )
	{
		char szTemp[MAX_PATH];
		sprintf( szTemp, "[ERROR] GetRelayTarget IPPORT Slot:%d, Recv-%d,%d, Server-%d,%d",
			pRecvBuffer->GetSlotIdx(), pRecvBuffer->GetIp(), pRecvBuffer->GetPort(), m_pRelayMember[ pRecvBuffer->GetSlotIdx() ].GetIp(), m_pRelayMember[ pRecvBuffer->GetSlotIdx() ].GetPort());
		WRITE_LOG_RELAY( i32Thread, szTemp );
		return FALSE;
	}	

	m_wCreateTime = g_pConfig->GetServerTime();

	pRecvBuffer->SetAddress( m_pRelayMember[ pRecvBuffer->GetTargetIdx() ].GetIp(), m_pRelayMember[ pRecvBuffer->GetTargetIdx() ].GetPort() );

	return TRUE;
}

BOOL CRelayGroup::CheckEncryptKey( UINT32 wEncryptKey )
{
	return m_wEncryptKey == wEncryptKey;
}
