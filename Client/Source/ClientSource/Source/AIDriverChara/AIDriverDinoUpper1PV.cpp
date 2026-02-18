#include "pch.h"
#include "AIDriverDinoUpper1PV.h"
#include "GlobalVariables.h"
#include "../GameCharaAnimContext.h"

I3_CLASS_INSTANCE( AIDriverDinoUpper1PV, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// Attack1_A
// ANIMATION : Chara/Dino_Velociraptor/1PV/Common/Attack1_A.i3a
//
static void AIPROC_Attack1_A_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_ATTACK_RUN( pCtx, tm);
}

static void AIPROC_Attack1_A_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_ATTACK_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Attack1_B
// ANIMATION : Chara/Dino_Velociraptor/1PV/Common/Attack1_B.i3a
//
static void AIPROC_Attack1_B_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_ATTACK_RUN( pCtx, tm);
}

static void AIPROC_Attack1_B_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_ATTACK_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// Attack2
// ANIMATION : Chara/Dino_Velociraptor/1PV/Common/Attack2.i3a
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
// AttackIdle
// ANIMATION : Chara/Dino_Velociraptor/1PV/Common/AttackIdle.i3a
//
static void AIPROC_AttackIdle_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_IDLE_RUN( pCtx, tm);
}

static void AIPROC_AttackIdle_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DamageAttackIdle
// ANIMATION : Chara/Dino_Velociraptor/1PV/Common/Damage_AttackIdle.i3a
//
static void AIPROC_DamageAttackIdle_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DAMAGE_RUN( pCtx, tm);
}

static void AIPROC_DamageAttackIdle_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// MoveAttackIdle
// ANIMATION : Chara/Dino_Velociraptor/1PV/Common/Move_AttackIdle.i3a
//
static void AIPROC_MoveAttackIdle_Run( i3AIContext * pCtx, REAL32 tm)
{
	int iii = 0;
}

static void AIPROC_MoveAttackIdle_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// JumpStart
// ANIMATION : Chara/Dino_Velociraptor/1PV/Common/AttackIdle.i3a
//
static void AIPROC_JumpStart_Run( i3AIContext * pCtx, REAL32 tm)
{
	int iii = 0;
}

static void AIPROC_JumpStart_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// JumpEnd
// ANIMATION : Chara/Dino_Velociraptor/1PV/Common/AttackIdle.i3a
//
static void AIPROC_JumpEnd_Run( i3AIContext * pCtx, REAL32 tm)
{
	int iii = 0;
}

static void AIPROC_JumpEnd_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_JUMP_END_FIN( pCtx, tm);
}


static I3_AISTATE_PROC_TABLE s_DinoUpper1PVTable[] =
{
	{	"Attack1_A",	AIPROC_Attack1_A_Run,	AIPROC_Attack1_A_Fin	},
	{	"Attack1_B",	AIPROC_Attack1_B_Run,	AIPROC_Attack1_B_Fin	},
	{	"Attack2",	AIPROC_Attack2_Run,	AIPROC_Attack2_Fin	},
	{	"AttackIdle",	AIPROC_AttackIdle_Run,	AIPROC_AttackIdle_Fin	},
	{	"DamageAttackIdle",	AIPROC_DamageAttackIdle_Run,	AIPROC_DamageAttackIdle_Fin	},
	{	"JumpEnd",	AIPROC_JumpEnd_Run,	AIPROC_JumpEnd_Fin	},
	{	"JumpStart",	AIPROC_JumpStart_Run,	AIPROC_JumpStart_Fin	},
	{	"MoveAttackIdle",	AIPROC_MoveAttackIdle_Run,	AIPROC_MoveAttackIdle_Fin	},
};

AIDriverDinoUpper1PV::AIDriverDinoUpper1PV(void)
{
	setAIProcTable( s_DinoUpper1PVTable);
}

AIDriverDinoUpper1PV::~AIDriverDinoUpper1PV(void)
{
}
