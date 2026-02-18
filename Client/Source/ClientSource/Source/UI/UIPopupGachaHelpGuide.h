#pragma once

#include "UIPopupBase.h"

class UIPopupGachaHelpGuide : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupGachaHelpGuide, UIPopupBase);

public:
	UIPopupGachaHelpGuide();
	virtual ~UIPopupGachaHelpGuide();

	void clicked();

public:
	virtual void	OnCreate( i3GameNode * pParent) override;
	
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;

	virtual void	OnEntranceEnd( void) override;

	virtual bool	OnExitStart( void) override;

	virtual bool	OnKey_Escape(void) override;

};