#pragma once

#include "./Domination_FlowBase.h"

class gmode_domination;
class CDomi_Flow_Survival : public CDominationFlowBase
{
private:
	INT32						m_iKillCount;

public:
	CDomi_Flow_Survival( gmode_domination* pmode);
	~CDomi_Flow_Survival(void);

	void						ProcessModeData( INT32 nIndex, INT32 nData);
};
