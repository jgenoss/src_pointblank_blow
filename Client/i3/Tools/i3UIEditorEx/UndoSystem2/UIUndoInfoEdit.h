#if !defined( __I3UICONTROL_UNDO_INFO_EDIT_H__)
#define __I3UICONTROL_UNDO_INFO_EDIT_H__

#include "UIUndoInfo.h"

//#if defined( I3_DISTRIBUTE)

//프로퍼티창을 통해 값을 바꾸는 경우의 UndoInfo
class UIUndoInfoEdit : public UIUndoInfo
{
	I3_CLASS_DEFINE( UIUndoInfoEdit, UIUndoInfo);

protected:
	i3UIControl*	m_pClone;
		
	void			_MakeClone( void);
	void			_Swap( void);
public:
	UIUndoInfoEdit(void);
	virtual ~UIUndoInfoEdit(void);

	virtual void	setObject( i3UIControl * pObj) override;
	virtual bool	Undo( i3UIScene * pScene) override;
	virtual bool	Redo( i3UIScene * pScene) override;

	virtual void	PrintUndoInfo( void) override;
};

//#endif	// I3_DISTRIBUTE

#endif	//__I3UICONTROL_UNDO_INFO_EDIT_H__
