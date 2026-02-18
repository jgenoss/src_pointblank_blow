/**
 * \file	UI\UIPhaseClanLobby.h
 *
 * Declares the user interface phase lobby class.
 */


#if !defined( __UI_PHASE_CLANLOBBY_H__)
#define __UI_PHASE_CLANLOBBY_H__

#include "UIPhaseClanBase.h"
#include "UIPhaseBase.h"
#include "UIDefine.h"

#include "i3Base/itl/signal/signal_common.h"

class CClanContext;
class UILoadingIcon;


class UIMatchSquadMemberSlot
{
private:
	i3UIButton *			m_pSlotBG;
	i3UIButtonImageSet *	m_pRank;
	i3UIImageBox *			m_pHostMark;
	i3UIStaticText *		m_pNickName;

private:
	void _SetSelectSlot(bool is_select);

public:
	UIMatchSquadMemberSlot(void);
	~UIMatchSquadMemberSlot(void);

	bool InitSlot(i3UIFrameWnd * pWnd, const INT32 idx);
	void Reset(void);

	void SetSlot(const CLAN_MATCH_SQUAD_INFO * pInfo, const CLAN_MATCH_SQUAD_MEMBER & Member, INT32 Rank = 0);
	void EnableSlot(bool is_enable);
};

class UIPhaseClanLobby : public UIPhaseBase, public ClanProcessGameEvent
{
	I3_CLASS_DEFINE( UIPhaseClanLobby, UIPhaseBase);

	enum LOADING_TYPE
	{
		NONE = 0,
		CLAN_INFO,
		TEAM_REFRESH,
		MATCH_MAKING
	};

	enum 
	{
		SQUAD_TEAM_MAX = 8, SQUAD_MEMBER_MAX = 8 
	};

	// 로비쪽 패킷수신처리를 따로 마련하지 않으면 베이스로 넘어감.. 패킷처리를 위해 추가..(2011.05.12 수빈)
	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData) override;	// 파생클래스의 가상함수는 직접 접근하지 않으면 private으로할것.	

private:
	//UTIL MEMBER
	REAL32					m_fUpdateTeamListTime;
	size_t					m_temp;
	bool					m_bFirstLoad;
	bool					m_bPushedBackPhase;

	ClanMatchRoomList		m_ClanMatchRoomList;

	UIMatchSquadMemberSlot	m_MemberSlot[SQUAD_MEMBER_MAX];

	i3UIComboBox*			m_pSquadCombo;
	i3UIImageBoxEx*			m_pClanMark;	
	INT32					m_nScroolPage;
	INT32					m_nCurrentSlotSquad;
	bool					m_bInRoomState;

	UILoadingIcon* 			m_pLoadingIcon;
	LOADING_TYPE			m_eLoadingType;

	i3::scoped_connection_set	m_SigConnection;
	bool					m_bIsProcessInviteMove;
	bool					m_bProcessLeaveTeam;			// 용병이 팀을 나가는 중, EVENT_CLAN_LIST_N 을 지속적으로 보내지 않기 위해..

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

	void			_InitMyTeam(void);
	void			_RefreshTeam(void);

	void			_SetSquadInfo(void);
	void			_SetSquadMemberInfo(void);
	void			_SetSquadOperation(void);
	void			_SetSquadCombo(void);
	void			_SetQuickMatchButton(void);
	void			_SetJoinTeamState(void);
	void			_EnableQuickMatchButton(bool enable);

	void			_SetClanDetailInfo(void);
	void			_SetClanMark(CLAN_DETAIL_INFO * pInfo);
	void			_SetClanExpRate(i3::stack_wstring& out_wstr, REAL32 & i32OutRate , UINT32 ui32Exp);

	void			_SetTeamSlotInfo(void);
	
	inline bool		_isSameLoading(LOADING_TYPE type)	const	{ return m_eLoadingType == type; }

public:
	UIPhaseClanLobby();
	virtual ~UIPhaseClanLobby();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArv2 = nullptr) override; 	// 입장 시작
	virtual void	OnEntranceEnd( void) override;											// 입장 완료

	virtual bool	OnExitStart( void) override;

	virtual void	OnGotoBackPhase( void) override;
	virtual bool	OnInviteMove( void) override;

public:
	//UTIL
	void			UpdateMember(void);
	void			UpdateTeam(void);
	void			SetLeaveTeamFlag() { m_bProcessLeaveTeam = true; }
	T_TeamID		m_SeletedTeamID;

private:

	typedef void	(UIPhaseClanLobby::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func >	m_EventFuncMap;

	void		RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void		ReceiveEvent_ClanWar_GetTeamList( INT32, const i3::user_data&);
	void		ReceiveEvent_ClanWar_CreateTeam( INT32 arg, const i3::user_data&);
	void		ReceiveEvent_ClanWar_JoinTeam( INT32 arg, const i3::user_data&);
	void		ReceiveEvent_ClanWar_LeaveTeam( INT32 arg, const i3::user_data&);
	void		ReceiveEvent_ClanWar_ChangeOperation( INT32, const i3::user_data&);
	void		ReceiveEvent_ClanWar_ChangeMaxPerson( INT32, const i3::user_data&);
	void		ReceiveEvent_ClanWar_RegisterMatchMaking( INT32, const i3::user_data&);
	void		ReceiveEvent_ClanWar_RegisterCancelMatchMaking(INT32, const i3::user_data&);

	void		ReceiveEvent_Clan_List_Context( INT32, const i3::user_data&);
	void		ReceiveEvent_Clan_DetailInfo( INT32 arg, const i3::user_data&);
	void		ReceiveEvent_Create_Room( INT32 arg, const i3::user_data&);
	void		ReceiveEvent_Join_Room( INT32 arg, const i3::user_data&);
	void		ReceiveEvent_Leave_Lobby( INT32 arg, const i3::user_data&);
	void		ReceiveEvent_PlayTimeAlarm( INT32 arg, const i3::user_data&);
	void		ReceiveEvent_ShutDownTimeAlarm( INT32 arg, const i3::user_data&);

	/************************************************************************/
	/*                        Lua Glue Callback Func                        */
	/************************************************************************/ 
	void		Callback_CreateTeam();
	void		Callback_JoinTeam(INT32 RoomIdx);
	void		Callback_LeaveTeam();
	void		Callback_QuickMatch();
	void		Callback_QuickMatchCancel();
	void		Callback_EnterRoomList();
	void		Callback_ScrollTeamList(INT32 scrollPos);
	void		Callback_ChangePerson(INT32 matchType);
	void		Callback_ChangeOperation (const i3::literal_range& strOperation);

	// ETC CallBack Func.
	static void CbCreateTeam(void* pThis,INT32 nParam);
	static void CbJoinTeam(void* pThis,INT32 nParam);
	static void CbLeaveTeam(void* pThis, INT32 nParam);

};


#endif

