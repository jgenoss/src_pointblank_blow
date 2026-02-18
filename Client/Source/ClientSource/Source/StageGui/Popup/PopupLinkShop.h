
#ifndef __POPUP_LINKSHOP_H__
#define __POPUP_LINKSHOP_H__

#include "PopupBase.h"


class CPopupLinkShop : public CPopupBase
{
	I3_CLASS_DEFINE(CPopupLinkShop);

public:
	CPopupLinkShop(void);
	virtual ~CPopupLinkShop();

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnExitKey(void);
	virtual void OnOpenPopup(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void OnClosePopup(void * pArg1 = NULL, void * pArg2 = NULL);

	virtual	BOOL	OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);

private:
	
	i3GuiStatic* m_pCheckPrimaryWeapon;
	i3GuiStatic* m_pSelectLinkShop;
	i3GuiStatic* m_pBuyPrimaryWeapon;
	
	i3GuiButton* m_pBuyPrimaryWeaponConfirm;
	i3GuiButton* m_pCheckPrimaryWeaponConfirm;
	i3GuiButton* m_pSelectLinkShopConfirm;
	i3GuiButton* m_pSelectLinkShopCancle;

	i3GuiEditBox* m_pCheckPrimaryWeaponMessage;//주무기가 장비되지 않아 상점으로 이동합니다.
	i3GuiEditBox* m_pBuyPrimaryWeaponMessage;  //주무기를 구입후 장비하세요
	i3GuiEditBox* m_pSelectLinkShopMessage;    //주무기가 장비되지 않았습니다. 상점으로 이동하시겠습니까?
	i3GuiEditBox* m_pCheckPrimaryWeaponCaption;
	i3GuiEditBox* m_pBuyPrimaryWeaponCaption;  
	i3GuiEditBox* m_pSelectLinkShopCaption;  

	void _CheckPrimaryWeaponConfirm(void);
	void _BuyPrimaryWeaponConfirm(void);

	void _SelectLinkShopConfirm(void);
	void _SelectLinkShopCancle(void);


};

#endif	//	