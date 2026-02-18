#pragma once

#ifdef	DOMI_FLOW

#include "Domination_FlowBase.h"


class gmode_domination;

class CDominationIntermission : public CDominationFlowBase
{
public:
	CDominationIntermission( gmode_domination* pmode);
	virtual ~CDominationIntermission(void);

	virtual void ProcessEventKey(INT32 arg);

	virtual void StartFlow();
	virtual void EndFlow();

	virtual void OnUpdate( REAL32 rDeltaSeconds);

private:
	REAL32		m_rOutputLabelTime;
	REAL32		m_rInputIntervalTime;
};

#endif