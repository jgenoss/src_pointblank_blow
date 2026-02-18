#include "i3SceneDef.h"
#include "i3ParticleRender.h"
#include "i3ParticleRenderNode.h"
#include "i3SceneTracer.h"

I3_CLASS_INSTANCE( i3ParticleRenderNode);

I3_TRACE_RESULT I3SG_NODE_PROC( i3ParticleRenderNode)( i3Node * pNode, i3SceneTracer * pTracer)
{
	g_pParticleRender->Render( pTracer);

	if( pNode->GetChildCount() > 0)
	{
		return i3Node::CallTraceProc( pNode, pTracer);
	}

	return I3_TRACE_RESULT_CONTINUE;
}

i3ParticleRenderNode::i3ParticleRenderNode()
{
	_setTraceProc( I3SG_NODE_PROC( i3ParticleRenderNode));
}

