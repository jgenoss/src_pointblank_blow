#include "pch.h"

#include "UISMenu_SlotSub.h"

#include "UISMenu_Equip.h"
#include "UISideMenu.h"
#include "ItemImageMgr.h"
#include "UIShopFunction.h"
#include "UIMainFrame.h"

#include "UIInvenInfoMgr.h"


/*************************************/
//SMenuSlotSubParts
namespace parts
{
	SMenuSlotSubParts* g_this = 0;
	enum	{	ROW = 5, COL = 1, SLOT = ROW * COL,	};
}

extern "C" 
{
	int smenu_slotsub_parts( LuaState * L)
	{
		parts::g_this->clicked();
		return 0;
	}
	int smenu_slotsub_parts2( LuaState * L)
	{
		parts::	g_this->double_clicked();
		return 0;
	}
}

LuaFuncReg sm_sub_slot_parts_glue[] =
{
	{ "Click", smenu_slotsub_parts	},
	{ "Click2", smenu_slotsub_parts2	},
	{ nullptr,							nullptr}
};

/*************************************/
SMenuSlotSubParts::SMenuSlotSubParts(iSMenu* p) : SMenuSubSlot(p)
{
	parts::g_this = this;
	mSlot.EnableScroll( true );
}

SMenuSlotSubParts::~SMenuSlotSubParts()
{
}


void SMenuSlotSubParts::create_impl( i3GameNode * pParent)
{
	//Slot 만들고
	mSlot.OnCreate((UIBase*)get_base(), parts::ROW, parts::COL);
	mSlot.RegisterSlotScene("Scene/Main/PointBlankRe_SideMenu_Slot.i3UIs", sm_sub_slot_parts_glue);

	m_ppImgSet = new UIImgSetCtrl*[parts::SLOT];
	MEMDUMP_NEW( m_ppImgSet, sizeof( UIImgSetCtrl*) * parts::SLOT);

	::memset( m_ppImgSet, 0, sizeof(UIImgSetCtrl*) * parts::SLOT);
}




/*************************************/
//SMenuSlotSubWeapon

namespace weapon
{
	SMenuSlotSubWeapon* g_this = 0;
	enum	{	ROW = 5, COL = 1, SLOT = ROW * COL,	};
}


extern "C" 
{
	int smenu_slotsub_weapon( LuaState * L)
	{
		weapon::g_this->clicked();
		return 0;
	}

	int smenu_slotsub_weapon2( LuaState * L)
	{
		weapon::g_this->double_clicked();
		return 0;
	}
}

LuaFuncReg sm_sub_slot_weapon_glue[] =
{
	{ "Click", smenu_slotsub_weapon	},
	{ "Click2", smenu_slotsub_weapon2	},
	{ nullptr,							nullptr}
};

//------------------------------------------------------------------------------//
SMenuSlotSubWeapon::SMenuSlotSubWeapon(iSMenu* p) : SMenuSubSlot(p)
{
	weapon::g_this = this;
	mSlot.EnableScroll( true );
}

SMenuSlotSubWeapon::~SMenuSlotSubWeapon()
{
}


void SMenuSlotSubWeapon::create_impl( i3GameNode * pParent)
{
	//Slot 만들고
	mSlot.OnCreate((UIBase*)get_base(), weapon::ROW, weapon::COL);
	mSlot.RegisterSlotScene("Scene/Main/PointBlankRe_SideMenu_Slot.i3UIs", sm_sub_slot_weapon_glue, false, true);

	m_ppImgSet = new UIImgSetCtrl*[weapon::SLOT];
	MEMDUMP_NEW( m_ppImgSet, sizeof( UIImgSetCtrl*) * weapon::SLOT);

	::memset( m_ppImgSet, 0, sizeof(UIImgSetCtrl*) * weapon::SLOT);
}
