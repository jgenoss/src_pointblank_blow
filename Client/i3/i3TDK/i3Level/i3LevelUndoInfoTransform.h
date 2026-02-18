#if !defined( __I3_LEVEL_UNDO_INFO_TRANSFORM_H)
#define __I3_LEVEL_UNDO_INFO_TRANSFORM_H

#include "i3LevelUndoInfo.h"

class I3_EXPORT_TDK i3LevelUndoInfoTransform : public i3LevelUndoInfo
{
	I3_EXPORT_CLASS_DEFINE( i3LevelUndoInfoTransform, i3LevelUndoInfo);
protected:
	MATRIX		m_mMatrix;

public:
	void			SetTransform( MATRIX * pmtx);

public:
	virtual bool	Undo(i3LevelScene * pScene) override;
	virtual bool	Redo(i3LevelScene * pScene) override;
	virtual void	setObject( i3LevelElement * pElement) override;
};

#endif
