#pragma once

#include "i3LevelUndoInfo.h"

class I3_EXPORT_TDK i3NavMeshUndoInfoAddVertex : public i3LevelUndoInfo
{
	I3_EXPORT_CLASS_DEFINE( i3NavMeshUndoInfoAddVertex, i3LevelUndoInfo );

public:
	void		SetUndoInfo( int idx )
	{
		m_idx = idx;
	}

public:
	int			m_idx = -1;

public:
	virtual bool	Undo( i3LevelScene * pScene ) override;
	virtual bool	Redo( i3LevelScene * pScene ) override;
};