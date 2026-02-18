#ifndef __I3_LAYER_ROOT_NODE_H__
#define __I3_LAYER_ROOT_NODE_H__

#include "i3AttrSet.h"

class I3_EXPORT_SCENE i3LayerRootNode : public i3AttrSet
{
	I3_CLASS_DEFINE( i3LayerRootNode);

public:
	BOOL			m_bResetLighting;

public:
	i3LayerRootNode(void);
	virtual ~i3LayerRootNode(void);

	void	SetResetLighting( BOOL bFlag)		{ m_bResetLighting = bFlag;		}
};

#endif // __I3_LAYER_ROOT_NODE_H__
