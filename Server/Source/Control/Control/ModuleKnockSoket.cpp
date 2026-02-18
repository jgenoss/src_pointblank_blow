#include "pch.h"
#include "ModuleKnock.h"
#include "ModuleKnockSoket.h"

I3_CLASS_INSTANCE( CModuleKnockSoket, i3NetworkClientSocket );

CModuleKnockSoket::CModuleKnockSoket(void)
{
	m_bConnect				= FALSE; 
	m_ui32KnockSendTime	=  5;
}


CModuleKnockSoket::~CModuleKnockSoket(void)
{
	OnDestroy();
}

BOOL CModuleKnockSoket::OnCreate(char * pstrAddress, UINT16 ui16PortNo, INT32 i32LogIdx )
{
	m_i32LogIdx = i32LogIdx;

	BOOL bRv = i3NetworkClientSocket::OnCreate(pstrAddress, ui16PortNo);
	m_ui32LastRecvTime	= i3ThreadTimer::GetServerTime();
	return bRv; 
}

void CModuleKnockSoket::OnDestroy(void)
{
	DisConnect();
}

void CModuleKnockSoket::DisConnect()
{
	m_bConnect = FALSE; 
	i3NetworkClientSocket::DisConnect();
}

void CModuleKnockSoket::PacketParsing(i3NetworkPacket  * pPacket)
{
	PROTOCOL	ProtocolID = pPacket->GetProtocolID(); 

	switch(ProtocolID)
	{
	case PROTOCOL_KNOCK_CONNECTIONSUCCESS	://연결되었음
		{
			g_pLog->WriteLog( L"[SUCC] Knock Server Connect" );
			m_bConnect = TRUE;
		}
		break;

	default:
		g_pLog->WriteLog( L"[CModuleControlSocket::PacketParsing]Unknown Message : %d ", ProtocolID  );
		break; 
	}

	return;
}