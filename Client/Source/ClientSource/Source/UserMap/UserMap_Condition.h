#if !defined( __USERMAP_CONDITION_H)
#define __USERMAP_CONDITION_H

#include "UserMap_Define.h"

class CUserMap_Condition : public i3NamedElement
{
	I3_CLASS_DEFINE( CUserMap_Condition);
protected:
	USMAP::COND::INFO		m_Info;

public:
	CUserMap_Condition(void);
	virtual ~CUserMap_Condition(void);

	USMAP::COND::INFO *		getInfo(void)				{ return &m_Info; }

};

#endif
