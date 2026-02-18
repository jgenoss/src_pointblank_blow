#ifndef __POPUP_SHOP_GIFT_H
#define __POPUP_SHOP_GIFT_H

#include "PopupBase.h"
#include "ShopContext.h"

#define GIFT_POPUP_ELE_COUNT	8

class CPopupShopGift : public CPopupBase
{
	I3_CLASS_DEFINE(CPopupShopGift);

public:
	CPopupShopGift(void);
	virtual ~CPopupShopGift(void);

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void OnPreGuiNotify(void);
	virtual void OnExitKey(void);
	virtual void OnWheelUpdate(INT32 scrollby);
	virtual BOOL OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);
	virtual void OnGameEvent( INT32 event, INT32 arg);

	virtual void OnOpenPopup(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void OnClosePopup(void * pArg1 = NULL, void * pArg2 = NULL);

protected:
	SHOP_GOODS	m_Goods;

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

	i3GuiEditBox * m_pStaticPoint[3];
	i3GuiEditBox * m_pStaticHavePoint;
	i3GuiEditBox * m_pStaticRemainPoint;
	i3GuiEditBox * m_pValueBuyPoint;
	i3GuiEditBox * m_pValueHavePoint;
	i3GuiEditBox * m_pValueRemainPoint;

	i3GuiStatic *	m_pCashSymbol;			// 한국 N코인 심볼 이미지
	i3GuiStatic	*	m_pHaveCashSymbol;		// 한국 N코인 심볼 이미지
	i3GuiStatic *	m_pRemainCashSymbol;	// 한국 N코인 심볼 이미지	

	i3GuiEditBox * m_pStaticReceiverName;
	i3GuiEditBox * m_pStaticReceiverSelect;
	i3GuiEditBox * m_pReceiverInput;
	i3GuiButton * m_pComboFriend;
	i3GuiButton * m_pComboClan;
	i3GuiList * m_pFriendScroll;
	i3GuiStatic * m_pFriendDropDown;
	i3GuiList * m_pClanScroll;
	i3GuiStatic * m_pClanDropDown;

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

	INT32 m_idxFriendPopupStartSlot;
	INT32 m_idxClanPopupStartSlot;

	char m_szReceiverNick[MAX_STRING_COUNT];

	BOOL	m_WaitAuthBuy;	// 구입 인증을 대기하는지

	void _InputConfirm(void);
	void _InputCancel(void);
	void _InputCashCharge(void);
	void _ComboFriend(void);
	void _ComboClan(void);
	void _SetWeaponInfo(SHOP_GOODS * pGoods);
	void _SetCharacterInfo(SHOP_GOODS * pGoods);
	void _SetItemInfo(SHOP_GOODS * pGoods);
	void _SetPackageGoodsInfo(SHOP_GOODS* pGoods);
	void _SetPointInfo(SHOP_GOODS * pGoods);
	void _SelectFriend(void);
	void _SelectClanMember(void);
	void _SetWaitAuthBuy(BOOL Wait);
};

#endif	//	__POPUP_SHOP_GIFT_H