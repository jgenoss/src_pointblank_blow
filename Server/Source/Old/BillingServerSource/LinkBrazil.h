#ifndef __LINK_BRAZIL_H__
#define __LINK_BRAZIL_H__

#include "LinkDB.h"

class CLinkBrazil : public CLinkDB
{
	I3_CLASS_DEFINE( CLinkBrazil );

	BOOL					m_bRunning;

	BOOL					_GetUserCash();
	BOOL					_SetUserCash();
public:
	CLinkBrazil();
	virtual ~CLinkBrazil();

	virtual BOOL			OnCreate( CRingBuffer* pRingLinkIn, CRingBuffer* pRingLinkOut );
	void					OnDestroy();

	virtual UINT32			OnRunning( void* pUserData );
};

#endif