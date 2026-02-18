#include "pch.h"
#include "AIDriverShotGun_Fire13PV.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverShotGun_Fire13PV);

////////////////////////////////////////////////////////////////////////////////
//
// Fire_1PV
// ANIMATION : Weapon/SuperShorty870/Resourse/Attack_1PV.i3a
//
static void AIPROC_Fire_1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_Fire_1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire_3PV
// ANIMATION : Weapon/SuperShorty870/Resourse/Attack_3PV.i3a
//
static void AIPROC_Fire_3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_Fire_3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Change
// ANIMATION : Weapon/SuperShorty870/Resourse/Change.i3a
//
static void AIPROC_Change_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN( pCtx, tm);
}

static void AIPROC_Change_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle
// ANIMATION : Weapon/SuperShorty870/Resourse/Idle.i3a
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
// LoadBullet_1PV
// ANIMATION : Weapon/SuperShorty870/Resourse/ReloadC_1PV.i3a
//
static void AIPROC_LoadBullet_1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadBullet_1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet_3PV
// ANIMATION : Weapon/SuperShorty870/Resourse/ReloadC_3PV.i3a
//
static void AIPROC_LoadBullet_3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadBullet_3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_1PV
// ANIMATION : Weapon/SuperShorty870/Resourse/ReloadB_1PV.i3a
//
static void AIPROC_LoadMag_1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag_1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);	
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_3PV
// ANIMATION : Weapon/SuperShorty870/Resourse/ReloadB_3PV.i3a
//
static void AIPROC_LoadMag_3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag_3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_IdleA
// ANIMATION : Weapon/SuperShorty870/Resourse/UI_IdleA.i3a
//
static void AIPROC_UI_IdleA_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*) pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon( pCtx, tm);
}

static void AIPROC_UI_IdleA_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_IdleB
// ANIMATION : Weapon/SuperShorty870/Resourse/UI_IdleB.i3a
//
static void AIPROC_UI_IdleB_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*) pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon( pCtx, tm);
}

static void AIPROC_UI_IdleB_Fin( i3AIContext * pCtx, REAL32 tm)
{
}



static i3::statictable::AISTATE_PROC s_ShotGun_Fire13PVTable[] =
{
	{	"Change",	AIPROC_Change_Run,	AIPROC_Change_Fin	},
	{	"Fire1PV",	AIPROC_Fire_1PV_Run,	AIPROC_Fire_1PV_Fin	},
	{	"Fire3PV",	AIPROC_Fire_3PV_Run,	AIPROC_Fire_3PV_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"LoadBullet_1PV",	AIPROC_LoadBullet_1PV_Run,	AIPROC_LoadBullet_1PV_Fin	},
	{	"LoadBullet_3PV",	AIPROC_LoadBullet_3PV_Run,	AIPROC_LoadBullet_3PV_Fin	},
	{	"LoadMag_1PV",	AIPROC_LoadMag_1PV_Run,	AIPROC_LoadMag_1PV_Fin	},
	{	"LoadMag_3PV",	AIPROC_LoadMag_3PV_Run,	AIPROC_LoadMag_3PV_Fin	},
	{	"UI_IdleA",	AIPROC_UI_IdleA_Run,	AIPROC_UI_IdleA_Fin	},
	{	"UI_IdleB",	AIPROC_UI_IdleB_Run,	AIPROC_UI_IdleB_Fin	},
};

AIDriverShotGun_Fire13PV::AIDriverShotGun_Fire13PV(void)
{
	setAIProcTable( s_ShotGun_Fire13PVTable, (sizeof(s_ShotGun_Fire13PVTable)/sizeof(s_ShotGun_Fire13PVTable[0])) );
}

AIDriverShotGun_Fire13PV::~AIDriverShotGun_Fire13PV(void)
{
}
