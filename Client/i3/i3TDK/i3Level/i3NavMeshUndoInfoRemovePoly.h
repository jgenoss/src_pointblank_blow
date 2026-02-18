#pragma once

#include "i3LevelUndoInfo.h"
#include "i3NavMeshUtil.h"

class I3_EXPORT_TDK i3NavMeshUndoInfoRemovePoly : public i3LevelUndoInfo
{
	I3_EXPORT_CLASS_DEFINE( i3NavMeshUndoInfoRemovePoly, i3LevelUndoInfo );

public:
	void	SetUndoInfo( int idx )
	{
		m_idxList.clear();
		m_idxList.insert( idx );
	}

	void	SetUndoInfo( const i3::set< int >& undoList )
	{
		m_idxList = undoList ;
	}

public:
	i3::set< int >		m_idxList;

public:
	virtual bool	Undo( i3LevelScene * pScene ) override;
	virtual bool	Redo( i3LevelScene * pScene ) override;

};