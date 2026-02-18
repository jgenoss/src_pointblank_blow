#include "pch.h"
#include "UserSession.h"
#include "RoomManager.h"
#include "UdpHolePunching.h"
#include "DataBase.h"
#include "Usermanager.h"
#include "ServerContext.h"
#include "Room.h"
#include "ModuleContextNc.h"
#include "ModuleTrans.h"

BOOL	CUserSession::_PacketParsingRsIgs( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled )
{
	PROTOCOL	ProtocolID = pPacket->GetProtocolID();
	BOOL		bSendMessage = FALSE; 

	switch( ProtocolID )
	{
	case PROTOCOL_RS_ITEM_INFO_REQ:
		break;
	
	case PROTOCOL_FIELDSHOP_GOODSLIST_REQ:
		{
			CHECKVALID_CHANNEL; 

			_SendFieldShopInfo( 0 );
		}
		break;
	case PROTOCOL_FIELDSHOP_OPEN_REQ:
		{
			CHECKVALID_CHANNEL; 

			// Read the version info from client packet
			UINT32	ui32FieldShopVersion = 0;
			UINT8	ui8StageType;

			char strMD5[NET_MD5_KEY_SIZE+1];
			UINT8 ui8SendShopInfo = 0;	// 상점 데이터 전송 여부. 0 : 전송X , 1 : 전송O
			
			pPacket->ReadData(&ui32FieldShopVersion,	sizeof(UINT32));
			pPacket->ReadData(&ui8StageType,			sizeof(UINT8));
			pPacket->ReadData(strMD5,					sizeof(char) * NET_MD5_KEY_SIZE ); 

			strMD5[NET_MD5_KEY_SIZE]	= 0;

			// 상점목록을 보낸다.
			if( TRUE == _SendShopInfo(strMD5) ) 
			{
				ui8SendShopInfo = 1;
			}

			i3NetworkPacket FieldShopOpenAckPacket( PROTOCOL_FIELDSHOP_OPEN_ACK );
			UINT8 ui8Result; // 0: Fail, 1:Ok

			if(ui32FieldShopVersion != g_pContextMain->GetFieldShopVersion())
			{
				// Different version, Send Fail Message to client
				ui8Result = 0; // Fail
				FieldShopOpenAckPacket.WriteData( &ui8SendShopInfo,  sizeof (UINT8) );
				FieldShopOpenAckPacket.WriteData( &ui8Result,  sizeof (UINT8) );
				SendPacketMessage( &FieldShopOpenAckPacket );

				_SendFieldShopInfo(ui32FieldShopVersion);
			}
			else
			{
				// Same version, Send Ok Message to client
				ui8Result = 1; // OK
				FieldShopOpenAckPacket.WriteData( &ui8SendShopInfo,  sizeof (UINT8) );
				FieldShopOpenAckPacket.WriteData( &ui8Result,  sizeof (UINT8) );
				SendPacketMessage( &FieldShopOpenAckPacket );
			}
		}
		break;
	default:
			char strTemp[256];
			sprintf( strTemp, "UnKnown Packet - %d", ProtocolID );
			I3TRACE( strTemp );
			I3TRACE( "UnKnown Packet - Kill User"); 
			bisKilled 	= TRUE; 
			m_ui32KillReason 	= KILL_REASON_UNKNOWN_PACKET;
			break;
	}

	return bSendMessage;
}