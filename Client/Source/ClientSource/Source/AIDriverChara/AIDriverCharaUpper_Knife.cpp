#include "pch.h"
#include "GlobalVariables.h"
#include "AIDriverCharaUpper_Knife.h"
#include "../GameCharaAnimContext.h"

I3_CLASS_INSTANCE( AIDriverCharaUpper_Knife);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// Move
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/KNIFE/UP_MOVE_ATTACKIDLE.I3A
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
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/KNIFE/UP_ATTACKIDLE.I3A
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
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/KNIFE/UP_CHANGE.I3A
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
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/KNIFE/UP_ATTACK.I3A
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
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/KNIFE/UP_ATTACKIDLE.I3A
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
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/KNIFE/UP_ATTACKIDLE.I3A
//
static void AIPROC_JumpStart_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_JumpStart_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Attack2
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/KNIFE/UP_ATTACK_STAB.I3A
//
static void AIPROC_Attack2_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_ATTACK_RUN( pCtx, tm);
}

static void AIPROC_Attack2_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_ATTACK_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// SecondaryAttack
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/KNIFE/UP_ATTACK_STAB.I3A
//
static void AIPROC_SecondaryAttack_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_ATTACK_RUN( pCtx, tm);
}

static void AIPROC_SecondaryAttack_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_ATTACK_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// DamageFrontRightB
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/KNIFE/UP_DAMAGEFRONTRIGHT_B.I3A
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
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/KNIFE/UP_DAMAGEFRONTLEFT_B.I3A
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
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/KNIFE/UP_DAMAGEBACK_B.I3A
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
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/KNIFE/UP_DAMAGEFRONTRIGHT_A.I3A
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
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/KNIFE/UP_DAMAGEFRONTLEFT_A.I3A
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
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/KNIFE/UP_DAMAGEBACK_A.I3A
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
// UI_IdleA
// ANIMATION : CHARA/SWAT_MALE/3ÀÎÄª/KNIFE/UI/UI_IDLEA.I3A
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
// UI_IdleB
// ANIMATION : CHARA/SWAT_MALE/3ÀÎÄª/KNIFE/UI/UI_IDLEB.I3A
//
static void AIPROC_UI_IdleB_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_UI_IDLEB_RUN( pCtx,  tm);
}

static void AIPROC_UI_IdleB_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_UI_IDLEB_FIN( pCtx,  tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// UI_Idle_Change
// ANIMATION : Chara/SWAT_Male/3PV/Spetnatz_Field_Shovel/UI/Idle_Change.i3a
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
// ANIMATION : Chara/SWAT_Male/3PV/Spetnatz_Field_Shovel/UI/Idle_Stand.i3a
//
static void AIPROC_UI_Idle_Stand_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_UI_IDLE_STAND_RUN(pCtx, tm);
}

static void AIPROC_UI_Idle_Stand_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_UI_IDLE_STAND_FIN(pCtx, tm);
}



static i3::statictable::AISTATE_PROC s_CharaUpper_KnifeTable[] =
{
	{	"Attack",	AIPROC_Attack_Run,	AIPROC_Attack_Fin	},
	{	"Attack2",	AIPROC_Attack2_Run,	AIPROC_Attack2_Fin	},
	{	"Change",	AIPROC_Change_Run,	AIPROC_Change_Fin	},
	{	"DamageBackA",	AIPROC_DamageBackA_Run,	AIPROC_DamageBackA_Fin	},
	{	"DamageBackB",	AIPROC_DamageBackB_Run,	AIPROC_DamageBackB_Fin	},
	{	"DamageFrontLeftA",	AIPROC_DamageFrontLeftA_Run,	AIPROC_DamageFrontLeftA_Fin	},
	{	"DamageFrontLeftB",	AIPROC_DamageFrontLeftB_Run,	AIPROC_DamageFrontLeftB_Fin	},
	{	"DamageFrontRightA",	AIPROC_DamageFrontRightA_Run,	AIPROC_DamageFrontRightA_Fin	},
	{	"DamageFrontRightB",	AIPROC_DamageFrontRightB_Run,	AIPROC_DamageFrontRightB_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"JumpEnd",	AIPROC_JumpEnd_Run,	AIPROC_JumpEnd_Fin	},
	{	"JumpStart",	AIPROC_JumpStart_Run,	AIPROC_JumpStart_Fin	},
	{	"Move",	AIPROC_Move_Run,	AIPROC_Move_Fin	},
	{	"SecondaryAttack",	AIPROC_SecondaryAttack_Run,	AIPROC_SecondaryAttack_Fin	},
	{	"UI_Idle_Change",	AIPROC_UI_Idle_Change_Run,	AIPROC_UI_Idle_Change_Fin	},
	{	"UI_Idle_Stand",	AIPROC_UI_Idle_Stand_Run,	AIPROC_UI_Idle_Stand_Fin	},
	{	"UI_IdleA",	AIPROC_UI_IdleA_Run,	AIPROC_UI_IdleA_Fin	},
	{	"UI_IdleB",	AIPROC_UI_IdleB_Run,	AIPROC_UI_IdleB_Fin	},
};

AIDriverCharaUpper_Knife::AIDriverCharaUpper_Knife(void)
{
	setAIProcTable( s_CharaUpper_KnifeTable, (sizeof(s_CharaUpper_KnifeTable)/sizeof(s_CharaUpper_KnifeTable[0])));
}

AIDriverCharaUpper_Knife::~AIDriverCharaUpper_Knife(void)
{
}
