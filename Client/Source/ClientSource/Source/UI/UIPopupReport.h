#if !defined( __UI_POPUP_REPORT_H__)
#define __UI_POPUP_REPORT_H__

#include "UIPopupBase.h"



class UIPopupReport : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupReport, UIPopupBase);

public:
	UIPopupReport();
	virtual ~UIPopupReport();


	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnKey_Enter( void) override;
};

#endif
