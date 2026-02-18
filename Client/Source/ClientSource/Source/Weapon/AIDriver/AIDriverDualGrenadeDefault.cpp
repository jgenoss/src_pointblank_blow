#include "pch.h"
#include "AIDriverDualGrenadeDefault.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverDualGrenadeDefault);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// Idle_Left
// ANIMATION : Weapon/K-413/Resource/Idle_L.i3a
//
static void AIPROC_Idle_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm);
}

static void AIPROC_Idle_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
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
// Fire_Left
// ANIMATION : Weapon/K-413/Resource/AttackC_L.i3a
//
static void AIPROC_Fire_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_Fire_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle_Right
// ANIMATION : Weapon/K-413/Resource/Idle_R.i3a
//
static void AIPROC_Idle_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm);
}

static void AIPROC_Idle_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire_Right
// ANIMATION : Weapon/K-413/Resource/AttackC_R.i3a
//
static void AIPROC_Fire_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_Fire_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}



static i3::statictable::AISTATE_PROC s_DualGrenadeDefaultTable[] =
{
	{	"Fire_Right",	AIPROC_Fire_Right_Run,	AIPROC_Fire_Right_Fin	},
	{	"Fire_Left",	AIPROC_Fire_Left_Run,	AIPROC_Fire_Left_Fin	},
	{	"Idle_Right",	AIPROC_Idle_Right_Run,	AIPROC_Idle_Right_Fin	},
	{	"Idle_Left",	AIPROC_Idle_Left_Run,	AIPROC_Idle_Left_Fin	},
	{	"LoadBullet",	AIPROC_LoadBullet_Run,	AIPROC_LoadBullet_Fin	},
	{	"LoadMag",	AIPROC_LoadMag_Run,	AIPROC_LoadMag_Fin	},
};

AIDriverDualGrenadeDefault::AIDriverDualGrenadeDefault(void)
{
	setAIProcTable( s_DualGrenadeDefaultTable, (sizeof(s_DualGrenadeDefaultTable)/sizeof(s_DualGrenadeDefaultTable[0])) );
}

AIDriverDualGrenadeDefault::~AIDriverDualGrenadeDefault(void)
{
}
