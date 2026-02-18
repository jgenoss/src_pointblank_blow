#include "pch.h"
#include "AIDriverGunMachineGun.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverGunMachineGun);//, i3AIDriver);

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
// LoadBullet1PVFemale
// ANIMATION : Weapon/MK46/Resource/LoadBullet_Female_1PV.i3a
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
// Idle
// ANIMATION : Weapon/MK46/Resource/IDLE.I3A
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
// Fire
// ANIMATION : Weapon/MK46/Resource/ATTACK_01.I3A
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
// LoadMag
// ANIMATION : Weapon/MK46/Resource/RELOADA.I3A
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
// Fire1
// ANIMATION : Weapon/MK46/Resource/ATTACK_02.I3A
//
static void AIPROC_Fire1_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_Fire1_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire2
// ANIMATION : Weapon/MK46/Resource/ATTACK_03.I3A
//
static void AIPROC_Fire2_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_Fire2_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire3
// ANIMATION : Weapon/MK46/Resource/ATTACK_04.I3A
//
static void AIPROC_Fire3_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_Fire3_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire4
// ANIMATION : Weapon/MK46/Resource/ATTACK_05.I3A
//
static void AIPROC_Fire4_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_Fire4_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Move
// ANIMATION : Weapon/MK46/Resource/MOVE_ATTACKIDLE.I3A
//
static void AIPROC_Move_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm);
}

static void AIPROC_Move_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Change
// ANIMATION : Weapon/MK46/Resource/CHANGE.I3A
//
static void AIPROC_Change_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm);
}

static void AIPROC_Change_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Damage
// ANIMATION : Weapon/MK46/Resource/DAMAGE_ATTACKIDLE.I3A
//
static void AIPROC_Damage_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm);
}

static void AIPROC_Damage_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// JumpEnd
// ANIMATION : Weapon/MK46/Resource/JUMPEND_ATTACKIDLE.I3A
//
static void AIPROC_JumpEnd_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm);
}

static void AIPROC_JumpEnd_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet3PVMale
// ANIMATION : Weapon/MK46/Resource/LoadBullet_Male_3PV.i3a
//
static void AIPROC_LoadBullet3PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadBullet3PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet3PVFemale
// ANIMATION : Weapon/MK46/Resource/LoadBullet_Female_3PV.i3a
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
// LoadBullet1PVMale
// ANIMATION : Weapon/MK46/Resource/LoadBullet_Male_1PV.i3a
//
static void AIPROC_LoadBullet1PVMale_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_RUN( pCtx, tm);
}

static void AIPROC_LoadBullet1PVMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_LOAD_BULLET_FIN( pCtx, tm);
}



static i3::statictable::AISTATE_PROC s_GunMachineGunTable[] =
{
	{	"Change",	AIPROC_Change_Run,	AIPROC_Change_Fin	},
	{	"Damage",	AIPROC_Damage_Run,	AIPROC_Damage_Fin	},
	{	"ExtensionActivate",	AIPROC_ExtensionActivate_Run,	AIPROC_ExtensionActivate_Fin	},
	{	"ExtensionDeactivate",	AIPROC_ExtensionDeactivate_Run,	AIPROC_ExtensionDeactivate_Fin	},
	{	"Fire",	AIPROC_Fire_Run,	AIPROC_Fire_Fin	},
	{	"Fire1",	AIPROC_Fire1_Run,	AIPROC_Fire1_Fin	},
	{	"Fire2",	AIPROC_Fire2_Run,	AIPROC_Fire2_Fin	},
	{	"Fire3",	AIPROC_Fire3_Run,	AIPROC_Fire3_Fin	},
	{	"Fire4",	AIPROC_Fire4_Run,	AIPROC_Fire4_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"JumpEnd",	AIPROC_JumpEnd_Run,	AIPROC_JumpEnd_Fin	},
	{	"LoadBullet1PVFemale",	AIPROC_LoadBullet1PVFemale_Run,	AIPROC_LoadBullet1PVFemale_Fin	},
	{	"LoadBullet1PVMale",	AIPROC_LoadBullet1PVMale_Run,	AIPROC_LoadBullet1PVMale_Fin	},
	{	"LoadBullet3PVFemale",	AIPROC_LoadBullet3PVFemale_Run,	AIPROC_LoadBullet3PVFemale_Fin	},
	{	"LoadBullet3PVMale",	AIPROC_LoadBullet3PVMale_Run,	AIPROC_LoadBullet3PVMale_Fin	},
	{	"LoadMag",	AIPROC_LoadMag_Run,	AIPROC_LoadMag_Fin	},
	{	"Move",	AIPROC_Move_Run,	AIPROC_Move_Fin	},
};

AIDriverGunMachineGun::AIDriverGunMachineGun(void)
{
	setAIProcTable( s_GunMachineGunTable, (sizeof(s_GunMachineGunTable)/sizeof(s_GunMachineGunTable[0])) );
}

AIDriverGunMachineGun::~AIDriverGunMachineGun(void)
{
}
