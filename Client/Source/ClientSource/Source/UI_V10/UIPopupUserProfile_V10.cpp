#include "pch.h"
#include "UIPopupUserProfile_V10.h"

#include "CommunityContext.h"
#include "../UI/UIMainFrame_V10.h"
#include "../UI/UIUtil.h"
#include "GameContextUtil.h"
#include "UserInfoContext.h"
#include "ClanGameContext.h"
#include "Record.h"

I3_CLASS_INSTANCE( UIPopupUserProfile_V10);//, UIPopupBase);

static UIPopupUserProfile_V10 * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);
	return (UIPopupUserProfile_V10*) pFrame->GetPopup( UPW_USERPROFILE );
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//

extern "C" {

	int UIPopupUserProfile_V10_Close(LuaState * L)
	{
		UIPopupUserProfile_V10 * userProfile = GetThis();
		if( userProfile != nullptr)
			userProfile->Close();

		return 0;
	}

	int UIPopupUserProfile_V10_Accpet(LuaState * L)
	{
		UIPopupUserProfile_V10 * userProfile = GetThis();
		if( userProfile != nullptr)
			userProfile->Accpet();

		return 0;
	}

	int UIPopupUserProfile_V10_Visit(LuaState * L)
	{
		UIPopupUserProfile_V10 * userProfile = GetThis();
		if( userProfile != nullptr)
			userProfile->VIsit();

		return 0;
	}

	int UIPopupUserProfile_V10_Invite(LuaState * L)
	{
		UIPopupUserProfile_V10 * userProfile = GetThis();
		if( userProfile != nullptr)
			userProfile->Invite();

		return 0;
	}

	int UIPopupUserProfile_V10_Delete(LuaState * L)
	{
		UIPopupUserProfile_V10 * userProfile = GetThis();
		if( userProfile != nullptr)
			userProfile->Delete();

		return 0;
	}

	int UIPopupUserProfile_V10_Refuse(LuaState * L)
	{
		UIPopupUserProfile_V10 * userProfile = GetThis();
		if( userProfile != nullptr)
			userProfile->Refuse();

		return 0;
	}

	int UIPopupUserProfile_V10_AddFriend(LuaState * L)
	{
		UIPopupUserProfile_V10 * userProfile = GetThis();
		if( userProfile != nullptr)
			userProfile->AddFriend();

		return 0;
	}

	int UIPopupUserProfile_V10_TabClick(LuaState * L)
	{
		INT32 tab = i3Lua::GetIntegerArg(L, 1);
		UIPopupUserProfile_V10 * userProfile = GetThis();
		if (userProfile != nullptr)
			userProfile->TabClick(tab);

		return 0;
	}
}

LuaFuncReg	UIPopupUserProfile_V10_Glue[] =
{
	{ "Close",			UIPopupUserProfile_V10_Close },
	{ "Accpet",			UIPopupUserProfile_V10_Accpet },
	{ "Visit",			UIPopupUserProfile_V10_Visit },
	{ "Invite",			UIPopupUserProfile_V10_Invite },
	{ "Delete",			UIPopupUserProfile_V10_Delete },
	{ "Refuse",			UIPopupUserProfile_V10_Refuse },
	{ "AddFriend",		UIPopupUserProfile_V10_AddFriend },
	{ "TabClick",		UIPopupUserProfile_V10_TabClick },
	{ nullptr,			nullptr }
};

UIPopupUserProfile_V10::UIPopupUserProfile_V10() : text_NickName_(nullptr), text_Position_(nullptr), Buttonidx_(0)
, basicInfo_(false), stateInfo_(false), static_textPosition_(nullptr), ViewFriendIdx_(-1), tab_Clan_(nullptr), tab_Person_(nullptr)
, btn_Refuse_(nullptr), btn_AddFriend_(nullptr), static_Season_(nullptr), static_Total_(nullptr), Clantabidx_(-1), m_bClanMercenary(false)
{
	vecSeason.reserve(STATIC_TEXT_TOTAL);
	vecTotal.reserve(STATIC_TEXT_TOTAL);
}

UIPopupUserProfile_V10::~UIPopupUserProfile_V10()
{

}

void UIPopupUserProfile_V10::OnCreate( i3GameNode * pParent )
{
	UIPopupBase::OnCreate(pParent);

	// Load Scene
	AddScene("Scene/Popup/PBRe_PopUp_UserDetailInfo.i3UIs", UIPopupUserProfile_V10_Glue);
}

void UIPopupUserProfile_V10::_InitializeAtLoad( i3UIScene * pScene )
{
	UIPopupBase::_InitializeAtLoad(pScene);
	
	// 설정할 텍스트 컨트롤
	i3::string text;
	for(INT32 i = 0; i < STATIC_TEXT_TOTAL; i++)
	{
		i3::sprintf(text, "Season%d", i);
		i3UIStaticText * temp = (i3UIStaticText*)pScene->FindChildByName(text.c_str());
		vecSeason.push_back(temp);

		i3::sprintf(text, "Total%d", i);
		temp = (i3UIStaticText*)pScene->FindChildByName(text.c_str());
		vecTotal.push_back(temp);
	}
	
	text_NickName_ = (i3UIStaticText *) pScene->FindChildByName("NickName");
	text_Position_ = (i3UIStaticText *) pScene->FindChildByName("Position");
	static_textPosition_ = (i3UIStaticText *) pScene->FindChildByName("Static_Position");
	static_Season_ = (i3UIStaticText *)pScene->FindChildByName("Static_Season");
	static_Total_ = (i3UIStaticText *)pScene->FindChildByName("Static_Total");

	// 탭
	Clantabidx_ = 0;
	tab_Clan_ = (i3UITab *)pScene->FindChildByName("Tab_Clan");
	if (tab_Clan_ != nullptr)
	{
		tab_Clan_->SetText(GAME_STRING("STR_TBL_PROFILE_CLAN"));
	}

	tab_Person_ = (i3UITab *)pScene->FindChildByName("Tab_Person");
	if (tab_Person_ != nullptr)
		tab_Person_->SetText(GAME_STRING("STR_POPUP_PRIVATE"));

	// 버튼들
	btn_Accpet_ = (i3UIButtonComposed3 *) pScene->FindChildByName("Btn_Accpet");
	btn_Visit_ = (i3UIButtonComposed3 *) pScene->FindChildByName("Btn_Visit");
	btn_Invite_ = (i3UIButtonComposed3 *) pScene->FindChildByName("Btn_Invite");
	btn_Delete_ = (i3UIButtonComposed3 *) pScene->FindChildByName("Btn_Delete");
	btn_Refuse_ = (i3UIButtonComposed3 *) pScene->FindChildByName("Btn_Refuse");
	btn_AddFriend_ = (i3UIButtonComposed3 *) pScene->FindChildByName("Btn_AddFriend");
	if(btn_AddFriend_ != nullptr)
	{
		addFriendBasePos_ = *(btn_AddFriend_->getPos());
	}
}

void UIPopupUserProfile_V10::_ClearAtUnload( i3UIScene * pScene )
{
	UIPopupBase::_ClearAtUnload(pScene);
	vecSeason.clear();
	vecTotal.clear();

	text_NickName_ = nullptr;
	text_Position_ = nullptr;
	static_textPosition_ = nullptr;

	btn_Accpet_ = nullptr;
	btn_Visit_ = nullptr;
	btn_Invite_ = nullptr;
	btn_Delete_ = nullptr;
	btn_Refuse_ = nullptr;
	btn_AddFriend_ = nullptr;

	tab_Clan_ = nullptr;
	tab_Person_ = nullptr;

	Buttonidx_ = 0;
	Clantabidx_ = 0;
}

bool UIPopupUserProfile_V10::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/ )
{
	if( UIPopupBase::OnEntranceStart(pArg1, pArg2) == false)
		return false;

	// 잠시 고정
	Buttonidx_ = *static_cast<INT32*>(pArg1);
	if(Buttonidx_ != BUTTONSTATE_CLAN)
		ViewFriendIdx_ = __GetViewFriendIdx();
	else
		ViewFriendIdx_ = __GetViewClanIdx();

	if (ClanGameContext::i()->IsExistMercDetailInfo())
	{
		m_bClanMercenary = true;
		//ClanGameContext::i()->ResetMercDetailInfo();
	}
	else
		m_bClanMercenary = false;

	_SetButton(Buttonidx_);

	// 초기 세팅합니다.
	_SetInfoText();

	return true;
}

// 종료 
void UIPopupUserProfile_V10::Close()
{
	ClanGameContext::i()->ResetMercDetailInfo();
	GetMainFrame()->TogglePopup(UPW_USERPROFILE);
}

// 친구 수락
void UIPopupUserProfile_V10::Accpet()
{
	// 수락합니다.
	GameUI::AcceptFriend(ViewFriendIdx_);
	Close();
}

// 찾아가기
void UIPopupUserProfile_V10::VIsit()
{
	// View Index로 찾아갑니다.
	if (Buttonidx_ == BUTTONSTATE_CLAN)
		GameUI::FindAwayClan(ViewFriendIdx_);
	else
		GameUI::FindAwayFriend(ViewFriendIdx_);

	Close();
}

// 초대하기
void UIPopupUserProfile_V10::Invite()
{
	// 1.0 초대하기에는 클랜 멤버가 안들어오므로 무조건 친구로 합니다.
	// 아직은 무조건 친구입니다.
	// 클랜 탭이 있을 경우 클랜탭으로 합니다.
	// 커뮤니티가 만들어지면 세팅합니다 하핫
	if (Buttonidx_ == BUTTONSTATE_CLAN)
		GameUI::InviteClanMember(ViewFriendIdx_);
	else
	{
		if (m_bClanMercenary)
		{
			T_MerID mID = ClanGameContext::i()->GetSelectedMerID();
			GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_SEND_INVITE_MERCENARY, &mID);
		}
		else
		{
			GameUI::InviteFriend(USERLIST_TAB_FRIEND, ViewFriendIdx_);
		}
	}
	Close();
}

// 친구 삭제
void UIPopupUserProfile_V10::Delete()
{
	// 삭제 합니다.
	GameUI::RemoveFriend(ViewFriendIdx_);
	Close();
}

// 친구 거절
void UIPopupUserProfile_V10::Refuse()
{
	// 콜백으로 삭제를 해야 할 거 같습니다.
	GameUI::RefuseFriend(ViewFriendIdx_, MAKE_CALLBACK(CbDeleteFriend), this);
}

// 친구 추가
void UIPopupUserProfile_V10::AddFriend()
{
	i3::wstring wnick = CCommunityContext::i()->GetCommnunityInfoNick();
	if(wnick.empty())
	{
		I3PRINTLOG(I3LOG_NOTICE, "nick Value is nullptr");
	}

	GameUI::AddFriend(wnick);
	Close();
}

void UIPopupUserProfile_V10::TabClick(INT32 idx) 
{
	// 어떤 탭을 눌렀는지 기억합니다.
	Clantabidx_ = idx;

	// 다시 세팅
	_SetButton(Buttonidx_);
	_SetInfoText();
}

// 초기 버튼 설정
void UIPopupUserProfile_V10::_SetButton(INT32 idx)
{
	__SetButtonDefault();

	// INT로 넘어온 것을 분기로 나눕니다.
	switch(idx)
	{
		// 1번 유저리스트
	case BUTTONSTATE_USERLIST:
		__SetButtonUserList();
		break;
		// 2번 친구 목록
	case BUTTONSTATE_FIREND:
		__SetButtonFriendList();
		break;
		// 3번 유저 검색으로 찾기
	case BUTTONSTATE_SEARCHUSER:
		__SetButtonSearchUser();
		break;
		// 4번 클랜원 보기
	case BUTTONSTATE_CLAN:
		__SetButtonClanList();
		break;
	default:
		I3TRACE("[1.0]UserInfo call wrong Tab..\n");
		break;
	}
}

// 정보 세팅
void UIPopupUserProfile_V10::_SetInfoText()
{
	const USER_DETAIL_INFO * pInfo = CCommunityContext::i()->GetUserDetailInfo();
	
	// 닉네임
	text_NickName_->SetText(CCommunityContext::i()->GetCommnunityInfoNick());

	// 시즌 정보
	__SetSeasonRecord(&pInfo->_UserInfo.m_UserInfoRecord);

	// 토탈 정보
	__SetTotalRecord(&pInfo->_UserInfo.m_UserInfoRecord);
	
	if(Buttonidx_ == BUTTONSTATE_USERLIST)
	{
		static_textPosition_->SetText("");
		text_Position_->SetText("");
	}
	else if (Buttonidx_ == BUTTONSTATE_CLAN)
	{
		static_textPosition_->SetText(GAME_RCSTRING("STR_POPUP_CUR_POSITION"));

		i3::wstring wstrText;
		UINT64 ClanState = __GetIsClanState();
		GameUI::GetTextClanState(wstrText, ClanState);
		text_Position_->SetText(wstrText);
	}
	else
	{
		static_textPosition_->SetText(GAME_RCSTRING("STR_POPUP_CUR_POSITION"));
		
		i3::wstring wstrText;
		INT32 friendState = __GetIsFriendState();
		GameUI::GetTextFriendState(wstrText, friendState);
		text_Position_->SetText(wstrText);
	}
}

void UIPopupUserProfile_V10::__SetSeasonRecord( const USER_INFO_RECORD * record )
{
	i3::rc_wstring wstrText;

	// 전적 Title 부터 시작합니다.
	// 클랜일 경우
	if (Buttonidx_ == BUTTONSTATE_CLAN)
	{
		if(Clantabidx_ == 0)
			wstrText = GAME_RCSTRING("STR_POPUP_CLAN_SEASON_HISTORY");
		else
			wstrText = GAME_RCSTRING("STR_POPUP_PRIVATE_SEASON_HISTORY");
	}
	else
	{
		if (ClanGameContext::i()->IsExistMercDetailInfo())
			wstrText = GAME_RCSTRING("STR_IDX_MERCENARY_SEASON_SCORE");
		else
			wstrText = GAME_RCSTRING("STR_TBL_GUI_READY_SEASON_BATTLE_HISTORY");
	}
	
	static_Season_->SetText(wstrText);
	
	// 시즌 전적 전 무 패
	i3::sprintf(wstrText, L"%d%s %d%s %d%s", record->_smatch, GAME_RCSTRING("STBL_IDX_BATTLE_COUNT"),
											 record->_swin, GAME_RCSTRING("STBL_IDX_BATTLE_WIN_COUNT"),
											 record->_slose, GAME_RCSTRING("STBL_IDX_BATTLE_LOSE_COUNT") );
	vecSeason[STATIC_SCORE]->SetText(wstrText);

	// 시즌 킬 Rate
	INT32 killrate = GameContextUtil::getKillRate(record->_skillcount, record->_sdeath);
	i3::sprintf(wstrText, L"%s %d/%d/%d(%d%%)", GAME_RCSTRING("STR_TBL_PROFILE_KILL_DEATH_RATE3"),
										record->_skillcount, record->_sAssistCount, record->_sdeath, killrate);
	vecSeason[STATIC_KILLDEATH]->SetText(wstrText);

	// 시즌 헤드샷 비율
	INT32 headshot = SeasonHeadShotPercent(record);
	i3::sprintf(wstrText, GAME_RCSTRING("STR_TBL_PROFILE_HEADSHOT_RATE_MODE1"), headshot);
	vecSeason[STATIC_HEADSHOT]->SetText(wstrText);
}

void UIPopupUserProfile_V10::__SetTotalRecord( const USER_INFO_RECORD * record )
{
	i3::rc_wstring wstrText;

	// 전적 Title 부터 시작합니다.
	// 클랜일 경우
	if (Buttonidx_ == BUTTONSTATE_CLAN)
	{
		if (Clantabidx_ == 0)
			wstrText = GAME_RCSTRING("STR_POPUP_CLAN_TOTAL_HISTORY");
		else
			wstrText = GAME_RCSTRING("STR_POPUP_PRIVATE_TOTAL_HISTORY");
	}
	else
	{
		if (ClanGameContext::i()->IsExistMercDetailInfo())
			wstrText = GAME_RCSTRING("STR_IDX_MERCENARY_TOTAL_SCORE");
		else
			wstrText = GAME_RCSTRING("STR_TBL_GUI_READY_TOTAL_BATTLE_HISTORY");
	}
		

	static_Total_->SetText(wstrText);

	// 토탈 전적 전 무 패
	i3::sprintf(wstrText, L"%d%s %d%s %d%s", record->_match, GAME_RCSTRING("STBL_IDX_BATTLE_COUNT"),
											 record->_win, GAME_RCSTRING("STBL_IDX_BATTLE_WIN_COUNT"),
											 record->_lose, GAME_RCSTRING("STBL_IDX_BATTLE_LOSE_COUNT") );
	vecTotal[STATIC_SCORE]->SetText(wstrText);

	// 토탈 킬 Rate
	INT32 killrate = GameContextUtil::getKillRate(record->_killcount, record->_death);
	i3::sprintf(wstrText, L"%s %d/%d/%d(%d%%)", GAME_RCSTRING("STR_TBL_PROFILE_KILL_DEATH_RATE3"),
		record->_killcount, record->_AssistCount, record->_death, killrate);
	vecTotal[STATIC_KILLDEATH]->SetText(wstrText);

	// 토탈 헤드샷 비율
	INT32 headshot = TotalHeadShotPercent(record);
	i3::sprintf(wstrText, GAME_RCSTRING("STR_TBL_PROFILE_HEADSHOT_RATE_MODE1"), headshot);
	vecTotal[STATIC_HEADSHOT]->SetText(wstrText);
}

void UIPopupUserProfile_V10::__SetButtonSearchUser()
{	
	if(btn_Delete_ != nullptr)
		btn_Delete_->EnableCtrl(false);

	if(btn_Accpet_ != nullptr)
		btn_Accpet_->EnableCtrl(false);

	if(btn_Refuse_ != nullptr)
		btn_Refuse_->EnableCtrl(false);

	if(btn_Visit_ != nullptr)
		btn_Visit_->disable_btn();

	if(btn_Invite_ != nullptr)
		btn_Invite_->disable_btn();

	if(btn_AddFriend_ != nullptr)
	{
		VEC3D * pos_ = btn_Accpet_->getPos();
		btn_AddFriend_->setPos(pos_);

		if( GameUI::IsCommunityFriend(CCommunityContext::i()->GetCommnunityInfoNick()) == true || 
			i3::generic_is_equal(UserInfoContext::i()->GetMyNickName(), CCommunityContext::i()->GetCommnunityInfoNick()) == true)
		{
			btn_AddFriend_->disable_btn();
		}
	}
}

void UIPopupUserProfile_V10::__SetButtonClanList()
{
	// 탭 오픈
	if (tab_Person_ != nullptr)
		tab_Person_->SetVisible(true);

	if (tab_Clan_ != nullptr)
	{
		tab_Clan_->SetVisible(true);
		tab_Clan_->SetPushFlag(true);
	}

	// 친구 추가는 무조건 꺼야 합니다.
	if (btn_AddFriend_ != nullptr)
		btn_AddFriend_->EnableCtrl(false);

	// 클랜에서는 수락 / 거절 / 삭제는 없습니다.
	if (btn_Accpet_ != nullptr)
		btn_Accpet_->EnableCtrl(false);

	if (btn_Refuse_ != nullptr)
		btn_Refuse_->EnableCtrl(false);

	if (btn_Delete_ != nullptr)
		btn_Delete_->EnableCtrl(false);

	UINT64 friendState = __GetIsClanState();
	if (friendState != 0)
	{
		bool visit_disable = false;			// 찾아가기 버튼 켜지는지
		bool invite_disable = false;		// 초대하기 버튼 켜지는지

		switch (GET_FRIEND_STATE(friendState))
		{
			// Online / Offline 둘 다 찾아가기 초대하기 전부 안됩니다.
		case FRIEND_STATE_OFFLINE:
			invite_disable = true;
		case FRIEND_STATE_ONLINE:
		case FRIEND_STATE_LOBBY:
			{
				visit_disable = true;
			} break;
		case FRIEND_STATE_ROOM:
			{
				if (GetMainFrame()->GetCurrentPhaseType() != UIPHASE_READY_ROOM)
				{
					invite_disable = true;
				}
			} break;
		}

		// 버튼 Enable 설정
		if (visit_disable)
		{
			btn_Visit_->disable_btn();
		}

		if (invite_disable)
		{
			btn_Invite_->disable_btn();
		}
	}
}

void UIPopupUserProfile_V10::__SetButtonUserList()
{
	if(btn_Delete_ != nullptr)
		btn_Delete_->EnableCtrl(false);

	if(btn_Visit_ != nullptr)
		btn_Visit_->EnableCtrl(false);
	
	if (btn_Invite_ != nullptr)
	{
		if (m_bClanMercenary && ClanGameContext::i()->isMatchTeamLeader())
		{
			btn_Invite_->SetText(GAME_RCSTRING("STR_MERC_INVITE_BTN"));
		}
		else
		{
			btn_Invite_->SetText(GAME_RCSTRING("STR_TBL_GUI_ETC_CONTEXT_INVITE"));
			btn_Invite_->EnableCtrl(false);
		}
	}

	if(btn_Accpet_ != nullptr)
		btn_Accpet_->EnableCtrl(false);
	
	if (btn_Refuse_ != nullptr)
		btn_Refuse_->EnableCtrl(false);

	if(btn_AddFriend_ != nullptr)
	{
		if( GameUI::IsCommunityFriend(CCommunityContext::i()->GetCommnunityInfoNick()) == true || 
			i3::generic_is_equal(UserInfoContext::i()->GetMyNickName(), CCommunityContext::i()->GetCommnunityInfoNick()) == true)
		{
			btn_AddFriend_->disable_btn();
		}
	}
}

void UIPopupUserProfile_V10::__SetButtonFriendList()
{
	INT32 friendState = __GetIsFriendState();
	bool visit_disable = false;			// 찾아가기 버튼 켜지는지
	bool invite_disable = false;		// 초대하기 버튼 켜지는지

	bool isfriend = false;

	if( friendState != 0)
	{
		switch(GET_FRIEND_STATE(friendState))
		{
		case FRIEND_STATE_REQUEST:
				isfriend = true;
		case FRIEND_STATE_HOLD:
			{
				visit_disable = true;
				invite_disable = true;
			} break;
		case FRIEND_STATE_OFFLINE:
				invite_disable = true;
		case FRIEND_STATE_ONLINE:		
		case FRIEND_STATE_LOBBY:
			{
				isfriend = true;
				visit_disable = true;
			} break;
		case FRIEND_STATE_ROOM:
			{
				if( GetMainFrame()->GetCurrentPhaseType() != UIPHASE_READY_ROOM)
				{
					invite_disable = true;
				}
				isfriend = true;
			} break;
		}	

		// 버튼 Enable 설정
		if(visit_disable)
		{
			btn_Visit_->disable_btn();
		}

		if(invite_disable)
		{
			btn_Invite_->disable_btn();
		}

		// 친구 인지 아닌지 검사 후 버튼 켜고 꺼야 합니다.
		if( isfriend)
		{
			btn_Accpet_->EnableCtrl(false);
			btn_Refuse_->EnableCtrl(false);
		}
		else
		{
			btn_Delete_->EnableCtrl(false);
		}

		// 친구 추가는 무조건 꺼야 합니다.
		if(btn_AddFriend_ != nullptr)
			btn_AddFriend_->EnableCtrl(false);
	}
}

void UIPopupUserProfile_V10::__SetButtonDefault()
{
	if(btn_Delete_ != nullptr)
	{
		btn_Delete_->EnableCtrl(true);
	}

	if(btn_Visit_ != nullptr)
	{
		btn_Visit_->enable_btn();
		btn_Visit_->EnableCtrl(true);
	}

	if(btn_Invite_ != nullptr)
	{
		btn_Invite_->enable_btn();
		btn_Invite_->EnableCtrl(true);
	}
	
	if(btn_Accpet_ != nullptr)
	{
		btn_Accpet_->EnableCtrl(true);
	}

	if(btn_Refuse_ != nullptr)
	{
		btn_Refuse_->EnableCtrl(true);
	}

	if(btn_AddFriend_ != nullptr)
	{
		btn_AddFriend_->enable_btn();
		btn_AddFriend_->EnableCtrl(true);
		btn_AddFriend_->setPos(&addFriendBasePos_);
	}

	if (tab_Clan_ != nullptr)
	{
		tab_Clan_->SetVisible(false);
	}

	if (tab_Person_ != nullptr)
	{
		tab_Person_->SetVisible(false);
	}
}

void UIPopupUserProfile_V10::ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData )
{
	UIPopupBase::ProcessGameEvent(event, Arg, UserData);
	
	switch(event)
	{
	case EVENT_GET_USERINFO_SESSION:
	case EVENT_GET_USERINFO_SUBTASK:
		// 정보가 들어와야 세팅 할 수 있다.
		_SetButton(Buttonidx_);
		_SetInfoText();
		break;

	default:
		break;
	}
}

UINT32 UIPopupUserProfile_V10::__GetIsFriendState( )
{
	i3::wstring wnick = CCommunityContext::i()->GetCommnunityInfoNick();
	if(wnick.empty())
	{
		I3PRINTLOG(I3LOG_NOTICE, "nick Value is nullptr");
	}

	UINT32 i = 0;
	for( i = 0; i < CCommunityContext::i()->GetFriendCount(); i++)
	{
		const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriend(i);
		if( node != nullptr)
		{
			if( i3::generic_is_iequal( wnick, node->_wstr_nick) )
			{
				return node->_state;
			}
		}
	}

	return 0;
}

INT32 UIPopupUserProfile_V10::__GetViewFriendIdx()
{
	i3::wstring wnick = CCommunityContext::i()->GetCommnunityInfoNick();
	if(wnick.empty())
	{
		I3PRINTLOG(I3LOG_NOTICE, "nick Value is nullptr");
	}

	UINT32 i = 0;
	for( i = 0; i < CCommunityContext::i()->GetFriendCount(); i++)
	{
		const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriend(i);
		if( node != nullptr)
		{
			if( i3::generic_is_iequal( wnick, node->_wstr_nick) )
			{
				return i;
			}
		}
	}

	return -1;
}

UINT64 UIPopupUserProfile_V10::__GetIsClanState()
{
	i3::wstring wnick = CCommunityContext::i()->GetCommnunityInfoNick();
	if (wnick.empty())
	{
		I3PRINTLOG(I3LOG_NOTICE, "nick Value is nullptr");
	}

	UINT32 i = 0;
	for (i = 0; i < CCommunityContext::i()->GetClanMemberCount(); i++)
	{
		const CLAN_MEMBER_NODE* node = CCommunityContext::i()->GetViewClanMember(i);
		if (node != nullptr)
		{
			if (i3::generic_is_iequal(wnick, node->_wstr_nick))
			{
				return node->_state;
			}
		}
	}

	return 0;
}

INT32 UIPopupUserProfile_V10::__GetViewClanIdx()
{
	i3::wstring wnick = CCommunityContext::i()->GetCommnunityInfoNick();
	if (wnick.empty())
	{
		I3PRINTLOG(I3LOG_NOTICE, "nick Value is nullptr");
	}

	UINT32 i = 0;
	for (i = 0; i < CCommunityContext::i()->GetClanMemberCount(); i++)
	{
		const CLAN_MEMBER_NODE* node = CCommunityContext::i()->GetViewClanMember(i);
		if (node != nullptr)
		{
			if (i3::generic_is_iequal(wnick, node->_wstr_nick))
			{
				return i;
			}
		}
	}

	return -1;
}

void UIPopupUserProfile_V10::__SetRoomPosition()
{
}

void UIPopupUserProfile_V10::CbDeleteFriend( void* pThis,INT32 nParam )
{
	if (nParam == MBR_OK)
	{
		UIPopupUserProfile_V10* userinfo = (UIPopupUserProfile_V10*)pThis;

		if (false == CCommunityContext::i()->DeleteViewFriend( userinfo->GetViewFriend()))
		{
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_FRINED_TO_DELETE"));/*삭제할 친구를 선택하셔야합니다.*/
		}

		userinfo->Close();
	}
}
