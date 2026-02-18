#include "pch.h"
#include "ClientTCPSocket.h"

#include "../StageBattle/AIModeContext.h"
#include "UserInfoContext.h"
#include "LobbyContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "QuickJoinContext.h"
#include "CommunityContext.h"

#include "UI/UIMainFrame.h"
#include "ClanGameContext.h"
#include "ChannelContext.h"

#include "ValidCheck.h"

#include "i3Base/string/ext/mb_to_utf16.h"




namespace
{
	const char * getProtocolName( PROTOCOL protocol)
	{
		switch( protocol)
		{
		case PROTOCOL_LOBBY_ENTER_ACK : return "PROTOCOL_LOBBY_ENTER_ACK";
		case PROTOCOL_LOBBY_LEAVE_ACK : return "PROTOCOL_LOBBY_LEAVE_ACK";
		case PROTOCOL_LOBBY_GET_ROOMLIST_ACK : return nullptr;
		case PROTOCOL_LOBBY_GET_ROOMINFO_ACK : return "PROTOCOL_LOBBY_GET_ROOMINFO_ACK";
		case PROTOCOL_LOBBY_QUICKJOIN_ROOM_ACK : return "PROTOCOL_LOBBY_QUICKJOIN_ROOM_ACK";
		case PROTOCOL_LOBBY_GET_ROOMINFOADD_ACK : return "PROTOCOL_LOBBY_GET_ROOMINFOADD_ACK";
		case PROTOCOL_LOBBY_ABUSING_POPUP_END_ACK : return "PROTOCOL_LOBBY_ABUSING_POPUP_END_ACK";
		case PROTOCOL_LOBBY_CHATTING_ACK : return "PROTOCOL_LOBBY_CHATTING_ACK";
		case PROTOCOL_LOBBY_NEW_VIEW_USER_ITEM_ACK : return "PROTOCOL_LOBBY_NEW_VIEW_USER_ITEM_ACK";
		case PROTOCOL_LOBBY_VIEW_USER_ITEM_ACK: return "PROTOCOL_LOBBY_VIEW_USER_ITEM_ACK";
		default :
			I3PRINTLOG(I3LOG_FATAL,  "PROTOCOL_LOBBY_XXX 프로토콜 이름 등록해주이소~");
			return "PROTOCOL_LOBBY_UNKNOWN";
		}
	}
}

void ClientTCPSocket::_PacketParsingLobby(i3NetworkPacket  * pPacket)
{
	i3::unordered_map< UINT32, netFunc >::iterator it;
	it = m_NetFuncMap_Lobby.find( pPacket->GetProtocolID());

	if ( it != m_NetFuncMap_Lobby.end() )
	{
		#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
		const char * pszProtocolName = getProtocolName( pPacket->GetProtocolID());
		if( pszProtocolName != nullptr)
		{
			I3PRINTLOG(I3LOG_QA, "* %s -->", pszProtocolName);
		}
		#endif

		netFunc & mf = it->second;
		(this->*mf)(pPacket);

		#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
		if( pszProtocolName != nullptr)
		{
			I3PRINTLOG(I3LOG_QA, "* %s <--", pszProtocolName);
		}
		#endif
	}
	else
		__Parse_UnknownProtocol( pPacket);
}

void ClientTCPSocket::__Register_Lobby_Function( void)
{
	RegisterParser( PROTOCOL_LOBBY_ENTER_ACK,					&ClientTCPSocket::__Parse_Lobby_Enter); 
	RegisterParser( PROTOCOL_LOBBY_LEAVE_ACK,					&ClientTCPSocket::__Parse_Lobby_Leave);
	RegisterParser( PROTOCOL_LOBBY_GET_ROOMLIST_ACK,			&ClientTCPSocket::__Parse_Lobby_ReceiveRoomList);
	RegisterParser( PROTOCOL_LOBBY_GET_ROOMINFO_ACK,			&ClientTCPSocket::__Parse_Lobby_ReceiveRoomInfo);
	RegisterParser( PROTOCOL_LOBBY_QUICKJOIN_ROOM_ACK,			&ClientTCPSocket::__Parse_Lobby_QuickJoinRoom);
	RegisterParser( PROTOCOL_LOBBY_GET_ROOMINFOADD_ACK,			&ClientTCPSocket::__Parse_Lobby_ReceiveRoomAddableInfo);
	RegisterParser( PROTOCOL_LOBBY_ABUSING_POPUP_END_ACK,		&ClientTCPSocket::__Parse_Lobby_WaitingEndAbusing);
	RegisterParser( PROTOCOL_LOBBY_CHATTING_ACK,				&ClientTCPSocket::__Parse_Lobby_Chat);
	RegisterParser( PROTOCOL_LOBBY_NEW_VIEW_USER_ITEM_ACK,		&ClientTCPSocket::__Parse_Lobby_ViewUserItem);
	RegisterParser( PROTOCOL_LOBBY_VIEW_USER_ITEM_ACK,			&ClientTCPSocket::__Parser_Lobby_ViewUserItemForUserProfile);
}

void ClientTCPSocket::__Parse_Lobby_Enter( i3NetworkPacket * pPacket)
{
	INT32	i32Rv;
	UINT8	ui8EnterType;			// 입장종류. S2MO_ROOM_ENTER
	UINT8	ui8IsMerPenalty;		// 클랜 매치메이킹 - 자기자신의 용병 패널티 여부
	pPacket->ReadData( &i32Rv,			sizeof(INT32)); 
	pPacket->ReadData( &ui8EnterType,	sizeof(UINT8));
	pPacket->ReadData( &ui8IsMerPenalty, sizeof(UINT8));
	ClanGameContext::i()->SetMyMercDepartured(ui8IsMerPenalty > 0 ? true : false);
	
	if (EV_FAILED(i32Rv))
	{
		QuickJoinContext::i()->setPassingthroughLobby(false);
		I3PRINTLOG(I3LOG_NOTICE,"[QuickJoin] Lobby entrance failed!!");
	}
	else
	{
		if (ui8EnterType == S2MO_ROOM_ENTER_QUICK_JOIN)
			QuickJoinContext::i()->setPassingthroughLobby(true);
		else
			QuickJoinContext::i()->setPassingthroughLobby(false);
	}
#if defined(AUTO_LOBBY)
	// 처음 닉네임 만들기.
	// 처음 로그인.
	if( false == CCharaInfoContext::i()->HasBasicChara() )
		g_pFramework->GetUIMainframe()->MovePhase( UIPHASE_CREATE_CHARA);
	else
	{// revision 59499
		if( ChannelContext::i()->GetBattleChannel() )
		{
			// 클랜 팀만들기 기능이 활성화된 경우 클랜로비로 이동합니다.- 클랜 매치메이킹
			if( UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW) &&
				ClanGameContext::i()->getMyMercenaryState() == false)
			{
				g_pFramework->GetUIMainframe()->MovePhase( UIPHASE_CLANLOBBY);
				ClanGameContext::i()->setClanBattleLobbyEnable(true);
				ClanGameContext::i()->resetMyMatchSquadInfo();
			}
			else // 클랜로비가 아닌 일반로비로 바로 이동합니다.
			{
				g_pFramework->GetUIMainframe()->MovePhase(UIPHASE_LOBBY);
				ClanGameContext::i()->setClanBattleLobbyEnable(false);
			}
		}
		else
			g_pFramework->GetUIMainframe()->MovePhase( UIPHASE_LOBBY);
	}
#else
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_ENTER_LOBBY);
#endif
}

void ClientTCPSocket::__Parse_Lobby_Leave( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData(&Temp, sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_LEAVE_LOBBY, Temp);
}

void ClientTCPSocket::__Parse_Lobby_ReceiveRoomList( i3NetworkPacket * pPacket)
{
	//예외처리를 해야한다. 
	INT32 Temp;
	INT32 PageNum;

	pPacket->ReadData( &Temp, sizeof(INT32)); 

	I3_BOUNDCHK( Temp, MAXIMUM_ROOM_COUNT);	// 300개 이상의 방은 없을 것
	LobbyContext::i()->setLobbyRoomCount( Temp );

	pPacket->ReadData( &PageNum, sizeof(INT32));
	I3_BOUNDCHK( PageNum, MAXIMUM_ROOM_COUNT/10);
	LobbyContext::i()->setLobbyRoomPage( PageNum );		//Page

	pPacket->ReadData( &Temp, sizeof(INT32));
	if ( Temp > 0)
	{
		I3ASSERT( Temp < MAXIMUM_ROOM_COUNT);
		ROOM_INFO_BASIC * pRoomInfoList = (ROOM_INFO_BASIC *)i3MemAlloc(sizeof(ROOM_INFO_BASIC) * Temp);
		pPacket->ReadData( pRoomInfoList, sizeof(ROOM_INFO_BASIC) * Temp);
		LobbyContext::i()->SetRoomInfo( PageNum, Temp, pRoomInfoList );
		i3MemFree(pRoomInfoList);
	}

	// Lobby UserList
	pPacket->ReadData( &Temp,		sizeof(INT32));
	I3ASSERT( Temp < MAXIMUM_USER_COUNT);	/// 5천명을 넘을 수 있을까?
	LobbyContext::i()->SetLobbyUserCount( Temp );

	pPacket->ReadData( &PageNum,	sizeof(INT32));
	I3ASSERT( PageNum < MAXIMUM_USER_COUNT/10);
	pPacket->ReadData( &Temp,		sizeof(INT32));
	if ( Temp > 0 )
	{
		I3ASSERT( Temp < MAXIMUM_USER_COUNT/10);
		LOBBY_USER_INFO_LIST * pUserInfoList = (LOBBY_USER_INFO_LIST*)i3MemAlloc(sizeof(LOBBY_USER_INFO_LIST)*Temp);
		pPacket->ReadData( pUserInfoList,  sizeof(LOBBY_USER_INFO_LIST)*Temp);
		LobbyContext::i()->SetLobbyUserInfo(PageNum, Temp, pUserInfoList);
		i3MemFree(pUserInfoList);
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_UPDATE_USER_LIST);
}

void ClientTCPSocket::__Parse_Lobby_ReceiveRoomInfo( i3NetworkPacket * pPacket)
{
	ROOM_INFO_BASIC	roomInfo;
	pPacket->ReadData( &roomInfo, sizeof(ROOM_INFO_BASIC) );
	MyRoomInfoContext::i()->setMyRoomInfo(&roomInfo);
}

void ClientTCPSocket::__Parse_Lobby_CreateRoom( i3NetworkPacket * pPacket)
{
	I3PRINTLOG(I3LOG_DEFALUT,  "* CREATE ROOM");

	PACKET_LOBBY_CREATE_ROOM_ACK CretaRoomData;
	
	pPacket->ReadData( &CretaRoomData.m_i32RoomIdx,		sizeof(INT32) );

	if(CretaRoomData.m_i32RoomIdx > -1)
	{
		pPacket->ReadData( &CretaRoomData.m_RoomInfoBasic,	sizeof(ROOM_INFO_BASIC) );
		pPacket->ReadData( &CretaRoomData.m_RoomInfoAdd,	sizeof(ROOM_INFO_ADD) );

		MyRoomInfoContext::i()->setMyRoomInfo(&CretaRoomData.m_RoomInfoBasic);
		MyRoomInfoContext::i()->setMyRoomInfoAdd(&CretaRoomData.m_RoomInfoAdd);

		//Set Slot Info
		USER_INFO_BASIC MyUserInfoBasic;
		UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);

		USER_INFO_INVITEM_DATA myUserInfoInvItemData;
		UserInfoContext::i()->GetMyUserInfoInvItemData(&myUserInfoInvItemData);

		UINT8 rank;

		if (myUserInfoInvItemData._wAprilFoolRank != RANK_NONE)
			rank = (UINT8)myUserInfoInvItemData._wAprilFoolRank;
		else if(myUserInfoInvItemData._wRank != RANK_NONE)
			rank = (UINT8)myUserInfoInvItemData._wRank;
		else
			rank = (UINT8)MyUserInfoBasic.m_ui32Rank;

		BattleSlotContext::i()->ResetSlotInfos();
		BattleSlotContext::i()->SetSlotInfo( 0, SLOT_STATE_NORMAL, rank, MyUserInfoBasic.m_ui32Clanidx,
									MyUserInfoBasic.m_ui32ClanState, MyUserInfoBasic.m_ui8ClanExpRank, MyUserInfoBasic.m_ui32ClanMark,
									MyUserInfoBasic.m_ui8PCCafe, MyUserInfoBasic.m_ui8ESportID, UserInfoContext::i()->GetNationalCode());

		BattleSlotContext::i()->setNickColorForSlot(0, myUserInfoInvItemData._ui8NickColor);
		BattleSlotContext::i()->setNickForSlot(0, MyUserInfoBasic.m_strNick); 		
		
		//Set MyIndex
		BattleSlotContext::i()->setMySlotIdx( 0 );
		BattleSlotContext::i()->setMainSlotIdx( 0 );
		CCharaInfoContext::i()->SetTeamSlotBySlotIdx( 0 );
	}

	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
	{
		GameEventSender::i()->SetEvent( EVENT_READY_BATTLE );
	}
	else
	{
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CREATE_ROOM, CretaRoomData.m_i32RoomIdx);
	}
}

void ClientTCPSocket::__Parse_Lobby_JoinRoom( i3NetworkPacket * pPacket)
{
	I3PRINTLOG(I3LOG_DEFALUT,  "* JOIN ROOM");

	PACKET_ROOM_JOIN_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	if( EV_SUCCESSED( Recv.m_TResult ) )
	{	
		MyRoomInfoContext::i()->setMyRoomInfo( &Recv.m_RoomInfoBasic );
		MyRoomInfoContext::i()->setMyRoomInfoAdd( &Recv.m_RoomInfoAdd );
		MyRoomInfoContext::i()->setMyRoomScore( Recv.m_RoomNowScore );
		if (Recv.m_strRoomPasswd.GetCount() > 0)
			MyRoomInfoContext::i()->SetMyRoomPW(&Recv.m_strRoomPasswd, NET_ROOM_PW);
		else
			MyRoomInfoContext::i()->ClearMyRoomPW(); //SetMyRoomPW(&Recv.m_strRoomPasswd, NET_ROOM_PW);
		MyRoomInfoContext::i()->SetQuickJoin( (Recv.m_ui8EnterType>0)?true:false);

		// 카운트 다운 설정
		if( Recv.m_i8StartCountDown > 0 ) 
			MyRoomInfoContext::i()->SetCountDown( (INT32)Recv.m_i8StartCountDown);

		// 방장 설정
		BattleSlotContext::i()->setMainSlotIdx( Recv.m_ui8MainSlotIdx );
	
		if( (Recv.m_ui8PersonNow == 0) || (Recv.m_ui8PersonNow > SLOT_MAX_COUNT))
		{
			I3PRINTLOG(I3LOG_FATAL,  "invalid size");
			return;
		}
		
		BattleSlotContext::i()->ResetSlotInfos();

		// 슬롯 정보 설정
		for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
		{
			i3mem::Copy( BattleSlotContext::i()->setSlotInfo(i), &Recv.m_aSlotTotalInfo[i], sizeof(SLOT_INFO));
			BattleSlotContext::i()->setNickForSlot(i, Recv.m_aSlotTotalInfo[i].m_strNickName);
			BattleSlotContext::i()->setNickColorForSlot( i, Recv.m_aSlotTotalInfo[i].m_ui8NickColor );
			BattleSlotContext::i()->SetUseCashItem(i);
			BattleSlotContext::i()->setFireMuzzleColorForSlot(i, Recv.m_aSlotTotalInfo[i].m_ui16MuzzleFlashColor);
			ClanGameContext::i()->SetMercRank(i, Recv.m_aSlotTotalInfo[i].m_ui8MerClass, Recv.m_aSlotTotalInfo[i].m_ui8MerRank); 
		}

		if( (MyRoomInfoContext::i()->getStage()->IsAiMode()))
		{
			AIModeContext::i()->EnemyCount = Recv.m_i8EnemyAiCount;
			AIModeContext::i()->StartLevel = Recv.m_i8DifficultyLevel;
		}
		BattleSlotContext::i()->setMySlotIdx( Recv.m_ui8Slot );
		CCharaInfoContext::i()->SetTeamSlotBySlotIdx( Recv.m_ui8Slot );
	}
	else
	{
		BattleSlotContext::i()->setMySlotIdxEx(	0 );
	}

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_JOIN_ROOM, Recv.m_TResult );
}

void ClientTCPSocket::__Parse_Lobby_QuickJoinRoom( i3NetworkPacket * pPacket)
{
	INT32 Temp;

	pPacket->ReadData( &Temp, sizeof(INT32) );
	if(Temp > -1)
	{
		ROOM_INFO_BASIC	roomInfo;
		ROOM_INFO_ADD	roomAddInfo;
		pPacket->ReadData( &Temp,	sizeof(INT32) );
		pPacket->ReadData(&roomInfo,		sizeof(ROOM_INFO_BASIC));
		pPacket->ReadData(&roomAddInfo,		sizeof(ROOM_INFO_ADD));
		MyRoomInfoContext::i()->setMyRoomInfo(&roomInfo);
		MyRoomInfoContext::i()->setMyRoomInfoAdd(&roomAddInfo);

		if ((MyRoomInfoContext::i()->getStage()->IsAiMode()))
		{
			INT8 enemyAiCount;
			INT8 difficultyLevel;

			pPacket->ReadData(&enemyAiCount, sizeof(INT8));
			pPacket->ReadData(&difficultyLevel, sizeof(INT8));

			AIModeContext::i()->EnemyCount = enemyAiCount;
			AIModeContext::i()->StartLevel = difficultyLevel;
		}

		BattleSlotContext::i()->setMySlotIdx( Temp );
		CCharaInfoContext::i()->SetTeamSlotBySlotIdx( Temp );
	}
	else
	{
		BattleSlotContext::i()->setMySlotIdxEx(	0 );
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_JOIN_QUICK, Temp);
}

void ClientTCPSocket::__Parse_Lobby_CreateNickname( i3NetworkPacket * pPacket)
{
	PACKET_BASE_CREATE_NICK_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	if( EV_SUCCESSED( Recv.m_TResult ) )
	{
		COMMON_PARTS_INFO CommonPartsInfo;
		// vv3 - check
		CommonPartsInfo.SetPartsIdx( Recv.m_aCommonEquipment[ CHAR_EQUIPMENT_COMMON_DINO ].m_TItemDBIdx );
		if( ValidCheck::Item::isDinoItemID( Recv.m_aCommonEquipment[ CHAR_EQUIPMENT_COMMON_DINO ].m_TItemID) == true)
			CommonPartsInfo.SetPartsItemID( Recv.m_aCommonEquipment[ CHAR_EQUIPMENT_COMMON_DINO ].m_TItemID );
		else
			CommonPartsInfo.SetPartsItemID(DEFAULT_DINO);
		CCharaInfoContext::i()->SetCommonPartsInfo( CommonPartsInfo );
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CREATE_NICK_NAME, Recv.m_TResult );
}

void ClientTCPSocket::__Parse_Lobby_ReceiveRoomAddableInfo( i3NetworkPacket * pPacket)
{
	pPacket->ReadData(LobbyContext::i()->setSelectedRoomInfoAdd(), sizeof(ROOM_INFO_ADD));
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_GET_ROOM_ADDINFO_N);
}

void ClientTCPSocket::__Parse_Lobby_WaitingEndAbusing( i3NetworkPacket * pPacket)
{
	//어뷰징(Abusing) 팝업 시간을 다 기다린경우 보상 지급 : 하동익
	UINT32	m_Exp, m_GetExp;
	UINT32	m_Point, m_GetPoint;
	USER_INFO_BASIC		MyUserInfoBasic;

	pPacket->ReadData(&m_Exp,	sizeof(INT32));
	pPacket->ReadData(&m_Point,	sizeof(INT32));
	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);

	m_GetExp = MyUserInfoBasic.m_ui32Exp;
	m_GetPoint = MyUserInfoBasic.m_ui32Point;
	m_GetExp = m_Exp + m_GetExp;
	m_GetPoint = m_Point + m_GetPoint;
	MyUserInfoBasic.m_ui32Exp = m_GetExp;
	MyUserInfoBasic.m_ui32Point = m_GetPoint;
	UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);
}

void ClientTCPSocket::__Parse_Lobby_Chat( i3NetworkPacket * pPacket)
{
	INT32	i32SessionIdx;
	UINT8	ui8NickSize;
	UINT16	strlen;
	TTCHAR	pNickName[NET_NICK_NAME_SIZE];
	TTCHAR	pString[MAX_STRING_COUNT];
	bool	bGameMaster;
	::memset(pString, 0, sizeof(TTCHAR) * MAX_STRING_COUNT);

	UINT8	ui8NickColor;

	pPacket->ReadData( &i32SessionIdx,	sizeof(INT32) );
	pPacket->ReadData( &ui8NickSize,	sizeof(UINT8) );
	if( ui8NickSize > NET_NICK_NAME_SIZE ) ui8NickSize = NET_NICK_NAME_SIZE;
	pPacket->ReadData( pNickName,		ui8NickSize * sizeof(TTCHAR) );
	pPacket->ReadData( &ui8NickColor,	sizeof(UINT8) );	// 20110804 김현우 컬러닉
	pPacket->ReadData( &bGameMaster, sizeof(bool));			// 20140924 김경주 GM FLAG
	pPacket->ReadData( &strlen,			sizeof(UINT16) );

	if(strlen < MAX_STRING_COUNT)
	{
		pPacket->ReadData( pString,		strlen * sizeof(TTCHAR) );				
		pNickName[NET_NICK_NAME_SIZE - 1]	= 0;
		pString[MAX_STRING_COUNT - 1]		= 0;

		i3::rc_wstring wstrNickName = pNickName;
		i3::wstring wstrChat = pString;

		g_pFramework->getChatBox()->PutUserChatting( wstrNickName, wstrChat, bGameMaster, CHATTING_TYPE_LOBBY, ui8NickColor); 
	}
}

void ClientTCPSocket::__Parse_Lobby_ViewUserItem( i3NetworkPacket * pPacket)
{
	__Parse_Room_GetUserItem(pPacket); //같은 내용이므로 이 함수를 호출
}

void ClientTCPSocket::__Parser_Lobby_ViewUserItemForUserProfile( i3NetworkPacket* pPacket)
{
	//1.1 상세보기 정보를 위하여 만든 패킷
	//UINT32 ui32ItemCount, ui32ItemID[20]
	//이 아래 2개 데이터는 받기만 한다. 사용하진 않음.
	//pPacket->ReadData( &ui32ItemCount,sizeof(INT32));	// 사용중인 캐쉬아이템(기간제)  갯수
	//pPacket->ReadData( ui32ItemID,	(UINT16)(sizeof(UINT32)*ui32ItemCount)); // 사용중인 
	PACKET_LOBBY_VIEW_USER_ITEM_ACK Packet;
	S2MORecvPacket( &Packet, pPacket->GetPacketBuffer() );

	UI_CHARA_INFO * p_AnotherUserInfo = CCommunityContext::i()->SetAnotherUserInfo(0);
	
	//파츠 정보 변환
	CHARA_PARTS_INFO* p_CharaParts = nullptr;
	for( int i = 0 ; i < TEAM_COUNT ; i++ )
	{
		int temp = g_pEnvSet->IsV2Version() ? TEAM_RED : i;
		p_CharaParts = p_AnotherUserInfo->setCharaParts(i);
		p_CharaParts->SetPartsItemID( CHAR_EQUIPMENT_PARTS_CHARA,	Packet.m_TCharItemID[temp] );
		p_CharaParts->SetPartsItemID( CHAR_EQUIPMENT_PARTS_HEAD,	Packet.m_aCharaEquip[CHAR_EQUIPMENT_HEAD]  );
		p_CharaParts->SetPartsItemID( CHAR_EQUIPMENT_PARTS_FACE,	Packet.m_aCharaEquip[CHAR_EQUIPMENT_FACE]  );
		p_CharaParts->SetPartsItemID( CHAR_EQUIPMENT_PARTS_UPPER,	Packet.m_aCharaEquip[CHAR_EQUIPMENT_UPPER] );
		p_CharaParts->SetPartsItemID( CHAR_EQUIPMENT_PARTS_LOWER,	Packet.m_aCharaEquip[CHAR_EQUIPMENT_LOWER] );
		p_CharaParts->SetPartsItemID( CHAR_EQUIPMENT_PARTS_GLOVE,	Packet.m_aCharaEquip[CHAR_EQUIPMENT_GLOVE] );
		p_CharaParts->SetPartsItemID( CHAR_EQUIPMENT_PARTS_BELT,	Packet.m_aCharaEquip[CHAR_EQUIPMENT_BELT]  );
		p_CharaParts->SetPartsItemID( CHAR_EQUIPMENT_PARTS_HOLSTER, Packet.m_aCharaEquip[CHAR_EQUIPMENT_HOLSTER] );
		p_CharaParts->SetPartsItemID( CHAR_EQUIPMENT_PARTS_SKIN,	Packet.m_aCharaEquip[CHAR_EQUIPMENT_SKIN]  );
		p_CharaParts->SetPartsItemID( CHAR_EQUIPMENT_PARTS_BERET,	Packet.m_aCharaEquip[CHAR_EQUIPMENT_BERET] );
	}
	
	
	//무기 정보 변환
	p_AnotherUserInfo->setWeaponItemID(WEAPON_SLOT_PRIMARY,		Packet.m_aCharaEquip[CHAR_EQUIPMENT_PRIMARY]);
	p_AnotherUserInfo->setWeaponItemID(WEAPON_SLOT_SECONDARY,   Packet.m_aCharaEquip[CHAR_EQUIPMENT_SECONDARY]);
	p_AnotherUserInfo->setWeaponItemID(WEAPON_SLOT_MELEE,		Packet.m_aCharaEquip[CHAR_EQUIPMENT_MELEE]);
	p_AnotherUserInfo->setWeaponItemID(WEAPON_SLOT_THROWING1,	Packet.m_aCharaEquip[CHAR_EQUIPMENT_THROWING1]);
	p_AnotherUserInfo->setWeaponItemID(WEAPON_SLOT_THROWING2,	Packet.m_aCharaEquip[CHAR_EQUIPMENT_THROWING2]);

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_USER_DETAIL_INFO);
}