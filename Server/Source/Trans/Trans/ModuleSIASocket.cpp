#include "pch.h"
#include "Config.h"
#include "ModuleSIA.h"
#include "ModuleSIASocket.h"
#include "TaskProcessor.h"

I3_CLASS_INSTANCE( CModuleSIASocket, i3ElementBase );

CModuleSIASocket::CModuleSIASocket(void)
{
	m_socket					= INVALID_SOCKET;
	m_recvEvent					= INVALID_HANDLE_VALUE;
	m_i32receivedPacketSize		= 0;
	m_bConnect					= FALSE;
	m_ui32ControlSendTime		= 0;
	m_ui32LastRecvTime			= 0;
}

CModuleSIASocket::~CModuleSIASocket(void)
{
	OnDestroy();
}

BOOL CModuleSIASocket::OnCreate(char* pServerIp, UINT16 ui16ServerPort, INT32 i32LogIdx )
{
	return OnCreate( inet_addr(pServerIp), ui16ServerPort, i32LogIdx );
}

BOOL CModuleSIASocket::OnCreate(UINT32 serverIp, UINT16 ui16serverPort, INT32 i32LogIdx)
{
	m_i32LogIdx = i32LogIdx;
	// 리시브 이벤트 생성
	m_recvEvent = ::WSACreateEvent();

	// 소켓 생성
	m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_socket)return FALSE;

	// 서버 접속
	struct sockaddr_in serverAddr;
	i3mem::FillZero(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = serverIp;
	serverAddr.sin_port = ::htons(ui16serverPort);

	INT32 i32result = ::connect(m_socket, (sockaddr*)&serverAddr, sizeof(serverAddr));

	INT32 i32optVal = 1024*1024*2;
	INT32 i32optLen = sizeof(INT32);
	setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (char*)&i32optVal, i32optLen );
	setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char*)&i32optVal, i32optLen );

	if (SOCKET_ERROR == i32result)
	{
		return FALSE;
	}

	// 리시브 관련 멤버 초기화
	m_i32receivedPacketSize = 0;

	return TRUE;
}

void CModuleSIASocket::OnDestroy(void)
{
	if ( m_bConnect )
	{
		S2RingBuffer* pRing = g_pTaskProcessor->GetRingSIAPop();
		TASK_INVEN_BUFFER* pBuffer = (TASK_INVEN_BUFFER*)pRing->PushPointer();
		if( NULL != pBuffer )
		{
			// SIA서버와 연결 끊어짐
			pBuffer->m_ui8Type			= TASK_INVEN_TYPE_PCCAFE;
			pBuffer->m_ui8Action		= TYPE_PCCAFE_DISCONNECT;

			pRing->PushPointerIdx();
		}
	}

	m_bConnect = FALSE;
	if (INVALID_HANDLE_VALUE != m_recvEvent)
	{
		::WSACloseEvent(m_recvEvent);
		m_recvEvent = INVALID_HANDLE_VALUE;
	}
	if (INVALID_SOCKET != m_socket)
	{
		::shutdown(m_socket, SD_BOTH);
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
	m_i32receivedPacketSize = 0;
}

void CModuleSIASocket::OnReceive(void)
{
	INT32 receivedByte = ::recv(m_socket, &m_receiveBuffer[m_i32receivedPacketSize], (TCP_SOCKET_BUFFER_SIZE - m_i32receivedPacketSize), 0);

	if (SOCKET_ERROR == receivedByte)
	{
		OnDestroy();
		return;		
	}
	
	if (0 == receivedByte)return;
	
	// 받은 데이터 크기 누적
	m_i32receivedPacketSize += receivedByte;

	// 버퍼상의 파싱 시작 위치 
	INT32	i32startIdx = 0;
	INT32	i32ReadSize;
	BOOL	bRead	 = TRUE; 
	while(bRead)
	{
		i32ReadSize = PacketParsing( &m_receiveBuffer[i32startIdx],  m_i32receivedPacketSize - i32startIdx);
		if(i32ReadSize == 0)bRead = FALSE;

		i32startIdx = i32startIdx + i32ReadSize; 
		if(i32startIdx >= m_i32receivedPacketSize)bRead = FALSE; 
	}

	//Data Rearrange
	m_i32receivedPacketSize -= i32startIdx;
	if (0 < i32startIdx && 0 < m_i32receivedPacketSize)
	{
		memmove(m_receiveBuffer, m_receiveBuffer + i32startIdx, m_i32receivedPacketSize);
	}

	return;
}

INT32 CModuleSIASocket::SendPacketMessage(char * pPacket, INT32 i32size)
{
	// 소켓이 닫혔으면 보네지 않는다.
	if (INVALID_SOCKET == m_socket)	return 0;

	// 패킷을 다 보낼때까지 반복한다.
	INT32 i32sendIdx = 0;
	while(i32size > i32sendIdx)
	{
		INT32 i32sendedByte = ::send(m_socket, pPacket + i32sendIdx, i32size - i32sendIdx, 0);

		// 전송 실패 
		if (SOCKET_ERROR == i32sendedByte)return ::WSAGetLastError();

		// 연결이 끊어졌다.
		if (0 == i32sendedByte)return 0;

		// 전송 바이트만큼 인덱스 증가
		i32sendIdx += i32sendedByte;
	}

	return i32sendIdx;
}

void CModuleSIASocket::SelectEvent(void)
{
	WSANETWORKEVENTS event;
	i3mem::FillZero(&event, sizeof(event));

	// 처리할 이벤트를 확인한다.
	if (SOCKET_ERROR != ::WSAEventSelect(m_socket, m_recvEvent, FD_WRITE | FD_READ | FD_CLOSE))
	{
		if (SOCKET_ERROR != ::WSAEnumNetworkEvents(m_socket, m_recvEvent, &event))
		{
			if(event.lNetworkEvents & FD_READ)	OnReceive();
			if(event.lNetworkEvents & FD_CLOSE) OnDestroy();
			return;
		}
	}
	return;
}

BOOL CModuleSIASocket::IsConnected(void) const
{
	if (INVALID_SOCKET == m_socket)	return FALSE;	
	return TRUE;
}

INT32 CModuleSIASocket::PacketParsing( char * pBuffer, INT32 Packetsize )
{
	if( Packetsize < 6 )	return 0;
	if (pBuffer == NULL)
	{
		g_pLog->WriteLog( L"[CModuleSIASocket::PacketParsing] pBuffer is NULL!!" );
		return 0;
	}

	SIA_SEND_MESSAGE* pPacket = (SIA_SEND_MESSAGE*)pBuffer;
	if( Packetsize < pPacket->_ui16TotalSize )	return 0;

	INT32 Rv = pPacket->_ui16TotalSize; 
	pPacket->Clear();
	
	switch( pPacket->_ui16Protocol )
	{
	case PROTOCOL_MASK_HEARTBIT:
		{
		}
		break;
	case PROTOCOL_MASK_CONNECT:
		{
			m_bConnect = TRUE;
		}
		break;
	case PROTOCOL_MASK_LOGIN:
		{
			ST_PACKET_LOGIN_ACK LoginAck;
			pPacket->ReadData( &LoginAck,	sizeof(ST_PACKET_LOGIN_ACK) );
			LoginAck.m_strID[ NET_ID_SIZE-1 ] = '\0';

			S2RingBuffer* pRing = g_pTaskProcessor->GetRingSIAPop();
			TASK_INVEN_BUFFER* pSIA = (TASK_INVEN_BUFFER*)pRing->PushPointer();
			if( NULL == pSIA ) break;

			pSIA->m_ui8Type				= TASK_INVEN_TYPE_LOGIN;
			pSIA->m_i64UID				= LoginAck.m_TUID;
			pSIA->m_i32Result			= LoginAck.m_i32Result; 
			pSIA->m_ui8PBVer			= LoginAck.m_ui8PBVer;
			pSIA->m_i32ServerIdx		= LoginAck.m_i32ServerIdx;
			pSIA->m_i32SessionIdx		= LoginAck.m_i32SessionIdx;
			pSIA->m_ui32AuthKey			= LoginAck.m_ui32AuthKey;
			pSIA->m_ui64UniqueKey		= LoginAck.m_ui64UniqueKey;						// 인증키
			pSIA->m_ui64BillingIdx		= LoginAck.m_ui64BillingIdx;	// 러시아 BillingIdx
			pSIA->m_ui32IP				= LoginAck.m_ui32IP; 
			pSIA->m_ui32Cash			= LoginAck.m_i32Cash;
			pSIA->m_ui8PCCafe			= LoginAck.m_ui8PCCafe;
			pSIA->m_ui32Birthday		= LoginAck.m_ui32Birthday;
			pSIA->m_ui32LoginTime		= LoginAck.m_ui32LoginTime;
			pSIA->m_ui8LinkThreadIdx	= LoginAck.m_ui8LinkThreadIdx;
			pSIA->m_i8UIDCreateType		= LoginAck.m_i8UIDCreateType;
			pSIA->m_i8LogOutSendType	= LoginAck.m_i8LogOutSendType;
			pSIA->m_ui8LocaleCode		= LoginAck.m_ui8LocaleCode;
			pSIA->m_ui8Country			= LoginAck.m_ui8Country;

			pSIA->m_ui32PCCafeGUID		= LoginAck.m_ui32PCCafeGUID;
			pSIA->m_ui32PCCafeBillingUID= LoginAck.m_ui32PCCafeBillingUID;
			pSIA->m_ui8LinkMethod		= LoginAck.m_ui8LinkMethod;

			i3String::Copy( pSIA->m_strID, LoginAck.m_strID, NET_ID_SIZE );
			pSIA->m_strID[NET_ID_SIZE-1] = '\0';

			i3mem::Copy( pSIA->m_strBillingID,	LoginAck.m_strBillingID,	BILLING_ID_SIZE );

			i3mem::Copy( pSIA->m_strExtUID, LoginAck.m_strExtUID, EXT_UID_SIZE );

			i3mem::Copy( pSIA->m_strWebLoginKey, LoginAck.m_strWebLoginKey, WEB_LOGINKEY_SIZE );

			i3mem::Copy( pSIA->m_aui8ShutDownTime, LoginAck.m_aui8ShutDownTime, sizeof(UINT8) * SHUTDOWN_TIME_SIZE );	

			pRing->PushPointerIdx();
		}
		break;
	case PROTOCOL_MASK_LOGIN_WAIT:
		{
		}
		break;
	case PROTOCOL_MASK_BUY_GOODS:
		{
			S2RingBuffer* pRing = g_pTaskProcessor->GetRingSIAPop();
			TASK_INVEN_BUFFER* pSIA = (TASK_INVEN_BUFFER*)pRing->PushPointer();

			PACKET_BUY_GOODS_ACK	BuyAck;
			pPacket->ReadData( &BuyAck,	sizeof(PACKET_BUY_GOODS_BASE_ACK));
			pPacket->ReadData( BuyAck.m_i32Rv,		sizeof(INT32) * BuyAck.m_ui8GoodsCount );
			pPacket->ReadData( BuyAck.m_aGoodsID,	sizeof(T_GoodsID) * BuyAck.m_ui8GoodsCount );

			pSIA->m_ui8Type						= TASK_INVEN_TYPE_BUY_GOODS;
			pSIA->m_i64UID						= BuyAck.m_TUID;
			pSIA->m_ui8GoodsCount				= BuyAck.m_ui8GoodsCount;
			i3mem::Copy( pSIA->m_i32Rv, BuyAck.m_i32Rv, sizeof( INT32 ) * BuyAck.m_ui8GoodsCount );
			i3mem::Copy( pSIA->m_aGoodsID, BuyAck.m_aGoodsID, sizeof( T_GoodsID ) * BuyAck.m_ui8GoodsCount );

			pSIA->m_ui32CurrentUserCash			= BuyAck.m_ui32CurrentUserCash;
			pSIA->m_ui32CurrentUserCashType		= BuyAck.m_ui32CurrentUserCashType;

			pSIA->m_ui8Action					= BuyAck.m_ui8Action;

			switch( pSIA->m_ui8Action )
			{
			case TYPE_GACHA_PURCHASE:
				{
					pPacket->ReadData( &pSIA->m_stGachaBuffer,					sizeof(GACHA_PURCHACE_BASE) );
					pPacket->ReadData( &pSIA->m_stGachaBuffer.m_TTicketDBIdx,	sizeof(T_ItemDBIdx));
				}
				break;
			case TYPE_SHOP_CHARA_BUY:
				{
					pPacket->ReadData( &pSIA->m_stCharaBuffer, sizeof(BUFFER_BUY_CHARA) );
				}
				break;
			case TYPE_SHOP_REPAIR:
				{
					pPacket->ReadData( pSIA->m_stRepairBuffer.m_stRepairItem, sizeof(REPAIR_ITEM) * BuyAck.m_ui8GoodsCount );
					pSIA->m_stRepairBuffer.m_ui8ItemCount = BuyAck.m_ui8GoodsCount;
				}
				break;
			case TYPE_SHOP_GIFT_BUY:
				{
					pPacket->ReadData( &pSIA->m_stGiftBuffer.m_TReceiverUID, sizeof(T_UID) );
					for( UINT8 i = 0 ; i < BuyAck.m_ui8GoodsCount; ++i )
					{
						pPacket->ReadData( &pSIA->m_stGiftBuffer.m_aGiftDBIdx[i], sizeof(T_GiftDBIdx) );
					}
					pPacket->ReadData( &pSIA->m_stGiftBuffer.m_ui8MessageSize, sizeof(UINT8) );
					pPacket->ReadData( &pSIA->m_stGiftBuffer.m_strMessage, sizeof(TTCHAR) * pSIA->m_stGiftBuffer.m_ui8MessageSize );

					pSIA->m_stGiftBuffer.m_strMessage[ pSIA->m_stGiftBuffer.m_ui8MessageSize] = 0;
				}
				break;
			case TYPE_SHOP_GOODS_BUY:
			case TYPE_SHOP_ITEM_EXTEND:
				{
					pPacket->ReadData(  &pSIA->m_stShopBuffer.m_TItemDBIdx,		sizeof(T_ItemDBIdx) );
					for( UINT8 i = 0 ; i < BuyAck.m_ui8GoodsCount; ++i )
					{
						BUY_GOODS *pGoods = &pSIA->m_stShopBuffer.m_stDBGoods[i];
						pPacket->ReadData(  &pGoods->m_ui8ItemCount,		sizeof(UINT8) );  
						pPacket->ReadData(  pGoods->m_aItemDBIdx,			sizeof(T_ItemDBIdx) * pGoods->m_ui8ItemCount );  
					}
					 
				}
				break;
			};
	
			pRing->PushPointerIdx();
		}
		break;

	case PROTOCOL_MASK_GET_CASH:
		{
			S2RingBuffer* pRing = g_pTaskProcessor->GetRingSIAPop();
			TASK_INVEN_BUFFER* pSIA = (TASK_INVEN_BUFFER*)pRing->PushPointer();

			PACKET_GET_CASH_ACK	GetCashAck;
			pPacket->ReadData( &GetCashAck,	sizeof(PACKET_GET_CASH_ACK));

			pSIA->m_ui8Type					= TASK_INVEN_TYPE_GET_CASH;
			pSIA->m_i32Result				= GetCashAck.m_i32Result;
			pSIA->m_i64UID					= GetCashAck.m_TUID;
			if( EV_SUCCESSED( pSIA->m_i32Result ))
			{
				pSIA->m_ui32Cash				= GetCashAck.m_ui32Cash;
			}	
		
			pRing->PushPointerIdx();
		}
		break;
	

	case PROTOCOL_MASK_USE_GIFTCOUPON:
		{
			S2RingBuffer* pRing = g_pTaskProcessor->GetRingSIAPop();
			TASK_INVEN_BUFFER* pSIA = (TASK_INVEN_BUFFER*)pRing->PushPointer();
			if( NULL == pSIA ) break;

			PACKET_USE_GIFTCOUPON_ACK UseCouponAck;
			pPacket->ReadData( &UseCouponAck,	sizeof(PACKET_USE_GIFTCOUPON_ACK));

			pSIA->m_ui8Type			= TASK_INVEN_TYPE_USE_GIFTCOUPON;
			pSIA->m_i32Result		= UseCouponAck.m_i32Result;
			pSIA->m_i64UID			= UseCouponAck.m_TUID;
			i3mem::Copy( pSIA->m_strCouponCode, UseCouponAck.m_strCouponCode, GIFTCOUPON_LENGTH + 1);
			if( EV_SUCCESSED( pSIA->m_i32Result ))
			{
				pSIA->m_ui32Cash			= UseCouponAck.m_ui32IncCash;
				pSIA->m_ui8GoodsCount	= UseCouponAck.m_i8GoodsCount;
				i3mem::Copy( pSIA->m_i32Rv, UseCouponAck.m_ai32GoodsID, sizeof(INT32)*UseCouponAck.m_i8GoodsCount );
			}	
		
			pRing->PushPointerIdx();
		}
		break;

	case PROTOCOL_MASK_PCCAFE_SUBTRACTION_ACK:
		{
			S2RingBuffer* pRing = g_pTaskProcessor->GetRingSIAPop();
			TASK_INVEN_BUFFER* pSIA = (TASK_INVEN_BUFFER*)pRing->PushPointer();
			if( NULL == pSIA ) break;

			PACKET_PCCAFE_SUBTRACTION_ACK SubtractionAck;
			pPacket->ReadData( &SubtractionAck, sizeof(PACKET_PCCAFE_SUBTRACTION_ACK) );

			pSIA->m_ui8Type			= TASK_INVEN_TYPE_PCCAFE;
			pSIA->m_ui8Action		= TYPE_PCCAFE_SUBTRACTION;

			i3mem::Copy( &pSIA->m_Subtraction, &SubtractionAck, sizeof(PACKET_PCCAFE_SUBTRACTION_ACK) );

			pRing->PushPointerIdx();
		}
		break;

	case PROTOCOL_MASK_PCCAFE_CONNECT_ACK:
		{
			S2RingBuffer* pRing = g_pTaskProcessor->GetRingSIAPop();
			TASK_INVEN_BUFFER* pSIA = (TASK_INVEN_BUFFER*)pRing->PushPointer();

			// 웹젠연동서버와 재연결 성공
			pSIA->m_ui8Type			= TASK_INVEN_TYPE_PCCAFE;
			pSIA->m_ui8Action		= TYPE_PCCAFE_CONNECT;

			pRing->PushPointerIdx();
		}
		break;

	case PROTOCOL_MASK_PCCAFE_DISCONNECT_ACK:
		{
			S2RingBuffer* pRing = g_pTaskProcessor->GetRingSIAPop();
			TASK_INVEN_BUFFER* pSIA = (TASK_INVEN_BUFFER*)pRing->PushPointer();

			// 웹젠연동서버와 연결 끊어짐
			pSIA->m_ui8Type			= TASK_INVEN_TYPE_PCCAFE;
			pSIA->m_ui8Action		= TYPE_PCCAFE_DISCONNECT;

			pRing->PushPointerIdx();
		}
		break;

	case PROTOCOL_MASK_PCCAFE_LOGIN_ACK:
		{
			S2RingBuffer* pRing = g_pTaskProcessor->GetRingSIAPop();
			TASK_INVEN_BUFFER* pSIA = (TASK_INVEN_BUFFER*)pRing->PushPointer();

			PACKET_PCCAFE_LOGIN_ACK LoginAck;
			pPacket->ReadData( &LoginAck, sizeof(PACKET_PCCAFE_LOGIN_ACK) );

			pSIA->m_ui8Type			= TASK_INVEN_TYPE_PCCAFE;
			pSIA->m_ui8Action		= TYPE_PCCAFE_LOGIN;

			i3mem::Copy( &pSIA->m_Login, &LoginAck, sizeof(PACKET_PCCAFE_LOGIN_ACK) );

			pRing->PushPointerIdx();
		}
		break;

	case PROTOCOL_MASK_PCCAFE_EMPTY_AMOUNT_ACK:
		{
			S2RingBuffer* pRing = g_pTaskProcessor->GetRingSIAPop();
			TASK_INVEN_BUFFER* pSIA = (TASK_INVEN_BUFFER*)pRing->PushPointer();
			if( NULL == pSIA ) break;

			PACKET_PCCAFE_EMPTY_AMOUNT_ACK EmptyAck;
			pPacket->ReadData( &EmptyAck, sizeof(PACKET_PCCAFE_EMPTY_AMOUNT_ACK) );

			pSIA->m_ui8Type			= TASK_INVEN_TYPE_PCCAFE;
			pSIA->m_ui8Action		= TYPE_PCCAFE_EMPTY_AMOUNT;

			i3mem::Copy( &pSIA->m_EmptyAmount, &EmptyAck, sizeof(PACKET_PCCAFE_EMPTY_AMOUNT_ACK) );

			pRing->PushPointerIdx();
		}
		break;

	case PROTOCOL_MASK_PCCAFE_CHECK_AMOUNT_ACK:
		{
			S2RingBuffer* pRing = g_pTaskProcessor->GetRingSIAPop();
			TASK_INVEN_BUFFER* pSIA = (TASK_INVEN_BUFFER*)pRing->PushPointer();

			PACKET_PCCAFE_CHECK_AMOUNT_ACK AmountAck;
			pPacket->ReadData( &AmountAck, sizeof(PACKET_PCCAFE_CHECK_AMOUNT_ACK) );

			pSIA->m_ui8Type			= TASK_INVEN_TYPE_PCCAFE;
			pSIA->m_ui8Action		= TYPE_PCCAFE_CHECK_AMOUNT;

			i3mem::Copy( &pSIA->m_CheckAmount, &AmountAck, sizeof(PACKET_PCCAFE_CHECK_AMOUNT_ACK) );

			pRing->PushPointerIdx();
		}
		break;

	default:
		g_pLog->WriteLog( L"[CModuleSIASocket::PacketParsing]Unknown Message / protocol : %d ", pPacket->_ui16Protocol  );
		break; 
	}

	m_ui32LastRecvTime	= i3ThreadTimer::GetServerTime();

	return Rv; 
}