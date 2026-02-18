#pragma once

#include "i3LevelUndoInfo.h"
#include "i3NavMeshUtil.h"

class I3_EXPORT_TDK i3NavMeshUndoInfoCreatePoly : public i3LevelUndoInfo
{
	I3_EXPORT_CLASS_DEFINE( i3NavMeshUndoInfoCreatePoly, i3LevelUndoInfo );

public:
	void	SetUndoInfo( int idx )
	{
		m_idx = idx;
	}

public:
	int		m_idx = 0;

public:
	virtual bool	Undo( i3LevelScene * pScene ) override;
	virtual bool	Redo( i3LevelScene * pScene ) override;
};