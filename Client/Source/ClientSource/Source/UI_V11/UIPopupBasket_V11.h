#if !defined( __UI_POPUP_BASKET_H__)
#define __UI_POPUP_BASKET_H__

#include "UI/UIPopupBase.h"

#include "ShopContext.h"

class UITabShopInform;
class CShopBasket;

class UIPopupBasket_V11 : public UIPopupBase
{
	I3_CLASS_DEFINE(UIPopupBasket_V11, UIPopupBase);
	typedef UIPopupBase Base;

public:
	UIPopupBasket_V11();
	virtual ~UIPopupBasket_V11();

	virtual void	OnCreate( i3GameNode * pParent);
	virtual bool	OnEntranceStart(void * pArg1 /* = NULL */, void * pArg2 /* = NULL */);
	virtual void	_InitializeAtLoad(i3UIScene * pScene);
	virtual void	ProcessGameEvent(GAME_EVENT event, INT32 Arg, const i3::user_data& UserData);
	virtual void	OnUpdate( REAL32 rDeltaSeconds);

public:
	void			Confirm();
	void			NotifyFail(INT32);
	void			NotifySuccess();
	void			Cancel();
	void Click_Slot_X(INT32 index);
	void Change_Combo(INT32 index, INT32 tab);

	virtual bool	OnKey_Enter() { Confirm(); return true; }
	virtual bool	OnKey_Escape() { Cancel(); return true;  } 

	//-------------------------------  할인쿠폰 관련 -------------------------------------//
	void			OnClickCouponBtn(i3UIControl * pCtrl, INT32 nSlotIdx);
	void			UseDiscountCoupon(i3UIControl * pCtrl, INT32 nCouponIdx);
	void			RevertUseCoupon(INT32 nSlotIdx);
	void			ChangeCouponBtnTemplate(INT32 slot, bool bUseCoupon);
	
	SHOP_GOODS *	GetTargetGoods();

	void			SetCurrentSlotIdx(INT32 slotIdx) { m_nCurrentBasketSlotIdx = slotIdx ;}
	INT32			GetCurrentSlotIdx() const { return m_nCurrentBasketSlotIdx ;}
	//-------------------------------------------------------------------------------------//

private:
	void _updateList();
	void _updatePrice(UINT32 point, UINT32 cash,
		UINT32 pointPer, UINT32 cashPer);
	void _updateCoupon(UINT32 coupon);

	//-----------  할인쿠폰 관련 -----------//
	void _ClearUseCoupon();
	INT32 _GetDiscountPriceCash(INT32 nCash, INT64 nCouponDBIdx);
	//--------------------------------------//


	i3::vector< size_t > m_LuaIntAtt, m_LuaBoolAtt;
	i3::vector< i3::rc_wstring > m_LuaStringAtt;

	void _addLua(BOOL value)	{	m_LuaBoolAtt.push_back( value );	}
	void _addLua(size_t value)	{	m_LuaIntAtt.push_back( value );	}
	void _addLua(const i3::rc_wstring& value)	{	m_LuaStringAtt.push_back( value );	}
	void _toLua(const char* lua_name);

private:
	//bool			m_bWaitingConfirmSubscripting;
	CShopContext * m_pShopContext;
	CShopBasket  * m_pShopBasket;
	UITabShopInform* m_pParent;
	UINT32 m_TotalPointPrice; 
	UINT32 m_TotalCashPrice;

	bool					m_bFromExtension;

	INT32					m_nCurrentBasketSlotIdx;
};

#endif