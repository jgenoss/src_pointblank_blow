#include "stdafx.h"
#include "i3NavMeshUndoInfoGenerateMesh.h"

#include "i3NavMeshDataMgr.h"
#include "i3LevelGlobalVariable.h"

I3_CLASS_INSTANCE( i3NavMeshUndoInfoGenerateMesh); //, i3LevelUndoInfo );


i3NavMeshUndoInfoGenerateMesh::~i3NavMeshUndoInfoGenerateMesh()
{
	I3_SAFE_RELEASE( m_prev );
	I3_SAFE_RELEASE( m_next );
}


void	i3NavMeshUndoInfoGenerateMesh::SetUndoInfo( i3NavMeshDataMgr* prev, i3NavMeshDataMgr* next )
{
	I3_REF_CHANGE( m_prev, prev );
	I3_REF_CHANGE( m_next, next );
}

bool	i3NavMeshUndoInfoGenerateMesh::Undo( i3LevelScene * pScene )
{
	if( nullptr == i3Level::GetNavMesh() )
	{
		return false;
	}
	
	i3Level::GetNavMesh()->SetDataMgr( m_prev );
	return true;
}

bool	i3NavMeshUndoInfoGenerateMesh::Redo( i3LevelScene * pScene )
{
	if( nullptr == i3Level::GetNavMesh() )
	{
		return false;
	}

	i3Level::GetNavMesh()->SetDataMgr( m_next );
	return true;
}