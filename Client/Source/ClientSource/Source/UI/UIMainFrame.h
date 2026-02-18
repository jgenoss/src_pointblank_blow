/**
 * \file	UI\UIMainFrame.h
 *
 * Declares the user interface main frame class.
 */


#if !defined( __UI_MAINFRAME_H__)
#define __UI_MAINFRAME_H__

#include "UIUserActionDef.h"
#include "UIDefine.h"

#include "UIPopupFocus.h"
#include "../UserGuideMgr.h"

class UIBase;
class UIPhaseBase;
class UIPhaseSubBase;
class UIPhaseLogin;
class UIFloatingBase;
class UIPopupBase;
class UIPopupUserActionInteractBase;
class UIToolTipBase;
class UITopMenu;
class UITopMenu_V10;
class UITopMenu_V11;
class UITopMenu_V15;
class UISideMenu;

class ItemImageMgr;
class CAwayChecker;

class CToolTip;
class CRoomMasterKickManager;

#if legacy_gui_operator
// UIMainFrame 클래스에서 구 GUI 함수들만 모와둠.
struct gui_operator
{
public:
	gui_operator() : m_pGuiRoot(0) {}

public:
	void	 _LinkControl( void);
	void	 _InitControl( void);
	void _GuiNotifyPump( void);

public:
	void OnLoadEnd();
	void LoadOldGUI( void);
	void Destroy();

public:
	i3GuiRoot* m_pGuiRoot;		// 이전 버전의 GUI입니다.
};
#endif

class UIMainFrame : public i3GameNode
{
	I3_CLASS_DEFINE( UIMainFrame, i3GameNode);

	friend struct gui_operator;

protected:
	UITopMenu *			m_pTopMenu;
	UISideMenu *		m_pSideMenu;

	bool				m_bNickSpace;

	i3::vector<UIFloatingBase *>		m_FloatWins;
	i3::vector<UIPopupBase *>			m_Popup;
	i3::vector<UIPhaseBase *>			m_Phase;
	i3::vector<UIPhaseSubBase *>		m_SubPhase;

	i3::vector<INT32>	m_FloatWinQue;					// 딱 한군데만 쓰이는데...제대로 안쓰이는것 같다...(2012.06.01.수빈)

	// Popup window
	///UIPopupBase *		m_pPopup[ UPW_MAX];

	// Phase
	//UIPhaseBase *	m_pPhases[ UIPHASE_MAX];
	//UIPhaseSubBase *	m_pSubPhases[ USP_MAX];	

	i3::vector<UIPHASE>	m_PhaseQueue;
	PhaseState			m_phaseState;
	UIPHASE				m_currentPhase;
	static UIPHASE		m_lastPhase;

	UISUBPHASE				m_CurrentSubPhase;
	UISUBPHASE				m_SubOldPhase;
	i3::vector<UISUBPHASE>	m_SubPhaseQueue;
	PhaseState				m_SubPhaseState;

	// 버튼 효과의 타이머 시작 여부
	// true이면 시간을 계산한다.
	bool			m_bUIEffectBegin;

	// UI상 Effect 효과의 여부
	// Enable되면 특정 버튼이 깜빡이도록 한다.
	bool			m_bUIEffectState;

	REAL32			m_rUIEffectActiveTime;

	bool			m_bClanMember;
	bool			m_bClanNonMember;

	bool			m_bChangeClanPopup;

	UISUBPHASE m_ExclusedEnterEventSubPhase;

	//i3::rc_wstring	m_awstrServerSlotText[ SS_MAINTYPE_MAX];

	// BGM
	UI_BGM			m_CurrentBGM;
	bool			m_bNetload;		///<<< 서버에서 Option을 받으면 설정

	// exit timer
	bool				m_bExiting;
	REAL32				m_fExitTimer;
	UIMessageBox*		m_pWaitExitingMessageBox;
	
	void			__CheckAwayInput( void);

	void			__ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData);
	void			__GameEventPump( void);

	void			__UpdatePhase( REAL32 rDeltaSeconds);
	void			__UpdateSubPhase( REAL32 rDeltaSeconds);
	
	virtual void	_EnableFloatingList() { }

	void _SideMenu_Entrance_() const;
	void _SideMenu_Exit_() const;

protected:
	/** \brief Play BGM */
	void			_PlayBGM( UI_BGM bgm);


	//--------------------------------------------------------------------------//
	//								Inline Functions							//
	//--------------------------------------------------------------------------//
public:
	UITopMenu *			GetTopMenu( void)				{ return m_pTopMenu; }
	UITopMenu_V15 *		GetTopMenu_V15( void)			{ return (UITopMenu_V15*)m_pTopMenu; }
	UITopMenu_V11 *		GetTopMenu_V11( void)			{ return (UITopMenu_V11*)m_pTopMenu; }
	UITopMenu_V10 *		GetTopMenu_V10( void)			{ return (UITopMenu_V10*)m_pTopMenu; }
	void				SetUICharaCamera(INT32 cam);
	
	UISideMenu *	GetSideMenu( void)				{ return m_pSideMenu; }

	UIPHASE			GetCurrentPhaseType( void)		{ return m_currentPhase; }
	PhaseState		GetCurrentPhaseState( void)		{ return m_phaseState; }
	UIPhaseBase *	GetCurrentPhase( void)			
	{ 
		if( m_currentPhase > UIPHASE_START && m_currentPhase < UIPHASE_MAX) 
			return GetPhase(m_currentPhase); 
		return nullptr;
	}

	UISUBPHASE		GetCurrentSubPhaseType( void)	{ return m_CurrentSubPhase; }

	// 현재 CurrentSubPhase와 SubPhaseQueue가 둘 다 없거나 None일 경우에만 None으로 되야 함.
	bool			IsSubPhaseNone()
	{
		if(m_CurrentSubPhase == USP_NONE && m_SubPhaseQueue.empty() == true)
			return true;
		else
			return false;
	}

	/** \brief 현재 SubPhase를 반환한다.
		\note nullptr일 수 있다. */
	UIPhaseSubBase*	GetCurrentSubPhase()
	{
		if( m_CurrentSubPhase != USP_NONE)
		{
			UIPhaseSubBase * pSubPhaseBase = m_SubPhase.at(m_CurrentSubPhase);
			return pSubPhaseBase;
		}
			
		return nullptr;
	}

	PhaseState		GetCurrentSubPhaseState( void)	{ return m_SubPhaseState; }

	UIPhaseBase *	GetPhase( UIPHASE phase)
	{
		I3_BOUNDCHK( phase, UIPHASE_MAX);
		UIPhaseBase * pPhasebase = m_Phase.at(phase);

		return pPhasebase;
	}

	UIPhaseSubBase *	GetSubPhase( UISUBPHASE phase)
	{
		I3_BOUNDCHK( phase, USP_MAX);

		UIPhaseSubBase * pSubPhaseBase = m_SubPhase.at(phase);
		return pSubPhaseBase;
	}

	UIFloatingBase * GetFloating( UIFLOATWIN type);

	UIPopupBase *	GetPopup( UIPOPUPWIN type)
	{
		I3_BOUNDCHK( type, UPW_MAX);
		UIPopupBase * pPopupbase =  m_Popup.at(type);
		return pPopupbase;

	}

	UIPOPUPWIN GetCurrentPopupState();

	//마지막 popup을 반환한다.
	UIPopupBase*	GetCurrentPopup() const	{	return m_popup_focus.GetLastestPopup();	}

	void			setNetLoad( bool bLoad)				{ m_bNetload = bLoad; }

	void			ChangeClanPopup( void);
	bool			isChangeClanPopup( void)			{ return m_bChangeClanPopup; }

public:
	UIMainFrame();
	virtual ~UIMainFrame();
	static  void	InitItemImageMgr(void);

	virtual void	Create( i3GameNode* pParent) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;

	
	virtual void	OnLoadEnd(i3Stage* pStage);
	virtual void	OnFinish( void);
	/** \brief Phase를 시작합니다.
		\desc Phase 시작시 보여야 하는 창이 있으면 여기에 추가하십시오.
			Phase 전환시 BGM을 변경합니다. */
	virtual void	StartPhase( UIPHASE MovePhase);
	/** \brief Phase를 종료합니다.
		\desc Phase를 종료할시 닫는 창이 있다면 여기에 추가하십시오.. */
	virtual void	EndPhase( void) = 0;
	/** \brief Phase를 변경합니다.
		\desc 다음 Frame에서 변경되어짐.. */
	virtual void	MovePhase( UIPHASE Phase);
	
	/** \brief SubPhase를 시작하기 전 호출
		\desc SubPhase의 전에 퇴장하는 윈도우들을 퇴장시켜줍니다. 
		(1.0, 1.1), 1.5 각각 행동이 다르므로 virtual로 빼겠습니다*/
	virtual void	PreStartSubPhase( UISUBPHASE phase) {};
	
	/** \brief SubPhase의 퇴장 완료시 호출 
	    (1.0, 1.1), 1.5 각각 행동이 다르므로 virtual로 빼겠습니다*/
	virtual void	EndSubPhase( UISUBPHASE phase) {};

	/** \brief SubPhase 진입시 호출
		(1.0, 1.1), 1.5 각각 행동이 다르므로 virtual로 빼겠습니다*/
	virtual void	EntranceSubPhase( void) {};

	/** \brief SubPhase의 퇴장시 호출
		 (1.0, 1.1), 1.5 각각 행동이 다르므로 virtual로 빼겠습니다*/
	virtual void	ExitSubPhase( void) {};

	/** \brief 좌측하단의 위치표시 */
	virtual void	UpdateLocation( void);


	/** \brief */  // 이것은 클래스 생성전에도 들어갈수 있기 때문에, static함수로 두고, static멤버변수를 변경합니다. ( 11.10.14. 수빈)
	static void		SetStartPhase( UIPHASE Phase);

	void			ChangeSubPhase( UISUBPHASE phase);

	
	void			SetExitTimer( bool bSet, float fTime = 3.0f );

	void			GameEventTransferToSubPhase( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData);

	void			GameEventTransferToFloatingWin( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData);

	void			EnableFloatWin( UIFLOATWIN type, void * pArg1 = nullptr, void * pArg2 = nullptr);
	void			DisableFloatWin( UIFLOATWIN type);
	void			DisableFloatWinInput( UIFLOATWIN type,bool bTrue);	
	BOOL			IsEnableFloatWin(UIFLOATWIN type);

	/** \brief Popup Window를 Enable/Disable시킨다.
		\param[in] bOnTop 최상위 Popup 유무. true이면 나머지 WindowFrame에 대해 Input을 Disable하게 됩니다.
		\param[in] pArg1 Argument
		#param[in] pArg2 Argument */
	void			TogglePopup( UIPOPUPWIN popupType, void * pArg1 = nullptr, void * pArg2 = nullptr);

	/** \brief 강제로 Popup을 띄웁니다. */
	void			OpenPopup( UIPOPUPWIN popupType, void * pArg1 = nullptr, void * pArg2 = nullptr);

	/** \brief 강제로 Popup을 닫습니다. */
	void			ClosePopup( UIPOPUPWIN popupType);
	void			CloseAllPopup();

	bool			IsOpenedPopup( UIPOPUPWIN type);
	UIPOPUPWIN		FindPopup( LuaState * L);
	UIPOPUPWIN		FindPopupByPointer( UIPopupBase * pPopup);

	bool			IsPossibleToOpenPopup(void);

	// Popup을 Focus로 하여 나머지 윈도우에 대해 input을 막는다.
	void	FocusPopup(UIPopupBase* popup, bool bFocus, LPCSTR szFile = nullptr, int nLine = 0);
	bool	IsPopupFocused() const { return m_popup_focus.IsAnyPopupOpened(); }
	bool	IsPopupFocused(UIPopupBase* popup) const { return m_popup_focus.IsActivePopup(popup); }
	bool	IsFocusedTopPopup(UIPopupBase* popup) const { return m_popup_focus.IsTopPopup(popup); }
	void	InputDisableMainUI(bool bEnable);
	void	InputDisableFloatWindow(bool bEnable);

	/** \brief Nick Name 설정중 확인 */
	bool			IsCreatingNick( void);

	UIBase *		FindUIBase( LuaState * L);

	void			GameEventTransferToPopup( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData);

	void			GetCurrentSeverInfo( i3::stack_wstring& outServerInfo );

	/** \brief character view의 character를 Ceremony animation하도록 한다. */
	void			PlayCeremonyCharacter( void);

	/** \brief Popup이 열려 있으면 닫아주고 false를 반환
		\desc 일부 SubPhase와 Popup이 열려 있는 경우 해당 창을 닫고 false를 반환하게됩니다.
				종료 Popup은 true를 반환해야 나옴..*/
	bool			EscapeKey( void);

	/** \brief 종료 Popup을 띄웁니다. */
	void			OnEscape( void);

	// 버튼 효과
	void			SetEffectBegin(void) { m_bUIEffectBegin = true;}
	void			SetEffectButtonBegin(void) { m_bUIEffectBegin = true; 	m_rUIEffectActiveTime = 0.f;   }
	void			SetEffectButtonStop(void)  { m_bUIEffectBegin = false; 	m_rUIEffectActiveTime = 0.f;   }
	bool			GetButtonEffect() { return m_bUIEffectState; }

	void			UpdateEffectTime( REAL32 rDeltaSeconds);

	void	 ExclusedEnterSubPhase( UISUBPHASE phase);
	void	 EnterDelayedEventSubPhase(void);
	void	 LeaveSubPhase( UISUBPHASE phase);
	bool	 EventEnterSubPhase(UISUBPHASE phase);

	// Profile 입장중인지 확인
	bool IsEnteredSubPhase(UISUBPHASE sub);

	void ExclusedEnterPopup( UIPOPUPWIN popup);
	void EnterDelayedEventPopup(void);
	void LeavePopup( UIPOPUPWIN popup);

	/**	아이템 인증 (무기는 장비) */
	void			AuthStartSuccess(INT32 arg);

	// Announce Message
	void			SetSlideNotice_Permanant( const i3::rc_wstring& wstrMsg);
	void			SetSlideNotice_Temporary( const i3::rc_wstring& wstrMsg);

	void			StartHighlightMailbox( void);
	void			StartAlarmNote( void);
	
	/** \brief 초대하기에 의한 Phase 이동을 한다. */
	void			OnInviteMove( void);

	/** \brief Clan 초대하기에 의한 Phase 이동을 한다. */
	void			OnInviteMoveClan( void);

	/** \brief Chatting window를 업데이트한다. */
	virtual void	UpdateChatBuffer( void) { }
	/** \brief Chatting window의 MegaPhone 을 업데이트한다. */
	virtual void	UpdateMegaPhoneBuffer(void) { }

	/** */
	bool			isLockedInput( void){ return m_bLockInput; };
	void			SetLockInput( bool Sig){ m_bLockInput = Sig;};

	//--------------------------------------------------------------------------//
	//							V10 / V11	User Action							//
	//--------------------------------------------------------------------------//
	virtual void	ResetUserActionPopup( void) { }
	virtual void	AddUserActionCommand( USER_CONTEXT_MENU cmd) { }
	virtual void	OpenUserActionPopup( void * pEventReceiver, INT32 slotIdx) { }

	/** \brief 다른 Control 입력에 대한 Popup을 설정한다. 
		\desc Popup이 열려 있는 경우 강제 입력이 아니라면 그 popup을 닫는다. */
	void			ClosePopupByAnotherControlInput( i3UIControl * pControl);

	bool			IsPhaseAndSubPhaseSlideDone();
	//--------------------------------------------------------------------------//
	//								Away Checker								//
	//--------------------------------------------------------------------------//
private:
	CRoomMasterKickManager* m_pRoomMasterAwayCheckerManager;

	CAwayChecker*	m_pAwayChecker;
	UIMessageBox*	m_pAwayMessageBox;

//	List			m_CurrPopupList;
	UIPopupFocus	m_popup_focus;
		

	bool			m_bProcessLeaveRoom;

	UISUBPHASE		m_oldAwayPhase;

	bool			m_bKickByNoStartGame;		// 방장의 경우 게임 시작을 안해 강퇴된 경우
	bool			m_bLockInput;
	REAL32			m_rLockInputTimer;			// Phase Lock 이 시간 동안 입력 무시

	bool			m_bDeprotationClanChannel;  // 채널리스트에서 강제로 제명 당함.
	i3::rc_wstring  m_sDeprotationClanMessage;	// 채널리스트에서 강제로 제명 당함.

public:
	CRoomMasterKickManager*	GetRoomMasterAwayCheckerManager() { return m_pRoomMasterAwayCheckerManager; }

	void			OpenAwayMessageBox(const i3::rc_wstring& wstrMessage);
	void			CloseAwayMessageBox();
	
	void			setProcessLeaveRoom(const bool bFlag)		{ m_bProcessLeaveRoom = bFlag; if( bFlag){m_oldAwayPhase = m_CurrentSubPhase;}}
	void			setKickBtNoStartGame( bool bHost)			{ m_bKickByNoStartGame = bHost; }
	bool			getDeprotationClanChannel()					{ return m_bDeprotationClanChannel; }
	void			setDeprotationClanChannel(const bool bFlag, const i3::rc_wstring& message) 
					{
						m_bDeprotationClanChannel = bFlag; 
						m_sDeprotationClanMessage = message;
					}

private:
#if legacy_gui_operator
	gui_operator m_gui_operator; //구 gui
#endif

#if legacy_not_use_operator
	// 마우스 휠 값
	REAL32			m_rWheelDelta;
	void			__ProcessWheel( void);
	void			__CheckCheating( void); // 메모리 변조 체크
#endif


};

#endif

