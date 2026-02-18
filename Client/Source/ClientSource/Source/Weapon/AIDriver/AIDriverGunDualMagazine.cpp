#include "pch.h"
#include "AIDriverGunDualMagazine.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverGunDualMagazine);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// Idle_Right
// ANIMATION : WEAPON/AK-47/IDLE_LEFT.I3A
//
static void AIPROC_Idle_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm );
}

static void AIPROC_Idle_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{	
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet_Right
// ANIMATION : WEAPON/AK-47/IDLE_LEFT.I3A
//
static void AIPROC_LoadBullet_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_LoadBullet_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire_Right
// ANIMATION : WEAPON/AK-47/ATTACK_LEFT.I3A
//
static void AIPROC_Fire_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm );
}

static void AIPROC_Fire_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionActivate
// ANIMATION : WEAPON/AK-47/RELOAD_B.I3A
//
static void AIPROC_ExtensionActivate_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_EXTENSION_ACTIVATE_RUN( pCtx, tm );
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
// Idle
// ANIMATION : WEAPON/AK-47/IDLE_RIGHT.I3A
//
static void AIPROC_Idle_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm );
}

static void AIPROC_Idle_Fin( i3AIContext * pCtx, REAL32 tm)
{	
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet
// ANIMATION : WEAPON/AK-47/IDLE_RIGHT.I3A
//
static void AIPROC_LoadBullet_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_LoadBullet_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_RightAll
// ANIMATION : WEAPON/AK-47/RELOAD_C.I3A
//
static void AIPROC_LoadMag_RightAll_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMag_RightAll_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire
// ANIMATION : WEAPON/AK-47/ATTACK_RIGHT.I3A
//
static void AIPROC_Fire_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm );
}

static void AIPROC_Fire_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag
// ANIMATION : WEAPON/AK-47/RELOAD_A.I3A
//
static void AIPROC_LoadMag_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMag_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_Right
// ANIMATION : WEAPON/AK-47/RELOAD_B.I3A
//
static void AIPROC_LoadMag_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_QUICK_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMag_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_QUICK_LOAD_MAGAZINE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_RightShift
// ANIMATION : WEAPON/AK-47/RELOAD_D.I3A
//
static void AIPROC_LoadMag_RightShift_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_QUICK_LOAD_MAGAZINE_RUN( pCtx,tm );
}

static void AIPROC_LoadMag_RightShift_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_QUICK_LOAD_MAGAZINE_FIN( pCtx, tm );
}


////////////////////////////////////////////////////////////////////////////////
//
// LoadMagMaster1PV
// ANIMATION : Weapon/AK-47/Reload_A_Master_1PV.i3a
//
static void AIPROC_LoadMagMaster1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMagMaster1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagMaster3PV
// ANIMATION : Weapon/AK-47/Reload_A_Master_3PV.i3a
//
static void AIPROC_LoadMagMaster3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMagMaster3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_RightAllMaster3PV
// ANIMATION : Weapon/AK-47/Reload_D_Master_3PV.i3a
//
static void AIPROC_LoadMag_RightAllMaster3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMag_RightAllMaster3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_RightAllMaster1PV
// ANIMATION : Weapon/AK-47/Reload_D_Master_1PV.i3a
//
static void AIPROC_LoadMag_RightAllMaster1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMag_RightAllMaster1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm );
}


static i3::statictable::AISTATE_PROC s_GunDualMagazineTable[] =
{
	{	"ExtensionActivate",	AIPROC_ExtensionActivate_Run,	AIPROC_ExtensionActivate_Fin	},
	{	"ExtensionDeactivate",	AIPROC_ExtensionDeactivate_Run,	AIPROC_ExtensionDeactivate_Fin	},
	{	"Fire",	AIPROC_Fire_Run,	AIPROC_Fire_Fin	},
	{	"Fire_Right",	AIPROC_Fire_Right_Run,	AIPROC_Fire_Right_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"Idle_Right",	AIPROC_Idle_Right_Run,	AIPROC_Idle_Right_Fin	},
	{	"LoadBullet",	AIPROC_LoadBullet_Run,	AIPROC_LoadBullet_Fin	},
	{	"LoadBullet_Right",	AIPROC_LoadBullet_Right_Run,	AIPROC_LoadBullet_Right_Fin	},
	{	"LoadMag",	AIPROC_LoadMag_Run,	AIPROC_LoadMag_Fin	},
	{	"LoadMag_Right",	AIPROC_LoadMag_Right_Run,	AIPROC_LoadMag_Right_Fin	},
	{	"LoadMag_RightAll",	AIPROC_LoadMag_RightAll_Run,	AIPROC_LoadMag_RightAll_Fin	},
	{	"LoadMag_RightAllMaster1PV",	AIPROC_LoadMag_RightAllMaster1PV_Run,	AIPROC_LoadMag_RightAllMaster1PV_Fin	},
	{	"LoadMag_RightAllMaster3PV",	AIPROC_LoadMag_RightAllMaster3PV_Run,	AIPROC_LoadMag_RightAllMaster3PV_Fin	},
	{	"LoadMag_RightShift",	AIPROC_LoadMag_RightShift_Run,	AIPROC_LoadMag_RightShift_Fin	},
	{	"LoadMagB",	AIPROC_LoadMagB_Run,	AIPROC_LoadMagB_Fin	},
	{	"LoadMagMaster1PV",	AIPROC_LoadMagMaster1PV_Run,	AIPROC_LoadMagMaster1PV_Fin	},
	{	"LoadMagMaster3PV",	AIPROC_LoadMagMaster3PV_Run,	AIPROC_LoadMagMaster3PV_Fin	},
};

AIDriverGunDualMagazine::AIDriverGunDualMagazine(void)
{
	setAIProcTable( s_GunDualMagazineTable, (sizeof(s_GunDualMagazineTable)/sizeof(s_GunDualMagazineTable[0])));
}

AIDriverGunDualMagazine::~AIDriverGunDualMagazine(void)
{
}
