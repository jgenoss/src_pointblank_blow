#if !defined( __I3_LEVEL_UNDO_INFO_H)
#define __I3_LEVEL_UNDO_INFO_H

#include "i3LevelElement3D.h"

class I3_EXPORT_TDK i3LevelUndoInfo : public i3NamedElement
{
	I3_EXPORT_CLASS_DEFINE( i3LevelUndoInfo, i3NamedElement);

protected:
	i3::vector<i3LevelUndoInfo*>		m_UndoList;		//гою╖ undo list   undo --+-- undo
														//						  +-- undo
	i3LevelElement		*	m_pElement = nullptr;

public:
	virtual ~i3LevelUndoInfo(void);

	virtual void	setObject( i3LevelElement * pElement) {	I3_REF_CHANGE(m_pElement, pElement);}
	i3LevelElement		*	getObject(void)					{	return m_pElement;}

	INT32	GetChildInfoCount(void) {	return (INT32)m_UndoList.size();	}
public:
	void			AddChildUndoInfo( i3LevelUndoInfo * pChildInfo);

	virtual bool	Undo(i3LevelScene * pScene);
	virtual bool	Redo(i3LevelScene * pScene);
};

#endif
