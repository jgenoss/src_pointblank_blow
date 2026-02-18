#include "pch.h"
#include "UISMenu_SlotBase.h"

#include "UISMenuBase.h"
#include "UISMenu_Equip.h"
#include "UISideMenu.h"

#include "UICharaInfoMgr.h"
#include "UIInvenInfoMgr.h"
#include "ItemImageMgr.h"
#include "UIShopFunction.h"
#include "UIUtil.h"
#include "UIMainFrame.h"
#include "Batmanager.h"
#include "UIPhaseReadyRoom.h"
#include "BattleSlotContext.h"

/************************//************************/
//iSMenuSlot
iSMenuSlot::iSMenuSlot(iSMenu* p) : mParent(p), m_pCharaInfoMgr(0), mCategory( item_def::SCTG_NA )
{
	m_pCharaInfoMgr = CharaInfoMgr::instance();
}

iSMenuSlot::~iSMenuSlot()
{
}

/************************/
UISideMenu* iSMenuSlot::get_base()
{
	return mParent->GetParent();
}

void iSMenuSlot::send_msg( SLOT_MSG msg, void * pArg /* = nullptr */)
{
	((SMenuEquip*)mParent)->Notify(msg, pArg);
}

void iSMenuSlot::adjust_position()
{
	INT32 x =0, y =0;
	UINT32 w =0,h =0;
	((SMenuEquip*)mParent)->GetFrmPos(x,y,w,h);

	mSlot.SetPosition(x, y, 0,  0);
}


/************************/
void iSMenuSlot::OnCreate( i3GameNode * pParent)
{
	create_impl(pParent);
}

void iSMenuSlot::OnLoadAllScenes()
{
	load_impl();

	mSlot.SetEnableMouseOver(true);
	mSlot.SetEnable(false);
}

void iSMenuSlot::OnUnloadAllScenes()
{
	mSlot.OnUnloadAllScenes();

	unload_impl();
}

void iSMenuSlot::OnUpdate( REAL32 rDeltaSeconds)
{
	mSlot.OnUpdate();
	update_impl(rDeltaSeconds);
}

void iSMenuSlot::OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code)
{
	mSlot.OnEvent(event, pObj, param2, code);
	event_impl(event, pObj, param2, code);
}

/************************/
void iSMenuSlot::Entrance()
{
}

void iSMenuSlot::Exit()
{
	mSlot.SetEnable(false);
}


/************************/
void iSMenuSlot::cb_Load_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
	m_ppImgSet[slot_idx] = GetItemImageMgr()->CreateImgSetCtrl(slot_scn, "i3UIButtonImageSet");
}

void iSMenuSlot::cb_Unload_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
	if( m_ppImgSet[slot_idx] )
		m_ppImgSet[slot_idx]->RestoreSizePos();
	I3_SAFE_RELEASE(m_ppImgSet[slot_idx]);
}

void iSMenuSlot::cb_Disabled_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
	slot_scn->EnableScene(false);
}
void iSMenuSlot::cb_Enter_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
	LuaState * L = UILua::CallLuaFunction( slot_scn, "Enter");
	UILua::EndLuaFunction(L , 0);
}
void iSMenuSlot::cb_Leave_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
	LuaState * L = UILua::CallLuaFunction( slot_scn,  "Leave");
	UILua::EndLuaFunction(L , 0);
}












/************************//************************/
//SMenuMainSlot
SMenuMainSlot::SMenuMainSlot(iSMenu* p) : iSMenuSlot(p)
{
	enum { X = 0, Y = 156, };
	m_Pos.set_pos(X, Y);
	m_Pos.set_dir(GameUI::APD_LEFT, GameUI::APD_TOP);
}

SMenuMainSlot::~SMenuMainSlot()
{
}

/************************/
void SMenuMainSlot::clicked()
{
	CharaInfoMgr::CurrentState & state = m_pCharaInfoMgr->m_current_state;

	INT32 appliedSelectedSlotIdx =	iSlot::apply_select();
	I3_BOUNDCHK(appliedSelectedSlotIdx, UINT8_MAX+1);

	state.slot_idx = static_cast<UINT8>(appliedSelectedSlotIdx);
	state.info = m_pCharaInfoMgr->find(state.chara_idx, state.ctg, state.slot_idx);

	mSlot.UpdateList( m_pCharaInfoMgr->size(state.chara_idx, state.ctg) );	
	send_msg( SLOT_MSG_CLOSE_SLOT_SUB );
	send_msg( SLOT_MSG_OPEN_SLOT_SUB);
}

void SMenuMainSlot::msg_update_item()
{
	CharaInfoMgr::CurrentState & state = m_pCharaInfoMgr->m_current_state;

	if(mCategory == state.ctg)
	{
		mSlot.UpdateList( m_pCharaInfoMgr->size(state.chara_idx, state.ctg) );
	}
}

void SMenuMainSlot::Notify( SLOT_MSG msg, void * pArg)
{
	if( msg == SLOT_MSG_UPDATE_ITEM)	msg_update_item();
}

/************************/
void SMenuMainSlot::Entrance()
{
	size_t size = m_pCharaInfoMgr->size(m_pCharaInfoMgr->m_current_state.chara_idx, mCategory);

	iSlot::reset_select( size );
	mSlot.UpdateList( size );

	adjust_position();
}

void SMenuMainSlot::load_impl()
{
	mSlot.OnLoadAllScenes( (INT32)m_Pos.get_pos().x, (INT32)m_Pos.get_pos().y );
}


/************************/
void	SMenuMainSlot::cb_Enabled_impl( INT32 slot_idx, i3UIScene* slot_scn)
{
	slot_scn->EnableScene(true);

	CharaInfoMgr::CurrentState & state = m_pCharaInfoMgr->m_current_state;

	INT32 index = mSlot.GetStartInx() + slot_idx;

	const ITEM_INFO & info = m_pCharaInfoMgr->find( state.chara_idx, state.ctg, index );

	GetItemImageMgr()->SelectImage(m_ppImgSet[slot_idx], info.m_TItemID);

	i3::rc_wstring wstrItemName;
	usf::GetShopItemName(wstrItemName, info.m_TItemID);

	i3::rc_wstring wstr_weapon_type_str; //주무기는 usage name, 그 외는 class name
	if(item_def::get_weapon_type(info.m_TItemID) == ITEM_TYPE_PRIMARY )
		wstr_weapon_type_str = usf::GetItemUsageName_Only_PrimaryWeapon(info.m_TItemID);
	else
		wstr_weapon_type_str = usf::GetItemClassName(info.m_TItemID);

	LuaState * L = UILua::CallLuaFunction( slot_scn, "InitSlot");
	i3Lua::PushStringUTF16To8( L, wstrItemName);
	i3Lua::PushStringUTF16To8( L, wstr_weapon_type_str);
	UILua::EndLuaFunction(L , 2);

	if( iSlot::is_selected(index) )
		L = UILua::CallLuaFunction( slot_scn, "Select");
	else
		L = UILua::CallLuaFunction( slot_scn, "Unselect");
	UILua::EndLuaFunction(L , 0);
}











/************************//************************/
//SMenuSubSlot
SMenuSubSlot::SMenuSubSlot(iSMenu* p) : iSMenuSlot(p)
{
	m_HitTestCallback.SetObj( this );
	m_tmHitTest.SetCallback( &m_HitTestCallback );
	m_hit_entered = false;
}

SMenuSubSlot::~SMenuSubSlot()
{
}

/************************/
void SMenuSubSlot::clicked()
{
	iSlot::apply_select();
	mSlot.UpdateList( mSubInfoList.size() );
}

void SMenuSubSlot::double_clicked()
{
	if ( g_pFramework->GetUIMainframe()->GetCurrentPhaseType() == UIPHASE_READY_ROOM )
	{
		if( BattleSlotContext::i()->getMySlotInfo()->_State == SLOT_STATE_READY )
		{
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_EQUIP_USE_FAIL"));
			return;
		}
	}
	CharaInfoMgr::CurrentState & state = m_pCharaInfoMgr->m_current_state;
	state.info = mSubInfoList.find( get_now_slot() );

	send_msg( SLOT_MSG_ITEM_CHANGE);
	send_msg( SLOT_MSG_CLOSE_SLOT_SUB);
}

/************************/
void SMenuSubSlot::adjust_position()
{
	INT32 x =0, y =0;
	UINT32 w =0,h =0;
	((SMenuEquip*)mParent)->GetSubFrmPos(x,y,w,h);

	mSlot.SetPosition(x, y, 0,  0);
}

void SMenuSubSlot::Notify( SLOT_MSG msg, void * pArg)
{
	if( msg == SLOT_MSG_OPEN_SLOT_SUB)	adjust_position();
}

/************************/
void SMenuSubSlot::Entrance()
{
	CharaInfoMgr::CurrentState & state = m_pCharaInfoMgr->m_current_state;

	mSubInfoList.list_up(state.ctg, state.slot_idx);

	if( state.ctg == item_def::SCTG_WEAPON )
	{
		if( mParent->GetParent()->GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM )
		{
			mSubInfoList.remove_locked_weapon();
		}
	}

	mSubInfoList.sort(state.ctg);

	iSlot::reset_select( mSubInfoList.size() );

	size_t selectedInx = mSubInfoList.find_idx( state.info );
	iSlot::set_select( selectedInx );

	mSlot.UpdateList( mSubInfoList.size() );

	// Key : User Guide System
	{
		UserGuideMgr::i()->OnLoadGuideTargetInfo( get_base() );
	}

	m_hit_entered = true;
}

void SMenuSubSlot::Exit()
{
	iSMenuSlot::Exit();

	// Key : User Guide System
	{
		UserGuideMgr::i()->UnLoadGuideTargetInfo( get_base() );
	}
}

void SMenuSubSlot::load_impl()
{
	mSlot.OnLoadAllScenes( -(INT32)(i3UI::getManager()->getDisplayOffsetX() * i3UI::getManager()->getScreenStretchRatioX() ), 0, 0,  0);

	i3UIScene* scn = get_base()->GetScene("PointBlankRe_SideMenu2");
	m_tmHitTest.add( scn, "i3UIFrameWnd0" );
	m_tmHitTest.add( (i3UIControl*)mSlot.GetScrollCtrl().GetScrollBar() );
}

void SMenuSubSlot::unload_impl()
{
	m_tmHitTest.clear();
}

/************************/
void	SMenuSubSlot::cb_Enabled_impl( INT32 slot_idx, i3UIScene* slot_scn)
{
	slot_scn->EnableScene(true);

	INT32 index = mSlot.GetStartInx() + slot_idx;

	const ITEM_INFO & info = mSubInfoList.find( index );

	GetItemImageMgr()->SelectImage(m_ppImgSet[slot_idx], info.m_TItemID);

	i3::rc_wstring wstrItemName;
	usf::GetShopItemName(wstrItemName, info.m_TItemID);

	i3::rc_wstring wstr_weapon_type_str; //주무기는 usage name, 그 외는 class name
	if(item_def::get_weapon_type(info.m_TItemID) == ITEM_TYPE_PRIMARY )
		wstr_weapon_type_str = usf::GetItemUsageName_Only_PrimaryWeapon(info.m_TItemID);
	else
		wstr_weapon_type_str = usf::GetItemClassName(info.m_TItemID);

	LuaState * L = UILua::CallLuaFunction( slot_scn, "InitSlot");
	i3Lua::PushStringUTF16To8( L, wstrItemName);
	i3Lua::PushStringUTF16To8( L, wstr_weapon_type_str);
	UILua::EndLuaFunction(L , 2);

	if( iSlot::is_selected(index) )
		L = UILua::CallLuaFunction( slot_scn, "Select");
	else
		L = UILua::CallLuaFunction( slot_scn, "Unselect");
	UILua::EndLuaFunction(L , 0);
}


void SMenuSubSlot::hit_test_enter( UINT32 inx, i3UIControl* pWnd )
{
	m_hit_entered = true;
}
void SMenuSubSlot::hit_test_leave( UINT32 inx, i3UIControl* pWnd )
{
	m_hit_entered = false;
	GameUI::ResetMousePressedUp();
}

void SMenuSubSlot::update_impl( REAL32 rDeltaSeconds)
{
	m_tmHitTest.OnUpdate(rDeltaSeconds);

	if(!m_hit_entered && GameUI::IsMousePressedUp(I3I_MOUSE_LBUTTON) )
	{
		if( !g_pFramework->GetUIMainframe()->IsPopupFocused() )
		{
			if( BaTMaN::enable_move_inven_wnd )
			{
				// Key : User Guide System
				{
					UserGuideMgr::i()->UnLoadGuideTargetInfo(get_base());
				}

				send_msg( SLOT_MSG_CLOSE_SLOT_SUB);
			}
		}
	}
}