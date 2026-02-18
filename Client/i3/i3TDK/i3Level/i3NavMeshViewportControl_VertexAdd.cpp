#include "stdafx.h"
#include "i3NavMeshViewportControl_VertexAdd.h"

#include "../i3TDKGlobalVariable.h"

#include "i3LevelGlobalVariable.h"
#include "i3LevelViewport.h"

#include "i3NavMeshDataMgr.h"
#include "i3NavMesh.h"

I3_CLASS_INSTANCE( i3NavMeshViewportControl_VertexAdd); //, i3LevelViewportControl );


bool	i3NavMeshViewportControl_VertexAdd::OnLButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point )
{
	if( i3Level::GetNavMesh() )
		i3Level::GetNavMesh()->AddVertex( point );

	return true;
}

bool	i3NavMeshViewportControl_VertexAdd::OnMouseMove( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point )
{
	// ´À¸²..
	if( i3Level::GetNavMesh() )
		i3Level::GetNavMesh()->UpdateMouseMove_AddVertex( point );

	return true;
}