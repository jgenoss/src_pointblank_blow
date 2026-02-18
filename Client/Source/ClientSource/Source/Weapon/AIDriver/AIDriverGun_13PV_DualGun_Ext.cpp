#include "pch.h"
#include "AIDriverGun_13PV_DualGun_Ext.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverGun_13PV_DualGun_Ext);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionActivate
// ANIMATION : Weapon/HK45_Dual/Resource/Idle.i3a
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
// ANIMATION : Weapon/HK45_Dual/Resource/Idle.i3a
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
// ANIMATION : Weapon/HK45_Dual/Resource/Idle.i3a
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
// Fire
// ANIMATION : Weapon/HK45_Dual/Resource/Attack.i3a
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
// LoadMag1PV_DualGun_Left
// ANIMATION : Weapon/HK45_Dual/Resource/Reload_Left_1PV.i3a
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
// ANIMATION : Weapon/HK45_Dual/Resource/Reload_Left_3PV.i3a
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
// ANIMATION : Weapon/HK45_Dual/Resource/Reload_Right_1PV.i3a
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
// ANIMATION : Weapon/HK45_Dual/Resource/Reload_Right_3PV.i3a
//
static void AIPROC_LoadMag3PV_DualGun_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag3PV_DualGun_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Change3PV_DualGun_Left
// ANIMATION : Weapon/HK45_Dual/Resource/Change_Left_3PV.i3a
//
static void AIPROC_Change3PV_DualGun_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_Change3PV_DualGun_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change3PV_DualGun_Right
// ANIMATION : Weapon/HK45_Dual/Resource/Change_Right_3PV.i3a
//
static void AIPROC_Change3PV_DualGun_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_Change3PV_DualGun_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change1PV_DualGun_Left
// ANIMATION : Weapon/HK45_Dual/Resource/Change_Left_1PV.i3a
//
static void AIPROC_Change1PV_DualGun_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN( pCtx, tm);
}

static void AIPROC_Change1PV_DualGun_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change1PV_DualGun_Right
// ANIMATION : Weapon/HK45_Dual/Resource/Change_Right_1PV.i3a
//
static void AIPROC_Change1PV_DualGun_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN( pCtx, tm);
}

static void AIPROC_Change1PV_DualGun_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire_Ext
// ANIMATION : Weapon/HK45_Dual/Resource/Ext_Attack.i3a
//
static void AIPROC_Fire_Ext_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_Fire_Ext_Fin( i3AIContext * pCtx, REAL32 tm)
{
}



static i3::statictable::AISTATE_PROC s_Gun_13PV_DualGun_ExtTable[] =
{
	{	"Change1PV_DualGun_Left",	AIPROC_Change1PV_DualGun_Left_Run,	AIPROC_Change1PV_DualGun_Left_Fin	},
	{	"Change1PV_DualGun_Right",	AIPROC_Change1PV_DualGun_Right_Run,	AIPROC_Change1PV_DualGun_Right_Fin	},
	{	"Change3PV_DualGun_Left",	AIPROC_Change3PV_DualGun_Left_Run,	AIPROC_Change3PV_DualGun_Left_Fin	},
	{	"Change3PV_DualGun_Right",	AIPROC_Change3PV_DualGun_Right_Run,	AIPROC_Change3PV_DualGun_Right_Fin	},
	{	"ExtensionActivate",	AIPROC_ExtensionActivate_Run,	AIPROC_ExtensionActivate_Fin	},
	{	"ExtensionDeactivate",	AIPROC_ExtensionDeactivate_Run,	AIPROC_ExtensionDeactivate_Fin	},
	{	"Fire",	AIPROC_Fire_Run,	AIPROC_Fire_Fin	},
	{	"Fire_Ext",	AIPROC_Fire_Ext_Run,	AIPROC_Fire_Ext_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"LoadMag1PV_DualGun_Left",	AIPROC_LoadMag1PV_DualGun_Left_Run,	AIPROC_LoadMag1PV_DualGun_Left_Fin	},
	{	"LoadMag1PV_DualGun_Right",	AIPROC_LoadMag1PV_DualGun_Right_Run,	AIPROC_LoadMag1PV_DualGun_Right_Fin	},
	{	"LoadMag3PV_DualGun_Left",	AIPROC_LoadMag3PV_DualGun_Left_Run,	AIPROC_LoadMag3PV_DualGun_Left_Fin	},
	{	"LoadMag3PV_DualGun_Right",	AIPROC_LoadMag3PV_DualGun_Right_Run,	AIPROC_LoadMag3PV_DualGun_Right_Fin	},
};

AIDriverGun_13PV_DualGun_Ext::AIDriverGun_13PV_DualGun_Ext(void)
{
	setAIProcTable( s_Gun_13PV_DualGun_ExtTable, (sizeof(s_Gun_13PV_DualGun_ExtTable)/sizeof(s_Gun_13PV_DualGun_ExtTable[0])) );
}

AIDriverGun_13PV_DualGun_Ext::~AIDriverGun_13PV_DualGun_Ext(void)
{
}
