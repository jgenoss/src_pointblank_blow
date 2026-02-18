/**
 * \file	UI\UIPhaseLogin_V10.h
 *
 * Declares the user interface phase login class.
 */


#if !defined( __UI_PHASE_LOGIN_V10_H__)
#define __UI_PHASE_LOGIN_V10_H__

#include "UI/UIPhaseBase.h"
#include "UI/UIPhaseLogin.h"

class UIPhaseLogin_V10 : public UIPhaseLogin
{
	I3_CLASS_DEFINE( UIPhaseLogin_V10, UIPhaseLogin);

public:
	UIPhaseLogin_V10();
	virtual ~UIPhaseLogin_V10();

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene);
};

#endif
