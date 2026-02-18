#pragma once

#ifdef	DOMI_FLOW

#include "Domination_FlowBase.h"


class gmode_domination;

class CDominationDayEpilogue : public CDominationFlowBase
{
public:
	CDominationDayEpilogue( gmode_domination* pmode);
	virtual ~CDominationDayEpilogue(void);
};

#endif