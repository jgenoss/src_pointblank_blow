#ifndef __POPUP_RANDOMMAP_H__
#define __POPUP_RANDOMMAP_H__

#include "PopupBase.h"
#include "GameContext.h"
#include "RandomMap.h"
//·£´ý¸Ê 
#define COUNT_CHECKBOX_MAX			RANDOMMAP_MAX_COUNT

class CPopupRandomMap : public CPopupBase
{
	I3_CLASS_DEFINE(CPopupRandomMap);

public:
	CPopupRandomMap(void);
	virtual ~CPopupRandomMap();

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnExitKey(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void OnOpenPopup(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void OnClosePopup(void * pArg1 = NULL, void * pArg2 = NULL);

	virtual	BOOL	OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);

private:
	
	i3GuiStatic* m_pRandomMapWin;
	i3GuiStatic* m_pCheckCountWin;
	i3GuiEditBox* m_pRandomMapCaption;
	i3GuiButton* m_pRandomMapConfirm;
	i3GuiButton* m_pCheckCountConfirm;
	i3GuiButton* m_pRandomMapCancel;
	i3GuiEditBox* m_pCheckCountEditBox;

	i3GuiCheckBox* m_pMapCheck[COUNT_CHECKBOX_MAX];
	i3GuiEditBox* m_pMapName[COUNT_CHECKBOX_MAX];

	INT32 m_IndexMap;
	INT32 m_IndexCheck;
	void _ChallengeMatchCheckBox();
	void _DeathMatchCheckBox();
	void _BossCheckBox(void);
	void _BombCheckBox(void);
	void _DestroyCheckBox(void);
	void _OnRandomMapConfirm(void);
	void _OnRandomMapCancel(void);
	void _RunRandomMap();
	void _CountCheck();
	void _InitCheckState();

	BOOL m_bPrevRandom[COUNT_CHECKBOX_MAX];
	//INT32 BombMatchList;
	//INT32 DeathMatchList;
	//INT32 DestroyMatchList;

	STAGE_TYPE	m_CurGameMode;
};

#endif	//	__POPUP_RANDOMMAP_H