#include "pch.h"
#include "GlobalVariables.h"
#include "AIDriverCharaUpper_Mission.h"
#include "../GameCharaAnimContext.h"

I3_CLASS_INSTANCE( AIDriverCharaUpper_Mission);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// Move
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/MISSION/C4/UP_MOVE.I3A
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
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/MISSION/C4/UP_IDLE.I3A
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
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/MISSION/C4/UP_CHANGE.I3A
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
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/MISSION/C4/UP_INSTALL.I3A
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
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/MISSION/C4/UP_IDLE.I3A
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
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/MISSION/C4/UP_IDLE.I3A
//
static void AIPROC_JumpStart_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DAMAGE_RUN( pCtx, tm);
}

static void AIPROC_JumpStart_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DamageFrontRightB
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/MISSION/C4/UP_DAMAGEFRONTRIGHT_B.I3A
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
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/MISSION/C4/UP_DAMAGEFRONTLEFT_B.I3A
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
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/MISSION/C4/UP_DAMAGEBACK_B.I3A
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
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/MISSION/C4/UP_DAMAGEFRONTRIGHT_A.I3A
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
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/MISSION/C4/UP_DAMAGEFRONTLEFT_A.I3A
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
// ANIMATION : CHARA/TYPEB/3ÀÎÄª/MISSION/C4/UP_DAMAGEBACK_A.I3A
//
static void AIPROC_DamageBackA_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DAMAGE_RUN( pCtx, tm);
}

static void AIPROC_DamageBackA_Fin( i3AIContext * pCtx, REAL32 tm)
{
}



static i3::statictable::AISTATE_PROC s_CharaUpper_MissionTable[] =
{
	{	"Attack",	AIPROC_Attack_Run,	AIPROC_Attack_Fin	},
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
};

AIDriverCharaUpper_Mission::AIDriverCharaUpper_Mission(void)
{
	setAIProcTable( s_CharaUpper_MissionTable, (sizeof(s_CharaUpper_MissionTable)/sizeof(s_CharaUpper_MissionTable[0])));
}

AIDriverCharaUpper_Mission::~AIDriverCharaUpper_Mission(void)
{
}
