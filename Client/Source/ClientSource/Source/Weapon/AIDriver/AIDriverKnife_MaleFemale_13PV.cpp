#include "pch.h"
#include "AIDriverKnife_MaleFemale_13PV.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverKnife_MaleFemale_13PV);

////////////////////////////////////////////////////////////////////////////////
//
// Attack_A_Male_1PV
// ANIMATION : Weapon/Nunchaku/Resource/Attack_A_Male_1PV.i3a
//
static void AIPROC_Attack_A_Male_1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN(pCtx, tm);
}

static void AIPROC_Attack_A_Male_1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Attack_B_Male_3PV
// ANIMATION : Weapon/Nunchaku/Resource/Attack_B_Male_3PV.i3a
//
static void AIPROC_Attack_B_Male_3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnSecondaryFire( pCtx, tm);
}

static void AIPROC_Attack_B_Male_3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Attack_B_FeMale_1PV
// ANIMATION : Weapon/Nunchaku/Resource/Attack_B_Female_1PV.i3a
//
static void AIPROC_Attack_B_FeMale_1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnSecondaryFire( pCtx, tm);
}

static void AIPROC_Attack_B_FeMale_1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Attack_B_FeMale_3PV
// ANIMATION : Weapon/Nunchaku/Resource/Attack_B_Female_1PV.i3a
//
static void AIPROC_Attack_B_FeMale_3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnSecondaryFire( pCtx, tm);
}

static void AIPROC_Attack_B_FeMale_3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Attack_B_Male_1PV
// ANIMATION : Weapon/Nunchaku/Resource/Attack_B_Male_1PV.i3a
//
static void AIPROC_Attack_B_Male_1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = (WeaponBase *) pCtx->getOwner();
	pWeapon->OnSecondaryFire( pCtx, tm);
}

static void AIPROC_Attack_B_Male_1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change_Male_1PV
// ANIMATION : Weapon/Nunchaku/Resource/Change_Male_1PV.i3a
//
static void AIPROC_Change_Male_1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN(pCtx, tm);
}

static void AIPROC_Change_Male_1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change_Male_3PV
// ANIMATION : Weapon/Nunchaku/Resource/Change_Male_3PV.i3a
//
static void AIPROC_Change_Male_3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN(pCtx, tm);
}

static void AIPROC_Change_Male_3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change_FeMale_1PV
// ANIMATION : Weapon/Nunchaku/Resource/Change_Female_1PV.i3a
//
static void AIPROC_Change_FeMale_1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN(pCtx, tm);
}

static void AIPROC_Change_FeMale_1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change_FeMale_3PV
// ANIMATION : Weapon/Nunchaku/Resource/Change_Female_1PV.i3a
//
static void AIPROC_Change_FeMale_3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN(pCtx, tm);
}

static void AIPROC_Change_FeMale_3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_IdleA_Male
// ANIMATION : Weapon/Nunchaku/Resource/UI_IdleA_Male.i3a
//
static void AIPROC_UI_IdleA_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_IdleA_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_IdleB_Male
// ANIMATION : Weapon/Nunchaku/Resource/UI_IdleB_Male.i3a
//
static void AIPROC_UI_IdleB_Male_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_IdleB_Male_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle_Male_1PV
// ANIMATION : Weapon/Nunchaku/Resource/AttackIdle_Male_1PV.i3a
//
static void AIPROC_Idle_Male_1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN(pCtx, tm);
}

static void AIPROC_Idle_Male_1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle_Male_3PV
// ANIMATION : Weapon/Nunchaku/Resource/AttackIdle_Male_3PV.i3a
//
static void AIPROC_Idle_Male_3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN(pCtx, tm);
}

static void AIPROC_Idle_Male_3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle_FeMale_1PV
// ANIMATION : Weapon/Nunchaku/Resource/AttackIdle_Female_1PV.i3a
//
static void AIPROC_Idle_FeMale_1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN(pCtx, tm);
}

static void AIPROC_Idle_FeMale_1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle_FeMale_3PV
// ANIMATION : Weapon/Nunchaku/Resource/AttackIdle_Female_1PV.i3a
//
static void AIPROC_Idle_FeMale_3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN(pCtx, tm);
}

static void AIPROC_Idle_FeMale_3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Attack_A_Male_3PV
// ANIMATION : Weapon/Nunchaku/Resource/Attack_A_Male_3PV.i3a
//
static void AIPROC_Attack_A_Male_3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN(pCtx, tm);
}

static void AIPROC_Attack_A_Male_3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Attack_A_FeMale_1PV
// ANIMATION : Weapon/Nunchaku/Resource/Attack_A_Female_1PV.i3a
//
static void AIPROC_Attack_A_FeMale_1PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN(pCtx, tm);
}

static void AIPROC_Attack_A_FeMale_1PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Attack_A_FeMale_3PV
// ANIMATION : Weapon/Nunchaku/Resource/Attack_A_Female_1PV.i3a
//
static void AIPROC_Attack_A_FeMale_3PV_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN(pCtx, tm);
}

static void AIPROC_Attack_A_FeMale_3PV_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_IdleA_FeMale
// ANIMATION : Weapon/Nunchaku/Resource/UI_IdleA_CT-FORCE_Female(Hide).i3a
//
static void AIPROC_UI_IdleA_FeMale_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_IdleA_FeMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_IdleB_FeMale
// ANIMATION : Weapon/Nunchaku/Resource/UI_IdleB_Female.i3a
//
static void AIPROC_UI_IdleB_FeMale_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_IdleB_FeMale_Fin( i3AIContext * pCtx, REAL32 tm)
{
}



static i3::statictable::AISTATE_PROC s_Knife_MaleFemale_13PVTable[] =
{
	{	"Attack_A_FeMale_1PV",	AIPROC_Attack_A_FeMale_1PV_Run,	AIPROC_Attack_A_FeMale_1PV_Fin	},
	{	"Attack_A_FeMale_3PV",	AIPROC_Attack_A_FeMale_3PV_Run,	AIPROC_Attack_A_FeMale_3PV_Fin	},
	{	"Attack_A_Male_1PV",	AIPROC_Attack_A_Male_1PV_Run,	AIPROC_Attack_A_Male_1PV_Fin	},
	{	"Attack_A_Male_3PV",	AIPROC_Attack_A_Male_3PV_Run,	AIPROC_Attack_A_Male_3PV_Fin	},
	{	"Attack_B_FeMale_1PV",	AIPROC_Attack_B_FeMale_1PV_Run,	AIPROC_Attack_B_FeMale_1PV_Fin	},
	{	"Attack_B_FeMale_3PV",	AIPROC_Attack_B_FeMale_3PV_Run,	AIPROC_Attack_B_FeMale_3PV_Fin	},
	{	"Attack_B_Male_1PV",	AIPROC_Attack_B_Male_1PV_Run,	AIPROC_Attack_B_Male_1PV_Fin	},
	{	"Attack_B_Male_3PV",	AIPROC_Attack_B_Male_3PV_Run,	AIPROC_Attack_B_Male_3PV_Fin	},
	{	"Change_FeMale_1PV",	AIPROC_Change_FeMale_1PV_Run,	AIPROC_Change_FeMale_1PV_Fin	},
	{	"Change_FeMale_3PV",	AIPROC_Change_FeMale_3PV_Run,	AIPROC_Change_FeMale_3PV_Fin	},
	{	"Change_Male_1PV",	AIPROC_Change_Male_1PV_Run,	AIPROC_Change_Male_1PV_Fin	},
	{	"Change_Male_3PV",	AIPROC_Change_Male_3PV_Run,	AIPROC_Change_Male_3PV_Fin	},
	{	"Idle_FeMale_1PV",	AIPROC_Idle_FeMale_1PV_Run,	AIPROC_Idle_FeMale_1PV_Fin	},
	{	"Idle_FeMale_3PV",	AIPROC_Idle_FeMale_3PV_Run,	AIPROC_Idle_FeMale_3PV_Fin	},
	{	"Idle_Male_1PV",	AIPROC_Idle_Male_1PV_Run,	AIPROC_Idle_Male_1PV_Fin	},
	{	"Idle_Male_3PV",	AIPROC_Idle_Male_3PV_Run,	AIPROC_Idle_Male_3PV_Fin	},
	{	"UI_IdleA_FeMale",	AIPROC_UI_IdleA_FeMale_Run,	AIPROC_UI_IdleA_FeMale_Fin	},
	{	"UI_IdleA_Male",	AIPROC_UI_IdleA_Male_Run,	AIPROC_UI_IdleA_Male_Fin	},
	{	"UI_IdleB_FeMale",	AIPROC_UI_IdleB_FeMale_Run,	AIPROC_UI_IdleB_FeMale_Fin	},
	{	"UI_IdleB_Male",	AIPROC_UI_IdleB_Male_Run,	AIPROC_UI_IdleB_Male_Fin	},
};

AIDriverKnife_MaleFemale_13PV::AIDriverKnife_MaleFemale_13PV(void)
{
	setAIProcTable( s_Knife_MaleFemale_13PVTable, (sizeof(s_Knife_MaleFemale_13PVTable)/sizeof(s_Knife_MaleFemale_13PVTable[0])) );
}

AIDriverKnife_MaleFemale_13PV::~AIDriverKnife_MaleFemale_13PV(void)
{
}
