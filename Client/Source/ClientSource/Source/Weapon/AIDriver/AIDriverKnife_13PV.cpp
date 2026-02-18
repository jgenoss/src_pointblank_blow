#include "pch.h"
#include "AIDriverKnife_13PV.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverKnife_13PV);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// Idle1PV
// ANIMATION : Weapon/GH5007/Resource/Idle_1PV.i3a
//
static void AIPROC_Idle1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnIdle( pCtx, tm);
}

static void AIPROC_Idle1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// SecondaryFire
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
// Idle3PV
// ANIMATION : Weapon/GH5007/Resource/Idle_3PV.i3a
//
static void AIPROC_Idle3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnIdle( pCtx, tm);
}

static void AIPROC_Idle3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}


////////////////////////////////////////////////////////////////////////////////
//
// Change1PV
// ANIMATION : Weapon/GH5007/Resource/Idle_1PV.i3a
//
static void AIPROC_Change1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN( pCtx, tm);
}

static void AIPROC_Change1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change3PV
// ANIMATION : Weapon/GH5007/Resource/Idle_3PV.i3a
//
static void AIPROC_Change3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN( pCtx, tm);
}

static void AIPROC_Change3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}



static i3::statictable::AISTATE_PROC s_Knife_13PVTable[] =
{
	{	"AttackA_Left",	AIPROC_AttackA_Left_Run,	AIPROC_AttackA_Left_Fin	},
	{	"AttackA_Right",	AIPROC_AttackA_Right_Run,	AIPROC_AttackA_Right_Fin	},
	{	"AttackB_Left",	AIPROC_AttackB_Left_Run,	AIPROC_AttackB_Left_Fin	},
	{	"AttackB_Right",	AIPROC_AttackB_Right_Run,	AIPROC_AttackB_Right_Fin	},
	{	"Change1PV",	AIPROC_Change1PV_Run,	AIPROC_Change1PV_Fin	},
	{	"Change3PV",	AIPROC_Change3PV_Run,	AIPROC_Change3PV_Fin	},
	{	"Change_Left",	AIPROC_Change_Left_Run,	AIPROC_Change_Left_Fin	},
	{	"Change_Right",	AIPROC_Change_Right_Run,	AIPROC_Change_Right_Fin	},
	{	"Fire",	AIPROC_Fire_Run,	AIPROC_Fire_Fin	},
	{	"Idle1PV",	AIPROC_Idle1PV_Run,	AIPROC_Idle1PV_Fin	},
	{	"Idle3PV",	AIPROC_Idle3PV_Run,	AIPROC_Idle3PV_Fin	},
	{	"LoadBullet",	AIPROC_LoadBullet_Run,	AIPROC_LoadBullet_Fin	},
	{	"LoadMag",	AIPROC_LoadMag_Run,	AIPROC_LoadMag_Fin	},
	{	"SecondaryFire",	AIPROC_SecondaryFire_Run,	AIPROC_SecondaryFire_Fin	},
	{	"UI_Rebel_Common",	AIPROC_UI_Rebel_Common_Run,	AIPROC_UI_Rebel_Common_Fin	},
	{	"UI_Swat_Common",	AIPROC_UI_Swat_Common_Run,	AIPROC_UI_Swat_Common_Fin	},
};

AIDriverKnife_13PV::AIDriverKnife_13PV(void)
{
	setAIProcTable( s_Knife_13PVTable, (sizeof(s_Knife_13PVTable)/sizeof(s_Knife_13PVTable[0])));
}

AIDriverKnife_13PV::~AIDriverKnife_13PV(void)
{
}
