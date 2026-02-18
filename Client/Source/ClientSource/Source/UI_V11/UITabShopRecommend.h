#if !defined( __UI_TAB_SHOPRECOMMEND_H__)
#define __UI_TAB_SHOPRECOMMEND_H__

#include "UI_V11/UITabShopBase.h"

//애초에 구현이 안되어 있던 페이지 일단 컨버팅 완료 2015-05-21 김대영

class UIToolTipShop;

class UITabShopRecommend : public UITabShopBase
{

public:
	UITabShopRecommend(UIPhaseSubBase* phase, UIToolTipShop* pTTWeapon, UIToolTipShop* pTTNonWeapon);
	virtual ~UITabShopRecommend();
	
};

#endif
