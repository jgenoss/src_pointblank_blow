#include "pch.h"

#if 0

#include "UISMenu_Chara.h"
#include "UISMenu_Equip.h"

#include "UISideMenu.h"
#include "ItemImageMgr.h"
#include "UIShopFunction.h"
#include "UI/UIUtil.h"

#include "UIMainFrame.h"
#include "UIFloatCharacterView.h"

#include "UISignal.h"

namespace
{
	SMenuSlotChara* g_this = 0;

	enum	{	ROW = 4, COL = 1, SLOT = ROW * COL,	};
	enum	{	UI_WEAPON = 0,	UI_PARTS,	UI_CHARA	};

	UINT32 g_main_chara_idx, g_category_type;
	bool is_chara_loading = false;
}


extern "C" 
{
	int smenu_chara( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		g_this->clicked(idx);
		return 0;
	}
}

LuaFuncReg sm_main_char_glue[] =
{
	{ "Click", smenu_chara	},
	{ nullptr, nullptr}
};


static void recModifyControlStyle( i3UIControl * pCtrl, I3UI_STYLE addStyle, I3UI_STYLE removeStyle)
{
	pCtrl->addStyle( addStyle);
	pCtrl->removeStyle( removeStyle);

	i3UIControl * pChild = static_cast<i3UIControl*>(pCtrl->getFirstChild());
	while( pChild != nullptr)
	{
		recModifyControlStyle( pChild, addStyle, removeStyle);
		pChild = static_cast<i3UIControl*>(pChild->getNext());
	}
}

void SMenuSlotChara::clicked(INT32 idx)
{
	INT32 slot_idx = iSlot::apply_select();
	if( slot_idx != UI::NON_SLOT_INDEX )
		g_main_chara_idx = slot_idx;

	switch( idx )
	{
	case UI_CHARA:
		idx = g_category_type;
	case UI_WEAPON: case UI_PARTS:
		{
			CharaInfoMgr::CurrentState & state = CharaInfoMgr::instance()->m_current_state;

			state.slot_idx = 0;
			state.ctg = (item_def::SideCategory)(UINT32)idx;
			state.chara_menu_ctg = (item_def::SideCategory)(UINT32)idx;

			int chara_idx = CCharaInfoContext::i()->GetMainCharaIdx();

		//	UI::call_cb( "smenu_equip_slot_clear");
			UISignal::SMenuEquipSlotClear();

			if( chara_idx != (int)g_main_chara_idx )
			{
				state.chara_idx = g_main_chara_idx;
				CCharaInfoContext::i()->SetMainCharaIdx(g_main_chara_idx);

				SetIgnoreInput( true);
				UIFloatCharacterView::instance()->SetChangeChara( state.chara_idx, false );
			}
			else
			{
				UISideMenu::instance()->smenu_click_equip( true);	//매개변수 true는 모든 내용 갱신
			}

			g_category_type = idx;

			//UI::call_cb("inven_frame_block");	-- 사용할 지 모르겠음... 클릭후 인벤 hitest가 말썽이 됨.
		}
		break;
	}
	
	mSlot.UpdateList( mList.size() );
}

void SMenuSlotChara::SetIgnoreInput( bool bIgnore)
{
	for( UINT32 i = 0; i < mList.size(); i++)
	{
		i3UIScene * pScene = mSlot.GetSlotScene( i);
		recModifyControlStyle( static_cast<i3UIControl*>(pScene->getFirstChild()), bIgnore?I3UI_STYLE_IGNOREINPUT:0, bIgnore?0:I3UI_STYLE_IGNOREINPUT);
	}

	is_chara_loading = bIgnore;
	mSlot.SetEnableMouseOver( !bIgnore);
}

void SMenuSlotChara::update_impl( REAL32 rDeltaSeconds)
{
	if( is_chara_loading == true )
	{
		if( UIFloatCharacterView::instance()->isLoadWaiting() == false )
		{
			SetIgnoreInput( false);
		}
	}
}

//------------------------------------------------------------------------------//
SMenuSlotChara::SMenuSlotChara(iSMenu* p) : iSMenuSlot(p)
{
	g_this = this;
	mSlot.EnableScroll( true );

	enum { X = 826, Y = 218, W = 180, H = 106};
	m_Pos.set(X, Y, W, H);
	m_Pos.set_dir(GameUI::APD_RIGHT, GameUI::APD_TOP);
	g_category_type = UI_WEAPON;

	is_chara_loading = true;
}

SMenuSlotChara::~SMenuSlotChara()
{
}


//------------------------------------------------------------------------------//
void SMenuSlotChara::create_impl( i3GameNode * pParent)
{
	//Slot 만들고
	mSlot.OnCreate((UIBase*)get_base(), ROW, COL);
	mSlot.RegisterSlotScene("Scene/Main/PointBlankRe_SideMenu_Chara.i3UIs", sm_main_char_glue);

	m_ppImgSet = new UIImgSetCtrl*[SLOT];
	MEMDUMP_NEW( m_ppImgSet, sizeof( UIImgSetCtrl*) * SLOT);

	::memset( m_ppImgSet, 0, sizeof(UIImgSetCtrl*) * SLOT);
}

void SMenuSlotChara::load_impl()
{
	VEC3D pos = m_Pos;
	mSlot.OnLoadAllScenes( (INT32)pos.x, (INT32)pos.y );
}

void SMenuSlotChara::update_position()
{
	VEC3D pos = m_Pos;
	mSlot.SetPosition( (INT32)pos.x, (INT32)pos.y );
	mSlot.SetScrollFocus();
}

/********************************//********************************/
void SMenuSlotChara::Entrance()
{
	is_chara_loading = true;

	mList.list_up();

	iSlot::reset_select( mList.size() );

	g_main_chara_idx = CCharaInfoContext::instance()->GetMainCharaIdx();

	mSlot.UpdateList( mList.size() );
}

void SMenuSlotChara::Exit()
{
	is_chara_loading = false;

	mSlot.SetEnable(false);

	g_category_type = UI_WEAPON;
	CharaInfoMgr::instance()->m_current_state.chara_menu_ctg = item_def::SCTG_WEAPON;
}

/********************************//********************************/
void SMenuSlotChara::cb_Load_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
	m_ppImgSet[slot_idx] = GetItemImageMgr()->CreateImgSetCtrl(slot_scn, "i3UIButtonImageSet");
}

void SMenuSlotChara::cb_Unload_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
	if( m_ppImgSet[slot_idx] )
		m_ppImgSet[slot_idx]->RestoreSizePos();
	I3_SAFE_RELEASE(m_ppImgSet[slot_idx]);
}

void	SMenuSlotChara::cb_Enabled_impl( INT32 slot_idx, i3UIScene* slot_scn)
{
	slot_scn->EnableScene(true);

	INT32 index = mSlot.GetStartInx() + slot_idx;

	const ITEM_INFO & multi_chara_info = mList[index];

	T_ItemID item_id = multi_chara_info.m_TItemID;
	GetItemImageMgr()->SelectImage_UseTemplate(m_ppImgSet[slot_idx], item_id, PBITEM_CHARAC_SIDE_MENU);

	i3::string strItemName;
	usf::GetShopItemMultiCharaName(strItemName, multi_chara_info);

	LuaState * L = UILua::CallLuaFunction( slot_scn, "InitSlot");
	i3Lua::PushA2UTF8String( L, strItemName.c_str());		// 서버 협업 후 변경 처리한다. (2014.07.23.수빈)
															// 현재 이 cpp 전체가 사용되지 않고 있어서 투두 제거..
	UILua::EndLuaFunction(L , 1);

	if(g_main_chara_idx == (UINT32)index)
	{
		L = UILua::CallLuaFunction( slot_scn, "Select");
		UILua::EndLuaFunction(L , 0);

		L = UILua::CallLuaFunction(slot_scn, "TabEnable");
		i3Lua::PushInteger( L, g_category_type);
		UILua::EndLuaFunction(L , 1);
	}
	else
	{
		L = UILua::CallLuaFunction( slot_scn, "Leave");
		UILua::EndLuaFunction(L , 0);

		L = UILua::CallLuaFunction(slot_scn, "TabClear");
		UILua::EndLuaFunction(L , 0);
	}
}

void SMenuSlotChara::cb_Enter_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
	LuaState* L = UILua::CallLuaFunction( slot_scn, "Enter");
	UILua::EndLuaFunction(L , 0);
}

void SMenuSlotChara::cb_Leave_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
	INT32 index = mSlot.GetStartInx() + slot_idx;

	LuaState* L = nullptr;
	if(g_main_chara_idx == (UINT32)index)
	{
		L = UILua::CallLuaFunction( slot_scn, "Select");
	}
	else
	{
		L = UILua::CallLuaFunction( slot_scn, "Leave");
	}

	UILua::EndLuaFunction(L , 0);
}
#endif