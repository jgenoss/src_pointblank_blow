#include "pch.h"
#include "AIDriver_Default.h"
#include "../GameCharaAnimContext.h"

I3_CLASS_INSTANCE( AIDriver_Default);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// AttackIdle
// ANIMATION : Chara/NPC_Kel_Mahum_Archer/Kel_Mahum_Archer_Atkwait.i3a
//
static void AIPROC_AttackIdle_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_AttackIdle_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Idle
// ANIMATION : Chara/NPC_Kel_Mahum_Archer/Kel_Mahum_Archer_Atkwait.i3a
//
static void AIPROC_Idle_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_Idle_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Attack
// ANIMATION : Chara/NPC_Kel_Mahum_Archer/Kel_Mahum_Archer_Atk01.i3a
//
static void AIPROC_Attack_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_Attack_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Damage
// ANIMATION : Chara/NPC_Kel_Mahum_Archer/Kel_Mahum_Archer_Damaged.i3a
//
static void AIPROC_Damage_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_Damage_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Death
// ANIMATION : Chara/NPC_Kel_Mahum_Archer/Kel_Mahum_Archer_Death.i3a
//
static void AIPROC_Death_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_Death_Fin( i3AIContext * pCtx, REAL32 tm)
{
}



static i3::statictable::AISTATE_PROC s__DefaultTable[] =
{
	{	"Attack",	AIPROC_Attack_Run,	AIPROC_Attack_Fin	},
	{	"AttackIdle",	AIPROC_AttackIdle_Run,	AIPROC_AttackIdle_Fin	},
	{	"Damage",	AIPROC_Damage_Run,	AIPROC_Damage_Fin	},
	{	"Death",	AIPROC_Death_Run,	AIPROC_Death_Fin	},
	{	"Idle",	AIPROC_Idle_Run,	AIPROC_Idle_Fin	},
};

AIDriver_Default::AIDriver_Default(void)
{
	setAIProcTable( s__DefaultTable, (sizeof(s__DefaultTable)/sizeof(s__DefaultTable[0])) );
}

AIDriver_Default::~AIDriver_Default(void)
{
}
