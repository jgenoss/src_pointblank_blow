#include "pch.h"
#include "TaskProcessor.h"
#include "ModuleSIA.h"
#include "ModuleControl.h"
#include "ModuleDBShop.h"
#include "UserFinder.h"

//	 변수,체크값
#define LENGTH_CHECK( val, check )	{ if(val>check)	{val=check;} }

void CTaskProcessor::_ParsingGame( UINT32 ui32ServerIdx, i3NetworkPacket* pPacket )
{
	//PROTOCOL XX = pPacket->GetProtocolID(); 
	switch(pPacket->GetProtocolID())
	{
	case PROTOCOL_BASE_HEART_BIT_REQ		: 
		{
			i3NetworkPacket	SendPacket(PROTOCOL_BASE_HEART_BIT_ACK);
			SendGamePacket(ui32ServerIdx, &SendPacket);
		}
		break;
	case PROTOCOL_LOGIN_REQ						:
		{
			UINT8	ui8IDSize;;
			UINT8	ui8PassSize;
					
			ST_PACKET_LOGIN_REQ stLogInReq;

			stLogInReq.m_i32ServerIdx	= ui32ServerIdx;
			
			pPacket->ReadData( &stLogInReq.m_ui8PBVer,				sizeof(UINT8) );
			pPacket->ReadData( &stLogInReq.m_i32SessionIdx,			sizeof(INT32) );
			pPacket->ReadData( &stLogInReq.m_ui32AuthKey,			sizeof(UINT32) );
			pPacket->ReadData( &stLogInReq.m_ui32IP,				sizeof(UINT32) );
			pPacket->ReadData( &stLogInReq.m_ui64MacAddress,		sizeof(UINT64) );
			pPacket->ReadData( &stLogInReq.m_ui8LocaleCode,			sizeof(UINT8) );
			pPacket->ReadData( &ui8IDSize,							sizeof(UINT8) );	LENGTH_CHECK( ui8IDSize,	NET_ID_SIZE );
			pPacket->ReadData( &ui8PassSize,						sizeof(UINT8) );	LENGTH_CHECK( ui8PassSize,	NET_PW_SIZE );
			pPacket->ReadData( stLogInReq.m_strID,					ui8IDSize );
			pPacket->ReadData( stLogInReq.m_strPW,					ui8PassSize );
			pPacket->ReadData( &stLogInReq.m_ui16SessionKeySize,	sizeof(UINT16) );	LENGTH_CHECK( stLogInReq.m_ui16SessionKeySize,	NET_ENCRYPTED_TOKEN_SIZE );
			pPacket->ReadData( stLogInReq.m_pSessionKey,			stLogInReq.m_ui16SessionKeySize );
			pPacket->ReadData(&stLogInReq.m_ui8LinkMethod,			sizeof(UINT8));
			
			stLogInReq.m_strID[ NET_ID_SIZE - 1 ]		= '\0'; 
			stLogInReq.m_strPW[ NET_PW_SIZE - 1 ]		= '\0';
			stLogInReq.m_pSessionKey[ stLogInReq.m_ui16SessionKeySize ] = '\0';

			_UserLogin( &stLogInReq );
		}
		break;
	case PROTOCOL_AUTH_LOGOUT_REQ		: 
		{
			INT32 iSessionIdx;
			T_UID i64UID; 
			UINT32 ui32AuthKey;
			pPacket->ReadData( &iSessionIdx,		sizeof(INT32) );
			pPacket->ReadData( &i64UID,			sizeof(T_UID) );
			pPacket->ReadData( &ui32AuthKey,		sizeof(UINT32) );

			// DBIdx 가 -1 인 유저는 로그인 처리가 완료되지 않았으므로 그냥 종료시킵니다.
			/*if( SERVER_VALUE_UNKNOWN == i64UID )
			{
			m_pLoginManager->DisableUser( ui32AuthKey );
			}
			else
			{*/
			USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID(i64UID); 

			if(pNode != NULL)
			{
				_SaveUserInfo(pNode, pPacket, 1, ui32ServerIdx);
				_LogoutCommand( pNode );
			}
			else if( SERVER_VALUE_UNKNOWN != i64UID ) 
			{
				USER_NODE Node;
				i3mem::FillZero( &Node, sizeof( USER_NODE ));
				Node._i32Serveridx	= ui32ServerIdx;
				Node._i32Sessionidx	= iSessionIdx;
				Node._i64UID		= i64UID;
				Node._ui32State		= USER_STATE_PLAYING;
				_SaveUserInfo(&Node, pPacket, 2, ui32ServerIdx);

				USER_NODE * pInsertNode = g_pUserFinder->Insert( &Node );

				if( pInsertNode )
				{
					_LogoutCommand( pInsertNode );
				}
				else
				{
					g_pLog->WriteLog( L"[ERROR]LogOut Insert Fail" );
				}
#ifdef USE_LOG_KILLREASON
				g_pLog->WriteLog( L"[ERROR]PROTOCOL_AUTH_LOGOUT_REQ UserNone Server:%d, SID:%d, UID:%I64d", iServerIdx, iSessionIdx, i64UID );
#endif
			}
			/*}*/
		}
		break; 
	case PROTOCOL_BASE_GET_USER_INFO_REQ	: 
		{
			INT32 i32Rv = EVENT_ERROR_SUCCESS;
			INT32 i32SessionIdx;
			T_UID i64UID;
			UINT32 ui32ClientIP;

			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(T_UID));
			pPacket->ReadData(&ui32ClientIP,	sizeof(UINT32));

			USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID( i64UID ); 
			if( NULL == pNode )
			{
				i32Rv = EVENT_ERROR_FAIL;

				g_pLog->WriteLog( L"[ERROR] PROTOCOL_BASE_GET_USER_INFO_REQ UID Server=%d, iUID=%I64d", ui32ServerIdx, i64UID );
			}
			else if( USER_STATE_LOGIN != pNode->_ui32State )		// Login 상태가 아니면 메시지가 올 수 없습니다.
			{
				i32Rv = EVENT_ERROR_FAIL;

				g_pLog->WriteLog( L"[ERROR] PROTOCOL_BASE_GET_USER_INFO_REQ STATE Server=%d, iUID=%I64d, State=%d", ui32ServerIdx, i64UID, pNode->_ui32State );
			}
			else
			{
				pNode->_IpAddress	= ui32ClientIP;

				i32Rv = m_pDBUserLoad->PushLoadInfo( pNode );
			}

			if( EV_FAILED( i32Rv ) )
			{
				i3NetworkPacket	SendPacket( PROTOCOL_BASE_GET_USER_INFO_ACK );
				SendPacket.WriteData( &i32SessionIdx,	sizeof( INT32 ) );
				SendPacket.WriteData( &i64UID,			sizeof( T_UID ) );
				SendPacket.WriteData( &i32Rv,			sizeof( INT32 ) );
				SendGamePacket( ui32ServerIdx, &SendPacket );
			}
		}
		break;	
	case PROTOCOL_BASE_USER_ENTER_REQ:
		{
			INT32		i32Rv = EVENT_ERROR_SUCCESS;
			T_UID		i64UID;
			INT32		i32SessionIdx;
			UINT16		ui16ItemCount = 0;

			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32) );
			pPacket->ReadData(&i64UID,			sizeof(T_UID) );

			USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID( i64UID );

			if( NULL == pNode ) 
			{
				i32Rv = EVENT_ERROR_FAIL;

				//이하단 체크 합니다.				
				g_pLog->WriteLog( L"[ERROR]USER_ENTER UID Error Server=%d, iUID=%I64d", ui32ServerIdx, i64UID );
			}
			else if( pNode->_ui32State != USER_STATE_MOVING )				// 이동중이 아니면 이 메시지는 유효하지 않습니다.
			{
				i32Rv = EVENT_ERROR_FAIL;

				g_pLog->WriteLog( L"[ERROR]USER_ENTER STATE Error Server=%d, iUID=%I64d, State=%d", ui32ServerIdx, i64UID, pNode->_ui32State  );
			}
			else
			{
				//Update Position
				pNode->_i32Serveridx		= ui32ServerIdx;
				pNode->_i32Sessionidx		= i32SessionIdx;
				pNode->_ui32State			= USER_STATE_PLAYING;
				pNode->_ui32ConnectState	= MAKE_FRIEND_STATE( FRIEND_STATE_ONLINE, ui32ServerIdx, 0, 0 );
				pNode->_LastRecvTimeST		= i3ThreadTimer::GetServerTime();

				// 이상한 아이템을 걸러낸다.
				for(INT32 i = 0 ; i < MAX_INVEN_COUNT ; i++ )
				{
					if( 0 == pNode->_Inven[ i ]._ui32ItemID )							continue;
					ui16ItemCount++;

					if( ITEM_ATTR_TYPE_AUTH != pNode->_Inven[ i ]._ui8ItemType )		continue;
					if( i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI() <= pNode->_Inven[ i ]._ui32ItemArg )	continue;

					SHOP_ITEM_BASE*	pItem = g_pShop->GetItemData( pNode->_Inven[ i ]._ui32ItemID );
					if( NULL == pItem )													continue;
					if( ITEM_EXPIRE_LEAVE == pItem->_ui8AuthExpireType )				continue;

					i3mem::FillZero( &pNode->_Inven[ i ], sizeof( INVEN_BUFFER ) );
					ui16ItemCount--;
				}

				pNode->_ServerLeaveTimeST = 0;
			}

			i3NetworkPacket	SendPacket(PROTOCOL_BASE_USER_ENTER_ACK);
			SendPacket.WriteData( &i32SessionIdx,		sizeof( INT32 ) );
			SendPacket.WriteData( &i64UID,				sizeof( T_UID ) );
			SendPacket.WriteData( &i32Rv,				sizeof( INT32 ) );

			//인벤토리 넣기 
			if( EV_SUCCESSED( i32Rv ) )
			{
				SendPacket.WriteData( &pNode->_ui8LocaleCode,	sizeof(UINT8));
				SendPacket.WriteData( pNode->_strID,			NET_ID_SIZE);

				_MakeUserInfoPacket( &SendPacket, pNode, TRUE);	// 서버 이동시에는 클랜 Load O.

				_SendUserCharaPacket( pNode);
				_SendUserInvenPacket( pNode, ui16ItemCount );
			}

			SendGamePacket(ui32ServerIdx, &SendPacket);
		}
		break;
	case PROTOCOL_BASE_USER_LEAVE_REQ:
		{
			INT32		i32Rv = EVENT_ERROR_SUCCESS;
			INT32		i32SessionIdx;
			T_UID		i64UID;
			UINT8		ui8LocaleCode;

			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32) );
			pPacket->ReadData(&i64UID,			sizeof(T_UID) );
			pPacket->ReadData(&ui8LocaleCode,	sizeof(UINT8) );

			//1. 정보 없으면 에러입니다. 
			USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID( i64UID );
			if( NULL == pNode )
			{
				i32Rv = EVENT_ERROR_FAIL;

				g_pLog->WriteLog( L"[ERROR]USER_LEAVE UID Error Server=%d, iUID=%I64d", ui32ServerIdx, i64UID  );
			}
			else if( USER_STATE_PLAYING != pNode->_ui32State )				// 게임중이 아니면 이동 할 수 없습니다.
			{
				i32Rv = EVENT_ERROR_FAIL;

				g_pLog->WriteLog( L"[ERROR]USER_LEAVE STATE Error Server=%d, iUID=%I64d, State=%d", ui32ServerIdx, i64UID, pNode->_ui32State );
			}
			else
			{
				pNode->_ui8LocaleCode = ui8LocaleCode;
				_SaveUserInfo(pNode, pPacket, 3, ui32ServerIdx);
				pNode->_ui32State			= USER_STATE_MOVING;						// 서버 이동으로 상태 변환

				pNode->_ui32ConnectState	= MAKE_FRIEND_STATE( FRIEND_STATE_ONLINE, ui32ServerIdx, 0, 0 );

				pNode->_ServerLeaveTimeST	= i3ThreadTimer::GetServerTime();
			}

			//3. 패킷보내기 
			i3NetworkPacket	SendPacket(PROTOCOL_BASE_USER_LEAVE_ACK);
			SendPacket.WriteData( &i32SessionIdx,	sizeof( INT32 ) );
			SendPacket.WriteData( &i64UID,			sizeof( T_UID ) );
			SendPacket.WriteData( &i32Rv,			sizeof( INT32 ) );
			SendGamePacket(ui32ServerIdx, &SendPacket);
		}
		break;

	case PROTOCOL_AUTH_CHARA_DB_SAVE_REQ:
		{
			T_UID		i64UID;
			UINT8		ui8Count = 0 ;

			pPacket->ReadData(&i64UID,			sizeof(T_UID) );
			pPacket->ReadData(&ui8Count,		sizeof(UINT8) );
		
			//1. 정보 없으면 에러입니다. 
			USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID( i64UID );
			if( NULL == pNode )
			{
				g_pLog->WriteLog( L"[ERROR] CHARA_DB_SAVE  UID Error Server=%d, iUID=%I64d, Count=%d", ui32ServerIdx, i64UID, ui8Count  );
				break;
			}

			ui8Count = MIN( ui8Count, S2MO_MULTI_SLOT_PACKET_COUNT );
			_SaveCharaInfo(pNode, ui8Count, pPacket, ui32ServerIdx);
		}
		break;
		// PROTOCOL_LOBBY
	case PROTOCOL_LOBBY_ENTER_REQ:
		{
			T_UID	i64UID;
			INT32	i32Channelidx;
			
			pPacket->ReadData( &i64UID, sizeof(T_UID) );
			pPacket->ReadData( &i32Channelidx, sizeof(INT32) );

			USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID( i64UID );

			if( NULL == pNode ) 
			{						
				g_pLog->WriteLog( L"[ERROR]USER_LOBBY_ENTER UID Error Server=%d, iUID=%I64d", ui32ServerIdx, i64UID );
			}
			else
			{
				pNode->_ui32ConnectState = MAKE_FRIEND_STATE( FRIEND_STATE_LOBBY, GET_FRIEND_SERVER(pNode->_ui32ConnectState), i32Channelidx, 0 );
			}
		}
		break;
	case PROTOCOL_LOBBY_LEAVE_REQ:
		{
			T_UID i64UID;

			pPacket->ReadData( &i64UID, sizeof(T_UID) );

			USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID( i64UID );

			if( NULL == pNode ) 
			{				
				g_pLog->WriteLog( L"[ERROR]USER_LOBBY_LEAVE UID Error Server=%d, iUID=%I64d", ui32ServerIdx, i64UID );
			}
			else
			{
				pNode->_ui32ConnectState = MAKE_FRIEND_STATE( FRIEND_STATE_ONLINE, GET_FRIEND_SERVER(pNode->_ui32ConnectState), 0, 0 );
			}

		}
		break;
		// PROTOCOL_ROOM
	case PROTOCOL_ROOM_JOIN_REQ:
		{
			T_UID	i64UID;
			INT32	i32Roomidx;

			pPacket->ReadData( &i64UID, sizeof(T_UID) );
			pPacket->ReadData( &i32Roomidx, sizeof(INT32) );

			USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID( i64UID );

			if( NULL == pNode ) 
			{
				g_pLog->WriteLog( L"[ERROR]USER_ROOM_JOIN UID Error Server=%d, iUID=%I64d", ui32ServerIdx, i64UID );
			}
			else
			{
				pNode->_ui32ConnectState = MAKE_FRIEND_STATE( FRIEND_STATE_ROOM, GET_FRIEND_SERVER(pNode->_ui32ConnectState), GET_FRIEND_CHANNEL(pNode->_ui32ConnectState), i32Roomidx );
			}

		}
		break;
	case PROTOCOL_ROOM_LEAVE_ROOM_REQ:
		{
			T_UID i64UID;

			pPacket->ReadData( &i64UID, sizeof(T_UID) );

			USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID( i64UID );

			if( NULL == pNode ) 
			{
				g_pLog->WriteLog( L"[ERROR]USER_ROOM_LEAVE UID Error Server=%d, iUID=%I64d", ui32ServerIdx, i64UID );
			}
			else
			{
				pNode->_ui32ConnectState = MAKE_FRIEND_STATE( FRIEND_STATE_LOBBY, GET_FRIEND_SERVER(pNode->_ui32ConnectState), GET_FRIEND_CHANNEL(pNode->_ui32ConnectState), 0 );
			}

		}
		break;
	case PROTOCOL_BASE_CHECK_NICK_NAME_REQ	: 
		{
			INT32		i32Rv = EVENT_ERROR_SUCCESS;
			INT32		i32SessionIdx;
			T_UID		i64UID;
			UINT8		ui8NickSize;
			TTCHAR		strNickName[NET_NICK_NAME_SIZE]; 

			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32) );
			pPacket->ReadData(&i64UID,			sizeof(T_UID) );

			pPacket->ReadData(&ui8NickSize,		sizeof(UINT8) );
			if( ui8NickSize > NET_NICK_NAME_SIZE ) ui8NickSize = NET_NICK_NAME_SIZE;
			pPacket->ReadData(strNickName,		sizeof(TTCHAR) * ui8NickSize );
			strNickName[NET_NICK_NAME_SIZE-1] = 0; 

			USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID( i64UID );
			if( NULL == pNode ) i32Rv = EVENT_ERROR_FAIL;

			if(EV_SUCCESSED(i32Rv))
			{
				i32Rv = m_pDBNick->PushNickName( TYPE_NICK_T_FIND, pNode, strNickName );
			}

			if(EV_FAILED(i32Rv))
			{
				i3NetworkPacket	SendPacket(PROTOCOL_BASE_CHECK_NICK_NAME_ACK);
				SendPacket.WriteData( &i32SessionIdx,	sizeof( INT32 ) );
				SendPacket.WriteData( &i64UID,			sizeof( T_UID ) );
				SendPacket.WriteData( &i32Rv,			sizeof( INT32 ) );
				SendGamePacket(ui32ServerIdx, &SendPacket);
			}
		}
		break; 
	case PROTOCOL_BASE_CREATE_NICK_REQ:
		{
			INT32		i32Rv = EVENT_ERROR_SUCCESS;
			INT32		i32SessionIdx;
			UINT8		ui8NickSize;
			RING_CREATE_NICK_IN CreateNick;

			pPacket->ReadData( &i32SessionIdx,			sizeof(INT32) );
			pPacket->ReadData( &CreateNick.m_TUID,		sizeof(T_UID) );
			pPacket->ReadData( &ui8NickSize,			sizeof(UINT8) );	LENGTH_CHECK( ui8NickSize, NET_NICK_NAME_SIZE );
			pPacket->ReadData( CreateNick.m_strNick,	sizeof(TTCHAR) * ui8NickSize );
			CreateNick.m_strNick[ NET_NICK_NAME_SIZE -1 ] = 0; 

			USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID( CreateNick.m_TUID );

			if( NULL == pNode )	i32Rv = EVENT_ERROR_FAIL;

			if(EV_SUCCESSED(i32Rv))
			{
				i32Rv = m_pDBUserSave->PushCreateNick( &CreateNick );
			}

			if(EV_FAILED(i32Rv))
			{
				i3NetworkPacket	SendPacket( PROTOCOL_BASE_CREATE_NICK_ACK );
				SendPacket.WriteData( &i32SessionIdx,		sizeof( INT32 ) );
				SendPacket.WriteData( &CreateNick.m_TUID,	sizeof( T_UID ) );
				SendPacket.WriteData( &i32Rv,				sizeof( INT32 ) );
				SendGamePacket(ui32ServerIdx, &SendPacket);
			}
		}
		break;
	case PROTOCOL_CHAR_CREATE_CHARA_REQ:
		{
			INT32				i32SessionIdx;
			T_UID				TUID;
			T_RESULT			TResult;

			pPacket->ReadData( &i32SessionIdx,				sizeof(INT32) );
			pPacket->ReadData( &TUID,						sizeof(T_UID) );

			USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID(TUID);
			if (NULL == pNode)	break;
			
			TResult = _PacketCreateChara(pNode, pPacket );
			
			if( EV_FAILED( TResult ) )
			{
				_SendFailCreateChara(pNode, TResult );
			}
		}
		break;

#ifdef USE_TRANS_FIND
	case PROTOCOL_LOBBY_FIND_NICK_NAME_REQ:		// 유저찾기 요청 (닉네임으로)
		{
			INT32		i32Rv = EVENT_ERROR_SUCCESS;
			INT32		i32SessionIdx;
			T_UID		i64UID;
			UINT8		ui8NickSize;
			char		strNickName[NET_NICK_NAME_SIZE]; 

			UINT32		userDbIdx = 0;
			UINT32		userConnectState = MAKE_FRIEND_STATE(FRIEND_STATE_OFFLINE, 0, 0, 0);

			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32) );
			pPacket->ReadData(&i64UID,			sizeof(T_UID) );

			pPacket->ReadData(&ui8NickSize,		sizeof(UINT8) );
			if( ui8NickSize > NET_NICK_NAME_SIZE ) ui8NickSize = NET_NICK_NAME_SIZE;
			pPacket->ReadData(strNickName,		ui8NickSize );
			strNickName[NET_NICK_NAME_SIZE-1] = 0; 

			//  로그인 유저만 찾음
			USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID( i64UID );
			if( NULL == pNode ) i32Rv = EVENT_ERROR_FAIL;


			//// DB 접근 안함
			//if(EV_SUCCESSED(i32Rv))
			//{
			//	iRv = m_pDBUserLoad->PushNickName(TYPE_NICK_T_FIND, pNode, strNickName);
			//}
			i32Rv = EVENT_ERROR_FAIL;

			// 유저를 못찾은 경우
			if(EV_FAILED(iRv))
			{
				i3NetworkPacket	SendPacket(PROTOCOL_LOBBY_FIND_NICK_NAME_ACK);
				SendPacket.WriteData( &i32SessionIdx,		sizeof( INT32 ) );
				SendPacket.WriteData( &i64UID,				sizeof( T_UID ) );
				SendPacket.WriteData( &i32Rv,				sizeof( INT32 ) );
				SendPacket.WriteData( &userDbIdx,			sizeof( UINT32 ) );
				SendPacket.WriteData( &userConnectState,	sizeof( UINT32 ) );

				SendGamePacket(ui32ServerIdx, &SendPacket);
				break;
			}

			// 유저를 찾은 경우
			// 찾은 유저의 상태를 발신자에게 전송
			userDbIdx = pNode->_iUID;
			userConnectState = MAKE_FRIEND_STATE(FRIEND_STATE_OFFLINE, 0, 0, 0);

			i3NetworkPacket	SendPacket( PROTOCOL_LOBBY_FIND_NICK_NAME_ACK);
			SendPacket.WriteData( &i32SessionIdx,		sizeof( INT32 ) );
			SendPacket.WriteData( &i64UID,				sizeof( T_UID ) );
			SendPacket.WriteData( &i32Rv,				sizeof( INT32 ) );
			SendPacket.WriteData( &userDbIdx,			sizeof( UINT32 ) );
			SendPacket.WriteData( &userConnectState,	sizeof( UINT32 ) );
			SendGamePacket(iServerIdx, &SendPacket);

		}
		break;
	case PROTOCOL_LOBBY_GET_UID_BY_NICK_NAME_REQ:
		{
			INT32		i32Rv = EVENT_ERROR_SUCCESS;
			INT32		i32SessionIdx;
			T_UID		i64UID;
			UINT8		ui8NickSize;
			char		strNickName[NET_NICK_NAME_SIZE]; 

			UINT32		userDbIdx = 0;
			UINT32		userConnectState = MAKE_FRIEND_STATE(FRIEND_STATE_OFFLINE, 0, 0, 0);

			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32) );
			pPacket->ReadData(&i64UID,			sizeof(T_UID) );

			pPacket->ReadData(&ui8NickSize,		sizeof(UINT8) );
			if( ui8NickSize > NET_NICK_NAME_SIZE ) ui8NickSize = NET_NICK_NAME_SIZE;
			pPacket->ReadData(strNickName,		ui8NickSize );
			strNickName[NET_NICK_NAME_SIZE-1] = 0; 

			//  로그인 유저만 찾음
			USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID( i64UID );
			if( NULL == pNode ) i32Rv = EVENT_ERROR_FAIL;

			// DB에서 찾기
			if(EV_FAILED(iRv))
			{
				i32Rv = m_pDBUserLoad->PushNickName(TYPE_NICK_T_FIND, pNode, strNickName);
			}

			// 유저를 못찾은 경우
			if(EV_FAILED(iRv))
			{
				i3NetworkPacket	SendPacket(PROTOCOL_LOBBY_FIND_NICK_NAME_ACK);
				SendPacket.WriteData( &i32SessionIdx,		sizeof( INT32 ) );
				SendPacket.WriteData( &i64UID,				sizeof( T_UID ) );
				SendPacket.WriteData( &i32Rv,				sizeof( INT32 ) );
				SendPacket.WriteData( &userDbIdx,			sizeof( UINT32 ) );

				SendGamePacket(ui32ServerIdx, &SendPacket);
			}

			// 유저를 찾은 경우
			// 찾은 유저의 상태를 발신자에게 전송
			userDbIdx = pNode->_iUID;
			userConnectState = MAKE_FRIEND_STATE(FRIEND_STATE_ONLINE, 0, 0, 0);

			i3NetworkPacket	SendPacket( PROTOCOL_LOBBY_FIND_NICK_NAME_ACK);
			SendPacket.WriteData( &i32SessionIdx,		sizeof( INT32 ) );
			SendPacket.WriteData( &i64UID,				sizeof( T_UID ) );
			SendPacket.WriteData( &i32Rv,				sizeof( INT32 ) );
			SendPacket.WriteData( &userDbIdx,			sizeof( UINT32 ) );
			SendGamePacket(iServerIdx, &SendPacket);
		}
		break;
#endif
		// PROTOCOL_AUTH
	case PROTOCOL_AUTH_SHOP_VERSION_REQ:
		{
			SendNewVersion(ui32ServerIdx);	// 일단 버전정보만 보냅니다.
			SendFieldShop( ui32ServerIdx );
		}
		break;
	case PROTOCOL_AUTH_SHOP_LIST_REQ:
		{
			SendGoodsList( ui32ServerIdx );
		}
		break;
	case PROTOCOL_AUTH_SHOP_GOODS_BUY_REQ:			// 장바구니 상품 구입
		{
			T_UID	TUID;
			UINT8	ui8GoodsCount;
			BUY_BASKET	aBasket[ MAX_SHOP_GOODS_BUY_COUNT ];
			INT32	i32Rv = EVENT_ERROR_SUCCESS;

			pPacket->ReadData( &TUID,				sizeof(T_UID));
			pPacket->ReadData( &ui8GoodsCount,		sizeof(UINT8));
			pPacket->ReadData( aBasket,				sizeof(BUY_BASKET) * ui8GoodsCount );

			USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID( TUID );
			if( NULL == pNode )	break;

			DB_SHOP_GOODS DBShopBuy;
			DBShopBuy.m_TUID			= TUID;
			DBShopBuy.m_ui8JobAction	= TYPE_SHOP_GOODS_BUY;
			DBShopBuy.m_ui8GoodsCount	= MIN(ui8GoodsCount,MAX_SHOP_GOODS_BUY_COUNT ) ;

			for( UINT8 i = 0 ; i < DBShopBuy.m_ui8GoodsCount ; i++ )
			{
				DBShopBuy.m_stDBGoods[i].m_TGoodsID		= aBasket[i].m_ui32GoodsID;
				DBShopBuy.m_stDBGoods[i].m_ui8BuyType	= aBasket[i].m_ui8BuyType; 
			}

			i32Rv = _BuyGoods( pNode, &DBShopBuy );
			if( EV_FAILED( i32Rv ) )	 
			{
				for( UINT8 k = 0; k < DBShopBuy.m_ui8GoodsCount ;k++ )  DBShopBuy.m_stDBGoods[k].m_i32Rv = i32Rv;
				_SendDataShopBuy( pNode, &DBShopBuy );
			}

		}
		break;
	case PROTOCOL_AUTH_SHOP_GOODS_GIFT_REQ:			// 선물하기
		{
			
			INT32	i32Rv = EVENT_ERROR_SUCCESS;
			UINT8	ui8MessageSize;
			SEND_GIFT	stSendGift;

			pPacket->ReadData( &stSendGift.m_TSenderUID,				sizeof(T_UID) );
			pPacket->ReadData( stSendGift.m_strSenderNick,				sizeof(TTCHAR) * NET_NICK_NAME_SIZE );
			pPacket->ReadData( stSendGift.m_strReceiverNick,			sizeof(TTCHAR) * NET_NICK_NAME_SIZE );
			pPacket->ReadData( &ui8MessageSize,							sizeof(UINT8));
			pPacket->ReadData( stSendGift.m_strMessage,					sizeof(TTCHAR) * ui8MessageSize);
			pPacket->ReadData( &stSendGift.m_ui8GiftCount,				sizeof(UINT8));
			for( UINT8 i = 0 ; i < stSendGift.m_ui8GiftCount ; ++i )
			{
				pPacket->ReadData( &stSendGift.m_aGift[i],				sizeof(BUY_BASKET) );
				stSendGift.m_aGift[i].m_i32Rv		= EVENT_ERROR_FAIL;
				stSendGift.m_aGift[i].m_TGiftDBIdx	= 0;
			}

			// 선물 보낸 사람 검색.
			USER_NODE* pSenderNode = g_pUserFinder->GetUserNodeByUID( stSendGift.m_TSenderUID );
			if( NULL == pSenderNode ) 
			{				
				break;
			}
						
			stSendGift.m_eGiftType = GIFT_TYPE_USER;

			// 선물 받는 사람 검색.
			USER_NODE* pReceiverNode = g_pUserFinder->GetUserNodeByNick( stSendGift.m_strReceiverNick );
			if( NULL == pReceiverNode )
			{	// 받는 유저가 Offline이라면 Nick 쓰레드로 보냅니다.  
				
				i32Rv = m_pDBNick->PushNickName( TYPE_NICK_T_GIFT, pSenderNode, stSendGift.m_strReceiverNick, stSendGift.m_strSenderNick, 0, &stSendGift );
				if( EVENT_RETURN_OK != i32Rv )
				{
					_SendGiftSender( pSenderNode, &stSendGift );
					break;
				}
			}
			else	
			{	// 받는 유저가 Online이라면 SIA DB 쓰레드로 보냅니다. 
				stSendGift.m_TReceiverUID = pReceiverNode->_i64UID;

				if( pReceiverNode->_i64UID == stSendGift.m_TSenderUID )
				{
					// 자신에게는 선물 불가합니다.
					 _SendGiftSender(pSenderNode, &stSendGift);
					break;
				}

				if( 0 == stSendGift.m_TReceiverUID )
				{	// 확인용 로그 남깁니다.
					g_pLog->WriteLog( L"Gift Fail TReceiverUID Error / SenderUID : %I64d / ReceiverNick : %s ", stSendGift.m_TSenderUID, stSendGift.m_strReceiverNick );	
					 _SendGiftSender(pSenderNode, &stSendGift);
					break;
				}

				i32Rv = _GiftInsertDB( &stSendGift, ITEM_INSERT_REQUEST_USER_GIFT );
				if( EV_FAILED( i32Rv ) )	 
				{
					 _SendGiftSender(pSenderNode, &stSendGift);
					break;
				}
			}

			// 서비스 로그 기록.
			for( UINT8 i = 0 ; i < stSendGift.m_ui8GiftCount ; ++i )
			{
				char strTemp[MAX_PATH];
				i3String::Format(strTemp, MAX_PATH, "GIFT,REQ, BuyType = %d, SenderUID = %I64d, ReceiverNick = %s, GoodsId = %d ",
				stSendGift.m_aGift[i].m_ui8BuyType, stSendGift.m_TSenderUID, stSendGift.m_strReceiverNick, stSendGift.m_aGift[i].m_ui32GoodsID ); 
				WRITE_SERVICE_LOG_GIFT_TASK(strTemp);	
			}
		}
		break;
	case PROTOCOL_AUTH_SHOP_AUTH_GIFT_REQ:			// 선물 인증.
		{
			DB_GIFT_IN		GiftIn;
			
			pPacket->ReadData( &GiftIn.m_TUID,							sizeof(T_UID));
			pPacket->ReadData( &GiftIn.m_ui8Type,						sizeof(UINT8));
			pPacket->ReadData( &GiftIn.m_ui8GiftCount,					sizeof(UINT8));
			for( UINT8 i = 0 ; i < GiftIn.m_ui8GiftCount ; ++i )
			{
				pPacket->ReadData( &GiftIn.m_aAuthGift[i].m_TDBIdx,		sizeof(T_GiftDBIdx));
			}
			
			USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID( GiftIn.m_TUID );
			if( NULL == pNode )	break;
			
			if( RINGBUFFER_SUCCESS != g_pModuleDBShop->InGift( &GiftIn ) )
			{	 
				DB_GIFT_OUT	GiftOut;
				GiftOut.m_TUID			= GiftIn.m_TUID;
				GiftOut.m_ui8Type		= GiftIn.m_ui8Type;
				GiftOut.m_ui8GiftCount	= GiftIn.m_ui8GiftCount;

				for( UINT8 i = 0; i < GiftIn.m_ui8GiftCount ; ++i )
				{
					GiftOut.m_aAuthGift[i].m_i32Rv = EVENT_ERROR_EVENT_BUFFER_FULL;
				}

				_SendGiftAuth( pNode, &GiftOut );

				g_pLog->WriteLog( L"BUFFER OVER - DBBuffer Gift Auth / UID : %I64d / Type : %d ", GiftIn.m_TUID, GiftIn.m_ui8Type ); 
				break;
			}					
		}
		break;
	case PROTOCOL_AUTH_SHOP_INSERT_ITEM_REQ:
		{
			T_UID	i64UID;
			INT32	i32SessionIdx;
			INT8	i8InsertType;
			UINT8	ui8ItemCount = 0;
			ST_ITEM pDBItem[MAX_SHOP_DB_ITEM_COUNT];

			pPacket->ReadData( &i64UID,				sizeof(T_UID));
			pPacket->ReadData( &i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData( &i8InsertType,		sizeof(INT8)); 
			pPacket->ReadData( &ui8ItemCount,		sizeof(UINT8));
			pPacket->ReadData( pDBItem,				sizeof(ST_ITEM) * ui8ItemCount);

			INT32	i32Result = _ItemInsertDB(i64UID, (ITEM_INSERT_REQUEST_TYPE)i8InsertType, ui8ItemCount, pDBItem ) ;
			if( EV_FAILED( i32Result ) )
			{	
				g_pLog->WriteLog( L"ItemInsert Req Fail / UID:%I64d / Type:%d / ItemID:%d / ItemArg:%d",  i64UID, i8InsertType, pDBItem[0].m_TItemID, pDBItem[0].m_TItemArg );
				
				i3NetworkPacket	SendPacket( PROTOCOL_AUTH_SHOP_INSERT_ITEM_ACK );
				SendPacket.WriteData( &i32SessionIdx,		sizeof( INT32 ) );
				SendPacket.WriteData( &i64UID,				sizeof( T_UID ) );
				SendPacket.WriteData( &i8InsertType,		sizeof( INT8 ) );
				SendPacket.WriteData( &i32Result,			sizeof( INT32 ) );
				SendGamePacket( ui32ServerIdx, &SendPacket ); 
				break;
			}
		}
		break;
	case PROTOCOL_GET_MEDAL_REWARD_REQ:
		{
			T_UID	i64UID;
			INT32	i32SessionIdx;
			
			MEDAL_REWARD_INFO	MedalReward; 
			ST_ITEM pDBItem[MAX_SHOP_DB_ITEM_COUNT];

			pPacket->ReadData( &i64UID,						sizeof(T_UID));
			pPacket->ReadData( &i32SessionIdx,				sizeof(INT32));
			pPacket->ReadData( &MedalReward.m_ui8Count,		sizeof(UINT8));	
			pPacket->ReadData( MedalReward.m_aDetail,		sizeof(MEDAL_REWARD_DETAIL_INFO)*MedalReward.m_ui8Count);	

			for( UINT8 i = 0 ; i < MedalReward.m_ui8Count; ++i )
			{
				pDBItem[i].m_TItemID	= MedalReward.m_aDetail[i].m_TItemID;
				pDBItem[i].m_TItemArg	= MedalReward.m_aDetail[i].m_TItemArg; 
			}

			
			INT32 i32Rv = _ItemInsertDB(i64UID, ITEM_INSERT_REQUEST_MEDAL_REWARD, MedalReward.m_ui8Count, pDBItem, 0, &MedalReward );
			if( EV_FAILED( i32Rv ) )
			{	
				g_pLog->WriteLog( L"ItemInsert Req Fail MedalReward / UID:%I64d / ItemID:%d / ItemArg:%d",  i64UID,  pDBItem[0].m_TItemID, pDBItem[0].m_TItemArg );

				i3NetworkPacket	SendPacket( PROTOCOL_GET_MEDAL_REWARD_ACK );
				SendPacket.WriteData( &i32SessionIdx,					sizeof( INT32 ) );
				SendPacket.WriteData( &i64UID,							sizeof( T_UID ) );
				SendPacket.WriteData( &i32Rv,							sizeof( INT32 ) );
				SendPacket.WriteData( &MedalReward.m_ui8Count,			sizeof( UINT8 ) );
				SendPacket.WriteData( MedalReward.m_aDetail,			sizeof(MEDAL_REWARD_DETAIL_INFO)*MedalReward.m_ui8Count);	 
				SendGamePacket( ui32ServerIdx, &SendPacket );
				break;
			}
		}
		break;
	case PROTOCOL_AUTH_SHOP_DELETE_ITEM_REQ:
		{
			INT32		i32SessionIdx;
			T_UID		i64UID;
			T_ItemDBIdx	TItemWareIdx;

			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID));
			pPacket->ReadData(&TItemWareIdx,		sizeof(T_ItemDBIdx));

			DB_SHOP_IN DBShopIn;
			DBShopIn._ui8InsertType	= TYPE_SHOP_ITEM_DELETE;
			DBShopIn._i64UID		= i64UID;
			DBShopIn._TWareDBIdx	= TItemWareIdx;
			DBShopIn._ui32ItemArg	= 0xFFFFFFFF;
			DBShopIn._i8SlotIdx		= -1;

			if( RINGBUFFER_SUCCESS != g_pModuleDBShop->InInsertItem( &DBShopIn ) )
			{	// Error	 
				g_pLog->WriteLog( L"BUFFER OVER - DBBuffer Item Delete / UID : %I64d / ItemWareIdx : %d", i64UID, TItemWareIdx );	 
				_SendDataFail_Type( TYPE_SHOP_ITEM_DELETE, ui32ServerIdx, i64UID, EVENT_ERROR_EVENT_BUFFER_FULL, i32SessionIdx );
				break;
			}
		}
		break;
	case PROTOCOL_SHOP_REPAIR_REQ:
		{
			T_UID	i64UID;
			S2MO_REPAIR_INFO	stInfo;
			i3mem::FillZero( &stInfo, sizeof( S2MO_REPAIR_INFO ));

			pPacket->ReadData(&i64UID,							sizeof(T_UID));
			pPacket->ReadData(&stInfo.m_ui8ItemCount,			sizeof(UINT8));	
			pPacket->ReadData(stInfo.m_stRepairItem,			sizeof(S2MO_REPAIR_ITEM)*stInfo.m_ui8ItemCount);

			if ( 0 == stInfo.m_ui8ItemCount  || stInfo.m_ui8ItemCount > CHAR_EQUIPMENT_COUNT)
			{	// 로그 남기고 실패 처리 합니다.
				g_pLog->WriteLog(L"Repair Count Over /  UID : %I64d / ItemCount : %d / ItemDBIdx : %d",
					i64UID, stInfo.m_ui8ItemCount, stInfo.m_stRepairItem[0].m_TItemDBIdx);

				stInfo.m_ui8ItemCount = 1;
				stInfo.m_stRepairItem[0].m_i32Rv = EVENT_ERROR_PBV15_SHOP_REPAIR_HACK_PACKET;
				_SendDataRepair(i64UID, &stInfo);
				break;
			}
			
			USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID( i64UID );
			if( NULL == pNode )	break;

			DB_REPAIR DBIn;
			DBIn.m_TUID				= i64UID;
			DBIn.m_eType			= DB_REPAIR_TYPE_BUY; 
			i3mem::Copy( &DBIn.m_stRepairInfo, &stInfo,  sizeof( S2MO_REPAIR_INFO ) );


			if( RINGBUFFER_SUCCESS != g_pModuleDBShop->InRepair( &DBIn ) )
			{	// Error
				for( UINT8 i = 0 ; i < stInfo.m_ui8ItemCount ; i++ ) stInfo.m_stRepairItem[i].m_i32Rv =  EVENT_ERROR_EVENT_BUFFER_FULL;
				_SendDataRepair( i64UID, &stInfo  );
				g_pLog->WriteLog( L"BUFFER OVER - DBBuffer Repair /  UID : %I64d / ItemCount : %d / ItemDBIdx : %d",
					i64UID,  stInfo.m_ui8ItemCount, stInfo.m_stRepairItem[0].m_TItemDBIdx );	
			}
			else 
			{	// 로그 남깁니다.
				for( UINT8 i = 0 ; i < stInfo.m_ui8ItemCount ; ++i )
				{
					char strTemp[MAX_PATH];
					i3String::Format(strTemp, MAX_PATH, "REPAIR,REQ,[%d] ID = %s, uid = %I64d  Point = %d Cash = %d ItemID = %d ItemArg = %d DBIdx = %d",
					i, pNode->_strID, pNode->_i64UID, stInfo.m_stRepairItem[i].m_ui32RepairPoint, stInfo.m_stRepairItem[i].m_ui32RepairCash, stInfo.m_stRepairItem[i].m_TItemID, stInfo.m_stRepairItem[i].m_ui32AuthArg, stInfo.m_stRepairItem[i].m_TItemDBIdx); 
					WRITE_SERVICE_LOG_REPAIR_TASK(strTemp);	
				}
			}
		}
		break;

	case PROTOCOL_AUTH_SHOP_ITEM_AUTH_REQ:		// 아이템 인증
		{
			INT32		i32SessionIdx;
			T_UID		i64UID;
			T_ItemDBIdx	TItemWareIdx;
			INT32		i32ItemArg;
			UINT8		ui8CharaSlotIdx;
			UINT32		ui32ItemID;

			// 현재 DB에 아이템이 없는 상태로 설정합니다.
			// Index의 값
			// 0 : 정상, 1 : inven의 DBIdx에 Client가 보내준 DBIdx가 없을 경우, 2 : Inventory가 Full 일 때
			UINT8				ui8FailIndex = 1;
			enum EVENT_ERROR	eEventError = EVENT_ERROR_EVENT_AUTH_FAIL;
			
			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID));
			pPacket->ReadData(&TItemWareIdx,		sizeof(T_ItemDBIdx));
			pPacket->ReadData(&i32ItemArg,			sizeof(INT32));
			pPacket->ReadData(&ui8CharaSlotIdx,		sizeof(UINT8));
			pPacket->ReadData(&ui32ItemID,			sizeof(UINT32));
		
			ui32ItemID = GET_ITEM_TYPE ( ui32ItemID );

			DB_SHOP_IN DBShopIn;
			DBShopIn._ui8InsertType	= TYPE_SHOP_ITEM_AUTH; 
			DBShopIn._i64UID		= i64UID;
			DBShopIn._TWareDBIdx	= TItemWareIdx;
			DBShopIn._ui32ItemArg	= i32ItemArg;
			DBShopIn._i8SlotIdx		= ui8CharaSlotIdx;

			USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID(i64UID);
			if( NULL == pNode )
			{
				_SendDataFail_Type( TYPE_SHOP_ITEM_AUTH, ui32ServerIdx, i64UID, EVENT_ERROR_FAIL, i32SessionIdx);
				break;
			}

			for( INT32 i = 0 ; i < MAX_INVEN_COUNT ; i++)
			{
				if( pNode->_Inven[i]._TItemWareDBIdx == TItemWareIdx )
				{
					// 아이템이 있을 경우 정상으로 변경
					ui8FailIndex = 0;
					if( ui32ItemID ==  ITEM_TYPE_WRAP_COUNT  ||
						ui32ItemID ==  ITEM_TYPE_WRAP_PERIOD )
					{
						if(pNode->_ui16InvenCount > MAX_INVEN_COUNT - MAX_CAPSULE_REWARD_COUNT)
						{
							// Inventory가 Full 일 경우
							eEventError = EVENT_ERROR_EVENT_BUY_GOODS_INVENTORY_FULL;
							ui8FailIndex = 2;
							break;
						}
					}
					break;						
				}
			}

			if( 0 != ui8FailIndex)
			{	// Error
				g_pLog->WriteLog(L"Fail Item Auth FailIdx : %d / UID : %I64d / ItemWareIdx : %d / InvenCount : %d", ui8FailIndex, i64UID, TItemWareIdx, pNode->_ui16InvenCount);
				
				_SendDataFail_Type( TYPE_SHOP_ITEM_AUTH, ui32ServerIdx, i64UID, eEventError, i32SessionIdx);
				break;
			}

			if( RINGBUFFER_SUCCESS != g_pModuleDBShop->InInsertItem( &DBShopIn ) )
			{	// Error
				g_pLog->WriteLog( L"BUFFER OVER - DBBuffer Item Auth / UID : %I64d / ItemWareIdx : %d / ItemArg : %d", 
					i64UID, TItemWareIdx, i32ItemArg );	
				_SendDataFail_Type( TYPE_SHOP_ITEM_AUTH, ui32ServerIdx, i64UID, EVENT_ERROR_EVENT_BUFFER_FULL, i32SessionIdx);
				break;
			}
		}
		break;
	case PROTOCOL_AUTH_SHOP_ITEM_AUTH_DATA_REQ:
		{
			INT32		i32Result = EVENT_ERROR_FAIL;
			INT32		i32SessionIdx;
			T_UID		i64UID;
			T_ItemDBIdx	TItemWareIdx;
			UINT32		ui32ItemID;
			UINT8		ui8Ability = 0;
			TTCHAR		pAbility[ 255 ];

			pPacket->ReadData(&i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData(&i64UID,				sizeof(T_UID));
			pPacket->ReadData(&TItemWareIdx,		sizeof(T_ItemDBIdx));
			pPacket->ReadData(&ui32ItemID,			sizeof(UINT32));
			pPacket->ReadData(&ui8Ability,			sizeof(UINT8));
			pPacket->ReadData(pAbility,				ui8Ability);

			USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID( i64UID );
			if( NULL == pNode )
			{
				i32Result = EVENT_ERROR_EVENT_AUTH_START_PARAMETER_USER;
				_SendDataFail_Type( TYPE_SHOP_ITEM_AUTH, ui32ServerIdx, i64UID, i32Result, i32SessionIdx);
				break;
			}

			i32Result = _SetItemAbilityAuth( pNode, ui32ItemID, TItemWareIdx, ui8Ability, pAbility );

			if( EVENT_ERROR_SUCCESS == i32Result )
			{
				DB_SHOP_IN DBShopIn;
				DBShopIn._ui8InsertType	= TYPE_SHOP_ITEM_AUTH;
				DBShopIn._i64UID		= i64UID;
				DBShopIn._TWareDBIdx	= TItemWareIdx;
				DBShopIn._ui32ItemArg	= 1;
				DBShopIn._i8SlotIdx		= -1;

				if( RINGBUFFER_SUCCESS != g_pModuleDBShop->InInsertItem( &DBShopIn ) )
				{	// Error
					g_pLog->WriteLog( L"BUFFER OVER - DBBuffer Item Auth Data / UID : %I64d / ItemWareIdx : %d / ItemArg : %d", i64UID, TItemWareIdx, 1 );	
					i32Result = EVENT_ERROR_EVENT_BUFFER_FULL;
				}
			}

			if( EV_FAILED( i32Result ) )
			{
				_SendDataFail_Type( TYPE_SHOP_ITEM_AUTH, ui32ServerIdx, i64UID, i32Result, i32SessionIdx);
				break;
			}
		}
		break;
	case PROTOCOL_CHEAT_ITEM_AUTH_REQ:
		{
			INT32			i32SessionIdx;
			T_UID			i64UID;
			T_ItemDBIdx		TItemWareIdx;
			INT32			i32ItemArg;
			UINT8			ui8CharaSlotIdx;
			UINT32			ui32ItemID;
			TLoopCnt		TLoopCount;

			// 현재 DB에 아이템이 없는 상태로 설정합니다.
			UINT8				ui8FailIndex = 1;
			enum EVENT_ERROR	eEventError = EVENT_ERROR_EVENT_AUTH_FAIL;

			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(T_UID));
			pPacket->ReadData(&TItemWareIdx,	sizeof(T_ItemDBIdx));
			pPacket->ReadData(&i32ItemArg,		sizeof(INT32));
			pPacket->ReadData(&ui8CharaSlotIdx, sizeof(UINT8));
			pPacket->ReadData(&ui32ItemID,		sizeof(UINT32));
			pPacket->ReadData(&TLoopCount,		sizeof(TLoopCnt));

			ui32ItemID				= GET_ITEM_TYPE(ui32ItemID);

			const ENUM_TYPE_SHOP eShopItemType = TYPE_SHOP_ITEM_LOOP_AUTH;

			DB_SHOP_IN DBShopIn;
			DBShopIn._ui8InsertType		= eShopItemType;
			DBShopIn._i64UID			= i64UID;
			DBShopIn._TWareDBIdx		= TItemWareIdx;
			DBShopIn._ui32ItemArg		= i32ItemArg;
			DBShopIn._i8SlotIdx			= ui8CharaSlotIdx;
			DBShopIn._32Arg1.TLoopCount = TLoopCount;
			DBShopIn._ui32GoodsID		= static_cast<UINT32>(TLoopCount);

			USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID(i64UID);
			if (nullptr == pNode)
			{
				_SendDataFail_Type(eShopItemType, ui32ServerIdx, i64UID, EVENT_ERROR_FAIL, i32SessionIdx);
				break;
			}

			for (INT32 i = 0; i < MAX_INVEN_COUNT; i++)
			{
				if (pNode->_Inven[i]._TItemWareDBIdx == TItemWareIdx)
				{
					// 아이템이 있을 경우 정상으로 변경
					ui8FailIndex = 0;
					if (ui32ItemID == ITEM_TYPE_WRAP_COUNT ||
						ui32ItemID == ITEM_TYPE_WRAP_PERIOD)
					{
						if (pNode->_ui16InvenCount > MAX_INVEN_COUNT - MAX_CAPSULE_REWARD_COUNT)
						{
							// Inventory가 Full 일 경우
							eEventError = EVENT_ERROR_EVENT_BUY_GOODS_INVENTORY_FULL;
							ui8FailIndex = 2;
							break;
						}
					}
					break;
				}
			}

			if (0 != ui8FailIndex)
			{	// Error
				g_pLog->WriteLog(L"Fail Item Auth FailIdx : %d / UID : %I64d / ItemWareIdx : %d / InvenCount : %d", ui8FailIndex, i64UID, TItemWareIdx, pNode->_ui16InvenCount);

				_SendDataFail_Type(eShopItemType, ui32ServerIdx, i64UID, eEventError, i32SessionIdx);
				break;
			}

			if (RINGBUFFER_SUCCESS != g_pModuleDBShop->InInsertItem(&DBShopIn))
			{	// Error
				g_pLog->WriteLog(L"BUFFER OVER - DBBuffer Item Auth Loop / UID : %I64d / ItemWareIdx : %d / ItemArg : %d",
					i64UID, TItemWareIdx, i32ItemArg);
				_SendDataFail_Type(eShopItemType, ui32ServerIdx, i64UID, EVENT_ERROR_EVENT_BUFFER_FULL, i32SessionIdx);
				break;
			}
		}
		break;
	case PROTOCOL_AUTH_SHOP_GET_GIFTLIST_REQ:		//  받은 선물 목록 요청
		{
			INT32	i32SessionIdx;
			T_UID	i64UID;

			pPacket->ReadData(&i32SessionIdx,	sizeof(UINT32));
			pPacket->ReadData(&i64UID,			sizeof(T_UID));

			DB_GIFT_IN DBGiftIn;
			DBGiftIn.m_TUID			= i64UID;
			DBGiftIn.m_ui8Type		= TYPE_SHOP_GIFT_LIST;
			DBGiftIn.m_ui8GiftCount = 0;

			if( RINGBUFFER_SUCCESS != g_pModuleDBShop->InGift( &DBGiftIn ) )
			{	// Error
				g_pLog->WriteLog( L"BUFFER OVER - DBBuffer Gift / UID : %I64d ", i64UID );	
				_SendDataFail_Type( TYPE_SHOP_GIFT_LIST, ui32ServerIdx, i64UID, EVENT_ERROR_EVENT_BUFFER_FULL, i32SessionIdx );
				break;
			}
		}
		break;	

		// GameServer 와 통신	
	case PROTOCOL_AUTH_GET_POINT_CASH_REQ:
		{
			INT32		sessionIdx;
			T_UID		i64UID;

			pPacket->ReadData(&sessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,		sizeof(T_UID));

			USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID(i64UID);
			if( NULL == pNode )
			{
				_AlarmCoin( ui32ServerIdx, sessionIdx, i64UID, EVENT_ERROR_EVENT_AUTH_START_PARAMETER_USER, 0, 0 );
				break;
			}

			PACKET_GET_CASH_REQ	CashReq;
			CashReq.m_TUID					= i64UID;
			CashReq.m_ui64UserBillingIdx	= pNode->_ui64UserBillingIdx;
			CashReq.m_ui8LinkMethod			= pNode->m_ui8LinkMethod;
			i3mem::Copy( CashReq.m_strBillingID, pNode->m_strBillingID, BILLING_ID_SIZE );

			// #### SIA 서버로 보낸다.
			if( FALSE == g_pModuleSIA->GetCash_T( &CashReq ) )
			{	
				if (FALSE == g_pConfig->m_bSIAActive)
				{
					// SIA 비활성화시 TransServer가 가진 포인트, 캐시 정보를 전송
					_AlarmCoin(ui32ServerIdx, sessionIdx, i64UID, EVENT_ERROR_SUCCESS, pNode->_sBasic.m_ui32Point, pNode->_ui32Cash);
					break;
				} 
				// Error
				g_pLog->WriteLog( L"SIA Send Fail - Get Cash Check / UID : %I64d ", i64UID );
			}
		}
		break;
	case PROTOCOL_AUTH_USED_WEAPON_REQ:
		{
			INT32					i32SessionIdx;
			T_UID					i64UID;
			UINT32					ui32Exp;
			UINT32					ui32Point;
			USER_INFO_SEASON_RECORD	stSeaonRecord;
			USER_INFO_LAST_GAME		stLastGame;
			UINT16					ui16UsedItemCount;
			USED_ITEM				aUsedItemList[ MAX_INVEN_COUNT ];

			pPacket->ReadData( &i32SessionIdx,		sizeof(INT32) );
			pPacket->ReadData( &i64UID,				sizeof(T_UID) );
			pPacket->ReadData( &ui32Exp,			sizeof(UINT32) );
			pPacket->ReadData( &ui32Point,			sizeof(UINT32) );
			pPacket->ReadData( &stSeaonRecord,		sizeof(USER_INFO_SEASON_RECORD) );
			pPacket->ReadData( &stLastGame,			sizeof(USER_INFO_LAST_GAME) );
			pPacket->ReadData( &ui16UsedItemCount,	sizeof(UINT16) );
			if( MAX_INVEN_COUNT < ui16UsedItemCount )	break;
			pPacket->ReadData( aUsedItemList,		sizeof(USED_ITEM)*ui16UsedItemCount );

			USER_NODE* pNode =  g_pUserFinder->GetUserNodeByUID(i64UID);
			if( NULL == pNode ) break;

			pNode->_sBasic.m_ui32Exp		= ui32Exp;
			pNode->_sBasic.m_ui32Point	= ui32Point;
			i3mem::Copy( &pNode->_sRecord,		&stSeaonRecord, sizeof(USER_INFO_SEASON_RECORD) );
			i3mem::Copy( &pNode->m_LastGame,	&stLastGame,	sizeof(USER_INFO_LAST_GAME) );

			pNode->_ui32UpdateInfo |= (SAVE_DATABASE_BASEINFO | SAVE_DATABASE_RECORD);

			UsedWeaponList_T( pNode, ui16UsedItemCount, aUsedItemList );
			//UsedWeaponList_T( i64UID, ui8WeaponCount, i64WeaponList );
		}
		break;
	case PROTOCOL_ALIVE_USER_REQ : 
		{
			INT32 i32SessionIdx;
			T_UID i64UID; 
			UINT8 ui8Count;
			USER_NODE * pNode;
			BOOL bResult;

			pPacket->ReadData(&ui8Count,		sizeof(UINT8));

			for( INT32 i = 0; i < ui8Count; i++ )
			{
				pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
				pPacket->ReadData(&i64UID,			sizeof(T_UID));

				pNode = g_pUserFinder->GetUserNodeByUID(i64UID); 
				bResult = FALSE;

				if( pNode != NULL )
				{	//Recv 패킷을 저장합니다.
					pNode->_LastRecvTimeST = i3ThreadTimer::GetServerTime();
					bResult = _SaveUserInfo(pNode, pPacket, 4, ui32ServerIdx);
				}

				if( FALSE == bResult )
				{
					g_pLog->WriteLog( L"PROTOCOL_ALIVE_USER_REQ NonUser : Server:%d, SID:%d, DB(UID):%I64d", ui32ServerIdx, i32SessionIdx, i64UID  );

					// pNode 가 없더라도 pPacket 는 비워줘야 합니다.
					USER_SAVE_DATA stUserData;
					_GetUserSaveData( pPacket, &stUserData );
				}
			}
		}
		break;
	case PROTOCOL_AUTH_UPDATE_BASICINFO_REQ : 	
		break; 
	case PROTOCOL_AUTH_UPDATE_RECORD_REQ	: 
		break; 			
	case PROTOCOL_AUTH_UPDATE_EQUIP_REQ		: 
		break; 
	case PROTOCOL_AUTH_UPDATE_MEDAL_REQ		: 
		break;
	case PROTOCOL_BASE_GET_RECORD_INFO_DB_REQ:
		{
			// 유저 정보 요청
			INT32 i32SessionIdx;
			T_UID i64UID, i64FUID;

			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));
			pPacket->ReadData(&i64UID,			sizeof(T_UID));
			pPacket->ReadData(&i64FUID,			sizeof(T_UID));

			i3NetworkPacket	SendPacket( PROTOCOL_BASE_GET_RECORD_INFO_DB_ACK );
			SendPacket.WriteData( &i32SessionIdx,		sizeof( INT32 ) );
			SendPacket.WriteData( &i64UID,				sizeof( T_UID ) );

			INT32 i32Result;
			USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID( i64FUID );
			if( pNode )
			{
				i32Result = EVENT_ERROR_SUCCESS;
				SendPacket.WriteData( &i32Result,		sizeof( INT32 ) );
				SendPacket.WriteData( &pNode->_sRecord,	sizeof( USER_INFO_RECORD ) );
			}
			else
			{
				i32Result = EVENT_ERROR_FAIL;
				SendPacket.WriteData( &i32Result,		sizeof( INT32 ) );
			}
			SendGamePacket( ui32ServerIdx, &SendPacket );
		}
		break;
	case PROTOCOL_GSM_ACCOUNT_KICK_ACK:
		{
			T_UID i64UID;
			pPacket->ReadData( &i64UID,	sizeof(T_UID) );

			USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID( i64UID ); 
			if( NULL == pNode )
			{
				g_pLog->WriteLog( L"[ERROR]GSM_ACCOUNT_KICK NonUser : Server:%d, DB(UID):%I64d", ui32ServerIdx, i64UID );
			}
			else if( USER_STATE_ACCOUNTING != pNode->_ui32State )
			{
				g_pLog->WriteLog( L"[ERROR]GSM_ACCOUNT_KICK STATE Error : Server:%d, DB(UID):%I64d, State:%d", ui32ServerIdx, pNode->_i64UID, pNode->_ui32State );
			}
			else
			{
				_LogoutCommand( pNode );
			}
		}
		break;
	case PROTOCOL_ROOM_GM_BLOCK_USER_REQ:
		{
			T_UID i64UID;
			UINT8 ui8CommentLength;
			TTCHAR strComment[GM_COMMENT_SIZE+1];

			pPacket->ReadData( &i64UID,				sizeof(T_UID) );
			pPacket->ReadData( &ui8CommentLength,	sizeof(UINT8) );
			pPacket->ReadData( strComment,			sizeof(TTCHAR) * ui8CommentLength );
			strComment[ui8CommentLength] = 0;

			BlockUser( i64UID, strComment );
		}
		break;
	case PROTOCOL_ASC_WEB_USER_INFO_LOCATION_ACK:
		{
			INT32	i32WebSessionIdx = -1;
			INT32	i32Rv;
			INT8	i8ServerIdx = 0;
			INT8	i8ChannelNum = 0;
			INT32	i32RoomIdx = 0;
			pPacket->ReadData( &i32WebSessionIdx,	sizeof(INT32) );
			pPacket->ReadData( &i32Rv,				sizeof(INT32) );
			pPacket->ReadData( &i8ServerIdx,		sizeof(INT8)  );
			pPacket->ReadData( &i8ChannelNum,		sizeof(INT8)  );
			pPacket->ReadData( &i32RoomIdx,			sizeof(INT32) );

			g_pModuleControl->SendUserInfoLocationWebAck( i32WebSessionIdx, i32Rv, i8ServerIdx, i8ChannelNum, i32RoomIdx );
		}
		break;
	case PROTOCOL_BASE_ATTENDANCE_REQ:	// 출석 도장 찍기
		{
			INT32	i32Rv = EVENT_ERROR_ATTENDANCE_UNKWON;
			T_UID	i64UID;
			UINT32	ui32AttendanceNum;
			UINT8	ui8EventType;		// 이벤트 타입

			pPacket->ReadData( &i64UID,						sizeof( T_UID ) );
			pPacket->ReadData( &ui32AttendanceNum,			sizeof( UINT32 ) );
			pPacket->ReadData( &ui8EventType,				sizeof( UINT8 ) );

			USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID( i64UID ); 

			if( NULL == pNode )							break;
			else
			{
				i32Rv = _CheckAttendanceMark( pNode, ui32AttendanceNum, ui8EventType );
			}

			i3NetworkPacket	SendPacket( PROTOCOL_BASE_ATTENDANCE_ACK );
			SendPacket.WriteData( &pNode->_i32Sessionidx,				sizeof( INT32 ) );
			SendPacket.WriteData( &i64UID,								sizeof( T_UID ) );
			SendPacket.WriteData( &i32Rv,								sizeof( INT32 ) );
			SendPacket.WriteData( &pNode->_Attendance[ui8EventType],	sizeof( ATTENDANCE_USER_SVR ) );
			SendGamePacket( ui32ServerIdx, &SendPacket );
		}
		break;
	case PROTOCOL_BASE_ATTENDANCE_CLEAR_ITEM_REQ:
		{
			// 클라이언트에서 기간내에 출석체크 이벤트를 완료하여 보상 버튼이 활성화되어 유저가 원하는 아이템을 보상으로 요청합니다.
			INT32	i32Rv = EVENT_ERROR_ATTENDANCE_UNKWON;
			T_UID	i64UID;
			UINT32	ui32AttendanceID;	// 출석체크 번호
			UINT8	ui8ItemNum;			// 보상 아이템 번호 ( 왼쪽이냐 오른쪽이냐 )
			UINT8	ui8EventType;		// 이벤트 타입

			pPacket->ReadData( &i64UID,				sizeof( T_UID ) );
			pPacket->ReadData( &ui32AttendanceID,	sizeof( UINT32 ) );
			pPacket->ReadData( &ui8ItemNum,			sizeof( UINT8 ) );
			pPacket->ReadData( &ui8EventType,		sizeof( UINT8 ) );

			USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID( i64UID ); 

			if( pNode == NULL )						break;
					
			i32Rv = _CheckAttendanceInfo( pNode, ui32AttendanceID, ui8ItemNum, ui8EventType );

			i3NetworkPacket	SendPacket( PROTOCOL_BASE_ATTENDANCE_CLEAR_ITEM_ACK );
			SendPacket.WriteData( &pNode->_i32Sessionidx,	sizeof( INT32 ) );
			SendPacket.WriteData( &i64UID,					sizeof( T_UID ) );
			SendPacket.WriteData( &i32Rv,					sizeof( INT32 ) );
			SendGamePacket( ui32ServerIdx, &SendPacket );
		}
		break;
	case PROTOCOL_LOBBY_SET_BIRTHDAY_REQ:
		{
			INT32		i32Rv = EVENT_ERROR_SUCCESS;
			INT32		i32SessionIdx;
			T_UID		i64UID;
			UINT32		ui32BirthDay;

			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32) );
			pPacket->ReadData(&i64UID,			sizeof(T_UID) );

			pPacket->ReadData(&ui32BirthDay,	sizeof(UINT32) );

			USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID( i64UID );
			if( NULL == pNode )i32Rv = EVENT_ERROR_FAIL;

			if(EV_SUCCESSED(i32Rv))
			{
				i32Rv = m_pDBUID->PushBirthDaySet(ui32ServerIdx, i32SessionIdx, i64UID, ui32BirthDay);
			}

			if(EV_FAILED(i32Rv))
			{
				i3NetworkPacket	SendPacket(PROTOCOL_LOBBY_SET_BIRTHDAY_ACK);
				SendPacket.WriteData( &i32SessionIdx,	sizeof( INT32 ) );
				SendPacket.WriteData( &i64UID,			sizeof( T_UID ) );
				SendPacket.WriteData( &i32Rv,			sizeof( INT32 ) );
				SendGamePacket(ui32ServerIdx, &SendPacket);
			}
		}
		break;
	case PROTOCOL_AUTH_SHOP_ITEM_EXTEND_REQ:			// 기간(개수) 연장
		{
			INT32			i32SessionIdx;
			T_UID			TUID;
			EXTEND_BASKET	stBasket;
			INT32			i32Rv = EVENT_ERROR_SUCCESS;

			pPacket->ReadData( &i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData( &TUID,				sizeof(T_UID));
			pPacket->ReadData( &stBasket,			sizeof(EXTEND_BASKET) );

			USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID( TUID );
			if( NULL == pNode )	break;

			DB_SHOP_GOODS DBShopBuy;
			DBShopBuy.m_TUID = TUID;
			DBShopBuy.m_ui8GoodsCount = 1;
			DBShopBuy.m_ui8JobAction = TYPE_SHOP_ITEM_EXTEND;
			DBShopBuy.m_stDBGoods[0].m_ui8BuyType = stBasket.m_Basket.m_ui8BuyType;
			DBShopBuy.m_stDBGoods[0].m_TGoodsID = stBasket.m_Basket.m_ui32GoodsID;
			DBShopBuy.m_TItemDBIdx = stBasket.m_TWareDBIdx;

			i32Rv = _BuyGoods( pNode, &DBShopBuy );
			if( EV_FAILED( i32Rv ) )
			{
				DBShopBuy.m_stDBGoods[0].m_i32Rv = i32Rv;
				_SendDataShopBuy( pNode, &DBShopBuy );
			}		
		}
		break;
	case PROTOCOL_CHAR_DELETE_CHARA_REQ:
		{ 
			RING_CREATE_CHAR	DeleteChar;

			pPacket->ReadData( &DeleteChar.m_TUID,			sizeof(T_UID) );
			pPacket->ReadData( &DeleteChar.m_ui8CharSlot,	sizeof(UINT8) );
			pPacket->ReadData( &DeleteChar.m_Inven._TItemWareDBIdx,	sizeof(T_ItemDBIdx) );

			DeleteChar.m_eType = CHARA_RING_TYPE_DELETE;

			USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID(DeleteChar.m_TUID);
			if (NULL == pNode)	break;

			DB_SHOP_IN DBShopIn;
			DBShopIn._ui8InsertType = TYPE_SHOP_ITEM_DELETE;
			DBShopIn._i64UID = DeleteChar.m_TUID;
			DBShopIn._TWareDBIdx = DeleteChar.m_Inven._TItemWareDBIdx;
			DBShopIn._ui32ItemArg = 0xFFFFFFFF;
			DBShopIn._i8SlotIdx = DeleteChar.m_ui8CharSlot;
			if ( FALSE == g_pModuleDBShop->InInsertItem(&DBShopIn))
			{	// Error	  
				_SendDeleteChara(pNode, EVENT_ERROR_EVENT_BUFFER_FULL, DeleteChar.m_ui8CharSlot);

				g_pLog->WriteLog(L"BUFFER OVER - DBBuffer Chara Delete / UID : %I64d / SlotIdx : %d /  ItemWareIdx : %d",
					DeleteChar.m_TUID, DeleteChar.m_ui8CharSlot, DeleteChar.m_Inven._TItemWareDBIdx);
			} 
		}
		break;

	case PROTOCOL_AUTH_SPECIALITY_CLASS_SETTING_REQ	: 
		{
			// 사용 안함. Game서버에서 처리.
		}
		break; 

	case PROTOCOL_BASE_GET_USER_INFO_UID_REQ:
		{
			INT32		i32SessionIdx;
			T_UID		TUID;
			T_UID		TFindUID;
			T_RESULT	TResult;
 
			pPacket->ReadData( &i32SessionIdx,				sizeof(INT32) );
			pPacket->ReadData( &TUID,						sizeof(T_UID) );
			pPacket->ReadData( &TFindUID,					sizeof(T_UID) );

			S2_USER_DETAIL_INFO	stInfo;
						
			USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID( TFindUID );
			if( pNode )
			{
				TResult = EVENT_ERROR_SUCCESS;

				i3NetworkPacket SendPacket( PROTOCOL_BASE_GET_USER_DETAIL_INFO_ACK );
				SendPacket.WriteData( &i32SessionIdx,		sizeof( INT32 ) ); 
				SendPacket.WriteData( &TUID,				sizeof( T_UID ) );
				SendPacket.WriteData( &TResult,				sizeof(T_RESULT) ); 
				
				_SetUserDetailInfo( stInfo, pNode );
				
				SendPacket.WriteData( &stInfo,				sizeof(S2_USER_DETAIL_INFO) ); 
				SendGamePacket( ui32ServerIdx, &SendPacket );
			}
			else
			{
				RING_USER_INFO_IN RingIn;
				RingIn.m_i32ServerIdx		= ui32ServerIdx;
				RingIn.m_i32SessionIdx		= i32SessionIdx;
				RingIn.m_TUID				= TUID;
				RingIn.m_eFindState			= USER_INFO_FIND_STATE_UID;
				RingIn.m_TFindUID			= TFindUID;
				RingIn.m_strNickName[ 0 ]	= '\0';

				m_pDBUserInfo->PushUserInfo( &RingIn );
			}		
			
		}
		break;
	case PROTOCOL_BASE_GET_USER_INFO_NICK_REQ:
		{
			INT32		i32SessionIdx;
			T_UID		TUID;
			T_RESULT	TResult;
			UINT8		ui8NickSize;
			TTCHAR		strNickName[ NET_NICK_NAME_SIZE ];
						 
			pPacket->ReadData( &i32SessionIdx,		sizeof(INT32));
			pPacket->ReadData( &TUID,				sizeof(T_UID) );
			pPacket->ReadData( &ui8NickSize,		sizeof(UINT8));
			pPacket->ReadData( strNickName,			sizeof(TTCHAR) * ui8NickSize);

			S2_USER_DETAIL_INFO	stInfo;

			USER_NODE* pNode = g_pUserFinder->GetUserNodeByNick( strNickName );

			if( pNode )
			{
				TResult = EVENT_ERROR_SUCCESS;

				i3NetworkPacket SendPacket( PROTOCOL_BASE_GET_USER_DETAIL_INFO_ACK );
				SendPacket.WriteData( &i32SessionIdx,		sizeof( INT32 ) ); 
				SendPacket.WriteData( &TUID,				sizeof( T_UID ) );
				SendPacket.WriteData( &TResult,				sizeof(T_RESULT) ); 
				
				_SetUserDetailInfo( stInfo, pNode );
				
				SendPacket.WriteData( &stInfo,				sizeof(S2_USER_DETAIL_INFO) ); 
				SendGamePacket( ui32ServerIdx, &SendPacket );
			}
			else
			{
				RING_USER_INFO_IN RingIn;
				RingIn.m_i32ServerIdx		= ui32ServerIdx;
				RingIn.m_i32SessionIdx		= i32SessionIdx;
				RingIn.m_TUID				= TUID;
				RingIn.m_eFindState			= USER_INFO_FIND_STATE_NICK;
				RingIn.m_TFindUID			= 0;

				i3String::Copy( RingIn.m_strNickName, strNickName, NET_NICK_NAME_SIZE );
				RingIn.m_strNickName[ ui8NickSize ] = '\0';

				m_pDBUserInfo->PushUserInfo( &RingIn );
			}
					
		}
		break;

		// 클랜 마크 변경
	case PROTOCOL_BASE_CHANGE_CLAN_MARK_REQ :	
		{
			UINT32		ui32ClanMark;
			UINT8		ui8UserCount;
			T_UID		pTUIDList[ MAX_MEMBER_COUNT ];
						 
			pPacket->ReadData( &ui32ClanMark,			sizeof(UINT32));
			pPacket->ReadData( &ui8UserCount,			sizeof(UINT8));
			pPacket->ReadData( pTUIDList,				sizeof(T_UID)*ui8UserCount );

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID( pTUIDList[i] );
				if( pNode )
				{
					pNode->_sClan._ui32Mark	= ui32ClanMark;
					pNode->_ui32UpdateInfo |= SAVE_DATABASE_BASEINFO;
				}
			}
		}
		break;

	// 클랜 이름 변경
	case PROTOCOL_BASE_CHANGE_CLAN_NAME_REQ :	
		{	
			TTCHAR		strClanName[NET_CLAN_NAME_SIZE];
			UINT8		ui8UserCount;
			T_UID		pTUIDList[ MAX_MEMBER_COUNT ];
						 
			pPacket->ReadData( strClanName,				sizeof(TTCHAR) * NET_CLAN_NAME_SIZE);
			pPacket->ReadData( &ui8UserCount,			sizeof(UINT8));
			pPacket->ReadData( pTUIDList,				sizeof(T_UID)*ui8UserCount );

			for( UINT8 i = 0 ; i < ui8UserCount ; i++ )
			{
				USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID( pTUIDList[i] );
				if( pNode )
				{
					i3String::Copy( pNode->_sClan._strClanName, strClanName, NET_CLAN_NAME_SIZE );
					pNode->_ui32UpdateInfo |= SAVE_DATABASE_BASEINFO;
				}
			}
		}
		break;
	
	case PROTOCOL_COMMUNITY_USER_INVITED_REQ:
		{				
			PACKET_TRANS_COMMUNITY_USER_INVITED_REQ		PacketReq;
			INT32 i32Rv = EVENT_ERROR_SUCCESS;

			USER_NODE* HostNode;
			USER_NODE* InviteeNode = NULL;
			
			pPacket->ReadData(&PacketReq,	sizeof(PACKET_TRANS_COMMUNITY_USER_INVITED_REQ));

			// 초대요청자와 초대자가 온라인상태인지 확인
			if ( NULL == (HostNode = g_pUserFinder->GetUserNodeByUID(PacketReq.m_i64UID)) )
			{
				i32Rv = EVENT_ERROR_EVENT_FRIEND_INVITED_PARAMETER_USER;
			}
			else if ( NULL == (InviteeNode = g_pUserFinder->GetUserNodeByUID(PacketReq.m_i64InviteUser)) )
			{
				i32Rv = EVENT_ERROR_EVENT_FRIEND_INVITED_NOT_CONNECTED;
			}			

			if (EV_FAILED(i32Rv))
			{
				/* Do nothing*/
				g_pLog->WriteLog( L"[ERROR] User Invited Fail [uid:%I64d][Error:%x]",PacketReq.m_i64UID, i32Rv );

				// Send Game Server
				i3NetworkPacket sendPacket(PROTOCOL_COMMUNITY_USER_INVITED_ACK);
				PACKET_TRANS_COMMUNITY_USER_INVITED_ACK	PacketAck;
				PacketAck.m_i32Rv				= EVENT_ERROR_EVENT_FAIL_INVITED_USER;
				PacketAck.m_i32SessionIdx		= PacketReq.m_i32SessionIdx;
				PacketAck.m_i64UID				= PacketReq.m_i64UID;
				PacketAck.m_i64InviteUserUID	= PacketReq.m_i64InviteUser;
				sendPacket.WriteData(&PacketAck, sizeof(PACKET_TRANS_COMMUNITY_USER_INVITED_ACK));
				SendGamePacket(ui32ServerIdx, &sendPacket);
			}
			else
			{
				
				if (0 == InviteeNode->_i32Serveridx)
				{
					g_pLog->WriteLog( L"[ERROR] User Invited Fail2 [uid:%I64d][Error:%x]",PacketReq.m_i64UID, i32Rv );

					// Send Game Server
					i3NetworkPacket sendPacket(PROTOCOL_COMMUNITY_USER_INVITED_ACK);
					PACKET_TRANS_COMMUNITY_USER_INVITED_ACK	PacketAck;
					PacketAck.m_i32Rv				= EVENT_ERROR_EVENT_FAIL_INVITED_USER;
					PacketAck.m_i32SessionIdx		= PacketReq.m_i32SessionIdx;
					PacketAck.m_i64UID				= PacketReq.m_i64UID;
					PacketAck.m_i64InviteUserUID	= PacketReq.m_i64InviteUser;
					sendPacket.WriteData(&PacketAck, sizeof(PACKET_TRANS_COMMUNITY_USER_INVITED_ACK));
					SendGamePacket(ui32ServerIdx, &sendPacket);
				}
				else
				{
					i3NetworkPacket sendPacket(PROTOCOL_COMMUNITY_USER_INVITED_REQUEST_ACK);
					PACKET_TRANS_COMMUNITY_USER_INVITED_REQUEST_ACK	PacketAck;
					PacketAck.m_i32SessionIdx	= InviteeNode->_i32Sessionidx;
					PacketAck.m_i64UID			= InviteeNode->_i64UID;
					PacketAck.m_ConnectState	= HostNode->_ui32ConnectState;
					i3mem::Copy(PacketAck.m_strNick, HostNode->_sBasic.m_strNick, sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
					PacketAck.m_strNick[NET_NICK_NAME_SIZE-1] = '\0';
					sendPacket.WriteData(&PacketAck, sizeof(PACKET_TRANS_COMMUNITY_USER_INVITED_REQUEST_ACK));
					SendGamePacket(InviteeNode->_i32Serveridx, &sendPacket);
				}
			}			
		}
		break;
	case PROTOCOL_SS_GACHA_PURCHASE_REQ:
		{
			PACKET_GACHA_SS_PURCHASE_REQ stPurchaseReq;
			pPacket->ReadData(&stPurchaseReq, sizeof(stPurchaseReq));

			USER_NODE* pUser = g_pUserFinder->GetUserNodeByUID(stPurchaseReq.m_TUID);			
			if( !pUser ) 
			{
				PACKET_GACHA_SS_RESULT_HEADER stResultHeader;
				i3mem::FillZero( &stResultHeader, sizeof( PACKET_GACHA_SS_RESULT_HEADER ) );
				stResultHeader.m_eErrorCode			= GACHA_ERROR_USER;
				stResultHeader.m_TUID				= stPurchaseReq.m_TUID;
				stResultHeader.m_i32ResultCount		= 0;

				i3NetworkPacket stPacket(PROTOCOL_SS_GACHA_PURCHASE_ACK);
				stPacket.WriteData( &stResultHeader, sizeof(PACKET_GACHA_SS_RESULT_HEADER) ); 
				SendGamePacket(ui32ServerIdx, &stPacket);

				g_pLog->WriteLog( L"[ERROR][Gacha_Puchase_Req] Not found user node : %I64d", stPurchaseReq.m_TUID); 
				break;
			}
			
			GACHA_ERROR eErrorCode = RecvGachaPurchaseReq(pUser, &stPurchaseReq);
			if( eErrorCode != GACHA_ERROR_SUCCESS )
			{
				DB_GACHA stGacha; 
				i3mem::Copy( &stGacha, &stPurchaseReq, sizeof( GACHA_PURCHACE_BASE ) );
				stGacha.m_i32ResultCount = 0;
				SendGachaPurchaseAck(pUser, eErrorCode, &stGacha );
			}
		}
		break;

	case PROTOCOL_SS_GACHA_NOTICE_REQ:
		{
			if( !g_pConfig->IsContentsEnabled(CONTENTS_GACHA) )	break;

			CGachaShop* pGachaShop = g_pConfig->GetGachaShop();

			UINT8 ui8NoticeCount = (UINT8)pGachaShop->GetNoticeCount();

			i3NetworkPacket SendPacket(PROTOCOL_SS_GACHA_NOTICE_ACK);
			SendPacket.WriteData(pGachaShop->GetGachaShopMD5Key(),		sizeof(char) * NET_MD5_KEY_SIZE);
			SendPacket.WriteData(&ui8NoticeCount, sizeof(UINT8));

			for( INT32 i = 0; i < ui8NoticeCount; ++i )
			{
				PACKET_GACHA_NOTICE stNotice;
				pGachaShop->MakePacketGachaNotice(i, &stNotice); 
				SendPacket.WriteData(&stNotice, sizeof(PACKET_GACHA_NOTICE)); 
			}

			SendGamePacket(ui32ServerIdx, &SendPacket);
		}
		break;

	case PROTOCOL_SS_GACHA_ITEM_INFO_REQ:
		{
			if( !g_pConfig->IsContentsEnabled(CONTENTS_GACHA) )	break;

			GACHA_GROUP eGachaGroup;
			pPacket->ReadData( &eGachaGroup, sizeof(GACHA_GROUP) );

			if( eGachaGroup <= GACHA_GROUP_NONE ) break;
			if( eGachaGroup >= GACHA_GROUP_COUNT ) break;

			CGachaShop* pGachaShop = g_pConfig->GetGachaShop();
			SS_GACHA_GROUP_INFO stGachaGroup;
			pGachaShop->MakePacketGachaInfo(eGachaGroup, &stGachaGroup);

			bool bActive = true;
			if( GACHA_GROUP_NONE == stGachaGroup.m_eGachaGroup )
			{
				bActive = false;
			}

			i3NetworkPacket SendPacket(PROTOCOL_SS_GACHA_ITEM_INFO_ACK);
			SendPacket.WriteData(&eGachaGroup,							sizeof(GACHA_GROUP));
			SendPacket.WriteData(&bActive,								sizeof(bool));
			if( bActive )
			{
				//SendPacket.WriteData(&stGachaGroup.m_ui32PaymentGoodsID,	sizeof(UINT32));
				//SendPacket.WriteData(&stGachaGroup.m_i32RequireTicket,		sizeof(INT32));
				SendPacket.WriteData(&stGachaGroup.m_ui32LuckyGoodsID,		sizeof(UINT32));
				SendPacket.WriteData(&stGachaGroup.m_i32LuckyProbability,	sizeof(INT32));
				//SendPacket.WriteData(&stGachaGroup.m_i32GloryCount,			sizeof(INT32));

				SendPacket.WriteData(&stGachaGroup.m_i32WinItemCount,		sizeof(INT32));
				SendPacket.WriteData(stGachaGroup.m_arWinItems,				(UINT16)(sizeof(SS_GACHA_ITEM_INFO)*stGachaGroup.m_i32WinItemCount) );

				SendPacket.WriteData(&stGachaGroup.m_i32RandomItemCnt,		sizeof(INT32));
				SendPacket.WriteData(stGachaGroup.m_arRandomGoodsIDs,		(UINT16)(sizeof(UINT32)*stGachaGroup.m_i32RandomItemCnt) );
			}			

			SendGamePacket(ui32ServerIdx, &SendPacket);

		}
		break;

	case PROTOCOL_GMCHAT_APPLY_PENALTY_REQ:
		{
			INT32	i32Sessionidx;
			T_UID	i64UID;
			INT8	i8PenaltyType;
			INT32	i32PenaltyAmount;

			pPacket->ReadData( &i32Sessionidx,		sizeof(INT32));
			pPacket->ReadData( &i64UID,				sizeof(T_UID));
			pPacket->ReadData( &i8PenaltyType,		sizeof(INT8));
			pPacket->ReadData( &i32PenaltyAmount,	sizeof(INT32));

			DirectUserPenalty( ui32ServerIdx, i32Sessionidx, i64UID, i8PenaltyType, i32PenaltyAmount );
				
		}
		break;

	case PROTOCOL_AUTH_USE_GIFTCOUPON_REQ:
		{
			INT32	i32Sessionidx;
			T_UID	i64UID;
			char	strCoupon[GIFTCOUPON_LENGTH + 1];

			pPacket->ReadData( &i32Sessionidx,		sizeof(INT32));
			pPacket->ReadData( &i64UID,				sizeof(T_UID));
			pPacket->ReadData( strCoupon,			GIFTCOUPON_LENGTH + 1);

			USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID( i64UID );
			if( NULL == pNode )	break;

			DB_COUPON DBIn;
			DBIn.m_TUID		= i64UID;
			DBIn.m_eType	= DB_COUPON_TYPE_USE;
			i3mem::Copy( &DBIn.m_strCoupon, strCoupon, GIFTCOUPON_LENGTH + 1 );

			if( RINGBUFFER_SUCCESS != g_pModuleDBShop->InCoupon( &DBIn ) )
			{
				strCoupon[GIFTCOUPON_LENGTH] = '\0';
				// 쿠폰 안쓰니까 strCoupon 걍 처리합니다. 20150708 김현우
				g_pLog->WriteLog( L"SIA Send Fail - Use GiftCoupon / UID : %I64d / CouponCode : %s", i64UID, strCoupon );
			}
		}
		break;

	case PROTOCOL_AUTH_SHOP_REWARD_ITEM_REQ:			// 보상 아이템 선물 지급
		{
			SEND_GIFT	stSendGift;
			UINT8		ui8RequestType;
			
			pPacket->ReadData( &stSendGift.m_TReceiverUID,					sizeof(T_UID) );
			pPacket->ReadData( &ui8RequestType,								sizeof(UINT8));
			pPacket->ReadData( &stSendGift.m_ui8GiftCount,					sizeof(UINT8));
			for( UINT8 i = 0 ; i < stSendGift.m_ui8GiftCount ; ++i )
			{
				pPacket->ReadData( &stSendGift.m_aGift[i].m_ui32GoodsID,	sizeof(T_GoodsID) ); 
			}

			 /*  
			USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID( stSendGift.m_TReceiverUID );
			if( NULL == pNode ) break; */
						
			stSendGift.m_eGiftType	= GIFT_TYPE_EVENT;
			stSendGift.m_TSenderUID	= 0;

			_GiftInsertDB( &stSendGift, (ITEM_INSERT_REQUEST_TYPE)ui8RequestType); 
		}
		break;

	case PROTOCOL_AUTH_SHOP_EXPIRE_DELETE_ITEM_REQ:
	{
		T_UID		TUID;
		UINT16		ui16ItemCount;
		T_ItemDBIdx	aTItemDBIdx[MAX_INVEN_COUNT];

		pPacket->ReadData(&TUID,			sizeof(T_UID));
		pPacket->ReadData(&ui16ItemCount,sizeof(UINT16));
		pPacket->ReadData(aTItemDBIdx,		sizeof(T_ItemDBIdx)* ui16ItemCount);

		ui16ItemCount = MIN(ui16ItemCount, MAX_INVEN_COUNT);

		USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID(TUID);
		if (!pNode) break;

		// 유저노드에서 삭제
		for (INT32 i = 0; i < ui16ItemCount; ++i)
		{
			UserDeleteItem(pNode, aTItemDBIdx[i]);

			DB_SHOP_IN DBShopIn;
			DBShopIn._ui8InsertType = TYPE_SHOP_ITEM_DELETE_NOACK;
			DBShopIn._i64UID		= TUID;
			DBShopIn._TWareDBIdx	= aTItemDBIdx[i];
			DBShopIn._ui32ItemArg	= 0xFFFFFFFF;
			DBShopIn._i8SlotIdx		= -1;

			if (RINGBUFFER_SUCCESS != g_pModuleDBShop->InInsertItem(&DBShopIn))
			{	// Error	 
				g_pLog->WriteLog(L"BUFFER OVER - DBBuffer Expire Item Delete / UID : %I64d / ItemWareIdx : %d", TUID, aTItemDBIdx[i]);
				break;
			}
		}
	}
	break;

	default: 
		{			
			g_pLog->WriteLog( L"[WARNING] CTaskProcessor::_ParsingGame() - Unknown protocol id(%d)", pPacket->GetProtocolID() );
		}
		break; 
	}

	return; 
}
