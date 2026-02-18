#include "pch.h"
#include "UITopMenu_V10.h"

#include "UI/UIMainFrame.h"
#include "UI/UIMainFrame_V10.h"
#include "UI/UIPhaseBase.h"
#include "UI/UIPhaseSubBase.h"
#include "UI/UIUtil.h"
#include "UI/UIMath.h"
#include "UI/UIPhaseNonClanMember.h"
#include "UI_V10/UIFloatCommunity.h"
#include "UI/UIPhaseClanLobby.h"
#include "UI/UISideMenu.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "UserInfoContext.h"
#include "MailBoxContext.h"
#include "BattleSlotContext.h"
#include "ClanGameContext.h"
#include "../GameGUI.h"
#include "UserInfoContext.h"
#include "ChannelContext.h"
#include "ShopContext.h"
#include "UI/Batmanager.h"

#include "GameLoginContext.h"

#define BACKBUTTON_CLICK_DELAY 1.f

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
static UITopMenu_V10 *	GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

	if( pFrame != nullptr)
	{
		return (UITopMenu_V10*) pFrame->GetTopMenu_V10();
	}

	return nullptr;
}

extern "C" {
	int UITopMenu_V10_Clicked( LuaState * L)
	{
		INT32 idx =  i3Lua::GetIntegerArg(L, 1);
		GetThis()->Clicked(idx);

		return 0;
	}

	/////////////////////////////////////////////////////
}

LuaFuncReg UITopMenu_V10_Glue[] =
{
	{"Clicked",				UITopMenu_V10_Clicked},
	{nullptr,				nullptr}
};

extern "C" {

	int UITopMenu_V10_SetMyInfo( LuaState * L)
	{
		GetThis()->InitMyInfo();
		return 0;
	}
}

LuaFuncReg UITopMenu_MyInfo_V10_Glue[] =
{
	{"SetMyInfo",					 UITopMenu_V10_SetMyInfo},
	{nullptr,				nullptr}
};

//------------------------------------------------------------------------------//

I3_CLASS_INSTANCE( UITopMenu_V10);

UITopMenu_V10::UITopMenu_V10() 
{
	m_pLight = nullptr;
	m_pCamNode = nullptr;
	m_pAttrSet = nullptr;
	m_pBG = nullptr;
	m_pRefAttr = nullptr;
	m_pNationFlag = nullptr;

	m_pShopButton = nullptr;
	m_bCommunity = false;

	for( UINT32 i = 0; i < CAS_MAX; i++)
		m_pCam[i] = nullptr;

	m_bCallOverlap = false;

	m_iPrePushBtn_idx = 0;

	// 이벤트 처리 유무 확인이 필요...
	RegisterGameEventFunc( EVENT_RANK_UP,					&UITopMenu_V10::ReceiveGameEvent_Update_MyInfo);
	RegisterGameEventFunc( EVENT_GET_REMAIN_MONEY,			&UITopMenu_V10::ReceiveGameEvent_Update_MyInfo);
	RegisterGameEventFunc( EVENT_REFRESH_CASH,				&UITopMenu_V10::ReceiveGameEvent_Update_MyInfo);
	RegisterGameEventFunc( EVENT_BUY_GOODS,					&UITopMenu_V10::ReceiveGameEvent_Update_MyInfo);
	RegisterGameEventFunc( EVENT_AUTH_GIFT_GOODS,			&UITopMenu_V10::ReceiveGameEvent_Update_GiftGoods);
	RegisterGameEventFunc( EVENT_RECEIVE_NOTE,				&UITopMenu_V10::ReceiveGameEvent_Update_ReceiveNote);
	RegisterGameEventFunc( EVENT_DELETE_NOTE,				&UITopMenu_V10::ReceiveGameEvent_Update_ReceiveNote);
	RegisterGameEventFunc( EVENT_GET_INGAME_URL,			&UITopMenu_V10::ReceiveGameEvent_Update_IngameURL);
	//RegisterGameEventFunc( EVENT_AUTH_DELETE,				&UITopMenu_V10::ReceiveGameEvent_Update_MyInfo);
}

UITopMenu_V10::~UITopMenu_V10()
{
	for( UINT32 i = 0; i < CAS_MAX; i++)
	{
		I3_SAFE_NODE_RELEASE( m_pCam[ i]);
	}
	I3_SAFE_NODE_RELEASE(m_pBG);
	I3_SAFE_RELEASE(m_pNationFlag);
	I3_SAFE_RELEASE(m_pRefAttr);

	m_pAttrSet->RemoveAllChild();
	m_pAttrSet->RemoveAllAttrs();
	I3_SAFE_NODE_RELEASE(m_pAttrSet);
	
	if( g_pFramework != nullptr)
	{
		if( m_pLight != nullptr)
		{
			g_pFramework->removeLight( m_pLight);
			I3_SAFE_RELEASE( m_pLight);
		}
	}
}

//------------------------------------------------------------------------------//
//							Callback rutine										//
//------------------------------------------------------------------------------//

void UITopMenu_V10::CbExitNormal(void* pThis,INT32 nParam)
{
}

// 용병으로 재참가
void UITopMenu_V10::CbReRegistMercenary(void* pThis, INT32 nParam)
{
	if (nParam == MB_OK)
	{
		UIMainFrame * pUIFrame = ((UITopMenu_V10*)pThis)->GetMainFrame();
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


void UITopMenu_V10::CbRemoveMercenary(void* pThis, INT32 nParam)
{
	if (nParam == MB_OK)
	{
		UIMainFrame * pUIFrame = ((UITopMenu_V10*)pThis)->GetMainFrame();
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

/*virtual*/ void UITopMenu_V10::_InitializeAtLoad( i3UIScene * pScene)
{
	UIBase::_InitializeAtLoad( pScene);

	if( i3::generic_is_iequal( pScene->GetName(), "PointBlankRe_Main"))
	{
		std::string strPushBtnName[MaxCount] = 
		{
			"i3UIButton_Youtube",	// Youtube
			"i3UIButton_Clan",		// Clan
			"i3UIButton_Mission",	// Mission
			"i3UIButton_Inven",		// Inven
			"i3UIButton_Shop",		// Shop
			"i3UIButton_Community",	// Community
			"",						// Setting
			""						// Exit	
		};

		m_PushBtnList.clear();

		for(INT32 idx = 0 ; idx < MaxCount ; ++idx)
		{
			if(strPushBtnName[idx].length() == 0) 
				continue;

			PushButton BtnInfo;

			std::string strNormal = strPushBtnName[idx];

			BtnInfo.i32Idx = idx;
			BtnInfo.pNormalBtn = static_cast<i3UIButton*>( pScene->FindChildByName(strNormal.c_str() ));

			m_PushBtnList.push_back(BtnInfo);
		}

		switch( m_Mode)
		{
		case TOPMENU_MODE_SERVER :_SetServerMode();		break;
		case TOPMENU_MODE_CHANNEL :_SetChannelMode();	break;
		case TOPMENU_MODE_READY : _SetReadyMode();		break;
		}

		LuaState * L = UILua::CallLuaFunction(GetScene(m_main_scn_info.idx), "__init");
		if (L != nullptr)
		{
			UILua::EndLuaFunction(L, 0);
		}

		m_FlowEditBox.SetFlowEditBox(static_cast<i3UIEditBox*>( GetScene(m_main_scn_info.idx)->FindChildByName( "i3UIEditBox_Main")));

		_AddLight();
		_LoadBG();

		g_pFramework->setShadowQuality( g_pEnvSet->m_nShadowQualityType, false);

		ResizeMainScene();
	}
	else
	{
		// 유저 접속 국가 국기 표시 시스템
#if !defined(NATION_FLAG)
		m_pNationFlag = static_cast<i3UIButtonImageSet*>(GetScene(m_myinfo_scn_info.idx)->FindChildByName("i3UIButtonImageSet"));
		I3ASSERT(m_pNationFlag != nullptr);

		m_pNationFlag->EnableCtrl(false);
#endif
	}
}

/*virtual*/ void UITopMenu_V10::_ClearAtUnload( i3UIScene * pScene)
{

	if( i3::generic_is_iequal( pScene->GetName(), "PointBlankRe_Main"))
	{
		m_PushBtnList.clear();
		m_FlowEditBox.Reset();
	}

	UIBase::_ClearAtUnload( pScene);

}

void UITopMenu_V10::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
{
	UIBase::ProcessGameEvent( evt, arg, UserData);

	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}
}

// 기존 1.0 UI에서는 해상도에 따라서 UI Control Size가 변경되었는데
// 기존 방식대로 하기 위해서는 일부 Control의 기본 Size를 보관해야할 필요가 있어서
// 처리 방식에 대한 내부 TR 이후 추가 작업이 필요
void UITopMenu_V10::ResizeMainScene( void)
{
	// 버튼 focus 영역을 늘려야 한다.
	i3UIScene * pScene = GetScene( 0);
	I3ASSERT( pScene != nullptr);

	i3UIFrameWnd * pFrame = (i3UIFrameWnd*) pScene->FindChildByName( "i3UIFrameWnd_Main");
	I3ASSERT( pFrame != nullptr);

	i3UIManager * pMng = i3UI::getManager();
	REAL32 scaleY = (REAL32)g_pViewer->GetViewHeight() / (REAL32)pMng->getActualHeight();

	REAL32 rateY = pMng->getScreenStretchRatioY();
	REAL32 offset = (REAL32) pMng->getDisplayOffsetX();

	// MainFrame
	{
		pFrame->setSize( (REAL32)g_pViewer->GetViewWidth(), getY( pFrame->getSize()) * scaleY);
	}

	// TopFrame
	{

		i3UIImageBox* pTopImage = (i3UIImageBox*) pScene->FindChildByName("bgMenu");
		I3ASSERT( pTopImage != nullptr);

		pTopImage->setSize( (REAL32)g_pViewer->GetViewWidth(), (REAL32)pTopImage->getHeight());
		pTopImage->setPos( pFrame->getPos()->x - offset , -(REAL32)pMng->getDisplayOffsetY() * rateY);
	}

	// FlowNotice
	{
		i3UIFrameWnd * pFlowNoticeFrame = (i3UIFrameWnd*) pScene->FindChildByName("i3UIFrameWnd_FlowEditBox");
		I3ASSERT( pFlowNoticeFrame != nullptr);

		pFlowNoticeFrame->setSize( (REAL32)pFlowNoticeFrame->getWidth(), (REAL32)pFlowNoticeFrame->getHeight());

		i3UIFrameWnd * pFlowNoticeImage = (i3UIFrameWnd*) pScene->FindChildByName("i3UIFrameWnd");
		I3ASSERT( pFlowNoticeImage != nullptr);

		pFlowNoticeImage->setSize( (REAL32)pFlowNoticeImage->getWidth(), (REAL32)pFlowNoticeImage->getHeight());

		i3UIEditBox * pFlowNotice = (i3UIEditBox*) pScene->FindChildByName("i3UIEditBox_Main");
		I3ASSERT( pFlowNotice != nullptr);

		pFlowNotice->setSize( (REAL32)pFlowNotice->getWidth(), (REAL32)pFlowNotice->getHeight());
	}

	// MenuBar Frame
	{
		i3UIFrameWnd * pBottomFrame = (i3UIFrameWnd*)pScene->FindChildByName("i3UIFrameWnd0");
		I3ASSERT(pBottomFrame != nullptr);

		pBottomFrame->setSize((REAL32)g_pViewer->GetViewWidth(), (REAL32)pBottomFrame->getHeight());

		i3UIFrameWnd * pMenuBarFrame = (i3UIFrameWnd*) pScene->FindChildByName("i3UIFrameWnd_MainBtn");
		I3ASSERT( pMenuBarFrame != nullptr);
 
		pMenuBarFrame->setSize( (REAL32)g_pViewer->GetViewWidth(), (REAL32)pMenuBarFrame->getHeight());
 		//pMenuBarFrame->setPos( pFrame->getPos()->x - offset , (REAL32)pMng->getDisplayOffsetY());
 
		i3UIImageBox* pMenuBarImage = (i3UIImageBox*) pScene->FindChildByName("i3UIImageBox");
		I3ASSERT( pMenuBarImage != nullptr);
 
		pMenuBarImage->setSize( (REAL32)g_pViewer->GetViewWidth(), (REAL32)pMenuBarImage->getHeight());
 		//pMenuBarImage->setPos( pFrame->getPos()->x - offset , (REAL32)pMng->getDisplayOffsetY());
	}


}

void UITopMenu_V10::SetYouTubeButton()
{
	i3::vector<BANNER_URL_INFO> url_list;
	GameLoginContext::i()->GetIngameURLInfo(url_list, URL_TYPE_INGAME_WEB);
	if (url_list.size() == 0 && m_PushBtnList[Youtube].pNormalBtn != nullptr)
	{
		m_PushBtnList[Youtube].pNormalBtn->EnableCtrl(false);
		m_PushBtnList[Youtube].pNormalBtn->setInputDisable(false);
		m_PushBtnList[Youtube].pNormalBtn->SetVisible(false);
	}
}

void UITopMenu_V10::StartHighlightMailbox(void)
{
	if (m_Mode < TOPMENU_MODE_READY)
		return;

	m_bAlarmCommunity = true;
	m_bAlarmFlag = true;
	m_rAlarmTerm = 0.0f;
}

void UITopMenu_V10::_SetServerMode( void)
{
	LuaState * L = _CallLuaFunction( "SetServerMode");
	_EndLuaFunction( L, 0);
	_EndHighlightMailbox();
}

void UITopMenu_V10::_SetChannelMode( void)
{
	LuaState * L = _CallLuaFunction( "SetChannelMode");
	_EndLuaFunction( L, 0);
	_EndHighlightMailbox();
}

void UITopMenu_V10::_SetReadyMode( void)
{
	bool bClanFlag = false;

	bClanFlag = UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN);

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
	i3Lua::PushBoolean(L, bClanFlag);
	_EndLuaFunction( L, 1);

#ifdef LOCALE_RUSSIA
	i3::stack_wstring wstrServer;
	//현재 서버 위치 가져오기
	GetMainFrame()->GetCurrentSeverInfo(wstrServer);
	L = UILua::CallLuaFunction(GetScene(m_main_scn_info.idx), "SetLocation");
	i3Lua::PushStringUTF16To8(L, wstrServer);
	_EndLuaFunction(L, 1);
#endif // !LOCALE_RUSSIA 일단 러시아만 1.2
}

//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//
/*virtual*/ void UITopMenu_V10::OnCreate( i3GameNode * pParent)
{
	UIBase::OnCreate( pParent);

	// Load Scene
	m_main_scn_info.idx = GetSceneCount();
	AddScene( "Scene/Main/PointBlankRe_Main.i3UIs", UITopMenu_V10_Glue, true, false);

	m_myinfo_scn_info.idx = GetSceneCount();
	AddScene( "Scene/Main/PointBlankRe_MyInfo.i3UIs", UITopMenu_MyInfo_V10_Glue);

	mPBGaugeBar.init(GetScene(m_main_scn_info.idx));

	VEC3D vUp;

	i3Vector::Set( &vUp, 0.f, 1.f, 0.f);

	// 항시 보이므로 Enable한다.
	SetEnable( true);
}

/*virtual*/ void UITopMenu_V10::OnUpdate( REAL32 rDeltaSeconds)
{
	UIBase::OnUpdate( rDeltaSeconds);
	BaTMaN::BatMan_Outgame_Update(rDeltaSeconds);


	_UpdateFlowAnnounce( rDeltaSeconds);

	mPBGaugeBar.update(rDeltaSeconds);

	UpdateCamera();

	if (m_Mode >= TOPMENU_MODE_READY)
	{
		_SetPushedButton_V10();		
		_UpdateMailBoxAlaram(rDeltaSeconds);
	}

	m_rElapsedBackButtonTime -= rDeltaSeconds;
}

void UITopMenu_V10::OnLoadAllScenes()
{
	m_main_scn_info.scn = GetScene(m_main_scn_info.idx);
	m_myinfo_scn_info.scn = GetScene(m_myinfo_scn_info.idx);	
	_EndHighlightMailbox();	

	mPBGaugeBar.reset();
}

bool UITopMenu_V10::OnRevive( i3RenderContext * pCtx)
{
	if( m_bCallOverlap == false)
	{
		ResizeMainScene();
		m_bCallOverlap = true;
	}
	else
	{
		m_bCallOverlap = false;
	}

	if( UIBase::OnRevive( pCtx) == false)
		return false;

	if( m_pCam[m_CurCameraState] != nullptr)
	{
		i3Camera * pCam = static_cast<i3Camera*>( i3Scene::FindNodeByType( m_pCam[m_CurCameraState], i3Camera::static_meta()));
		I3ASSERT( pCam != nullptr);
		g_pFramework->getSgContext()->setCurrentCamera( pCam);
		pCam->SetPerspective( pCam->GetFOV(), (REAL32) g_pEnvSet->m_nScreenWidth, (REAL32) g_pEnvSet->m_nScreenHeight, pCam->GetNear(), pCam->GetFar());
	}

	return true;
}

void UITopMenu_V10::_SetMyDefaultInfo()
{
	LuaState * L = UILua::CallLuaFunction(GetScene(m_myinfo_scn_info.idx), "SetDefaultInfo");
	if(L != nullptr)
	{
		//i3Lua::PushInteger( L, UserInfoContext::i()->GetMyRank());				// Rank
		USER_INFO_INVITEM_DATA MyUserInfoInvItemData;
		UserInfoContext::i()->GetMyUserInfoInvItemData(&MyUserInfoInvItemData);
	
		if (MyUserInfoInvItemData._wAprilFoolRank != RANK_NONE)
			i3Lua::PushInteger(L, MyUserInfoInvItemData._wAprilFoolRank);
		else if (MyUserInfoInvItemData._wRank != RANK_NONE)
			i3Lua::PushInteger(L, MyUserInfoInvItemData._wRank);
		else
			i3Lua::PushInteger(L, UserInfoContext::i()->GetMyRank());

		i3Lua::PushStringUTF16To8( L, UserInfoContext::i()->GetMyNickName());	// NickName

		USER_INFO_INVITEM_DATA myUserInfoInvItemData;
		UserInfoContext::i()->GetMyUserInfoInvItemData(&myUserInfoInvItemData);

		const COLOR * pColor = GameGUI::GetColor(GCT_INVERSE);

		if( myUserInfoInvItemData._ui8NickColor != 0 )
			pColor = GetColorCashItem( myUserInfoInvItemData._ui8NickColor );

		i3Lua::PushColor( L, pColor);

		UILua::EndLuaFunction(L, 6);	
	}
}

/*virtual*/ bool UITopMenu_V10::OnEnterMouse( i3UIControl * pControl)
{
	if( UIBase::OnEnterMouse( pControl) == false)
		return false;

	if( i3::same_of<i3UIButton*, i3UIButtonComposed3*>(pControl) ) 
	{
		PlaySound( UISND_LIST_OVER);
	}

	return true;
}

/*virtual*/ bool UITopMenu_V10::OnKey_Enter( void)
{
	//UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	//UIPhaseNonClanMember * pPhase = (UIPhaseNonClanMember*)pFrame->GetCurrentSubPhase();
	//if( i3::same_of<UIPhaseNonClanMember*>(pPhase))
	//{
	//	pPhase->EnterClanSearch();
	//}

	return false;
}

namespace
{
	bool _EnterSubPhase(UIMainFrame * pFrame, UISUBPHASE sub)
	{
		if( i3UI::getDisableUIInput() == true)
			return false;

		if( pFrame->IsEnteredSubPhase(sub) )
			pFrame->ExclusedEnterSubPhase( USP_NONE);
		else
			pFrame->ExclusedEnterSubPhase( sub );			

		return true;
	}
}

//------------------------------------------------------------------------------//
//									MenuBar										//
//------------------------------------------------------------------------------//

void UITopMenu_V10::Clicked(INT32 value)
{
	// 클릭 이벤트가 넘어 올 때 자신이 인게임 로딩중이면 
	// 다른 서브페이지로 넘어가면 안되므로 막아둡니다. - jinsik(2014. 11. 20)
	if(GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM && 
		BattleSlotContext::i()->getMySlotInfo()->_State >= SLOT_STATE_LOAD) 
		return;

	//if( _IsAvailablePush(value) )
//		_SetPushedButton(value);

	switch (value)
	{
	case Youtube:
		I3TRACE("###InGameWeb###\n");
		ClickYoutube();
		break;
	case Clan:
		I3TRACE("###Clan###\n");
		if(UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW))
		{
			if( ChannelContext::i()->GetBattleChannel() && 
				UserInfoContext::i()->GetMyClanState() == CLAN_MEMBER_LEVEL_UNKNOWN )
			{
				GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_MERC_CREATE_MATCH_ROOM"));
				break;
			}
		}
		ClickClan();
		break;
	case Missioin:
		I3TRACE("###Missioin###\n");
		ClickMission();
		break;
	case Inven:	
		I3TRACE("###Inven###\n");
		ClickInventory();
		break;
	case Shop:	
		I3TRACE("###Shop###\n");	
		ClickShop();
		break;	
	case Community:
		I3TRACE("###Setting###\n");
		ClickCommunity();	
		break;
	case Setting:
		I3TRACE("###Setting###\n");
		ClickOption();						
		break;
	case Exit:
		I3TRACE("###Exit###\n");
		ClickExit();			
		break;
	}
}

bool UITopMenu_V10::ClickBackButton( void)
{
	if( m_rElapsedBackButtonTime > 0.f)
		return false;
	bool bSucess = false;
	if( GetMainFrame()->GetCurrentSubPhaseType() != USP_NONE)
	{
		if( GetMainFrame()->GetCurrentSubPhase()->isSliding() == false)
		{
			GetMainFrame()->ExclusedEnterSubPhase( USP_NONE);
			bSucess = true;
		}
	}
	else if( GetMainFrame()->GetCurrentPhase()->isSliding() == false)
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
				else if (GetMainFrame()->GetCurrentPhaseType() == UIPHASE_CLANLOBBY)
				{
					g_pFramework->GetClanContext()->MatchTeamLeave();
					if (UserInfoContext::i()->GetMyClanState() == CLAN_MEMBER_LEVEL_UNKNOWN)
					{
						GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_MERC_REJOIN_POPUP"), nullptr, MAKE_CALLBACK(CbReRegistMercenary), this);
					}
					else
					{
						GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_MERC_CHANNEL_LIST_MOVE_CANCLE"), nullptr, MAKE_CALLBACK(CbRemoveMercenary), this);
					}
					return false;
				}
			}
		}

		GetMainFrame()->GetCurrentPhase()->OnGotoBackPhase();
		bSucess = true;
	}

	// 팝업창은 모두 닫는다.
	GetMainFrame()->TogglePopup( UPW_NULL);

	AllMenuBtnShapeToNormal();

	m_rElapsedBackButtonTime = BACKBUTTON_CLICK_DELAY;

	AllMenuBtnShapeToNormal();

	return bSucess;
}

bool UITopMenu_V10::ClickYoutube()
{
	i3::vector<BANNER_URL_INFO> url_list;	//복수로 웹툴에서 제공은 할 수 있는데 기획상 해당 부분에서 복수가 되면 안된다. 따라서 0만 쓸께여
	GameLoginContext::i()->GetIngameURLInfo(url_list, URL_TYPE_INGAME_WEB);
	if (url_list.size() != 0)
	{
		bool Ingame_Web_Button = true;
		i3::string str_url;		i3::utf16_to_mb(url_list[0].wsURL, str_url);
		GetMainFrame()->OpenPopup(UPW_WEB_BOX, const_cast<char*>(str_url.c_str()), &Ingame_Web_Button);
	}
	return true;
}

//ClanSubPhase 방식은 서버에서 날라오는 멤버, 비멤버 구분에 따라
//멤버(USP_CLAN_MEMBER) 또는 비 멤버(USP_CLAN_NON_MEMBER)에 따라 다른 SubPhase가 열린다.
//즉, ClanSubPhase 활성화를 하기 위해 
//USP_CLAN_MEMBER 또는 USP_CLAN_NON_MEMBER 로 EnteredSubPhase 호출한다. (서버측 응답으로 구분)
bool UITopMenu_V10::ClickClan()
{
	UIMainFrame_V10 * pFrame = static_cast<UIMainFrame_V10*>(GetMainFrame());

	if(pFrame != nullptr)
	{
		if (pFrame->GetCurrentSubPhaseType() == USP_PROFILE)
		{
			pFrame->ChangeSubPhase(USP_TEMP);
			return true;
		}
		else
		{
			return _EnterSubPhase(pFrame, USP_TEMP);
		}
	}

	return false;
}

bool UITopMenu_V10::ClickMission()
{
	UIMainFrame_V10 * pFrame = static_cast<UIMainFrame_V10*>(GetMainFrame());
	if(pFrame != nullptr)
	{
		if (pFrame->GetCurrentSubPhaseType() == USP_TEMP)
		{
			pFrame->ChangeSubPhase(USP_PROFILE);
			return true;
		}
		else
		{
			return _EnterSubPhase(pFrame, USP_PROFILE);
		}
	}

	return false;
}

bool UITopMenu_V10::ClickInventory( void)
{
	UIMainFrame_V10 * pFrame = static_cast<UIMainFrame_V10*>(GetMainFrame());
	if(pFrame != nullptr)
	{
		if (pFrame->GetCurrentSubPhaseType() == USP_SHOP)
		{
			pFrame->ChangeSubPhase(USP_INVENTORY);
			return true;
		}
		else
		{
			return _EnterSubPhase(pFrame, USP_INVENTORY);
		}
	}
	
	return false;
}

bool UITopMenu_V10::ClickShop( void)
{
	if( ! g_pConfigEx->GetShop().Enable )
	{
		/*차후에 지원될 예정입니다.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, 
			GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_UNDER_CONSTRUCTION"));
		return false;
	}

	//CShopContext* pShopContext = CShopContext::i();
	//pShopContext->BuildGoodsList();

	UIMainFrame_V10 * pFrame = static_cast<UIMainFrame_V10*>(GetMainFrame());
	if (pFrame != nullptr)
	{
/*		if (pFrame->GetCurrentSubPhaseType() == USP_INVENTORY)
		{
			//@eunil. PBBUG-15311 인벤에서 상점 갈때도 최신화
			//장비 변경 교체 한다.
			UISideMenu::instance()->send_change_equip();

			pFrame->ChangeSubPhase(USP_SHOP);
			return true;
		}
		else
		{
			return _EnterSubPhase(pFrame, USP_SHOP);
		}*/

		if ( pFrame->GetCurrentSubPhaseType() != USP_SHOP)
		{
			return pFrame->EventEnterSubPhase( USP_SHOP);
		}
		else
		{
			return _EnterSubPhase(pFrame, USP_SHOP);
		}
	}

	return false;
}

void UITopMenu_V10::ClickCommunity( void)
{
	//UIFloatCommunity * pThis = static_cast<UIFloatCommunity *>(GameUI::GetFloating(UFW_COMMUNITY));
	//pThis->SwitchClick();
	
	SetEnableCommunity(!m_bCommunity);
}

void UITopMenu_V10::ClickOption( void)
{
	GetMainFrame()->TogglePopup( UPW_OPTION);
}

void UITopMenu_V10::_SetPushedButton_V10()
{
	INT32 find_PushBtn_idx = -1;

	if (m_bCommunity)
		find_PushBtn_idx = Community;
	else
	{
		switch (GetMainFrame()->GetCurrentSubPhaseType())
		{

		case	USP_SHOP:
			find_PushBtn_idx = Shop;
			break;
		case	USP_INVENTORY:
			find_PushBtn_idx = Inven;
			break;
		case    USP_PROFILE:
			find_PushBtn_idx = Missioin;
			break;
		case    USP_TEMP:
			find_PushBtn_idx = Clan;
			break;
		default:
			find_PushBtn_idx = -1;
			break;
		}
	}

	if (m_iPrePushBtn_idx != find_PushBtn_idx)
	{
		if (find_PushBtn_idx > -1)
		{
			AllMenuBtnShapeToNormal();
			for each(PushButton _PushBtn in m_PushBtnList)
			{
				if (_PushBtn.i32Idx == find_PushBtn_idx)
					_PushBtn.SetPushed();
			}
		}
		else
		{
			AllMenuBtnShapeToNormal();
		}

		m_iPrePushBtn_idx = find_PushBtn_idx;
	}
}

/*virtual*/void UITopMenu_V10::AllMenuBtnShapeToNormal( void )
{
	for(size_t idx = 0 ; idx < m_PushBtnList.size(); ++idx)
	{
		if (m_PushBtnList[idx].GetPushed())
			m_PushBtnList[idx].SetPushed();
	}
}

/*virtual*/void UITopMenu_V10::SyncSubPhaseWithMenuBtnState()
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	UISUBPHASE currentSubPhaseType = pFrame->GetCurrentSubPhaseType();
	INT32 tabValue = -1;

	switch (currentSubPhaseType)
	{
		case USP_NONE:			tabValue = -1;			break;
		case USP_TEMP:			tabValue = Clan;		break;
		case USP_PROFILE:		tabValue = Missioin;	break;
		case USP_INVENTORY:		tabValue = Inven;		break;
		case USP_SHOP:			tabValue = Shop;		break;
		//case USP_GIFTSHOP:    tabValue = GiftShop;	break;
		default:				tabValue = -1;			break;
	}

	if (tabValue == -1)
		AllMenuBtnShapeToNormal();
	/*else
	{
		if (_IsAvailablePush(tabValue))
			_SetPushedButton(tabValue);
	}*/
}

void UITopMenu_V10::SetEnableCommunity(bool enable)
{
	m_bCommunity = enable;
	if( enable == true)
		GetMainFrame()->EnableFloatWin(UFW_COMMUNITY);
	else
	{
		GetMainFrame()->DisableFloatWin(UFW_COMMUNITY);
	}

}

//-----------------------------------------------------------------------------------//

void UITopMenu_V10::ClickExit( void)
{
	OnEscapeKey();
}

void UITopMenu_V10::cb_tmenu_gauge( bool lastestTarget)
{
	if( UserInfoContext::i()->IsDailyGiftGauge() == false || UserInfoContext::i()->IsUserInfoDaily() == false)
	{
		mPBGaugeBar.reset(); return; 
	}

	if( UserInfoContext::i()->GetUserInfoDaily()->_ui8GetItem == DAILY_GIFT_TYPE_MAX ) 
	{
		mPBGaugeBar.reset(); return; 
	}

	INT32 idx = UserInfoContext::i()->GetUserInfoDaily()->_ui8GetItem;
	INT32 max_gauge = UserInfoContext::i()->GetDailyGiftGauge(idx);

	REAL32 factor = 0.f;
	if( max_gauge != 0 ) factor = (REAL32)(UserInfoContext::i()->GetUserInfoDaily()->_DGPlaytime / (REAL32)max_gauge);

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

	::ColorString(wstr_msg, 255,0,0);
	GameUI::PrintSystemChat(L"\n");
	GameUI::PrintSystemChat(wstr_msg);
	GameUI::PrintSystemChat(L"\n");
#endif
#endif

	if( factor == 0.f )
	{
		mPBGaugeBar.reset();
	}
	else
	{
		if( lastestTarget == true)
		{
			mPBGaugeBar.set_lastest_target_factor();
		}
		mPBGaugeBar.set_factor(factor);
	}
}

//------------------------------------------------------------------------------//
//								ShowRoom Camera									//
//------------------------------------------------------------------------------//

const char* UITopMenu_V10::_GetCameraAnimationFileName(CAM_ANIM_STATE state) const
{
	char* strTable[CAS_MAX] = {
		"Gui/Showroom/UI_V10/Camera_Idle.I3S",
		"Gui/Showroom/UI_V10/Camera_IdleToLeft.I3S",
		"Gui/Showroom/UI_V10/Camera_IdleToRight.I3S",
		"Gui/Showroom/UI_V10/Camera_LeftToIdle.I3S",
		"Gui/Showroom/UI_V10/Camera_LeftToRight.I3S",
		"Gui/Showroom/UI_V10/Camera_RightToIdle.I3S",
		"Gui/Showroom/UI_V10/Camera_RightToLeft.I3S",
	};

	return strTable[state];
}

void UITopMenu_V10::_AddLight( void)
{
	if( m_pLight == nullptr)
	{	
		m_pLight = i3Light::new_object();
		m_pLight->SetLightType( I3G_LIGHT_DIRECTIONAL);

		i3RegKey * pRoot = Config::_GetRegRoot("Stage/Stage");
		I3ASSERT( pRoot != nullptr);

		COLOR col;
		COLORREAL d_diffuse, d_specular, ambient;
		VEC4D vd_diffuse, vd_specular;
		VEC3D d_rot;

		i3RegKey * pKey = Config::FindKey( "Lobby", pRoot);
		if( pKey != nullptr)
		{
			FIND_REG_DATA( pKey, "Ambient", &col);
			i3Color::Set( &ambient, i3Color::GetR(&col),i3Color::GetG(&col),i3Color::GetB(&col),i3Color::GetA(&col) );

			//directional light
			FIND_REG_DATA( pKey, "D_Light_Diffuse", &vd_diffuse);
			i3Color::Set( &d_diffuse, getX( &vd_diffuse), getY( &vd_diffuse), getZ( &vd_diffuse),getW( &vd_diffuse));

			FIND_REG_DATA( pKey, "D_Light_Specular", &vd_specular);
			i3Color::Set( &d_specular, getX( &vd_specular), getY( &vd_specular), getZ( &vd_specular),getW( &vd_specular));

			FIND_REG_DATA( pKey, "D_Light_Rotate",	&d_rot);
		}
		else
		{
			i3Color::Set( &ambient, 0.4f, 0.4f, 0.4f, 1.0f);
			i3Color::Set( &d_diffuse, 1.0f, 0.0f, 0.0f, 1.0f);
			i3Color::Set( &d_specular, 1.0f, 1.0f, 1.0f, 1.0f);

			i3Vector::Set( &d_rot, -12.0f, -34.0f, 4.0f);
		}

		m_pLight->SetDiffuse( &d_diffuse);
		m_pLight->SetSpecular( &d_specular);

		g_pFramework->getSgContext()->setAmbient( &ambient);

		MATRIX mtx;

		i3Matrix::SetTranslate( &mtx, 0.0f, 0.0f, getZ( &d_rot));
		i3Matrix::PostRotateX( &mtx, I3_DEG2RAD( getY( &d_rot)));
		i3Matrix::PostRotateY( &mtx, I3_DEG2RAD( getX( &d_rot)));

		m_pLight->SetWorldSpaceTransform( &mtx);

		g_pFramework->addLight( m_pLight);
	}
}

void UITopMenu_V10::_LoadBG( void)
{
	I3ASSERT( m_pAttrSet == nullptr);
	m_pAttrSet = i3AttrSet::new_object();

	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
		pAttr->Set( true);
		m_pAttrSet->AddAttr( pAttr);
	}

	// 3D 배경 읽기
	if( m_pBG == nullptr)
	{
		i3SceneFile file;
		i3::rc_string strFileName;

		if ( g_pEnvSet->GetUIVersion() == UFV_1_2)
		{
			i3::sprintf(strFileName, "Gui/Showroom/UI_V15/Gunshowcaseroom_Lighting.i3s");
		}
		else
		{
			i3::sprintf(strFileName, "Gui/Showroom/UI_V10/Gunshowcaseroom_Lighting.i3s");
		}

		if ( file.Load( strFileName.c_str()) == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_WARN, "Couldn't find %s. Load Default Setting", "Gunshowcaseroom_Lighting.i3s");
			return;
		}

		m_pBG = file.GetSceneGraph();
		I3_MUST_ADDREF(m_pBG);

		i3BoundBox * pBox = i3BoundBox::new_object_ref();
		m_pBG->GetWrappingBound( pBox);
		m_pBG->SetBound( pBox);
	}

	m_pAttrSet->AddChild( m_pBG);

	m_pCamNode = i3Node::new_object_ref();
#if defined( I3_DEBUG)
	m_pCamNode->SetName( "CamNode");
#endif
	m_pAttrSet->AddChild( m_pCamNode);

	g_pFramework->Get3DRoot()->AddChild( m_pAttrSet);

	if( m_pRefAttr == nullptr)
	{	// ReflectionMap bind
		i3ImageFile refFile;

		i3::rc_string strFileName;
		if ( g_pEnvSet->GetUIVersion() == UFV_1_2)
		{
			i3::sprintf(strFileName, "Gui/Showroom/UI_V15/Gunshowcaseroom_Ref.dds");
		}
		else
		{
			i3::sprintf( strFileName, "Gui/Showroom/UI_V10/Gunshowcaseroom_Ref.dds");
		}

		i3TextureCube * pTex = (i3TextureCube*) refFile.Load( strFileName.c_str());
		I3ASSERT( pTex != nullptr);

		{
			m_pRefAttr = i3ReflectMapBindAttr::new_object();
			m_pRefAttr->SetTexture( pTex);
			I3_MUST_RELEASE(pTex);

			m_pAttrSet->AddAttr( m_pRefAttr);
		}
	}

	if( m_pCam[0] == nullptr)
	{
		// load camera animation 
		for( UINT32 i = 0; i < CAS_MAX; i++)
		{
			i3SceneFile camFile;
			if( camFile.Load( _GetCameraAnimationFileName((CAM_ANIM_STATE)i)) == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_WARN,  "Invalid I3S file (%s)", _GetCameraAnimationFileName((CAM_ANIM_STATE)i) );
				return;
			}

			m_pCam[ i] = camFile.GetSceneGraph();
			I3_MUST_ADDREF( m_pCam[ i]);

			i3::vector<i3Node*> transList;
			i3Scene::FindNodeByExactType( m_pCam[ i], i3Transform::static_meta(), transList);
			for( UINT32 j = 0; j < transList.size(); j++)
			{
				i3Transform * pTrans = static_cast<i3Transform*>(transList.at(j));
				if( pTrans->IsAnimated())
					pTrans->SetPlayMode( I3_ANIM_PLAY_MODE_CLAMP);
			}

			i3Camera * pCam = static_cast<i3Camera*>( i3Scene::FindNodeByType( m_pCam[ i], i3Camera::static_meta()));
			I3ASSERT( pCam != nullptr);

			// MAX에서 설정된 Camera의 FOV는 세로 FOV가 아니라 가로 FOV이다.
			// 그러나 i3Export에서 가로 FOV를 그냥 세로 FOV값으로 설정하면서, 실제로는 더욱 큰 FOV로 바뀌는
			// 버그를 피하기 위해, 강제로 일정한 값으로 설정함.
			pCam->SetFOV( I3_DEG2RAD( 34.5f));
			pCam->SetPerspective( pCam->GetFOV(), (REAL32) g_pEnvSet->m_nScreenWidth, (REAL32) g_pEnvSet->m_nScreenHeight, pCam->GetNear(), pCam->GetFar());
		}

		m_CurCameraState = CAS_IDLE;

		m_pCamNode->AddChild( m_pCam[ m_CurCameraState]);

		// select current camera animation
		i3Camera * pCam = static_cast<i3Camera*>( i3Scene::FindNodeByType( m_pCam[ m_CurCameraState], i3Camera::static_meta()));
		I3ASSERT( pCam != nullptr);
		g_pFramework->getSgContext()->setCurrentCamera( pCam);
	}
}

void UITopMenu_V10::UpdateCamera( void)
{
	CAM_ANIM_STATE newState = CAS_IDLE;

	REAL32 rPlayTime = ((i3Transform*)(m_pCam[m_CurCameraState]->GetChild(0)))->getPlayTime();
	REAL32 rDuration = ((i3Transform*)(m_pCam[m_CurCameraState]->GetChild(0)))->GetDuration();

	switch( m_CurCameraState)
	{
	case CAS_IDLE:				// fall throught
	case CAS_LEFT_TO_IDLE:		// fall throught
	case CAS_RIGHT_TO_IDLE:
		switch( m_CurCameraLook)
		{
		case CL_IDLE:	return;
		case CL_SHOP:	newState = CAS_IDLE_TO_RIGHT;	break;
		case CL_WEAPON: newState = CAS_IDLE_TO_LEFT;	break;
		}
		break;
	case CAS_IDLE_TO_LEFT:		// fall throught
	case CAS_RIGHT_TO_LEFT:
		switch( m_CurCameraLook)
		{
		case CL_IDLE:	newState = CAS_LEFT_TO_IDLE;	break;
		case CL_SHOP:	newState = CAS_LEFT_TO_RIGHT;	break;
		case CL_WEAPON: return;
		}
		break;
	case CAS_IDLE_TO_RIGHT:		// fall throught
	case CAS_LEFT_TO_RIGHT:
		switch( m_CurCameraLook)
		{
		case CL_IDLE:	newState = CAS_RIGHT_TO_IDLE;	break;
		case CL_SHOP:	return;
		case CL_WEAPON:	newState = CAS_RIGHT_TO_LEFT;	break;
		}
		break;
	default:
		I3PRINTLOG(I3LOG_FATAL, "invalid camera state!!!!");	// 카메라 스테이트가 잘못되었습니다.
		break;
	}

	// 애니메이션 중이라면 변경하지 않는다.
	if( m_CurCameraState != newState && (rPlayTime > rDuration) )
	{
		// 애니메이션을 교체한다
		m_pCamNode->RemoveChild(m_pCam[m_CurCameraState]);

		m_CurCameraState = newState;

		m_pCamNode->AddChild(m_pCam[m_CurCameraState]);

		i3::vector<i3Node*> transList;
		i3Scene::FindNodeByExactType( m_pCam[ m_CurCameraState], i3Transform::static_meta(), transList);

		for( UINT32 i = 0; i < transList.size(); i++)
		{
			i3Transform * pTrans = static_cast<i3Transform*>( transList.at(i));
			if( pTrans->IsAnimated())
			{
				pTrans->SetPlayCount(0);
				pTrans->SetPlayTime(0.0f);
			}
		}

		{
			i3Camera * pCam = (i3Camera *) i3Scene::FindNodeByExactType( m_pCam[ m_CurCameraState], i3Camera::static_meta());
			I3ASSERT( pCam != nullptr);

			g_pFramework->getSgContext()->setCurrentCamera( pCam);

			// Aspect Ratio의 조정
			pCam->SetPerspective( pCam->GetFOV(), (REAL32) g_pViewer->GetViewWidth(), (REAL32) g_pViewer->GetViewHeight(), pCam->GetNear(), pCam->GetFar());
		}
	}
}

void UITopMenu_V10::SetOriginalCamera()
{
	i3Camera * pCam = static_cast<i3Camera*>( i3Scene::FindNodeByType( m_pCam[ m_CurCameraState], i3Camera::static_meta()));
	I3ASSERT( pCam != nullptr);
	g_pFramework->getSgContext()->setCurrentCamera( pCam);
}

void UITopMenu_V10::SetUICharaCamera( CAM_LOOK_CMD state)
{
	m_CurCameraLook = state;
}

//------------------------------------------------------------------------------//
//								       MyInfo									//
//------------------------------------------------------------------------------//

void UITopMenu_V10::InitMyInfo()
{
	_SetMyDefaultInfo();
	_SetUserExp();
	_SetMoney();

#if defined(NATION_FLAG)
	_SetMyNationFlag();
#endif
}

void UITopMenu_V10::UpdateMyInfo()
{
	_SetMyDefaultInfo();
	_SetUserExp();
	_SetMoney();

#if defined(NATION_FLAG)
	_SetMyNationFlag();
#endif
}

void UITopMenu_V10::_SetMyNationFlag()
{
	LuaState * L = UILua::CallLuaFunction(GetScene(m_myinfo_scn_info.idx), "SetNationFlag");
	if(L != nullptr)
	{
		i3Lua::PushInteger( L, (INT32)UserInfoContext::i()->GetNationalCode());
		UILua::EndLuaFunction( L, 1);
	}
}

void UITopMenu_V10::_SetUserExp()
{
	USER_INFO_BASIC _MyInfoBasic;
	UserInfoContext::i()->GetMyUserInfoBasic(&_MyInfoBasic);

	i3::stack_wstring wstrExpText;
	INT32 nExpRate = 100;
	UINT32 wRank = _MyInfoBasic.m_ui32Rank;

	INT32 nCurrentExp = _MyInfoBasic.m_ui32Exp - GameUI::GetMaxExp(wRank);
	nCurrentExp = MAX(nCurrentExp, 0);

	// 상대평가라도 목표 경험치 추가
	if (GameUI::is_hero_rank(wRank, true) == true)
	{
		i3::sprintf(wstrExpText, L"%d (100%%)   ", _MyInfoBasic.m_ui32Exp);
	}
	else
	{
		INT32 nMaxExp = GameUI::GetMaxExp(wRank + 1) - GameUI::GetMaxExp(wRank);
		//if( nMaxExp != 0) nExpRate = (nCurrentExp * 100) / nMaxExp;

		//nExpRate = MINMAX(0, nExpRate, 100);	// 100%가 넘을 경우 100%으로 고정 출력
		nExpRate = GameUI::GetExpRate(wRank, nCurrentExp);
		i3::sprintf( wstrExpText, L"%d/%d (%d%%)  ", nCurrentExp, nMaxExp, nExpRate);
	}

	LuaState * L = UILua::CallLuaFunction(GetScene(m_myinfo_scn_info.idx), "SetExp");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, wstrExpText);
		i3Lua::PushNumber( L, nExpRate * 0.01f);
		UILua::EndLuaFunction( L, 2);
	}
}

void UITopMenu_V10::_SetMoney()
{
	USER_INFO_BASIC _MyInfoBasic;
	UserInfoContext::i()->GetMyUserInfoBasic(&_MyInfoBasic);

	LuaState * L = UILua::CallLuaFunction(GetScene(m_myinfo_scn_info.idx), "SetMoney");
	if ( L != nullptr)
	{
		i3::stack_wstring wstrPoint;	i3_locale::itoa_comma(_MyInfoBasic.m_ui32Point, wstrPoint);
		i3::stack_wstring wstrCash;		i3_locale::itoa_comma(_MyInfoBasic.m_ui32Cash, wstrCash);

		if( wstrPoint.length() > 0)
			i3Lua::PushStringUTF16To8(L , wstrPoint);
		else
			i3Lua::PushString(L, L"0");

		if( wstrCash.length() > 0)
			i3Lua::PushStringUTF16To8(L , wstrCash);
		else
			i3Lua::PushString(L, L"0");

		UILua::EndLuaFunction( L, 2);
	}
}

//------------------------------------------------------------------------------//
//							Received Game Eent Functions						//
//------------------------------------------------------------------------------//

void UITopMenu_V10::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UITopMenu_V10::ReceiveGameEvent_Update_MyInfo( INT32, const i3::user_data&)
{
	UpdateMyInfo(); 
}

void UITopMenu_V10::ReceiveGameEvent_Update_GiftGoods(INT32, const i3::user_data&)
{

}

void UITopMenu_V10::ReceiveGameEvent_Update_ReceiveNote(INT32, const i3::user_data&)
{

}

void UITopMenu_V10::ReceiveGameEvent_Update_IngameURL(INT32, const i3::user_data&)
{
	SetYouTubeButton();
}

