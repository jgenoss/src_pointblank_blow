
#ifndef __POPUP_USER_DETAIL_INFO_H
#define __POPUP_USER_DETAIL_INFO_H

#include "PopupBase.h"


class CPopupUserDetailInfo : public CPopupBase
{
	I3_CLASS_DEFINE( CPopupUserDetailInfo);

public:
	CPopupUserDetailInfo(void);
	virtual ~CPopupUserDetailInfo(void);

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void OnPreGuiNotify(void);
	virtual void OnExitKey(void);
	virtual	void OnModeTab(BOOL bModeFlag);
	virtual void OnTypeComboButton(void);
	virtual void OnTypeComboPopup(INT32 idx);

	virtual void OnGameEvent( INT32 event, INT32 arg);

	virtual void OnOpenPopup(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void OnClosePopup(void * pArg1 = NULL, void * pArg2 = NULL);

	void OnUserDetailInfo(void);
	void OnUserDetailInfoPupupMsgCancel(void);
	void OnUserDetailInfoPupupMsgConfirm(void);

	void OnAddFriend(void);

protected:
	i3GuiStatic	 *	m_pRoot;

	i3GuiEditBox *	m_pCaption;

	// UserDetailInfoPopupFrame
	// User Info
	i3GuiStatic	 *	m_pRankMark;
	i3GuiEditBox *	m_pNicknameValue;
	i3GuiEditBox *	m_pRank;
	i3GuiEditBox *	m_pClanName;	
	i3GuiEditBox *	m_pTitle;	
	i3GuiEditBox *	m_pPoint;
	i3GuiEditBox *	m_pToll;

	i3GuiEditBox *	m_pPositionKey;
	i3GuiEditBox *	m_pPositionValue;

	// UserDetailInfoPopupFrame2
	// Total Record
	i3GuiEditBox * m_pTotalRecordKey;
	i3GuiEditBox * m_pTotalRecord;
	i3GuiEditBox * m_pTotalKillDeath;
	i3GuiEditBox * m_pTotalHeadshot;
	i3GuiEditBox * m_pTotalDis;
	i3GuiEditBox * m_pTotalUserRanking;

	// Season Record
	i3GuiEditBox * m_pSeasonRecordKey;
	i3GuiEditBox * m_pSeasonRecord;
	i3GuiEditBox * m_pSeasonKillDeath;
	i3GuiEditBox * m_pSeasonHeadshot;
	i3GuiEditBox * m_pSeasonDis;
	i3GuiEditBox * m_pSeasonUserRanking;

	// UserDetailInfoPopupFrame3	
	i3GuiButton * m_pRecordTab[2];	// Åë»ê, ½ÃÁð tab
	BOOL m_bRecordTabFlag;

	i3GuiEditBox * m_pModeTypeStatic;
	i3GuiEditBox * m_pModeTypeCombo;
	i3GuiButton * m_pModeTypeButton;
	i3GuiPopupList * m_pModeTypePopup;

	i3GuiEditBox * m_pModeRecordValue;
	i3GuiEditBox * m_pModeKillDeath;
	i3GuiEditBox * m_pModeHeadshot;
	i3GuiEditBox * m_pModeHitPercent;

	INT32 m_nModeTypeComboIdx;
	
	// [Button]
	// Button
	i3GuiButton * m_pButton[2];
	i3GuiButton * m_pClose;

	LOBBY_USER_INFO_LIST m_CurrentUser;
	BOOL m_bDone;

	BOOL m_bUserDetailInfoFlag;	

	// UserDetailInfoPopupMessageBox
	i3GuiStatic*	m_pUserDetailInfoPopupMsg;
	i3GuiEditBox*	m_pUserDetailInfoPopupMsgCaption;
	i3GuiEditBox*	m_pUserDetailInfoPopupMsgMessage;
	i3GuiButton*	m_pUserDetailInfoPopupMsgConfirm;
	i3GuiButton*	m_pUserDetailInfoPopupMsgCancel;

protected:
#ifdef USE_MODE_RECORD
	void _SetRecord( const USER_INFO_ALL_RECORD * pRecord);
#endif
};

#endif
