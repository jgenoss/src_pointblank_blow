#ifndef __POPUP_CLAN_SEND_NOTE_H
#define __POPUP_CLAN_SEND_NOTE_H

#include "PopupBase.h"
#include "ClanContext.h"

class CPopupClanSendNote : public CPopupBase
{
	I3_CLASS_DEFINE(CPopupClanSendNote);

public:
	CPopupClanSendNote(void);
virtual ~CPopupClanSendNote(void);

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void OnPreGuiNotify(void);
	virtual void OnGameEvent( INT32 event, INT32 arg);
	virtual void OnExitKey(void);
	virtual BOOL OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);
	virtual void OnWheelUpdate(INT32 scrollby);

	virtual void OnOpenPopup(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void OnClosePopup(void * pArg1 = NULL, void * pArg2 = NULL);

	void OnSending(void);
	void OnClear(void);
	void OnTypeComboButton(void);
	void OnTypeComboPopup(INT32 idx);

	BOOL SendNoteToServer(void);
	void FocusToEdit(void)				{ m_pSendNoteEdit->SetFocus(TRUE);				}

	static void CbNoteSending(void * pThis, INT32 nParam);
	static void CbNoMessage(void * pThis, INT32 nParam);

protected:

	i3GuiStatic * m_pGuiRoot;
	i3GuiEditBox * m_pRootCaption;
	i3GuiButton * m_pButtonSending;
	i3GuiButton * m_pButtonClear;
	i3GuiButton * m_pButtonClose;

	i3GuiEditBox * m_pSendNoteEdit;
	i3GuiEditBox * m_pSendTypeStatic;
	i3GuiEditBox * m_pSendTypeCombo;
	i3GuiButton * m_pSendTypeButton;
	i3GuiPopupList * m_pSendTypePopup;

	INT32 m_nTypeComboIdx;
};

#endif	//	__POPUP_CLAN_SEND_NOTE_H