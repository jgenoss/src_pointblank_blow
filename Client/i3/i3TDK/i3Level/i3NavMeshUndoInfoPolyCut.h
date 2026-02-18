#pragma once

#include "i3LevelUndoInfo.h"
#include "i3NavMeshUtil.h"

class I3_EXPORT_TDK i3NavMeshUndoInfoPolyCut : public i3LevelUndoInfo
{
	I3_EXPORT_CLASS_DEFINE( i3NavMeshUndoInfoPolyCut, i3LevelUndoInfo );

public:
	void	SetUndoInfo( int addVertex, const i3::vector< int >& removePoly, const i3::vector< int >& createPoly )
	{
		m_addVertex = addVertex;
		m_removePolyList = removePoly;
		m_createPolyList = createPoly;
	}

public:
	i3::vector< int >	m_removePolyList;	
	i3::vector< int >	m_createPolyList;	

	int			m_addVertex;

public:
	virtual bool	Undo( i3LevelScene * pScene ) override;
	virtual bool	Redo( i3LevelScene * pScene ) override;

};