#if !defined( __I3_AI_DRIVER_H)
#define __I3_AI_DRIVER_H

#include "i3AIState.h"

typedef struct _tagi3AIStateProcInfo
{
	char				m_szName[64];
	I3_AISTATE_PROC		m_pRunProc;
	I3_AISTATE_PROC		m_pFinishProc;
} I3_AISTATE_PROC_TABLE;

class I3_EXPORT_FRAMEWORK i3AIDriver : public i3ElementBase
{
	I3_CLASS_DEFINE( i3AIDriver);
protected:
	I3_AISTATE_PROC_TABLE *		m_pTable;	

public:
	i3AIDriver(void);
	virtual ~i3AIDriver(void);

	I3_AISTATE_PROC_TABLE *	getAIProcTable(void)			{ return m_pTable; }
	void					setAIProcTable( I3_AISTATE_PROC_TABLE * pTable)
	{
		m_pTable = pTable; 
	}

	I3_AISTATE_PROC			getRunAIProc( INT32 idx)		{ return m_pTable[idx].m_pRunProc; }
	I3_AISTATE_PROC			getFinishAIProc( INT32 idx)		{ return m_pTable[idx].m_pFinishProc; }
};

#endif
