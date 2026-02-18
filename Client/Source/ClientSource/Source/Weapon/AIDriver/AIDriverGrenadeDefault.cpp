#include "pch.h"
#include "AIDriverGrenadeDefault.h"
#include "WeaponBase.h"

I3_CLASS_INSTANCE( AIDriverGrenadeDefault);

////////////////////////////////////////////////////////////////////////////////
//
// Idle
//
static void AIPROC_Idle_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();

	pWeapon->OnIdle( pCtx, tm);
}

static void AIPROC_Idle_Fin( i3AIContext * pCtx, REAL32 tm)
{	
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnEndIdle(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire
//
static void AIPROC_Fire_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();

	pWeapon->OnFire( pCtx, tm);
}

static void AIPROC_Fire_Fin( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();

	pWeapon->OnEndFire( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag
//
static void AIPROC_LoadMag_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_LoadMag_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet
//
static void AIPROC_LoadBullet_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_LoadBullet_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// SecondaryFire
// ANIMATION : Weapon/M14_Mine/Resource/Attack_R_C.i3a
//
static void AIPROC_SecondaryFire_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();

	pWeapon->OnFire( pCtx, tm);
}

static void AIPROC_SecondaryFire_Fin( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();

	pWeapon->OnEndFire( pCtx, tm);
}




static i3::statictable::AISTATE_PROC s_GrenadeDefaultTable[] =
{
	{	"Fire",	AIPROC_Fire_Run,	AIPROC_Fire_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"LoadBullet",	AIPROC_LoadBullet_Run,	AIPROC_LoadBullet_Fin	},
	{	"LoadMag",	AIPROC_LoadMag_Run,	AIPROC_LoadMag_Fin	},
	{	"SecondaryFire",	AIPROC_SecondaryFire_Run,	AIPROC_SecondaryFire_Fin	},
};

AIDriverGrenadeDefault::AIDriverGrenadeDefault(void)
{
	setAIProcTable( s_GrenadeDefaultTable, (sizeof(s_GrenadeDefaultTable)/sizeof(s_GrenadeDefaultTable[0])));
}

AIDriverGrenadeDefault::~AIDriverGrenadeDefault(void)
{
}
