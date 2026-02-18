#ifndef _StepShop_TabWeapon_h
#define _StepShop_TabWeapon_h

#include "StepShop_Tab.h"



class	CRepairFrameWeapon	:
	public	CRepairFrame
{
public:
	CRepairFrameWeapon(CTabBase * pOwner);

public:
	virtual	void	OnLinkControl(i3BinList * plist);
	virtual	void	Init();

	virtual	void	Update(RT_REAL32 rDeltaSec);
};



class	CTabWeapon	:
	public	CTabBase
{
public:
	CTabWeapon(CStepShop * pOwner = NULL);
	virtual	~CTabWeapon();

	virtual	void	Init();
	virtual	BOOL	OnOpen();
	virtual	void	OnClose();
	virtual	void	OnUpdate(RT_REAL32 rDeltaSec);
	virtual	void	OnFocus(UINT32 idxSlot);
	virtual	BOOL	OnSelectSlot(UINT32 idxSlot, BOOL ForceSelect = FALSE);
	virtual	void	OnSelectGoodsRadio(UINT32 RadioIndex = 0);
	virtual	void	OnBuyGoodsSuccess();
	virtual	void	OnPreGuiNotify();
	virtual	void	OnLinkControl(i3BinList * plist);
	virtual	void	OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);


private:
	void	OnChangeGroup(WEAPON_SLOT_TYPE nWeaponUsage, bool ResetVestige = false);
	void	OnWeaponDetail(void);
	void	OnWeaponPreview(void);
	void	OnRepair(void);

	void	_UpdateSlotList(void);
	void	_UpdateDesc(void);
	void	_UpdateBuyType(void);
	void	_CheckUpdateGoodsFrame(RT_REAL32 rDeltaSec);

	void	_ShowWeaponGage(UINT32 nInfoType,UINT32 nRate);

	void	_SetWeaponBuyType(INT32 nType);

	void	MakeGroupList();

	i3GuiButton *	GetGroupButton(WEAPON_SLOT_TYPE GroupUsage);


private:
	WeaponSlot		m_Slot[MAX_SLOT_FOR_SHOP_WEAPON];

	enum {MAX_SHOP_WEAPON_INFO = MAX_WEAPON_INFO};
	WeaponInfo		m_weaponInfo[MAX_SHOP_WEAPON_INFO];

	i3GuiButton* 	m_pAllGroup;
	i3GuiButton* 	m_pPrimaryGroup;
	i3GuiButton* 	m_pSecondaryGroup;
	i3GuiButton* 	m_pMeleeGroup;
	i3GuiButton* 	m_pThrowingGroup;
	i3GuiButton* 	m_pItemGroup;

	i3GuiButton* 	m_pDetailWeapon;
	i3GuiButton* 	m_pPreviewWeapon;
	
	i3GuiEditBox* 	m_pBulletKey;
	i3GuiEditBox* 	m_pBulletValue;

	i3GuiEditBox* 	m_pRangeKey;
	i3GuiEditBox* 	m_pRangeValue;

	i3GuiStatic* 	m_pInfo;
	i3GuiStatic* 	m_pImage;
	i3GuiEditBox* 	m_pInfoKey;
	i3GuiEditBox* 	m_pInfoValue;
	i3GuiEditBox* 	m_pInfoTerm;
	i3GuiEditBox* 	m_pInfoPurchase;
	i3GuiStatic* 	m_pInfoExtension;

	i3GuiStatic* 	m_pGoodsFrame;
	i3GuiButton* 	m_pBuyWeaponCombo1;		// Æ÷ÀÎÆ® ÅÇ
	i3GuiButton* 	m_pBuyWeaponCombo2;		// Ä³½¬ ÅÇ

	CRepairFrameWeapon 	m_RepairFrame;

	WEAPON_SLOT_TYPE	m_nWeaponUsageType;
	UINT32				m_nCurViewWeaponID;

protected:
	WEAPON_SLOT_TYPE _GetFirstGroup();
	UINT32 _GetFirstSelectItem();
	void _ApplyScroll();
};


#endif
