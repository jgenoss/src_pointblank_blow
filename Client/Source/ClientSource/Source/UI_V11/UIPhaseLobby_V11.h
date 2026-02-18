/**
 * \file	UI\UIPhaseLobby_V11_V11.h
 *
 * Declares the user interface phase lobby class.
 */


#if !defined( __UI_PHASE_LOBBY_V11_H__)
#define __UI_PHASE_LOBBY_V11_H__

#include "UI/UIPhaseBase.h"
#include "UI/UIDefine.h"
#include "UI/UIPhaseLobby.h"
#include "UI/UIMainFrame.h"


class UIPopupNickNameSetting;
//class UIToolTipLobbyMapInfo;
class UILoadingIcon;


enum UI_V11_LOBBY_BUTTON
{
	ULB__V11_NONE = -1,
	ULB__V11_TUTORIAL,		// 튜토리얼
	ULB__V11_QUICK,			// 퀵조인
	ULB__V11_REFRESH,		// 새로고침
	ULB__V11_CREATEROOM,		// 방생성 버튼
	ULB__V11_ENTERROOM,		// 입장가능 버튼
	ULB__V11_CLANMATCH,		// 클랜 매치

	ULB__V11_MAX
};


class UIPhaseLobby_V11 : public UIPhaseBase
{
	I3_CLASS_DEFINE( UIPhaseLobby_V11, UIPhaseBase);

private:
	REAL32			ButtonTermTime;
	bool			m_bAllowRefresh;
	bool			m_bWaitingFromServer;

	i3::vector<LOBBY_ROOM_INFO*> m_RoomList;
	REAL32			m_rEnterLobbyWaitTime;

	// Room List 받는중 message
	//UIMessageBox*	m_pWaitRoomListMassageBox;
	UILoadingIcon *	m_pWaitRecevingRoomList;
	UIMessageBox*	m_pKickedMassageBox;

	// 확장된 방생성 팝업
	UINT32			m_nStageID;
	UINT8 			m_SubType;
	UINT8 			m_LockWeapon;
	UINT8 			m_LockInterEnter;
	UINT8 			m_LockObserver;
	UINT8 			m_LockTeamBalance;

	// 초대 받음
	INVITE_INFO		m_InviteInfo;
	bool			m_bCheckInvite;

	bool			m_bEnableCreateRoomMsgBox;
	bool			m_bEnableUsingClanChannel;

	INT32			m_SelectedElement;	// Room Index
	bool			m_bQueRoomInfo;		// RoomInfo 창이 큐잉되면 true,
	bool			m_bRefreshButtonActive;	// 리프레시버튼을 누르고, 응답받을때까지 true
	REAL32			m_fRefreshElapsedTime;

	bool			m_bFirstLogin;
	bool			m_bLogin;

	bool			m_bRequestRoomInfoAdd;
	bool			m_bNickNameCreate;
	REAL32			m_LastRefreshTime;

	// 로비 방 정보 툴팁입니다.
	//UIToolTipLobbyMapInfo *	m_pTTMapInfo;

	// 입장하는 방의 정보
	ROOM_INFO_BASIC	m_JoinTargetRoom;

	// 어뷰징 관련
	UIMessageBox *	m_pAbusingBox;
	REAL32			m_rAbuseTime;

	i3UIButton *		m_pBtnTutorial;
	i3UIButton *		m_pBtnTeamLIst;
	i3UIListView_Box*	m_pItemListBox;
	bool			m_bTutorialEnable;
	bool m_is_sort_room_list; //서버로 부터 방정보 받고, 방 정보 정렬 여부

	CLAN_MEMBER_LEVEL_TYPE m_PreClanMemberLevel; // 이전 클랜멤버 상태

	// 방 목록 스크롤
	i3UIScrollBar * m_pScrollRoomList;

	// 입장 가능 or 불가능 방 소팅을 위한 저장 변수
	bool			m_bViewAdmissionRoom;			

	static SORT_ORDER_INFO sm_SortOrder[MAX_SORT_TYPE];
	static INT32	sm_nCurSortType;
	static bool		sm_bAwayInput;
	static bool		m_bPcCafeNotice;				// PcCafe 공지 창을 보여주는지에 대한 여부
	static bool		m_bEventCardSetNotice;
	static bool		m_bEvnetNotice;

	void			__AutoRefreshRoomList( REAL32 rDeltaSeconds);
	void			__ProcessAbusing( REAL32 rDeltaSeconds);

protected:
	void			_SetButtonState( void);
	void			_SetCurSelectItem(void);

	// Room List
	void			_BuildRoomListDummy(void);
	void			_UpdateRoomList(void); // Room Update: AutoRefresh
	void			_SortRoomList(void); // Room Sort: Refresh
	void			_EmptyRoomList(void);
	void			_WaitConnect(REAL32 rDeltaSeconds);
	void			_CloseWaitMessageBox(void);

	void			_ProcessFirstLogin( void);
	void			_ProcessCheckInvite(REAL32 rDeltaSeconds);

	void			_ProcessCommon( REAL32 rDeltaSeconds);

	void			_InitRoomList(void);
	void			_SetRoomList( void);
	void			_SetRoomInfo( INT32 idx, INT32 RoomIdx, ROOM_INFO_BASIC* pInfo);
	void			_SetEmptyRoomInfo( INT32 idx);
	void			_SetRoomColor( INT32 RoomIdx, I3COLOR * pColor);
	void			_RestoreRoomColor(INT32 RoomIdx);

	void			_FirstLoginPopup( void);

	void			_CloseAllSubPhaseAndPopup( void);

	void			_ClearRoomInfo( void);

	void			_SetUserFilterCallback( bool bEnter);
	static void		UserFilterCallback(i3UIControl * pThis);
	void			ChangeUserFilterPosition(i3UIListView_Box* pThis);

	void			_ClearRoomList( INT32 count);

	// SubPhase-> Phase 전환간 Msg Box가 Floating Window보다 하단에 있기때문에
	// 강제로 위치를 변경해줍니다.
	void			_AddMsgBoxToTopMenuScreen(void);

	//------------------------------------------------------------------------------//
	//								Inline Functions								//
	//------------------------------------------------------------------------------//
public:
	// 입장하는 Room의 정보
	const ROOM_INFO_BASIC &	GetJoinTargetRoom()	{ return m_JoinTargetRoom; }

public:
	UIPhaseLobby_V11();
	virtual ~UIPhaseLobby_V11();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void	OnUpdate_NoneRsc(REAL32 rDeltaSeconds) override;

	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData) override;

	// 입장 시작
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArv2 = nullptr) override;

	// 입장중
	virtual bool	OnEntrancing( REAL32 rDeltaSeconds) override;

	// 입장 완료
	virtual void	OnEntranceEnd( void) override;

	virtual bool	OnExitStart( void) override;

	virtual void	OnGotoBackPhase( void) override;

	virtual bool	OnDblClick( i3UIControl * pControl) override;

	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;

	void			ClickButton( INT32 BtnIdx);

	/** \brief Tutorial입장*/
	void			EnterTutorial( void);

	void			EnterAIMode(void);

	// 방 만들기 취소
	void			OnCreateRoomCancel( void);

	// 방에 입장
	void			EnterRoom( INT32 idxRoom);

	void			SendEventQuickJoin( void);

	// select room
	void			OnClickRoom( INT32 idxRoom);

	// NickName 설정 Popup을 띄운다.
	void			ShowNicknamePopup( void);

	// PcCafe용 Popup을 띄운다.
	void			ShowPcCafePopup(void);

	// EventItem지급용 Popup을 띄운다.
	void			EventItemPopup(void);

	// Event 출석체크
	void			ShowAttendancePopup(void);

	// 공지 사항 창을 띄운다.
	void			ShowNoticePopup(void);

	static bool		CompareRoom( const LOBBY_ROOM_INFO* pItem1, const LOBBY_ROOM_INFO* pItem2);
	static bool		CompareAdmissionRoom(const LOBBY_ROOM_INFO* pItem1, const LOBBY_ROOM_INFO* pItem2);

	bool			OnInviteMove( void);

	void			SelectRoom( INT32 idx);

	void			RefreshSelectedRoom( void);

	void			OnRefreshButton();

	// 방 목록 스크롤
	void			OnScrollRoomList(void);
	void			OnWheelRoomList(INT32 zDelta);
	INT32			GetRoomListScrollPos(void)	{ return m_pScrollRoomList->getScrollPos(); }

	void			SortRoomList(INT32 nType , INT32 bAscend);
	void			SortAdmissionRoom(bool bViewAdmissionRoom);

	/** \brief AbusePopup에서 OK하면 호출 */
	void			ConfirmAbuseOK( void);

	void			DisableRoomTooltip(void);

	void			EnableAllowRefresh(bool enable) { m_bAllowRefresh = enable; }

	//------------------------------------------------------------------------------//
	//								Error Handler									//
	//------------------------------------------------------------------------------//
public:
	void			EhWrongQuickJoin(void);
	void			EhSlotFull(void);
	void			EhKickedByVote(void);
	void			EhWrongPW(void);
	void			EhNotExist(void);
	void			EhLimitInterenter(void);
	void			EhLimitLever(void);
	void			EhLimitTraining(void);
	void			EhLimitMERC(void);
	void			EhStageError(INT32 Event,INT32 Arg);
	void			TutorialPopup( void);

	static void		CbSameNick(void* pThis,INT32 nParam);
	static void		CbBeInvited(void* pThis,INT32 nParam);
	static void		CbSuppliedConfirm(void* pThis,INT32 nParam);
	static void		CbEventCardSetConfirm(void* pThis,INT32 nParam);
	static void		CbClanInvite(void* pThis, INT32 nParam);
	static void		CbBlockUser(void* pThis, INT32 nParam);
	static void     CbTutorialMode(void* pThis, INT32 nParam);
	static void		CbAIMode(void* pThis, INT32 nParam);
	static void		CbKickedByVote( void * pThis, INT32 nParam);
	// Key : 출석 체크 추가
	static void		CbShowAttendancePopup(void * pThis, INT32 nParam);
	static void		CbOnGotoBackPhase(void * pThis, INT32 nParam);

	/// 이전 UI
private:
	bool			m_bEnableNicknameMsgBox;

public:
	void			CreateNickName();
	void			ResetSelectedElement()
	{
		m_SelectedElement = -1;
		GetMainFrame()->EnableFloatWin( UFW_LOBBYBANNER);
	}


private:
	typedef void	(UIPhaseLobby_V11::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func >	m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveGameEvent_Ready_Battle( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Join_Quick( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Create_Room( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Join_Room( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Leave_Lobby( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Get_RoomAddInfo( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_DestroyedNetwork( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Update_UserList( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Ready_Box_Message( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Attendance_Clear( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_PlayTimeAlarm( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_ShutDownTimeAlarm( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Create_NickName( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Create_Chara( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Enter_Temp( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Leave_Temp( INT32 arg, const i3::user_data&);
};

#endif

