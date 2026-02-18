#include "pch.h"
#include "AIDriverKnuckleDefault.h"
#include "WeaponAIDriver_Macro.h"

I3_CLASS_INSTANCE( AIDriverKnuckleDefault);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// Idle_Male_Left
// ANIMATION : Weapon/Brass_Knuckle/Resource/Idle_Male_Left.i3a
//
static void AIPROC_Idle_Male_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm);
}

static void AIPROC_Idle_Male_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// SecondaryFire
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
// LoadMag
//
static void AIPROC_LoadMag_Run( i3AIContext * pCtx, REAL32 tm)
{

}

static void AIPROC_LoadMag_Fin( i3AIContext * pCtx, REAL32 tm)
{

}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet
//
static void AIPROC_LoadBullet_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_LoadBullet_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Fire
//
static void AIPROC_Fire_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_Fire_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// AttackA_Male_Left
// ANIMATION : Weapon/Brass_Knuckle/Resource/Idle_Male_Left.i3a
//
static void AIPROC_AttackA_Male_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_AttackA_Male_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// AttackA_Male_Right
// ANIMATION : Weapon/Brass_Knuckle/Resource/Idle_Male_Right.i3a
//
static void AIPROC_AttackA_Male_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_AttackA_Male_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// AttackA_Female_Left
// ANIMATION : Weapon/Brass_Knuckle/Resource/Idle_Female_Left.i3a
//
static void AIPROC_AttackA_Female_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_AttackA_Female_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// AttackA_Female_Right
// ANIMATION : Weapon/Brass_Knuckle/Resource/Idle_Female_Right.i3a
//
static void AIPROC_AttackA_Female_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_AttackA_Female_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Change_Male_Left
// ANIMATION : Weapon/Brass_Knuckle/Resource/Change_Male_Left.i3a
//
static void AIPROC_Change_Male_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN( pCtx, tm);
}

static void AIPROC_Change_Male_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change_Male_Right
// ANIMATION : Weapon/Brass_Knuckle/Resource/Change_Male_Right.i3a
//
static void AIPROC_Change_Male_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN( pCtx, tm);
}

static void AIPROC_Change_Male_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// AttackB_Male_Left
// ANIMATION : Weapon/Brass_Knuckle/Resource/Idle_Male_Left.i3a
//
static void AIPROC_AttackB_Male_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_AttackB_Male_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// AttackB_Male_Right
// ANIMATION : Weapon/Brass_Knuckle/Resource/Idle_Male_Right.i3a
//
static void AIPROC_AttackB_Male_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_AttackB_Male_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// AttackB_Female_Left
// ANIMATION : Weapon/Brass_Knuckle/Resource/Idle_Female_Left.i3a
//
static void AIPROC_AttackB_Female_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_AttackB_Female_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// AttackB_Female_Right
// ANIMATION : Weapon/Brass_Knuckle/Resource/Idle_Female_Right.i3a
//
static void AIPROC_AttackB_Female_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_RUN( pCtx, tm);
}

static void AIPROC_AttackB_Female_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_FIRE_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle_Male_Right
// ANIMATION : Weapon/Brass_Knuckle/Resource/Idle_Male_Right.i3a
//
static void AIPROC_Idle_Male_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm);
}

static void AIPROC_Idle_Male_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle_Female_Left
// ANIMATION : Weapon/Brass_Knuckle/Resource/Idle_Female_Left.i3a
//
static void AIPROC_Idle_Female_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm);
}

static void AIPROC_Idle_Female_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle_Female_Right
// ANIMATION : Weapon/Brass_Knuckle/Resource/Idle_Female_Right.i3a
//
static void AIPROC_Idle_Female_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_RUN( pCtx, tm);
}

static void AIPROC_Idle_Female_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_IDLE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Change_Female_Left
// ANIMATION : Weapon/Brass_Knuckle/Resource/Change_Female_Left.i3a
//
static void AIPROC_Change_Female_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN( pCtx, tm);
}

static void AIPROC_Change_Female_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change_Female_Right
// ANIMATION : Weapon/Brass_Knuckle/Resource/Change_Female_Right.i3a
//
static void AIPROC_Change_Female_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_WEAPON_SWAP_RUN( pCtx, tm);
}

static void AIPROC_Change_Female_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
}



static i3::statictable::AISTATE_PROC s_KnuckleDefaultTable[] =
{
	{	"AttackA_Female_Left",	AIPROC_AttackA_Female_Left_Run,	AIPROC_AttackA_Female_Left_Fin	},
	{	"AttackA_Female_Right",	AIPROC_AttackA_Female_Right_Run,	AIPROC_AttackA_Female_Right_Fin	},
	{	"AttackA_Male_Left",	AIPROC_AttackA_Male_Left_Run,	AIPROC_AttackA_Male_Left_Fin	},
	{	"AttackA_Male_Right",	AIPROC_AttackA_Male_Right_Run,	AIPROC_AttackA_Male_Right_Fin	},
	{	"AttackB_Female_Left",	AIPROC_AttackB_Female_Left_Run,	AIPROC_AttackB_Female_Left_Fin	},
	{	"AttackB_Female_Right",	AIPROC_AttackB_Female_Right_Run,	AIPROC_AttackB_Female_Right_Fin	},
	{	"AttackB_Male_Left",	AIPROC_AttackB_Male_Left_Run,	AIPROC_AttackB_Male_Left_Fin	},
	{	"AttackB_Male_Right",	AIPROC_AttackB_Male_Right_Run,	AIPROC_AttackB_Male_Right_Fin	},
	{	"Change_Female_Right",	AIPROC_Change_Female_Right_Run,	AIPROC_Change_Female_Right_Fin	},
	{	"Change_Female_Left",	AIPROC_Change_Female_Left_Run,	AIPROC_Change_Female_Left_Fin	},
	{	"Change_Male_Left",	AIPROC_Change_Male_Left_Run,	AIPROC_Change_Male_Left_Fin	},
	{	"Change_Male_Right",	AIPROC_Change_Male_Right_Run,	AIPROC_Change_Male_Right_Fin	},
	{	"Fire",	AIPROC_Fire_Run,	AIPROC_Fire_Fin	},
	{	"Idle_Female_Left",	AIPROC_Idle_Female_Left_Run,	AIPROC_Idle_Female_Left_Fin	},
	{	"Idle_Female_Right",	AIPROC_Idle_Female_Right_Run,	AIPROC_Idle_Female_Right_Fin	},
	{	"Idle_Male_Left",	AIPROC_Idle_Male_Left_Run,	AIPROC_Idle_Male_Left_Fin	},
	{	"Idle_Male_Right",	AIPROC_Idle_Male_Right_Run,	AIPROC_Idle_Male_Right_Fin	},
	{	"LoadBullet",	AIPROC_LoadBullet_Run,	AIPROC_LoadBullet_Fin	},
	{	"LoadMag",	AIPROC_LoadMag_Run,	AIPROC_LoadMag_Fin	},
	{	"SecondaryFire",	AIPROC_SecondaryFire_Run,	AIPROC_SecondaryFire_Fin	},
};

AIDriverKnuckleDefault::AIDriverKnuckleDefault(void)
{
	setAIProcTable( s_KnuckleDefaultTable, (sizeof(s_KnuckleDefaultTable)/sizeof(s_KnuckleDefaultTable[0])) );
}

AIDriverKnuckleDefault::~AIDriverKnuckleDefault(void)
{
}
