#include "pch.h"
#include "AIDriverGun_13PV_GrenadeLauncher.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverGun_13PV_GrenadeLauncher);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// Change1PV
// ANIMATION : Weapon/M4A1_Scope2/Resource/Idle_Opend.i3a
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
// ANIMATION : Weapon/GrenadeLauncher_M4A1_A_Lv3/Resource/Idle.i3a
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
// ANIMATION : Weapon/GrenadeLauncher_M4A1_A_Lv3/Resource/Reload_Female_1PV.i3a
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
// Fire
// ANIMATION : Weapon/GrenadeLauncher_M4A1_A_Lv3/Resource/Attack.i3a
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
// LoadMag3PVMale
// ANIMATION : Weapon/GrenadeLauncher_M4A1_A_Lv3/Resource/Reload_Male_3PV.i3a
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
// ANIMATION : Weapon/M4A1_Scope2/Resource/Idle_Opend.i3a
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
// ANIMATION : Weapon/GrenadeLauncher_M4A1_A_Lv3/Resource/Reload_Master_1PV.i3a
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
// ANIMATION : Weapon/GrenadeLauncher_M4A1_A_Lv3/Resource/Reload_Master_3PV.i3a
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
// RemoveShell_1PV
// ANIMATION : Weapon/GrenadeLauncher_M4A1_A_Lv3/Resource/Reload_Grenade_A_1PV.i3a
//
static void AIPROC_RemoveShell_1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_REMOVE_SHELL_RUN( pCtx, tm);
}

static void AIPROC_RemoveShell_1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadShell_1PV
// ANIMATION : Weapon/GrenadeLauncher_M4A1_A_Lv3/Resource/Reload_Grenade_B_1PV.i3a
//
static void AIPROC_LoadShell_1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_RELOAD_SHELL_RUN( pCtx, tm);
}

static void AIPROC_LoadShell_1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// FireShell
// ANIMATION : Weapon/GrenadeLauncher_M4A1_A_Lv3/Resource/Attack.i3a
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
// RemoveShell_3PV
// ANIMATION : Weapon/GrenadeLauncher_M4A1_A_Lv3/Resource/Reload_Grenade_A_1PV.i3a
//
static void AIPROC_RemoveShell_3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_REMOVE_SHELL_RUN( pCtx, tm);
}

static void AIPROC_RemoveShell_3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadShell_3PV
// ANIMATION : Weapon/GrenadeLauncher_M4A1_A_Lv3/Resource/Reload_Grenade_B_3PV.i3a
//
static void AIPROC_LoadShell_3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_RELOAD_SHELL_RUN( pCtx, tm);
}

static void AIPROC_LoadShell_3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag3PVFemale
// ANIMATION : Weapon/GrenadeLauncher_M4A1_A_Lv3/Resource/Reload_Female_3PV.i3a
//
static void AIPROC_LoadMag3PVFemale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMag3PVFemale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag1PVMale
// ANIMATION : Weapon/GrenadeLauncher_M4A1_A_Lv3/Resource/Reload_Male_1PV.i3a
//
static void AIPROC_LoadMag1PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMag1PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm );
}



static i3::statictable::AISTATE_PROC s_Gun_13PV_GrenadeLauncherTable[] =
{
	{	"Change1PV",	AIPROC_Change1PV_Run,	AIPROC_Change1PV_Fin	},
	{	"Change3PV",	AIPROC_Change3PV_Run,	AIPROC_Change3PV_Fin	},
	{	"ExtensionActivate",	AIPROC_ExtensionActivate_Run,	AIPROC_ExtensionActivate_Fin	},
	{	"ExtensionDeactivate",	AIPROC_ExtensionDeactivate_Run,	AIPROC_ExtensionDeactivate_Fin	},
	{	"Fire",	AIPROC_Fire_Run,	AIPROC_Fire_Fin	},
	{	"FireShell",	AIPROC_FireShell_Run,	AIPROC_FireShell_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"LoadMag1PVFemale",	AIPROC_LoadMag1PVFemale_Run,	AIPROC_LoadMag1PVFemale_Fin	},
	{	"LoadMag1PVMale",	AIPROC_LoadMag1PVMale_Run,	AIPROC_LoadMag1PVMale_Fin	},
	{	"LoadMag3PVFemale",	AIPROC_LoadMag3PVFemale_Run,	AIPROC_LoadMag3PVFemale_Fin	},
	{	"LoadMag3PVMale",	AIPROC_LoadMag3PVMale_Run,	AIPROC_LoadMag3PVMale_Fin	},
	{	"LoadMagMaster1PV",	AIPROC_LoadMagMaster1PV_Run,	AIPROC_LoadMagMaster1PV_Fin	},
	{	"LoadMagMaster3PV",	AIPROC_LoadMagMaster3PV_Run,	AIPROC_LoadMagMaster3PV_Fin	},
	{	"LoadShell_1PV",	AIPROC_LoadShell_1PV_Run,	AIPROC_LoadShell_1PV_Fin	},
	{	"LoadShell_3PV",	AIPROC_LoadShell_3PV_Run,	AIPROC_LoadShell_3PV_Fin	},
	{	"RemoveShell_1PV",	AIPROC_RemoveShell_1PV_Run,	AIPROC_RemoveShell_1PV_Fin	},
	{	"RemoveShell_3PV",	AIPROC_RemoveShell_3PV_Run,	AIPROC_RemoveShell_3PV_Fin	},
};

AIDriverGun_13PV_GrenadeLauncher::AIDriverGun_13PV_GrenadeLauncher(void)
{
	setAIProcTable( s_Gun_13PV_GrenadeLauncherTable, (sizeof(s_Gun_13PV_GrenadeLauncherTable)/sizeof(s_Gun_13PV_GrenadeLauncherTable[0])) );
}

AIDriverGun_13PV_GrenadeLauncher::~AIDriverGun_13PV_GrenadeLauncher(void)
{
}
