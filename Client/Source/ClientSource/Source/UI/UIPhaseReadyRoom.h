/**
 * \file	UI\UIPhaseReadyRoom.h
 *
 * Declares the user interface phase ready room class.
 */


#if !defined( __UI_PHASE_READY_ROOM_H__)
#define __UI_PHASE_READY_ROOM_H__

#include "UIPhaseBase.h"
#include "room_detail.h"
#include "UIDefine.h"

#include "CSI_Rule.h"

#define		PRIZE_SLOT_VIEW_COUNT	3

//
enum READYROOM_BTN_STATE
{
	RBS_UNKNOWN = -1,
	RBS_SEARCH_HOST,		// 방장 검색 버튼
	RBS_INVITE,				// 초대 버튼
	RBS_ROOMSET,			// 방 설정 버튼

	RBS_TEAMCHANGE,			// 팀 교체 버튼
	RBS_READY,				// 레디버튼
	RBS_START,				// 게임 시작 버튼

	RBS_MAX
};

enum UI_READYSTATE
{
	UIRS_NONE = -1,
	UIRS_SETTING,
	UIRS_READY,
	UIRS_PLAY,
};

enum UI_READYSTATE_V10
{
	UIRS_V10_NONE = -1,
	UIRS_V10_READY,
	UIRS_V10_PLAY,
	UIRS_V10_SHOP,
	UIRS_V10_INVEN,
	UIRS_V10_INFO,
	UIRS_V10_CLAN,
};


enum READY_ROOM_TYPE
{
	ROOM_T_NORMAL = 0,
	ROOM_T_DOMI,

	ROOM_T_MAX
};

enum COMBO_TYPE { 
	COMBO_RULE = 0,			// 게임 룰
	COMBO_STAGE,			// 스테이지
	COMBO_ROUND,			// 승리조건(킬/라운드)
	COMBO_TIME,				// 승리조건(제한 시간)
	COMBO_TEAM_CHANGE,		// 공수교대
	COMBO_AI_LEVEL,			// AI모드 난이도
	COMBO_AI_COUNT,			// AI모드 인원수
	COMBO_MAX
};

class CAwayChecker;

struct CBBoxData
{
	i3::vector<INT32>	i32Idx;
	i3::vector<INT32>	_value;
	i3::vector<UINT8>	_type;	// Round, Time 을 계산해야 할 경우에 대비한 값

	CBBoxData(void)
	{
		i32Idx.clear();
		_value.clear();
		_type.clear();
	}


	// set함수는 Add함수의 기능을 포함합니다
	void SetData(INT32 idx, INT32 value)
	{
		if( idx < (INT32)i32Idx.size() )
		{
			i32Idx[idx] = idx;
			_value[idx] = value;
		}
		else
		{
			i32Idx.push_back(idx);
			_value.push_back(value);
		}
	}
	void SetData_subType(INT32 idx, UINT8 value)
	{
		if(  idx < (INT32)i32Idx.size()  )
		{
			i32Idx[idx] = idx;
			_type[idx] = value;
		}
		else
		{
			i32Idx.push_back(idx);
			_type.push_back(value);
		}
	}

	void ClearData()
	{
		_value.clear();
		_type.clear();
		i32Idx.clear();
	}

	INT32 GetData(INT32 idx)
	{
		return _value[idx];
	}

	UINT8 GetData_subType(INT32 idx)
	{
		return _type[idx];
	}
};

class UIPhaseReadyRoom : public UIPhaseBase
{
	I3_CLASS_DEFINE( UIPhaseReadyRoom, UIPhaseBase);

	friend class room_base;
	friend class room_normal;
	friend class room_domi;
	friend class room_normal_V10;

protected:
	room_base*			m_room_detail[ROOM_T_MAX];
	room_base*			m_curr_room;

	CBBoxData			m_cb_Detail[COMBO_MAX];
	i3UIComboBox*		m_cbBox[COMBO_MAX];
	i3UIButton*			m_btnRoomSet;

	i3UIFrameWnd*		m_hostFmw;
	char				pw[NET_ROOM_PW];
	i3UIEditBox*		m_pPwEditBox;
	i3UIEditBox*		m_pTitleEditBox;
	i3::rc_wstring		m_strTitle;

	void				SetRoomTitle();

	void				SetLuaPasswordOpt();							// 방 비번 옵션 설정 루아 적용
public:
	READY_ROOM_TYPE		get_room_type( STAGE_ID e);
	void				change_room_detail( STAGE_ID e);
	room_base*			get_room_detail( READY_ROOM_TYPE e) { I3_BOUNDCHK( e, ROOM_T_MAX); return m_room_detail[e];}
	room_base*			getCurrRoom()	{	return m_curr_room;	}

protected:
	UIMessageBox* 		m_pBeHostMessageBox;
	INT32				m_transferHostSlotIdx;
	REAL32				m_rSearchHostDelay;
	INT32				m_OldHostIndex;				///<< 이전 방장 Index 이것과 현재 Index가 다르면 방장 변경 메세지를 표시한다.	

	REAL32				m_rInviteLobbyUserTimer;

	i3SoundPlayInfo*	m_pCountdownSound[COUNTDOWN_COUNT];

	UIMessageBox* 		m_pWaitInviteMB;
	i3UICheckBox *		m_pGMObserver;

	INT32				m_nSelectedSlotIdx;
	i3UIButton *		m_pBtnReady;
	i3UIButton *		m_pBtnStart;

	bool 				m_bInviteMessage;
	bool 				m_bInviteResult;
	bool 				m_bEnableUpdate;
	bool				m_bReadyEnable;
	bool				m_bQueJumpToBattle;	// 배틀 점프 예약 (아바타 로딩이 완료 되면 점프한다)
	bool	 			m_bIsHost;
	bool 				m_bKickingState;		// 강퇴 중 파업 중복 호출 막기위한 플래그 - 킥시점 true, LOBBY_ENTER응답시 false
	bool				m_bLeaveTeamState;		// 용병이 배틀을 마치고 난 후, 강제로 레디룸에서 나가는 중에 맵 정보 업데이트를 막기 위한 플래그

	bool				m_bGiveupBattle; // 클랜전 중도 포기 처리용.

	enum{ UserListMaxFrm = 6 }; //PointBlankRe_ReadyRoom_TeamSelect + PBRe_Domi_TeamSelect 에 사용되는 모든 FrameWnd 수
	i3UIFrameWnd *		m_pMainWnd[UserListMaxFrm];

	i3UIFrameWnd *		m_pTitleWnd;
	UINT8				m_SubType;
	//i3::rc_wstring		m_aawstrServerText[ SS_MAINTYPE_MAX][ SS_SUBTYPE_COUNT];

	CSI_RULE			m_CurStageRule;
	STAGE_ID			m_nCurStageID;
	INT32				m_nCurComboIdx[COMBO_MAX];	// 콤보박스 관련. 현재 위치한 셀의 Index

	// Util
	ROOM_INFO_ALL		GetCurRoonInfo();			// 너무 자주 사용되어 추가함
	void				Departured();

protected:
	void			_SetButtonTooltip( void);
		
	/** \brief 방장 검색 */
	void			_DoSearchHost( void);

	/** \brief 초대하기 */
	void 			_DoInviteFromLobby(void);

	/** \brief EVENT_BATTLE_COUNTDOWN 처리 */
	void			_EventProcess_BattlCountDown( INT32 arg, const i3::user_data&);

	void			_Process_Normal_BattleCountDown(INT32 arg);
	void			_Process_ClanMatch_BattleCountDown(INT32 arg);

	void 			_UpdateMapInfo( ROOM_INFO_ALL * pInfo);
	void 			_UpdateTitlePassword(bool bInit = false);
	void 			_DelayInviteResponse(REAL32 rDeltaSeconds);//for OnUpdate function for Invite list updating
	void 			OnInvitedMessage(void);

	/** \brief 방 정보 변경 처리 */
	void 			_ModifyRoomOption( INT32, const i3::user_data&);
	void			_ProcessInviteLobbyUser( REAL32 rDeltaSeconds);

	/** \breief SubType 초기값 받아오기 */
	void			_GetMissionRoundType(UINT8& subtype, STAGE_MODE mode);
	/** \breief AiMode로 레디룸에서 바뀐다면 해줘야 하는 처리*/
	void			_SetAiModeConditon(STAGE_ID stageid);

	//--------------------------------------------------------------------------//
	//							inline functions								//
	//--------------------------------------------------------------------------//
public:
	INT32			getSelectedSlot( void)		{ return m_nSelectedSlotIdx; }

	
public:
	UIPhaseReadyRoom();
	virtual ~UIPhaseReadyRoom();

	void 			RunRandomMap(void);
	void 			AllUserReady();
	static void 	PopupTabSkill(void * pThis, INT32 nParam);


//-------------------------------------------------------------------------
//기존 ready room 글루 함수 room_normal 일 경우에만 호출 됨.
	void			ClickSlot( INT32 idx);
	void			ClickRSlot( INT32 idx);
	void			ClickCloseButton( INT32 idx);

//-------------------------------------------------------------------------
//기존 ready room 글루 함수 room_domi 와 같이 사용
	void			ClickButton( INT32 BtnIdx);

	// Open Popup
	// 사용중인 CashItem에 대한 알림
	static void		OpenPopupUsingCashItem();

	void			CheckTSReward();
	void			CheckQuickMedalReward(void);
	void			Apply_Password();
	
	bool			IsRoomUser( const char* pNick) { return m_curr_room->is_room_user( pNick);}	

	void			AdjustRandomMapData(ROOM_INFO_ALL &Info ) const;

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
	static void		CbDomiEnter(void* pThis, INT32 nParam);

	//-----------------------------------------------------------------------------//
	//							Kick RoomMaster

	//Room Master GameStart Timer After All User Ready
private:
	bool			m_bBeHostMessageOK;
	
private:
	void			 _GetUserInfo(INT32);

public:
	void			Set_bGameStart(bool);
	void			DecreaseRemainCount() { m_iRemainCount -= 1; SetRemainCount(m_iRemainCount); }
	void			StartCount(INT32);
	void			SetRemainCount(INT32);
	
	bool			OnInviteMove( void);

	const bool		IsBattleCountDown(void) { return (m_iRemainCount > 0) || (m_bGameStart == true); }

private:
	bool			m_bGameStart;
	REAL32			m_rRemainTime;
	INT32			m_iRemainCount;

public:
	//bool			_IsAlone();											//m_curr_room->is_alone()
	virtual void			JoinGame(void) { m_curr_room->do_ready_game();}


	//GM Observer 모드
	void			_InitGMObserverEnable( void);
	void			_UpdateGMObserverControlState(void);	
	void			ChangeGMObserver(bool bChecked);


private:
	typedef void	(UIPhaseReadyRoom::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func > m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveEvent_Ready_Battle( INT32 result, const i3::user_data&);
	void			ReceiveEvent_Start_Game( INT32 result, const i3::user_data&);
	void			ReceiveEvent_GetLobbyUserInfo( INT32 result, const i3::user_data&);
	void			ReceiveEvent_EnterLobby( INT32 result, const i3::user_data&);
	void			ReceiveEvent_KickUser( INT32 result, const i3::user_data&);
	void			ReceiveEvent_Unready_4vs4( INT32, const i3::user_data&);
	void			ReceiveEvent_GetRoomAddInfo( INT32, const i3::user_data&);
	void			ReceiveEvent_DestroyNetwork( INT32, const i3::user_data&);
	void			ReceiveEvent_SearchHost( INT32 result, const i3::user_data&);
	void			ReceiveEvent_Request_HostChange( INT32 result, const i3::user_data&);
	void			ReceiveEvent_Transfer_Host( INT32 result, const i3::user_data&);
	void			ReceiveEvent_Item_Auth( INT32 result, const i3::user_data&);
	void			ReceiveEvent_Item_Auth_Data( INT32 result, const i3::user_data&);
	void			ReceiveEvent_User_Slot( INT32, const i3::user_data&);
	void			ReceiveEvent_Get_UserEquipmentInfo( INT32 result, const i3::user_data&);
	void			ReceiveEvent_Change_ReadyRoomInfo( INT32 result, const i3::user_data& UserData);
	void			ReceiveEvent_Enter_Domination( INT32, const i3::user_data&);
	void			ReceiveEvent_RoomScore( INT32, const i3::user_data&);
	void			ReceiveEvent_Giveup_Battle( INT32, const i3::user_data&);
	void			ReceiveEvent_PlayTimeAlarm( INT32 arg, const i3::user_data&);
	void			ReceiveEvent_ShutDownTimeAlarm( INT32 arg, const i3::user_data&);
	void			ReceiveEvent_PCCafeChangedStatus( INT32 arg, const i3::user_data&);
	void			ReceiveEvent_ForceLeaveMatchroom(INT32 arg, const i3::user_data&);
	void			ReceiveEvent_Get_UserEquipmentInfo_V10( INT32 result, const i3::user_data&);

public:
	virtual void		OnCreate( i3GameNode * pParent) override;
	virtual void		_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void		_ClearAtUnload( i3UIScene * pScene) override;
	virtual bool		OnRevive( i3RenderContext * pCtx) override;

	virtual void		ProcessGameEvent( GAME_EVENT evt,INT32 arg, const i3::user_data& UserData) override;
	virtual void		OnUpdate(REAL32 rDeltaSeconds) override;
	virtual void		OnGotoBackPhase( void) override;

	virtual bool		OnKey_Escape( void) override;
	virtual bool		OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void		OnEntranceEnd( void) override;
	virtual bool		OnExitStart( void) override;
	virtual void		OnExitEnd( void) override;

	virtual bool		OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code) override;
	virtual bool		OnPlayClickSound(i3UIControl * pCtrl) override;

	virtual void		SetFrmWndPos();							// Frame window 정렬
	virtual void		UpdateCBBoxInfo(COMBO_TYPE BoxType);

	virtual void		ApplyLua_cbbox(COMBO_TYPE _cType, const i3::wliteral_range& wstrCondition, INT32 _cel);
	virtual void		ApplyLua_auto_team_change_enable(UINT32 stageID);
	virtual void		ChangeGameMode(INT32 _mode, INT32 _celidx);

	virtual void		SetLuaText();
	virtual void 		CheckBeHost(void){};
};

#endif
