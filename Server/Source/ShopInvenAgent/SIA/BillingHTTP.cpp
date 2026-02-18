#include "pch.h"
#include "BillingHTTP.h"

CBillingHTTP::CBillingHTTP()
{
	m_pHttp				= NULL;
	m_strLink[0]		= '\0';
	m_eConnectMethod	= i3NetworkHTTP::REQUEST_METHOD_UNKNOWN;
	m_i32HTTPRetryCount	= 0;
}

CBillingHTTP::~CBillingHTTP()
{
	ReleaseThread();
	OnDestroy();
}

BOOL CBillingHTTP::OnCreate( INT32 i32Idx, UINT8 ui8MethodIdx )
{
	i3String::NCopy( m_strLink, g_pConfig->m_astrBillingHttp[ui8MethodIdx], PATH_STRING_COUNT );
	m_eConnectMethod = i3NetworkHTTP::REQUEST_METHOD_POST;

	m_pHttp		= i3NetworkHTTP::NewObject();
	m_pHttp->CreateMethod( 0, 0, 0, 1000, 1024 ); 
	if( FALSE == OnConnect() )		return FALSE;	

	return CBillingBase::OnCreate( i32Idx, ui8MethodIdx );
}

void CBillingHTTP::OnDestroy()
{
	I3_SAFE_RELEASE( m_pHttp );

}

BOOL CBillingHTTP::OnConnect()
{
	if( FALSE == m_pHttp->Connect( m_strLink, m_eConnectMethod ) )	return FALSE;
	
	return TRUE;
}

BOOL CBillingHTTP::ReConnect()
{
	m_i32HTTPRetryCount++;
	m_pHttp->DisConnect();

	if( FALSE == OnConnect() )
	{
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[ERROR] LINK HTTP Connection Fail. retring:%d", m_i32HTTPRetryCount);

		return FALSE;
	}

	return TRUE;
}