#include "pch.h"
#include "ModuleControl.h"
#include "ModuleControlSocket.h"
#include "GroupMaker.h"
#include "DediRoom.h"
#include "ModuleControl.h"
#include "iocpServer.h"
#include "MapManager.h"

extern char* g_pszHackName[];

I3_CLASS_INSTANCE( CModuleControlSocket, i3NetworkClientSocket );


CModuleControlSocket::CModuleControlSocket(void)
{
	m_bConnect				= FALSE;
	m_ui32ControlSendTime	= 1000;
	m_i32PortIdx			= 0;
	m_ui32LastRecvTime		= 0;
	m_ui32LastSendTime		= 0;
}

CModuleControlSocket::~CModuleControlSocket(void)
{

}

BOOL CModuleControlSocket::OnCreate( UINT32 ui32IP, UINT16 ui16PortNo )
{
	DWORD dwOld = GetTickCount();
	BOOL bRv = i3NetworkClientSocket::OnCreate( ui32IP, ui16PortNo);
	DSERROR("Connection Return in %d ms\n", GetTickCount() - dwOld );
	m_ui32LastRecvTime	= i3ThreadTimer::GetServerTime();
	m_ui32LastSendTime	= i3ThreadTimer::GetServerTime();

	if( bRv == TRUE )
	{
		// 인도네시아 라이브(2011-12-02) 
		SOCKET s = GetSocket();
		// relay에서는 100, 100을 사용. 200,200으로 바꿨는데도 문제 남아서, 변경.
		INT32 i32RecvBufSize = 1024 * 1024 * 100;
		INT32 i32SendBufSize = 2147483647; //1024 * 1024 * 300;
		setsockopt( s, SOL_SOCKET, SO_RCVBUF, (const char *) &i32RecvBufSize, sizeof(INT32));
		setsockopt( s, SOL_SOCKET, SO_SNDBUF, (const char *) &i32SendBufSize, sizeof(INT32));
	}

	return bRv;
}

void CModuleControlSocket::OnDestroy(void)
{
	m_bConnect = FALSE; 
	i3NetworkClientSocket::OnDestroy();

	DSERROR("[CModuleControlSocket::OnDestroy] Disconnected From Control Server!\n");
}

void CModuleControlSocket::PacketParsing(i3NetworkPacket  * pPacket)
{
	//AT Mode 
	if( g_pRoomManager->GetAtMode() )
	{
		m_ui32LastRecvTime	= i3ThreadTimer::GetServerTime();
		return;
	}

	i3NetworkPacket SendPacket;
	SendPacket.Clear();
	
	BOOL		bSendMessage = FALSE; 
	PROTOCOL	ProtocolID = pPacket->GetProtocolID(); 

	switch(ProtocolID)
	{
	case PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS	://연결되었음
		{
			DSMSG("CONTROL SERVER CONNECT MESSAGE RECV\n");
			pPacket->ReadData( &m_ui32ControlSendTime,			sizeof(UINT32));
			pPacket->ReadData( &g_pConfig->m_ControlVersion,	sizeof(SS_VERSION));

			INT32 i32ConnectType= ASC_CONNECT_TYPE_SERVER;			//Connect Type
			UINT8 ui8ServerType	= ASC_SERVER_BATTLE;			 	//Server Type
			UINT8 iBattleType	= BATTLE_SERVER_TYPE_DEDICATION;	//Battle Type 

			bSendMessage = TRUE;
			SendPacket.SetProtocol( PROTOCOL_ASC_CONFIRMATION_REQ );
			SendPacket.WriteData(&i32ConnectType,				sizeof(INT32));
			SendPacket.WriteData(&ui8ServerType,				sizeof(UINT8));
			SendPacket.WriteData(&g_pConfig->m_Version,			sizeof(SS_VERSION));
			SendPacket.WriteData(&g_pConfig->m_ui32UdpActiveIp,	sizeof(UINT32));
			SendPacket.WriteData(&iBattleType,					sizeof(UINT8));
		}
		break;
	case PROTOCOL_ASC_CONFIRMATION_ACK				: 
		{
			I3ASSERT(m_bConnect == FALSE);
			m_bConnect = TRUE; // control server connection established

			switch ( g_eServerState )
			{
			case SERVER_STATE_START :	// Control 서버 Reconnect된 상태
				g_pModuleControl->ServerInit( MODULE_CONTROL_THREAD_IDX, EVENT_ERROR_SUCCESS );
				// break; // 주석이 맞음.
			default :
				m_ui32LastRecvTime	= i3ThreadTimer::GetServerTime(); 
				return ;

			case SERVER_STATE_LOADING :
				break;
					
			}
			
			pPacket->ReadData( &g_pConfig->m_ui32CastServerIP,		sizeof(UINT32));					// get group count 무시
			pPacket->ReadData( &g_pConfig->m_ui16CastServerPort,		sizeof(UINT16));					// get group count 무시

			INT32 i32GroupCount = 0;
			pPacket->ReadData( &i32GroupCount,	sizeof(UINT32));					// get group count 무시

			//Set Port 
			pPacket->ReadData(&g_pConfig->m_ui32UdpPortCount, sizeof(UINT8));		
			g_pConfig->m_pui16UdpPort = (UINT16*)i3MemAlloc( sizeof(UINT16) * g_pConfig->m_ui32UdpPortCount );  
			pPacket->ReadData(g_pConfig->m_pui16UdpPort, sizeof(UINT16)*g_pConfig->m_ui32UdpPortCount);
			pPacket->ReadData(&g_pConfig->m_eServerVersion, sizeof(PB_VER) );
			pPacket->ReadData(&g_pConfig->m_ui8MultiWeaponCount, sizeof(UINT8) );
			// 핵체크 기본값
			UINT8 aui8Ratio[MAX_PATH];
			UINT8 ui8Count;
			pPacket->ReadData(&ui8Count, sizeof(UINT8));
			I3ASSERT(ui8Count == HACK_TYPE_MAX);
			if( ui8Count != HACK_TYPE_MAX )
			{
				DSERROR("ui8Count != HACK_TYPE_MAX. Hack Check Ratio Not Set!(check control server version)\n");
				break;
			}
				
			pPacket->ReadData(aui8Ratio, sizeof(UINT8) * ui8Count);

			for (UINT32 i = 0; i < HACK_TYPE_MAX; i++)
			{
				if( g_pConfig->m_bHackCheck )
				{
					g_pConfig->m_aui8HackCheckLevel[i] = aui8Ratio[i] / 10;
				}
				else
				{
					if (HACK_TYPE_INSTALL_C4 == i)
						g_pConfig->m_aui8HackCheckLevel[i] = 100;
					else
						g_pConfig->m_aui8HackCheckLevel[i] = 0;
				}
			} 
			
			g_eServerState = SERVER_STATE_INIT;							// change the server state into INIT
		}
		break; 
	case PROTOCOL_ASC_CONFIG_RELOAD_ACK	: // 컨트롤 서버로부터 정보 갱신 (현재는 무기 리스트 변경되었다는 의미)
		{
			// ignore 2014년에 체크함. 하지만 별 필요 없음. 
			UINT32 ui32SIAVersion = 0;
			pPacket->ReadData(&ui32SIAVersion,	sizeof( UINT32 ));
		}
		break;
	case PROTOCOL_BASE_HEART_BIT_ACK			: break;
	case PROTOCOL_ASC_CHANGE_HACKCHECKRATIO_NTF:	// Receive Hack check ratio from control server (control tool -> control server)
	{
		if (!g_pConfig) break;
		ChangedHackCheckRatio	sChangedRatio;
		pPacket->ReadData(&sChangedRatio, sizeof(ChangedHackCheckRatio));

		if (HACK_TYPE_NO < sChangedRatio.m_ui8HackCheckType &&
			sChangedRatio.m_ui8HackCheckType < HACK_TYPE_MAX)
		{
			DSMUST("Change %s Ratio : %d->%d\n",
				g_pszHackName[sChangedRatio.m_ui8HackCheckType],
				g_pConfig->m_aui8HackCheckLevel[sChangedRatio.m_ui8HackCheckType] * 10,
				sChangedRatio.m_ui8ChangedCheckRatio
				);
			g_pConfig->m_aui8HackCheckLevel[sChangedRatio.m_ui8HackCheckType] = sChangedRatio.m_ui8ChangedCheckRatio / 10;
		}
		else
		{
			DSERROR(" Unknown Hack Type(%d)\n", sChangedRatio.m_ui8HackCheckType);
		}
	}
	break;
	default: 
		DSERROR("[CModuleControlSocket::PacketParsing] Unknown Message(%d)\n", ProtocolID ); break; 
	}
	
	if( bSendMessage )
	{
		if( 0 >= SendPacketMessage( &SendPacket ))
		{
			DSERROR("SendPacketMessage:%d\n", SendPacket.GetProtocolID() );
		}
		
		m_ui32LastSendTime	= i3ThreadTimer::GetServerTime();
	}
	
	m_ui32LastRecvTime	= i3ThreadTimer::GetServerTime();
	
	return;
}
