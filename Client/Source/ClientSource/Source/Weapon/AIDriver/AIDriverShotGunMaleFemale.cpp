#include "pch.h"
#include "AIDriverShotGunMaleFemale.h"
#include "../GameCharaAnimContext.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverShotGunMaleFemale);

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagC_Male
// ANIMATION : Weapon/UTS-15/Resource/Reload_C_Male.i3a
//
static void AIPROC_LoadMagC_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMagC_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagB_Female
// ANIMATION : Weapon/UTS-15/Resource/Reload_B_Female.i3a
//
static void AIPROC_LoadMagB_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMagB_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire_Male
// ANIMATION : Weapon/UTS-15/Resource/Attack_Male.i3a
//
static void AIPROC_Fire_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_Fire_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle
// ANIMATION : Weapon/UTS-15/Resource/Idle.i3a
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
// Fire_Female
// ANIMATION : Weapon/UTS-15/Resource/Attack_Female.i3a
//
static void AIPROC_Fire_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_Fire_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagD_Female
// ANIMATION : Weapon/UTS-15/Resource/Reload_D_Female.i3a
//
static void AIPROC_LoadMagD_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadMagD_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_Ready_Female
// ANIMATION : Weapon/UTS-15/Resource/Reload_A_Female.i3a
//
static void AIPROC_LoadMag_Ready_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*) pCtx->getOwner();
	pWeapon->OnLoadMag_Ready( pCtx, tm);
}

static void AIPROC_LoadMag_Ready_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagB_Male
// ANIMATION : Weapon/UTS-15/Resource/Reload_B_Male.i3a
//
static void AIPROC_LoadMagB_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMagB_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagD_Male
// ANIMATION : Weapon/UTS-15/Resource/Reload_D_Male.i3a
//
static void AIPROC_LoadMagD_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadMagD_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagC_Female
// ANIMATION : Weapon/UTS-15/Resource/Reload_C_Female.i3a
//
static void AIPROC_LoadMagC_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMagC_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_Ready_Male
// ANIMATION : Weapon/UTS-15/Resource/Reload_A_Male.i3a
//
static void AIPROC_LoadMag_Ready_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*) pCtx->getOwner();
	pWeapon->OnLoadMag_Ready( pCtx, tm);
}

static void AIPROC_LoadMag_Ready_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
}



static i3::statictable::AISTATE_PROC s_ShotGunMaleFemaleTable[] =
{
	{	"ExtensionActivate",	AIPROC_ExtensionActivate_Run,	AIPROC_ExtensionActivate_Fin	},
	{	"Fire_Female",	AIPROC_Fire_Female_Run,	AIPROC_Fire_Female_Fin	},
	{	"Fire_Male",	AIPROC_Fire_Male_Run,	AIPROC_Fire_Male_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"LoadMag_Ready_Female",	AIPROC_LoadMag_Ready_Female_Run,	AIPROC_LoadMag_Ready_Female_Fin	},
	{	"LoadMag_Ready_Male",	AIPROC_LoadMag_Ready_Male_Run,	AIPROC_LoadMag_Ready_Male_Fin	},
	{	"LoadMagB_Female",	AIPROC_LoadMagB_Female_Run,	AIPROC_LoadMagB_Female_Fin	},
	{	"LoadMagB_Male",	AIPROC_LoadMagB_Male_Run,	AIPROC_LoadMagB_Male_Fin	},
	{	"LoadMagC_Female",	AIPROC_LoadMagC_Female_Run,	AIPROC_LoadMagC_Female_Fin	},
	{	"LoadMagC_Male",	AIPROC_LoadMagC_Male_Run,	AIPROC_LoadMagC_Male_Fin	},
	{	"LoadMagD_Female",	AIPROC_LoadMagD_Female_Run,	AIPROC_LoadMagD_Female_Fin	},
	{	"LoadMagD_Male",	AIPROC_LoadMagD_Male_Run,	AIPROC_LoadMagD_Male_Fin	},
};

AIDriverShotGunMaleFemale::AIDriverShotGunMaleFemale(void)
{
	setAIProcTable( s_ShotGunMaleFemaleTable, (sizeof(s_ShotGunMaleFemaleTable)/sizeof(s_ShotGunMaleFemaleTable[0])) );
}

AIDriverShotGunMaleFemale::~AIDriverShotGunMaleFemale(void)
{
}
