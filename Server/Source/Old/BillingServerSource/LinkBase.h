#ifndef __LINK_BASE_H__
#define __LINK_BASE_H__

#include "RingBuffer.h"

class CLinkBase : public i3Thread
{
	I3_CLASS_DEFINE( CLinkBase );

protected:
	INT32					m_i32LogThreadIdx;

	CRingBuffer*			m_pRingLinkIn;
	CRingBuffer*			m_pRingLinkOut;

public:
	CLinkBase();
	virtual ~CLinkBase();

	virtual BOOL			OnCreate( CRingBuffer* pRingLinkIn, CRingBuffer* pRingLinkOut );
	void					OnDestroy();

};

#endif