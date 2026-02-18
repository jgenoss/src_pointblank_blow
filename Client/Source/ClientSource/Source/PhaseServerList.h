#ifndef		__PHASE_SERVER_SERVER_LIST_FRAME_H__
#define		__PHASE_SERVER_SERVER_LIST_FRAME_H__


#include "PhaseBase.h"


class	CServerInfo;


class	CPhaseServerList	:	public	CPhaseBase
{
	I3_CLASS_DEFINE(CPhaseServerList);

public:
	CPhaseServerList();


	virtual	void	OnGameEvent(INT32 event,INT32 arg);
	virtual	BOOL	OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual	void	OnInitControl(void);
	virtual	void	OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual	void	OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual	void	OnWheelUpdate(INT32 scrollby);

	virtual	void	OnEntranceStart(void);
	virtual void	OnEntranceEnd(void);
	virtual	void	OnExitEnd(void);

	virtual	BOOL	OnSliding(RT_REAL32 rDeltaSeconds);

	virtual	BOOL OnInviteMove(void)		{return FALSE;}

private:
	// UI 처리 함수.
	void	OnChangeTab(SChannelType TabIndex);
	void	OnChangeServerListScroll();

	BOOL	TryEnterServer(INT32 ServerIndex, SChannelType SType = S_CHANNEL_TYPE_ALL);
	BOOL	ForceEnterServer(INT32 ServerIndex, SChannelType SType = S_CHANNEL_TYPE_ALL);
	BOOL	ForceEnterSelectServer();
	void	UpdateServerTab();
	void	UpdateServerSlotList();

	static	void	ServerPasswordCallback(BOOL OK, const string & Password, void * pUserContext);

	void	SetServerPassword(const string & pw)		{m_ServerPassword = pw;}
	void	InputDiable(BOOL Val);

private:
	struct	ServerListSlot
	{
	public:
		ServerListSlot();

		void	SetEnable(BOOL Enable);
		void	Update(const CServerInfo * pInfo, BOOL Selected);

		i3GuiStatic *	pFrame;
		i3GuiEditBox *	pText;
		i3GuiStatic *	pUserGauge;
	};
	enum {MAX_SERVER_LIST_SLOT = 24};

	struct	ServerTab
	{
	public:
		ServerTab();

		void	SetSelect(BOOL Enable);
		void	SetEnable(BOOL Enable);
		void	SetType(SChannelType _Type);

		i3GuiButton *	pCtrl;
		SChannelType	Type;
	};
	enum {MAX_UI_TAP = 6};

	void			SetCurrentTab(SChannelType eType) 
	{ 
		//GlobalFunc::PB_TRACE("SetCurrentTab %d", eType);
		m_CurrentTab = eType; 
	}

	SChannelType	GetCurrentTab()		{ return m_CurrentTab; }
	ServerTab *		GetTab(SChannelType Type);

private:
	i3GuiStatic *		m_pServerListWin;
	i3GuiStatic *		m_pServerListFrame;
	i3GuiStatic *		m_pServerListScrollFrame;
	i3GuiList *			m_pServerListScrollList;
	INT32				m_ServerListStartSlot;

	REAL32				m_SlidingY;


	ServerTab			m_Tab[MAX_UI_TAP];
	ServerListSlot		m_ServerListSlots[MAX_SERVER_LIST_SLOT];

	INT32				m_SelectedServerIndex;
	SChannelType		m_CurrentTab;

	i3GuiButton *		m_pServerRefresh;	// ssServerRefresh
	i3GuiButton *		m_pServerEnter;		// ssServerEnter
	REAL32				m_LastRefreshTime;
	CMessageBox *		m_pMBInfoWait;
	INT32				m_ConnectFailedCount;		// 일회 게이트웨이서버로 접속 시도시에 일어난 컨넥트 실패 횟수
	INT32				m_ConnectFailedTotalCount;	// 게임서버 -> 게이트웨이 접속이 실패한 총 횟수.

	string				m_ServerPassword;

	INT32				m_PrevServerIndex;
	SChannelType		m_PrevServerTab;

	INT32				m_TargetServerIndex;
	SChannelType		m_TargetServerTab;
};


#endif	// __PHASE_SERVER_SERVER_LIST_FRAME_H__

