#if !defined( __UI_POPUP_DESIG_SELECT_V10_H__)
#define __UI_POPUP_DESIG_SELECT_V10_H__


#include "../UI/UIDefine.h"
#include "../UI/UIUtil.h"
#include "../UI/UIPopupBase.h"

class UIPopupDesigSelect_V10 : public UIPopupBase
{
	I3_CLASS_DEFINE(UIPopupDesigSelect_V10, UIPopupBase)

public:
	UIPopupDesigSelect_V10();
	virtual ~UIPopupDesigSelect_V10();

protected:
	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;
	virtual void	_ClearAtUnload(i3UIScene * pScene) override;
	
public:
	virtual void	OnCreate(i3GameNode * pParent) override;
	virtual void	ProcessGameEvent(GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;
	virtual bool	OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;
	
	virtual bool	OnEntranceStart(void * pArg1 = nullptr, void * pArg2 = nullptr) override;

private:
	void InitUI();
	void SetSlotInfo(INT32 Slotidx);

private:
	i3UIButtonImageSet* m_pBtnIcon[3];
	i3UIButtonImageSet* m_pImgLock[3];
	i3UIStaticText* m_pTitle[3];
	i3UIFrameWnd* m_pFmwSelected[3];

// LUA callback
public:
	void ClickOK();
	void SelectIcon(INT32 Slot); 

private:
	INT32 m_SelectedSlot;

};

#endif