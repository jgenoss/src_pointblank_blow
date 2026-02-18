#include "pch.h"
#include "AIDriverGunSniper_DualMagazine.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverGunSniper_DualMagazine);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// Fire
// ANIMATION : Weapon/DSR_1/Resource/Idle.i3a
//
static void AIPROC_Fire_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_Fire_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagB
//
static void AIPROC_LoadMagB_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_LoadMagB_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet1PVFemale
// ANIMATION : Weapon/DSR_1/Resource/ReloadC_Female_1PV.i3a
//
static void AIPROC_LoadBullet1PVFemale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadBullet1PVFemale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle
// ANIMATION : Weapon/DSR_1/Resource/Idle.i3a
//
static void AIPROC_Idle_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm);
}

static void AIPROC_Idle_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionActivate
//
static void AIPROC_ExtensionActivate_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_ExtensionActivate_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionDeactivate
//
static void AIPROC_ExtensionDeactivate_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_ExtensionDeactivate_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag3PVSwapShell
// ANIMATION : Weapon/DSR_1/Resource/ReloadA_3PV.i3a
//
static void AIPROC_LoadMag3PVSwapShell_Run( i3AIContext * pCtx, REAL32 tm)
{
		AIPROC_WEAPON_QUICK_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMag3PVSwapShell_Fin( i3AIContext * pCtx, REAL32 tm)
{
		AIPROC_WEAPON_QUICK_LOAD_MAGAZINE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet1PVMale
// ANIMATION : Weapon/DSR_1/Resource/ReloadC_Male_1PV.i3a
//
static void AIPROC_LoadBullet1PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadBullet1PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag1PVSwapShell
// ANIMATION : Weapon/DSR_1/Resource/ReloadA_1PV.i3a
//
static void AIPROC_LoadMag1PVSwapShell_Run( i3AIContext * pCtx, REAL32 tm)
{
		AIPROC_WEAPON_QUICK_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMag1PVSwapShell_Fin( i3AIContext * pCtx, REAL32 tm)
{
		AIPROC_WEAPON_QUICK_LOAD_MAGAZINE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet3PVFemale
// ANIMATION : Weapon/DSR_1/Resource/ReloadC_Female_3PV.i3a
//
static void AIPROC_LoadBullet3PVFemale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadBullet3PVFemale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet3PVMale
// ANIMATION : Weapon/DSR_1/Resource/ReloadC_Male_3PV.i3a
//
static void AIPROC_LoadBullet3PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadBullet3PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag3PV
// ANIMATION : Weapon/DSR_1/Resource/ReloadB_3PV.i3a
//
static void AIPROC_LoadMag3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag1PV
// ANIMATION : Weapon/DSR_1/Resource/ReloadB_1PV.i3a
//
static void AIPROC_LoadMag1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}



static i3::statictable::AISTATE_PROC s_GunSniper_DualMagazineTable[] =
{
	{	"ExtensionActivate",	AIPROC_ExtensionActivate_Run,	AIPROC_ExtensionActivate_Fin	},
	{	"ExtensionDeactivate",	AIPROC_ExtensionDeactivate_Run,	AIPROC_ExtensionDeactivate_Fin	},
	{	"Fire",	AIPROC_Fire_Run,	AIPROC_Fire_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"LoadBullet1PVFemale",	AIPROC_LoadBullet1PVFemale_Run,	AIPROC_LoadBullet1PVFemale_Fin	},
	{	"LoadBullet1PVMale",	AIPROC_LoadBullet1PVMale_Run,	AIPROC_LoadBullet1PVMale_Fin	},
	{	"LoadBullet3PVFemale",	AIPROC_LoadBullet3PVFemale_Run,	AIPROC_LoadBullet3PVFemale_Fin	},
	{	"LoadBullet3PVMale",	AIPROC_LoadBullet3PVMale_Run,	AIPROC_LoadBullet3PVMale_Fin	},
	{	"LoadMag1PV",	AIPROC_LoadMag1PV_Run,	AIPROC_LoadMag1PV_Fin	},
	{	"LoadMag1PVSwapShell",	AIPROC_LoadMag1PVSwapShell_Run,	AIPROC_LoadMag1PVSwapShell_Fin	},
	{	"LoadMag3PV",	AIPROC_LoadMag3PV_Run,	AIPROC_LoadMag3PV_Fin	},
	{	"LoadMag3PVSwapShell",	AIPROC_LoadMag3PVSwapShell_Run,	AIPROC_LoadMag3PVSwapShell_Fin	},
	{	"LoadMagB",	AIPROC_LoadMagB_Run,	AIPROC_LoadMagB_Fin	},
};

AIDriverGunSniper_DualMagazine::AIDriverGunSniper_DualMagazine(void)
{
	setAIProcTable( s_GunSniper_DualMagazineTable, (sizeof(s_GunSniper_DualMagazineTable)/sizeof(s_GunSniper_DualMagazineTable[0])) );
}

AIDriverGunSniper_DualMagazine::~AIDriverGunSniper_DualMagazine(void)
{
}
