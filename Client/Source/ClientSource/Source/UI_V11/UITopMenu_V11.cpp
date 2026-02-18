#include "pch.h"
#include "UITopMenu_V11.h"

#include "UI/UIMainFrame.h"
#include "UI/UIPhaseBase.h"
#include "UI/UIPhaseSubBase.h"
#include "UI/UIUtil.h"
#include "UI/UIMath.h"
#include "UI/UIPhaseNonClanMember.h"
#include "UI/UIPhaseClanLobby.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "UserInfoContext.h"
#include "MailBoxContext.h"
#include "BattleSlotContext.h"
#include "ClanGameContext.h"
#include "MyRoomInfoContext.h"
#include "ChannelContext.h"

#if defined (LOCALE_CHINA)
#include "../../../SDOALink/SDOALink.h"
#endif

#define BACKBUTTON_CLICK_DELAY 1.f

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
static UITopMenu_V11 *	GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

	if( pFrame != nullptr)
	{
		return (UITopMenu_V11*) pFrame->GetTopMenu();
	}

	return nullptr;
}

extern "C" {
	int UITopMenu_V11_ClickClan( LuaState * L)
	{
		bool bOK = GetThis()->ClickClan();
		i3Lua::PushBoolean(L, bOK);
		return 1;
	}

	int UITopMenu_V11_ClickMyInventory( LuaState * L)
	{
		bool bOK = GetThis()->ClickInventory();
		i3Lua::PushBoolean(L, bOK);
		return 1;
	}

	int UITopMenu_V11_ClickShop( LuaState * L)
	{
		bool bOK = GetThis()->ClickShop();
		i3Lua::PushBoolean(L, bOK);
		return 1;
	}

	int UITopMenu_V11_ClickGiftShop(LuaState * L)
	{
		bool bOK = GetThis()->ClickGiftShop();
		i3Lua::PushBoolean(L, bOK);
		return 1;
	}

	int UITopMenu_V11_ClickMailBox( LuaState * L)
	{
		GetThis()->ClickMailBox();
		return 0;
	}

	int UITopMenu_V11_ClickOption( LuaState * L)
	{
		GetThis()->ClickOption();
		return 0;
	}

	int UITopMenu_V11_ClickBackButton( LuaState * L)
	{
		bool bSucess = GetThis()->ClickBackButton();
		i3Lua::PushBoolean(L, bSucess);
		return 1;
	}

	int UITopMenu_V11_ClickExit( LuaState * L)
	{
		I3TRACE( "ClickExit!!\n");
		GetThis()->ClickExit();
		return 0;
	}

	int UITopMenu_V11_ClickRouletteshop( LuaState * L)
	{
		bool bOK = GetThis()->ClickRouletteShop();
		i3Lua::PushBoolean(L, bOK);
		return 1;
	}
}

LuaFuncReg UITopMenu_V11_Glue[] =
{
	{"ClickClan",					UITopMenu_V11_ClickClan},
	{"ClickMyInventory",			UITopMenu_V11_ClickMyInventory},
	{"ClickShop",					UITopMenu_V11_ClickShop},
	{"ClickMailBox",				UITopMenu_V11_ClickMailBox},
	{"ClickOption",					UITopMenu_V11_ClickOption},
	{"ClickBack",					UITopMenu_V11_ClickBackButton},
	{"ClickExit",					UITopMenu_V11_ClickExit},
	{"ClickRouletteShop",			UITopMenu_V11_ClickRouletteshop},
	{"ClickGiftShop",				UITopMenu_V11_ClickGiftShop},
	{nullptr,				nullptr}
};

//------------------------------------------------------------------------------//

I3_CLASS_INSTANCE( UITopMenu_V11);

UITopMenu_V11::UITopMenu_V11() 
{
	m_rElapsedBackButtonTime = 0.f;
	m_pBottom = nullptr;
	m_bBtnReset = false;
}

UITopMenu_V11::~UITopMenu_V11()
{
}

//------------------------------------------------------------------------------//
//							Callback rutine										//
//------------------------------------------------------------------------------//

void UITopMenu_V11::CbExitNormal(void* pThis,INT32 nParam)
{
#if defined (LOCALE_CHINA)
	if ( nParam != MBR_OK )
	{
		g_pSDOA_Link->SetbRanderFlag(true);
	}
#endif
}

// 용병으로 재참가
void UITopMenu_V11::CbReRegistMercenary(void* pThis, INT32 nParam)
{
	if (nParam == MB_OK)
	{
		UIMainFrame * pUIFrame = ((UITopMenu_V11*)pThis)->GetMainFrame();
		pUIFrame->MovePhase(UIPHASE_LOBBY);
		ClanGameContext::i()->setClanBattleLobbyEnable(false);
	}
	else
	{
		((UIPhaseClanLobby*)GameUI::GetPhase(UIPHASE_CLANLOBBY))->SetLeaveTeamFlag();
		ClanGameContext::i()->setClanBattleLobbyEnable(false);
		GameEventSender::i()->SetEvent(EVENT_LEAVE_LOBBY);
	}
}

void UITopMenu_V11::CbRemoveMercenary(void* pThis, INT32 nParam)
{
	if (nParam == MB_OK)
	{
		UIMainFrame * pUIFrame = ((UITopMenu_V11*)pThis)->GetMainFrame();
		UIPhaseBase * pPhase = pUIFrame->GetCurrentPhase();
		if (pPhase != nullptr && pPhase->isSliding() == false)
		{
			pPhase->OnGotoBackPhase();
			pUIFrame->TogglePopup(UPW_NULL);
		}

		ClanGameContext::i()->setMyMercenaryState(false);
		GameEventSender::i()->SetEvent(EVENT_CLAN_WAR_REMOVE_MERCENARY);
	}
}

/*virtual*/ void UITopMenu_V11::_InitializeAtLoad( i3UIScene * pScene)
{
	UIBase::_InitializeAtLoad( pScene);

	if( i3::generic_is_iequal( pScene->GetName(), "PB_MainBottom"))
	{
		m_pBottom = pScene;

		//m_pAnnounce = (i3UIEditBox*) pScene->FindChildByName( "i3UIEditBox_Main");
		//I3ASSERT( m_pAnnounce);
		//m_pAnnounce->SetTextAlign( I3UI_TEXT_ALIGN_X_LEFT, I3UI_TEXT_ALIGN_Y_MIDDLE);
		m_FlowEditBox.SetFlowEditBox(static_cast<i3UIEditBox*>( pScene->FindChildByName( "i3UIEditBox_Main")));

		m_pRoomInfo = static_cast<i3UIEditBox*>(pScene->FindChildByName("i3UIEditBox7"));
		I3ASSERT(m_pRoomInfo);

		Resize();
	}
	else
	{
		switch( m_Mode)
		{
		case TOPMENU_MODE_SERVER :_SetServerMode();		break;
		case TOPMENU_MODE_CHANNEL :_SetChannelMode();	break;
		case TOPMENU_MODE_READY : _SetReadyMode();		break;
		}
	}

	
}

/*virtual*/ void UITopMenu_V11::_ClearAtUnload( i3UIScene * pScene)
{
	UIBase::_ClearAtUnload( pScene);

	m_FlowEditBox.Reset();
}

void UITopMenu_V11::Resize( void)
{
	// UI 1.1은 와이드시 컨트롤이 증가하지 않습니다...
}

void UITopMenu_V11::_SetServerMode( void)
{
	LuaState * L = _CallLuaFunction( "SetServerMode");
	_EndLuaFunction( L, 0);
	_EndHighlightMailbox();
}

void UITopMenu_V11::_SetChannelMode( void)
{
	LuaState * L = _CallLuaFunction( "SetChannelMode");
	_EndLuaFunction( L, 0);
	_EndHighlightMailbox();

	_SetRoomInfo();
}

void UITopMenu_V11::_SetReadyMode( void)
{
	//bool bGachaFlag = false;
	bool bClanFlag = false;
	bool bGiftFlag = false;

	bClanFlag = UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN);
	bGiftFlag = UserInfoContext::i()->IsContentsEndabled(CONTENTS_GIFTSHOP);

	if (UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW))
	{
		// 13553 용병 계정(클랜 미가입계정)으로 클랜채널입장시 클랜 버튼이 비활성화 되어야 함
		if (ChannelContext::i()->GetBattleChannel() &&
			UserInfoContext::i()->GetMyClanState() == CLAN_MEMBER_LEVEL_UNKNOWN)
		{
			bClanFlag = false;
		}
	}

	LuaState * L = _CallLuaFunction( "SetReadyMode");
	i3Lua::PushBoolean(L, bGiftFlag);
	i3Lua::PushBoolean(L, bClanFlag);
	_EndLuaFunction( L, 2);

	_SetRoomInfo();
	
}

void UITopMenu_V11::_SetRoomInfo()
{
	i3::rc_wstring wstrTitle;
	i3::wstring wstrBuf;
	ROOM_INFO_BASIC info;

	wstrTitle = L"";

	if (GetMainFrame()->GetCurrentPhaseType() > UIPHASE_CREATE_CHARA)
	{
		INT32 chIdx = g_pServerInfo->GetCurChannelNumber();
		INT32 serverIdx = g_pServerInfo->GetCurGameServerIndex();
		ChannelType _channelType = ChannelContext::i()->GetChannelType(serverIdx, chIdx);

		MyRoomInfoContext::i()->getMyRoomInfo(&info);
		ChannelContext::i()->GetServerChannelText(wstrTitle, _channelType, SS_SUBTYPE_CATEGORY_SLOT_TEXT);

		i3::sprintf(wstrBuf, L" %02d", chIdx + 1);
		i3::generic_string_cat(wstrTitle, wstrBuf);

		if (GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM)
		{
			i3::generic_string_cat(wstrTitle, L" >> ");
			i3::sprintf(wstrBuf, GAME_STRING("STBL_IDX_ROOM_TEXT1"), (info._Idx + 1));
			i3::generic_string_cat(wstrTitle, wstrBuf);
		}
	}

	m_pRoomInfo->SetText(wstrTitle.c_str());
}

void UITopMenu_V11::_ResetRoomInfo()
{
	m_pRoomInfo->SetText(L"");
}

void UITopMenu_V11::cb_tmenu_gauge(bool lastestTarget)
{
	if (UserInfoContext::i()->IsDailyGiftGauge() == false || UserInfoContext::i()->IsUserInfoDaily() == false)
	{
		mPBGaugeBar.reset(); return;
	}

	if (UserInfoContext::i()->GetUserInfoDaily()->_ui8GetItem == DAILY_GIFT_TYPE_MAX)
	{
		mPBGaugeBar.reset(); return;
	}

	INT32 idx = UserInfoContext::i()->GetUserInfoDaily()->_ui8GetItem;
	INT32 max_gauge = UserInfoContext::i()->GetDailyGiftGauge(idx);

	REAL32 factor = 0.f;
	if (max_gauge != 0) factor = (REAL32)(UserInfoContext::i()->GetUserInfoDaily()->_DGPlaytime / (REAL32)max_gauge);

	factor = uiMath::limited(factor, 0.f, 1.0f);

#if visible_on_current_value
#ifndef NC_BUILD
	i3::stack_wstring	wstr_stack;
	i3::wstring wstr_msg = L"turn reward ->";
	wstr_msg += L" play ";
	i3::itoa(UserInfoContext::i()->GetUserInfoDaily()->_DGPlaytime, wstr_stack);
	wstr_msg += wstr_stack;

	wstr_msg += L", max ";
	i3::itoa(max_gauge, wstr_stack);
	wstr_msg += wstr_stack;

	wstr_msg += L", result ";
	i3::ftoa(factor, wstr_stack, 3);
	wstr_msg += wstr_stack;

	::ColorString(wstr_msg, 255, 0, 0);
	GameUI::PrintSystemChat(L"\n");
	GameUI::PrintSystemChat(wstr_msg);
	GameUI::PrintSystemChat(L"\n");
#endif
#endif

	if (factor == 0.f)
	{
		mPBGaugeBar.reset();
	}
	else
	{
		if (lastestTarget == true)
		{
			mPBGaugeBar.set_lastest_target_factor();
		}
		mPBGaugeBar.set_factor(factor);
	}
}

//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//
/*virtual*/ void UITopMenu_V11::OnCreate( i3GameNode * pParent)
{
	UIBase::OnCreate( pParent);

	m_mainSceneIdx = GetSceneCount();
	// Load Scene
	AddScene( "Scene/Main/PointBlankRe_Main.i3UIs", UITopMenu_V11_Glue, true, false);
	AddScene( "Scene/Main/PB_MainBottom.i3UIs");

	i3UIScene *pScene_Main = GetScene(m_mainSceneIdx);
	if (nullptr != GetScene( m_mainSceneIdx ))
	{
		mPBGaugeBar.init(pScene_Main);
	}

	// 항시 보이므로 Enable한다.
	SetEnable( true);

	switch (m_Mode)
	{
	case TOPMENU_MODE_SERVER:_SetServerMode();		break;
	case TOPMENU_MODE_CHANNEL:_SetChannelMode();	break;
	case TOPMENU_MODE_READY: _SetReadyMode();		break;
	}
}

/*virtual*/ void UITopMenu_V11::OnUpdate( REAL32 rDeltaSeconds)
{
	UIBase::OnUpdate( rDeltaSeconds);

	_UpdateFlowAnnounce( rDeltaSeconds);
	if (m_Mode >= TOPMENU_MODE_READY)
	{
		_SetPushedButton_V11();
		_UpdateMailBoxAlaram(rDeltaSeconds);
	}

	mPBGaugeBar.update(rDeltaSeconds);

	m_rElapsedBackButtonTime -= rDeltaSeconds;
}

void UITopMenu_V11::OnLoadAllScenes()
{
	//g_scn_info.scn = GetScene(g_scn_info.idx);
	//g_last_popup_tab = g_last_phase_tab = None;
	_EndHighlightMailbox();	

	mPBGaugeBar.reset();
}

bool UITopMenu_V11::OnRevive( i3RenderContext * pCtx)
{
	if( UIBase::OnRevive( pCtx) == false)
		return false;

	Resize();

	return true;
}

/*virtual*/ bool UITopMenu_V11::OnEnterMouse( i3UIControl * pControl)
{
	if( UIBase::OnEnterMouse( pControl) == false)
		return false;

	if( i3::same_of<i3UIButton*, i3UIButtonComposed3*>(pControl) ) 
	{
		PlaySound( UISND_LIST_OVER);
	}

	return true;
}

/*virtual*/ bool UITopMenu_V11::OnKey_Enter( void)
{
	//UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	//UIPhaseNonClanMember * pPhase = (UIPhaseNonClanMember*)pFrame->GetCurrentSubPhase();
	//if( i3::same_of<UIPhaseNonClanMember*>(pPhase))
	//{
	//	pPhase->EnterClanSearch();
	//}

	return false;
}

/*virtual*/ void UITopMenu_V11::AllMenuBtnShapeToNormal()
{
	ClearTabSelection();
}

/*virtual*/void UITopMenu_V11::SyncSubPhaseWithMenuBtnState()
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	UISUBPHASE currentSubPhaseType = pFrame->GetCurrentSubPhaseType();
	INT32 tabValue = -1;

	switch (currentSubPhaseType)
	{
		case USP_NONE:			tabValue = -1;			break;
		case USP_TEMP:			tabValue = Clan;		break;
		case USP_INVENTORY:		tabValue = Inven;		break;
		case USP_SHOP:			tabValue = Shop;		break;
		case USP_GIFTSHOP:		tabValue = GiftShop;	break;
		default:				tabValue = -1;			break;
	}

	if (tabValue == -1)
		AllMenuBtnShapeToNormal();
	else
	{
		LuaState * L = _CallLuaFunction("SetTabButtonPush");
		i3Lua::PushInteger(L, tabValue);
		_EndLuaFunction(L, 1);
	}
}

void UITopMenu_V11::OnModifyCurrentPosition( const char * pszStr)
{
	I3ASSERT( pszStr);

	LuaState * L = _CallLuaFunction( 1, "SetLocation");
	i3Lua::PushA2UTF8String( L, pszStr);
	_EndLuaFunction( L, 1);
}

namespace
{
	bool _EnterSubPhase(UIMainFrame * pFrame, UISUBPHASE sub)
	{
		if( i3UI::getDisableUIInput() == true)
			return false;

		GetThis()->SetBtnResetState(false);

		if (pFrame->IsEnteredSubPhase(sub))
		{
			pFrame->ExclusedEnterSubPhase(USP_NONE);
			GetThis()->SetBtnResetState(true);
		}
		else
			pFrame->ExclusedEnterSubPhase(sub);

		return true;
	}
}

void UITopMenu_V11::ClearTabSelection() const
{
	i3UIScene* menu_scn = GetScene(0);
	LuaState* L = UILua::CallLuaFunction(menu_scn, "ClearTabSelection");
	UILua::EndLuaFunction(L, 0);
}

void UITopMenu_V11::Clicked(INT32 value)
{
	// 클릭 이벤트가 넘어 올 때 자신이 인게임 로딩중이면 
	// 다른 서브페이지로 넘어가면 안되므로 막아둡니다. - jinsik(2014. 11. 20)
	if(GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM && 
		BattleSlotContext::i()->getMySlotInfo()->_State >= SLOT_STATE_LOAD) return;

	//UIMainFrame * pFrame = GetMainFrame();

	switch (value)
	{
	case Shop:		
		ClickShop();
		break;	
	case Inven:	
		ClickInventory();
		break;
	//case Clan:
	//	{
	//		if(ClanGameContext::i()->getMyMercenaryState() == true &&
	//			UserInfoContext::i()->GetMyClanState() == CLAN_MEMBER_LEVEL_UNKNOWN )
	//		{
	//			GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_MERC_CREATE_MATCH_ROOM"));
	//			break;
	//		}

	//		UIPOPUPWIN popup = UPW_CLAN_MEMBER;
	//		if(pFrame->IsOpenedPopup(UPW_CLAN_NON_MEMBER) == true )
	//			popup = UPW_CLAN_NON_MEMBER;
	//		EnterPopup(pFrame, popup, true);
	//	}
	//	break;
	//case Myinfo		: EnterPopup(pFrame, UPW_PROFILE);		break;	
	case MailBox	: ClickMailBox();						break;
	case Setting	: ClickOption();						break;
	case Back		: ClickBackButton();					break;
	case Exit		: ClickExit();							break;
	//case GiftShop	: EnterSubPhase(pFrame, USP_GIFTSHOP);	break;
	}
}

//ClanSubPhase 방식은 서버에서 날라오는 멤버, 비멤버 구분에 따라
//멤버(USP_CLAN_MEMBER) 또는 비 멤버(USP_CLAN_NON_MEMBER)에 따라 다른 SubPhase가 열린다.
//즉, ClanSubPhase 활성화를 하기 위해 
//USP_CLAN_MEMBER 또는 USP_CLAN_NON_MEMBER 로 EnteredSubPhase 호출한다. (서버측 응답으로 구분)
bool UITopMenu_V11::ClickClan()
{
	if(ClanGameContext::i()->getMyMercenaryState() == true &&
		UserInfoContext::i()->GetMyClanState() == CLAN_MEMBER_LEVEL_UNKNOWN )
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_MERC_CREATE_MATCH_ROOM"));
		return false;
	}


	UIMainFrame * pFrame = GetMainFrame();

	if(pFrame != nullptr)
		return _EnterSubPhase(pFrame, USP_TEMP);
	return 0;
}

bool UITopMenu_V11::ClickInventory( void)
{
	UIMainFrame * pFrame = GetMainFrame();
	return _EnterSubPhase(pFrame, USP_INVENTORY);
}

bool UITopMenu_V11::ClickShop( void)
{
	if( ! g_pConfigEx->GetShop().Enable )
	{
		/*차후에 지원될 예정입니다.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, 
			GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_UNDER_CONSTRUCTION"));
		return false;
	}

	UIMainFrame * pFrame = GetMainFrame();
	return _EnterSubPhase(pFrame, USP_SHOP);
}

bool UITopMenu_V11::ClickGiftShop(void)
{
	//@eunil
	//todo : 레벨 제한 체크
	UIMainFrame * pFrame = GetMainFrame();
	return _EnterSubPhase(pFrame, USP_GIFTSHOP);
}

bool UITopMenu_V11::ClickRouletteShop(void)
{
//	UIMainFrame * pFrame = GetMainFrame();
//	return _EnterSubPhase(pFrame, USP_ROULETTE_SHOP);
	return 0;
}

void UITopMenu_V11::ClickMailBox( void)
{
	GetMainFrame()->TogglePopup( UPW_MAILBOX);
}

void UITopMenu_V11::ClickOption( void)
{
	GetMainFrame()->TogglePopup( UPW_OPTION);
}

void UITopMenu_V11::_SetPushedButton_V11()
{
	if (!m_bBtnReset)
		return;

	ClearTabSelection();
}

//
// 실제로 페이즈 전환 가능한 경우에 true값을 리턴하게 된다..
// 이 값이 true가 되면 루아에서 서브페이즈쪽 탭버튼을 끕니다..
//

bool UITopMenu_V11::ClickBackButton( void)
{
	if( m_rElapsedBackButtonTime > 0.f)
		return false;
	bool bSucess = false;
	if( GetMainFrame()->GetCurrentSubPhaseType() != USP_NONE)
	{
		if( GetMainFrame()->GetCurrentSubPhase()->isSliding() == false || !ClanGameContext::i()->IsForceLeaveChannel())
		{
			GetMainFrame()->ExclusedEnterSubPhase( USP_NONE);
			bSucess = true;
		}
	}
	else if( GetMainFrame()->GetCurrentPhase()->isSliding() == false || !ClanGameContext::i()->IsForceLeaveChannel())
	{
		// 클랜채널에서 용병이 뒤로가기 버튼을 누르면 용병 취소 확인 팝업이 떠야함
		if (ChannelContext::i()->GetBattleChannel())
		{
			if (ClanGameContext::i()->getMyMercenaryState())
			{
				if (GetMainFrame()->GetCurrentPhaseType() == UIPHASE_LOBBY)
				{
					GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_MERC_CHANNEL_LIST_MOVE_CANCLE"), nullptr, MAKE_CALLBACK(CbRemoveMercenary), this);
					return false;
				}
				else if (GetMainFrame()->GetCurrentPhaseType() == UIPHASE_CLANLOBBY )
				{
					g_pFramework->GetClanContext()->MatchTeamLeave();
					return false;
				}
			}
		}

		GetMainFrame()->GetCurrentPhase()->OnGotoBackPhase();
		bSucess = true;
	}

	// 팝업창은 모두 닫는다.
	GetMainFrame()->TogglePopup( UPW_NULL);

	ClearTabSelection();

	m_rElapsedBackButtonTime = BACKBUTTON_CLICK_DELAY;

	return bSucess;
}

void UITopMenu_V11::StartHighlightMailbox( void)
{
	UIPHASE phase = GetMainFrame()->GetCurrentPhaseType();
	if( phase == UIPHASE_SERVER_CHANNEL_LIST )
		return;

	m_bAlarmCommunity = true;
	m_bAlarmFlag = true;
	m_rAlarmTerm = 0.0f;
}

//-----------------------------------------------------------------------------------//

void UITopMenu_V11::ClickExit( void)
{
	OnEscapeKey();
}
