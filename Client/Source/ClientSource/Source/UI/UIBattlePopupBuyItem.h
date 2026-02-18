#pragma once

#include "UIBattlePopupBase.h"
#include "UIBattlePopupBuyItemSlot.h"

namespace buy
{
	enum PurchaseRouteType;
}

namespace i3 { template<class> class unique_signal; }

class UIBattlePopupBuyItem : public UIBattlePopupBase
{
	I3_CLASS_DEFINE( UIBattlePopupBuyItem, UIBattlePopupBase);
public:
	UIBattlePopupBuyItem();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

	/*
	param[in] pArg1 -> UINT32
	*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd(void) override;
	virtual void	OnLoadAllScenes() override;

	virtual void	OnUnloadAllScenes() override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code) override;

	virtual bool OnKey_Enter( void) override;

public:
	void clicked(INT32 idx);
	void update_item_paymenu();

	buy::PurchaseRouteType getPurchaseRoute() const { return m_purchase_route_type; }

	void ClosePopup() const;

	static void cb_close_duplicate_popup(void* pThis,INT32 nParam);

private:
	bool check_enable_buy(i3::rc_wstring& out_fail_reason);



	//-------------------------------  할인쿠폰 관련 -------------------------------------//
	//	void			OnClickCouponBtn(i3UIControl * pCtrl, INT32 nSlotIdx);
	//	void			UseDiscountCoupon(i3UIControl * pCtrl, INT32 nCouponIdx);
	void			RevertUseCoupon(INT32 nSlotIdx);
	void			ChangeCouponBtnTemplate(INT32 slot, bool bUseCoupon);

	//	SHOP_GOODS *	GetTargetGoods();

	//	void			SetCurrentSlotIdx(INT32 slotIdx) { m_nCurrentBasketSlotIdx = slotIdx ;}
	//	INT32			GetCurrentSlotIdx() const { return m_nCurrentBasketSlotIdx ;}
	//-------------------------------------------------------------------------------------//


private:
	UIBattlePopupBuyItemSlot mSlot;
	UINT32 m_TotalPointPrice; 
	UINT32 m_TotalCashPrice;
	buy::PurchaseRouteType m_purchase_route_type;
//	std::string m_cb_close;
	i3::unique_signal<void(bool, bool)>*  m_pSigClose;

};
