#include "stdafx.h"
#include "i3NavMeshUndoInfoInvertPolyNormal.h"

#include "i3NavMeshDataMgr.h"
#include "i3LevelGlobalVariable.h"

I3_CLASS_INSTANCE( i3NavMeshUndoInfoInvertPolyNormal); //, i3LevelUndoInfo );

bool	i3NavMeshUndoInfoInvertPolyNormal::Undo( i3LevelScene * pScene )
{
	if( nullptr == i3Level::GetNavMesh() )
	{
		return false;
	}

	i3NavMeshDataMgr* dataMgr = i3Level::GetNavMesh()->GetDataMgr();

	if( nullptr == dataMgr )
	{
		return false;
	}

	for each( int idx in m_idxList )
	{
		 dataMgr->InvertPolygonNormal( idx );
	}
	
	return true;
}

bool	i3NavMeshUndoInfoInvertPolyNormal::Redo( i3LevelScene * pScene )
{
	if( nullptr == i3Level::GetNavMesh() )
	{
		return false;
	}

	i3NavMeshDataMgr* dataMgr = i3Level::GetNavMesh()->GetDataMgr();

	if( nullptr == dataMgr )
	{
		return false;
	}

	for each( int idx in m_idxList )
	{
		dataMgr->InvertPolygonNormal( idx );
	}

	return true;
}