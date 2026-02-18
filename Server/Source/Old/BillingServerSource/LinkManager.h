#ifndef __LINK_MANAGER_H__
#define __LINK_MANAGER_H__

#include "LinkBase.h"

class CLinkManager : public i3ElementBase
{
	I3_CLASS_DEFINE( CLinkManager );

	CLinkBase*				m_pLink;

public:
	CLinkManager();
	virtual ~CLinkManager();

	BOOL					OnCreate( CRingBuffer* pRingLinkIn, CRingBuffer* pRingLinkOut );
	void					OnDestroy();

};

#endif