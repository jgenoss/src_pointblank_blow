#if !defined( __I3UICONTROL_UNDO_INFO_DELETE_H__)
#define __I3UICONTROL_UNDO_INFO_DELETE_H__

#include "UIUndoInfo.h"

//#if defined( I3_DISTRIBUTE)

class UIUndoInfoDelete : public UIUndoInfo
{
	I3_CLASS_DEFINE( UIUndoInfoDelete, UIUndoInfo);

protected:
	i3GameNode* m_pParent;

public:
	UIUndoInfoDelete(void);
	virtual ~UIUndoInfoDelete(void);

	virtual void	setObject( i3UIControl * pObj) override;
	virtual bool	Undo( i3UIScene * pScene) override;
	virtual bool	Redo( i3UIScene * pScene) override;

	virtual void	PrintUndoInfo( void);
};

//#endif	// I3_DISTRIBUTE

#endif	//__I3UICONTROL_UNDO_INFO_DELETE_H__
