#include "pch.h"
#include "AIDriverGun_13PV_FoldedGun_OA.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverGun_13PV_FoldedGun_OA);

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionActivate
// ANIMATION : Weapon/OA-93/Resource/Idle.i3a
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
// ANIMATION : Weapon/OA-93/Resource/Idle.i3a
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
// ANIMATION : Weapon/OA-93/Resource/Idle.i3a
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
// ANIMATION : Weapon/OA-93/Resource/Reload_3PV.i3a
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
// ANIMATION : Weapon/OA-93/Resource/Idle.i3a
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
// ANIMATION : Weapon/OA-93/Resource/Reload_1PV.i3a
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
// Fire_Folded
// ANIMATION : Weapon/OA-93/Resource/Idle_Folded.i3a
//
static void AIPROC_Fire_Folded_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_Fire_Folded_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle_Folded
// ANIMATION : Weapon/OA-93/Resource/Idle_Folded.i3a
//
static void AIPROC_Idle_Folded_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm);
}

static void AIPROC_Idle_Folded_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag1PV_Folded
// ANIMATION : Weapon/OA-93/Resource/Reload_1PV.i3a
//
static void AIPROC_LoadMag1PV_Folded_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag1PV_Folded_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag3PV_Folded
// ANIMATION : Weapon/OA-93/Resource/Reload_3PV.i3a
//
static void AIPROC_LoadMag3PV_Folded_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_LoadMag3PV_Folded_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle_Folded_Change
// ANIMATION : Weapon/OA-93/Resource/Change.i3a
//
static void AIPROC_Idle_Folded_Change_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN( pCtx, tm);
}

static void AIPROC_Idle_Folded_Change_Fin( i3AIContext * pCtx, REAL32 tm)
{
}



static i3::statictable::AISTATE_PROC s_Gun_13PV_FoldedGun_OATable[] =
{
	{	"ExtensionActivate",	AIPROC_ExtensionActivate_Run,	AIPROC_ExtensionActivate_Fin	},
	{	"ExtensionDeactivate",	AIPROC_ExtensionDeactivate_Run,	AIPROC_ExtensionDeactivate_Fin	},
	{	"Fire",	AIPROC_Fire_Run,	AIPROC_Fire_Fin	},
	{	"Fire_Folded",	AIPROC_Fire_Folded_Run,	AIPROC_Fire_Folded_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"Idle_Folded",	AIPROC_Idle_Folded_Run,	AIPROC_Idle_Folded_Fin	},
	{	"Idle_Folded_Change",	AIPROC_Idle_Folded_Change_Run,	AIPROC_Idle_Folded_Change_Fin	},
	{	"LoadMag1PV",	AIPROC_LoadMag1PV_Run,	AIPROC_LoadMag1PV_Fin	},
	{	"LoadMag1PV_Folded",	AIPROC_LoadMag1PV_Folded_Run,	AIPROC_LoadMag1PV_Folded_Fin	},
	{	"LoadMag3PV",	AIPROC_LoadMag3PV_Run,	AIPROC_LoadMag3PV_Fin	},
	{	"LoadMag3PV_Folded",	AIPROC_LoadMag3PV_Folded_Run,	AIPROC_LoadMag3PV_Folded_Fin	},
};

AIDriverGun_13PV_FoldedGun_OA::AIDriverGun_13PV_FoldedGun_OA(void)
{
	setAIProcTable( s_Gun_13PV_FoldedGun_OATable, (sizeof(s_Gun_13PV_FoldedGun_OATable)/sizeof(s_Gun_13PV_FoldedGun_OATable[0])) );
}

AIDriverGun_13PV_FoldedGun_OA::~AIDriverGun_13PV_FoldedGun_OA(void)
{
}
