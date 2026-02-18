#ifndef _StepShop_h
#define _StepShop_h

#include "StepBase.h"
#include "StepInventory.h"
#include "StageDef.h"
#include "ShopDef.h"
#include "ShopContext.h"
#include "../../CommonSource/WeaponDefine.h"

#include "StepShop_Tab.h"
#include "StepShop_TabWeapon.h"
#include "StepShop_TabChara.h"
#include "StepShop_TabItem.h"
#include "StepShop_TabSet.h"

#include "GuiButtonEffector.h"

enum	SHOP_BUY_TYPE
{
	SHOP_BUY_TYPE_POINT,
	SHOP_BUY_TYPE_CASH,
};




class CStepShop: public CStepBase
{
	I3_CLASS_DEFINE(CStepShop);
public:
	CStepShop();
	virtual ~CStepShop();

	virtual void OnPreGuiNotify(void);	
	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void OnExitKey(void);
	virtual void OnWheelUpdate(INT32 scrollby);	
	virtual void SetInputDisable(BOOL bDisable);
	virtual BOOL GetInputDisable(void);

	virtual void OnStart(void);
	virtual void OnEnd(void);
	virtual void OnInitRun(void);

	virtual void OnGameEvent(INT32 event,INT32 arg);
	virtual BOOL OnSliding(RT_REAL32 rDeltaSeconds);
	virtual void AuthStartSuccess(INT32 arg);

	void EnterShop(void);
	void LeaveShop(void);

public:
	void OnGoodsBuy(UINT32 idxSlot, BOOL bDoubleClicked = FALSE);
	void OnGoodsGift(UINT32 idxSlot);
	void TogglePopupExtension();
	void _GetGoodsUseDay(BOOL ExpendableItem, UINT8 itemAttr, UINT32 itemArg, char * pOut, const size_t sizeOutMax);
	void _PrintSaleText(i3GuiEditBox* pSaleTextCtrl[SALE_TEXT_MAX], UINT32 BuyYupe);
	
	void SetEffectButton(i3GuiButton * pButton);

	BOOL CheckVaildGoods( SHOP_GOODS * pGoods);

protected:
	void OnLeave(void);	
	void OnCashCharge(void);


private:
	BOOL	_CheckBuyingGoods(SHOP_GOODS * pGoods, BOOL bGift = FALSE, BOOL bDoubleClicked = FALSE);
	void	ChangeTab(CTabBase * pNewTab, bool ResetVestige = false);



private:
	CShopContext *	m_pShopContext;

	i3GuiStatic *	m_pShopGui;
	i3GuiStatic *	m_pShopWin;
	i3GuiEditBox *	m_pShopCaption;	
	i3GuiButton *	m_pLeave;
	i3GuiButton *	m_pCashCharge;

	CGuiButtonEffector	m_BtnEffector;

	CTabBase *		m_pCurrentTab;
	CTabWeapon		m_TabWeapon;
	CTabChara		m_TabChara;
	CTabItem		m_TabItem;
	CTabPackage		m_TabPackage;


};

#endif //_StepShop_h
