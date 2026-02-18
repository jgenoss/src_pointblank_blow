#pragma once

#include "./Domination_FlowBase.h"

class gmode_domination;
class CDomi_Flow_Boss : public CDominationFlowBase 
{
private:
	enum MISSION_BOMB_EVENT
	{
		BOMB_SUM_COUNT = 110,
		BOMB_COUNT
	};

	i3GameObj*					m_pGuideObject;

public:
	CDomi_Flow_Boss( gmode_domination* pmode);
	~CDomi_Flow_Boss(void);

	void ProcessEventRoundStart() {}
	void ProcessModeData( INT32 nIndex, INT32 nData);
};
