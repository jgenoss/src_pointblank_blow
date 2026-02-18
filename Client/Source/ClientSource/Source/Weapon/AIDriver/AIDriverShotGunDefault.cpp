#include "pch.h"
#include "AIDriverShotGunDefault.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverShotGunDefault);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet
// ANIMATION : WEAPON/870MCS/RELOADC.I3A
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
// LoadMag
// ANIMATION : WEAPON/870MCS/RELOADB.I3A
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
// Fire
// ANIMATION : WEAPON/870MCS/ATTACK.I3A
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
// ANIMATION : WEAPON/870MCS/IDLE.I3A
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
// LoadD_Master3PV
// ANIMATION : Weapon/870MCS/ReloadD_Master_3PV.i3a
//
static void AIPROC_LoadD_Master3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadD_Master3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadC_Master3PV
// ANIMATION : Weapon/870MCS/ReloadC_Master_3PV.i3a
//
static void AIPROC_LoadC_Master3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponShotGun * pShotgunWeapon = (WeaponShotGun *) pCtx->getOwner();

	I3ASSERT( i3::same_of<WeaponShotGun*>(pShotgunWeapon));

	pShotgunWeapon->OnLoadC_MasterRun( pCtx, tm);
}

static void AIPROC_LoadC_Master3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadB_Master3PV
// ANIMATION : Weapon/870MCS/ReloadB_Master_3PV.i3a
//
static void AIPROC_LoadB_Master3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponShotGun * pShotgunWeapon = (WeaponShotGun *) pCtx->getOwner();
	
	I3ASSERT( i3::same_of<WeaponShotGun*>(pShotgunWeapon));

	pShotgunWeapon->OnLoadB_MasterRun( pCtx, tm);
}

static void AIPROC_LoadB_Master3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadD_Master1PV
// ANIMATION : Weapon/870MCS/ReloadD_Master_1PV.i3a
//
static void AIPROC_LoadD_Master1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadD_Master1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadB_Master1PV
// ANIMATION : Weapon/870MCS/ReloadB_Master_1PV.i3a
//
static void AIPROC_LoadB_Master1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponShotGun * pShotgunWeapon = (WeaponShotGun *) pCtx->getOwner();
	
	I3ASSERT( i3::same_of<WeaponShotGun*>(pShotgunWeapon));

	pShotgunWeapon->OnLoadB_MasterRun( pCtx, tm);
}

static void AIPROC_LoadB_Master1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadC_Master1PV
// ANIMATION : Weapon/870MCS/ReloadC_Master_1PV.i3a
//
static void AIPROC_LoadC_Master1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponShotGun * pShotgunWeapon = (WeaponShotGun *) pCtx->getOwner();
	
	I3ASSERT( i3::same_of<WeaponShotGun*>(pShotgunWeapon));

	pShotgunWeapon->OnLoadC_MasterRun( pCtx, tm);
}

static void AIPROC_LoadC_Master1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	
}


static i3::statictable::AISTATE_PROC s_ShotGunDefaultTable[] =
{
	{	"ExtensionActivate",	AIPROC_ExtensionActivate_Run,	AIPROC_ExtensionActivate_Fin	},
	{	"ExtensionDeactivate",	AIPROC_ExtensionDeactivate_Run,	AIPROC_ExtensionDeactivate_Fin	},
	{	"Fire",	AIPROC_Fire_Run,	AIPROC_Fire_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"LoadB_Master1PV",	AIPROC_LoadB_Master1PV_Run,	AIPROC_LoadB_Master1PV_Fin	},
	{	"LoadB_Master3PV",	AIPROC_LoadB_Master3PV_Run,	AIPROC_LoadB_Master3PV_Fin	},
	{	"LoadBullet",	AIPROC_LoadBullet_Run,	AIPROC_LoadBullet_Fin	},
	{	"LoadC_Master1PV",	AIPROC_LoadC_Master1PV_Run,	AIPROC_LoadC_Master1PV_Fin	},
	{	"LoadC_Master3PV",	AIPROC_LoadC_Master3PV_Run,	AIPROC_LoadC_Master3PV_Fin	},
	{	"LoadD_Master1PV",	AIPROC_LoadD_Master1PV_Run,	AIPROC_LoadD_Master1PV_Fin	},
	{	"LoadD_Master3PV",	AIPROC_LoadD_Master3PV_Run,	AIPROC_LoadD_Master3PV_Fin	},
	{	"LoadMag",	AIPROC_LoadMag_Run,	AIPROC_LoadMag_Fin	},
};

AIDriverShotGunDefault::AIDriverShotGunDefault(void)
{
	setAIProcTable( s_ShotGunDefaultTable, (sizeof(s_ShotGunDefaultTable)/sizeof(s_ShotGunDefaultTable[0])));
}

AIDriverShotGunDefault::~AIDriverShotGunDefault(void)
{
}
