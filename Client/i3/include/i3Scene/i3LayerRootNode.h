#ifndef __I3_LAYER_ROOT_NODE_H__
#define __I3_LAYER_ROOT_NODE_H__

#include "i3AttrSet.h"

class I3_EXPORT_SCENE i3LayerRootNode : public i3AttrSet
{
	I3_EXPORT_CLASS_DEFINE( i3LayerRootNode, i3AttrSet);

public:
	bool			m_bResetLighting = false;

public:
	void	SetResetLighting( bool bFlag)		{ m_bResetLighting = bFlag;		}
};

#endif // __I3_LAYER_ROOT_NODE_H__
