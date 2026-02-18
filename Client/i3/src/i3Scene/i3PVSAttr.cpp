#include "i3SceneDef.h"
#include "i3PVSAttr.h"
#include "i3SceneTracer.h"

I3_CLASS_INSTANCE( i3PVSAttr, i3Node );

I3_TRACE_RESULT I3SG_NODE_PROC( i3PVSAttr )( i3Node * pNode, i3SceneTracer * pTracer)
{
	i3PVSAttr * pAttrSet = (i3PVSAttr *) pNode;
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;
	i3AttrStackManager * pStackManager = pTracer->GetAttrStackManager();


	
	return Result;
}

i3PVSAttr::i3PVSAttr()
{
}

i3PVSAttr::~i3PVSAttr()
{	
}

UINT32	i3PVSAttr::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;

	return Result;
}

UINT32	i3PVSAttr::OnLoad( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;


	return Result;
}

