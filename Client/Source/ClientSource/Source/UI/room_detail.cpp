#include "pch.h"
#include "room_detail.h"

#include "UIUtil.h"
#include "UIMainFrame.h"
#include "UIPhaseBase.h"
#include "UIPhaseReadyRoom.h"
#include "UIFloatCharacterView.h"

#include "UITopMenu.h"
#include "UISideMenu.h"

#include "WebControl.h"

#include "RandomMapContext.h"
#include "ClanGameContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "ChannelContext.h"
#include "ClanSeasonContext.h"
#include "UserInfoContext.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/mb_to_utf16.h"

#include "ClientStageInfoUtil.h"

void room_base::do_config_room()
{
	if( phase_ready_room->_IsHost())
	{
		bool isLobby = false;
		//GetMainFrame()->TogglePopup( UPW_SETTINGROOM);
		phase_ready_room->GetMainFrame()->TogglePopup( UPW_CREATEROOM_EX, &isLobby);
	}
}

void room_normal::update( REAL32 elapsed_time)
{
	m_rTeamChangeDelay = MAX(m_rTeamChangeDelay - elapsed_time, 0.0f);
}

void room_normal::do_ready_game()
{
	//레디 예외 처리
	if( (MyRoomInfoContext::i()->getStage()->IsAiMode()) )									//훈련전
	{
		if( phase_ready_room->_IsHost())
		{
			if( false == is_ready())
			{
				g_pFramework->PutSystemChatting(GAME_RCSTRING("STBL_IDX_EP_ROOM_NO_TRAINIG_TEAM_S"));
				return;
			}
		}
		else																	//1. 방장이 아닐때는 레디를 할때만 체크 합니다.
		{
			if( (BattleSlotContext::i()->getMySlotInfo()->_State) == SLOT_STATE_NORMAL)
			{
				if( (BattleSlotContext::i()->getMySlotIdx() % 2) != (BattleSlotContext::i()->getMainSlotIdx() % 2) )
				{
					g_pFramework->PutSystemChatting(GAME_RCSTRING("STBL_IDX_EP_ROOM_NO_TRAINIG_TEAM_S"));
					return;
				}
			}
		}
	}
	else																		//방장일때만 예외 처리 합니다.
	{
		if( phase_ready_room->_IsHost() && (false == is_ready()))
		{
			g_pFramework->PutSystemChatting(GAME_RCSTRING("STBL_IDX_EP_ROOM_NO_READY_TEAM_S"));
			return;
		}
	}

	// 클랜 채널 && 매치 메이킹 레디룸일 경우.
	if( ChannelContext::i()->GetBattleChannel() && (ClanGameContext::i()->getClanBattleLobbyEnable() == true) )
	{
		if( ClanSeasonContext::i()->IsPossibleSeasonMatch() == false )
		{
			GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_SEASON_END_NOTICE"), nullptr, MAKE_CALLBACK(callback_season_match), this );
			return;
		}
	}

	ready_process();
}

void room_normal::ready_process()
{
	if( MyRoomInfoContext::i()->IsRandomMap() && phase_ready_room->_IsHost())
		phase_ready_room->RunRandomMap();

	ROOM_INFO_BASIC	roomInfo;
	MyRoomInfoContext::i()->getMyRoomInfo(&roomInfo);

	if (roomInfo._State != ROOM_STATE_READY)
	{
		INT32 my_team_count = 0;

		INT32 slot_idx = BattleSlotContext::i()->getMySlotIdx() % 2;
		while (slot_idx < SLOT_MAX_COUNT)
		{
			const SLOT_INFO* pSlot = BattleSlotContext::i()->getSlotInfo(slot_idx);

			if (SLOT_STATE_READY <= pSlot->_State)
			{
				my_team_count++;
			}

			slot_idx += 2;
		}

		struct _tag_enter_limit
		{
			INT32		limit_count;
			i3::string	str_notice;
		};

		_tag_enter_limit limit[STAGE_SLOT_MODE_MAX] = {
			{ 8,	"STR_TBL_GUI_LOBBY_LIMIT_ENTER_ROOM" },			/*난입이 제한된 방입니다..*/
			{ 4,	"STR_TBL_GUI_READY_NOTICE_LIMIT_ENTER_4VS4" },	/*4vs4 모드 인원수 제한으로 난입할 수 없습니다.*/
			{ 5,	"STR_TBL_GUI_READY_NOTICE_LIMIT_ENTER_5VS5" }	/*5vs5 모드 인원수 제한으로 난입할 수 없습니다.*/
		};

		STAGE_SLOT_MODE eMode = CStageInfoUtil::GetSlotMode(*CStageInfoUtil::GetStageData(roomInfo._StageID));

		if (limit[eMode].limit_count <= my_team_count)
		{
			g_pFramework->PutSystemChatting(GAME_RCSTRING(limit[eMode].str_notice));
			return;
		}
	}

	//장비 변경 교체 한다.
	UISideMenu::instance()->send_change_equip();

	bool bEventSent = GameEventSender::i()->SetEventOnce( EVENT_READY_BATTLE )?true:false;

	bool bStartBtnEnable = false;

	if( phase_ready_room->_IsHost())									// 방장 or 게임중일때는 Ready Btn이 아닌 Start Btn 을 사용한다.
		bStartBtnEnable = true;
	else
	{
		for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
		{
			const SLOT_INFO * pSlot = BattleSlotContext::i()->getSlotInfo(i);
			if ((SLOT_STATE_LOAD <= pSlot->_State) && (pSlot->_State<= SLOT_STATE_BATTLE))
			{
				bStartBtnEnable = true;
				break;
			}
		}
	}

	if(bStartBtnEnable == true && bEventSent)
		g_pFramework->GetUIMainframe()->GetTopMenu()->SetInputDisable(true);
}

bool room_normal::is_ready()
{
	if ( MyRoomInfoContext::i()->IsSoloPlayFlag() )
		return true;

	// 일반 모드는 false, AI전은 true로 초기화.
	bool bIsReady = (MyRoomInfoContext::i()->getStage()->IsAiMode()) ? true : false;

	// 상대팀 유저만 확인
	INT32 i32OtherTeam = (BattleSlotContext::i()->getMainSlotIdx() + 1) % 2;

	for(INT32 idx = i32OtherTeam ; idx < SLOT_MAX_COUNT; idx += 2)
	{
		const SLOT_INFO * pInfo = BattleSlotContext::i()->getSlotInfo(idx);

		// 상대팀에 레디 유저가 있을 때, 일반 모드는 true, AI전은 false 리턴.
		if(pInfo->_State == SLOT_STATE_READY)
		{
			bIsReady = !bIsReady;
			break;
		}
	}

	return bIsReady;
}

void room_normal::set_button_state()
{
	bool bStartBtnEnable = false;

	if( phase_ready_room->_IsHost())									// 방장 or 게임중일때는 Ready Btn이 아닌 Start Btn 을 사용한다.
		bStartBtnEnable = true;
	else
	{
		for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
		{
			const SLOT_INFO * pSlot = BattleSlotContext::i()->getSlotInfo(i);
			if ((SLOT_STATE_LOAD <= pSlot->_State) && (pSlot->_State<= SLOT_STATE_BATTLE))
			{
				bStartBtnEnable = true;
				break;
			}
		}
	}

	bool isInvite = true;
	if(ClanGameContext::i()->getClanBattleLobbyEnable() == true)
		isInvite = false;
	LuaState * L = phase_ready_room->_CallLuaFunction( "SetButtonState");
	if( L != nullptr)
	{
		i3Lua::PushBoolean(L , bStartBtnEnable);
		i3Lua::PushBoolean(L , phase_ready_room->_IsHost());
		i3Lua::PushBoolean(L , MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS));
		i3Lua::PushBoolean(L , isInvite);
		phase_ready_room->_EndLuaFunction( L, 4);
	}
}

bool room_normal::is_room_user( const i3::wliteral_range& wNickRng )
{
	if( phase_ready_room->GetMainFrame()->GetCurrentPhaseType() != UIPHASE_READY_ROOM) 
		return false;

	for( int i = 0; i < SLOT_ROOM_VIEW_COUNT; i++)
	{
		if( i < SLOT_MAX_COUNT)
		{
			const SLOT_INFO * pSlot = BattleSlotContext::i()->getSlotInfo(i);

			if (SLOT_STATE_CLOSE != pSlot->_State && SLOT_STATE_EMPTY != pSlot->_State)
			{
				i3::rc_wstring wstrUserNick = BattleSlotContext::i()->getNickForSlot(i);
				if( i3::generic_is_iequal( wstrUserNick, wNickRng )  ) return true;
			}
		}
	}
	return false;
}





void room_normal::process_update_slot()
{
	for( INT32 i = 0; i < SLOT_ROOM_VIEW_COUNT; i++)											// 슬롯 상태 확인
	{
		if( i < SLOT_MAX_COUNT)
		{
			const SLOT_INFO * pSlot = BattleSlotContext::i()->getSlotInfo(i);
			
			if (SLOT_STATE_CLOSE != pSlot->_State && SLOT_STATE_EMPTY != pSlot->_State)			// 슬롯에 누군가가 있다.
			{	
				close_slot( i, false);
				if( i3::generic_empty( BattleSlotContext::i()->getNickForSlot(i) ) == false ) //이름 없는 유저는 표시 안 한다.
					set_slot_info( i);
				else
					empty_slot( i);
			}
			else
			{
				if (SLOT_STATE_CLOSE == pSlot->_State)											// 닫혀있는 슬롯 처리
				{
					close_slot(i, true);
					empty_slot( i);
				}
				else
				{
					close_slot( i, false);
					empty_slot( i);
				}
			}
		}
		//else
		//{
		//	_CloseSlot( i, true, true);
		//}
	}
}

void room_normal::close_slot( INT32 slot, bool bClose, bool bLock /*= false*/)
{
	LuaState * L = phase_ready_room->_CallLuaFunction( "CloseSlot");
	if( L != nullptr)
	{
		i3Lua::PushInteger(L, slot);
		i3Lua::PushBoolean(L, bClose);
		i3Lua::PushBoolean(L, bLock || !(phase_ready_room->_IsHost()));
		phase_ready_room->_EndLuaFunction(L,3);
	}
}

void room_normal::empty_slot( INT32 slot)
{
	bool is_enable_close = (phase_ready_room->_IsHost() == true) && (phase_ready_room->IsBattleCountDown() == false);

	LuaState * L = phase_ready_room->_CallLuaFunction( "SetSlotInfo");

	if( L != nullptr)
	{
		i3Lua::PushInteger( L, slot);				// slot
		GameUI::SetClanMarkToLua( L, 0xFFFFFFFF);
		i3Lua::PushStringUTF16To8( L, L"");			// clanname tooltip
		i3Lua::PushInteger( L, -1);					// rank
		i3Lua::PushStringUTF16To8( L, L"");			// rank tooltip
		i3Lua::PushStringUTF16To8( L, L"");			// nick
		i3Lua::PushColor( L, nullptr);
		i3Lua::PushInteger( L, -1);
		i3Lua::PushInteger( L, -1);
		i3Lua::PushBoolean(L , is_enable_close);
		i3Lua::PushBoolean(L , BattleSlotContext::i()->getMainSlotIdx() == slot);
		i3Lua::PushInteger( L, -1);
		phase_ready_room->_EndLuaFunction( L, 19);
	}
}

void room_normal::set_slot_info( INT32 slot)
{
	const SLOT_INFO* pInfo = BattleSlotContext::i()->getSlotInfo( slot);

	if (pInfo == nullptr) return;

	// 상태
	UI_READYSTATE state = UIRS_NONE;

	switch( pInfo->_State)
	{
	case SLOT_STATE_EMPTY :								break;
	case SLOT_STATE_CLOSE :								break;
	case SLOT_STATE_SHOP :		state = UIRS_SETTING;	break;
	case SLOT_STATE_INFO :										// 유저 정보
	case SLOT_STATE_CLAN :		state = UIRS_SETTING;	break;	// 클랜
	case SLOT_STATE_INVENTORY :	state = UIRS_SETTING;	break;
	case SLOT_STATE_GIFTSHOP :	state = UIRS_SETTING;	break;	// 선물 상점	
	case SLOT_STATE_NORMAL :							break;
	case SLOT_STATE_READY :		state = UIRS_READY;		break;
	case SLOT_STATE_LOAD :										// (게임중) 로딩중입니다.			Time Out ( 2분)     처음시작 방장시작후 (10초)
	case SLOT_STATE_RENDEZVOUS :								// (게임중) 랑데뷰서버 홀펀칭		Time Out ( )
	case SLOT_STATE_PRESTART :									// (게임중) 홀펀칭					Time Out ( )
	case SLOT_STATE_BATTLE_READY :								// (게임중) 홀펀칭 완료후 게임 시작을 기다림 
	case SLOT_STATE_BATTLE :	state = UIRS_PLAY;		break;	// (게임중) 전투중입니다.						
	case SLOT_STATE_GACHA :		state = UIRS_SETTING;	break;	// 뽑기 상점
	}
	// 계급을 표시합니다.
	INT32 rank = 0;

	if( ChannelContext::i()->IsBeginnerChannel1() )
	{	
		rank = (INT32)(RANK_NEWBIE);
	}
	else
	{	
		rank = (INT32)(pInfo->_Rank);
	}
		// 클랜 매치메이킹
	if(ChannelContext::i()->GetBattleChannel() &&
		UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW) )
	{
		if(ClanGameContext::i()->GetMercRank(slot) != -1) // 값이 없는 경우는 용병이 아님
			rank = ClanGameContext::i()->GetMercRank(slot);
	}


	i3::rc_wstring wstrRankToolTip = GetCharaRank(rank);

	// 자기 슬롯 강조
	if (BattleSlotContext::i()->getMySlotIdx() == slot)
		effect_slot( slot);

	//	클랜 마크
	i3::stack_wstring wstrClanToolTip;

	if( pInfo->_clanidx > 0 && pInfo->_clanstate != CLAN_MEMBER_LEVEL_UNKNOWN)
	{	
		// 클랜마크 툴팁 ( 매치인 경우, 분대명으로 툴팁 생성)
		if( g_pFramework->GetClanContext()->InBattleTeam() )
		{
			i3::wstring wstrTeamName;
			TEAM_TYPE team_type = CHARA::CharaTeamType2TeamType( BattleSlotContext::i()->getTeam(slot) );

			const CLAN_MATCH_BATTLE_TEAM * pTeamInfo = ClanGameContext::i()->getClanBattleTeamInfo(team_type);

			GetBattleTeamName( wstrTeamName, pTeamInfo->getMatchTeamID() );
			i3::sprintf( wstrClanToolTip, L"%s %s", pTeamInfo->getClanName(), wstrTeamName);		// TODO : 자동 형변환되고 있음.(2014.07.18.수빈)
		}
		else
		{
			// 클랜등급 표시
			i3::rc_wstring wstrTemp; 
			GetClanRankPreName2( wstrTemp, pInfo->_clanExpRank);

			i3::sprintf( wstrClanToolTip, L"%s %s",wstrTemp, pInfo->_ClanName);		// TODO : 자동 형변환되고 있음.(2014.07.18.수빈)
		}
	}

	UI_EVENT_MARK pcState = UIEM_NONE;
	//// PC방 표시
	switch( pInfo->_pcCafe )
	{
	case PC_NOT_CAFE			: pcState = UIEM_NONE;			break;
	case PC_NORMAL_CAFE			: pcState = UIEM_PCCAFE;		break;
	case PC_PREMIUM_CAFE		: pcState = UIEM_PCCAFE_PLUS;	break;
	case PC_VACATION_EVENT_ITEM	: pcState = UIEM_NONE;			break;
	case PC_INDO_TELCOM_ITEM	: pcState = UIEM_NONE;			break; 
	case PC_GARENA_GCA			: pcState = UIEM_GARENA_GCA;	break;
	case 0xFF					: pcState = UIEM_NONE;			break;
	default						: pcState = UIEM_NONE;			I3ASSERT_0;	break;
	}

	//	유저 E-Sport 태그
	INT32 esports_index = GameUI::GetEsportsTemplateIndex( (ESPORT_ID_LEVEL)pInfo->_eSportID );

	const I3COLOR * pNickColor = GameUI::GetNickNameColor( BattleSlotContext::i()->getNickColorForSlot( slot));

	bool is_enable_close =  (phase_ready_room->_IsHost() == true) && (phase_ready_room->IsBattleCountDown() == false);

	LuaState * L = phase_ready_room->_CallLuaFunction( "SetSlotInfo");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, slot);											// slot
		GameUI::SetClanMarkToLua( L, pInfo->_clanMark);
		i3Lua::PushStringUTF16To8( L, wstrClanToolTip);								// clanname tooltip
		i3Lua::PushInteger( L, rank);											// rank
		i3Lua::PushStringUTF16To8( L, wstrRankToolTip);								// rank tooltip
		i3Lua::PushStringUTF16To8( L, BattleSlotContext::i()->getNickForSlot( slot));		// nick
		i3Lua::PushColor( L, pNickColor);
		i3Lua::PushInteger( L, pcState);
		i3Lua::PushInteger( L, state);
		i3Lua::PushBoolean(L , is_enable_close);
		i3Lua::PushBoolean(L , BattleSlotContext::i()->getMainSlotIdx() == slot);
		i3Lua::PushInteger( L, esports_index);									// Esports Mark
		phase_ready_room->_EndLuaFunction( L, 19);
	}
}

void room_normal::effect_slot( INT32 slot)
{
	LuaState* L = phase_ready_room->_CallLuaFunction( "SetEffectSlot");
	if( L != nullptr)
	{
		i3Lua::PushInteger(L, slot);
		phase_ready_room->_EndLuaFunction(L,1);
	}
}

bool room_normal::is_alone()
{
	INT32 nPersonNow = 0;
	for( INT32 nSlot = 0; nSlot < SLOT_MAX_COUNT; ++nSlot)
	{
		const SLOT_INFO * pSlot = BattleSlotContext::i()->getSlotInfo(nSlot);
		if( !pSlot ) continue;
		if( SLOT_STATE_CLOSE != pSlot->_State && SLOT_STATE_EMPTY != pSlot->_State)
		{
			++nPersonNow;
		}
		if( nPersonNow > 1 )
			return false;
	}
	return true;
}

void room_normal::change_team( INT32 team)
{
	if( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) )
		return;

	INT32 idxMySlot = BattleSlotContext::i()->getMySlotIdx();
	if(idxMySlot < 0 || idxMySlot >= USER_SLOT_COUNT)		return;
	const SLOT_INFO* pInfo = BattleSlotContext::i()->getSlotInfo(idxMySlot);
	if(pInfo == nullptr)	return;
	
	if( team == idxMySlot % 2)								// 현재 팀과 같으면 이동할 필요없다
		return;

	if( SLOT_STATE_READY == pInfo->_State)					// Ready 중엔 팀 변경 금지
	{
		((CGameFramework*)g_pFramework)->PutSystemChatting(GAME_RCSTRING("STBL_IDX_EP_ROOM_NO_CHANGE_SLOT_S"));
		return;
	}
	GameEventSender::i()->SetEvent( EVENT_CHANGE_TEAM_N , &team);
}

void room_normal::do_swap_team()
{
	if( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) )
		return;

	if (0.0f < m_rTeamChangeDelay)
		return;

	m_rTeamChangeDelay = 2.0f;								// 2초간 기다린다.

	if( phase_ready_room->_IsHost())
		GameEventSender::i()->SetEvent(EVENT_TEAM_CHANGE);
}

void room_normal::add_all_ramdom_map()
{
	//ROOM_INFO_BASIC Info;
	//MyRoomInfoContext::i()->getMyRoomInfo(&Info);

	//i3::vector<STAGE_ID> MapList;
	//STAGE_OPTION_TYPE option = STAGE_OPTION_NONE;

	//if (MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_CHALLENGE))		option = STAGE_OPTION_AI;
	//else if (MyRoomInfoContext::i()->getStage()->IsWeaponModeforWType(WEAPON_KNUCKLE_ONLY))		option = STAGE_OPTION_KNUCKLE;
	//else if (MyRoomInfoContext::i()->getStage()->IsWeaponModeforWType(WEAPON_SNIPER_ONLY))		option = STAGE_OPTION_SNIPER;
	//else if( MyRoomInfoContext::i()->getStage()->IsWeaponModeforWType(WEAPON_SHOTGUN_ONLY))		option = STAGE_OPTION_SHOTGUN;

	//GameUI::GetEnableMapList( MapList, (STAGE_ID)Info._StageID, option);	

	//ROOM_INFO_ADD addInfo;
	//MyRoomInfoContext::i()->getMyRoomInfoAdd(&addInfo);

	//RandomMapContext::i()->ClearStageList();

	//for(size_t idx = 0 ; idx < MapList.size() ; idx++)
	//{
	//	STAGE_ID StageID = MapList[idx];
	//	STAGE_ORDINAL_ID ord = getStageOrd(StageID);
	//	//RandomMapContext::i()->AddRandomMap(ord);

	//	if (GET_ENABLE_RANDOM_MAP(addInfo._ui64OpenStage, ord))
	//		RandomMapContext::i()->AddStage(ord);
	//}
}

void room_normal::callback_season_match(void* pThis,INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		reinterpret_cast<room_normal*>(pThis)->ready_process();
	}
}














   





//----------------------------------------------------------------------------
//domi ready room
INT16 room_domi::m_grade = -1;
void room_domi::entrance_end()
{
	open_rank_web();

	LuaState * L = phase_ready_room->_CallLuaFunction( ROOM_T_DOMI, "refresh_score");
	if( L != nullptr)
	{
		i3Lua::PushInteger(L , Domination_Util::g_current_score);
		i3Lua::PushInteger(L , Domination_Util::g_best_score);

		phase_ready_room->_EndLuaFunction( L, 2);
	}
}

void room_domi::exit_start()
{
	close_rank_web( true);
}

void room_domi::revive()
{
	if( m_create_web)
	{// 레디움이 아닐 경우에도 OnRevive가 호출 될 경우도있기 때문에..
		close_rank_web( true);
		open_rank_web();
	}
}

void room_domi::do_ready_game()
{
	//장비 변경 교체 한다.
	UISideMenu::instance()->send_change_equip();

	//방장일 경우
	if( phase_ready_room->_IsHost())
		g_pFramework->GetUIMainframe()->OpenPopup( UPW_DOMI_ROUND);
	else
		GameEventSender::i()->SetEventOnce( EVENT_READY_BATTLE);
}

bool room_domi::is_ready()
{//방장 혼자라도 실행 할 수 있습니다.
	return true;
}

void room_domi::set_button_state()
{
	bool is_host = phase_ready_room->_IsHost();
	bool start_button = is_host;

	for(INT32 i = 0; i < DOMI_SLOT_COUNT; i++)
	{
		const SLOT_INFO * info = BattleSlotContext::i()->getSlotInfo( convert_slot( i,  DOMI_RED));
		if ((SLOT_STATE_LOAD <= info->_State) && (info->_State<= SLOT_STATE_BATTLE))
		{
			start_button = true;
			break;
		}
	}

	//----------------------------------------------------------------------------------
	//기존 ready room 버튼을 같이 사용 하기 때문에 기존 ready room의 루아를 호출.!!
	LuaState * L = phase_ready_room->_CallLuaFunction( "SetButtonState");
	if( L != nullptr)
	{
		i3Lua::PushBoolean(L , start_button);
		i3Lua::PushBoolean(L , phase_ready_room->_IsHost());				//domi room은 방설정 off 스프라이트 부족으로 방설정 창을 같이 못 띄우고 있음..
		i3Lua::PushBoolean(L , true);
		phase_ready_room->_EndLuaFunction( L, 3);
	}
}

bool room_domi::is_room_user( const i3::wliteral_range& wNickRng)
{
	if( phase_ready_room->GetMainFrame()->GetCurrentPhaseType() != UIPHASE_READY_ROOM) 
		return false;

	for( int i = 0; i < DOMI_SLOT_COUNT; i++)
	{
		const SLOT_INFO * info = BattleSlotContext::i()->getSlotInfo( convert_slot( i,  DOMI_RED));

		if (SLOT_STATE_CLOSE != info->_State && SLOT_STATE_EMPTY != info->_State)
		{
			i3::rc_wstring wstrNick = BattleSlotContext::i()->getNickForSlot( convert_slot( i,  DOMI_RED));
			if( i3::generic_is_iequal( wstrNick, wNickRng ) ) return true;
		}

	}
	return false;
}



void room_domi::process_update_slot()
{
	for( INT32 i = 0; i < DOMI_SLOT_COUNT; i++)
	{
		const SLOT_INFO* info = BattleSlotContext::i()->getSlotInfo( convert_slot( i, DOMI_RED));

		if( info->_State != SLOT_STATE_CLOSE && info->_State != SLOT_STATE_EMPTY)
		{
			close_slot( i, false);
			set_slot_info( i);
		}
		else
		{
			if( info->_State == SLOT_STATE_CLOSE)
			{
				close_slot( i, true);
				empty_slot( i);
			}
			else
			{
				close_slot( i, false);
				empty_slot( i);
			}
		}
	}

	//i3::string msg;
	//domi_util::to_string_reward_grade_room(msg);
	//if(msg.empty() == false)
	//{
	//	GameUI::PrintSystemChat(msg.c_str());
	//}
}

void room_domi::grade_domi_room(bool in_battle)
{
	i3::wstring wstr_msg;
		
	if(in_battle)
	{
		domi_util::to_string_reward_grade_room_inbattle(wstr_msg, m_grade);
	}
	else
	{
		domi_util::to_string_reward_grade_room(wstr_msg, m_grade);
	}

	if(wstr_msg.empty() == false)
	{
		GameUI::PrintSystemChat(wstr_msg);
	}
}

void room_domi::close_slot( INT32 slot, bool bClose, bool bLock /*= false*/)
{
	LuaState* L = phase_ready_room->_CallLuaFunction( ROOM_T_DOMI, "close_slot");
	if( L != nullptr)
	{
		i3Lua::PushInteger(L, slot);
		i3Lua::PushBoolean(L, bClose);
		i3Lua::PushBoolean(L, bLock || !(phase_ready_room->_IsHost()));
		phase_ready_room->_EndLuaFunction( L, 3);
	}
}

void room_domi::empty_slot( INT32 slot)
{
	LuaState * L = phase_ready_room->_CallLuaFunction( ROOM_T_DOMI, "set_slot_info");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, slot);				// slot
		GameUI::SetClanMarkToLua( L, 0xFFFFFFFF);
		i3Lua::PushStringUTF16To8( L, L"");			// clanname tooltip
		i3Lua::PushInteger( L, -1);					// rank
		i3Lua::PushStringUTF16To8( L, L"");			// rank tooltip
		i3Lua::PushStringUTF16To8( L, L"");			// nick
		i3Lua::PushColor( L, nullptr);
		i3Lua::PushInteger( L, -1);
		i3Lua::PushBoolean(L , phase_ready_room->_IsHost());
		i3Lua::PushBoolean(L , BattleSlotContext::i()->getMainSlotIdx() == convert_slot( slot, DOMI_RED));

		phase_ready_room->_EndLuaFunction( L, 17);
	}
}

void room_domi::set_slot_info( INT32 slot)
{
	const SLOT_INFO* info = BattleSlotContext::i()->getSlotInfo( convert_slot( slot, DOMI_RED));

	UI_READYSTATE state = UIRS_NONE;							// 상태
	switch( info->_State)
	{
	case SLOT_STATE_EMPTY :								break;
	case SLOT_STATE_CLOSE :								break;
	case SLOT_STATE_SHOP :		state = UIRS_SETTING;	break;
	case SLOT_STATE_INFO :										// 유저 정보
	case SLOT_STATE_CLAN :								break;	// 클랜
	case SLOT_STATE_INVENTORY :	state = UIRS_SETTING;	break;
	case SLOT_STATE_GIFTSHOP :	state = UIRS_SETTING;	break;	// 선물 상점
	case SLOT_STATE_NORMAL :							break;
	case SLOT_STATE_READY :		state = UIRS_READY;		break;
	case SLOT_STATE_LOAD :										// (게임중) 로딩중입니다.			Time Out ( 2분)     처음시작 방장시작후 (10초)
	case SLOT_STATE_RENDEZVOUS :								// (게임중) 랑데뷰서버 홀펀칭		Time Out ( )
	case SLOT_STATE_PRESTART :									// (게임중) 홀펀칭					Time Out ( )
	case SLOT_STATE_BATTLE_READY :								// (게임중) 홀펀칭 완료후 게임 시작을 기다림 
	case SLOT_STATE_BATTLE :	state = UIRS_PLAY;		break;	// (게임중) 전투중입니다.						
	case SLOT_STATE_GACHA :		state = UIRS_SETTING;	break;	// 뽑기 상점
	}

	INT32 rank;
	if( ChannelContext::i()->IsBeginnerChannel1())
		rank = (INT32)(RANK_NEWBIE);
	else
		rank = (INT32)(info->_Rank);

	i3::rc_wstring wstrRankTooltip = GetCharaRank( rank);

	if ( BattleSlotContext::i()->getMySlotIdx() == convert_slot( slot, DOMI_RED))
		effect_slot( slot);

	i3::stack_wstring wstrClanToolTip;

	if( info->_clanidx > 0 && info->_clanstate != CLAN_MEMBER_LEVEL_UNKNOWN)
	{			
		i3::rc_wstring wstrTemp;
		GetClanRankPreName2( wstrTemp, info->_clanExpRank);						// 클랜등급 표시

		i3::sprintf( wstrClanToolTip, L"%s %s",wstrTemp, info->_ClanName);
	}

	const I3COLOR * pNickColor = GameUI::GetNickNameColor( BattleSlotContext::i()->getNickColorForSlot( convert_slot( slot, DOMI_RED)));
	LuaState * L = phase_ready_room->_CallLuaFunction( ROOM_T_DOMI, "set_slot_info");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, slot);
		GameUI::SetClanMarkToLua( L, info->_clanMark);
		i3Lua::PushStringUTF16To8( L, wstrClanToolTip);	
		i3Lua::PushInteger( L, rank);											// rank
		i3Lua::PushStringUTF16To8( L, wstrRankTooltip);								// rank tooltip
		i3Lua::PushStringUTF16To8( L, BattleSlotContext::i()->getNickForSlot( convert_slot( slot, DOMI_RED)));		// nick
		i3Lua::PushColor( L, pNickColor);
		i3Lua::PushInteger( L, state);
		i3Lua::PushBoolean(L , phase_ready_room->_IsHost());
		i3Lua::PushBoolean(L , BattleSlotContext::i()->getMainSlotIdx() == convert_slot( slot, DOMI_RED));

		phase_ready_room->_EndLuaFunction( L, 17);
	}
}

void room_domi::effect_slot( INT32 slot)
{
	LuaState* L = phase_ready_room->_CallLuaFunction( ROOM_T_DOMI, "effect_slot");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, slot);
		phase_ready_room->_EndLuaFunction( L, 1);
	}
}

bool room_domi::is_alone()
{
	INT32 cnt = 0;
	for( INT32 i = 0; i < DOMI_SLOT_COUNT; ++i)
	{
		const SLOT_INFO * pSlot = BattleSlotContext::i()->getSlotInfo( convert_slot( i, DOMI_RED));
		if( !pSlot) continue;

		if( SLOT_STATE_CLOSE != pSlot->_State && SLOT_STATE_EMPTY != pSlot->_State)
			if( ++cnt > 1 ) return false;
	}
	return true;
}

void room_domi::click_slot( INT32 slot)
{
	I3_BOUNDCHK( slot, DOMI_SLOT_COUNT);
	if( slot < 0 || slot >= DOMI_SLOT_COUNT) return;

	const SLOT_INFO* info = BattleSlotContext::i()->getSlotInfo( convert_slot( slot, DOMI_RED));
	if( SLOT_STATE_EMPTY == info->_State)
	{
		//
	}
	else if( SLOT_STATE_CLOSE != info->_State)
	{
		UIFloatCharacterView * pCharaView = static_cast<UIFloatCharacterView *>(GameUI::GetFloating( UFW_CHARACTER));
		if( pCharaView != nullptr)
			pCharaView->ChangeOtherUserChara( convert_slot( slot, DOMI_RED));
	}
}

void room_domi::click_R_slot( INT32 slot)
{
	I3_BOUNDCHK( slot, DOMI_SLOT_COUNT);
	if( slot < 0 || slot >= DOMI_SLOT_COUNT) return;

	phase_ready_room->m_nSelectedSlotIdx = convert_slot( slot, DOMI_RED);
	const SLOT_INFO* info = BattleSlotContext::i()->getSlotInfo( convert_slot( slot, DOMI_RED));

	if (SLOT_STATE_EMPTY != info->_State && SLOT_STATE_CLOSE != info->_State)
	{
		click_slot( slot);

		i3::rc_wstring wstrUser = BattleSlotContext::i()->getNickForSlot( convert_slot( slot, DOMI_RED));
		if( i3::generic_string_size(wstrUser) > 0)
		{
			GameEventSender::i()->SetEvent( EVENT_GET_USERINFO_NICK, wstrUser.c_str());
		}
		phase_ready_room->PlaySound( UISND_ITEM_SELECTING);
	}
}

void room_domi::click_close( INT32 slot)
{
	I3_BOUNDCHK( slot, DOMI_SLOT_COUNT);
	if( slot < 0 || slot >= DOMI_SLOT_COUNT) return;

	if( !phase_ready_room->_IsHost()) return;
	
	if( BattleSlotContext::i()->getMainSlotIdx() == convert_slot( slot, DOMI_RED)) return;

	INT32 slot_flag = 0;
	const SLOT_INFO* info = BattleSlotContext::i()->getSlotInfo( convert_slot( slot, DOMI_RED));

	if (SLOT_STATE_CLOSE == info->_State)
		slot_flag = 0x10000000;

	slot_flag |= convert_slot( slot, DOMI_RED);

	GameEventSender::i()->SetEvent( EVENT_CHANGE_SLOT_N, &slot_flag);
}
 
void OnDocumentCompeteInReadyRoom( CWebCtrl* ctl)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

	if( pFrame != nullptr)
	{
		UIPhaseBase * pPhase = pFrame->GetCurrentPhase();
		if( i3::same_of<UIPhaseReadyRoom*>(pPhase))
		{
			room_domi* domi = (room_domi*)((UIPhaseReadyRoom*)pPhase)->get_room_detail( ROOM_T_DOMI);

			if( ctl && ctl->GethrNavigate2() == S_FALSE)
			{
				domi->m_connected = false;
				domi->close_rank_web( false);
				//ui..
			}
			else
				domi->m_connected = true;
		}
	}
}

void room_domi::open_rank_web()
{ 
	i3::pack::RECT rc;
	calc_rect( rc);
 
	UINT32 w = (UINT32)(rc.right - rc.left);
	UINT32 h = (UINT32)(rc.bottom - rc.top);

	if( m_create_web == false)
	{  
		if( InitWebCtrl())
		{
			m_hwnd_web = ::CreateWebControl( Domination_Util::g_domi_rank_web, WS_CLIPCHILDREN | WS_CHILD,
				(UINT32)rc.left, (UINT32)rc.top, w, h, g_pViewer->GetWindowHandle(), NULL);

			m_create_web = true;
			::ShowWindow( m_hwnd_web, SW_SHOW);

			CWebCtrl* web_ctl = nullptr;
			web_ctl = (CWebCtrl*)GetProp( m_hwnd_web, "this");
			if( web_ctl)
				web_ctl->SetCBDocumentComplete( OnDocumentCompeteInReadyRoom);
		}
	}
	else
	{
		if( m_connected)
			reposition_rank_web();
	}
}

void room_domi::close_rank_web( bool del)
{
	if( m_create_web)
	{
		if( del)
		{
			::SetFocus(g_pViewer->GetWindowHandle());
			UninitWebCtrl();
			::DestroyWindow( m_hwnd_web);

			m_create_web = false;
			m_connected = false;
			m_hwnd_web = 0;
		}
		else
		{
			::ShowWindow( m_hwnd_web, SW_HIDE);
		}
	}
}

void room_domi::calc_rect( i3::pack::RECT& rc)
{
	i3UIFrameWnd* frame_web = (i3UIFrameWnd*)frame_wnd->FindChildByName( "FrameWnd_Ranking");
	i3UIImageBox* image_web = (i3UIImageBox*)frame_wnd->FindChildByName( "web");

	I3ASSERT( image_web != nullptr);
	I3ASSERT( frame_web != nullptr);
	i3Viewer* pViewer = g_pFramework->GetViewer();

	REAL32 ui_ratio = (REAL32)i3UI::getManager()->getWidth() / (REAL32)i3UI::getManager()->getHeight();
	REAL32 view_ratio = (REAL32)pViewer->GetViewWidth() / (REAL32)pViewer->GetViewHeight();


	//pos
	VEC3D* image_pos = image_web->getAccumulatedPos();

	INT32 offset_x = i3UI::getManager()->getDisplayOffsetX();
	INT32 offset_y = i3UI::getManager()->getDisplayOffsetY();

	VEC3D v;
	v.x = (REAL32)pViewer->GetViewWidth() / (REAL32)i3UI::getManager()->getWidth();
	v.y = (REAL32)pViewer->GetViewHeight() / (REAL32)i3UI::getManager()->getHeight();

	REAL32 ratio = v.y;
	if( ui_ratio > view_ratio)
		ratio = v.x;

	rc.left = (image_pos->x*ratio) + offset_x;
	rc.top = (image_pos->y*ratio) + offset_y;

  


	//size
	i3::pack::RECT cs;
	image_web->getAccumulatedRect( &cs);

	REAL32 w = cs.right - cs.left;
	REAL32 h = cs.bottom - cs.top;

	rc.right = rc.left + (w * ratio);
	rc.bottom = rc.top + (h * ratio);
}

void room_domi::reposition_rank_web()
{
	if( m_create_web)
	{
		i3::pack::RECT rc;
		calc_rect( rc);

		UINT32 w = (UINT32)(rc.right - rc.left);
		UINT32 h = (UINT32)(rc.bottom - rc.top);

		::ShowWindow( m_hwnd_web, SW_SHOW);
		::MoveWindow( m_hwnd_web, (UINT32)rc.left, (UINT32)rc.top, w, h, TRUE);
	}
}



static room_domi* get_room_domi()
{
	UIMainFrame* frame = g_pFramework->GetUIMainframe();
	if( frame)
	{
		UIPhaseBase* phase = frame->GetCurrentPhase();
		if( i3::same_of<UIPhaseReadyRoom*>(phase))
		{
			UIPhaseReadyRoom* phase_ready_room = (UIPhaseReadyRoom*)phase;
			room_domi* room = (room_domi*)phase_ready_room->get_room_detail( ROOM_T_DOMI);
			return room;
		}
	}
	return nullptr;
}

void room_normal_V10::set_slot_info( INT32 slot)
{
	const SLOT_INFO* pInfo = BattleSlotContext::i()->getSlotInfo( slot);

	// 상태
	UI_READYSTATE_V10 state = UIRS_V10_NONE;

	switch( pInfo->_State)
	{
	case SLOT_STATE_NORMAL :							break;
	case SLOT_STATE_EMPTY :								break;
	case SLOT_STATE_CLOSE :								break;
	case SLOT_STATE_GACHA :										// 뽑기 상점
	case SLOT_STATE_GIFTSHOP :
	case SLOT_STATE_SHOP :		state = UIRS_V10_SHOP;	break;
	case SLOT_STATE_INFO :		state = UIRS_V10_INFO;	break;	// 유저 정보
	case SLOT_STATE_CLAN :		state = UIRS_V10_CLAN;	break;	// 클랜
	case SLOT_STATE_INVENTORY :	state = UIRS_V10_INVEN;	break;
	case SLOT_STATE_READY :		state = UIRS_V10_READY;		break;	
	case SLOT_STATE_LOAD :										// (게임중) 로딩중입니다.			Time Out ( 2분)     처음시작 방장시작후 (10초)
	case SLOT_STATE_RENDEZVOUS :								// (게임중) 랑데뷰서버 홀펀칭		Time Out ( )
	case SLOT_STATE_PRESTART :									// (게임중) 홀펀칭					Time Out ( )
	case SLOT_STATE_BATTLE_READY :								// (게임중) 홀펀칭 완료후 게임 시작을 기다림 
	case SLOT_STATE_BATTLE :	state = UIRS_V10_PLAY;		break;	// (게임중) 전투중입니다.						
	}
	// 계급을 표시합니다.
	INT32 rank = 0;

	if( ChannelContext::i()->IsBeginnerChannel1() )
	{	
		rank = (INT32)(RANK_NEWBIE);
	}
	else
	{	
		rank = (INT32)(pInfo->_Rank);
	}
	// 클랜 매치메이킹
	if(ChannelContext::i()->GetConnectedChannelType() == CHANNEL_TYPE_CLAN &&
		UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW) )
	{
		if(ClanGameContext::i()->GetMercRank(slot) != -1) // 값이 없는 경우는 용병이 아님
			rank = ClanGameContext::i()->GetMercRank(slot);
	}


	i3::rc_wstring wstrRankToolTip = GetCharaRank(rank);

	// 자기 슬롯 강조
	if (BattleSlotContext::i()->getMySlotIdx() == slot)
		effect_slot( slot);

	//	클랜 마크
	i3::stack_wstring wstrClanToolTip;

	if( pInfo->_clanidx > 0 && pInfo->_clanstate != CLAN_MEMBER_LEVEL_UNKNOWN)
	{	
		// 클랜마크 툴팁 ( 매치인 경우, 분대명으로 툴팁 생성)
		if( g_pFramework->GetClanContext()->InBattleTeam() )
		{
			i3::wstring wstrTeamName;
			TEAM_TYPE team_type = CHARA::CharaTeamType2TeamType( BattleSlotContext::i()->getTeam(slot) );

			const CLAN_MATCH_BATTLE_TEAM * pTeamInfo = ClanGameContext::i()->getClanBattleTeamInfo(team_type);

			GetBattleTeamName( wstrTeamName, pTeamInfo->getMatchTeamID() );
			i3::sprintf( wstrClanToolTip, L"%s %s", pTeamInfo->getClanName(), wstrTeamName);		// TODO : 자동 형변환되고 있음.(2014.07.18.수빈)
		}
		else
		{
			// 클랜등급 표시
			i3::rc_wstring wstrTemp; 
			GetClanRankPreName2( wstrTemp, pInfo->_clanExpRank);

			i3::sprintf( wstrClanToolTip, L"%s %s",wstrTemp, pInfo->_ClanName);		// TODO : 자동 형변환되고 있음.(2014.07.18.수빈)
		}
	}

	UI_EVENT_MARK pcState = UIEM_NONE;
	//// PC방 표시
	switch( pInfo->_pcCafe )
	{
	case PC_NOT_CAFE			: pcState = UIEM_NONE;			break;
	case PC_NORMAL_CAFE			: pcState = UIEM_PCCAFE;		break;
	case PC_PREMIUM_CAFE		: pcState = UIEM_PCCAFE_PLUS;	break;
	case PC_VACATION_EVENT_ITEM	: pcState = UIEM_NONE;			break;
	case PC_INDO_TELCOM_ITEM	: pcState = UIEM_NONE;			break; 
	case PC_GARENA_GCA			: pcState = UIEM_GARENA_GCA;	break;
	case 0xFF					: pcState = UIEM_NONE;			break;
	default						: pcState = UIEM_NONE;			I3ASSERT_0;	break;
	}

	//	유저 E-Sport 태그
	INT32 esports_index = GameUI::GetEsportsTemplateIndex( (ESPORT_ID_LEVEL)pInfo->_eSportID );

	const I3COLOR * pNickColor = GameUI::GetNickNameColor( BattleSlotContext::i()->getNickColorForSlot( slot));

	bool is_enable_close =  (phase_ready_room->_IsHost() == true) && (phase_ready_room->IsBattleCountDown() == false);

	LuaState * L = phase_ready_room->_CallLuaFunction( "SetSlotInfo");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, slot);											// slot
		GameUI::SetClanMarkToLua( L, pInfo->_clanMark);
		i3Lua::PushStringUTF16To8( L, wstrClanToolTip);								// clanname tooltip
		i3Lua::PushInteger( L, rank);											// rank
		i3Lua::PushStringUTF16To8( L, wstrRankToolTip);								// rank tooltip
		i3Lua::PushStringUTF16To8( L, BattleSlotContext::i()->getNickForSlot( slot));		// nick
		i3Lua::PushColor( L, pNickColor);

		i3Lua::PushInteger( L, pcState);
		i3Lua::PushInteger( L, state);
		i3Lua::PushBoolean(L , is_enable_close);
		i3Lua::PushBoolean(L , BattleSlotContext::i()->getMainSlotIdx() == slot);

		i3Lua::PushInteger( L, esports_index);									// Esports Mark
		phase_ready_room->_EndLuaFunction( L, 19);
	}
}

void room_normal_V10::ready_process()
{
	if( MyRoomInfoContext::i()->IsRandomMap() && phase_ready_room->_IsHost())
		phase_ready_room->RunRandomMap();

	ROOM_INFO_BASIC	roomInfo;
	MyRoomInfoContext::i()->getMyRoomInfo(&roomInfo);

	if (roomInfo._State != ROOM_STATE_READY)
	{
		INT32 my_team_count = 0;

		INT32 slot_idx = BattleSlotContext::i()->getMySlotIdx() % 2;
		while (slot_idx < SLOT_MAX_COUNT)
		{
			const SLOT_INFO* pSlot = BattleSlotContext::i()->getSlotInfo(slot_idx);

			if (SLOT_STATE_READY <= pSlot->_State)
			{
				my_team_count++;
			}

			slot_idx += 2;
		}

		struct _tag_enter_limit
		{
			INT32		limit_count;
			i3::string	str_notice;
		};

		_tag_enter_limit limit[STAGE_SLOT_MODE_MAX] = {
			{ 8,	"STR_TBL_GUI_LOBBY_LIMIT_ENTER_ROOM" },			/*난입이 제한된 방입니다..*/
			{ 4,	"STR_TBL_GUI_READY_NOTICE_LIMIT_ENTER_4VS4" },	/*4vs4 모드 인원수 제한으로 난입할 수 없습니다.*/
			{ 5,	"STR_TBL_GUI_READY_NOTICE_LIMIT_ENTER_5VS5" }	/*5vs5 모드 인원수 제한으로 난입할 수 없습니다.*/
		};

		STAGE_SLOT_MODE eMode = CStageInfoUtil::GetSlotMode(*CStageInfoUtil::GetStageData(roomInfo._StageID));

		if (limit[eMode].limit_count <= my_team_count)
		{
			g_pFramework->PutSystemChatting(GAME_RCSTRING(limit[eMode].str_notice));
			return;
		}
	}

	//장비 변경 교체 한다.
	bool bEventSent = GameEventSender::i()->SetEventOnce( EVENT_READY_BATTLE )?true:false;

	bool bStartBtnEnable = false;

	if( phase_ready_room->_IsHost())									// 방장 or 게임중일때는 Ready Btn이 아닌 Start Btn 을 사용한다.
		bStartBtnEnable = true;
	else
	{
		for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
		{
			const SLOT_INFO * pSlot = BattleSlotContext::i()->getSlotInfo(i);
			if ((SLOT_STATE_LOAD <= pSlot->_State) && (pSlot->_State<= SLOT_STATE_BATTLE))
			{
				bStartBtnEnable = true;
				break;
			}
		}
	}

	if(bStartBtnEnable == true && bEventSent)
		g_pFramework->GetUIMainframe()->GetTopMenu()->SetInputDisable(true);
}


extern "C" {
	int room_domi_click_slot( LuaState* L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		get_room_domi()->click_slot( idx);
		return 0;
	}

	int room_domi_click_R_slot( LuaState* L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		get_room_domi()->click_R_slot( idx);
		return 0;
	}

	int room_domi_click_close( LuaState* L)
	{
		// 대기실 팀원 레디 시 방장 튕기는 기능 제거
		//INT32 idx = i3Lua::GetIntegerArg( L, 1);
		//get_room_domi()->click_close( idx);
		return 0;
	}
};