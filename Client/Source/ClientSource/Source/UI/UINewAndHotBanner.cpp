#include "pch.h"
#include "UINewAndHotBanner.h"

#include "UIMainFrame.h"
#include "UITopMenu_V15.h"
#include "UIShopFunction.h"
#include "UIUtil.h"
#include "UIItemInfoUtil.h"
#include "UIPhaseShop.h"

#include "ItemImageMgr.h"
#include "Shop.h"
#include "ShopContext.h"

I3_CLASS_INSTANCE( UINewAndHotBanner);//, UIFloatingBase);

UINewAndHotBanner::UINewAndHotBanner()
	:m_nCurGoodsIndex(-1), m_fChangeTime(0.f), m_fAniTime(0.f), m_bOnAni(false), m_nCurGoodsType(0),
	m_goods_img_ctrl(nullptr), m_prev_img_ctrl(nullptr), m_bMakeGoodsList(false), m_bNewAndHotClicked(false)
{
	m_AlignedPos = UIFLOATING_POS_RIGHTBOTTOM;
	m_bFixedWindow = true;

#if defined( I3_DEBUG)
	SetName( "NewAndHotBanner");
#endif
}

UINewAndHotBanner::~UINewAndHotBanner()
{
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//

extern "C" {
	int			UINewHot_ClickGotoShop( LuaState * L)
	{
		UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

		if( pFrame != nullptr)
		{
			// 상점에서 상품이 속한 탭으로 이동
			UIPhaseShop * pPhaseShop = (UIPhaseShop*)g_pFramework->GetUIMainframe()->GetSubPhase(USP_SHOP);
			if( pPhaseShop != nullptr)
			{
				UINewAndHotBanner * pThis = static_cast<UINewAndHotBanner*>(GameUI::GetFloating(UFW_NEWANDHOT));
				if( pThis != nullptr)
					pPhaseShop->SetEnteranceTab(pThis->GetCurGoodsType());
			}

			UITopMenu_V15* pTopMenu = pFrame->GetTopMenu_V15();
			pTopMenu->GotoShopFromNewAndHot();
		}

		UINewAndHotBanner * pThis = static_cast<UINewAndHotBanner*>(GameUI::GetFloating(UFW_NEWANDHOT));
		if( pThis != nullptr)
			pThis->SetNewAndHotClicked( true);
		I3TRACE( "## Go to Shop From Lobby New Hot Banner \n");

		return 0;
	}

	int			UINewHot_ClickChangeItemView( LuaState * L)
	{
		bool bNextItem = i3Lua::GetBooleanArg(L , 1);

		UINewAndHotBanner * pThis = static_cast<UINewAndHotBanner*>(GameUI::GetFloating(UFW_NEWANDHOT));
		if( pThis != nullptr)
			pThis->ChangeItemView(bNextItem == true ? true : false);

		return 0;
	}
}

LuaFuncReg UINewHotBanner_Glue[] = 
{
	{"ClickGotoShop",				UINewHot_ClickGotoShop},
	{"ClickChangeItemView",				UINewHot_ClickChangeItemView},
	{ nullptr,							nullptr}
};
//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//
/*virtual*/ void UINewAndHotBanner::OnCreate( i3GameNode * pParent)
{
	UIFloatingBase::OnCreate( pParent);
	// Load Scene

	AddScene( "Scene/Main/PointBlankRe_Lobby_NewHot.i3UIs", UINewHotBanner_Glue);

	m_RefGoodsList.clear();
}

/*virtual*/ bool UINewAndHotBanner::OnEntranceStart( void* pArg1 /*= 0*/, void* pArg2 /*= 0*/)
{
	if( UIFloatingBase::OnEntranceStart( pArg1, pArg2) == false )
		return false;

	m_slide.ForceEnd();	//slide 행동을 강제한다.
	
	_InitVariables();
	SetNewAndHotClicked( false);

	MakeNewAndHotGoodsList();
	SelectGoodsInList();

	return true;
}

/*virtual*/ bool UINewAndHotBanner::OnExitStart()
{
	if( UIFloatingBase::OnExitStart() == false)
		return false;
	
	m_slide.ForceEnd();	//slide 행동을 강제한다.
	
	return true;
}

/*virtual*/void	UINewAndHotBanner::OnLoadAllScenes()
{
	UIFloatingBase::OnLoadAllScenes();

	i3UIScene * pScene = GetScene( "PointBlankRe_Lobby_NewHot");
	LuaState* L = nullptr;
	
	L = UILua::CallLuaFunction( pScene, "OnLoadScene");
	UILua::EndLuaFunction(L , 0);
	
	i3UIButtonImageSet* btnset = nullptr;

	L = UILua::CallLuaFunction( pScene, "get_goods_img_ctrl");
	btnset = reinterpret_cast< i3UIButtonImageSet* >(UILua::ReturnLuaPointer(L, 0));
	m_goods_img_ctrl = GetItemImageMgr()->CreateImgSetCtrl( btnset);

	L = UILua::CallLuaFunction( pScene, "get_ani_img_ctrl");
	btnset = reinterpret_cast< i3UIButtonImageSet* >(UILua::ReturnLuaPointer(L, 0));
	m_prev_img_ctrl = GetItemImageMgr()->CreateImgSetCtrl( btnset);
}

void UINewAndHotBanner::_InitializeAtLoad( i3UIScene * pScene)
{
	UIFloatingBase::_InitializeAtLoad( pScene);
}

void UINewAndHotBanner::_ClearAtUnload( i3UIScene * pScene)
{
	UIFloatingBase::_ClearAtUnload( pScene);
}

/*virtual*/void	UINewAndHotBanner::OnUnloadAllScenes()
{
	i3::safe_destroy_instance(m_goods_img_ctrl);
	i3::safe_destroy_instance(m_prev_img_ctrl);

//	I3_SAFE_DELETE( m_goods_img_ctrl);
//	I3_SAFE_DELETE( m_prev_img_ctrl);

	_InitVariables();
}

/*virtual*/ void UINewAndHotBanner::OnUpdate( REAL32 rDeltaSeconds)
{
	UIFloatingBase::OnUpdate( rDeltaSeconds);

	//로비 일때만
	for( int sptype = 0; sptype < USP_MAX; ++sptype )
	{
		if( GetMainFrame()->GetCurrentSubPhaseType() == (UISUBPHASE)sptype ) return;
	}
	if( GetMainFrame()->GetCurrentPhaseType() != UIPHASE_LOBBY ) return;

	if( m_nCurGoodsIndex != -1)
		m_fChangeTime += rDeltaSeconds;

	//5초 마다 바꿉니다.
	if( m_fChangeTime >= (REAL32)GOODS_CHANGE_TIME )
	{
		SelectGoodsInList();
		m_fChangeTime = 0.f;
	}

	_AniUpdate( rDeltaSeconds);
}

/*virtual*/ void UINewAndHotBanner::ProcessGameEvent( GAME_EVENT Event, INT32 Arg, const i3::user_data& UserData)
{
	UIFloatingBase::ProcessGameEvent( Event, Arg, UserData);

	switch(Event)
	{
	case EVENT_ENTER_SHOP :
	case EVENT_ENTER_GIFTSHOP :
		{
			_InitVariables();
			SetNewAndHotClicked( false);
		}
		break;
	}
}


void	UINewAndHotBanner::_InitVariables( void)
{
	m_nCurGoodsIndex = -1;
	m_fChangeTime = 0.f;
	m_fAniTime = 0.f;
	m_bOnAni = false;

	SetMakeList( false);
}

void	UINewAndHotBanner::_SetBlankGoods( void)
{
	i3UIScene * pScene = GetScene( "PointBlankRe_Lobby_NewHot");

	LuaState* L = nullptr;
	L = UILua::CallLuaFunction( pScene, "set_blank");
	UILua::EndLuaFunction(L , 0);
}

//UITapShopNewAndHot을 참고함.
void	UINewAndHotBanner::_MakeGroupList_ShopGoodsType( i3::vector<const SHOP_GOODS_PACK*>& refGoodsList, SHOP_GOODS_TYPE t)
{	
	CShopContext * pShopContext = CShopContext::i();
	INT32 goodsPackCount = pShopContext->GetGoodsPackCount(t);

	for(INT32 i = 0 ; i < goodsPackCount ; i++)
	{
		const SHOP_GOODS_PACK * pGoodsPack = pShopContext->GetGoodsPack( t, i);
		if( pGoodsPack == nullptr)
			continue;
		SHOP_GOODS * pGoods = pGoodsPack->GetGoods( 0);
		if( pGoods == nullptr)
			continue;
		INT32  itemID = pGoods->_ItemID;
		if (itemID <= -1) continue;

		// 상점 아이템만 표시.
		if( pGoods->isShopGoods() == false )
			continue;
		
		if (pGoods->GetSaleMark(PRICE_TYPE_SHOP_REAL) == 0)	
			continue;

		//keyword 없음.
		refGoodsList.push_back(pGoodsPack);
	}
}

void	UINewAndHotBanner::_StartAni( INT32 cur, INT32 prev )
{
	m_bOnAni = true;

	m_goods_img_ctrl->SetAlpha( 0.f);
	
	m_prev_img_ctrl->SetAlpha( 1.f); 
	m_prev_img_ctrl->SetEnable( true);
}

void	UINewAndHotBanner::_EndAni( void)
{
	m_bOnAni = false;

	m_goods_img_ctrl->SetAlpha( 1.f);

	m_prev_img_ctrl->SetAlpha( 0.f); 
	m_prev_img_ctrl->SetEnable( false);
}

void	UINewAndHotBanner::_AniUpdate( REAL32 rDeltaSeconds)
{
	if( m_bOnAni)
	{
		m_fAniTime += rDeltaSeconds;

		REAL32 ratio = ( m_fAniTime / GOODS_ANIMATION_TIME);

		m_goods_img_ctrl->SetAlpha( ratio);
		m_prev_img_ctrl->SetAlpha( 1.f - ratio);

		if( m_fAniTime >= (REAL32)GOODS_ANIMATION_TIME )
		{
			_EndAni();
			m_fAniTime = 0.f;	
		}
	}
}

INT32	UINewAndHotBanner::_SelectIndexBySequence( INT32 nCurIndex, INT32 nRange)
{
	++nCurIndex;

	if( (nCurIndex < 0) || (nCurIndex >= nRange) ) return 0;

	return nCurIndex;
}

INT32	UINewAndHotBanner::_SelectIndexByRandom( INT32 nCurIndex, INT32 nRange)
{
	INT32 rand_Index = 0;
	rand_Index = i3Math::Rand() % nRange;
	if( rand_Index == nCurIndex) //같은 수 나왔을 경우.
	{
		if( rand_Index == 0)
		{
			rand_Index = i3Math::Rand() % (nRange - 1);
			rand_Index++;
		}
		else
		{
			rand_Index = i3Math::Rand() % nCurIndex;
		}
	}
	
	I3ASSERT( rand_Index < nRange);

	return rand_Index;
}

void	UINewAndHotBanner::MakeNewAndHotGoodsList( void)
{
	CShop* shop = CShop::i();
	INT32 nGoodsCount = shop->GetGoodsCount();

	//I3TRACE( " Total Goods Count : %d \n", nGoodsCount);

	if( nGoodsCount <= 0) return;

	CShopContext* pShopContext = CShopContext::i();
	pShopContext->BuildGoodsList();

	m_RefGoodsList.clear();
	for( int nType = SHOP_WEAPON_GOODS; nType < SHOP_PACKAGE_GOODS; ++nType )
	{
		_MakeGroupList_ShopGoodsType( m_RefGoodsList, (SHOP_GOODS_TYPE)nType);
	}

	SetMakeList( true);
}

INT32	UINewAndHotBanner::SelectGoodsInList()
{
	UINT32 nCount = m_RefGoodsList.size();

	//I3TRACE( "## new hot count : %d\n", nCount);

	if( nCount == 0 ) 
	{//new & hot 상품이 없음.
		_SetBlankGoods();
		m_nCurGoodsIndex = -1;
		return -1;
	}			

	i3UIScene * pScene = GetScene( "PointBlankRe_Lobby_NewHot");

	LuaState* L = nullptr;
	L = UILua::CallLuaFunction( pScene, "set_nah_goods");
	UILua::EndLuaFunction(L , 0);

	INT32 prevGoodsIndex = m_nCurGoodsIndex;

	//m_nCurGoodsIndex = _SelectIndexByRandom( m_nCurGoodsIndex, nCount);
	m_nCurGoodsIndex = _SelectIndexBySequence( m_nCurGoodsIndex, nCount);

	//1. current goods
	const SHOP_GOODS_PACK* pgoods_pack = m_RefGoodsList[m_nCurGoodsIndex];
	I3ASSERT( pgoods_pack != nullptr);
	I3ASSERT( m_goods_img_ctrl != nullptr);
	if(m_goods_img_ctrl == nullptr)
		return 0;
	
	SHOP_GOODS* pgoods = pgoods_pack->GetGoods( 0);
	if( pgoods == nullptr )
		return -1;

	// GD팀 요청으로 배너에 출력중인 상품 클릭 시 상점의 해당 탭으로 이동되어야함 - sanghun.han (2014. 12. 23) 
	SetGoodsType(pgoods);

	GetItemImageMgr()->SelectImage( m_goods_img_ctrl, pgoods);
	
	//2. prev goods
	if( prevGoodsIndex == -1 )
	{//prev goods 가 없으면 animation을 안함.
		m_prev_img_ctrl->SetEnable(false);
		return m_nCurGoodsIndex;
	}
	pgoods_pack = m_RefGoodsList[prevGoodsIndex];
	I3ASSERT( pgoods_pack != nullptr);
	pgoods = pgoods_pack->GetGoods( 0);
	GetItemImageMgr()->SelectImage( m_prev_img_ctrl, pgoods);

	_StartAni( m_nCurGoodsIndex, prevGoodsIndex);

	return m_nCurGoodsIndex;
}

void UINewAndHotBanner::ChangeItemView(bool bNextItem)
{
	UINT32 nCount = m_RefGoodsList.size();
	
	if(nCount == 0)
		return;

	if(bNextItem)
	{
		m_nCurGoodsIndex ++;
		
		if((UINT32)m_nCurGoodsIndex >= nCount)
			m_nCurGoodsIndex = 0;
	}
	else
	{
		m_nCurGoodsIndex --;

		if(m_nCurGoodsIndex < 0)
			m_nCurGoodsIndex = nCount - 1;
	}

	const SHOP_GOODS_PACK* pgoods_pack = m_RefGoodsList[m_nCurGoodsIndex];
	I3ASSERT( pgoods_pack != nullptr);
	SHOP_GOODS* pgoods = pgoods_pack->GetGoods( 0);

	SetGoodsType(pgoods);
	GetItemImageMgr()->SelectImage( m_goods_img_ctrl, pgoods);

	_EndAni();
	m_fAniTime = 0.0f;
	m_fChangeTime = 0.0f;
}

void UINewAndHotBanner::SetGoodsType(SHOP_GOODS* pGoods)
{
	if (pGoods == nullptr)
		return;

	switch (item_def::get_category(pGoods->_ItemID))
	{
	case item_def::CTG_WEAPON:
		m_nCurGoodsType = 0;
		break;
	case item_def::CTG_ITEM:
		m_nCurGoodsType = 4;
		break;
	case item_def::CTG_CHARA:
		m_nCurGoodsType = 1;
		break;
	case item_def::CTG_PARTS:
		m_nCurGoodsType = 2;
		break;
	case item_def::CTG_SKIN:
		m_nCurGoodsType = 3;
		break;
	default:
		m_nCurGoodsType = 5;
		break;
	}
}