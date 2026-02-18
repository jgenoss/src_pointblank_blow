#include "pch.h"
#include "AIDriverDinoUpper3PV.h"
#include "GlobalVariables.h"
#include "../GameCharaAnimContext.h"

I3_CLASS_INSTANCE( AIDriverDinoUpper3PV, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// UpAttack_1_A
// ANIMATION : Chara/Dino_Velociraptor/3PV/Velociraptor/Up_Attack_1_A.i3a
//
static void AIPROC_UpAttack_1_A_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UpAttack_1_A_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Up_Attack_1_B
// ANIMATION : Chara/Dino_Velociraptor/3PV/Velociraptor/Up_Attack_1_B.i3a
//
static void AIPROC_Up_Attack_1_B_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_Up_Attack_1_B_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UpAttack_2
// ANIMATION : Chara/Dino_Velociraptor/3PV/Velociraptor/Up_Attack_2.i3a
//
static void AIPROC_UpAttack_2_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UpAttack_2_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Up_AttackIdle
// ANIMATION : Chara/Dino_Velociraptor/3PV/Velociraptor/Up_AttackIdle.i3a
//
static void AIPROC_Up_AttackIdle_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_Up_AttackIdle_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UpDamageLeftA
// ANIMATION : Chara/Dino_Velociraptor/3PV/Velociraptor/Up_DamageLeftA.i3a
//
static void AIPROC_UpDamageLeftA_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UpDamageLeftA_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UpDamageRightA
// ANIMATION : Chara/Dino_Velociraptor/3PV/Velociraptor/Up_DamageRightA.i3a
//
static void AIPROC_UpDamageRightA_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UpDamageRightA_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// UpMoveAttackIdle
// ANIMATION : Chara/Dino_Velociraptor/3PV/Velociraptor/Up_Move_AttackIdle.i3a
//
static void AIPROC_UpMoveAttackIdle_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_UpMoveAttackIdle_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// JumpStart
// ANIMATION : Chara/Dino_Velociraptor/3PV/Velociraptor/Up_AttackIdle.i3a
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
// ANIMATION : Chara/Dino_Velociraptor/3PV/Velociraptor/Up_AttackIdle.i3a
//
static void AIPROC_JumpEnd_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_JumpEnd_Fin( i3AIContext * pCtx, REAL32 tm)
{
}



static I3_AISTATE_PROC_TABLE s_DinoUpper3PVTable[] =
{
	{	"JumpEnd",	AIPROC_JumpEnd_Run,	AIPROC_JumpEnd_Fin	},
	{	"JumpStart",	AIPROC_JumpStart_Run,	AIPROC_JumpStart_Fin	},
	{	"Up_Attack_1_B",	AIPROC_Up_Attack_1_B_Run,	AIPROC_Up_Attack_1_B_Fin	},
	{	"Up_AttackIdle",	AIPROC_Up_AttackIdle_Run,	AIPROC_Up_AttackIdle_Fin	},
	{	"UpAttack_1_A",	AIPROC_UpAttack_1_A_Run,	AIPROC_UpAttack_1_A_Fin	},
	{	"UpAttack_2",	AIPROC_UpAttack_2_Run,	AIPROC_UpAttack_2_Fin	},
	{	"UpDamageLeftA",	AIPROC_UpDamageLeftA_Run,	AIPROC_UpDamageLeftA_Fin	},
	{	"UpDamageRightA",	AIPROC_UpDamageRightA_Run,	AIPROC_UpDamageRightA_Fin	},
	{	"UpMoveAttackIdle",	AIPROC_UpMoveAttackIdle_Run,	AIPROC_UpMoveAttackIdle_Fin	},
};

AIDriverDinoUpper3PV::AIDriverDinoUpper3PV(void)
{
	setAIProcTable( s_DinoUpper3PVTable);
}

AIDriverDinoUpper3PV::~AIDriverDinoUpper3PV(void)
{
}
