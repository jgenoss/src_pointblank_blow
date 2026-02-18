#include "stdafx.h"
#include "i3NavMeshUndoInfoRemoveVertex.h"

#include "i3LevelGlobalVariable.h"

I3_CLASS_INSTANCE( i3NavMeshUndoInfoRemoveVertex); //, i3LevelUndoInfo );


bool	i3NavMeshUndoInfoRemoveVertex::Undo( i3LevelScene * pScene )
{
	if( nullptr == i3Level::GetNavMesh() )
	{
		return false;
	}

	for each( int idx in m_idxList )
	{
		i3Level::GetNavMesh()->ReUseVertex( idx );
	}

	return true;
}

bool	i3NavMeshUndoInfoRemoveVertex::Redo( i3LevelScene * pScene )
{
	if( nullptr == i3Level::GetNavMesh() )
	{
		return false;	
	}

	i3Level::GetNavMesh()->RemoveVertex( m_idxList, false );

	return true;
}