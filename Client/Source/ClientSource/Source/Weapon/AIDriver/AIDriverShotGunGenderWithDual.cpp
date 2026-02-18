#include "pch.h"
#include "AIDriverShotGunGenderWithDual.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverShotGunGenderWithDual);

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet_Male
// ANIMATION : Weapon/Cerberus_Shotgun/Resource/Reload_C_Male.i3a
//
static void AIPROC_LoadBullet_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadBullet_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_Male
// ANIMATION : Weapon/Cerberus_Shotgun/Resource/Reload_B_Male.i3a
//
static void AIPROC_LoadMag_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle_Ext_Female
// ANIMATION : Weapon/Cerberus_Shotgun/Resource/Idle_Folded_Female.i3a
//
static void AIPROC_Idle_Ext_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm);
}

static void AIPROC_Idle_Ext_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionActivate_Female
// ANIMATION : Weapon/Cerberus_Shotgun/Resource/Idle_Female.i3a
//
static void AIPROC_ExtensionActivate_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm);
}

static void AIPROC_ExtensionActivate_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionDeactivate_Female
// ANIMATION : Weapon/Cerberus_Shotgun/Resource/Change_Female.i3a
//
static void AIPROC_ExtensionDeactivate_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm);
}

static void AIPROC_ExtensionDeactivate_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_Female
// ANIMATION : Weapon/Cerberus_Shotgun/Resource/Reload_B_Female.i3a
//
static void AIPROC_LoadMag_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire_Male
// ANIMATION : Weapon/Cerberus_Shotgun/Resource/Attack_Male.i3a
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
// Fire_Ext_Male
// ANIMATION : Weapon/Cerberus_Shotgun/Resource/Attack_Folded_Male.i3a
//
static void AIPROC_Fire_Ext_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_Fire_Ext_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire_Ext_Female
// ANIMATION : Weapon/Cerberus_Shotgun/Resource/Attack_Folded_Female.i3a
//
static void AIPROC_Fire_Ext_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_Fire_Ext_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Change_Ext_Dual_Left_Male
// ANIMATION : Weapon/Cerberus_Shotgun/Resource/Change_Folded_Male_L.i3a
//
static void AIPROC_Change_Ext_Dual_Left_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN( pCtx, tm);
}

static void AIPROC_Change_Ext_Dual_Left_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change_Ext_Dual_Left_Female
// ANIMATION : Weapon/Cerberus_Shotgun/Resource/Change_Folded_Female_L.i3a
//
static void AIPROC_Change_Ext_Dual_Left_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN( pCtx, tm);
}

static void AIPROC_Change_Ext_Dual_Left_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_Ready_Male
// ANIMATION : Weapon/Cerberus_Shotgun/Resource/Reload_A_Male.i3a
//
static void AIPROC_LoadMag_Ready_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase*) pCtx->getOwner();
	pWeapon->OnLoadMag_Ready( pCtx, tm);
}

static void AIPROC_LoadMag_Ready_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet_Female
// ANIMATION : Weapon/Cerberus_Shotgun/Resource/Reload_C_Female.i3a
//
static void AIPROC_LoadBullet_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadBullet_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle_Ext_Male
// ANIMATION : Weapon/Cerberus_Shotgun/Resource/Idle_Folded_Male.i3a
//
static void AIPROC_Idle_Ext_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm);
}

static void AIPROC_Idle_Ext_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle_Female
// ANIMATION : Weapon/Cerberus_Shotgun/Resource/Idle_Female.i3a
//
static void AIPROC_Idle_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm);
}

static void AIPROC_Idle_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle_Male
// ANIMATION : Weapon/Cerberus_Shotgun/Resource/Idle_Male.i3a
//
static void AIPROC_Idle_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm);
}

static void AIPROC_Idle_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionActivate_Male
// ANIMATION : Weapon/Cerberus_Shotgun/Resource/Idle_Male.i3a
//
static void AIPROC_ExtensionActivate_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm);
}

static void AIPROC_ExtensionActivate_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionDeactivate_Male
// ANIMATION : Weapon/Cerberus_Shotgun/Resource/Change_Male.i3a
//
static void AIPROC_ExtensionDeactivate_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm);
}

static void AIPROC_ExtensionDeactivate_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Change_Male
// ANIMATION : Weapon/Cerberus_Shotgun/Resource/Change_Male.i3a
//
static void AIPROC_Change_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN( pCtx, tm);
}


static void AIPROC_Change_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change_Female
// ANIMATION : Weapon/Cerberus_Shotgun/Resource/Change_Female.i3a
//
static void AIPROC_Change_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN( pCtx, tm);
}

static void AIPROC_Change_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire_Female
// ANIMATION : Weapon/Cerberus_Shotgun/Resource/Attack_Female.i3a
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
// LoadMag_Ready_Female
// ANIMATION : Weapon/Cerberus_Shotgun/Resource/Reload_A_Female.i3a
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
// Change_Ext_Dual_Right_Male
// ANIMATION : Weapon/Cerberus_Shotgun/Resource/Change_Folded_Male_R.i3a
//
static void AIPROC_Change_Ext_Dual_Right_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN(pCtx, tm);
}

static void AIPROC_Change_Ext_Dual_Right_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change_Ext_Dual_Right_Female
// ANIMATION : Weapon/Cerberus_Shotgun/Resource/Change_Folded_Female_R.i3a
//
static void AIPROC_Change_Ext_Dual_Right_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN(pCtx, tm);
}

static void AIPROC_Change_Ext_Dual_Right_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
}



static i3::statictable::AISTATE_PROC s_ShotGunGenderWithDualTable[] =
{
	{	"Change_Ext_Dual_Left_Female",	AIPROC_Change_Ext_Dual_Left_Female_Run,		AIPROC_Change_Ext_Dual_Left_Female_Fin	},
	{	"Change_Ext_Dual_Left_Male",	AIPROC_Change_Ext_Dual_Left_Male_Run,		AIPROC_Change_Ext_Dual_Left_Male_Fin	},
	{	"Change_Ext_Dual_Right_Female",	AIPROC_Change_Ext_Dual_Right_Female_Run,	AIPROC_Change_Ext_Dual_Right_Female_Fin	},
	{	"Change_Ext_Dual_Right_Male",	AIPROC_Change_Ext_Dual_Right_Male_Run,		AIPROC_Change_Ext_Dual_Right_Male_Fin	},
	{	"Change_Female",				AIPROC_Change_Female_Run,					AIPROC_Change_Female_Fin				},
	{	"Change_Male",					AIPROC_Change_Male_Run,						AIPROC_Change_Male_Fin					},
	{	"ExtensionActivate_Female",		AIPROC_ExtensionActivate_Female_Run,		AIPROC_ExtensionActivate_Female_Fin		},
	{	"ExtensionActivate_Male",		AIPROC_ExtensionActivate_Male_Run,			AIPROC_ExtensionActivate_Male_Fin		},
	{	"ExtensionDeactivate_Female",	AIPROC_ExtensionDeactivate_Female_Run,		AIPROC_ExtensionDeactivate_Female_Fin	},
	{	"ExtensionDeactivate_Male",		AIPROC_ExtensionDeactivate_Male_Run,		AIPROC_ExtensionDeactivate_Male_Fin		},
	{	"Fire_Ext_Female",				AIPROC_Fire_Ext_Female_Run,					AIPROC_Fire_Ext_Female_Fin				},
	{	"Fire_Ext_Male",				AIPROC_Fire_Ext_Male_Run,					AIPROC_Fire_Ext_Male_Fin				},
	{	"Fire_Female",					AIPROC_Fire_Female_Run,						AIPROC_Fire_Female_Fin					},
	{	"Fire_Male",					AIPROC_Fire_Male_Run,						AIPROC_Fire_Male_Fin					},
	{	"Idle_Ext_Female",				AIPROC_Idle_Ext_Female_Run,					AIPROC_Idle_Ext_Female_Fin				},
	{	"Idle_Ext_Male",				AIPROC_Idle_Ext_Male_Run,					AIPROC_Idle_Ext_Male_Fin				},
	{	"Idle_Female",					AIPROC_Idle_Female_Run,						AIPROC_Idle_Female_Fin					},
	{	"Idle_Male",					AIPROC_Idle_Male_Run,						AIPROC_Idle_Male_Fin					},
	{	"LoadBullet_Female",			AIPROC_LoadBullet_Female_Run,				AIPROC_LoadBullet_Female_Fin			},
	{	"LoadBullet_Male",				AIPROC_LoadBullet_Male_Run,					AIPROC_LoadBullet_Male_Fin				},
	{	"LoadMag_Female",				AIPROC_LoadMag_Female_Run,					AIPROC_LoadMag_Female_Fin				},
	{	"LoadMag_Male",					AIPROC_LoadMag_Male_Run,					AIPROC_LoadMag_Male_Fin					},
	{	"LoadMag_Ready_Female",			AIPROC_LoadMag_Ready_Female_Run,			AIPROC_LoadMag_Ready_Female_Fin			},
	{	"LoadMag_Ready_Male",			AIPROC_LoadMag_Ready_Male_Run,				AIPROC_LoadMag_Ready_Male_Fin			},
};

AIDriverShotGunGenderWithDual::AIDriverShotGunGenderWithDual(void)
{
	setAIProcTable( s_ShotGunGenderWithDualTable, (sizeof(s_ShotGunGenderWithDualTable)/sizeof(s_ShotGunGenderWithDualTable[0])) );
}

AIDriverShotGunGenderWithDual::~AIDriverShotGunGenderWithDual(void)
{
}
