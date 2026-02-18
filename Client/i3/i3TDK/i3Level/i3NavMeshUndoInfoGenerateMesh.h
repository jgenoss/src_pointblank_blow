#pragma once

#include "i3LevelUndoInfo.h"
#include "i3NavMeshUtil.h"

class i3NavMeshDataMgr;

class I3_EXPORT_TDK i3NavMeshUndoInfoGenerateMesh : public i3LevelUndoInfo
{
	I3_EXPORT_CLASS_DEFINE( i3NavMeshUndoInfoGenerateMesh, i3LevelUndoInfo );

public:
	void	SetUndoInfo( i3NavMeshDataMgr* prev, i3NavMeshDataMgr* next );

public:
	i3NavMeshDataMgr*		m_prev = nullptr;
	i3NavMeshDataMgr*		m_next = nullptr;

public:
	virtual bool	Undo( i3LevelScene * pScene ) override;
	virtual bool	Redo( i3LevelScene * pScene ) override;

public:
	~i3NavMeshUndoInfoGenerateMesh();
};