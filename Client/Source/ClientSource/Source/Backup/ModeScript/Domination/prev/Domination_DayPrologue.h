#pragma once

#ifdef	DOMI_FLOW

#include "Domination_FlowBase.h"


class gmode_domination;

class CDominationDayPrologue : public CDominationFlowBase
{
public:
	CDominationDayPrologue( gmode_domination* pmode);
	virtual ~CDominationDayPrologue(void);

	virtual void ProcessEventRoundStart(void);
	virtual void ProcessEventRoundEnd();
};

#endif