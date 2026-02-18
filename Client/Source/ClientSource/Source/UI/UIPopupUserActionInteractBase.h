#if !defined( __UI_POPUP_USERACTIONINTERACTBASE_H__)
#define __UI_POPUP_USERACTIONINTERACTBASE_H__

#include "UIPopupBase.h"

class UIPopupUserActionInteractBase : public UIPopupBase
{
	I3_ABSTRACT_CLASS_DEFINE( UIPopupUserActionInteractBase);

protected:
	char			m_szNick[ NET_NICK_NAME_SIZE];

public:
	UIPopupUserActionInteractBase();
	virtual ~UIPopupUserActionInteractBase();

	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArg2 = NULL);
};

#endif
