#include "pch.h"
#include "ModuleRendezvousSocket.h"
#include "UserManager.h"
#include "UserSession.h"
#include "ModuleLogFile.h"
#include "ModuleContext.h"
#include "ModuleContextNc.h"

I3_CLASS_INSTANCE( CModuleRendezvousSocket, i3NetworkClientSocket );


CModuleRendezvousSocket::CModuleRendezvousSocket(void)
{
	m_bConnect			= FALSE; 	
	m_bNewClanCreate	= FALSE; 
}

CModuleRendezvousSocket::~CModuleRendezvousSocket(void)
{

}

BOOL CModuleRendezvousSocket::OnCreate(char * Address, UINT16 PortNo)
{
	BOOL Rv = i3NetworkClientSocket::OnCreate(Address, PortNo);
	m_iLastRecvTime	= g_pContextMain->GetServerTime(); 	
	return Rv; 
}

void CModuleRendezvousSocket::OnDestroy(void)
{
	m_bConnect = FALSE; 
	i3NetworkClientSocket::OnDestroy(); 
}

void CModuleRendezvousSocket::PacketParsing(i3NetworkPacket  * pPacket)
{
	PROTOCOL	ProtocolID = pPacket->GetProtocolID(); 

	switch(ProtocolID)
	{
	case PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS:		
		{
			pPacket->ReadData(&g_pContextMain->m_RendezvousServerUDPIp, sizeof(UINT32));
			pPacket->ReadData(g_pContextMain->m_RendezvousServerUDPPort, sizeof(UINT16)*RENDEZVOUS_SEND_PORT_COUNT);

			//연결 되었음			
			m_bConnect = TRUE; 
		}
		break; 
	case PROTOCOL_BASE_HEART_BIT_ACK:	
		{
			//심장 소리 
			m_iLastRecvTime	= g_pContextMain->GetServerTime(); 
			break;
		}
		break;
	default: 		
		break; 
	}

	return; 
}