#if !defined( __UI_POPUP_INTERACTBASE_H__)
#define __UI_POPUP_INTERACTBASE_H__

#include "UIPopupBase.h"

class UIPopupInteractBase : public UIPopupBase
{
	I3_ABSTRACT_CLASS_DEFINE( UIPopupInteractBase, UIPopupBase);

protected:
	INT32			m_SelectedIndex;
	i3::rc_wstring	m_wstrNick;

public:
	UIPopupInteractBase();
	virtual ~UIPopupInteractBase();

	/*
	param[in] pArg2 INT32*
	*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
};

#endif
