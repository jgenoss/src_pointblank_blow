#include "pch.h"
#include "GlobalVariables.h"
#include "AIDriverCharaUpper_SniperRifle.h"
#include "../GameCharaAnimContext.h"

I3_CLASS_INSTANCE( AIDriverCharaUpper_SniperRifle);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// Move
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/SNIPERRIFLE_B/UP_MOVE_ATTACKIDLE_ZOOM.I3A
//
static void AIPROC_Move_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_Move_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/SNIPERRIFLE_B/UP_ATTACKIDLE_ZOOM.I3A
//
static void AIPROC_Idle_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_IDLE_RUN( pCtx, tm);
}

static void AIPROC_Idle_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Change
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/SNIPERRIFLE_B/SSG69/UP_CHANGE.I3A
//
static void AIPROC_Change_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_SWAP_WEAPON_RUN( pCtx, tm);
}

static void AIPROC_Change_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Attack
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/SNIPERRIFLE_B/UP_ATTACK_ZOOM.I3A
//
static void AIPROC_Attack_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_ATTACK_RUN( pCtx, tm);
}

static void AIPROC_Attack_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_ATTACK_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// JumpEnd
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/SNIPERRIFLE_B/UP_ATTACKIDLE_ZOOM.I3A
//
static void AIPROC_JumpEnd_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_JumpEnd_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_JUMP_END_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// JumpStart
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/SNIPERRIFLE_B/UP_ATTACKIDLE_ZOOM.I3A
//
static void AIPROC_JumpStart_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_JumpStart_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadBullet
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/SNIPERRIFLE_B/SSG69/UP_RELOADC.I3A
//
static void AIPROC_LoadBullet_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_LOAD_BULLET_RUN( pCtx, tm);	
}

static void AIPROC_LoadBullet_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// LoadMagazine
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/SNIPERRIFLE_B/SSG69/UP_RELOAD.I3A
//
static void AIPROC_LoadMagazine_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_LOAD_MAGAZINE_RUN( pCtx, tm);
}

static void AIPROC_LoadMagazine_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DamageFrontRightB
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/SNIPERRIFLE_B/UP_DAMAGEFRONTRIGHT_B.I3A
//
static void AIPROC_DamageFrontRightB_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DAMAGE_RUN( pCtx, tm);
}

static void AIPROC_DamageFrontRightB_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DamageFrontLeftB
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/SNIPERRIFLE_B/UP_DAMAGEFRONTLEFT_B.I3A
//
static void AIPROC_DamageFrontLeftB_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DAMAGE_RUN( pCtx, tm);
}

static void AIPROC_DamageFrontLeftB_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DamageBackB
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/SNIPERRIFLE_B/UP_DAMAGEBACK_B.I3A
//
static void AIPROC_DamageBackB_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DAMAGE_RUN( pCtx, tm);
}

static void AIPROC_DamageBackB_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DamageFrontRightA
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/SNIPERRIFLE_B/UP_DAMAGEFRONTRIGHT_A.I3A
//
static void AIPROC_DamageFrontRightA_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DAMAGE_RUN( pCtx, tm);
}

static void AIPROC_DamageFrontRightA_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DamageFrontLeftA
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/SNIPERRIFLE_B/UP_DAMAGEFRONTLEFT_A.I3A
//
static void AIPROC_DamageFrontLeftA_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DAMAGE_RUN( pCtx, tm);
}

static void AIPROC_DamageFrontLeftA_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DamageBackA
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/SNIPERRIFLE_B/UP_DAMAGEBACK_A.I3A
//
static void AIPROC_DamageBackA_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DAMAGE_RUN( pCtx, tm);
}

static void AIPROC_DamageBackA_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionDeactivate
//
static void AIPROC_ExtensionDeactivate_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_EXTENSION_DEACTIVATE_RUN( pCtx, tm);
}

static void AIPROC_ExtensionDeactivate_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// ExtensionActivate
//
static void AIPROC_ExtensionActivate_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_EXTENSION_ACTIVATE_RUN( pCtx, tm);
}

static void AIPROC_ExtensionActivate_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_IdleB
//
static void AIPROC_UI_IdleB_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_UI_IDLEB_RUN( pCtx,  tm);
}

static void AIPROC_UI_IdleB_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_UI_IDLEB_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_IdleA
//
static void AIPROC_UI_IdleA_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UI_IdleA_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_UI_IDLEA_FIN( pCtx,  tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_Idle_Change
//
static void AIPROC_UI_Idle_Change_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_UI_IDLE_CHANGE_RUN(pCtx, tm);
}

static void AIPROC_UI_Idle_Change_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_UI_IDLE_CHANGE_FIN(pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_Idle_Stand
//
static void AIPROC_UI_Idle_Stand_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_UI_IDLE_STAND_RUN(pCtx, tm);
}

static void AIPROC_UI_Idle_Stand_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_UI_IDLE_STAND_FIN(pCtx, tm);
}



static i3::statictable::AISTATE_PROC s_CharaUpper_SniperRifleTable[] =
{
	{	"Attack",	AIPROC_Attack_Run,	AIPROC_Attack_Fin	},
	{	"Change",	AIPROC_Change_Run,	AIPROC_Change_Fin	},
	{	"DamageBackA",	AIPROC_DamageBackA_Run,	AIPROC_DamageBackA_Fin	},
	{	"DamageBackB",	AIPROC_DamageBackB_Run,	AIPROC_DamageBackB_Fin	},
	{	"DamageFrontLeftA",	AIPROC_DamageFrontLeftA_Run,	AIPROC_DamageFrontLeftA_Fin	},
	{	"DamageFrontLeftB",	AIPROC_DamageFrontLeftB_Run,	AIPROC_DamageFrontLeftB_Fin	},
	{	"DamageFrontRightA",	AIPROC_DamageFrontRightA_Run,	AIPROC_DamageFrontRightA_Fin	},
	{	"DamageFrontRightB",	AIPROC_DamageFrontRightB_Run,	AIPROC_DamageFrontRightB_Fin	},
	{	"ExtensionActivate",	AIPROC_ExtensionActivate_Run,	AIPROC_ExtensionActivate_Fin	},
	{	"ExtensionDeactivate",	AIPROC_ExtensionDeactivate_Run,	AIPROC_ExtensionDeactivate_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"JumpEnd",	AIPROC_JumpEnd_Run,	AIPROC_JumpEnd_Fin	},
	{	"JumpStart",	AIPROC_JumpStart_Run,	AIPROC_JumpStart_Fin	},
	{	"LoadBullet",	AIPROC_LoadBullet_Run,	AIPROC_LoadBullet_Fin	},
	{	"LoadMagazine",	AIPROC_LoadMagazine_Run,	AIPROC_LoadMagazine_Fin	},
	{	"Move",	AIPROC_Move_Run,	AIPROC_Move_Fin	},
	{	"UI_Idle_Change",	AIPROC_UI_Idle_Change_Run,	AIPROC_UI_Idle_Change_Fin	},
	{	"UI_Idle_Stand",	AIPROC_UI_Idle_Stand_Run,	AIPROC_UI_Idle_Stand_Fin	},
	{	"UI_IdleA",	AIPROC_UI_IdleA_Run,	AIPROC_UI_IdleA_Fin	},
	{	"UI_IdleB",	AIPROC_UI_IdleB_Run,	AIPROC_UI_IdleB_Fin	},
};

AIDriverCharaUpper_SniperRifle::AIDriverCharaUpper_SniperRifle(void)
{
	setAIProcTable( s_CharaUpper_SniperRifleTable, (sizeof(s_CharaUpper_SniperRifleTable)/sizeof(s_CharaUpper_SniperRifleTable[0])));
}

AIDriverCharaUpper_SniperRifle::~AIDriverCharaUpper_SniperRifle(void)
{
}
