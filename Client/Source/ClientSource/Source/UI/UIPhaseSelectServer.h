#if !defined( __UI_PHASE_SELECT_SERVER_H__)
#define __UI_PHASE_SELECT_SERVER_H__

#include "UIPhaseBase.h"

#define		SERVER_LIST_SLOT_HORI_COUNT		4
#define		SERVER_LIST_SLOT_VERT_COUNT		6
#define		INVALID_SERVER_INDEX			-1

#define		MAX_SERVER_TAB					6

enum SERVER_GAUGE
{
	SERVER_GAUGE_1		= 0, 
	SERVER_GAUGE_2,
	SERVER_GAUGE_3,
	SERVER_GAUGE_4,
	SERVER_GAUGE_5,
	SERVER_GAUGE_6,
	SERVER_GAUGE_7,
	SERVER_GAUGE_8,
	SERVER_GAUGE_9,
	SERVER_GAUGE_FULL,

	MAX_SERVER_GAUGE
};

class UILoadingIcon;

class UIPhaseSelectServer : public UIPhaseBase
{
	I3_CLASS_DEFINE( UIPhaseSelectServer, UIPhaseBase);

	friend class CGameUnitTest_UI;

private:
	INT32			m_SelectedServerIndex;

	REAL32			m_LastRefreshTime;
	SChannelType	m_CurrentTab;

	UILoadingIcon *	m_pInfoWaitIcon;
	//UIMessageBox *	m_pMBInfoWait;				// 서버 리스트 업데이트 웨이트 message
	INT32			m_ConnectFailedCount;		// 일회 게이트웨이서버로 접속 시도시에 일어난 컨넥트 실패 횟수
	INT32			m_ConnectFailedTotalCount;	// 게임서버 -> 게이트웨이 접속이 실패한 총 횟수.

	INT32			m_PrevServerIndex;
	SChannelType	m_PrevServerTab;

	INT32			m_TargetServerIndex;
	SChannelType	m_TargetServerTab;

	i3::string		m_ServerPassword;
	

	i3::vector<SChannelType> m_TabCheck;
	//SChannelType	m_TabCheck[ MAX_SERVER_TAB];

	i3UIListView_Box *	m_pServerListBox;

	bool			m_bChinaMessage;
	bool			m_bMemoryMessage;
	bool			m_bOnceChina;
	bool			m_bOnceMemoryCheck;


	bool			m_bViewChannelList;	

private:
	void			__SetServerInfo( INT32 slot, const i3::wliteral_range& wNameRng, COLOR * pColor/*, INT32 EventType, INT32 PeopleCount*/);

	i3::rc_wstring __GetServerTabTypeText( SChannelType TabType) const;
	i3::rc_wstring	__GetServerTypeText( SChannelType ServerType ) const;

	void			__AutoServerSelect( );
	INT32			__GetCurrnetTabIndex(UINT32 targetIdx);

	// 제거 예정
	//SERVER_GAUGE	__CalcuGaugeLevelIndex( INT32 UserCount, INT32 MaxUserCount);

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;

	//--------------------------------------------------------------------------//
	//								inline functions							//
public:
	void			SetCurrentTab(SChannelType eType)	{ m_CurrentTab = eType; }
	SChannelType	GetCurrentTab( void)				{ return m_CurrentTab; }

	void			SetSelectServerIndex( INT32 idx)
	{
		m_SelectedServerIndex = idx;
		SetButtonState();		// 선택된 슬롯 인덱스에 따른 버튼 상태를 설정..
	}

	INT32 GetTargetServerIdx() const	{ return m_TargetServerIndex; }
	void SetTargetServerIdx(INT32 idx)	{ m_TargetServerIndex = idx;}
	SChannelType GetCurTargetSChannelType() const { return m_TargetServerTab; }
	

public:
	UIPhaseSelectServer();
	virtual ~UIPhaseSelectServer();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData) override;

	virtual bool	OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /* = I3_EVT_CODE_ACTIVATE */) override;

	// 입장 시작
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArv2 = nullptr) override;

	virtual void	OnEntranceEnd() override;

	virtual bool	OnExitStart( void) override;

	virtual void	OnLoadAllScenes() override;

	// 퇴장 완료
	virtual void	OnExitEnd( void) override;

	virtual void	OnSlideIn( void) override;

	virtual void	OnGotoBackPhase( void) override;

	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;

	virtual bool	OnInviteMove(void) override;

	void			OnChangeTab( SChannelType TabIndex);

	//--------------------------------------------------------------------------//
	/** \brief Button상태를 설정합니다. */
	void			SetButtonState( void);
	
	void			UpdateServerTab( void);

	void			UpdateServerSlotList( void);

	void			SetServerPassword(const i3::string & pw)		{ m_ServerPassword = pw;}
	
	// 양승천 작성
	void			RequestChannelList(INT32 ServerIndex);
	void			ToggleTextColor(INT32 nServerIdx);
	void			WaitForCloseChannelList(bool bWait);
	void			SendEventForChannel(INT32 event, const void * pArg1 = nullptr, const void * pArg2 = nullptr);

	bool			TryEnterServerChannel(INT32 nChannelList);
	void			SetCellTextColor(INT32 nSlot, COLOR * pColor);

	//

	void			SelectGroupListItem(INT32 idx);
	//--------------------------------------------------------------------------//
	//							Callback rutine									//
	//--------------------------------------------------------------------------//
	static void		ServerPasswordCallback(bool OK, const i3::string & Password, void * pUserContext);
	static void		CbOpenURL(void *pThis, INT32 nParam);


private:

	typedef void	(UIPhaseSelectServer::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func > m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveGameEvent_Connect_Server_Game( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Connect_Server_Gateway( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_GetInfo( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_User_Enter( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_User_Leave( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Destroyed_Network( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Enter_Password( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Get_ChannelList( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Join_Room( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_SetTab_AutoSelectServer( INT32 arg, const i3::user_data&);
};


//--------------------------------------------------------------------------//
//								Lua Glue Functions							//
namespace UIPhaseSelectServerScript {
	extern "C" int	getServerCount( LuaState * L);
	extern "C" int	getServerName( LuaState * L);
};



#endif
