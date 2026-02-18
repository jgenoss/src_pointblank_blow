#ifndef _StepShop_TabItem_h
#define _StepShop_TabItem_h

#include "StepShop_Tab.h"

////////////////////////////////////////////////////////////////////////
class	CTabItem	:
	public	CTabBase
{
public:
	CTabItem(CStepShop * pOwner = NULL);
	virtual	~CTabItem();

	virtual	void	Init();
	virtual	BOOL	OnOpen();
	virtual	void	OnClose();
	virtual	void	OnUpdate(RT_REAL32 rDeltaSec);
	virtual	void	OnFocus(UINT32 idxSlot);
	virtual	void	OnPreGuiNotify();
	virtual	void	OnLinkControl(i3BinList * plist);
	virtual	void	OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);


private:
	void	_ChangeCashItemUsage(ITEM_TYPE nItemUsage);

	void	_UpdateSlotList(void);
	void	_UpdateDesc(void);

	void	MakeGroupList();

	i3GuiButton *	GetGroupButton(ITEM_TYPE GroupUsage);

public:
	CashItemSlot	m_Slot[MAX_SLOT_FOR_SHOP_ITEM];

	i3GuiButton* m_pAllGroup;
	i3GuiButton* m_pExpendableGroup;
	i3GuiButton* m_pMaintenanceGroup;
	
	i3GuiStatic* m_pInfo;
	i3GuiStatic* m_pInfoDummy;
	i3GuiEditBox* m_pInfoValue;
	i3GuiEditBox* m_pInfoDesc;

	ITEM_TYPE m_nCashItemUsageType;

};

#endif