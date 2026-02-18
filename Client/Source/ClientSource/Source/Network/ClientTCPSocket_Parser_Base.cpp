#include "pch.h"
#include "../../../Common/CommonStructure/ShareClient/ProtocolStruct.h"
#include "ClientTCPSocket.h"
#include "SecurityHeader.h"
#include "IGShopItemController.h"
#include "MedalManager.h"

#include "../GameStateMgr.h"
#include "../Mode/Domination/Domination_Defines.h"

#include "IGShopContext.h"
#include "CommunityContext.h"
#include "EnvInfoContext.h"
#include "MailBoxContext.h"
#include "MedalContext.h"
#include "AttendanceContext.h"
#include "ReadyBoxContext.h"
#include "GameLoginContext.h"
#include "UserInfoContext.h"
#include "TutorialContext.h"
#include "GMAuthorityContext.h"
#include "ClanSeasonContext.h"
#include "../StageBattle/UserContext.h"
#include "../StageBattle/NetworkContext.h"
#include "../ChannelContext.h"
#include "../QuickJoinContext.h"
#include "../GiftShopContext.h"
#include "../ShopContext.h"
#include "../LobbyContext.h"

#include "../UI/HUD/UIHUD_Domination_Manager.h"
#include "UI/UITabProfileSkill.h"
#include "UI/UIUtil.h"
#include "UI/UIMainFrame.h"

#include "UI/UIPopupOption.h"
#include "UI/UICharaInfoMgr.h"

#include "ValidCheck.h"

#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf8_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/RemoveColorString.h"

#include "Designation/DesigUtil.h"
#include "MissionCard/MCardUtil.h"

#include "GameNetworkManager.h"

#include "ClientStageInfoUtil.h"
#include "ClientStageInfoContext.h"
#if defined( _XTRAP_)
#include "../../Xtrap/Include/Xtrap_C_Interface.h"
#include "../../Xtrap/Include/Xtrap_S_Interface.h"
#endif

#include "SteamContext.h"

#define  MAX_MULTIWEAPON_SLOT 10
#define	 MIN_MULTIWEAPON_SLOT 0
#if defined( _XIGNCODE_)
extern void XCALL OnProbeComplete(xhandle CodeBox,
	CPACKETDATA ReceivedPacket,
	PACKETDATA Buffer,
	SIZE_T BufferSize,
	xpvoid Context)
{
	i3NetworkPacket SendPacket;
	SC_VERSION	ClientVersion;
	SendPacket.SetProtocol(PROTOCOL_BASE_GAMEGUARD_ACK);

	//LS CHECK
	HACK_CHECK_REALTIME hcRealTime;
	memset(&hcRealTime, 1, sizeof(HACK_CHECK_REALTIME));
#ifdef USE_LEUCO_SHELL
	LeucoShellCore* pLSCore = LeucoShellCore::GetInstance();
	if (pLSCore)
	{
		memset(&hcRealTime, 0, sizeof(HACK_CHECK_REALTIME));
		
		// DIRECT X
		hcRealTime.m_ui8Type[0] = (UINT16)pLSCore->GetDXG()->GetVFResult();
		hcRealTime.m_ui8Type[1] = (UINT16)pLSCore->GetDXG()->GetVDResult();

		hcRealTime.m_ui64DXFunction[0] = pLSCore->GetDXG()->GetDectectVF(0);
		hcRealTime.m_ui64DXFunction[1] = pLSCore->GetDXG()->GetDectectVF(1);

		hcRealTime.m_ui64DXDevice[0] = pLSCore->GetDXG()->GetDectectVD(0);
		hcRealTime.m_ui64DXDevice[1] = pLSCore->GetDXG()->GetDectectVD(1);

		// FUNC
		hcRealTime.m_ui8Type[2] = (UINT16)pLSCore->GetFCG()->GetResult();
		hcRealTime.m_ui8Type[3] = (UINT16)pLSCore->GetFCG()->GetDetectIndex();
		
		// VALUE
		hcRealTime.m_ui8Type[4] = (UINT16)LS::GLOBAL::FUNCTION::GetFakeValueIndex();

		//추가된 로그인 탐지 항목들
		//hcRealTime.m_ui8Type[5];
		//hcRealTime.m_ui8Type[6];
		//hcRealTime.m_ui8Type[7];
	}
	else
	{
		memset(&hcRealTime, 0, sizeof(HACK_CHECK_REALTIME));
		hcRealTime.m_ui8Type[0] = (UINT16)MFV_CORE_NULL;
		hcRealTime.m_ui8Type[1] = (UINT16)MFV_CORE_NULL;
		hcRealTime.m_ui8Type[2] = (UINT16)MFV_CORE_NULL;
		hcRealTime.m_ui8Type[3] = (UINT16)MFV_CORE_NULL;
		hcRealTime.m_ui8Type[4] = (UINT16)MFV_CORE_NULL;
		hcRealTime.m_ui8Type[5] = (UINT16)MFV_CORE_NULL;
		hcRealTime.m_ui8Type[6] = (UINT16)MFV_CORE_NULL;
		hcRealTime.m_ui8Type[7] = (UINT16)MFV_CORE_NULL;
	}
#else
	memset(&hcRealTime, 0, sizeof(HACK_CHECK_REALTIME));
#endif
	SendPacket.WriteData(&hcRealTime, sizeof(HACK_CHECK_REALTIME));
	//LS END


	SendPacket.WriteData(&ClientVersion, sizeof(SC_VERSION));
	SendPacket.WriteData(Buffer, sizeof(char) * XIGNCODE_PACKET_SIZE);

	CGameNetworkManager::i()->SendTCPMessage(&SendPacket);
}

#endif


namespace
{
	const char * getProtocolName( PROTOCOL protocol)
	{
		switch( protocol)
		{
		case PROTOCOL_LOGIN_ACK : return "PROTOCOL_LOGIN_ACK";
		case PROTOCOL_BASE_CREATE_NICK_ACK : return "PROTOCOL_BASE_CREATE_NICK_ACK";
		case PROTOCOL_BASE_CONNECT_ACK : return "PROTOCOL_BASE_CONNECT_ACK";
		case PROTOCOL_BASE_LOGIN_WAIT_ACK : return "PROTOCOL_BASE_LOGIN_WAIT_ACK";
		case PROTOCOL_BASE_GET_SYSTEM_INFO_ACK : return "PROTOCOL_BASE_GET_SYSTEM_INFO_ACK";
		case PROTOCOL_BASE_GET_USER_INFO_ACK : return "PROTOCOL_BASE_GET_USER_INFO_ACK";
		case PROTOCOL_BASE_GET_INVEN_INFO_ACK : return "PROTOCOL_BASE_GET_INVEN_INFO_ACK";
		case PROTOCOL_BASE_GET_OPTION_ACK : return "PROTOCOL_BASE_GET_OPTION_ACK";
		case PROTOCOL_CHAR_CREATE_CHARA_ACK : return "PROTOCOL_CHAR_CREATE_CHARA_ACK";
		//case PROTOCOL_BASE_GET_NEW_USER_BASIC_INFO_ACK : return "PROTOCOL_BASE_GET_NEW_USER_BASIC_INFO_ACK";
		case PROTOCOL_BASE_GET_RECORD_INFO_DB_ACK : return "PROTOCOL_BASE_GET_RECORD_INFO_DB_ACK";
		case PROTOCOL_BASE_GET_RECORD_INFO_SESSION_ACK : return "PROTOCOL_BASE_GET_RECORD_INFO_SESSION_ACK";
		case PROTOCOL_BASE_RENDEZVOUS_NAT_STATE_ACK : return "PROTOCOL_BASE_RENDEZVOUS_NAT_STATE_ACK";
		case PROTOCOL_BASE_OPTION_SAVE_ACK : return "PROTOCOL_BASE_OPTION_SAVE_ACK";
		case PROTOCOL_BASE_ENTER_PASS_ACK : return "PROTOCOL_BASE_ENTER_PASS_ACK";
		case PROTOCOL_BASE_GAMEGUARD_REQ : return "PROTOCOL_BASE_GAMEGUARD_REQ";
		case PROTOCOL_BASE_GET_MYINFO_RECORD_ACK : return "PROTOCOL_BASE_GET_MYINFO_RECORD_ACK";
		case PROTOCOL_BASE_GET_MYINFO_BASIC_ACK : return "PROTOCOL_BASE_GET_MYINFO_BASIC_ACK";
		case PROTOCOL_BASE_GET_MYINFO_ALL_ACK : return "PROTOCOL_BASE_GET_MYINFO_ALL_ACK";
		case PROTOCOL_BASE_RANK_UP_ACK : return "PROTOCOL_BASE_RANK_UP_ACK";
		case PROTOCOL_BASE_INITIAL_RANK_UP_ACK : return "PROTOCOL_BASE_INITIAL_RANK_UP_ACK";
		case PROTOCOL_BASE_INV_ITEM_DATA_ACK : return "PROTOCOL_BASE_INV_ITEM_DATA_ACK";
		case PROTOCOL_BASE_SUPPLY_BOX_ANNOUNCE_ACK : return "PROTOCOL_BASE_SUPPLY_BOX_ANNOUNCE_ACK";
		case PROTOCOL_BASE_SUPPLY_BOX_PRESENT_MESSAGE_ACK : return "PROTOCOL_BASE_SUPPLY_BOX_PRESENT_MESSAGE_ACK";
		case PROTOCOL_BASE_SUPPLY_BOX_PRESENT_ACK : return "PROTOCOL_BASE_SUPPLY_BOX_PRESENT_ACK";
		case PROTOCOL_BASE_LOGOUT_ACK : return "PROTOCOL_BASE_LOGOUT_ACK";
		case PROTOCOL_BASE_CHATTING_ACK: return "PROTOCOL_BASE_CHATTING_ACK";
		case PROTOCOL_NEW_SET_CHARA_ACK : return "PROTOCOL_NEW_SET_CHARA_ACK";
		case PROTOCOL_CHAR_DELETE_CHARA_ACK : return "PROTOCOL_CHAR_DELETE_CHARA_ACK";
		case PROTOCOL_CHAR_CHANGE_EQUIP_ACK : return "PROTOCOL_CHAR_CHANGE_EQUIP_ACK";
		case PROTOCOL_CHAR_CHANGE_STATE_ACK : return "PROTOCOL_CHAR_CHANGE_STATE_ACK";
		//case PROTOCOL_SET_CHARA_SKILL_ACK : return "PROTOCOL_SET_CHARA_SKILL_ACK";
		//case PROTOCOL_RESET_CHARA_SKILL_ACK : return "PROTOCOL_RESET_CHARA_SKILL_ACK";
		case PROTOCOL_SKILL_SET_ACK : return "PROTOCOL_SKILL_SET_ACK";
		case PROTOCOL_SKILL_CLASS_SET_ACK : return "PROTOCOL_SKILL_CLASS_SET_ACK";
		//case PROTOCOL_NEW_GAME_SERVER_STATE_ACK : return "PROTOCOL_NEW_GAME_SERVER_STATE_ACK";
		case PROTOCOL_BASE_GET_CHANNELLIST_ACK : return "PROTOCOL_BASE_GET_CHANNELLIST_ACK";
		case PROTOCOL_BASE_SELECT_CHANNEL_ACK : return "PROTOCOL_BASE_SELECT_CHANNEL_ACK";
		case PROTOCOL_BASE_USER_LEAVE_ACK : return "PROTOCOL_BASE_USER_LEAVE_ACK";
		case PROTOCOL_BASE_USER_ENTER_ACK : return "PROTOCOL_BASE_USER_ENTER_ACK";
		case PROTOCOL_CHAR_CHANGE_CHARA_NICK_ACK : return "PROTOCOL_CHAR_CHANGE_CHARA_NICK_ACK";
		//case PROTOCOL_BASE_FIND_USER_POSITION_ACK : return "PROTOCOL_BASE_FIND_USER_POSITION_ACK";
		case PROTOCOL_BASE_GET_USER_BASIC_INFO_ACK : return "PROTOCOL_BASE_GET_USER_BASIC_INFO_ACK";
		case PROTOCOL_BASE_GET_USER_DETAIL_INFO_ACK : return "PROTOCOL_BASE_GET_USER_DETAIL_INFO_ACK";
		case PROTOCOL_COMMUNITY_USER_INVITED_ACK : return "PROTOCOL_COMMUNITY_USER_INVITED_ACK";
		case PROTOCOL_COMMUNITY_USER_INVITED_REQUEST_ACK : return "PROTOCOL_COMMUNITY_USER_INVITED_REQUEST_ACK";
		//case PROTOCOL_BASE_ATTENDANCE_INFO_ACK : return "PROTOCOL_BASE_ATTENDANCE_INFO_ACK";
		case PROTOCOL_BASE_ATTENDANCE_ACK : return "PROTOCOL_BASE_ATTENDANCE_ACK";
		case PROTOCOL_BASE_ATTENDANCE_CLEAR_ITEM_ACK : return "PROTOCOL_BASE_ATTENDANCE_CLEAR_ITEM_ACK";
		case PROTOCOL_BASE_NEW_CHARA_SHIFT_POS_ACK : return "PROTOCOL_BASE_NEW_CHARA_SHIFT_POS_ACK";
		case PROTOCOL_BASE_GUIDE_COMPLETE_ACK : return "PROTOCOL_BASE_GUIDE_COMPLETE_ACK";
		case PROTOCOL_BASE_NEW_REWARD_POPUP_ACK : return "PROTOCOL_BASE_NEW_REWARD_POPUP_ACK";
		case PROTOCOL_BASE_EQUIPMENT_ACK : return "PROTOCOL_BASE_EQUIPMENT_ACK";
		case PROTOCOL_BASE_DAILY_RECORD_ACK : return "PROTOCOL_BASE_DAILY_RECORD_ACK";
		case PROTOCOL_BASE_CHANGE_PCCAFE_STATUS_ACK : return "PROTOCOL_BASE_CHANGE_PCCAFE_STATUS_ACK";
		case PROTOCOL_BASE_GET_USER_SUBTASK_ACK : return "PROTOCOL_BASE_GET_USER_SUBTASK_ACK";
		case PROTOCOL_BASE_MEGAPHONE_ACK : return "PROTOCOL_BASE_MEGAPHONE_ACK";
		case PROTOCOL_BASE_EVENT_ITEM_ACK: return "PROTOCOL_BASE_EVENT_ITEM_ACK";

		// 호칭..
		case PROTOCOL_BASE_USER_TITLE_CHANGE_ACK: return "PROTOCOL_BASE_USER_TITLE_CHANGE_ACK";
		case PROTOCOL_BASE_USER_TITLE_EQUIP_ACK: return "PROTOCOL_BASE_USER_TITLE_EQUIP_ACK";
		case PROTOCOL_BASE_USER_TITLE_RELEASE_ACK: return "PROTOCOL_BASE_USER_TITLE_RELEASE_ACK";
		case PROTOCOL_BASE_USER_TITLE_INFO_ACK: return "PROTOCOL_BASE_USER_TITLE_INFO_ACK";

		// 임무카드..
		case PROTOCOL_BASE_QUEST_GET_ACK:				return "PROTOCOL_BASE_QUEST_GET_ACK";
		case PROTOCOL_BASE_QUEST_GET_ITEM_ACK:			return "PROTOCOL_BASE_QUEST_GET_ITEM_ACK";
		case PROTOCOL_BASE_QUEST_GET_INFO_ACK:			return "PROTOCOL_BASE_QUEST_GET_INFO_ACK";
		case PROTOCOL_BASE_QUEST_BUY_CARD_SET_ACK:		return "PROTOCOL_BASE_QUEST_BUY_CARD_SET_ACK";
		case PROTOCOL_BASE_QUEST_CHANGE_ACK:			return "PROTOCOL_BASE_QUEST_CHANGE_ACK";
		case PROTOCOL_BASE_QUEST_DELETE_CARD_SET_ACK:	return "PROTOCOL_BASE_QUEST_DELETE_CARD_SET_ACK";
		case PROTOCOL_BASE_QUEST_ACTIVE_IDX_CHANGE_ACK:	return "PROTOCOL_BASE_QUEST_ACTIVE_IDX_CHANGE_ACK";
		case PROTOCOL_BASE_MEDAL_GET_INFO_ACK:			return "PROTOCOL_BASE_MEDAL_GET_INFO_ACK";

		case PROTOCOL_BASE_GET_CHARA_INFO_ACK :			return "PROTOCOL_BASE_GET_CHARA_INFO_ACK";

		case PROTOCOL_BASE_MAP_RULELIST_ACK:			return "PROTOCOL_BASE_MAP_RULELIST_ACK";	
		case PROTOCOL_BASE_MAP_MATCHINGLIST_ACK:		return "PROTOCOL_BASE_MAP_MATCHINGLIST_ACK";	

		case PROTOCOL_BASE_NOTICE_ACK:					return "PROTOCOL_BASE_NOTICE_ACK";					//웹툴 공지
		case PROTOCOL_BASE_URL_LIST_ACK:				return "PROTOCOL_BASE_URL_LIST_ACK";				//웹 베너 유알엘

		case PROTOCOL_BASE_BOOSTEVENT_INFO_ACK:			return "PROTOCOL_BASE_BOOSTEVENT_INFO_ACK";	// 랜덤맵 부스트이벤트 경험치, 포인트 전달.

		default :
			I3PRINTLOG(I3LOG_FATAL,  "PROTOCOL_BASE_XXX 프로토콜 이름 등록해주이소~");
			return "PROTOCOL_BASE_UNKNOWN";
		}
	}
}

void ClientTCPSocket::__Register_Base_Function( void)
{
	RegisterParser( PROTOCOL_BASE_CREATE_NICK_ACK,				&ClientTCPSocket::__Parse_Lobby_CreateNickname);
	RegisterParser( PROTOCOL_BASE_CONNECT_ACK,					&ClientTCPSocket::__Parse_Base_Connect);
	RegisterParser( PROTOCOL_LOGIN_ACK,							&ClientTCPSocket::__Parse_Base_Login);
	RegisterParser( PROTOCOL_BASE_LOGIN_WAIT_ACK,				&ClientTCPSocket::__Parse_Base_LoginWait);
	RegisterParser( PROTOCOL_BASE_GET_SYSTEM_INFO_ACK,			&ClientTCPSocket::__Parse_Base_GetSystemInfo);
	RegisterParser( PROTOCOL_BASE_GET_USER_INFO_ACK,			&ClientTCPSocket::__Parse_Base_GetUserInfo);
	RegisterParser( PROTOCOL_BASE_GET_CHARA_INFO_ACK,			&ClientTCPSocket::__Parse_Base_GetCharacterInfo);
	RegisterParser( PROTOCOL_BASE_GET_INVEN_INFO_ACK,			&ClientTCPSocket::__Parse_Base_GetInventoryInfo);
	RegisterParser( PROTOCOL_BASE_GET_OPTION_ACK,				&ClientTCPSocket::__Parse_Base_GetOptionInfo);
	RegisterParser( PROTOCOL_CHAR_CREATE_CHARA_ACK,				&ClientTCPSocket::__Parse_Base_CreateCharacter);
	//RegisterParser( PROTOCOL_BASE_GET_NEW_USER_BASIC_INFO_ACK, &ClientTCPSocket::__Parse_Base_GetBasicUserInfo);
	RegisterParser( PROTOCOL_BASE_GET_RECORD_INFO_DB_ACK,		&ClientTCPSocket::__Parse_Base_GetBasicUserInfo);
	RegisterParser( PROTOCOL_BASE_GET_RECORD_INFO_SESSION_ACK,	&ClientTCPSocket::__Parse_Base_GetSessionRecord);
	RegisterParser( PROTOCOL_BASE_RENDEZVOUS_NAT_STATE_ACK,		&ClientTCPSocket::__Parse_Base_RendezvousNAT_State);
	RegisterParser( PROTOCOL_BASE_OPTION_SAVE_ACK,				&ClientTCPSocket::__Parse_Base_SaveOptionInfo);
	RegisterParser( PROTOCOL_BASE_ENTER_PASS_ACK,				&ClientTCPSocket::__Parse_Base_EnterPassword);
	RegisterParser( PROTOCOL_BASE_GAMEGUARD_REQ,				&ClientTCPSocket::__Parse_Base_StartGameGuard);
	RegisterParser( PROTOCOL_BASE_GET_MYINFO_RECORD_ACK,		&ClientTCPSocket::__Parse_Base_GetMyInfo_Record);
	RegisterParser( PROTOCOL_BASE_GET_MYINFO_BASIC_ACK,			&ClientTCPSocket::__Parse_Base_GetMyInfo_Basic);
	RegisterParser( PROTOCOL_BASE_GET_MYINFO_ALL_ACK,			&ClientTCPSocket::__Parse_Base_GetMyInfo_All);
	RegisterParser( PROTOCOL_BASE_RANK_UP_ACK,					&ClientTCPSocket::__Parse_Base_RankUp);
	RegisterParser( PROTOCOL_BASE_INITIAL_RANK_UP_ACK,			&ClientTCPSocket::__Parse_Base_InitialRankUp);
	RegisterParser( PROTOCOL_BASE_INV_ITEM_DATA_ACK,			&ClientTCPSocket::__Parse_Base_GetMyInfo_InventoryItem);
	RegisterParser( PROTOCOL_BASE_SUPPLY_BOX_ANNOUNCE_ACK,		&ClientTCPSocket::__Parse_Base_Announce_SupplyBox);
	RegisterParser( PROTOCOL_BASE_SUPPLY_BOX_PRESENT_MESSAGE_ACK, &ClientTCPSocket::__Parse_Base_Message_SupplyBox);
	RegisterParser( PROTOCOL_BASE_SUPPLY_BOX_PRESENT_ACK,		&ClientTCPSocket::__Parse_Base_Present_SupplyBox);
	RegisterParser( PROTOCOL_BASE_LOGOUT_ACK,					&ClientTCPSocket::__Parse_Base_Logout);
	RegisterParser( PROTOCOL_BASE_CHATTING_ACK,					&ClientTCPSocket::__Parse_Base_ChatLimit);
	
	
	RegisterParser( PROTOCOL_CHAR_DELETE_CHARA_ACK,				&ClientTCPSocket::__Parse_Base_DeleteCharacter);
	RegisterParser( PROTOCOL_CHAR_CHANGE_EQUIP_ACK,				&ClientTCPSocket::__Parse_Base_ChangeCharacterEquipment);
	RegisterParser( PROTOCOL_CHAR_CHANGE_STATE_ACK,				&ClientTCPSocket::__Parse_Base_ChangeCharaState);
	RegisterParser( PROTOCOL_SKILL_SET_ACK,						&ClientTCPSocket::__Parse_Base_LearnSkillSlot);
	RegisterParser( PROTOCOL_SKILL_CLASS_SET_ACK,				&ClientTCPSocket::__Parse_Base_LearnSkillClass);
	//RegisterParser( PROTOCOL_NEW_GAME_SERVER_STATE_ACK,			&ClientTCPSocket::__Parse_Base_GameServerState);
	RegisterParser( PROTOCOL_BASE_GET_CHANNELLIST_ACK,			&ClientTCPSocket::__Parse_Base_GetChannelList);
	RegisterParser( PROTOCOL_BASE_SELECT_CHANNEL_ACK,			&ClientTCPSocket::__Parse_Base_SelectChannel);
	RegisterParser( PROTOCOL_BASE_USER_LEAVE_ACK,				&ClientTCPSocket::__Parse_Base_LeaveServer);
	RegisterParser( PROTOCOL_BASE_USER_ENTER_ACK,				&ClientTCPSocket::__Parse_Base_EnterServer);
	RegisterParser( PROTOCOL_CHAR_CHANGE_CHARA_NICK_ACK,		&ClientTCPSocket::__Parse_Base_ChangeCharacterName);
	//RegisterParser( PROTOCOL_BASE_FIND_USER_POSITION_ACK,		&ClientTCPSocket::__Parse_Base_FindUserPosition);
	RegisterParser( PROTOCOL_BASE_GET_USER_BASIC_INFO_ACK,		&ClientTCPSocket::__Parse_Base_GetUserBasicInfo);
	RegisterParser( PROTOCOL_BASE_GET_USER_DETAIL_INFO_ACK,		&ClientTCPSocket::__Parse_Base_GetUserDetailInfo);
	RegisterParser( PROTOCOL_COMMUNITY_USER_INVITED_ACK,		&ClientTCPSocket::__Parse_Base_InviteNormalUser);
	RegisterParser( PROTOCOL_COMMUNITY_USER_INVITED_REQUEST_ACK, &ClientTCPSocket::__Parse_Base_RequestInviteNormalUser);
	//RegisterParser( PROTOCOL_BASE_ATTENDANCE_INFO_ACK,		&ClientTCPSocket::__Parse_Base_Attendance_Info);
	RegisterParser( PROTOCOL_BASE_ATTENDANCE_ACK,				&ClientTCPSocket::__Parse_Base_Attendance_Check);
	RegisterParser( PROTOCOL_BASE_ATTENDANCE_CLEAR_ITEM_ACK,	&ClientTCPSocket::__Parse_Base_Attendance_Clear_Item);
	RegisterParser( PROTOCOL_BASE_GUIDE_COMPLETE_ACK,			&ClientTCPSocket::__Parse_Base_UserGuideComplete);
	RegisterParser( PROTOCOL_BASE_NEW_REWARD_POPUP_ACK,			&ClientTCPSocket::__Parse_Base_Reward_Popup);	
	RegisterParser( PROTOCOL_BASE_EQUIPMENT_ACK,				&ClientTCPSocket::__Parse_Base_Equipment);	
	RegisterParser( PROTOCOL_BASE_DAILY_RECORD_ACK,				&ClientTCPSocket::__Parse_Base_DailyRecord);
	RegisterParser( PROTOCOL_BASE_CHANGE_PCCAFE_STATUS_ACK,		&ClientTCPSocket::__Parse_Base_ChangePCCafeStatus);
	RegisterParser( PROTOCOL_BASE_GET_USER_SUBTASK_ACK,			&ClientTCPSocket::__Parse_Base_GetUserSubtaskInfo);
	RegisterParser( PROTOCOL_BASE_MEGAPHONE_ACK,				&ClientTCPSocket::__Parse_Base_ReceiveMegaPhone);
	RegisterParser(	PROTOCOL_BASE_NOTICE_ACK,					&ClientTCPSocket::__Parse_Base_Notice);
	RegisterParser( PROTOCOL_BASE_EVENT_ITEM_ACK,				&ClientTCPSocket::__Parse_Base_EventItem);	// 호칭 패킷..
	RegisterParser( PROTOCOL_BASE_URL_LIST_ACK,					&ClientTCPSocket::__Parse_Base_URL_List);	
	RegisterParser( PROTOCOL_BASE_USER_TITLE_EQUIP_ACK,			&ClientTCPSocket::__Parse_PROTOCOL_BASE_USER_TITLE_EQUIP_ACK);
	RegisterParser( PROTOCOL_BASE_USER_TITLE_CHANGE_ACK,		&ClientTCPSocket::__Parse_PROTOCOL_BASE_USER_TITLE_CHANGE_ACK);
	RegisterParser( PROTOCOL_BASE_USER_TITLE_RELEASE_ACK,		&ClientTCPSocket::__Parse_PROTOCOL_BASE_USER_TITLE_RELEASE_ACK);
	RegisterParser( PROTOCOL_BASE_USER_TITLE_INFO_ACK,			&ClientTCPSocket::__Parse_PROTOCOL_BASE_USER_TITLE_INFO_ACK);

	// 임무카드 패킷..
	RegisterParser(PROTOCOL_BASE_QUEST_GET_ACK,					&ClientTCPSocket::__Parse_PROTOCOL_BASE_QUEST_GET_ACK);
	RegisterParser(PROTOCOL_BASE_QUEST_GET_ITEM_ACK, &ClientTCPSocket::__Parse_PROTOCOL_BASE_QUEST_GET_ITEM_ACK);
	RegisterParser(PROTOCOL_BASE_QUEST_GET_INFO_ACK, &ClientTCPSocket::__Parse_PROTOCOL_BASE_QUEST_GET_INFO_ACK);
	RegisterParser(PROTOCOL_BASE_QUEST_BUY_CARD_SET_ACK, &ClientTCPSocket::__Parse_PROTOCOL_BASE_QUEST_BUY_CARD_SET_ACK);
	RegisterParser(PROTOCOL_BASE_QUEST_CHANGE_ACK, &ClientTCPSocket::__Parse_PROTOCOL_BASE_QUEST_CHANGE_ACK);
	RegisterParser(PROTOCOL_BASE_QUEST_DELETE_CARD_SET_ACK, &ClientTCPSocket::__Parse_PROTOCOL_BASE_QUEST_DELETE_CARD_SET_ACK);
	RegisterParser(PROTOCOL_BASE_QUEST_ACTIVE_IDX_CHANGE_ACK, &ClientTCPSocket::__Parse_PROTOCOL_BASE_QUEST_ACTIVE_IDX_CHANGE_ACK);
	RegisterParser(PROTOCOL_BASE_MEDAL_GET_INFO_ACK, &ClientTCPSocket::__Parse_PROTOCOL_BASE_MEDAL_GET_INFO_ACK);

	RegisterParser(PROTOCOL_BASE_MAP_RULELIST_ACK, &ClientTCPSocket::__Parse_Base_Get_MAP_RULELIST);
	RegisterParser(PROTOCOL_BASE_MAP_MATCHINGLIST_ACK, &ClientTCPSocket::__Parse_Base_Get_MAP_MATCHINGLIST);

	RegisterParser(PROTOCOL_BASE_BOOSTEVENT_INFO_ACK, &ClientTCPSocket::__Parse_Base_Get_BOOSTEVENT_INFO); // 랜덤맵 부스트이벤트 경험치, 포인트 전달.

	// 그 밖에 살펴볼 패킷들.
	// PROTOCOL_BATTLE_NOTIFY_BE_KICKED_BY_KICKVOTE_ACK


}

void ClientTCPSocket::_PacketParsingBase(i3NetworkPacket  * pPacket)
{
	i3::unordered_map< UINT32, netFunc >::iterator it;
	it = m_NetFuncMap_Base.find( pPacket->GetProtocolID());

	if ( it != m_NetFuncMap_Base.end() )
	{
		I3PRINTLOG(I3LOG_QA, "* %s -->", getProtocolName( pPacket->GetProtocolID()));
		netFunc & mf = it->second;
		(this->*mf)(pPacket);
		I3PRINTLOG(I3LOG_QA, "* %s <--", getProtocolName( pPacket->GetProtocolID()));
	}
	else
	{
		I3PRINTLOG(I3LOG_NOTICE, "*UnknownProtocol = %s -->", getProtocolName(pPacket->GetProtocolID()));
		__Parse_UnknownProtocol(pPacket);
	}
}



void ClientTCPSocket::__Parse_Base_Connect( i3NetworkPacket * pPacket)
{
	PACKET_BASE_CONNECT_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	SC_VERSION	ClientVersion;
	SC_VERSION	ServerVersion = Recv.m_stServerVersion;
	I3PRINTLOG(I3LOG_DEFALUT,  "VSN C:%d.%d S:%d.%d ", ClientVersion._ui8VerGame, ClientVersion._ui16VerSC, 
	ServerVersion._ui8VerGame, ServerVersion._ui16VerSC );

	INT32 i32ReadSize;
	S2MOReadPublicKey( (char*)&Recv.m_pRSAKey, i32ReadSize );
		
	m_ConnectionInfo.m_SessionIndex = Recv.m_i32SessionIdx;
	m_i16PacketRandSeed				= Recv.m_i16PacketRandSeed;

	switch(m_TargetServerType)
	{
	case SERVER_TYPE_GATEWAY:
		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CONNECT_SERVER_GATEWAY );
		I3TRACE0("SERVER_TYPE_GATEWAY\n");
		break;
	case SERVER_TYPE_GAME:
#if defined( AUTO_LOBBY)
		GameEventSender::i()->SetEventOnce(EVENT_USER_LEAVE);
#else
		/*if (g_pEnvSet->m_nUIVersion > 0)
			GameEventSender::i()->SetEventOnce(EVENT_USER_LEAVE);
		else*/
		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CONNECT_SERVER_GAME );
#endif

		I3TRACE0("SERVER_TYPE_GAME\n");
		break;
	default:	I3ASSERT_0;	break;
	}

	m_ConnectionSuccess = true;

	/*
			
	// 서버 보너스
	if(InfoHeader.m_ui16ServerBonusCount > 0)
	{
		UINT16 ui16ServerBonusCount = InfoHeader.m_ui16ServerBonusCount;

		PACKET_D_GET_MYINFO_COMPLETE_SERVER_BONUS * pServerBonus = nullptr;
		INT32 nServerBonusSize = sizeof(PACKET_D_GET_MYINFO_COMPLETE_SERVER_BONUS) * ui16ServerBonusCount;
		pServerBonus = (PACKET_D_GET_MYINFO_COMPLETE_SERVER_BONUS *) i3MemAlloc(nServerBonusSize);
		pPacket->ReadData(pServerBonus, nServerBonusSize);

#if defined( I3_DEBUG)
		char Notice[256] = {0};
		for(UINT16 i = 0 ; i < ui16ServerBonusCount ; i++)
		{
			sprintf_s( Notice,"보너스 카운트 = %d 서버보너스 정보를 받았습니다. Type : %d, Exp : %d, Point : %d",
				ui16ServerBonusCount, pServerBonus[i].m_ui16Type, pServerBonus[i].m_ui32Exp, pServerBonus[i].m_ui32Point );
			I3PRINTLOG(I3LOG_QA,Notice);
		}
#endif

		I3MEM_SAFE_FREE(pServerBonus);
	}

	// 채널 보너스
	if(InfoHeader.m_ui8ChannelBonusCount > 0)
	{
		UINT8 ui8ChannelBonusCount = InfoHeader.m_ui8ChannelBonusCount;

		PACKET_D_GET_MYINFO_COMPLETE_CHANNEL_BONUS * pChannelBonus = nullptr;
		INT32 nChannelBonusSize = sizeof(PACKET_D_GET_MYINFO_COMPLETE_CHANNEL_BONUS) * ui8ChannelBonusCount;
		pChannelBonus = (PACKET_D_GET_MYINFO_COMPLETE_CHANNEL_BONUS *) i3MemAlloc(nChannelBonusSize);
		pPacket->ReadData(pChannelBonus, nChannelBonusSize);

#if defined( I3_DEBUG)
		char Notice[256] = {0};
		TYPE_CHANNEL_BONUS * stChannelBonus = nullptr ;
		for(UINT8 i = 0 ; i < ui8ChannelBonusCount ; i++)
		{
			stChannelBonus = &pChannelBonus[i].m_stChannelBonus;

			sprintf_s( Notice,"채널별 정보를 받았습니다. Type : %d, Exp : %d, Point : %d",
				stChannelBonus->_ui16Type, stChannelBonus->_ui32Exp, stChannelBonus->_ui32Point);
			I3PRINTLOG(I3LOG_QA,Notice);
		}
#endif

		I3MEM_SAFE_FREE(pChannelBonus);
	}
	*/	
}

void ClientTCPSocket::__Parse_Base_GetBasicUserInfo( i3NetworkPacket * pPacket)
{
	pPacket->ReadData( CCommunityContext::i()->SetCommnuitRecord(), sizeof(USER_INFO_RECORD));
	CCommunityContext::i()->SetCommunityUserInfo(true);
}

void ClientTCPSocket::__Parse_Base_GetSessionRecord( i3NetworkPacket * pPacket)
{
	pPacket->ReadData( CCommunityContext::i()->SetCommnuitRecord(), sizeof(USER_INFO_RECORD));
	CCommunityContext::i()->SetCommunityUserInfo(true);
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_COMMUNITY_RECORD_SESSION_N);
}

void ClientTCPSocket::__Parse_Base_RendezvousNAT_State( i3NetworkPacket * pPacket)
{
	if (GameLoginContext::i()->IsWebLogin())
	{
		GameEventSender::i()->SetEvent(EVENT_LOG_IN_WEBKEY);
	}
	else
	{
		GameEventSender::i()->SetEvent(EVENT_LOG_IN);
	}
}

void ClientTCPSocket::__Parse_Base_Login( i3NetworkPacket * pPacket)
{
	RecvData_EVENT_LOG_IN		data;
	PACKET_LOGIN_ACK Packet;
	S2MORecvPacket( &Packet, pPacket->GetPacketBuffer() );

	if( EV_SUCCESSED( Packet.m_TResult ) )
	{
		NetworkContext::i()->UID = Packet.m_TUID;
		GameLoginContext::i()->setUserUID(NetworkContext::i()->UID);
		I3PRINTLOG(I3LOG_VIEWER, "UID : %ld", NetworkContext::i()->UID);
		
		char strExtUID[EXT_UID_SIZE] = { 0, };
		Packet.m_strExtUID.GetValue(strExtUID, EXT_UID_SIZE);

		GameLoginContext::i()->SetExtUID(strExtUID);
		if (Packet.m_strWebLoginKey.GetCount() > 0)
		{
			char strWebKey[WEB_LOGINKEY_SIZE] = { 0, };
			Packet.m_strWebLoginKey.GetValue(strWebKey, WEB_LOGINKEY_SIZE);
			GameLoginContext::i()->setWebLoginKey(strWebKey);
		}

		// 커스템 메세지가 있을때 저장
		if( '\0' != Packet.m_strCustom[0] )
		{
			char strCustomMsg[ NORTH_CUSTOM_MSG_SIZE ];
			Packet.m_strCustom.GetValue( strCustomMsg, NORTH_CUSTOM_MSG_SIZE );

			i3::wstring wstrCustomMsg;		i3::mb_to_utf16(strCustomMsg, wstrCustomMsg);

			data.wstrCustomMsg = wstrCustomMsg;
		}
	}
	// 북미 로그인 인증 시스템 추가로 인한 커스텀메세지 출력부분 [2011.06.16 양승천]
	else if( Packet.m_TResult == EVENT_ERROR_EVENT_LOG_IN_CUSTOM_MESSAGE || Packet.m_TResult == EVENT_ERROR_SUCCESS)
	{	
		if( '\0' != Packet.m_strCustom[0] )
		{
			char strCustomMsg[ NORTH_CUSTOM_MSG_SIZE ];
			Packet.m_strCustom.GetValue( strCustomMsg, NORTH_CUSTOM_MSG_SIZE );

			i3::wstring wstrCustomMsg;		i3::mb_to_utf16(strCustomMsg, wstrCustomMsg);

			data.wstrCustomMsg = wstrCustomMsg;
		}
	}

	if (SteamContext::i()->IsSteamUser())
	{
		char billingID[BILLING_ID_SIZE] = { 0, };
		Packet.m_strUID.GetValue(billingID, BILLING_ID_SIZE);
		GameLoginContext::i()->setBillingID(billingID);
	}

	if(Packet.m_TResult == EVENT_ERROR_EVENT_LOG_IN_BLOCK_ACCOUNT)
	{
		// m_ui32BlockDate에 값이 있으면 GM에게 블럭당한 계정임
		GMAuthorityContext::i()->SetBlockDate(Packet.m_ui32BlockDate);

		i3::rc_wstring wstrComment = &Packet.m_strUserNoticeComment;
		GMAuthorityContext::i()->SetBlockComment(wstrComment);

		GMAuthorityContext::i()->SetPenaltyAmount(Packet.m_i32PenaltyAmount);

		GameEventReceiver::i()->PushReceivedGameEventUserData( EVENT_M_BLOCK_ACCOUNT, Packet.m_TResult );
	}
	else if (Packet.m_TResult == EVENT_ERROR_EVENT_LOG_IN_AAS)
	{
		GameEventReceiver::i()->PushReceivedGameEventUserData( EVENT_SHUTDOWN_TIME_LOGIN, Packet.m_TResult );
	}
	else if (Packet.m_TResult == EVENT_ERROR_EVENT_LOG_IN_STEAM_HTTP)
	{
		I3PRINTLOG(I3LOG_NOTICE, "[STEAM] : Login Error From Server");
	}
	else
	{
		GameEventReceiver::i()->PushReceivedGameEventUserData( EVENT_LOG_IN, data, Packet.m_TResult );
	}
}


void ClientTCPSocket::__Parse_Base_LoginWait( i3NetworkPacket * pPacket)
{
	RecvData_EVENT_LOG_IN_WAIT data;
	pPacket->ReadData( &data.WaitCount,		sizeof(INT32) );			//Login AuthKey
	pPacket->ReadData( &data.WaitLoginTime,	sizeof(UINT32) );			//Login AuthKey
	GameEventReceiver::i()->PushReceivedGameEventUserData(EVENT_LOG_IN_WAIT, data);
}


void ClientTCPSocket::__Parse_Base_GetOptionInfo( i3NetworkPacket * pPacket)
{
	PACKET_BASE_GET_OPTION_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	if( EV_SUCCESSED( Recv.m_TResult ) )
	{
		EnvInfoContext::i()->SetEnvSetDefault( Recv.m_ui8DefaultKey );

		if( ENV_SETTING_DEFAULT != EnvInfoContext::i()->GetEnvSetDefault() )
		{
			PACKET_D_BASE_GET_OPTION OptionAck;
			pPacket->ReadData( &OptionAck, sizeof(PACKET_D_BASE_GET_OPTION) );

			EnvInfoContext::i()->SetNetEnvSet( &Recv.m_NetEnvSet);
			EnvInfoContext::i()->SetKeyMapArray( &Recv.m_KeyMapArray);
			
			i3::rc_wstring wstrMacro;

			wchar_t wstrMacroOrg[ MACRO_MAX_LENGHT ];
			for( INT32 i = 0 ; i < MACRO_MAX_COUNT ; i++ )
			{				
				Recv.m_strMacro[i].GetValue( wstrMacroOrg,  MACRO_MAX_LENGHT );
				wstrMacro.clear();
				RemoveColorStringIfNeeded( wstrMacroOrg, wstrMacro);
				//				GameUI::RemoveColorKeyword( Recv.m_strMacro[i], Recv.m_strMacro[i], MACRO_MAX_LENGHT);

				if( wstrMacro.empty() )
				{
					wstrMacro = &Recv.m_strMacro[i];
				}
				EnvInfoContext::i()->SetMacro( i, wstrMacro );
			}

			g_pEnvSet->Net_Load();
			UIPopupOption *pOption = UIPopupOption::instance();
			if(pOption != nullptr)
				pOption->load_apply_option(false); 
		}

		if( g_pFramework->GetUIMainframe() != nullptr)
		{
			g_pFramework->GetUIMainframe()->setNetLoad( true);
		}

		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_GET_MYOPTION_N);


	}
	else
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_LOADSETTING_ERROR"));
	}

}

void ClientTCPSocket::__Parse_Base_SaveOptionInfo(i3NetworkPacket * pPacket)
{
	PACKET_BASE_OPTION_SAVE_ACK OptSaveAck;

	pPacket->ReadData( &OptSaveAck, sizeof(PACKET_BASE_OPTION_SAVE_ACK) );

	if( EV_FAILED(OptSaveAck.m_i32Rv) )
	{
		I3PRINTLOG(I3LOG_NOTICE," Save optoin FAILED ");
	}
}

void ClientTCPSocket::__Parse_Base_SaveOptionMacroInfo(i3NetworkPacket * pPacket)
{
	// Ack 패킷은 Option Ack와 동일
	PACKET_BASE_OPTION_SAVE_ACK OptSaveAck;

	pPacket->ReadData( &OptSaveAck, sizeof(PACKET_BASE_OPTION_SAVE_ACK) );

	if( EV_FAILED(OptSaveAck.m_i32Rv) )
	{
		I3PRINTLOG(I3LOG_NOTICE," Save option macro FAILED ");
	}
}


void ClientTCPSocket::__Parse_Base_EnterPassword( i3NetworkPacket * pPacket)
{
	PACKET_BASE_ENTER_PASS_ACK Recv;
	S2MORecvPacket( &Recv, (char*)pPacket->GetPacketBuffer() );

	if( EV_FAILED( Recv.m_TResult ) )
	{
		ForceDestroy();
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_ENTER_PW, Recv.m_TResult);
}

void ClientTCPSocket::__Parse_Base_StartGameGuard( i3NetworkPacket * pPacket)
{
#if defined (_NPROTECT_)
	I3PRINTLOG(I3LOG_DEFALUT,  "[nProtect] : 1" );  //서버에서 CS 인증 요청

	// 서버로부터 인증 패킷을 수신
	UINT32 size = 0;
	pPacket->ReadData( &size, sizeof(UINT32) );

	PBYTE pRecvPacket = new BYTE[size];
	MEMDUMP_NEW( pRecvPacket, sizeof(BYTE) * size);
	pPacket->ReadData(pRecvPacket, size);

	nProtect::SetServerAuth3( pRecvPacket, size );

	I3_SAFE_DELETE( pRecvPacket);
#elif defined (_XTRAP_)
	// XTrap 입니다.
	// 받아서 처리하시면 됩니다.
	char pXTrapInBuffer[ XTRAP_SETINFO_PACKETBUFF_SIZE ];
	char pXTrapOutBuffer[ XTRAP_SETINFO_PACKETBUFF_SIZE ];

	pPacket->ReadData( pXTrapInBuffer, XTRAP_SETINFO_PACKETBUFF_SIZE );

	#ifdef _XTRAP_DUMP
	HexaDump4XTrap_V1( "XtrapCheck.txt", pXTrapInBuffer, XTRAP_SETINFO_PACKETBUFF_SIZE, "[*1 In DLL]");
	#endif

	// Winmain에서 CSStep2Proc로 등록된 콜백 함수에게 값을 전달후 다시 값을 받아온다.
	INT32 len = XTRAP_SETINFO_PACKETBUFF_SIZE;
	
	/* 아래 함수에서 해킹 감지 등의 에러가 발생하면 XTRAP이 클라이언트를 바로 종료한 후 에러창을 띄우고 XTRAP Web Manager로 
	   에러 로그를 보냅니다. 따라서 클라이언트에서는 반환값 검사에 따른 예외 처리를 하지 않습니다. 할 필요도 없습니다.
	   * 반환값 형태를 지니고 있으나 실질적으로 무용지물임(함정 코드) : XTRAP 측에 건의함.
	     - EX. 2011년 11월에 아래의 PROTOCOL_BASE_GAMEGUARD_ACK 패킷을 서버에서 받는데 실패하여 접속종료 건이 상당히 많은 관계로 원인분석을 하였음.
	           아래의 CallInteractProc3 함수 에러 발생시 반환값을 서버에 패킷을 보내 로그를 남기도록 기능을 추가하였으나 서버에 아무런 로그가 남지 않음.
	           에러 발생시 XTRAP에서 바로 클라이언트 종료를 하기 때문에 응답 패킷을 못 보내서 서버에 로그가 남지 않음. 
			   XTRAP 회사에 연락해서 XTRAP Web Manager 계정을 할당받고 접속해서 확인해보니 에러로그를 확인할 수 있었음. 
	
	  - XTRAP Web Manager 참조 :  svn://pbsvn/PointBlank/trunk/Source_New/Xtrap/Doc 

		2011.11.28. ugeun.ji
	*/
	g_pInterface->CallInteractProc3((void*)pXTrapInBuffer, (void*)pXTrapOutBuffer, (void*)len);

	#ifdef _XTRAP_DUMP
	HexaDump4XTrap_V1( "XtrapCheck.txt", pXTrapOutBuffer, XTRAP_SETINFO_PACKETBUFF_SIZE, "[*4 Out DLL]");
	#endif

	// 바로 응답을 보낸다.
	{
		i3NetworkPacket SendPacket;
		SC_VERSION	ClientVersion;

		SendPacket.SetProtocol( PROTOCOL_BASE_GAMEGUARD_ACK );


		//LS CHECK
		HACK_CHECK_REALTIME hcRealTime;
		memset(&hcRealTime, 1, sizeof(HACK_CHECK_REALTIME));
#ifdef USE_LEUCO_SHELL
		LeucoShellCore* pLSCore = LeucoShellCore::GetInstance();
		if (pLSCore)
		{
			memset(&hcRealTime, 0, sizeof(HACK_CHECK_REALTIME));

			// DIRECT X
			hcRealTime.m_ui8Type[0] = (UINT16)pLSCore->GetDXG()->GetVFResult();
			hcRealTime.m_ui8Type[1] = (UINT16)pLSCore->GetDXG()->GetVDResult();

			hcRealTime.m_ui64DXFunction[0] = pLSCore->GetDXG()->GetDectectVF(0);
			hcRealTime.m_ui64DXFunction[1] = pLSCore->GetDXG()->GetDectectVF(1);

			hcRealTime.m_ui64DXDevice[0] = pLSCore->GetDXG()->GetDectectVD(0);
			hcRealTime.m_ui64DXDevice[1] = pLSCore->GetDXG()->GetDectectVD(1);

			// FUNC
			hcRealTime.m_ui8Type[2] = (UINT16)pLSCore->GetFCG()->GetResult();
			hcRealTime.m_ui8Type[3] = (UINT16)pLSCore->GetFCG()->GetDetectIndex();

			// VALUE
			hcRealTime.m_ui8Type[4] = (UINT16)LS::GLOBAL::FUNCTION::GetFakeValueIndex();

			//추가된 로그인 탐지 항목들
			//hcRealTime.m_ui8Type[5];
			//hcRealTime.m_ui8Type[6];
			//hcRealTime.m_ui8Type[7];
		}
		else
		{
			memset(&hcRealTime, 0, sizeof(HACK_CHECK_REALTIME));
			hcRealTime.m_ui8Type[0] = (UINT16)MFV_CORE_NULL;
			hcRealTime.m_ui8Type[1] = (UINT16)MFV_CORE_NULL;
			hcRealTime.m_ui8Type[2] = (UINT16)MFV_CORE_NULL;
			hcRealTime.m_ui8Type[3] = (UINT16)MFV_CORE_NULL;
			hcRealTime.m_ui8Type[4] = (UINT16)MFV_CORE_NULL;
			hcRealTime.m_ui8Type[5] = (UINT16)MFV_CORE_NULL;
			hcRealTime.m_ui8Type[6] = (UINT16)MFV_CORE_NULL;
			hcRealTime.m_ui8Type[7] = (UINT16)MFV_CORE_NULL;
		}
#else
		memset(&hcRealTime, 0, sizeof(HACK_CHECK_REALTIME));
#endif
		SendPacket.WriteData(&hcRealTime, sizeof(HACK_CHECK_REALTIME));
		//LS END


		SendPacket.WriteData(&ClientVersion, sizeof(SC_VERSION));
		SendPacket.WriteData( pXTrapOutBuffer,	XTRAP_SETINFO_PACKETBUFF_SIZE );
		SendMessage(&SendPacket);
	}
#elif defined (_XIGNCODE_)

	//pPacket->ReadData( &ui8Len, sizeof(UINT8));

	// 확인용 로그 추후제거.
	I3PRINTLOG(I3LOG_NOTICE, "#### Rcv XCXC \n");

	char  pXignCodeBuffer[XIGNCODE_PACKET_SIZE];

	pPacket->ReadData(pXignCodeBuffer, sizeof(char)* XIGNCODE_PACKET_SIZE);
	//	서버로부터 응답 요청을 받는다.
	if (!ZCWAVE_ProbeEx(pXignCodeBuffer, XIGNCODE_PACKET_SIZE, OnProbeComplete, NULL))
	{
		I3TRACE("Taht's NONO\n");
		I3PRINTLOG(I3LOG_NOTICE, "#### Rcv XCXC \n");
	}
	else I3PRINTLOG(I3LOG_NOTICE, "#### Rcv XCXC \n");

#else
	i3NetworkPacket SendPacket;
	SC_VERSION	ClientVersion;

	SendPacket.SetProtocol( PROTOCOL_BASE_GAMEGUARD_ACK );

	//LS CHECK
	HACK_CHECK_REALTIME hcRealTime;
	memset(&hcRealTime, 1, sizeof(HACK_CHECK_REALTIME));
#ifdef USE_LEUCO_SHELL
	LeucoShellCore* pLSCore = LeucoShellCore::GetInstance();
	if (pLSCore)
	{
		memset(&hcRealTime, 0, sizeof(HACK_CHECK_REALTIME));

		// DIRECT X
		hcRealTime.m_ui8Type[0] = (UINT16)pLSCore->GetDXG()->GetVFResult();
		hcRealTime.m_ui8Type[1] = (UINT16)pLSCore->GetDXG()->GetVDResult();

		hcRealTime.m_ui64DXFunction[0] = pLSCore->GetDXG()->GetDectectVF(0);
		hcRealTime.m_ui64DXFunction[1] = pLSCore->GetDXG()->GetDectectVF(1);

		hcRealTime.m_ui64DXDevice[0] = pLSCore->GetDXG()->GetDectectVD(0);
		hcRealTime.m_ui64DXDevice[1] = pLSCore->GetDXG()->GetDectectVD(1);

		// FUNC
		hcRealTime.m_ui8Type[2] = (UINT16)pLSCore->GetFCG()->GetResult();
		hcRealTime.m_ui8Type[3] = (UINT16)pLSCore->GetFCG()->GetDetectIndex();

		// VALUE
		hcRealTime.m_ui8Type[4] = (UINT16)LS::GLOBAL::FUNCTION::GetFakeValueIndex();

		//추가된 로그인 탐지 항목들
		//hcRealTime.m_ui8Type[5];
		//hcRealTime.m_ui8Type[6];
		//hcRealTime.m_ui8Type[7];
	}
	else
	{
		memset(&hcRealTime, 0, sizeof(HACK_CHECK_REALTIME));
		hcRealTime.m_ui8Type[0] = (UINT16)MFV_CORE_NULL;
		hcRealTime.m_ui8Type[1] = (UINT16)MFV_CORE_NULL;
		hcRealTime.m_ui8Type[2] = (UINT16)MFV_CORE_NULL;
		hcRealTime.m_ui8Type[3] = (UINT16)MFV_CORE_NULL;
		hcRealTime.m_ui8Type[4] = (UINT16)MFV_CORE_NULL;
		hcRealTime.m_ui8Type[5] = (UINT16)MFV_CORE_NULL;
		hcRealTime.m_ui8Type[6] = (UINT16)MFV_CORE_NULL;
		hcRealTime.m_ui8Type[7] = (UINT16)MFV_CORE_NULL;
	}
#else
	memset(&hcRealTime, 0, sizeof(HACK_CHECK_REALTIME));
#endif
	SendPacket.WriteData(&hcRealTime, sizeof(HACK_CHECK_REALTIME));
	//LS END


	SendPacket.WriteData(&ClientVersion, sizeof(SC_VERSION));
	SendMessage(&SendPacket);
#endif
}

void ClientTCPSocket::__Parse_Base_GetMyInfo_Record( i3NetworkPacket * pPacket)
{
	USER_INFO_RECORD* myRecord = UserInfoContext::i()->SetMyUserInfoRecord();
	pPacket->ReadData(myRecord, sizeof(USER_INFO_RECORD));
}

void ClientTCPSocket::__Parse_Base_GetMyInfo_Basic( i3NetworkPacket * pPacket)
{
	USER_INFO_BASIC MyUserInfoBasic;
	pPacket->ReadData(&MyUserInfoBasic, sizeof(USER_INFO_BASIC));
	UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);
}

void ClientTCPSocket::__Parse_Base_GetMyInfo_All( i3NetworkPacket * pPacket)
{
	USER_INFO_BASIC MyUserInfoBasic;
	USER_INFO_RECORD* myRecord = UserInfoContext::i()->SetMyUserInfoRecord();

	pPacket->ReadData(&MyUserInfoBasic, sizeof(USER_INFO_BASIC));			
	pPacket->ReadData(myRecord, sizeof(USER_INFO_RECORD));

	UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);
}

void ClientTCPSocket::__Parse_Base_RankUp( i3NetworkPacket * pPacket)
{
	UINT32 rank;
	UINT32 keep_rank;
	UINT32 point;

	pPacket->ReadData(&rank, sizeof(UINT32));
	pPacket->ReadData(&keep_rank, sizeof(UINT32));
	pPacket->ReadData(&point, sizeof(UINT32));

	UserInfoContext::i()->SetRankUp(rank, point);
	USER_INFO_BASIC MyUserInfoBasic;
	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
	MyUserInfoBasic.m_ui32Rank = rank;
	MyUserInfoBasic.m_ui32Point += point;
	
	if(0 < point && rank > keep_rank)
	{
		MyUserInfoBasic.m_ui32KeepRank = rank;
		UserInfoContext::i()->SetGeneralRankUpFlag(true);
	}
	UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_RANK_UP);
}

void ClientTCPSocket::__Parse_Base_InitialRankUp( i3NetworkPacket * pPacket)
{
	UserInfoContext::i()->SetInitialRankUpFlag(true);
}

void ClientTCPSocket::__Parse_Base_GetMyInfo_InventoryItem( i3NetworkPacket * pPacket)
{
	USER_INFO_INVITEM_DATA	MyInvenData;
	pPacket->ReadData(&MyInvenData, sizeof(USER_INFO_INVITEM_DATA));
	UserInfoContext::i()->SetMyUserInfoInvItemData(MyInvenData);
}

void ClientTCPSocket::__Parse_Base_Announce_SupplyBox( i3NetworkPacket * pPacket)
{
	//TODO :  서버에서 SUPPLY_BOX_ANNOUNCE_SETTING 를 완전 안써먹는듯 하여 일단 지움 (2014.07.21.수빈)

	// 보급상자 남은 상자개수 & 리셋시간
	//이정우 : 남은 보급 상자 갯수 및 초기화 시간을 알린다.
	/*
	SUPPLY_BOX_ANNOUNCE_SETTING stSupplyAnnounce;
	pPacket->ReadData( &stSupplyAnnounce,		sizeof(SUPPLY_BOX_ANNOUNCE_SETTING) );

	if( LocaleValue::Enable("EnableReadyBox") )
	{
		int Hour = (stSupplyAnnounce.ui16ResetTime / 100);
		int Minute = (stSupplyAnnounce.ui16ResetTime % 100);

		i3::stack_wstring wstrResetTime;

		if( Hour < 12 )
		{
			i3::sprintf(wstrResetTime, L"AM %d:%02d", Hour, Minute );
		}
		else
		{
			i3::sprintf(wstrResetTime, L"PM %d:%02d", Hour - 12, Minute );
		}

		i3::rc_wstring wstrBuf;

		i3::sprintf(wstrBuf, GAME_RCSTRING("STR_READYBOX_ANNOUNCE"),
			stSupplyAnnounce.ui16NormalStock,
			stSupplyAnnounce.ui16NormalStockMax,
			stSupplyAnnounce.ui16SilverStock,
			stSupplyAnnounce.ui16SilverStockMax,
			stSupplyAnnounce.ui16GoldStock,
			stSupplyAnnounce.ui16GoldStockMax,
			wstrResetTime);

		//일일 지급되는 보급상자가 일반 %d/500개, 실버 %d/300 개, 골드 %d/100 개 남았습니다. {col:240, 30, 40, 255}(익일 %s에 초기화 됩니다.){/col}
		//대기방에 위쪽에 흐르는 글씨를 날려준다.
		g_pFramework->GetUIMainframe()->SetSlideNotice_Permanant( wstrBuf);
	}
	*/
}

void ClientTCPSocket::__Parse_Base_Message_SupplyBox( i3NetworkPacket * pPacket)
{
	// 보급상자 지급 알림 메시지
	UINT8	ui8PresentMessage;				// 메시지 표출 범위 (enum SUPPLY_BOX_TYPE)
	UINT32	ui32ServerIdx;					// 서버 인덱스
	UINT32	ui32ChannelNum;					// 채널 인덱스
	UINT32	ui32RoomIdx;					// 룸 인덱스
	UINT32	ui32ItemID;						// 지급 아이템
	UINT8	ui8Size;						// 닉네임 길이
	char	strNick[NET_NICK_NAME_SIZE+1];	// 닉네임


	pPacket->ReadData( &ui8PresentMessage,	sizeof(UINT8) );
	pPacket->ReadData( &ui32ServerIdx,		sizeof(UINT32) );
	pPacket->ReadData( &ui32ChannelNum,		sizeof(UINT32) );
	pPacket->ReadData( &ui32RoomIdx,		sizeof(UINT32) );
	pPacket->ReadData( &ui32ItemID,			sizeof(UINT32) );
	pPacket->ReadData( &ui8Size,			sizeof(UINT8) );
	if( ui8Size > NET_NICK_NAME_SIZE ) ui8Size = NET_NICK_NAME_SIZE;
	pPacket->ReadData( strNick,				ui8Size	);

	if( LocaleValue::Enable("EnableReadyBox") )
	{
		//게임 종료후에 대기 방에서 ReadyBox 획득 한 사람 정보
		ReadyBoxContext::i()->SetReadyBoxMsg(ui8PresentMessage, ui32ServerIdx, ui32ChannelNum,
			ui32RoomIdx, ui32ItemID, strNick);

		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_READYBOX_MESSAGE);
	}
}

void ClientTCPSocket::__Parse_Base_Present_SupplyBox( i3NetworkPacket * pPacket)
{
	INT8	i8Presented;					// 지급받은 박스 타입 (enum SUPPLY_BOX_TYPE)
	UINT32	ui32ItemId;

	pPacket->ReadData( &i8Presented,		sizeof(INT8) );
	pPacket->ReadData( &ui32ItemId,			sizeof(UINT32) );
	
	if( LocaleValue::Enable("EnableReadyBox") )
	{
		ReadyBoxContext::i()->SetReadyBoxReward(i8Presented, ui32ItemId);
	}
}

void ClientTCPSocket::__Parse_Base_Logout( i3NetworkPacket * pPacket)
{
	if( g_pFramework->GetUIMainframe() )
		g_pFramework->GetUIMainframe()->SetExitTimer( false );

	// 이패킷을 받아야 정상 종료 처리된다.
	GOTO_EXIT_STAGE();
}

void ClientTCPSocket::__Parse_Base_ChatLimit( i3NetworkPacket * pPacket)
{
	INT8 chatLimitState = CHAT_LIMIT_STATE_NORMAL;

	pPacket->ReadData(&chatLimitState, sizeof(INT8));
	if (chatLimitState != CHAT_LIMIT_STATE_NORMAL)
	{
		INT32 i32LimitTime = 0;
		i3::stack_wstring wstrString;
		pPacket->ReadData(&i32LimitTime, sizeof(INT32));

		switch (chatLimitState)
		{
		case CHAT_LIMIT_STATE_ING:	//(1)
			if ( false == g_pFramework->CheckChatLimit(i32LimitTime) )
			{
				g_pFramework->SetChatLimit(i32LimitTime);
				g_pFramework->ChatLimit_Start();
			}

			i3::sprintf(wstrString, GAME_STRING("STR_MESSAGE_BLOCK_ING"), i32LimitTime);
			g_pFramework->getChatBox()->PutSystemChatting(wstrString);
			break;
		case CHAT_LIMIT_STATE_START: //(2)
		{
			/* 입력하신 메세지가 도배/매크로로 판단되어 i32LimitTime초간 채팅 이용이 불가능합니다*/
			i3::sprintf(wstrString, GAME_STRING("STR_MESSAGE_BLOCK_START"), i32LimitTime);
			GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrString);
			
			g_pFramework->SetChatLimit(i32LimitTime);
			g_pFramework->ChatLimit_Start();
		}
		break;
		default:
			I3TRACE("[PROTOCOL_BASE_CHATTING_ACK] Unknown Chat Limit State : %d", chatLimitState);
			break;
		}
	}
}

//==================================================================================================//
//										1.5 용 Protocol												//
//==================================================================================================//
void ClientTCPSocket::__Parse_Base_GetSystemInfo( i3NetworkPacket * pPacket)
{
	PACKET_BASE_GET_SYSTEM_INFO_ACK Recv;
	S2MORecvPacket( &Recv, (char*)pPacket->GetPacketBuffer() );

	// 랭크업 보상 아이템
	for(INT32 i=0; i < MAX_RANK_COUNT; i++)
	{
		UserInfoContext::i()->SetUserRankUpItemList(i, &Recv.m_aRankUpItem[i]);
	}

	// 게임 서버 & 채널 정보
	UINT32 ui32AllServerCount  = Recv.m_aGameServerInfo.GetCount();

	if(ui32AllServerCount > 0)
	{
		I3ASSERT( ui32AllServerCount < 1000);

		// 첫번째 배열은 Gateway서버이므로, 카운트 -1 한다.
		UINT32 ui32GameServerCount = ui32AllServerCount - 1;

		// 채널 타입 리스트 초기화
		ChannelContext::i()->ClearChannelType();

		// 배열의 첫번쨰는 Gateway서버의 정보입니다.
		// 클라이언트에서는 사용 안하기 때문에, 
		// 받은 서버 정보에 1번째 배열부터 정보를 추출합니다.
		for(UINT32 idx = 0; idx < ui32GameServerCount ; idx++)
		{
			// 채널 정보는 직접 설정한다.
			i3mem::Copy(ChannelContext::i()->SetChannelTypePtr(idx), Recv.m_aGameServerInfo[idx + 1].m_aui8ChannelType, sizeof(UINT8) * GAME_CHANNEL_COUNT);
		}

		// 서버 정보 설정
		g_pServerInfo->CreateServerInfo( 1, ui32GameServerCount);
		g_pServerInfo->UpdateServerInfo(ui32GameServerCount, &Recv.m_aGameServerInfo[1]);
	}

	// 접속할 제압모드 랭킹 웹페이지를 로그인시에 받는다. 2013.02.15 안국정
	i3::generic_string_copy( Domination_Util::g_domi_rank_web, &Recv.m_strRankingURL );
	// 제압모드 최고점수
	Domination_Util::g_best_score		= Recv.m_i32DomiScore;
		
	m_ConnectionInfo.m_GamePort					= Recv.m_ui16UDPPort;
	m_ConnectionInfo.m_MyUdpInfo.m_nLocalIP		= m_InAddr.sin_addr.s_addr;
	m_ConnectionInfo.m_MyUdpInfo.m_nLocalPort	= m_ConnectionInfo.m_GamePort;

	// 야전상점 모드별 리스트 사용 체크 (사용되지 않는거 같아 주석처리 2013.02.15)
	/*UINT32 ui32FieldShopDataFlag;
	pPacket->ReadData( &ui32FieldShopDataFlag, sizeof(UINT32) );
	g_pFramework->GetIGShopContext()->Allocate_StageID(ui32FieldShopDataFlag);*/

	// 신규 내구도 여부
	UserInfoContext::i()->SetNewRepair( Recv.m_bNewRepair );

	UserInfoContext::i()->SetContentsFlag( Recv.m_ContentsFlag );

	I3_BOUNDCHK( Recv.m_ui8MultiWeaponCount - 1, 10);

	UINT8 nMultiWeaponCount = 1;

	if ( Recv.m_ui8MultiWeaponCount > MIN_MULTIWEAPON_SLOT && Recv.m_ui8MultiWeaponCount <= MAX_MULTIWEAPON_SLOT)
		nMultiWeaponCount = Recv.m_ui8MultiWeaponCount;

	UserInfoContext::i()->SetMaxMultiWeapon( nMultiWeaponCount);

#ifdef USE_REPORT
	GameLoginContext::i()->SetReportFTPAddress( Recv.m_strReportFtpAddress, Recv.m_ui16ReportFtpPort );
#endif

	// 판수보상 단계별 시간값
	UserInfoContext::i()->SetDailyGiftGaugeArray( &Recv.m_ui32DailyGiftGauge );


	// 클랜 메세지 핸들 생성
	MailBoxContext::i()->CreateClanMessageHandle();

 	ClanSeasonContext::i()->SetSeasonInfo(Recv.m_ClanMatchSeason);

	GiftShopContext::i()->BuildGiftRankingData( &Recv.m_aGiftBuyRanking);

	GameEventSender::i()->SetEvent(EVENT_GET_USER_INFO);
	
	// 서버쪽에서 char 고정으로 협의했으며 이에 따라 UTF-8로 취급한다..
	
	i3::rc_wstring wstrBillingURL;
	i3::utf8_to_utf16( &Recv.m_strBillingURL, wstrBillingURL);

	UserInfoContext::i()->SetBillingURL(wstrBillingURL);

	// 상점 태그 정렬 순서.
	for (INT32 i=0; i<Recv.m_ui8TagCount; i++)
		CShopContext::i()->AddTagPriority(Recv.m_ui8TagPriority[i]);

	// 서버에서 주는 전체 임무카드 목록..(내것이 아니라..게임전체)
	UINT32 ui32QuestCardGoodsID[ QUEST_CARDSET_TYPE_COUNT ];
	Recv.m_ui32QuestCardGoodsID.GetValue( ui32QuestCardGoodsID, QUEST_CARDSET_TYPE_COUNT );
	MCardUtil::Update_Recv_PROTOCOL_BASE_GET_SYSTEM_INFO_ACK( (UINT8)Recv.m_pui32ActiveQuest.GetCount(), &Recv.m_pui32ActiveQuest, ui32QuestCardGoodsID );
	
	GameEventSender::i()->SetEvent(EVENT_GET_STAGE);
}

void ClientTCPSocket::__Parse_Base_GetUserInfo( i3NetworkPacket * pPacket)
{
	PACKET_BASE_GET_USER_INFO_ACK Recv;
	S2MORecvPacket( &Recv, (char*)pPacket->GetPacketBuffer() );

	if( S2MO_IS_SUCCESS( Recv.m_TResult ) )
	{
		UserInfoContext::i()->SetReceivedUserInfo(true);

		USER_INFO_BASIC*	pUserInfoBasic = &Recv.m_UserInfoBasic;
		S2_MULTI_SLOT_INFO* pMultiSlotInfo = &Recv.m_MultiSlotInfo;
		// 나이
		UserInfoContext::i()->setAge( Recv.m_ui8Age );

		// 로그인 횟수
		UserInfoContext::i()->setLoginCount( Recv.m_ui32TotalConnectCount );
		// 유저 개인 정보

		UserInfoContext::i()->SetMyUserInfoBasic( Recv.m_UserInfoBasic );
		UserInfoContext::i()->SetAuthLevel();

		// Point
		UserInfoContext::i()->SetMyPoint(pUserInfoBasic->m_ui32Point);

		// PC Cafe Bonus
		UserInfoContext::i()->SetPCCafeExp( (UINT32)(Recv.m_r32PCCafeExp * 100.0f) );
		UserInfoContext::i()->SetPCCafePoint( (UINT32)(Recv.m_r32PCCafePoint * 100.0f) );

		TutorialContext::i()->SetOldTutorialIng();
		UserInfoContext::i()->SetMyUserInfoInvItemData( Recv.m_UserInfoInvItemData );

		// 개인 전적
		*UserInfoContext::i()->SetMyUserInfoRecord() = (Recv.m_UserInfoRecord);

		//개인 전적에서 오늘의 전적값을 얻어 온다.
		UserInfoContext::i()->SetUserInfoDaily( Recv.m_UserInfoDaily );

		for(INT32 i=0; i<MAX_NOTIFY_MADAL_COUNT; i++)
		{
			CMedalManager::i()->setNotifyMedal(i, (NOTIFY_MEDAL*)&Recv.m_NotifyMedal[i]);
		}

		// 약장, 휘장, 훈장, 마스터 훈장
		MedalContext::i()->SetMedalInfo( Recv.m_ui32Miniature, Recv.m_ui32Insignia, Recv.m_ui32Order, Recv.m_ui32Master);

		// 빠른 방 입장 정보
		for (INT32 i=0; i<MAX_QUICK_JOIN_INFO_COUNT; i++)
		{
			QuickJoinContext::i()->setQuickJoinInfo(i)->m_ui32StageID	=	Recv.m_aQuickJoinInfo[i].m_ui32StageID;
		}

		// 이벤트 아이템 지급 여부 확인
		UserContext::i()->ReceivedEvent = Recv.m_bGetEventItem;

		// 출석 체크 정보
		for (int i = ATTENDANCE_TYPE_DORMANT; i < ATTENDANCE_TYPE_MAX; ++i)
		{
			AttendanceData*		data = AttendanceContext::i()->SetData((ATTENDANCE_TYPE)i);
			
			data->m_AttendanceUser = Recv.m_stAttendance[i];
			data->m_AttendanceInfo = Recv.m_stAttendanceInfo[i];
			
			// 출석 체크 버전이 다르다면 유저정보를 초기화
			if (data->m_AttendanceUser.m_ui32Version != data->m_AttendanceInfo.m_ui32Version)
			{
				i3mem::FillZero(&data->m_AttendanceUser, sizeof(ATTENDANCE_USER));
			}
		}

		CCharaInfoContext::i()->SetMainCharaIdx( TEAM_RED, pMultiSlotInfo->m_ui8TeamSlot[TEAM_RED] );
		CCharaInfoContext::i()->SetMainCharaIdx( TEAM_BLUE, pMultiSlotInfo->m_ui8TeamSlot[TEAM_BLUE] );
		 

		// 추후 정리 예정 -- 김현우 --
		COMMON_PARTS_INFO CommonPartsInfo;
		// vv3 - check
		CommonPartsInfo.SetPartsIdx( pMultiSlotInfo->m_aCommonEquips[ CHAR_EQUIPMENT_COMMON_DINO ].m_TItemDBIdx );
		if( ValidCheck::Item::isDinoItemID( pMultiSlotInfo->m_aCommonEquips[ CHAR_EQUIPMENT_COMMON_DINO ].m_TItemID) == true)
			CommonPartsInfo.SetPartsItemID( pMultiSlotInfo->m_aCommonEquips[ CHAR_EQUIPMENT_COMMON_DINO ].m_TItemID); 
		else
			CommonPartsInfo.SetPartsItemID(DEFAULT_DINO);
		CCharaInfoContext::i()->SetCommonPartsInfo( CommonPartsInfo );

		i3Net::GetName(GetSocket(), &m_InAddr);
		m_ConnectionInfo.m_IsRealIp	= ( m_InAddr.sin_addr.s_addr == Recv.m_ui32ClientIP ) ? true : false;
		m_ConnectionInfo.m_MyUdpInfo.m_nRealIP		= Recv.m_ui32ClientIP;
		m_ConnectionInfo.m_MyUdpInfo.m_nRealPort	= 0;

		GameLoginContext::i()->SetReportFTPTime( Recv.m_ui32ReportServerTime );

		// 선물 자동 지급
		if( Recv.m_bGiftAutoAuth )
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_PERIODOVER"));

		UserInfoContext::i()->SetNationalCode(Recv.m_ui8Country);

		GameEventSender::i()->SetEvent(EVENT_GET_INVEN_INFO);
		GameEventSender::i()->SetEvent(EVENT_GET_REMAIN_MONEY);

		// 호칭 정보!! 추가..
		UINT8 desigSetTypes[ MAX_EQUIP_USER_TITLE ];
		Recv.m_aui8EquipUserTitle.GetValue( desigSetTypes, MAX_EQUIP_USER_TITLE );
		DesigUtil::Update_Recv_PROTOCOL_BASE_GET_USER_INFO_ACK(Recv.m_ui64GainUserTitle, 
			desigSetTypes, Recv.m_i32LimitUserTitleCount);
		//

		// 미션카드 정보!! 추가..
		UINT8 aQuestCtxActive[ MAX_QUEST_ALL_USE_NEW ];
		Recv.m_aQuestCtxActive.GetValue( aQuestCtxActive, MAX_QUEST_ALL_USE_NEW );
		MCardUtil::Update_Recv_PROTOCOL_BASE_GET_USER_INFO_ACK( &Recv.m_QuestIngInfo, aQuestCtxActive);
	}
	else
	{
		I3TRACE("PROTOCOL_BASE_GET_USER_INFO_ACK - Error Code = %d\n", Recv.m_TResult);
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_GET_USER_INFO, Recv.m_TResult);
	
	// 모든 정보를 받았기 때문에 세팅합니다.
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_SET_SERVERTAB_SELECT);
	
}

void ClientTCPSocket::__Parse_Base_GetCharacterInfo( i3NetworkPacket * pPacket)
{
	PACKET_BASE_GET_CHARA_INFO_ACK Packet;
	S2MORecvPacket( &Packet, pPacket->GetPacketBuffer() );

	if ( 0 == Packet.m_ui8PacketIdx  )
	{
		CCharaInfoContext::i()->ClearAll();
	}

	// 캐릭터 정보
	if( 0 < Packet.m_aChara.GetCount() )
	{
		S2MO_CHAR_INFO* pS2CharInfo;
		CHARA_INFO		CharInfo;
		CharInfo.Reset();

		ITEM_INFO		pItemInfo[ CHAR_EQUIPMENT_COUNT ];
		for( int i = 0; i < Packet.m_aChara.GetCount() ; ++i )
		{
			pS2CharInfo = &Packet.m_aChara[i]; //&Recv.m_MultiSlotInfo.m_aCharInfo[i];
			CharInfo.m_eState									= (MULTI_SLOT_STATE)pS2CharInfo->m_ui8SlotState;

			switch( CharInfo.m_eState )
			{
			case MULTI_SLOT_STATE_NORMAL:
				CharInfo.m_CharaBasicInfo.m_ui8CharaState			= CHARA_STATE_CREATED;		
				break;
			default:
				CharInfo.m_CharaBasicInfo.m_ui8CharaState			= CHARA_STATE_NONE;		
				break;
			}
			// 캐릭터 상태( 0 : 없음, 1: 생성)
			CharInfo.m_CharaBasicInfo.m_ui8CharaSlotIdx			= pS2CharInfo->m_ui8SlotIdx;										// 캐릭터 Slot Idx( 0 ~ 9 ) -> 배열인덱스로 대체가능하면 삭제할것				
			CharInfo.m_CharaBasicInfo.m_ui32CreateDate			= pS2CharInfo->m_BasicInfo.m_ui32CreateDate;						// 생성일
			CharInfo.m_CharaSkillInfo.m_PrimarySkill			= (CHARA_CLASS)pS2CharInfo->m_SkillInfo.m_ui8Class;		// 클래스				
			CharInfo.m_CharaSkillInfo.m_CommonSkill				= (COMMON_SKILL_TYPE)pS2CharInfo->m_SkillInfo.m_ui8CommonSkill;
			CharInfo.m_CharaSkillInfo.m_MainSkill				= (MAIN_SKILL_TYPE)pS2CharInfo->m_SkillInfo.m_ui8MainSkill;
			CharInfo.m_CharaSkillInfo.m_AssistSkill				= (ASSIST_SKILL_TYPE)pS2CharInfo->m_SkillInfo.m_ui8AssistSkill;
			CharInfo.m_CharaSkillInfo.m_ui8MainSkillLv			= pS2CharInfo->m_SkillInfo.m_ui8MainSkillLv;
			CharInfo.m_CharaSkillInfo.m_ui8AssistSkillLv		= pS2CharInfo->m_SkillInfo.m_ui8AssistSkillLv;
			CharInfo.m_CharaSkillInfo.m_ui8CommonSkillLv		= pS2CharInfo->m_SkillInfo.m_ui8CommonSkillLv;

			i3::generic_string_copy( CharInfo.m_CharaBasicInfo.m_strCharaName, pS2CharInfo->m_BasicInfo.m_strCharName );

			pS2CharInfo->m_EquipInfo.GetCharEquipmentAll( pItemInfo );

			i3mem::Copy( CharInfo.m_CharaEquipInfo.m_CharWeaponInfo.m_Info, &pItemInfo[ CHAR_EQUIPMENT_PRIMARY ], sizeof(ITEM_INFO)*CHAR_EQUIPMENT_WEAPON_COUNT );

			// vv3 - check
			ITEM_INFO* pPartsInfo = CharInfo.m_CharaEquipInfo.m_CharPartsInfo.GetPartsInfo( CHAR_EQUIPMENT_PARTS_CHARA );
			i3mem::Copy( pPartsInfo, &pItemInfo[ CHAR_EQUIPMENT_CHARA ], sizeof(ITEM_INFO)*(CHAR_EQUIPMENT_PARTS_COUNT) );

			CharInfo.m_ui32BattleTime	 = pS2CharInfo->m_BasicInfo.m_ui32BattleTime;
			CharInfo.m_ui32SkillRelTime	= pS2CharInfo->m_BasicInfo.m_ui32SkillRelTime;

			CCharaInfoContext::i()->AddCharacterInfo( CharInfo );
		}
		
	}

	CCharaInfoContext::i()->SetAllCharaData();
	CCharaInfoContext::i()->CalculateMultiSlotIdx();

	//GameEventReceiver::i()->PushReceivedGameEvent(EVENT_GET_USER_INFO, Recv.m_TResult);
}

void ClientTCPSocket::__Parse_Base_GetInventoryInfo( i3NetworkPacket * pPacket)
{
	PACKET_BASE_GET_INVEN_INFO_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	UINT16 ui16Count		= ( UINT16)Recv.m_aInventory.GetCount();
	INVEN_BUFFER* pBuffer	= &Recv.m_aInventory;
	CInvenList::i()->AddItem( pBuffer, ui16Count );
	
	UserInfoContext::i()->SetReceivedMyInfo(true); 

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_GET_INVEN_INFO, EVENT_ERROR_SUCCESS );

#if defined(AUTO_LOBBY)
	g_pServerInfo->SetGameServer(0, S_CHANNEL_TYPE_ALL);
	GameEventSender::i()->SetEvent(EVENT_CONNECT_SERVER_GAME);
//#else
//	if (g_pEnvSet->m_nUIVersion > 0) 
//	{
//		g_pServerInfo->SetGameServer(0, S_CHANNEL_TYPE_ALL);
//		GameEventSender::i()->SetEvent(EVENT_CONNECT_SERVER_GAME);
//	}
#endif
}

void ClientTCPSocket::__Parse_Base_CreateCharacter( i3NetworkPacket * pPacket)
{
	PACKET_CHAR_CREATE_CHARA_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	if( EV_SUCCESSED( Recv.m_TResult ) )
	{		
		UserInfoContext::i()->SetMyPoint(Recv.m_ui32Point);
		UserInfoContext::i()->SetMyCash(Recv.m_ui32Cash);

		S2MO_CHAR_BASE_INFO* pBase		= &Recv.m_BaseInfo;
		S2MO_CHAR_SKILL_INFO* pSkill	= &Recv.m_SkillInfo;
		S2MO_CHAR_EQUIP_INFO* pEquip	= &Recv.m_CharEquipInfo;
		if( Recv.m_bAddCharaSlot )
		{
			ITEM_INFO	CharItemInfo;
			pEquip->GetCharEquipmentInfo( CHAR_EQUIPMENT_CHARA, &CharItemInfo );

			CHARA_INFO CharInfo;
			CharInfo.Reset();
			UINT8 ui8SlotState = Recv.m_ui8SlotState;
			CharInfo.m_eState								= (MULTI_SLOT_STATE)(ui8SlotState);	// 캐릭터 구매상태. 전대영
			switch( Recv.m_ui8SlotState )
			{
			case MULTI_SLOT_STATE_NORMAL:
				CharInfo.m_CharaBasicInfo.m_ui8CharaState		= CHARA_STATE_CREATED;														// 캐릭터 상태( 0 : 없음, 1: 생성)
				break;
			default:
				CharInfo.m_CharaBasicInfo.m_ui8CharaState		= CHARA_STATE_NONE;		
				break;
			}
			CharInfo.m_CharaBasicInfo.m_ui8CharaSlotIdx		= Recv.m_ui8SlotIdx;										// 캐릭터 Slot Idx( 0 ~ 9 ) -> 배열인덱스로 대체가능하면 삭제할것				
			CharInfo.m_CharaBasicInfo.m_ui32CreateDate		= pBase->m_ui32CreateDate;							// 생성일
			CharInfo.m_CharaSkillInfo.m_PrimarySkill		= (CHARA_CLASS)pSkill->m_ui8Class;		// 클래스				

			i3::generic_string_copy( CharInfo.m_CharaBasicInfo.m_strCharaName, pBase->m_strCharName );


			// vv3 - check
			ITEM_INFO pItemInfo[ CHAR_EQUIPMENT_COUNT ];
			pEquip->GetCharEquipmentAll( pItemInfo );

			i3mem::Copy( CharInfo.m_CharaEquipInfo.m_CharWeaponInfo.m_Info, &pItemInfo[ CHAR_EQUIPMENT_PRIMARY ], sizeof(ITEM_INFO)*CHAR_EQUIPMENT_WEAPON_COUNT );

			ITEM_INFO* pPartsInfo = CharInfo.m_CharaEquipInfo.m_CharPartsInfo.GetPartsInfo( CHAR_EQUIPMENT_PARTS_CHARA );
			i3mem::Copy( pPartsInfo, &pItemInfo[ CHAR_EQUIPMENT_CHARA ], sizeof(ITEM_INFO)*(CHAR_EQUIPMENT_PARTS_COUNT) );

			CharInfo.m_ui32BattleTime	= pBase->m_ui32BattleTime;
			CharInfo.m_ui32SkillRelTime	= pBase->m_ui32SkillRelTime;

			if( pItemInfo[ CHAR_EQUIPMENT_CHARA ].m_TItemDBIdx & DEFAULT_ITEM_DBIDX_MIN )
			{ 
				// 이 경우 최초 생성 이므로 초기화를 해줍니다.
				// 2015.7.14 추가. V1.0/V1.1의 경우에는 ACK가 두번 들어와서 캐릭터가 두가지 생성되기 때문에 캐릭터 보유 갯수로도 체크함
				// 2015.7.25 수정. V1.5에서 아래 작업이 이루어지지 않아 크래시 발생. UI버전으로 체크하도록 수정합니다. 문제시 알려주세요 gyungju.kIm
				if(g_pEnvSet->m_nUIVersion == UFV_1_5)
				{
					CCharaInfoContext::i()->ClearAll();
				}
				else
				{
					if(CCharaInfoContext::i()->GetHasCharacterCount() <= 0)
						CCharaInfoContext::i()->ClearAll();

				}
			}
			CCharaInfoContext::i()->AddCharacterInfo( CharInfo );

			UI_CHARA_INFO	* pNetCharaInfo = UserInfoContext::i()->SetMyUICharaInfo();
			CCharaInfoContext::i()->GetUICharaInfo(pNetCharaInfo);

			//@eunil. V1.0의 경우 패킷이 두번 들어옴, 가진 캐릭터가 기본 캐릭터 뿐일 경우 정렬 불필요
			if (CCharaInfoContext::i()->GetHasCharacterCount() > 2)
				CCharaInfoContext::i()->SortCharacterInfo();

			CCharaInfoContext::i()->CalculateMultiSlotIdx();	
			CCharaInfoContext::i()->SetAllCharaData();

			if( g_pEnvSet->IsV2Version() )
			{
				if (g_pFramework->GetUIMainframe())
				{
					if (g_pFramework->GetUIMainframe()->GetCurrentPhaseType() == UIPHASE_READY_ROOM)
					{
						GameEventSender::i()->SetEvent(EVENT_CHANGE_MAINCHARA_FROM_ROOM, (void*)CharInfo.m_CharaBasicInfo.m_ui8CharaSlotIdx);
					}
				}
			}
		}
	}

	//캐릭터 구입 이후 바뀐 point, cash값 갱신
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_REFRESH_CASH);
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CREATE_CHARA, Recv.m_TResult);
}


void ClientTCPSocket::__Parse_Base_DeleteCharacter( i3NetworkPacket * pPacket)
{
	INT32 i32Rv;
	UINT8 ui8DeleteSlotIdx;
	T_ItemDBIdx TItemDBIdx;

	INT32 i32MainSlotIdx = -1;				// 장착중인 캐릭터 삭제시 변경된 메인 슬롯 인덱스
	
	pPacket->ReadData(&i32Rv,				sizeof(INT32));
	pPacket->ReadData(&ui8DeleteSlotIdx,	sizeof(UINT8));

	if( EV_SUCCESSED(i32Rv) )
	{
		pPacket->ReadData(&TItemDBIdx,			sizeof(T_ItemDBIdx));
		pPacket->ReadData(&i32MainSlotIdx,		sizeof(INT32));

		// 메인슬롯 변경.
		if (-1 != i32MainSlotIdx)
		{
			CCharaInfoContext::i()->SetMainCharaBySlotIdx(static_cast<UINT8>(i32MainSlotIdx));
		}

		UINT8 main_chara_slot_idx[TEAM_COUNT] = {};

		for (INT32 idx = 0; idx < TEAM_COUNT; ++idx)
			main_chara_slot_idx[idx] = CCharaInfoContext::i()->GetMainCharaSlotIdx( static_cast<TEAM_TYPE>(idx) );

		CCharaInfoContext::i()->DelCharacterInfo( ui8DeleteSlotIdx );
		CCharaInfoContext::i()->CalculateMultiSlotIdx();

		CCharaInfoContext::i()->SetMainCharaBySlotIdx(main_chara_slot_idx[TEAM_RED]);
		CCharaInfoContext::i()->SetMainCharaBySlotIdx(main_chara_slot_idx[TEAM_BLUE]);

		CInvenList::i()->DeleteInvenBuffer_only_Chara( TItemDBIdx );//TItemDBIdxChar); 캐릭터는 슬롯 번호를 DBIdx로 사용합니다. 더이상 DBIdx를 서버로부터 받지 않음.
	}

	i3::user_data userdata = i3::user_data(RecvData_EVENT_DELETE_CHARA());
	RecvData_EVENT_DELETE_CHARA &data = i3::value_cast<RecvData_EVENT_DELETE_CHARA>(userdata);
	data._DeleteSlotIdx = ui8DeleteSlotIdx;
	GameEventReceiver::i()->PushReceivedGameEventUserData( EVENT_DELETE_CHARA, userdata, i32Rv);
	

}

void ClientTCPSocket::__Parse_Base_ChangeCharacterEquipment( i3NetworkPacket * pPacket)
{
	PACKET_CHAR_CHANGE_EQUIP_ACK Recv;
	S2MORecvPacket( &Recv, (char*)pPacket->GetPacketBuffer() );
	
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_INVEN_CHANGE_CHARA_EQUIP, Recv.m_TResult);
}

void ClientTCPSocket::__Parse_Base_ChangeCharaState( i3NetworkPacket * pPacket)
{
	PACKET_CHAR_CHANGE_STATE_ACK Recv;
	S2MORecvPacket( &Recv, (char*)pPacket->GetPacketBuffer() );

	UINT8 ui8SlotState = Recv.m_ui8SlotState;
	CCharaInfoContext::i()->SetPlayType(Recv.m_ui8SlotIdx,static_cast<eCharaState>(ui8SlotState));

	// 여기를 막자.
	if (CharaInfoMgr::instance()->GetAuthAndStateCharaChange() == true)
		CharaInfoMgr::instance()->send_event_change_equip();
	else
		CharaInfoMgr::instance()->SetAuthAndStateCharaChange();
}

void ClientTCPSocket::__Parse_Base_LearnSkillSlot( i3NetworkPacket * pPacket)
{
	PACKET_SKILL_SET_ACK ack;
	pPacket->ReadData(&ack, sizeof(PACKET_SKILL_SET_ACK));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_SKILL_SLOT_LEARN,ack.m_i32Rv);
}

void ClientTCPSocket::__Parse_Base_LearnSkillClass( i3NetworkPacket * pPacket)
{
	PACKET_SKILL_CLASS_SET_ACK ack;
	pPacket->ReadData(&ack, sizeof(PACKET_SKILL_CLASS_SET_ACK));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_SKILL_CLASS_SET,ack.m_i32Rv);
}

//void ClientTCPSocket::__Parse_Base_GameServerState( i3NetworkPacket * pPacket)
//{
//	PACKET_H_GAME_SERVER_STATE_ACK ServerHeader;
//	pPacket->ReadData(&ServerHeader, sizeof(PACKET_H_GAME_SERVER_STATE_ACK));
//	
//	UINT32 ui32AllServerCount  = ServerHeader.m_i32GameServerCount;
//
//	if(ui32AllServerCount > 0)
//	{
//		PACKET_D_GAME_SERVER_STATE_GAME_SERVER_INFO * pInfo = nullptr;
//		INT32 nInfoSize = sizeof(PACKET_D_GAME_SERVER_STATE_GAME_SERVER_INFO) * ui32AllServerCount;
//		pInfo = (PACKET_D_GAME_SERVER_STATE_GAME_SERVER_INFO*) i3MemAlloc(nInfoSize);
//		pPacket->ReadData(pInfo, nInfoSize);
//
//		{
//			// 첫번째 배열은 트랜스서버이므로, 카운트 -1 한다.
//			UINT32 ui32GameServerCount = ui32AllServerCount - 1;
//
//			GAME_SERVER_INFO * pServerInfo = nullptr;
//			INT32 nServerSize = sizeof(GAME_SERVER_INFO) * ui32GameServerCount;
//			pServerInfo = (GAME_SERVER_INFO *) i3MemAlloc(nServerSize);
//
//			// 채널 타입 리스트 초기화
//			ChannelContext::i()->ClearChannelType();
//
//			// 배열의 첫번쨰는 트랜스서버의 정보입니다.
//			// 클라이언트에서는 사용 안하기 때문에, 받은 서버 정보에 1번째 배열부터
//			// 정보를 추출합니다.
//			for(UINT32 idx = 0; idx < ui32GameServerCount ; idx++)
//			{
//				// 게임 서버 정보 추출
//				i3mem::Copy(&pServerInfo[idx], &pInfo[idx + 1].m_GameServerInfo, sizeof(GAME_SERVER_INFO));
//
//				// 채널 정보는 직접 설정한다.
//				i3mem::Copy(ChannelContext::i()->SetChannelTypePtr(idx), &pInfo[idx + 1].m_pChannelType, sizeof(UINT8) * GAME_CHANNEL_COUNT);
//			}
//
//			// 서버 정보 설정
//			g_pServerInfo->UpdateServerInfo(ui32GameServerCount, pServerInfo);
//			I3MEM_SAFE_FREE(pServerInfo);
//		}
//
//		I3MEM_SAFE_FREE(pInfo);
//	}
//}

void ClientTCPSocket::__Parse_Base_GetChannelList( i3NetworkPacket * pPacket)
{
	PACKET_BASE_GET_CHANNELLIST_ACK Recv;
	S2MORecvPacket( &Recv, (char*)pPacket->GetPacketBuffer() );

	if( Recv.m_ui8ChannelCount > 0)
	{
		ChannelContext::i()->SetChannelCount( (INT32)Recv.m_ui8ChannelCount );
		ChannelContext::i()->SetChannelMaxUser( (INT32)Recv.m_ui16MaxUserCount );
		ChannelContext::i()->SetChannelUserCountArray( &Recv.m_ui16NowUserCount, Recv.m_ui8ChannelCount );
	}

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_GET_CHANNEL_LIST);
#if defined(AUTO_LOBBY)
	INT32 i = 8;
	GameEventSender::i()->SetEvent(EVENT_SELECT_CHANNEL, &i);
//#else
//	if (g_pEnvSet->m_nUIVersion > 0)
//	{
//		INT32 i = 8;
//		GameEventSender::i()->SetEvent(EVENT_SELECT_CHANNEL, &i);
//	}
#endif

}

void ClientTCPSocket::__Parse_Base_SelectChannel( i3NetworkPacket * pPacket)
{
	PACKET_SELECT_CHANNEL_ACK Recv;
	S2MORecvPacket( &Recv, (char*)pPacket->GetPacketBuffer() );

	// 이거 구조좀...
	if( EV_SUCCESSED( Recv.m_TResult ) )
	{
		Recv.m_TResult = Recv.m_ui16ChannelNum;
	}
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_SELECT_CHANNEL, Recv.m_TResult );

	I3PRINTLOG(I3LOG_QA,"  ** channel %d", Recv.m_TResult);
#if defined(AUTO_LOBBY)
	GameEventSender::i()->SetEventOnce(EVENT_ENTER_LOBBY);
//#else
//	if (g_pEnvSet->m_nUIVersion > 0)
//		GameEventSender::i()->SetEventOnce(EVENT_ENTER_LOBBY);
#endif

}

void ClientTCPSocket::__Parse_Base_LeaveServer( i3NetworkPacket * pPacket)
{
#if defined(CHECK_CONNECTION_STATE)
	if ( NetworkContext::i()->ConnectionState != CONNECTION_STATE_LEAVING_FROM_SERVER )
	{
		I3PRINTLOG(I3LOG_FATAL, "Recv Packet PROTOCOL_NEW_USER_LEAVE_ACK error : %d", NetworkContext::i()->ConnectionState);
	}

	NetworkContext::i()->ConnectionState = CONNECTION_STATE_LEAVED_FROM_SERVER;
#endif

	PACKET_BASE_USER_LEAVE_ACK Packet;
	S2MORecvPacket( &Packet, (char*)pPacket->GetPacketBuffer() );

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_USER_LEAVE, Packet.m_TResult);

	I3PRINTLOG(I3LOG_QA," ** %d / %d", Packet.m_TResult, m_ConnectionInfo.m_SessionIndex);

#if defined(AUTO_LOBBY)
	NetworkContext::i()->ChangeSocket();
	GameEventSender::i()->SetEventOnce(EVENT_USER_ENTER);
//#else
//	if (g_pEnvSet->m_nUIVersion > 0)
//	{
//		NetworkContext::i()->ChangeSocket();
//		GameEventSender::i()->SetEventOnce(EVENT_USER_ENTER);
//	}
#endif
}

void ClientTCPSocket::__Parse_Base_EnterServer( i3NetworkPacket * pPacket)
{
#if defined(CHECK_CONNECTION_STATE)
	if ( NetworkContext::i()->ConnectionState != CONNECTION_STATE_ENTERING_TO_SERVER )
	{
		I3PRINTLOG(I3LOG_FATAL, "Recv Packet PROTOCOL_BASE_USER_ENTER_ACK error : %d", NetworkContext::i()->ConnectionState);
	}

	NetworkContext::i()->ConnectionState = CONNECTION_STATE_ENTERED_TO_SERVER;
#endif
	PACKET_BASE_USER_ENTER_ACK Recv;
	S2MORecvPacket( &Recv, (char*)pPacket->GetPacketBuffer() );

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_USER_ENTER, Recv.m_TResult);

	SetEnteredServer(EV_SUCCESSED( Recv.m_TResult ));
	DEV_LOG("  PROTOCOL_BASE_USER_ENTER_ACK %d / %d", Recv.m_TResult, m_ConnectionInfo.m_SessionIndex);

#if defined(AUTO_LOBBY)
	INT32 i=8;
	GameEventSender::i()->SetEventOnce(EVENT_SELECT_CHANNEL, &i);
//#else
//	if (g_pEnvSet->m_nUIVersion > 0)
//	{
//		INT32 i=8;
//		GameEventSender::i()->SetEventOnce(EVENT_SELECT_CHANNEL, &i);
//	}
#endif
}

void ClientTCPSocket::__Parse_Base_ChangeCharacterName( i3NetworkPacket * pPacket)
{
	PACKET_CHAR_CHANGE_CHARA_NICK_ACK Recv;
	S2MORecvPacket( &Recv, (char*)pPacket->GetPacketBuffer() );

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CHANGE_CHARA_NICK, Recv.m_TResult );
}

void ClientTCPSocket::__Parse_Base_FindUserPosition( i3NetworkPacket * pPacket)
{
	PACKET_BASE_USER_POSITION_ACK ack;
	pPacket->ReadData(&ack, sizeof(PACKET_BASE_USER_POSITION_ACK));

	CCommunityContext::i()->SetUserPosInfo(&ack);
}

void ClientTCPSocket::__Parse_Base_GetUserBasicInfo( i3NetworkPacket * pPacket)
{
	//PACKET_BASE_GET_USER_BASIC_INFO_ACK Recv;
	//S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	//if(EV_SUCCESSED( Recv.m_TResult ))
	//{
	//	PACKET_BASE_USER_BASIC_INFO_ACK BasicInfo;

	//	BasicInfo.m_i32Rv				= Recv.m_TResult;
	//	BasicInfo.m_i64FindUID			= Recv.m_TFindUID;
	//	BasicInfo.m_clanMark			= Recv.m_ui32ClanMark;
	//	BasicInfo.m_rank				= Recv.m_ui8Rank;
	//	BasicInfo.m_exp					= Recv.m_ui32Exp;
	//	BasicInfo.m_killCount			= Recv.m_ui32KillCount;
	//	BasicInfo.m_deathCount			= Recv.m_ui32DeathCount;
	//	BasicInfo.m_State				= Recv.m_i8State;
	//	BasicInfo.m_ServerIdx			= Recv.m_i8ServerIdx;
	//	BasicInfo.m_ChannelNum			= Recv.m_i8ChannelNum;
	//	BasicInfo.m_RoomIdx				= Recv.m_i16RoomIdx;
	//	BasicInfo.m_ui32TotalBattleTime	= Recv.m_ui32TotalBattleTime;
	//	i3::generic_string_copy( BasicInfo.m_strClanName,	Recv.m_strClanName );
	//	i3::generic_string_copy( BasicInfo.m_nickName,		Recv.m_strNickName );

	//	g_pCommunity->SetUserSimpleInfo( &BasicInfo );
	//}

	//GameEventReceiver::i()->PushReceivedGameEvent( EVENT_GET_USERINFO_SESSION, Recv.m_TResult );
}

void ClientTCPSocket::__Parse_Base_GetUserDetailInfo( i3NetworkPacket * pPacket)
{
	PACKET_BASE_GET_USER_DETAIL_INFO_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	PACKET_H_BASE_USER_DETAIL_INFO_ACK Info;
	Info.m_i32Rv				= Recv.m_TResult;

	S2_USER_DETAIL_INFO* pUserDetail = &Recv.m_UserDetailInfo;

	Info.m_UserInfoLastGame				= pUserDetail->m_LastGame;
	Info.m_iCount						= pUserDetail->m_i8CharCount;

	Info.m_UserInfoRecord._match		= pUserDetail->m_ui32Match;
	Info.m_UserInfoRecord._win			= pUserDetail->m_ui32Win;
	Info.m_UserInfoRecord._lose			= pUserDetail->m_ui32Lose;
	Info.m_UserInfoRecord._draw			= pUserDetail->m_ui32Draw;
	Info.m_UserInfoRecord._killcount	= pUserDetail->m_ui32Kill;
	Info.m_UserInfoRecord._headshot		= pUserDetail->m_ui32HeadShot;
	Info.m_UserInfoRecord._death		= pUserDetail->m_ui32Death;
	Info.m_UserInfoRecord._dmatch		= pUserDetail->m_ui32Match;
	Info.m_UserInfoRecord._dkillcount	= pUserDetail->m_ui32HeadShot;
	Info.m_UserInfoRecord._dis			= pUserDetail->m_ui32DisConnect;
	Info.m_UserInfoRecord._AssistCount	= pUserDetail->m_ui32Assist;

	Info.m_UserInfoRecord._smatch		= pUserDetail->m_ui32Match;
	Info.m_UserInfoRecord._swin			= pUserDetail->m_ui32Win;
	Info.m_UserInfoRecord._slose		= pUserDetail->m_ui32Lose;
	Info.m_UserInfoRecord._sdraw		= pUserDetail->m_ui32Draw;
	Info.m_UserInfoRecord._skillcount	= pUserDetail->m_ui32Kill;
	Info.m_UserInfoRecord._sheadshot	= pUserDetail->m_ui32HeadShot;
	Info.m_UserInfoRecord._sdeath		= pUserDetail->m_ui32Death;
	Info.m_UserInfoRecord._sdmatch		= pUserDetail->m_ui32Match;
	Info.m_UserInfoRecord._sdkillcount	= pUserDetail->m_ui32HeadShot;
	Info.m_UserInfoRecord._sdis			= pUserDetail->m_ui32DisConnect;
	Info.m_UserInfoRecord._sAssistCount = pUserDetail->m_ui32Assist;

	if(EV_SUCCESSED( Recv.m_TResult ))
	{
		CCommunityContext::i()->SetUserDetailInfo( &Info );

		if( pUserDetail->m_i8CharCount > 0)
		{
			CCommunityContext::i()->SetUserDetailInfo()->_Clear();

			for(INT32 idx = 0 ; idx < pUserDetail->m_i8CharCount ; ++idx)
			{
				PACKET_D_BASE_USER_DETAIL_INFO_CHARA ack_Chara;
				ack_Chara.m_ui8CharaState					= CHARA_STATE_POS;
				ack_Chara.m_ui8SlotNum						= pUserDetail->m_aCharInfo[idx].m_ui8SlotIdx;
				ack_Chara.m_ui32CharaItemID					= pUserDetail->m_aCharInfo[idx].m_TItemID;
				i3::generic_string_copy( ack_Chara.m_strCharaName, pUserDetail->m_aCharInfo[idx].m_strCharName );
				ack_Chara.m_ui32CreateDate					= pUserDetail->m_aCharInfo[idx].m_ui32CreateDate;
				ack_Chara.m_CharaSkill.m_PrimarySkill		= (CHARA_CLASS)pUserDetail->m_aCharInfo[idx].m_SkillInfo.m_ui8Class;
				ack_Chara.m_CharaSkill.m_CommonSkill		= (COMMON_SKILL_TYPE)pUserDetail->m_aCharInfo[idx].m_SkillInfo.m_ui8CommonSkill;
				ack_Chara.m_CharaSkill.m_MainSkill			= (MAIN_SKILL_TYPE)pUserDetail->m_aCharInfo[idx].m_SkillInfo.m_ui8MainSkill;
				ack_Chara.m_CharaSkill.m_AssistSkill		= (ASSIST_SKILL_TYPE)pUserDetail->m_aCharInfo[idx].m_SkillInfo.m_ui8AssistSkill;
				ack_Chara.m_CharaSkill.m_ui8CommonSkillLv	= pUserDetail->m_aCharInfo[idx].m_SkillInfo.m_ui8CommonSkillLv;
				ack_Chara.m_CharaSkill.m_ui8MainSkillLv		= pUserDetail->m_aCharInfo[idx].m_SkillInfo.m_ui8MainSkillLv;
				ack_Chara.m_CharaSkill.m_ui8AssistSkillLv	= pUserDetail->m_aCharInfo[idx].m_SkillInfo.m_ui8AssistSkillLv;
		
				CCommunityContext::i()->AddUserDetailInfoChara(ack_Chara);
			}
		}
		PACKET_BASE_USER_BASIC_INFO_ACK BasicInfo;

		BasicInfo.m_i32Rv				= Recv.m_TResult;
		BasicInfo.m_i64FindUID			= pUserDetail->m_TUID;
		BasicInfo.m_clanMark			= pUserDetail->m_ui32ClanMark;
		BasicInfo.m_rank				= pUserDetail->m_ui8Rank;
		BasicInfo.m_exp					= pUserDetail->m_ui32Exp;
		BasicInfo.m_killCount			= pUserDetail->m_ui32Kill;
		BasicInfo.m_deathCount			= pUserDetail->m_ui32Death;
		BasicInfo.m_State				= pUserDetail->m_ui32State;
		BasicInfo.m_ServerIdx			= pUserDetail->m_ui32ServerIdx;
		BasicInfo.m_ChannelNum			= pUserDetail->m_ui32ChannelNum;
		BasicInfo.m_RoomIdx				= pUserDetail->m_ui32RoomIdx;
		BasicInfo.m_nickColor			= pUserDetail->m_InvenData._ui8NickColor;
		BasicInfo.m_DisguiseRank		= pUserDetail->m_InvenData._wRank;
		BasicInfo.m_AprilFoolRank		= pUserDetail->m_InvenData._wAprilFoolRank;

		//BasicInfo.m_ui32TotalBattleTime	= Recv.m_UserDetailInfo.;

		i3::generic_string_copy( BasicInfo.m_strClanName,	pUserDetail->m_strClanName );

		i3::safe_string_copy( BasicInfo.m_nickName, pUserDetail->m_strNickName, NET_NICK_NAME_SIZE);

		CCommunityContext::i()->SetUserSimpleInfo( &BasicInfo );
	}

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_GET_USERINFO_SESSION, Recv.m_TResult );
}

void ClientTCPSocket::__Parse_Base_GetUserSubtaskInfo( i3NetworkPacket * pPacket)
{
	PACKET_BASE_GET_USER_SUBTASK_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	PACKET_H_BASE_USER_SUBTASK_ACK Info;
	Info.m_SessionId = Recv.m_i32SessionIdx;
	Info.m_Subtask	 = Recv.m_ui8SubTask;

	CCommunityContext::i()->SetUserSubTaskInfo( &Info );
}

void ClientTCPSocket::__Parse_Base_InviteNormalUser( i3NetworkPacket * pPacket)
{
	PACKET_COMMUNITY_USER_INVITED_ACK ack;
	pPacket->ReadData(&ack, sizeof(PACKET_COMMUNITY_USER_INVITED_ACK));

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_INVITED_NORMAL_USER_MESSAGE, 0);

	switch( ack.m_i32Rv)
	{
	case EVENT_ERROR_EVENT_FAIL_INVITED_USER:
		{
			/*초대한 유저가 채널 선택을 하지 않은 상태입니다.\n채널 선택 후 초대해 주시기 바랍니다.*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_NETWORK_FAIL_INVITED_USER"));
		}
		break;			
	case EVENT_ERROR_EVENT_FAIL_INVITED_USER_IN_CLAN_MATCH:
		{
			/*클랜전을 대기 중인 유저입니다.\n해당 유저가 팀등록 상태가 아니면\n초대 가능합니다.*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_NETWORK_FAIL_INVITED_USER_IN_CLAN_MATCH"));
		}
		break;			
	default:
		break;
	}
}

void ClientTCPSocket::__Parse_Base_RequestInviteNormalUser( i3NetworkPacket * pPacket)
{
	if(	g_pFramework->IsUIStage() == false)
	{				
		return;	// ReadyStage가 아니면 처리하지 않는다.
	}

	// 배틀 팀 중이면 리턴
	if( g_pFramework->GetClanContext()->InBattleTeam() ) 
		return;

	PACKET_COMMUNITY_USER_INVITED_REQUEST_ACK ack;

	pPacket->ReadData(&ack , sizeof(PACKET_COMMUNITY_USER_INVITED_REQUEST_ACK));

	if(ack.m_ConnectState > 0)
	{
		if( CCommunityContext::i()->SetMovingToNormalUser(ack.m_ConnectState))
		{
			CCommunityContext::i()->SetInvitedNormalUser(&ack);
			GameEventReceiver::i()->PushReceivedGameEvent(EVENT_INVITED_NORMAL_USER_MESSAGE);
		}
		else
		{
			I3TRACE("Invited message full\n");
		}
	}
	else
	{
		I3TRACE("Invalid User State(%d)\n", ack.m_ConnectState);
	}
}

//void ClientTCPSocket::__Parse_Base_Attendance_Info( i3NetworkPacket * pPacket)
//{
//	AttendanceData*		data = AttendanceContext::i()->SetData();
//
//	pPacket->ReadData( &data->m_AttendanceUser,		sizeof(ATTENDANCE_USER));
//	pPacket->ReadData( &data->m_AttendanceInfo,		sizeof(ATTENDANCE_INFO));
//
//	// 출석 체크 버전이 다르다면 유저정보를 초기화
//	if(data->m_AttendanceUser.m_ui32Version != data->m_AttendanceInfo.m_ui32Version)
//	{
//		i3mem::FillZero(&data->m_AttendanceUser, sizeof(ATTENDANCE_USER));
//	}
//}

void ClientTCPSocket::__Parse_Base_Attendance_Check( i3NetworkPacket * pPacket)
{
	INT32 i32Rv;
	ATTENDANCE_USER pAttendanceUser;

	pPacket->ReadData(&i32Rv, sizeof(INT32));
	
	if(EV_IS_ERROR(i32Rv , EVENT_ERROR_ATTENDANCE_SUCCESS))
	{
		pPacket->ReadData(&pAttendanceUser, sizeof(ATTENDANCE_USER));

		ATTENDANCE_TYPE eType = ATTENDANCE_TYPE(pAttendanceUser.m_ui8AttendanceType);

		AttendanceData*		data = AttendanceContext::i()->SetData(eType);

		if(data->m_AttendanceUser.m_ui32Version == pAttendanceUser.m_ui32Version)
			data->m_AttendanceUser = pAttendanceUser;
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_ATTENDANCE_CHECK, i32Rv);
}

void ClientTCPSocket::__Parse_Base_Attendance_Clear_Item( i3NetworkPacket * pPacket)
{
	INT32 i32Rv;

	pPacket->ReadData(&i32Rv, sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_ATTENDANCE_CLEAR, i32Rv);
}

// Key : User Guide System
void ClientTCPSocket::__Parse_Base_UserGuideComplete( i3NetworkPacket * pPacket)
{
	PACKET_BASE_GUIDE_COMPLETE_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );
	
	// 성공시에만 데이터 설정
	if(EV_SUCCESSED( Recv.m_TResult ))
	{
		USER_INFO_BASIC MyInfoBasic;

		UserInfoContext::i()->GetMyUserInfoBasic(&MyInfoBasic);
		MyInfoBasic.m_ui64GuideComplete		= Recv.m_ui64GuideComplete;
		MyInfoBasic.m_ui32Point				= Recv.m_ui32Point;

		UserInfoContext::i()->SetMyUserInfoBasic(MyInfoBasic);

		// Key : User Guide System
		{
			UserGuideMgr::i()->SetReceivedGuideData(Recv.m_ui16GuideEventIdx, Recv.m_ui32GetPoint);
		}
	}

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_USER_GUIDE, Recv.m_TResult );
} 

void ClientTCPSocket::__Parse_Base_Reward_Popup(i3NetworkPacket* pPacket)
{
	PACKET_H_BASE_REWARD_POPUP_ACK		PacketAckH;
	pPacket->ReadData(&PacketAckH,	sizeof(PACKET_H_BASE_REWARD_POPUP_ACK));

	switch(PacketAckH.m_i8RequestType)
	{
	case ITEM_INSERT_REQUEST_TS_BONUS:
		{
			// TODO: 판수보상 UI 연결
			PACKET_D_BASE_REWARD_DAILY_GIFT data;
			pPacket->ReadData(&data, sizeof(PACKET_D_BASE_REWARD_DAILY_GIFT));
			
			UserInfoContext::i()->SetTSRewardItemData(data.m_TItemID, data.m_ui32ItemArg);
			
			UIMainFrame* pFrame = g_pFramework->GetUIMainframe();
			
			if ( pFrame )
			{
				pFrame->OpenPopup( UPW_TS_REWARD );
				UserInfoContext::i()->SetTSRewardFlag(false);
			}
		}
		break;
	case ITEM_INSERT_REQUEST_DOMI_REWARD:
		{
			PACKET_D_BASE_REWARD_DOMI	PacketD;
			pPacket->ReadData(&PacketD, sizeof(PACKET_D_BASE_REWARD_DOMI));

			// TODO: 제압보상 UI 연결.
			domi_state_info* info = domi_state_info::i();
			if( info)
			{
				info->award_item = PacketD.m_TItemID;
				info->award_item_arg = PacketD.m_ui32ItemArg;

				if( PacketD.m_ui16Point > 0 && PacketD.m_TItemID == 0)
				{
					info->award_item = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_POINT, CASHITEM_DAY_NO, 1);
					info->award_point = PacketD.m_ui16Point;
				}

				UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_DETAIL, dui::FLOW_D_AWARD);
				
				//제압 미션 최종 결과창 닫기.
				UIHUDDominationMgr::i()->EnableHUD( HUD_DOMI_ROUND_RESULT, false);
			}
		}
		break;
	default:
		break;
	}
}

void ClientTCPSocket::__Parse_Base_Equipment(i3NetworkPacket* pPacket)
{
	pPacket->ReadData(&IGShopItemController::GetInstance()->m_Base_ItemInfo_Buf, 
		sizeof(PACKET_PROTOCOL_BASE_EQUIPMENT_ACK));
}

void ClientTCPSocket::__Parse_Base_DailyRecord(i3NetworkPacket* pPacket)
{
	pPacket->ReadData( UserInfoContext::i()->SetUserInfoDaily(), sizeof(USER_INFO_DAILY));
	// 중첩 예외처리
	if (g_pFramework != nullptr)
	{
		UIMainFrame * pMainframe = g_pFramework->GetUIMainframe();
		if (pMainframe != nullptr)
		{
			pMainframe->OpenPopup(UPW_EXIT_POPUP);
		}
	}
}

void ClientTCPSocket::__Parse_Base_ChangePCCafeStatus(i3NetworkPacket* pPacket)
{
	PACKET_BASE_CHANGE_PCCAFE_STATUS_ACK		pkt;
	S2MORecvPacket( &pkt, pPacket->GetPacketBuffer() );

	USER_INFO_BASIC myUserInfoBasic;
	UserInfoContext::i()->GetMyUserInfoBasic(&myUserInfoBasic);
	myUserInfoBasic.m_ui8PCCafe = pkt.m_ui8PCCafe;
	UserInfoContext::i()->SetMyUserInfoBasic(myUserInfoBasic);

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_PCCAFE_CHANGED_STATUS, (INT32)pkt.m_ui8PCCafe);
}

void ClientTCPSocket::__Parse_Base_ReceiveMegaPhone( i3NetworkPacket * pPacket)
{
	PACKET_BASE_MEGAPHONE_ACK Packet;
	S2MORecvPacket( &Packet, pPacket->GetPacketBuffer() );

	TTCHAR senderNick[NET_NICK_NAME_SIZE];
	TTCHAR message[MAX_STRING_COUNT];

	Packet.m_strNickName.GetValue( senderNick, NET_NICK_NAME_SIZE);
	i3::rc_wstring wstrstrNickMsg = senderNick;

	Packet.m_strChatMessage.GetValue( message, MAX_STRING_COUNT);

	i3::wstring wstrstrMsg = message;

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	//테스트용 코드입니다... 300개를 직접 입력하는건 사람이 할 짓이 아닌것 같아서...
	i3::wstring cheat = L"TEST_PUT_300";
	if( i3::contain_string( wstrstrMsg, cheat ) != -1 )
	{
		char Test[20];
		for( int i = 0 ; i < 300 ; i++ )
		{
			sprintf(Test,"메시지_%d",i+1);
			i3::mb_to_utf16(Test, wstrstrMsg);
			g_pFramework->getChatBox()->PutUserMegaPhone( wstrstrNickMsg, wstrstrMsg); 
		}
	}else
	{
		g_pFramework->getChatBox()->PutUserMegaPhone( wstrstrNickMsg, wstrstrMsg);
	}
#else
	g_pFramework->getChatBox()->PutUserMegaPhone( wstrstrNickMsg, wstrstrMsg); 
#endif
	
}

void ClientTCPSocket::__Parse_Base_Notice(i3NetworkPacket* pPacket)
{

	PACKET_NOTICE_ACK Recv;
	S2MORecvPacket(&Recv, pPacket->GetPacketBuffer());

	TTCHAR strSliding[NOTICE_STRING_COUNT] = { 0, };
	TTCHAR strChat[NOTICE_STRING_COUNT] = { 0, };
	//	UINT32 nLength;

	Recv.m_strSlidingNotice.GetValue(strSliding, _countof(strSliding));
	Recv.m_strChatNotice.GetValue(strChat, _countof(strChat));

	INT32 i32Color = Recv.m_i32SlidingRGB;

	// Sliding Notice Setting
	i3::rc_wstring wstrNotice = strSliding;
	g_pFramework->SetNotice_Permanant(wstrNotice, i32Color);

	i32Color = Recv.m_i32ChatRGB;

	// Chatting Notice Setting 
	wstrNotice = strChat;
	g_pFramework->setChatNotice(wstrNotice, i32Color);
}

void ClientTCPSocket::__Parse_Base_URL_List(i3NetworkPacket* pPacket)
{
	UINT8 URLVersion = 0;
	UINT8 listCount = 0;

	pPacket->ReadData(&URLVersion, sizeof(UINT8));
	pPacket->ReadData(&listCount, sizeof(UINT8));

	GameLoginContext::i()->SetIngameURLVersion(URLVersion);
	GameLoginContext::i()->ClearIngameURLInfo();

	BANNER_URL_INFO urlInfo;
	UINT16 length = 0;

	for (int i = 0; i < listCount; i++)
	{
		wchar_t url[MAX_STRING_COUNT] = { 0 };

		pPacket->ReadData(&length, sizeof(UINT16));
		pPacket->ReadData(&urlInfo.i32Type, sizeof(UINT32));
		pPacket->ReadData(&urlInfo.i32Data, sizeof(UINT32));
		pPacket->ReadData(url, sizeof(wchar_t) * length);
		urlInfo.wsURL = url;

		GameLoginContext::i()->SetIngameURLInfo(urlInfo);
	}
	
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_GET_INGAME_URL);
}

void ClientTCPSocket::__Parse_PROTOCOL_BASE_USER_TITLE_EQUIP_ACK( i3NetworkPacket * pPacket)
{
	INT32 result;
	pPacket->ReadData(&result, sizeof(result));

	if( result == EVENT_ERROR_SUCCESS)
	{
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_DESIGNATION_CHANGE, result);

		DesigUtil::Update_Recv_PROTOCOL_BASE_USER_TITLE_EQUIP_ACK();

		/*선택하신 호칭이 정상적으로 등록되었습니다.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_NETWORK_REGISTER_DESIGNATION"));

	}
	else if( result == EVENT_ERROR_TITLE_EQUIP_DUPLICATE_CATEGORY)
	{
		I3TRACE("Equip Fail - TITLE_EQUIP_DUPLICATE_CATEGORY\n");
		/*다른 슬롯에 동일한 계열의 호칭을 사용하고 있어 사용에 실패하였습니다.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_NETWORK_DUPLICATED_DESIGNATION_TYPE"));
	}
	else if (result == EVENT_ERROR_TITLE_EQUIP_SLOTIDX)
	{
		
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_NETWORK_DUPLICATED_DESIGNATION_TYPE_3"));
	}
	else if(result == EVENT_ERROR_FAIL)
	{
		/*다른 슬롯에 동일한 계열의 호칭을 사용하고 있어 사용에 실패하였습니다.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_NETWORK_DUPLICATED_DESIGNATION_TYPE"));
	}
	
}

void	ClientTCPSocket::__Parse_PROTOCOL_BASE_USER_TITLE_CHANGE_ACK( i3NetworkPacket* pPacket)
{
	INT32 result;
	INT32 nSlotCount;
	pPacket->ReadData(&result,		sizeof(result));
	pPacket->ReadData(&nSlotCount,	sizeof(INT32));

	if( result == EVENT_ERROR_SUCCESS)
	{
		DesigUtil::Update_Recv_PROTOCOL_BASE_USER_TITLE_CHANGE_ACK(nSlotCount);
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_DESIGNATION_GET, result);
}

void	ClientTCPSocket::__Parse_PROTOCOL_BASE_USER_TITLE_RELEASE_ACK(i3NetworkPacket* pPacket)
{
	INT32 result;
	pPacket->ReadData(&result, sizeof(result));

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_DESIGNATION_RELEASE);

	if( result == EVENT_ERROR_SUCCESS)
	{
		DesigUtil::Update_Recv_PROTOCOL_BASE_USER_TITLE_RELEASE_ACK();
	}
	else if( result == EVENT_ERROR_FAIL)
	{
		/*호칭해제에 실패하였습니다.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_NETWORK_FAIL_RELEASE_DESIGNATION"));
	}
	
	
}

void	ClientTCPSocket::__Parse_PROTOCOL_BASE_USER_TITLE_INFO_ACK(i3NetworkPacket* pPacket)
{
	INT64	i64UID;
	INT32	count;
	UINT64	ui64UserTitle;
	UINT8	EquipTitle[MAX_EQUIP_USER_TITLE];

	pPacket->ReadData(&i64UID,	sizeof(UINT32) );
	pPacket->ReadData(&ui64UserTitle,	sizeof(UINT64));
	pPacket->ReadData(EquipTitle,		sizeof(UINT8) * MAX_EQUIP_USER_TITLE);
	pPacket->ReadData(&count,			sizeof(INT32));

	bool	IsMyUID = NetworkContext::i()->UID == i64UID;
	DesigUtil::Update_Recv_PROTOCOL_BASE_USER_TITLE_INFO_ACK(IsMyUID, ui64UserTitle, EquipTitle, count);
}

void	ClientTCPSocket::__Parse_PROTOCOL_BASE_QUEST_GET_ACK(i3NetworkPacket* pPacket)
{
	QUEST_CTX quest;			// 이빠이 받음? 전체는 아니고 4개 정도를 받게 되어있다..
								// 파일에서 읽는 것 이외의 처리..
	for (INT32 i = 0 ; i < MAX_CARDSET_PER_USER ; ++i)
	{	for (INT32 j = 0 ; j < MAX_CARD_PER_CARDSET; ++j)
		{	for (INT32 k = 0 ; k < MAX_QUEST_PER_CARD_NEW ; ++k)
			{
				pPacket->ReadData(&quest, sizeof(QUEST_CTX));			
				MCardUtil::Update_Recv_PROTOCOL_BASE_QUEST_GET_ACK(i,j,k,quest);
			}
		}
	}

}

void	ClientTCPSocket::__Parse_PROTOCOL_BASE_QUEST_GET_ITEM_ACK(i3NetworkPacket* pPacket)
{
	QUEST_ITEM_INFO_NEW	QuestItemInfo;
	
	for (INT32 i = 0 ; i < MAX_CARDSET_PER_USER ; ++i)
	{
		for (INT32 j = 0 ; j < MAX_CARD_PER_CARDSET ; ++j)
		{
			pPacket->ReadData(&QuestItemInfo, sizeof(QUEST_ITEM_INFO_NEW));
			
			MCardUtil::Update_Recv_PROTOCOL_BASE_QUEST_GET_ITEM_ACK(i, j, QuestItemInfo);
		}
	}
	
	
}

void	ClientTCPSocket::__Parse_PROTOCOL_BASE_QUEST_GET_INFO_ACK(i3NetworkPacket* pPacket)
{
	//Quest 받기								완료
	INT8			i8CardSlot;
	QUESTING_INFO	AllQuestInfo;

	pPacket->ReadData( &i8CardSlot,			sizeof(INT8));
	pPacket->ReadData( &AllQuestInfo,		sizeof(QUESTING_INFO));

	MCardUtil::Update_Recv_PROTOCOL_BASE_QUEST_GET_INFO_ACK(&AllQuestInfo, i8CardSlot);

}

// 카드 구입 완료시 오는 패킷..
void	ClientTCPSocket::__Parse_PROTOCOL_BASE_QUEST_BUY_CARD_SET_ACK(i3NetworkPacket* pPacket)
{
	INT32 result;			
	pPacket->ReadData(&result, sizeof(INT32));
	if( EV_SUCCESSED( result ))
	{
		INT8			i8QuestSlotIdx;
		QUESTING_INFO	AllQuestInfo;
		UINT32 ui32Point = 0;
		pPacket->ReadData( &ui32Point,			sizeof(UINT32) );//Set Point 
		pPacket->ReadData( &i8QuestSlotIdx,		sizeof(INT8) );
		pPacket->ReadData( &AllQuestInfo,		sizeof(QUESTING_INFO));

		UserInfoContext::i()->SetMyPoint(ui32Point);
		MCardUtil::Update_Recv_PROTOCOL_BASE_QUEST_BUY_CARD_SET_ACK(&AllQuestInfo, i8QuestSlotIdx);
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_REFRESH_CASH);
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_QUEST_BUY_CARD_SET, result);
}

void	ClientTCPSocket::__Parse_PROTOCOL_BASE_QUEST_CHANGE_ACK(i3NetworkPacket* pPacket)
{
	UINT8 ui8FinishFlagAndProcessedQuestIdx;		
	UINT8 ui8ProcessedTaskCount;		

	pPacket->ReadData( &ui8FinishFlagAndProcessedQuestIdx, sizeof(UINT8) );		// Finish : 0xF0, Index : 0x0F
	pPacket->ReadData( &ui8ProcessedTaskCount, sizeof(UINT8) );					// Count

	MCardUtil::Update_Recv_PROTOCOL_BASE_QUEST_CHANGE_ACK(ui8FinishFlagAndProcessedQuestIdx, ui8ProcessedTaskCount);
}

void	ClientTCPSocket::__Parse_PROTOCOL_BASE_QUEST_DELETE_CARD_SET_ACK(i3NetworkPacket* pPacket)
{
	//카드 삭제								완료
	INT32 result;
	pPacket->ReadData(&result, sizeof(INT32));
	if(result == EVENT_ERROR_SUCCESS)
	{
		QUESTING_INFO	AllQuestInfo;
		pPacket->ReadData( &AllQuestInfo, sizeof(QUESTING_INFO));
		MCardUtil::Update_Recv_PROTOCOL_BASE_QUEST_DELETE_CARD_SET_ACK(&AllQuestInfo);
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_QUEST_DELETE_CARD_SET, result);
}

void	ClientTCPSocket::__Parse_PROTOCOL_BASE_QUEST_ACTIVE_IDX_CHANGE_ACK(i3NetworkPacket* pPacket)
{
	INT32 Rv; 
	UINT8 CardSetIdx; 
	pPacket->ReadData(&Rv, sizeof(INT32));
	pPacket->ReadData(&CardSetIdx, sizeof(UINT8));
	I3TRACE("Rv(0x%x), CardSetIdx(%d)\n", Rv, CardSetIdx);
	i3Error::Log( "Rv(0x%x), CardSetIdx(%d)\n", Rv, CardSetIdx );

	// 일반 보상을 받는다
	if ( TESTBIT(Rv, GET_QUEST_EXP_POINT) )
	{
		i3Error::Log( "최종 아이템 보상창 [ 소켓 - 일반 보상 ] " );

		USER_INFO_BASIC MyUserInfoBasic;
		UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
		UINT32 uiMiniature, uiInsignia, uiOrder, uiMaster;

		pPacket->ReadData( &MyUserInfoBasic.m_ui32Exp,		sizeof(UINT32));
		pPacket->ReadData( &MyUserInfoBasic.m_ui32Point,	sizeof(UINT32));
		pPacket->ReadData( &uiMiniature,	sizeof(UINT32));
		pPacket->ReadData( &uiInsignia,	sizeof(UINT32));
		pPacket->ReadData( &uiOrder,		sizeof(UINT32));
		pPacket->ReadData( &uiMaster,		sizeof(UINT32));
		pPacket->ReadData( &MyUserInfoBasic.m_ui32Rank,	sizeof(UINT32));

		UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);

		MedalContext::i()->setMiniature(uiMiniature);
		MedalContext::i()->setInsignia(uiInsignia);
		MedalContext::i()->setOrder(uiOrder);
		MedalContext::i()->setMaster(uiMaster);
	}


	// v2에서는 GAMEFLAG를 전혀 쓰지 않는 것 같음..일단은 이벤트에 딸려서 정수값을 보내는 것으로 
	// 대체하고 나중에 이쪽 구현을 해야할 것임...
	bool bFinalPrize = false;

	if ( TESTBIT(Rv, GET_QUEST_FINISH_LAST_CARD) )
	{
		i3Error::Log( "최종 아이템 보상창 [ 소켓 - 최종 보상 ] " );
		bFinalPrize = true;
//		g_pGameContext->AddGameFlag( GAMEFLAG_FINALPRIZE);
	}
	
	MCardUtil::Update_Recv_PROTOCOL_BASE_QUEST_ACTIVE_IDX_CHANGE_ACK(bFinalPrize);

	//
	// 카드 하나가 완료되면 
	// 최종보상을 제외하면, 다음번 카드 자동 선택 처리를 해야하는데...그 부분 처리는 
	// EVENT_QUEST_CHANGE_ACTIVE_IDX 처리하는 부분에서 한다..
	//

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_QUEST_CHANGE_ACTIVE_IDX);
}

void	ClientTCPSocket::__Parse_PROTOCOL_BASE_MEDAL_GET_INFO_ACK(i3NetworkPacket* pPacket)
{
	INT64 i64UID;

	pPacket->ReadData( &i64UID, sizeof(INT64));
	UINT32 uiMiniature, uiInsignia, uiOrder, uiMaster;

	pPacket->ReadData( &uiMiniature,	sizeof(UINT32));
	pPacket->ReadData( &uiInsignia,	sizeof(UINT32));
	pPacket->ReadData( &uiOrder,		sizeof(UINT32));
	pPacket->ReadData( &uiMaster,	sizeof(UINT32));

	MedalContext::i()->setMiniature(uiMiniature);
	MedalContext::i()->setInsignia(uiInsignia);
	MedalContext::i()->setOrder(uiOrder);
	MedalContext::i()->setMaster(uiMaster);
}

void	ClientTCPSocket::__Parse_Base_EventItem(i3NetworkPacket * pPacket)
{
	if (UserInfoContext::i() != nullptr)
		UserInfoContext::i()->SetEventReawardFlag(true);
}

void ClientTCPSocket::__Parse_Base_Get_MAP_RULELIST(i3NetworkPacket* pPacket)
{
	PACKET_BASE_MAP_RULELIST_ACK Recv;
	S2MORecvPacket(&Recv, pPacket->GetPacketBuffer());

	if (0 < Recv.m_aRuleData.GetCount())
	{
		MAP_RULE_DATA* rule_data;
		for (int i = 0; i < Recv.m_aRuleData.GetCount(); ++i)
		{
			rule_data = &Recv.m_aRuleData[i];
			if (rule_data->m_ui32Rule <= 0) continue;

			CStageInfoUtil::AddRuleDatatoCSICotext(rule_data);
		}
	}
}

void ClientTCPSocket::__Parse_Base_Get_MAP_MATCHINGLIST(i3NetworkPacket* pPacket)
{
	PACKET_BASE_MAP_MATCHINGLIST_ACK Recv;
	S2MORecvPacket(&Recv, pPacket->GetPacketBuffer());

	if (0 < Recv.m_aStageMatching.GetCount())
	{
		MAP_STAGE_MATCHING* matching_data;
		for (int i = 0; i < Recv.m_aStageMatching.GetCount(); ++i)
		{
			matching_data = &Recv.m_aStageMatching[i];
			CStageInfoUtil::AddStageListtoCSICotext(matching_data);
		}
	}

	//마지막으로 스테이지를 정보를 받았다면 필요한 룰만 정리한다.
	if (Recv.m_bLastMatchingList)
	{
		ClientStageInfoContext::i()->StrightRuleData();
	}
	
}

void ClientTCPSocket::__Parse_Base_Get_BOOSTEVENT_INFO(i3NetworkPacket* pPacket)
{
	/*
	if (0 < g_pContextMain->m_ui16RandomMapCount)
	{
		ui8Enable = 1;
		SendPacket.WriteData(&ui8Enable, sizeof(UINT8));
		SendPacket.WriteData(&r32Exp, sizeof(REAL32));
		SendPacket.WriteData(&r32Point, sizeof(REAL32));
	}
	else
	{
		SendPacket.WriteData(&ui8Enable, sizeof(UINT8));
	}
	ui8Enable = 1;
	SendPacket.WriteData(&ui8Enable, sizeof(UINT8));
	*/

	UINT8 uiUseRandomMap = 0; // 1이면 랜덤맵 개선 사용
	REAL32 r32Exp = 0.0f;
	REAL32 r32Point = 0.0f;

	pPacket->ReadData(&uiUseRandomMap, sizeof(UINT8));

	if (1 == uiUseRandomMap)
	{
		pPacket->ReadData(&r32Exp, sizeof(REAL32));
		pPacket->ReadData(&r32Point, sizeof(REAL32));
		ClientStageInfoContext::i()->SetUseRandomMapRenewalSystem(true);
	}
	else
	{
		ClientStageInfoContext::i()->SetUseRandomMapRenewalSystem(false);
	}

	// 넘길땐 정수로 넘긴다 (ex. 2.001 -> 200%로 표출)
	UINT32 n32Exp = static_cast<UINT32>(r32Exp * 100);
	UINT32 n32Point = static_cast<UINT32>(r32Point * 100);

	ClientStageInfoContext::i()->SetRandomMapEventData(n32Exp, n32Point);
}


