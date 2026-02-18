#include "pch.h"
#include "AIDriverDinoUpper.h"
#include "GlobalVariables.h"
#include "../GameCharaAnimContext.h"

I3_CLASS_INSTANCE( AIDriverDinoUpper);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// Attack_A
// ANIMATION : Chara/Dino_Velociraptor/3PV/Velociraptor/Up_Attack_A.i3a
//
static void AIPROC_Attack_A_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_ATTACK_RUN( pCtx, tm);
}

static void AIPROC_Attack_A_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_ATTACK_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Attack_B
// ANIMATION : Chara/Dino_Velociraptor/3PV/Velociraptor/Up_Attack_B.i3a
//
static void AIPROC_Attack_B_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_ATTACK_RUN( pCtx, tm);
}

static void AIPROC_Attack_B_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_ATTACK_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// DamageLeft
// ANIMATION : Chara/Dino_Velociraptor/3PV/Velociraptor/Up_DamageLeftA.i3a
//
static void AIPROC_DamageLeft_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DAMAGE_RUN( pCtx, tm);
}

static void AIPROC_DamageLeft_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DamageRight
// ANIMATION : Chara/Dino_Velociraptor/3PV/Velociraptor/Up_DamageRightA.i3a
//
static void AIPROC_DamageRight_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DAMAGE_RUN( pCtx, tm);
}

static void AIPROC_DamageRight_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// JumpStart
// ANIMATION : Chara/Dino_Velociraptor/3PV/Velociraptor/Up_JumpStart.i3a
//
static void AIPROC_JumpStart_Run( i3AIContext * pCtx, REAL32 tm)
{
		
}

static void AIPROC_JumpStart_Fin( i3AIContext * pCtx, REAL32 tm)
{

}

////////////////////////////////////////////////////////////////////////////////
//
// JumpEnd
// ANIMATION : Chara/Dino_Velociraptor/3PV/Velociraptor/Up_JumpEnd.i3a
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
// MoveAttack_A
// ANIMATION : Chara/Dino_Velociraptor/3PV/Velociraptor/Up_Move_Attack_A.i3a
//
static void AIPROC_MoveAttack_A_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_ATTACK_RUN( pCtx, tm);
}

static void AIPROC_MoveAttack_A_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_ATTACK_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// MoveAttack_B
// ANIMATION : Chara/Dino_Velociraptor/3PV/Velociraptor/Up_Move_Attack_B.i3a
//
static void AIPROC_MoveAttack_B_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_ATTACK_RUN( pCtx, tm);
}

static void AIPROC_MoveAttack_B_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_ATTACK_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// MoveDamageLeftA
// ANIMATION : Chara/Dino_Velociraptor/3PV/Velociraptor/Up_Move_DamageLeftA.i3a
//
static void AIPROC_MoveDamageLeftA_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DAMAGE_RUN( pCtx, tm);
}

static void AIPROC_MoveDamageLeftA_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// MoveDamageRightA
// ANIMATION : Chara/Dino_Velociraptor/3PV/Velociraptor/Up_Move_DamageRightA.i3a
//
static void AIPROC_MoveDamageRightA_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DAMAGE_RUN( pCtx, tm);
}

static void AIPROC_MoveDamageRightA_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle
// ANIMATION : Chara/Dino_Velociraptor/3PV/Velociraptor/Up_AttackIdle.i3a
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
// Walk_Idle
// ANIMATION : Chara/Dino_Velociraptor/3PV/Velociraptor/Up_Walk_AttackIdle.i3a
//
static void AIPROC_Walk_Idle_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_Walk_Idle_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Move
// ANIMATION : Chara/Dino_Velociraptor/3PV/Velociraptor/Up_Move_AttackIdle.i3a
//
static void AIPROC_Move_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_Move_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Sit_Idle
// ANIMATION : Chara/Dino_Velociraptor/3PV/Velociraptor/Up_Sit_AttackIdle.i3a
//
static void AIPROC_Sit_Idle_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_IDLE_RUN( pCtx, tm);
}

static void AIPROC_Sit_Idle_Fin( i3AIContext * pCtx, REAL32 tm)
{
}



static i3::statictable::AISTATE_PROC s_DinoUpperTable[] =
{
	{	"Attack_A",	AIPROC_Attack_A_Run,	AIPROC_Attack_A_Fin	},
	{	"Attack_B",	AIPROC_Attack_B_Run,	AIPROC_Attack_B_Fin	},
	{	"DamageLeft",	AIPROC_DamageLeft_Run,	AIPROC_DamageLeft_Fin	},
	{	"DamageRight",	AIPROC_DamageRight_Run,	AIPROC_DamageRight_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"JumpEnd",	AIPROC_JumpEnd_Run,	AIPROC_JumpEnd_Fin	},
	{	"JumpStart",	AIPROC_JumpStart_Run,	AIPROC_JumpStart_Fin	},
	{	"Move",	AIPROC_Move_Run,	AIPROC_Move_Fin	},
	{	"MoveAttack_A",	AIPROC_MoveAttack_A_Run,	AIPROC_MoveAttack_A_Fin	},
	{	"MoveAttack_B",	AIPROC_MoveAttack_B_Run,	AIPROC_MoveAttack_B_Fin	},
	{	"Sit_Idle",	AIPROC_Sit_Idle_Run,	AIPROC_Sit_Idle_Fin	},
	{	"MoveDamageLeftA",	AIPROC_MoveDamageLeftA_Run,	AIPROC_MoveDamageLeftA_Fin	},
	{	"MoveDamageRightA",	AIPROC_MoveDamageRightA_Run,	AIPROC_MoveDamageRightA_Fin	},
	{	"Walk_Idle",	AIPROC_Walk_Idle_Run,	AIPROC_Walk_Idle_Fin	},
};

AIDriverDinoUpper::AIDriverDinoUpper(void)
{
	setAIProcTable( s_DinoUpperTable, (sizeof(s_DinoUpperTable)/sizeof(s_DinoUpperTable[0])) );
}

AIDriverDinoUpper::~AIDriverDinoUpper(void)
{
}
