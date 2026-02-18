#pragma once

#include "i3LevelUndoInfo.h"
#include "i3NavMeshUtil.h"

class I3_EXPORT_TDK i3NavMeshUndoInfoMoveVertex : public i3LevelUndoInfo
{
	I3_EXPORT_CLASS_DEFINE( i3NavMeshUndoInfoMoveVertex, i3LevelUndoInfo );

public:
	void	SetUndoInfo( int idx, const VEC3D& vMove )
	{
		m_idxList.clear();
		m_idxList.insert( idx );
		m_move = vMove;
	}

	void	SetUndoInfo( const i3::set< int >& idxList, const VEC3D& vMove )
	{
		m_idxList = idxList;
		m_move = vMove;
	}

public:
	i3::set< int >		m_idxList;
	VEC3D				m_move;

public:
	virtual bool	Undo( i3LevelScene * pScene ) override;
	virtual bool	Redo( i3LevelScene * pScene ) override;

};