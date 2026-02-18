#if !defined( __UI_POPUP_RESULT_USEITEM_H__)
#define __UI_POPUP_RESULT_USEITEM_H__

#include "UIPopupItemBase.h"

class UIPopupResultUseItem : public UIPopupItemBase
{
	I3_CLASS_DEFINE( UIPopupResultUseItem, UIPopupItemBase);

private:
	i3UIButton* m_pFireEffect;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual LuaState * _SetItemInfo( void) override;

public:
	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnKey_Enter() override { _OnClose(); return true; }

	/*
	param[in] pArg1 T_ItemID*
	*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual bool	OnExitStart( void) override;
};

#endif
