#include "pch.h"
#include "LoginTCP.h"

#define RECONNECT_TIME	10

static INT32 _DefCallBackProc(void* pAgent, char* pPacket, INT32 i32Size)
{
	return ((CLoginTCP*)pAgent)->RecvMessage(pPacket, i32Size); //중남미 리시브 쓰레드
}

CLoginTCP::CLoginTCP()
{
	m_pSocket = NULL;
	m_ui32ReconnectTime = 0;
}

CLoginTCP::~CLoginTCP()
{
	OnDestroy();
}

BOOL CLoginTCP::OnCreate(INT32 i32idx, UINT32 ui32BufferMaxCount, UINT8 ui8MethodIdx)
{
	m_ui32IP = g_pConfig->m_aui32LoginIp[ui8MethodIdx];
	m_ui16Port = g_pConfig->m_aui16LoginPort[ui8MethodIdx];

	m_pSocket = i3NetworkClientSocket2::NewObject();
	if (NULL == m_pSocket)																return FALSE;
	if (FALSE == m_pSocket->OnCreate(m_ui32IP, m_ui16Port, _DefCallBackProc, this))		return FALSE;
	
	m_ui32ReconnectTime = i3ThreadTimer::GetServerTime();

	return CLoginBase::OnCreate(i32idx, ui32BufferMaxCount, ui8MethodIdx);
}

void CLoginTCP::OnDestroy()
{
	I3_SAFE_RELEASE(m_pSocket);
}

BOOL CLoginTCP::OnRunningEX()
{
	if (m_pSocket->IsConnected())
	{
		if (m_pSocket->SelectEvent() > 0)
			return TRUE;
	}
	else
	{
		if (m_ui32ReconnectTime + RECONNECT_TIME < i3ThreadTimer::GetServerTime())
		{	// 10초에 한번씩 Reconnect 합니다.
			m_pSocket->OnDestroy();
			
			if (m_pSocket->OnCreate(m_ui32IP, m_ui16Port, _DefCallBackProc, this))
			{
				g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L"Reconnect tcp Success");
			}
			else
			{
				m_pSocket->OnDestroy();
				g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L"Reconnect tcp  Fail");
			}
			m_ui32ReconnectTime = i3ThreadTimer::GetServerTime();
		}
	}
		
	return FALSE;
}

INT32 CLoginTCP::SendMessage(char* pPacket, INT32 i32Size)
{
	return m_pSocket->SendPacketMessage(pPacket, i32Size);
}

INT32 CLoginTCP::RecvMessage(char* pPacket, INT32 i32Size)
{	pPacket;
	return i32Size;
}