#include "i3SceneDef.h"
#include "i3UserNode.h"
#include "i3SceneTracer.h"

I3_CLASS_INSTANCE( i3UserNode);

I3_TRACE_RESULT I3SG_NODE_PROC( i3UserNode)( i3Node * pNode, i3SceneTracer * pTracer)
{
	i3UserNode * pUserNode = (i3UserNode *) pNode;
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;

	#if NODE_PROFILE
	START_PROFILE( 34);
	#endif

	pUserNode->Drive( pTracer );

	#if NODE_PROFILE
	END_PROFILE( 34);
	#endif

	if( pNode->GetChildCount() > 0)
	{
		Result = i3Node::CallTraceProc( pNode, pTracer);
	}

	return Result;
}

i3UserNode::i3UserNode()
{
	_setTraceProc( I3SG_NODE_PROC( i3UserNode));

	m_pUserAttr = i3UserAttr::new_object();
}

i3UserNode::~i3UserNode()
{
	I3_SAFE_RELEASE( m_pUserAttr );
}

void i3UserNode::SetCallBack( I3USERNODEPROC pFunc, void *pParam )
{
	m_pFunc = pFunc;
	m_pParam = pParam;
}

void i3UserNode::Drive( i3SceneTracer * pTracer )
{
	if( m_bNeedToFlush)
	{
		i3AttrStackManager * pStackManager = pTracer->GetAttrStackManager();
		pStackManager->FlushPushedAttrs( pTracer->GetOutput());
	}

	if( m_pFunc != nullptr )
	{
		m_pFunc( pTracer, m_pParam );
	}

	pTracer->AddAttributeToOutput( m_pUserAttr );
}


