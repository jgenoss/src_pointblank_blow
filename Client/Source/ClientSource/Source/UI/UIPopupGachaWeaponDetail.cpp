#include "pch.h"
#include "UIPopupGachaWeaponDetail.h"
#include "UIPhaseGachaShop.h"
#include "UIMainFrame.h"

#include "GameView.hpp"
#include "WeaponView.hpp"

namespace gwd
{
	struct null_view : view
	{
	public:
		virtual void create(UIPopupGachaWeaponDetail* parent, i3UI3DViewBox* p3DView, T_ItemID item_id) final {}
		virtual void release() final {}
		virtual void entrance() final {}
		virtual void exit() final {}
		virtual void update(REAL32 rDeltaSeconds) final {}
		virtual void revive() final {}
	} _null_view;
}

I3_CLASS_INSTANCE( UIPopupGachaWeaponDetail);

extern "C" 
{
	int UIPopupGachaWeaponDetail_Click( LuaState * L)
	{
		INT32 value = i3Lua::GetIntegerArg( L, 1);
		UIPopupGachaWeaponDetail * pThis = static_cast<UIPopupGachaWeaponDetail*>(GameUI::GetPopup(UPW_GACHA_WEAPON_DETAIL));
		if( pThis != nullptr)
			pThis->clicked(value);
		return 0;
	}
}

LuaFuncReg UIPopupGachaWeaponDetail_Glue[] = 
{
	{"Click", UIPopupGachaWeaponDetail_Click},
	{nullptr, nullptr},
};

static UIPhaseGachaShop* GetGachaShop( void)
{
	if ( UIMainFrame* pFrame = g_pFramework->GetUIMainframe() ) 
		return i3::same_cast<UIPhaseGachaShop*>(pFrame->GetCurrentSubPhase());
	return nullptr;
}

//------------------------------------------------------------------------------//
UIPopupGachaWeaponDetail::UIPopupGachaWeaponDetail() : m_p3DView(0), curr_view(nullptr)
{
}

UIPopupGachaWeaponDetail::~UIPopupGachaWeaponDetail()
{
	I3_SAFE_RELEASE(m_p3DView);
}

//------------------------------------------------------------------------------//
void UIPopupGachaWeaponDetail::clicked(INT32 idx) 
{
	enum view_type { game_view, weapon_view, popup_close, };

	switch(idx)
	{
	case game_view: switch_viewer(gwd::vt_first_person); break;
	case weapon_view: switch_viewer(gwd::vt_third_dimensions); break;
	case popup_close: OnKey_Escape(); break;
	}
}

//------------------------------------------------------------------------------//
/*virtual*/ void UIPopupGachaWeaponDetail::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad(pScene);

	view_frame_wnd = (i3UIFrameWnd*)pScene->FindChildByName( "i3UIFrameWnd" );
	create_3d_view_box(view_frame_wnd);
}

/*virtual*/ void UIPopupGachaWeaponDetail::_ClearAtUnload( i3UIScene * pScene)
{
	UIPopupBase::_ClearAtUnload( pScene);
	delete_3d_view_box();

	if( GetGachaShop() )
		GetGachaShop()->SetComboItem();
}

//------------------------------------------------------------------------------//
/*virtual*/ void UIPopupGachaWeaponDetail::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	AddScene( "Scene/Popup/PBRe_PopUp_Lottery_detaillook.i3UIs", UIPopupGachaWeaponDetail_Glue);
}

/*virtual*/ bool UIPopupGachaWeaponDetail::OnEntranceStart( void * pArg1, void * pArg2)
{
	T_ItemID item_id = *static_cast<T_ItemID*>(pArg1);

	//UIGachaView 클래스에서 호출되어지기 때문에 item_id가 항상 유효한 값이라 예상된다.
	//행여나, item_id 가 유효하지 않을 경우 아래 코드를 활성화 해주면 된다. (UIPopupGachaWeaponDetail 창이 활성화 안 됨)
	//if( usf::CheckValidGoods_in_Pef(item_id) == false )
	//{
	//	OnKey_Escape();
	//	return false;
	//}

	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	update_weapon_attribute(item_id);

	gwd::_first_person_view.create(this, m_p3DView, item_id);
	gwd::_third_dimensions_view.create(this, m_p3DView, item_id);

	curr_view_type = gwd::vt_null;
	curr_view = &gwd::_null_view;

	switch_viewer(gwd::vt_first_person);

	GetSlide()->ForceEnd();

	return true;
}

/*virtual*/ void UIPopupGachaWeaponDetail::OnLoadAllScenes()
{
	UIPopupBase::OnLoadAllScenes();
}

/*virtual*/ void UIPopupGachaWeaponDetail::OnUnloadAllScenes()
{
	UIPopupBase::OnUnloadAllScenes();

	curr_view->exit();

	curr_view_type = gwd::vt_null;
	curr_view = &gwd::_null_view;

	gwd::_first_person_view.release();
	gwd::_third_dimensions_view.release();
}

void UIPopupGachaWeaponDetail::OnExitEnd(void)
{
	UIPopupBase::OnExitEnd();
}

void UIPopupGachaWeaponDetail::OnUpdate( REAL32 rDeltaSeconds)
{
	UIPopupBase::OnUpdate(rDeltaSeconds);
	curr_view->update(rDeltaSeconds);
}

bool UIPopupGachaWeaponDetail::OnKey_Escape( void)
{
	curr_view_type = gwd::vt_null;
	curr_view = &gwd::_null_view;

	return UIPopupBase::OnKey_Escape();
}

bool UIPopupGachaWeaponDetail::OnRevive(i3RenderContext * pCtx)
{
	if (UIPopupBase::OnRevive( pCtx) == false)
		return false;

	if (curr_view)
		curr_view->revive();

	return true;
}

//------------------------------------------------------------------------------//
void UIPopupGachaWeaponDetail::update_weapon_attribute(T_ItemID itemId)
{
	i3::stack_wstring wstrRange;
	i3::stack_wstring wstrBulletCount;

	INT32 damage = 0, rapid = 0, accuracy = 0, recoil_v = 0, recoil_h = 0, speed = 0;

	if( itemId != -1 )
	{
		GameUI::GetNormalizedWeaponInfo( itemId, 
			damage, rapid, accuracy, 
			recoil_v, recoil_h, speed, 
			wstrRange, wstrBulletCount);
	}

	LuaState * L = UILua::CallLuaFunction( GetScene(0), "SetWeaponInfo" );

	i3Lua::PushNumber(L, float(damage) / 100.f);
	i3Lua::PushNumber(L, float(rapid) / 100.f);
	i3Lua::PushNumber(L, float(accuracy) / 100.f);
	i3Lua::PushNumber(L, float(recoil_v) / 100.f);
	i3Lua::PushNumber(L, float(recoil_h) / 100.f);
	i3Lua::PushNumber(L, float(speed) / 100.f);

	i3::stack_wstring wstr_temp;

	GameUI::GetGaugeString(damage, wstr_temp);
	i3Lua::PushStringUTF16To8(L, wstr_temp);
	GameUI::GetGaugeString(rapid, wstr_temp);
	i3Lua::PushStringUTF16To8(L, wstr_temp);
	GameUI::GetGaugeString(accuracy, wstr_temp);
	i3Lua::PushStringUTF16To8(L, wstr_temp);
	GameUI::GetGaugeString(recoil_v, wstr_temp);
	i3Lua::PushStringUTF16To8(L, wstr_temp);
	GameUI::GetGaugeString(recoil_h, wstr_temp);
	i3Lua::PushStringUTF16To8(L, wstr_temp);
	GameUI::GetGaugeString(speed, wstr_temp);
	i3Lua::PushStringUTF16To8(L, wstr_temp);

	i3Lua::PushStringUTF16To8(L, wstrRange );
	i3Lua::PushStringUTF16To8(L, wstrBulletCount );

	UILua::EndLuaFunction(L, 14);

	CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo( itemId );
	if( pInfo == nullptr) return;

	i3::rc_wstring wstrName, wstrDescription;
	pInfo->GetWeaponName(wstrName);
	pInfo->GetDescription(wstrDescription);

	L = UILua::CallLuaFunction( GetScene(0), "SetWeaponString" );
	i3Lua::PushStringUTF16To8(L, wstrName.c_str(), wstrName.size() );
	i3Lua::PushStringUTF16To8(L, wstrDescription.c_str(), wstrDescription.size());
	UILua::EndLuaFunction(L, 2);
}

//------------------------------------------------------------------------------//
void UIPopupGachaWeaponDetail::create_3d_view_box( i3UIControl * pParent)
{
	m_p3DView = i3UI3DViewBox::new_object();
	pParent->AddChild( m_p3DView);
}
void UIPopupGachaWeaponDetail::delete_3d_view_box( void)
{
	m_p3DView->getParent()->RemoveChild( m_p3DView);
	I3_SAFE_RELEASE( m_p3DView);
}

void UIPopupGachaWeaponDetail::switch_viewer(gwd::view_type type)
{
	if( curr_view_type == type ) return;

	curr_view->exit();

	switch(type)
	{
	case gwd::vt_first_person: curr_view = &gwd::_first_person_view; view_frame_wnd->setSize(647.f, 382.f); break;
	case gwd::vt_third_dimensions: curr_view = &gwd::_third_dimensions_view; view_frame_wnd->setSize(647.f, 322.f); break;
	}

	m_p3DView->setSize( view_frame_wnd->getWidth()-2, view_frame_wnd->getHeight()-2);
	m_p3DView->setPos( 1, 1);

	curr_view->entrance();

	curr_view_type = type;
}