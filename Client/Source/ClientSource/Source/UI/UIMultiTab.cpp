#include "pch.h"
#include "UIMultiTab.h"

#include "UIMultiTabSlot.h"
#include "UIClanFunction.h"
#include "ItemImageMgr.h"
#include "UIUtil.h"
#include "UIMainFrame.h"
#include "UISideMenu.h"
#include "UICharaInfoMgr.h"
#include "GameGUI.h"
#include "UILBScrollSlot.h"
#include "UISMenu_Ability.h"
#include "UserInfoContext.h"

#include "UIFloatCharacterView.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/RemoveColorString.h"

#include "MedalManager.h"

/********************************/
namespace
{
	enum	{
		FRM_WEAPON = item_def::CTG_WEAPON,		
		FRM_CHARA = item_def::CTG_CHARA,
		FRM_PARTS = item_def::CTG_PARTS,
		FRM_SKIN = item_def::CTG_SKIN,
		FRM_ITEM = item_def::CTG_ITEM,
		FRM_NEWHOT = item_def::CTG_NEWHOT,
		COMBO_UNFOLDER = -1,
		COMBO_FOLDER = -2,
	};
	iMultiTap* g_this = 0;

	i3UIComboBox* g_combo_box = 0;
	UI::tmHover g_tmHover;
	enum	{	HOVER_BUTTON, HOVER_EDIT_BOX, HOVER_LIST_BOX, };
	INT32 g_selected_tab_combo = 0;
	bool g_not_yet_hide_display_info = 0;	//popup 이 활성화가 되면 lua 호출이 무의미 해져서 만듬..
	size_t current_menu_idx = 0;
}

/********************************/
extern "C" 
{
	int itap_menu( LuaState * L)
	{
		INT32 value = i3Lua::GetIntegerArg( L, 1);
		g_this->clicked_menu(value);

		if(FRM_WEAPON <= value && value <= FRM_NEWHOT)
			g_this->clicked_tap(value, -1);

		return 0;
	}
	int itap_tab1( LuaState * L)
	{
		INT32 idx1 = i3Lua::GetIntegerArg( L, 1);
		INT32 idx2 = i3Lua::GetIntegerArg( L, 2);
		g_this->clicked_tap(idx1, idx2);
		return 0;
	}
	int itap_tab2( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		g_this->clicked_tap_combo(idx);
		return 0;
	}
	int itap_ClickSort(LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1);
		g_this->clicked_Sort(idx);
		return 0;
	}
	int shop_CancelSearch(LuaState * L)
	{
		g_this->clicked_CancelSearch();
		return 0;
	}
	int shop_OnLeave(LuaState * L)
	{
		g_this->OnEscapeKey();
		return 0;
	}
	int shop_ClickUseInven(LuaState * L)
	{
		g_this->clicked_UseInven();
		return 0;
	}
	int shop_ClickBuyType(LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1);
		g_this->clicked_BuyType(idx);
		return 0;
	}
	int shop_ClickBuySlot(LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1);
		g_this->clicked_BuySlot(idx);
		return 0;
	}
}

/********************************/
LuaFuncReg itap_menu_glue[] =
{
	{ "Click",			itap_menu },
	{ "ClickSub",		itap_tab1 },
	{ "ClickCombo",		itap_tab2 },
	{ "ClickSort",		itap_ClickSort },
	{ "CancelSearch",	shop_CancelSearch },
	{ "OnLeave",		shop_OnLeave },
	{ "ClickUseInven",	shop_ClickUseInven },
	{ "ClickBuyType",	shop_ClickBuyType },
	{ "ClickBuySlot",	shop_ClickBuySlot },
	{ nullptr, nullptr}
};

LuaFuncReg itap_tap_glue[] =
{
	{ "Click", itap_tab1	},
	{ "ClickCombo", itap_tab2	},
	{ nullptr, nullptr}
};

/********************************/
void iMultiTap::SetSortCombo(INT32 idx)
{
	ucf::ComboString str_combo;
	switch (idx)
	{
	case item_def::CTG_NEWHOT:
	{
		str_combo << GAME_RCSTRING("STR_SHOP_FILTER_DEFAULT")
			<< GAME_RCSTRING("STR_SHOP_FILTER_NEW")
			<< GAME_RCSTRING("STR_SHOP_FILTER_HOT");
		const i3::wstring cs_wstr = str_combo;
		m_CurrentSortType = SHOP_SORT_CATEGORY_NEWHOT;
		m_SortComboSlot = 0;
	}
	break;
	case item_def::CTG_WEAPON:
	{
		str_combo << GAME_RCSTRING("STR_SHOP_FILTER_DEFAULT")
			<< GAME_RCSTRING("STR_SHOP_FILTER_NEW")
			<< GAME_RCSTRING("STR_SHOP_FILTER_NAME")
			<< GAME_RCSTRING("STR_SHOP_FILTER_DAMAGE")
			<< GAME_RCSTRING("STR_SHOP_FILTER_RAPIDFIRE")
			<< GAME_RCSTRING("STR_SHOP_FILTER_SPEED")
			<< GAME_RCSTRING("STR_SHOP_FILTER_POINT")
			<< GAME_RCSTRING("STR_SHOP_FILTER_CASH");
		//<< L";";
		const i3::wstring cs_wstr = str_combo;
		m_CurrentSortType = SHOP_SORT_CATEGORY_WEAPON;
		m_SortComboSlot = 0;
	}

	break;
	case item_def::CTG_CHARA:
	case item_def::CTG_PARTS:
	case item_def::CTG_SKIN:
	case item_def::CTG_ITEM:
	{
		str_combo << GAME_RCSTRING("STR_SHOP_FILTER_DEFAULT")
			<< GAME_RCSTRING("STR_SHOP_FILTER_NEW")
			<< GAME_RCSTRING("STR_SHOP_FILTER_NAME")
			<< GAME_RCSTRING("STR_SHOP_FILTER_POINT")
			<< GAME_RCSTRING("STR_SHOP_FILTER_CASH");
		//			<< L";";

		m_CurrentSortType = SHOP_SORT_CATEGORY_ITEM;
		m_SortComboSlot = 0;
		break;
	}
	default:
		break;
	}

	const i3::wstring cs_wstr = str_combo;
	LuaState * L = UILua::CallLuaFunction(m_menu_scn_info.scn, "SetSortType");
	i3Lua::PushStringUTF16To8(L, cs_wstr.c_str(), cs_wstr.size());
	UILua::EndLuaFunction(L, 1);

}
void iMultiTap::clicked_menu(INT32 idx) 
{
	switch( idx )
	{
	case FRM_WEAPON:	case FRM_CHARA:	case FRM_PARTS:	case FRM_SKIN: case FRM_ITEM: case FRM_NEWHOT:
		open_menu( idx );
		current_menu_idx = idx;
		break;
	}

	clicked_menu_impl(idx);
}

void iMultiTap::clicked_tap(INT32 idx1, INT32 idx2)
{
	LuaState * L = UILua::CallLuaFunction(m_menu_scn_info.scn, "SetInfoTap");
	i3Lua::PushInteger(L, -1);
	UILua::EndLuaFunction(L, 1);

	switch( idx1 )
	{
	case COMBO_UNFOLDER:	
		m_pSlot->SetSlotSkip(true);
		if( g_tmHover.get_stayed_hover_idx() != HOVER_BUTTON )
		{
			g_combo_box->FoldListBox();
			clicked_tap_combo(g_selected_tab_combo);
			m_BtnCombo->getEditBox()->OnActivate(false);
			m_BtnCombo->getEditBox()->setModifyState();
		}
		break;
	case COMBO_FOLDER:	
		m_pSlot->SetSlotSkip(false);
		g_combo_box->SetComboBoxTextColor( GameGUI::GetColor(GCT_VALUE) );
		break;
	default:
		g_combo_box->RestoreTextColor();
		m_pSlot->Entrance((item_def::Category)(UINT32)idx1, (UINT32)idx2);
		clicked_CancelSearch();		//순서중요 entrace후에 호출해야 합니다. 
		hide_display_info();
		clicked_tap_impl(idx1, idx2);
		m_BtnCombo->getEditBox()->OnActivate(true);
		m_BtnCombo->getEditBox()->setModifyState();
	}
}

void iMultiTap::clicked_tap_combo(INT32 idx)
{
	g_selected_tab_combo = idx;
	m_pSlot->Entrance((item_def::WeaponComboType)(UINT32)idx);
	clicked_CancelSearch();		//순서중요 entrace후에 호출해야 합니다. 
	hide_display_info();
	clicked_tap_combo_impl(idx);
}

void iMultiTap::open_menu(INT32 idx) const
{
	LuaState * L = UILua::CallLuaFunction( m_menu_scn_info.scn, "EnableCompare");
	i3Lua::PushBoolean( L, false );
	UILua::EndLuaFunction(L , 1);

	//if( ( idx == FRM_ITEM ) 
	//	|| (idx == FRM_NEWHOT))
	//	idx = FRM_PARTS;

	L = UILua::CallLuaFunction( m_menu_scn_info.scn, "SetInfoTap");
	i3Lua::PushInteger( L, -1);
	UILua::EndLuaFunction(L , 1);
}

void iMultiTap::hide_display_info()
{
	if( m_slot_scn_info.scn->isEnable() == true )
		m_slot_scn_info.scn->EnableScene(false);

	m_pSlot->update_btn_state_at_first(); //menu 버튼 상태 변경
	m_pSlot->update_empty_item_attribute(); //menu 속성 변경

	LuaState * L = _CallLuaFunction(m_menu_scn_info.idx, "DisplayInputDisable");
	i3Lua::PushBoolean(L, true);
	UILua::EndLuaFunction(L , 1);

	g_not_yet_hide_display_info = GetMainFrame()->IsPopupFocused();	//popup 활성화 상태이면 다시 호출해야 된다.
}

void iMultiTap::show_display_info()
{
	if (m_slot_scn_info.scn->isEnable() == false)
		m_slot_scn_info.scn->EnableScene(true);

	LuaState * L = _CallLuaFunction(m_menu_scn_info.idx, "DisplayInputDisable");
	i3Lua::PushBoolean(L, false);
	UILua::EndLuaFunction(L , 1);
}

size_t iMultiTap::get_current_menu_idx() const
{
	return current_menu_idx;
}

void iMultiTap::set_current_menu_idx(size_t value)
{
	current_menu_idx = value;
}

/********************************/
void iMultiTap::init_slide()
{
	//UI::tmSlideAttContainer list1(m_menu_scn_info.scn);
	//list1.add("Fmw");
	//list1.add("i3UIFrameWnd1");
	//list1.add("i3UIFrameWnd3");
	//list1.add("i3UIFrameWnd11");
	//list1.add("i3UIFWnd_Weapon");
	//list1.add("i3UIFWnd_Chara");
	//list1.add("i3UIFWnd_Parts");
	//list1.add("i3UIFWnd_skin");
	//list1.add("i3UIFWnd_Item");
	//list1.set_target_by_offset(m_slide_offset_x, m_slide_offset_y);
	//m_tmSlide.add(list1);

	//UI::tmSlideAttContainer list2(m_tap_scn_info.scn);
	//list2.add("i3UIFWnd_Weapon");
	//list2.add("i3UIFWnd_Chara");
	//list2.add("i3UIFWnd_Parts");
	//list2.add("i3UIFWnd_skin");
	//list2.add("i3UIFWnd_Item");
	//list2.set_target_by_offset(m_slide_offset_x, m_slide_offset_y);
	//m_tmSlide.add(list2);

	//UI::tmSlideAttContainer list3(m_slot_scn_info.scn);
	//list3.add("i3UIFrameWnd0");
	//list3.set_target_by_offset(m_slide_offset_x, m_slide_offset_y);
	//m_tmSlide.add(list3);

	//init_slide_impl();
}


void iMultiTap::init_scene()
{
	LuaState * L = 0;

	m_menu_scn_info.scn= GetScene( m_menu_scn_info.idx );
	L = _CallLuaFunction(m_menu_scn_info.idx, "LoadAllScenes");
	UILua::EndLuaFunction(L , 0);

	L = UILua::CallLuaFunction(m_menu_scn_info.scn, "EnableCompare");
	i3Lua::PushBoolean(L, false);
	UILua::EndLuaFunction(L, 1);

	g_combo_box = (i3UIComboBox*)m_menu_scn_info.scn->FindChildByName("i3UIComboBox");
	g_combo_box->BackupTextColor();

	g_tmHover.add(g_combo_box->getButtonCtrl());
	g_tmHover.add(g_combo_box->getEditBox());
	g_tmHover.add(g_combo_box->getListBox());

	//m_tap_scn_info.scn = GetScene( m_tap_scn_info.idx );
	//L = _CallLuaFunction(m_tap_scn_info.idx, "LoadAllScenes");
	//UILua::EndLuaFunction(L , 0);
	
	//g_combo_box = (i3UIComboBox*)m_tap_scn_info.scn->FindChildByName("i3UIComboBox");
	//g_combo_box->BackupTextColor();

	m_slot_scn_info.scn = GetScene( m_slot_scn_info.idx );
	L = _CallLuaFunction(m_slot_scn_info.idx, "LoadAllScenes");
	UILua::EndLuaFunction(L , 0);

	//main frame slot
	VEC3D pos = *m_SlotFrm[1]->getPos();
	i3UIControl* item_ctrl = static_cast<i3UIControl*>(m_slot_scn_info.scn->getFirstChild());
	item_ctrl->setPos(&pos);
	//item_ctrl->setPos(191.f, 623.f);


	m_ppImgSet = GetItemImageMgr()->CreateImgSetCtrl(m_slot_scn_info.scn, "i3UIButtonImageSet");
}

void iMultiTap::init_combo()
{
	ucf::ComboString cs;

	cs	<< GAME_RCSTRING("STBL_IDX_SHOP_FILTER_PRIMARY")
		<< GAME_RCSTRING("STR_SHOP_MENU_WEAPON_ASSAULT")
		<< GAME_RCSTRING("STR_SHOP_MENU_WEAPON_SMG")
		<< GAME_RCSTRING("STR_SHOP_MENU_WEAPON_SNIPER")
		<< GAME_RCSTRING("STR_SHOP_MENU_WEAPON_MACHINE")
		<< GAME_RCSTRING("STR_SHOP_MENU_WEAPON_SHOTGUN");

	//LuaState * L = UILua::CallLuaFunction( m_menu_scn_info.scn, "SetFilterList");
	LuaState * L = UILua::CallLuaFunction(m_menu_scn_info.scn, "SetFilterList");

	const i3::wstring cs_wstr = cs;
	i3Lua::PushStringUTF16To8(L, cs_wstr.c_str(), cs_wstr.size());
	UILua::EndLuaFunction(L , 1);

	g_selected_tab_combo = 0;
}

void iMultiTap::add_slide(UI::tmSlideAtt* att)
{	
	m_tmSlide.add(att);	
	att->set_target_by_offset(m_slide_offset_x, m_slide_offset_y);	
}

/********************************/
iMultiTap::iMultiTap() : m_pCharaInfoMgr(0), m_phase_type(USP_NONE)
{
	m_pSlot = nullptr;
	m_ppImgSet = nullptr;

	m_CurrentSortType = SHOP_SORT_CATEGORY_NEWHOT;
	m_SortComboSlot = 0;

	m_SortData[SHOP_SORT_CATEGORY_NEWHOT].push_back(SHOP_SORT_TYPE_NORMAL);
	m_SortData[SHOP_SORT_CATEGORY_NEWHOT].push_back(SHOP_SORT_TYPE_NEW);
	m_SortData[SHOP_SORT_CATEGORY_NEWHOT].push_back(SHOP_SORT_TYPE_HOT);

	m_SortData[SHOP_SORT_CATEGORY_WEAPON].push_back(SHOP_SORT_TYPE_NORMAL);
	m_SortData[SHOP_SORT_CATEGORY_WEAPON].push_back(SHOP_SORT_TYPE_NEW);
	m_SortData[SHOP_SORT_CATEGORY_WEAPON].push_back(SHOP_SORT_TYPE_NAME);
	m_SortData[SHOP_SORT_CATEGORY_WEAPON].push_back(SHOP_SORT_TYPE_DAMAGE);
	m_SortData[SHOP_SORT_CATEGORY_WEAPON].push_back(SHOP_SORT_TYPE_RAPID);
	m_SortData[SHOP_SORT_CATEGORY_WEAPON].push_back(SHOP_SORT_TYPE_SPEED);
	m_SortData[SHOP_SORT_CATEGORY_WEAPON].push_back(SHOP_SORT_TYPE_POINT);
	m_SortData[SHOP_SORT_CATEGORY_WEAPON].push_back(SHOP_SORT_TYPE_CASH);

	m_SortData[SHOP_SORT_CATEGORY_ITEM].push_back(SHOP_SORT_TYPE_NORMAL);
	m_SortData[SHOP_SORT_CATEGORY_ITEM].push_back(SHOP_SORT_TYPE_NEW);
	m_SortData[SHOP_SORT_CATEGORY_ITEM].push_back(SHOP_SORT_TYPE_NAME);
	m_SortData[SHOP_SORT_CATEGORY_ITEM].push_back(SHOP_SORT_TYPE_POINT);
	m_SortData[SHOP_SORT_CATEGORY_ITEM].push_back(SHOP_SORT_TYPE_CASH);
}

iMultiTap::~iMultiTap()
{
	I3_SAFE_RELEASE(m_ppImgSet);
}


/********************************/
void iMultiTap::OnCreate( i3GameNode * pParent)
{
	UIPhaseSubBase::OnCreate( pParent);

	m_menu_scn_info.idx = GetSceneCount();
	AddScene("Scene/Main/PointBlankRe_Inven_Menu_plus_finder.i3UIs", itap_menu_glue, true, false);

	//m_tap_scn_info.idx = GetSceneCount();
	//AddScene("Scene/Main/PointBlankRe_Inven_SubMenu.i3UIs", itap_tap_glue, true, false);

	m_slot_scn_info.idx = GetSceneCount();
	AddScene(slot_scene_name.c_str(), nullptr, true, false);

	m_pSlot->OnCreate( pParent );
}

void iMultiTap::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPhaseSubBase::_InitializeAtLoad( pScene);
	if( i3::generic_is_iequal( pScene->GetName(), "PointBlankRe_Inven_Menu_plus_finder") )
	{
		m_BtnCombo	= (i3UIButtonComboBox*) pScene->FindChildByName( "i3UIComboBox" );
		I3ASSERT( m_BtnCombo);

		m_SearchEditBox = (i3UIEditBox*)pScene->FindChildByName("i3UIEditBox1");
		I3ASSERT(m_SearchEditBox);
		m_SearchFrm = (i3UIFrameWnd*)pScene->FindChildByName("i3UIFrameWnd2");
		I3ASSERT(m_SearchFrm);
		m_SlotFrm[0] = (i3UIFrameWnd*)pScene->FindChildByName("i3UIFrameWndSlot");
		I3ASSERT(m_SearchFrm);
		m_SlotFrm[1] = (i3UIFrameWnd*)pScene->FindChildByName("i3UIFrameWndSlot2");
		I3ASSERT(m_SearchFrm);
		
	}

}


bool iMultiTap::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	SetEnable( true );
	CMedalManager::i()->MakeViewList(TAB_BASIC);
	return true;
}

void iMultiTap::OnEntranceEnd()
{
	UIPhaseSubBase::OnEntranceEnd();

	m_SortComboSlot = 0;
	m_menu_scn_info.scn->Attach(false);
	m_slot_scn_info.scn->Attach(false);
	m_pSlot->MoveBackScene();
	m_pSlot->Entrance();
	//m_tap_scn_info.scn->Attach( false );

	entrance_end_impl();
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_REFRESH_CASH);
}

/*virtual*/ void iMultiTap::OnLoadAllScenes()
{
	UIPhaseSubBase::OnLoadAllScenes();
	
	g_this = this;

	m_tmSlide.clear();

	init_scene();
	init_slide();
	init_combo();

	m_pSlot->Initialize();
	m_pSlot->OnLoadAllScenes();
	m_pSlot->set_display_item(m_ppImgSet, m_slot_scn_info.scn, m_menu_scn_info.scn);

	m_SortComboBox = (i3UIComboBox*)m_menu_scn_info.scn->FindChildByName("i3UIComboBox_NoEditBox");

	m_tmSlide.move_target();

	m_pCharaInfoMgr = CharaInfoMgr::instance();

	if(UISideMenu::instance()
		&&UISideMenu::instance()->is_entranced() == false )
	{
		m_pCharaInfoMgr->list_up();
	}

	m_CurrentSortType = SHOP_SORT_CATEGORY_NEWHOT;
	m_SortComboSlot = 0;
}

bool iMultiTap::OnExitStart(void)
{
	m_ppImgSet->RestoreSizePos();
	I3_SAFE_RELEASE(m_ppImgSet);

	SetEnable( false );
	
	SMenuAbility* sidemenu = static_cast<SMenuAbility*>(UISideMenu::instance()->getSubSidemenu(SIDEMENU_BTN_ABILITY));
	sidemenu->EnableMainInfo(false);
	UISideMenu::instance()->get_frm_wnd(SIDEMENU_BTN_ABILITY)->EnableCtrl(false);
	m_pSlot->ResetList();

	return true;
}

void	iMultiTap::OnUnloadAllScenes()
{
	m_pSlot->OnUnloadAllScenes();

	m_tmSlide.move_origin();
	m_tmSlide.clear();

	g_tmHover.clear();

	current_menu_idx = (size_t)-1;

	UIPhaseSubBase::OnUnloadAllScenes();
}

bool iMultiTap::OnRevive(i3RenderContext * pCtx)
{
	if (pCtx && UIPhaseSubBase::OnRevive(pCtx) == false) //OnLoadAllScenes에서 OnRevive 이 함수를 호출할때 매개인자로 0을 넣는다. 
		return false;

	if (isEnable() == false) return false;

	m_pSlot->OnLoadAllScenes();
	VEC3D pos = *m_SlotFrm[1]->getPos();
	i3UIControl* item_ctrl = static_cast<i3UIControl*>(m_slot_scn_info.scn->getFirstChild());
	item_ctrl->setPos(&pos);

	return true;
}


/*virtual*/ bool iMultiTap::OnKeyInput(i3InputDeviceManager * pInputMng)
{
	UIPhaseSubBase::OnKeyInput(pInputMng);

	i3InputKeyboard* pKeyboard = pInputMng->GetKeyboard();
	i3UIControl * pCurCtrl = reinterpret_cast<i3UIControl*>(g_pFramework->getFocus());
	if (pCurCtrl == m_SearchEditBox)
	{
		i3::wstring wstr = (reinterpret_cast<i3UIEditBox*>(m_SearchEditBox))->getText();
		i3::stack_wstring result;
		::RemoveColorString(wstr, result);

		if (pKeyboard->GetStrokeState(I3I_KEY_ENTER) == true)
			m_pSlot->SearchKeyword(result.c_str(), true);
		else
			m_pSlot->SearchKeyword(result.c_str(), false);
		return true;
	}


	return false;
}

void iMultiTap::ProcessGameEvent(GAME_EVENT event, INT32 Arg, const i3::user_data& UserData)
{
#if !defined(LOCALE_RUSSIA)
	if (event == EVENT_REFRESH_CASH)
	{
		i3::rc_wstring wstr_point;
		i3::rc_wstring wstr_cash;

		i3::sprintf(wstr_point, L"%d %s", UserInfoContext::i()->GetMyPoint(), GAME_RCSTRING("STBL_IDX_SHOP_MONEY"));
		i3::sprintf(wstr_cash, L"%d %s", UserInfoContext::i()->GetMyCash(), GAME_RCSTRING("STR_SHOP_CASH"));

		LuaState * L = UILua::CallLuaFunction(m_menu_scn_info.scn, "SetTextCash");
		i3Lua::PushStringUTF16To8(L, wstr_point);
		i3Lua::PushStringUTF16To8(L, wstr_cash);
		UILua::EndLuaFunction(L, 2);
	}
#endif
}

void iMultiTap::OnUpdate( REAL32 rDeltaSeconds)
{
	m_tmSlide.OnUpdate(rDeltaSeconds);
	m_pSlot->OnUpdate(rDeltaSeconds);

	if( g_not_yet_hide_display_info )
	{
		if( GetMainFrame()->IsPopupFocused() == false )	//popup 이 비활성화 되면 다시 호출한다.
			hide_display_info();
	}

	update_impl(rDeltaSeconds);
}

bool iMultiTap::OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code)
{
	m_pSlot->OnEvent( event, pObj, param2, code );

	return UIPhaseSubBase::OnEvent( event, pObj, param2, code);
}

/*virtual*/ bool iMultiTap::OnPlayClickSound(i3UIControl * pCtrl)
{
	if( i3::same_of<i3UIEditBox*>(pCtrl) && i3::same_of<i3UIComboBox*>(pCtrl->getParent()) )
	{
		return PlaySound(UISND_ITEM_SELECTING);
	}
	else if  (i3::same_of<i3UIFrameWnd*>(pCtrl))
	{
		i3UIScene * pScene = i3::same_cast<i3UIScene*>(pCtrl->getParent());

		if(pScene != nullptr && i3::contain_string(slot_scene_name, pScene->GetNameString()) >= 0)
			return PlaySound(UISND_ITEM_SELECTING);
	}

	return UIPhaseSubBase::OnPlayClickSound(pCtrl);
}

void iMultiTap::SelectedEnteranceTab(INT32 idx)
{
	LuaState * L = _CallLuaFunction(m_menu_scn_info.idx, "TabEnable");
	i3Lua::PushInteger(L, idx);
	UILua::EndLuaFunction(L , 1);
}

void iMultiTap::clicked_CancelSearch()
{
	m_SearchEditBox->SetText(L"");
	m_pSlot->SearchKeyword(L"", false);
}

void iMultiTap::clicked_Sort(INT32 idx)
{
	m_SortComboSlot = idx;
	m_pSlot->SortType((item_def::Category)get_current_menu_idx(), m_SortData[m_CurrentSortType].at(m_SortComboSlot));
}