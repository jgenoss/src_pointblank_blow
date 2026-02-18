#if !defined( __I3_OPT_CHANGE_CHANGE_PHYSX_FORMAT_H__)
#define __I3_OPT_CHANGE_CHANGE_PHYSX_FORMAT_H__

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptChangePhysXFormat : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptChangePhysXFormat, i3SceneOptimizer);
protected:
	i3PhysixShapeSet::FORMAT		m_Format = i3PhysixShapeSet::FORMAT_BINARY;

public:
	i3OptChangePhysXFormat(void);

	i3PhysixShapeSet::FORMAT		getFormat(void)									{ return m_Format; }
	void							setFormat( i3PhysixShapeSet::FORMAT format)		{ m_Format = format; }

	virtual bool	OnNode( i3Node * pNode, i3Node * pParent) override;
};

#endif
