#include "pch.h"
#include "AIDriverGunExtension.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverGunExtension);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// Idle_Ext
// ANIMATION : Weapon/M4A1/Idle_Opend.i3a
//
static void AIPROC_Idle_Ext_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm );
}

static void AIPROC_Idle_Ext_Fin( i3AIContext * pCtx, REAL32 tm)
{	
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet_Ext
// ANIMATION : Weapon/M4A1/Idle_Opend.i3a
//
static void AIPROC_LoadBullet_Ext_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm );
}

static void AIPROC_LoadBullet_Ext_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_Ext
// ANIMATION : Weapon/M4A1/Reload_Opend.i3a
//
static void AIPROC_LoadMag_Ext_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMag_Ext_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire_Ext
// ANIMATION : Weapon/M4A1/Attack_Opend.i3a
//
static void AIPROC_Fire_Ext_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm );
}

static void AIPROC_Fire_Ext_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionDeactivate
// ANIMATION : WEAPON/M4A1/CLOSE.I3A
//
static void AIPROC_ExtensionDeactivate_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_EXTENSION_DEACTIVATE_RUN( pCtx, tm );
}

static void AIPROC_ExtensionDeactivate_Fin( i3AIContext * pCtx, REAL32 tm)
{
	
}

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionActivate
// ANIMATION : WEAPON/M4A1/OPEN.I3A
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
// Idle
// ANIMATION : Weapon/M4A1/Idle_Closed.i3a
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
// ANIMATION : Weapon/M4A1/Idle_Closed.i3a
//
static void AIPROC_LoadBullet_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm );
}

static void AIPROC_LoadBullet_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag
// ANIMATION : Weapon/M4A1/Reload_Closed.i3a
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
// LoadMagB
// ANIMATION : Weapon/MK23/Dual_Reload.i3a
//
static void AIPROC_LoadMagB_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMagB_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire
// ANIMATION : WEAPON/M4A1/ATTACK_CLOSED.I3A
//
static void AIPROC_Fire_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm );
}

static void AIPROC_Fire_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm );
}



static i3::statictable::AISTATE_PROC s_GunExtensionTable[] =
{
	{	"ExtensionActivate",	AIPROC_ExtensionActivate_Run,	AIPROC_ExtensionActivate_Fin	},
	{	"ExtensionDeactivate",	AIPROC_ExtensionDeactivate_Run,	AIPROC_ExtensionDeactivate_Fin	},
	{	"Fire_Ext",	AIPROC_Fire_Ext_Run,	AIPROC_Fire_Ext_Fin	},
	{	"Fire",	AIPROC_Fire_Run,	AIPROC_Fire_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"Idle_Ext",	AIPROC_Idle_Ext_Run,	AIPROC_Idle_Ext_Fin	},
	{	"LoadBullet",	AIPROC_LoadBullet_Run,	AIPROC_LoadBullet_Fin	},
	{	"LoadBullet_Ext",	AIPROC_LoadBullet_Ext_Run,	AIPROC_LoadBullet_Ext_Fin	},
	{	"LoadMag_Ext",	AIPROC_LoadMag_Ext_Run,	AIPROC_LoadMag_Ext_Fin	},
	{	"LoadMag",	AIPROC_LoadMag_Run,	AIPROC_LoadMag_Fin	},
	{	"LoadMagB",	AIPROC_LoadMagB_Run,	AIPROC_LoadMagB_Fin	},
};

AIDriverGunExtension::AIDriverGunExtension(void)
{
	setAIProcTable( s_GunExtensionTable, (sizeof(s_GunExtensionTable)/sizeof(s_GunExtensionTable[0])));
}

AIDriverGunExtension::~AIDriverGunExtension(void)
{
}
