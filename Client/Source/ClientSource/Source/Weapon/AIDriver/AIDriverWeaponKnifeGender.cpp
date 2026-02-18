#include "pch.h"
#include "WeaponBase.h"
#include "AIDriverWeaponKnifeGender.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverWeaponKnifeGender);

////////////////////////////////////////////////////////////////////////////////
//
// Idle
// ANIMATION : Weapon/Bamboo-Runcing/Resource/Idle_Male.i3a
//
static void AIPROC_Idle_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnIdle( pCtx, tm);
}

static void AIPROC_Idle_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// SecondaryFire
// ANIMATION : Weapon/Bamboo-Runcing/Resource/Attack_B_Male.i3a
//
static void AIPROC_SecondaryFire_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnSecondaryFire( pCtx, tm);
}

static void AIPROC_SecondaryFire_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire
// ANIMATION : Weapon/Bamboo-Runcing/Resource/Attack_A_Male.i3a
//
static void AIPROC_Fire_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnFire( pCtx, tm);
}

static void AIPROC_Fire_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle_Female
// ANIMATION : Weapon/Bamboo-Runcing/Resource/Idle_Female.i3a
//
static void AIPROC_Idle_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnIdle( pCtx, tm);
}

static void AIPROC_Idle_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire_Female
// ANIMATION : Weapon/Bamboo-Runcing/Resource/Attack_A_Female.i3a
//
static void AIPROC_Fire_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnFire( pCtx, tm);
}

static void AIPROC_Fire_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// SecondaryFire_Female
// ANIMATION : Weapon/Bamboo-Runcing/Resource/Attack_B_Female.i3a
//
static void AIPROC_SecondaryFire_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnSecondaryFire( pCtx, tm);
}

static void AIPROC_SecondaryFire_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag1PV_Female
//
static void AIPROC_LoadMag1PV_Female_Run( i3AIContext * pCtx, REAL32 tm)
{

}

static void AIPROC_LoadMag1PV_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{

}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag1PV
//
static void AIPROC_LoadMag1PV_Run( i3AIContext * pCtx, REAL32 tm)
{

}

static void AIPROC_LoadMag1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{

}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag3PV_Female
//
static void AIPROC_LoadMag3PV_Female_Run( i3AIContext * pCtx, REAL32 tm)
{

}

static void AIPROC_LoadMag3PV_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{

}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag3PV
//
static void AIPROC_LoadMag3PV_Run( i3AIContext * pCtx, REAL32 tm)
{

}

static void AIPROC_LoadMag3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{

}


static i3::statictable::AISTATE_PROC s_WeaponKnifeGenderTable[] =
{
	{	"Fire",	AIPROC_Fire_Run,	AIPROC_Fire_Fin	},
	{	"Fire_Female",	AIPROC_Fire_Female_Run,	AIPROC_Fire_Female_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"Idle_Female",	AIPROC_Idle_Female_Run,	AIPROC_Idle_Female_Fin	},
	{	"LoadMag1PV",	AIPROC_LoadMag1PV_Run,	AIPROC_LoadMag1PV_Fin	},
	{	"LoadMag1PV_Female",	AIPROC_LoadMag1PV_Female_Run,	AIPROC_LoadMag1PV_Female_Fin	},
	{	"LoadMag3PV",	AIPROC_LoadMag3PV_Run,	AIPROC_LoadMag3PV_Fin	},
	{	"LoadMag3PV_Female",	AIPROC_LoadMag3PV_Female_Run,	AIPROC_LoadMag3PV_Female_Fin	},
	{	"SecondaryFire",	AIPROC_SecondaryFire_Run,	AIPROC_SecondaryFire_Fin	},
	{	"SecondaryFire_Female",	AIPROC_SecondaryFire_Female_Run,	AIPROC_SecondaryFire_Female_Fin	},
};

AIDriverWeaponKnifeGender::AIDriverWeaponKnifeGender(void)
{
	setAIProcTable( s_WeaponKnifeGenderTable, (sizeof(s_WeaponKnifeGenderTable)/sizeof(s_WeaponKnifeGenderTable[0])) );
}

AIDriverWeaponKnifeGender::~AIDriverWeaponKnifeGender(void)
{
}
