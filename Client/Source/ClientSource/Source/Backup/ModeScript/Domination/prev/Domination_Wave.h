#pragma once

#ifdef	DOMI_FLOW

#include "Domination_FlowBase.h"

class gmode_domination;
class CDominationWave : public CDominationFlowBase
{
public:
	CDominationWave( gmode_domination* pmode);
	virtual ~CDominationWave(void);

	virtual void ProcessEventPreStartRound(void);
	virtual void ProcessEventRoundStart(void);
	virtual void ProcessEventRoundEnd(void);

	virtual void OnUpdate( REAL32 rDeltaSeconds);
};

#endif