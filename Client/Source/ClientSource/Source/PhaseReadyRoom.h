#ifndef _PhaseReadyRoom_h
#define _PhaseReadyRoom_h

#include "ReadyPhaseBase.h"
#include "GameCharaDef.h"
#include "GameChat_Body.h"
#include "RandomMap.h"

//////////////////////////////////////////////////////////////////////////
#define		PRIZE_SLOT_VIEW_COUNT	3
#define		COUNTDOWN_COUNT			5

struct CardComplateSlot
{
	i3GuiStatic* _bg;
	i3GuiStatic* _ribbonDummy;
	i3GuiStatic* _medalDummy;
};


struct TeamSlot
{
	i3GuiStatic* _pOpen;
	i3GuiStatic* _pClose;
	i3GuiStatic* _pRank;
	i3GuiEditBox* _pNickname;
	i3GuiStatic* _pState;
	i3GuiButton* _pKick;
	i3GuiStatic* _pPcCafe;
	i3GuiEditBox* _pClanRank;
	i3GuiStatic* _pClanMark;
	i3GuiStatic* _pRankBeginner;
	i3GuiStatic* _pESportLeft;
	i3GuiStatic* _pESportRight;
};

struct InviteSlot
{
	i3GuiCheckBox* _pCheck;
	i3GuiStatic* _pRank;
	i3GuiEditBox* _pNickname;
	i3GuiStatic* _pRankBeginner;
};

class CGameCharaBase;
class CMessageBox;

class CPhaseReadyRoom: public CReadyPhaseBase
{
	I3_CLASS_DEFINE(CPhaseReadyRoom);
public:
	CPhaseReadyRoom();
	virtual ~CPhaseReadyRoom();

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual void OnGameEvent(INT32 event,INT32 arg);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnPreGuiNotify(void);
	virtual void OnUpdate(REAL32 rDeltaSeconds);

	virtual void OnEntranceStart(void);
	virtual BOOL OnEntrancing(RT_REAL32 rDeltaSeconds);
	virtual void OnEntranceEnd(void);
	virtual void OnExitStart(void);
	virtual BOOL OnExiting(RT_REAL32 rDeltaSeconds);
	virtual void OnExitEnd(void);

	virtual void OnSlideOut(BOOL bRight);
	virtual void OnSlideIn(BOOL bRight);

	virtual void OnExitKey(void);
	virtual void OnWheelUpdate(INT32 scrollby);
	virtual void SetInputDisable(BOOL bDisable);
	virtual BOOL GetInputDisable(void);

	virtual BOOL OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);
	virtual BOOL OnSliding(RT_REAL32 rDeltaSeconds);

	void OnRecordWindow(void);
	void OnGameStartButtonClick(void);
	void OnExitButtonClick(void);
	void OnPasswordChangeButtonClick(void);
	void OnTitleChangeButtonClick(void);
	void OnModePrevButtonClick(void);
	void OnModeNextButtonClick(void);
	void OnVictoryPrevButtonClick(void);
	void OnVictoryNextButtonClick(void);
	void OnVictory2PrevButtonClick(void);
	void OnVictory2NextButtonClick(void);
	void OnInviteListButtonClick(void);
	void OnTransferHost(UINT32 idxSlot);
	void OnKickButton(UINT32 idxSlot);
	void OnMapChange(void);
	void OnMapCombo(void);
	void OnMapPopup(UINT32 idxSlot);
	void OnRedListFocus(UINT32 idxRedSlot);
	void OnRedListSelect(UINT32 idxRedSlot);
	void OnRedListContext(UINT32 idxRedSlot);
	void OnBlueListFocus(UINT32 idxBlueSlot);
	void OnBlueListSelect(UINT32 idxBlueSlot);
	void OnBlueListContext(UINT32 idxBlueSlot);
	void OnChatCombo(void);
	void OnChatPopup(UINT32 idxItem);
	void OnOption(void);
	//랜던맵관련함수
	void OnRandomMap(void);
	void OnCheckRandomMap(void);
	void RunRandomMap(void);
	void RefreshRandomMap();
	void InitRandomCheck();

	void OnTrainingTimePrev(void);
	void OnTrainingTimeNext(void);
	void OnTrainingEnemyPrev(void);
	void OnTrainingEnemyNext(void);
	void OnTrainingDifficultyCombo(void);
	void OnTrainingDifficultyPopup(UINT32 idxSlot);
	void OnAdvencedConfirm(void);
	void OnAdvencedCancel(void);
	void OnJoinLimitCheck(BOOL check);
	void OnObserverLimitCheck(BOOL check);
	void OnTeamBalanceCheck(INT32 check);
	void OnInviteConfirm(void);
	void OnInviteCancel(void);
	void OnComplateConfirm(void);
	void OnButtonConfirm(void);
	void OnFinalComplateWinConfirm(void);
	void OnFinalComplateExWinConfirm(void);
	void OnFinalComplatePrizeWinConfirm(void);
	BOOL OnOpenPopupRewardItemForCardSetComplete(void);
	void OnRankUpConfirm(void);
	void OnSearchHost(void);
	void OnTeamChange(void);
	void OpenPopupUsingCashItem(void);

	virtual	BOOL OnInviteMove(void);
	void OnAddFriend(INT32 slotIdx);
	void OnWhisper(INT32 slotIdx);
	void OnViewOtherUserItemInfo(INT32 slotIndex);
	void OnClanInvite(INT32 slotIdx);
	void OnAddBlock(INT32 slotIdx);

	void CheckBattleChallengeComplate(void);

	void setCharaAttachRoot( i3Node * pNode)		{ m_pCharaAttachRoot = pNode; }

	void EventMessage(void);
	void SetUpdateEnable(BOOL bVal)					{ m_bEnableUpdate = bVal;		}
	
	//Room Master GameStart Timer After All User Ready
	void AllUserReady();
	//Button Effect
	void UpdateReadyButton();
	void UpdateStartButton();

	//보급 상자 보급창
	void ApplyRewardReadyBox();

private:
		void _OpenPopupReadyBoxRewardItem(INT32 ItemID);

private:
    void _ProcessCommon( REAL32 rDeltaSeconds);
	void _ProcessCharaInfo( REAL32 rDeltaSeconds);
	void _ProcessCharaDetailInfo( REAL32 rDeltaSeconds);
	void _ProcessMapInfo( REAL32 rDeltaSeconds);
	void _ProcessSlot( REAL32 rDeltaSeconds);
	void _ProcessChat( REAL32 rDeltaSeconds);
	void _ProcessClanBattle(REAL32 rDeltaSeconds);
	void _InitUserInfo(void);
	void _ShowSlotState(INT32 idxSlot);
	void _ShowSlotRankMark(INT32 idxSlot);
	void _UpdateEquipViewMatrix(void);
	BOOL _IsReady(void);
	void _SlotClick(INT32 idxRedSlot);
	void _ChangeTeam(INT32 nTeamType);
	void _GetUserInfo(INT32 idxGameSlot);
	void _InitCommon(void);
	void _InitSlot(void);
	void _InitInfo(void);
	void _InitChat(void);
	void _CheckBeHost(void);
	BOOL _BeHost(void);
	void _UpdateMapInfo( ROOM_INFO_ALL * pInfo);
	void _RotateModeIndex(BOOL bNext,BOOL bRotateMap = FALSE);
	void _RotateTypeIndex(BOOL bNext);
	void _RotateTimeIndex(BOOL bNext);
	void _RotateTrainingTime(void);
	void _RotateTrainingEnemy(BOOL bNext);
	void _RotateTrainingDifficulty(BOOL bNext);
	void _UpdateTitlePassword(BOOL bInit = FALSE);
	void _InitRecord(void);
	void _Update3DCharacter( void );
	void _InitAdvencedWin(void);
	void _InitInviteWin(void);
	void _InitCardComplateWin(void);
	void _InitFinalComplateWin(void);
	void _InitRankUpWin(void);
	void _NotifyChangeOption(void);
	void _ProcessNotifyChangeOption(REAL32 rDeltaSeconds);
	void _OpenContextMenu(INT32 slotIdx);
	void _SlideRecordWindow(RT_REAL32 rDeltaSeconds);
	void _InitSlideRecordWindow(void);
	void _DelayInviteResponse(RT_REAL32 rDeltaSeconds);
	void _OpenFinalPrizeWindow(void);

	void _OpenFinalPrizeWindowUI( CARDSET_ID CardsetID );

	void _OpenPopupRewardItemForCardSetComplete(INT32 ItemID);
	BOOL _SetClanInfoInSlot( INT32 slotNum, SLOT_INFO* pInfo);
	void _OpenRankUpPopup(void);
	BOOL _CheckReady(void);

	//	클랜 매치메이킹
	void _InitClanBattle(void);
	void _SetNormalRoomControl(void);
	void _SetClanBattleRoomControl(void);

	void _InitTrainingSlot(void);

public:
	void EhKicked(void);
	void EhNoRealIP(void);
	void EhNoReadyTeam(void);
	void EhStageError(INT32 Event,INT32 Arg);

	static void CbBeHost(void* pThis,INT32 nParam);
	static void CbSearchHost(void* pThis,INT32 nParam);
	static void cbQueryLeaveClanBattle(void * pThis, INT32 nParam);
	static void CbMoveChannel(void* pThis,INT32 nParam);
	static void CbBlockUser(void* pThis, INT32 nParam);
	static void CbFinalComplateConfirm(void* pThis,INT32 nParam);
	static void CbMoveShop(void* pThis, INT32 nParam);

	static INT32 _GetRewardMedalByCardSet(UINT32 cardSetId);
	static void _GetRewardRibbonByCardSet(UINT32 cardSetId, i3GuiStatic * pStatic, char * pszOutString, const size_t sizeOutStrMax);

private:
	i3GuiStatic* m_pRoomGui;
	i3GuiStatic* m_pRoomWin;
	i3GuiEditBox* m_pRoomCaption;

	i3GuiStatic* m_pKey;
	i3GuiStatic* m_pBattleMark;
	i3GuiButton* m_pStart;
	i3GuiButton* m_pReady;
	i3GuiButton* m_pCancelStart;
	i3GuiButton* m_pCancelReady;
	i3GuiButton* m_pInvite;
	i3GuiButton* m_pLeave;
	i3GuiButton* m_pSearchHost;
	i3GuiButton* m_pTeamChange;

	i3GuiStatic* m_pRedBg;
	i3GuiStatic* m_pBlueBg;
	i3GuiPopupList* m_pRedList;
	i3GuiPopupList* m_pBlueList;
	TeamSlot m_slot[SLOT_MAX_COUNT];
	i3GuiStatic* m_pRedSelect;
	i3GuiStatic* m_pRedFocus;
	i3GuiStatic* m_pBlueSelect;
	i3GuiStatic* m_pBlueFocus;

	i3GuiStatic* m_pMapImage;
	i3GuiStatic* m_pTypeImage;
	i3GuiStatic* m_pExtraTypeImage;
	i3GuiStatic* m_pMapNewMark;
	i3GuiStatic* m_pMapXMasMark;
	i3GuiStatic* m_pMapCircle;
	i3GuiEditBox* m_pMap;
	i3GuiButton* m_pMapCombo;
	i3GuiPopupList* m_pMapList;
	i3GuiEditBox* m_pTitleKey;
	i3GuiEditBox* m_pTitleValue;
	i3GuiButton* m_pTitleChange;
	i3GuiEditBox* m_pPasswordKey;
	i3GuiEditBox* m_pPasswordValue;
	i3GuiButton* m_pPasswordChange;
	i3GuiEditBox* m_pModeKey;
	i3GuiEditBox* m_pModeValue;
	i3GuiButton* m_pModePrev;
	i3GuiButton* m_pModeNext;
	i3GuiEditBox* m_pTypeKey;
	i3GuiEditBox* m_pTypeValue;
	i3GuiButton* m_pTypePrev;
	i3GuiButton* m_pTypeNext;
	i3GuiEditBox* m_pTimeKey;
	i3GuiEditBox* m_pTimeValue;
	i3GuiButton* m_pTimePrev;
	i3GuiButton* m_pTimeNext;
	i3GuiEditBox* m_pOptionKey;
	i3GuiButton* m_pOption;
	i3GuiButton* m_pRandomMap;
	i3GuiEditBox* m_pTrainingTimeKey;
	i3GuiEditBox* m_pTrainingTimeValue;
	i3GuiButton* m_pTrainingTimePrev;
	i3GuiButton* m_pTrainingTimeNext;
	i3GuiEditBox* m_pTrainingEnemyKey;
	i3GuiEditBox* m_pTrainingEnemyValue;
	i3GuiButton* m_pTrainingEnemyPrev;
	i3GuiButton* m_pTrainingEnemyNext;
	i3GuiEditBox* m_pTrainingDifficultyKey;
	i3GuiEditBox* m_pTrainingDifficultyValue;
	i3GuiButton* m_pTrainingDifficultyCombo;
	i3GuiPopupList* m_pTrainingDifficultyList;


	// 상세정보창
	i3GuiStatic* m_pRecordWin;
	i3GuiEditBox* m_pRecordCaption;
	i3GuiEditBox* m_pRecordNickname;
	i3GuiStatic* m_pRecordRank;
	i3GuiStatic* m_pRecordRank_Beginner;
	i3GuiEditBox* m_pSeasonKey;
	i3GuiEditBox* m_pSeasonRecord;
	i3GuiEditBox* m_pSeasonKillDeath;
	i3GuiEditBox* m_pSeasonHeadshot;
	i3GuiEditBox* m_pSeasonKillDeath2;
	i3GuiEditBox* m_pSeasonDisconnect;
	i3GuiEditBox* m_pTotalKey;
	i3GuiEditBox* m_pTotalRecord;
	i3GuiEditBox* m_pTotalKillDeath;
	i3GuiEditBox* m_pTotalHeadshot;
	i3GuiEditBox* m_pTotalKillDeath2;
	i3GuiEditBox* m_pTotalDisconnect;
	i3GuiEditBox* m_pClanKey;
	i3GuiEditBox* m_pClanNameKey;
	i3GuiEditBox* m_pClanNameValue;
	i3GuiEditBox* m_pClanRankKey;
	i3GuiEditBox* m_pClanRankValue;
	i3GuiStatic* m_pClanMark;
	i3GuiStatic* m_pClanRank;

	// 고급옵션창
	i3GuiStatic* m_pAdvencedWin;
	i3GuiEditBox* m_pAdvencedCaption;
	i3GuiButton* m_pAdvencedConfirm;
	i3GuiButton* m_pAdvencedCancel;
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
	i3GuiEditBox* m_pTeamBalanceKey;
	i3GuiEditBox* m_pTeamBalancePersonnelKey;
	i3GuiEditBox* m_pTeamBalanceAbilityKey;
	i3GuiEditBox* m_pTeamBalanceNoKey;
	i3GuiCheckBox* m_pTeamBalancePersonnel;
	i3GuiCheckBox* m_pTeamBalanceAbility;
	i3GuiCheckBox* m_pTeamBalanceNo;

	i3GuiEditBox* m_pLimitObserver3PV_FreeKey;
	i3GuiCheckBox* m_pLimitObserver3PV_Free;
	i3GuiEditBox* m_pLimitObserver1PV_FreeKey;
	i3GuiCheckBox* m_pLimitObserver1PV_Free;
	i3GuiEditBox* m_pLimitObserverEnemy_FreeKey;
	i3GuiCheckBox* m_pLimitObserverEnemy_Free;
	i3GuiEditBox* m_pLimitObserverEnemyHPKey;
	i3GuiCheckBox* m_pLimitObserverEnemyHP;

	i3GuiStatic* m_pInviteWin;
	i3GuiEditBox* m_pInviteCaption;
	i3GuiButton* m_pInviteConfirm;
	i3GuiButton* m_pInviteCancel;
	InviteSlot m_inviteSlot[MAX_INVITE_COUNT];
	//랜덤맵창
	i3GuiCheckBox* m_pSelectRandomMap;

	// CardComplateWin
	i3GuiStatic* m_pCardComplateWin;
	i3GuiEditBox* m_pCardComplateCaption;
	i3GuiButton* m_pCardComplateConfirm;
	i3GuiEditBox* m_pCardComplateExp;
	i3GuiEditBox* m_pCardComplatePoint;

	CardComplateSlot m_cardComplateSlot;
	i3GuiEditBox * m_pCardCompleteName;


	// FinalComplateWin
	i3GuiStatic* m_pFinalComplateWin;
	i3GuiEditBox* m_pFinalComplateCaption;
	i3GuiButton* m_pFinalComplateConfirm;
	i3GuiEditBox* m_pFinalComplateName;
	i3GuiEditBox* m_pFinalComplateDescription;
	i3GuiStatic* m_pFinalComplateBg;
	i3GuiStatic* m_pFinalComplateDummy;
	i3GuiStatic* m_pFinalComplateBg2;
	i3GuiStatic* m_pFinalComplateDummy2;

	// FinalComplateExWin
	i3GuiStatic* m_pFinalComplateExWin;
	i3GuiEditBox* m_pFinalComplateExCaption;
	i3GuiButton* m_pFinalComplateExConfirm;
	i3GuiEditBox* m_pFinalComplateExName;
	i3GuiEditBox* m_pFinalComplateExDescription;
	i3GuiStatic* m_pFinalComplateExPrizeBg;
	i3GuiStatic* m_pFinalComplateExRibbonDummy;
	i3GuiStatic* m_pFinalComplateExMedalDummy;
	i3GuiStatic* m_pFinalComplateExWeaponBg;
	i3GuiStatic* m_pFinalComplateExWeaponDummy;

	// FinalComplatePrizeWin
	i3GuiStatic* m_pFinalComplatePrizeWin;
	i3GuiEditBox* m_pFinalComplatePrizeCaption;
	i3GuiButton* m_pFinalComplatePrizeConfirm;
	i3GuiEditBox* m_pFinalComplatePrizeName;
	i3GuiEditBox* m_pFinalComplatePrizePoint;
	i3GuiEditBox* m_pFinalComplatePrizeExp;
	i3GuiEditBox* m_pFinalComplatePrizeDescription;
	i3GuiStatic* m_pFinalComplatePrizeDummy;			// 표장 이미지가 들어가는 컨트롤.

	INT32 m_nItemIDForCardSetComplete;		//	최종 보상 아이템 ID
	INT32 m_nCountForItemIDForCardSetComplete;	//	최종 보상 아이템 ID 갯수

	// 진급 포인트 시스템 윈도우
	i3GuiStatic* m_pRankUpWin;
	i3GuiEditBox* m_pRankUpCaption;
	i3GuiButton* m_pRankUpConfirm;
	i3GuiEditBox* m_pRankUpMessage;
	i3GuiStatic* m_pRankUpMark;
	i3GuiEditBox* m_pRankUpName;

	//	일반 룸
	i3GuiStatic * m_pNormalRoomInfoGui;
	i3GuiStatic * m_pNormalRoomBottomGui;

	//	클랜전 룸
	i3GuiStatic * m_pClanBattleInfoGui;
	i3GuiStatic * m_pClanBattleInfoRedClanMark;
	i3GuiStatic * m_pClanBattleInfoBlueClanMark;
	i3GuiEditBox * m_pClanBattleInfoRedClanName;
	i3GuiEditBox * m_pClanBattleInfoRedTeamName;
	i3GuiEditBox * m_pClanBattleInfoBlueClanName;
	i3GuiEditBox * m_pClanBattleInfoBlueTeamName;
	i3GuiEditBox * m_pClanBattleInfoModeKey;
	i3GuiEditBox * m_pClanBattleInfoModeValue;
	i3GuiButton * m_pClanBattleInfoModePrev;
	i3GuiButton * m_pClanBattleInfoModeNext;
	i3GuiEditBox * m_pClanBattleInfoTypeKey;
	i3GuiEditBox * m_pClanBattleInfoTypeValue;
	i3GuiButton * m_pClanBattleInfoTypePrev;
	i3GuiButton * m_pClanBattleInfoTypeNext;
	i3GuiEditBox * m_pClanBattleInfoTimeKey;
	i3GuiEditBox * m_pClanBattleInfoTimeValue;
	i3GuiButton * m_pClanBattleInfoTimePrev;
	i3GuiButton * m_pClanBattleInfoTimeNext;
	i3GuiEditBox * m_pClanBattleInfoOptionKey;
	i3GuiButton * m_pClanBattleInfoOptionButton;

	i3GuiStatic * m_pClanBattleBottomGui;
	i3GuiButton * m_pClanBattleBottomTeamChange;

	i3GuiCheckBox* m_pChkObserver;
	i3GuiEditBox* m_pEditObserver;

	BOOL m_bOpenRedSlot[MAX_SLOT_PER_TEAM];
	BOOL m_bOpenBlueSlot[MAX_SLOT_PER_TEAM];
	INT32 m_nOldViewSlot;

	BOOL m_bIsHost;
	BOOL m_bEnableUpdate;
	BOOL m_bChagingTeam;
	BOOL m_bOnceChatType;
	BOOL m_bOnceChatTypeAll;
	INT32 m_nOldTeam;

	REAL32 m_fViewPosX;
	REAL32 m_fViewPosY;
	REAL32 m_fViewPosZ;
	REAL32 m_fViewAngleY;

	i3Node *	m_pCharaAttachRoot;

	CMessageBox* m_pWaitInviteMB;
	CMessageBox* m_pBeHostMessageBox;

	BOOL m_bChangeOptionNotify;
	BOOL m_bKickingState;

	BOOL m_bRecordWindowSliding;
	BOOL m_bInRecordWindow;
	REAL32 m_rRecordSlideTimeStamp;

	BOOL m_bInviteResult;
	BOOL m_bInviteMessage;
	BOOL m_pOnFlag;

	INT32 m_transferHostSlotIdx;
	REAL32 m_rSearchHostDelay;
	REAL32 m_rTeamChangeDelay;

	INT32 m_nSelectedSlotIdx;

	// 섬멸전<->단체전 변경 화살표 애니메이션용
	REAL32 m_circleTerm;
	INT32 m_circleIdx;
	BOOL m_playCircleAnimation;

	INT32	m_IndexMap;
	INT32	m_CheckMap;
	i3SoundPlayInfo* m_pCountdownSound[COUNTDOWN_COUNT];

	REAL32 m_teamChangePosX;

	AI_MODE_STATE m_AiModeState;
};

#endif //_PhaseReadyRoom_h
