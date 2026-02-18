#pragma once

#include "i3LevelUndoInfo.h"
#include "i3NavMeshUtil.h"

class I3_EXPORT_TDK i3NavMeshUndoInfoInvertPolyNormal : public i3LevelUndoInfo
{
	I3_EXPORT_CLASS_DEFINE( i3NavMeshUndoInfoInvertPolyNormal, i3LevelUndoInfo );

public:
	void	SetUndoInfo( int idx )
	{
		m_idxList.clear();
		m_idxList.insert( idx );
	}

	void	SetUndoInfo( const i3::set< int >& idxList )
	{
		m_idxList = idxList;
	}

public:
	i3::set< int >		m_idxList;

public:
	virtual bool	Undo( i3LevelScene * pScene ) override;
	virtual bool	Redo( i3LevelScene * pScene ) override;

};