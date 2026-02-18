#include "pch.h"
#include "UIPhaseGachaShop.h"
#include "UIMainFrame.h"
#include "Shop.h"
#include "UI/UIUtil.h"
#include "UIShopFunction.h"
#include "UIFloatChat.h"
#include "UITopMenu_V15.h"
#include "BuyUtil.h"
#include "ItemImageMgr.h"
#include "UISlide.h"

#include "UIGachaObject.h"
#include "UIGachaView.h"

#include "../GameGUI.h"

#include "i3Base/string/ext/insert_string.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/utf8_to_utf16.h"

#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/sprintf.h"
#include "i3Base/string/compare/generic_is_iless.h"

#include "GachaContext.h"
#include "UserInfoContext.h"
#include "ShopContext.h"
#include "i3Base/string/ext/itoa.h"

#include "UISignal.h"

#include <math.h>

I3_CLASS_INSTANCE( UIPhaseGachaShop);

namespace
{
	UI::tmPopupSlideMtd g_slide_mtd;

	bool g_check_slide;
	REAL32 elapse_time;
}


namespace sort_f	
{
	namespace inner
	{
		struct fn_nameUp
		{
			bool operator()(const GACHA_ITEM_INFO * lhs, const GACHA_ITEM_INFO * rhs) const
			{
				i3::rc_wstring lhs_name;
				UINT32 pitemid = 0;
				pitemid =  CShopContext::i()->ConvertGoodsID(lhs->m_ui32GoodsID);
				usf::GetShopItemName(lhs_name, pitemid);

				i3::rc_wstring rhs_name; 
				pitemid =  CShopContext::i()->ConvertGoodsID(rhs->m_ui32GoodsID);
				usf::GetShopItemName(rhs_name, pitemid);
				//return i3::generic_iless()(lhs_name, rhs_name);  기능부족으로 CompareString를 씁니다.
				return i3::generic_is_iless(lhs_name, rhs_name);
			}
		};
		struct fn_nameDown
		{
			bool operator()(const GACHA_ITEM_INFO * lhs, const GACHA_ITEM_INFO * rhs) const
			{
				i3::rc_wstring lhs_name;
				UINT32 pitemid = 0;
				pitemid =  CShopContext::i()->ConvertGoodsID(lhs->m_ui32GoodsID);
				usf::GetShopItemName(lhs_name, pitemid);

				i3::rc_wstring rhs_name; 
				pitemid =  CShopContext::i()->ConvertGoodsID(rhs->m_ui32GoodsID);
				usf::GetShopItemName(rhs_name, pitemid);
				//return i3::generic_iless()(lhs_name, rhs_name);  기능부족으로 CompareString를 씁니다.

				return i3::generic_is_iless(rhs_name, lhs_name);
			}
		};
		struct fn_saleUp
		{
			bool operator()(const GACHA_ITEM_INFO * lhs, const GACHA_ITEM_INFO * rhs) const
			{
				return (lhs->m_i32RemainAmount < rhs->m_i32RemainAmount);
			}
		};
		struct fn_saleDown
		{
			bool operator()(const GACHA_ITEM_INFO * lhs, const GACHA_ITEM_INFO * rhs) const
			{
				return (lhs->m_i32RemainAmount > rhs->m_i32RemainAmount);
			}
		};
		void name(i3::vector<const GACHA_ITEM_INFO* >* inven_list, bool SortType)
		{
			if(SortType == true)
			{
				std::sort( (*inven_list).begin(), (*inven_list).end(), sort_f::inner::fn_nameUp() );
			}
			else
			{
				std::sort( (*inven_list).begin(), (*inven_list).end(), sort_f::inner::fn_nameDown() );
			}
		}
		void sale(i3::vector<const GACHA_ITEM_INFO* >* inven_list, bool SortType)
		{
			if(SortType == true)
				std::sort( (*inven_list).begin(), (*inven_list).end(), sort_f::inner::fn_saleUp() );
			else
				std::sort( (*inven_list).begin(), (*inven_list).end(), sort_f::inner::fn_saleDown() );
		}
		void newitem(i3::vector<const GACHA_ITEM_INFO* >* inven_list)
		{
			std::reverse( (*inven_list).begin(), (*inven_list).end());
		}
	}
}

extern "C" {
	int UIGacha_ItemListView( LuaState * L)
	{
		UIPhaseGachaShop * pThis = static_cast<UIPhaseGachaShop*>(GameUI::GetSubPhase(USP_GACHA));
		
		if( pThis != nullptr)
			pThis->_SetOtherItem();
		return 0;
	}
	int UIGacha_SelectComboItem( LuaState * L)
	{
		INT32 i32Slot = i3Lua::GetIntegerArg(L , 1);
		INT32 i32ComboIdx = i3Lua::GetIntegerArg(L , 2);

		if(i32Slot > -1 && i32ComboIdx > -1)
		{
			UIPhaseGachaShop * pThis = static_cast<UIPhaseGachaShop*>(GameUI::GetSubPhase(USP_GACHA));
			if( pThis != nullptr)
				pThis->_SelectComboItem(i32Slot, i32ComboIdx);
		}
		return 0;
	}
	int UIGacha_Click_Tab( LuaState * L)
	{
		INT32 i32Tab = i3Lua::GetIntegerArg(L , 1);
		UIPhaseGachaShop * pThis = static_cast<UIPhaseGachaShop*>(GameUI::GetSubPhase(USP_GACHA));
		if( pThis != nullptr)
			pThis->_SelectTab(i32Tab);

		return 0;
	}

	int UIGacha_Clieck_Sort( LuaState * L)
	{
		INT32 i32Sort2 = i3Lua::GetIntegerArg(L , 1);
		UIPhaseGachaShop * pThis = static_cast<UIPhaseGachaShop*>(GameUI::GetSubPhase(USP_GACHA));
		if( pThis != nullptr)
			pThis->_ListSort(i32Sort2);

		return 0;
	}
	int UIGacha_Click_Weapon_Detail( LuaState * L)
	{
		UIPhaseGachaShop * pThis = static_cast<UIPhaseGachaShop*>(GameUI::GetSubPhase(USP_GACHA));
		if( pThis != nullptr)
			pThis->Click_Weapon_Detail();
		return 0;
	}
	int UIGacha_Click_LuckyItem_Detail( LuaState * L)
	{
		UIPhaseGachaShop * pThis = static_cast<UIPhaseGachaShop*>(GameUI::GetSubPhase(USP_GACHA));
		if( pThis != nullptr)
			pThis->Click_LuckyItem_Detail();
		return 0;
	}
	int UIGacha_Clieck_Search( LuaState * L)
	{
		char strKeyword[MAX_STRING_COUNT] = {0,};	//	--> UTF8로 간주한다.
		i3Lua::GetStringArg(L, 1, strKeyword, MAX_STRING_COUNT-1);
		
		i3::stack_wstring wstrKeyword;	i3::utf8_to_utf16(strKeyword, wstrKeyword);

		UIPhaseGachaShop * pThis = static_cast<UIPhaseGachaShop*>(GameUI::GetSubPhase(USP_GACHA));
		if( pThis != nullptr)
			pThis->_SelectSearch(wstrKeyword);
		return 0;
	}
	int UIGacha_Clieck_Help( LuaState * L)
	{
		UIPhaseGachaShop * pThis = static_cast<UIPhaseGachaShop*>(GameUI::GetSubPhase(USP_GACHA));
		if( pThis != nullptr)
			pThis->Click_Help();
		return 0;
	}
	int UIGacha_Clieck_Gacha( LuaState * L)
	{
		INT32 i32Amount = i3Lua::GetIntegerArg(L , 1);
		UIPhaseGachaShop * pThis = static_cast<UIPhaseGachaShop*>(GameUI::GetSubPhase(USP_GACHA));
		if( pThis != nullptr)
			pThis->GachaStart((UIPhaseGachaShop::EGACHA_CLICK)i32Amount);
		return 0;
	}
	int UIGacha_Clieck_Cash( LuaState * L)
	{
		UIPhaseGachaShop * pThis = static_cast<UIPhaseGachaShop*>(GameUI::GetSubPhase(USP_GACHA));
		if( pThis != nullptr)
			pThis->Clieck_Cash();
		return 0;
	}	
}

LuaFuncReg UIGacha_LeftUI_Glue[] =
{
	{"ItemListView",				UIGacha_ItemListView},
	{"SelectComboItem",				UIGacha_SelectComboItem},
	{"Click_Tab",					UIGacha_Click_Tab},
	{"Clieck_Sort",					UIGacha_Clieck_Sort},
	{"Click_Weapon_Detail",			UIGacha_Click_Weapon_Detail },
	{"Click_LuckyItem_Detail",		UIGacha_Click_LuckyItem_Detail},
	{"Clieck_Search",				UIGacha_Clieck_Search },
	{nullptr,				nullptr}
};

LuaFuncReg UIGacha_RightUI_Glue[] =
{
	{"Clieck_Help",				UIGacha_Clieck_Help},
	{"Clieck_Gacha",			UIGacha_Clieck_Gacha},
	{"Clieck_Cash",				UIGacha_Clieck_Cash},
	{nullptr,				nullptr}
};

UIPhaseGachaShop::UIPhaseGachaShop(void) :	m_bItemListSend(false), m_CurrentTab(GACHA_GROUP_POOR), 
											m_CurrentIdx(0),		m_iGachaAmount(EGACHA_NON_CLICK),
											m_pItemListBox(nullptr),	m_goods_img_ctrl(nullptr), 
											m_pAniImage(nullptr),		m_pAnnounce(nullptr),
											m_rFlowTime(0.f),		m_bFCMFlag(false),
											m_bFlowFlag(false),		m_AniTime(0),
											m_pGachaView(nullptr),		m_gachaRstProcStatus(EGACHA_RST_PROCST_NONE),
											m_bIsLuckyItemDetail(false)
											
{
	m_gachaRstProc.m_owner = this;

	for(INT32 i = 0; i < GACHA_GROUP_COUNT; i++)
	{
		m_pGachaGroup[i] = nullptr;
	}

	for(INT32 i = 0; i < RANDOM_ITEM_COUNT_BY_ITEM; i++)
	{
		m_OtherItemimg_ctrl[i] = nullptr;
	}

	RegisterGameEventFunc( EVENT_BUY_GOODS,				&UIPhaseGachaShop::ReceiveGameEvent_Buy_Goods);
	RegisterGameEventFunc( EVENT_ENTER_GACHA_INFO,		&UIPhaseGachaShop::ReceiveGameEvent_Enter_Gacha_Info);
	RegisterGameEventFunc( EVENT_END_GACHA_STATE,		&UIPhaseGachaShop::ReceiveGameEvent_End_Gacha_State);
	RegisterGameEventFunc( EVENT_START_GACHA,			&UIPhaseGachaShop::ReceiveGameEvent_Start_Gacha);
	RegisterGameEventFunc( EVENT_GACHA_ANNOUNCE,		&UIPhaseGachaShop::ReceiveGameEvent_Gacha_Announce);
	RegisterGameEventFunc( EVENT_ITEM_AUTH,				&UIPhaseGachaShop::ReceiveGameEvent_Item_Auth);
	RegisterGameEventFunc( EVENT_REFRESH_CASH,			&UIPhaseGachaShop::ReceiveGameEvent_Refresh_Cash);

	m_pLeftMenuScene = nullptr;
	m_pCtrlListView = nullptr;
	m_pCtrlButtonList1 = nullptr;
	m_pCtrlButtonList2 = nullptr;

}

UIPhaseGachaShop::~UIPhaseGachaShop(void)
{
}

/*virtual*/ void UIPhaseGachaShop::OnCreate( i3GameNode * pParent )
{
	UIPhaseSubBase::OnCreate( pParent);

	//AddScene( "Scene/Main/PointBlankRe_Lottery_rightmenu.i3UIs", UIGacha_RightUI_Glue);
	AddScene( "Scene/Main/PointBlankRe_Lottery_rightmenu.i3UIs", UIGacha_RightUI_Glue, false, true); //true, false);
	AddScene( "Scene/Main/PointBlankRe_Lottery_leftmenu.i3UIs", UIGacha_LeftUI_Glue, false, true); //true, false);

	g_pEffectManager->EnterStage();	// create Decal hit box
}

/*virtual*/ void UIPhaseGachaShop::OnUpdate( REAL32 rDeltaSeconds )
{
	UIPhaseSubBase::OnUpdate(rDeltaSeconds);

	UpdateGachaRstProcessing(rDeltaSeconds);

	m_AniSlot.OnUpdate(rDeltaSeconds);
	_UpdateFlowAnnounce(rDeltaSeconds);
	
	m_AniTime+=rDeltaSeconds;

	//i3UIListView_Item * pItem = m_pItemListBox->getItemList()[0];
	_UpdateItemAni();

	if(m_pGachaView)
	{
		m_pGachaView->OnUpdate(rDeltaSeconds);
	}

	// Time Check
	PurchaseTimeOut();
	UpdateStateTimeCheck();

	FocusOutItemListView();
}

/*virtual*/ bool UIPhaseGachaShop::OnExitStart( void )
{
	if( UIPhaseSubBase::OnExitStart() == false)
		return false;
	
	Avatar * pavtar = g_pCharaManager->GetLobbyAvatar();
	if( pavtar )
	{
		pavtar->SetVisible(true);
	}

	m_purchaseTimecheck.Reset();
	m_UpdateStateTime.Reset();
	
	return true;
}

/*virtual*/ bool UIPhaseGachaShop::OnEntranceStart( void* pArg1 /*= 0*/, void* pArg2 /*= 0*/)
{
	if( UIPhaseSubBase::OnEntranceStart( pArg1, pArg2) == false )
		return false;

	//룰렛씬에서는 아바타 안보이게설정
	Avatar * pavtar = g_pCharaManager->GetLobbyAvatar();
	if( pavtar )
	{
		pavtar->SetVisible(false);
	}


	HDIObject1(false);
	return true;
}

/*virtual*/ void UIPhaseGachaShop::OnEntranceEnd( void )
{
	UIPhaseSubBase::OnEntranceEnd();

	m_pGachaView->CreateViewGacha();
	
	m_pGachaView->OnIdleStart();

	LuaState * L = _CallLuaFunction( 0, "setBGEnable");
	if( L != nullptr)
	{
		i3Lua::PushBoolean(L, false);
		i3Lua::PushBoolean(L, true);
		_EndLuaFunction( L, 2);
	}

	if( m_bItemListSend == false)
	{
		GameEventSender::i()->SetEvent( EVENT_ENTER_GACHA_INFO );
		m_bItemListSend = true;
	}


	for(INT32 i = 0; i< GACHA_GROUP_COUNT; i++)
		m_pGachaGroup[i] = GachaContext::i()->GetGachaData(i);
	
	ResetGachaRstProcessing();

//	GachaContext::i()->SetGachaBuyGoodId(2500001);

	m_CurrentIdx = 0;

	_SelectTab(0);
	UpdateGachaRight();
	SetButtonText();

	if( m_pAnnounce->getText() != nullptr)
	{
		if( i3::generic_is_equal( m_pAnnounce->getTextRange(), m_strPermanatNotice) == false)
			m_pAnnounce->SetText( m_strPermanatNotice );
	}

	m_pAnnounce->setScrollState( i3UIEditBox::UBSS_INIT);
	
	// State Update 시간체크
	m_UpdateStateTime.setBaseTime(m_UpdateStateTime.GACHA_STATE_UPDATE);
	m_UpdateStateTime.Start();
}

/*virtual*/ void UIPhaseGachaShop::OnLoadAllScenes()
{
	//기본정보를 세팅합니다.
	LuaState * L = _CallLuaFunction( 1,"__init");
	if( L != nullptr)
	{
		_EndLuaFunction(L, 0);
	}

	UIPhaseSubBase::OnLoadAllScenes();
}

/*virtual*/ void UIPhaseGachaShop::OnUnloadAllScenes()
{
		UIPhaseSubBase::OnUnloadAllScenes();

		I3_SAFE_RELEASE(m_pGachaView);
		for(INT32 i = 0; i < GACHA_OTHERITEM_MAX; i++)
		{
			if( m_OtherItemimg_ctrl[i] )m_OtherItemimg_ctrl[i]->RestoreSizePos();
			I3_SAFE_RELEASE(m_OtherItemimg_ctrl[i]);
		}

		if( m_goods_img_ctrl )m_goods_img_ctrl->RestoreSizePos();
		I3_SAFE_RELEASE(m_goods_img_ctrl);
		m_pAnnounce = nullptr;
		m_AniSlot.pAniImage = nullptr;

		m_pLeftMenuScene = nullptr;
		m_pCtrlListView = nullptr;
		m_pCtrlButtonList1 = nullptr;
		m_pCtrlButtonList2 = nullptr;
}

/*virtual*/ void UIPhaseGachaShop::Leave( void )
{	
	DisableUIInput(false);
	ResetGachaRstProcessing();
	GameEventSender::i()->SetEventOnce(EVENT_LEAVE_GACHA);
}
bool UIPhaseGachaShop :: OnClick( i3UIControl * pControl)
{
	return UIPhaseSubBase::OnClick( pControl);
}

/*virtual*/ bool UIPhaseGachaShop::OnKeyInput( i3InputDeviceManager * pInputMng )
{
	i3InputKeyboard * pKey = pInputMng->GetKeyboard();

	if( pKey->GetStrokeState(I3I_KEY_ESC))
	{
		if(m_pLeftMenuScene)
		{
			if(m_pCtrlListView && m_pCtrlButtonList1 && m_pCtrlButtonList2 && m_pCtrlListView->isEnable() == true)
			{
				m_pCtrlListView->EnableCtrl(false);
				m_pCtrlButtonList1->EnableCtrl(true);
				m_pCtrlButtonList2->EnableCtrl(false);
				
				return true;	
			}
		}		
	}
	
	
	//if( pKey->GetStrokeState( I3I_KEY_T))
	//{


	//	m_Slide.move_target_and_size();
	//	m_pGachaView->UpdateParentSize();

	//	//pFrame->UpdateRenderObjects();
	//	m_pGachaView->setAnimationState(GACHA_PLAY_SELECT_A);
	//	m_pGachaView->setBuyAnimationStart();

	//	m_pGachaView->m_bObjectAniStart = true;
	//	i3Animation * pAni = m_pGachaView->getRouletteObject()->getCurrentAni();
	//	m_pGachaView->m_rObjectDurationTime = pAni->GetDuration();
	//	m_pGachaView->m_rObjectElapseTime = 0.0f;

	//	LuaState * L = _CallLuaFunction( 1, "setBGEnable");
	//	if( L != nullptr)
	//	{
	//		i3Lua::PushBoolean(L, true);
	//		i3Lua::PushBoolean(L, false);
	//		_EndLuaFunction( L, 2);
	//	}
	//	GetMainFrame()->GetTopMenu_V15()->SetInputDisable(true);
	//	SetInputDisable(true);
	//}
	//else if(pKey->GetStrokeState(I3I_KEY_Q))
	//{
	//	static bool bgOnOff = false;
	//	bgOnOff = !bgOnOff;
	//	LuaState * L = _CallLuaFunction( 1, "setBGEnable");
	//	if( L != nullptr)
	//	{
	//		i3Lua::PushBoolean(L, bgOnOff);
	//		i3Lua::PushBoolean(L, !bgOnOff);
	//		_EndLuaFunction( L, 2);
	//	}
	//}

	return UIPhaseSubBase::OnKeyInput( pInputMng);

}

/*virtual*/ void UIPhaseGachaShop::_InitializeAtLoad( i3UIScene * pScene )
{
	UIPhaseSubBase::_InitializeAtLoad( pScene);

	
	if( i3::generic_is_iequal( pScene->GetName(), "PointBlankRe_Lottery_rightmenu") )
	{
		I3_SAFE_RELEASE(m_pGachaView);

		m_pGachaView = UIGachaView::new_object();

		i3UIFrameWnd * pCharaWnd = (i3UIFrameWnd*) pScene->FindChildByName( "i3UIFrameWnd13");
		I3ASSERT( pCharaWnd != nullptr);

		m_ClickButton[0] = (i3UIButtonComposed3*)pScene->FindChildByName("i3UIButtonComposed30");
		m_ClickButton[1] = (i3UIButtonComposed3*)pScene->FindChildByName("i3UIButtonComposed31");
		m_ClickButton[2] = (i3UIButtonComposed3*)pScene->FindChildByName("i3UIButtonComposed32");

		//SetButtonText();
			
		m_pGachaView->Create3DViewBox( pCharaWnd, pScene);

 		m_pAnnounce = (i3UIEditBox *) pScene->FindChildByName("i3UIEditBox1");

		{
			i3UIFrameWnd* couponFrm = (i3UIFrameWnd*)pScene->FindChildByName("i3UIFrameWnd1");

			if(couponFrm)
			{
				i3UIStaticText* couponFrmTitle = (i3UIStaticText*)couponFrm->FindChildByName("i3UIStaticText1");

				if(couponFrmTitle)
					couponFrmTitle->SetText(GAME_RCSTRING("STR_LUCKYSHOP_COUPON_TITLE"));

				//chunjong.song 2014-2-14 : 캐시충전에 보유이용권은 아직 기획처리중(안보이게처리)			
				couponFrm->EnableCtrl(false, true);
			}

		}

		I3ASSERT(m_pAnnounce);

	}
	else if( i3::generic_is_iequal( pScene->GetName(), "PointBlankRe_Lottery_leftmenu") )
	{
		m_pLeftMenuScene = pScene;

		m_pCtrlListView = (i3UIControl*)m_pLeftMenuScene->FindChildByName( "i3UIFrameWndListView");
		m_pCtrlButtonList1 = (i3UIControl*)m_pLeftMenuScene->FindChildByName( "i3UIButtonList1");
		m_pCtrlButtonList2 = (i3UIControl*)m_pLeftMenuScene->FindChildByName( "i3UIButtonList2");

		m_pItemListBox = (i3UIListView_Box*) pScene->FindChildByName( "i3UIListView_Box_RL");
		i3UIButtonImageSet* pAniImage = (i3UIButtonImageSet*) pScene->FindChildByName( "i3UIButtonImageSet_ani");
		m_AniSlot.SetButton(pAniImage);
		
		i3UIButtonImageSet* btnset = (i3UIButtonImageSet*) pScene->FindChildByName( "i3UIButtonImageSet2");
		m_goods_img_ctrl = GetItemImageMgr()->CreateImgSetCtrl( btnset);

		char szSale[MAX_STRING_COUNT] = {0};
		for(INT32 i = 0; i < GACHA_OTHERITEM_MAX; i++)
		{
			i3::snprintf(szSale , MAX_STRING_COUNT , "i3UIButtonImageSetother%d", i);
			btnset = (i3UIButtonImageSet*) pScene->FindChildByName( szSale);
			m_OtherItemimg_ctrl[i] = GetItemImageMgr()->CreateImgSetCtrl( btnset);
		}

		i3::snprintf(szSale , MAX_STRING_COUNT , "i3UIButtonImageSetother5");
		btnset = (i3UIButtonImageSet*) pScene->FindChildByName( szSale);
		btnset->SetViewOnly(true);

		m_pItemListText = (i3UIEditBox*) pScene->FindChildByName("i3UIEditBox0");
		m_pItemListText->SetReadOnly(true);

		
		i3UIFrameWnd * pCharaWnd = (i3UIFrameWnd*) pScene->FindChildByName( "i3UIFrameWnd2");
		m_pGachaView->setFrame2(pCharaWnd);

		m_SearchEditBox = (i3UIEditBox*) pScene->FindChildByName( "i3UIEditBox");
		I3ASSERT( m_SearchEditBox);
	}

}

/*virtual*/ void UIPhaseGachaShop::ProcessGameEvent( GAME_EVENT evt,INT32 arg, const i3::user_data& UserData)
{
	UIPhaseSubBase::ProcessGameEvent( evt, arg, UserData);

	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}
}

bool UIPhaseGachaShop::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code)
{
	bool bRv = UIPhaseSubBase::OnEvent( event, pObj, param2, code);
	if( event == I3_EVT_IME)
	{
		switch( param2)
		{
		case EDIT_STATE_RETURN:
			{
 				_SelectSearch(m_SearchEditBox->getText());
			}
			break;
		}
	}

	return bRv;
}

void UIPhaseGachaShop::ResetGachaRstProcessing()
{
	m_gachaRstProcStatus = EGACHA_RST_PROCST_NONE;
}

void UIPhaseGachaShop::ByeOpenPopup()			// 해석이 안되는건 아닌데..거슬린다....
{
	if( GetMainFrame()->GetCurrentSubPhaseState() != PS_IDLE
		&& GetMainFrame()->GetCurrentSubPhaseState() != PS_PLAYING)
		return;

	const GACHA_ITEM_INFO* pInfo = GetSelectGachaInfo();

	if( pInfo == nullptr)
	{
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_ROULETTE_ERROR_ITEMLIST_POPUP"));
		return;
	}

	UINT32 pitemid =  CShopContext::i()->ConvertGoodsID(pInfo->m_ui32GoodsID);

	if(pitemid == 0) // INVALID_ITEM_ID
	{
#if defined(_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
		{
			i3::rc_wstring wstr_errMsg;
			i3::sprintf(wstr_errMsg, L"Err! - No match item(GoodsID=%d) in itemlist of shop..", pInfo->m_ui32GoodsID);
			GameUI::MsgBox(MSG_TYPE_GAME_OK, wstr_errMsg);
		}
#endif
		return;
	}

	const SHOP_GOODS_PACK* pack = CShopContext::i()->GetGoodsPack( pitemid );
	if(!pack)
	{
#if defined(_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
		{
			i3::rc_wstring wstr_errMsg;
			i3::sprintf(wstr_errMsg, L"Err! - No match SHOP_GOODS_PACK info in shop (itemid=%d)", pitemid);
			GameUI::MsgBox(MSG_TYPE_GAME_OK, wstr_errMsg);
		}
#endif
		return;
	}

	buy::PushImmediate(pack);

	// 가격은 goodsid로 검색할 수 있다.
	MINI_MATCHING matching_;
	memset(&matching_, 0, sizeof(matching_));
	CShop::i()->GetGachaMatcing((UINT8)m_CurrentTab+1, pInfo->m_i32ItemKey, &matching_);

	SHOP_GOODS_EXPANSION* price = CShop::i()->GetGoodsData(matching_.m_aMatching[m_iGachaAmount]._ui32GoodsID);

	UINT32 coupon = 0; UINT32 cash = 0; UINT32 point = 0;

	cash = price->GetCash();
	point = price->GetPoint();
	coupon = price->GetCoin();
	
	// 가격 정보 입력
	GachaContext::i()->SetGachaBuyCashPrice(cash);
	GachaContext::i()->SetGachaBuyPointPrice(point);
	GachaContext::i()->SetGachaBuyCoupon(coupon);
	
	if(cash > 0)
		GachaContext::i()->SetGachaBuyType(GOODS_BUY_CASH);
	else
		GachaContext::i()->SetGachaBuyType(GOODS_BUY_POINT);

	GetMainFrame()->OpenPopup(UPW_BUYITEM, (void*)buy::PurchaseRoute_Immediate, &UISignal::FinishBuyPopup); // "finish_buy_popup");	//즉시구매

}

void UIPhaseGachaShop::GachaStart(EGACHA_CLICK Amount)
{
	if( GetMainFrame()->GetCurrentSubPhaseState() == PS_PLAYING)
	{
		if(IsInGachaRstProcessing())
		{
			GameUI::MsgBox(MSG_TYPE_GAME_OK, L"reward to Gacha Result is in progress..");
			return;
		}

		g_pSndMng->StartLotterySnd(GSND_LOTTERY_BUYYONBUY);
		m_iGachaAmount = Amount;
		ByeOpenPopup();
	}
}

void UIPhaseGachaShop::Click_Weapon_Detail()
{
	if( GetMainFrame()->GetCurrentSubPhaseState() == PS_IDLE
		|| GetMainFrame()->GetCurrentSubPhaseState() == PS_PLAYING)
	{
		if(GetSelectGachaInfo())
		{
			T_ItemID item_id = CShopContext::i()->ConvertGoodsID(GetSelectGachaInfo()->m_ui32GoodsID);
			GetMainFrame()->OpenPopup(UPW_GACHA_WEAPON_DETAIL, (void*)&item_id);
		}
	}
}

void UIPhaseGachaShop::Click_LuckyItem_Detail()
{
	if( GetMainFrame()->GetCurrentSubPhaseState() == PS_IDLE
		|| GetMainFrame()->GetCurrentSubPhaseState() == PS_PLAYING)
	{
		T_ItemID item_id = (T_ItemID)CShopContext::i()->ConvertGoodsID(m_pGachaGroup[m_CurrentTab]->m_ui32LuckyGoodsID);
		GetMainFrame()->OpenPopup(UPW_GACHA_WEAPON_DETAIL, (void*)&item_id);
	}
}

void UIPhaseGachaShop::Clieck_Cash()
{
	//	캐시 충전 웹페이지 호출
#if defined(LOCALE_TAIWAN)
	g_pFramework->OpenWebBrowser( UserInfoContext::i()->GetBillingURL(), nullptr, false);
#else
	::ExcuteCashWeb();
#endif
}

void UIPhaseGachaShop::UpdateGachaItem( void)
{

	LuaState * L = _CallLuaFunction( 1, "ClearList");

	// Point와 Cash를 나누는 리스트
	UINT32 nTotalCount = m_ItemList_Point[m_CurrentTab].size() + m_ItemList_Cash[m_CurrentTab].size();

	if( L != nullptr)
	{
		i3Lua::PushInteger( L, nTotalCount);
		_EndLuaFunction( L, 1);
	}
	
	_SetItemInfo();
	//_SetLuckyItemDetail();
	UpdateGachaRight();
}

void UIPhaseGachaShop::UpdateGachaRight( void)
{
	UINT32 iCash =0;

	//chunjong.song 2014-2-14 : 현재 송치윤님에게 문의결과 미구현기획사항 입니다.
	//UINT32 iCoupon= 0, nGoodid = 0;
	//nGoodid = GachaContext::i()->GetGachaBuyGoodId();

	//CInvenList* pInven = CInvenList::i();
	// UINT32 itemCnt = pInven->GetItemCount( INVEN::DB_CASHITEM);
	//for( size_t i = 0; i < itemCnt; ++i)
	//{
	//	INVEN_BUFFER* item = pInven->GetItem( INVEN::DB_CASHITEM, i);
	//	if( item->_ui32ItemID == nGoodid)
	//	{
	//		iCoupon = item->_ui32ItemArg;
	//	}
	//}

	iCash = UserInfoContext::i()->GetMyCash();
	i3::stack_wstring my_cash; i3::itoa(iCash, my_cash); 
	i3::stack_wstring my_coupon;

	//my_coupon = StringUtil::ToString(iCoupon);

	LuaState * L = _CallLuaFunction( 0, "SetUiText");
	if( L != nullptr)
	{
		if(my_cash.length() > 0)
			i3Lua::PushStringUTF16To8( L, my_cash);
		else
			i3Lua::PushString(L, L"0");

		if(my_coupon.length() > 0)
			i3Lua::PushStringUTF16To8( L, my_coupon);
		else
			i3Lua::PushString(L, L"0");

		_EndLuaFunction( L, 2);
	}

}

void UIPhaseGachaShop::_SetLuckyItemDetail()
{
	if ( usf::GetShopItemCategory(CShopContext::i()->ConvertGoodsID(m_pGachaGroup[m_CurrentTab]->m_ui32LuckyGoodsID) == SHOP_ITEM_CATEGORY_WEAPON))
	{
		m_bIsLuckyItemDetail = true;
	}

	LuaState * L = _CallLuaFunction( 1, "LuckyItemDetail");
	if ( L != nullptr)
	{
		i3Lua::PushBoolean( L, m_bIsLuckyItemDetail);
		_EndLuaFunction(L, 1);
	}
}

void UIPhaseGachaShop::_SetItemInfo()
{
	INT32 slot = 0;
	// 아이템 리스트 분리
	__SetItemListInfo(slot, &m_ItemList_Cash[m_CurrentTab], L"{col:255,185,133}");
	__SetItemListInfo(slot, &m_ItemList_Point[m_CurrentTab], L"{col:112,171,228}");

	m_pItemListBox->SelectData(m_CurrentIdx, true);
}

void UIPhaseGachaShop::__SetItemListInfo( INT32& slot, i3::vector<const GACHA_ITEM_INFO* >* inven_list, i3::rc_wstring color )
{
	i3::vector<const GACHA_ITEM_INFO*>::iterator pInfo;
	//INT32 slot = 0;
	for(pInfo = inven_list->begin(); pInfo !=  inven_list->end(); ++pInfo)
	{
		LuaState * L;
		L = _CallLuaFunction( 1, "SetItemBox");
		if( L != nullptr)
		{
			// 슬롯 번호
			i3Lua::PushInteger( L, slot);

			// 박스 이미지 변경 번호
			//i3Lua::PushInteger( L, __GetAmount((*pInfo)->m_i32PurchaseAmount));

			i3Lua::PushInteger( L, __GetAmount(*pInfo));

			// Win 아이템 이름
			UINT32 pitemid =  CShopContext::i()->ConvertGoodsID((*pInfo)->m_ui32GoodsID);
			i3::rc_wstring wstr_temp;
			usf::GetShopItemName(wstr_temp, pitemid);

#if defined(_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)			
			{
				i3::rc_wstring gloryTestInfo;
				i3::sprintf(gloryTestInfo, L"[%d,%d,%d]", (*pInfo)->m_i32GloryCount, (*pInfo)->m_i32PurchaseAmount, __GetAmount(*pInfo));
				i3Lua::PushStringUTF16To8( L, gloryTestInfo + color + wstr_temp);
			}			
#else
			i3Lua::PushStringUTF16To8( L, color + wstr_temp);
#endif

			// 가격은 goodsid로 검색할 수 있다.
			MINI_MATCHING matching_;
			memset(&matching_, 0, sizeof(matching_));
			CShop::i()->GetGachaMatcing((UINT8)m_CurrentTab+1, (*pInfo)->m_i32ItemKey, &matching_);

			SHOP_GOODS_EXPANSION* price = CShop::i()->GetGoodsData(matching_.m_aMatching[0]._ui32GoodsID);

			i3::stack_wstring wstrSale;
			if(price->GetCash() > 0)
				i3::sprintf(wstrSale , L"%s%d %s{/col}", color, price->GetCash(), GAME_RCSTRING("STR_SHOP_INFO_CASH"));
			else
				i3::sprintf(wstrSale , L"%s%d %s{/col}", color, price->GetPoint(), GAME_RCSTRING("STR_SHOP_INFO_POINT"));
			i3Lua::PushStringUTF16To8( L, wstrSale);

			// 남은 갯수 / 총 갯수
			i3::sprintf(wstrSale , L"%d/%d", (*pInfo)->m_i32RemainAmount, (*pInfo)->m_i32TotalAmount);
			i3Lua::PushStringUTF16To8( L, wstrSale);

			_EndLuaFunction( L, 5);
		}

		i3UIControl* cell = m_pItemListBox->getCellChild(slot, 0);
		if (cell)
			cell->setInputDisable(true);

		slot++;
	}
}

UIPhaseGachaShop::EGACHA_PURCHASEAMOUNT UIPhaseGachaShop::__GetAmount( const GACHA_ITEM_INFO* pInfo )
{
	//const GACHA_GROUP_INFO* gacha_group = GachaContext::i()->GetGachaData(m_CurrentTab);
	// Glory count와 Purchase Amount는 각각의 WinItem Info에 들어있다.
	INT32 glory_count = pInfo->m_i32GloryCount;
	const INT32 purchase_amount = pInfo->m_i32PurchaseAmount;

	// 구매갯수가 1개미만 일경우 무조건 빈상자
	if(purchase_amount < 1)
		return EGACHA_PURCHASEAMOUNT_NONE;

	// 구매갯수가 2개미만 일경우 꽉찬상자(5단계)
	if((glory_count - purchase_amount) < 2)
		return EGACHA_PURCHASEAMOUNT_FULL;

	// 1~3단계 처리	
	if(purchase_amount < static_cast<INT32>((glory_count * 0.2) + 0.5))
		return EGACHA_PURCHASEAMOUNT_20;
	else if(purchase_amount < static_cast<INT32>((glory_count * 0.4) + 0.5))
		return EGACHA_PURCHASEAMOUNT_40;
	else if(purchase_amount < static_cast<INT32>((glory_count * 0.6) + 0.5))
		return EGACHA_PURCHASEAMOUNT_60;
	
	// 4단계(리본만남음)
	// if(purchase_amount < static_cast<INT32>((glory_count * 0.8) + 0.5))
	return EGACHA_PURCHASEAMOUNT_80;
}

void UIPhaseGachaShop::_SetOtherItem()
{
	const GACHA_ITEM_INFO * pInfo = GetSelectGachaInfo();

	LuaState * L = _CallLuaFunction( 1, "SetOtherText");
	if( L != nullptr )
	{
		i3::rc_wstring wstr_temp = L"----";

		if( pInfo != nullptr )
		{
			// main item name
			T_ItemID itemID =  CShopContext::i()->ConvertGoodsID(pInfo->m_ui32GoodsID);
			usf::GetShopItemName(wstr_temp, itemID);
			i3Lua::PushStringUTF16To8( L, wstr_temp.c_str(), wstr_temp.size());	// 아이템이름

			// other item name & shape
			for(INT32 i = 0; i< GACHA_OTHERITEM_MAX; i++)
			{
				itemID = 0;

				if( pInfo->m_arRandomGoodsIDs[i] != INVALID_GOODS_ID )
				{
					itemID =  CShopContext::i()->ConvertGoodsID(pInfo->m_arRandomGoodsIDs[i]);
					GetItemImageMgr()->SelectImage( m_OtherItemimg_ctrl[i], itemID);
					usf::GetShopItemName(wstr_temp, itemID);		
				}

				if(itemID == 0)
				{
					m_OtherItemimg_ctrl[i]->SetImage(-1);
					i3::generic_string_copy(wstr_temp, L"----");
				}

				i3Lua::PushStringUTF16To8( L, wstr_temp.c_str(), wstr_temp.size());	// 아이템이름
			}

			// random item name
			i3::stack_wstring wstrInfo;
			for(INT32 j= 0; j < RANDOM_ITEM_COUNT_BY_TAB; j++)
			{
				if( m_pGachaGroup[m_CurrentTab]->m_arRandomGoodsIDs[j] == INVALID_GOODS_ID )
					continue;

				itemID =  CShopContext::i()->ConvertGoodsID(m_pGachaGroup[m_CurrentTab]->m_arRandomGoodsIDs[j]);
				usf::GetShopItemName(wstr_temp, itemID);
				i3::generic_string_cat(wstrInfo, L"[");
				i3::generic_string_cat(wstrInfo, wstr_temp);
				i3::generic_string_cat(wstrInfo, L"] ");
			}
			i3Lua::PushStringUTF16To8( L, wstrInfo.c_str(), wstrInfo.size());	// 아이템이름
		}
		else
		{
			// other item shape
			for(INT32 idx = 0; idx < GACHA_OTHERITEM_MAX; ++idx)
				m_OtherItemimg_ctrl[idx]->SetImage(-1);

			// main item & other item & random item name
			for(INT32 idx = 0 ; idx < 7 ; ++idx)
				i3Lua::PushStringUTF16To8( L, wstr_temp.c_str(), wstr_temp.size() );
		}

		_EndLuaFunction( L, 7);
	}
}

void UIPhaseGachaShop::_SetWeaponDetail(T_ItemID itemID)
{
	bool isWeaponDetail = false;
	
	if( usf::GetShopItemCategory(itemID) == SHOP_ITEM_CATEGORY_WEAPON)
		isWeaponDetail = true;

	LuaState * L = _CallLuaFunction( 1,"WeaponDetail");
	if( L != nullptr)
	{
		i3Lua::PushBoolean( L, isWeaponDetail);
		_EndLuaFunction(L, 1);
	}
}

void UIPhaseGachaShop::_UpdateItemAni()
{
	if( m_AniTime > GACHA_MAXANITIME)
	{
		if( m_AniSlot.bPlay == false)
		{
			if(m_pItemListBox->getDataList().size() > 0 &&
				m_pItemListBox->getDataList().size() > (UINT32)m_CurrentIdx )
			{
				INT32 nTotalCount = m_pItemListBox->getDataList().size();

				if(nTotalCount > m_pItemListBox->getItemCount()) // 보이는 item의 갯수로만 제한한다.
					nTotalCount = m_pItemListBox->getItemCount();

				VEC3D Pos;
				Pos.x = 0;
				Pos.y = (REAL32)(31*(rand() % nTotalCount));
				Pos.z = 0;

				m_AniSlot.SetStartAni(&Pos);
				m_AniTime = 0;
			}
		}
	}
}

void UIPhaseGachaShop::_SelectComboItem(INT32 Slot, INT32 ComboIdx)
{
	// 같은 슬롯을 클릭 할 시 건너뛰게 합니다.
	if( m_CurrentIdx == Slot) return;
	m_CurrentIdx = Slot;
	SetButtonText();
	SetComboItem();
}

void UIPhaseGachaShop::SetComboItem()
{
	UINT32 itemID = 0;	
	const GACHA_ITEM_INFO* pInfo = GetSelectGachaInfo();

	// item shape setting
	if( pInfo != nullptr)
	{
		itemID =  CShopContext::i()->ConvertGoodsID(pInfo->m_ui32GoodsID);
		GetItemImageMgr()->SelectImage( m_goods_img_ctrl, itemID);
		m_pGachaView->setItemInfo(pInfo);
	}
	else
		m_goods_img_ctrl->SetImage(-1);

	// detail popup button
	_SetWeaponDetail(itemID);

	// items name setting
	_SetOtherItem();		
}

void UIPhaseGachaShop::_SelectTab(INT32 i32Tab)
{
	_InitGachaItemList(i32Tab);
	_SelectComboItem(0, 0);
}

void UIPhaseGachaShop::_InitGachaItemList(INT32 i32Tab)
{
	if( m_pGachaGroup[m_CurrentTab] == nullptr)
		return;

	m_CurrentTab = (GACHA_GROUP)i32Tab;
	m_CurrentIdx = -1;
	m_ItemList_Cash[m_CurrentTab].clear(); m_ItemList_Point[m_CurrentTab].clear();
	for(INT32 i = 0; i< m_pGachaGroup[m_CurrentTab]->m_i32WinItemCount; i++)
	{
		MINI_MATCHING matching_;
		memset(&matching_, 0, sizeof(matching_));
		CShop::i()->GetGachaMatcing((UINT8)m_CurrentTab+1, m_pGachaGroup[m_CurrentTab]->m_arWinItems[i].m_i32ItemKey, &matching_);

		SHOP_GOODS_EXPANSION* price = CShop::i()->GetGoodsData(matching_.m_aMatching[0]._ui32GoodsID);
		
		if( !price ) continue;
		
		// Cash 정보가 있으면 Cash 아이템이므로 
		if(price->GetCash() > 0)
			m_ItemList_Cash[m_CurrentTab].push_back(&m_pGachaGroup[m_CurrentTab]->m_arWinItems[i]);
		else
			m_ItemList_Point[m_CurrentTab].push_back(&m_pGachaGroup[m_CurrentTab]->m_arWinItems[i]);
	}

	const GACHA_ITEM_INFO* pInfo = GetSelectGachaInfo();

	//이미지셋팅
	UINT32 itemID;
	if( pInfo != nullptr )
	{
		itemID =  CShopContext::i()->ConvertGoodsID(pInfo->m_ui32GoodsID);
		GetItemImageMgr()->SelectImage( m_goods_img_ctrl, itemID);
	}else
		m_goods_img_ctrl->SetImage(-1);

	//이름셋팅
	_SetOtherItem();	

	UpdateGachaItem();
}

void UIPhaseGachaShop::_SelectSearch(const i3::wliteral_range& wKeywordRng)
{
	if( i3::generic_string_size( wKeywordRng) == 0)
	{
		// Tab을 초기화 하는 것이므로 이 함수를 쓰는 편이 좋습니다.
		// Jinsik.kim(2015/04/10)
		_SelectTab((INT32)m_CurrentTab);
		return;
	}

	m_ItemList_Cash[m_CurrentTab].clear(); m_ItemList_Point[m_CurrentTab].clear();
	i3::rc_wstring wstr_temp;

	for(INT32 i = 0; i< m_pGachaGroup[m_CurrentTab]->m_i32WinItemCount; i++)
	{
		const GACHA_ITEM_INFO *pInfo = &m_pGachaGroup[m_CurrentTab]->m_arWinItems[i];
		UINT32 pitemid =  CShopContext::i()->ConvertGoodsID(pInfo->m_ui32GoodsID);
		usf::GetShopItemName(wstr_temp, pitemid);

		if( i3::icontain_string( wstr_temp, wKeywordRng) >= 0 )
		{
			MINI_MATCHING matching_;
			memset(&matching_, 0, sizeof(matching_));
			CShop::i()->GetGachaMatcing((UINT8)m_CurrentTab+1, m_pGachaGroup[m_CurrentTab]->m_arWinItems[i].m_i32ItemKey, &matching_);

			SHOP_GOODS_EXPANSION* price = CShop::i()->GetGoodsData(matching_.m_aMatching[0]._ui32GoodsID);
			// Cash 정보가 있으면 Cash 아이템이므로 
			if(price->GetCash() > 0)
				m_ItemList_Cash[m_CurrentTab].push_back(pInfo);
			else
				m_ItemList_Point[m_CurrentTab].push_back(pInfo);
		}
	}
	UpdateGachaItem();
	
	// 새로이 검색을 하는 것이므로 Current Idx를 초기화 하고 다시 선택하도록 합니다.
	// Jinsik.kim(2015/04/10)
	m_CurrentIdx = -1;
	INT32 default_idx = m_ItemList_Cash[m_CurrentTab].empty() ? -1 : 0 ;
	if(default_idx == -1)
		default_idx = m_ItemList_Point[m_CurrentTab].empty() ? -1 : 0 ;
	_SelectComboItem(default_idx, 0);
}

void UIPhaseGachaShop::_ListSort(INT32 SortValue)
{
	switch(SortValue)
	{
	case 0:
		sort_f::inner::name(&m_ItemList_Cash[m_CurrentTab], true);
		sort_f::inner::name(&m_ItemList_Point[m_CurrentTab], true);
		break;
	case 1:
		sort_f::inner::name(&m_ItemList_Cash[m_CurrentTab], false);
		sort_f::inner::name(&m_ItemList_Point[m_CurrentTab], false);
		break;
	case 2:
		sort_f::inner::sale(&m_ItemList_Cash[m_CurrentTab], true);
		sort_f::inner::sale(&m_ItemList_Point[m_CurrentTab], true);
		break;
	case 3:
		sort_f::inner::sale(&m_ItemList_Cash[m_CurrentTab], false);
		sort_f::inner::sale(&m_ItemList_Point[m_CurrentTab], false);
		break;
	case 4:
	case 5:
		sort_f::inner::newitem(&m_ItemList_Cash[m_CurrentTab]);
		sort_f::inner::newitem(&m_ItemList_Point[m_CurrentTab]);
		break;
	default:
		I3PRINTLOG(I3LOG_NOTICE,"sort Error");
	}
	_SetItemInfo();
}

void UIPhaseGachaShop::_UpdateFlowAnnounce(REAL32 rDeltaSeconds)
{
	if( m_pAnnounce == nullptr)
		return;

	if( m_pAnnounce->getScrollState() == i3UIEditBox::UBSS_END)
	{
//		if(m_rFlowTime > 10.0f)
		{
			m_bFCMFlag = !m_bFCMFlag;
			m_bFlowFlag = false;
			m_rFlowTime = 0.0f;
			//i3::generic_string_copy(m_strTemporaryNotice, m_strPermanatNotice);
		}

		i3::rc_wstring			wstrText;

		if( m_bFCMFlag)	
		{
			wstrText = m_strTemporaryNotice;
		}
		else
		{
			wstrText = m_strPermanatNotice;
		}

		if( m_pAnnounce->getText() != nullptr)
		{
			if( i3::generic_is_equal( m_pAnnounce->getText(), wstrText) == false)
				m_pAnnounce->SetText( wstrText );
		}

		m_pAnnounce->setScrollState( i3UIEditBox::UBSS_INIT);
	}

	m_rFlowTime += rDeltaSeconds;

}

void UIPhaseGachaShop::SetSlideNotice_Permanant( const i3::rc_wstring& wStr)
{
	if( MAX_STRING_COUNT > i3::generic_string_size( wStr))
	{
		// 기존 공지와 다를 경우만 바꾼다
		if ( m_strPermanatNotice != wStr)
		{
			m_strPermanatNotice = wStr;		
			m_bFCMFlag = true;
			m_bFlowFlag = true;
		}
	}

}

void UIPhaseGachaShop::SetSlideNotice_Temporary(const i3::rc_wstring& wStr)
{
	if (MAX_STRING_COUNT > i3::generic_string_size( wStr))
	{
		// 기존 공지와 다를 경우만 바꾼다
		if ( m_strTemporaryNotice != wStr)
		{
			m_strTemporaryNotice = wStr;		
			m_bFCMFlag = true;
			m_bFlowFlag = true;
		}
	}

}

void UIPhaseGachaShop::_ClearAtUnload( i3UIScene * pScene )
{

	UIPhaseSubBase::_ClearAtUnload( pScene);
}

void UIPhaseGachaShop::slide_proc()
{

}


void UIPhaseGachaShop::HDIObject1(bool isenable )
{
	LuaState * L = _CallLuaFunction( 1, "setBGEnable");
	if( L != nullptr)
	{
		i3Lua::PushBoolean(L, isenable);
		_EndLuaFunction( L, 1);
	}
}

void UIPhaseGachaShop::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UIPhaseGachaShop::ReceiveGameEvent_Buy_Goods( INT32 arg, const i3::user_data&)
{
	if(IsInGachaRstProcessing())
	{
//		I3ASSERT(0 && "ERR! - Reward to gacha result is in progress..");
		return;
	}

	// 빌드 되게 임시 값으로 셋팅합니다. 노준영
	const GACHA_ITEM_INFO* pInfo = GetSelectGachaInfo();
	PACKET_GACHA_PURCHASE_REQ purchase;
	UINT8	ui8BuyType = static_cast<SIA_GOODS_BUY>(GachaContext::i()->GetGachaBuyType());

	MINI_MATCHING matching_;
	memset(&matching_, 0, sizeof(matching_));
	CShop::i()->GetGachaMatcing((UINT8)m_CurrentTab+1, pInfo->m_i32ItemKey, &matching_);

	UINT32	TGoodsID = matching_.m_aMatching[m_iGachaAmount]._ui32GoodsID;	// 임시.
	INT32	i32ItemIdx = GetCurrentDataIndex(pInfo->m_i32ItemKey);

	//i32ItemIdx++;

	//purchase.m_i32ItemKey = pInfo->m_i32ItemKey;
	//purchase.m_i32Amount = m_iGachaAmount;

	GameEventSender::i()->SetEvent(EVENT_START_GACHA, &ui8BuyType, &TGoodsID, &i32ItemIdx );
	GetMainFrame()->ClosePopup(UPW_BUYITEM);
	g_pFramework->GetUIMainframe()->OpenAwayMessageBox(GAME_RCSTRING("STBL_IDX_EP_LOGIN_WAIT_CONNECT_W"));/*장시간 움직임이 없으므로 5초 후 로비로 이동합니다.*/

	GachaContext::i()->ClearGachaRst();
	GachaContext::i()->SetMyBuy(true);
	m_gachaRstProcStatus = EGACHA_RST_PROCST_WAITRST_FROM_SVR;
	DisableUIInput(true);

	// 뽑기상점을 샀을 경우 TimeOut을 체크한다.
	m_purchaseTimecheck.setBaseTime(m_purchaseTimecheck.GACHA_ACK_TIME_OUT);
	m_purchaseTimecheck.Start();
}

void UIPhaseGachaShop::ReceiveGameEvent_Enter_Gacha_Info( INT32 arg, const i3::user_data&)
{
	m_pGachaGroup[arg] = GachaContext::i()->GetGachaData(arg);
	_SelectTab(0);
}

void UIPhaseGachaShop::ReceiveGameEvent_End_Gacha_State( INT32 arg, const i3::user_data&)
{
	UpdateGachaItem();
}

void UIPhaseGachaShop::ReceiveGameEvent_Start_Gacha( INT32 arg, const i3::user_data&)
{
	if(m_gachaRstProcStatus != EGACHA_RST_PROCST_WAITRST_FROM_SVR)
	{
		I3TRACE("ERR! - called in bad status (%d)", m_gachaRstProcStatus);
		return;
	}

	// Close message box "Connecting to the server Please wait message box.."
	g_pFramework->GetUIMainframe()->CloseAwayMessageBox();
	g_pFramework->GetUIMainframe()->InputDisableMainUI(true);

	// 위에 CloseAwayMessageBox하면서 UI Input락이 풀림
	DisableUIInput(true);

	GACHA_ERROR rst = (GACHA_ERROR)arg;

	if(rst != GACHA_ERROR_SUCCESS)
	{
		m_gachaRstProc.m_msgBoxStr = GenerateGachaRstErrMsg(rst);
		m_gachaRstProcStatus = EGACHA_RST_PROCST_ERR_MSGBOX;
		return;
	}

	m_gachaRstProcStatus = EGACHA_RST_PROCST_REWARD_START;

	// Gacha가 시작할 때 Sound를 틀러주어야 하므로 여기서 사운드를 실행한다.
	switch(m_iGachaAmount)
	{
	case EGACHA_FIRST_CLICK: 
		g_pSndMng->StartLotterySnd(GSND_LOTTERY_CAPSULEDROP_ONE);
		break;
	case EGACHA_SECOND_CLICK: 
		g_pSndMng->StartLotterySnd(GSND_LOTTERY_CAPSULEDROP_FIVE);
		break;
	case EGACHA_THIRD_CLICK: 
		g_pSndMng->StartLotterySnd(GSND_LOTTERY_CAPSULEDROP_TEN);
		break;
	}
	

	// 구매한 가차 항목의 구매횟수 업데이트
	INT32 groupidx = GET_GOODS_SUBTYPE(GachaContext::i()->GetGachaRstHeader()->m_TGoodsID);
	INT32 itemkey = (INT32)GET_GOODS_NUMBER(GachaContext::i()->GetGachaRstHeader()->m_TGoodsID);
	INT32 Itemidx = GachaContext::i()->GetGachaRstHeader()->m_i32ItemIdx;
	INT32 accumPurchase = GachaContext::i()->GetGachaRstHeader()->m_i32AccumPurchase;

	GACHA_GROUP_INFO* pGroupinfo = GachaContext::i()->GetGachaData(groupidx-1);

	if(pGroupinfo->m_arWinItems[Itemidx].m_i32ItemKey == itemkey)
	{
		pGroupinfo->m_arWinItems[Itemidx].m_i32PurchaseAmount = accumPurchase;
	}

	//bool contFind = true;

	//for(INT32 i = 0; contFind && i < GACHA_GROUP_COUNT; i++)
	//{
	//	GACHA_GROUP_INFO* pGroupinfo = GachaContext::i()->GetGachaData(i);

	//	for(INT32 j = 0; contFind && j < WIN_ITEM_COUNT_BY_TAB; j++)
	//	{
	//		if( pGroupinfo->m_arWinItems[j].m_i32ItemKey == findItemKey)
	//		{
	//			pGroupinfo->m_arWinItems[j].m_i32PurchaseAmount = accumPurchase;
	//			contFind = false;
	//			break;
	//		}
	//	}
	//}

	// 구매항목 업데이트에따라서 UI갱신
	UpdateGachaItem();


	//////////////////////////////////////////////////////////////////////////
	// reward 데이터 구성
	//////////////////////////////////////////////////////////////////////////

	// 1. 당첨된 가차아이템 or 꽝아이템
	// 2. 럭키아이템(당첨되었을경우) 
	// 3. 글로리아이템(당첨아이템이 뽑히지않아 가격탭당 지정된 구매횟수 충족시)
	
	// 가차1회 실시에 가차아이템,럭키아이템 두개를 실시한다.
	// 꽝아이템을 지정된횟수만큼 뽑으면 글로리 아이템이 지급된다.

	bool fatalErr = false;
	INT32 fatalRst0 = 0, fatalRst1 = 0, fatalRst2 = 0;

	for(INT32 i=0; !fatalErr && i< GachaContext::i()->GetGachaRstDataCount(); i++)
	{
		PACKET_GACHA_RESULT_DATA dat;

		GachaContext::i()->GetGachaRstData(i, &dat);

		if(dat.m_eGachaResult == GACHA_RESULT_NONE)
		{
			fatalErr = true;
			fatalRst0 = i;
			fatalRst1 = dat.m_ui32WinGoodsID;
			fatalRst2 = dat.m_ui32LuckyGoodsID;		
			continue;
		}

		// GACHA_RESULT_WIN/GACHA_RESULT_GLORY/GACHA_RESULT_RANDOM
		// 당첨아이템 / 영예아이템 / 랜덤아이템이 이쪽으로 들어옴
		if(dat.m_ui32WinGoodsID != INVALID_GOODS_ID)
		{
			GachaContext::GACHA_REAWARD reward;

			reward.m_result = dat.m_eGachaResult;
			reward.m_rewardGoodsID = dat.m_ui32WinGoodsID;

			GachaContext::i()->PushGachaReward(reward);
		}

		// GACHA_RESULT_LUCKY
		// 럭키아이템
		if(dat.m_ui32LuckyGoodsID != INVALID_GOODS_ID)
		{
			GachaContext::GACHA_REAWARD reward;

			// GACHA_RESULT_LUCKY 이값은 서버에서 사용되지않음
			reward.m_result = GACHA_RESULT_LUCKY;
			reward.m_rewardGoodsID = dat.m_ui32LuckyGoodsID;

			GachaContext::i()->PushGachaReward(reward);
		}		
	}

	if(fatalErr)
	{

#if defined(I3_DEBUG) // || defined(BUILD_RELEASE_QA_VERSION)
		char buf[MAX_PATH]={0}; 
		const PACKET_GACHA_RESULT_HEADER* gachaHdr = GachaContext::i()->GetGachaRstHeader();
		I3TRACE("ERR! ===== Server Gacha Result Data err Start ====");
		sprintf_s(buf, MAX_PATH, "ERR! [Gacha Header] err=%d, itemKey=[%d], accumPurchase=[%d], resultCount=[%d]",
			gachaHdr->m_eErrorCode, gachaHdr->m_i32ItemIdx, gachaHdr->m_i32AccumPurchase, gachaHdr->m_i32ResultCount);
		I3TRACE(buf);

		sprintf_s(buf, MAX_PATH, "ERR! [Gacha RstDat] no=%d , win=%d, lucky=%d", fatalRst0, fatalRst1, fatalRst2);
		I3TRACE(buf);
		I3TRACE("ERR! ===== Server Gacha Result Data err End ====");
#endif

		GachaContext::i()->ClearGachaReward();
		// 여긴 사실들어오면 안되는곳인데. 서버쪽에서 뭔가처리가 잘못되어서(랜덤아이템이 웹툴에서 설정안되어있다던지)
		// 결과중에 오류가 섞여들어온거, 유저는 환불받아야겠지
		// 일단 정상결과라도 출력해주기위해서
		m_gachaRstProcStatus = EGACHA_RST_PROCST_ERR_FATAL;

		return;
	}

}

void UIPhaseGachaShop::UpdateGachaRstProcessing(REAL32 rDeltaSeconds)
{
	if(m_gachaRstProcStatus == EGACHA_RST_PROCST_NONE)
		return;

	switch(m_gachaRstProcStatus)
	{
	case EGACHA_RST_PROCST_WAITRST_FROM_SVR:
		{
			_AttachToTopScreen();

			DisableUIInput(true);
			HDIObject1(true);
			m_pGachaView->BeginGachaPlayingAni();
		}
		return;

	case EGACHA_RST_PROCST_REWARD_START:
		{
			if(m_pGachaView->UpdateGachaPlayingAni(rDeltaSeconds))
			{
				// 결과패킷은 다왔지만, 애니메이션중이라면 끝날때까지 기다려준다.
				return;
			}
			HDIObject1(false);
			m_gachaRstProcStatus = EGACHA_RST_PROCST_UPDATE;
			m_gachaRstProc.WaitGachaResultPopup();
			m_purchaseTimecheck.Stop();
			break;
		}

	case EGACHA_RST_PROCST_ERR_MSGBOX:
		{
			// 애니메이션종료
			m_pGachaView->UpdateGachaPlayingAni(999.0f);

			m_gachaRstProcStatus = EGACHA_RST_PROCST_UPDATE;
			m_gachaRstProc.WaitMessageBox();
		}
		break;

	case EGACHA_RST_PROCST_UPDATE:
		{
			if(!m_gachaRstProc.Update(rDeltaSeconds))
			{
				DisableUIInput(false);
				m_gachaRstProcStatus = EGACHA_RST_PROCST_NONE;
				break;
			}
		}
		break;

	case EGACHA_RST_PROCST_ERR_FATAL:
		{
			// 애니메이션종료
			m_pGachaView->UpdateGachaPlayingAni(999.0f);

			m_gachaRstProcStatus = EGACHA_RST_PROCST_NONE;

			DisableUIInput(true);

			GameUI::MsgBox(MSG_TYPE_SYSTEM_OK, GAME_RCSTRING("STBL_IDX_EP_GAME_EXIT_ASSERT_E"), 
				nullptr, EXIT_CALLBACK(CallBackFatalErrExitMsgBox,true), this);
		}
		return;

	default:
		return;
	}
}


void UIPhaseGachaShop::ReceiveGameEvent_Gacha_Announce( INT32 arg, const i3::user_data&)
{
	SetSlideNotice_Permanant(GachaContext::i()->GetGachaNotice());

	if(arg == GachaContext::EANNOUNCE_WINITEMUSER)
	{
		i3::wstring winItemMsgPrefix;
		i3::sprintf(winItemMsgPrefix, L"%s {col:210,0,0,255}(%s){/col} ", 
			GachaContext::i()->GetGachaLuckyUser(), GachaContext::i()->GetGachaLuckyGoodsName());

		i3::wstring winItemMsg;
		i3::sprintf(winItemMsg, GAME_RCSTRING("STR_LUCKYSHOP_NOTICE_WINNER"), winItemMsgPrefix);
		SetSlideNotice_Temporary(winItemMsg);
	}
	else
	{
		SetSlideNotice_Temporary(GachaContext::i()->GetGachaLuckyUser());
	}

	GameEventSender::i()->SetEvent( EVENT_GACHA_PURCHASE_COUNT );
}

void UIPhaseGachaShop::ReceiveGameEvent_Item_Auth( INT32, const i3::user_data&)
{
	UpdateGachaRight();
}

void UIPhaseGachaShop::ReceiveGameEvent_Refresh_Cash( INT32, const i3::user_data&)
{
	UpdateGachaRight();
}


//
// UIPhaseGachaShop::GachaRstProcessor
//

void UIPhaseGachaShop::GachaRstProcessor::OnMsgBoxCallback(void* pThis, INT32 nParam)
{
	if(!pThis)
		return;

	UIPhaseGachaShop::GachaRstProcessor* callback = (UIPhaseGachaShop::GachaRstProcessor*)pThis;
	callback->OnMsgBoxResult(nParam);
}

UIPhaseGachaShop::GachaRstProcessor::GachaRstProcessor() 
 : CMsgBoxCallBack(__FILE__, __LINE__, 
     UIPhaseGachaShop::GachaRstProcessor::OnMsgBoxCallback, false, false)
{
	m_msgBox = nullptr;
	m_owner = nullptr;
	m_isWaiting = 0;
}

UIPhaseGachaShop::GachaRstProcessor::~GachaRstProcessor()
{
	Stop();
}

void UIPhaseGachaShop::GachaRstProcessor::WaitGachaResultPopup()
{
	if(m_isWaiting)
		return;

	// 가차결과 창 보여주기
	UIMainFrame* pFrame = g_pFramework->GetUIMainframe();
	if( pFrame->GetCurrentSubPhaseState() == PS_IDLE
		|| pFrame->GetCurrentSubPhaseState() == PS_PLAYING)
	{
		m_isWaiting = 1;
		pFrame->OpenPopup(UPW_GACHA_RESULT);			
	}		

}

void UIPhaseGachaShop::GachaRstProcessor::WaitMessageBox()
{
	if(m_isWaiting)
		return ;

	m_isWaiting = 2;
	m_msgBox = GameUI::MsgBox(MSG_TYPE_GAME_OK, m_msgBoxStr, nullptr, (*this), this);			
}

void UIPhaseGachaShop::GachaRstProcessor::Stop()
{
	if(!m_isWaiting)
		return;

	if(m_isWaiting == 1)
	{
		if(g_pFramework->GetUIMainframe()->IsOpenedPopup(UPW_GACHA_RESULT))
			g_pFramework->GetUIMainframe()->ClosePopup(UPW_GACHA_RESULT);				
	}
	else if(m_isWaiting == 2)
	{
		CloseMsgBox();
	}

	m_isWaiting = 0;
}

void UIPhaseGachaShop::GachaRstProcessor::OnMsgBoxResult(INT32 nParam)
{
	Stop();
}

void UIPhaseGachaShop::GachaRstProcessor::CloseMsgBox()
{
	if(m_msgBox)
	{
		m_msgBox->Close();
///		GameUI::CloseMsgBox(m_msgBox);
		m_msgBox = nullptr;
	}
}

bool UIPhaseGachaShop::GachaRstProcessor::Update(REAL32 rDeltaSec)
{
	if(!m_isWaiting)
		return false;

	if(m_isWaiting == 1)
	{
		if(!g_pFramework->GetUIMainframe()->IsOpenedPopup(UPW_GACHA_RESULT))
		{
			Stop();
			return false;
		}		
	}
	else if(m_isWaiting == 2)
	{
		if(!m_msgBox)
			return false;
	}

	return true;
}

void UIPhaseGachaShop::DisableUIInput(bool disable)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	i3UIFrameWnd* btnsFrame = pFrame->GetTopMenu_V15()->GetMainBtnsFrame();

	btnsFrame->setInputDisable(disable);
	SetInputDisable(disable);	
}

const wchar_t* UIPhaseGachaShop::GenerateGachaRstErrMsg(GACHA_ERROR err)
{
	const wchar_t* errMsg = L"Unknown Error!";

	switch(err)
	{
	case GACHA_ERROR_SUCCESS			: errMsg = L"Success"; break;
	case GACHA_ERROR_INTERNAL			: errMsg = L"Internal server error!"; break;					// 서버 내부 에러
	case GACHA_ERROR_CHARGE				: errMsg = L"Insufficient charge or ticket "; break;			// 캐시, 티켓 부족
	case GACHA_ERROR_INVALID_REQUEST	: errMsg = L"Invalid server request "; break;				// 유효하지 않은 요청
	case GACHA_ERROR_PAYMENT			: errMsg = L"Error in payment"; break;						// 결제 실패G
	case GACHA_ERROR_DISABLED			: errMsg = L"Gacha is in unusable state"; break;				// 사용할 수 없음
	case GACHA_ERROR_USER				: errMsg = L"User infomation error"; break;				// 유저 오류
	default:
		break;
	}

	return errMsg;
}

void UIPhaseGachaShop::CallBackFatalErrExitMsgBox(void* pThis, INT32 nParam)
{
	// 심각한에러가 발생해서 클라이언트를 종료합니다
	I3TRACE("ERR! - Force disconnect from server..[Error in Gacha Results!!]");
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_M_DISCONNECTION);
}

void UIPhaseGachaShop::Click_Help()
{
	if( GetMainFrame()->GetCurrentSubPhaseState() == PS_IDLE
		|| GetMainFrame()->GetCurrentSubPhaseState() == PS_PLAYING)
	{
		GetMainFrame()->OpenPopup(UPW_GACHA_HELP_GUIDE);
	}
}

void UIPhaseGachaShop::PurchaseTimeOut( )
{
	if(m_purchaseTimecheck.Update())
	{
#ifndef _DEBUG
		//	서버로 구입 확인을 보내고 답변을 받을 때까지 기다린다. 
		//	만약 대기 시간이 초과될 경우 서버로 부터 답변을 못받았다고 판단되므로 게임을 강제 종료합니다.

		/*네트워크 문제로 게임이 종료 되었습니다.*/
		GameUI::ExitGamePopup( GAME_RCSTRING("STBL_IDX_EP_BATTLE_TIMEOUT_HOST_C") );
#else
		/*네트워크에 문제로 실패했습니다.\n 다시 시도해 주십시오.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_FAILED_BY_NET"), &GAME_RCSTRING("STBL_IDX_EP_SHOP_BUY_SUCCESS_CAPTION") );
#endif
	}
}

void UIPhaseGachaShop::SetButtonText()
{
	const GACHA_ITEM_INFO* pInfo = GetSelectGachaInfo();
	if(pInfo == nullptr)
		return;

	MINI_MATCHING matching_;
	memset(&matching_, 0, sizeof(matching_));
	CShop::i()->GetGachaMatcing((UINT8)m_CurrentTab+1, pInfo->m_i32ItemKey, &matching_);

	i3::rc_wstring buttonString[3];
	for(INT32 i = 0; i < EGACHA_CLICK_COUNT; i++)
	{
		i3::sprintf(buttonString[i], L"%d", matching_.m_aMatching[i]._ui32ItemArg);
		
		if(matching_.m_aMatching[i]._ui32BonusArg > 0)
		{
			i3::rc_wstring temp;
			i3::sprintf(temp, L"+%d", matching_.m_aMatching[i]._ui32BonusArg);
			buttonString[i] += temp;
		}
		
		m_ClickButton[i]->SetText(buttonString[i] + GAME_RCSTRING("STR_LUCKYSHOP_START_BUY"));
	}
}

void UIPhaseGachaShop::UpdateStateTimeCheck()
{
	if(m_UpdateStateTime.Update())
	{
		// 이벤트를 보내 REQ 패킷을 보낸다.
		GameEventSender::i()->SetEvent(EVENT_GACHA_STATE_REQ);

		// 시간을 다시 시작한다.
		m_UpdateStateTime.Start();
	}
}

void UIPhaseGachaShop::FocusOutItemListView()
{	
	if( m_pCtrlListView == nullptr)
		return;

	if( !m_pCtrlListView->isEnable() )
		return;


	i3UIFrameWnd * pGameCurCtrl = reinterpret_cast<i3UIFrameWnd*>(g_pFramework->getFocus());

	if( pGameCurCtrl == nullptr )
		return;

	if( pGameCurCtrl != m_pCtrlListView
		&& pGameCurCtrl != m_pCtrlButtonList1
		&& pGameCurCtrl != m_pCtrlButtonList2 
		&& pGameCurCtrl != m_pCtrlListView->getTopFrameWnd() )
	{
		m_pCtrlListView->EnableCtrl(false);
		m_pCtrlButtonList1->EnableCtrl(true);
		m_pCtrlButtonList2->EnableCtrl(false);
	}	
}