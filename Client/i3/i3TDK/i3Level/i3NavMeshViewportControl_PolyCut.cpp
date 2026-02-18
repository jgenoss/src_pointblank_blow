#include "stdafx.h"
#include "i3NavMeshViewportControl_PolyCut.h"
#include "i3LevelViewport.h"
#include "i3LevelFramework.h"
#include "i3LevelGlobalVariable.h"
#include "../i3TDKGlobalVariable.h"

#include "i3NavMeshDataMgr.h"
#include "i3NavMesh.h"

I3_CLASS_INSTANCE( i3NavMeshViewportControl_PolyCut); //, i3LevelViewportControl );

bool	i3NavMeshViewportControl_PolyCut::OnLButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point )
{	
	if( nullptr == i3Level::GetNavMesh() )
	{
		return false;
	}

	i3Level::GetNavMesh()->UpdateCuttingPoly( point );
	i3Level::GetNavMesh()->CuttingPoly();
	return false;
}

bool	i3NavMeshViewportControl_PolyCut::OnMouseMove( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point )
{	
	if( nullptr == i3Level::GetNavMesh() )
	{
		return true;
	}

	i3Level::GetNavMesh()->UpdateMouseMove( point, true, false );
	i3Level::GetNavMesh()->UpdateCuttingPoly( point );
	return true;
}