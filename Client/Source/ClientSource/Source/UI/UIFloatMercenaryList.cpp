#include "pch.h"
#include "UIFloatMercenaryList.h"

#include "ClanGameContext.h"
#include "UserInfoContext.h"

#include "../UI_V10/UIFloatUserList_V10.h"
#include "../UI_V11/UIFloatChat_V11.h"
#include "UIMainFrame.h"
#include "UIUtil.h"

#include "UIPhaseClanLobby.h"
#include "../UI_V10/UIPhaseLobby_V10.h"

namespace
{
	const INT32		MERCENARY_LIST_COUNT = 7;
	const REAL32	MERCENARY_UPDATE_TIME = 3.0f;

	REAL32			g_mercenary_update_time = 0.0f;

	namespace callback
	{
		void mercenary_register(void * pThis, INT32 nParam)
		{
			if( nParam == MB_OK)
				reinterpret_cast<UIFloatMercenaryList *>(pThis)->Register();
		}

		void mercenary_invite(void * pThis, INT32 nParam)
		{
			if( nParam == MB_OK)
				reinterpret_cast<UIFloatMercenaryList *>(pThis)->Invite();
		}

		void mercenary_invite_response(void * pThis, INT32 nParam)
		{
			if( nParam == MB_OK)
				reinterpret_cast<UIFloatMercenaryList *>(pThis)->AcceptInvite();
			else
				reinterpret_cast<UIFloatMercenaryList *>(pThis)->DenialInvite();
		}
	}
}

extern "C" {

	int UIFloatMercenaryList_ToggleFrame( LuaState* L)
	{
		bool bEnable = i3Lua::GetBooleanArg( L, 1);

		UIFloatMercenaryList * pThis = static_cast<UIFloatMercenaryList *>(GameUI::GetFloating( UFW_MERCENARY));
		if( pThis != nullptr)
			pThis->ToggleFrame( bEnable == true );

		return 0;
	}

	int UIFloatMercenaryList_Request( LuaState* L)
	{
		UIFloatMercenaryList * pThis = static_cast<UIFloatMercenaryList *>(GameUI::GetFloating( UFW_MERCENARY));
		if( pThis != nullptr)
			pThis->ClickRequest();

		return 0;
	}

	int UIFloatMercenaryList_Cancel( LuaState* L)
	{
		UIFloatMercenaryList * pThis = static_cast<UIFloatMercenaryList *>(GameUI::GetFloating( UFW_MERCENARY));
		if( pThis != nullptr)
			pThis->ClickCancel();

		return 0;
	}

	int UIFloatMercenaryList_ScrollList(LuaState * L)
	{
		UIFloatMercenaryList * pThis = static_cast<UIFloatMercenaryList *>(GameUI::GetFloating( UFW_MERCENARY));
		if( pThis != nullptr)
			pThis->OnScrollList();

		return 0;
	}

	int UIFloatMercenaryList_WheelList(LuaState * L)
	{
		INT32 zDelta = i3Lua::GetIntegerArg( L, 1);
		UIFloatMercenaryList * pThis = static_cast<UIFloatMercenaryList *>(GameUI::GetFloating( UFW_MERCENARY));
		if( pThis != nullptr)
			pThis->OnWheelList(zDelta);

		return 0;
	}

	int UIFloatMercenaryList_SelectUser( LuaState * L)
	{
		INT32 selectIdx = i3Lua::GetIntegerArg( L, 1);

		UIFloatMercenaryList * pThis = static_cast<UIFloatMercenaryList *>(GameUI::GetFloating( UFW_MERCENARY));
		if( pThis != nullptr)
			pThis->SelectUser( selectIdx);

		return 0;
	}

	int UIFloatMercenaryList_OpenMercDetail( LuaState * L )
	{
		INT32 whisperIdx = i3Lua::GetIntegerArg( L, 1);

		UIFloatMercenaryList * pThis = static_cast<UIFloatMercenaryList *>(GameUI::GetFloating( UFW_MERCENARY));
		if( pThis != nullptr)
			pThis->OpenMercDetail( whisperIdx );

		return 0;
	}
};

LuaFuncReg UIFloatMercenaryList_Glue[] = {
	{ "ToggleFrame",			UIFloatMercenaryList_ToggleFrame },
	{ "Request",				UIFloatMercenaryList_Request },
	{ "Cancel",					UIFloatMercenaryList_Cancel },
	{ "ScrollMercenaryList",	UIFloatMercenaryList_ScrollList },
	{ "WheelMercenaryList",		UIFloatMercenaryList_WheelList },
	{ "SelectUser",				UIFloatMercenaryList_SelectUser},
	{ "OpenMercDetail",			UIFloatMercenaryList_OpenMercDetail}, 
	{ nullptr,						nullptr}
};

I3_CLASS_INSTANCE( UIFloatMercenaryList);

UIFloatMercenaryList::UIFloatMercenaryList() : m_pScrollBar(nullptr), m_i32SelectedIdx(-1), m_pInviteBtn(nullptr), 
m_pMainFmw(nullptr), m_InviteMsgBox(nullptr), m_bLeaveClanChannel(false)
{
	m_AlignedPos = UIFLOATING_POS_LEFTTOP;

	RegisterGameEventFunc( EVENT_CLAN_WAR_GET_MERCENARY_LIST,		&UIFloatMercenaryList::ReceiveEvent_ClanWar_GetMercenaryList);
	RegisterGameEventFunc( EVENT_CLAN_WAR_REGISTER_MERCENARY,		&UIFloatMercenaryList::ReceiveEvent_ClanWar_RegisterMercenary);
	RegisterGameEventFunc( EVENT_CLAN_WAR_REMOVE_MERCENARY,			&UIFloatMercenaryList::ReceiveEvent_ClanWar_RemoveMercenary);
	RegisterGameEventFunc( EVENT_CLAN_WAR_SEND_INVITE_MERCENARY,	&UIFloatMercenaryList::ReceiveEvent_ClanWar_SendInviteMercenary);
	RegisterGameEventFunc( EVENT_CLAN_WAR_RECV_INVITE_MERCENARY,	&UIFloatMercenaryList::ReceiveEvent_ClanWar_RecvInviteMercenary);
	RegisterGameEventFunc( EVENT_CLAN_WAR_ACCEPT_INVITE,			&UIFloatMercenaryList::ReceiveEvent_ClanWar_AcceptInvite);
	RegisterGameEventFunc( EVENT_CLAN_WAR_DENIAL_INVITE,			&UIFloatMercenaryList::ReceiveEvent_ClanWar_DenialInvite);

	RegisterGameEventFunc( EVENT_CLAN_WAR_CREATE_TEAM,				&UIFloatMercenaryList::ReceiveEvent_ClanWar_ModifyMatchTeamInfo);
	RegisterGameEventFunc( EVENT_CLAN_WAR_JOIN_TEAM,				&UIFloatMercenaryList::ReceiveEvent_ClanWar_ModifyMatchTeamInfo);
	RegisterGameEventFunc( EVENT_CLAN_WAR_LEAVE_TEAM,				&UIFloatMercenaryList::ReceiveEvent_ClanWar_ModifyMatchTeamInfo);
	RegisterGameEventFunc( EVENT_CLAN_WAR_MERCENARY_DETAIL_INFO,	&UIFloatMercenaryList::ReceiveEvent_ClanWar_GetMercenaryDetail);
	m_SecCount.SetReverse(true);
	m_SecCountCB.SetObj(this);
	m_SecCount.SetCallback(&m_SecCountCB);

}

UIFloatMercenaryList::~UIFloatMercenaryList()
{
}

/*virtual*/ void UIFloatMercenaryList::_InitializeAtLoad(i3UIScene * pScene)
{
	UIFloatingBase::_InitializeAtLoad(pScene);
	m_pScrollBar = i3::same_cast<i3UIScrollBar*>( pScene->FindChildByName( "Scroll_Mercenary_List"));
	I3ASSERT( m_pScrollBar != nullptr );

	m_pInviteBtn = i3::same_cast<i3UIButtonComposed3*>( pScene->FindChildByName( "Btn_Request") );
	I3ASSERT( m_pInviteBtn != nullptr );

	m_pMainFmw = i3::same_cast<i3UIFrameWnd*>( pScene->FindChildByName( "FrameWnd_Clan_Mercenary") );
	I3ASSERT( m_pMainFmw != nullptr );

	if(g_pEnvSet->IsBaseOnUIVersion_1_0())
		m_slide.AddFrameWnd(m_pMainFmw, UISlide::R_TO_L);
	else if(g_pEnvSet->m_nUIVersion == UFV_1_1)
		m_slide.AddFrameWnd(m_pMainFmw, UISlide::L_TO_R);


	m_SecCount.SetBaseSec(30);
	m_bFixedWindow = true;
}

/*virtual*/ void UIFloatMercenaryList::_ClearAtUnload(i3UIScene * pScene)
{
	UIFloatingBase::_ClearAtUnload(pScene);
	m_pScrollBar = nullptr;
	m_pInviteBtn = nullptr;
	m_pMainFmw = nullptr;
}

void UIFloatMercenaryList::_InitMercenaryList(void)
{
	_InitUIText();
	_SetButtonActivate();
	ClanGameContext::i()->resetClanMercenaryList();
	ToggleFrame(true);

	if(m_pInviteBtn != nullptr)
		m_pInviteBtn->disable_btn();
}

void UIFloatMercenaryList::_InitUIText(void)
{
	LuaState * L = _CallLuaFunction("InitUIText");

	if ( L != nullptr )
	{
		i3::rc_wstring wstr_list;
		i3::rc_wstring wstr_request_button;

		if( isLobbyPhase() )
		{
			i3::sprintf( wstr_list, GAME_RCSTRING("STR_MERC_REQUEST_LIST_TEXT") );				// 용병 신청 리스트
			i3::sprintf( wstr_request_button, GAME_RCSTRING("STR_MERC_REQUEST_TEXT") );			// 용병 신청
		}
		else
		{
			i3::sprintf( wstr_list, GAME_RCSTRING("STR_MERC_POPUP_TITLE") );					// 용병 대기 목록
			i3::sprintf( wstr_request_button, GAME_RCSTRING("STR_OPT_TAP_COMMUNITY_INVITE") );	// 초대
		}

		i3Lua::PushStringUTF16To8(L , wstr_list);
		i3Lua::PushStringUTF16To8(L , wstr_request_button);
		_EndLuaFunction(L , 2);
	}
}

void UIFloatMercenaryList::_UpdateMercenaryList(void)
{
	const INT32 scroll_pos = m_pScrollBar->getScrollPos();
	const INT32 mercenary_count = ClanGameContext::i()->getClanMercenaryCount();

	// list view update
	for(INT32 idx = 0 ; idx < MERCENARY_LIST_COUNT; ++idx)
	{
		INT32 real_idx = idx + MAX(0, scroll_pos);
		const CLAN_MERCENARY_INFO * pInfo = ClanGameContext::i()->getClanMercenaryInfo(real_idx);
		_SetUserInfo(idx, pInfo);
	}

	// scroll bar update;
	{
		INT32 new_scroll_pos = MINMAX( 0, scroll_pos, mercenary_count );
		m_pScrollBar->setScrollRange(0, mercenary_count);
		m_pScrollBar->setScrollPos( new_scroll_pos );
	}
	
	// user count update
	_SetUserCount( mercenary_count );
}

void UIFloatMercenaryList::_SetUserInfo(const INT32 idx, const CLAN_MERCENARY_INFO * pInfo)
{
	LuaState * L = _CallLuaFunction("SetUserInfo");
	if( L != nullptr )
	{
 		INT32 mercenary_rank = -1;
		i3::rc_wstring wstr_nick = L"";

		if( pInfo != nullptr )
		{
			wstr_nick = pInfo->getNickName();
			mercenary_rank = ClanGameContext::i()->GetMercIconIdx(pInfo);
		}

		i3Lua::PushInteger(L , idx);
		i3Lua::PushInteger(L , mercenary_rank);
		i3Lua::PushStringUTF16To8(L , wstr_nick );
		_EndLuaFunction(L , 3);
	}
}

void UIFloatMercenaryList::_SetUserCount(const INT32 count)
{
	LuaState * L = _CallLuaFunction("SetUserCount");
	if( L != nullptr )
	{
		i3::rc_wstring wstr_nick;
		i3::sprintf(wstr_nick, GAME_RCSTRING("STBL_IDX_CLAN_DECLARE_PERSON"), count);	// %d 명

		i3Lua::PushStringUTF16To8(L , wstr_nick);
		_EndLuaFunction(L , 1);
	}
}

void UIFloatMercenaryList::_SetCurSelectItem( void)
{
	if(m_i32SelectedIdx < 0)
		return;

	INT32 select_idx = m_i32SelectedIdx - MAX(0, m_pScrollBar->getScrollPos() );

	LuaState * L = _CallLuaFunction("SetCurSelectItem");

	if( L != nullptr)
	{	
		i3Lua::PushInteger(L, select_idx);
		_EndLuaFunction(L, 1);		
	}
}

void UIFloatMercenaryList::_SetRequestButtonEnable( void)
{
	bool bEnable = true;
	
	// 일반 로비 : 용병 상태에 따라 등록 / 취소 를 번갈아가며 출력
	if( isLobbyPhase() )
		bEnable = ( ClanGameContext::i()->getMyMercenaryState() == false );

	LuaState * L = _CallLuaFunction("SetRequestBtnEnable");
	if ( L != nullptr )
	{
		i3Lua::PushBoolean(L , bEnable);
		_EndLuaFunction(L , 1);
	}
}
void UIFloatMercenaryList::_SetButtonActivate(void)
{
	bool is_activate = false;

	if( isLobbyPhase() )
	{
		is_activate = isClanMember();

		// 용병도 [취소] 버튼이 활성화 되어야 함
		// 어차피 _SetRequestButtonEnable 에서 [등록] 버튼이 SetEnable(false) 됨
		if(ClanGameContext::i()->getMyMercenaryState())
			is_activate = true;
	}
	else
	{
		is_activate = ClanGameContext::i()->isMatchTeamLeader();
	}

	LuaState * L = _CallLuaFunction("SetButtonActivate");
	if ( L != nullptr )
	{
		i3Lua::PushBoolean(L, is_activate);
		_EndLuaFunction(L, 1);
	}
}

/*virtual*/ void UIFloatMercenaryList::OnCreate(i3GameNode * pParent)
{
	UIFloatingBase::OnCreate( pParent);

	// Load Scene
	AddScene("Scene/Clan/PointBlankRe_Clan_match_mercenary.i3UIs", UIFloatMercenaryList_Glue);
}

/*virtual*/ void UIFloatMercenaryList::OnUpdate( REAL32 rDeltaSeconds)
{
	UIFloatingBase::OnUpdate( rDeltaSeconds);
	m_SecCount.Update();

	//	3초에 1번씩 용병 리스트 요청
	g_mercenary_update_time += rDeltaSeconds;
	if( g_mercenary_update_time > MERCENARY_UPDATE_TIME)
	{
		g_mercenary_update_time = 0.f;

		// 현재 phase가 로비일 때만  요청.
		if(!m_bLeaveClanChannel)
			GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_GET_MERCENARY_LIST);
	}

	if(!isLobbyPhase() && (ClanGameContext::i()->getClanMercenaryInfo(m_i32SelectedIdx) != nullptr))
	{
		if(m_pInviteBtn != nullptr)
		{
			if(ClanGameContext::i()->isMatchTeamLeader() )
				m_pInviteBtn->enable_btn();
			else
				m_pInviteBtn->disable_btn();
		}
	}

	if(isLobbyPhase() &&
		ClanGameContext::i()->getMyMercenaryState() == false)
	{
		if(m_pInviteBtn != nullptr)
			m_pInviteBtn->enable_btn();
	}

}

void UIFloatMercenaryList::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
{
	UIFloatingBase::ProcessGameEvent( evt, arg, UserData);

	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}
}

/*virtual*/ bool UIFloatMercenaryList::OnEntranceStart(void * pArg1 /* = nullptr */, void * pArg2 /* = nullptr */)
{
	// 클랜 매치가 Off 상태면, 용병도 사용 안합니다.
	if( UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW) == false )
		return false;

	if( UIFloatingBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	//m_slide.ForceEnd();	//slide 행동을 강제한다.

	g_mercenary_update_time = 0.f;
	m_i32SelectedIdx = -1;

	_InitMercenaryList();
	_SetFramePos();

	if (g_pEnvSet->IsBaseOnUIVersion_1_0())
	{
		//강제로 위치를 맞춰주기 때문에 슬라이드 최종 위치도 수정해준다.
		m_slide.SetOriginalPos(m_pMainFmw->getPos()->x - g_pViewer->GetViewWidth(), m_pMainFmw->getPos()->y);
	}
	
	return true;
}

/*virtual*/ bool UIFloatMercenaryList::OnExitStart(void)
{
	if( UIFloatingBase::OnExitStart() == false)
		return false;

	return true;
}

bool UIFloatMercenaryList::OnRevive(i3RenderContext * pCtx)
{
	_SetFramePos();

	return UIFloatingBase::OnRevive(pCtx);
}

void UIFloatMercenaryList::ToggleFrame(const bool bEnable)
{
	// list frame toggle
	LuaState * L = _CallLuaFunction("ToggleFrame");
	if( L != nullptr )
	{
		i3Lua::PushBoolean(L, bEnable);
		_EndLuaFunction(L , 1);
	}

	// init scroll bar
	{
		const INT32 mercenary_count = ClanGameContext::i()->getClanMercenaryCount();

		m_pScrollBar->setTickSize(1);
		m_pScrollBar->setPageSize(MERCENARY_LIST_COUNT);
		m_pScrollBar->setScrollPos(0);
		m_pScrollBar->setScrollRange(0, mercenary_count);
	}

	m_i32SelectedIdx = -1;

	// set member list
	if( bEnable)
	{
		g_pFramework->SetFocus( m_pScrollBar);
		_SetRequestButtonEnable();
		_UpdateMercenaryList();
	}
}

void  UIFloatMercenaryList::OnScrollList(void)
{
	if( m_pScrollBar == nullptr)
		return;

	if( ClanGameContext::i()->getClanMercenaryCount() <= MERCENARY_LIST_COUNT)
		return;

	_UpdateMercenaryList();
	_SetCurSelectItem();
}

void  UIFloatMercenaryList::OnWheelList(INT32 zDelta)
{
	if( m_pScrollBar == nullptr)
		return;

	if( ClanGameContext::i()->getClanMercenaryCount() <= MERCENARY_LIST_COUNT)
		return;

	INT32 new_scroll_pos = m_pScrollBar->getScrollPos() + (zDelta / -120);
	INT32 scroll_min, scroll_max = 0;

	m_pScrollBar->getScrollRange(&scroll_min, &scroll_max);
	new_scroll_pos = MINMAX(scroll_min, new_scroll_pos, scroll_max);

	m_pScrollBar->setScrollPos( new_scroll_pos );

	_UpdateMercenaryList();
	_SetCurSelectItem();
}

void UIFloatMercenaryList::SelectUser( const INT32 select_idx )
{
	m_i32SelectedIdx = select_idx + MAX(0, m_pScrollBar->getScrollPos() );
}

void UIFloatMercenaryList::OpenMercDetail( const INT32 whisper_idx )
{
	// 우클릭 시에도 유저 슬롯 번호가 필요하기 때문에 선택된 것으로 간주해야함.
	m_i32SelectedIdx = whisper_idx + MAX(0, m_pScrollBar->getScrollPos() );
	// 정보 요청
	DetailInfo();
}

void UIFloatMercenaryList::ClickRequest(void)
{
	// 클랜원인 경우에만 처리.
	if( isClanMember() == false )
		return;

	// 기획 요청으로 매치팀에 소속된 인원이 최대 인원과 동일하면 팝업 띄우고 초대 막음
	if( !isLobbyPhase() )
	{
		const CLAN_MATCH_TEAM_INFO * pTeamInfo = ClanGameContext::i()->getMyClanMatchTeam(ClanGameContext::i()->getMyClanMatchTeamIdx());
		I3ASSERT(pTeamInfo != nullptr);

		if( pTeamInfo->isMaxPerson() )
		{
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MERC_ROOM_FULL"));
			return;
		}
	}

	MSG_TYPE			type = MSG_TYPE_GAME_OK;
	i3::rc_wstring		wstr_msg = L"";
	MSG_BOX_CALLBACK	call_fn = nullptr;

	if( isLobbyPhase() )
	{
		REAL32  Killcount	= (REAL32)UserInfoContext::i()->GetMyUserInfoRecord()->_killcount;
		REAL32 Deathcount	= (REAL32)UserInfoContext::i()->GetMyUserInfoRecord()->_death;
		if( (Killcount / (Killcount + Deathcount)) < 0.5 || (Killcount + Deathcount) == 0 )
		{
			// 용병 신청을 하기 위해서는 클랜원이어도 기본조건(K/D 50%↑)을 충족해야함
			/* K/D가 50% 미만이므로, 용병 신청에 실패 하였습니다.*/
			GameUI::MsgBox(MSG_TYPE_ERROR, GAME_STRING("STR_MERC_USECLAN_APP_FAIL"));
			return;
		}

		type = MSG_TYPE_GAME_QUERY;
		call_fn = &callback::mercenary_register;
		/* 용병 신청을 하겠습니까? */
		wstr_msg = GAME_RCSTRING("STR_MERC_CLANUSER_JOIN_POPUP");
	}
	else
	{
		enum INVITE_RESULT
		{ 
			SUCCESS = 0,					// 성공
			FAIL_NOLEADER,					// 실패: 리더아님
			FAIL_NOTFOUND,					// 실패: 유저 찾기 실패
			FAIL_MERCEMAX,					// 실패: 이미 팀내 용병이 2명
		}; 
		
		INVITE_RESULT result = SUCCESS;

		if( m_i32SelectedIdx < 0 )
			result = FAIL_NOTFOUND;

		const CLAN_MERCENARY_INFO * pInfo = ClanGameContext::i()->getClanMercenaryInfo(m_i32SelectedIdx);
	
		if( pInfo == nullptr)
			result = FAIL_NOTFOUND;

		if( ClanGameContext::i()->isMatchTeamLeader() == false )
			result = FAIL_NOLEADER;

		if( ClanGameContext::i()->IsMaxMerceMyTeam() == true )		
			result = FAIL_MERCEMAX;

		switch(result)
		{
		case SUCCESS		:	
			{
				type = MSG_TYPE_GAME_QUERY;
				call_fn = &callback::mercenary_invite;
				i3::sprintf(wstr_msg, GAME_RCSTRING("STR_MERC_INVITEUSER_POPUP"), pInfo->getNickName() );
			}
			break;
		case FAIL_NOLEADER	:	wstr_msg = GAME_RCSTRING("STBL_IDX_CLAN_BATTLE_ONLY_LEADER");		break;
		case FAIL_NOTFOUND	:	wstr_msg = GAME_RCSTRING("STR_POPUP_MESSAGE_CANNOT_FIND_USER");		break;
		case FAIL_MERCEMAX  :	wstr_msg = GAME_RCSTRING("STR_MERC_FULL_INVITE_FAIL");		break;
		}
	}

	GameUI::MsgBox(type, wstr_msg, nullptr, MAKE_CALLBACK(call_fn), this);
}

void UIFloatMercenaryList::ClickCancel(void)
{
	if(UserInfoContext::i()->GetMyClanState() == CLAN_MEMBER_LEVEL_UNKNOWN)
	{
		/* 용병 신청을 취소하면 채널리스트로 복귀합니다. 취소하시겠습니까? */
		GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_CLANCHANNEL_MERCENARY_CANCEL"),nullptr, MAKE_CALLBACK(UIFloatMercenaryList::CbRemoveMercenary), this);
	}
	else
		GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_REMOVE_MERCENARY);
}

void UIFloatMercenaryList::Register(void)
{
	GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_REGISTER_MERCENARY);
}

void UIFloatMercenaryList::Invite(void)
{
	const CLAN_MERCENARY_INFO * pInfo = ClanGameContext::i()->getClanMercenaryInfo(m_i32SelectedIdx);
	if(pInfo != nullptr)
	{
		T_MerID TMercenaryID = pInfo->getMecenaryID();
		GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_SEND_INVITE_MERCENARY, &TMercenaryID);
	}
}

void UIFloatMercenaryList::DetailInfo(void)
{
	i3::rc_wstring wstrName = UserInfoContext::i()->GetMyNickName();
	const CLAN_MERCENARY_INFO* pInfo = ClanGameContext::i()->getClanMercenaryInfo(m_i32SelectedIdx);

	if(pInfo != nullptr)
	{
		if(i3::generic_is_equal(pInfo->_wstrNickName,wstrName ))
			return; // 자기 자신은 미니팝업을 띄우지 않음.. 

		T_MerID TMercenaryID = pInfo->getMecenaryID();
		ClanGameContext::i()->SetSelectedMerID(m_i32SelectedIdx);
		GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_MERCENARY_DETAIL_INFO, &TMercenaryID);
	}
}

void UIFloatMercenaryList::AcceptInvite(void)
{
	m_SecCount.Stop();
	GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_ACCEPT_INVITE);
}

void UIFloatMercenaryList::DenialInvite(void)
{
	m_SecCount.Stop();
	GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_DENIAL_INVITE);
}

const bool UIFloatMercenaryList::isLobbyPhase(void)
{ 
	return (GetMainFrame()->GetCurrentPhaseType() == UIPHASE_LOBBY);
}

const bool UIFloatMercenaryList::isClanMember(void)
{
	USER_INFO_BASIC MyUserInfo;
	UserInfoContext::i()->GetMyUserInfoBasic( &MyUserInfo );

	return (MyUserInfo.m_ui32Clanidx != 0);
}

void UIFloatMercenaryList::_SetFramePos(void)
{
	if (!isEnable())
		return;

	VEC3D* pos = m_pMainFmw->getPos();
	VEC2D* size = m_pMainFmw->getSize();

	if(g_pEnvSet->m_nUIVersion == UFV_1_5)
	{
		pos->x = GameUI::AutoPosition::get_max_right() - 637.f - size->x;
		m_pMainFmw->setPos( pos );	
	}
	else if(g_pEnvSet->IsBaseOnUIVersion_1_0())
	{	
		const VEC3D* pBenchmark_pos = nullptr;
		UIPhaseClanLobby* p_clanlobby = ((UIPhaseClanLobby*)GetMainFrame()->GetPhase(UIPHASE_CLANLOBBY));
		UIPhaseLobby_V10* p_lobby     = ((UIPhaseLobby_V10*)GetMainFrame()->GetPhase(UIPHASE_LOBBY));
		
		if (p_clanlobby && p_clanlobby->isEnable())
			pBenchmark_pos = p_clanlobby->GetMainFrameWndPos();
		else
		if (p_lobby && p_lobby->isEnable())
			pBenchmark_pos = p_lobby->GetMainFrameWndPos();

		if (pBenchmark_pos != nullptr)
			pos->x = pBenchmark_pos->x - size->x;
		
		m_pMainFmw->setPos( pos );
	}
}

void	UIFloatMercenaryList::OnSecCount_Start(INT32 result_time)
{
}

void	UIFloatMercenaryList::OnSecCount_Change(INT32 result_time)
{
}

void	UIFloatMercenaryList::OnSecCount_DoneOrStop(INT32 result_time)
{
	if(m_InviteMsgBox != nullptr)
	{
		if(!m_InviteMsgBox->IsClosing())
		{
			GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_DENIAL_INVITE);
			GameUI::CloseMsgBox(m_InviteMsgBox);
		}
	}
}

/************************************************************************/
/*						Event Receive CallBack Func						*/
/************************************************************************/

void UIFloatMercenaryList::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UIFloatMercenaryList::ReceiveEvent_ClanWar_GetMercenaryList( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg) )
	{
		_UpdateMercenaryList();
	}
}

void UIFloatMercenaryList::ReceiveEvent_ClanWar_RegisterMercenary( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg) )
	{
		ClanGameContext::i()->setMyMercenaryState(true);
		_SetRequestButtonEnable();

		g_mercenary_update_time = 0.f;
		GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_GET_MERCENARY_LIST);
	}
	else
	{
		if (arg == EVENT_ERROR_CLAN_WAR_MERCENAY_DIS_TIME)
		{
			GameUI::MsgBox(MSG_TYPE_ERROR, GAME_RCSTRING("STR_MERC_BATTLE_ESCAPE"));
		}
	}
}

void UIFloatMercenaryList::ReceiveEvent_ClanWar_RemoveMercenary( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg) )
	{
		ClanGameContext::i()->setMyMercenaryState(false);
		_SetRequestButtonEnable();

		g_mercenary_update_time = 0.f;
		GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_GET_MERCENARY_LIST);
	}
}

void UIFloatMercenaryList::ReceiveEvent_ClanWar_SendInviteMercenary( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg) )
	{
		const CLAN_MERCENARY_INFO* pInfo = ClanGameContext::i()->getClanMercenaryInfo(m_i32SelectedIdx);
		const i3::rc_wstring wstrName = pInfo->getNickName();
		if(pInfo != nullptr)
		{
			i3::stack_wstring wstr_msg;
			i3::sprintf(wstr_msg, GAME_RCSTRING("STR_POPUP_MESSAGE_SEND_INVITE_MESSAGE"), wstrName);
			GameUI::MsgBox(MSG_TYPE_GAME_OK, wstr_msg);
		}
	}
	else
	{
		if (arg == EVENT_ERROR_CLAN_WAR_MERCENAY_DIS_TIME)
		{
			GameUI::MsgBox(MSG_TYPE_ERROR, GAME_RCSTRING("STR_MERC_BATTLE_ESCAPE"));
		}
	}
}

void UIFloatMercenaryList::ReceiveEvent_ClanWar_RecvInviteMercenary( INT32 arg, const i3::user_data & data)
{
	if( EV_SUCCESSED(arg) )
	{
		const i3::rc_wstring & wstr_clan = i3::value_cast<i3::rc_wstring>(data);
		
		i3::rc_wstring wstr_msg = L"";
		i3::sprintf(wstr_msg,  GAME_RCSTRING("STR_MERC_INVITE_POPUP"), wstr_clan);

		m_InviteMsgBox = GameUI::MsgBox(MSG_TYPE_GAME_QUERY, wstr_msg, nullptr, MAKE_CALLBACK(callback::mercenary_invite_response), this);
		m_SecCount.Start();
	}
}

void UIFloatMercenaryList::ReceiveEvent_ClanWar_AcceptInvite( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg) )
	{
		//@eunil. PBBUG-16639 유저리스트는 업데이트 하지만 버튼은 UIPhase 구분이 걸려있어서 타이밍이 애매하다
		//정확한 이벤트 방식으로 업데이트 해준다.
		LuaState * L = _CallLuaFunction("SetRequestBtnEnable");
		if (L != nullptr)
		{
			i3Lua::PushBoolean(L, true);
			_EndLuaFunction(L, 1);
		}

		ClanGameContext::i()->setClanBattleLobbyEnable(true);
		GetMainFrame()->MovePhase(UIPHASE_CLANLOBBY);
	}
	else
	{
		GameUI::MsgBox(MSG_TYPE_ERROR, GAME_RCSTRING("STR_MERC_JOIN_FAIL_MESSAGE"));
	}
}

void UIFloatMercenaryList::ReceiveEvent_ClanWar_DenialInvite( INT32 arg, const i3::user_data & data)
{
	if( EV_SUCCESSED(arg) )
	{
		// 용병이 거부하였는지는 방장만 알아도 됨
		if(!ClanGameContext::i()->isMatchTeamLeader() ) return;

		const T_MerID & MercenaryID = i3::value_cast<T_MerID>(data);
		const CLAN_MERCENARY_INFO * pInfo = ClanGameContext::i()->getClanMercenaryInfo(MercenaryID);

		if( pInfo != nullptr )
		{
			i3::rc_wstring wstr_msg = L"";
			i3::sprintf( wstr_msg, GAME_RCSTRING("STR_MERC_INVITE_REFUSE_POPUP"), pInfo->getNickName());
			GameUI::MsgBox(MSG_TYPE_GAME_OK, wstr_msg);
		}
	}
}

void UIFloatMercenaryList::ReceiveEvent_ClanWar_ModifyMatchTeamInfo( INT32 arg, const i3::user_data&)
{
	_SetRequestButtonEnable();
	_SetButtonActivate();
}

void UIFloatMercenaryList::ReceiveEvent_ClanWar_GetMercenaryDetail ( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg) )
	{
		ClanGameContext::i()->SetMercDetailInfo();
		if(g_pEnvSet->m_nUIVersion == UFV_1_5)
			GetMainFrame()->OpenPopup(UPW_USERPROFILE, nullptr);
		else 
		{
			GetMainFrame()->ResetUserActionPopup();
			GetMainFrame()->AddUserActionCommand(UCM_PROFILE);
			GetMainFrame()->AddUserActionCommand(UCM_ADD_FRIEND);
			GetMainFrame()->AddUserActionCommand(UCM_WHISPER);

			if (UserInfoContext::i()->GetMyClanState() == CLAN_MEMBER_LEVEL_MASTER ||
				UserInfoContext::i()->GetMyClanState() == CLAN_MEMBER_LEVEL_STAFF)
				GetMainFrame()->AddUserActionCommand(UCM_CLAN_INVITE);

			GetMainFrame()->OpenUserActionPopup(this, m_i32SelectedIdx);
		}
	}
}

void UIFloatMercenaryList::CbRemoveMercenary(void* pThis,INT32 nParam)
{
	if(nParam == MB_OK)
	{
		GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_REMOVE_MERCENARY);
	}
}
