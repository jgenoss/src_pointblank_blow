#include "pch.h"
#include "AIDriverDinoCommon.h"
#include "GlobalVariables.h"
#include "../GameCharaAnimContext.h"

I3_CLASS_INSTANCE( AIDriverDinoCommon);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// Death
// ANIMATION : Chara/Dino_Velociraptor/3PV/Common/Death.i3a
//
static void AIPROC_Death_Run( i3AIContext * pCtx, REAL32 tm)
{
}

static void AIPROC_Death_Fin( i3AIContext * pCtx, REAL32 tm)
{
}



static i3::statictable::AISTATE_PROC s_DinoCommonTable[] =
{
	{	"Death",	AIPROC_Death_Run,	AIPROC_Death_Fin	},
};

AIDriverDinoCommon::AIDriverDinoCommon(void)
{
	setAIProcTable( s_DinoCommonTable, (sizeof(s_DinoCommonTable)/sizeof(s_DinoCommonTable[0])));
}

AIDriverDinoCommon::~AIDriverDinoCommon(void)
{
}
