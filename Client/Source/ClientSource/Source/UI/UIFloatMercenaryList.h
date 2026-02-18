#if !defined( __UI_FLOAT_MERCENARY_LIST_H__)
#define __UI_FLOAT_MERCENARY_LIST_H__

#include "UIFloatingBase.h"
#include "UISecCount.h"

class UIFloatMercenaryList : public UIFloatingBase
{
	I3_CLASS_DEFINE( UIFloatMercenaryList, UIFloatingBase);

private:
	i3UIScrollBar *	m_pScrollBar;
	i3UIButtonComposed3 *	m_pInviteBtn;
	i3UIFrameWnd*	m_pMainFmw;

	INT32			m_i32SelectedIdx;
	UISecCount		m_SecCount;			// 용병 초대팝업 측정하는 용도

	UIMessageBox*	m_InviteMsgBox;
	bool			m_bInvited;			// 팀리더의 경우, 용병을 초대한 상태인지. 답변ACK 올 때까지 다른 용병에게 초대를 못함
	bool			m_bLeaveClanChannel;	// 클랜 채널에서 나갈 경우. EVENT_CLAN_WAR_GET_MERCENARY_LIST

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

protected:
	void			_InitMercenaryList(void);
	void			_InitUIText(void);
	void			_UpdateMercenaryList(void);
	void			_SetUserInfo(const INT32 idx, const CLAN_MERCENARY_INFO * pInfo);
	void			_SetUserCount(const INT32 count);
	void			_SetCurSelectItem(void);
	void			_SetRequestButtonEnable(void);
	void			_SetButtonActivate(void);
	void			_SetFramePos(void);

public:
	UIFloatMercenaryList();
	virtual ~UIFloatMercenaryList();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;	
	virtual bool	OnExitStart( void) override;

	virtual bool	OnRevive(i3RenderContext * pCtx) override;

public:
	void			ToggleFrame(const bool bEnable);
	void			OnScrollList(void);
	void			OnWheelList(INT32 zDelta);
	void			SelectUser(const INT32 select_idx);
	void			OpenMercDetail( const INT32 whisper_idx );

	void			ClickRequest(void);
	void			ClickCancel(void);

	void			Register(void);
	void			Invite(void);
	void			AcceptInvite(void);
	void			DenialInvite(void);
	void			DetailInfo(void);	

	const bool		isLobbyPhase(void);
	const bool		isClanMember(void);							

	void			OnSecCount_Start(INT32 result_time);
	void			OnSecCount_Change(INT32 result_time);
	void			OnSecCount_DoneOrStop(INT32 result_time);

	void			SetLeaveClanChannel(bool enable) { m_bLeaveClanChannel = enable; }
	bool			IsLeaveClanChannel()			 { return m_bLeaveClanChannel; }

private:
			TUISecCallback<UIFloatMercenaryList, 
		&UIFloatMercenaryList::OnSecCount_Start,
		&UIFloatMercenaryList::OnSecCount_Change,
		&UIFloatMercenaryList::OnSecCount_DoneOrStop,
		&UIFloatMercenaryList::OnSecCount_DoneOrStop
	> m_SecCountCB;

	/************************************************************************/
	/*						Event Receive CallBack Func						*/
	/************************************************************************/
private:	
	typedef void	(UIFloatMercenaryList::*call_func)( INT32, const i3::user_data &);
	i3::unordered_map< GAME_EVENT, call_func > m_EventFuncMap;

	void		RegisterGameEventFunc( GAME_EVENT evt, call_func f);
	
	void		ReceiveEvent_ClanWar_GetMercenaryList( INT32, const i3::user_data & );
	void		ReceiveEvent_ClanWar_RegisterMercenary( INT32, const i3::user_data & );
	void		ReceiveEvent_ClanWar_RemoveMercenary( INT32, const i3::user_data & );
	void		ReceiveEvent_ClanWar_SendInviteMercenary( INT32, const i3::user_data & );
	void		ReceiveEvent_ClanWar_RecvInviteMercenary( INT32, const i3::user_data & );
	void		ReceiveEvent_ClanWar_AcceptInvite( INT32, const i3::user_data & );
	void		ReceiveEvent_ClanWar_DenialInvite( INT32, const i3::user_data & );
	void		ReceiveEvent_ClanWar_GetMercenaryDetail( INT32, const i3::user_data &);

	void		ReceiveEvent_ClanWar_ModifyMatchTeamInfo( INT32, const i3::user_data & );

	/*
		CallBack Function
	*/
	static void		CbRemoveMercenary(void* pThis,INT32 nParam);

};

#endif	// __UI_FLOAT_MERCENARY_LIST_H__
