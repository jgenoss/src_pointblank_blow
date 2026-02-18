#ifndef _PhaseLobby_h
#define _PhaseLobby_h

#include "ReadyPhaseBase.h"
#include "StageDef.h"
#include "GameChat_Body.h"
#include "GuiButtonEffector.h"
#include "RandomMap.h"

typedef struct SSortOrder
{
	INT32	_nType;
	BOOL	_bAscending;
}SORT_ORDER;

struct RoomSlot 
{
	i3GuiStatic* _pSlot;
	i3GuiEditBox* _pSlotIdx;
	i3GuiEditBox* _pSlotMode;
	i3GuiEditBox* _pSlotMap;
	i3GuiEditBox* _pSlotTitle;
	i3GuiEditBox* _pSlotState;
	i3GuiEditBox* _pSlotPerson;
	i3GuiStatic* _pSlotBattleMarkFront;
	i3GuiStatic* _pSlotBattleMarkRear;
	i3GuiStatic* _pSlotKey;
	i3GuiStatic* _pSlotPing;
	i3GuiStatic* _pSlotNonRealIP;
};

struct UserSlot
{
	i3GuiStatic* _pSlot;
	i3GuiStatic* _pClanMark;
	i3GuiStatic* _pRank;
	i3GuiStatic* _pBeginner;
	i3GuiStatic* _pESport;
	i3GuiEditBox* _pNick;
	i3GuiStatic* _pFilter;
};


class CMessageBox;

class CPhaseLobby: public CReadyPhaseBase
{
	I3_CLASS_DEFINE(CPhaseLobby);
public:
	CPhaseLobby();
	virtual ~CPhaseLobby();

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual void OnGameEvent(INT32 event,INT32 arg);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnPreGuiNotify(void);
	virtual void OnUpdate(REAL32 rDeltaSeconds);	
	virtual void OnWheelUpdate(INT32 scrollby);	
	virtual void SetInputDisable(BOOL bDisable);
	virtual BOOL GetInputDisable(void);

	virtual void OnEntranceStart(void);
	virtual BOOL OnEntrancing(RT_REAL32 rDeltaSeconds);
	virtual void OnEntranceEnd(void);
	virtual void OnExitStart(void);
	virtual BOOL OnExiting(RT_REAL32 rDeltaSeconds);
	virtual void OnExitEnd(void);	

	virtual void OnSlideOut(BOOL bRight);
	virtual void OnSlideIn(BOOL bRight);
	
	virtual BOOL OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);
	virtual BOOL OnSliding(RT_REAL32 rDeltaSeconds);

	void OnQuickStart(void);
	void OnSpecialWar(BOOL bFlag);
	void OnDino(BOOL bFlag); //EscapeMission 공룡모드 설정창

	//중간 위치, 크기를 변경한다.
	void OnAdjustSpecialWarShape();

	void On4vs4(void);

	enum SP_MODE_SPECIAL_WAR
	{
		SP_MODE_CHALLENGE,
		SP_MODE_SHOTGUN,
		SP_MODE_SNIPER,
		SP_MODE_HEADHUNTER,
		SP_MODE_KNUCKLE,
		SP_MODE_TUTORIAL,

		SP_MODE_MAX
	};
	enum DINO_ESCAPE_MODE
	{
		DINO_ESCAPE,
		DINO_DOMINATION,

		DINO_MODE_MAX
	};

	void SetStateSpecialWarButton(SP_MODE_SPECIAL_WAR mode, bool bOnMouse);
	
	void OnSpecialWarChallenge(void);
	void OnSpecialWarSniper(void);	
	void OnSpecialWarShotgun(void);
	void OnSpecialWarHeadHunter(void);
	void OnSpecialWarKnuckle(void);

	//EscapeMission
	void SetStateDinoButton(DINO_ESCAPE_MODE mode, bool bOnMouse);
	void OnDinoEscape(void);
	void OnDinoDomination(void);

	void OnCreateRoom(void);
	void OnRefresh(void);
	void OnServer(void);
	void OnEnter(void);


	void OnCreateRoomModePopup(UINT32 idxItem);
	void OnCreateRoomModeCombo(void);	
	void OnCreateRoomConfirmButtonClick(void);
	void OnCreateRoomCancelButtonClick(void);

	void OnSlotOnMouse(INT32 idxSlot);
	void OnSlotClick(INT32 idxRoom);
	void OnSlotDrag(void);

	void OnEnter(INT32 idxRoom);
	void OnPcCafeConfirm(void);
	void OnCreateNickname(void);
	void OnSortNumber(void);
	void OnSortMode(void);
	void OnSortMap(void);
	void OnSortName(void);
	void OnSortState(void);
	void OnSortPerson(void);
	void OnSortPing(void);

	virtual	BOOL OnInviteMove(void);

	// Chat	
	void OnChatCombo(void);
	void OnChatPopup(UINT32 idxItem);
	void OnWhisper(LOBBY_USER_INFO_LIST * pUser);

	//	user list	
	void OnUserListScroll(void);
	void OnUserListSelect(INT32 nIdx);
	void OnUserListClick(INT32 nIdx);
	void OnUserListFocus(INT32 nIdx);
	void OnAddFriend(LOBBY_USER_INFO_LIST * pUser);
	void OnUserDetail(LOBBY_USER_INFO_LIST * pUser);
	void OnUserDetail(INT32 nIdx);
	void OnClanInvite(LOBBY_USER_INFO_LIST * pUser);
	void OnAddBlock(LOBBY_USER_INFO_LIST * pUser); 
	// ButtonEffect
	void UpdateButton();
	// Abusing

	//#ifdef USE_ABUSING_CHECK
	void OnAbusingConfirm(void);
	//#endif
	

private:
	void _ProcessCommon(REAL32 rDeltaSeconds);
	void _ProcessRoomList(REAL32 rDeltaSeconds);
//#ifdef USE_ABUSING_CHECK
	void _ProcessAbusing(REAL32 rTime);
//#endif
    void _AddListRoomEmpty(UINT32 idxRoom);
	void _AddListRoomInfo(UINT32 idxRoom,ROOM_INFO_BASIC * pInfo);	
	void _ProcessCreateRoom( REAL32 rDeltaSeconds);
	void _ProcessRoomInfo( REAL32 rDeltaSeconds);
	void _ShowPingLevel(INT32 idxSlot,INT32 nLevel);
	void _SetRealIP(INT32 idxSlot, BOOL bReal);
	void _SetRoomListColor(INT32 idx, I3COLOR * color);
	void _ClearRoomInfo(void);
	void _AutoRefresh(REAL32 rDeltaSeconds);
	void _ProcessFirstLogin(void);

	void FirstLoninPopup();

	void _ShowNicknamePopup(void);	
	void _ProcessNickname(REAL32 rDeltaSeconds);
	void _ProcessCheckInvite(REAL32 rDeltaSeconds);
	void _Join(void);
	// Room List
	void _UpdateRoomList(void); // Room Update: AutoRefresh
	void _SortRoomList(void); // Room Sort: Refresh
	void _EmptyRoomList(void);
	void _ChangeSortOrder(INT32 SortType);
	void _WaitConnect(REAL32 rDeltaSeconds);
	void _CloseWaitMessageBox(void);
	void _BuildRoomListDummy(void);

	//	User list
	void _ProcessUserInfoList(REAL32 rDeltaSeconds);
	void _SetUserInfoListSlot(INT32 slot, const LOBBY_USER_INFO_LIST* pUser);
	void _ResetUserInfoList(void);
	void _BuildUserInfoListDummy(void);
	void _BuildUserInfoList(void);
	void _ShowUserInfoList(void);
	BOOL _SetSelectedUserInfo(INT32 nUserListSlotIdx);
	const LOBBY_USER_INFO_LIST * _GetUserInfo(INT32 idx);
	INT32 _GetUserInfoCount(void);

	void _SetInputDisableGUI(BOOL bDisable);

	// Chat
	void _InitChat(void);		
	void _ProcessChat( REAL32 rDeltaSeconds);

	// 확장된 방생성 팝업
	void ResetCreateRoomAddInfo(void);
	void OnMapCombo(void);
	void OnMapPopup(UINT32 idxSlot);
	void _ProcessMapInfo( REAL32 rDeltaSeconds);
	void OnModePrevButtonClick(void);
	void OnModeNextButtonClick(void);
	void _RotateModeIndex(BOOL bNext,BOOL bRotateMap = FALSE);
	void OnVictoryPrevButtonClick(void);
	void OnVictoryNextButtonClick(void);
	void _RotateTypeIndex(BOOL bNext);
	void OnVictory2PrevButtonClick(void);
	void OnVictory2NextButtonClick(void);
	void _RotateTimeIndex(BOOL bNext);
	void OnJoinLimitCheck(BOOL check);
	void OnObserverLimitCheck(BOOL check);
	void OnTeamBalanceCheck(INT32 check);
	void _UpdateMapInfo(void);	
	//랜덤맵 관련 함수
	void InitRandomMap();
	void OnCheckRandomMap();
	void OnRandomMap(void);

	void _ProcessSpecialMode( REAL32 rDeltaSeconds);	// 특수전 활성화 처리

	//이정우(4월 1일) : Static Gui를 재 조정한다.
	//특정 국가에 요청에 따른 Gui를 다시 조정한다.
	void _ModifyStaticGui();

	//2011년 4월 1일 - 이정우 : 닉네임 생성시 버튼 클릭 되는 경우
	//현재 상태를 확인 하고, Gui 버튼 클릭 될 때 판단한다.
	bool m_bNickSpace;

public:
	void EhWrongQuickJoin(void);
	void EhKickedByVote(void);
	void EhSlotFull(void);
	void EhWrongPW(void);
	void EhNotExist(void);
	void EhLimitInterenter(void);
	void EhLimitLever(void);
	void EhLimitTraining(void);
	void EhStageError(INT32 Event,INT32 Arg);

	static void CbSameNick(void* pThis,INT32 nParam);
	static void CbBeInvited(void* pThis,INT32 nParam);
	static void CbSuppliedConfirm(void* pThis,INT32 nParam);
	static void CbEventCardSetConfirm(void* pThis,INT32 nParam);
	static void CbClanInvite(void* pThis, INT32 nParam);
	static void CbBlockUser(void* pThis, INT32 nParam);
	static void CbTutorialGuideOK(void* pThis,INT32 nParam);

	static INT32 CompareRoom(void* pRoom1,void* pRoom2);	

	void ShowPcCafePopup(void);
	void ShowEventCardSetPopup(void);
	void ShowNoticePopup(void);	
	void ShowTutorialGuide(void);

	void EventMessage(void);

	void DoClanInvite(void);
	void DoBlockUser(void);

	void OpenItemWin(INT32 ItemId);
	void OnItemConfirm();
	//이벤트 팝업
	void OpenEventWin(INT32 ItemId);
	//이벤트 팝업 아이템 2개 출력 
	void OpenEventWin2(INT32 ItemId1,INT32 ItemId2);
	void OnEventConfirm();
	const ROOM_INFO_BASIC &		GetJoinTargetRoom()	{ return m_JoinTargetRoom; }
private:
	void	_InitItemWin();
	void	_InitEventWin();
	static	void	JoinRoomPasswordCallback(BOOL OK, const string & Password, void * pUserContext);

private:
	i3GuiStatic* m_pLobbyGui;
	i3GuiStatic* m_pInfoWin;
	i3GuiStatic* m_pRoomWin;
	i3GuiStatic* m_pCreateWin;
	i3GuiStatic* m_pNicknameWin;

    i3GuiEditBox* m_pInfoCaption;
	i3GuiEditBox* m_pRoomCaption;
	i3GuiEditBox* m_pServerInfoValue;

	i3GuiButton* m_pbtnQuickStart;
	i3GuiButton* m_pbtnSpecialWar;
	i3GuiButton* m_pbtnDino;

	i3GuiButton* m_pServer;		// 채널리스트로 돌아가는 컨트롤
	i3GuiButton* m_pEnter;
	i3GuiButton* m_pRefresh;
	i3GuiButton* m_pCreate;
	i3GuiEditBox* m_pTeamCaption;
	i3GuiEditBox* m_pServerCaption;
	i3GuiEditBox* m_pEnterCaption;
	i3GuiEditBox* m_pRefreshCaption;
	i3GuiEditBox* m_pCreateCaption;

	i3GuiList* m_pList;
	i3GuiStatic* m_pSelect;
	i3GuiStatic* m_pFocus;	
	RoomSlot m_roomSlot[MAX_ROOM_PER_PAGE_EX];

	i3GuiStatic* m_pImageDummy;
	i3GuiStatic* m_pImageDummy2;
	i3GuiStatic* m_pImageDummy3;
	i3GuiStatic* m_pImageDummy4;
	i3GuiStatic* m_pImageXMas;
	i3GuiEditBox* m_pMapKey;
	i3GuiEditBox* m_pMapValue;

	i3GuiEditBox* m_pModeKey;
	i3GuiEditBox* m_pModeValue;
	
	i3GuiEditBox* m_pTypeKey;
	i3GuiEditBox* m_pTypeValue;
	i3GuiEditBox* m_pHostKey;
	i3GuiEditBox* m_pHostValue;
	i3GuiEditBox* m_pStateKey;
	i3GuiEditBox* m_pStateValue;
	i3GuiEditBox* m_pJoinLimitKey;
	i3GuiEditBox* m_pJoinLimitValue;
	i3GuiEditBox* m_pWeaponLimitKey;
	i3GuiEditBox* m_pWeaponLimitValue;
	i3GuiEditBox* m_pObserverLimitKey;
	i3GuiEditBox* m_pObserverLimitValue;
	i3GuiEditBox* m_pTeamBalanceKey;
	i3GuiEditBox* m_pTeamBalanceValue;

	i3GuiEditBox* m_pIdxCaption;
	i3GuiEditBox* m_pModeCaption;
	i3GuiEditBox* m_pMapCaption;
	i3GuiEditBox* m_pTitleCaption;
	i3GuiEditBox* m_pStateCaption;
	i3GuiEditBox* m_pPersonCaption;
	i3GuiEditBox* m_pPingCatpion;

	i3GuiEditBox* m_pNicknameCaption;
	i3GuiEditBox* m_pNicknameMessage;
	i3GuiEditBox* m_pNicknameInput;
	i3GuiButton* m_pNicknameConfirm;

	i3GuiEditBox* m_pCreateWinCaption;
	i3GuiEditBox* m_pCreateTitleKey;
	i3GuiEditBox* m_pCreateTitleValue;
	i3GuiEditBox* m_pCreatePasswordKey;
	i3GuiEditBox* m_pCreatePasswordValue;
	i3GuiEditBox* m_pCreateModeKey;
	i3GuiEditBox* m_pCreateModeValue;
	i3GuiButton* m_pModeCombo;
	i3GuiPopupList* m_pModePopup;
	i3GuiButton* m_pCreateConfirm;
	i3GuiButton* m_pCreateCancel;
	i3GuiCheckBox* m_pCreatePasswordCheck;

	///////////////////////////////////////////////////////////////////////////////////////////
	// 확장된 방생성 팝업
	i3GuiStatic* m_pMapImage;
	i3GuiStatic* m_pTypeImage;
	i3GuiStatic* m_pExtraTypeImage;
	i3GuiStatic* m_pMapNewMark;
	i3GuiStatic* m_pMapXMasMark;

	i3GuiEditBox* m_pMap;
	i3GuiButton* m_pMapCombo;
	i3GuiPopupList* m_pMapList;	
	i3GuiEditBox* m_pGameModeKey;
	i3GuiEditBox* m_pGameModeValue;
	i3GuiButton* m_pGameModePrev;
	i3GuiButton* m_pGameModeNext;
	i3GuiEditBox* m_pGameTypeKey;
	i3GuiEditBox* m_pGameTypeValue;
	i3GuiButton* m_pGameTypePrev;
	i3GuiButton* m_pGameTypeNext;
	i3GuiEditBox* m_pTimeKey;
	i3GuiEditBox* m_pTimeValue;
	i3GuiButton* m_pTimePrev;
	i3GuiButton* m_pTimeNext;

	i3GuiEditBox* m_pLimitKey;		
	i3GuiCheckBox* m_pLimitPrimary;
	i3GuiEditBox* m_pLimitPrimaryKey;
	i3GuiStatic* m_pLimitPrimaryLock;
	i3GuiCheckBox* m_pLimitSecondary;
	i3GuiEditBox* m_pLimitSecondaryKey;
	i3GuiStatic* m_pLimitSecondaryLock;
	i3GuiCheckBox* m_pLimitMelee;
	i3GuiEditBox* m_pLimitMeleeKey;
	i3GuiStatic* m_pLimitMeleeLock;
	i3GuiCheckBox* m_pLimitThrow;
	i3GuiEditBox* m_pLimitThrowKey;
	i3GuiStatic* m_pLimitThrowLock;
	i3GuiCheckBox* m_pLimitRPG7;
	i3GuiEditBox* m_pLimitRPG7Key;
	i3GuiStatic* m_pLimitRPG7Lock;

	i3GuiEditBox* m_pLimitObserver3PV_FreeKey;	
	i3GuiCheckBox* m_pLimitObserver3PV_Free;
	i3GuiEditBox* m_pLimitObserver1PV_FreeKey;	
	i3GuiCheckBox* m_pLimitObserver1PV_Free;
	i3GuiEditBox* m_pLimitObserverEnemy_FreeKey;	
	i3GuiCheckBox* m_pLimitObserverEnemy_Free;
	i3GuiEditBox* m_pLimitObserverEnemyHPKey;	
	i3GuiCheckBox* m_pLimitObserverEnemyHP;

	i3GuiEditBox* m_pLimitJoinKey;
	i3GuiEditBox* m_pLimitJoinYesKey;
	i3GuiEditBox* m_pLimitJoinNoKey;
	i3GuiCheckBox* m_pLimitJoinYes;
	i3GuiCheckBox* m_pLimitJoinNo;	
	i3GuiEditBox* m_pLimitObserverKey;
	i3GuiEditBox* m_pLimitObserverYesKey;
	i3GuiEditBox* m_pLimitObserverNoKey;
	i3GuiCheckBox* m_pLimitObserverYes;
	i3GuiCheckBox* m_pLimitObserverNo;
	i3GuiEditBox* m_pGameTeamBalanceKey;
	i3GuiEditBox* m_pGameTeamBalancePersonnelKey;
	i3GuiEditBox* m_pGameTeamBalanceAbilityKey;
	i3GuiEditBox* m_pGameTeamBalanceNoKey;
	i3GuiCheckBox* m_pGameTeamBalancePersonnel;
	i3GuiCheckBox* m_pGameTeamBalanceAbility;
	i3GuiCheckBox* m_pGameTeamBalanceNo;
///////////////////////////////////////////////////////////////////////
//		RandomMapPopup추가
///////////////////////////////////////////////////////////////////////
	i3GuiCheckBox* m_pLobbyCheckRandomMap;
	i3GuiEditBox* m_pLobbyRandomMapEditBox;
	i3GuiButton* m_pLobbyRandomMapButton;
//
	///////////////////////////////////////////////////////////////////////////////////////////
	//채팅창 UID 관련
	i3GuiStatic* m_pLobbyChatUID[6];

	// PC방 알림 팝업
	i3GuiStatic* m_pPcCafeWin;
	i3GuiEditBox* m_pPcCafeCaption;
	i3GuiButton* m_pPcCafeConfirm;
	i3GuiEditBox* m_pPcCafeMessage;
	i3GuiEditBox* m_pPcCafeExpKey;
	i3GuiEditBox* m_pPcCafePointKey;
	i3GuiEditBox* m_pPcCafeItemKey;
	i3GuiEditBox* m_pPcCafeExpValue;
	i3GuiEditBox* m_pPcCafePointValue;
	i3GuiEditBox* m_pPcCafeItemValue;

	//	유저 목록
	i3GuiStatic * m_pUserListWin;
	i3GuiEditBox* m_pUserListCaption;
	i3GuiList *	  m_pUserListScroll;	
	i3GuiStatic*  m_pUserListSelect;
	i3GuiStatic*  m_pUserListFocus;	
	UserSlot	  m_pUserListSlot[ MAX_LOBBY_USER_SLOT];	

	// 어뷰징 관련
	i3GuiStatic		*m_pAbuseWindow;
	i3GuiEditBox	*m_pAbuseEditbox;
	i3GuiButton		*m_pAbuseConfirm;
	
	i3List m_listRoom;
	INT32 m_nRoomPage;
	REAL32 m_rEnterLobbyWaitTime;

	CMessageBox* m_pWaitRoomListMassageBox;

	INVITE_INFO m_InviteInfo;
	BOOL m_bCheckInvite;
	CMessageBox* m_pInviteMessageBox;

	REAL32 ButtonTermTime;
	BOOL m_bAllowRefresh;

	BOOL m_bEnableNicknameMsgBox;
	BOOL m_bEnableCreateRoomMsgBox;
	STAGE_TYPE m_CreateGameMode;

	INT32 m_SelectedElement;

	REAL32 m_fRefreshElapsedTime;

	BOOL m_bFirstLogin;

	BOOL	m_bRequestRoomInfoAdd;
	REAL32	m_LastRefreshTime;

	static BOOL sm_bAwayInput;
	static SORT_ORDER sm_SortOrder[MAX_SORT_TYPE];
	static BOOL m_bPcCafeNotice;
	static BOOL m_bEventCardSetNotice;
	static BOOL m_bEventNotice;


	// 채팅
	INT32 m_nChatType;
	INT32 m_nOldChatType;

	// User list
	i3List m_ViewUserInfoList;
	//
	BOOL m_bLogin;
	
	BOOL m_bRefrashUserInfoList;
	INT32 m_nUserInfoListStartIdx;
	INT32 m_nUserInfoListSelectIdx;
	LOBBY_USER_INFO_LIST m_SelectUserInfo;

	// 확장된 방생성 팝업
	UINT32 m_nStageID;
	UINT8 m_SubType;
	UINT8 m_LockWeapon;
	UINT8 m_LockInterEnter;
	UINT8 m_LockObserver;
	UINT8 m_LockTeamBalance;

	ROOM_INFO_BASIC		m_JoinTargetRoom;

	// 어뷰징 관련
	//#ifdef USE_ABUSING_CHECK
	// UINT32 m_uiAbusingTotalTime;
	REAL32 m_rAbusingTotalTime;
	BOOL m_bAbuseFlag;
	//#endif
	// 특수전
private:
	i3GuiStatic* m_pSpecialWarWin;
	i3GuiEditBox* m_pSpecialWarCaption;
	i3GuiButton* m_pSpecialWarCancel;

	i3GuiStatic* m_pDinoWin;
	i3GuiEditBox* m_pDinoCaption;
	i3GuiButton* m_pDinoCancel;



	i3GuiStatic* m_pItemWin;


	// FinalComplateWin
	i3GuiButton* m_pItemConfirm;
	i3GuiEditBox* m_pItemCaption;
	i3GuiEditBox* m_pItemName;
	i3GuiEditBox* m_pItemDescription;
	i3GuiStatic* m_pItemBg;
	i3GuiStatic* m_pItemDummy;
	i3GuiStatic* m_pItemBg2;
	i3GuiStatic* m_pItemDummy2;



	i3GuiStatic* m_pEventWin;
	// FinalComplateWin

	i3GuiStatic*  m_pEventTitle;
	i3GuiStatic*  m_pEventLayout;
	i3GuiButton*  m_pEventConfirm;
	i3GuiEditBox* m_pEventCaption;
	i3GuiEditBox* m_pEventName;
	i3GuiEditBox* m_pEventDescription;
	i3GuiStatic* m_pEventBg;
	i3GuiStatic* m_pEventDummy;
	i3GuiStatic* m_pEventBg2;
	i3GuiStatic* m_pEventDummy2;
	i3GuiStatic* m_pEventBg3;
	i3GuiStatic* m_pEventDummy3;

	

	struct	SpecialWar
	{
		i3GuiStatic *	pButton;
		i3GuiEditBox *	pButtonDesc;
		i3GuiEditBox *	pBtnSTitleDesc;
		i3GuiStatic *	pArrow;
		i3GuiStatic *	pEmpty;
	};
	
	struct	Dino
	{
		i3GuiStatic *	pButton;
		i3GuiEditBox *	pButtonDesc;
		i3GuiEditBox *	pBtnSTitleDesc;
		i3GuiStatic *	pArrow;
		i3GuiStatic *	pEmpty;
	};

	SpecialWar	m_SpecialWar[SP_MODE_MAX];
	Dino		m_Dino[DINO_MODE_MAX];
	//CGuiButtonEffector	m_BtnEffector;
	//CGuiButtonEffector	m_BtnEffector1;

	// 튜토리얼 모드
public:
	void StartTutorialMode();
};

#endif //_PhaseLobby_h
