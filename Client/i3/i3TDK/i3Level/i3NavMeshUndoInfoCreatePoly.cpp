#include "stdafx.h"
#include "i3NavMeshUndoInfoCreatePoly.h"

#include "i3LevelGlobalVariable.h"

I3_CLASS_INSTANCE( i3NavMeshUndoInfoCreatePoly); //, i3LevelUndoInfo );

bool	i3NavMeshUndoInfoCreatePoly::Undo( i3LevelScene * pScene )
{
	if( nullptr == i3Level::GetNavMesh() )
	{
		return false;
	}

	i3Level::GetNavMesh()->RemovePolygon( m_idx, false );
	return true;
}

bool	i3NavMeshUndoInfoCreatePoly::Redo( i3LevelScene * pScene )
{
	if( nullptr == i3Level::GetNavMesh() )
	{
		return false;	
	}

	i3Level::GetNavMesh()->ReUsePolygon( m_idx );
	return true;
}