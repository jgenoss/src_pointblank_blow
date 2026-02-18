#pragma once

#include "i3LevelUndoInfo.h"
#include "i3NavMeshUtil.h"

class I3_EXPORT_TDK i3NavMeshUndoInfoRemoveVertex : public i3LevelUndoInfo
{
	I3_EXPORT_CLASS_DEFINE( i3NavMeshUndoInfoRemoveVertex, i3LevelUndoInfo );

public:
	void	SetUndoInfo( int idx )
	{
		m_idxList.clear();
		m_idxList.push_back( idx );
	}

	void	SetUndoInfo( const i3::vector< int >& idxList )
	{
		m_idxList = idxList;
	}

public:
	i3::vector< int >		m_idxList;

public:
	virtual bool	Undo( i3LevelScene * pScene ) override;
	virtual bool	Redo( i3LevelScene * pScene ) override;
};