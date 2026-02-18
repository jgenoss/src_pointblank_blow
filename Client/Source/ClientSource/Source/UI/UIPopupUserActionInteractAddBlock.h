#if !defined( __UI_POPUP_USERACTIONADDBLOCK_H__)
#define __UI_POPUP_USERACTIONADDBLOCK_H__

#include "UIPopupUserActionInteractBase.h"

class UIPopupUserActionInteractAddBlock : public UIPopupUserActionInteractBase
{
	I3_CLASS_DEFINE( UIPopupUserActionInteractAddBlock);
private:
	i3UIEditBox *	m_pReasonBox;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene);

public:
	UIPopupUserActionInteractAddBlock();
	virtual ~UIPopupUserActionInteractAddBlock();

	virtual void	OnCreate( i3GameNode * pParent);
	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void	OnOKButton( void);
};

#endif
