#if !defined( __UI_POPUP_BUYITEM_H__)
#define __UI_POPUP_BUYITEM_H__

#include "UI/UIPopupBase.h"

#if defined( ENABLE_UI2)

enum
{
	MAX_POPUP_BUY_RADIO_COUNT = 4
};

class UIPopupBuyItem_V11 : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupBuyItem_V11, UIPopupBase);
	typedef UIPopupBase		base;

protected:
	virtual void	OnLoadAllScenes();
	virtual void	OnUnloadAllScenes();

public:
	UIPopupBuyItem_V11();
	virtual ~UIPopupBuyItem_V11();

	virtual void	OnCreate( i3GameNode * pParent);
	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void	OnExitEnd(void);		// 팝업과 팝업사이의 열고닫음시 포커스 문제가 있어서 OnExitStart()대신 OnExitEnd()로 구현을 옮기겠습니다.(11.08.01.수빈)

	void			OnRadioChecked_Date(INT32 radioIdx);
	void			OnCashCharge();
	
	void			UpdateInfoByTabChange(INT32 radioIdx);

	void			OnClickCouponBtn(i3UIControl * pCtrl);
	void			UseDiscountCoupon(i3UIControl * pCtrl, INT32 nCouponIdx);
	void			RevertUseCoupon();

	void			SendBuyEvent();
	void			InputConfirm(void);
	SHOP_GOODS*		GetBuyGoods()  { return &m_copy_goods; }
		//m_pSel_GoodsPack->GetGoods(m_aiRadioMap[m_iRadioIndex]); }

	SHOP_GOODS*		GetTargetGoods() const { return m_pSel_GoodsPack->GetGoods(m_aiRadioMap[m_iRadioIndex]);}
	
	void			ClickPriceTab(INT32 tab_idx);
	void			ChangeShopGoodsPack(SHOP_GOODS_PACK* goods_pack);
	
	const INT32		GetCouponIdx() const { return m_nCouponIndex; }
	const bool		IsCashItem();

private:
	void			_Update_Shop_Purchase_Info();
	void			_SetCouponEnable();

	SHOP_GOODS_PACK*		m_pSel_GoodsPack;	
	SHOP_GOODS				m_copy_goods;			// 구매할때만 쓴다...

	class UIImgSetCtrl*		m_pImageSet;

	INT32					m_iRadioIndex;

	GOODS_PRICE_TYPE		m_buy_mtd;
	INT32					m_numRadio;
	INT32					m_aiRadioMap[MAX_POPUP_BUY_RADIO_COUNT];

	bool					m_bSucess;
	bool					m_bFromExtension;

	INT32					m_nCouponIndex;
};

#endif	// ENABLE_UI2

#endif
