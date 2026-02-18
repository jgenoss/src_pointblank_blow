#if !defined( __I3UICONTROL_UNDO_INFO_CREATE_H__)
#define __I3UICONTROL_UNDO_INFO_CREATE_H__

#include "UIUndoInfo.h"

//#if defined( I3_DISTRIBUTE)

class UIUndoInfoCreate : public UIUndoInfo
{
	I3_CLASS_DEFINE( UIUndoInfoCreate, UIUndoInfo);

protected:
	i3GameNode* m_pParent;

public:
	UIUndoInfoCreate(void);
	virtual ~UIUndoInfoCreate(void);

	virtual void	setObject( i3UIControl * pObj) override;
	virtual bool	Undo( i3UIScene * pScene) override;
	virtual bool	Redo( i3UIScene * pScene) override;

	virtual void	PrintUndoInfo( void) override;
};

//#endif	// I3_DISTRIBUTE

#endif	//__I3UICONTROL_UNDO_INFO_CREATE_H__
