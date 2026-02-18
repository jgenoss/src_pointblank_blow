#include "stdafx.h"
#include "i3NavMeshUndoInfoMoveVertex.h"

#include "i3NavMeshDataMgr.h"
#include "i3LevelGlobalVariable.h"

I3_CLASS_INSTANCE( i3NavMeshUndoInfoMoveVertex); //, i3LevelUndoInfo );

bool	i3NavMeshUndoInfoMoveVertex::Undo( i3LevelScene * pScene )
{
	if( nullptr == i3Level::GetNavMesh() )
	{
		return false;
	}

	VEC3D vReverse;
	
	vReverse.x = (-m_move.x);
	vReverse.y = (-m_move.y);
	vReverse.z = (-m_move.z);
	
	i3Level::GetNavMesh()->MoveVertex( m_idxList, vReverse, false );
	return true;
}

bool	i3NavMeshUndoInfoMoveVertex::Redo( i3LevelScene * pScene )
{
	if( nullptr == i3Level::GetNavMesh() )
	{
		return false;
	}

	i3Level::GetNavMesh()->MoveVertex( m_idxList, m_move, false );
	return true;
}