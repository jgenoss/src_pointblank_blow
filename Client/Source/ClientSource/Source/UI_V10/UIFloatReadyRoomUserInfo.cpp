#include "pch.h"
#include "UIFloatReadyRoomUserInfo.h"

#include "../UI/UIMainFrame_V10.h"
#include "../UI_V10/UITopMenu_V10.h"
#include "UserInfoContext.h"
#include "CommunityContext.h"
#include "ClanGameContext.h"
#include "ChannelContext.h"
#include "GameContextUtil.h"
#include "Record.h"

I3_CLASS_INSTANCE( UIFloatReadyRoomUserInfo);

#define SLIDE_SPPED 4.0f

static UIFloatReadyRoomUserInfo * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);

	return (UIFloatReadyRoomUserInfo*) pFrame->GetFloating( UFW_READYROOM_USERINFO );
}
//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {

	int UIFloatReadyRoomUserInfo_ClickButton( LuaState * L)
	{
		GetThis()->ClickButton();
		return 0;
	}
}

LuaFuncReg UIFloatReadyRoomUserInfo_Glue[] = 
{
	{ "Click",		UIFloatReadyRoomUserInfo_ClickButton	},
	{ nullptr,			nullptr}
};

UIFloatReadyRoomUserInfo::UIFloatReadyRoomUserInfo() : static_TextTitle_(nullptr), enable_OpenClose_(true), image_Class_(nullptr)
, text_Nickname_(nullptr), text_Clanname_(nullptr), text_Clanlevel_(nullptr), image_ClanMark_(nullptr), button_ClanRankIcon_(nullptr)
{
	vec_Season_.reserve(STATIC_TEXT_MAX);
	vec_Total_.reserve(STATIC_TEXT_MAX);

	m_AlignedPos = UIFLOATING_POS_RIGHTBOTTOM;
	m_bFixedWindow = true;
}

UIFloatReadyRoomUserInfo::~UIFloatReadyRoomUserInfo()
{

}

void UIFloatReadyRoomUserInfo::OnLoadAllScenes()
{
	m_slide.AddFrameWnd(GetFirstFrameWnd(), UISlide::R_TO_L);
}

void UIFloatReadyRoomUserInfo::OnCreate( i3GameNode * pParent )
{
	UIFloatingBase::OnCreate(pParent);

	AddScene("Scene/Main/PointBlankRe_ReadyRoom_UserInfo.i3UIs", UIFloatReadyRoomUserInfo_Glue, true, false);
}

void UIFloatReadyRoomUserInfo::_InitializeAtLoad( i3UIScene * pScene )
{
	UIFloatingBase::_InitializeAtLoad(pScene);

	static_TextTitle_ = (i3UIStaticText *) pScene->FindChildByName("Static_UserInfoTitle");
	image_Class_ = (i3UIButtonImageSet *) pScene->FindChildByName("ClassIcon");
	text_Nickname_ = (i3UIStaticText *) pScene->FindChildByName("UserNickName");
	text_Clanname_ = (i3UIStaticText *) pScene->FindChildByName("ClanName");
	text_Clanlevel_ = (i3UIStaticText *) pScene->FindChildByName("ClanLevel");

	for(INT32 i = 0 ; i < STATIC_TEXT_MAX; i++)
	{
		i3::string textname; i3UIStaticText * temp;
		
		i3::sprintf(textname, "Season%d", i);
		temp = (i3UIStaticText*) pScene->FindChildByName(textname.c_str());
		vec_Season_.push_back(temp);

		i3::sprintf(textname, "Total%d", i);
		temp = (i3UIStaticText*) pScene->FindChildByName(textname.c_str());
		vec_Total_.push_back(temp);
	}

	frame_MainInfo_ = (i3UIFrameWnd *) pScene->FindChildByName("MainInfoWnd");
	frame_HideInfo_ = (i3UIFrameWnd *) pScene->FindChildByName("HideInfoWnd");

	m_ControlPos[0] = *frame_MainInfo_->getPos();
	m_ControlPos[1] = *frame_HideInfo_->getPos();

	//m_slide.AddFrameWnd(frame_MainInfo_, UISlide::R_TO_L);

	button_ClanRankIcon_ = (i3UIButtonImageSet *) pScene->FindChildByName("ClanRankIcon");
	if( button_ClanRankIcon_ != nullptr)
	{
		button_ClanRankIcon_->EnableCtrl(false);
	}
	image_ClanMark_ = (i3UIImageBoxEx *) pScene->FindChildByName("ClanMark");
	if(image_ClanMark_ != nullptr)
	{
		image_ClanMark_->EnableCtrl(false);
	}
}

bool UIFloatReadyRoomUserInfo::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/ )
{
	if(UIFloatingBase::OnEntranceStart(pArg1, pArg2) == false)
		return false;

	//_SetOpenCloseisFrame();
	_SetMyInfo();

	return true;
}

void UIFloatReadyRoomUserInfo::_SetMyInfo()
{
	//_SetOpenCloseisFrame();

	USER_INFO_BASIC myinfo;
	UserInfoContext::i()->GetMyUserInfoBasic(&myinfo);
	
	USER_INFO_INVITEM_DATA MyUserInfoInvItemData;
	UserInfoContext::i()->GetMyUserInfoInvItemData(&MyUserInfoInvItemData);

	// 계급 세팅
	if (MyUserInfoInvItemData._wAprilFoolRank != RANK_NONE)
		_SetUserClassImage(MyUserInfoInvItemData._wAprilFoolRank);
	else if (MyUserInfoInvItemData._wRank != RANK_NONE)
		_SetUserClassImage(MyUserInfoInvItemData._wRank);
	else
		_SetUserClassImage(myinfo.m_ui32Rank);

	//NickName
	text_Nickname_->SetText(myinfo.m_strNick);

	if(myinfo.m_ui32Clanidx != 0)
	{
		// Clan Name
		i3::stack_wstring wstrClan;
		i3::sprintf(wstrClan, L"{col:255,255,255,255}%s{/col}", myinfo.m_strClanName);
		text_Clanname_->SetText(wstrClan);

		// Clan Rank
		i3::wstring clanrank;
		GetClanFullName2(clanrank, myinfo.m_ui8ClanExpRank, myinfo.m_ui8ClanPerRank);
		i3::sprintf(wstrClan, L"{col:255,255,255,255}%s{/col}", clanrank);
		text_Clanlevel_->SetText(wstrClan);

		_SetClanMarkIcon(&myinfo, true);
	}
	else
	{
		i3::wstring wstrText = GAME_STRING("STR_TBL_GUI_READY_NO_JOINING");
		text_Clanname_->SetText(wstrText);
		text_Clanlevel_->SetText(wstrText);

		_SetClanMarkIcon(&myinfo, false);
	}

	const USER_INFO_RECORD * myrecord = UserInfoContext::i()->GetMyUserInfoRecord();
	if (UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW))
	{
		if (ChannelContext::i()->GetBattleChannel() &&
			g_pFramework->GetClanContext()->InBattleTeam())
		{
			CLAN_PERSONAL_MATCH_RECORD* pMatchRecord = ClanGameContext::i()->getMathRecord(m_SelectedSlot);
			if (pMatchRecord != nullptr)
			{
				_SetClanMatchRecord(pMatchRecord);
				_SetTotalRecord(myrecord);
				return;
			}
		}
	}
	_SetSeasonRecord(myrecord);
	_SetTotalRecord(myrecord);
}

void UIFloatReadyRoomUserInfo::_SetUserInfo()
{
	//_SetOpenCloseisFrame();

	const USER_INFO_BASIC * userinfo = CCommunityContext::i()->GetRoomFindUserBasicInfo();

	// 계급 세팅
	_SetUserClassImage(userinfo->m_ui32Rank);

	//NickName
	text_Nickname_->SetText(userinfo->m_strNick);

	if(userinfo->m_ui32Clanidx != 0)
	{
		// Clan Name
		i3::stack_wstring wstrClan;
		i3::sprintf(wstrClan, L"{col:255,255,255,255}%s{/col}", userinfo->m_strClanName);
		text_Clanname_->SetText(wstrClan);

		// Clan Rank
		i3::wstring clanrank;
		GetClanFullName2(clanrank, userinfo->m_ui8ClanExpRank, userinfo->m_ui8ClanPerRank);
		i3::sprintf(wstrClan, L"{col:255,255,255,255}%s{/col}", clanrank);
		text_Clanlevel_->SetText(wstrClan);

		_SetClanMarkIcon(userinfo, true);
	}
	else
	{
		i3::wstring wstrText = GAME_STRING("STR_TBL_GUI_READY_NO_JOINING");
		text_Clanname_->SetText(wstrText);
		text_Clanlevel_->SetText(wstrText);

		_SetClanMarkIcon(userinfo, false);
	}

	const USER_INFO_RECORD * userrecord = CCommunityContext::i()->GetRoomFindUserRecord();
	if (UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW))
	{
		if (ChannelContext::i()->GetBattleChannel() &&
			g_pFramework->GetClanContext()->InBattleTeam())
		{
			CLAN_PERSONAL_MATCH_RECORD* pMatchRecord = ClanGameContext::i()->getMathRecord(m_SelectedSlot);
			if (pMatchRecord != nullptr)
			{
				_SetClanMatchRecord(pMatchRecord);
				_SetTotalRecord(userrecord);
				return;
			}
		}
	}
	_SetSeasonRecord(userrecord);
	_SetTotalRecord(userrecord);
}

/*virtual*/ void UIFloatReadyRoomUserInfo::OnUpdate(REAL32 rDeltaSeconds)
{
	UIFloatingBase::OnUpdate(rDeltaSeconds);


	if (enable_OpenClose_ == true)
	{
		VEC3D* pos = frame_MainInfo_->getPos();
		if (pos->y > m_ControlPos[0].y)
		{
			pos->y = pos->y - ((rDeltaSeconds/ (REAL32)MAX_ANITIME) * (m_ControlPos[1].y - m_ControlPos[0].y));
			if (pos->y > m_ControlPos[1].y)
				pos->y = m_ControlPos[1].y;
			frame_MainInfo_->setPos(pos);
		}

	}
	else
	{
		VEC3D* pos = frame_MainInfo_->getPos();
		if (pos->y < m_ControlPos[1].y)
		{
			pos->y = pos->y + ((rDeltaSeconds/ (REAL32)MAX_ANITIME) * (m_ControlPos[1].y - m_ControlPos[0].y));
			if (pos->y < m_ControlPos[0].y)
				pos->y = m_ControlPos[0].y;
			frame_MainInfo_->setPos(pos);
		}
				
	}


}

void UIFloatReadyRoomUserInfo::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData )
{
	UIFloatingBase::ProcessGameEvent(evt, arg, UserData);
	switch(evt)
	{
	case EVENT_GET_USERINFO_N:
		_SetUserInfo();
		break;
	}

}

void UIFloatReadyRoomUserInfo::_SetOpenCloseisFrame()
{
	enable_OpenClose_ = !enable_OpenClose_;
}

void UIFloatReadyRoomUserInfo::_SetUserClassImage( INT32 _class )
{
	if(image_Class_ != nullptr)
	{
		image_Class_->SetShapeIdx(_class);
	}
}

void UIFloatReadyRoomUserInfo::_SetSeasonRecord( const USER_INFO_RECORD * record )
{
	i3::rc_wstring wstrText;

	// 시즌 전적 전 무 패
	i3::sprintf(wstrText, L"%d%s %d%s %d%s", record->_smatch, GAME_STRING("STBL_IDX_BATTLE_COUNT"), 
											record->_swin, GAME_STRING("STBL_IDX_BATTLE_WIN_COUNT"), 
											record->_slose, GAME_STRING("STBL_IDX_BATTLE_LOSE_COUNT") );
	vec_Season_[STATIC_RECORD]->SetText(wstrText);

	// 시즌 킬 Rate
	INT32 killrate = GameContextUtil::getKillRate(record->_skillcount, record->_sdeath);
	i3::sprintf(wstrText, GAME_RCSTRING("STR_TBL_PROFILE_KILL_DEATH_RATE_MODE1"), killrate);
	vec_Season_[STATIC_KILLRATE]->SetText(wstrText);

	// 시즌 킬 데스
	i3::sprintf(wstrText, L"%d / %d / %d", record->_skillcount, record->_sAssistCount, record->_sdeath);
	vec_Season_[STATIC_KILLDEATH]->SetText(wstrText);

	// 시즌 헤드샷 비율
	INT32 headshot = SeasonHeadShotPercent(record);
	i3::sprintf(wstrText, GAME_RCSTRING("STR_TBL_PROFILE_HEADSHOT_RATE_MODE1"), headshot);
	vec_Season_[STATIC_HEADSHOT]->SetText(wstrText);

	// 시즌 전장 이탈수
	i3::sprintf(wstrText, GAME_RCSTRING("STR_TBL_GUI_READY_BATTLE_LEAVE_RATE"), record->_sdis);
	vec_Season_[STATIC_LEAVE]->SetText(wstrText);
}

void UIFloatReadyRoomUserInfo::_SetTotalRecord( const USER_INFO_RECORD * record )
{
	i3::rc_wstring wstrText;

	// 시즌 전적 전 무 패
	i3::sprintf(wstrText, L"%d%s %d%s %d%s", record->_match, GAME_STRING("STBL_IDX_BATTLE_COUNT"), 
		record->_win, GAME_STRING("STBL_IDX_BATTLE_WIN_COUNT"), 
		record->_lose, GAME_STRING("STBL_IDX_BATTLE_LOSE_COUNT") );
	vec_Total_[STATIC_RECORD]->SetText(wstrText);

	// 시즌 킬 Rate
	INT32 killrate = GameContextUtil::getKillRate(record->_killcount, record->_death);
	i3::sprintf(wstrText, GAME_RCSTRING("STR_TBL_PROFILE_KILL_DEATH_RATE_MODE1"), killrate);
	vec_Total_[STATIC_KILLRATE]->SetText(wstrText);

	// 시즌 킬 데스
	i3::sprintf(wstrText, L"%d / %d / %d", record->_killcount, record->_AssistCount, record->_death);
	vec_Total_[STATIC_KILLDEATH]->SetText(wstrText);

	// 시즌 헤드샷 비율
	INT32 headshot = TotalHeadShotPercent(record);
	i3::sprintf(wstrText, GAME_RCSTRING("STR_TBL_PROFILE_HEADSHOT_RATE_MODE1"), headshot);
	vec_Total_[STATIC_HEADSHOT]->SetText(wstrText);

	// 시즌 전장 이탈수
	i3::sprintf(wstrText, GAME_RCSTRING("STR_TBL_GUI_READY_BATTLE_LEAVE_RATE"), record->_dis);
	vec_Total_[STATIC_LEAVE]->SetText(wstrText);
}

void UIFloatReadyRoomUserInfo::_SetClanMatchRecord(const CLAN_PERSONAL_MATCH_RECORD * record)
{
	if (record == nullptr)		return; 
	i3::rc_wstring wstrText;

	// 시즌 전적 %d전 %d승 %d패
	i3::sprintf(wstrText, L"%d%s %d%s %d%s", record->getWinCount()+ record->getLoseCount(), GAME_STRING("STBL_IDX_BATTLE_COUNT"),
		record->getWinCount(), GAME_STRING("STBL_IDX_BATTLE_WIN_COUNT"),
		record->getLoseCount(), GAME_STRING("STBL_IDX_BATTLE_LOSE_COUNT"));
	vec_Season_[STATIC_RECORD]->SetText(wstrText);

	// 시즌 킬 Rate
	INT32 killrate = GameContextUtil::getKillRate(record->getKillCount(), record->getDeathCount());
	i3::sprintf(wstrText, GAME_RCSTRING("STR_TBL_PROFILE_KILL_DEATH_RATE_MODE1"), killrate);
	vec_Season_[STATIC_KILLRATE]->SetText(wstrText);

	// 시즌 킬 데스
	i3::sprintf(wstrText, L"%d / %d", record->getKillCount(), record->getDeathCount());
	vec_Season_[STATIC_KILLDEATH]->SetText(wstrText);

	// 시즌 헤드샷 비율
	vec_Season_[STATIC_HEADSHOT]->SetText("");

	// 시즌 전장 이탈	
	vec_Season_[STATIC_LEAVE]->SetText("");
}


void UIFloatReadyRoomUserInfo::_ClearAtUnload( i3UIScene * pScene )
{
	UIFloatingBase::_ClearAtUnload(pScene);
	
	image_Class_ = nullptr;
	static_TextTitle_ = nullptr;
	text_Nickname_ = nullptr;
	text_Clanname_ = nullptr;
	text_Clanlevel_ = nullptr;

	button_ClanRankIcon_ = nullptr;
	image_ClanMark_ = nullptr;

	m_slide.ClearFrameWndList();
	vec_Season_.clear();
	vec_Total_.clear();
}

void UIFloatReadyRoomUserInfo::ClickButton()
{
	// Title 세팅
	_SetOpenCloseisFrame();
}

bool UIFloatReadyRoomUserInfo::OnExitStart( void )
{
	if( UIFloatingBase::OnExitStart() == false)
		return false;
	
	enable_OpenClose_ = true;
	return true;
}

void UIFloatReadyRoomUserInfo::_SetClanMarkIcon(const USER_INFO_BASIC * info_basic , bool iconEnable)
{	
	if(iconEnable)
	{
		GameUI::SetClanTexture(image_ClanMark_, CLAN_MARK_SMALL, info_basic->m_ui32ClanMark);
		image_ClanMark_->SetShapeSize(20.0f, 20.0f);
		// 크기는 아직 정하지 않습니다. 테스트 해봐야함

		// 클랜든 Exp로 체크 합니다.
		button_ClanRankIcon_->SetShapeIdx(info_basic->m_ui8ClanExpRank);
	}
	else
	{
		image_ClanMark_->EnableCtrl(false);
		button_ClanRankIcon_->EnableCtrl(false);
	}
}
