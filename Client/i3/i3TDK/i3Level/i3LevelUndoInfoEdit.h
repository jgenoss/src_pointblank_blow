#if !defined( __I3_LEVEL_UNDO_INFO_EDIT_H)
#define __I3_LEVEL_UNDO_INFO_EDIT_H

#include "i3LevelUndoInfo.h"

class I3_EXPORT_TDK i3LevelUndoInfoEdit : public i3LevelUndoInfo
{
	I3_EXPORT_CLASS_DEFINE( i3LevelUndoInfoEdit, i3LevelUndoInfo);

protected:
	char	m_szFilePath[MAX_PATH] = { 0 };

	char *	m_pObjBuf = nullptr;
	UINT32	m_nBufSize = 0;
	
	UINT32				__MakeDataBuf( i3LevelElement * pElement, char **ppBuf);
	i3LevelElement *	__LoadDataBuf( char * pBuf, UINT32 size);

public:
	virtual ~i3LevelUndoInfoEdit(void);

	virtual void	setObject( i3LevelElement * pElement) override;

public:
	virtual bool	Undo(i3LevelScene * pScene) override;
	virtual bool	Redo(i3LevelScene * pScene) override;
};

#endif
