#include "i3SceneDef.h"
#include "i3OcclusionQuery.h"
#include "i3SceneTracer.h"
#include "i3CommonNodeProcs.h"

I3_CLASS_INSTANCE( i3OcclusionQuery);

I3_TRACE_RESULT I3SG_NODE_PROC( i3OcclusionQuery)( i3Node * pNode, i3SceneTracer * pTracer)
{
	i3OcclusionQuery * pQuery = (i3OcclusionQuery *) pNode;
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;

	if( pNode->GetChildCount() > 0)
	{
		// Issue¿ë
		pTracer->AddAttributeToOutput( pQuery->m_pAttr);

		Result = i3Node::CallTraceProc( pNode, pTracer);

		// Gather result¿ë
		pTracer->AddAttributeToOutput( pQuery->m_pAttr);
	}

	return Result;
}

i3OcclusionQuery::i3OcclusionQuery(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3OcclusionQuery));

	m_pAttr = i3OcclusionQueryAttr::new_object();
}

i3OcclusionQuery::~i3OcclusionQuery(void)
{
	I3_SAFE_RELEASE( m_pAttr);
}
