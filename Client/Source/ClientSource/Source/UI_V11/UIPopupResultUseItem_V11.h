#if !defined( __UI_POPUP_RESULT_USEITEM_H__)
#define __UI_POPUP_RESULT_USEITEM_H__

#include "UI/UIPopupItemBase.h"

class UIPopupResultUseItem_V11 : public UIPopupItemBase
{
	I3_CLASS_DEFINE( UIPopupResultUseItem_V11, UIPopupItemBase);

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene);
	virtual LuaState * _SetItemInfo( void);

public:
	virtual void	OnCreate( i3GameNode * pParent);
	virtual bool	OnKey_Enter() { _OnClose(); return true; }

	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArg2 = NULL);
	virtual bool	OnExitStart( void);
};

#endif
