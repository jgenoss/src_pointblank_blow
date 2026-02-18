#if !defined( __UI_POPUP_BUYITEM_H__)
#define __UI_POPUP_BUYITEM_H__

#include "UIPopupBase.h"
#include "UIPopupBuyItemSlot.h"

namespace buy
{
	enum PurchaseRouteType;
}

namespace i3 { template<class> class unique_signal; }

class UIPopupBuyItem : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupBuyItem, UIPopupBase);

public:
	UIPopupBuyItem();
	virtual ~UIPopupBuyItem();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

	/*
	param[in] pArg1 (buy::type)(UINT32)
	param[in] pArg2 const char*
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
	void process_buy_item(void);
	void set_notice_message(void);

	buy::PurchaseRouteType getPurchaseRoute() const { return m_purchase_route_type; }

	void ClosePopup() const;

	static void cb_close_duplicate_popup(void* pThis,INT32 nParam);

private:
	bool check_enable_buy(i3::rc_wstring& out_wstr_fail_reason);
	bool check_enable_buy_Gacha(i3::rc_wstring & out_wstr_fail_reason);
	bool check_enable_buy_gift(i3::rc_wstring & out_wstr_fail_reason);


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
	ShopPopupBuyItemSlot mSlot;
	UINT32 m_TotalPointPrice; 
	UINT32 m_TotalCashPrice;
	buy::PurchaseRouteType m_purchase_route_type;

	i3::unique_signal<void(bool,bool)>* m_pSigClose = nullptr;
	//std::string m_cb_close;
	//UIFUNCTYPE	m_cb_close;		// close시 호출되는 callback type

private:
	i3UIButtonComposed3 * buyBtn;
};

#endif
