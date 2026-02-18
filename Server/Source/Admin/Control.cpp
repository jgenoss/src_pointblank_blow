#include "stdafx.h"
#include "Control.h"
#include "LogFile.h"
CControl * g_pControl = NULL;

I3_CLASS_INSTANCE( CControl, i3ElementBase);

CControl::CControl(void)
{

}

CControl::~CControl(void)
{

}

BOOL CControl::Create(const char * szFileName)
{
	//0. Load 
	if( !LoadFromFile(szFileName) ) 
	{
		return FALSE; 
	}
	//1. Make TCP Socket 
	m_SocketAccept = INVALID_SOCKET; 
	m_timeout.tv_sec  = 0;
	m_timeout.tv_usec = 100;
	if(!m_AcceptSocket.OnCreate( inet_addr(m_Ip), m_Port) )return FALSE; 

	m_RecvEvent = INVALID_HANDLE_VALUE;

	return TRUE; 
}

BOOL CControl::Destroy(void)
{
	CloseSocket(); 
	return TRUE;
}

void CControl::CloseSocket(void)
{
	closesocket( m_SocketAccept ); 
	m_SocketAccept = INVALID_SOCKET; 

	::WSACloseEvent( m_RecvEvent );
	m_RecvEvent = INVALID_HANDLE_VALUE;
}

INT32 CControl::Update(void)
{
	REAL32 rDeltaTime = m_Timer.GetDeltaSec(); 

	if(m_SocketAccept == INVALID_SOCKET)
	{
		INT32  AddrSize; 
		struct sockaddr_in FromAddr; 
		AddrSize = sizeof(sockaddr_in); 
		m_SocketAccept = m_AcceptSocket.AcceptConnection( &m_timeout, &FromAddr, &AddrSize );

		if(m_SocketAccept != INVALID_SOCKET)
		{
			m_rConnectTime = 0.0f; 
			m_RecvEvent = ::WSACreateEvent();

			//Save Address Infomation 
			char * pIpString = NULL; 
			struct in_addr	ConIp; 
			ConIp.s_addr	=	FromAddr.sin_addr.s_addr;
			pIpString		=	inet_ntoa( ConIp );

			i3String::Copy(m_ConnectIp, pIpString, 32); 
			g_pLogFile->WriteLog( pIpString ); 
		}
		else
		{
			return 0; 
		}
	}

	m_rConnectTime = m_rConnectTime + rDeltaTime; 
	if( m_rConnectTime > 5.f)
	{
		g_pLogFile->WriteLog( "TimeOut" ); 
		CloseSocket(); 		
		return 0; 
	}

	WSANETWORKEVENTS nEvent; 
	i3mem::FillZero( &nEvent, sizeof( nEvent ) ); 
	::WSAEventSelect( m_SocketAccept, m_RecvEvent, FD_WRITE | FD_READ | FD_CLOSE ); 
	::WSAEnumNetworkEvents( m_SocketAccept, m_RecvEvent, &nEvent ); 

	INT32 Rv = 0; 
	if( (nEvent.lNetworkEvents & FD_READ )	== FD_READ) Rv = OnReceive();
	if( (nEvent.lNetworkEvents & FD_CLOSE ) == FD_CLOSE) 
	{
		g_pLogFile->WriteLog( "Disconnect" ); 
		CloseSocket();	
	}

	return Rv; 
}

void CControl::OnSend(INT32 Rv)
{
	if( m_SocketAccept == INVALID_SOCKET)return; 

	OVERLAPPED	Overlapped; 
	DWORD		WrittenBytes; 
	i3mem::FillZero(&Overlapped, sizeof(Overlapped)); 
	Rv = ::WriteFile( (HANDLE)m_SocketAccept, &Rv, 4, &WrittenBytes, (LPOVERLAPPED ) &Overlapped);	
}

/*
리턴값으로 서버를 컨트롤 합니다.
*/
INT32 CControl::OnReceive(void)
{
	INT32			Rv = 0; 
	DWORD			TransferByte; 
	DWORD			Byte = 128;
	OVERLAPPED		overlapped;
	
	char pBuffer[128]; 
	char pLogMessage[128]; 
	i3mem::FillZero( &overlapped, sizeof(OVERLAPPED) );
	::ReadFile((HANDLE)m_SocketAccept, &pBuffer[0], Byte,  &TransferByte, (LPOVERLAPPED)&overlapped ); 
		
	if( TransferByte == 65 )//사이즈는 고정합니다. 
	{
		memcpy( m_pServiceName	, pBuffer, 64 ); 
		memcpy( pLogMessage		, pBuffer, 64 ); 

		pLogMessage[64] = 0; 
		switch( (UINT8)pBuffer[64] )
		{
		case 1 : 
			Rv = 1; 
			strcat(pLogMessage, "Message : Server Start");
			break; 
		case 2 : 
			Rv = 2; 
			strcat(pLogMessage, "Message : Server Stop");
			break;
		default:  
			Rv = 0; 
			strcat(pLogMessage, "Message : Command Unknown");
			break;
		}
		g_pLogFile->WriteLog( pLogMessage ); 
	}
	else
	{
		sprintf(pLogMessage, "Message Unknown size = %d", TransferByte);
		g_pLogFile->WriteLog( pLogMessage ); 
		CloseSocket();
	}
	return Rv; 
}


BOOL CControl::LoadFromFile(const char * szFileName)
{
	if( szFileName == NULL)	return FALSE;

	i3IniParser Parser;
	INT32	Temp; 

	if( !Parser.OpenFromFile( szFileName))	return FALSE;

	if( Parser.ReadSection("Common"))
	{
		strcpy(m_Ip, Parser.GetStringValue("IP"));
		Parser.GetValue( "Port", Temp, &Temp);
		m_Port = (UINT16)Temp; 
	}
	else 
	{
		return FALSE; 
	}

	Parser.Close();

	return TRUE;
}
