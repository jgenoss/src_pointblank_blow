
// ControlToolView.h : interface of the CControlToolView class
//


#pragma once

#include "SocketWork.h"
#include "DlgUpdate.h"
#include "DlgUpdateServerList.h"

#define MAX_GAMESERVER_COUNT 100
#define MAX_DEDISERVER_COUNT	100

#define COMMON_SERVER_COUNT		9

enum SOCKET_STATE
{
	DISCONNECT		= 0,
	CONNECTED,
	WRONG_PACKET,
};

enum WARNING_TYPE
{
	WARNING_TYPE_NORMAL,
	WARNING_TYPE_NOTICE,
	WARNING_TYPE_COUNT
};

enum WARNING_LEVEL
{
	WARNING_LEVEL_NO,
	WARNING_LEVEL_PERFOMANCE,
	WARNING_LEVEL_NETWORK,
	WARNING_LEVEL_DB,
	WARNING_LEVEL_GAME_GUARD,
	WARNING_LEVEL_CRASH,
	WARNING_LEVEL_COUNT
};

class CControlToolView : public CListView
{
protected: // create from serialization only
	CControlToolView();
	DECLARE_DYNCREATE(CControlToolView)

// Attributes
public:
	CControlToolDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CControlToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	//View 
	void	SetCaption(void); 
	void	SetCommonServerInfo( SERVER_INFO_COMMON * pList, INT32 iCount);
	void	SetControlServerInfo(INT8 CurState);
	INT32	m_i32ServerIdx; 

	void	SetWarningText( TCHAR * pString, UINT32 iWarningValue);
	UINT32	GetFlagWarningLevel( INT32 i32WarningLevel );
// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	virtual void PreSubclassWindow();
	DECLARE_MESSAGE_MAP()

	TCHAR				m_strID[ NET_ID_SIZE ];
	TCHAR				m_strPass[ NET_PW_SIZE ];
	UINT64				m_ui64MacAddress;

	TCHAR				m_strControlToolFile[MAX_PATH];

	INT32				m_i32UpdateTime;

	INT32				m_i32AllServerCount;

	INT32					m_iCommonServerCount;
	SERVER_INFO_COMMON*		m_pCommonServerList;

	INT32					m_iGameServerCount;
	SERVER_INFO_COMMON*		m_pGameServerList;

	INT32					m_iBattleServerCount;
	SERVER_INFO_COMMON*		m_pBattleServerList; 

	INT32					m_i32CastServerCount;
	SERVER_INFO_COMMON*		m_pCastServerList;
	
	INT32					m_i32UpdateServerCount;
	SERVER_INFO_COMMON	*	m_pUpdateServerList;

	GSHackCheckRatio	*	m_pGameHackCheckRatio;
	ServerHackCheckRatio*	m_pBattleHackCheckRatio;
	

	UINT8					m_pNotAllowRoomCreationList[MAX_DEDISERVER_COUNT];

	UINT32					m_HackCount; 
	
	UINT32					m_pHackCount[HACK_TYPE_MAX];

	INT32				m_i32Separator;

	INT8				m_i8AuthWrite;
	INT8				m_i8AuthService;

	INT32				m_i32MaxCCU;
	UINT32				m_i32MaxCCUTime;

	INT32				m_i32NowCCU;
	UINT32				m_i32NowCCUTime;

	DWORD				m_dwLoginTime;

	INT32				m_i32WarningSwitch;

	CRect				m_rcList;
	LVCOLUMN			m_col;

	UINT32		*		m_pi32WarningIcon;	// 0 = 워닝 X. 1 = 워닝 O
//	UINT32				m_pi32WarningIcon[ALL_SERVER_COUNT];	// 0 = 워닝 X. 1 = 워닝 O

	SS_VERSION			m_ControlVer;

	INT8				m_i8CServerState;		//ControlServer State 저장
	DWORD				m_dwControlSVRIp;		//Display용 Control Server IP
	BOOL				m_bCommonServer;
	CDlgUpdate	*		m_pDlgUpdate;			//Update를 표시 하기 위한 Dialog
	CDlgUpdateServerList* m_pDlgUpdateServerList;

	INT32				m_i32OldTime;			//시간당 Graph를 표시 하기 위한 데이터

	HANDLE				m_hFile;
	TCHAR				m_strCurFilePath[MAX_PATH];
	TCHAR				m_strUpdateFilePath[MAX_PATH];
	INT32				m_i32UpdateFileSize;				//업데이트 받는 파일의 사이즈
	INT32				m_i32CurUpdateFileSize;

public:
	// To Hach Check ratio
	void SendChangedHackRatio(INT32 i32HackCheckType, UINT8 ui8ChangedRatio, ASC_SERVER_TYPE eServerType = ASC_SERVER_BATTLE);
	void UpdateHackCheckRatioUI(ASC_SERVER_TYPE eServerType, int iBattleServerIdx);
	void ClearHackCheckRatio(int iBattleServerIdx);
	void DisableBattleServerMenuitem(CMenu* pMenu);
	void DisableGameServerMenuitem(CMenu* pMenu);
	 
public:
	bool				m_bStopApp;
	bool				m_bStart;
	CImageList			m_ImageList;
	CImageList			m_ImageListSmall;
	CSocketWork*		m_pControlSocket;
	CSocketWork*		m_pSCMSocket;

	char				m_strServiceName[ASC_SERVER_COUNT][64];		//0: 중계서버 1: 클랜서버 2: 메신저서버 3: 릴레이 서버 4:랑데뷰서버 10: 게임 서버 11: Dedication 서버

	afx_msg void		OnDestroy();
	virtual INT_PTR		OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	afx_msg void		OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void		OnConnectControlSVR();
	afx_msg void		OnAllServerStart();
	afx_msg void		OnAllServerStop();
	afx_msg void		OnPopStartserver();
	afx_msg void		OnPopStopserver();
	afx_msg void		OnSetMD5Key();
	afx_msg void		OnAllUserkick();
	afx_msg void		OnPopUserkick();
	afx_msg void		OnPopTransuserkick();
	afx_msg void		OnPopWarningClear();
	afx_msg void		OnNoticeLobbynotice();
	afx_msg void		OnNoticeEmergencrynotice();
	afx_msg void		OnModifyStyleLarge();
	afx_msg void		OnModifyStyleSmall();
	afx_msg void		OnModifyStyleList();
	afx_msg void		OnModifyStyleDetail();
	afx_msg void		OnAllWarningClear();

	INT32				CellRectFromPoint( CPoint& point, RECT* cellrect, int* col ) const;
	BOOL				OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult );
	void				GetMACAddress( UINT64 * pMAC,INT32 i32NICCount = 1 );
	BOOL				CheckAuth();
	void				OutputAddString( INT32 i32Type, TCHAR* strText );
	void				ExitClient();
	INT32				PacketParsing();
	INT32				PacketParsing_SCM();
	BOOL				Initialization();
	void				ConnectSCM();
	void				Remove_Old();
	void				ReStartTool();
	BOOL				CheckSocketState();
	void				SetControlServerState(INT8 CurrentState)		{ m_i8CServerState = CurrentState; SetControlServerInfo(CurrentState); }
	BOOL				SendUpdatePacket(UINT32 ui32Port, char* strServiceName);
	void				SetHackCheckMenu( CMenu* pMainMenu );

	afx_msg void		OnPopHackChange( UINT i32Param );
	
	afx_msg void		OnPopNotallowroomcreation();
	
	afx_msg void		OnServerUpdate();
	afx_msg void		OnDisableAllHackCheck();
	afx_msg void		OnDllcheat(UINT i32Param);

	void				GetIP( UINT32 ui32IP, TCHAR* wstrIP );
};

#ifndef _DEBUG  // debug version in ControlToolView.cpp
inline CControlToolDoc* CControlToolView::GetDocument() const
   { return reinterpret_cast<CControlToolDoc*>(m_pDocument); }
#endif
