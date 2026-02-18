#if !defined( _POPUP_CLAN_SUBSCRIBER_INFO_H__)
#define	_POPUP_CLAN_SUBSCRIBER_INFO_H__

#include "PopupBase.h"


class CPopupClanSubscriberInfo : public CPopupBase
{
	I3_CLASS_DEFINE( CPopupClanSubscriberInfo);

public:
	CPopupClanSubscriberInfo(void);
	virtual ~CPopupClanSubscriberInfo();

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void OnPreGuiNotify(void);
	virtual void OnExitKey(void);
	virtual void OnWheelUpdate(INT32 scrollby);
	virtual void OnGameEvent( INT32 event, INT32 arg);
	virtual void SetInputDisable(BOOL bDisable);
	virtual BOOL GetInputDisable(void);

	virtual void OnOpenPopup(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void OnClosePopup(void * pArg1 = NULL, void * pArg2 = NULL);

	void SetSubscriberReceiver(char * pszName);
	void SetSubscriberUser(char * pszName, UINT8 nRank);
	void SetSubscriberWin(UINT32 nKill, UINT32 nDeath);
	void SetSubscriberScore(UINT32 nMatch, UINT32 nWin, UINT32 nLose);
	void SetSubscriberMessage(char * pszMessage);

protected:

	CClanContext *	m_pClanContext;

	i3GuiStatic	*	m_pClanSubscriberInfoGui;

	i3GuiEditBox *	m_pClanSubscriberInfoCaption;

	i3GuiButton *	m_pClanSubscriberInfoPrev;
	i3GuiButton *	m_pClanSubscriberInfoNext;
	i3GuiButton *	m_pClanSubscriberInfoOk;
	
	i3GuiStatic *	m_pClanSubscriberInfoRank;
	i3GuiEditBox *	m_pClanSubscriberInfoReceiverStatic;
	i3GuiEditBox *	m_pClanSubscriberInfoReceiverValue;
	i3GuiEditBox *	m_pClanSubscriberInfoNickStatic;
	i3GuiEditBox *	m_pClanSubscriberInfoNickValue;
	i3GuiEditBox *	m_pClanSubscriberInfoWinStatic;
	i3GuiEditBox *	m_pClanSubscriberInfoWinValue;
	i3GuiEditBox *	m_pClanSubscriberInfoScoreStatic;
	i3GuiEditBox *	m_pClanSubscriberInfoScoreValue;
	i3GuiEditBox *	m_pClanSubscriberInfoMessageStatic;
	i3GuiEditBox *	m_pClanSubscriberInfoMessageValue;
	i3GuiEditBox *	m_pClanSubscriberInfoPageValue;

	INT32 m_nPage;

protected:
	void	OnPrevPage(void);
	void	OnNextPage(void);

	BOOL	IsMultiPage(void);
	INT32	GetCurrentPage(void)	{ return m_nPage; }
};

#endif	// #define	_POPUP_CLAN_SUBSCRIBER_INFO_H__