#pragma once

#include "UIPopupBase.h"

class UIPopupDomiScore : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupDomiScore, UIPopupBase);
	
public:
	UIPopupDomiScore();
	virtual ~UIPopupDomiScore();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd() override;
	virtual bool	OnExitStart(void) override;

	virtual bool	OnKey_Enter() override;
	virtual bool	OnKey_Escape() override;
};
