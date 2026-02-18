#include "pch.h"
#include "AIDriverGunGrenadeLauncher.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverGunGrenadeLauncher);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// Idle
// ANIMATION : Weapon/K201/Resource/IDLE.I3A
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
// LoadBullet
// ANIMATION : Weapon/K201/Resource/IDLE.I3A
//
static void AIPROC_LoadBullet_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadBullet_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag3PVMale
// ANIMATION : Weapon/K201/Resource/Reload_Male_3PV.i3a
//
static void AIPROC_LoadMag3PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag3PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire
// ANIMATION : Weapon/K201/Resource/ATTACK.I3A
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
// ExtensionActivate
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
// RemoveShell
// ANIMATION : Weapon/K201/Resource/RELOAD_GRENADE_A.I3A
//
static void AIPROC_RemoveShell_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_REMOVE_SHELL_RUN( pCtx, tm);
}

static void AIPROC_RemoveShell_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadShell
// ANIMATION : Weapon/K201/Resource/RELOAD_GRENADE_B.I3A
//
static void AIPROC_LoadShell_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_RELOAD_SHELL_RUN( pCtx, tm);
}

static void AIPROC_LoadShell_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// FireShell
// ANIMATION : Weapon/K201/Resource/IDLE.I3A
//
static void AIPROC_FireShell_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_SHELL_RUN( pCtx, tm);
}

static void AIPROC_FireShell_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagMaster1PV
// ANIMATION : Weapon/K201/Resource/Reload_Master_1PV.i3a
//
static void AIPROC_LoadMagMaster1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMagMaster1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagMaster3PV
// ANIMATION : Weapon/K201/Resource/Reload_Master_3PV.i3a
//
static void AIPROC_LoadMagMaster3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMagMaster3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag1PVFemale
// ANIMATION : Weapon/K201/Resource/Reload_Female_1PV.i3a
//
static void AIPROC_LoadMag1PVFemale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag1PVFemale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag3PVFemale
// ANIMATION : Weapon/K201/Resource/Reload_Female_3PV.i3a
//
static void AIPROC_LoadMag3PVFemale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag3PVFemale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag1PVMale
// ANIMATION : Weapon/K201/Resource/Reload_Male_1PV.i3a
//
static void AIPROC_LoadMag1PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag1PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}



static i3::statictable::AISTATE_PROC s_GunGrenadeLauncherTable[] =
{
	{	"ExtensionActivate",	AIPROC_ExtensionActivate_Run,	AIPROC_ExtensionActivate_Fin	},
	{	"ExtensionDeactivate",	AIPROC_ExtensionDeactivate_Run,	AIPROC_ExtensionDeactivate_Fin	},
	{	"Fire",	AIPROC_Fire_Run,	AIPROC_Fire_Fin	},
	{	"FireShell",	AIPROC_FireShell_Run,	AIPROC_FireShell_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"LoadBullet",	AIPROC_LoadBullet_Run,	AIPROC_LoadBullet_Fin	},
	{	"LoadMag1PVFemale",	AIPROC_LoadMag1PVFemale_Run,	AIPROC_LoadMag1PVFemale_Fin	},
	{	"LoadMag1PVMale",	AIPROC_LoadMag1PVMale_Run,	AIPROC_LoadMag1PVMale_Fin	},
	{	"LoadMag3PVFemale",	AIPROC_LoadMag3PVFemale_Run,	AIPROC_LoadMag3PVFemale_Fin	},
	{	"LoadMag3PVMale",	AIPROC_LoadMag3PVMale_Run,	AIPROC_LoadMag3PVMale_Fin	},
	{	"LoadMagMaster1PV",	AIPROC_LoadMagMaster1PV_Run,	AIPROC_LoadMagMaster1PV_Fin	},
	{	"LoadMagMaster3PV",	AIPROC_LoadMagMaster3PV_Run,	AIPROC_LoadMagMaster3PV_Fin	},
	{	"LoadShell",	AIPROC_LoadShell_Run,	AIPROC_LoadShell_Fin	},
	{	"RemoveShell",	AIPROC_RemoveShell_Run,	AIPROC_RemoveShell_Fin	},
};

AIDriverGunGrenadeLauncher::AIDriverGunGrenadeLauncher(void)
{
	setAIProcTable( s_GunGrenadeLauncherTable, (sizeof(s_GunGrenadeLauncherTable)/sizeof(s_GunGrenadeLauncherTable[0])) );
}

AIDriverGunGrenadeLauncher::~AIDriverGunGrenadeLauncher(void)
{
}
