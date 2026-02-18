#include "pch.h"
#include "AIDriverGrenadeMaleFemale.h"
#include "WeaponBase.h"

I3_CLASS_INSTANCE( AIDriverGrenadeMaleFemale);

////////////////////////////////////////////////////////////////////////////////
//
// Idle_Female
// ANIMATION : Weapon/Water_Bomb/Resource/Idle_Female.i3a
//
static void AIPROC_Idle_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();

	pWeapon->OnIdle( pCtx, tm);
}

static void AIPROC_Idle_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *)pCtx->getOwner();

	pWeapon->OnEndIdle(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire_Female
//
static void AIPROC_Fire_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();

	pWeapon->OnFire( pCtx, tm);
}

static void AIPROC_Fire_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();

	pWeapon->OnEndFire( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_Female
//
static void AIPROC_LoadMag_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_LoadMag_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet_Female
//
static void AIPROC_LoadBullet_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_LoadBullet_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle_Male
// ANIMATION : Weapon/Water_Bomb/Resource/Idle_Male.i3a
//
static void AIPROC_Idle_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();

	pWeapon->OnIdle( pCtx, tm);
}

static void AIPROC_Idle_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *)pCtx->getOwner();

	pWeapon->OnEndIdle(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire_Male
//
static void AIPROC_Fire_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();

	pWeapon->OnFire( pCtx, tm);
}

static void AIPROC_Fire_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();

	pWeapon->OnEndFire( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_Male
//
static void AIPROC_LoadMag_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_LoadMag_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet_Male
//
static void AIPROC_LoadBullet_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_LoadBullet_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
}



static i3::statictable::AISTATE_PROC s_GrenadeMaleFemaleTable[] =
{
	{	"Fire_Female",	AIPROC_Fire_Female_Run,	AIPROC_Fire_Female_Fin	},
	{	"Fire_Male",	AIPROC_Fire_Male_Run,	AIPROC_Fire_Male_Fin	},
	{	"Idle_Female",	AIPROC_Idle_Female_Run,	AIPROC_Idle_Female_Fin	},
	{	"Idle_Male",	AIPROC_Idle_Male_Run,	AIPROC_Idle_Male_Fin	},
	{	"LoadBullet_Female",	AIPROC_LoadBullet_Female_Run,	AIPROC_LoadBullet_Female_Fin	},
	{	"LoadBullet_Male",	AIPROC_LoadBullet_Male_Run,	AIPROC_LoadBullet_Male_Fin	},
	{	"LoadMag_Female",	AIPROC_LoadMag_Female_Run,	AIPROC_LoadMag_Female_Fin	},
	{	"LoadMag_Male",	AIPROC_LoadMag_Male_Run,	AIPROC_LoadMag_Male_Fin	},
};

AIDriverGrenadeMaleFemale::AIDriverGrenadeMaleFemale(void)
{
	setAIProcTable( s_GrenadeMaleFemaleTable, (sizeof(s_GrenadeMaleFemaleTable)/sizeof(s_GrenadeMaleFemaleTable[0])) );
}

AIDriverGrenadeMaleFemale::~AIDriverGrenadeMaleFemale(void)
{
}
