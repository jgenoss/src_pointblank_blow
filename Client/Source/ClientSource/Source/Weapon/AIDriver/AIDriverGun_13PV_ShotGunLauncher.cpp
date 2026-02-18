#include "pch.h"
#include "AIDriverGun_13PV_ShotGunLauncher.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverGun_13PV_ShotGunLauncher);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// Change1PV
// ANIMATION : Weapon/LSS_M4A1_C_Lv3/Resource/Idle.i3a
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
// Idle
// ANIMATION : Weapon/LSS_M4A1_C_Lv3/Resource/Idle.i3a
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
// LoadMag1PVFemale
// ANIMATION : Weapon/LSS_M4A1_C_Lv3/Resource/Reload_Female_1PV.i3a
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
// Fire1PV
// ANIMATION : Weapon/LSS_M4A1_C_Lv3/Resource/Attack.i3a
//
static void AIPROC_Fire1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_Fire1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag3PVMale
// ANIMATION : Weapon/LSS_M4A1_C_Lv3/Resource/Reload_Male_3PV.i3a
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
// Change3PV
// ANIMATION : Weapon/LSS_M4A1_C_Lv3/Resource/Idle.i3a
//
static void AIPROC_Change3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN( pCtx, tm);
}

static void AIPROC_Change3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
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
// LoadMagMaster1PV
// ANIMATION : Weapon/LSS_M4A1_C_Lv3/Resource/Reload_Master_1PV.i3a
//
static void AIPROC_LoadMagMaster1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMagMaster1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagMaster3PV
// ANIMATION : Weapon/LSS_M4A1_C_Lv3/Resource/Reload_Master_3PV.i3a
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
// FireShell1PV
// ANIMATION : Weapon/LSS_M4A1_C_Lv3/Resource/Attack_LSS_1PV.i3a
//
static void AIPROC_FireShell1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_SHELL_RUN( pCtx, tm);
}

static void AIPROC_FireShell1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_SHELL_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire3PV
// ANIMATION : Weapon/LSS_M4A1_C_Lv3/Resource/Attack.i3a
//
static void AIPROC_Fire3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_Fire3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// FireShell3PV
// ANIMATION : Weapon/LSS_M4A1_C_Lv3/Resource/Attack_LSS_3PV.i3a
//
static void AIPROC_FireShell3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_SHELL_RUN( pCtx, tm);
}

static void AIPROC_FireShell3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_SHELL_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadShell1PV
// ANIMATION : Weapon/LSS_M4A1_C_Lv3/Resource/Reload_LSS_1PV.i3a
//
static void AIPROC_LoadShell1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_RELOAD_SHELL_RUN( pCtx, tm);
}

static void AIPROC_LoadShell1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_RELOAD_SHELL_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadShell3PV
// ANIMATION : Weapon/LSS_M4A1_C_Lv3/Resource/Reload_LSS_3PV.i3a
//
static void AIPROC_LoadShell3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_RELOAD_SHELL_RUN( pCtx, tm);
}

static void AIPROC_LoadShell3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_RELOAD_SHELL_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag3PVFemale
// ANIMATION : Weapon/LSS_M4A1_C_Lv3/Resource/Reload_Female_3PV.i3a
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
// ANIMATION : Weapon/LSS_M4A1_C_Lv3/Resource/Reload_Male_1PV.i3a
//
static void AIPROC_LoadMag1PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag1PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}



static i3::statictable::AISTATE_PROC s_Gun_13PV_ShotGunLauncherTable[] =
{
	{	"Change1PV",	AIPROC_Change1PV_Run,	AIPROC_Change1PV_Fin	},
	{	"Change3PV",	AIPROC_Change3PV_Run,	AIPROC_Change3PV_Fin	},
	{	"ExtensionActivate",	AIPROC_ExtensionActivate_Run,	AIPROC_ExtensionActivate_Fin	},
	{	"ExtensionDeactivate",	AIPROC_ExtensionDeactivate_Run,	AIPROC_ExtensionDeactivate_Fin	},
	{	"Fire1PV",	AIPROC_Fire1PV_Run,	AIPROC_Fire1PV_Fin	},
	{	"Fire3PV",	AIPROC_Fire3PV_Run,	AIPROC_Fire3PV_Fin	},
	{	"FireShell1PV",	AIPROC_FireShell1PV_Run,	AIPROC_FireShell1PV_Fin	},
	{	"FireShell3PV",	AIPROC_FireShell3PV_Run,	AIPROC_FireShell3PV_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"LoadMag1PVFemale",	AIPROC_LoadMag1PVFemale_Run,	AIPROC_LoadMag1PVFemale_Fin	},
	{	"LoadMag1PVMale",	AIPROC_LoadMag1PVMale_Run,	AIPROC_LoadMag1PVMale_Fin	},
	{	"LoadMag3PVFemale",	AIPROC_LoadMag3PVFemale_Run,	AIPROC_LoadMag3PVFemale_Fin	},
	{	"LoadMag3PVMale",	AIPROC_LoadMag3PVMale_Run,	AIPROC_LoadMag3PVMale_Fin	},
	{	"LoadMagMaster1PV",	AIPROC_LoadMagMaster1PV_Run,	AIPROC_LoadMagMaster1PV_Fin	},
	{	"LoadMagMaster3PV",	AIPROC_LoadMagMaster3PV_Run,	AIPROC_LoadMagMaster3PV_Fin	},
	{	"LoadShell1PV",	AIPROC_LoadShell1PV_Run,	AIPROC_LoadShell1PV_Fin	},
	{	"LoadShell3PV",	AIPROC_LoadShell3PV_Run,	AIPROC_LoadShell3PV_Fin	},
};

AIDriverGun_13PV_ShotGunLauncher::AIDriverGun_13PV_ShotGunLauncher(void)
{
	setAIProcTable( s_Gun_13PV_ShotGunLauncherTable, (sizeof(s_Gun_13PV_ShotGunLauncherTable)/sizeof(s_Gun_13PV_ShotGunLauncherTable[0])) );
}

AIDriverGun_13PV_ShotGunLauncher::~AIDriverGun_13PV_ShotGunLauncher(void)
{
}
