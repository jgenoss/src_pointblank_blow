#include "pch.h"
#include "AIDriverGun_13PV_DualGun.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverGun_13PV_DualGun);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionActivate
// ANIMATION : WEAPON/DESERTEAGLE/IDLE.I3A
//
static void AIPROC_ExtensionActivate_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_EXTENSION_ACTIVATE_RUN( pCtx, tm);
}

static void AIPROC_ExtensionActivate_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionDeactivate
// ANIMATION : WEAPON/DESERTEAGLE/IDLE.I3A
//
static void AIPROC_ExtensionDeactivate_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_EXTENSION_DEACTIVATE_RUN( pCtx, tm);
}

static void AIPROC_ExtensionDeactivate_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle
// ANIMATION : WEAPON/DESERTEAGLE/IDLE.I3A
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
// LoadMag3PV
// ANIMATION : WEAPON/DESERTEAGLE/RELOAD.I3A
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
// Fire
// ANIMATION : WEAPON/DESERTEAGLE/ATTACK.I3A
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
// LoadMag1PV
// ANIMATION : WEAPON/DESERTEAGLE/RELOAD.I3A
//
static void AIPROC_LoadMag1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag1PV_DualGun_Left
// ANIMATION : Weapon/DesertEagle/Dual_Reload_Left_1PV.i3a
//
static void AIPROC_LoadMag1PV_DualGun_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag1PV_DualGun_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag3PV_DualGun_Left
// ANIMATION : Weapon/DesertEagle/Dual_Reload_Left_3PV.i3a
//
static void AIPROC_LoadMag3PV_DualGun_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag3PV_DualGun_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag1PV_DualGun_Right
// ANIMATION : Weapon/DesertEagle/Dual_Reload_Right_1PV.i3a
//
static void AIPROC_LoadMag1PV_DualGun_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag1PV_DualGun_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag3PV_DualGun_Right
// ANIMATION : Weapon/DesertEagle/Dual_Reload_Right_3PV.i3a
//
static void AIPROC_LoadMag3PV_DualGun_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag3PV_DualGun_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}



static i3::statictable::AISTATE_PROC s_Gun_13PV_DualGunTable[] =
{
	{	"ExtensionActivate",	AIPROC_ExtensionActivate_Run,	AIPROC_ExtensionActivate_Fin	},
	{	"ExtensionDeactivate",	AIPROC_ExtensionDeactivate_Run,	AIPROC_ExtensionDeactivate_Fin	},
	{	"Fire",	AIPROC_Fire_Run,	AIPROC_Fire_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"LoadMag1PV",	AIPROC_LoadMag1PV_Run,	AIPROC_LoadMag1PV_Fin	},
	{	"LoadMag1PV_DualGun_Left",	AIPROC_LoadMag1PV_DualGun_Left_Run,	AIPROC_LoadMag1PV_DualGun_Left_Fin	},
	{	"LoadMag1PV_DualGun_Right",	AIPROC_LoadMag1PV_DualGun_Right_Run,	AIPROC_LoadMag1PV_DualGun_Right_Fin	},
	{	"LoadMag3PV",	AIPROC_LoadMag3PV_Run,	AIPROC_LoadMag3PV_Fin	},
	{	"LoadMag3PV_DualGun_Left",	AIPROC_LoadMag3PV_DualGun_Left_Run,	AIPROC_LoadMag3PV_DualGun_Left_Fin	},
	{	"LoadMag3PV_DualGun_Right",	AIPROC_LoadMag3PV_DualGun_Right_Run,	AIPROC_LoadMag3PV_DualGun_Right_Fin	},
};

AIDriverGun_13PV_DualGun::AIDriverGun_13PV_DualGun(void)
{
	setAIProcTable( s_Gun_13PV_DualGunTable, (sizeof(s_Gun_13PV_DualGunTable)/sizeof(s_Gun_13PV_DualGunTable[0])));
}

AIDriverGun_13PV_DualGun::~AIDriverGun_13PV_DualGun(void)
{
}
