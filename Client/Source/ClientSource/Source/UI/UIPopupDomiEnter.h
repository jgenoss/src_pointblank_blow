#pragma once

#include "UIPopupBase.h"

class UIPopupDomiEnter : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupDomiEnter, UIPopupBase);

private:
	i3UIEditBox * m_pNearEnd;
	i3UIEditBox * m_pLate;

public:
	UIPopupDomiEnter();
	virtual ~UIPopupDomiEnter();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd() override;
	virtual bool	OnExitStart(void) override;

	virtual bool	OnKey_Enter( ) override;
	virtual bool	OnKey_Escape() override;

	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData) override;
};
