#include "pch.h"
#include "LoginHTTP.h"

CLoginHTTP::CLoginHTTP()
{
	m_strLink[0]		= '\0';
	m_i32HTTPRetryCount	= 0;
	m_pHttp				= NULL;
}

CLoginHTTP::~CLoginHTTP()
{
	OnDestroy();
}

BOOL CLoginHTTP::OnCreate( INT32 i32Idx, UINT32 ui32BufferMaxCount, UINT8 ui8MethodIdx )
{
	i3String::NCopy( m_strLink, g_pConfig->m_astrLoginHttp[ui8MethodIdx], PATH_STRING_COUNT );
	m_eConnectMethod = (i3NetworkHTTP::REQUEST_METHOD)g_pConfig->m_aui8LoginHttpMethod[ui8MethodIdx];

	m_pHttp		= i3NetworkHTTP::NewObject();
	m_pHttp->CreateMethod( 0, 0, 0, 1000, 1024 ); 
	if( FALSE == OnConnect() )		return FALSE;	

	return CLoginBase::OnCreate( i32Idx, ui32BufferMaxCount, ui8MethodIdx );
}

void CLoginHTTP::OnDestroy()
{
	I3_SAFE_RELEASE( m_pHttp );
}

BOOL CLoginHTTP::OnConnect()
{
	if( FALSE == m_pHttp->Connect( m_strLink, m_eConnectMethod ) )	return FALSE;
	
	return TRUE;
}

BOOL CLoginHTTP::ReConnect()
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