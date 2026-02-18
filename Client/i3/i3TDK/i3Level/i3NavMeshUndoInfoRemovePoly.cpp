#include "stdafx.h"
#include "i3NavMeshUndoInfoRemovePoly.h"

#include "i3LevelGlobalVariable.h"

I3_CLASS_INSTANCE( i3NavMeshUndoInfoRemovePoly);//, i3LevelUndoInfo );

bool	i3NavMeshUndoInfoRemovePoly::Undo( i3LevelScene * pScene )
{
	if( nullptr == i3Level::GetNavMesh() )
	{
		return false;	
	}

	for each( int idx in m_idxList )
	{
		i3Level::GetNavMesh()->ReUsePolygon( idx );
	}

	return true;
}

bool	i3NavMeshUndoInfoRemovePoly::Redo( i3LevelScene * pScene )
{
	if( nullptr == i3Level::GetNavMesh() )
	{
		return false;
	}

	for each( int idx in m_idxList )
	{
		i3Level::GetNavMesh()->RemovePolygon( idx, false );	
	}	
	return true;
}