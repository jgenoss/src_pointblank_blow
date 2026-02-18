#ifndef _StepShop_TabChara_h
#define _StepShop_TabChara_h

#include "StepShop_Tab.h"

////////////////////////////////////////////////////////////////////////
class	CTabChara	:
	public	CTabBase
{
public:
	CTabChara(CStepShop * pOwner = NULL);
	virtual	~CTabChara();

	virtual	void	Init();
	virtual	BOOL	OnOpen();
	virtual	void	OnClose();
	virtual	void	OnUpdate(RT_REAL32 rDeltaSec);
	virtual	void	OnFocus(UINT32 idxSlot);
	virtual	void	OnPreGuiNotify();
	virtual	void	OnLinkControl(i3BinList * plist);
	virtual	void	OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);


private:
	void	OnCharacterTypePopup(UINT32 idxItem);

	void	_UpdateSlotList(void);
	void	_UpdateDesc(void);

	void	OnCharacterTypeButton(void);
	void	_ChangeCharacterClassType(CHARACTER_CLASS_TYPE nClassType);

	void	MakeGroupList();

	i3GuiButton *	GetGroupButton(CHARACTER_CLASS_TYPE GroupUsage);

public:
	CharacterSlot m_Slot[MAX_SLOT_FOR_SHOP_CHARACTER];

	i3GuiButton* m_pAllGroup;
	i3GuiButton* m_pTypeGroup;
	i3GuiButton* m_pHeadGroup;
	
	i3GuiStatic* m_pInfo;
	i3GuiStatic* m_pInfoDummy;
	i3GuiEditBox* m_pInfoValue;
	i3GuiEditBox* m_pTermKey;
	i3GuiEditBox* m_pTermValue;
	i3GuiEditBox* m_pTypeCombo;
	i3GuiButton* m_pTypeButton;
	i3GuiPopupList* m_pTypePopup;
	i3GuiEditBox* m_pDescription;

	CHARACTER_CLASS_TYPE	m_nClassType;

	//// 1.5 Check CharaTeam
	//INT32					m_nFilter;

};


#endif