#include "pch.h"
#include "PCCafeTCP.h"

#define RECONNECT_TIME	10
static INT32 _DefCallBackProc( void* pAgent, char* pPacket, INT32 i32Size )
{
	return ((CPCCafeTCP*)pAgent)->RecvMessage( pPacket, i32Size );
}

CPCCafeTCP::CPCCafeTCP()
{
	m_pSocket			= NULL;
	m_ui32ReconnectTime	= 0;
}

CPCCafeTCP::~CPCCafeTCP()
{
	ReleaseThread();
	OnDestroy();
}

BOOL CPCCafeTCP::OnCreate( INT32 i32Idx )
{
	m_ui32IP		= g_pConfig->m_ui32PCCafeIp;
	m_ui16Port		= g_pConfig->m_ui16PCCafePort;

	m_pSocket = i3NetworkClientSocket2::NewObject();
	if( NULL == m_pSocket )																return FALSE;
	if( FALSE == m_pSocket->OnCreate( m_ui32IP, m_ui16Port, _DefCallBackProc, this ))	return FALSE;

	m_ui32ReconnectTime = i3ThreadTimer::GetServerTime();

	return CPCCafeBase::OnCreate( i32Idx );
}

void CPCCafeTCP::OnDestroy()
{
	I3_SAFE_RELEASE( m_pSocket );
}

BOOL CPCCafeTCP::OnRunning()
{
	if( m_pSocket->IsConnected() )
	{
		m_pSocket->SelectEvent();
	}
	else
	{
		if( m_ui32ReconnectTime + RECONNECT_TIME < i3ThreadTimer::GetServerTime() )
		{	// 10초에 한번씩 Reconnect 합니다.
			m_pSocket->OnDestroy();
			if( m_pSocket->OnCreate( m_ui32IP, m_ui16Port, _DefCallBackProc, this ) )
			{
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"Reconnect Success" ); 

			}
			else
			{
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"Reconnect Fail" ); 
			}
			m_ui32ReconnectTime = i3ThreadTimer::GetServerTime();
		}
	}
	return TRUE;
}

INT32 CPCCafeTCP::SendMessage( char* pPacket, INT32 i32Size )
{
	return m_pSocket->SendPacketMessage( pPacket, i32Size );
}

INT32 CPCCafeTCP::RecvMessage( char* pPacket, INT32 i32Size )
{	pPacket;
	return i32Size;
}