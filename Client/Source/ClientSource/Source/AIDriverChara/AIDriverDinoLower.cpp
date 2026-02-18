#include "pch.h"
#include "AIDriverDinoLower.h"
#include "GlobalVariables.h"
#include "../GameCharaAnimContext.h"

I3_CLASS_INSTANCE( AIDriverDinoLower); //, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// WalkFront
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Low_Stand_WalkFront.i3a
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
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Low_Stand_WalkFrontLeft.i3a
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
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Low_Stand_WalkFrontRight.i3a
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
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Low_Stand_WalkLeft.i3a
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
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Low_Stand_WalkRight.i3a
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
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Low_Stand_WalkBackLeft.i3a
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
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Low_Stand_WalkBack.i3a
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
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Low_Stand_WalkBackRight.i3a
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
// JumpEndMove
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Low_JumpEnd_Move.i3a
//
static void AIPROC_JumpEndMove_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_JumpEndMove_Fin( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_JUMP_END_FIN( pCtx, tm);
}

////////////////////////////////////////////////////////////////////////////////
//
// DeathRightA
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Death.i3a
//
static void AIPROC_DeathRightA_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_DeathRightA_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DeathRightB
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Death.i3a
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
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Death.i3a
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
// DeathFrontA1
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Death.i3a
//
static void AIPROC_DeathFrontA1_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_DeathFrontA1_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DeathFrontB
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Death.i3a
//
static void AIPROC_DeathFrontB_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_DeathFrontB_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DeathFrontH1
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Death.i3a
//
static void AIPROC_DeathFrontH1_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_DeathFrontH1_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DeathBackA
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Death.i3a
//
static void AIPROC_DeathBackA_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_DeathBackA_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DeathBackB
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Death.i3a
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
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Death.i3a
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
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Death.i3a
//
static void AIPROC_DeathLeftA_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_DeathLeftA_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DeathLeftB
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Death.i3a
//
static void AIPROC_DeathLeftB_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_DeathLeftB_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DeathLeftH
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Death.i3a
//
static void AIPROC_DeathLeftH_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_DeathLeftH_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Low_Sit_Idle.i3a
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
// Front
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Low_Stand_MoveFront.i3a
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
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Low_Stand_MoveFrontLeft.i3a
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
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Low_Stand_MoveFrontRight.i3a
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
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Low_Stand_MoveLeft.i3a
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
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Low_Stand_MoveRight.i3a
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
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Low_Stand_MoveBackLeft.i3a
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
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Low_Stand_MoveBack.i3a
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
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Low_Stand_MoveBackRight.i3a
//
static void AIPROC_BackRight_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_BackRight_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DeathFrontA3
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Death.i3a
//
static void AIPROC_DeathFrontA3_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_DeathFrontA3_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DeathFrontA2
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Death.i3a
//
static void AIPROC_DeathFrontA2_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_DeathFrontA2_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DeathFrontH2
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Death.i3a
//
static void AIPROC_DeathFrontH2_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_DeathFrontH2_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// JumpStart
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Low_JumpStart.i3a
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
// JumpStartMove
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Low_JumpStart_Move.i3a
//
static void AIPROC_JumpStartMove_Run( i3AIContext * pCtx, REAL32 tm)
{
		AIPROC_CHARA_JUMP_START_RUN( pCtx, tm);
}

static void AIPROC_JumpStartMove_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// JumpEnd
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Low_JumpEnd.i3a
//
static void AIPROC_JumpEnd_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_JumpEnd_Fin( i3AIContext * pCtx, REAL32 tm)
{
}



////////////////////////////////////////////////////////////////////////////////
//
// Status_Idle
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Raptor_Status_Idle.i3a
//
static void AIPROC_Status_Idle_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_IDLE_RUN( pCtx, tm);
}

static void AIPROC_Status_Idle_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Status_Roar
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Raptor_Status_Roar.i3a
//
static void AIPROC_Status_Roar_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_IDLE_RUN( pCtx, tm);
}

static void AIPROC_Status_Roar_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Status_Stun
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Raptor_Status_Idle.i3a
//
static void AIPROC_Status_Stun_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_IDLE_RUN( pCtx, tm);
}

static void AIPROC_Status_Stun_Fin( i3AIContext * pCtx, REAL32 tm)
{
}


static i3::statictable::AISTATE_PROC s_DinoLowerTable[] =
{
	{	"Back",	AIPROC_Back_Run,	AIPROC_Back_Fin	},
	{	"BackLeft",	AIPROC_BackLeft_Run,	AIPROC_BackLeft_Fin	},
	{	"BackRight",	AIPROC_BackRight_Run,	AIPROC_BackRight_Fin	},
	{	"DeathBackA",	AIPROC_DeathBackA_Run,	AIPROC_DeathBackA_Fin	},
	{	"DeathBackB",	AIPROC_DeathBackB_Run,	AIPROC_DeathBackB_Fin	},
	{	"DeathBackH",	AIPROC_DeathBackH_Run,	AIPROC_DeathBackH_Fin	},
	{	"DeathFrontA1",	AIPROC_DeathFrontA1_Run,	AIPROC_DeathFrontA1_Fin	},
	{	"DeathFrontA2",	AIPROC_DeathFrontA2_Run,	AIPROC_DeathFrontA2_Fin	},
	{	"DeathFrontA3",	AIPROC_DeathFrontA3_Run,	AIPROC_DeathFrontA3_Fin	},
	{	"DeathFrontB",	AIPROC_DeathFrontB_Run,	AIPROC_DeathFrontB_Fin	},
	{	"DeathFrontH1",	AIPROC_DeathFrontH1_Run,	AIPROC_DeathFrontH1_Fin	},
	{	"DeathFrontH2",	AIPROC_DeathFrontH2_Run,	AIPROC_DeathFrontH2_Fin	},
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
	{	"JumpEndMove",	AIPROC_JumpEndMove_Run,	AIPROC_JumpEndMove_Fin	},
	{	"JumpStart",	AIPROC_JumpStart_Run,	AIPROC_JumpStart_Fin	},
	{	"JumpStartMove",	AIPROC_JumpStartMove_Run,	AIPROC_JumpStartMove_Fin	},
	{	"Left",	AIPROC_Left_Run,	AIPROC_Left_Fin	},
	{	"Right",	AIPROC_Right_Run,	AIPROC_Right_Fin	},
	{	"Status_Idle",	AIPROC_Status_Idle_Run,	AIPROC_Status_Idle_Fin	},
	{	"Status_Roar",	AIPROC_Status_Roar_Run,	AIPROC_Status_Roar_Fin	},
	{	"Status_Stun",	AIPROC_Status_Stun_Run,	AIPROC_Status_Stun_Fin	},
	{	"WalkBack",	AIPROC_WalkBack_Run,	AIPROC_WalkBack_Fin	},
	{	"WalkBackLeft",	AIPROC_WalkBackLeft_Run,	AIPROC_WalkBackLeft_Fin	},
	{	"WalkBackRight",	AIPROC_WalkBackRight_Run,	AIPROC_WalkBackRight_Fin	},
	{	"WalkFront",	AIPROC_WalkFront_Run,	AIPROC_WalkFront_Fin	},
	{	"WalkFrontLeft",	AIPROC_WalkFrontLeft_Run,	AIPROC_WalkFrontLeft_Fin	},
	{	"WalkFrontRight",	AIPROC_WalkFrontRight_Run,	AIPROC_WalkFrontRight_Fin	},
	{	"WalkLeft",	AIPROC_WalkLeft_Run,	AIPROC_WalkLeft_Fin	},
	{	"WalkRight",	AIPROC_WalkRight_Run,	AIPROC_WalkRight_Fin	},
};

AIDriverDinoLower::AIDriverDinoLower(void)
{
	setAIProcTable( s_DinoLowerTable, (sizeof(s_DinoLowerTable)/sizeof(s_DinoLowerTable[0])) );
}

AIDriverDinoLower::~AIDriverDinoLower(void)
{
}
