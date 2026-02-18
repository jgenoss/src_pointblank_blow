#include "pch.h"
#include "AIDriverGunSniper_LoadBullet_Type.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverGunSniper_LoadBullet_Type);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagMaster3PVFemale
//
static void AIPROC_LoadMagMaster3PVFemale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMagMaster3PVFemale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire
// ANIMATION : Weapon/M70/Resource/Idle.i3a
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
// LoadMag_Ready_Master1PVFemale
//
static void AIPROC_LoadMagReadyMaster1PVFemale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_READY_RUN( pCtx, tm);
}

static void AIPROC_LoadMagReadyMaster1PVFemale_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle
// ANIMATION : Weapon/M70/Resource/Idle.i3a
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
// LoadMagMaster3PVMale
//
static void AIPROC_LoadMagMaster3PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMagMaster3PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagReadyMaster1PVMale
//
static void AIPROC_LoadMagReadyMaster1PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_READY_RUN( pCtx, tm);
}

static void AIPROC_LoadMagReadyMaster1PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagMaster1PVFemale
//
static void AIPROC_LoadMagMaster1PVFemale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMagMaster1PVFemale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagMaster1PVMale
//
static void AIPROC_LoadMagMaster1PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMagMaster1PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagReadyMaster3PVFemale
//
static void AIPROC_LoadMagReadyMaster3PVFemale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_READY_RUN( pCtx, tm);
}

static void AIPROC_LoadMagReadyMaster3PVFemale_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagReadyMaster3PVMale
//
static void AIPROC_LoadMagReadyMaster3PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_READY_RUN( pCtx, tm);
}

static void AIPROC_LoadMagReadyMaster3PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// FireBulletMaster3PVFemale
//
static void AIPROC_FireBulletMaster3PVFemale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_BULLET_RUN( pCtx, tm);
}

static void AIPROC_FireBulletMaster3PVFemale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBulletMaster1PVFemale
//
static void AIPROC_LoadBulletMaster1PVFemale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadBulletMaster1PVFemale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// FireBulletMaster3PVMale
//
static void AIPROC_FireBulletMaster3PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_BULLET_RUN( pCtx, tm);
}

static void AIPROC_FireBulletMaster3PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBulletMaster1PVMale
//
static void AIPROC_LoadBulletMaster1PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadBulletMaster1PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// FireBulletMaster1PVFemale
//
static void AIPROC_FireBulletMaster1PVFemale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_BULLET_RUN( pCtx, tm);
}

static void AIPROC_FireBulletMaster1PVFemale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// FireBulletMaster1PVMale
//
static void AIPROC_FireBulletMaster1PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_BULLET_RUN( pCtx, tm);
}

static void AIPROC_FireBulletMaster1PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBulletMaster3PVFemale
//
static void AIPROC_LoadBulletMaster3PVFemale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadBulletMaster3PVFemale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBulletMaster3PVMale
//
static void AIPROC_LoadBulletMaster3PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadBulletMaster3PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag3PVFemale
// ANIMATION : Weapon/M70/Resource/ReloadB_Female_3PV.i3a
//
static void AIPROC_LoadMag3PVFemale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMag3PVFemale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagReady1PVFemale
// ANIMATION : Weapon/M70/Resource/ReloadA_Female_1PV.i3a
//
static void AIPROC_LoadMagReady1PVFemale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_READY_RUN( pCtx, tm);
}

static void AIPROC_LoadMagReady1PVFemale_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag3PVMale
// ANIMATION : Weapon/M70/Resource/ReloadB_Male_3PV.i3a
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
// LoadMagReady1PVMale
// ANIMATION : Weapon/M70/Resource/ReloadA_Male_1PV.i3a
//
static void AIPROC_LoadMagReady1PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_READY_RUN( pCtx, tm);
}

static void AIPROC_LoadMagReady1PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMag1PVFemale
// ANIMATION : Weapon/M70/Resource/ReloadB_Female_1PV.i3a
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
// LoadMag1PVMale
// ANIMATION : Weapon/M70/Resource/ReloadB_Male_1PV.i3a
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
// LoadMagReady3PVFemale
// ANIMATION : Weapon/M70/Resource/ReloadA_Female_3PV.i3a
//
static void AIPROC_LoadMagReady3PVFemale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_READY_RUN( pCtx, tm);
}

static void AIPROC_LoadMagReady3PVFemale_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagReady3PVMale
// ANIMATION : Weapon/M70/Resource/ReloadA_Male_3PV.i3a
//
static void AIPROC_LoadMagReady3PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_MAGAZINE_READY_RUN( pCtx, tm);
}

static void AIPROC_LoadMagReady3PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// FireBullet3PVFemale
// ANIMATION : Weapon/M70/Resource/ReloadD_Female_3PV.i3a
//
static void AIPROC_FireBullet3PVFemale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_BULLET_RUN( pCtx, tm);
}

static void AIPROC_FireBullet3PVFemale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet1PVFemale
// ANIMATION : Weapon/M70/Resource/ReloadC_Female_1PV.i3a
//
static void AIPROC_LoadBullet1PVFemale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadBullet1PVFemale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// FireBullet3PVMale
// ANIMATION : Weapon/M70/Resource/ReloadD_Male_3PV.i3a
//
static void AIPROC_FireBullet3PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_BULLET_RUN( pCtx, tm);
}

static void AIPROC_FireBullet3PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet1PVMale
// ANIMATION : Weapon/M70/Resource/ReloadC_Male_1PV.i3a
//
static void AIPROC_LoadBullet1PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadBullet1PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// FireBullet1PVFemale
// ANIMATION : Weapon/M70/Resource/ReloadD_Female_1PV.i3a
//
static void AIPROC_FireBullet1PVFemale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_BULLET_RUN( pCtx, tm);
}

static void AIPROC_FireBullet1PVFemale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// FireBullet1PVMale
// ANIMATION : Weapon/M70/Resource/ReloadD_Male_1PV.i3a
//
static void AIPROC_FireBullet1PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_BULLET_RUN( pCtx, tm);
}

static void AIPROC_FireBullet1PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet3PVFemale
// ANIMATION : Weapon/M70/Resource/ReloadC_Female_3PV.i3a
//
static void AIPROC_LoadBullet3PVFemale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadBullet3PVFemale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet3PVMale
// ANIMATION : Weapon/M70/Resource/ReloadC_Male_3PV.i3a
//
static void AIPROC_LoadBullet3PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadBullet3PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm);
}



static i3::statictable::AISTATE_PROC s_GunSniper_LoadBullet_TypeTable[] =
{
	{	"ExtensionActivate",	AIPROC_ExtensionActivate_Run,	AIPROC_ExtensionActivate_Fin	},
	{	"ExtensionDeactivate",	AIPROC_ExtensionDeactivate_Run,	AIPROC_ExtensionDeactivate_Fin	},
	{	"Fire",	AIPROC_Fire_Run,	AIPROC_Fire_Fin	},
	{	"FireBullet1PVFemale",	AIPROC_FireBullet1PVFemale_Run,	AIPROC_FireBullet1PVFemale_Fin	},
	{	"FireBullet1PVMale",	AIPROC_FireBullet1PVMale_Run,	AIPROC_FireBullet1PVMale_Fin	},
	{	"FireBullet3PVFemale",	AIPROC_FireBullet3PVFemale_Run,	AIPROC_FireBullet3PVFemale_Fin	},
	{	"FireBullet3PVMale",	AIPROC_FireBullet3PVMale_Run,	AIPROC_FireBullet3PVMale_Fin	},
	{	"FireBulletMaster1PVFemale",	AIPROC_FireBulletMaster1PVFemale_Run,	AIPROC_FireBulletMaster1PVFemale_Fin	},
	{	"FireBulletMaster1PVMale",	AIPROC_FireBulletMaster1PVMale_Run,	AIPROC_FireBulletMaster1PVMale_Fin	},
	{	"FireBulletMaster3PVFemale",	AIPROC_FireBulletMaster3PVFemale_Run,	AIPROC_FireBulletMaster3PVFemale_Fin	},
	{	"FireBulletMaster3PVMale",	AIPROC_FireBulletMaster3PVMale_Run,	AIPROC_FireBulletMaster3PVMale_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"LoadBullet1PVFemale",	AIPROC_LoadBullet1PVFemale_Run,	AIPROC_LoadBullet1PVFemale_Fin	},
	{	"LoadBullet1PVMale",	AIPROC_LoadBullet1PVMale_Run,	AIPROC_LoadBullet1PVMale_Fin	},
	{	"LoadBullet3PVFemale",	AIPROC_LoadBullet3PVFemale_Run,	AIPROC_LoadBullet3PVFemale_Fin	},
	{	"LoadBullet3PVMale",	AIPROC_LoadBullet3PVMale_Run,	AIPROC_LoadBullet3PVMale_Fin	},
	{	"LoadBulletMaster1PVFemale",	AIPROC_LoadBulletMaster1PVFemale_Run,	AIPROC_LoadBulletMaster1PVFemale_Fin	},
	{	"LoadBulletMaster1PVMale",	AIPROC_LoadBulletMaster1PVMale_Run,	AIPROC_LoadBulletMaster1PVMale_Fin	},
	{	"LoadBulletMaster3PVFemale",	AIPROC_LoadBulletMaster3PVFemale_Run,	AIPROC_LoadBulletMaster3PVFemale_Fin	},
	{	"LoadBulletMaster3PVMale",	AIPROC_LoadBulletMaster3PVMale_Run,	AIPROC_LoadBulletMaster3PVMale_Fin	},
	{	"LoadMag1PVFemale",	AIPROC_LoadMag1PVFemale_Run,	AIPROC_LoadMag1PVFemale_Fin	},
	{	"LoadMag1PVMale",	AIPROC_LoadMag1PVMale_Run,	AIPROC_LoadMag1PVMale_Fin	},
	{	"LoadMag3PVFemale",	AIPROC_LoadMag3PVFemale_Run,	AIPROC_LoadMag3PVFemale_Fin	},
	{	"LoadMag3PVMale",	AIPROC_LoadMag3PVMale_Run,	AIPROC_LoadMag3PVMale_Fin	},
	{	"LoadMagMaster1PVFemale",	AIPROC_LoadMagMaster1PVFemale_Run,	AIPROC_LoadMagMaster1PVFemale_Fin	},
	{	"LoadMagMaster1PVMale",	AIPROC_LoadMagMaster1PVMale_Run,	AIPROC_LoadMagMaster1PVMale_Fin	},
	{	"LoadMagMaster3PVFemale",	AIPROC_LoadMagMaster3PVFemale_Run,	AIPROC_LoadMagMaster3PVFemale_Fin	},
	{	"LoadMagMaster3PVMale",	AIPROC_LoadMagMaster3PVMale_Run,	AIPROC_LoadMagMaster3PVMale_Fin	},
	{	"LoadMagReady1PVFemale",	AIPROC_LoadMagReady1PVFemale_Run,	AIPROC_LoadMagReady1PVFemale_Fin	},
	{	"LoadMagReady1PVMale",	AIPROC_LoadMagReady1PVMale_Run,	AIPROC_LoadMagReady1PVMale_Fin	},
	{	"LoadMagReady3PVFemale",	AIPROC_LoadMagReady3PVFemale_Run,	AIPROC_LoadMagReady3PVFemale_Fin	},
	{	"LoadMagReady3PVMale",	AIPROC_LoadMagReady3PVMale_Run,	AIPROC_LoadMagReady3PVMale_Fin	},
	{	"LoadMagReadyMaster1PVFemale",	AIPROC_LoadMagReadyMaster1PVFemale_Run,	AIPROC_LoadMagReadyMaster1PVFemale_Fin	},
	{	"LoadMagReadyMaster1PVMale",	AIPROC_LoadMagReadyMaster1PVMale_Run,	AIPROC_LoadMagReadyMaster1PVMale_Fin	},
	{	"LoadMagReadyMaster3PVFemale",	AIPROC_LoadMagReadyMaster3PVFemale_Run,	AIPROC_LoadMagReadyMaster3PVFemale_Fin	},
	{	"LoadMagReadyMaster3PVMale",	AIPROC_LoadMagReadyMaster3PVMale_Run,	AIPROC_LoadMagReadyMaster3PVMale_Fin	},
	{	"LoadMagB",	AIPROC_LoadMagB_Run,	AIPROC_LoadMagB_Fin	},
};

AIDriverGunSniper_LoadBullet_Type::AIDriverGunSniper_LoadBullet_Type(void)
{
	setAIProcTable( s_GunSniper_LoadBullet_TypeTable, (sizeof(s_GunSniper_LoadBullet_TypeTable)/sizeof(s_GunSniper_LoadBullet_TypeTable[0])) );
}

AIDriverGunSniper_LoadBullet_Type::~AIDriverGunSniper_LoadBullet_Type(void)
{
}
