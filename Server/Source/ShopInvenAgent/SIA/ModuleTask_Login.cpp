#include "pch.h"
#include "ModuleTask.h" 
#include "ModuleNet.h"

void CModuleTask::_ParshingLogin( LOGIN_OUTPUT* pOutBuffer )
{
	ST_PACKET_LOGIN_ACK Packet;
	i3mem::Copy( &Packet, pOutBuffer, sizeof(ST_PACKET_LOGIN_ACK) );

	NET_PACKET_BUFFER pBuffer( PROTOCOL_MASK_LOGIN );
	pBuffer.WriteData( &Packet,			sizeof( ST_PACKET_LOGIN_ACK ) );
	g_pModuleNet->SendPush( pOutBuffer->m_i32TransServerIdx, &pBuffer);
}