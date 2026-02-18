#if !defined( __UI_POPUP_MEDAL_REWARD_H__)
#define __UI_POPUP_MEDAL_REWARD_H__

#include "UIPopupBase.h"

class UIPopupMedalReward : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupMedalReward, UIPopupBase);

public:
	UIPopupMedalReward();
	virtual ~UIPopupMedalReward();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnEntranceStart(void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual bool	OnOKButton( void) override;
};

#endif
