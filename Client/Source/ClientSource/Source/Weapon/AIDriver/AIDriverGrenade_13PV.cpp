#include "pch.h"
#include "AIDriverGrenade_13PV.h"
#include "WeaponBase.h"

I3_CLASS_INSTANCE( AIDriverGrenade_13PV);

////////////////////////////////////////////////////////////////////////////////
//
// Change1PV
//
static void AIPROC_Change1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *)pCtx->getOwner();
	pWeapon->OnSwaping(pCtx, tm);
}

static void AIPROC_Change1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change3PV
//
static void AIPROC_Change3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *)pCtx->getOwner();
	pWeapon->OnSwaping(pCtx, tm);
}

static void AIPROC_Change3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// AttackReady1PV
//
static void AIPROC_AttackReady1PV_Run(i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_AttackReady1PV_Fin(i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// AttackReady3PV
//
static void AIPROC_AttackReady3PV_Run(i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_AttackReady3PV_Fin(i3AIContext * pCtx, REAL32 tm)
{
}


////////////////////////////////////////////////////////////////////////////////
//
// Idle1PV
// ANIMATION : Weapon/BeerFest_Grenade/Resource/Idle.i3a
//
static void AIPROC_Idle1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *)pCtx->getOwner();
	pWeapon->OnIdle(pCtx, tm);
}

static void AIPROC_Idle1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *)pCtx->getOwner();
	pWeapon->OnEndIdle(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle3PV
// ANIMATION : Weapon/BeerFest_Grenade/Resource/Idle_3pv.i3a
//
static void AIPROC_Idle3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *)pCtx->getOwner();
	pWeapon->OnIdle(pCtx, tm);
}

static void AIPROC_Idle3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *)pCtx->getOwner();
	pWeapon->OnEndIdle(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire1PV
//
static void AIPROC_Fire1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *)pCtx->getOwner();
	pWeapon->OnFire(pCtx, tm);
}

static void AIPROC_Fire1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *)pCtx->getOwner();
	pWeapon->OnEndFire(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire3PV
//
static void AIPROC_Fire3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *)pCtx->getOwner();
	pWeapon->OnFire(pCtx, tm);
}

static void AIPROC_Fire3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *)pCtx->getOwner();
	pWeapon->OnEndFire(pCtx, tm);
}



static i3::statictable::AISTATE_PROC s_Grenade_13PVTable[] =
{
	{ "AttackReady1PV",	AIPROC_AttackReady1PV_Run,	AIPROC_AttackReady1PV_Fin },
	{ "AttackReady3PV",	AIPROC_AttackReady3PV_Run,	AIPROC_AttackReady3PV_Fin },
	{ "Change1PV",	AIPROC_Change1PV_Run,	AIPROC_Change1PV_Fin },
	{ "Change3PV",	AIPROC_Change3PV_Run,	AIPROC_Change3PV_Fin },
	{ "Fire1PV",	AIPROC_Fire1PV_Run,	AIPROC_Fire1PV_Fin },
	{ "Fire3PV",	AIPROC_Fire3PV_Run,	AIPROC_Fire3PV_Fin },
	{ "Idle1PV",	AIPROC_Idle1PV_Run,	AIPROC_Idle1PV_Fin },
	{ "Idle3PV",	AIPROC_Idle3PV_Run,	AIPROC_Idle3PV_Fin },
};

AIDriverGrenade_13PV::AIDriverGrenade_13PV(void)
{
	setAIProcTable( s_Grenade_13PVTable, (sizeof(s_Grenade_13PVTable)/sizeof(s_Grenade_13PVTable[0])) );
}

AIDriverGrenade_13PV::~AIDriverGrenade_13PV(void)
{
}
