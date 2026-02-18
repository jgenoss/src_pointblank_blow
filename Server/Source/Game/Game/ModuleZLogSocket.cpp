#include "pch.h"
#include "ModuleZlog.h"
#include "ModuleZLogSocket.h"

I3_CLASS_INSTANCE(CModuleZLogSocket, i3ElementBase); 

CModuleZLogSocket::CModuleZLogSocket()
{
	m_bConnect = FALSE;
}

CModuleZLogSocket::~CModuleZLogSocket()
{
	OnDestroy();
}

BOOL CModuleZLogSocket::OnCreate(UINT32 ui32IP, UINT16 ui16Port)
{
	m_bConnect = i3NetworkClientSocket::OnCreate(ui32IP, ui16Port);

	INT32 i32optVal = 2 * MByte; //2M
	INT32 i32optLen = sizeof(INT32);
	if (SOCKET_ERROR == setsockopt(GetSocket(), SOL_SOCKET, SO_SNDBUF, (char*)&i32optVal, i32optLen))	return FALSE;

	return m_bConnect;
}

void CModuleZLogSocket::OnDestroy(void)
{
	DisConnect();
}

void CModuleZLogSocket::DisConnect()
{
	m_bConnect = FALSE;
	i3NetworkClientSocket::DisConnect();
}

void CModuleZLogSocket::PacketParsing(i3NetworkPacket  * pPacket)
{
	
	PROTOCOL	ProtocolID = pPacket->GetProtocolID();
	
	switch( ProtocolID )
	{

	case PROTOCOL_ZLOG_FILE_FINISH:
		{
			BOOL result;
			pPacket->ReadData( &result, sizeof(BOOL) );

			g_pLog->WriteLog(L"[INFO] Finish Packet Recived. State : %d, Result : %d", g_pModuleZlog->m_eUpdateState, result);

			if( result )
				g_pModuleZlog->m_eUpdateState = ZLOG_UPDATE_STATE_FILE_DELETE;
			else
				g_pModuleZlog->m_eUpdateState = ZLOG_UPDATE_STATE_FIND;

		}
		break;

	default:
		break;

	}
	
	return; 
}

INT32 CModuleZLogSocket::SendPacketMessage(i3NetworkPacket* packet)
{
	INT32 i32ret = -1;

	while (i32ret < 0)
	{
		i32ret = i3NetworkClientSocket::SendPacketMessage(packet);

		if( SOCKET_ERROR == i32ret )
		{
			DWORD dwerrorCode = GetLastError();

			if ( dwerrorCode != WSAEWOULDBLOCK )
			{
				g_pLog->WriteLog(L"[ERR]SocketError. error code:%d ", dwerrorCode);
				return i32ret;
			}
			else
			{
				Sleep(10);
			}
		}
	}
	
	return i32ret;
}
