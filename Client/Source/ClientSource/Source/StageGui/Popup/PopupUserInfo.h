
#ifndef __POPUP_USER_INFO_H
#define __POPUP_USER_INFO_H

#include "PopupBase.h"


class CPopupUserInfo : public CPopupBase
{
	I3_CLASS_DEFINE( CPopupUserInfo);

public:
	CPopupUserInfo(void);
	virtual ~CPopupUserInfo(void);

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void OnPreGuiNotify(void);
	virtual void OnExitKey(void);
	virtual void OnGameEvent( INT32 event, INT32 arg);

	virtual void OnOpenPopup(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void OnClosePopup(void * pArg1 = NULL, void * pArg2 = NULL);

	void OnAddFriend(void);

protected:
	i3GuiStatic * m_pRoot;

	i3GuiEditBox * m_pCaption;

	i3GuiEditBox * m_pNicknameKey;
	i3GuiEditBox * m_pNicknameValue;

	i3GuiEditBox * m_pSeasonRecordKey;
	i3GuiEditBox * m_pSeasonRecordValue;
	i3GuiEditBox * m_pSeasonKillDeath;
	i3GuiEditBox * m_pSeasonHeadshot;

	i3GuiEditBox * m_pTotalRecordKey;
	i3GuiEditBox * m_pTotalRecordValue;
	i3GuiEditBox * m_pTotalKillDeath;
	i3GuiEditBox * m_pTotalHeadshot;

	i3GuiEditBox * m_pPositionKey;
	i3GuiEditBox * m_pPositionValue;

	i3GuiButton * m_pButton[2];
	i3GuiButton * m_pClose;

	LOBBY_USER_INFO_LIST m_CurrentUser;
	BOOL m_bDone;

protected:
	void _SetRecord( const USER_INFO_RECORD * pRecord);
	
};

#endif
