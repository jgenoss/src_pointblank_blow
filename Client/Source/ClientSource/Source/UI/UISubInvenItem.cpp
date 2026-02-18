#include "pch.h"
#include "UISubInvenItem.h"

#include "UIPhaseInventory.h"
#include "ItemImageMgr.h"

#include "UIMainFrame.h"
#include "UITopMenu.h"

enum	{	ROW = 1, COL = 4, SLOT = ROW * COL,	};


extern "C" 
{
	int yg_eq_click( LuaState * L)
	{
		
		return 0;
	}
}

LuaFuncReg yg_eq_glue[] =
{
	{ "ClickButton",			yg_eq_click	},
	{ NULL,							NULL}
};


//------------------------------------------------------------------------------//
SubInvenItem::SubInvenItem(UIPhaseInventory* p, UISlide::slider_type t) : SubInven(p,t)
{
}

SubInvenItem::~SubInvenItem()
{

}


void SubInvenItem::OnCreate( i3GameNode * pParent)
{
	//Slot 만들고
	m_Slot.OnCreate(m_pParent, ROW, COL);
	m_Slot.RegisterSlotScene("UIRe/PointBlankRe_Inven_ItemList.i3UIe", yg_eq_glue);

	m_ppImgSet = new UIImgSetCtrl*[SLOT];
	::memset( m_ppImgSet, 0, sizeof(UIImgSetCtrl*) * SLOT);
}

static void tt(i3List& refGoodsList)
{
	refGoodsList.Clear();

	refGoodsList.Add((void*) 0);
	refGoodsList.Add((void*) 1);
	refGoodsList.Add((void*) 2);
	refGoodsList.Add((void*) 3);

	refGoodsList.Add((void*) 3);
	refGoodsList.Add((void*) 2);
	refGoodsList.Add((void*) 1);
	refGoodsList.Add((void*) 0);

	refGoodsList.Add((void*) 0);
	refGoodsList.Add((void*) 1);
	refGoodsList.Add((void*) 2);
	refGoodsList.Add((void*) 3);
}

void SubInvenItem::OnLoadAllScenes()
{
	//Bg Size : 728, Top Menu Size : 64
	//Slot Count : 4, offset 10
	//Slot 위치 조정

	INT32 start_x = (INT32)(230.f);
	INT32 start_y = (INT32)(500.f);

	m_Slot.OnLoadAllScenes(start_x, start_y, 10,  10);
	m_Slot.AddSlide( m_pParent->GetSlide(), mSlider_type );

	tt( m_List );
	m_Slot.AttachList(&m_List);
}
void SubInvenItem::OnUnloadAllScenes()
{
}
void SubInvenItem::OnUpdate( REAL32 rDeltaSeconds)
{
	m_Slot.OnUpdate();
}

bool SubInvenItem::OnEntranceStart( void * pArg1, void * pArv2)
{
	m_Slot.SetEnableMouseOver(true);

	return true;
}


void	SubInvenItem::cb_Load_impl( INT32 slot_idx, i3UIScene* slot_scn)
{
	m_ppImgSet[slot_idx] = GetItemImageMgr()->CreateImgSetCtrl(slot_scn, "i3UIButtonImageSet");
}

void	SubInvenItem::cb_Unload_impl( INT32 slot_idx, i3UIScene* slot_scn)
{

}

void	SubInvenItem::cb_Enabled_impl( INT32 slot_idx, i3UIScene* slot_scn)
{
	INT32 start = m_Slot.GetStartInx();
	INT32 cnt = (INT32)m_List.GetItem(slot_idx + start);

	GetItemImageMgr()->SelectImage(m_ppImgSet[slot_idx], cnt);
}

void	SubInvenItem::cb_Disabled_impl( INT32 slot_idx, i3UIScene* slot_scn)
{
	slot_scn->SetEnable(FALSE);
}

void	SubInvenItem::cb_Enter_impl( INT32 slot_idx, i3UIScene* slot_scn)
{

}

void SubInvenItem::cb_Leave_impl( INT32 slot_idx, i3UIScene* slot_scn)
{

}