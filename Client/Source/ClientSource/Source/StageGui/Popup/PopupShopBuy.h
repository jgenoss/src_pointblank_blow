#ifndef __POPUP_SHOP_BUY_H
#define __POPUP_SHOP_BUY_H

#if !defined( ENABLE_UI2)

#include "PopupBase.h"
#include "ShopContext.h"
#include "ShopDef.h"
#include "GuiButtonEffector.h"

#define			STATIC_POINT_COUNT			3

class CPopupShopBuy : public CPopupBase
{
	I3_CLASS_DEFINE(CPopupShopBuy);

public:
	CPopupShopBuy(void);
	virtual ~CPopupShopBuy(void);

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void OnPreGuiNotify(void);
	virtual void OnExitKey(void);

	virtual void OnOpenPopup(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void OnClosePopup(void * pArg1 = NULL, void * pArg2 = NULL);


	SHOP_GOODS *	GetBuyGoods(void)		{ return &m_BuyGoods;			}
	void			SendBuyEvent(SHOP_GOODS * pBuyGoods);


	//	call back
	static void CbBuyForHavingGoods(void * pThis, INT32 nParam);
	static void CbAuthGoods(void* pThis, INT32 nParam);
	static void CbEquipGoods(void* pThis, INT32 nParam);

protected:

	SHOP_GOODS	m_BuyGoods;

	i3GuiStatic * m_pGui;
	i3GuiStatic * m_pMainForm;
	i3GuiEditBox * m_pMainCaption;
	i3GuiButton * m_pButtonOk;
	i3GuiButton * m_pButtonCancel;

	i3GuiButton * m_pButtonCashOk;
	i3GuiButton * m_pButtonCashCancel;
	i3GuiButton * m_pButtonCashCharge;
	i3GuiStatic * m_pButtonCashChargeMark;
	i3GuiEditBox * m_pButtonCashChargeStatic;

	i3GuiEditBox * m_pSaleText;

	i3GuiEditBox * m_pStaticPoint[STATIC_POINT_COUNT];
	i3GuiEditBox * m_pStaticHavePoint;
	i3GuiEditBox * m_pStaticRemainPoint;
	i3GuiEditBox * m_pValueBuyPoint;
	i3GuiEditBox * m_pValueHavePoint;
	i3GuiEditBox * m_pValueRemainPoint;

	i3GuiStatic *	m_pCashSymbol;			// 한국 N코인 심볼 이미지
	i3GuiStatic	*	m_pHaveCashSymbol;		// 한국 N코인 심볼 이미지
	i3GuiStatic *	m_pRemainCashSymbol;	// 한국 N코인 심볼 이미지	

	i3GuiStatic * m_pWeaponInfoFrame;
	i3GuiEditBox * m_pWeaponInfoValue;
	i3GuiEditBox * m_pWeaponInfoKey;
	i3GuiStatic * m_pWeaponInfoImage;
	i3GuiStatic * m_pWeaponInfoExtension;
	i3GuiStatic * m_pWeaponInfoUsed;

	i3GuiStatic * m_pCharacterInfoFrame;
	i3GuiEditBox * m_pCharacterInfoValue;
	i3GuiStatic * m_pCharacterInfoImage;
	i3GuiStatic * m_pCharacterInfoUsed;

	i3GuiStatic * m_pItemInfoFrame;
	i3GuiEditBox * m_pItemInfoValue;
	i3GuiStatic * m_pItemInfoImage;
	i3GuiStatic * m_pItemInfoUsed;

	BOOL	m_bConfirm;
	REAL32	m_fTimeout;


	void _InputConfirm(void);
	void _InputCancel(void);
	void _InputCashCharge(void);
	void _SetWeaponInfo(SHOP_GOODS * pGoods);
	void _SetCharacterInfo(SHOP_GOODS * pGoods);
	void _SetItemInfo(SHOP_GOODS * pGoods);
	void _SetPackageGoodsInfo(SHOP_GOODS* pGoods);
	void _SetPointInfo(SHOP_GOODS * pGoods);
	BOOL _AuthGoods(SHOP_GOODS * pGoods);
	void _SetButtonState(GUI_CONTROL_STATE State);



	enum EFFECT_BUTTON {EFFECT_BTN_OK, EFFECT_BTN_CASH, MAX_EFFECT_BTN};
	CGuiButtonEffector	m_BtnEffector[MAX_EFFECT_BTN];
};

#endif	// ENABLE_UI2

#endif	//	__POPUP_SHOP_BUY_H