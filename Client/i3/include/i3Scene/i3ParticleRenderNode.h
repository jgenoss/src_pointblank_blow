#ifndef __I3_PARTICLERENDER_NODE_H__
#define __I3_PARTICLERENDER_NODE_H__

#include "i3Base.h"
#include "i3Node.h"

class I3_EXPORT_SCENE i3ParticleRenderNode : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3ParticleRenderNode, i3Node );
	
public:
	i3ParticleRenderNode();
};


#endif //__I3_USERNODE_H__
