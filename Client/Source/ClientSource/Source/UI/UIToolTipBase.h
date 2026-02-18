#if !defined( __UI_TOOL_TIP_BASE_H__)
#define __UI_TOOL_TIP_BASE_H__

#include "UIBase.h"

class UIToolTipBase : public UIBase
{
	I3_ABSTRACT_CLASS_DEFINE( UIToolTipBase, UIBase);

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;

public:
	UIToolTipBase();
	virtual ~UIToolTipBase();

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
};

#endif
