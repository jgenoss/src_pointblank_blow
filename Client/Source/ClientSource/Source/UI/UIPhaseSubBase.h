#if !defined( __UI_SUBPHASE_BASE_H__)
#define __UI_SUBPHASE_BASE_H__

#include "UIPhaseBase.h"
class UITabShopBase;

class UIPhaseSubBase : public UIPhaseBase
{
	I3_ABSTRACT_CLASS_DEFINE( UIPhaseSubBase, UIPhaseBase);

public:
	virtual void		OnUpdate(REAL32 rDeltaSeconds) override;
	virtual void		OnEscapeKey( void) override;

	virtual i3UIScene*  GetMenuScene(){ return nullptr; }
	virtual i3::rc_wstring GetKeywordString(){return L"";}
	virtual void ChangeSubCategory(){;}
	virtual UITabShopBase* FindShopTab_ByGoods(SHOP_GOODS* goods){ return nullptr;}
	virtual void EnableCharaWnd(bool enable){}
public:
	UIPhaseSubBase();
	virtual ~UIPhaseSubBase();

};

#endif
