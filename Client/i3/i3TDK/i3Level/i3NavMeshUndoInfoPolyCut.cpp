#include "stdafx.h"
#include "i3NavMeshUndoInfoPolyCut.h"

#include "i3LevelGlobalVariable.h"

I3_CLASS_INSTANCE( i3NavMeshUndoInfoPolyCut);//, i3LevelUndoInfo );

bool	i3NavMeshUndoInfoPolyCut::Undo( i3LevelScene * pScene )
{
	if( nullptr == i3Level::GetNavMesh() )
	{
		return false;
	}

	for each( int idx in m_createPolyList )
	{
		i3Level::GetNavMesh()->RemovePolygon( idx, false );	
	}

	i3Level::GetNavMesh()->RemoveVertex( m_addVertex, false );

	for each( int idx in m_removePolyList )
	{
		i3Level::GetNavMesh()->ReUsePolygon( idx );	
	}

	return true;
}

bool	i3NavMeshUndoInfoPolyCut::Redo( i3LevelScene * pScene )
{
	if( nullptr == i3Level::GetNavMesh() )
	{
		return false;
	}

	for each( int idx in m_removePolyList )
	{
		i3Level::GetNavMesh()->RemovePolygon( idx, false );	
	}

	i3Level::GetNavMesh()->ReUseVertex( m_addVertex );

	for each( int idx in m_createPolyList )
	{
		i3Level::GetNavMesh()->ReUsePolygon( idx );	
	}
	
	return true;
}