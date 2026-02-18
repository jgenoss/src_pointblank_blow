#include "pch.h"
#include "TaskProcessor.h"
#include "RingBuffer.h"

void CTaskProcessor::_ParsingServer( INT32 i32Idx, void* pBuffer )
{
	UINT16* pui16Size = (UINT16*)pBuffer;

	i3NetworkPacket Packet;
	Packet.CopyToBuffer( (char*)pBuffer, (*pui16Size + 4) );

	switch( Packet.GetProtocolID() )
	{
	case PROTOCOL_BILLING_CONNECT_ACK:
		{
		}
		break;
	case PROTOCOL_BILLING_HEART_BIT_REQ:
		{
			i3NetworkPacket SendPacket( PROTOCOL_BILLING_HEART_BIT_ACK );
			ServerSendPacket( i32Idx, &SendPacket );
		}
		break;
	case PROTOCOL_BILLING_GET_CASH_REQ:
		{
			UINT8	ui8IDSize;
			char	strID[ MAX_ID_SIZE ];
			INT64	i64UID;

			Packet.ReadData( &ui8IDSize,		sizeof(UINT8) );	if( ui8IDSize > MAX_ID_SIZE )	ui8IDSize = MAX_ID_SIZE;
			Packet.ReadData( strID,				ui8IDSize );
			Packet.ReadData( &i64UID,			sizeof(INT64) );
			strID[ MAX_ID_SIZE-1 ] = '\0';

			TYPE_GET_CASH_IN* pGetCashIn = (TYPE_GET_CASH_IN*)m_pRingDBUserCashGetIn->PushPointer();
			if( pGetCashIn )
			{
				pGetCashIn->_i32ServerIdx	= i32Idx;
				pGetCashIn->_i64UID		= i64UID;
				i3String::Copy( pGetCashIn->_strID, strID, MAX_ID_SIZE );

				m_pRingDBUserCashGetIn->PushPointerIdx();
			}
			else
			{
				INT32	i32ReturnCode	= RETURN_CODE_ERROR_SYSTEM;
				UINT64	ui64Cash		= 0;
				i3NetworkPacket SendPacket( PROTOCOL_BILLING_GET_CASH_ACK );
				SendPacket.WriteData( &ui8IDSize,		sizeof(UINT8)	);
				SendPacket.WriteData( strID,			ui8IDSize		);
				SendPacket.WriteData( &i64UID,			sizeof(INT64)	);
				SendPacket.WriteData( &i32ReturnCode,	sizeof(INT32)	);
				SendPacket.WriteData( &ui64Cash,		sizeof(UINT64)	);
				ServerSendPacket( i32Idx, &SendPacket );

				WriteLog( m_i32LogThreadIdx, "[Task] TYPE_GET_CASH_IN Buffer Full" );
			}
		}
		break;
	case PROTOCOL_BILLING_ITEM_BUY_REQ:
		{
			UINT8	ui8IDSize;
			char	strID[ MAX_ID_SIZE ];
			INT64	i64UID;
			UINT32	ui32Price;
			UINT32	ui32MainBufferIdx;

			Packet.ReadData( &ui8IDSize,			sizeof(UINT8) );	if( ui8IDSize > MAX_ID_SIZE )	ui8IDSize = MAX_ID_SIZE;
			Packet.ReadData( strID,					ui8IDSize );
			Packet.ReadData( &i64UID,				sizeof(INT64) );
			Packet.ReadData( &ui32Price,			sizeof(UINT32) );
			Packet.ReadData( &ui32MainBufferIdx,	sizeof(UINT32) );
			strID[ MAX_ID_SIZE-1 ] = '\0';

			TYPE_ITEM_BUY_IN* pItemBuyIn = (TYPE_ITEM_BUY_IN*)m_pRingDBUserCashBuyIn->PushPointer();
			if( pItemBuyIn )
			{
				pItemBuyIn->_i32ServerIdx		= i32Idx;
				pItemBuyIn->_i64UID				= i64UID;
				pItemBuyIn->_ui32Price			= ui32Price;
				pItemBuyIn->_ui32MainBufferIdx	= ui32MainBufferIdx;
				i3String::Copy( pItemBuyIn->_strID, strID, MAX_ID_SIZE );

				m_pRingDBUserCashBuyIn->PushPointerIdx();
			}
			else
			{
				INT32	i32ReturnCode	= RETURN_CODE_ERROR_SYSTEM;
				UINT64	ui64Cash		= 0;
				i3NetworkPacket SendPacket( PROTOCOL_BILLING_ITEM_BUY_ACK );
				SendPacket.WriteData( &ui8IDSize,			sizeof(UINT8)	);
				SendPacket.WriteData( strID,				ui8IDSize		);
				SendPacket.WriteData( &i64UID,				sizeof(INT64)	);
				SendPacket.WriteData( &i32ReturnCode,		sizeof(INT32)	);
				SendPacket.WriteData( &ui64Cash,			sizeof(UINT64)	);
				SendPacket.WriteData( &ui32MainBufferIdx,	sizeof(UINT32)	);
				ServerSendPacket( i32Idx, &SendPacket );

				WriteLog( m_i32LogThreadIdx, "[Task] TYPE_ITEM_BUY_IN Buffer Full" );
			}
		}
		break;
	default:
		break;
	}
}