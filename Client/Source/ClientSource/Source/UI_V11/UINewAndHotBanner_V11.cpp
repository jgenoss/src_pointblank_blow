#include "pch.h"
#include "UINewAndHotBanner_V11.h"

#include "UI/UIMainFrame.h"
#include "UITopMenu_V11.h"
//#include "UIShopFunction.h"
#include "UI/ItemImageMgr.h"

#include "Shop.h"
#include "ShopContext.h"

I3_CLASS_INSTANCE( UINewAndHotBanner_V11);

UINewAndHotBanner_V11::UINewAndHotBanner_V11()
:m_nCurGoodsIndex(-1), m_fChangeTime(0.f), m_fAniTime(0.f), m_bOnAni(false),
m_goods_img_ctrl(nullptr), m_prev_img_ctrl(nullptr), m_bMakeGoodsList(false), m_bNewAndHotClicked(false)
{
	m_AlignedPos = UIFLOATING_POS_LEFT;

#if defined( I3_DEBUG)
	SetName( "NewAndHotBanner");
#endif
}

UINewAndHotBanner_V11::~UINewAndHotBanner_V11()
{
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
static UINewAndHotBanner_V11* GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

	if( pFrame != nullptr)
	{
		return (UINewAndHotBanner_V11*) pFrame->GetFloating( UFW_NEWANDHOT);
	}
	return nullptr;
}

extern "C" {
	int			UINewHot_ClickGotoShop_V11( LuaState * L)
	{
		UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

		if( pFrame != nullptr)
		{
			UITopMenu* pTopMenu = pFrame->GetTopMenu();
			pTopMenu->GotoShopFromNewAndHot();				// 상점이 들어가기 전까지 막습니다. 김대영 패스
		}

		GetThis()->SetNewAndHotClicked( true);
		I3TRACE( "## Go to Shop From Lobby New Hot Banner \n");

		return 0;
	}
}

LuaFuncReg UINewHotBanner_Glue_V11[] = 
{
	{"ClickGotoShop",				UINewHot_ClickGotoShop_V11},
	{ nullptr,							nullptr}
};
//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//
/*virtual*/ void UINewAndHotBanner_V11::OnCreate( i3GameNode * pParent)
{
	UIFloatingBase::OnCreate( pParent);
	// Load Scene

	AddScene( "Scene/Popup/PointBlankRe_Lobby_NewHot.i3UIs", UINewHotBanner_Glue_V11);

	m_RefGoodsList.clear();
}

/*virtual*/ bool UINewAndHotBanner_V11::OnEntranceStart( void* pArg1 /*= 0*/, void* pArg2 /*= 0*/)
{
	if( UIFloatingBase::OnEntranceStart( pArg1, pArg2) == false )
		return false;
	
	_InitVariables();
	SetNewAndHotClicked( false);

	MakeNewAndHotGoodsList();
	SelectGoodsInList();

	return true;
}

/*virtual*/ void UINewAndHotBanner_V11::OnExitEnd()
{
	UIBase::OnExitEnd();
}

/*virtual*/void	UINewAndHotBanner_V11::OnLoadAllScenes()
{
	UIFloatingBase::OnLoadAllScenes();
	m_slide.AddFrameWnd(GetFirstFrameWnd(), UISlide::L_TO_R);

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

	//  [3/7/2012 dosun.lee]
	// 위치 이동을 소스로 해줬었는데 리소스 자체의 위치를 맞춰주는걸로 변경함
	/*VEC2D vPos = m_goods_img_ctrl->GetPos();
	m_prev_img_ctrl->SetPos( vPos);

	L = UILua::CallLuaFunction( pScene, "get_none_text_ctrl");
	i3UIStaticText* st = reinterpret_cast< i3UIStaticText* >(UILua::ReturnLuaPointer(L, 0));
	VEC3D v3Pos;
	i3Vector::Copy( &v3Pos, st->getPos());
	v3Pos.x = vPos.x;
	v3Pos.y = vPos.y;
	st->setPos( &v3Pos);*/
}

/*virtual*/void	UINewAndHotBanner_V11::OnUnloadAllScenes()
{
	i3::safe_destroy_instance( m_goods_img_ctrl);
	i3::safe_destroy_instance( m_prev_img_ctrl);

	_InitVariables();
}

/*virtual*/ void UINewAndHotBanner_V11::OnUpdate( REAL32 rDeltaSeconds)
{
	UIFloatingBase::OnUpdate( rDeltaSeconds);

	//UIFloatingBase::OnUpdate( rDeltaSeconds);
	//시점에서 OnUnloadAllScenes 코드가 이루어 지기때문에 예외처리..
	if( !m_goods_img_ctrl || !m_prev_img_ctrl )
	{
		return;
	}
	//로비 일때만
	for( int sptype = 0; sptype < USP_MAX; ++sptype )
	{
		if( GetMainFrame()->GetCurrentSubPhaseType() == (UISUBPHASE)sptype ) return;
	}
	if( GetMainFrame()->GetCurrentPhaseType() != UIPHASE_LOBBY ) return;

	// 데이터가 없을 경우 새롭게 생성 합니다.
	if( false == m_bMakeGoodsList )
	{
		MakeNewAndHotGoodsList();
		SelectGoodsInList();
	}

	if( m_nCurGoodsIndex != -1)
		m_fChangeTime += rDeltaSeconds;

	if( m_fChangeTime >= (REAL32)GOODS_CHANGE_TIME )
	{//5초 마다 바꿉니다.
		SelectGoodsInList();
	}

	_AniUpdate( rDeltaSeconds);
}


void	UINewAndHotBanner_V11::_InitVariables( void)
{
	m_nCurGoodsIndex = -1;
	m_fChangeTime = 0.f;
	m_fAniTime = 0.f;
	m_bOnAni = false;

	SetMakeList( false);
}

void	UINewAndHotBanner_V11::_SetBlankGoods( void)
{
	i3UIScene * pScene = GetScene( "PointBlankRe_Lobby_NewHot");

	LuaState* L = nullptr;
	L = UILua::CallLuaFunction( pScene, "set_blank");
	UILua::EndLuaFunction(L , 0);
}

//UITapShopNewAndHot을 참고함.
void	UINewAndHotBanner_V11::_MakeGroupList_ShopGoodsType( i3::vector<const SHOP_GOODS_PACK*>& refGoodsList, SHOP_GOODS_TYPE t)
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

void	UINewAndHotBanner_V11::_StartAni( INT32 cur, INT32 prev )
{
	m_bOnAni = true;

	m_goods_img_ctrl->SetAlpha( 0.f);
	
	m_prev_img_ctrl->SetAlpha( 1.f); 
	m_prev_img_ctrl->SetVisible( true);
}

void	UINewAndHotBanner_V11::_EndAni( void)
{
	m_bOnAni = false;

	m_goods_img_ctrl->SetAlpha( 1.f);

	m_prev_img_ctrl->SetAlpha( 0.f); 
	m_prev_img_ctrl->SetVisible( false);
}

void	UINewAndHotBanner_V11::_AniUpdate( REAL32 rDeltaSeconds)
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

INT32	UINewAndHotBanner_V11::_SelectIndexBySequence( INT32 nCurIndex, INT32 nRange)
{
	++nCurIndex;

	if( (nCurIndex < 0) || (nCurIndex >= nRange) ) return 0;

	return nCurIndex;
}

INT32	UINewAndHotBanner_V11::_SelectIndexByRandom( INT32 nCurIndex, INT32 nRange)
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

void	UINewAndHotBanner_V11::MakeNewAndHotGoodsList( void)
{
	CShop* pShop = CShop::i();
	if( false == pShop->IsLoadShop() ) return;

	CShopContext* pShopContext = CShopContext::i();
	pShopContext->BuildGoodsList();

	m_RefGoodsList.clear();
	for( int nType = SHOP_WEAPON_GOODS; nType < SHOP_PACKAGE_GOODS; ++nType )
	{
		_MakeGroupList_ShopGoodsType( m_RefGoodsList, (SHOP_GOODS_TYPE)nType);
	}

	SetMakeList( true);
}

INT32	UINewAndHotBanner_V11::SelectGoodsInList()
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
	SHOP_GOODS* pgoods = pgoods_pack->GetGoods( 0);

	I3ASSERT( m_goods_img_ctrl != nullptr);
	if( m_goods_img_ctrl == nullptr)
		return -1;

	GetItemImageMgr()->SelectImage( m_goods_img_ctrl, pgoods);
	
	
	//2. prev goods
	if( prevGoodsIndex == -1 )
	{//prev goods 가 없으면 animation을 안함.
		m_prev_img_ctrl->SetVisible( false);
		return m_nCurGoodsIndex;
	}
	pgoods_pack = m_RefGoodsList[prevGoodsIndex];
	I3ASSERT( pgoods_pack != nullptr);
	pgoods = pgoods_pack->GetGoods( 0);
	GetItemImageMgr()->SelectImage( m_prev_img_ctrl, pgoods);

	_StartAni( m_nCurGoodsIndex, prevGoodsIndex);

	m_fChangeTime = 0.f;

	return m_nCurGoodsIndex;
}