#include "pch.h"
#include "AIDriverGun_13PV_DualSMG.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverGun_13PV_DualSMG);

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionActivate
// ANIMATION : Weapon/Uzi/Resource/Idle.i3a
//
static void AIPROC_ExtensionActivate_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_EXTENSION_ACTIVATE_RUN(pCtx, tm);
}

static void AIPROC_ExtensionActivate_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionDeactivate
// ANIMATION : Weapon/Uzi/Resource/Idle.i3a
//
static void AIPROC_ExtensionDeactivate_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_EXTENSION_DEACTIVATE_RUN(pCtx, tm);
}

static void AIPROC_ExtensionDeactivate_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle
// ANIMATION : Weapon/Scorpion_Vz61/Resource/Idle.i3a
//
static void AIPROC_Idle_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN(pCtx, tm);
}

static void AIPROC_Idle_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag3PV_DualGun_Left
// ANIMATION : Weapon/Scorpion_Vz61/Resource/Reload_Left.i3a
//
static void AIPROC_LoadMag3PV_DualGun_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN(pCtx, tm);
}

static void AIPROC_LoadMag3PV_DualGun_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire
// ANIMATION : Weapon/Scorpion_Vz61/Resource/Attack_Semi.i3a
//
static void AIPROC_Fire_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN(pCtx, tm);
}

static void AIPROC_Fire_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag1PV_DualGun_Left
// ANIMATION : Weapon/Scorpion_Vz61/Resource/Reload_Left.i3a
//
static void AIPROC_LoadMag1PV_DualGun_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN(pCtx, tm);
}

static void AIPROC_LoadMag1PV_DualGun_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// FireBurst
// ANIMATION : Weapon/Scorpion_Vz61/Resource/Attack_Burst.i3a
//
static void AIPROC_FireBurst_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN(pCtx, tm);
}

static void AIPROC_FireBurst_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag1PV_DualGun_Right
// ANIMATION : Weapon/Scorpion_Vz61/Resource/Reload_Right.i3a
//
static void AIPROC_LoadMag1PV_DualGun_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN(pCtx, tm);
}

static void AIPROC_LoadMag1PV_DualGun_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag3PV_DualGun_Right
// ANIMATION : Weapon/Scorpion_Vz61/Resource/Reload_Right.i3a
//
static void AIPROC_LoadMag3PV_DualGun_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN(pCtx, tm);
}

static void AIPROC_LoadMag3PV_DualGun_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Change1PV_DualGun_Right
// ANIMATION : Weapon/Scorpion_Vz61/Resource/Change_Right.i3a
//
static void AIPROC_Change1PV_DualGun_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN(pCtx, tm);
}

static void AIPROC_Change1PV_DualGun_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change3PV_DualGun_Left
// ANIMATION : Weapon/Scorpion_Vz61/Resource/Change_Left.i3a
//
static void AIPROC_Change3PV_DualGun_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN(pCtx, tm);
}

static void AIPROC_Change3PV_DualGun_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change3PV_DualGun_RIght
// ANIMATION : Weapon/Scorpion_Vz61/Resource/Change_Right.i3a
//
static void AIPROC_Change3PV_DualGun_RIght_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN(pCtx, tm);
}

static void AIPROC_Change3PV_DualGun_RIght_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change1PV_DualGun_Left
// ANIMATION : Weapon/Scorpion_Vz61/Resource/Change_Left.i3a
//
static void AIPROC_Change1PV_DualGun_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN(pCtx, tm);
}

static void AIPROC_Change1PV_DualGun_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
}



static i3::statictable::AISTATE_PROC s_Gun_13PV_DualSMGTable[] =
{
	{	"Change1PV_DualGun_Left",	AIPROC_Change1PV_DualGun_Left_Run,	AIPROC_Change1PV_DualGun_Left_Fin	},
	{	"Change1PV_DualGun_Right",	AIPROC_Change1PV_DualGun_Right_Run,	AIPROC_Change1PV_DualGun_Right_Fin	},
	{	"Change3PV_DualGun_RIght",	AIPROC_Change3PV_DualGun_RIght_Run,	AIPROC_Change3PV_DualGun_RIght_Fin	},
	{	"Change3PV_DualGun_Left",	AIPROC_Change3PV_DualGun_Left_Run,	AIPROC_Change3PV_DualGun_Left_Fin	},
	{	"ExtensionActivate",	AIPROC_ExtensionActivate_Run,	AIPROC_ExtensionActivate_Fin	},
	{	"ExtensionDeactivate",	AIPROC_ExtensionDeactivate_Run,	AIPROC_ExtensionDeactivate_Fin	},
	{	"Fire",	AIPROC_Fire_Run,	AIPROC_Fire_Fin	},
	{	"FireBurst",	AIPROC_FireBurst_Run,	AIPROC_FireBurst_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"LoadMag1PV_DualGun_Left",	AIPROC_LoadMag1PV_DualGun_Left_Run,	AIPROC_LoadMag1PV_DualGun_Left_Fin	},
	{	"LoadMag1PV_DualGun_Right",	AIPROC_LoadMag1PV_DualGun_Right_Run,	AIPROC_LoadMag1PV_DualGun_Right_Fin	},
	{	"LoadMag3PV_DualGun_Left",	AIPROC_LoadMag3PV_DualGun_Left_Run,	AIPROC_LoadMag3PV_DualGun_Left_Fin	},
	{	"LoadMag3PV_DualGun_Right",	AIPROC_LoadMag3PV_DualGun_Right_Run,	AIPROC_LoadMag3PV_DualGun_Right_Fin	},
};

AIDriverGun_13PV_DualSMG::AIDriverGun_13PV_DualSMG(void)
{
	setAIProcTable( s_Gun_13PV_DualSMGTable, (sizeof(s_Gun_13PV_DualSMGTable)/sizeof(s_Gun_13PV_DualSMGTable[0])) );
}

AIDriverGun_13PV_DualSMG::~AIDriverGun_13PV_DualSMG(void)
{
}
