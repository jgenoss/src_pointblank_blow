#include "pch.h"
#include "UISMenu_SlotMain.h"

#include "UISMenu_Equip.h"
#include "UISideMenu.h"
#include "ItemImageMgr.h"
#include "UIShopFunction.h"
#include "UIMainFrame.h"

#include "UICharaInfoMgr.h"

/*************************************/
//SMenuSlot5
namespace slot5
{
	SMenuSlot5* g_this = 0;
	enum	{	ROW = 5, COL = 1, SLOT = ROW * COL,	};
}

extern "C" 
{
	int smenu_slot5( LuaState * L)
	{
		slot5::g_this->clicked();
		return 0;
	}
}

LuaFuncReg sm_main_slot5_glue[] =
{
	{ "Click",			smenu_slot5	},
	{ nullptr,							nullptr}
};

//------------------------------------------------------------------------------//
SMenuSlot5::SMenuSlot5(iSMenu* p) : SMenuMainSlot(p)
{
	slot5::g_this = this;
	m_inven_phase = false;
	mSlot.EnableScroll( false );
	mCategory = item_def::SCTG_WEAPON;
}

SMenuSlot5::~SMenuSlot5()
{
}


void SMenuSlot5::create_impl( i3GameNode * pParent)
{
	mSlot.OnCreate((UIBase*)get_base(), slot5::ROW, slot5::COL);
	mSlot.RegisterSlotScene("Scene/Main/PointBlankRe_SideMenu_Slot2.i3UIs", sm_main_slot5_glue);

	m_ppImgSet = new UIImgSetCtrl*[slot5::SLOT];
	MEMDUMP_NEW( m_ppImgSet, sizeof( UIImgSetCtrl*) * slot5::SLOT);

	::memset( m_ppImgSet, 0, sizeof(UIImgSetCtrl*) * slot5::SLOT);
}

void SMenuSlot5::Entrance()
{
	m_pCharaInfoMgr->list_up( CCharaInfoContext::i()->GetMainCharaSlotIdx());

	SMenuMainSlot::Entrance();
}

/*************************************/
//SMenuSlot7Parts
namespace parts
{
	SMenuSlot7Parts* g_this = 0;
	enum	{	ROW = 7, COL = 1, SLOT = ROW * COL,	};
}

extern "C" 
{
	int smenu_slot7_parts( LuaState * L)
	{
		parts::g_this->clicked();
		return 0;
	}
	int smenu_slot7_parts2( LuaState * L)
	{
		return 0;
	}
}

LuaFuncReg sm_main_slot7_parts_glue[] =
{
	{ "Click", smenu_slot7_parts	},
	{ "Click2", smenu_slot7_parts2},
	{ nullptr,							nullptr}
};

//------------------------------------------------------------------------------//
SMenuSlot7Parts::SMenuSlot7Parts(iSMenu* p) : SMenuMainSlot(p)
{
	parts::g_this = this;
	m_inven_phase = true;
	mSlot.EnableScroll( false );
	mCategory = item_def::SCTG_PARTS;
}

SMenuSlot7Parts::~SMenuSlot7Parts()
{
}


void SMenuSlot7Parts::create_impl( i3GameNode * pParent)
{
	//Slot 만들고
	mSlot.OnCreate((UIBase*)get_base(), parts::ROW, parts::COL);
	mSlot.RegisterSlotScene("Scene/Main/PointBlankRe_SideMenu_Slot.i3UIs", sm_main_slot7_parts_glue);

	m_ppImgSet = new UIImgSetCtrl*[parts::SLOT];
	MEMDUMP_NEW( m_ppImgSet, sizeof( UIImgSetCtrl*) * parts::SLOT);

	::memset( m_ppImgSet, 0, sizeof(UIImgSetCtrl*) * parts::SLOT);
}









/*************************************/
//SMenuSlot7Weapon
namespace weapon
{
	SMenuSlot7Weapon* g_this = 0;
	enum	{	ROW = 7, COL = 1, SLOT = ROW * COL,	};
}

extern "C" 
{
	int smenu_slot7_weapon( LuaState * L)
	{
		weapon::g_this->clicked();
		return 0;
	}
	int smenu_slot7_weapon2( LuaState * L)
	{
		return 0;
	}
}

LuaFuncReg sm_main_slot7_weapon_glue[] =
{
	{ "Click",	 smenu_slot7_weapon	},
	{ "Click2", smenu_slot7_weapon2	},
	{ nullptr,							nullptr}
};

//------------------------------------------------------------------------------//
SMenuSlot7Weapon::SMenuSlot7Weapon(iSMenu* p) : SMenuMainSlot(p)
{
	weapon::g_this = this;
	m_inven_phase = true;
	mSlot.EnableScroll( false );
	mCategory = item_def::SCTG_WEAPON;
}

SMenuSlot7Weapon::~SMenuSlot7Weapon()
{
}


void SMenuSlot7Weapon::create_impl( i3GameNode * pParent)
{
	//Slot 만들고
	mSlot.OnCreate((UIBase*)get_base(), weapon::ROW, weapon::COL);
	mSlot.RegisterSlotScene("Scene/Main/PointBlankRe_SideMenu_Slot.i3UIs", sm_main_slot7_weapon_glue);

	m_ppImgSet = new UIImgSetCtrl*[weapon::SLOT];
	MEMDUMP_NEW( m_ppImgSet, sizeof( UIImgSetCtrl*) * weapon::SLOT);

	::memset( m_ppImgSet, 0, sizeof(UIImgSetCtrl*) * weapon::SLOT);
}