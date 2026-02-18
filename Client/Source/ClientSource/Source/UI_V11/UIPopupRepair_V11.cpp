#include "pch.h"
#include "UI_V11/UIPopupRepair_V11.h"

#include "UI/UIMainFrame_V11.h"
#include "UI/UIUtil.h"
#include "UI/UIShopFunction.h"
#include "UI/ItemImageMgr.h"
#include "UI/PurchaseMgr.h"

#include "../GlobalVariables.h"
#include "../ShopContext.h"
#include "UserInfoContext.h"


//	구입후 시간동안 서버 응답을 기다립니다.
#define BUY_TIMEOUT			20

I3_CLASS_INSTANCE( UIPopupRepair_V11);

// Lua용 Glue 함수
static UIPopupRepair_V11* GetThis( void)
{
	UIMainFrame*	pFrame	= g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);
	return (UIPopupRepair_V11*) pFrame->GetPopup( UPW_REPAIR );
}

extern "C" {
	int UIPopupRepair_V11_Confirm( LuaState * L)
	{
		GetThis()->RepairConfirm();
		return 0;
	}

	int UIPopupRepair_V11_ChargeCash( LuaState * L)
	{
		GetThis()->InputCashCharge();
		return 0;
	}
}

LuaFuncReg UIPopupRepair_V11_Glue[] =
{
	{"Confirm",			UIPopupRepair_V11_Confirm	},
	{"ChargeCash",		UIPopupRepair_V11_ChargeCash},
	{NULL,				NULL}
};


UIPopupRepair_V11::UIPopupRepair_V11() : m_pImgCtrl( NULL )
{
	m_ItemID		= 0;
	m_CanRepair		= FALSE;

	m_bConfirm	= FALSE;
	m_fTimeout	= 0.0f;
}

UIPopupRepair_V11::~UIPopupRepair_V11()
{	
	I3_SAFE_RELEASE( m_pImgCtrl );
}

/*virtual*/ void UIPopupRepair_V11::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	AddScene( "Scene/Popup/PBRe_PopUp_InvenRepair.i3UIs", UIPopupRepair_V11_Glue );
}

/*virtual*/ void UIPopupRepair_V11::OnLoadAllScenes(void)
{
	UIPopupBase::OnLoadAllScenes();

	I3_SAFE_RELEASE( m_pImgCtrl );
	m_pImgCtrl	= GetItemImageMgr()->CreateImgSetCtrl( GetScene(0), "i3UIButtonImageSet" );
}

bool UIPopupRepair_V11::OnEntranceStart(void * pArg1, void * pArg2)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	// 초기화.
	m_bConfirm = FALSE;
	m_fTimeout = 0.0f;

	m_ItemID	= (UINT32)pArg1;
	m_CanRepair	= FALSE;
	
	return _SetRepairData();
	//return true;
}

/*virtual*/ void UIPopupRepair_V11::OnEntranceEnd(void)
{
	UIPopupBase::OnEntranceEnd();

	GetItemImageMgr()->SelectImage( m_pImgCtrl, m_ItemID);
}

void UIPopupRepair_V11::InputCashCharge(void)
{
	//	캐시 충전 웹페이지 호출
#if defined(LOCALE_CHINA) 
	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_SHOP_PREPARE_PLAN"));/*차후 지원될 예정입니다.*/
	//g_pSDOA_Link->OpenWidget();
#elif defined(LOCALE_TAIWAN)
	g_pFramework->OpenWebBrowser(g_pConfigEx->GetShop().ChargeURL,NULL,false);
#else
	::ExcuteCashWeb();
#endif
}


void UIPopupRepair_V11::_ClosePopup(void)
{
	GetMainFrame()->ClosePopup(UPW_REPAIR);	
}

// 팝업에 내구도 데이터를 설정
bool UIPopupRepair_V11::_SetRepairData(void)
{
	LuaState*	L	= _CallLuaFunction( "SetRepairData" );

	if( L != NULL )
	{
		CShopContext*		pShopContext	= CShopContext::i();

		UINT32				Price;
		GOODS_PRICE_TYPE	PriceType;
		if ( !pShopContext->GetRepairPrice( m_ItemID, Price, PriceType ) )
		{
			I3PRINTLOG(I3LOG_FATAL,  "ID : %u, Price : %d, PriceType : %d\n",  m_ItemID, Price, PriceType);
			return false;
		}

		switch ( pShopContext->CheckCanRepair(m_ItemID) )
		{
		case REPAIR_TYPE_CANNOT:	return false;		// 돈이 모자람
		case REPAIR_TYPE_NEEDLESS:	return false;		// 내구도 100
		case REPAIR_TYPE_CAN_FULL:						// 전체 다 수리 가능
		case REPAIR_TYPE_CAN_SOME:						// 부분적으로만 수리가능
			m_CanRepair = TRUE;
			break;
		}


		i3::rc_wstring		PriceTypeText;
		i3::rc_wstring		szItemName;
		USER_INFO_BASIC		UserInfo;
		UINT32				UserMoney;

		
		usf::GetShopItemName( szItemName, m_ItemID );

		UserInfoContext::i()->GetMyUserInfoBasic( &UserInfo );

		if( PriceType == GOODS_PRICE_CASH )		
		{
			PriceTypeText	= GAME_RCSTRING("STR_SHOP_INFO_CASH");
			UserMoney		= UserInfo.m_ui32Cash;
		}
		else
		{
			PriceTypeText	= GAME_RCSTRING("STR_SHOP_INFO_POINT");
			UserMoney		= UserInfo.m_ui32Point;
		}
		
		i3::stack_wstring strUserMoney, strPrice;

		i3_locale::itoa_comma(Price, strPrice);
		i3_locale::itoa_comma(UserMoney, strUserMoney);

		if( !UserInfoContext::i()->IsNewRepair() )	i3Lua::PushStringUTF16To8( L, GAME_RCSTRING("STR_POPUP_REPAIR_TIP") );	// 팁
		else										i3Lua::PushStringUTF16To8( L, GAME_RCSTRING("STR_POP_REPAIR_TIP") );
		i3Lua::PushStringUTF16To8( L, szItemName );			// 무기명
		i3Lua::PushStringUTF16To8( L, PriceTypeText );		// 수리비 타입
		i3Lua::PushStringUTF16To8( L, strUserMoney.c_str() );	// 소지금
		i3Lua::PushStringUTF16To8( L, strPrice.c_str() );		// 수리비
		i3Lua::PushBoolean( L, g_pConfigEx->GetShop().Enable_ChargeCash );
		_EndLuaFunction( L, 6 );
	}

	return true;
}

void UIPopupRepair_V11::_SendBuyEvent(void)
{
	SetInputDisable( true );

	m_bConfirm = TRUE;

	UIPhaseInventory_V11* phase = static_cast<UIPhaseInventory_V11*>(GetMainFrame()->GetSubPhase(USP_INVENTORY));
	PurchaseMgr::i()->repair( phase, "finish_repair_item", m_ItemID );
}