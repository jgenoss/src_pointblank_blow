#include "pch.h"
#include "AIDriverGun_13PV_KnifeGun.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverGun_13PV_KnifeGun);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// Idle_Ext
// ANIMATION : Weapon/Pindad_SS_SS1-R5_Carbine/Resource/Idle_Knife.i3a
//
static void AIPROC_Idle_Ext_Run( i3AIContext * pCtx, REAL32 tm)
{
		AIPROC_WEAPON_IDLE_RUN( pCtx, tm );
}

static void AIPROC_Idle_Ext_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag3PVFemale
// ANIMATION : Weapon/Pindad_SS_SS1-R5_Carbine/Resource/Reload_Female_3PV.i3a
//
static void AIPROC_LoadMag3PVFemale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMag3PVFemale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag1PV_Ext_Female
// ANIMATION : Weapon/Pindad_SS_SS1-R5_Carbine/Resource/Reload_Knife_Female_1PV.i3a
//
static void AIPROC_LoadMag1PV_Ext_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMag1PV_Ext_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// SecondaryFire
// ANIMATION : Weapon/Pindad_SS_SS1-R5_Carbine/Resource/Idle_Knife.i3a
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
// LoadMagMaster3PV_Ext
// ANIMATION : Weapon/Pindad_SS_SS1-R5_Carbine/Resource/Reload_Master_Knife_3PV.i3a
//
static void AIPROC_LoadMagMaster3PV_Ext_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMagMaster3PV_Ext_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle
// ANIMATION : Weapon/Pindad_SS_SS1-R5_Carbine/Resource/Idle.i3a
//
static void AIPROC_Idle_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm );
}

static void AIPROC_Idle_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag3PV_Ext_Female
// ANIMATION : Weapon/Pindad_SS_SS1-R5_Carbine/Resource/Reload_Knife_Female_3PV.i3a
//
static void AIPROC_LoadMag3PV_Ext_Female_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag3PV_Ext_Female_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag1PVFemale
// ANIMATION : Weapon/Pindad_SS_SS1-R5_Carbine/Resource/Reload_Female_1PV.i3a
//
static void AIPROC_LoadMag1PVFemale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag1PVFemale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagMaster1PV
// ANIMATION : Weapon/Pindad_SS_SS1-R5_Carbine/Resource/Reload_Master_1PV.i3a
//
static void AIPROC_LoadMagMaster1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMagMaster1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire
// ANIMATION : Weapon/Pindad_SS_SS1-R5_Carbine/Resource/Idle.i3a
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
// Fire_Ext
// ANIMATION : Weapon/Pindad_SS_SS1-R5_Carbine/Resource/Idle_Knife.i3a
//
static void AIPROC_Fire_Ext_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_Fire_Ext_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionActivate3PV
// ANIMATION : Weapon/Pindad_SS_SS1-R5_Carbine/Resource/Knife_On_3PV.i3a
//
static void AIPROC_ExtensionActivate3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_EXTENSION_ACTIVATE_RUN( pCtx, tm);
}

static void AIPROC_ExtensionActivate3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionDeactivate3PV
// ANIMATION : Weapon/Pindad_SS_SS1-R5_Carbine/Resource/Knife_Off_3PV.i3a
//
static void AIPROC_ExtensionDeactivate3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_EXTENSION_DEACTIVATE_RUN( pCtx, tm);
}

static void AIPROC_ExtensionDeactivate3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionActivate
// ANIMATION : Weapon/Pindad_SS_SS1-R5_Carbine/Resource/Knife_On_1PV.i3a
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
// ANIMATION : Weapon/Pindad_SS_SS1-R5_Carbine/Resource/Knife_Off_1PV.i3a
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
// LoadMag1PVMale
// ANIMATION : Weapon/Pindad_SS_SS1-R5_Carbine/Resource/Reload_Male_1PV.i3a
//
static void AIPROC_LoadMag1PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag1PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag1PV_Ext_Male
// ANIMATION : Weapon/Pindad_SS_SS1-R5_Carbine/Resource/Reload_Knife_Male_1PV.i3a
//
static void AIPROC_LoadMag1PV_Ext_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag1PV_Ext_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag3PVMale
// ANIMATION : Weapon/Pindad_SS_SS1-R5_Carbine/Resource/Reload_Male_3PV.i3a
//
static void AIPROC_LoadMag3PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag3PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag3PV_Ext_Male
// ANIMATION : Weapon/Pindad_SS_SS1-R5_Carbine/Resource/Reload_Knife_Male_3PV.i3a
//
static void AIPROC_LoadMag3PV_Ext_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag3PV_Ext_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagMaster1PV_Ext
// ANIMATION : Weapon/Pindad_SS_SS1-R5_Carbine/Resource/Reload_Master_Knife_1PV.i3a
//
static void AIPROC_LoadMagMaster1PV_Ext_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMagMaster1PV_Ext_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagMaster3PV
// ANIMATION : Weapon/Pindad_SS_SS1-R5_Carbine/Resource/Reload_Master_3PV.i3a
//
static void AIPROC_LoadMagMaster3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMagMaster3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}



static i3::statictable::AISTATE_PROC s_Gun_13PV_KnifeGunTable[] =
{
	{	"ExtensionActivate",	AIPROC_ExtensionActivate_Run,	AIPROC_ExtensionActivate_Fin	},
	{	"ExtensionActivate3PV",	AIPROC_ExtensionActivate3PV_Run,	AIPROC_ExtensionActivate3PV_Fin	},
	{	"ExtensionDeactivate",	AIPROC_ExtensionDeactivate_Run,	AIPROC_ExtensionDeactivate_Fin	},
	{	"ExtensionDeactivate3PV",	AIPROC_ExtensionDeactivate3PV_Run,	AIPROC_ExtensionDeactivate3PV_Fin	},
	{	"Fire",	AIPROC_Fire_Run,	AIPROC_Fire_Fin	},
	{	"Fire_Ext",	AIPROC_Fire_Ext_Run,	AIPROC_Fire_Ext_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"Idle_Ext",	AIPROC_Idle_Ext_Run,	AIPROC_Idle_Ext_Fin	},
	{	"LoadMag1PV_Ext_Female",	AIPROC_LoadMag1PV_Ext_Female_Run,	AIPROC_LoadMag1PV_Ext_Female_Fin	},
	{	"LoadMag1PV_Ext_Male",	AIPROC_LoadMag1PV_Ext_Male_Run,	AIPROC_LoadMag1PV_Ext_Male_Fin	},
	{	"LoadMag1PVFemale",	AIPROC_LoadMag1PVFemale_Run,	AIPROC_LoadMag1PVFemale_Fin	},
	{	"LoadMag1PVMale",	AIPROC_LoadMag1PVMale_Run,	AIPROC_LoadMag1PVMale_Fin	},
	{	"LoadMag3PV_Ext_Female",	AIPROC_LoadMag3PV_Ext_Female_Run,	AIPROC_LoadMag3PV_Ext_Female_Fin	},
	{	"LoadMag3PV_Ext_Male",	AIPROC_LoadMag3PV_Ext_Male_Run,	AIPROC_LoadMag3PV_Ext_Male_Fin	},
	{	"LoadMag3PVFemale",	AIPROC_LoadMag3PVFemale_Run,	AIPROC_LoadMag3PVFemale_Fin	},
	{	"LoadMag3PVMale",	AIPROC_LoadMag3PVMale_Run,	AIPROC_LoadMag3PVMale_Fin	},
	{	"LoadMagMaster1PV",	AIPROC_LoadMagMaster1PV_Run,	AIPROC_LoadMagMaster1PV_Fin	},
	{	"LoadMagMaster1PV_Ext",	AIPROC_LoadMagMaster1PV_Ext_Run,	AIPROC_LoadMagMaster1PV_Ext_Fin	},
	{	"LoadMagMaster3PV",	AIPROC_LoadMagMaster3PV_Run,	AIPROC_LoadMagMaster3PV_Fin	},
	{	"LoadMagMaster3PV_Ext",	AIPROC_LoadMagMaster3PV_Ext_Run,	AIPROC_LoadMagMaster3PV_Ext_Fin	},
	{	"SecondaryFire",	AIPROC_SecondaryFire_Run,	AIPROC_SecondaryFire_Fin	},
};

AIDriverGun_13PV_KnifeGun::AIDriverGun_13PV_KnifeGun(void)
{
	setAIProcTable( s_Gun_13PV_KnifeGunTable, (sizeof(s_Gun_13PV_KnifeGunTable)/sizeof(s_Gun_13PV_KnifeGunTable[0])) );
}

AIDriverGun_13PV_KnifeGun::~AIDriverGun_13PV_KnifeGun(void)
{
}
