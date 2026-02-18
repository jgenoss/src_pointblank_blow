#include "pch.h"
#include "AIDriverKnifeDefault.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverKnifeDefault);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// Idle
//
static void AIPROC_Idle_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnIdle( pCtx, tm);
}

static void AIPROC_Idle_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire
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
// SecondaryFire
// ANIMATION : Weapon/M-7Knife/Attack_Chop.i3a
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
// AttackA_Left
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
// AttackA_Right
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
// AttackB_Right
//
static void AIPROC_AttackB_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnFire( pCtx, tm);
}

static void AIPROC_AttackB_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change_Left
//
static void AIPROC_Change_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN( pCtx, tm);
}

static void AIPROC_Change_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change_Right
//
static void AIPROC_Change_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN( pCtx, tm);
}

static void AIPROC_Change_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
}


////////////////////////////////////////////////////////////////////////////////
//
// UI_RebelMale_Left
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
// ANIMATION : Weapon/GH5007/Resource/UI_Idle_Swat.i3a
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
// ANIMATION : Weapon/GH5007/Resource/UI_Idle_Rebel.i3a
//
static void AIPROC_UI_Rebel_Common_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_Rebel_Common_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_RebelMale_Left2
//
static void AIPROC_UI_RebelMale_Left2_Run( i3AIContext * pCtx, REAL32 tm)
{
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
}

static void AIPROC_UI_RebelMale_Right2_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_RebelFemale_Left2
// ANIMATION : Weapon/M-9/Resource/Dual_UI_VierRed_Left.i3a
//
static void AIPROC_UI_RebelFemale_Left2_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_RebelFemale_Left2_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_RebelFemale_Right2
// ANIMATION : Weapon/M-9/Resource/Dual_UI_VierRed_Right.i3a
//
static void AIPROC_UI_RebelFemale_Right2_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_RebelFemale_Right2_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

static i3::statictable::AISTATE_PROC s_KnifeDefaultTable[] =
{
	{	"AttackA_Left",	AIPROC_AttackA_Left_Run,	AIPROC_AttackA_Left_Fin	},
	{	"AttackA_Right",	AIPROC_AttackA_Right_Run,	AIPROC_AttackA_Right_Fin	},
	{	"AttackB_Left",	AIPROC_AttackB_Left_Run,	AIPROC_AttackB_Left_Fin	},
	{	"AttackB_Right",	AIPROC_AttackB_Right_Run,	AIPROC_AttackB_Right_Fin	},
	{	"Change_Left",	AIPROC_Change_Left_Run,	AIPROC_Change_Left_Fin	},
	{	"Change_Right",	AIPROC_Change_Right_Run,	AIPROC_Change_Right_Fin	},
	{	"Fire",	AIPROC_Fire_Run,	AIPROC_Fire_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"LoadBullet",	AIPROC_LoadBullet_Run,	AIPROC_LoadBullet_Fin	},
	{	"LoadMag",	AIPROC_LoadMag_Run,	AIPROC_LoadMag_Fin	},
	{	"SecondaryFire",	AIPROC_SecondaryFire_Run,	AIPROC_SecondaryFire_Fin	},
	{	"UI_Rebel_Common",	AIPROC_UI_Rebel_Common_Run,	AIPROC_UI_Rebel_Common_Fin	},
	{	"UI_RebelFemale_Left",	AIPROC_UI_RebelFemale_Left_Run,	AIPROC_UI_RebelFemale_Left_Fin	},
	{	"UI_RebelFemale_Left2",	AIPROC_UI_RebelFemale_Left2_Run,	AIPROC_UI_RebelFemale_Left2_Fin	},
	{	"UI_RebelFemale_Right2",	AIPROC_UI_RebelFemale_Right2_Run,	AIPROC_UI_RebelFemale_Right2_Fin	},
	{	"UI_RebelFemale_Right",	AIPROC_UI_RebelFemale_Right_Run,	AIPROC_UI_RebelFemale_Right_Fin	},
	{	"UI_RebelMale_Left2",	AIPROC_UI_RebelMale_Left2_Run,	AIPROC_UI_RebelMale_Left2_Fin	},
	{	"UI_RebelMale_Left",	AIPROC_UI_RebelMale_Left_Run,	AIPROC_UI_RebelMale_Left_Fin	},
	{	"UI_RebelMale_Right2",	AIPROC_UI_RebelMale_Right2_Run,	AIPROC_UI_RebelMale_Right2_Fin	},
	{	"UI_RebelMale_Right",	AIPROC_UI_RebelMale_Right_Run,	AIPROC_UI_RebelMale_Right_Fin	},
	{	"UI_Swat_Common",	AIPROC_UI_Swat_Common_Run,	AIPROC_UI_Swat_Common_Fin	},
};

AIDriverKnifeDefault::AIDriverKnifeDefault(void)
{
	setAIProcTable( s_KnifeDefaultTable, (sizeof(s_KnifeDefaultTable)/sizeof(s_KnifeDefaultTable[0])));
}

AIDriverKnifeDefault::~AIDriverKnifeDefault(void)
{
}

