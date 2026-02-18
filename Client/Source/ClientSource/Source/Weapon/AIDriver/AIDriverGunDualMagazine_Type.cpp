#include "pch.h"
#include "AIDriverGunDualMagazine_Type.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverGunDualMagazine_Type);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// Idle_Right
// ANIMATION : Weapon/AK-47_DualMag/Resource/Idle_Left.i3a
//
static void AIPROC_Idle_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm );
}

static void AIPROC_Idle_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet_Right
// ANIMATION : Weapon/AK-47_DualMag/Resource/Idle_Left.i3a
//
static void AIPROC_LoadBullet_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_LoadBullet_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire_Right
// ANIMATION : Weapon/AK-47_DualMag/Resource/Attack_Left.i3a
//
static void AIPROC_Fire_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
		AIPROC_WEAPON_FIRE_RUN( pCtx, tm )
}

static void AIPROC_Fire_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm );
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
// LoadMagB
//
static void AIPROC_LoadMagB_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_LoadMagB_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle
// ANIMATION : Weapon/AK-47_DualMag/Resource/Idle_Right.i3a
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
// LoadBullet
// ANIMATION : Weapon/AK-47_DualMag/Resource/Idle_Right.i3a
//
static void AIPROC_LoadBullet_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_LoadBullet_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_RightShift
// ANIMATION : Weapon/AK-47_DualMag/Resource/Reload_C.i3a
//
static void AIPROC_LoadMag_RightShift_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_QUICK_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMag_RightShift_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_QUICK_LOAD_MAGAZINE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire
// ANIMATION : Weapon/AK-47_DualMag/Resource/Attack_Right.i3a
//
static void AIPROC_Fire_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm );
}

static void AIPROC_Fire_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_Right
// ANIMATION : Weapon/AK-47_DualMag/Resource/Reload_B.i3a
//
static void AIPROC_LoadMag_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_QUICK_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMag_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_QUICK_LOAD_MAGAZINE_FIN( pCtx, tm );
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
// LoadMag3PVMale
// ANIMATION : Weapon/AK-47_DualMag/Resource/Reload_A_Male_3PV.i3a
//
static void AIPROC_LoadMag3PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMag3PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_RightAll_1PV_FEMALE
// ANIMATION : Weapon/AK-47_DualMag/Resource/Reload_D_Female_1PV.i3a
//
static void AIPROC_LoadMag_RightAll_1PV_FEMALE_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMag_RightAll_1PV_FEMALE_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagMaster1PV
// ANIMATION : Weapon/AK-47_DualMag/Resource/Reload_A_Master_1PV.i3a
//
static void AIPROC_LoadMagMaster1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
		AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMagMaster1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
		AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagMaster3PV
// ANIMATION : Weapon/AK-47_DualMag/Resource/Reload_A_Master_3PV.i3a
//
static void AIPROC_LoadMagMaster3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
		AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMagMaster3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
		AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_RightAllMaster3PV
// ANIMATION : Weapon/AK-47_DualMag/Resource/Reload_D_Master_3PV.i3a
//
static void AIPROC_LoadMag_RightAllMaster3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
		AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMag_RightAllMaster3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
		AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_RightAllMaster1PV
// ANIMATION : Weapon/AK-47_DualMag/Resource/Reload_D_Master_1PV.i3a
//
static void AIPROC_LoadMag_RightAllMaster1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
		AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMag_RightAllMaster1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
		AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag1PVFemale
// ANIMATION : Weapon/AK-47_DualMag/Resource/Reload_A_Female_1PV.i3a
//
static void AIPROC_LoadMag1PVFemale_Run( i3AIContext * pCtx, REAL32 tm)
{
		AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMag1PVFemale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag3PVFemale
// ANIMATION : Weapon/AK-47_DualMag/Resource/Reload_A_Female_3PV.i3a
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
// LoadMag1PVMale
// ANIMATION : Weapon/AK-47_DualMag/Resource/Reload_A_Male_1PV.i3a
//
static void AIPROC_LoadMag1PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMag1PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_RightAll_1PV_MALE
// ANIMATION : Weapon/AK-47_DualMag/Resource/Reload_D_Male_1PV.i3a
//
static void AIPROC_LoadMag_RightAll_1PV_MALE_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMag_RightAll_1PV_MALE_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_RightAll_3PV_MALE
// ANIMATION : Weapon/AK-47_DualMag/Resource/Reload_D_Male_3PV.i3a
//
static void AIPROC_LoadMag_RightAll_3PV_MALE_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMag_RightAll_3PV_MALE_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm );
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag_RightAll_3PV_FEMALE
// ANIMATION : Weapon/AK-47_DualMag/Resource/Reload_D_Female_3PV.i3a
//
static void AIPROC_LoadMag_RightAll_3PV_FEMALE_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm );
}

static void AIPROC_LoadMag_RightAll_3PV_FEMALE_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm );
}



static i3::statictable::AISTATE_PROC s_GunDualMagazine_TypeTable[] =
{
	{	"ExtensionActivate",	AIPROC_ExtensionActivate_Run,	AIPROC_ExtensionActivate_Fin	},
	{	"ExtensionDeactivate",	AIPROC_ExtensionDeactivate_Run,	AIPROC_ExtensionDeactivate_Fin	},
	{	"Fire",	AIPROC_Fire_Run,	AIPROC_Fire_Fin	},
	{	"Fire_Right",	AIPROC_Fire_Right_Run,	AIPROC_Fire_Right_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"Idle_Right",	AIPROC_Idle_Right_Run,	AIPROC_Idle_Right_Fin	},
	{	"LoadBullet",	AIPROC_LoadBullet_Run,	AIPROC_LoadBullet_Fin	},
	{	"LoadBullet_Right",	AIPROC_LoadBullet_Right_Run,	AIPROC_LoadBullet_Right_Fin	},
	{	"LoadMag1PVFemale",	AIPROC_LoadMag1PVFemale_Run,	AIPROC_LoadMag1PVFemale_Fin	},
	{	"LoadMag1PVMale",	AIPROC_LoadMag1PVMale_Run,	AIPROC_LoadMag1PVMale_Fin	},
	{	"LoadMag3PVFemale",	AIPROC_LoadMag3PVFemale_Run,	AIPROC_LoadMag3PVFemale_Fin	},
	{	"LoadMag3PVMale",	AIPROC_LoadMag3PVMale_Run,	AIPROC_LoadMag3PVMale_Fin	},
	{	"LoadMag_Right",	AIPROC_LoadMag_Right_Run,	AIPROC_LoadMag_Right_Fin	},
	{	"LoadMag_RightAll_1PV_FEMALE",	AIPROC_LoadMag_RightAll_1PV_FEMALE_Run,	AIPROC_LoadMag_RightAll_1PV_FEMALE_Fin	},
	{	"LoadMag_RightAll_1PV_MALE",	AIPROC_LoadMag_RightAll_1PV_MALE_Run,	AIPROC_LoadMag_RightAll_1PV_MALE_Fin	},
	{	"LoadMag_RightAll_3PV_FEMALE",	AIPROC_LoadMag_RightAll_3PV_FEMALE_Run,	AIPROC_LoadMag_RightAll_3PV_FEMALE_Fin	},
	{	"LoadMag_RightAll_3PV_MALE",	AIPROC_LoadMag_RightAll_3PV_MALE_Run,	AIPROC_LoadMag_RightAll_3PV_MALE_Fin	},
	{	"LoadMag_RightAllMaster1PV",	AIPROC_LoadMag_RightAllMaster1PV_Run,	AIPROC_LoadMag_RightAllMaster1PV_Fin	},
	{	"LoadMag_RightAllMaster3PV",	AIPROC_LoadMag_RightAllMaster3PV_Run,	AIPROC_LoadMag_RightAllMaster3PV_Fin	},
	{	"LoadMag_RightShift",	AIPROC_LoadMag_RightShift_Run,	AIPROC_LoadMag_RightShift_Fin	},
	{	"LoadMagB",	AIPROC_LoadMagB_Run,	AIPROC_LoadMagB_Fin	},
	{	"LoadMagMaster1PV",	AIPROC_LoadMagMaster1PV_Run,	AIPROC_LoadMagMaster1PV_Fin	},
	{	"LoadMagMaster3PV",	AIPROC_LoadMagMaster3PV_Run,	AIPROC_LoadMagMaster3PV_Fin	},
};

AIDriverGunDualMagazine_Type::AIDriverGunDualMagazine_Type(void)
{
	setAIProcTable( s_GunDualMagazine_TypeTable, (sizeof(s_GunDualMagazine_TypeTable)/sizeof(s_GunDualMagazine_TypeTable[0])) );
}

AIDriverGunDualMagazine_Type::~AIDriverGunDualMagazine_Type(void)
{
}
