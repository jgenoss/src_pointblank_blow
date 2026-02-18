#include "pch.h"
#include "GlobalVariables.h"
#include "AIDriverCharaCommon.h"
#include "../GameCharaAnimContext.h"

I3_CLASS_INSTANCE( AIDriverCharaCommon);//, i3AIDriver);

////////////////////////////////////////////////////////////////////////////////
//
// Ceremony
// ANIMATION : CHARA/REBEL_FEMALE/3ÀÎÄª/COMMON/UI_CEREMONY.I3A
//
static void AIPROC_Ceremony_Run( i3AIContext * pCtx, REAL32 tm)
{
	AIPROC_CHARA_UI_CEREMONY_RUN( pCtx, tm);
}

static void AIPROC_Ceremony_Fin( i3AIContext * pCtx, REAL32 tm)
{
}



static i3::statictable::AISTATE_PROC s_CharaCommonTable[] =
{
	{	"Ceremony",	AIPROC_Ceremony_Run,	AIPROC_Ceremony_Fin	},
};

AIDriverCharaCommon::AIDriverCharaCommon(void)
{
	setAIProcTable( s_CharaCommonTable, (sizeof(s_CharaCommonTable)/sizeof(s_CharaCommonTable[0])));
}

AIDriverCharaCommon::~AIDriverCharaCommon(void)
{
}
