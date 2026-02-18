#include "pch.h"
#include "AIDriverGun_13PV_WithUI.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverGun_13PV_WithUI);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// Change1PV
// ANIMATION : Weapon/Glock18_Ex/Resource/Change.i3a
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
// ANIMATION : Weapon/Glock18_Ex/Resource/Idle.i3a
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
// ANIMATION : Weapon/Glock18_Ex/Resource/Idle.i3a
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
// ANIMATION : Weapon/Glock18_Ex/Resource/Reload_3PV.i3a
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
// ANIMATION : Weapon/Glock18_Ex/Resource/Attack_Semi.i3a
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
// ANIMATION : Weapon/Glock18_Ex/Resource/Reload_1PV.i3a
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
// Change3PV
// ANIMATION : Weapon/Glock18_Ex/Resource/Change.i3a
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
// ANIMATION : Weapon/Glock18_Ex/Resource/Idle.i3a
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
// UI_IdleB_SWAT_Female
// ANIMATION : Weapon/Glock18_Ex/Resource/UI_IdleB_SWAT_Female.i3a
//
static void AIPROC_UI_IdleB_SWAT_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_IdleB_SWAT_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_IdleB_SWAT_Male
// ANIMATION : Weapon/Glock18_Ex/Resource/UI_IdleB_SWAT_Male.i3a
//
static void AIPROC_UI_IdleB_SWAT_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_IdleB_SWAT_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_IdleB_REBEL_Female
// ANIMATION : Weapon/Glock18_Ex/Resource/Idle_UI.i3a
//
static void AIPROC_UI_IdleB_REBEL_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_IdleB_REBEL_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_IdleB_REBEL_Male
// ANIMATION : Weapon/Glock18_Ex/Resource/Idle_UI.i3a
//
static void AIPROC_UI_IdleB_REBEL_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_IdleB_REBEL_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_IdleA_SWAT_Female
// ANIMATION : Weapon/Glock18_Ex/Resource/Idle_UI.i3a
//
static void AIPROC_UI_IdleA_SWAT_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_IdleA_SWAT_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_IdleA_SWAT_Male
// ANIMATION : Weapon/Glock18_Ex/Resource/Idle_UI.i3a
//
static void AIPROC_UI_IdleA_SWAT_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_IdleA_SWAT_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_IdleA_REBEL_Female
// ANIMATION : Weapon/Glock18_Ex/Resource/Idle_UI.i3a
//
static void AIPROC_UI_IdleA_REBEL_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_IdleA_REBEL_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_IdleA_REBEL_Male
// ANIMATION : Weapon/Glock18_Ex/Resource/Idle_UI.i3a
//
static void AIPROC_UI_IdleA_REBEL_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_IdleA_REBEL_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
}



static i3::statictable::AISTATE_PROC s_Gun_13PV_WithUITable[] =
{
	{	"Change1PV",	AIPROC_Change1PV_Run,	AIPROC_Change1PV_Fin	},
	{	"Change3PV",	AIPROC_Change3PV_Run,	AIPROC_Change3PV_Fin	},
	{	"ExtensionActivate",	AIPROC_ExtensionActivate_Run,	AIPROC_ExtensionActivate_Fin	},
	{	"ExtensionDeactivate",	AIPROC_ExtensionDeactivate_Run,	AIPROC_ExtensionDeactivate_Fin	},
	{	"Fire",	AIPROC_Fire_Run,	AIPROC_Fire_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"LoadMag1PV",	AIPROC_LoadMag1PV_Run,	AIPROC_LoadMag1PV_Fin	},
	{	"LoadMag3PV",	AIPROC_LoadMag3PV_Run,	AIPROC_LoadMag3PV_Fin	},
	{	"UI_IdleA_REBEL_Female",	AIPROC_UI_IdleA_REBEL_Female_Run,	AIPROC_UI_IdleA_REBEL_Female_Fin	},
	{	"UI_IdleA_REBEL_Male",	AIPROC_UI_IdleA_REBEL_Male_Run,	AIPROC_UI_IdleA_REBEL_Male_Fin	},
	{	"UI_IdleA_SWAT_Female",	AIPROC_UI_IdleA_SWAT_Female_Run,	AIPROC_UI_IdleA_SWAT_Female_Fin	},
	{	"UI_IdleA_SWAT_Male",	AIPROC_UI_IdleA_SWAT_Male_Run,	AIPROC_UI_IdleA_SWAT_Male_Fin	},
	{	"UI_IdleB_REBEL_Female",	AIPROC_UI_IdleB_REBEL_Female_Run,	AIPROC_UI_IdleB_REBEL_Female_Fin	},
	{	"UI_IdleB_REBEL_Male",	AIPROC_UI_IdleB_REBEL_Male_Run,	AIPROC_UI_IdleB_REBEL_Male_Fin	},
	{	"UI_IdleB_SWAT_Female",	AIPROC_UI_IdleB_SWAT_Female_Run,	AIPROC_UI_IdleB_SWAT_Female_Fin	},
	{	"UI_IdleB_SWAT_Male",	AIPROC_UI_IdleB_SWAT_Male_Run,	AIPROC_UI_IdleB_SWAT_Male_Fin	},
};

AIDriverGun_13PV_WithUI::AIDriverGun_13PV_WithUI(void)
{
	setAIProcTable( s_Gun_13PV_WithUITable, (sizeof(s_Gun_13PV_WithUITable)/sizeof(s_Gun_13PV_WithUITable[0])));
}

AIDriverGun_13PV_WithUI::~AIDriverGun_13PV_WithUI(void)
{
}
