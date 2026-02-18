#if !defined( __I3_AI_DRIVER_H)
#define __I3_AI_DRIVER_H

#include "i3AIState.h"

// static table struct
namespace i3
{
	namespace statictable
	{
		struct AISTATE_PROC
		{
			char				m_szName[64];
			I3_AISTATE_PROC		m_pRunProc;
			I3_AISTATE_PROC		m_pFinishProc;
		};
	}
}

class I3_EXPORT_FRAMEWORK i3AIDriver : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3AIDriver, i3ElementBase);
protected:
	i3::statictable::AISTATE_PROC *		m_pTable = nullptr;
	INT32						m_nCount = 0;

public:
	i3::statictable::AISTATE_PROC *	getAIProcTable(void)			{ return m_pTable; }
	void					setAIProcTable( i3::statictable::AISTATE_PROC * pTable, INT32 count)
	{
		m_pTable = pTable; 
		m_nCount = count;
	}

	I3_AISTATE_PROC			getRunAIProc( INT32 idx)		{ if( idx < 0 || idx >= m_nCount) return nullptr; else return m_pTable[idx].m_pRunProc; }
	I3_AISTATE_PROC			getFinishAIProc( INT32 idx)		{ if( idx < 0 || idx >= m_nCount) return nullptr; else return m_pTable[idx].m_pFinishProc; }
	INT32					getAIProcCount(void)			{ return m_nCount;	}
};

#endif
