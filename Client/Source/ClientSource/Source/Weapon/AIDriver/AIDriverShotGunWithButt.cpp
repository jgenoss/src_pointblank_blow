#include "pch.h"
#include "AIDriverShotGunWithButt.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverShotGunWithButt);

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet
// ANIMATION : Weapon/SPAS15/Resource/Idle.i3a
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
// Fire
// ANIMATION : WEAPON/SPAS15/ATTACK.I3A
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
// Idle
// ANIMATION : WEAPON/SPAS15/IDLE.I3A
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
// LoadMag
// ANIMATION : WEAPON/SPAS15/RELOAD.I3A
//
static void AIPROC_LoadMag_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagMaster1PV
// ANIMATION : Weapon/SPAS15/Reload_Master_1PV.i3a
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
// ANIMATION : Weapon/SPAS15/Reload_Master_3PV.i3a
//
static void AIPROC_LoadMagMaster3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMagMaster3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}



static void AIPROC_SwingButt_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnSecondaryFire( pCtx, tm);
}

static void AIPROC_SwingButt_Fin( i3AIContext * pCtx, REAL32 tm)
{
}



static i3::statictable::AISTATE_PROC s_ShotGunWithButtTable[] =
{
	
	{	"ExtensionActivate",	AIPROC_ExtensionActivate_Run,	AIPROC_ExtensionActivate_Fin	},
	{	"ExtensionDeactivate",	AIPROC_ExtensionDeactivate_Run,	AIPROC_ExtensionDeactivate_Fin	},
	{	"Fire",	AIPROC_Fire_Run,	AIPROC_Fire_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"LoadBullet",	AIPROC_LoadBullet_Run,	AIPROC_LoadBullet_Fin	},
	{	"LoadMag",	AIPROC_LoadMag_Run,	AIPROC_LoadMag_Fin	},
	{	"LoadMagMaster1PV",	AIPROC_LoadMagMaster1PV_Run,	AIPROC_LoadMagMaster1PV_Fin	},
	{	"LoadMagMaster3PV",	AIPROC_LoadMagMaster3PV_Run,	AIPROC_LoadMagMaster3PV_Fin	},
	{	"SwingButt",	AIPROC_SwingButt_Run,	AIPROC_SwingButt_Fin	},
};

AIDriverShotGunWithButt::AIDriverShotGunWithButt(void)
{
	setAIProcTable( s_ShotGunWithButtTable, (sizeof(s_ShotGunWithButtTable)/sizeof(s_ShotGunWithButtTable[0])));
}

AIDriverShotGunWithButt::~AIDriverShotGunWithButt(void)
{
}
