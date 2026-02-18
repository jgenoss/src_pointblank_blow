/**
* \file	UI\UIPhaseLogin_V11.h
*
* Declares the user interface phase login class.
*/


#if !defined( __UI_PHASE_LOGIN_V10_H__)
#define __UI_PHASE_LOGIN_V11_H__

#include "UI/UIPhaseBase.h"
#include "UI/UIPhaseLogin.h"

class UIPhaseLogin_V11 : public UIPhaseLogin
{
	I3_CLASS_DEFINE(UIPhaseLogin_V11, UIPhaseLogin);

public:
	UIPhaseLogin_V11();
	virtual ~UIPhaseLogin_V11();

protected:
	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;
};

#endif
