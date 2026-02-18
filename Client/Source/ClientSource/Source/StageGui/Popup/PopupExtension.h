#ifndef _PopupExtension_h
#define _PopupExtension_h

#include "PopupBase.h"

class CPopupExtension: public CPopupBase
{
	I3_CLASS_DEFINE(CPopupExtension);
public:
	CPopupExtension();
	virtual ~CPopupExtension();

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnOpenPopup(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void OnClosePopup(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void OnExitKey(void);

	static void SetWeaponFlag(INT32 nWeaponFlag);

	void OnLeave(void);

private:
	void _ShowWeaponGage(UINT32 nInfoType,UINT32 nRate);

private:
	i3GuiStatic* m_pExtensionGui;
	i3GuiStatic* m_pExtensionWin;
	i3GuiEditBox* m_pExtensionCaption;
	i3GuiButton* m_pLeave;

	i3GuiStatic* m_pImageDummy;
	i3GuiEditBox* m_pName;
	i3GuiEditBox* m_pCategory;
	
	i3GuiEditBox* m_pDamageKey;
	i3GuiEditBox* m_pRapidKey;
	i3GuiEditBox* m_pHitRateKey;
	i3GuiEditBox* m_pVertRecoilKey;
	i3GuiEditBox* m_pHorzRecoilKey;
	i3GuiEditBox* m_pSpeedKey;
	i3GuiEditBox* m_pRangeKey;
	i3GuiEditBox* m_pBulletKey;
	i3GuiEditBox* m_pDamageValue;
	i3GuiEditBox* m_pRapidValue;
	i3GuiEditBox* m_pHitRateValue;
	i3GuiEditBox* m_pVertRecoilValue;
	i3GuiEditBox* m_pHorzRecoilValue;
	i3GuiEditBox* m_pSpeedValue;
	i3GuiEditBox* m_pRangeValue;
	i3GuiEditBox* m_pBulletValue;
	i3GuiStatic* m_pDamageGage;
	i3GuiStatic* m_pRapidGage;
	i3GuiStatic* m_pHitRateGage;
	i3GuiStatic* m_pVertRecoilGage;
	i3GuiStatic* m_pHorzRecoilGage;
	i3GuiStatic* m_pSpeedGage;
	i3GuiEditBox* m_pDescription;

	i3GuiEditBox* m_pExtensionKey;
	i3GuiEditBox* m_pExtensionValue;
	i3GuiEditBox* m_pControlKey;
	i3GuiEditBox* m_pControlValue;
	i3GuiEditBox* m_pExtensionDescription;	
	i3GuiStatic* m_pExtensionDummy;
	i3GuiStatic * m_pExtensionControlSection;

	static INT32 m_nWeaponFlag;
};

#endif //_PopupExtension_h
