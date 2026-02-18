#include "pch.h"
#include "GlobalVariables.h"
#include "AIDriverCharaLower.h"
#include "../GameCharaAnimContext.h"

I3_CLASS_INSTANCE( AIDriverCharaLower);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// Idle
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Low_Sit_AttackIdle.i3a
//
static void AIPROC_Idle_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_Idle_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Front
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Low_Sit_MoveFront.i3a
//
static void AIPROC_Front_Run( i3AIContext * pCtx, REAL32 tm)
{	
	AIPROC_CHARA_MOVE_RUN( pCtx, tm);
}

static void AIPROC_Front_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// FrontLeft
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Low_Sit_MoveFrontLeft.i3a
//
static void AIPROC_FrontLeft_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_MOVE_RUN( pCtx, tm);
}

static void AIPROC_FrontLeft_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// FrontRight
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Low_Sit_MoveFrontRight.i3a
//
static void AIPROC_FrontRight_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_MOVE_RUN( pCtx, tm);
}

static void AIPROC_FrontRight_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Left
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Low_Sit_MoveLeft.i3a
//
static void AIPROC_Left_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_MOVE_RUN( pCtx, tm);
}

static void AIPROC_Left_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Right
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Low_Sit_MoveRight.i3a
//
static void AIPROC_Right_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_MOVE_RUN( pCtx, tm);
}

static void AIPROC_Right_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// BackLeft
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Low_Sit_MoveBackLeft.i3a
//
static void AIPROC_BackLeft_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_MOVE_RUN( pCtx, tm);
}

static void AIPROC_BackLeft_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Back
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Low_Sit_MoveBack.i3a
//
static void AIPROC_Back_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_MOVE_RUN( pCtx, tm);
}

static void AIPROC_Back_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// BackRight
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Low_Sit_MoveBackRight.i3a
//
static void AIPROC_BackRight_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_MOVE_RUN( pCtx, tm);
}

static void AIPROC_BackRight_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// WalkFront
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Low_Sit_WalkFront.i3a
//
static void AIPROC_WalkFront_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_MOVE_RUN( pCtx, tm);
}

static void AIPROC_WalkFront_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// WalkFrontLeft
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Low_Sit_WalkFrontLeft.i3a
//
static void AIPROC_WalkFrontLeft_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_MOVE_RUN( pCtx, tm);
}

static void AIPROC_WalkFrontLeft_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// WalkFrontRight
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Low_Sit_WalkFrontRight.i3a
//
static void AIPROC_WalkFrontRight_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_MOVE_RUN( pCtx, tm);
}

static void AIPROC_WalkFrontRight_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// WalkLeft
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Low_Sit_WalkLeft.i3a
//
static void AIPROC_WalkLeft_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_MOVE_RUN( pCtx, tm);
}

static void AIPROC_WalkLeft_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// WalkRight
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Low_Sit_WalkRight.i3a
//
static void AIPROC_WalkRight_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_MOVE_RUN( pCtx, tm);
}

static void AIPROC_WalkRight_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// WalkBackLeft
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Low_Sit_WalkBackLeft.i3a
//
static void AIPROC_WalkBackLeft_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_MOVE_RUN( pCtx, tm);
}

static void AIPROC_WalkBackLeft_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// WalkBack
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Low_Sit_WalkBack .i3a
//
static void AIPROC_WalkBack_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_MOVE_RUN( pCtx, tm);
}

static void AIPROC_WalkBack_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// WalkBackRight
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Low_Sit_WalkBackRight.i3a
//
static void AIPROC_WalkBackRight_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_MOVE_RUN( pCtx, tm);
}

static void AIPROC_WalkBackRight_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// JumpEnd
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Low_JumpEnd.i3a
//
static void AIPROC_JumpEnd_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_JUMP_END_RUN( pCtx, tm);
}

static void AIPROC_JumpEnd_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_JUMP_END_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// JumpStart
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Low_JumpStart.i3a
//
static void AIPROC_JumpStart_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_JUMP_START_RUN( pCtx, tm);
}

static void AIPROC_JumpStart_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DeathRightA
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Sit_DeathRightA.i3a
//
static void AIPROC_DeathRightA_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_DeathRightA_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DEATH_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// DeathRightB
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Sit_DeathRightB.i3a
//
static void AIPROC_DeathRightB_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_DeathRightB_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DEATH_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// DeathRightH
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Sit_DeathRightH.i3a
//
static void AIPROC_DeathRightH_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_DeathRightH_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DEATH_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// DeathFrontA
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Sit_DeathFrontA.i3a
//
static void AIPROC_DeathFrontA_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_DeathFrontA_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DEATH_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// DeathFrontB
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Sit_DeathFrontB.i3a
//
static void AIPROC_DeathFrontB_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_DeathFrontB_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DEATH_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// DeathFrontH
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Sit_DeathFrontH.i3a
//
static void AIPROC_DeathFrontH_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_DeathFrontH_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DEATH_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// DeathBackA
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Sit_DeathBackA.i3a
//
static void AIPROC_DeathBackA_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_DeathBackA_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DEATH_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// DeathBackB
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Sit_DeathBackB.i3a
//
static void AIPROC_DeathBackB_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_DeathBackB_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DEATH_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// DeathBackH
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Sit_DeathBackH.i3a
//
static void AIPROC_DeathBackH_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_DeathBackH_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DEATH_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// DeathLeftA
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Sit_DeathLeftA.i3a
//
static void AIPROC_DeathLeftA_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_DeathLeftA_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DEATH_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// DeathLeftB
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Sit_DeathLeftB.i3a
//
static void AIPROC_DeathLeftB_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_DeathLeftB_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DEATH_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// DeathLeftH
// ANIMATION : Chara/TypeB/3ÀÎÄª/Common/Sit_DeathLeftH.i3a
//
static void AIPROC_DeathLeftH_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_DeathLeftH_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_DEATH_FIN( pCtx, tm);
}



static i3::statictable::AISTATE_PROC s_CharaLowerTable[] =
{
	{	"Back",	AIPROC_Back_Run,	AIPROC_Back_Fin	},
	{	"BackLeft",	AIPROC_BackLeft_Run,	AIPROC_BackLeft_Fin	},
	{	"BackRight",	AIPROC_BackRight_Run,	AIPROC_BackRight_Fin	},
	{	"DeathBackA",	AIPROC_DeathBackA_Run,	AIPROC_DeathBackA_Fin	},
	{	"DeathBackB",	AIPROC_DeathBackB_Run,	AIPROC_DeathBackB_Fin	},
	{	"DeathBackH",	AIPROC_DeathBackH_Run,	AIPROC_DeathBackH_Fin	},
	{	"DeathFrontA1",	AIPROC_DeathFrontA_Run,	AIPROC_DeathFrontA_Fin	},
	{	"DeathFrontA2",	AIPROC_DeathFrontA_Run,	AIPROC_DeathFrontA_Fin	},
	{	"DeathFrontA3",	AIPROC_DeathFrontA_Run,	AIPROC_DeathFrontA_Fin	},
	{	"DeathFrontB",	AIPROC_DeathFrontB_Run,	AIPROC_DeathFrontB_Fin	},
	{	"DeathFrontH1",	AIPROC_DeathFrontH_Run,	AIPROC_DeathFrontH_Fin	},
	{	"DeathFrontH2",	AIPROC_DeathFrontH_Run,	AIPROC_DeathFrontH_Fin	},
	{	"DeathLeftA",	AIPROC_DeathLeftA_Run,	AIPROC_DeathLeftA_Fin	},
	{	"DeathLeftB",	AIPROC_DeathLeftB_Run,	AIPROC_DeathLeftB_Fin	},
	{	"DeathLeftH",	AIPROC_DeathLeftH_Run,	AIPROC_DeathLeftH_Fin	},
	{	"DeathRightA",	AIPROC_DeathRightA_Run,	AIPROC_DeathRightA_Fin	},
	{	"DeathRightB",	AIPROC_DeathRightB_Run,	AIPROC_DeathRightB_Fin	},
	{	"DeathRightH",	AIPROC_DeathRightH_Run,	AIPROC_DeathRightH_Fin	},
	{	"Front",	AIPROC_Front_Run,	AIPROC_Front_Fin	},
	{	"FrontLeft",	AIPROC_FrontLeft_Run,	AIPROC_FrontLeft_Fin	},
	{	"FrontRight",	AIPROC_FrontRight_Run,	AIPROC_FrontRight_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
	{	"JumpEnd",	AIPROC_JumpEnd_Run,	AIPROC_JumpEnd_Fin	},
	{	"JumpStart",	AIPROC_JumpStart_Run,	AIPROC_JumpStart_Fin	},
	{	"Left",	AIPROC_Left_Run,	AIPROC_Left_Fin	},
	{	"Right",	AIPROC_Right_Run,	AIPROC_Right_Fin	},
	{	"WalkBack",	AIPROC_WalkBack_Run,	AIPROC_WalkBack_Fin	},
	{	"WalkBackLeft",	AIPROC_WalkBackLeft_Run,	AIPROC_WalkBackLeft_Fin	},
	{	"WalkBackRight",	AIPROC_WalkBackRight_Run,	AIPROC_WalkBackRight_Fin	},
	{	"WalkFront",	AIPROC_WalkFront_Run,	AIPROC_WalkFront_Fin	},
	{	"WalkFrontLeft",	AIPROC_WalkFrontLeft_Run,	AIPROC_WalkFrontLeft_Fin	},
	{	"WalkFrontRight",	AIPROC_WalkFrontRight_Run,	AIPROC_WalkFrontRight_Fin	},
	{	"WalkLeft",	AIPROC_WalkLeft_Run,	AIPROC_WalkLeft_Fin	},
	{	"WalkRight",	AIPROC_WalkRight_Run,	AIPROC_WalkRight_Fin	},
};

AIDriverCharaLower::AIDriverCharaLower(void)
{
	setAIProcTable( s_CharaLowerTable, (sizeof(s_CharaLowerTable)/sizeof(s_CharaLowerTable[0])));
}

AIDriverCharaLower::~AIDriverCharaLower(void)
{
}
