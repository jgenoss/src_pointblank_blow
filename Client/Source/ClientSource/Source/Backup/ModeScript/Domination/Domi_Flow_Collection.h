#pragma once

#include "./Domination_FlowBase.h"

class gmode_domination;
class CDomi_Flow_Collection : public CDominationFlowBase
{
private:
	i3Object*			m_pDestinationObj;		//µµÂø ÁöÁ¡.

public:
	CDomi_Flow_Collection( gmode_domination* pmode);
	~CDomi_Flow_Collection(void);

	void				ProcessModeData( INT32 nIndex, INT32 nData);
};
