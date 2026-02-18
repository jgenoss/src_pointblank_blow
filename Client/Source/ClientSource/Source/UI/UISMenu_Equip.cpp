#include "pch.h"
#include "UISMenu_Equip.h"

#include "UISideMenu.h"
#include "UIMainFrame.h"

#include "UIFloatCharacterView.h"
#include "GameCharaAnimContext.h"
#include "InvenDef.h"

#include "UISMenu_Chara.h"
#include "UISMenu_SlotMain.h"
#include "UISMenu_SlotSub.h"
#include "UserInfoContext.h"

#include "UICharaInfoMgr.h"
#include "UISMenu_Ability.h"
#include "UISignal.h"
#include "i3Base/itl/bind/bind.h"
#include "i3Base/itl/bind/placeholders.h"

/***********************//***********************/
namespace
{
	enum { SLOT_NA = -1, SLOT_7 =1, SLOT_5 	};
	enum { ACCEPT, CLOSE, AUTO_REPAIR };

	class NullSMenuSlot : public iSMenuSlot
	{
	public:
		NullSMenuSlot(iSMenu* p) : iSMenuSlot(p) {}
		virtual ~NullSMenuSlot() {}

		virtual void OnCreate( i3GameNode * pParent) final {}
		virtual void OnLoadAllScenes() final {}
		virtual void OnUnloadAllScenes() final {}
		virtual void OnUpdate( REAL32 rDeltaSeconds) final {}
		virtual void OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code) final {}
		virtual void OnRevive( i3RenderContext * pCtx) final {}

		virtual void create_impl( i3GameNode * pParent) final {}
		virtual void load_impl() final {}
	};
	NullSMenuSlot * g_pNullSlot = 0;

	SMenuEquip* g_this = 0;

	bool g_bInvenPhase = false;
	MultiSceneInfo g_scn_info;
	bool g_is_opened = false, g_is_sub_opened = false;

	void main_adjust(int state);
	void sub_adjust(bool open);
}

/***********************//***********************/
extern "C" 
{
	int sm_eq_click( LuaState * L)
	{
		INT32 value = i3Lua::GetIntegerArg( L, 1);
		g_this->clicked( value );
		return 0;
	}
}

LuaFuncReg sm_eq_glue[] =
{
	{ "Click",			sm_eq_click	},
	{ nullptr,							nullptr}
};

/***********************//***********************/
void SMenuEquip::clicked(int idx)
{
	switch (idx)
	{
		case ACCEPT			: mParent->send_change_equip(); break;
		case CLOSE			: cb_smenu_equip_close(false); break;
		case AUTO_REPAIR	: break;
	}
}

void SMenuEquip::cb_smenu_equip_close(bool bArg)
{
	// 캐릭터 장비를 이전상태로 돌린다.
	if( (m_bBackupEquipment == true) && bArg == false )
	{
		UI_CHARA_INFO * pInfo = UserInfoContext::i()->SetMyUICharaInfo();
		m_TempCharaInfo.CopyTo( pInfo);

		// 아바타 뷰를 갱신
		g_pFramework->OnEvent( UIEVENT_CHANGE_AVATAR, nullptr, 0, I3_EVT_CODE_ACTIVATE);
	}

	mParent->Notify("menu_init");
	Exit();

	if(	bArg == false) //(BOOL)arg1 == FALSE )
	{
		CharaInfoMgr::instance()->list_up();
	}

//	UI::call_cb("update_slot9");
	UISignal::UpdateSlot9(false, false);

}

void SMenuEquip::cb_smenu_equip_slot_clear()
{
	sub_adjust( false );
	pMainSlot->Exit();	pMainSlot = g_pNullSlot;
	pSubSlot->Exit();		pSubSlot = g_pNullSlot;
} 

//------------------------------------------------------------------------------//
SMenuEquip::SMenuEquip(UISideMenu* p) : iSMenu(p), 
				pMainSlot(0), pSubSlot(0), pCharaSlot(0), m_bBackupEquipment(false)
{
	g_this = this;
	g_pNullSlot = new NullSMenuSlot(this);
	MEMDUMP_NEW( g_pNullSlot, sizeof( NullSMenuSlot));

	pMainSlot = pSubSlot = pCharaSlot = g_pNullSlot;

	mSubList.resize(SM_SLOT_MAX);

	mSubList[SM_SLOT5] = new SMenuSlot5(this);
	mSubList[SM_SLOT7_P] = new SMenuSlot7Parts(this);
	mSubList[SM_SLOT7_W] = new SMenuSlot7Weapon(this);
	mSubList[SM_SUB_P] = new SMenuSlotSubParts(this);
	mSubList[SM_SUB_W] = new SMenuSlotSubWeapon(this);

	MEMDUMP_NEW( mSubList[SM_SLOT5], sizeof( SMenuSlot5));
	MEMDUMP_NEW( mSubList[SM_SLOT7_P], sizeof( SMenuSlot7Parts));
	MEMDUMP_NEW( mSubList[SM_SLOT7_W], sizeof( SMenuSlot7Weapon));
	MEMDUMP_NEW( mSubList[SM_SUB_P], sizeof( SMenuSlotSubParts));
	MEMDUMP_NEW( mSubList[SM_SUB_W], sizeof( SMenuSlotSubWeapon));

	RegisterGameEventFunc( EVENT_INVEN_CHANGE_CHARA_EQUIP,			&SMenuEquip::ReceivedGameEvent_ChangedCharaEquip);
}

SMenuEquip::~SMenuEquip()
{
	I3_SAFE_DELETE( g_pNullSlot);

	struct Fn	 {  void operator()(iSMenuSlot* p) {	I3_SAFE_DELETE( p); } };

	i3::for_each(mSubList.begin(), mSubList.end(), Fn() );

	mSubList.clear();
}

/***********************//***********************/
void SMenuEquip::Entrance(bool arg1)	//frame Open, data update
{
	//arg1 : 인벤이 아닐경우 open(true), skip(false)
	CharaInfoMgr::CurrentState & state = CharaInfoMgr::instance()->m_current_state;

	state.chara_idx = CCharaInfoContext::i()->GetMainCharaSlotIdx();

	bool InvenPhase = (mParent->GetMainFrame()->GetCurrentSubPhaseType() == USP_INVENTORY);
	if( InvenPhase == false)
	{
		if(arg1 == false) return;

		main_adjust( SLOT_5 );
		
		pMainSlot = mSubList[SM_SLOT5];
		state.ctg = item_def::SCTG_WEAPON; //무기 메뉴만 존재한다.

		// Key : User Guide System
		{
			UserGuideMgr::i()->OnLoadGuideTargetInfo(mParent);
		}
	}
	else
	{
		state.ctg = state.chara_menu_ctg; //왼쪽 캐릭터 메뉴가 중요하다.
	}

	//main_adjust 하면 한개의 frame만 활성화 되기 때문에 
	//sub slot 이 열린 상태이면 sub_adjust 임의로 열여 줘야 한다.
	if(g_is_sub_opened)
	{
		sub_adjust(true);
	}

	pMainSlot->Entrance();
	pSubSlot->Entrance();
	pCharaSlot->Entrance();

	// 현재 장비들을 임시로 저장해둔다.
	UI_CHARA_INFO * pCharaInfo = (UI_CHARA_INFO*) UserInfoContext::i()->GetMyUICharaInfo();
	pCharaInfo->CopyTo( &m_TempCharaInfo);
	m_bBackupEquipment = true;
}

void SMenuEquip::Exit(void* arg1, void* arg2)
{
	// Key : User Guide System
	{
		UserGuideMgr::i()->UnLoadGuideTargetInfo(mParent);
	}

	main_adjust( SLOT_NA );
	sub_adjust( false );

	pSubSlot->Exit();		pSubSlot=g_pNullSlot;
	pMainSlot->Exit();	pMainSlot = g_pNullSlot;
	pCharaSlot->Exit();	pCharaSlot = g_pNullSlot;

	m_bBackupEquipment = false;
}

/***********************/
void SMenuEquip::notify_open_slot_sub_impl(void * pArg)
{
	sub_adjust(true);

	CharaInfoMgr::CurrentState & state = CharaInfoMgr::instance()->m_current_state;

	if( state.ctg == item_def::SCTG_WEAPON )	pSubSlot = mSubList[SM_SUB_W];
	else if( state.ctg == item_def::SCTG_PARTS ) pSubSlot = mSubList[SM_SUB_P];

	pSubSlot->Notify( iSMenuSlot::SLOT_MSG_OPEN_SLOT_SUB, pArg);	//SubFrame 위치 조정한다.
	pSubSlot->Entrance();
	pSubSlot->SetScrollFocus();
}

void SMenuEquip::notify_close_slot_sub_impl(void * pArg)
{
	sub_adjust(false);
	pSubSlot->Exit();
	pSubSlot = g_pNullSlot;
}

void SMenuEquip::notify_item_change_impl(void * pArg)
{
	CharaInfoMgr::CurrentState & state = CharaInfoMgr::instance()->m_current_state;

	ItemInfoConvert item_convert(state.info);
	CharaInfoMgr::instance()->change_equip_item(item_convert, state.info);

	if( item_def::get_type(state.info.m_TItemID) != ITEM_TYPE_DINO )
	{
		pMainSlot->Notify( iSMenuSlot::SLOT_MSG_UPDATE_ITEM, pArg);

		UINT32 eventType = 0;
		switch( state.ctg) 
		{
		case item_def::SCTG_WEAPON :	eventType = UIEVENT_CHANGE_WEAPON;		break;
		case item_def::SCTG_PARTS :		eventType = UIEVENT_EQUIP_PARTS;		break;
		default :
			I3PRINTLOG(I3LOG_FATAL,  "invalid category!!!!");
			break;
		}

		g_pFramework->OnEvent( eventType, nullptr, (UINT32) (&state), I3_EVT_CODE_ACTIVATE);
	}

//	UI::call_cb("update_slot9");
	UISignal::UpdateSlot9(false, false);

	mParent->PlaySound( UISND_ITEM_EQUIP);

	SMenuAbility* ability = 
		static_cast<SMenuAbility*>(UISideMenu::instance()->getSubSidemenu(SIDEMENU_BTN_ABILITY));
	ability->SetMainInfoUpdateType(AMUT_PURCHASED);
}

void SMenuEquip::Notify( iSMenuSlot::SLOT_MSG msg, void * pArg)
{
	if( msg == iSMenuSlot::SLOT_MSG_OPEN_SLOT_SUB)	//SubFrame을 활성화 한다.
	{
		notify_open_slot_sub_impl(pArg);
	}
	else if( msg == iSMenuSlot::SLOT_MSG_CLOSE_SLOT_SUB) //SubFrame을 비활성화 한다.
	{
		notify_close_slot_sub_impl(pArg);
	}
	else if( msg == iSMenuSlot::SLOT_MSG_ITEM_CHANGE)	//MainSlot 에 ItemId 를 변경한다.
	{
		notify_item_change_impl(pArg);
	}
}

/***********************//***********************/

void SMenuEquip::GetFrmPos(INT32 &x, INT32 &y, UINT32 &w, UINT32 &h) const
{
	i3UIButton* curr_btn = //g_this->GetParent()->get_current_enable_btn();
		g_this->GetParent()->get_btn(SIDEMENU_BTN_EQUIP);

	VEC3D* pos = curr_btn->getAccumulatedPos();
	VEC2D* size = curr_btn->getSize();

	x = (UINT32)pos->x;	y = (UINT32)pos->y + (UINT32)size->y;
	w = (UINT32)size->x;	h = (UINT32)size->y;
}
void SMenuEquip::GetSubFrmPos( INT32 &x,  INT32 &y, UINT32 &w, UINT32 &h) const
{
	LuaState* L = UILua::CallLuaFunction(g_scn_info.scn, "GetSubFrmPtr");
	i3UIFrameWnd* pWnd = (i3UIFrameWnd*)UILua::ReturnLuaPointer(L , 0);

	VEC3D* pos = pWnd->getAccumulatedPos();
	VEC2D* size = pWnd->getSize();

	x = (INT32)pos->x;	y = (INT32)pos->y;
	w = (UINT32)size->x;	h = (UINT32)size->y;
}


/***********************//***********************/
void SMenuEquip::OnCreate( i3GameNode * pParent)
{
	g_scn_info.idx = mParent->GetSceneCount();
	mParent->AddScene("Scene/Main/PointBlankRe_SideMenu2.i3UIs", sm_eq_glue, true, false);

	struct Fn	 
	{
		i3GameNode * parent;
		Fn(i3GameNode * p) : parent(p) {}

		void operator()(iSMenuSlot* p) {	p->OnCreate(parent); 	}
	};
	i3::for_each(mSubList.begin(), mSubList.end(), Fn(pParent) );
}

void SMenuEquip::OnLoadAllScenes()
{
	//멤버 콜백 함수 등록

	m_SigConnection += UISignal::SMenuEquipClickClose.connect(i3::bind(&SMenuEquip::cb_smenu_equip_close, this, i3::_1));
	m_SigConnection += UISignal::SMenuEquipSlotClear.connect(i3::bind(&SMenuEquip::cb_smenu_equip_slot_clear, this));



//	UI::insert_cb( "smenu_equip_click_close", this, &SMenuEquip::cb_smenu_equip_close);
//	UI::insert_cb( "smenu_equip_slot_clear", this, &SMenuEquip::cb_smenu_equip_slot_clear);


	//Lua 초기화 한다.
	g_scn_info.scn = mParent->GetScene(g_scn_info.idx);
	LuaState* L = UILua::CallLuaFunction(g_scn_info.scn, "LoadAllScenes");
	UILua::EndLuaFunction(L , 0);

	struct Fn	 {  void operator()(iSMenuSlot* p) {	p->OnLoadAllScenes(); } };
	i3::for_each(mSubList.begin(), mSubList.end(), Fn() );
}

void SMenuEquip::OnUnloadAllScenes()
{
	//멤버 콜백 함수 날려주기
	//UI::remove_cb(this);
	m_SigConnection.clear();

	struct Fn	 {  void operator()(iSMenuSlot* p) {	p->OnUnloadAllScenes(); } };
	i3::for_each(mSubList.begin(), mSubList.end(), Fn() );
}

void SMenuEquip::OnUpdate( REAL32 rDeltaSeconds)
{
	pMainSlot->OnUpdate(rDeltaSeconds);
	pSubSlot->OnUpdate(rDeltaSeconds);
	pCharaSlot->OnUpdate(rDeltaSeconds);
}

void SMenuEquip::OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code)
{
	pMainSlot->OnEvent(event, pObj, param2, code);
	pSubSlot->OnEvent(event, pObj, param2, code);
	pCharaSlot->OnEvent(event, pObj, param2, code);

	if( event == UIEVENT_SMENU_CHANGE_ITEM)
	{
		UIEvent_ChangedCharaEquip();
	}
}

void SMenuEquip::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
{
	i3::unordered_map< GAME_EVENT, func >::iterator it;
	it = m_RecvEventMap.find( evt);

	if ( it != m_RecvEventMap.end() )
	{
		func & f = it->second;
		(this->*f)(arg, UserData);
	}
}

void SMenuEquip::MoveBackScene()
{
	g_scn_info.scn->Attach(false);

	struct Fn	 {  void operator()(iSMenuSlot* p) {	p->MoveBackScene(); } };
	i3::for_each(mSubList.begin(), mSubList.end(), Fn() );
}

void SMenuEquip::SetDefaultWeapon( WEAPON_SLOT_TYPE slot, T_ItemID ui32ItemID )
{
	g_pCharaManager->GetLobbyAvatar()->Cmd_CreateWeapon( slot, ui32ItemID );
	if( slot == WEAPON_SLOT_PRIMARY)
	{
		if( g_pCharaManager->GetLobbyAvatar()->Cmd_ChangeWeapon( slot, true))
		{
			g_pCharaManager->GetLobbyAvatar()->getAnimContext()->ChangeAnimationContext();
		}
	}
}

i3UIScene * SMenuEquip::GetMainSlotScene(INT32 idx)
{
	if(pMainSlot != g_pNullSlot)
		return pMainSlot->GetSlotScene(idx);

	return nullptr;
}

i3UIScene * SMenuEquip::GetSubSlotScene(INT32 idx)
{
	if(pSubSlot != g_pNullSlot)
		return pSubSlot->GetSlotScene(idx);

	return nullptr;
}

bool SMenuEquip::IsSelectedSlot(INT32 idx)
{
	if(pMainSlot != g_pNullSlot)
		return pMainSlot->is_selected_slot(idx);

	return false;
}

namespace 
{
	void main_adjust(int state)
	{
		LuaState* L = nullptr;

		if( state == SLOT_NA )
		{
			L = UILua::CallLuaFunction(g_scn_info.scn, "CloseFrm");
			UILua::EndLuaFunction(L , 0);
			g_is_opened = false;
		}
		else
		{
			L = UILua::CallLuaFunction(g_scn_info.scn, "OpenFrm");
			i3Lua::PushInteger( L, state );
			UILua::EndLuaFunction(L , 1);

			i3UIButton* curr_btn = //g_this->GetParent()->get_current_enable_btn();
				g_this->GetParent()->get_btn(SIDEMENU_BTN_EQUIP);
			VEC3D* pos = curr_btn->getAccumulatedPos(); VEC2D* size = curr_btn->getSize();

			L = UILua::CallLuaFunction(g_scn_info.scn, "GetFrmPtr");
			i3Lua::PushInteger( L, state);
			i3UIFrameWnd* pWnd = (i3UIFrameWnd*)UILua::ReturnLuaPointer(L , 1);
			pWnd->setPos( pos->x, pos->y + size->y );
			
			if( state == SLOT_5 )
			{
				L = UILua::CallLuaFunction(g_scn_info.scn, "RemoveOnMouse");
				UILua::EndLuaFunction(L , 0);
			}

			g_is_opened = true;
		}
	}

	void sub_adjust(bool open)
	{
		LuaState* L = nullptr;

		if( open == true )
		{
			L = UILua::CallLuaFunction(g_scn_info.scn, "OpenSubFrm");
			UILua::EndLuaFunction(L , 0);
		}
		else
		{
			L = UILua::CallLuaFunction(g_scn_info.scn, "CloseSubFrm");
			UILua::EndLuaFunction(L , 0);
		}

		g_is_sub_opened = open;
	}
}

void SMenuEquip::RegisterGameEventFunc( GAME_EVENT evt, func f)
{
	m_RecvEventMap.insert( i3::unordered_map< GAME_EVENT, func>::value_type( evt, f) );
}

void SMenuEquip::ReceivedGameEvent_ChangedCharaEquip( INT32, const i3::user_data&)
{
	//UIEvent_ChangedCharaEquip();
}


void SMenuEquip::UIEvent_ChangedCharaEquip( void)
{
	Notify( iSMenuSlot::SLOT_MSG_ITEM_CHANGE);
	pSubSlot->Entrance();
}
