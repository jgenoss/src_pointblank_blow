#ifndef _StepShop_TabSet_h
#define _StepShop_TabSet_h

#include "StepShop_Tab.h"

////////////////////////////////////////////////////////////////////////
class	CTabPackage	:
	public	CTabBase
{
public:
	CTabPackage(CStepShop * pOwner = NULL);
	virtual	~CTabPackage();

	virtual	void	Init();
	virtual	BOOL	OnOpen();
	virtual	void	OnClose();
	virtual	void	OnUpdate(RT_REAL32 rDeltaSec);
	virtual	void	OnFocus(UINT32 idxSlot);
	virtual	void	OnLinkControl(i3BinList * plist);
	virtual	void	OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);


private:
	void	OnPackageGoodsAllGroup(void);

	void	_UpdateSlotList(void);
	void	_UpdateDesc(void);

	void	MakeGroupList();
	virtual	BOOL	OnUpdateGoodsFrameText(SHOP_GOODS * pGoods, INT32 GoodsIndex);


private:
	PackageGoodsSlot m_Slot[MAX_SLOT_FOR_SHOP_PACKAGE];

	i3GuiButton* m_pAllGroup;
	i3GuiStatic* m_pInfo;
	i3GuiStatic* m_pInfoDummy;
	i3GuiEditBox* m_pInfoValue;
	i3GuiEditBox* m_pInfoDesc;
};


#endif