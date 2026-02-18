#if !defined( __I3_OPT_CHANGE_COLLISIONGROUP_H__)
#define __I3_OPT_CHANGE_COLLISIONGROUP_H__

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptChangeCollisionGroup : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptChangeCollisionGroup);
protected:
	UINT16			m_nChangeGroup;
	UINT32			m_nChangeGroupMask;

public:
	i3OptChangeCollisionGroup(void);
	virtual ~i3OptChangeCollisionGroup(void);

	void	SetCollisionGroup( UINT16 nGroup)				{ m_nChangeGroup = nGroup; }
	void	SetCollisionGroupMask( UINT32 nGroupMask)		{ m_nChangeGroupMask = nGroupMask; }

	virtual BOOL	OnNode( i3Node * pNode);
};

#endif
