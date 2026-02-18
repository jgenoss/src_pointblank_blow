#include "pch.h"
#include "AIDriverDummy.h"
#include "../WeaponDinoDummy.h"

I3_CLASS_INSTANCE( AIDriverDummy);

////////////////////////////////////////////////////////////////////////////////
//
// nullptr
//
static void AIPROC_NULL_Run( i3AIContext * pCtx, REAL32 tm)
{
	static_cast<WeaponDinoDummy*>(pCtx->getOwner())->OnNone(pCtx, tm);
}

static void AIPROC_NULL_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// IDLE
// ANIMATION : Weapon/Domination/Dummy/DummyLV1/Resource/Idle.i3a
//
static void AIPROC_IDLE_Run( i3AIContext * pCtx, REAL32 tm)
{
	static_cast<WeaponDinoDummy*>(pCtx->getOwner())->OnIdle(pCtx, tm);
}

static void AIPROC_IDLE_Fin( i3AIContext * pCtx, REAL32 tm)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// DESTROY
// ANIMATION : Weapon/Domination/Dummy/DummyLV1/Resource/Break.i3a
//
static void AIPROC_DESTROY_Run( i3AIContext * pCtx, REAL32 tm)
{
	static_cast<WeaponDinoDummy*>(pCtx->getOwner())->OnDestroy(pCtx, tm);
}

static void AIPROC_DESTROY_Fin( i3AIContext * pCtx, REAL32 tm)
{
}



static i3::statictable::AISTATE_PROC s_DummyTable[] =
{
	{	"DESTROY",	AIPROC_DESTROY_Run,	AIPROC_DESTROY_Fin	},
	{	"IDLE",	AIPROC_IDLE_Run,	AIPROC_IDLE_Fin	},
	{	"nullptr",	AIPROC_NULL_Run,	AIPROC_NULL_Fin	},
};

AIDriverDummy::AIDriverDummy(void)
{
	setAIProcTable( s_DummyTable, (sizeof(s_DummyTable)/sizeof(s_DummyTable[0])) );
}

AIDriverDummy::~AIDriverDummy(void)
{
}
