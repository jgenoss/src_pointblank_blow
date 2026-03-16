#include "pch.h"
#include "GameEventHandler.h"

#include "Shop.h"

#include "UI/UIUtil.h"

#if defined( _NPROTECT_)
#include "nProtect.h"
#endif

#include "SteamContext.h"

#include "Network/GameNetworkManager.h"
#include "Network/ClientTCPSocket.h"
#include "MedalManager.h"

#include "GameLoginContext.h"
#include "./StageBattle/NetworkContext.h"
#include "ChannelContext.h"
#include "EnvInfoContext.h"
#include "UserInfoContext.h"
#include "NEW_CREATE_CHARA.h"
#include "LobbyContext.h"
#include "BattleSlotContext.h"
#include "GMAuthorityContext.h"

#include "UI/UIMainFrame.h"
#include "UI/UICharaInfoMgr.h"
#include "GameEvent.h"

#include "i3Base/string/ext/utf16_to_mb.h"


#include "Designation/DesigUtil.h"

#include "USG_Miscs.h"

#if defined(I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
	#define SETEVENT_ERROR(evt,err,szFile,iLine)	I3PRINTLOG(I3LOG_FATAL,  "SetEvent [%s] error : %d at (%s - %d) : %d", evt, err, szFile, iLine);
	#define EVENT_TRACE	I3TRACE
#else
	#define SETEVENT_ERROR(evt,err,szFile,iLine)	__noop
	#define EVENT_TRACE	__noop
#endif

#define		QA_COMMAND_KILL_COUNT	"/killcount"
#define		QA_COMMAND_TS			"/ts"
#define		QA_COMMAND_SOLO_PLAY	"/solo"
#define		QA_COMMAND_TELEPORT		"/tp"
#define		QA_COMMAND_OBJECT_DMG	"/obdmg"
#define		QA_COMMAND_HACK_OFF		"/hackoff"

I3_CLASS_INSTANCE( CGameEventHandler);

CGameEventHandler::CGameEventHandler() : m_ReqMoneyTime(-11.f)
{
	__register_Base();
	__register_Shop();
	__register_Inventory();
	__register_Community();
	__register_Lobby();
	__register_Room();
	__register_Clan();
	__register_QuickJoin();
	__register_Battle();
}

CGameEventHandler::~CGameEventHandler()
{
	_CleanS2MO( &m_eventMap_Base );
	_CleanS2MO( &m_eventMap_Shop );
	_CleanS2MO( &m_eventMap_Inven );
	_CleanS2MO( &m_eventMap_Community );
	_CleanS2MO( &m_eventMap_Lobby );
	_CleanS2MO( &m_eventMap_Room );
	_CleanS2MO( &m_eventMap_Clan );
	_CleanS2MO( &m_eventMap_Battle );
	_CleanS2MO( &m_eventMap_QuickJoin );
	_CleanS2MO( &m_NetFuncMap_Gacha );
}

void CGameEventHandler::_CleanS2MO( i3::unordered_map< GAME_EVENT, SEND_EVENT >* pMap )
{
	i3::unordered_map< GAME_EVENT, SEND_EVENT >::iterator it;
	i3::unordered_map< GAME_EVENT, SEND_EVENT >::iterator end;

	for( it = pMap->begin() ; it != pMap->end() ; it++ )
	{
		SEND_EVENT & mf = it->second;
		I3_SAFE_DELETE( mf.m_pS2MOPacket );
	}
	
}

void CGameEventHandler::regSendEvent( GAME_EVENT event, func fn, S2MOPacketBase* pS2MOBase )
{
	SEND_EVENT Event;
	Event.m_fn			= fn;
	Event.m_pS2MOPacket	= pS2MOBase;

	if( event > EVENT_SHOP_S && event < EVENT_SHOP_E)
	{
		m_eventMap_Shop.insert( i3::unordered_map< GAME_EVENT, SEND_EVENT >::value_type( event, Event));
	}
	else if( event > EVENT_INVEN_S && event < EVENT_INVEN_E)
	{
		m_eventMap_Inven.insert( i3::unordered_map< GAME_EVENT, SEND_EVENT >::value_type( event, Event));
	}
	else if( (event > EVENT_CLAN_S && event < EVENT_CLAN_E) ||
		(event > EVENT_CLAN_NS && event < EVENT_CLAN_NE))
	{
		m_eventMap_Clan.insert(i3::unordered_map< GAME_EVENT, SEND_EVENT >::value_type( event, Event));
	}
	else if( event > EVENT_COMMUNITY_S && event < EVENT_COMMUNITY_E)
	{
		m_eventMap_Community.insert(i3::unordered_map< GAME_EVENT, SEND_EVENT >::value_type( event, Event));
	}
	else if( (event > EVENT_LOBBY_S && event < EVENT_LOBBY_E) ||
		(event > EVENT_LOBBY_NS && event < EVENT_LOBBY_NE) )
	{
		m_eventMap_Lobby.insert(i3::unordered_map< GAME_EVENT, SEND_EVENT >::value_type( event, Event));
	}
	else if( (event > EVENT_ROOM_S && event < EVENT_ROOM_E) ||
		(event > EVENT_ROOM_NS && event < EVENT_ROOM_NE) )
	{
		m_eventMap_Room.insert(i3::unordered_map< GAME_EVENT, SEND_EVENT >::value_type( event, Event));
	}
	else if( (event > EVENT_QUICKJOIN_S && event < EVENT_QUICKJOIN_E))
	{
		m_eventMap_QuickJoin.insert(i3::unordered_map< GAME_EVENT, SEND_EVENT>::value_type( event, Event));
	}
	else if( (event > EVENT_BATTLE_S && event < EVENT_BATTLE_E) ||
		(event > EVENT_BATTLE_NS && event < EVENT_BATTLE_NE) )
	{
		m_eventMap_Battle.insert(i3::unordered_map< GAME_EVENT, SEND_EVENT >::value_type( event, Event));
	}
	else
	{
		I3ASSERT( (event < EVENT_BASE_E) || (event > EVENT_BASE_NS && event < EVENT_BASE_NE) );
		
		m_eventMap_Base.insert(i3::unordered_map< GAME_EVENT, SEND_EVENT >::value_type( event, Event));
	}
}

void CGameEventHandler::SendEvent( GAME_EVENT event, const void * pArg/* = nullptr*/, const void * pArg1 /*= nullptr*/,
										 const void * pArg2 /*= nullptr*/, LPCSTR szFile /*= nullptr*/, int nLine /*= 0*/)
{
	i3::unordered_map< GAME_EVENT, SEND_EVENT >::iterator it;
	i3::unordered_map< GAME_EVENT, SEND_EVENT >::iterator end;

	if( event > EVENT_SHOP_S && event < EVENT_SHOP_E)
	{
		it = m_eventMap_Shop.find( event);
		end = m_eventMap_Shop.end();

#if defined( I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
		const char * pszEventName = __getShopEventName(event);
		if( pszEventName != nullptr)
		{
			EVENT_TRACE( "%s\n", pszEventName);
		}
		else
		{
			EVENT_TRACE( "Unknown Shop Event %d\n", event);
		}
#endif
	}
	else if( event > EVENT_INVEN_S && event < EVENT_INVEN_E)
	{
		it = m_eventMap_Inven.find( event);
		end = m_eventMap_Inven.end();

#if defined( I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
		const char * pszEventName = __getInventoryEventName(event);
		if( pszEventName != nullptr)
		{
			EVENT_TRACE( "%s\n", pszEventName);
		}
		else
		{
			EVENT_TRACE( "Unknown Inventory Event %d\n", event);
		}
#endif
	}
	else if( (event > EVENT_CLAN_S && event < EVENT_CLAN_E) ||
		(event > EVENT_CLAN_NS && event < EVENT_CLAN_NE) )
	{
		it = m_eventMap_Clan.find( event);
		end = m_eventMap_Clan.end();

#if defined( I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
		const char * pszEventName = __getClanEventName(event);
		if( pszEventName != nullptr)
		{
			EVENT_TRACE( "%s\n", pszEventName);
		}
		else
		{
			EVENT_TRACE( "Unknown Clan Event %d\n", event);
		}
#endif
	}
	else if( event > EVENT_COMMUNITY_S && event < EVENT_COMMUNITY_E)
	{
		it = m_eventMap_Community.find( event);
		end = m_eventMap_Community.end();

#if defined( I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
		const char * pszEventName = __getCommunityEventName( event);
		if( pszEventName != nullptr)
		{
			EVENT_TRACE( "%s\n", pszEventName);
		}
		else
		{
			EVENT_TRACE( "Unknown Community Event %d\n", event);
		}
#endif
	}
	else if( (event > EVENT_LOBBY_S && event < EVENT_LOBBY_E) ||
		(event > EVENT_LOBBY_NS && event < EVENT_LOBBY_NE) )
	{
		it = m_eventMap_Lobby.find( event);
		end = m_eventMap_Lobby.end();

#if defined( I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
		const char * pszEventName = __getLobbyEventName(event);
		if( pszEventName != nullptr)
		{
			EVENT_TRACE( "%s\n", pszEventName);
		}
		else
		{
			EVENT_TRACE( "Unknown Lobby Event %d\n", event);
		}
#endif
	}
	else if( (event > EVENT_ROOM_S && event < EVENT_ROOM_E) ||
		(event > EVENT_ROOM_NS && event < EVENT_ROOM_NE) )
	{
		it = m_eventMap_Room.find( event);
		end = m_eventMap_Room.end();

#if defined( I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
		const char * pszEventName = __getRoomEventName(event);
		if( pszEventName != nullptr)
		{
			EVENT_TRACE( "%s\n", pszEventName);
		}
		else
		{
			EVENT_TRACE( "Unknown Room Event %d\n", event);
		}
#endif
	}
	else if ( (event > EVENT_QUICKJOIN_S && event < EVENT_QUICKJOIN_E))
	{
		it = m_eventMap_QuickJoin.find( event);
		end = m_eventMap_QuickJoin.end();

#if defined( I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
		const char * pszEventName = __getQuickjoinEventName(event);
		if( pszEventName != nullptr)
		{
			EVENT_TRACE( "%s\n", pszEventName);
		}
		else
		{
			EVENT_TRACE( "Unknown QuickJoin Event %d\n", event);
		}
#endif
	}
	else if( (event > EVENT_BATTLE_S && event < EVENT_BATTLE_E) ||
		(event > EVENT_BATTLE_NS && event < EVENT_BATTLE_NE) )
	{
		it = m_eventMap_Battle.find( event);
		end = m_eventMap_Battle.end();

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
		const char * pszEventName = __getBattleEventName( event);
		if( pszEventName != nullptr)
		{
			EVENT_TRACE( "%s\n", pszEventName);
		}
		else
		{
			EVENT_TRACE( "Unknown Battle Event %d\n", event);
		}
#endif
	}
	else
	{
		it = m_eventMap_Base.find( event);
		end = m_eventMap_Base.end();

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
		I3ASSERT( (event < EVENT_BASE_E) || (event > EVENT_BASE_NS && event < EVENT_BASE_NE) );

		const char * pszEventName = __getBaseEventName(event);
		if( pszEventName != nullptr)
		{
			EVENT_TRACE( "%s\n", pszEventName);
		}
		else
		{
			EVENT_TRACE( "Unknown Base Event %d\n", event);
		}
#endif
	}

	if ( it != end )
	{
		SEND_EVENT & mf = it->second;

		
		GAMEEVENTINFO 	Info;

		Info._pArg			= pArg;
		Info._pArg1			= pArg1;
		Info._pArg2			= pArg2;
		Info._szFile		= szFile;
		Info._nLine			= nLine;
		Info.m_S2MOPacket	= mf.m_pS2MOPacket;
		
		if( (this->*mf.m_fn)( &Info) == true)
		{
			bool			bSendResult;

			if( Info.m_S2MOPacket )
			{
				bSendResult = CGameNetworkManager::i()->SendTCPMessage( Info.m_S2MOPacket );
			}
			else
			{
				bSendResult = CGameNetworkManager::i()->SendTCPMessage( &Info._SendPacket);
			}

			if( false == bSendResult )
			{
				if (!usg_misc::HackToolDetectionCtx::i()->isHackDetected)	// 울산가드 해킹 탐지 시 별도 팝업창 띄움.
					GameEventReceiver::i()->PushReceivedGameEvent(EVENT_M_ERROR, 0);
			}
		}
	}
	else
	{
		I3PRINTLOG(I3LOG_FATAL,  "Unknown event(%d) !!!!", event);
	}
}

const char * CGameEventHandler::__getBaseEventName( GAME_EVENT evt) const
{
	switch( evt)
	{
	case EVENT_LOG_IN :									return "EVENT_LOG_IN";
	case EVENT_LOG_IN_WEBKEY :							return "EVENT_LOG_IN_WEBKEY";
	case EVENT_GAME_END :								return "EVENT_GAME_END";
	case EVENT_DISCONNECT :								return "EVENT_DISCONNECT";
	case EVENT_RUN_PARAM_CHECK :						return "EVENT_RUN_PARAM_CHECK";
	//case EVENT_GET_SERVER_LIST :						return "EVENT_GET_SERVER_LIST";
	case EVENT_GET_CHANNEL_LIST :						return "EVENT_GET_CHANNEL_LIST";
	case EVENT_SELECT_CHANNEL :							return "EVENT_SELECT_CHANNEL";
	case EVENT_ENTER_LOBBY :							return "EVENT_ENTER_LOBBY";
	case EVENT_LEAVE_LOBBY :							return "EVENT_LEAVE_LOBBY";
	case EVENT_ENTER_PROFILE :							return "EVENT_ENTER_PROFILE";
	case EVENT_ENTER_INFO :								return "EVENT_ENTER_INFO";
	case EVENT_LEAVE_INFO :								return "EVENT_LEAVE_INFO";
	case EVENT_ENTER_ROOM :								return "EVENT_ENTER_ROOM";
	case EVENT_LEAVE_ROOM :								return "EVENT_LEAVE_ROOM";
	case EVENT_ENTER_SHOP :								return "EVENT_ENTER_SHOP";
	case EVENT_LEAVE_SHOP :								return "EVENT_LEAVE_SHOP";
	case EVENT_CONNECT_SERVER_GATEWAY :					return "EVENT_CONNECT_SERVER_GATEWAY";
	case EVENT_CONNECT_SERVER_GAME :					return "EVENT_CONNECT_SERVER_GAME";
	case EVENT_CHANGE_MAINCHARA_FROM_ROOM :				return "EVENT_CHANGE_MAINCHARA_FROM_ROOM";
	case EVENT_TIMEOUT_CLIENT :							return "EVENT_TIMEOUT_CLIENT";
	case EVENT_TIMEOUT_HOST :							return "EVENT_TIMEOUT_HOST";
	case EVENT_GAME_GUARD_AUTH2 :						return "EVENT_GAME_GUARD_AUTH2";
	case EVENT_GAME_GUARD_AUTH3 :						return "EVENT_GAME_GUARD_AUTH3";
	case EVENT_XTRAP_HACK_DETECTED :					return "EVENT_XTRAP_HACK_DETECTED";
	case EVENT_GET_USERINFO :							return "EVENT_GET_USERINFO";
	case EVENT_USER_LEAVE :								return "EVENT_USER_LEAVE";
	case EVENT_USER_ENTER :								return "EVENT_USER_ENTER";
	case EVENT_QUEST_BUY_CARD_SET :						return "EVENT_QUEST_BUY_CARD_SET";
	case EVENT_QUEST_DELETE_CARD_SET :					return "EVENT_QUEST_DELETE_CARD_SET";
	//case EVENT_ATTENDANCE_INFO :						return "EVENT_ATTENDANCE_INFO";
	case EVENT_ATTENDANCE_CHECK :						return "EVENT_ATTENDANCE_CHECK";
	case EVENT_ATTENDANCE_CLEAR :						return "EVENT_ATTENDANCE_CLEAR";
	case EVENT_QUEST_CHANGE_ACTIVE_IDX :				return "EVENT_QUEST_CHANGE_ACTIVE_IDX";
	case EVENT_KICK_USER :								return "EVENT_KICK_USER";
	case EVENT_EXIT_USER :								return "EVENT_EXIT_USER";
	case EVENT_BLOCK_USER :								return "EVENT_BLOCK_USER";
	case EVENT_DESTROY_ROOM :							return "EVENT_DESTROY_ROOM";
	case EVENT_LOBBY_EXIT_USER :						return "EVENT_LOBBY_EXIT_USER";
	case EVENT_FCM_INFO_RETRY :							return "EVENT_FCM_INFO_RETRY";
	case EVENT_LOADSTAGE :								return "EVENT_LOADSTAGE";
	case EVENT_KICK_CHEAT_USER :						return "EVENT_KICK_CHEAT_USER";
	case EVENT_USER_DETAIL_INFO :						return "EVENT_USER_DETAIL_INFO";
	case EVENT_GET_USER_INFO :							return "EVENT_GET_USER_INFO";
	//case EVENT_GET_CHARA_INFO :						return "EVENT_GET_CHARA_INFO";
	case EVENT_GET_INVEN_INFO :							return "EVENT_GET_INVEN_INFO";
	case EVENT_GET_SYSTEM_INFO :						return "EVENT_GET_SYSTEM_INFO";
	case EVENT_SET_CREATECHARA :						return "EVENT_SET_CREATECHARA";
	case EVENT_CHANGE_CHARA_NICK :						return "EVENT_CHANGE_CHARA_NICK";
	case EVENT_SKILL_CHARA_SET :						return "EVENT_SKILL_CHARA_SET";
	case EVENT_SKILL_CLASS_SET :						return "EVENT_SKILL_CLASS_SET";
	case EVENT_SKILL_SLOT_LEARN :						return "EVENT_SKILL_SLOT_LEARN";
	case EVENT_SKILL_SLOT_CANCEL :						return "EVENT_SKILL_SLOT_CANCEL";
	case EVENT_MEDAL_GET_NORMAL_INFO :					return "EVENT_MEDAL_GET_NORMAL_INFO";
	case EVENT_MEDAL_GET_SET_CUR_INFO :					return "EVENT_MEDAL_GET_SET_CUR_INFO";
	case EVENT_MEDAL_GET_SET_COM_INFO :					return "EVENT_MEDAL_GET_SET_COM_INFO";
	case EVENT_MEDAL_GET_REWARD :						return "EVENT_MEDAL_GET_REWARD";
	case EVENT_MEDAL_NOTIFY_REGISTER :					return "EVENT_MEDAL_NOTIFY_REGISTER";
	case EVENT_MEDAL_COMMAND :							return "EVENT_MEDAL_COMMAND";
	case EVENT_GET_USERINFO_SESSION :					return "EVENT_GET_USERINFO_SESSION";
	case EVENT_GET_USERINFO_UID :						return "EVENT_GET_USERINFO_UID";
	case EVENT_GET_USERINFO_NICK :						return "EVENT_GET_USERINFO_NICK";
	case EVENT_GET_USERINFO_ROOM :						return "EVENT_GET_USERINFO_ROOM";
	case EVENT_GET_USER_DETAIL_INFO :					return "EVENT_GET_USER_DETAIL_INFO";
	case EVENT_GET_MYOPTION_N :							return "EVENT_GET_MYOPTION_N";
	case EVENT_N_QUEST_END_NOTICE :						return "EVENT_N_QUEST_END_NOTICE";
	case EVENT_OPTION_SAVE_N :							return "EVENT_OPTION_SAVE_N";
	//case EVENT_OPTION_SAVE_MACRO_N :					return "EVENT_OPTION_SAVE_MACRO_N";
	case EVENT_CHATTING_N :								return "EVENT_CHATTING_N";
	case EVENT_USER_GUIDE :								return "EVENT_USER_GUIDE";
	case EVENT_PCCAFE_CHANGED_STATUS:					return "EVENT_PCCAFE_CHANGED_STATUS";
	case EVENT_DELETE_CHARA :							return "EVENT_DELETE_CHARA";
	case EVENT_GM_AUTHORITY_REQ :						return "EVENT_GM_AUTHORITY_REQ";
	case EVENT_GM_AUTHORITY_CHAT_SEND :					return "EVENT_GM_AUTHORITY_CHAT_SEND";
	case EVENT_GM_AUTHORITY_USER_PENALTY_APPLY :		return "EVENT_GM_AUTHORITY_USER_PENALTY_APPLY";
	case EVENT_GM_AUTHORITY_POPUP_CLOSE :				return "EVENT_GM_AUTHORITY_POPUP_CLOSE";

	case EVENT_GM_CHEATKEY_RANDOMBOX :					return "EVENT_GM_CHEATKEY_RANDOMBOX";

	case EVENT_DESIGNATION_GET:							return "EVENT_DESIGNATION_GET";
	case EVENT_DESIGNATION_CHANGE:						return "EVENT_DESIGNATION_CHANGE";
	case EVENT_DESIGNATION_RELEASE:						return "EVENT_DESIGNATION_RELEASE";

	case EVENT_GET_STAGE:								return "EVENT_GET_STAGE";
	//case EVENT_ABNORMAL_LOG:							return "EVENT_ABNORMAL_LOG";

	case EVENT_REQ_INGAME_URL:							return "EVENT_REQ_INGAME_URL";

	default:
		return "Unknown Base Event";
	}
}


void CGameEventHandler::__register_Base( void)
{
	regSendEvent( EVENT_LOG_IN,						&CGameEventHandler::__evtLogin,							new PACKET_LOGIN_REQ );
	regSendEvent( EVENT_LOG_IN_WEBKEY,				&CGameEventHandler::__evtLoginWebkey,					new PACKET_LOGIN_REQ );
	regSendEvent( EVENT_GAME_END,					&CGameEventHandler::__evtLogout,						new PACKET_BASE_LOGOUT_REQ );
	regSendEvent( EVENT_DISCONNECT,					&CGameEventHandler::__evtDisconnect);
	regSendEvent( EVENT_RUN_PARAM_CHECK,			&CGameEventHandler::__evtCheckRunParam);
	//regSendEvent( EVENT_GET_SERVER_LIST,			&CGameEventHandler::__evtRequestServerList);
	regSendEvent( EVENT_GET_CHANNEL_LIST,			&CGameEventHandler::__evtRequestChannelList,			new PACKET_BASE_GET_CHANNELLIST_REQ );
	regSendEvent( EVENT_SELECT_CHANNEL,				&CGameEventHandler::__evtSelectChannel,					new PACKET_SELECT_CHANNEL_REQ );
	regSendEvent( EVENT_ENTER_LOBBY,				&CGameEventHandler::__evtEnterLobby);
	regSendEvent( EVENT_LEAVE_LOBBY,				&CGameEventHandler::__evtLeaveLobby);
	regSendEvent( EVENT_ENTER_PROFILE,				&CGameEventHandler::__evtEnterMyProfile);
	regSendEvent( EVENT_ENTER_INFO,					&CGameEventHandler::__evtEnterMyProfile);
	regSendEvent( EVENT_LEAVE_INFO,					&CGameEventHandler::__evtLeaveMyProfile);
	regSendEvent( EVENT_ENTER_SHOP,					&CGameEventHandler::__evtEnterShop);
	regSendEvent( EVENT_LEAVE_SHOP,					&CGameEventHandler::__evtLeaveShop);
	regSendEvent( EVENT_CONNECT_SERVER_GATEWAY,		&CGameEventHandler::__evtConnectGatewayServer);
	regSendEvent( EVENT_CONNECT_SERVER_GAME,		&CGameEventHandler::__evtConnectGameServer);
	regSendEvent( EVENT_CHANGE_MAINCHARA_FROM_ROOM,	&CGameEventHandler::__evtChangeMainCharacterFromRoom);
	regSendEvent( EVENT_TIMEOUT_CLIENT,				&CGameEventHandler::__evtGlobal_TimeoutHost);
	regSendEvent( EVENT_TIMEOUT_HOST,				&CGameEventHandler::__evtGlobal_TimeoutClient);
	regSendEvent( EVENT_GAME_GUARD_AUTH2,			&CGameEventHandler::__evtGlobal_GameGuardAuth2);
	regSendEvent( EVENT_GAME_GUARD_AUTH3,			&CGameEventHandler::__evtGlobal_GameGuardAuth3);
	regSendEvent( EVENT_XTRAP_HACK_DETECTED,		&CGameEventHandler::__evtGlobal_XTrapAuth);
	regSendEvent( EVENT_GET_USERINFO,				&CGameEventHandler::__evtGlobal_GetUserInfo);
	regSendEvent( EVENT_USER_LEAVE,					&CGameEventHandler::__evtLeaveUser,						new PACKET_BASE_USER_LEAVE_REQ );
	regSendEvent( EVENT_USER_ENTER,					&CGameEventHandler::__evtEnterUser,						new PACKET_BASE_USER_ENTER_REQ );
	regSendEvent( EVENT_QUEST_BUY_CARD_SET,			&CGameEventHandler::__evtBuyQuestCardset);
	regSendEvent( EVENT_QUEST_DELETE_CARD_SET,		&CGameEventHandler::__evtDeleteQuestCardset);
	//regSendEvent( EVENT_ATTENDANCE_INFO,			&CGameEventHandler::__evtAttendanceInfo);
	regSendEvent( EVENT_ATTENDANCE_CHECK,			&CGameEventHandler::__evtAttendanceCheck);
	regSendEvent( EVENT_ATTENDANCE_CLEAR,			&CGameEventHandler::__evtAttendanceClear);
	regSendEvent( EVENT_QUEST_CHANGE_ACTIVE_IDX,	&CGameEventHandler::__evtQuest_ChangeActive);
	regSendEvent( EVENT_KICK_USER,					&CGameEventHandler::__evtGM_KickUserFromRoom);
	regSendEvent( EVENT_EXIT_USER,					&CGameEventHandler::__evtGM_ExitUserFromRoom);
	regSendEvent( EVENT_BLOCK_USER,					&CGameEventHandler::__evtGM_BlockUserFromRoom);
	regSendEvent( EVENT_DESTROY_ROOM,				&CGameEventHandler::__evtGM_DestroyRoom);
	regSendEvent( EVENT_LOBBY_EXIT_USER,			&CGameEventHandler::__evtGM_ExitUserFromLobby);
	regSendEvent( EVENT_FCM_INFO_RETRY,				&CGameEventHandler::__evtAuth_FCMInfo);
	regSendEvent( EVENT_LOADSTAGE,					&CGameEventHandler::__evtGuard_CheckStageCodename);
	regSendEvent( EVENT_KICK_CHEAT_USER,			&CGameEventHandler::__evtGuard_KickUser);
	regSendEvent( EVENT_USER_DETAIL_INFO,			&CGameEventHandler::__evtUser_DetailInfo);
	regSendEvent( EVENT_GET_USER_INFO,				&CGameEventHandler::__evtGetUserInfo,					new PACKET_BASE_GET_USER_INFO_REQ );
	//regSendEvent( EVENT_GET_CHARA_INFO,			&CGameEventHandler::__evtGetCharaInfo);
	regSendEvent( EVENT_GET_INVEN_INFO,				&CGameEventHandler::__evtGetInventoryInfo,				new PACKET_BASE_GET_INVEN_INFO_REQ );
	regSendEvent( EVENT_GET_SYSTEM_INFO,			&CGameEventHandler::__evtGetSystemInfo,					new PACKET_BASE_GET_SYSTEM_INFO_REQ );
	regSendEvent( EVENT_SET_CREATECHARA,			&CGameEventHandler::__evtCreateCharacter,				new PACKET_CHAR_CREATE_CHARA_REQ );
	regSendEvent( EVENT_CHANGE_CHARA_NICK,			&CGameEventHandler::__evtChangeCharacterName);
	regSendEvent( EVENT_EXIT_DAILYRECORD_SEND,		&CGameEventHandler::__evtDailyRecordInfo);
	regSendEvent( EVENT_REQ_INGAME_URL,				&CGameEventHandler::__evtRequestIngameURL);

	// 주특기
	regSendEvent( EVENT_SKILL_CHARA_SET,			&CGameEventHandler::__evtSkill_Characterset);
	regSendEvent( EVENT_SKILL_CLASS_SET,			&CGameEventHandler::__evtSkill_Classset);
	regSendEvent( EVENT_SKILL_SLOT_LEARN,			&CGameEventHandler::__evtSkill_Learn);

	// 메달
	regSendEvent( EVENT_MEDAL_GET_NORMAL_INFO,		&CGameEventHandler::__evtMedal_Normal_Info);
	regSendEvent( EVENT_MEDAL_GET_SET_CUR_INFO,		&CGameEventHandler::__evtMedal_Set_Cur_Info);
	regSendEvent( EVENT_MEDAL_GET_SET_COM_INFO,		&CGameEventHandler::__evtMedal_Set_Com_Info);
	regSendEvent( EVENT_MEDAL_GET_REWARD,			&CGameEventHandler::__evtMedal_Get_Reward);
	regSendEvent( EVENT_MEDAL_NOTIFY_REGISTER,		&CGameEventHandler::__evtMedal_Notify_Register);

	regSendEvent( EVENT_MEDAL_COMMAND,				&CGameEventHandler::__evtMedal_Command);

	regSendEvent( EVENT_GET_USERINFO_SESSION,		&CGameEventHandler::__evtFindSession_UserBasicInfo);
	regSendEvent( EVENT_GET_USERINFO_UID,			&CGameEventHandler::__evtFindUID_UserBasicInfo);
	regSendEvent( EVENT_GET_USERINFO_NICK,			&CGameEventHandler::__evtFindNick_UserBasicInfo);
	regSendEvent( EVENT_GET_USERINFO_ROOM,			&CGameEventHandler::__evtFindRoom_UserBasicInfo);
	regSendEvent( EVENT_GET_USER_DETAIL_INFO,		&CGameEventHandler::__evtFindUID_UserDetailInfo);
	regSendEvent( EVENT_GET_USERINFO_SUBTASK,		&CGameEventHandler::__evtFindSession_UserSubtaskInfo);

	regSendEvent( EVENT_GET_MYOPTION_N,				&CGameEventHandler::__evtGlobal_GetMyOption,			new  PACKET_BASE_GET_OPTION_REQ );
	regSendEvent( EVENT_N_QUEST_END_NOTICE,			&CGameEventHandler::__evtGlobal_EndQuest);
	regSendEvent( EVENT_OPTION_SAVE_N,				&CGameEventHandler::__evtSaveOption);
	//regSendEvent( EVENT_OPTION_SAVE_MACRO_N,		&CGameEventHandler::__evtSaveOptionMacro);
	regSendEvent( EVENT_CHATTING_N,					&CGameEventHandler::__evtGlobal_Chat);

	// 유저 가이드
	regSendEvent( EVENT_USER_GUIDE,					&CGameEventHandler::__evtUserGuide);
	regSendEvent( EVENT_DELETE_CHARA,				&CGameEventHandler::__evtDeleteChara);

	// GM 권한 팝업
	regSendEvent( EVENT_GM_AUTHORITY_REQ				,&CGameEventHandler::__evtGMAuthority,				new PACKET_GMCHAT_START_CHAT_REQ);
	regSendEvent( EVENT_GM_AUTHORITY_CHAT_SEND			,&CGameEventHandler::__evtSendMessage,				new PACKET_GMCHAT_SEND_CHAT_REQ );
	regSendEvent( EVENT_GM_AUTHORITY_USER_PENALTY_APPLY	,&CGameEventHandler::__evtPenaltyApply,				new PACKET_GMCHAT_APPLY_PENALTY_REQ); 
	regSendEvent( EVENT_GM_AUTHORITY_POPUP_CLOSE		,&CGameEventHandler::__evtGMAuthority_Popup_Close);

	// GM 치트키
	regSendEvent( EVENT_GM_CHEATKEY_RANDOMBOX,			&CGameEventHandler::__evtCheat_RandomBox);

	// QA자동화 커맨드
	regSendEvent( EVENT_QA_COMMAND_INCREASE_KILL_COUNT,	&CGameEventHandler::__evtQACommandIncreaseKillCount);
	regSendEvent( EVENT_QA_COMMAND_INCREASE_PLAY_TIME,	&CGameEventHandler::__evtQACommandIncreasePlayTime);
	regSendEvent( EVENT_QA_COMMAND_ENABLE_SOLO_PLAY,	&CGameEventHandler::__evtQACommandEnableSoloPlay);
	regSendEvent( EVENT_QA_COMMAND_TELEPORT_SOMEONE,	&CGameEventHandler::__evtQACommandTeleportSomeone);
	regSendEvent( EVENT_QA_COMMAND_DAMAGE_TO_OBJECT,	&CGameEventHandler::__evtQACommandDamageToObject);
	regSendEvent( EVENT_QA_COMMAND_DETECT_HACK_OFF,		&CGameEventHandler::__evtQACommandDetectHackOff);
	regSendEvent( EVENT_QA_COMMAND_TIMER_GMPAUSE,		&CGameEventHandler::__evtQACommandTimerGMPause);
	regSendEvent( EVENT_QA_COMMAND_CONTROL_ROUNDTIME,   &CGameEventHandler::__evtQACommandControlRoundTime);

	regSendEvent( EVENT_QA_COMMAND_CLAN_WAR_MATCHING_POINT,	&CGameEventHandler::__evtQACommandClanWarMatchingPoint);
	regSendEvent( EVENT_QA_COMMAND_CLAN_WAR_MATCH_RESULT,	&CGameEventHandler::__evtQACommandClanWarMatchResult);

	// 호칭 UI관련
	regSendEvent( EVENT_DESIGNATION_GET, &CGameEventHandler::__evt_EVENT_DESIGNATION_GET);
	regSendEvent( EVENT_DESIGNATION_CHANGE, &CGameEventHandler::__evt_EVENT_DESIGNATION_CHANGE);
	regSendEvent( EVENT_DESIGNATION_RELEASE, &CGameEventHandler::__evt_EVENT_DESIGNATION_RELEASE);

	// 미션 관련
	regSendEvent( EVENT_QUEST_CHANGE_ACTIVE_IDX, &CGameEventHandler::__evt_EVENT_QUEST_CHANGE_ACTIVE_IDX);

	//regSendEvent(EVENT_ABNORMAL_LOG, &CGameEventHandler::__evtLog_AbnormalExit);

	regSendEvent(EVENT_GET_STAGE, &CGameEventHandler::__evtGetStage);
}


bool CGameEventHandler::__evtLogin( GAMEEVENTINFO * pInfo)
{
	I3PRINTLOG(I3LOG_DEFALUT,  "* LOGIN 1 - start");

	GAME_LOGIN_INFO * pLoginInfo = GameLoginContext::i()->setLoginInfo();

	if( 0 == i3Net::GetMACAddress(&pLoginInfo->m_MacAddress, 1) )
	{
		// ! praptor - MACAddress를 얻지 못하였습니다.
		pLoginInfo->m_MacAddress = 0;
	}

	USER_INFO_BASIC	basicInfo;

	basicInfo.Reset();

	UserInfoContext::i()->SetMyUserInfoBasic(basicInfo);
	UserInfoContext::i()->SetMyUserInfoRecord()->Reset();
	

	// 런처에서 로그인 하는 국가는 아래 EVENT_LOG_IN_WEBKEY에서 처리합니다.
	// 여기서 낚이지 마세요 ㅠ.ㅠ
	PACKET_LOGIN_REQ* pSend = static_cast<PACKET_LOGIN_REQ*>(pInfo->m_S2MOPacket);

	// vv3 - job 양승촨

	if (g_pEnvSet->m_nUIVersion == UFV_1_5)
		pSend->m_ui8PBVer					= PB_VER_V15;
	else
		pSend->m_ui8PBVer					= PB_VER_V10;

	pSend->m_strID						= pLoginInfo->m_UserId;
	pSend->m_strPasswd					= pLoginInfo->m_UserPw;

	// Version
	SC_VERSION	ClientVersion;
	pSend->m_stVersion					= ClientVersion;

	// LocaleCode
	UINT8	ui8LocalNumber	= LOCALE_NUMBER;		// 국가 코드입니다. 255 개국 이상 나가면 사이즈 변경 생각해 봅시다.. 20101007 김현우
	pSend->m_ui8LocalCode				= ui8LocalNumber;
	pSend->m_i64MacAddress				= pLoginInfo->m_MacAddress;

#if defined(LOCALE_JAPAN)
	pSend->m_ui8LinkMethod = LINK_METHOD_JAPAN;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_JAPAN);
#elif defined(LOCALE_NORTHAMERICA)
	pSend->m_ui8LinkMethod = LINK_METHOD_NORTHAMERICA;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_NORTHAMERICA);
#elif defined(LOCALE_SINGMAL)
	pSend->m_ui8LinkMethod = LINK_METHOD_SM;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_SM);
#elif defined(LOCALE_PHILIPPINES)
	pSend->m_ui8LinkMethod = LINK_METHOD_PHILIPPINES;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_PHILIPPINES);
#elif defined(LOCALE_INDONESIA)
	pSend->m_ui8LinkMethod = LINK_METHOD_INDONESIA;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_INDONESIA);
#elif defined(LOCALE_RUSSIA)
	pSend->m_ui8LinkMethod = LINK_METHOD_RUSSIA;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_RUSSIA);
#elif defined(LOCALE_THAILAND)
	pSend->m_ui8LinkMethod = LINK_METHOD_THAI;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_THAI);
#elif defined(LOCALE_KOREA)
	pSend->m_ui8LinkMethod = LINK_METHOD_KOREA;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_KOREA);
#elif defined(LOCALE_TURKY)
	pSend->m_ui8LinkMethod = LINK_METHOD_TURKY;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_TURKY);
#elif defined(LOCALE_BRAZIL)
	pSend->m_ui8LinkMethod = LINK_METHOD_BRAZIL;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_BRAZIL);
#elif defined(LOCALE_LATINAMERICA)
	pSend->m_ui8LinkMethod = LINK_METHOD_LATINAMERICA;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_LATINAMERICA);
#elif defined(LOCALE_ITALY)
	pSend->m_ui8LinkMethod = LINK_METHOD_ITALY;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_ITALY);
#elif defined(LOCALE_CHINA)
	pSend->m_ui8LinkMethod = LINK_METHOD_CHINA;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_CHINA);
#elif defined(LOCALE_MIDDLEEAST)
	pSend->m_ui8LinkMethod = LINK_METHOD_MIDDLEEAST;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_MIDDLEEAST);
#elif defined(LOCALE_TAIWAN)
	pSend->m_ui8LinkMethod = LINK_METHOD_TAIWAN;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_TAIWAN);
#elif defined(LOCALE_VIETNAM)
	pSend->m_ui8LinkMethod = LINK_METHOD_VIETNAM;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_VIETNAM);
#else
	pSend->m_ui8LinkMethod = LINK_METHOD_NONE;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_NONE);
#endif

	// EncryptKey
	INT64	i64Key = _ttoi64(g_pFramework->GetCommandString());
	i64Key = GAMEEVENT::GetSecretKey( pLoginInfo->m_MacAddress );		// 인니 라이브에 붙을때 이 주석을 푼 후에 들어가시면 됩니다. 20100401 현우
	pSend->m_ui64EncryptKey				= i64Key;

	// MD5 Key
	unsigned char md5Key[16] = {};
	md5_file( ".\\UserFileList.dat", md5Key );
	UINT128 ui128Temp;
	char strMD5Key[33] = {};
	memcpy( &ui128Temp, md5Key, sizeof(char)*16 );
	i3MD5::GetMD5String( strMD5Key, ui128Temp, sizeof(strMD5Key));
	pSend->m_strMD5						= strMD5Key;

	//LS CHECK
	HACK_CHECK_LOGIN hcLogin;
	memset(&hcLogin, 1, sizeof(HACK_CHECK_LOGIN));
	pSend->m_stHackCheckLogin = hcLogin;
	//LS END


	//pSend->m_strNesiaPCbangKey			= Warnet_Premium::g_szRegKey;
	//pSend->m_dwNesiaPCbangHWKey			= 0;	
	
	//인도네시아 GWarnet 인증 시 사용 하는 MacAddress
	UINT64 ui64RealMacAddress = 0;
	if( CGameNetworkManager::i()->GetConnectedMacAddress( &ui64RealMacAddress) == false)
	{
		I3PRINTLOG(I3LOG_NOTICE, "[MAC ADDRESS] Can not Read Connected Mac Address.");
	}
	pSend->m_i64ActiveMacAddress		= ui64RealMacAddress;

	// 2016-11-02 User PC Specs 추가
	USER_INFO_PCSPECS tPcSpecInfo;
	GetCurrentPcSpecs(g_pD3D, tPcSpecInfo);
	pSend->m_stPCSpec = tPcSpecInfo;

	I3PRINTLOG(I3LOG_DEFALUT,  "* LOGIN 1 - end");

	return true;
}

bool CGameEventHandler::__evtLoginWebkey( GAMEEVENTINFO * pInfo)
{
	// MacAddress는 환경변수로 넘어오는 것을 받아서 쓴다

	I3PRINTLOG(I3LOG_DEFALUT,  " * LOGIN WEB - start");
	SC_VERSION	ClientVersion;
	UINT8		ui8LocalNumber	= LOCALE_NUMBER;		// 국가 코드입니다. 255 개국 이상 나가면 사이즈 변경 생각해 봅시다.. 20101007 김현우
	const GAME_LOGIN_INFO * pLoginInfo = GameLoginContext::i()->getLoginInfo();

	PACKET_LOGIN_REQ* pSend = static_cast<PACKET_LOGIN_REQ*>(pInfo->m_S2MOPacket);

	if (g_pEnvSet->m_nUIVersion == UFV_1_5)
		pSend->m_ui8PBVer = PB_VER_V15;
	else
		pSend->m_ui8PBVer = PB_VER_V10;

#if defined(LOCALE_JAPAN)
	pSend->m_ui8LinkMethod = LINK_METHOD_JAPAN;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_JAPAN);
#elif defined(LOCALE_NORTHAMERICA)
	pSend->m_ui8LinkMethod = LINK_METHOD_NORTHAMERICA;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_NORTHAMERICA);
#elif defined(LOCALE_SINGMAL)
	pSend->m_ui8LinkMethod = LINK_METHOD_SM;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_SM);
#elif defined(LOCALE_PHILIPPINES)
	pSend->m_ui8LinkMethod = LINK_METHOD_PHILIPPINES;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_PHILIPPINES);
#elif defined(LOCALE_INDONESIA)
	pSend->m_ui8LinkMethod = LINK_METHOD_INDONESIA;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_INDONESIA);
#elif defined(LOCALE_RUSSIA)
	pSend->m_ui8LinkMethod = LINK_METHOD_RUSSIA;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_RUSSIA);
#elif defined(LOCALE_THAILAND)
	pSend->m_ui8LinkMethod = LINK_METHOD_THAI;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_THAI);
#elif defined(LOCALE_KOREA)
	pSend->m_ui8LinkMethod = LINK_METHOD_KOREA;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_KOREA);
#elif defined(LOCALE_TURKY)
	pSend->m_ui8LinkMethod = LINK_METHOD_TURKY;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_TURKY);
#elif defined(LOCALE_BRAZIL)
	pSend->m_ui8LinkMethod = LINK_METHOD_BRAZIL;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_BRAZIL);
#elif defined(LOCALE_LATINAMERICA)
	pSend->m_ui8LinkMethod = LINK_METHOD_LATINAMERICA;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_LATINAMERICA);
#elif defined(LOCALE_ITALY)
	pSend->m_ui8LinkMethod = LINK_METHOD_ITALY;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_ITALY);
#elif defined(LOCALE_CHINA)
	pSend->m_ui8LinkMethod = LINK_METHOD_CHINA;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_CHINA);
#elif defined(LOCALE_MIDDLEEAST)
	pSend->m_ui8LinkMethod = LINK_METHOD_MIDDLEEAST;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_MIDDLEEAST);
#elif defined(LOCALE_TAIWAN)
	pSend->m_ui8LinkMethod = LINK_METHOD_TAIWAN;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_TAIWAN);
#elif defined(LOCALE_VIETNAM)
	pSend->m_ui8LinkMethod = LINK_METHOD_VIETNAM;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_VIETNAM);
#else
	pSend->m_ui8LinkMethod = LINK_METHOD_NONE;
	GameLoginContext::i()->SetLinkMethod(LINK_METHOD_NONE);
#endif
	
#if defined(LOCALE_RUSSIA)
	{
		pSend->m_strID						= GameLoginContext::i()->GetID_ForRussiaWebLogin();
		pSend->m_strPasswd					= GameLoginContext::i()->GetPass_ForRussiaWebLogin();
	}

#elif defined( LOCALE_LATIN_AMERICA )
	{
		pSend->m_strID						= GameLoginContext::i()->getUserID();
		pSend->m_strPasswd					= GameLoginContext::i()->getUserPW();
	}
#elif defined( LOCALE_TAIWAN)
	{	// 1.0 code must change 
		pSend->m_strID						= m_GameLoginInfo.m_UserId;
		pSend->m_strPasswd					= m_GameLoginInfo.m_UserPw;
	}
#elif defined(LOCALE_SINGMAL) || defined (LOCALE_PHILIPPINES) || defined(LOCALE_KOREA) || defined(LOCALE_VIETNAM)
	{
		INT16 tokenSize = (INT16)i3::generic_string_size(pLoginInfo->m_szGarenaToken);
		pSend->m_pSessionKey.SetValue( pLoginInfo->m_szGarenaToken, 0, tokenSize );

	}
#elif defined(LOCALE_JAPAN)
	{
		UINT16 ui16SessionKeySize = static_cast<UINT16>(i3::generic_string_size(pLoginInfo->m_szArarioSessionKey));
		pSend->m_pSessionKey.SetValue(pLoginInfo->m_szArarioSessionKey, 0, ui16SessionKeySize);

		pSend->m_ui8LinkMethod = LINK_METHOD_JAPAN;
		GameLoginContext::i()->SetLinkMethod(LINK_METHOD_JAPAN);
	}
#else
	{
		if (SteamContext::i()->IsSteamUser() == false)
		{
			char szSessionKey[NET_SESSION_KEY_SIZE] = { 0 };

			UINT16 ui16SessionKeySize = NET_SESSION_KEY_SIZE;
			memcpy(szSessionKey, pLoginInfo->m_SessionKey, ui16SessionKeySize);

			pSend->m_pSessionKey.SetValue(szSessionKey, 0, ui16SessionKeySize);
		}
	}
#endif
	pSend->m_stVersion					= ClientVersion;
	pSend->m_ui8LocalCode				= ui8LocalNumber;
	pSend->m_i64MacAddress				= pLoginInfo->m_MacAddress;


	// Steam빌드의 경우, Locale_Code와 무관하게 로그인 처리가 되어야 하므로
	// 모든 국가 로그인 세팅이 끝난 후 로그인 프로세스 진행합니다.
	char rgchToken[1024] = { 0, };
	uint32 unTokenLen = 0;

	if (SteamContext::i()->IsSteamUser())
	{
		// Steam으로부터 세션키 값을 받아옵니다.
		if (SteamContext::i()->GetSteamUser()->GetEncryptedAppTicket(rgchToken, sizeof(rgchToken), &unTokenLen))
		{
			pSend->m_pSessionKey.SetValue(rgchToken, 0, (UINT16)unTokenLen);
		}
		else
		{
			// 암호화 콜백을 받고서도 로그인 세션키 받아오는데 실패한 경우
			// 클라이언트 종료합니다.
			I3TRACE("[STEAM] Login ERROR. sk");
			GameEventSender::i()->SetEventOnce(EVENT_GAME_END);
			g_pFramework->GotoNextStage("Exit");
			return false;
		}

		pSend->m_ui8LinkMethod = LINK_METHOD_NORTHA_STEAM;
		GameLoginContext::i()->SetLinkMethod(LINK_METHOD_NORTHA_STEAM);
	}

	//LS CHECK
	HACK_CHECK_LOGIN hcLogin;
	memset(&hcLogin, 1, sizeof(HACK_CHECK_LOGIN));

	pSend->m_stHackCheckLogin = hcLogin;
	//LS END
	// 2016-11-02 User PC Specs 추가
	USER_INFO_PCSPECS tPcSpecInfo;
	GetCurrentPcSpecs(g_pD3D, tPcSpecInfo);
	pSend->m_stPCSpec = tPcSpecInfo;

	I3PRINTLOG(I3LOG_DEFALUT,  " * LOGIN WEB - end");
	return true;
}

bool CGameEventHandler::__evtLogout( GAMEEVENTINFO * pInfo)
{
	//if( CCharaInfoContext::i() )
	//	logoutReq.m_ui8CharaSlotIdx = CCharaInfoContext::i()->GetMainCharaSlotIdx();

	//PACKET_BASE_LOGOUT_REQ* pSend = static_cast<PACKET_BASE_LOGOUT_REQ*>(pInfo->m_S2MOPacket);

	if( g_pFramework->GetUIMainframe() )
		g_pFramework->GetUIMainframe()->SetExitTimer( true );
	return true;
}

bool CGameEventHandler::__evtDisconnect( GAMEEVENTINFO * pInfo)
{
	CGameNetworkManager::i()->DisconnectTCP();
	return false;
}

bool CGameEventHandler::__evtCheckRunParam( GAMEEVENTINFO * pInfo)
{
#if ! defined (I3_DEBUG) && ! defined (BUILD_RELEASE_QA_VERSION)
	#if defined (LOCALE_INDONESIA)
		#if 0
			const char * RUN_PARAMETER = "we.hate.hacker!";	//#define RUN_PARAMETER2	"we.hate.hacker!"

			// 실행시 커맨트 인자를 체크합니다.
			// 같지 않다면 서버에 알립니다.
			if ( 0 != i3String::CompareCase(RUN_PARAMETER, g_pFramework->GetCommandString()) )
			{
				bSendMessage = true;
				SendPacket.SetProtocol( PROTOCOL_BASE_NOT_USE_UPDATE_SERVER );
			}
		#endif
	#endif
#endif
	return false;
}

bool CGameEventHandler::__evtRequestChannelList( GAMEEVENTINFO * pInfo)
{
	DEV_LOG( "* PROTOCOL_BASE_GET_CHANNELLIST_REQ");
	ChannelContext::i()->Reset();

	PACKET_BASE_GET_CHANNELLIST_REQ* pSend	= static_cast<PACKET_BASE_GET_CHANNELLIST_REQ*>(pInfo->m_S2MOPacket);
	pSend->m_i32ServerIdx					= *(reinterpret_cast<const INT32*>(pInfo->_pArg));
	
	return true;
}

bool CGameEventHandler::__evtSelectChannel( GAMEEVENTINFO * pInfo)
{
	I3ASSERT( pInfo->_pArg != nullptr);

	PACKET_SELECT_CHANNEL_REQ* pSend	= static_cast<PACKET_SELECT_CHANNEL_REQ*>(pInfo->m_S2MOPacket);
	pSend->m_ui16ChannelIdx				= *(reinterpret_cast<const UINT16*>(pInfo->_pArg));
	g_pServerInfo->SetCurChannelNumber( pSend->m_ui16ChannelIdx );
	DEV_LOG("  PROTOCOL_BASE_SELECT_CHANNEL_REQ");

	return true;
}

bool CGameEventHandler::__evtEnterLobby( GAMEEVENTINFO * pInfo)
{
	LobbyContext::i()->InitLobbyData();

	pInfo->_SendPacket.SetProtocol( PROTOCOL_LOBBY_ENTER_REQ);

	UINT8 ui8ScopeType = UINT8(g_pEnvSet->m_nCrossHairType);
	pInfo->_SendPacket.WriteData(&ui8ScopeType, sizeof(UINT8));
	return true;
}

bool CGameEventHandler::__evtLeaveLobby( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_LOBBY_LEAVE_REQ);
	GameEventSender::i()->SetEvent(EVENT_REQ_INGAME_URL);

	return true;
}



bool CGameEventHandler::__evtEnterMyProfile( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_ROOM_INFO_ENTER_REQ);
	return true;
}

bool CGameEventHandler::__evtLeaveMyProfile( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_ROOM_INFO_LEAVE_REQ);
	return true;
}

bool CGameEventHandler::__evtEnterShop( GAMEEVENTINFO * pInfo)
{
	char strMD5Key[NET_MD5_KEY_SIZE+1];

	CShop::i()->ShopVerifyReset();	// revision 54146

	i3::safe_string_copy( strMD5Key, CShop::i()->GetShopMD5Key(), NET_MD5_KEY_SIZE+1 );
	strMD5Key[NET_MD5_KEY_SIZE] = 0;
	
	pInfo->_SendPacket.SetProtocol(PROTOCOL_SHOP_ENTER_REQ);
	pInfo->_SendPacket.WriteData( strMD5Key,		sizeof(char) * NET_MD5_KEY_SIZE ); 

	return true;
}

bool CGameEventHandler::__evtLeaveShop( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_SHOP_LEAVE_REQ);

	// 쭈니형을 위한 착한 코드 ♥ -> 난 나쁜남자임 ㅡㅡ
	/*PACKET_SHOP_LEAVE_REQ ReqPacket;
	for( INT32 idx = 0 ; idx < TEAM_COUNT ; ++idx)
		ReqPacket.m_ui8TeamSlot[idx] = CCharaInfoContext::i()->GetMainCharaSlotIdx( static_cast<TEAM_TYPE>(idx));

	ReqPacket.m_bCommonEquipFlag = false; 
	ReqPacket.m_ui8ChangeSlotCount = 0;

	pInfo->_SendPacket.WriteData( &ReqPacket, sizeof(PACKET_SHOP_LEAVE_REQ) );*/

	return true;
}

bool CGameEventHandler::__evtConnectGatewayServer( GAMEEVENTINFO * pInfo)
{
	DEV_LOG("EVENT_CONNECT_SERVER_GATEWAY");
	CGameNetworkManager::i()->DisconnectReserve();

	char IP[MAX_PATH] = {};
	i3::safe_string_copy(IP, g_pServerInfo->GetLogInServerIPStr(), MAX_PATH);

	if( CGameNetworkManager::i()->ConnectReserveToGateway( IP, g_pServerInfo->GetLogInServerPort()) == false)
	{
		I3TRACE0("EVENT_CONNECT_SERVER_GATEWAY, EVENT_ERROR_FAIL\n");
		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CONNECT_SERVER_GATEWAY, EVENT_ERROR_FAIL );
	}

	return false;
}

bool CGameEventHandler::__evtConnectGameServer( GAMEEVENTINFO * pInfo)
{
	DEV_LOG("EVENT_CONNECT_SERVER_GAME");
	CGameNetworkManager::i()->DisconnectReserve();

	char *	IP		= nullptr;
	UINT16	Port	= 0;

	// 0이면 게임서버로 접속하는것으로 간주..
	const CServerInfo *	pServerInfo = g_pServerInfo->GetCurGameServer();
	if ( pServerInfo )
	{
		IP		= (char*)pServerInfo->GetIPStr();
		Port	= pServerInfo->GetPort();
	}

	if( CGameNetworkManager::i()->ConnectReserveToGame( IP, Port) == false)
	{
		I3TRACE0("EVENT_CONNECT_SERVER_GAME, EVENT_ERROR_FAIL\n");
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CONNECT_SERVER_GAME, EVENT_ERROR_FAIL );
	}

	return false;
}

bool CGameEventHandler::__evtChangeMainCharacterFromRoom( GAMEEVENTINFO * pInfo)
{
	PACKET_BASE_NEW_CHARA_SHIFT_POS_REQ writePacket;

	writePacket.m_ui8CharaSlotIdx[TEAM_RED] = (UINT8)(pInfo->_pArg);
	//writePacket.m_ui8CharaSlotIdx[TEAM_BLUE] = (UINT8)(pInfo->_pArg);
	pInfo->_SendPacket.WriteData( &writePacket, sizeof( writePacket));
	pInfo->_SendPacket.SetProtocol( PROTOCOL_ROOM_CHARA_SHIFT_POS_REQ);
	return true;
}

bool CGameEventHandler::__evtGlobal_TimeoutHost( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_BATTLE_TIMEOUTMAINCLIENT_REQ );
	return true;
}

bool CGameEventHandler::__evtGlobal_TimeoutClient( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_BATTLE_TIMEOUTCLIENT_REQ );
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof( INT32 ) );
	return true;
}

bool CGameEventHandler::__evtGlobal_GameGuardAuth2( GAMEEVENTINFO * pInfo)
{
#if defined( _NPROTECT_)
	SC_VERSION	ClientVersion;
	pInfo->_SendPacket.SetProtocol( PROTOCOL_BASE_GAMEGUARD_ACK );

	pInfo->_SendPacket.WriteData( &ClientVersion,	sizeof(SC_VERSION) );
	pInfo->_SendPacket.WriteData( pInfo->_pArg,		sizeof(GG_AUTH_DATA) );
	return true;
#else
	return false;
#endif
}

bool CGameEventHandler::__evtGlobal_GameGuardAuth3( GAMEEVENTINFO * pInfo)
{
#if defined( _NPROTECT_)

	I3PRINTLOG(I3LOG_DEFALUT,  "[nProtect] : 4" ); //서버로 인증 받은 결과 보고

	SC_VERSION	ClientVersion;

	pInfo->_SendPacket.SetProtocol( PROTOCOL_BASE_GAMEGUARD_ACK );
	pInfo->_SendPacket.WriteData( &ClientVersion,	sizeof(SC_VERSION) );

	PCSAuth3Data pCSAuth3 = (PCSAuth3Data) pInfo->_pArg;
	if( pInfo->_pArg != false )
	{
		pInfo->_SendPacket.WriteData( (PVOID)&(pCSAuth3->dwPacketSize), sizeof(UINT32) );
		pInfo->_SendPacket.WriteData( (PVOID)pCSAuth3->bPacket, (UINT16)pCSAuth3->dwPacketSize );
	}
	else
	{
		//CS 서버에서 인증 받은 결과가 잘못 되었음.
		I3PRINTLOG(I3LOG_DEFALUT,  "[nProtect-err] : 4" ); //서버로 인증 받은 결과 보고 실패
	}
	return true;
#else
	return false;
#endif
}

bool CGameEventHandler::__evtGlobal_XTrapAuth( GAMEEVENTINFO * pInfo)
{
#ifdef _XTRAP_
	const unsigned int* hackResult = reinterpret_cast<const unsigned int*>(pInfo->_pArg);
	pInfo->_SendPacket.SetProtocol(PROTOCOL_BASE_GAMEGUARD_FAIL_NOACK);
	pInfo->_SendPacket.WriteData(hackResult, sizeof(unsigned int));

	if (*hackResult == 0x00000001) // XTRAPCODE_STATUS_DETECT
	{
		I3PRINTLOG(I3LOG_DEFALUT, "[XTRAP] XTRAP_CALLBACK_XtrapCallbackProcedure : XTRAPCODE_STATUS_DETECT");
	}
	else if (*hackResult == 0x00000002) // XTRAPCODE_STATUS_FAIL
	{
		I3PRINTLOG(I3LOG_DEFALUT, "[XTRAP] XTRAP_CALLBACK_XtrapCallbackProcedure : XTRAPCODE_STATUS_FAIL");
	}
	else if (*hackResult == 0x00000003) // XTRAPCODE_STATUS_REFERRENCE
	{
		I3PRINTLOG(I3LOG_DEFALUT, "[XTRAP] XTRAP_CALLBACK_XtrapCallbackProcedure : XTRAPCODE_STATUS_REFERRENCE");
	}
	else if (*hackResult == 0x00000000) // XTRAPCODE_STATUS_UNDEFINED
	{
		I3PRINTLOG(I3LOG_DEFALUT, "[XTRAP] XTRAP_CALLBACK_XtrapCallbackProcedure : XTRAPCODE_STATUS_UNDEFINED");
	}
	else
	{
		I3PRINTLOG(I3LOG_DEFALUT, "[XTRAP] XTRAP_CALLBACK_XtrapCallbackProcedure : There's no xtrap_code");
	}

	return true;
#else
	return false;
#endif
}

bool CGameEventHandler::__evtGlobal_GetMyOption( GAMEEVENTINFO * pInfo)
{
	//PACKET_BASE_GET_OPTION_REQ* pSend = static_cast<PACKET_BASE_GET_OPTION_REQ*>(pInfo->m_S2MOPacket);
	
	return true;
}

bool CGameEventHandler::__evtGlobal_GetUserInfo( GAMEEVENTINFO * pInfo)
{
	return false;
}

bool CGameEventHandler::__evtLeaveUser( GAMEEVENTINFO * pInfo)
{
#if defined(CHECK_CONNECTION_STATE)
	if ( NetworkContext::i()->ConnectionState != CONNECTION_STATE_INVALID &&
		NetworkContext::i()->ConnectionState != CONNECTION_STATE_ENTERED_TO_SERVER )
	{
		SETEVENT_ERROR( "EVENT_USER_LEAVE",  NetworkContext::i()->ConnectionState, pInfo->_szFile, pInfo->_nLine);
		return false;
	}

	NetworkContext::i()->ConnectionState = CONNECTION_STATE_LEAVING_FROM_SERVER;
#endif

	NetworkContext::i()->SetEnteredServer(false);

	//PACKET_BASE_USER_LEAVE_REQ* pSend = static_cast<PACKET_BASE_USER_LEAVE_REQ*>(pInfo->m_S2MOPacket);

	DEV_LOG("  PROTOCOL_BASE_USER_LEAVE_REQ %d", CGameNetworkManager::i()->GetTCPSocket()->GetConnectionInfo().m_SessionIndex);

	return true;
}

bool CGameEventHandler::__evtEnterUser( GAMEEVENTINFO * pInfo)
{
#if defined(CHECK_CONNECTION_STATE)
	if ( NetworkContext::i()->ConnectionState != CONNECTION_STATE_INVALID &&
		NetworkContext::i()->ConnectionState != CONNECTION_STATE_LEAVED_FROM_SERVER )
	{
		SETEVENT_ERROR( "EVENT_USER_ENTER", NetworkContext::i()->ConnectionState, pInfo->_szFile, pInfo->_nLine);
		return false;
	}

	NetworkContext::i()->ConnectionState = CONNECTION_STATE_ENTERING_TO_SERVER;
#endif

	PACKET_BASE_USER_ENTER_REQ* pSend = static_cast<PACKET_BASE_USER_ENTER_REQ*>(pInfo->m_S2MOPacket);
	pSend->m_TUID	= NetworkContext::i()->UID;
	pSend->m_strID	= GameLoginContext::i()->getLoginInfo()->m_UserId;

	DEV_LOG("  PROTOCOL_BASE_USER_ENTER_REQ %d", CGameNetworkManager::i()->GetTCPSocket()->GetConnectionInfo().m_SessionIndex);

	return true;
}

bool CGameEventHandler::__evtEnterPassword( GAMEEVENTINFO * pInfo)
{
	if( CGameNetworkManager::i()->IsReserveConnected())
	{
		char strPass[ NET_SERVER_PW ];
		i3mem::Copy( strPass, pInfo->_pArg1, NET_SERVER_PW );
		PACKET_BASE_ENTER_PASS_REQ* pSend = static_cast<PACKET_BASE_ENTER_PASS_REQ*>(pInfo->m_S2MOPacket);
		pSend->m_strPass = strPass;

		DEV_LOG("  PROTOCOL_BASE_ENTER_PASS_REQ");

		return true;
	}

	return false;
}

bool CGameEventHandler::__evtSaveOption( GAMEEVENTINFO * pInfo)
{
	UINT32 change = EnvInfoContext::i()->GetNetEnvSetBasicChange();
	if( change != 0)
	{
		// 초기화가 아닐때, Basic(0x00000001)와 key(0x00000002)도 변경점이 없다면 패킷 전송 안함.
		if( false == ( 0x10000000 & change ) && false == (change & 0x00000001) && false == (change & 0x00000002) && false == (change & 0x00000004) )
		{
			return false;
		}

		pInfo->_SendPacket.SetProtocol( PROTOCOL_BASE_OPTION_SAVE_REQ );
		pInfo->_SendPacket.WriteData( &change, sizeof(UINT32) );
		
		if( false == ( 0x10000000 & change ) )
		{
			if( 0x00000001 & change )
			{
				pInfo->_SendPacket.WriteData( EnvInfoContext::i()->GetNetEnvSet(), sizeof(NET_ENV_SET) );
			}
			if( 0x00000002 & change )
			{
				pInfo->_SendPacket.WriteData( EnvInfoContext::i()->GetKeyMapArray(0), (sizeof(GAME_KEY_STRUCT) * GAME_KEY_MAP_COUNT) );
			}
			if( 0x00000004 & change )
			{
				UINT8 ui8MacroSize;
				
				for(INT32 i = 0; i < MACRO_MAX_COUNT; i++)
				{
					const i3::rc_wstring& wstrMacro = EnvInfoContext::i()->GetMacro(i);
					const i3::rc_wstring& strMacro = wstrMacro;			

					ui8MacroSize = (UINT8)strMacro.size() + 1;
					pInfo->_SendPacket.WriteData( &ui8MacroSize,					sizeof(UINT8) );
					pInfo->_SendPacket.WriteData( strMacro.c_str(), ui8MacroSize * sizeof(TTCHAR) );
				}
			}
		}

		return true;
	}

	return false;
}

bool CGameEventHandler::__evtGlobal_Chat( GAMEEVENTINFO * pInfo)
{
	UINT16 Temp16;
	CHAT_INFO * pChatInfo = (CHAT_INFO *) pInfo->_pArg;
	
	const i3::rc_wstring& strText = pChatInfo->getString();
	
	Temp16 = (UINT16)strText.size() + 1;

	I3ASSERT( Temp16 < 256);

	switch( pChatInfo->_Type )
	{
	case CHATTING_TYPE_MATCH:				pInfo->_SendPacket.SetProtocol( PROTOCOL_CLAN_WAR_TEAM_CHATTING_REQ );	break;
	case CHATTING_TYPE_CLAN:				pInfo->_SendPacket.SetProtocol( PROTOCOL_CS_CHATTING_REQ );				break;
	case CHATTING_TYPE_CLAN_MEMBER_PAGE:	pInfo->_SendPacket.SetProtocol( PROTOCOL_CS_PAGE_CHATTING_REQ );		break;
	default:								pInfo->_SendPacket.SetProtocol( PROTOCOL_BASE_CHATTING_REQ );			break;
	}

	pInfo->_SendPacket.WriteData(&pChatInfo->_Type,			sizeof(UINT16));		//Type 0 모두,
	pInfo->_SendPacket.WriteData(&Temp16,					sizeof(UINT16));		//String Length
	pInfo->_SendPacket.WriteData(strText.c_str(),	Temp16 * sizeof(TTCHAR));				//String
	return true;
}

bool CGameEventHandler::__evtGlobal_EndQuest( GAMEEVENTINFO * pInfo)
{
#if defined( CHAMPETITION_BUILD )
	return false;
#else
	pInfo->_SendPacket.SetProtocol( PROTOCOL_BASE_QUEST_FINISH_NOTICE_REQ );
	I3TRACE( "Context SendMessage ( EVENT_N_QUEST_END_NOTICE )\n" );
	return true;
#endif
}

bool CGameEventHandler::__evtBuyQuestCardset( GAMEEVENTINFO * pInfo)
{
	UINT8 cardSetId = *(UINT8*)pInfo->_pArg;

	pInfo->_SendPacket.SetProtocol(PROTOCOL_BASE_QUEST_BUY_CARD_SET_REQ);
	pInfo->_SendPacket.WriteData(&cardSetId, sizeof(UINT8));
	return true;
}

bool CGameEventHandler::__evtDeleteQuestCardset( GAMEEVENTINFO * pInfo)
{
	UINT8 userCardSetIndex = *(UINT8*)pInfo->_pArg;

	pInfo->_SendPacket.SetProtocol(PROTOCOL_BASE_QUEST_DELETE_CARD_SET_REQ);
	pInfo->_SendPacket.WriteData(&userCardSetIndex, sizeof(UINT8));
	return true;
}

//bool CGameEventHandler::__evtAttendanceInfo( GAMEEVENTINFO * pInfo)
//{
//	pInfo->_SendPacket.SetProtocol(PROTOCOL_BASE_ATTENDANCE_INFO_REQ);
//	return true;
//}

bool CGameEventHandler::__evtAttendanceCheck( GAMEEVENTINFO * pInfo)
{
	UINT32 ui32Version = *(UINT32*)pInfo->_pArg;
	UINT8 ui8EventType = *(UINT8*)pInfo->_pArg1;

	pInfo->_SendPacket.SetProtocol(PROTOCOL_BASE_ATTENDANCE_REQ);
	pInfo->_SendPacket.WriteData(&ui32Version, sizeof(UINT32));
	pInfo->_SendPacket.WriteData(&ui8EventType, sizeof(UINT8));
	return true;
}

bool CGameEventHandler::__evtAttendanceClear( GAMEEVENTINFO * pInfo)
{
	ATTENDANCE_INFO* pAttInfo = (ATTENDANCE_INFO*)pInfo->_pArg;

	UINT32 ui32Version = pAttInfo->m_ui32Version;
	UINT8 ui8ItemNum = *(UINT8*)pInfo->_pArg1;
	UINT8 ui8Type = pAttInfo->m_ui8AttendanceType;

	pInfo->_SendPacket.SetProtocol(PROTOCOL_BASE_ATTENDANCE_CLEAR_ITEM_REQ);
	pInfo->_SendPacket.WriteData(&ui32Version, sizeof(UINT32));
	pInfo->_SendPacket.WriteData(&ui8ItemNum, sizeof(UINT8));
	pInfo->_SendPacket.WriteData(&ui8Type, sizeof(UINT8));
	return true;
}

bool CGameEventHandler::__evtQuest_ChangeActive( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_BASE_QUEST_ACTIVE_IDX_CHANGE_REQ);
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof( QUEST_ACTIVE_INDEX));
	return true;
}

bool CGameEventHandler::__evtGM_KickUserFromRoom( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_ROOM_GM_KICK_USER_REQ );
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof( INT8 ));
	return true;
}

bool CGameEventHandler::__evtGM_ExitUserFromRoom( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_ROOM_GM_EXIT_USER_REQ );
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof( INT8 ));
	return true;
}

bool CGameEventHandler::__evtGM_BlockUserFromRoom( GAMEEVENTINFO * pInfo)
{
	// Slot Index
	pInfo->_SendPacket.SetProtocol( PROTOCOL_ROOM_GM_BLOCK_USER_REQ );
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof( INT8 ));

	// Comment
	const TTCHAR* szComment = (const TTCHAR*)pInfo->_pArg1;

	UINT8 ui8Length = (UINT8) i3::generic_string_size( szComment );
	pInfo->_SendPacket.WriteData( &ui8Length, sizeof(UINT8) );
	pInfo->_SendPacket.WriteData( pInfo->_pArg1, ui8Length * sizeof(TTCHAR) );

	return true;
}

bool CGameEventHandler::__evtGM_DestroyRoom( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_ROOM_GM_DESTROY_ROOM_REQ );
	return true;
}

bool CGameEventHandler::__evtGM_ExitUserFromLobby( GAMEEVENTINFO * pInfo)
{
	// Session Index
	pInfo->_SendPacket.SetProtocol( PROTOCOL_LOBBY_GM_EXIT_USER_REQ );
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof( INT32 ));

	// Nick Name
	const wchar_t* wszNick = static_cast<const wchar_t*>(pInfo->_pArg1);

	const wchar_t* szNick = wszNick;
	UINT8 ui8Length = (UINT8) i3::generic_string_size( wszNick );

	pInfo->_SendPacket.WriteData( &ui8Length, sizeof(UINT8) );
	pInfo->_SendPacket.WriteData( szNick, ui8Length * sizeof(TTCHAR));
	return true;
}

bool CGameEventHandler::__evtAuth_FCMInfo( GAMEEVENTINFO * pInfo)
{
	return false;
}

bool CGameEventHandler::__evtGuard_CheckStageCodename( GAMEEVENTINFO * pInfo)
{
	const char * pCodeName = static_cast<const char *>(pInfo->_pArg);
	INT8 Length = static_cast<INT8>(i3::generic_string_size(pCodeName) +1);	// nullptr 사이즈 포함
	pInfo->_SendPacket.SetProtocol(PROTOCOL_ROOM_LOADING_START_REQ);
	pInfo->_SendPacket.WriteData( &Length,		sizeof(Length) );
	pInfo->_SendPacket.WriteData( pCodeName,	Length );
	return true;
}

bool CGameEventHandler::__evtGuard_KickUser( GAMEEVENTINFO * pInfo)
{
	const INT32* pCheatContext = static_cast<const INT32 *>(pInfo->_pArg);

	pInfo->_SendPacket.SetProtocol(PROTOCOL_BASE_HACKING_CHECK_KICK_REQ);
	pInfo->_SendPacket.WriteData( pCheatContext,		sizeof( INT32 ) );
	return true;
}

bool CGameEventHandler::__evtUser_DetailInfo( GAMEEVENTINFO * pInfo)
{
	const INT32* pUserSession = static_cast<const INT32 *>(pInfo->_pArg);
	pInfo->_SendPacket.SetProtocol(PROTOCOL_LOBBY_VIEW_USER_ITEM_REQ);
	pInfo->_SendPacket.WriteData( pUserSession,			sizeof(INT32));
	return true;
}

bool CGameEventHandler::__evtGetUserInfo( GAMEEVENTINFO * pInfo)
{
	I3ASSERT( UserInfoContext::i()->GetReceivedMyInfo() == false );

	//PACKET_BASE_GET_USER_INFO_REQ* pSend = static_cast<PACKET_BASE_GET_USER_INFO_REQ*>(pInfo->m_S2MOPacket);	

	return true;
}

//bool CGameEventHandler::__evtGetCharaInfo( GAMEEVENTINFO * pInfo)
//{
//	I3ASSERT( UserInfoContext::i()->GetReceivedMyInfo() == false );
//	pInfo->_SendPacket.SetProtocol(PROTOCOL_GET_CHARA_INFO_REQ);
//	return true;
//}

bool CGameEventHandler::__evtGetInventoryInfo( GAMEEVENTINFO * pInfo)
{
	I3ASSERT( UserInfoContext::i()->GetReceivedMyInfo() == false );
	
	//PACKET_BASE_GET_INVEN_INFO_REQ* pSend = static_cast<PACKET_BASE_GET_INVEN_INFO_REQ*>(pInfo->m_S2MOPacket);	

	return true;
}

bool CGameEventHandler::__evtGetSystemInfo( GAMEEVENTINFO * pInfo)
{
	I3ASSERT( UserInfoContext::i()->GetReceivedMyInfo() == false );

	PACKET_BASE_GET_SYSTEM_INFO_REQ* pSend = static_cast<PACKET_BASE_GET_SYSTEM_INFO_REQ*>(pInfo->m_S2MOPacket);
	pSend->m_ui8LinkMethod = GameLoginContext::i()->GetLinkMethod();
	
	return true;
}

bool CGameEventHandler::__evtCreateCharacter( GAMEEVENTINFO * pInfo)
{
	PACKET_CHAR_CREATE_CHARA_REQ* pSend = static_cast<PACKET_CHAR_CREATE_CHARA_REQ*>(pInfo->m_S2MOPacket);
	const NEW_CREATE_CHARA* pChara = static_cast<const NEW_CREATE_CHARA*>(pInfo->_pArg);
	I3ASSERT(pChara != nullptr);
	
	pSend->m_ui8CreateType	= pChara->m_ui8BuyType;
	pSend->m_TGoodsID		= GET_GOODSID_FROM_ITEMID( pChara->m_TItemID );
	pSend->m_ui8Class		= pChara->m_ui8Class;

	wchar_t wstrCharaName[ NET_CHARA_NICK_NAME_SIZE ];
	i3::safe_string_copy( wstrCharaName, pChara->m_wstrCharaName, NET_CHARA_NICK_NAME_SIZE);
	pSend->m_strCharaName = wstrCharaName;

	return true;
}

bool CGameEventHandler::__evtChangeCharacterName( GAMEEVENTINFO * pInfo)
{
	if( CCharaInfoContext::i() )
	{
		if( true == CCharaInfoContext::i()->IsChangedNickName() )
		{
			PACKET_H_NEW_CHANGE_CHARA_NICK_GROUP_REQ changeCharaNcikHeader;
			PACKET_D_NEW_CHANGE_CHARA_NICK_GROUP changeCharaNickData[ S2MO_MULTI_SLOT_COUNT ];

			CCharaInfoContext::i()->GetChangeCharaNickNameData( &changeCharaNcikHeader, changeCharaNickData );

			pInfo->_SendPacket.SetProtocol( PROTOCOL_CHAR_CHANGE_CHARA_NICK_REQ );
			pInfo->_SendPacket.WriteData( &changeCharaNcikHeader, sizeof(PACKET_H_NEW_CHANGE_CHARA_NICK_GROUP_REQ) );
			pInfo->_SendPacket.WriteData( &changeCharaNickData, sizeof(PACKET_D_NEW_CHANGE_CHARA_NICK_GROUP) * changeCharaNcikHeader.m_ui8CharaSlotCount );
			return true;
		}
		else
		{
			GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CHANGE_CHARA_NICK );
		}
	}

	return false;
}

bool CGameEventHandler::__evtSkill_Characterset( GAMEEVENTINFO * pInfo)
{
	const INT32 * charaSlot = reinterpret_cast<const INT32 *>(pInfo->_pArg);
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_SKILL_CHARA_SET, *charaSlot);
	return false;
}

bool CGameEventHandler::__evtSkill_Classset( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_SKILL_CLASS_SET_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(PACKET_SKILL_CLASS_SET_REQ));
	return true;
}


bool CGameEventHandler::__evtSkill_Learn( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_SKILL_SET_REQ);
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof(PACKET_SKILL_SET_REQ));
	return true;
}


bool CGameEventHandler::__evtFindSession_UserBasicInfo( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_BASE_GET_USER_INFO_LOBBY_REQ);
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof(INT32));
	return true;
}

bool CGameEventHandler::__evtFindSession_UserSubtaskInfo( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_BASE_GET_USER_SUBTASK_REQ);
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof(INT32));
	return true;
}

bool CGameEventHandler::__evtFindUID_UserBasicInfo( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_BASE_GET_USER_INFO_UID_REQ);
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof(INT64));
	return true;
}

bool CGameEventHandler::__evtFindNick_UserBasicInfo( GAMEEVENTINFO * pInfo)
{

	const wchar_t* wszNick = static_cast<const wchar_t*>(pInfo->_pArg);
	
	TTCHAR szNick[NET_NICK_NAME_SIZE];
	::memset(szNick, 0, sizeof(szNick));

	i3::safe_string_copy(szNick, wszNick, NET_NICK_NAME_SIZE);

	pInfo->_SendPacket.SetProtocol(PROTOCOL_BASE_GET_USER_INFO_NICK_REQ);
	pInfo->_SendPacket.WriteData( szNick, NET_NICK_NAME_SIZE);
	return true;
}

bool CGameEventHandler::__evtFindRoom_UserBasicInfo( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_BASE_GET_USER_INFO_ROOM_REQ);
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof(INT32));
	return true;
}

bool CGameEventHandler::__evtFindUID_UserDetailInfo( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_BASE_FIND_UID_GET_USER_DETAIL_INFO_REQ);
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof(PACKET_BASE_FIND_UID_USER_DETAIL_INFO_REQ));
	return true;
}

bool CGameEventHandler::__evtMedal_Normal_Info( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_GET_MEDAL_INFO_REQ);
	return true;
}

bool CGameEventHandler::__evtMedal_Set_Cur_Info( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_GET_CUR_MEDAL_SET_INFO_REQ);
	return true;
}

bool CGameEventHandler::__evtMedal_Set_Com_Info( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_GET_COM_MEDAL_SET_INFO_REQ);
	return true;
}

bool CGameEventHandler::__evtMedal_Get_Reward( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_GET_MEDAL_REWARD_REQ);

	PACKET_H_GET_MEDAL_REWARD_REQ * pHeader = (PACKET_H_GET_MEDAL_REWARD_REQ *)pInfo->_pArg;
	MEDAL_REWARD * pData = (MEDAL_REWARD *)pInfo->_pArg1;

	pInfo->_SendPacket.WriteData( pHeader, sizeof(PACKET_H_GET_MEDAL_REWARD_REQ) );
	pInfo->_SendPacket.WriteData( pData, sizeof(MEDAL_REWARD) * pHeader->m_ui8Count );

	return true;
}

bool CGameEventHandler::__evtMedal_Notify_Register( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_SET_NOTIFY_MEDAL_REQ);
	NOTIFY_MEDAL * notifyInfo = (NOTIFY_MEDAL *)(pInfo->_pArg);
	PACKET_SET_NOTIFY_MEDAL_REQ	pPacketData;
	for(INT32 i=0; i<MAX_NOTIFY_MADAL_COUNT; i++)
	{
		if(notifyInfo[i].m_ui8MedalType == MEDAL_SET_TYPE_NORMAL)
			pPacketData.m_aNotifyMedal[i].m_ui8MedalType = MEDAL_TYPE_NORMAL;
		else
			pPacketData.m_aNotifyMedal[i].m_ui8MedalType = MEDAL_TYPE_SET;
		pPacketData.m_aNotifyMedal[i].m_ui16Idx		= notifyInfo[i].m_ui16Idx;
		pPacketData.m_aNotifyMedal[i].m_ui16Count	= 0;
	}
	pInfo->_SendPacket.WriteData( &pPacketData, sizeof(PACKET_SET_NOTIFY_MEDAL_REQ));
	return true;
}

bool CGameEventHandler::__evtMedal_Command( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CHEAT_MEDAL_COMMAND_REQ);
	MEDAL_COMMAND * pMedalCommand = (MEDAL_COMMAND *)(pInfo->_pArg);
	PACKET_MEDAL_COMMAND_REQ	pPacket;
	i3mem::Copy( &pPacket.m_MedalCommand, pMedalCommand, sizeof( MEDAL_COMMAND ) );
	pInfo->_SendPacket.WriteData( &pPacket, sizeof(PACKET_MEDAL_COMMAND_REQ));

	return true;
}

bool CGameEventHandler::__evtQACommandIncreaseKillCount( GAMEEVENTINFO * pInfo)
{
	const GAMEINFO_CHARACTER * _pCharaInfoList = BattleSlotContext::i()->getGameInfo_Chara(BattleSlotContext::i()->getMySlotIdx(), CHARA::BUFFER_USER);
	if( nullptr == _pCharaInfoList ) return true;

	UINT32 ui32WeaponID = _pCharaInfoList->_tNetData._tWeaponData.getWeaponID();

	pInfo->_SendPacket.SetProtocol( PROTOCOL_CHEAT_INCREASE_KILL_COUNT_REQ );
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof(INT32) );
	pInfo->_SendPacket.WriteData( &ui32WeaponID, sizeof(UINT32) );

	return true;
}

bool CGameEventHandler::__evtQACommandIncreasePlayTime( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_CHEAT_REDUCE_TS_EVENT_REQ );
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof(INT32) );

	return true;
}

bool CGameEventHandler::__evtQACommandEnableSoloPlay( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_CHEAT_PLAY_SOLO_REQ );

	return true;
}

bool CGameEventHandler::__evtQACommandTeleportSomeone( GAMEEVENTINFO * pInfo)
{
	QA_COMMAND_TELEPORT_DATA StructData;

	i3mem::Copy( &StructData, pInfo->_pArg, sizeof(QA_COMMAND_TELEPORT_DATA) );
	pInfo->_SendPacket.SetProtocol( PROTOCOL_CHEAT_CHARACTER_TELEPORT_REQ );
	pInfo->_SendPacket.WriteData( &StructData, sizeof(QA_COMMAND_TELEPORT_DATA) );

	return true;
}

bool CGameEventHandler::__evtQACommandDamageToObject( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_CHEAT_DAMAGE_GAME_OBJECT_REQ );
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof(INT32) );
	pInfo->_SendPacket.WriteData( pInfo->_pArg1, sizeof(INT32) );

	return true;
}

bool CGameEventHandler::__evtQACommandTimerGMPause( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_CHEAT_TIMER_GM_PAUSE_REQ );
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof(UINT32) );

	return true;
}

bool CGameEventHandler::__evtQACommandDetectHackOff( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_CHEAT_DETECT_HACK_OFF_REQ );

	return true;
}

bool CGameEventHandler::__evtQACommandControlRoundTime( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_CHEAT_REDUCE_ROUND_TIME_REQ );
	return true;
}

bool CGameEventHandler::__evtQACommandClanWarMatchingPoint( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_CHEAT_CLAN_WAR_MATCHING_POINT_REQ );
	return true;
}

bool CGameEventHandler::__evtQACommandClanWarMatchResult( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_CHEAT_CLAN_WAR_RESULT_REQ );
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof(TEAM_TYPE) );
	return true;
}

bool CGameEventHandler::__evtUserGuide(GAMEEVENTINFO * pInfo)
{
	PACKET_NEW_GUIDE_COMPLETE_REQ packet;

	pInfo->_SendPacket.SetProtocol(PROTOCOL_BASE_GUIDE_COMPLETE_REQ);
	packet.m_ui16GuideEventIdx = static_cast<UINT16>(*(UINT16*)pInfo->_pArg);
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof(PACKET_NEW_GUIDE_COMPLETE_REQ));

	return true;
}
//bool CGameEventHandler::__evtLog_AbnormalExit(GAMEEVENTINFO * pInfo)
//{
//	pInfo->_SendPacket.SetProtocol(PROTOCOL_BASE_CLIENT_END_TYPE_REQ);
//
//	//pInfo->_SendPacket.WriteData(pInfo->_pArg, 129);
//
//	return true;
//}

bool CGameEventHandler::__evtDeleteChara( GAMEEVENTINFO * pInfo)
{
	PACKET_AUTH_NEW_SHOP_DELETE_CHARA_ITEM_REQ pack;
	pack.m_ui8CharaSlotIdx = static_cast<UINT8>(*(UINT8*)pInfo->_pArg1);
	pack.m_i64WareDBIdx = (*(T_ItemDBIdx*)pInfo->_pArg);
	
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CHAR_DELETE_CHARA_REQ);
	pInfo->_SendPacket.WriteData(&pack.m_ui8CharaSlotIdx, sizeof(UINT8));

	return true;
}

bool CGameEventHandler::__evtCheat_RandomBox( GAMEEVENTINFO * pInfo )
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_CHEAT_ITEM_AUTH_REQ);
	//const INT32* ptr = static_cast<const INT32*>(pInfo->_pArg);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(INT32));	// WareDBIdx
	pInfo->_SendPacket.WriteData(pInfo->_pArg1, sizeof(INT32));	// LoopCount

	return true;
}

bool CGameEventHandler::__evtGMAuthority( GAMEEVENTINFO * pInfo)
{
	PACKET_GMCHAT_START_CHAT_REQ* pSend = static_cast<PACKET_GMCHAT_START_CHAT_REQ*>(pInfo->m_S2MOPacket);

	// GM 유저 관리 팝업 실행 정보를 서버에 전달(선택한 유저의 위치, 인덱스, 닉네임)
	const wchar_t* wszNick = static_cast<const wchar_t*>(pInfo->_pArg2);
	TTCHAR szUserNickName[NET_NICK_NAME_SIZE];
	::memset(szUserNickName, 0, sizeof(szUserNickName));

	i3::safe_string_copy(szUserNickName, wszNick, NET_NICK_NAME_SIZE);

	pInfo->_SendPacket.SetProtocol(PROTOCOL_GMCHAT_START_CHAT_REQ);

	UINT8 indexType =(*(UINT8*)pInfo->_pArg);
	INT32 userIndex = (*(INT32*)pInfo->_pArg1);

	/*pInfo->_SendPacket.WriteData(&indexType,		sizeof(UINT8));
	pInfo->_SendPacket.WriteData(&userIndex,		sizeof(INT32));
	pInfo->_SendPacket.WriteData(szUserNickName,	NET_NICK_NAME_SIZE * sizeof(TTCHAR));*/

	pSend->m_i32idx			= userIndex;
	pSend->m_ui8idxType		= indexType;
	pSend->m_strNickname	= szUserNickName;
	
	return true;
}

bool CGameEventHandler::__evtSendMessage( GAMEEVENTINFO * pInfo)
{
	PACKET_GMCHAT_SEND_CHAT_REQ* pSend = static_cast<PACKET_GMCHAT_SEND_CHAT_REQ*>(pInfo->m_S2MOPacket);

	UINT16 temp_len = (*(UINT16*)pInfo->_pArg1);
	i3::wliteral_range wstrChat( static_cast<const wchar_t*>(pInfo->_pArg2) 
								, static_cast<const wchar_t*>(pInfo->_pArg2) + temp_len );
		
	wchar_t strResult[MAX_CHATTING_COUNT];	i3::safe_string_copy(strResult, wstrChat, MAX_CHATTING_COUNT);

	T_UID _UID = (*(T_UID*)pInfo->_pArg);

	i3::rc_wstring wstrNickName = UserInfoContext::i()->GetMyNickName();

	TTCHAR _NickName[NET_NICK_NAME_SIZE];
	memset(_NickName, 0, sizeof(_NickName));

	i3::safe_string_copy(_NickName, wstrNickName, NET_NICK_NAME_SIZE);

	pSend->m_i64UID			= _UID;
	pSend->m_strNickname	= _NickName;
	pSend->m_strChatMessage = strResult;

	return true;
}

bool CGameEventHandler::__evtPenaltyApply( GAMEEVENTINFO * pInfo)
{
	PACKET_GMCHAT_APPLY_PENALTY_REQ* pSend = static_cast<PACKET_GMCHAT_APPLY_PENALTY_REQ*>(pInfo->m_S2MOPacket);


	pSend->m_i64UserUID = (*(T_UID*)pInfo->_pArg);
	pSend->m_ui8PenaltyType = (*(UINT8*)pInfo->_pArg1);;
	pSend->m_i32PenaltyAmount = (*(INT32*)pInfo->_pArg2);

	return true;
}

bool CGameEventHandler::__evtGMAuthority_Popup_Close( GAMEEVENTINFO * pInfo)
{
	// 유저 관리 팝업을 종료
	pInfo->_SendPacket.SetProtocol(PROTOCOL_GMCHAT_END_CHAT_REQ );

	T_UID _UserUID = (*(T_UID*)pInfo->_pArg);

	pInfo->_SendPacket.WriteData(&_UserUID,			sizeof(T_UID));

	return true;
}

bool CGameEventHandler::__evtDailyRecordInfo( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_BASE_DAILY_RECORD_REQ);

	return true;

}

bool	CGameEventHandler::__evt_EVENT_DESIGNATION_GET(GAMEEVENTINFO * pInfo)
{
	UINT8 getIdx = *(UINT8*)(pInfo->_pArg);

	DesigUtil::Update_Send_EVENT_DESIGNATION_GET(getIdx);

	pInfo->_SendPacket.SetProtocol(PROTOCOL_BASE_USER_TITLE_CHANGE_REQ);
	pInfo->_SendPacket.WriteData(&getIdx, sizeof(UINT8) );

	return true;
}

bool	CGameEventHandler::__evt_EVENT_DESIGNATION_CHANGE(GAMEEVENTINFO * pInfo)
{
	UINT8 nSlotIdx= *(UINT8*)(pInfo->_pArg);
	UINT8 nDesignationIdx= *(UINT8*)(pInfo->_pArg1);

	DesigUtil::Update_Send_EVENT_DESIGNATION_CHANGE(nSlotIdx, nDesignationIdx);

	pInfo->_SendPacket.SetProtocol(PROTOCOL_BASE_USER_TITLE_EQUIP_REQ);
	pInfo->_SendPacket.WriteData(&nSlotIdx, sizeof(UINT8));
	pInfo->_SendPacket.WriteData(&nDesignationIdx, sizeof(UINT8));

	return true;
}

bool	CGameEventHandler::__evt_EVENT_DESIGNATION_RELEASE(GAMEEVENTINFO * pInfo)
{
	UINT8 nSlotIdx= *(UINT8*)(pInfo->_pArg);

	UINT8 ReleaseTitle = 0; 
	
	DesigUtil::Update_Send_EVENT_DESIGNATION_RELEASE(nSlotIdx, ReleaseTitle);

	pInfo->_SendPacket.SetProtocol(PROTOCOL_BASE_USER_TITLE_RELEASE_REQ);
	pInfo->_SendPacket.WriteData(&nSlotIdx, sizeof(UINT8));
	pInfo->_SendPacket.WriteData(&ReleaseTitle, sizeof(UINT8));
	
	return true;
}

bool	CGameEventHandler::__evt_EVENT_QUEST_CHANGE_ACTIVE_IDX(GAMEEVENTINFO* pInfo)
{
	QUEST_ACTIVE_INDEX* pQuestActiveIdx = (QUEST_ACTIVE_INDEX*)pInfo->_pArg;
	pInfo->_SendPacket.SetProtocol( PROTOCOL_BASE_QUEST_ACTIVE_IDX_CHANGE_REQ);
	pInfo->_SendPacket.WriteData( pQuestActiveIdx, sizeof( QUEST_ACTIVE_INDEX));

	return true;
}

bool CGameEventHandler::__evtGetStage(GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_BASE_MAP_LIST_REQ);
	return true;
}


bool CGameEventHandler::__evtRequestIngameURL(GAMEEVENTINFO * pInfo)
{
	DEV_LOG("* PROTOCOL_BASE_URL_LIST_REQ");
	
	UINT8 URLVersion = GameLoginContext::i()->GetIngameURLVersion();
	pInfo->_SendPacket.SetProtocol(PROTOCOL_BASE_URL_LIST_REQ);
	pInfo->_SendPacket.WriteData(&URLVersion, sizeof(UINT8));

	return true;
}