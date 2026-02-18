#include "pch.h"
#include "ModuleTask.h" 
#include "ModuleNet.h"

void CModuleTask::_ParshingPCCafe( PCCAFE_OUTPUT* pOutBuffer )
{
	switch( pOutBuffer->m_eOutputType )
	{
	case TYPE_PCCAFE_OUTPUT_FIRSTLOGIN:
		{
			m_pMainBufferLogin->SetRecvResult( pOutBuffer );
		}
		break;
	case TYPE_PCCAFE_OUTPUT_SUBTRACTION:
		{
			PACKET_PCCAFE_SUBTRACTION_ACK SubAck;
			SubAck.m_TUID			= pOutBuffer->m_TUID;
			SubAck.m_i32BillingGUID	= pOutBuffer->m_ui32PCCafeBillingUID;
			
			NET_PACKET_BUFFER SendPacket( PROTOCOL_MASK_PCCAFE_SUBTRACTION_ACK );
			SendPacket.WriteData( &SubAck, sizeof(PACKET_PCCAFE_SUBTRACTION_ACK) );
			g_pModuleNet->SendPush( &SendPacket );
		}
		break;
	case TYPE_PCCAFE_OUTPUT_CONNECT:
		{
			NET_PACKET_BUFFER SendPacket( PROTOCOL_MASK_PCCAFE_CONNECT_ACK );
			g_pModuleNet->SendPush( &SendPacket );
		}
		break;
	case TYPE_PCCAFE_OUTPUT_DISCONNECT:
		{
			NET_PACKET_BUFFER SendPacket( PROTOCOL_MASK_PCCAFE_DISCONNECT_ACK );
			g_pModuleNet->SendPush( &SendPacket );
		}
		break;
	case TYPE_PCCAFE_OUTPUT_LOGIN:
		{
			PACKET_PCCAFE_LOGIN_ACK LoginAck;
			LoginAck.m_TUID				= pOutBuffer->m_TUID;
			LoginAck.m_ui32PCCafeGUID	= pOutBuffer->m_ui32PCCafeGUID;
			LoginAck.m_ui32BillingUID	= pOutBuffer->m_ui32PCCafeBillingUID;
			LoginAck.m_ui8PCCafe 		= pOutBuffer->m_ui8PCCafe;

			NET_PACKET_BUFFER SendPacket( PROTOCOL_MASK_PCCAFE_LOGIN_ACK );
			SendPacket.WriteData( &LoginAck, sizeof(PACKET_PCCAFE_LOGIN_ACK) );
			g_pModuleNet->SendPush( &SendPacket );
		}
		break;
	case TYPE_PCCAFE_OUTPUT_EMPTY_AMOUNT:
		{
			PACKET_PCCAFE_EMPTY_AMOUNT_ACK AmountAck;
			AmountAck.m_TUID		= pOutBuffer->m_TUID;

			NET_PACKET_BUFFER SendPacket( PROTOCOL_MASK_PCCAFE_EMPTY_AMOUNT_ACK );
			SendPacket.WriteData( &AmountAck, sizeof(PACKET_PCCAFE_EMPTY_AMOUNT_ACK) );
			g_pModuleNet->SendPush( &SendPacket );
		}
		break;
	case TYPE_PCCAFE_OUTPUT_CHECK_AMOUNT:
		{
			PACKET_PCCAFE_CHECK_AMOUNT_ACK AmountAck;
			AmountAck.m_TUID			= pOutBuffer->m_TUID;
			AmountAck.m_ui32PCCafeGUID	= pOutBuffer->m_ui32PCCafeGUID;
			AmountAck.m_ui32BillingUID	= pOutBuffer->m_ui32PCCafeBillingUID;
			AmountAck.m_ui8PCCafe		= pOutBuffer->m_ui8PCCafe;

			NET_PACKET_BUFFER SendPacket( PROTOCOL_MASK_PCCAFE_CHECK_AMOUNT_ACK );
			SendPacket.WriteData( &AmountAck, sizeof(PACKET_PCCAFE_CHECK_AMOUNT_ACK) );
			g_pModuleNet->SendPush( &SendPacket );
		}
		break;
	}
}