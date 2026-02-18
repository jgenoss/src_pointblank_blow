// DediMember.cpp
//
// YouJong Ha
//	Last update : 2010-11-25 (yyyy:mm:dd)
//	
// Description:
//		
//
//
// Dependency: 
//	 	
//
#include "pch.h"
#include "DediMember.h"

#define INVALID_DB_IDX			(0xFFFFFFFF)
#define INVALID_SESSION_IDX		(-1)




I3_CLASS_INSTANCE(CDediMember, i3ElementBase);

CDediMember::CDediMember()
{
	// 멤버필드 초기화
	Init();
}

CDediMember::~CDediMember()
{
	// Do nothing
}

void CDediMember::Init(void)
{
	m_ui32Ip		= 0;
	m_ui16Port		= 0;
	m_bPlaying		= FALSE;
}


// DSERVER_MEMBER_INFO를 CDediMember 멤버로 설정하는 함수
// JoinGroup(UDP)에서 호출된다.
BOOL CDediMember::InsertContext( UINT32 ui32IP, UINT16 ui16Port )
{
	if ( m_ui32Ip == 0 && m_ui16Port == 0)
	{
		// 멤버 정보 설정
		m_ui32Ip		= ui32IP;
		m_ui16Port		= ui16Port;
		m_bPlaying		= TRUE;
		return TRUE;
	}
	else if ( m_ui32Ip == ui32IP && m_ui16Port == ui16Port )
	{
		return TRUE;
	}
	else if ( m_bPlaying )
	{
		DSWARNING("[CDediMember::InsertContext] This User Aleady in the Playing State!\n");
		return FALSE;
	}
	
	return FALSE; // ok ack를 보냄
}

BOOL CDediMember::DeleteContext( )
{
	Init();

	return TRUE;
}

//BOOL CDediMember::IsSameAddress( N_DSERVER_USER_ENTER* user )
//{
//	if (m_wIp == 0) return FALSE;
//	return ( m_wIp == GET_IP(&user->m_addr)  && m_sPort == GET_PORT(&user->m_addr) );
//}

BOOL CDediMember::IsSameAddress(UINT32 ui32Ip, UINT16 ui16Port)
{
	// 클라이언트가 룸을 나간 후 도착한 CN 패킷으로 인해 assert 제거.
	if( m_ui32Ip == 0 && m_ui16Port == 0 ) return FALSE;
	return ( m_ui32Ip == ui32Ip && m_ui16Port == ui16Port );
}

//BOOL CDediMember::CheckRegisteredIP(UINT32 dwIP)
//{
//	return ( m_wIp == dwIP );
//}