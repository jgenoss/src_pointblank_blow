#include "pch.h"
#include "BillingTCP.h"

#define RECONNECT_TIME	10
static INT32 _DefCallBackProc( void* pAgent, char* pPacket, INT32 i32Size )
{
	return ((CBillingTCP*)pAgent)->RecvMessage( pPacket, i32Size );
}

CBillingTCP::CBillingTCP()
{
	m_pSocket			= NULL;
	m_ui32ReconnectTime	= 0;
}

CBillingTCP::~CBillingTCP()
{
	ReleaseThread();
	OnDestroy();
}

BOOL CBillingTCP::OnCreate( INT32 i32Idx, UINT8 ui8MethodIdx )
{
	m_ui32IP		= g_pConfig->m_aui32BillingIp[ui8MethodIdx];
	m_ui16Port		= g_pConfig->m_aui16BillingPort[ui8MethodIdx];

	m_pSocket = i3NetworkClientSocket2::NewObject();
	if( NULL == m_pSocket )																return FALSE;
	if( FALSE == m_pSocket->OnCreate( m_ui32IP, m_ui16Port, _DefCallBackProc, this ))	return FALSE;

	m_ui32ReconnectTime = i3ThreadTimer::GetServerTime();

	return CBillingBase::OnCreate( i32Idx, ui8MethodIdx );
}

void CBillingTCP::OnDestroy()
{
	I3_SAFE_RELEASE( m_pSocket );
}

BOOL CBillingTCP::OnRunning()
{
	if( m_pSocket->IsConnected() )
	{
		if (m_pSocket->SelectEvent() > 0)
			return TRUE;
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
				m_pSocket->OnDestroy();
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"Reconnect Fail" ); 
			}
			m_ui32ReconnectTime = i3ThreadTimer::GetServerTime();
		}
	}
	return FALSE;
}

INT32 CBillingTCP::SendMessage( char* pPacket, INT32 i32Size )
{
	return m_pSocket->SendPacketMessage( pPacket, i32Size );
}

INT32 CBillingTCP::RecvMessage( char* pPacket, INT32 i32Size )
{	pPacket;
	return i32Size;
}