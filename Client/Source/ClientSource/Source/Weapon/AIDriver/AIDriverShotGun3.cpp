#include "pch.h"
#include "AIDriverShotGun3.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverShotGun3);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet_1PV
// ANIMATION : Weapon/M1887/Resource/ReloadC_1PV.i3a
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
// LoadMag_1PV
// ANIMATION : Weapon/M1887/Resource/ReloadB_1PV.i3a
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
// Fire
// ANIMATION : Weapon/M1887/Resource/Attack_A.i3a
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
// ANIMATION : Weapon/M1887/Resource/Idle.i3a
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
}

static void AIPROC_ExtensionDeactivate_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_3PV
// ANIMATION : Weapon/M1887/Resource/ReloadB_3PV.i3a
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
// LoadBullet_3PV
// ANIMATION : Weapon/M1887/Resource/ReloadC_3PV.i3a
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
// Fire_Sec_1PV
// ANIMATION : Weapon/M1887/Resource/Attack_B_1PV.i3a
//
static void AIPROC_Fire_Sec_1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnSecondaryFire( pCtx, tm);
}

static void AIPROC_Fire_Sec_1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire_Sec_3PV
// ANIMATION : Weapon/M1887/Resource/Attack_B_3PV.i3a
//
static void AIPROC_Fire_Sec_3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnSecondaryFire( pCtx, tm);
}

static void AIPROC_Fire_Sec_3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change_1PV
// ANIMATION : Weapon/M1887/Resource/Change_1PV.i3a
//
static void AIPROC_Change_1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN( pCtx, tm);
}

static void AIPROC_Change_1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change_3PV
// ANIMATION : Weapon/M1887/Resource/Change_3PV.i3a
//
static void AIPROC_Change_3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN( pCtx, tm);
}

static void AIPROC_Change_3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_Rebel_Male
// ANIMATION : Weapon/M1887/Resource/UI_IdleB_RedBulls.i3a
//
static void AIPROC_UI_Rebel_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*) pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon( pCtx, tm);
}

static void AIPROC_UI_Rebel_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_Rebel_Female
// ANIMATION : Weapon/M1887/Resource/UI_IdleB_Tarantula.i3a
//
static void AIPROC_UI_Rebel_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*) pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon( pCtx, tm);
}

static void AIPROC_UI_Rebel_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_Rebel_ViperRed
// ANIMATION : Weapon/M1887/Resource/UI_IdleB_ViperRed.i3a
//
static void AIPROC_UI_Rebel_ViperRed_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*) pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon( pCtx, tm);
}

static void AIPROC_UI_Rebel_ViperRed_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_Rebel_D_Fox
// ANIMATION : Weapon/M1887/Resource/UI_IdleB_D-Fox.i3a
//
static void AIPROC_UI_Rebel_D_Fox_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*) pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon( pCtx, tm);
}

static void AIPROC_UI_Rebel_D_Fox_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_SWAT_Male
// ANIMATION : Weapon/M1887/Resource/UI_IdleB_AcidPol.i3a
//
static void AIPROC_UI_SWAT_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*) pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon( pCtx, tm);
}

static void AIPROC_UI_SWAT_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_SWAT_Female
// ANIMATION : Weapon/M1887/Resource/UI_IdleB_KeenEyes.i3a
//
static void AIPROC_UI_SWAT_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*) pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon( pCtx, tm);
}

static void AIPROC_UI_SWAT_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_SWAT_Leopard
// ANIMATION : Weapon/M1887/Resource/UI_IdleB_Leopard.i3a
//
static void AIPROC_UI_SWAT_Leopard_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*) pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon( pCtx, tm);
}

static void AIPROC_UI_SWAT_Leopard_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_SWAT_Hide
// ANIMATION : Weapon/M1887/Resource/UI_IdleB_Hide.i3a
//
static void AIPROC_UI_SWAT_Hide_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*) pCtx->getOwner();
	pWeapon->OnUI_ShowWeapon( pCtx, tm);
}

static void AIPROC_UI_SWAT_Hide_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_Ready_1PV
// ANIMATION : Weapon/M1887/Resource/ReloadA_1PV.i3a
//
static void AIPROC_LoadMag_Ready_1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*) pCtx->getOwner();
	pWeapon->OnLoadMag_Ready( pCtx, tm);
}

static void AIPROC_LoadMag_Ready_1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_Ready_3PV
// ANIMATION : Weapon/M1887/Resource/ReloadA_3PV.i3a
//
static void AIPROC_LoadMag_Ready_3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*) pCtx->getOwner();
	pWeapon->OnLoadMag_Ready( pCtx, tm);
}

static void AIPROC_LoadMag_Ready_3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}


static i3::statictable::AISTATE_PROC s_ShotGun3Table[] =
{
	{	"Change_1PV",	AIPROC_Change_1PV_Run,	AIPROC_Change_1PV_Fin	},
	{	"Change_3PV",	AIPROC_Change_3PV_Run,	AIPROC_Change_3PV_Fin	},
	{	"ExtensionActivate",	AIPROC_ExtensionActivate_Run,	AIPROC_ExtensionActivate_Fin	},
	{	"ExtensionDeactivate",	AIPROC_ExtensionDeactivate_Run,	AIPROC_ExtensionDeactivate_Fin	},
	{	"Fire",	AIPROC_Fire_Run,	AIPROC_Fire_Fin	},
	{	"Fire_Sec_1PV",	AIPROC_Fire_Sec_1PV_Run,	AIPROC_Fire_Sec_1PV_Fin	},
	{	"Fire_Sec_3PV",	AIPROC_Fire_Sec_3PV_Run,	AIPROC_Fire_Sec_3PV_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"LoadBullet_1PV",	AIPROC_LoadBullet_1PV_Run,	AIPROC_LoadBullet_1PV_Fin	},
	{	"LoadBullet_3PV",	AIPROC_LoadBullet_3PV_Run,	AIPROC_LoadBullet_3PV_Fin	},
	{	"LoadMag_1PV",	AIPROC_LoadMag_1PV_Run,	AIPROC_LoadMag_1PV_Fin	},
	{	"LoadMag_3PV",	AIPROC_LoadMag_3PV_Run,	AIPROC_LoadMag_3PV_Fin	},
	{	"LoadMag_Ready_1PV",	AIPROC_LoadMag_Ready_1PV_Run,	AIPROC_LoadMag_Ready_1PV_Fin	},
	{	"LoadMag_Ready_3PV",	AIPROC_LoadMag_Ready_3PV_Run,	AIPROC_LoadMag_Ready_3PV_Fin	},
	{	"UI_Rebel_D_Fox",	AIPROC_UI_Rebel_D_Fox_Run,	AIPROC_UI_Rebel_D_Fox_Fin	},
	{	"UI_Rebel_Female",	AIPROC_UI_Rebel_Female_Run,	AIPROC_UI_Rebel_Female_Fin	},
	{	"UI_Rebel_Male",	AIPROC_UI_Rebel_Male_Run,	AIPROC_UI_Rebel_Male_Fin	},
	{	"UI_Rebel_ViperRed",	AIPROC_UI_Rebel_ViperRed_Run,	AIPROC_UI_Rebel_ViperRed_Fin	},
	{	"UI_SWAT_Female",	AIPROC_UI_SWAT_Female_Run,	AIPROC_UI_SWAT_Female_Fin	},
	{	"UI_SWAT_Hide",	AIPROC_UI_SWAT_Hide_Run,	AIPROC_UI_SWAT_Hide_Fin	},
	{	"UI_SWAT_Leopard",	AIPROC_UI_SWAT_Leopard_Run,	AIPROC_UI_SWAT_Leopard_Fin	},
	{	"UI_SWAT_Male",	AIPROC_UI_SWAT_Male_Run,	AIPROC_UI_SWAT_Male_Fin	},
};

AIDriverShotGun3::AIDriverShotGun3(void)
{
	setAIProcTable( s_ShotGun3Table, (sizeof(s_ShotGun3Table)/sizeof(s_ShotGun3Table[0])));
}

AIDriverShotGun3::~AIDriverShotGun3(void)
{
}
