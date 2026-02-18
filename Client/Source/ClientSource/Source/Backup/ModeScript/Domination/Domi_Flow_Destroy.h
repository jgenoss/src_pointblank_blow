#pragma once

#include "./Domination_FlowBase.h"

class gmode_domination;
class CDomi_Flow_Destroy : public CDominationFlowBase
{
private:
	enum MISSION_DESTROY_EVENT
	{
		EGG_SUM_COUNT = 110,
		EGG_DESTROY_COUNT,
		EGG_OBJECT_0,
		EGG_OBJECT_1,
		EGG_OBJECT_2,
		EGG_OBJECT_3,
		EGG_OBJECT_4,
		EGG_OBJECT_5
	};

	i3Object*			m_pDestroyObj;	

public:
	CDomi_Flow_Destroy( gmode_domination* pmode);
	~CDomi_Flow_Destroy(void);

	void						ProcessModeData( INT32 nIndex, INT32 nData);
	virtual	void				ResetObject();
};
