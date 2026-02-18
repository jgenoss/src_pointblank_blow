#include "pch.h"
#include "IPChecker.h"

CIPChecker::CIPChecker()
{
	m_bUsed					= FALSE;
	m_i32OpenIPCount		= 0;
}

CIPChecker::~CIPChecker()
{
}

BOOL CIPChecker::Create( BOOL bUsed, INT32 i32IPCount, IPBLOCK_IP* pOpenIP, INT32 i32NationCount, UINT8* pOpenNation )
{
	SetOpenList( bUsed, i32IPCount, pOpenIP, i32NationCount, pOpenNation );

	return TRUE;
}

void CIPChecker::SetOpenList( BOOL bUsed, INT32 i32IPCount, IPBLOCK_IP* pOpenIP, INT32 i32NationCount, UINT8* pui8OpenNation )
{
	m_bUsed = bUsed;

	i3mem::Copy( m_stOpenIP, pOpenIP, sizeof(IPBLOCK_IP)*i32IPCount );
	m_i32OpenIPCount = i32IPCount;

	i3mem::FillZero( m_bOpenNation, sizeof(BOOL)*GEOIP2_MAX_COUNTRY_COUNT );
	for( INT32 i = 0 ; i < i32NationCount ; i++ )
	{
		if( GEOIP2_MAX_COUNTRY_COUNT <= pui8OpenNation[i] )	continue;	// 의미 없는 코드입니다. UINT8 Size 0~255
		m_bOpenNation[ pui8OpenNation[i] ] = TRUE;
	}
}

BOOL CIPChecker::CheckBlock( UINT32 ui32IpNum, INT32 i32NationalCode)			// 입력받은 IP를 Config.ini에서 셋팅한 멤버변수 값과 비교하여 결과값 리턴.
{
	if( FALSE == m_bUsed )																					return TRUE;

	//NationCode가 0인경우 ( 국가에 할당된 IP 대역이 아닌경우 ) IP 블럭을 하지 않는다.
	if ( 0 == i32NationalCode )																				return TRUE;

	for( INT32 i = 0 ; i < m_i32OpenIPCount ; i++ )
	{
		if( (ui32IpNum >= m_stOpenIP[ i ].m_ui32StartIP) && (ui32IpNum <= m_stOpenIP[ i ].m_ui32EndIP) )	return TRUE;
	}

	if( GEOIP2_MAX_COUNTRY_COUNT <= i32NationalCode )														return FALSE;
	return m_bOpenNation[ i32NationalCode ];
}