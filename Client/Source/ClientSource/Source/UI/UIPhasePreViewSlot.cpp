#include "pch.h"
#include "UIPhasePreViewSlot.h"

#include "UIPhaseShop.h"
#include "ItemImageMgr.h"
#include "BuyUtil.h"
#include "UIShopFunction.h"
#include "UIUtil.h"
#include "UIItemInfo_Def.h"

#include "UIFloatCharacterView.h"
#include "UISignal.h"

namespace
{
	PreviewSlot* g_this = 0;
}

extern "C" 
{
	int preview_item_slot2( LuaState * L)
	{
		g_this->double_clicked();
		return 0;
	}
}

LuaFuncReg preview_slot_glue[] =
{
	{ "Click2",		preview_item_slot2	},
	{ nullptr,							nullptr}
};


PreviewSlot::PreviewSlot(UIPhaseShop* p) : mParent(p)
{
	g_this = this;
}

PreviewSlot::~PreviewSlot()
{
}

///************************/
void PreviewSlot::double_clicked()
{
	INT32 slot_idx = iSlot::apply_select();

	if (slot_idx >= 0)
		buy::RemovePreview(item_def::PartsType_ShopPreview(slot_idx));

	UIFloatCharacterView * pCharaViewWin = static_cast<UIFloatCharacterView*>(GameUI::GetFloating(UFW_CHARACTER));
	if( pCharaViewWin != nullptr)
		pCharaViewWin->Update_CharaView_Parts_Preview();

	UISignal::UpdatePreviewSlot();
//	UI::call_cb("update_preview_slot");
}


//************************/
void PreviewSlot::OnCreate( i3GameNode * pParent)
{
	enum	{	ROW = 7, COL = 1, SLOT = ROW * COL, };

	mSlot.EnableScroll( false );
	mSlot.OnCreate( (UIBase*)mParent, ROW, COL);
	mSlot.RegisterSlotScene("Scene/Main/PointBlankRe_Shop_Preview_Slot.i3UIs", preview_slot_glue, true);

	m_ppImgSet = new UIImgSetCtrl*[SLOT];
	MEMDUMP_NEW( m_ppImgSet, sizeof(UIImgSetCtrl*) * SLOT);

	::memset( m_ppImgSet, 0, sizeof(UIImgSetCtrl*) * SLOT);
}

void PreviewSlot::OnLoadAllScenes()
{
	if (m_ppImgSet == nullptr)
		return;
	enum	{	offset_x = 0, offset_y = 0, }; //이 값은 중요하지 않다. OnRevive에서 위치 조정한다.
	mSlot.OnLoadAllScenes(offset_x,offset_y,0,0);
	mSlot.SetEnableMouseOver(true);
	mSlot.SetEnable(false);

	buy::ClearPreview();

	iSlot::reset_select( buy::GetNumSlot_Preview() );			// 이값은 실제 갯수라기보다는 슬롯갯수라서 고정값 아닌가ㅏ....
}

void PreviewSlot::OnUnloadAllScenes()
{
	if (m_ppImgSet == nullptr)
		return;

	mSlot.OnUnloadAllScenes();
}

void PreviewSlot::OnRevive( i3RenderContext * pCtx)
{
	if (m_ppImgSet == nullptr)
		return;

	INT32 left = 0, top =0;
	mParent->get_left_top_position(left, top);
	mSlot.SetPosition(left+1, top+1, 0, 0);
}

void PreviewSlot::OnUpdate( REAL32 rDeltaSeconds)
{
	mSlot.OnUpdate();
}

void PreviewSlot::SetEnable(bool enable)
{
	mSlot.SetEnable(enable);
	if( enable )
	{
		mSlot.UpdateList(buy::GetNumSlot_Preview()); 
	}
}

void PreviewSlot::MoveBackScene()
{
	if (m_ppImgSet == nullptr)
		return;

	mSlot.MoveBackScene();
}
	

/************************/
void PreviewSlot::cb_Load_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
	m_ppImgSet[slot_idx] = GetItemImageMgr()->CreateImgSetCtrl(slot_scn, "i3UIButtonImageSet");
}

void PreviewSlot::cb_Unload_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
	if( m_ppImgSet[slot_idx] )
		m_ppImgSet[slot_idx]->RestoreSizePos();
	I3_SAFE_RELEASE(m_ppImgSet[slot_idx]);
}

void	PreviewSlot::cb_Enabled_impl( INT32 slot_idx, i3UIScene* slot_scn)
{
	slot_scn->EnableScene(true);

	i3::rc_wstring wstr_title;
	T_ItemID itemID = 0;

	const buy::DisplayedGoods* displayedGoods = buy::GetDisplayedGoods_ByPreviewSlot(item_def::PartsType_ShopPreview(slot_idx));

	if( displayedGoods != nullptr )
	{
		itemID = displayedGoods->GetGoodsPack()->GetGoods(0)->_ItemID; 			
		wstr_title = usf::GetItemLineUpName_Only_Parts(itemID);
	}
	else
	{
		wstr_title = usf::GetItemLineUpName_Only_Parts((item_def::PartsType_ShopPreview)slot_idx);
	}
	GetItemImageMgr()->SelectImage(m_ppImgSet[slot_idx], itemID);

	LuaState * L = UILua::CallLuaFunction( slot_scn, "SetTitle");
	i3Lua::PushStringUTF16To8( L, wstr_title);
	UILua::EndLuaFunction(L , 1);
}

void PreviewSlot::cb_Disabled_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
	slot_scn->EnableScene(false);
}

void PreviewSlot::cb_Enter_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
	LuaState * L = UILua::CallLuaFunction( slot_scn, "Enter");
	UILua::EndLuaFunction(L , 0);
}
void PreviewSlot::cb_Leave_impl( INT32 slot_idx, i3UIScene* slot_scn )
{
	LuaState * L = UILua::CallLuaFunction( slot_scn,  "Leave");
	UILua::EndLuaFunction(L , 0);
}
