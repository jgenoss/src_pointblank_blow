#include "pch.h"
#include "AIDriverShotGun_13PV.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverShotGun_13PV);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// Fire
// ANIMATION : Weapon/Kel_Tec_KSG_15/Resource/Attack.i3a
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
// ANIMATION : Weapon/Kel_Tec_KSG_15/Resource/Idle.i3a
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
// LoadBullet3PV
// ANIMATION : Weapon/Kel_Tec_KSG_15/Resource/ReloadC_3PV.i3a
//
static void AIPROC_LoadBullet3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadBullet3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet1PV
// ANIMATION : Weapon/Kel_Tec_KSG_15/Resource/ReloadC_1PV.i3a
//
static void AIPROC_LoadBullet1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadBullet1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm);
}


////////////////////////////////////////////////////////////////////////////////
//
// LoadMag3PV
// ANIMATION : Weapon/Kel_Tec_KSG_15/Resource/ReloadB_3PV.i3a
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
// LoadMag1PV
// ANIMATION : Weapon/Kel_Tec_KSG_15/Resource/ReloadB_1PV.i3a
//
static void AIPROC_LoadMag1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}


static i3::statictable::AISTATE_PROC s_ShotGun_13PVTable[] =
{
	{	"Fire",	AIPROC_Fire_Run,	AIPROC_Fire_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"LoadBullet1PV",	AIPROC_LoadBullet1PV_Run,	AIPROC_LoadBullet1PV_Fin	},
	{	"LoadBullet3PV",	AIPROC_LoadBullet3PV_Run,	AIPROC_LoadBullet3PV_Fin	},
	{	"LoadMag1PV",	AIPROC_LoadMag1PV_Run,	AIPROC_LoadMag1PV_Fin	},
	{	"LoadMag3PV",	AIPROC_LoadMag3PV_Run,	AIPROC_LoadMag3PV_Fin	},
};

AIDriverShotGun_13PV::AIDriverShotGun_13PV(void)
{
	setAIProcTable( s_ShotGun_13PVTable, (sizeof(s_ShotGun_13PVTable)/sizeof(s_ShotGun_13PVTable[0])) );
}

AIDriverShotGun_13PV::~AIDriverShotGun_13PV(void)
{
}