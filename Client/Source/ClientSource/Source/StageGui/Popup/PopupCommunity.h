#ifndef _PopupCommunity_h
#define _PopupCommunity_h

#include "PopupBase.h"
#include "GameContext.h"
#include "CommunityContext.h"


#define MAX_SLOT_FOR_CONTACT_LIST	8
#define MAX_NOTE_SLOT				10
#define MAX_NOTE_COUNT				100



struct ContactSlot
{
	i3GuiStatic* _pMark;
	i3GuiEditBox* _pNick;
	i3GuiEditBox* _pState;
};

struct NoteSlot
{
	i3GuiCheckBox* _pCheck;
	i3GuiEditBox* _pContent;
	i3GuiEditBox* _pWho;
	i3GuiEditBox* _pDate;
	i3GuiStatic* _pSelect;
};

enum COMMUNITY_TAB
{
	COMMUNITY_FRIEND_TAB = 0,	//	친구 
	COMMUNITY_BLOCK_TAB,		//	차단
	COMMUNITY_CLAN_TAB,			//	클랜
	COMMUNITY_TAB_INVALID,
};

enum NickPopupType
{
	NPT_ADD_FRIEND = 0,
	NPT_FIND_USER,
	NPT_ADD_BLOCK,
};

enum NOTE_TYPE
{
	NOTE_TYPE_SEND = 0,
	NOTE_TYPE_REPLY,
	NOTE_TYPE_GIFT,
};

class CUserLocationInfo;

class CPopupCommunity: public CPopupBase
{
	I3_CLASS_DEFINE(CPopupCommunity);
public:
	CPopupCommunity();
	virtual ~CPopupCommunity();

	virtual void OnPreGuiNotify(void);
	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void OnExitKey(void);
	virtual void SetInputDisable(BOOL bDisable);
	virtual BOOL GetInputDisable(void);
	#ifdef USE_MODE_RECORD
	virtual	void OnModeTab(BOOL bModeFlag);
	virtual void OnTypeComboButton(void);
	virtual void OnTypeComboPopup(INT32 idx);	
	#endif

	virtual void OnOpenPopup(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void OnClosePopup(void * pArg1 = NULL, void * pArg2 = NULL);

	virtual BOOL OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);
	virtual void OnGameEvent(INT32 event, INT32 arg);
	virtual void OnWheelUpdate(INT32 scrollby);

	void OnContactTab(void);
	void OnNoteTab(void);
	void OnLeave(void);
	void OnProfile(void);	
	void OnAddFriend(void);	
	void OnVisit(void);
	void OnWhisper(void);
	void OnClanInvite(void);
	void OnInviteRoom(void);	
	void OnVisitFriend(void);
	void OnVisitClan(void);
	void OnFriendInvite( COMMUNITY_TAB currentTab );	
	void OnClanRoomInvite(void);
	void OnContactClose(void);
	void OnFriendTab(void);
	void OnBlockTab(void);
	void OnClanTab(void);
	void OnRecordTab(INT32 idx);
	void OnFind(void);
	void OnAdd(void);
	void OnDelete(void);
	void OnRefuseFriend(void);
	void OnContactDrag(void);
	void OnContactFocus(UINT32 idxItem);
	void OnContactSelect(UINT32 idxItem);
	void OnContactAccept(UINT32 idxItem);
	void OnContactContext(UINT32 idxItem);
	void OnDeleteNote(void);
	void OnWriteNote(void);
	void OnNoteFocus(UINT32 idxItem);
	void OnNoteSelect(UINT32 idxItem);
	void OnSendNote(void);
	void OnNoteClose(void);
	void OnNoteAccept(BOOL bVal);
	void OnNickConfirm(void);
	void OnNickCancel(void);

	// Note
	void ReceiveAllNote( INT32 offset = 0);
	void SetNoteCheck( INT32 idx);
	void SetNoteType( NOTE_TYPE type);
	void SetSelectedNote( INT32 idx);
	void ResetNote(void);
	void StartAlarmNote(void);
	void EndAlarmNote(void);
	void _UpdateAlarmNote(REAL32 rDeltaSeconds);

	INT64	GetCurrentUserInfoUID() const		{ return m_CurrentUserInfoUID; }
	void	SetCurrentUserInfoUID(INT64 val)	{ m_CurrentUserInfoUID = val; }

	COMMUNITY_TAB GetCurrentUserInfoType() const { return m_CurrentUserInfoType; }
	void SetCurrentUserInfoType(COMMUNITY_TAB val) { m_CurrentUserInfoType = val; }

#ifdef USE_MESS_TEST
	INT64	_FindNoteByUid(char* nick);
	INT64	_FindUserByUid(char* nick);
#endif

	static void CbAcceptFriend(void* pThis,INT32 nParam);
	static void CbDeleteFriend(void* pThis,INT32 nParam);
	static void	CbDeleteNote( void * pThis, INT32 nParam);

private:
	void _InitCommon(void);
	void _InitContactSect(void);
	void _InitContactChild(void);
	void _InitUserDetailInfo(void);
	void _InitNoteSect(void);
	void _InitNoteChild(void);
	void _UpdateContactChild(void);

	void _UpdateContactList(void);
	void _AddFriend(char* nick);
	void _AddBlock(char* nick);
	void _FindUser(char* nick);
	void _UpdateNote(void);
	BOOL _SetNormalNote( NOTE_RECEIVE* pNote);
	BOOL _SetGiftNote( NOTE_RECEIVE* pNote);		//	Only gift note

	BOOL _IsMyFriend( INT64 uid);
	BOOL _IsMyClanMember( INT64 uid);
	BOOL _CheckCanWhisper(FRIEND_STATE State);

#if defined(CHANGE_ADDCOMMAND_METHOD)
	void AddPopupCommand(ContextMenuCommand Cmd, const CUserLocationInfo & UserLocInfo, INT64 UID);
#endif

	BOOL	GetCurSelUserLoccationInfo(CUserLocationInfo & UserLocInfo);

	#ifdef USE_MODE_RECORD
	void OnUserDetailInfo(void);
	void OnUserDetailInfoPupupMsgConfirm(void);
	void OnUserDetailInfoPupupMsgCancel(void);
	
	void _SetRecord( const USER_INFO_ALL_RECORD * pRecord);	
	#endif

private:
	i3GuiStatic* m_pCommunityGui;
	i3GuiStatic* m_pCommunityWin;
	i3GuiEditBox* m_pCommunityCaption;
	i3GuiStatic* m_pContactChild;
	i3GuiStatic* m_pNoteChild;
	i3GuiStatic* m_pContactSect;
	i3GuiStatic* m_pNoteSect;
	i3GuiButton* m_pContactTab;
	i3GuiButton* m_pNoteTab;
	i3GuiEditBox* m_pNoteKey;
	i3GuiStatic* m_pNoteGageBg;
	i3GuiStatic* m_pNoteGage;
	i3GuiButton* m_pLeave;

//////////////////////////////////////////////////////////////////////////
	i3GuiStatic	 *	m_pUserDetailInfoPopup;
	i3GuiEditBox *	m_pCaption;
	// UserDetailInfoPopupFrame
	// User Info
	i3GuiStatic	 *	m_pUserDetailInfoRankMark;
	i3GuiEditBox *	m_pUserDetailInfoNicknameValue;
	i3GuiEditBox *	m_pUserDetailInfoRank;
	i3GuiEditBox *	m_pUserDetailInfoClanName;	
	i3GuiEditBox *	m_pUserDetailInfoTitle;	
	i3GuiEditBox *	m_pUserDetailInfoPoint;
	i3GuiEditBox *	m_pUserDetailInfoToll;

	i3GuiEditBox *	m_pUserDetailInfoPositionKey;
	i3GuiEditBox *	m_pUserDetailInfoPositionValue;

	// UserDetailInfoPopupFrame2
	// Total Record
	i3GuiEditBox *	m_pUserDetailInfoTotalRecordKey;
	i3GuiEditBox *	m_pUserDetailInfoTotalRecord;
	i3GuiEditBox *	m_pUserDetailInfoTotalKillDeath;
	i3GuiEditBox *	m_pUserDetailInfoTotalHeadshot;
	i3GuiEditBox *	m_pUserDetailInfoTotalDis;
	i3GuiEditBox *	m_pUserDetailInfoTotalUserRanking;

	// Season Record
	i3GuiEditBox *	m_pUserDetailInfoSeasonRecordKey;
	i3GuiEditBox *	m_pUserDetailInfoSeasonRecord;
	i3GuiEditBox *	m_pUserDetailInfoSeasonKillDeath;
	i3GuiEditBox *	m_pUserDetailInfoSeasonHeadshot;
	i3GuiEditBox *	m_pUserDetailInfoSeasonDis;
	i3GuiEditBox *	m_pUserDetailInfoSeasonUserRanking;

	// UserDetailInfoPopupFrame3	
	i3GuiButton *	m_pModeRecordTab[2];	// Mode의 통산, 시즌 tab
	BOOL			m_bRecordTabFlag;

	i3GuiEditBox *	m_pModeTypeStatic;
	i3GuiEditBox *	m_pModeTypeCombo;
	i3GuiButton *	m_pModeTypeButton;
	i3GuiPopupList * m_pModeTypePopup;

	i3GuiEditBox *	m_pModeRecordValue;
	i3GuiEditBox *	m_pModeKillDeath;
	i3GuiEditBox *	m_pModeHeadshot;
	i3GuiEditBox *	m_pModeHitPercent;

	INT32			m_nModeTypeComboIdx;

	// [Button]
	// Button
	i3GuiButton*	m_pUserDetailInfoProfile;
	i3GuiButton*	m_pUserDetailInfoFriend;
	i3GuiButton*	m_pUserDetailInfoInvite;
	i3GuiButton*	m_pUserDetailInfoDetail;
	i3GuiButton*	m_pUserDetailInfoContactClose;
	i3GuiButton*	m_pUserDetailInfoFriendRefuse;

	BOOL			m_bUserDetailInfoFlag;			
	INT64			m_SelectUid;

	// UserDetailInfoPopupMessageBox
	i3GuiStatic*	m_pUserDetailInfoMsg;
	i3GuiEditBox*	m_pUserDetailInfoMsgCaption;
	i3GuiEditBox*	m_pUserDetailInfoMsgMessage;
	i3GuiButton*	m_pUserDetailInfoMsgConfirm;
	i3GuiButton*	m_pUserDetailInfoMsgCancel;

//////////////////////////////////////////////////////////////////////////
    i3GuiEditBox*	m_pContactKey;
	i3GuiButton*	m_pRecordTab[2];
	i3GuiButton*	m_pProfile;
	i3GuiButton*	m_pFriend;
	i3GuiButton*	m_pInvite;
	i3GuiButton*	m_pContactClose;
	i3GuiButton*	m_pFriendRefuse;
	i3GuiEditBox*	m_pNicknameKey;
	i3GuiEditBox*	m_pNicknameValue;
	i3GuiEditBox*	m_pSeasonRecordKey;
	i3GuiEditBox*	m_pSeasonRecord;
	i3GuiEditBox*	m_pSeasonKillDeath;
	i3GuiEditBox*	m_pSeasonHeadshot;
	i3GuiEditBox*	m_pTotalRecordKey;
	i3GuiEditBox*	m_pTotalRecord;
	i3GuiEditBox*	m_pTotalKillDeath;
	i3GuiEditBox*	m_pTotalHeadshot;
	i3GuiEditBox*	m_pPositionKey;
	i3GuiEditBox*	m_pPositionValue;

	i3GuiButton* m_pFriendTab;
	i3GuiButton* m_pBlockTab;
	i3GuiButton* m_pClanTab;
	i3GuiButton* m_pFind;
	i3GuiButton* m_pAdd;
//	i3GuiButton* m_pDelete;
	i3GuiEditBox* m_pContactListName;
	i3GuiEditBox* m_pContactListState;
	ContactSlot m_contactSlot[MAX_SLOT_FOR_CONTACT_LIST];
	i3GuiList* m_pContactList;
	i3GuiStatic* m_pContactSelect;
	i3GuiStatic* m_pContactFocus;

	i3GuiButton* m_pNoteListTab;
	i3GuiButton* m_pNoteDelete;
	i3GuiButton* m_pNoteWrite;
	i3GuiStatic* m_pNoteSelect;
	i3GuiStatic* m_pNoteFocus;
	i3GuiEditBox* m_pNoteListContent;
	i3GuiEditBox* m_pNoteListWho;
	i3GuiEditBox* m_pNoteListDate;
	NoteSlot m_noteSlot[MAX_NOTE_SLOT];
	i3GuiList* m_pNoteList;

	i3GuiEditBox* m_pNoteWhoKey;
	i3GuiEditBox* m_pNoteWhoValue;
	i3GuiEditBox* m_pNoteValue;
	i3GuiButton* m_pNoteSend;
	i3GuiButton* m_pNoteClose;
	i3GuiButton* m_pNoteAccept;
	i3GuiButton* m_pNoteReject;

	BOOL		 m_bReceiverUIDFlag;
	UINT32		 m_iListIdx;

	//	Gift note
	i3GuiStatic* m_pNoteGiftItemFrame;
	i3GuiStatic* m_pNoteGiftWeaponImage;
	i3GuiStatic* m_pNoteGiftCharacterImage;
	i3GuiStatic* m_pNoteGiftCashItemImage;
	i3GuiEditBox* m_pNoteGiftValue[2];
	i3GuiStatic* m_pNoteGiftSeparate;

	i3GuiStatic* m_pNicknameWin;
	i3GuiEditBox* m_pNicknameCaption;
	i3GuiEditBox* m_pNicknameMessage;
	i3GuiEditBox* m_pNicknameInput;
	i3GuiButton* m_pNicknameConfirm;
	i3GuiButton* m_pNicknameCancel;

	COMMUNITY_TAB m_currentTab;

	INT32 m_SelectedContact;
	INT32 m_SelectedNote;
	INT32 m_SelectedNoteIndex;
	UINT32 m_acceptFriendIdx;
	INT32 m_lastDeleteFriend;

	NickPopupType	m_nickPopupType;
	
	// Note
	NOTE_TYPE		m_nNoteType;
	REAL32			m_fNoteEmptyRate;
	BOOL			m_bNoteCheck[MAX_NOTE_COUNT];
	BOOL			m_bAlarmCommunity;
	REAL32			m_rAlarmTerm;
	BOOL			m_bAlarmFlag;
	static BOOL		m_bWarningNoteCount;

	FRIEND_NODE			m_SelectedFriendNode;
	CLAN_MEMBER_NODE	m_SelectedClanMemberNode;

	INT32			m_PackageGoodsID;

	INT32			m_nTypeMessage[ NOTE_MESSAGE_MAX ];


	INT64			m_CurrentUserInfoUID;
	COMMUNITY_TAB	m_CurrentUserInfoType;

};

#endif //_PopupCommunity_h

