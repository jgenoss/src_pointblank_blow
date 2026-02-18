#include "pch.h"
#include "RelayMember.h"

#define INVALID_DB_IDX			(0xFFFFFFFF)
#define INVALID_SESSION_IDX		(-1)

I3_CLASS_INSTANCE(CRelayMember, i3ElementBase);

CRelayMember::CRelayMember()
{
	// 멤버필드 초기화
	Init();
}

CRelayMember::~CRelayMember()
{
	// Do nothing
}

void CRelayMember::Init(void)
{
	m_wIp			= 0;
	m_sPort			= 0;
	m_bPlaying		= FALSE;
}

BOOL CRelayMember::InsertContext( RELAY_MEMBER_INFO* pRelayMember )
{
	//if( m_bPlaying )	return FALSE;

	if( m_wIp == 0 && m_sPort == 0 )
	{
		// 멤버 정보 설정
		m_wIp			= pRelayMember->_wIp;
		m_sPort			= pRelayMember->_sPort;
		m_bPlaying		= TRUE;
		return TRUE;
	}
	else if( m_wIp == pRelayMember->_wIp && m_sPort == pRelayMember->_sPort )
	{
		return TRUE;
	}
	else if( m_bPlaying )
	{
		return FALSE;
	}

	return FALSE;
}

BOOL CRelayMember::DeleteContext( )
{
	Init();

	return TRUE;
}

BOOL CRelayMember::IsSameAddress( RELAY_MEMBER_INFO* pRelayMember )
{
	if( m_wIp != pRelayMember->_wIp )				return FALSE;
	if( m_sPort != pRelayMember->_sPort )			return FALSE;

	return TRUE;
}

BOOL CRelayMember::IsSameAddress(UINT32 wIp, UINT16 sPort)
{
	if (m_wIp == wIp && m_sPort == sPort)			return TRUE;

	return FALSE;
}