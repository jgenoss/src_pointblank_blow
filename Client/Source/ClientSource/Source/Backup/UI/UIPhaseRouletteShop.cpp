#include "pch.h"
#include "UIPhaseRouletteShop.h"
#include "UIRouletteView.h"
#include "UIMainFrame.h"
#include "Shop.h"
#include "UI/UIUtil.h"
#include "UIShopFunction.h"
#include "UIFloatChat.h"

#include "i3Base/string/ext/sprintf.h"
#include "i3Base/string/compare/generic_is_equal.h"
#include "i3Base/string/ext/mb_to_utf16.h"

// 아이콘 에니메이션 관련 기능

AniData * UIAniItem::OnUpdate(REAL32 rDeltaSeconds)
{
	for(INT32 i = 0; i < 5; i++)
	{
		if( m_pDataList[i].pAniImage == NULL)
			return NULL;
		if( !m_pDataList[i].pAniImage->isEnable() && !m_pList.empty())
		{
			m_pDataList[i].pItem = m_pList.front();
			m_pList.pop();
			m_pDataList[i].iCount = 0;
			m_pDataList[i].pAniImage->EnableCtrl(true);
			VEC3D pvec;
			i3Vector::Zero( &pvec);
			m_pDataList[i].pAniImage->setPos(&pvec);
			m_pDataList[i].pAniImage->setSize( 66.f, 70.f);
			m_pDataList[i].pAniImage->SetShapeIdx( m_pDataList[i].pItem.pItemtype );
		}
	}

	for(INT32 i = 0; i < 5; i++)
	{
		if( !m_pDataList[i].pAniImage->isEnable())
			continue;

		if(m_pDataList[i].rTime < m_pDataList[i].rCurTime)
		{
			if( m_pDataList[i].iCount > -500 )
			{
				VEC3D * pvec = m_pDataList[i].pAniImage->getPos();
				VEC2D * psize = m_pDataList[i].pAniImage->getSize();
				pvec->z = 0.0f;
				pvec->x = m_pDataList[i].iCount;
				if( m_pDataList[i].iCount > -250 )
				{
					pvec->y -= 2;
					m_pDataList[i].pAniImage->setSize( psize->x+4, psize->y+4);
				}
				else
				{
					pvec->y += 2;
					m_pDataList[i].pAniImage->setSize( psize->x-4, psize->y-4);
				}
				m_pDataList[i].pAniImage->setPos(pvec);
				m_pDataList[i].rCurTime = 0;
				m_pDataList[i].iCount-=15;
			}
			else
			{
				m_pDataList[i].pAniImage->EnableCtrl(false);
				return &m_pDataList[i].pItem;
			}
		}
		else
			m_pDataList[i].rCurTime += rDeltaSeconds;
	}
	return NULL;

}


//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//

static UIPhaseRouletteShop* GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);
	UIPhaseSubBase* pSubPhase = pFrame->GetCurrentSubPhase();

	if( pSubPhase != NULL)
	{
		I3ASSERT( i3::same_of<UIPhaseRouletteShop*>(pSubPhase)); //->IsExactTypeOf( UIPhaseRouletteShop::static_meta()));
		return static_cast<UIPhaseRouletteShop*>(pSubPhase);
	}

	return NULL;
}


extern "C" {
	int UIRoulette_Window_Start( LuaState * L)
	{
		UIPhaseRouletteShop * pThis = GetThis();

		if( pThis != NULL)
		{
			bool bOK = pThis->Start();
			i3Lua::PushBoolean(L, bOK);
			return 1;
		}
		return 0;
	}

	int UIRoulette_Window_Shuffle( LuaState * L)
	{
		UIPhaseRouletteShop * pThis = GetThis();

		if( pThis != NULL)
		{
			bool bOK = pThis->Shuffle();
			i3Lua::PushBoolean(L, bOK);
			return 1;
		}

		return 0;
	}

	int UIRoulette_Window_Init( LuaState * L)
	{
		UIPhaseRouletteShop * pThis = GetThis();

		if( pThis != NULL)
		{
			bool bOK = pThis->Init();
			pThis->Click_Select_Tab( pThis->GetSelectTap());
			i3Lua::PushBoolean(L, bOK);
			return 1;
		}
		return 0;
	}
	int UIRoulette_Window_Auto( LuaState * L)
	{
		UIPhaseRouletteShop * pThis = GetThis();

		if( pThis != NULL)
		{
			bool bOK = pThis->Auto();
			i3Lua::PushBoolean(L, bOK);
			return 1;
		}

		return 0;
	}
	int UIRoulette_Window_GuideEnable( LuaState * L)
	{
		UIPhaseRouletteShop * pThis = GetThis();

		if( pThis != NULL)
		{
			pThis->GuideEnable(TRUE);
			i3Lua::PushBoolean(L, 1);
			return 1;
		}

		return 0;
	}
	
////////////////////////////RIGHT/////////////////////////////
	int UIRoulette_Item_Set( LuaState * L)
	{
		INT32 type = i3Lua::GetIntegerArg(L, 1);

		UIPhaseRouletteShop * pThis = GetThis();
		if( pThis != NULL)
			pThis->Click_Add_Item(type);
		return 0;
	}

	int UIRoulette_Item_ItemToolTip( LuaState * L)
	{
		INT32 type = i3Lua::GetIntegerArg(L, 1);
		UIPhaseRouletteShop * pThis = GetThis();
		if( pThis != NULL)
		{
			pThis->Click_Select_TooltipDisable();
			pThis->Click_Select_Tooltip(type);
		}
		return 0;
	}
	int UIRoulette_Tab_Select( LuaState * L)
	{
		INT32 type = i3Lua::GetIntegerArg(L, 1);
		UIPhaseRouletteShop * pThis = GetThis();
		if( pThis != NULL)
			pThis->Click_Select_Tab(type);
		return 0;
	}
	int UIRoulette_Hidden_Select( LuaState * L)
	{
		//INT32 type = i3Lua::GetIntegerArg(L, 1);
		UIPhaseRouletteShop * pThis = GetThis();
		if( pThis != NULL)
			pThis->Click_Select_Hiddn();
		return 0;
	}
	int UIRoulette_Item_ItemToolTipDisable( LuaState * L)
	{
		//INT32 type = i3Lua::GetIntegerArg(L, 1);
		UIPhaseRouletteShop * pThis = GetThis();
		if( pThis != NULL)
			pThis->Click_Select_TooltipDisable();
		return 0;
	}

	//Roulette GUIDE
	int UIRoulette_Guide_SetIndex( LuaState * L)
	{
		INT32 step = i3Lua::GetIntegerArg(L, 1);
		UIPhaseRouletteShop * pThis = GetThis();
		if( pThis != NULL)
			pThis->SetGuideStep(step);
		return 0;
	}
	int UIRoulette_Guide_DesableGuide( LuaState * L)
	{
		UIPhaseRouletteShop * pThis = GetThis();
		if( pThis != NULL)
			pThis->GuideEnable(FALSE);
		return 0;
	}
}

LuaFuncReg UIRoulette_Window_Glue[] =
{
	{"Start",			UIRoulette_Window_Start},
	{"Shuffle",			UIRoulette_Window_Shuffle},
	{"Init",			UIRoulette_Window_Init},
	{"Auto",			UIRoulette_Window_Auto},
	{"GuideEnable",		UIRoulette_Window_GuideEnable},
	{NULL,				NULL}
};

LuaFuncReg UIRoulette_Guide[] =
{
	{"SetGuideIndex",			UIRoulette_Guide_SetIndex},
	{"DesableGuide",			UIRoulette_Guide_DesableGuide},
	{NULL,				NULL}
};

LuaFuncReg UIRoulette_RightUI_Glue[] =
{
	{"SetItem",				UIRoulette_Item_Set},
	{"ItemTooltip",			UIRoulette_Item_ItemToolTip},
	{"Tab_Select",			UIRoulette_Tab_Select},
	{"Hidden_Select",		UIRoulette_Hidden_Select},	
	{"ItemTooltipDisable",	UIRoulette_Item_ItemToolTipDisable},	
	{NULL,				NULL}
};

I3_CLASS_INSTANCE( UIPhaseRouletteShop);//, UIPhaseSubBase);

UIPhaseRouletteShop::UIPhaseRouletteShop()
{
	m_rElapsedTime			= 0.f;
	m_rAutoRouletteDely		= 0.f;
	m_rFreeRouletteDely		= 0.f;
	m_bAuto					= false;
	m_nAutoCount			= 5;
	m_fUpdateInfoTimer		= 0.f;
	m_bJackPot				= FALSE;
	m_fUpdateJackpotTimer	= 0.f;
	m_pSelectTapIdx			= 0;
	m_bReceiveItem			= FALSE;	
	g_pGameContext->SetRsHiddnSelect( FALSE);
	m_bCheck				= TRUE;
	m_iGradeCount			= 0;
	m_pGuideFrame			= NULL;
	m_pRightFrame			= NULL;

	for(INT32 i = 0; i < 2; i++)
	{
		m_pTooltip[i]		= NULL;
	}


	m_bFlowTime = 0.0f;

	m_pAnnounce = NULL;
	m_pJackpotLight = NULL;
	m_pRouletteView = NULL;
	m_bGuideEnable = FALSE;
}

UIPhaseRouletteShop::~UIPhaseRouletteShop()
{
	I3_SAFE_RELEASE(m_pRouletteView);
	I3_SAFE_RELEASE(m_pTooltip[0]);
	I3_SAFE_RELEASE(m_pTooltip[1]);
}


//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//
/*virtual*/ void UIPhaseRouletteShop::OnCreate( i3GameNode * pParent)
{
	

	UIPhaseSubBase::OnCreate( pParent);
	m_pTooltip[0] = UIToolTipRoulette_Weapon::new_object();
	m_pTooltip[1] = UIToolTipRoulette_NonWeapon::new_object();
	m_pTooltip[0]->OnCreate( this);
	m_pTooltip[1]->OnCreate( this);
	// Load Scene
	//FrameWindow
	AddScene( "UIRe/Scene/Main/PBRe_Roulette_FrameWnd.i3UIe", NULL);
	//RouletteWindow
	AddScene( "UIRe/Scene/Main/PBRe_Roulette.i3UIe", UIRoulette_Window_Glue);
	//RouletteItemWindow
	AddScene( "UIRe/Scene/Main/PBRe_Roulette_item.i3UIe", UIRoulette_RightUI_Glue);
	//Roulette Guide
	AddScene( "UIRe/Scene/Main/PBRe_Roulette_Guide.i3UIe", UIRoulette_Guide);
	m_pRouletteView = UIRouletteView::new_object();
	I3ASSERT( m_pRouletteView);

}


/*virtual*/ void UIPhaseRouletteShop::OnEscapeKey(void)
{
	if (!IsPlayingRoulette())
	{
		UIPhaseSubBase::OnEscapeKey();

	}
	
}


/*virtual*/ void UIPhaseRouletteShop::Leave( void)
{
	g_pFramework->GetUIMainframe()->DisableFloatWinInput( UFW_CHAT,true);
	g_pGameContext->SetEventOnce( EVENT_LEAVE_GACHA);
	g_pGameContext->SetRecvRouletteStartPacket(false);					
	m_pRouletteView->ReleaseRoulette();
	/*g_pSndMng->StopBGM();
	g_pSndMng->StartBGM("Sound/UI/PB_BGM_NO1.ogg", true, true, 1.f);*/
}
void UIPhaseRouletteShop::RefreshCash()
{
	LuaState * L = _CallLuaFunction(1, "SetCash");
	char szTemp[ MAX_STRING_COUNT];

	if( L != NULL)
	{
		sprintf_s( szTemp, "%d", g_pGameContext->GetMyCash());
		i3Lua::PushA2UTF8String( L, szTemp);
		_EndLuaFunction( L, 1);
	}
}
void UIPhaseRouletteShop::AutoToggle()
{
	LuaState * L = _CallLuaFunction(1, "SetAuto");
	char szTemp[ MAX_STRING_COUNT];

	if( L != NULL)
	{

		if (g_pGameContext->isCheckAuto())
		{
			sprintf_s( szTemp, "%s",  GAME_STRING("STBL_IDX_BUTTON_CANCEL"));
		}
		else
		{
			sprintf_s( szTemp, "%s", GAME_STRING("STR_ROULETTE_SPIN_BTN_AUTO"));
		}

		i3Lua::PushA2UTF8String( L, szTemp);
		_EndLuaFunction( L, 1);
	}


}
/*virtual*/ void UIPhaseRouletteShop::OnUpdate( REAL32 rDeltaSeconds)
{
	UIPhaseSubBase::OnUpdate(rDeltaSeconds);

	_UpdateFlowAnnounce(rDeltaSeconds);

	if (m_pRouletteView->getRouletteObject() != NULL)
	{

		//룰렛이 IDLE상태가 아닐경우에는 상단 MENU INPUT을 막습니다.
		if (m_pRouletteView->getRouletteObject()->IsRouletteIdleState())
		{
			g_pFramework->GetUIMainframe()->DisableFloatWinInput( UFW_CHAT,false);
			if(m_pAniIcon.isEnable())
				g_pFramework->GetUIMainframe()->GetTopMenu()->SetInputDisable(true);
			else
				g_pFramework->GetUIMainframe()->GetTopMenu()->SetInputDisable(false);
		}
		else
		{
				g_pFramework->GetUIMainframe()->DisableFloatWinInput( UFW_CHAT,true);
				g_pFramework->GetUIMainframe()->GetTopMenu()->SetInputDisable(true);
		}

	
		//RecvFalsePacket 서버로 실패 패킷을 받을경우 처리합니다.
		if (!g_pGameContext->getRecvFalsePacket())
		{
			g_pGameContext->SetCheckAuto(false);
			//오토카운트 초기화합니다.
			g_pGameContext->SetAutoCount(0);
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_ROULETTE_ITEMLIST_ERROR_POPUP"));
			Init();
			g_pGameContext->SetRecvFalsePacket(true);
			AutoToggle();
		}
		//팝업관련
		_UpdatePopup(rDeltaSeconds);
		//자동진행관련업데이트
		_UpdateAutoRoulette(rDeltaSeconds);
		//보너스 카운트
		_UpdateBonusCount(rDeltaSeconds);



		m_fUpdateInfoTimer += rDeltaSeconds;
		if( m_fUpdateInfoTimer > 5.0)
		{
			g_pGameContext->SetEventOnce(EVENT_ENTER_RS_INFO);
			m_fUpdateInfoTimer = 0.f;

		}

		m_fUpdateJackpotTimer += rDeltaSeconds;
		if( m_fUpdateJackpotTimer > 0.2)
		{
			SetItemInfo();
			m_fUpdateJackpotTimer = 0.f;
			m_bJackPot = !m_bJackPot;
			m_pJackpotLight->SetShapeIdx(m_bJackPot);
		}

		AniData * pitem = m_pAniIcon.OnUpdate( rDeltaSeconds);
		if(pitem != NULL)
		{
			m_pRouletteView->getRouletteObject()->SetItem(*m_pRoulette_SlotInfo[pitem->iTabidx][pitem->iItemidx],m_pTabInfo[pitem->iItemidx].Grade);
			Click_Select_TooltipDisable();
			//루아 아이템 Setting
		}
		//플레이중에는 input를 막습니다. 
		if(IsPlayingRoulette())
		{
			m_pRightFrame->setInputDisable(true);
			return;
		}
		else
		{
			m_pRightFrame->setInputDisable(false);
		}
	}

}

/*virtual*/ void	UIPhaseRouletteShop::_InitializeAtLoad(i3UIScene * pScene)
{


	UIPhaseSubBase::_InitializeAtLoad( pScene);

	
		if( m_pRouletteView != NULL )
		{

			if( i3::generic_is_iequal( pScene->GetName(), "PBRe_Roulette") )
			{

				if (pScene != NULL)
				{
					i3UIFrameWnd * pCharaWnd = (i3UIFrameWnd*) pScene->FindChildByName( "fwdCharaView");
					I3ASSERT( pCharaWnd != NULL);
					m_pRouletteView->Create3DViewBox( pCharaWnd);
					m_pRouletteView->CreateViewRoulette();
					RouletteInit();

					m_pAnnounce = (i3UIEditBox*) pScene->FindChildByName( "i3UIEditBox");
					I3ASSERT( m_pAnnounce);
					m_pAnnounce->SetTextAlign( I3UI_TEXT_ALIGN_X_LEFT, I3UI_TEXT_ALIGN_Y_MIDDLE);

					_InitFlowAnnounce();

				}

			}
			else if ( i3::generic_is_iequal( pScene->GetName(), "PBRe_Roulette_item") )
			{

				if (pScene != NULL)
				{	
					m_pJackpotLight = (i3UIButtonImageSet*) pScene->FindChildByName( "i3UIButtonImagejackpot");
					m_pRightFrame = (i3UIFrameWnd*) pScene->FindChildByName( "i3UIFrameWnd_ItemList");

					for(INT32 i = 0; i < 5; i++)
					{
						char name[255];
						i3::snprintf( name, 255, "i3UIButtonImageSetIcon%d", i);
						m_pAniIcon.SetAni( (i3UIButtonImageSet*) pScene->FindChildByName( name), i, 0.005f);
					}
				}

			}
			else if ( i3::generic_is_iequal( pScene->GetName(), "PBRe_Roulette_Guide") )
			{

				if (pScene != NULL)
				{	
					m_pGuideFrame = (i3UIFrameWnd*) pScene->FindChildByName( "i3UIFrameWnd1");
					m_pGuideStep = (i3UIButtonImageSet*)pScene->FindChildByName( "i3UIButtonImageSet0");
					GuideEnable(FALSE);
				}

			}
		
		}
		g_pSndMng->StopBGM();
		g_pSndMng->StartBGM("Sound/Roulette/Lottery_bgm.ogg", true, true, 1.f);
		m_slide.AddFrameWnds(pScene, UISlide::R_TO_L);
		g_pGameContext->SetCheckAuto(false);
		

		

}
//////////////////////////////////////////////////////////////////////////
//	NAME		: RouletteInit()
//	DESCRIPTION : 
//////////////////////////////////////////////////////////////////////////
void UIPhaseRouletteShop::RouletteInit(void)
{

	//잊지않고 초기화 해줍니다.
	g_pGameContext->SetCheckAuto(false);
	g_pGameContext->SetEventOnce(EVENT_ENTER_RS_INFO);
	SetActivateItem(-2);
	m_bReceiveItem = FALSE;
	SetItemInfo();
	m_iGradeCount = 0;
	m_pSelectTapIdx = 0;
	g_pGameContext->SetRsHiddnSelect(false);

	LuaState* L = _CallLuaFunction(1, "SetChangeCount");
	i3Lua::PushInteger(L, g_pGameContext->GetRSTap1UseCount());
	_EndLuaFunction(L, 1);

	RefreshCash();
	AutoToggle();
	m_pRouletteView->getRouletteObject()->InitButton(m_pTabInfo[m_pSelectTapIdx].Grade);


}
void	UIPhaseRouletteShop::OnLoadAllScenes()
{
	for(INT32 i = 0; i < 2; i++)
	{
		m_pTooltip[i]->SetEnable(TRUE);
	}
}
void	UIPhaseRouletteShop::OnUnloadAllScenes()
{

	m_pRouletteView->Delete3DViewBox();
	for(INT32 i = 0; i < 2; i++)
	{
		m_pTooltip[i]->SetEnable(FALSE);
	}
}
//////////////////////////////////////////////////////////////////////////
//	NAME		: Init()
//	DESCRIPTION : 초기화 버튼처리입니다.
//////////////////////////////////////////////////////////////////////////

bool UIPhaseRouletteShop::Init(void)
{

	if(IsPlayingRoulette())
		return false;

	//루아 아이템 Setting
	SetActivateItem( -1);
	g_pGameContext->SetRsHiddnSelect( FALSE);
	m_iGradeCount = 0;
	m_pAniIcon.ResetData();



	m_pRouletteView->getRouletteObject()->InitButton(m_pTabInfo[m_pSelectTapIdx].Grade);

	PlaySound(GSND_ROULETTE_CLEAR);
	
	return true;

}


void UIPhaseRouletteShop::SetFlowAnnounce( const char * pszStr)
{
	if( MAX_STRING_COUNT > i3::generic_string_size( pszStr))
	{
		// 기존 공지와 다를 경우만 바꾼다
		if (false == i3::generic_is_equal( m_AnnounceBuffer, pszStr))
		{
			i3::safe_string_copy( m_AnnounceBuffer, pszStr, MAX_STRING_COUNT);		
			m_idxAnnounceLetter = -1;			
		}
	}
}

void UIPhaseRouletteShop::SetFlowAnnounce2(const char* pszStr)
{
	if (MAX_STRING_COUNT > i3::generic_string_size( pszStr))
	{
		// 기존 공지와 다를 경우만 바꾼다
		if (false == i3::generic_is_equal(m_AnnounceBuffer2, pszStr))
		{
			i3::safe_string_copy(m_AnnounceBuffer2, pszStr, MAX_STRING_COUNT);		
			m_idxAnnounceLetter = -1;			
			m_bFCMFlag = true;
			m_bFlowFlag = true;
		}
	}
}
void UIPhaseRouletteShop::_InitFlowAnnounce(void)
{

	REAL32 rateX = g_pViewer->GetViewWidth() * 0.9765625e-3f;
	m_nAnnounceWidth = (INT32)(m_pAnnounce->getWidth() * rateX);
	m_nAnnounceWidth = MAX(0, m_nAnnounceWidth - 60);

	// dummmy
	//SetFlowAnnounce(GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_ADVERTISEMENT"));
//	SetFlowAnnounce( g_pGameContext->getNormalNotice() );

	m_idxAnnounceLetter = 0;
	m_flowState = FLOW_STATE_INIT;

}

void UIPhaseRouletteShop::_UpdateFlowAnnounce(REAL32 rDeltaSeconds)
{
//	WCHAR szText[ MAX_STRING_COUNT];
	i3::wstring wstrText;

	if( m_pAnnounce == NULL)
		return;

	if( m_pAnnounce->getScrollState() == i3UIEditBox::UBSS_START ||
		m_pAnnounce->getScrollState() == i3UIEditBox::UBSS_END )
	{
		if(m_bFlowTime > 120.0f)
		{
			m_bFCMFlag = FALSE;
			m_bFlowFlag = FALSE;
			m_bFlowTime = 0.0f;
			i3::safe_string_copy(m_AnnounceBuffer2, m_AnnounceBuffer, MAX_STRING_COUNT);
		}

		if( m_bFCMFlag)	
		{
			i3::mb_to_utf16(m_AnnounceBuffer2, wstrText);
		//	i3String::CopyASCIItoWCHAR( szText, m_strAnnounceBuffer2);
		}
		else
		{
			i3::mb_to_utf16(m_AnnounceBuffer, wstrText);
		//	i3String::CopyASCIItoWCHAR( szText, m_strAnnounceBuffer);
		}

		if( m_pAnnounce->getText() != NULL)
		{
			if( i3::generic_is_equal( m_pAnnounce->getText(), wstrText) == false)
				m_pAnnounce->SetText( wstrText.c_str());
		}

		m_pAnnounce->setScrollState( i3UIEditBox::UBSS_INIT);
	}
	else
	{
		if( m_bFlowFlag)
		{
			m_bFlowTime += rDeltaSeconds;
		}
	}

}


//////////////////////////////////////////////////////////////////////////
//	NAME		: Start()
//	DESCRIPTION : Start버튼 처리
//////////////////////////////////////////////////////////////////////////
bool UIPhaseRouletteShop::Start(void)
{


	if(IsPlayingRoulette())
		return false;


	if (g_pGameContext->isCheckAuto())
	{
		return false;
	}


	char szString[1024];

	//룰렛 상태가 IDEL 애니메이션 상태가 아니면 START버튼을 활성화 하지 않습니다. 또한 자동룰렛기능이 적용중이면 버튼을 활성화하지 않습니다.


	
	//	_isCheckRoulette()  룰렛이 다 채워지지 않으면 에러팝업을 오픈합니다.
	if (!m_pRouletteView->getRouletteObject()->isCheckRoulette())
	{
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_STRING("STR_ROULETTE_ERROR_SLOT_POPUP"));
	}
	else if (m_pSelectTapIdx ==0 &&  g_pGameContext->GetRSTap1UseCount() ==5)
	{
		m_pRouletteView->getRouletteObject()->WriteData(m_pSelectTapIdx);
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_STRING("STR_ROULETTE_SERVICE_PLAY_POPUP") , NULL, MAKE_CALLBACK(CbRouletteFreeStart), this);
	}	
	//2번탭
	else if (m_pSelectTapIdx ==1  &&  g_pGameContext->GetRSTap2UseCount() ==5)
		{
			m_pRouletteView->getRouletteObject()->WriteData(m_pSelectTapIdx);
			GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_STRING("STR_ROULETTE_SERVICE_PLAY_POPUP") , NULL, MAKE_CALLBACK(CbRouletteFreeStart), this);
		}
	//3번탭
	else if (m_pSelectTapIdx ==2  &&  g_pGameContext->GetRSTap3UseCount() ==5)
		{
				m_pRouletteView->getRouletteObject()->WriteData(m_pSelectTapIdx);
				GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_STRING("STR_ROULETTE_SERVICE_PLAY_POPUP") , NULL, MAKE_CALLBACK(CbRouletteFreeStart), this);

		}
	else
	{
		INT32 Price = g_pGameContext->GetRSInfo()->_aRSTabInfo[m_pSelectTapIdx]._ui32Price;

		sprintf_s(szString, GAME_STRING("STR_ROULETTE_START_POPUP"), Price);
		
		// 콜백 CbRouletteStart를 호출 합니다.
		GameUI::MsgBox( MSG_TYPE_GAME_QUERY, szString , NULL, MAKE_CALLBACK(CbRouletteStart), this);
		m_pRouletteView->getRouletteObject()->WriteData(m_pSelectTapIdx);
	}
	
	return true;

}
void UIPhaseRouletteShop::AutoStart(void)
{
	
	INT32 nCount;
	nCount = 	g_pGameContext->GetAutoCount();
	I3TRACE("g_pGameContext->GetAutoCount()  %d\n", g_pGameContext->GetAutoCount());
	//다시한번 체크합니다.
	if (	g_pGameContext->GetAutoCount() >0 && m_pRouletteView->getRouletteObject()->GetRouletteState() == ROULETTE_STATE_IDLE)
	{
		AutoToggle();
		m_pRouletteView->getRouletteObject()->WriteData(m_pSelectTapIdx);
		g_pGameContext->SetEventOnce(EVENT_START_ROULETTE);
		nCount--;
		I3TRACE("nCount--; %d\n",nCount);
		g_pGameContext->SetAutoCount(nCount);

	}
	else
	{
		
		g_pGameContext->SetCheckAuto(false);
		AutoToggle();

	}

}

//////////////////////////////////////////////////////////////////////////
//	NAME		: CbRouletteStart(void* pThis,INT32 nParam)
//	DESCRIPTION : Callback함수
//////////////////////////////////////////////////////////////////////////
void UIPhaseRouletteShop::CbRouletteStart(void* pThis,INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		g_pGameContext->SetEventOnce(EVENT_START_ROULETTE);
	

	}
	else if( nParam == MBR_CANCEL || g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ESC ))
	{
		//취소버튼을 누를 경우는 어떠한 행동도 하지 않습니다.
	}

}
//////////////////////////////////////////////////////////////////////////
//	NAME		: CbRouletteFreeStart(void* pThis,INT32 nParam)
//	DESCRIPTION : Callback함수
//////////////////////////////////////////////////////////////////////////
void UIPhaseRouletteShop::CbRouletteFreeStart(void* pThis,INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		
		g_pGameContext->SetEventOnce(EVENT_START_ROULETTE);
	}

}
//////////////////////////////////////////////////////////////////////////
//	NAME		: Auto()
//	DESCRIPTION : 자동플레이 설정을 합니다.
//////////////////////////////////////////////////////////////////////////
bool UIPhaseRouletteShop::Auto(void)
{
	if(IsPlayingRoulette() && !g_pGameContext->isCheckAuto())
		return false;



	//오토활성화후 한번 더 클릭하면 오토 취소
	if (g_pGameContext->isCheckAuto())
	{
		g_pGameContext->SetCheckAuto(false);
		g_pGameContext->SetAutoCount(0);
		AutoToggle();
		GameUI::MsgBox( MSG_TYPE_ERROR, "연속기능이 취소되었습니다.\n");
		return false;
	}


	if (!m_pRouletteView->getRouletteObject()->isCheckRoulette())
	{
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_STRING("STR_ROULETTE_ERROR_SLOT_POPUP"));
	}
	else
	GetMainFrame()->OpenPopup(UPW_ROULETTE_AUTO,NULL);
	return true;
}
//////////////////////////////////////////////////////////////////////////
//	NAME		: Shuffle()
//	DESCRIPTION : 슬롯을 섞습니다.
//////////////////////////////////////////////////////////////////////////
bool UIPhaseRouletteShop::Shuffle(void)
{

	//룰렛애니메이션상태가 IDLE이 아니거나 룰렛슬롯에 아이템이 모두 셋팅되어 있지않으면 Shuffle할수 없도록 설정합니다.
	if(IsPlayingRoulette() || !m_pRouletteView->getRouletteObject()->isCheckRoulette())
		return false;


	

	m_pRouletteView->getRouletteObject()->Shuffle();

	PlaySound(GSND_ROULETTE_SHUFFLE);

	return true;
}

//////////////////////RIGHTUI///////////////////
//////////////////////////////////////////////////////////////////////////
//	NAME		: Click_Add_Item()
//	DESCRIPTION : 룰렛에 아이템 정보를 전달합니다.
//////////////////////////////////////////////////////////////////////////
void UIPhaseRouletteShop::Click_Add_Item(INT32 pidx)
{
	if(!g_pGameContext->GetRsHiddnSelect())
	{
		if(m_pRoulette_SlotInfo[m_pSelectTapIdx][pidx] == NULL)
			return;
		INT32 pindex = 0;
		INT32 Grade = 0;
		pindex = m_pRoulette_SlotInfo[m_pSelectTapIdx][pidx]->_eItemGrade;
		Grade  = m_pTabInfo[m_pSelectTapIdx].Grade;
		//별등급 갯수를 초과할 경우 리턴시킵니다.	
		if(!CheckStar(m_pRoulette_SlotInfo[m_pSelectTapIdx][pidx]->_eItemGrade))
			return;

		AniData pdata;
		pdata.iTabidx = m_pSelectTapIdx;
		pdata.iItemidx = pidx;
		pdata.pItemtype = m_pRouletteView->getRouletteObject()->ConvertItemIdToClassType(g_pGameContext->ConvertGoodsID(m_pRoulette_SlotInfo[m_pSelectTapIdx][pidx]->_ui32GoodsID));
		SetActivateItem(pidx);
		m_pAniIcon.AddData(pdata);
		Click_Select_TooltipDisable();

			
		PlaySound(GSND_ROULETTE_ITMESET);
		
	}
	
}

//////////////////////////////////////////////////////////////////////////
//	NAME		: Click_Select_Tab()
//	DESCRIPTION : 룻렛정보의 텝을 클릭햇을때 초기화 합니다.
//////////////////////////////////////////////////////////////////////////
void UIPhaseRouletteShop::Click_Select_Tab(INT32 pidx)
{
		if(IsPlayingRoulette())
			return;


		m_pSelectTapIdx = pidx;
		m_iGradeCount = 0;
		SetActivateItem(-1);
		g_pGameContext->SetTapIndex(m_pSelectTapIdx);

		m_pRouletteView->getRouletteObject()->InitButton(m_pTabInfo[m_pSelectTapIdx].Grade);

	
		Init();
		SetItemInfo();

		LuaState* L = _CallLuaFunction(1, "SetChangeCount");
		if (L == NULL)
		{
			I3ASSERT_0;
		}
		//1번탭
		if (m_pSelectTapIdx == 0)
		{
			i3Lua::PushInteger(L, g_pGameContext->GetRSTap1UseCount());	
		}
		//2번탭
		else if (m_pSelectTapIdx==1)
		{
			i3Lua::PushInteger(L, g_pGameContext->GetRSTap2UseCount());	
		}
		//3번탭
		else if (m_pSelectTapIdx==2)
		{
			i3Lua::PushInteger(L, g_pGameContext->GetRSTap3UseCount());	
		}
		_EndLuaFunction(L, 1);
		
}

void UIPhaseRouletteShop::Click_Select_Tooltip(INT32	pidx)
{
	if(m_pRoulette_SlotInfo[m_pSelectTapIdx][pidx] == NULL)
		return;
	VEC2D pvec;
	pvec.x = 250;pvec.y = 100;
	INT32 goodsClass = 0;
	goodsClass = GET_ITEM_FLAG_TYPE( g_pGameContext->ConvertGoodsID( m_pRoulette_SlotInfo[m_pSelectTapIdx][pidx]->_ui32GoodsID) );

	switch((ITEM_TYPE) goodsClass)
	{
	case ITEM_TYPE_PRIMARY:
	case ITEM_TYPE_SECONDARY:
	case ITEM_TYPE_MELEE:
	case ITEM_TYPE_THROWING1:
	case ITEM_TYPE_THROWING2:
		m_pTooltip[0]->EnableCurrentSlotToolTip(pvec, m_pRoulette_SlotInfo[m_pSelectTapIdx][pidx]->_ui32GoodsID);
		break;
	default:
		m_pTooltip[1]->EnableCurrentSlotToolTip(pvec, m_pRoulette_SlotInfo[m_pSelectTapIdx][pidx]->_ui32GoodsID);
		break;
	}
}

void UIPhaseRouletteShop::Click_Select_TooltipDisable()
{
	m_pTooltip[0]->DisableToolTip();
	m_pTooltip[1]->DisableToolTip();
}

void UIPhaseRouletteShop::Click_Select_Hiddn()
{
	if (IsPlayingRoulette())
	{
		return;
	}

	if(TRUE ==g_pGameContext->GetRsHiddnSelect())
		return;
	Init();
	g_pGameContext->SetRsHiddnSelect( TRUE);
	SetItemInfo();
	for(INT32 i = 0; i < MAX_RS_HIDDEN_ITEM_COUNT; i++)
	{
		INT32 pindex = 0;
		INT32 Grade = 0;
		pindex = m_pTabInfo[m_pSelectTapIdx].Info[i]->_eItemGrade;
		Grade =	 m_pTabInfo[m_pSelectTapIdx].Grade;	

		//별등급 갯수를 초과할 경우 리턴시킵니다.	
		if(!m_pRouletteView->getRouletteObject()->CheckStar(pindex,Grade))
			return;

		m_pRouletteView->getRouletteObject()->SetItem(*m_pTabInfo[m_pSelectTapIdx].Info[i],pindex);
	}

}
//////////////////////////////////////////////////////////////////////////
//	NAME		: UIPhaseRouletteShop()
//	DESCRIPTION : 
//////////////////////////////////////////////////////////////////////////
/*virtual*/ bool UIPhaseRouletteShop::OnKeyInput(i3InputDeviceManager * pInputMng)
{
	
	//i3InputDeviceManager* pInputMgr = g_pViewer->GetInputDeviceManager();
	i3InputKeyboard* pKey = pInputMng->GetKeyboard();
	//char szString[MAX_STRING_COUNT] = "";

	if( pKey->GetStrokeState( I3I_KEY_NUMPAD0))
	{
		g_pGameContext->SetRecvRouletteStartPacket(true);
		g_pGameContext->SetRSTestCode(1);
	}
	else if( pKey->GetStrokeState( I3I_KEY_NUMPAD1))
	{
		g_pGameContext->SetRecvRouletteStartPacket(true);
		g_pGameContext->SetRSTestCode(2);
	}
	else if( pKey->GetStrokeState( I3I_KEY_NUMPAD2))
	{
		g_pGameContext->SetRecvRouletteStartPacket(true);
		g_pGameContext->SetRSTestCode(3);
	}
	else if( pKey->GetStrokeState( I3I_KEY_NUMPAD3))
	{
		g_pGameContext->SetRecvRouletteStartPacket(true);
		g_pGameContext->SetRSTestCode(4);
	}
	else if( pKey->GetStrokeState( I3I_KEY_NUMPAD4))
	{
		g_pGameContext->SetRecvRouletteStartPacket(true);
		g_pGameContext->SetRSTestCode(5);
	}
	else if( pKey->GetStrokeState( I3I_KEY_NUMPAD5))
	{
		g_pGameContext->SetRecvRouletteStartPacket(true);
		g_pGameContext->SetRSTestCode(6);
	}
	else if( pKey->GetStrokeState( I3I_KEY_NUMPAD6))
	{
		g_pGameContext->SetRecvRouletteStartPacket(true);
		g_pGameContext->SetRSTestCode(7);
	}
	else if( pKey->GetStrokeState( I3I_KEY_NUMPAD7))
	{
		g_pGameContext->SetRecvRouletteStartPacket(true);
		g_pGameContext->SetRSTestCode(8);
	}
	else if( pKey->GetStrokeState( I3I_KEY_NUMPAD8))
	{
		g_pGameContext->SetRecvRouletteStartPacket(true);
		g_pGameContext->SetRSTestCode(9);
	}
	else if( pKey->GetStrokeState( I3I_KEY_NUMPAD9))
	{
		g_pGameContext->SetRecvRouletteStartPacket(true);
		g_pGameContext->SetRSTestCode(10);
	}


	if( pKey->GetPressState( I3I_KEY_LCTRL))
	{
		if( pKey->GetStrokeState( I3I_KEY_NUMPAD0))
		{
			g_pGameContext->SetRecvRouletteStartPacket(true);
			g_pGameContext->SetRSTestCode(11);
		}
		else if( pKey->GetStrokeState( I3I_KEY_NUMPAD1))
		{
			g_pGameContext->SetRecvRouletteStartPacket(true);
			g_pGameContext->SetRSTestCode(12);
		}
		else if( pKey->GetStrokeState( I3I_KEY_NUMPAD2))
		{
			g_pGameContext->SetRecvRouletteStartPacket(true);
			g_pGameContext->SetRSTestCode(13);
		}
	}
	return UIPhaseSubBase::OnKeyInput( pInputMng);
}

//////////////		RIGHT UI		//////////////////
//////////////////////////////////////////////////////////////////////////
//	NAME		: SetUiItem()
//	DESCRIPTION : 뽑기상점 UI 를 세팅합니다.
//////////////////////////////////////////////////////////////////////////
void	UIPhaseRouletteShop::SetUiItem()
{
	for(INT32 i = 0; i < 18; i++)
	{
		if(m_pRoulette_SlotInfo[m_pSelectTapIdx][i] != NULL)
		{
			INT32 itemID = m_pRoulette_SlotInfo[m_pSelectTapIdx][i]->_ui32GoodsID;
			i3::string Name;
			char pQuantity[MAX_STRING_COUNT] = {0,};
			i3::snprintf( pQuantity, MAX_STRING_COUNT, "%2d/%2d", m_pRoulette_SlotInfo[m_pSelectTapIdx][i]->_ui32CurUnit, m_pRoulette_SlotInfo[m_pSelectTapIdx][i]->_ui32MaxUnit);
			INT32 pGauge = INT32((float(m_pRoulette_SlotInfo[m_pSelectTapIdx][i]->_ui32CurUnit) / float(m_pRoulette_SlotInfo[m_pSelectTapIdx][i]->_ui32MaxUnit)) * 5);

			GetGoodsName( Name, itemID);
			//루아 아이템 Setting
			LuaState* L = _CallLuaFunction(2, "SetItem");
			if(L != NULL)
			{
				i3Lua::PushInteger(L, i);
				i3Lua::PushInteger(L, m_pRoulette_SlotInfo[m_pSelectTapIdx][i]->_eItemGrade);
				i3Lua::PushA2UTF8String(L, Name.c_str());
				i3Lua::PushA2UTF8String(L, pQuantity);
				i3Lua::PushInteger(L, (pGauge<0)?1:pGauge);
				_EndLuaFunction(L, 5);
			}
		}
		else
		{
			LuaState* L = _CallLuaFunction(2, "SetEnableBtn");
			if(L != NULL)
			{
				i3Lua::PushInteger(L, i);
				i3Lua::PushBoolean(L, FALSE);
				_EndLuaFunction(L, 2);
			}
		}

	}

	//루아 아이템 Setting
	LuaState* L = _CallLuaFunction(2, "SetTap");
	if(L != NULL)
	{
		char Name[3][MAX_STRING_COUNT]= {{0,},{0,},{0,}};
		for(INT32 i=0; i < 3; i++)
		{
			i3::snprintf( Name[i], MAX_STRING_COUNT, "%d ★ %d CASH", m_pTabInfo[i].Grade, m_pTabInfo[i].Price);
		}
		i3Lua::PushA2UTF8String(L, Name[0]);
		i3Lua::PushA2UTF8String(L, Name[1]);
		i3Lua::PushA2UTF8String(L, Name[2]);
		_EndLuaFunction(L, 3);
	}
}

//////////////////////////////////////////////////////////////////////////
//	NAME		: GetGoodsName()
//	DESCRIPTION : goods아이디를 이름으로 변환합니다.
//////////////////////////////////////////////////////////////////////////
void UIPhaseRouletteShop::GetGoodsName(i3::string& Name, UINT32 pGoodsid)
{
	UINT32 pitemid = g_pGameContext->ConvertGoodsID( pGoodsid) ;
	INT32 goodsClass = GET_ITEM_FLAG_TYPE(pitemid);
	INT32 num = GET_ITEM_FLAG_NUMBER( pitemid);

	switch((ITEM_TYPE) goodsClass)
	{
	case ITEM_TYPE_PRIMARY:
	case ITEM_TYPE_SECONDARY:
	case ITEM_TYPE_MELEE:
	case ITEM_TYPE_THROWING1:
	case ITEM_TYPE_THROWING2:
		{
			WEAPON_CLASS_TYPE type = (WEAPON_CLASS_TYPE) GET_ITEM_FLAG_CLASS( pitemid);
			CWeaponInfo * pInfo = g_pWeaponInfoDataBase->getWeaponInfo(type, num);
			pInfo->GetWeaponName( Name);
			//i3::safe_string_copy(Name, pInfo->GetName(), MAX_STRING_COUNT);
		}
		break;
	case ITEM_TYPE_CHARA:
	case ITEM_TYPE_HEADGEAR:
	case ITEM_TYPE_FACEGEAR :			// FaceGear
	case ITEM_TYPE_UPPER :			// 상의
	case ITEM_TYPE_LOWER :			// 하의
	case ITEM_TYPE_GLOVE :			// 장갑
	case ITEM_TYPE_BELT :				// 벨트
	case ITEM_TYPE_HOLSTER :
		{
			const CEquipInfo * pEquipInfo = g_pCharaInfoDataBase->GetInfo(pitemid);
			if( pEquipInfo)
			{
				g_pCharaInfoDataBase->GetCharacterName(pitemid, Name); //sizeof(outItemName));
			}		
			else
			{
				i3::sprintf(Name, "%s\nGoods:%d\nItem:%d", INVALID_TEXT,  pGoodsid, pitemid);
			}
		}
		break;
	case ITEM_TYPE_MAINTENANCE:
	case ITEM_TYPE_WRAP_PERIOD:
	case ITEM_TYPE_WRAP_COUNT:
			{
				ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(pitemid);
				if( pInfo )
				{
					g_pShopItemInfoDataBase->GetItemName(pitemid, Name);
				}
				else
					i3::sprintf(Name, "%s\nGoods:%d\nItem:%d", INVALID_TEXT, pGoodsid, pitemid);
			}
			break;
	case ITEM_TYPE_POINT:	// 포인트일 경우. 2012.01.13 노준영.
		{
			UINT32 ui32Point = GET_POINTITEM( pitemid );
			i3::sprintf(Name, "%d POINT", ui32Point);
		}
		break;
	}
	

	
}

//////////////////////////////////////////////////////////////////////////
//	NAME		: SetItemInfo()
//	DESCRIPTION : 아이템을 정보를 업데이트 합니다.
//////////////////////////////////////////////////////////////////////////
void UIPhaseRouletteShop::SetItemInfo()
{
		RS_ALL_ITEM_INFO* pInfo;
		pInfo = g_pGameContext->GetRSInfo();
		if(g_pGameContext->GetInfoRecive())
		{
			INT32 i = 0;
			INT32 j = 0;
			for(j = 0; j < 3; j++)
			{
				m_pTabInfo[j].Grade = pInfo->_aRSTabInfo[j]._ui8TotalItemGrade;
				m_pTabInfo[j].Price = pInfo->_aRSTabInfo[j]._ui32Price;
				for(i = 0; i < MAX_RS_HIDDEN_ITEM_COUNT; i++)
				{
					m_pTabInfo[j].Info[i] = &pInfo->_aRSTabInfo[j]._aRSHidden._aRSItemInfo[i];
				}
				if(g_pGameContext->GetRsHiddnSelect())
				{

					INT32 tHiddenCount = 0;
					INT32 tindextap1 = 0, tindextap2 = 0, tindextap3 = 0;

					for(i = 0; i < 18; i++)
					{
						m_pRoulette_SlotInfo[j][i] = NULL;
					}

					while(MAX_RS_HIDDEN_ITEM_COUNT > tHiddenCount )
					{
						if(m_pTabInfo[j].Info[tHiddenCount]->_ui32GoodsID != 0)
						{
							switch(m_pTabInfo[j].Info[tHiddenCount]->_eItemGrade)
							{
							case 1:
								m_pRoulette_SlotInfo[j][tindextap1+12] = m_pTabInfo[j].Info[tHiddenCount];
								tindextap1++;
								tHiddenCount++;
								break;
							case 2:
								m_pRoulette_SlotInfo[j][tindextap2+6] = m_pTabInfo[j].Info[tHiddenCount];
								tindextap2++;
								tHiddenCount++;
								break;
							case 3:
								m_pRoulette_SlotInfo[j][tindextap1] = m_pTabInfo[j].Info[tHiddenCount];
								tindextap3++;
								tHiddenCount++;
								break;
							}

						}
						else
						{
							tHiddenCount++;
						}						
					}
				}
				else
				{
					for(i = 0; i < MAX_RS_TAB_SHOW_GRADE_ITEM_COUNT; i++)
					{
						m_pRoulette_SlotInfo[j][i] = &pInfo->_aRSTabInfo[j]._aRSItemInfoG3[i];
					}

					for(i = 0; i < MAX_RS_TAB_SHOW_GRADE_ITEM_COUNT; i++)
					{
						m_pRoulette_SlotInfo[j][6+i] = &pInfo->_aRSTabInfo[j]._aRSItemInfoG2[i];
					}

					for(i = 0; i < MAX_RS_TAB_SHOW_GRADE_ITEM_COUNT; i++)
					{
						m_pRoulette_SlotInfo[j][12+i] =&pInfo->_aRSTabInfo[j]._aRSItemInfoG1[i];
					}

				}

			}

			if( m_bReceiveItem == FALSE)
			{
				SetBaseInfoInit();
			}
			SetUiItem();
		}
		else
		{
			SetActivateItem(-2);
		}
}

//////////////////////////////////////////////////////////////////////////
//	NAME		: SetActivateItem()
//	DESCRIPTION : 버튼 활성봐 비활성화 컨트롤 합니다.
//////////////////////////////////////////////////////////////////////////
void	UIPhaseRouletteShop::SetActivateItem(INT32 pbstate)
{
	if( pbstate == -1)
	{
		for(INT32 i = 0; i < 18; i++)
		{
			LuaState* L = _CallLuaFunction(2, "SetEnableBtn");
			if(L != NULL)
			{
				i3Lua::PushInteger(L, i);
				i3Lua::PushBoolean(L, TRUE);
				_EndLuaFunction(L, 2);
			}
		}
	}
	//루아 아이템 Setting
	LuaState* L = _CallLuaFunction(2, "SetActivateBtn");
	if(L != NULL)
	{
		i3Lua::PushInteger(L, pbstate);
		_EndLuaFunction(L, 1);
	}
}

//////////////////////////////////////////////////////////////////////////
//	NAME		: SetBaseInfoInit()
//	DESCRIPTION : 뽑기상점 입장시 기본 정보를 Seting 합니다.
//////////////////////////////////////////////////////////////////////////
void	UIPhaseRouletteShop::SetBaseInfoInit()
{
	SetActivateItem(-1);
	m_bReceiveItem = TRUE;

	char szString[1024]= {0,};
	//루아 Jackpot Name Setting
	i3::string Name;
	GetGoodsName(Name,  g_pGameContext->GetRSBaseInfo()->_ui32JackPotGoodsID);


	

	sprintf_s(szString,1024,"%s %s %s %s",g_pGameContext->GetRSBaseInfo()->_strJackPotNick,g_pGameContext->GetRSBaseInfo()->_aRSAdMessage[0]._strMessage,g_pGameContext->GetRSBaseInfo()->_aRSAdMessage[1]._strMessage,g_pGameContext->GetRSBaseInfo()->_aRSAdMessage[2]._strMessage);
	

	//g_pGameContext->SetRouletteNotice(szString);
	
	SetFlowAnnounce( szString);
	LuaState* L = _CallLuaFunction(2, "SetJackpotName");
	if(L != NULL)
	{
		i3Lua::PushA2UTF8String(L, Name.c_str());
		_EndLuaFunction(L, 1);
	}

	m_pRouletteView->getRouletteObject()->InitButton(m_pTabInfo[m_pSelectTapIdx].Grade);
}




bool UIPhaseRouletteShop::IsPlayingRoulette()
{
	if (m_pRouletteView->getRouletteObject()->IsRouletteIdleState() == false ||
		(g_pGameContext->isCheckAuto() && g_pGameContext->getRecvRouletteAckPacket()))
	{
		return true;
	}
	else
		return false;
}

void UIPhaseRouletteShop::_UpdateAutoRoulette(REAL32 rDeltaSeconds)
{
	
	m_rAutoRouletteDely += rDeltaSeconds;

	//AUTO ROULETTE 처리 
	if (	g_pGameContext->isCheckAuto() &&																			//오토체크된 상태
		m_pRouletteView->getRouletteObject()->GetRouletteState() == ROULETTE_STATE_IDLE&&								//룰렛상태가 IDLE 상태
		!(GetMainFrame()->IsOpenedPopup(UPW_ROULETTE_WIN) ) && !(GetMainFrame()->IsOpenedPopup(UPW_ROULETTE_JACKPOT) )&& !(GetMainFrame()->IsOpenedPopup(UPW_ROULETTE_DEFAULT) ))	//당첨팝업이 닫힌 상태
	{
		if (g_pGameContext->GetRSTap1UseCount() != 5 && g_pGameContext->GetRSTap2UseCount() != 5 && g_pGameContext->GetRSTap3UseCount() != 5)
		{
			g_pGameContext->SetCheckPopup(true);
		}



		//UPDATE돌면서 계속 들어올수 있으므로 3초의 딜레이를 줍니다.
		if (m_rAutoRouletteDely >0.f)
		{
			//1번탭
			if (m_pSelectTapIdx ==0)
			{
				if (g_pGameContext->GetRSTap1UseCount() ==5  && g_pGameContext->getCheckPopup())
				{
					g_pGameContext->SetCheckPopup(false);
					m_pRouletteView->getRouletteObject()->WriteData(m_pSelectTapIdx);
					GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_STRING("STR_ROULETTE_SERVICE_PLAY_POPUP") , NULL, MAKE_CALLBACK(CbRouletteFreeStart), this);
				}
				else if (g_pGameContext->GetRSTap1UseCount() !=5)
				{
					AutoStart();
				}				
				m_rAutoRouletteDely = -3.f;
			}
			//2번탭
			else if (m_pSelectTapIdx ==1)
			{

				if (g_pGameContext->GetRSTap2UseCount() ==5  && g_pGameContext->getCheckPopup())
				{
					g_pGameContext->SetCheckPopup(false);;
					m_pRouletteView->getRouletteObject()->WriteData(m_pSelectTapIdx);
					GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_STRING("STR_ROULETTE_SERVICE_PLAY_POPUP") , NULL, MAKE_CALLBACK(CbRouletteFreeStart), this);
				
				}
				else if (g_pGameContext->GetRSTap2UseCount() !=5)
				{
					AutoStart();
				}
				m_rAutoRouletteDely = -3.f;
			}
			//3번탭
			else if (m_pSelectTapIdx ==2)
			{

				if (g_pGameContext->GetRSTap3UseCount() ==5  && g_pGameContext->getCheckPopup())
				{
					g_pGameContext->SetCheckPopup(false);;
					m_pRouletteView->getRouletteObject()->WriteData(m_pSelectTapIdx);
					GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_STRING("STR_ROULETTE_SERVICE_PLAY_POPUP") , NULL, MAKE_CALLBACK(CbRouletteFreeStart), this);
				}
					else if (g_pGameContext->GetRSTap3UseCount() !=5)
				{
					AutoStart();
				}
				m_rAutoRouletteDely = -3.f;
			}

		}
	}

}
void UIPhaseRouletteShop::_UpdateBonusCount(REAL32 rDeltaSeconds)
{

	//결과 패킷을 받을경우 보너스 카운트를 갱신하도록 합니다.
	if(g_pGameContext->getRecvRouletteStartPacket())
	{
		RefreshCash();
		sprintf_s(m_szString, 1024, "%s %s %s %s",g_pGameContext->GetRSBaseInfo()->_strJackPotNick,g_pGameContext->GetRSBaseInfo()->_aRSAdMessage[0]._strMessage,g_pGameContext->GetRSBaseInfo()->_aRSAdMessage[1]._strMessage,g_pGameContext->GetRSBaseInfo()->_aRSAdMessage[2]._strMessage);
		SetFlowAnnounce(m_szString);
		LuaState* L = _CallLuaFunction(1, "SetChangeCount");
		if (L == NULL)
		{
			I3ASSERT_0;
		}
		//1번탭
		if (m_pSelectTapIdx == 0)
		{
			i3Lua::PushInteger(L, g_pGameContext->GetRSTap1UseCount());	
		}
		//2번탭
		else if (m_pSelectTapIdx==1)
		{
			i3Lua::PushInteger(L, g_pGameContext->GetRSTap2UseCount());	
		}
		//3번탭
		else if (m_pSelectTapIdx==2)
		{
			i3Lua::PushInteger(L, g_pGameContext->GetRSTap3UseCount());	
		}
		_EndLuaFunction(L, 1);
	}

}

void UIPhaseRouletteShop::_UpdatePopup(REAL32 rDeltaSeconds)
{

	m_rElapsedTime += rDeltaSeconds;

	// 당첨 팝업 출력 및 팝업 닫는 설정을 합니다.
	if(m_pRouletteView->getRouletteObject()->GetRouletteState() == ROULETTE_STATE_FINISH)
	{
		//대박은 10초
		if (g_pGameContext->GetRSItemGrade() ==  RS_ITEM_GRADE_JACKPOT)
		{
			GetMainFrame()->OpenPopup(UPW_ROULETTE_JACKPOT,NULL);
			m_rElapsedTime =-10.f;
		}
		//일반 당첨은 5초
		else if (g_pGameContext->GetRSItemGrade() !=  RS_ITEM_GRADE_LOSE)
		{
			GetMainFrame()->OpenPopup(UPW_ROULETTE_WIN,NULL);
			m_rElapsedTime =-5.f;
			
		}
		else
		{
			GetMainFrame()->OpenPopup(UPW_ROULETTE_DEFAULT,NULL);
			m_rElapsedTime =-2.f;	
		}
		

	}

	//대박팝업 10초오픈후 닫습니다.
	if (GetMainFrame()->IsOpenedPopup(UPW_ROULETTE_JACKPOT))
	{
		if (m_rElapsedTime >0.f)
		{
			GetMainFrame()->ClosePopup(UPW_ROULETTE_JACKPOT);
		}


	}
	//일반팝업 10초오픈후 닫습니다.
	if (GetMainFrame()->IsOpenedPopup(UPW_ROULETTE_WIN))
	{
		if (m_rElapsedTime >0.f)
		{
			GetMainFrame()->ClosePopup(UPW_ROULETTE_WIN);
		}

	}
	

	//일반팝업 10초오픈후 닫습니다.
	if (GetMainFrame()->IsOpenedPopup(UPW_ROULETTE_DEFAULT))
	{
		if (m_rElapsedTime >0.f)
		{
			GetMainFrame()->ClosePopup(UPW_ROULETTE_DEFAULT);
		}

	}

}


void UIPhaseRouletteShop::PlaySound(GSND_ROULETTE_TYPE eSound)
{
	VEC3D vPos;
	i3Vector::Set( &vPos,1.f,1.f,0.f);
	g_pSndMng->StartRouletteSnd(eSound,&vPos,false);
}

BOOL	UIPhaseRouletteShop::CheckStar(INT32 pgrade)	
{
	if( m_pTabInfo[m_pSelectTapIdx].Grade >= m_iGradeCount + pgrade)
	{
		m_iGradeCount += pgrade;
		return TRUE;
	}
	else
		return FALSE;
}

void	UIPhaseRouletteShop::GuideEnable(BOOL enable)
{
	m_bGuideEnable = enable;
	m_pGuideFrame->EnableCtrl(enable);
}
void	UIPhaseRouletteShop::SetGuideStep(INT32 step)
{
	INT32 Shapeidx = m_pGuideStep->GetShapeIdx();
	if( step == -1)
	{
		INT32 shape = ((Shapeidx-1)>-1)?(Shapeidx-1):3;
		if(shape > -1)
			m_pGuideStep->SetShapeIdx(shape);
	}
	else if( step == -2)
	{
		m_pGuideStep->SetShapeIdx(Shapeidx+1);
	}
	else
	{
		m_pGuideStep->SetShapeIdx(step);
	}
}