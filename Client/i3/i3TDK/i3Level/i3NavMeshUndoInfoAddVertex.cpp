#include "stdafx.h"
#include "i3NavMeshUndoInfoAddVertex.h"

#include "i3LevelGlobalVariable.h"

I3_CLASS_INSTANCE( i3NavMeshUndoInfoAddVertex); //, i3LevelUndoInfo );

bool	i3NavMeshUndoInfoAddVertex::Undo( i3LevelScene * pScene )
{
	if( i3Level::GetNavMesh() )
		i3Level::GetNavMesh()->RemoveVertex( m_idx, false );

	return true;
}

bool	i3NavMeshUndoInfoAddVertex::Redo( i3LevelScene * pScene )
{
	if( i3Level::GetNavMesh() )
		i3Level::GetNavMesh()->ReUseVertex( m_idx );

	return true;
}