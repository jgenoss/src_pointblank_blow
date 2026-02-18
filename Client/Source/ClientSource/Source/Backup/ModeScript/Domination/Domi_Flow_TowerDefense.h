#pragma once

#include "./Domination_FlowBase.h"

class gmode_domination;
class CDomi_Flow_TowerDefense : public CDominationFlowBase
{
private:
	UINT16						m_iLimit;

	i3RespawnObj*				m_pAiRespawnObj;
	i3RespawnObj*				m_pAiDestObj;

	std::vector<INT32>			m_vecTriggerAI;
	INT32						m_nCurrentTriggerAI;

public:
	CDomi_Flow_TowerDefense( gmode_domination* pmode);
	~CDomi_Flow_TowerDefense(void);

	void ProcessModeData( INT32 nIndex, INT32 nData);
	void ProcessUpdate( REAL32 rDeltaTime);
};
