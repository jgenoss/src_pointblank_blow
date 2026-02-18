#include "pch.h"
#include "AIDriverDualSword_TH.h"
#include "WeaponDualSwordTH.h"

I3_CLASS_INSTANCE( AIDriverDualSword_TH);

////////////////////////////////////////////////////////////////////////////////
//
// Idle
// ANIMATION : Weapon/Dual_Sword_TH/Resource/Idle.i3a
//
static void AIPROC_Idle_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnIdle( pCtx, tm);
}

static void AIPROC_Idle_Fin( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnEndIdle(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// SecondaryFire
// ANIMATION : Weapon/Dual_Sword_TH/Resource/Idle.i3a
//
static void AIPROC_SecondaryFire_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnSecondaryFire( pCtx, tm);
}

static void AIPROC_SecondaryFire_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag
//
static void AIPROC_LoadMag_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_LoadMag_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire
// ANIMATION : Weapon/Dual_Sword_TH/Resource/Idle.i3a
//
static void AIPROC_Fire_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnFire( pCtx, tm);
}

static void AIPROC_Fire_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// AttackA_Left
// ANIMATION : Weapon/Dual_Sword_TH/Resource/Idle.i3a
//
static void AIPROC_AttackA_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnFire( pCtx, tm);
}

static void AIPROC_AttackA_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// AttackB_Right
// ANIMATION : Weapon/Dual_Sword_TH/Resource/Attack_B_Right_1PV.i3a
//
static void AIPROC_AttackB_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponDualSwordTH* pWeapon = static_cast<WeaponDualSwordTH*>(pCtx->getOwner());
	pWeapon->OnAttackBRight( pCtx, tm);
}

static void AIPROC_AttackB_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change_Left
// ANIMATION : Weapon/Dual_Sword_TH/Resource/Idle.i3a
//
static void AIPROC_Change_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnSwaping( pCtx, tm);
}

static void AIPROC_Change_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change_Right
// ANIMATION : Weapon/Dual_Sword_TH/Resource/Idle.i3a
//
static void AIPROC_Change_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnSwaping( pCtx, tm);
}

static void AIPROC_Change_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// AttackA_Right
// ANIMATION : Weapon/Dual_Sword_TH/Resource/Idle.i3a
//
static void AIPROC_AttackA_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnFire( pCtx, tm);
}

static void AIPROC_AttackA_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// AttackB_Left
// ANIMATION : Weapon/Dual_Sword_TH/Resource/Idle.i3a
//
static void AIPROC_AttackB_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnFire( pCtx, tm);
}

static void AIPROC_AttackB_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_RebelMale_Left2
//
static void AIPROC_UI_RebelMale_Left2_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*) pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon( pCtx, tm);
}

static void AIPROC_UI_RebelMale_Left2_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_RebelMale_Right2
//
static void AIPROC_UI_RebelMale_Right2_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*) pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon( pCtx, tm);
}

static void AIPROC_UI_RebelMale_Right2_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_RebelFemale_Left2
// ANIMATION : Weapon/Dual_Sword_TH/Resource/Idle.i3a
//
static void AIPROC_UI_RebelFemale_Left2_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*) pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon( pCtx, tm);
}

static void AIPROC_UI_RebelFemale_Left2_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_RebelFemale_Right2
// ANIMATION : Weapon/Dual_Sword_TH/Resource/Idle.i3a
//
static void AIPROC_UI_RebelFemale_Right2_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*) pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon( pCtx, tm);
}

static void AIPROC_UI_RebelFemale_Right2_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet
//
static void AIPROC_LoadBullet_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_LoadBullet_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_RebelMale_Left
// ANIMATION : Weapon/Dual_Sword_TH/Resource/Idle.i3a
//
static void AIPROC_UI_RebelMale_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*) pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon( pCtx, tm);
}

static void AIPROC_UI_RebelMale_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_RebelMale_Right
// ANIMATION : Weapon/Dual_Sword_TH/Resource/Idle.i3a
//
static void AIPROC_UI_RebelMale_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*) pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon( pCtx, tm);
}

static void AIPROC_UI_RebelMale_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_RebelFemale_Left
// ANIMATION : Weapon/Dual_Sword_TH/Resource/Idle.i3a
//
static void AIPROC_UI_RebelFemale_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*) pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon( pCtx, tm);
}

static void AIPROC_UI_RebelFemale_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_RebelFemale_Right
// ANIMATION : Weapon/Dual_Sword_TH/Resource/Idle.i3a
//
static void AIPROC_UI_RebelFemale_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*) pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon( pCtx, tm);
}

static void AIPROC_UI_RebelFemale_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_Swat_Common
//
static void AIPROC_UI_Swat_Common_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_Swat_Common_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_Rebel_Common
//
static void AIPROC_UI_Rebel_Common_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_Rebel_Common_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// AttackB_Right_3PV
// ANIMATION : Weapon/Dual_Sword_TH/Resource/Attack_B_Right_3PV.i3a
//
static void AIPROC_AttackB_Right_3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnFire( pCtx, tm);
}

static void AIPROC_AttackB_Right_3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Reserve2
//
static void AIPROC_Reserve2_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_Reserve2_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Reserve3
//
static void AIPROC_Reserve3_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_Reserve3_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Reserve4
//
static void AIPROC_Reserve4_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_Reserve4_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Reserve1
//
static void AIPROC_Reserve1_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_Reserve1_Fin( i3AIContext * pCtx, REAL32 tm)
{
}



static i3::statictable::AISTATE_PROC s_DualSword_THTable[] =
{
	{	"AttackA_Left",	AIPROC_AttackA_Left_Run,	AIPROC_AttackA_Left_Fin	},
	{	"AttackA_Right",	AIPROC_AttackA_Right_Run,	AIPROC_AttackA_Right_Fin	},
	{	"AttackB_Left",	AIPROC_AttackB_Left_Run,	AIPROC_AttackB_Left_Fin	},
	{	"AttackB_Right",	AIPROC_AttackB_Right_Run,	AIPROC_AttackB_Right_Fin	},
	{	"AttackB_Right_3PV",	AIPROC_AttackB_Right_3PV_Run,	AIPROC_AttackB_Right_3PV_Fin	},
	{	"Change_Left",	AIPROC_Change_Left_Run,	AIPROC_Change_Left_Fin	},
	{	"Change_Right",	AIPROC_Change_Right_Run,	AIPROC_Change_Right_Fin	},
	{	"Fire",	AIPROC_Fire_Run,	AIPROC_Fire_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"LoadBullet",	AIPROC_LoadBullet_Run,	AIPROC_LoadBullet_Fin	},
	{	"LoadMag",	AIPROC_LoadMag_Run,	AIPROC_LoadMag_Fin	},
	{	"SecondaryFire",	AIPROC_SecondaryFire_Run,	AIPROC_SecondaryFire_Fin	},
	{	"UI_Rebel_Common",	AIPROC_UI_Rebel_Common_Run,	AIPROC_UI_Rebel_Common_Fin	},
	{	"Reserve2",	AIPROC_Reserve2_Run,	AIPROC_Reserve2_Fin	},
	{	"Reserve3",	AIPROC_Reserve3_Run,	AIPROC_Reserve3_Fin	},
	{	"Reserve4",	AIPROC_Reserve4_Run,	AIPROC_Reserve4_Fin	},
	{	"Reserve1",	AIPROC_Reserve1_Run,	AIPROC_Reserve1_Fin	},
	{	"UI_RebelFemale_Left",	AIPROC_UI_RebelFemale_Left_Run,	AIPROC_UI_RebelFemale_Left_Fin	},
	{	"UI_RebelFemale_Left2",	AIPROC_UI_RebelFemale_Left2_Run,	AIPROC_UI_RebelFemale_Left2_Fin	},
	{	"UI_RebelFemale_Right",	AIPROC_UI_RebelFemale_Right_Run,	AIPROC_UI_RebelFemale_Right_Fin	},
	{	"UI_RebelFemale_Right2",	AIPROC_UI_RebelFemale_Right2_Run,	AIPROC_UI_RebelFemale_Right2_Fin	},
	{	"UI_RebelMale_Left",	AIPROC_UI_RebelMale_Left_Run,	AIPROC_UI_RebelMale_Left_Fin	},
	{	"UI_RebelMale_Left2",	AIPROC_UI_RebelMale_Left2_Run,	AIPROC_UI_RebelMale_Left2_Fin	},
	{	"UI_RebelMale_Right",	AIPROC_UI_RebelMale_Right_Run,	AIPROC_UI_RebelMale_Right_Fin	},
	{	"UI_RebelMale_Right2",	AIPROC_UI_RebelMale_Right2_Run,	AIPROC_UI_RebelMale_Right2_Fin	},
	{	"UI_Swat_Common",	AIPROC_UI_Swat_Common_Run,	AIPROC_UI_Swat_Common_Fin	},
};

AIDriverDualSword_TH::AIDriverDualSword_TH(void)
{
	setAIProcTable( s_DualSword_THTable, (sizeof(s_DualSword_THTable)/sizeof(s_DualSword_THTable[0])) );
}

AIDriverDualSword_TH::~AIDriverDualSword_TH(void)
{
}
