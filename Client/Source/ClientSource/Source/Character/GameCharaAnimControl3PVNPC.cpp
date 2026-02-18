#include "pch.h"
#include "GameCharaAnimControl3PVNPC.h"
#include "GlobalVariables.h"
#include "GameCharaAnimContext.h"

I3_CLASS_INSTANCE( CGameCharaAnimControl3PVNPC);//, CGameCharaAnimControl);

CGameCharaAnimControl3PVNPC::CGameCharaAnimControl3PVNPC()
{
}

CGameCharaAnimControl3PVNPC::~CGameCharaAnimControl3PVNPC()
{
}

bool CGameCharaAnimControl3PVNPC::PlayDefault( ID_DEFAULT_AI id, REAL32 timeStart, REAL32 timeScale)
{
	bool bRv = false;

	I3_BOUNDCHK( id, ID_DEFAULT_AI_MAX );

	getAnimContext()->setOldDefaultID( id);
		
	if( (getAnimContext()->getAIDefault() != NULL))
	{
		INT32 nAIIdx = getAnimContext()->getAIDefaultID( id);
		I3ASSERT( nAIIdx >= 0);

		getAnimContext()->getAIDefault()->setCurrentAIState( nAIIdx, timeStart, timeScale);

		bRv = true;
	}

	return bRv;
}
