#include "pch.h"
#include "UIFloatMiniProfile.h"
#include "UIMainFrame.h"
#include "UIUtil.h"

#include "../GameGUI.h"
#include "../Record.h"


#include "i3Base/string/ext/format_string.h"
#include "i3Base/string/ext/generic_string_cat.h"

#include "UserInfoContext.h"
#include "GameContextUtil.h"
#include "ClanSeasonContext.h"
#include "ChannelContext.h"

I3_CLASS_INSTANCE( UIFloatMiniProfile);//, UIFloatingBase);

namespace
{
	UIClanMark::UIClanMark(void)
	{
		m_pClanMark = nullptr;
		m_pDefaultClanMark = nullptr;
	};

	UIClanMark::~UIClanMark(void)
	{
		m_pClanMark = nullptr;
		m_pDefaultClanMark = nullptr;
	};

	void UIClanMark::SetClanMarkCtrl( i3UIImageBoxEx * pClanMark, i3UIImageBox * pDefClanMark )
	{
		m_pClanMark = pClanMark;
		m_pDefaultClanMark = pDefClanMark;
	}

	void UIClanMark::SetClanMarkFlag( UINT32 ui32MarkFlag )
	{
		if( m_pClanMark != nullptr )
		{
			GameUI::SetClanTexture(m_pClanMark, CLAN_MARK_MEDIUM, ui32MarkFlag);
			m_pClanMark->SetShapeSize(PROFILE_MARK_SIZE, PROFILE_MARK_SIZE);

			if( m_pDefaultClanMark != nullptr )
				m_pDefaultClanMark->EnableCtrl( !m_pClanMark->isEnable() );
		}
	}
};

extern "C"{

	int UIFloatMiniProfile_ToggleUserInfo( LuaState * L)
	{
		bool bDetail = i3Lua::GetBooleanArg(L , 1);
		UIFloatMiniProfile * pThis = static_cast<UIFloatMiniProfile *>(GameUI::GetFloating( UFW_MINIPROFILE));
		if( pThis != nullptr)
			pThis->ToggleUserInfo(bDetail , true);
		return 0;
	}

	int UIFloatMiniProfile_ClickAnotherWnd(LuaState * L)
	{
		i3UIControl * pCtrl = reinterpret_cast<i3UIControl*>(i3Lua::GetUserDataArg(L , 1));
		UIFloatMiniProfile * pThis = static_cast<UIFloatMiniProfile *>(GameUI::GetFloating( UFW_MINIPROFILE));
		if( pThis != nullptr)
			pThis->CloseDetailInfo(pCtrl);
		return 0;
	}
}

LuaFuncReg FloatMiniProfile_Glue[] = {
	{"ToggleUserInfo",	UIFloatMiniProfile_ToggleUserInfo},
	{"ClickAnotherWnd", UIFloatMiniProfile_ClickAnotherWnd},
	{nullptr ,				nullptr}
};

UIFloatMiniProfile::UIFloatMiniProfile()
{
	m_AlignedPos = UIFLOATING_POS_LEFTTOP;
	m_bFixedWindow = true;

	m_ClanMark.SetClanMarkCtrl(nullptr, nullptr);

#if defined( I3_DEBUG)
	SetName( "MiniProfile");
#endif

	RegisterGameEventFunc( EVENT_CREATE_NICK_NAME,			&UIFloatMiniProfile::ReceiveGameEvent_Update_UserInfo);
	RegisterGameEventFunc( EVENT_CREATE_CHARA,				&UIFloatMiniProfile::ReceiveGameEvent_Update_UserInfo);
	RegisterGameEventFunc( EVENT_GET_SYSTEM_INFO,			&UIFloatMiniProfile::ReceiveGameEvent_Update_UserInfo);
	RegisterGameEventFunc( EVENT_CLAN_CREATE,				&UIFloatMiniProfile::ReceiveGameEvent_Update_UserInfo);
	RegisterGameEventFunc( EVENT_CLAN_SECESSION,			&UIFloatMiniProfile::ReceiveGameEvent_Update_UserInfo);
	RegisterGameEventFunc( EVENT_M_CLAN_MEMBER_INFO,		&UIFloatMiniProfile::ReceiveGameEvent_Update_UserInfo);
	RegisterGameEventFunc( EVENT_CLAN_DISSOLUTION,			&UIFloatMiniProfile::ReceiveGameEvent_Update_UserInfo);
	RegisterGameEventFunc( EVENT_MEDAL_GET_REWARD,			&UIFloatMiniProfile::ReceiveGameEvent_Update_UserInfo);
	RegisterGameEventFunc( EVENT_MEDAL_GET_ALL_REWARD,		&UIFloatMiniProfile::ReceiveGameEvent_Update_UserInfo);
	RegisterGameEventFunc( EVENT_RANK_UP,					&UIFloatMiniProfile::ReceiveGameEvent_Update_UserInfo);
	RegisterGameEventFunc( EVENT_USER_GUIDE,				&UIFloatMiniProfile::ReceiveGameEvent_Update_UserInfo);
	RegisterGameEventFunc( EVENT_GET_REMAIN_MONEY,			&UIFloatMiniProfile::ReceiveGameEvent_Update_UserInfo);
	RegisterGameEventFunc( EVENT_REFRESH_CASH,				&UIFloatMiniProfile::ReceiveGameEvent_Update_UserInfo);
	RegisterGameEventFunc( EVENT_M_CLAN_DEPROTATION,		&UIFloatMiniProfile::ReceiveGameEvent_Clan_Deprotation);
	RegisterGameEventFunc( EVENT_BUY_GOODS,					&UIFloatMiniProfile::ReceiveGameEvent_Update_UserInfo);
	RegisterGameEventFunc( EVENT_BUY_EXTEND_GOODS,			&UIFloatMiniProfile::ReceiveGameEvent_Update_UserInfo);
	RegisterGameEventFunc( EVENT_ITEM_REPAIR,				&UIFloatMiniProfile::ReceiveGameEvent_Update_UserInfo);
	RegisterGameEventFunc( EVENT_ITEM_AUTH,					&UIFloatMiniProfile::ReceiveGameEvent_Update_UserInfo);
	RegisterGameEventFunc( EVENT_AUTH_DELETE,				&UIFloatMiniProfile::ReceiveGameEvent_Update_UserInfo);
	RegisterGameEventFunc( EVENT_CLAN_PERSON_PLUS,			&UIFloatMiniProfile::ReceiveGameEvent_Update_UserInfo);
	RegisterGameEventFunc( EVENT_SELECT_CHANNEL,			&UIFloatMiniProfile::ReceiveGameEvent_Update_SeasonInfo);
	RegisterGameEventFunc( EVENT_SEND_GIFT,					&UIFloatMiniProfile::ReceiveGameEvent_Update_UserInfo);
	RegisterGameEventFunc( EVENT_LEAVE_LOBBY,				&UIFloatMiniProfile::ReceiveGameEvent_Reset_SeasonInfo);
}

UIFloatMiniProfile::~UIFloatMiniProfile()
{
}

void UIFloatMiniProfile::_SetDefaultInfo(const USER_INFO_BASIC & pMyInfo)
{
	if(i3::generic_string_size(pMyInfo.m_strNick) <= 0)
		return;

	LuaState * L;

	USER_INFO_INVITEM_DATA myUserInfoInvItemData;
	UserInfoContext::i()->GetMyUserInfoInvItemData(&myUserInfoInvItemData);

	// 닉네임 컬러
	const COLOR * pColor = GameGUI::GetColor(GCT_DEFAULT);
	if( myUserInfoInvItemData._ui8NickColor != 0 )
		pColor = GetColorCashItem( myUserInfoInvItemData._ui8NickColor );

	L = _CallLuaFunction( "SetDefaultInfo");
	if( L != nullptr)
	{
		if (myUserInfoInvItemData._wAprilFoolRank != RANK_NONE)
			i3Lua::PushInteger( L, myUserInfoInvItemData._wAprilFoolRank);
		else if (myUserInfoInvItemData._wRank != RANK_NONE)
			i3Lua::PushInteger( L, myUserInfoInvItemData._wRank);
		else
			i3Lua::PushInteger( L, pMyInfo.m_ui32Rank);

		i3Lua::PushStringUTF16To8( L, pMyInfo.m_strNick);

		i3Lua::PushColor( L, pColor);						// NickColor

		// E-Sports Mark
		if( pMyInfo.m_ui8ESportID == ESPORT_ID_LEVEL_ZERO)
			i3Lua::PushBoolean( L, false);
		else
			i3Lua::PushInteger( L, true);

		_EndLuaFunction( L, 7);
	}

	// 해피코인 ,NC코인 이제 안씁니다.
	L = _CallLuaFunction("SetCash");
	if(L != nullptr)
	{
		i3::stack_wstring wstrPoint;	i3_locale::itoa_comma(pMyInfo.m_ui32Point, wstrPoint);
		i3::stack_wstring wstrCash;		i3_locale::itoa_comma(pMyInfo.m_ui32Cash, wstrCash);

		if( wstrPoint.length() > 0)
			i3Lua::PushStringUTF16To8(L , wstrPoint);
		else
			i3Lua::PushString(L, L"0");

		if( wstrCash.length() > 0)
			i3Lua::PushStringUTF16To8(L , wstrCash);
		else
			i3Lua::PushString(L, L"0");

		_EndLuaFunction(L , 2);
	}
}

void UIFloatMiniProfile::_SetDetailInfo(const USER_INFO_BASIC & pMyInfo)
{
	LuaState * L;

	const USER_INFO_RECORD* pRecord = UserInfoContext::i()->GetMyUserInfoRecord();

	m_ClanMark.SetClanMarkFlag(pMyInfo.m_ui32ClanMark);
	
	i3::rc_wstring wstr_clan_name;
	if (i3::generic_string_size(pMyInfo.m_strClanName) > 0)
	{
		wstr_clan_name = pMyInfo.m_strClanName;
	}
	else
	{
		i3::sprintf(wstr_clan_name, GAME_RCSTRING("STBL_IDX_MINIPROFILE_CLAN_NOTJOIN"));
	}

	L = _CallLuaFunction("SetClanName");
	if (L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, wstr_clan_name);
		_EndLuaFunction(L, 1);
	}

	// 시즌전적
	L = _CallLuaFunction( "SetScore");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, pRecord->_smatch);
		i3Lua::PushInteger( L, pRecord->_swin);
		i3Lua::PushInteger( L, pRecord->_sdraw);			
		i3Lua::PushInteger( L, pRecord->_slose);
		_EndLuaFunction( L, 4);
	}

	// 시즌 킬/데스
	{	
		INT32 killRate = 0;

		L = _CallLuaFunction( "SetKillDeath");
		if( L != nullptr)
		{
			killRate = GameContextUtil::getKillRate(pRecord->_skillcount, pRecord->_sdeath);

			i3Lua::PushInteger(L, pRecord->_skillcount);
			i3Lua::PushInteger(L, pRecord->_sAssistCount);
			i3Lua::PushInteger(L, pRecord->_sdeath);
			i3Lua::PushInteger(L, killRate);
			_EndLuaFunction(L, 4);
		}		
	}

	// HeadShot
	{
		//i3::stack_wstring wstrTemp;

		INT32 nHeadshotRate = SeasonHeadShotPercent(pRecord);

		L = _CallLuaFunction( "SetHeadShot");
		if( L != nullptr)
		{
			i3Lua::PushInteger( L, pRecord->_sheadshot);
			i3Lua::PushInteger( L, nHeadshotRate);
			_EndLuaFunction( L, 2);
		}
	}
}


void UIFloatMiniProfile::_SetUserExp(const USER_INFO_BASIC & pMyInfo)
{
	LuaState * L;

//	char strBuf1[256];
//	char szTemp[ MAX_STRING_COUNT];
	i3::stack_wstring wstrExpText;

	UINT32 wRank = UserInfoContext::i()->GetRealRank();

	INT32 nCurrentExp = pMyInfo.m_ui32Exp - GameUI::GetMaxExp(wRank);
	nCurrentExp = MAX(nCurrentExp, 0);	// 음수값은 0
	INT32 nExpRate = 100;

	// 상대평가라도 목표 경험치 추가
	if (GameUI::is_hero_rank(wRank, true) == true)
	{
		i3::sprintf(wstrExpText, L"%d (100%%)   ", pMyInfo.m_ui32Exp);
	}
	else
	{
		INT32 nMaxExp = GameUI::GetMaxExp(wRank + 1) - GameUI::GetMaxExp(wRank);			
		//if( nMaxExp != 0) nExpRate = (nCurrentExp * 100) / nMaxExp;

		//nExpRate = MINMAX(0, nExpRate, 100);	// 100%가 넘을 경우 100%으로 고정 출력
		nExpRate = GameUI::GetExpRate(wRank, nCurrentExp);
		i3::sprintf( wstrExpText, L"%d/%d (%d%%)  ", nCurrentExp, nMaxExp, nExpRate);
	}

	L = _CallLuaFunction( "SetExpGauge");
	if( L != nullptr)
	{
		i3Lua::PushNumber( L, nExpRate * 0.01f);
		_EndLuaFunction( L, 1);
	}
}

void UIFloatMiniProfile::__UpdateUserInfo( void)
{
	USER_INFO_BASIC		MyUserInfoBasic;
	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);

	_SetDefaultInfo(MyUserInfoBasic);
	_SetUserExp(MyUserInfoBasic);

	_SetDetailInfo(MyUserInfoBasic);
}

void UIFloatMiniProfile::_ResetSeasonInfo(void)
{
	LuaState * L = _CallLuaFunction("SetSeasonInfo");

	if (L != nullptr)
	{
		i3Lua::PushBoolean(L, false);
		i3Lua::PushStringUTF16To8(L, L"");
		i3Lua::PushStringUTF16To8(L, L"");
		_EndLuaFunction(L, 3);
	}
}


void UIFloatMiniProfile::_SetSeasonInfo(void)
{
	LuaState * L = _CallLuaFunction("SetSeasonInfo");

	const CLAN_SEASON_INFO *  pCurrentSeason;
	pCurrentSeason = ClanSeasonContext::i()->GetCurrentSeason();

	if ( pCurrentSeason->GetSeasonName().size() <= 0 ) return;

	SYSTEMTIME StartDate = pCurrentSeason->GetSeasonStartDate();
	SYSTEMTIME EndDate = pCurrentSeason->GetSeasonEndDate();

	i3::wstring strStartDate = i3::format_string(L"%d.%d.%d" , StartDate.wYear , StartDate.wMonth , StartDate.wDay);
	i3::wstring strEndDate = i3::format_string(L"%d.%d.%d" , EndDate.wYear , EndDate.wMonth , EndDate.wDay);

	i3::wstring strPeriod = strStartDate ;
	strPeriod += L" ~ " ;
	strPeriod += strEndDate;

	if(L != nullptr)
	{
		i3Lua::PushBoolean(L, ChannelContext::i()->GetBattleChannel());
		i3Lua::PushStringUTF16To8(L,pCurrentSeason->GetSeasonName());
		i3Lua::PushStringUTF16To8(L,strPeriod);
		_EndLuaFunction(L , 3);
	}
}

/*virtual*/ void UIFloatMiniProfile::OnCreate( i3GameNode * pParent)
{
	UIFloatingBase::OnCreate( pParent);

	// Load Scene
	AddScene( "Scene/Main/PointBlankRe_MyInfo.i3UIs", FloatMiniProfile_Glue, true, false);
}

/*virtual*/ bool UIFloatMiniProfile::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIFloatingBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	m_slide.ForceEnd();

	__UpdateUserInfo();
	ToggleUserInfo(false, false);

	LuaState * L = _CallLuaFunction("__init");
	if( L != nullptr)
	{
		_EndLuaFunction(L, 0);
	}
	
	return true;
}

/*virtual*/ void UIFloatMiniProfile::_InitializeAtLoad(i3UIScene * pScene)
{
	UIFloatingBase::_InitializeAtLoad(pScene);

	i3UIImageBoxEx * pClanMark = static_cast<i3UIImageBoxEx*>( pScene->FindChildByName("ImgBox_ClanMark") );
	i3UIImageBox * pDefClanMark = static_cast<i3UIImageBox*>( pScene->FindChildByName("ImgBox_DefaultMark") );

	m_ClanMark.SetClanMarkCtrl(pClanMark, pDefClanMark);
}


/*virtual*/ void UIFloatMiniProfile::_ClearAtUnload(i3UIScene * pScene)
{
	m_ClanMark.SetClanMarkCtrl(nullptr, nullptr);
	UIFloatingBase::_ClearAtUnload(pScene);
}


/*virtual*/ bool UIFloatMiniProfile::OnExitStart( void)
{
	if( UIFloatingBase::OnExitStart() == false)
		return false;

//	UI::remove_cb(this);			// insert_cb가 없어서 주석처리합니다..

	m_slide.ForceEnd();

	return true;
}

/*virtual*/ void UIFloatMiniProfile::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
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

//////////////////////////////////////////////////////////////////////////////
void UIFloatMiniProfile::OnModifyCurrentPosition(const i3::wliteral_range& wTextRng)
{
	LuaState * L = _CallLuaFunction("SetLocation");
	i3Lua::PushStringUTF16To8( L, wTextRng);
	_EndLuaFunction( L, 1);

	_AttachToTopScreen();
}

void UIFloatMiniProfile::ToggleUserInfo(bool bDetail, bool bClickBtn)
{
	if(bDetail == true)
		_AttachToTopScreen();


	LuaState * L = _CallLuaFunction("SetUserInfo");

	if ( L != nullptr)
	{
		USER_INFO_BASIC		MyInfo;
		UserInfoContext::i()->GetMyUserInfoBasic(&MyInfo);

		i3Lua::PushBoolean(L , bDetail);

		// E-Sports Mark
		{
			if( MyInfo.m_ui8ESportID == ESPORT_ID_LEVEL_ZERO)
				i3Lua::PushBoolean( L, false);
			else
				i3Lua::PushInteger( L, true);
		}

		// 버튼 클릭인지, 다른 창 클릭인지 판단
		i3Lua::PushBoolean(L , bClickBtn);

		i3UIControl * pCtrl = reinterpret_cast<i3UIControl*>(_ReturnLuaPointer(L , 3));
		if(pCtrl != nullptr)
			g_pFramework->SetFocus(pCtrl);

		// Set Clan Mark Flag
		{
			if( bDetail == true )
				m_ClanMark.SetClanMarkFlag(MyInfo.m_ui32ClanMark);
		}
	}
}

void UIFloatMiniProfile::CloseDetailInfo(i3UIControl* pCtrl)
{
	
	i3UIControl * pCurCtrl = reinterpret_cast<i3UIControl*>(g_pFramework->getFocus());
	
	// FrameWindow가 같은 경우에는 포커스 처리 안함
	if(pCtrl != nullptr && pCurCtrl == pCtrl->getParent())
		return;
	else
		ToggleUserInfo(false, false);
}

void UIFloatMiniProfile::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}


void UIFloatMiniProfile::ReceiveGameEvent_Update_SeasonInfo( INT32, const i3::user_data&)
{
	ChannelType channeltype = ChannelContext::i()->GetConnectedChannelType();
	if (CHANNEL_TYPE_CLAN == channeltype)
	{
		_SetSeasonInfo();
	}
}

void UIFloatMiniProfile::ReceiveGameEvent_Reset_SeasonInfo(INT32, const i3::user_data&)
{
	_ResetSeasonInfo();
}

void UIFloatMiniProfile::ReceiveGameEvent_Update_UserInfo( INT32, const i3::user_data&)
{
	__UpdateUserInfo(); 
}

void UIFloatMiniProfile::ReceiveGameEvent_Clan_Deprotation( INT32, const i3::user_data&)
{
	USER_INFO_BASIC		MyUserInfoBasic;
	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
	MyUserInfoBasic.m_ui32Clanidx	= 0;
	MyUserInfoBasic.m_ui32ClanState	= CLAN_MEMBER_LEVEL_UNKNOWN;
	MyUserInfoBasic.m_ui32ClanMark	= CLAN_MARK_DEFAULT;
	i3mem::FillZero(MyUserInfoBasic.m_strClanName, sizeof(MyUserInfoBasic.m_strClanName));
	UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);

	__UpdateUserInfo(); 
}