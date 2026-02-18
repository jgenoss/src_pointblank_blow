#ifndef __POPUP_REPAIR_H
#define __POPUP_REPAIR_H

#include "PopupBase.h"

#if !defined( ENABLE_UI2)

class CPopupRepair : public CPopupBase
{
	I3_CLASS_DEFINE(CPopupRepair);

public:
	CPopupRepair(void);
	virtual ~CPopupRepair(void);

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void OnExitKey(void);
	virtual void OnGameEvent( INT32 event, INT32 arg);

	virtual void OnOpenPopup(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void OnClosePopup(void * pArg1 = NULL, void * pArg2 = NULL);

	void			SendBuyEvent();


protected:
	INT32			m_ItemID;
	BOOL			m_CanRepair;

	i3GuiStatic * m_pGui;
	i3GuiStatic * m_pMainForm;
	i3GuiEditBox * m_pMainCaption;
	i3GuiButton * m_pButtonOk;
	i3GuiButton * m_pButtonCancel;

	i3GuiButton * m_pButtonCashCharge;
	i3GuiStatic * m_pButtonCashChargeMark;
	i3GuiEditBox * m_pButtonCashChargeStatic;

	i3GuiEditBox * m_pText;

	i3GuiStatic * m_pWeaponInfoFrame;
	i3GuiEditBox * m_pWeaponInfoValue;
	i3GuiEditBox * m_pWeaponInfoKey;
	i3GuiStatic * m_pWeaponInfoImage;
	i3GuiStatic * m_pWeaponInfoExtension;


	BOOL	m_bConfirm;
	REAL32	m_fTimeout;



	void _InputCashCharge(void);

	BOOL _AuthGoods(SHOP_GOODS * pGoods);

};

#endif // ENABLE_UI2

#endif	//	__POPUP_REPAIR_H