#if !defined( __I3_LEVEL_UNDO_INFO_CREATE_H)
#define __I3_LEVEL_UNDO_INFO_CREATE_H

#include "i3LevelUndoInfo.h"
#include "i3LevelElement3D.h"

class I3_EXPORT_TDK i3LevelUndoInfoCreate : public i3LevelUndoInfo
{
	I3_EXPORT_CLASS_DEFINE( i3LevelUndoInfoCreate, i3LevelUndoInfo);

public:
	virtual void	setObject( i3LevelElement * pElement) override;

public:
	virtual bool	Undo(i3LevelScene * pScene) override;
	virtual bool	Redo(i3LevelScene * pScene) override;
};

#endif
