#if !defined(__UI_FLOAT_CHANNEL_LIST__)
#define __UI_FLOAT_CHANNEL_LIST__

#include "UIFloatingBase.h"
#include "UIDefine.h"

#define MAX_CHANNEL_CTRL 24

class UIFloatChannelList : public UIFloatingBase
{
	I3_CLASS_DEFINE( UIFloatChannelList, UIFloatingBase);

private:
	INT32			m_LastServerNumber;
	INT32			m_LastChannelNumber;

	INT32			m_nLastedSelectSlot;

	REAL32 			m_fRefreshClickElapseTime;
	REAL32 			m_fConnectClickElapseTime;
	REAL32 			m_fWaitDelayTime;

	bool			m_bInviteMoving;
	bool			m_bWaitConnect;
	UIMessageBox*	m_pMBConnectWait;

	UIMessageBox *	m_pMessageBox;
	bool			m_isWaitMercenary;

#if !defined(USE_LOCALE_UI)
	// 심의 마크
	i3Texture*		m_pVerifiTexture;
	i3Sprite2D*		m_pVerifiSprite;

	/** \brief 심의마크 생성을 합니다.
	\desc 국가에 따라 등록을 요하는 경우에 이곳을 수정하십시오. */
	void			_CreateVerifiMark(void);
#endif

	// 채널 관련	
	REAL32			m_rChannelWinX;

	INT32			m_nServerChannelIdx;
	//i3::rc_wstring	m_aawstrServerText[ SS_MAINTYPE_MAX][ SS_SUBTYPE_COUNT];
	INT32			m_nServerCount[ SS_MAINTYPE_MAX];
	INT32			m_LogInConnectFailedCount;

	bool			m_bEnterLobby;

	static void			cbMercenaryRegist(void *pThis, INT32 nParam);

private:
	REAL32			__CalcChannelGauge( INT32 UserCount, INT32 MaxUserCount);

protected:
	// Channel Label
	void			_UpdateChannelLabel();
	void			_SetServerEnv( INT32 nServer, INT32 nChannel, INT32 nChannelState);
	bool			_CheckEnterChannel( INT32 channel);


	/** \brief 버튼 상태에 따른 설정을 합니다. */
	//void			_SetButtonState( void);

public:
	UIFloatChannelList();
	virtual ~UIFloatChannelList();
	
	virtual void OnCreate(i3GameNode * pParent) override;
	virtual void _InitializeAtLoad(i3UIScene * pScene) override;

	virtual void OnUpdate( REAL32 rDeltaSeconds) override;

	virtual void ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;

	virtual bool OnEntranceStart(void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void OnEntranceEnd(void) override;

	virtual bool OnExitStart(void) override;
	virtual void OnExitEnd(void) override;

public:
	void			Refresh( void);
	void			RefreshByTime( void);
	void			SelectChannel( INT32 channel);
	void			TryEnterChannel( bool OnlyConnect = false);

	//--------------------------------------------------------------------------//
	//							Lua functions									//
	//--------------------------------------------------------------------------//
	void			SetChannelSlot( INT32 slot, const i3::wliteral_range& wNameRng,
		INT32 event, REAL32 rGauge, COLOR * pColor, const i3::wliteral_range& wToolTipRng = L"");
	
	void			ChangeChannelUserGauge(INT32 nSlot, REAL32 rGauge);
	void			SetCellTextColor(INT32 nSlot, COLOR * pColor);

	//--------------------------------------------------------------------------//
	//							Error handler functions							//
	//--------------------------------------------------------------------------//
protected:
	void			_EhFailConnect( void);
	void			_EhStageError( INT32 Event,INT32 Arg);
	void			PrintMessageForChannelEnterError( ChannelType channelType );
	void			_CloseWaitMessageBox( void);


	//--------------------------------------------------------------------------//
	//								Callback rutine								//
public:
	static void		CbRefleshServerList( void* pThis,INT32 nParam);
	bool			m_bMercenaryRegist;


	//			Receive Game Event
private:
	typedef void	(UIFloatChannelList::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func > m_EventFuncMap;

	void		RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void		ReceiveGameEvent_ConnectServer( INT32 arg, const i3::user_data&);
	void		ReceiveGameEvent_LeaveServer( INT32 arg, const i3::user_data&);
	void		ReceiveGameEvent_Get_ChannelList( INT32 arg, const i3::user_data&);
	void		ReceiveGameEvent_UserEnter( INT32 arg, const i3::user_data&);
	void		ReceiveGameEvent_SelectChannel( INT32 arg, const i3::user_data&);
	void		ReceiveGameEvent_EnterLobby( INT32 arg, const i3::user_data&);
	void		ReceiveGameEvent_DestroyedNetwork( INT32, const i3::user_data&);

	void		ReceiveGameEvent_CloseWaitMessageBox( INT32, const i3::user_data&);
	void		ReceiveGameEvent_ShutDownTime( INT32, const i3::user_data&);
	
	
}; 

#endif