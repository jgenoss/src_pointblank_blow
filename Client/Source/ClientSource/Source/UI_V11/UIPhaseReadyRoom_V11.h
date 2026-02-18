/**
 * \file	UI\UIPhaseReadyRoom_V11.h
 *
 * Declares the user interface phase ready room class.
 */


#if !defined( __UI_PHASE_READY_ROOM_V11_H__)
#define __UI_PHASE_READY_ROOM_V11_H__

#include "UI/UIPhaseBase.h"
#include "UI/UIDefine.h"
#include "UI/UIPhaseReadyRoom.h"
#include "../UI/UIUtil.h"

#define		PRIZE_SLOT_VIEW_COUNT	3
#define		COUNTDOWN_COUNT			5
#define		MAX_READYROOM_BUTTON	4

//
enum READYROOM_BTN_STATE_V11
{
	RBS_V11_UNKNOWN = -1,
	RBS_V11_TEAMCHANGE,
	RBS_V11_SEARCH_HOST_ACTIVATE,		// 방장 검색 활성상태
	RBS_V11_SEARCH_HOST_DEACTIVATE,		// 방장 검색 비활성상태

	RBS_V11_INVITE,
	RBS_V11_READY,
	RBS_V11_START,
	RBS_V11_START_CANCLE,

	RBS_V11_MAX
};

enum UI_V11_READYSTATE
{
	UIRS_V11_NONE = -1,
	UIRS_V11_READY,
	UIRS_V11_HOST,
	UIRS_V11_SHOP,
	UIRS_V11_INVEN,
	UIRS_V11_INFO,
	UIRS_V11_PLAY,
	UIRS_V11_PCCAFE,
	UIRS_V11_RS,
	UIRS_V11_PCCAFEPLUS,

};

class UIPhaseReadyRoom_V11 : public UIPhaseBase
{
	I3_CLASS_DEFINE( UIPhaseReadyRoom_V11, UIPhaseBase);

private:
	i3UIImageBoxEx *	m_pClanBattleMarkRed;
	i3UIImageBoxEx *	m_pClanBattleMarkBlue;
	i3UICheckBox *		m_pGMObserver;

	REAL32			m_SlotUpdateTime;

	bool			m_bOpenSlot[ SLOT_MAX_COUNT];
	INT32			m_nOldViewSlot;

	bool 			m_bIsHost;
	bool 			m_bEnableUpdate;
	bool 			m_bChagingTeam;
	bool 			m_bOnceChatType;
	bool 			m_bOnceChatTypeAll;
	INT32			m_nOldTeam;

	REAL32 			m_fViewPosX;
	REAL32 			m_fViewPosY;
	REAL32 			m_fViewPosZ;
	REAL32 			m_fViewAngleY;

	i3Node *		m_pCharaAttachRoot;

	UIMessageBox* 	m_pWaitInviteMB;
	UIMessageBox* 	m_pBeHostMessageBox;

	bool 			m_bChangeOptionNotify;
	bool 			m_bKickingState;

	bool 			m_bRecordWindowSliding;
	bool 			m_bInRecordWindow;
	REAL32			m_rRecordSlideTimeStamp;

	bool 			m_bInviteResult;
	bool 			m_bInviteMessage;
	bool 			m_pOnFlag;

	INT32			m_transferHostSlotIdx;
	REAL32			m_rSearchHostDelay;
	REAL32			m_rTeamChangeDelay;

	bool			m_bReadyPushed;
	bool			m_bIsReady;
	REAL32			m_rReadyDelay;			// 레디나 스타트 버튼 딜레이

	INT32			m_nSelectedSlotIdx;

	// 섬멸전<->단체전 변경 화살표 애니메이션용
	REAL32			m_circleTerm;
	INT32			m_circleIdx;
	bool			m_playCircleAnimation;

	INT32			m_IndexMap;
	INT32			m_CheckMap;
	i3SoundPlayInfo* m_pCountdownSound[COUNTDOWN_COUNT];

	REAL32			m_teamChangePosX;

	AI_MODE_STATE	m_AiModeState;

	REAL32			m_rInviteLobbyUserTimer;

	READYROOM_BTN_STATE_V11	m_ButtonState[ MAX_READYROOM_BUTTON];

	INT32			m_OldHostIndex;		///<< 이전 방장 Index 이것과 현재 Index가 다르면 방장 변경 메세지를 표시한다.
	
	//국기 표시 관련
	i3UIListView_Box * m_ListViewBox_RedTeam;
	i3UIListView_Box * m_ListViewBox_BlueTeam;

	GameUI::UserUI_NationMarkData	m_UserUI_NationMarkData;
	GameUI::NationMarkTemplate		m_UI_NationMark[SLOT_MAX_COUNT];

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;
	virtual bool	OnRevive(i3RenderContext * pCtx) override;

	/** \brief 레디하기 */
	void 			_DoReadyGame( void);

	/** \brief 방장 검색 */
	void			_DoSearchHost( void);

	/** \brief 팀 전체를 교체한다. */
	void			_DoSwapTeam();

	/** \brief 초대하기 */
	void 			_DoInviteFromLobby(void);

	/** \brief 게임 설정 */
	void 			_DoConfigRoom(void);

	/** \brief 버튼 설정 */
	void			_SetButtonState( void);
	void			_ReadyButtonActive( bool bActive );

	// 레디룸 클랜마크 프레임 사용 여부 (클랜전 - 활성화, 일반전 - 비활성화)
	void			_SetClanMarkFrame(void);

	/** \brief EVENT_BATTLE_COUNTDOWN 처리 */
	void			_EventProcess_BattlCountDown( INT32 arg , const i3::user_data&);

	void 			_ProcessCommon( REAL32 rDeltaSeconds);
	void 			_ProcessCharaInfo( REAL32 rDeltaSeconds);
	void 			_ProcessCharaDetailInfo( REAL32 rDeltaSeconds);
	void 			_ProcessMapInfo( REAL32 rDeltaSeconds);
	void 			_ProcessSlot( REAL32 rDeltaSeconds);
	void 			_ProcessClanBattle();
	void			_Process_ClanMatch_BattleCountDown(INT32 arg);
	void			_Process_Normal_BattleCountDown(INT32 arg);
	void			_ProcessInviteLobbyUser( REAL32 rDeltaSeconds);
	
	/** \brief Slot에 유저정보를 설정합니다. */
	void			_SetSlotInfo( INT32 slot);

	/** \brief 빈 공간으로 설정합니다. */
	void			_EmptySlot( INT32 slot);

	/** \brief 닫힌 공간으로 설정합니다.
		\param[in] slotNum Slot 번호
		\param[in] bClose 닫힘 유무
		\param[in] bLock 고정 유무(고정되면 Closed Button을 Hide시킨다)*/
	void 			_CloseSlot( INT32 slotNum, bool bClose, bool bLock = false);

	/** \brief Highligh 시킨다.
		\desc 자신의 슬롯만 설정되도록 합니다. */
	void 			_EffectSlot(INT32 idxSlot);

	/** \brief 방장 유무를 확인한다.
		\desc 방장이면 팝업창을..
				다른 유저는 Chatting창에 알려준다. */
	void 			CheckBeHost(void);

	bool 			_IsReady(void);
	void 			_UpdateMapInfo( ROOM_INFO_ALL * pInfo);
	void 			_UpdateTitlePassword(bool bInit = false);
	void 			_DelayInviteResponse(REAL32 rDeltaSeconds);//for OnUpdate function for Invite list updating
	void 			OnInvitedMessage(void);

	/** \brief 방 정보 변경 처리 */
	void 			_ModifyRoomOption(INT32, const i3::user_data&);

	/** \brief 자신이 팀 변경 */
	void			_ChangeTeam( INT32 team);

	// 방장이 아닌 유저가 랜덤맵 방에 입장할 경우 해당모드의 모든 랜덤맵을 추가
	void			_AddAllRandomMap();

	/** \brief GM 유무를 확인한다.
		\desc GM이면  옵저버 기능 활성화
				일반 유저는 옵저버 기능 비활성화 */
	void			_InitGMObserverEnable(void);

	void			_UpdateGMObserverControlState();

	void			_CheckEndurance(void);		// 내구도를 체크해서 메시지 출력


	//--------------------------------------------------------------------------//
	//							inline functions								//
	//--------------------------------------------------------------------------//
public:
	INT32			getSelectedSlot( void)		{ return m_nSelectedSlotIdx; }

	
public:
	UIPhaseReadyRoom_V11();
	virtual ~UIPhaseReadyRoom_V11();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	OnLoadAllScenes() override;
	virtual bool	OnKey_Escape( void) override;

	virtual void	ProcessGameEvent(GAME_EVENT event,INT32 arg, const i3::user_data& UserData) override;
	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;
	virtual void	OnGotoBackPhase( void) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd( void) override;
	virtual bool	OnExitStart( void) override;
	virtual void	OnExitEnd( void) override;

	virtual bool	OnEnterMouse( i3UIControl * pControl) override;

	virtual bool	OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code) override;

	virtual void	JoinGame(void) override { _DoReadyGame();}
	
		/** \brief 팀 변경 */
	void			OnSelectTeam( TEAM_TYPE team);
	
	void 			RunRandomMap(void);
	void 			AllUserReady();

	void			ClickSlot( INT32 idx);
	void			ClickRSlot( INT32 idx);
	void			ClickCloseButton( INT32 idx);

	void			ClickButton( INT32 btn);

	void			SetHostMark( INT32, INT32);

	// Open Popup
	// 사용중인 CashItem에 대한 알림
	void			OpenPopupUsingCashItem();

	void			CheckBattleChallengeComplate();
	void			CheckTSReward();
	

	bool			IsRoomUser( i3::rc_wstring pNick );

	// GM Observer 모드를 변경
	void			ChangeGMObserver(bool bChecked);
	// Host Mark 세팅
	void			InitHostIcon();

	//--------------------------------------------------------------------------//
	//								Error handler								//
	//--------------------------------------------------------------------------//
public:
	void 			EhKicked(bool isGM);

	//--------------------------------------------------------------------------//
	//							Callback rutine									//
	//--------------------------------------------------------------------------//
	static void 	CbBeHost(void* pThis,INT32 nParam);
	static void 	CbSearchHost(void* pThis,INT32 nParam);
	static void 	cbQueryLeaveClanBattle(void * pThis, INT32 nParam);
	static void 	CbMoveChannel(void* pThis,INT32 nParam);
	static void 	CbBlockUser(void* pThis, INT32 nParam);
	static void 	CbFinalComplateConfirm(void* pThis,INT32 nParam);
	static void 	CbMoveShop(void* pThis, INT32 nParam);


	//-----------------------------------------------------------------------------//
	//							Kick RoomMaster

	//Room Master GameStart Timer After All User Ready
private:
	bool			m_bBeHostMessageOK;
	bool			m_bPauseKickMaster;
	bool			m_bAllUserReady;
	bool			m_bRoomMsterAwayCountDown;
	INT32			m_RoomMasterAwayCheckerTime;

	CAwayChecker*	m_pRoomMasterAwayChecker;

	i3UIImageBox *	m_pHostImage;

private:
	void			__CreateRoomMasterKick( void);
	void			__CheckRoomMasterKick( void);
	void			__StopRoomMasterKick( void);
	void			__ResetRoomMasterKick( void);

	void			__ProcessKickRoomMaster( void);
	void			 _GetUserInfo(INT32);

	// 국기 표시 관련
	void			_NationInitAtLoad(i3UIScene * pScene);
	void			_Update_RankAndNaionMarkAlpha(void);

public:
	void			Set_bGameStart(bool);
	void			DecreaseRemainCount() { m_iRemainCount -= 1; SetRemainCount(m_iRemainCount); }
	void			StartCount(INT32);
	void			SetRemainCount(INT32);
	
	bool			OnInviteMove( void);

private:
	bool			m_bGameStart;
	REAL32			m_rRemainTime;
	INT32			m_iRemainCount;

	bool            m_bEnableBlockEscape;
	REAL32			m_rBlockEscapeTime;

	bool			m_bGiveupBattle; // 클랜전 중도 포기 처리용.

	// Game Event
private:
	typedef void	(UIPhaseReadyRoom_V11::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func >	m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);
	
	void			ReceiveEvent_Ready_Battle( INT32 result, const i3::user_data&);
	void			ReceiveEvent_Start_Game( INT32 result, const i3::user_data&);
	void			ReceiveEvent_GetLobbyUserInfo( INT32 result, const i3::user_data&);
	void			ReceiveEvent_EnterLobby( INT32 result, const i3::user_data&);
	void			ReceiveEvent_KickUser( INT32 result, const i3::user_data&);
	void			ReceiveEvent_Unready_4vs4( INT32 result, const i3::user_data&);
	void			ReceiveEvent_GetRoomAddInfo( INT32 result, const i3::user_data&);
	void			ReceiveEvent_DestroyNetwork( INT32 result, const i3::user_data&);
	void			ReceiveEvent_SearchHost( INT32 result, const i3::user_data&);
	void			ReceiveEvent_Request_HostChange( INT32 result, const i3::user_data&);
	void			ReceiveEvent_Transfer_Host( INT32 result, const i3::user_data&);
	void			ReceiveEvent_Item_Auth( INT32 result, const i3::user_data&);
	void			ReceiveEvent_Item_Auth_Data( INT32 result, const i3::user_data&);
	void			ReceiveEvent_User_Slot( INT32 result, const i3::user_data&);
	void			ReceiveEvent_Get_UserEquipmentInfo( INT32 result, const i3::user_data&);
	void			ReceiveEvent_Change_ReadyRoomInfo( INT32 result, const i3::user_data& UserData);
	void			ReceiveEvent_RoomScore( INT32, const i3::user_data&);
	void			ReceiveEvent_Giveup_Battle( INT32, const i3::user_data&);
	void			ReceiveEvent_PlayTimeAlarm( INT32 arg, const i3::user_data&);
	void			ReceiveEvent_ShutDownTimeAlarm( INT32 arg, const i3::user_data&);
	void			ReceiveEvent_ForceLeaveMatchroom(INT32 arg, const i3::user_data&);
};

#endif

