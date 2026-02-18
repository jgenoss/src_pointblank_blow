#include "pch.h"
#include "UserSession.h"

#include "ModuleTrans.h"

BOOL	CUserSession::_PacketParsingChar( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled )
{
	PROTOCOL	ProtocolID	= pPacket->GetProtocolID();	
	BOOL		bSendMessage= FALSE; 
	INT32		i32Temp;
	T_RESULT	TResult;

	switch( ProtocolID )
	{
		case PROTOCOL_CHAR_CREATE_CHARA_REQ:
		{
			CHECKVALID_INFO;

			PACKET_CHAR_CREATE_CHARA_REQ Recv;
			S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );
			
			TResult = _CheckCreateChar( &Recv );

			if( EV_FAILED( TResult ) )
			{
				PACKET_CHAR_CREATE_CHARA_ACK Packet;
				Packet.m_TResult = TResult;
				S2MOSendEvent( &Packet );
			}
		}
		break;	
	case PROTOCOL_CHAR_CHANGE_CHARA_NICK_REQ:
		{
			CHECKVALID_INFO;

			UINT8	ui8CharaSlotCount;
			UINT8	ui8CharaSlotIdx[ S2MO_MULTI_SLOT_COUNT ];								// 캐릭터 슬롯 번호
			TTCHAR	strNewCharaName[ S2MO_MULTI_SLOT_COUNT ][NET_CHARA_NICK_NAME_SIZE];	// 캐릭터 닉네임 16바이트 

			pPacket->ReadData( &ui8CharaSlotCount,		sizeof(UINT8) );
			for( INT32 i = 0 ; i < ui8CharaSlotCount ; i++ )
			{
				pPacket->ReadData( &ui8CharaSlotIdx[i],	sizeof(UINT8) );
				pPacket->ReadData( strNewCharaName[i],	sizeof(TTCHAR) * NET_CHARA_NICK_NAME_SIZE );

				m_MultiSlot.SetCharNick( ui8CharaSlotIdx[i], strNewCharaName[i] );

				ChangeCharaDataBaseInfo( ( (UINT64)0x01 << ui8CharaSlotIdx[i]) );
			}

			PACKET_CHAR_CHANGE_CHARA_NICK_ACK Packet;
			Packet.m_TResult = EVENT_ERROR_SUCCESS;
			S2MOSendEvent( &Packet );
		}
		break;
	case PROTOCOL_CHAR_CHANGE_EQUIP_REQ:
		{
			CHECKVALID_LOBBY;

			PACKET_CHAR_CHANGE_EQUIP_REQ Recv;
			S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

			i32Temp = _CharChangeEquipment( &Recv );

			if (EV_FAILED(i32Temp))
			{	// 원래 장착중이 었던 정보를 전달합니다.
				_SendEquipedInfo();
			}

			PACKET_CHAR_CHANGE_EQUIP_ACK Packet;
			Packet.m_TResult	= i32Temp;
			S2MOSendEvent( &Packet );
		}
		break;
	case PROTOCOL_CHAR_DELETE_CHARA_REQ:
		{
			CHECKVALID_LOBBY;

			UINT8 ui8CharaSlotIdx;
			pPacket->ReadData(&ui8CharaSlotIdx, sizeof(UINT8));

			T_RESULT TCheckResult = _CheckCharaDelete((INT32)ui8CharaSlotIdx);

			if (EV_FAILED(TCheckResult))
			{
				_FailDeleteChara(TCheckResult, ui8CharaSlotIdx);
				break;
			}

			ITEM_INFO CharaItemInfo;
			m_MultiSlot.GetCharEquipment(ui8CharaSlotIdx, CHAR_EQUIPMENT_CHARA, &CharaItemInfo);
			
			if( FALSE == g_pModuleTrans->AuthSendCharaDelete( m_WorkThreadIdx, this, ui8CharaSlotIdx, CharaItemInfo.m_TItemDBIdx ) )
			{
				_FailDeleteChara( EVENT_ERROR_SHOP_CHARA_DEL_FAIL_NETWORK, ui8CharaSlotIdx );
				break;
			}
		}
		break;
	}

	return bSendMessage;
}