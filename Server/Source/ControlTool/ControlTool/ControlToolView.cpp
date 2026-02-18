
// ControlToolView.cpp : implementation of the CControlToolView class
//

#include "pch.h"
#include "../CommonServerSource/DestroyReason.h"
#include "MainFrm.h"
#include "ControlTool.h"
#include "ControlToolDoc.h"
#include "ControlToolView.h"
//#include "HackCheckMenu.h"
//#include "VersionCreate.h"

#include "DlgUpdate.h"
#include "DlgUpdateServerList.h"
#include "DlgSCMConnect.h"
#include "DlgLogin.h"
#include "DlgAuth.h"
#include "DlgNotice.h"
#include "DlgMd5Key.h"

#include "ErrorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define EVENT_START 1

// CControlToolView

#define WARINING_IMAGE		2

INT32 g_i32Num = 0;

IMPLEMENT_DYNCREATE(CControlToolView, CListView)

BEGIN_MESSAGE_MAP(CControlToolView, CListView)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_REFLECT(NM_RCLICK,			&CControlToolView::OnNMRClick)
	ON_COMMAND(ID_ALL_SERVER_START,			&CControlToolView::OnAllServerStart)
	ON_COMMAND(ID_ALL_SERVER_STOP,			&CControlToolView::OnAllServerStop)
	ON_COMMAND(ID_POP_CONNECT_CONTROL,		&CControlToolView::OnConnectControlSVR)
	ON_COMMAND(ID_POP_STARTSERVER,			&CControlToolView::OnPopStartserver)
	ON_COMMAND(ID_POP_STOPSERVER,			&CControlToolView::OnPopStopserver)
	ON_COMMAND(ID_POP_USERKICK,				&CControlToolView::OnPopUserkick)
	ON_COMMAND(ID_POP_TRANSUSERKICK,		&CControlToolView::OnPopTransuserkick)
	ON_COMMAND(ID_POP_WARNINGCLEAR,			&CControlToolView::OnPopWarningClear)
	ON_COMMAND(ID_SET_MD5KEY,				&CControlToolView::OnSetMD5Key)
	ON_COMMAND(ID_ALL_USERKICK,				&CControlToolView::OnAllUserkick)
	ON_COMMAND(ID_NOTICE_LOBBYNOTICE,		&CControlToolView::OnNoticeLobbynotice)
	ON_COMMAND(ID_NOTICE_EMERGENCRYNOTICE,	&CControlToolView::OnNoticeEmergencrynotice)
	ON_COMMAND(ID_VIEW_SMALLICON,			&CControlToolView::OnModifyStyleSmall)
	ON_COMMAND(ID_VIEW_LARGEICON,			&CControlToolView::OnModifyStyleLarge)
	ON_COMMAND(ID_VIEW_LIST,				&CControlToolView::OnModifyStyleList)
	ON_COMMAND(ID_VIEW_DETAILS,				&CControlToolView::OnModifyStyleDetail)
	ON_COMMAND(ID_ALL_WARNING_CLEAR,		&CControlToolView::OnAllWarningClear)

	ON_COMMAND(ID_POP_NOTALLOWROOMCREATION, &CControlToolView::OnPopNotallowroomcreation)
	ON_COMMAND(ID_UPDATE,					&CControlToolView::OnServerUpdate)
	ON_COMMAND(ID_CHECKHACKING_DISABLE,		&CControlToolView::OnDisableAllHackCheck)
	ON_COMMAND_RANGE(ID_DLLCHEAT_0, ID_DLLCHEAT_100,			&CControlToolView::OnDllcheat)

	ON_COMMAND_RANGE(ID_HACKCHECK_USER_TYPE_START, ID_HACKCHECK_USER_TYPE_END, &CControlToolView::OnPopHackChange)
	
END_MESSAGE_MAP()

HACK_CHECK_MENU g_HackCheckMenu[] =
{
	{	HACK_TYPE_GRAVITY,			_T("Gravity"),			HACK_CHECK_ACTIVE_ONOFF	},
	{	HACK_TYPE_ESCAPE,			_T("Escape"),			HACK_CHECK_ACTIVE_ONOFF	},
	{	HACK_TYPE_RANGE,			_T("Range"),			HACK_CHECK_ACTIVE_ONOFF	},
	{	HACK_TYPE_SPEED,			_T("Speed"),			HACK_CHECK_ACTIVE_RATIO	},
	{	HACK_TYPE_WALLSHOT,			_T("Wall Shot"),		HACK_CHECK_ACTIVE_RATIO	},
	{	HACK_TYPE_GRENADE,			_T("Grenade"),			HACK_CHECK_ACTIVE_ONOFF	},
	{	NOT_USE_HACK_TYPE_MAGAZINE,	_T("Magazine"),			HACK_CHECK_ACTIVE_ONOFF	},
	{	HACK_TYPE_GHOST,			_T("Ghost"),			HACK_CHECK_ACTIVE_ONOFF },
	{	HACK_TYPE_RECALL,			_T("Recall(Magnet)"),	HACK_CHECK_ACTIVE_ONOFF	},
	{	HACK_TYPE_WEAPON,			_T("Weapon Change"),	HACK_CHECK_ACTIVE_ONOFF	},
	{	HACK_TYPE_TRAINING,			_T("Training Mode"),	HACK_CHECK_ACTIVE_ONOFF	},
	{	HACK_TYPE_FIRE_SPEED,		_T("Fire Speed"),		HACK_CHECK_ACTIVE_ONOFF	},
	{	HACK_TYPE_GRAVITY_MOVE,		_T("GravityMove"),		HACK_CHECK_ACTIVE_RATIO	},
	{	HACK_TYPE_RESPAWN_TIME,		_T("Respawn Time"),		HACK_CHECK_ACTIVE_ONOFF	},
	{	HACK_TYPE_RESPAWN_POSITION,	_T("Respawn Position"),	HACK_CHECK_ACTIVE_ONOFF	},
	{	HACK_TYPE_INSTALL_C4,		_T("C4 Installation"),	HACK_CHECK_ACTIVE_ONOFF	},
	{	HACK_TYPE_CLIENT_TIME,		_T("Client Time"),		HACK_CHECK_ACTIVE_ONOFF	},
	{	HACK_TYPE_HEADSHOT,			_T("Head Shot"),		HACK_CHECK_ACTIVE_ONOFF	},
	{	HACK_TYPE_ITEM,				_T("Item"),				HACK_CHECK_ACTIVE_ONOFF	},
	{	HACK_TYPE_WEAPON_PARAM,		_T("Weapon Parameter"),	HACK_CHECK_ACTIVE_ONOFF	},
	{	HACK_TYPE_WEAPON_NOT_FOUND,	_T("Weapon N Found"),	HACK_CHECK_ACTIVE_ONOFF	},
	{	HACK_TYPE_UDP_VERSION,		_T("UDP Version"),		HACK_CHECK_ACTIVE_ONOFF	},
	{	HACK_TYPE_PAUSE_MOVE,		_T("Pause Move"),		HACK_CHECK_ACTIVE_ONOFF	},
	{	HACK_TYPE_PAUSE_SHOOT,		_T("Pause Shoot"),		HACK_CHECK_ACTIVE_ONOFF	},
	{	HACK_TYPE_USURPATION,		_T("Usurpation"),		HACK_CHECK_ACTIVE_ONOFF	},
	{	HACK_TYPE_GRNEADE_POS,		_T("GrenadePos"),		HACK_CHECK_ACTIVE_ONOFF	},
	{	HACK_TYPE_RPG_CHEAT,		_T("RPG Cheat"),		HACK_CHECK_ACTIVE_ONOFF	},
};

// CControlToolView construction/destruction

CControlToolView::CControlToolView()
{
	m_pBattleHackCheckRatio = NULL;

	// TODO: add construction code here
	m_pControlSocket		= NULL;
	m_pSCMSocket			= NULL;

	m_bStopApp				= FALSE;
	m_bStart				= FALSE;

	m_iCommonServerCount	= 0;
	m_pCommonServerList		= NULL;

	m_iGameServerCount		= 0;
	m_pGameServerList		= NULL;

	m_iBattleServerCount	= 0;
	m_pBattleServerList		= NULL;

	m_i32CastServerCount	= 0;
	m_pCastServerList		= NULL;

	m_i32UpdateServerCount	= 0;
	m_pUpdateServerList		= NULL;

	m_i32AllServerCount		= 0;

	m_i32Separator			= 5;

	m_dwLoginTime			= 0;

	m_i32WarningSwitch		= 0;

	m_pi32WarningIcon		= NULL;
	
	m_bCommonServer			= FALSE;

	m_i32OldTime			= 0;

	m_pDlgUpdate			= NULL;
	m_pDlgUpdateServerList	= NULL;

	m_i32UpdateFileSize		= 0;

	m_i32CurUpdateFileSize = 0;

	m_hFile					= INVALID_HANDLE_VALUE;

	ZeroMemory(m_strUpdateFilePath, MAX_PATH);
	ZeroMemory(m_strCurFilePath, MAX_PATH);

	m_pBattleHackCheckRatio = new ServerHackCheckRatio[MAX_DEDISERVER_COUNT];
	memset(m_pBattleHackCheckRatio, 0, sizeof(ServerHackCheckRatio) * MAX_DEDISERVER_COUNT);

	m_pGameHackCheckRatio = new GSHackCheckRatio[MAX_GAMESERVER_COUNT];
	memset(m_pGameHackCheckRatio, 0, sizeof(GSHackCheckRatio) * MAX_GAMESERVER_COUNT);

	memset(m_pNotAllowRoomCreationList, 0, sizeof(INT8) * MAX_DEDISERVER_COUNT);

	m_HackCount = 0; 
	m_pHackCount; 
	
	memset( m_pHackCount, 0, (sizeof(UINT32) * HACK_TYPE_MAX));

	strcpy( &m_strServiceName[0][0], "PBTransServerService"); 
	strcpy( &m_strServiceName[1][0], "PBClanServerService"); 
	strcpy( &m_strServiceName[2][0], "PBMessengerServerService"); 
	strcpy( &m_strServiceName[3][0], "PBRelayServerService");			//배틀서버는 이름을 이렇게 변경합니다.	
	strcpy( &m_strServiceName[4][0], "PBRelayServerService");			//배틀서버는 이름을 이렇게 변경합니다.
	strcpy( &m_strServiceName[5][0], "UpdateServer_Service"); 
	strcpy( &m_strServiceName[6][0], "PBControlServerService"); 
	strcpy( &m_strServiceName[7][0], "ZLogServerService"); 
	strcpy( &m_strServiceName[8][0], "GatewayServer"); 
	strcpy( &m_strServiceName[9][0], "SIAServerService"); 
	strcpy( &m_strServiceName[10][0],"PBGameServerService"); 
	strcpy( &m_strServiceName[11][0], "_BillingServerService");
	strcpy( &m_strServiceName[12][0], "PBCastServerService");
}

CControlToolView::~CControlToolView()
{
	delete[] m_pGameHackCheckRatio;
	delete[] m_pBattleHackCheckRatio;
	ZeroMemory(m_pNotAllowRoomCreationList, sizeof(m_pNotAllowRoomCreationList));
	ZeroMemory(m_pHackCount, sizeof(m_pHackCount));

	/*if(m_pSCMSocket != NULL)
		SAFE_DELETE(m_pSCMSocket);

	if(m_pControlSocket != NULL)
		SAFE_DELETE(m_pControlSocket);

	if(m_pDlgUpdate != NULL)
		SAFE_DELETE(m_pDlgUpdate);*/
}

BOOL CControlToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CListView::PreCreateWindow(cs);
}


void CControlToolView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	// TODO: You may populate your ListView with items by directly accessing
	//  its list control through a call to GetListCtrl().
	// Start
	WSADATA wsaData;		
	if( ::WSAStartup( MAKEWORD(2,2), &wsaData ) != 0 )
	{
		return; 
	}
	if( wsaData.wVersion != MAKEWORD(2,2) )
	{
		::WSACleanup();
		return; 
	}

	m_ImageList.Create( IDB_BITMAP_NORMAL, 32, 1, RGB(0, 128, 128 ) );
	m_ImageListSmall.Create( IDB_BITMAP_SMALL, 16, 1, RGB(0, 128, 128 ) );

	GetListCtrl().SetImageList( &m_ImageList, LVSIL_NORMAL);
	GetListCtrl().SetImageList( &m_ImageListSmall, LVSIL_SMALL);

	OnModifyStyleDetail();
	GetListCtrl().SetExtendedStyle( LVS_EX_FULLROWSELECT );

	CRect rcList;
	GetWindowRect( &rcList );
	ScreenToClient( &rcList );

	LVCOLUMN col;
	col.mask    = LVCF_TEXT | LVCF_WIDTH;

	col.pszText = _T("Server");
	col.cx		= rcList.Width()/13;
	GetListCtrl().InsertColumn( 0, &col);

	col.pszText = _T("Version");
	col.cx		= rcList.Width()/10;
	GetListCtrl().InsertColumn( 1, &col);

	col.pszText = _T("IP");
	col.cx		= rcList.Width()/17;
	GetListCtrl().InsertColumn( 2, &col);

	col.pszText = _T("ServerState");
	col.cx		= rcList.Width()/9;
	GetListCtrl().InsertColumn( 3, &col);

	col.pszText = _T("ServerValue");
	col.cx		= rcList.Width()/5 + 30;
	GetListCtrl().InsertColumn( 4, &col);

	col.pszText = _T("Hack Check Ratio");
	col.cx		= rcList.Width()/3 + 250;
	GetListCtrl().InsertColumn( 5, &col);

	m_rcList	= rcList;
	m_col		= col;
	m_bStart	= true;
	
	//업데이트 후 새로시작되었다면 현재 기존의 파일을 지우는 작업.
	//Remove_Old();

	Initialization();
}

void CControlToolView::SetHackCheckMenu( CMenu* pMainMenu )
{
	INT32 i32Count = sizeof(g_HackCheckMenu) / sizeof(HACK_CHECK_MENU);

	CMenu* pSecondMenu		= pMainMenu->GetSubMenu( 7 );
	if( NULL == pSecondMenu )	return;

	TCHAR strString[ MAX_PATH ];

	UINT32 ui32ID;
	for( INT32 i = 0 ; i < i32Count ; i++ )
	{
		ui32ID = ID_HACKCHECK_USER_TYPE_START | g_HackCheckMenu[i].eHackType << 4;

		CMenu ThirdMenu;
		ThirdMenu.CreatePopupMenu();

		switch( g_HackCheckMenu[i].m_eActiveType )
		{
		case HACK_CHECK_ACTIVE_ONOFF:
			{
				ThirdMenu.AppendMenu( MF_STRING|MF_POPUP, ui32ID | 0, _T("OFF") );
				ThirdMenu.AppendMenu( MF_STRING|MF_POPUP, ui32ID | 10, _T("ON") );
			}
			break;
		case HACK_CHECK_ACTIVE_RATIO:
			{				
				for( INT32 i32Ratio = 0 ; i32Ratio < 11 ; i32Ratio++ )
				{
					_stprintf_s( strString, _T("%d %%"), i32Ratio*10 );

					ThirdMenu.AppendMenu( MF_STRING, ui32ID | i32Ratio, strString );
				}
			}
			break;
		}
		pSecondMenu->AppendMenu( MF_POPUP, (UINT_PTR)ThirdMenu.m_hMenu, g_HackCheckMenu[i].m_strName );
	}
}

void CControlToolView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CControlToolView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CControlToolView diagnostics

#ifdef _DEBUG
void CControlToolView::AssertValid() const
{
	CListView::AssertValid();
}

void CControlToolView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CControlToolDoc* CControlToolView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CControlToolDoc)));
	return (CControlToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CControlToolView message handlers
void CControlToolView::OnDestroy()
{
	CListView::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SAFE_DELETE_ARRAY( m_pCommonServerList );
	SAFE_DELETE_ARRAY( m_pGameServerList );
	SAFE_DELETE_ARRAY( m_pBattleServerList );
	SAFE_DELETE_ARRAY( m_pCastServerList );
	SAFE_DELETE_ARRAY( m_pUpdateServerList );

	SAFE_DELETE_ARRAY( m_pi32WarningIcon );

	SAFE_DELETE( m_pSCMSocket);
	SAFE_DELETE( m_pControlSocket );

	SAFE_DELETE( m_pDlgUpdate );
	SAFE_DELETE( m_pDlgUpdateServerList );
}


void CControlToolView::Remove_Old()
{
	CString strDelFilePath;
	TCHAR cpFilePath[MAX_PATH];
	GetModuleFileName(NULL, cpFilePath, MAX_PATH);

	strDelFilePath.Format(_T("%s_Old"), cpFilePath);

	_tremove(strDelFilePath);
}

void CControlToolView::ReStartTool()
{
	while(TRUE)
	{
		//서버들의 업데이트가 끝날때까지 대기.
		/*if(m_pDlgUpdate->IsUpdated())
			break;*/

		if(m_pDlgUpdateServerList == NULL)
			break;

		Sleep(10);
	}

	//현재 실행중인 파일의 이름을 변경한다. - 충돌방지
	TCHAR CurFile_To_Old[MAX_PATH] = {0, };
	TCHAR RenameCommand[MAX_PATH] = {0, };

	_stprintf_s( CurFile_To_Old, _T("rename %s_Old"), m_strCurFilePath);
	_stprintf_s( RenameCommand, _T("rename %s %s"), m_strUpdateFilePath, m_strCurFilePath);

	::_tsystem( CurFile_To_Old );
	::_tsystem( RenameCommand );
	//rename(m_strCurFilePath, CurFile_To_Old);

	// 프로그램을 실행한다.
	SHELLEXECUTEINFO execinfo;
	ZeroMemory( &execinfo, sizeof(execinfo) );
	execinfo.cbSize = sizeof(execinfo);
	execinfo.lpVerb = NULL;
	execinfo.lpFile = m_strCurFilePath;
	execinfo.lpParameters = m_strCurFilePath;
	execinfo.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;
	execinfo.nShow = SW_HIDE;
	execinfo.hInstApp	= NULL;

	
	if ( ! ShellExecuteEx(&execinfo) )
		return;

	//현재 프로세스 종료
	m_bStopApp = TRUE;
}

void CControlToolView::ConnectSCM()
{
	SHGetSpecialFolderPath( NULL, m_strControlToolFile, CSIDL_APPDATA, TRUE );
	_tcscat_s( m_strControlToolFile, MAX_PATH, _T("\\PBControlTool") );
	CreateDirectory( m_strControlToolFile, NULL );
	_tcscat_s( m_strControlToolFile, MAX_PATH, _T("\\PBControlTool.ini") );

	DWORD	SCip;
	INT32	SCPort;
	SCip	= ::GetPrivateProfileInt(	_T("LOGIN"), _T("IP"), 0, m_strControlToolFile );
	SCPort	= ::GetPrivateProfileInt(	_T("LOGIN"), _T("SCMPort"), 39180, m_strControlToolFile );

	//리스트 컨트롤의 화면을 비워준다.
	GetListCtrl().DeleteAllItems();

	CDlgSCMConnect dlgScmConnect;

	if(m_pSCMSocket != NULL)
	{
		m_pSCMSocket->OnDestroy();
		m_pSCMSocket->Init();
	}
	else
		m_pSCMSocket = new CSocketWork;

	while(TRUE)
	{
		if( IDOK == dlgScmConnect.DoModalEx( SCip, SCPort ) )
		{
			//ControlServer가 다른 서버들의 정보를 갖고 있지만 자체 정보는 알수 없음으로
			//Ip 주소를 따로 저장해 둠(SCM과 ControlServer는 같은 머신이라는 가정)
			m_dwControlSVRIp = htonl(dlgScmConnect.m_dwIP);
			if( false == m_pSCMSocket->OnCreate( ::htonl( dlgScmConnect.m_dwIP ), dlgScmConnect.m_ui32Port, FALSE ))
			{
				MessageBox( _T("Service Control Manager is not working. \nCheck the Service Control Manager first.") , _T("Error"), MB_OK);

				//m_pSCMSocket->SocketClose();
				m_pSCMSocket->OnDestroy();

				continue;
			}
			else
			{
				TCHAR szTemp[MAX_PATH];
				_stprintf_s( szTemp, MAX_PATH, _T("%d"), dlgScmConnect.m_dwIP );
				::WritePrivateProfileString( _T("LOGIN"), _T("IP"), szTemp, m_strControlToolFile );
				_stprintf_s( szTemp, MAX_PATH, _T("%d"), dlgScmConnect.m_ui32Port );
				::WritePrivateProfileString( _T("LOGIN"), _T("SCMPort"), szTemp, m_strControlToolFile );

				//m_pCommonServerList[0]._Ip = m_dwControlSVRIp;

				CPacket SendPacket;
				SendPacket.SetProtocol( PROTOCOL_SC_CONTROLTOOL_ON );

				WCHAR strControlServiceName[64];
				MultiByteToWideChar(CP_ACP, 0, m_strServiceName[6], sizeof(char) * 64, strControlServiceName, sizeof(WCHAR)*64);

				SendPacket.WriteData(&strControlServiceName, sizeof(P_SC_CONTROLTOOL_ON));
				m_pSCMSocket->SendPacketMessage( &SendPacket );

				CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();

				pFrame->m_bAdmin_Update = TRUE;

				m_i8AuthService = 1;

				break;
			}
		}
		else
		{
			m_bStopApp = TRUE;
			break;
		}
	}
}



BOOL CControlToolView::Initialization()
{
	SHGetSpecialFolderPath( NULL, m_strControlToolFile, CSIDL_APPDATA, TRUE );
	_tcscat_s( m_strControlToolFile, MAX_PATH, _T("\\PBControlTool") );
	CreateDirectory( m_strControlToolFile, NULL );
	_tcscat_s( m_strControlToolFile, MAX_PATH, _T("\\PBControlTool.ini") );

	TCHAR	strID[MAX_PATH];
	DWORD	dwIP;
	INT32	i32Port;
	::GetPrivateProfileString(			_T("LOGIN"), _T("ID"), _T(""), strID, MAX_PATH, m_strControlToolFile );
	dwIP	= ::GetPrivateProfileInt(	_T("LOGIN"), _T("IP"), 0, m_strControlToolFile );
	i32Port = ::GetPrivateProfileInt(	_T("LOGIN"), _T("Port"), 39120, m_strControlToolFile );

	CDlgLogin dlgLogin;
	if( IDOK == dlgLogin.DoModalEx( strID, dwIP, i32Port ) )
	{
		_tcscpy_s( m_strID, NET_ID_SIZE,	dlgLogin.m_strID.GetBuffer() );
		_tcscpy_s( m_strPass, NET_PW_SIZE,	dlgLogin.m_strPass.GetBuffer() );
		GetMACAddress( &m_ui64MacAddress );

		m_pControlSocket = new CSocketWork;
		m_dwControlSVRIp = htonl(dwIP);		//임시
		if( false == m_pControlSocket->OnCreate( ::htonl( dlgLogin.m_dwIP ), dlgLogin.m_ui32Port, TRUE ))
		{
			MessageBox( _T("Control server connection is down. \n Check Control Server and than Connect"), _T("Error"), MB_OK);

			m_pControlSocket->SocketClose();

			return FALSE;
		}

		TCHAR szTemp[MAX_PATH];
		::WritePrivateProfileString( _T("LOGIN"), _T("ID"), m_strID, m_strControlToolFile );
		_stprintf_s( szTemp, MAX_PATH, _T("%d"), dlgLogin.m_dwIP );
		::WritePrivateProfileString( _T("LOGIN"), _T("IP"), szTemp, m_strControlToolFile );
		_stprintf_s( szTemp, MAX_PATH, _T("%d"), dlgLogin.m_ui32Port );
		::WritePrivateProfileString( _T("LOGIN"), _T("Port"), szTemp, m_strControlToolFile );

		m_i32WarningSwitch = dlgLogin.m_i32WarningSwitch;
#ifdef _DEBUG
		m_i32WarningSwitch = 1; 
#endif

		m_col.pszText = _T("Warning");
		m_col.cx		= m_rcList.Width()/13;
		GetListCtrl().InsertColumn( 4, &m_col);

		SetControlServerInfo(SERVER_STATUS_ON);
	}
	else
	{
		m_bStopApp = TRUE;
		return FALSE;
	}

	return TRUE;
}


void CControlToolView::PreSubclassWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	EnableToolTips(TRUE);

	CListView::PreSubclassWindow();
}

INT_PTR CControlToolView::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	int row, col;
	RECT cellrect;
	row = CellRectFromPoint(point, &cellrect, &col );
	if ( row == -1 )	return -1;
	pTI->hwnd = m_hWnd;
	pTI->uId = (UINT)((row<<10)+(col&0x3ff)+1);
	pTI->lpszText = LPSTR_TEXTCALLBACK;
	pTI->rect = cellrect;
	return pTI->uId;

	//return CListView::OnToolHitTest(point, pTI);
}

int CControlToolView::CellRectFromPoint(CPoint & point, RECT * cellrect, int * col) const
{
	int colnum;
	// 리스트뷰가 LVS_REPORT 모드에있는지 확인
	if( (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT )
		return -1;
	// 현재 화면에 보이는 처음과 끝 Row 를 알아내기
	int row = GetListCtrl().GetTopIndex();
	int bottom = row + GetListCtrl().GetCountPerPage();
	if( bottom > GetListCtrl().GetItemCount() )
		bottom = GetListCtrl().GetItemCount();
	// 컬럼갯수 알아내기
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	// 현재보이는 Row 들간에 루프 돌기
	for( ;row <=bottom;row++)
	{
		// 아이템의 경계 사각형을 가져오고, 점이 포함되는지 체크
		CRect rect;
		GetListCtrl().GetItemRect( row, &rect, LVIR_BOUNDS );
		if( rect.PtInRect(point) )
		{
			// 컬럼찾기
			for( colnum = 0; colnum < nColumnCount; colnum++ )
			{
				int colwidth = GetListCtrl().GetColumnWidth(colnum);
				if( point.x >= rect.left
					&& point.x <= (rect.left + colwidth ) )
				{
					RECT rectClient;
					GetClientRect( &rectClient );
					if( col ) *col = colnum;
					rect.right = rect.left + colwidth;
					// 오른쪽 끝이 클라이언트 영역을 벗어나지 않도록 확인
					if( rect.right > rectClient.right )
						rect.right = rectClient.right;
					*cellrect = rect;
					return row;
				}
				rect.left += colwidth;
			}
		}
	}
	return -1;
}

BOOL CControlToolView::OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
	// ANSI 와 UNICODE 양쪽버젼의 메시지를 처리해야함
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	CString strTipText;
	UINT nID = pNMHDR->idFrom;
	if( nID == 0 ) // NT 에서의 자동생성 툴팁으로부터의 통지
		return FALSE; // 그냥 빠져나간다.
	int row = ((nID-1) >> 10) & 0x3fffff ;
	int col = (nID-1) & 0x3ff;
	//strTipText = GetListCtrl().GetItemText( row, col );
	if( 3 != col )	return FALSE;
	INT32 i32Idx = (INT32)GetListCtrl().GetItemData( row );
	switch( SET_SERVER_TYPE( i32Idx ) )
	{
	case ASC_SERVER_TRANS:		strTipText.Format( _T("ServerCount, UserCount") );	break;
	case ASC_SERVER_CLAN:		strTipText.Format( _T("ServerCount, ClanCount") );	break;
	case ASC_SERVER_MESSENGER:	strTipText.Format( _T("ServerCount, UserCount") );	break;
	case ASC_SERVER_BATTLE:		strTipText.Format( _T("RoomCount, Packet") );	break;
	case ASC_SERVER_UPDATER:	strTipText.Format( _T("UserCount") );	break;
	case ASC_SERVER_GATEWAY:	strTipText.Format( _T("UserCount, WaitIngUserCount") );	break;
	case ASC_SERVER_SIA:		strTipText.Format( _T("0, 0") );	break;
	case ASC_SERVER_GAME:		strTipText.Format( _T("UserCount, RoomCount") );	break;
	}

#ifndef _UNICODE
	if (pNMHDR->code == TTN_NEEDTEXTA)
		lstrcpyn(pTTTA->szText, strTipText, 80);
	else
		_mbstowcsz(pTTTW->szText, strTipText, 80);
#else
	if (pNMHDR->code == TTN_NEEDTEXTA)
		_wcstombsz(pTTTA->szText, strTipText, 80);
	else
		lstrcpyn(pTTTW->szText, strTipText, 80);
#endif
	*pResult = 0;
	return TRUE; // 메시지가 처리되었다.
}

void CControlToolView::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( SERVICE_AUTH_GUEST == m_i8AuthService )
	{
		*pResult = 0; 
		return;
	}

	POSITION posList = GetListCtrl().GetFirstSelectedItemPosition();
	if( NULL == posList )
	{
		*pResult = 0; 
		return;
	}

	CMenu Menu;
	Menu.LoadMenu( IDR_MENU_POP );
	CMenu* pMenu = Menu.GetSubMenu( 0 );
	SetHackCheckMenu( pMenu );

	pMenu->EnableMenuItem( ID_POP_USERKICK, MF_ENABLED );

	bool	bPopup = false;
	INT32	i32SelectIdx = -1;
	while( posList )
	{
		i32SelectIdx = GetListCtrl().GetNextSelectedItem( posList );
		if(m_i8CServerState != SERVER_STATUS_ON)
		{
			DWORD_PTR dwType = GetListCtrl().GetItemData( i32SelectIdx );
			if( -1 != dwType )
			{
				bPopup = true;
				pMenu->EnableMenuItem( ID_POP_STOPSERVER, MF_DISABLED|MF_GRAYED ); 
				pMenu->EnableMenuItem( ID_POP_USERKICK, MF_DISABLED|MF_GRAYED );
				pMenu->EnableMenuItem( ID_POP_TRANSUSERKICK, MF_DISABLED|MF_GRAYED );
				pMenu->EnableMenuItem( ID_POP_WARNINGCLEAR, MF_DISABLED|MF_GRAYED );
				pMenu->EnableMenuItem( ID_POP_NOTALLOWROOMCREATION, MF_DISABLED|MF_GRAYED );

				//DisableBattleServerMenuitem(pMenu);
			}
		}
		else if(m_i8CServerState == SERVER_STATUS_ON && i32SelectIdx == 0)
		{
			DWORD_PTR dwType = GetListCtrl().GetItemData( i32SelectIdx );
			if( -1 != dwType )
			{
				bPopup = true;
				pMenu->EnableMenuItem( ID_POP_STARTSERVER, MF_DISABLED|MF_GRAYED );
				pMenu->EnableMenuItem( ID_POP_USERKICK, MF_DISABLED|MF_GRAYED );
				pMenu->EnableMenuItem( ID_POP_TRANSUSERKICK, MF_DISABLED|MF_GRAYED );
				pMenu->EnableMenuItem( ID_POP_WARNINGCLEAR, MF_DISABLED|MF_GRAYED );
				pMenu->EnableMenuItem( ID_POP_NOTALLOWROOMCREATION, MF_DISABLED|MF_GRAYED );

				//DisableBattleServerMenuitem(pMenu);
			}
		}
		else
		{
			if( 0 > i32SelectIdx || i32SelectIdx >= m_i32AllServerCount ) 
			{
				*pResult = 0; 
				return;
			}

			DWORD_PTR dwType = GetListCtrl().GetItemData( i32SelectIdx );
			if( -1 != dwType )
			{
				bPopup = true;

				pMenu->EnableMenuItem( ID_POP_CONNECT_CONTROL, MF_DISABLED|MF_GRAYED );

				if( dwType == 0)
				{
					pMenu->EnableMenuItem( ID_POP_STARTSERVER, MF_DISABLED|MF_GRAYED );
					pMenu->EnableMenuItem( ID_POP_USERKICK, MF_DISABLED|MF_GRAYED );
					pMenu->EnableMenuItem( ID_POP_TRANSUSERKICK, MF_DISABLED|MF_GRAYED );
					pMenu->EnableMenuItem( ID_POP_WARNINGCLEAR, MF_DISABLED|MF_GRAYED );
					pMenu->EnableMenuItem( ID_POP_NOTALLOWROOMCREATION, MF_DISABLED|MF_GRAYED );
				}
				if( ASC_SERVER_GAME != SET_SERVER_TYPE( dwType ) )
				{
					DisableGameServerMenuitem(pMenu);
				}
				if( ASC_SERVER_TRANS != SET_SERVER_TYPE( dwType ) )
				{
					pMenu->EnableMenuItem( ID_POP_TRANSUSERKICK, MF_DISABLED|MF_GRAYED );
				}
				if( ASC_SERVER_BATTLE != SET_SERVER_TYPE( dwType ) )
				{
					DisableBattleServerMenuitem(pMenu);
				}
			}
		}
	}

	if( false == bPopup )
	{
		*pResult = 0;
		return;
	}

	CPoint ptMouse;
	GetCursorPos(&ptMouse);
	pMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, ptMouse.x, ptMouse.y, this );
}

void CControlToolView::DisableGameServerMenuitem(CMenu* pMenu)
{
	pMenu->EnableMenuItem(ID_POP_USERKICK, MF_DISABLED | MF_GRAYED);
	for (INT32 i = ID_DLLCHEAT_0; i < ID_DLLCHEAT_100 + 1; ++i)
	{
		pMenu->EnableMenuItem(i, MF_DISABLED | MF_GRAYED);
	}
}

void CControlToolView::DisableBattleServerMenuitem(CMenu* pMenu)
{
	pMenu->EnableMenuItem(ID_POP_NOTALLOWROOMCREATION, MF_DISABLED | MF_GRAYED);

	INT32 i32Count = sizeof(g_HackCheckMenu) / sizeof(HACK_CHECK_MENU);
	
	UINT32 ui32ID;
	for( INT32 i = 0 ; i < i32Count ; i++ )
	{
		ui32ID = ID_HACKCHECK_USER_TYPE_START | g_HackCheckMenu[i].eHackType << 4;

		CMenu ThirdMenu;
		ThirdMenu.CreatePopupMenu();

		switch( g_HackCheckMenu[i].m_eActiveType )
		{
		case HACK_CHECK_ACTIVE_ONOFF:
			{
				pMenu->EnableMenuItem( ui32ID | 0, MF_DISABLED|MF_GRAYED );
				pMenu->EnableMenuItem( ui32ID | 10, MF_DISABLED|MF_GRAYED );
			}
			break;
		case HACK_CHECK_ACTIVE_RATIO:
			{				
				for( INT32 i32Ratio = 0 ; i32Ratio < 11 ; i32Ratio++ )
				{
					pMenu->EnableMenuItem( ui32ID | i32Ratio, MF_DISABLED|MF_GRAYED );
				}
			}
			break;
		}
	}
}


void	CControlToolView::OnServerUpdate()
{
	//CPacket SendPacket;
	//SendPacket.SetProtocol( PROTOCOL_SC_SERVER_UPDATE_REQ );
	//m_pSCMSocket->SendPacketMessage( &SendPacket );
	//	
	//OutputAddString( 0, "Prepare to Server Update." );
	//Sleep(300);

	//SendPacket.SetProtocol( PROTOCOL_SC_CONTROLTOOL_UPDATE_REQ );
	//m_pSCMSocket->SendPacketMessage( &SendPacket );

	//m_pDlgUpdate = new CDlgUpdate;

	////Update용 Dialog
	//if(!m_pDlgUpdate->IsCreate())
	//{
	//	m_pDlgUpdate->Create(IDD_DLG_UPDATE);
	//	m_pDlgUpdate->ShowWindow(SW_SHOW);
	//	m_pDlgUpdate->StartUpdate();
	//}
	//else
	//	AfxMessageBox("Update is already done");	

	if(m_pDlgUpdateServerList != NULL)
	{
		SAFE_DELETE( m_pDlgUpdateServerList );
	}
	m_pDlgUpdateServerList = new CDlgUpdateServerList;

	if(m_pDlgUpdateServerList == NULL)
	{
		MessageBox( _T("Update dialog making is failed.") );
		return;
	}

	m_pDlgUpdateServerList->Create(IDD_DLG_UPDATE_LIST);
	m_pDlgUpdateServerList->ShowWindow(SW_SHOW);
}


void	CControlToolView::OnConnectControlSVR()
{
	if(m_i8CServerState != SERVER_STATUS_ON)
	{
		MessageBox( _T("Control Server is not working.\n Start Control Server first"), MB_OK);
		return;
	}
	else if(Initialization())
		SetControlServerInfo(SERVER_STATUS_ON);
}

void	CControlToolView::OnAllServerStart()
{
	if( SERVICE_AUTH_ADMIN > m_i8AuthService )	return;
	
	if( IDCANCEL == MessageBox( _T("To start all server services.\nReally?"), _T("Warning"), MB_OKCANCEL | MB_DEFBUTTON1 ) ) return;
	if( FALSE == CheckAuth() )	return;

	OutputAddString( 0, _T("Please wait for a response from the ControlServer.") );
	
	CPacket SendPacket( PROTOCOL_ASC_ADMIN_SERVER_ALL_START_REQ );
	m_pControlSocket->SendPacketMessage( &SendPacket );

	m_dwLoginTime = GetTickCount();		// 체킹 타임 초기화
}

void	CControlToolView::OnAllServerStop()
{
	if( SERVICE_AUTH_ADMIN > m_i8AuthService )	return;
	if( IDCANCEL == MessageBox( _T("To shut down all server services.\nReally?"), _T("Warning"), MB_OKCANCEL | MB_DEFBUTTON2 ) ) return;
	if( FALSE == CheckAuth() )	return;

	OutputAddString( 0, _T("Please wait for a response from the ControlServer.") );

	CPacket SendPacket( PROTOCOL_ASC_ADMIN_SERVER_ALL_STOP_REQ );		//모든 유저를 오프하게 만듬.	
	m_pControlSocket->SendPacketMessage( &SendPacket );

	m_dwLoginTime = GetTickCount();		// 체킹 타임 초기화
}

void CControlToolView::OnPopStartserver()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	char ServerIp[IP_ADDR_SIZE];
	UINT8 ui8ServerStart = SERVER_OPER_ON;

	//Control Server가 켜져 있을때 인증절차를 거침
	//SCM에 접속중인 상태에서 ControlServer 접속시 인증 무시
	if(m_i8CServerState == SERVER_STATUS_ON)
	{
		if( SERVICE_AUTH_ADMIN > m_i8AuthService )	return;
		if( FALSE == CheckAuth() )	return;
	}

	INT32 i32SelectIdx = -1;
	POSITION posList = GetListCtrl().GetFirstSelectedItemPosition();
	
	if( NULL == posList ) 
	{
		MessageBox( _T("Please select a server from a list."), _T("Error"), MB_OK );
		return;
	}

	OutputAddString( 0, _T("Please wait for a response from the ControlServer.") );

	while( posList )
	{
		i32SelectIdx = GetListCtrl().GetNextSelectedItem( posList );
		INT32 i32Type = (INT32)GetListCtrl().GetItemData( i32SelectIdx );
		if( -1 == i32Type ) continue;

		UINT8 ui8Type;
		if(i32Type == 0)
		{
			ui8Type = 6;								//ControlServerNaming - 박아놓음.
			SetControlServerInfo(SERVER_STATUS_ON);		//얘는 임시
			return;
		}
		else
			ui8Type = (UINT8)i32Type;

		//INT32 i32Idx = SET_SERVER_IDX( i32Type );
		
		/*	0 ~ 10번까지는 공백을 포함한 일반 서버들
			10 ~ 10 + m_iGameServerCount 까지 Game 서버들
			10 + m_iGameServerCount ~ 10 + m_iGameServerCount + m_iBattleServerCount + (공백)1 dedi 서버들
		*/
		struct in_addr ConIp;
		if(i32SelectIdx < 10)
		{
			//CommonServer List는 Control Server의 정보를 빠져 있음.
			//따라서 현재 리스트에 표현되고 있는 Control Server 하나를 감소 시켜야 리스트와 매칭된다.
			if(i32SelectIdx == 0)
			{
				ConIp.s_addr = m_dwControlSVRIp;
				sprintf(ServerIp, "%s", inet_ntoa(ConIp));
			}
			else
			{
				i32SelectIdx -= 1;

				ConIp.s_addr = m_pCommonServerList[i32SelectIdx]._ui32Ip;
				sprintf(ServerIp, "%s", inet_ntoa(ConIp));
			}
		}
		else if(10 <= i32SelectIdx && i32SelectIdx <= 10 + m_iGameServerCount)
		{
			INT32 CurServerNum = i32SelectIdx - 10;

			ConIp.s_addr = m_pGameServerList[CurServerNum]._ui32Ip;
			sprintf(ServerIp, "%s", inet_ntoa(ConIp));
		}
		else
		{
			INT32 CurServerNum = i32SelectIdx - m_iGameServerCount - 11;
			
			ConIp.s_addr = m_pBattleServerList[CurServerNum]._ui32Ip;
			sprintf(ServerIp, "%s", inet_ntoa(ConIp));
		}
		
		//SCM 으로 서버 On 요청
		/*CPacket SendPacket;
		SendPacket.SetProtocol( PROTOCOL_SC_SERVER_OPER_REQ );

		WCHAR strServiceName[SERVICE_NAME_SIZE];
		WCHAR strIpNum[IP_ADDR_SIZE];
		MultiByteToWideChar(CP_ACP, 0, m_strServiceName[ui8Type], sizeof(char) * SERVICE_NAME_SIZE, strServiceName, sizeof(WCHAR)*SERVICE_NAME_SIZE);
		MultiByteToWideChar(CP_ACP, 0, ServerIp, sizeof(char) * IP_ADDR_SIZE, strIpNum, sizeof(WCHAR)*IP_ADDR_SIZE);
		
		char strTemp[MAX_PATH] = {0, };
		sprintf(strTemp, "IP : %ls && Service Name :%ls ", strIpNum, strServiceName);
		OutputAddString( 0, strTemp);

		SendPacket.WriteData( &strIpNum, sizeof(WCHAR)*IP_ADDR_SIZE);
		SendPacket.WriteData( &strServiceName, sizeof(WCHAR) * SERVICE_NAME_SIZE);
		SendPacket.WriteData( &ui8ServerStart, sizeof(INT8));
		m_pSCMSocket->SendPacketMessage( &SendPacket );*/
		
		INT32 i32Idx = SET_SERVER_IDX( i32Type );
		
		CPacket SendPacket( PROTOCOL_ASC_ADMIN_SERVER_START_REQ );		//모든 유저를 오프하게 만듬.	
		SendPacket.WriteData( &ui8Type,	sizeof(UINT8) );
		SendPacket.WriteData( &i32Idx,	sizeof(INT32) );
		m_pControlSocket->SendPacketMessage( &SendPacket );
	}

	m_dwLoginTime = GetTickCount();		// 체킹 타임 초기화
}

void CControlToolView::OnPopStopserver()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( SERVICE_AUTH_ADMIN > m_i8AuthService )	return;
	if( IDCANCEL == MessageBox( _T("Server services to quit.\nReally?"), _T("Warning"), MB_OKCANCEL | MB_DEFBUTTON2 ) ) return;
	if( FALSE == CheckAuth() )	return;

	char ServerIp[IP_ADDR_SIZE];
	UINT8 ui8ServerStart = SERVER_OPER_OFF;

	INT32 i32SelectIdx = -1;
	POSITION posList = GetListCtrl().GetFirstSelectedItemPosition();
	if( NULL == posList ) 
	{
		MessageBox( _T("Please select a server from a list."), _T("Error"), MB_OK );
		return;
	}

	OutputAddString( 0, _T("Please wait for a response from the ControlServer.") );

	UINT8 ui8Type;
	INT32 i32Type;
	
	BOOL	bControlServerSelected = FALSE; // 컨트롤 서버 선택여부

	while( posList )
	{
		i32SelectIdx = GetListCtrl().GetNextSelectedItem( posList );
		i32Type = (INT32)GetListCtrl().GetItemData( i32SelectIdx );
		if( -1 == i32Type ) continue;

		if(i32Type == 0)
		{
			bControlServerSelected = TRUE;
			continue; 
		}
		else
		{
			ui8Type = (UINT8)i32Type;
		}
		//INT32 i32Idx = SET_SERVER_IDX( i32Type );
		
		/*	0 ~ 10번까지는 공백을 포함한 일반 서버들
			10 ~ 10 + m_iGameServerCount 까지 Game 서버들
			10 + m_iGameServerCount ~ 10 + m_iGameServerCount + m_iBattleServerCount + (공백)1 dedi 서버들
		*/
		struct in_addr ConIp;
		if(i32SelectIdx < 10)
		{
			ConIp.s_addr = m_pCommonServerList[i32SelectIdx - 1]._ui32Ip;
			sprintf(ServerIp, "%s", inet_ntoa(ConIp));
		}
		else if(10 <= i32SelectIdx && i32SelectIdx <= 10 + m_iGameServerCount)
		{
			INT32 CurServerNum = i32SelectIdx - 10;

			ConIp.s_addr = m_pGameServerList[CurServerNum]._ui32Ip;
			sprintf(ServerIp, "%s", inet_ntoa(ConIp));
		}
		else
		{
			INT32 CurServerNum = i32SelectIdx - m_iGameServerCount - 11;
			
			ConIp.s_addr = m_pBattleServerList[CurServerNum]._ui32Ip;
			sprintf(ServerIp, "%s", inet_ntoa(ConIp));
		}

		INT32 i32Idx = SET_SERVER_IDX( i32Type );
		CPacket SendPacket( PROTOCOL_ASC_ADMIN_SERVER_STOP_REQ );		//서버 종료 
		SendPacket.WriteData( &ui8Type,	sizeof(UINT8) );
		SendPacket.WriteData( &i32Idx,	sizeof(INT32) );
		m_pControlSocket->SendPacketMessage( &SendPacket );
	}

	// 컨트롤 서버는 가장 나중에 보냅니다.
	if( bControlServerSelected )
	{
		Sleep(5000);	// 5초 대기.

		ui8Type = ASC_SERVER_CONTROL;	//ControlServerNaming - 박아놓음.
		SetControlServerInfo(1);		//얘는 임시

		INT32 i32Idx = SET_SERVER_IDX( 0 );
		CPacket SendPacket( PROTOCOL_ASC_ADMIN_SERVER_STOP_REQ );		//서버 종료 
		SendPacket.WriteData( &ui8Type,	sizeof(UINT8) );
		SendPacket.WriteData( &i32Idx,	sizeof(INT32) );
		m_pControlSocket->SendPacketMessage( &SendPacket );
	}

	m_dwLoginTime = GetTickCount();		// 체킹 타임 초기화
}

void CControlToolView::OnSetMD5Key()
{
	if( SERVICE_AUTH_ADMIN > m_i8AuthService )	return;
	if( IDCANCEL == MessageBox( _T("Change MD5K\nReally?"), _T("Warning"), MB_OKCANCEL | MB_DEFBUTTON2 ) ) return;
	if( FALSE == CheckAuth() )	return;

	CPacket SendPacket( PROTOCOL_ASC_GET_MD5_KEY_REQ );		//MD5 키를 얻어옴
	m_pControlSocket->SendPacketMessage( &SendPacket );

	m_dwLoginTime = GetTickCount();		// 체킹 타임 초기화
}

void CControlToolView::OnAllUserkick()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( SERVICE_AUTH_ADMIN > m_i8AuthService )	return;
	if( IDCANCEL == MessageBox( _T("Diconnect all users. This will take about 60 seconds clear of the time.\nReally?"), _T("Warning"), MB_OKCANCEL | MB_DEFBUTTON2 ) ) return;
	if( FALSE == CheckAuth() )	return;

	CPacket SendPacket( PROTOCOL_ASC_ADMIN_SERVER_KICK_ALLUSER_REQ );		//모든 유저를 오프하게 만듬.	
	m_pControlSocket->SendPacketMessage( &SendPacket );

	m_dwLoginTime = GetTickCount();		// 체킹 타임 초기화
}

void CControlToolView::OnPopUserkick()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( SERVICE_AUTH_ADMIN > m_i8AuthService )	return;
	if( IDCANCEL == MessageBox( _T("Diconnect users of this server. This will take about 60 seconds clear of the time.\nReally?"), _T("Warning"), MB_OKCANCEL | MB_DEFBUTTON2 ) ) return;
	if( FALSE == CheckAuth() )	return;

	INT32 i32SelectIdx = -1;
	POSITION posList = GetListCtrl().GetFirstSelectedItemPosition();
	if( NULL == posList )
	{
		MessageBox( _T("Please select a server from a list."), _T("Warning"), MB_OK);
		return;
	}

	while( posList )
	{
		i32SelectIdx = GetListCtrl().GetNextSelectedItem( posList );
		INT32 i32Type = (INT32)GetListCtrl().GetItemData( i32SelectIdx );
		if( -1 == i32Type )										continue;
		if( ASC_SERVER_GAME != SET_SERVER_TYPE( i32Type ) )		continue;

		INT32 i32Idx = SET_SERVER_IDX( i32Type );

		CPacket SendPacket( PROTOCOL_ASC_ADMIN_SERVER_KICK_ONEUSER_REQ );		//모든 유저를 오프하게 만듬.	
		SendPacket.WriteData( &i32Idx, sizeof(INT32) );
		m_pControlSocket->SendPacketMessage( &SendPacket );
	}

	m_dwLoginTime = GetTickCount();		// 체킹 타임 초기화
}

void CControlToolView::OnPopTransuserkick()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( SERVICE_AUTH_ADMIN > m_i8AuthService )	return;
	if( IDCANCEL == MessageBox( _T("Diconnect users of this server. This will take about 60 seconds clear of the time.\nReally?"), _T("Warning"), MB_OKCANCEL | MB_DEFBUTTON2 ) ) return;
	bool bUserRemaind = false;
	for( INT32 i = 0 ; i < m_iGameServerCount ; i++ )
	{
		if( 0 == m_pGameServerList[i]._i32Arg0 ) continue;
		bUserRemaind = true;
	}
	if( bUserRemaind )
	{
		if( IDCANCEL == MessageBox( _T("Users still remain in the GameServer.\nUsers who have remained don’t save in DB.\nDo you really want to exit?"), _T("Warning"), MB_OKCANCEL | MB_DEFBUTTON2 ) ) return;
	}

	if( FALSE == CheckAuth() )	return;

	CPacket SendPacket( PROTOCOL_ASC_ADMIN_KICK_TRANS_USER_REQ );		//모든 유저를 오프하게 만듬.	
	m_pControlSocket->SendPacketMessage( &SendPacket );

	m_dwLoginTime = GetTickCount();		// 체킹 타임 초기화
}

void CControlToolView::OnPopWarningClear()
{
	if( SERVICE_AUTH_ADMIN > m_i8AuthService )	return;
	if( IDCANCEL == MessageBox( _T("This Server Warning Level Clear \nReally?"), _T("Warning"), MB_OKCANCEL | MB_DEFBUTTON2 ) ) return;
	if( FALSE == CheckAuth() )	return;

	INT32 i32SelectIdx = -1;
	POSITION posList = GetListCtrl().GetFirstSelectedItemPosition();
	if( NULL == posList ) 
	{
		MessageBox( _T("Please select a server from a list."), _T("Error"), MB_OK );
		return;
	}

	// OutputAddString( 0, "Please wait for a response from the ControlServer." );

	while( posList )
	{
		i32SelectIdx = GetListCtrl().GetNextSelectedItem( posList );
		INT32 i32Type = (INT32)GetListCtrl().GetItemData( i32SelectIdx );
		if( -1 == i32Type ) continue;

		m_pi32WarningIcon[ i32SelectIdx ] = WARNING_LEVEL_NO;

		OutputAddString( 0, _T("Warning Level Clear Success") );
	}

	m_dwLoginTime = GetTickCount();		// 체킹 타임 초기화
}

void CControlToolView::OnNoticeLobbynotice()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CDlgNotice dlgNotice;
	if( IDCANCEL == dlgNotice.DoModal() )		return;

	INT32 i32Len = dlgNotice.m_strNotice.GetLength()+1;
	if( NOTICE_STRING_MAX_COUNT < i32Len )
	{
		MessageBox( _T("Long length of the message does not send."), _T("Warning"), MB_OK); 
		return; 
	}

	INT32 i32Type = NOTICE_TYPE_NORMAL;
	TCHAR strNotice[ NOTICE_STRING_MAX_COUNT ];
	_tcscpy_s( strNotice, NOTICE_STRING_MAX_COUNT, dlgNotice.m_strNotice.GetBuffer() );

	CPacket SendPacket( PROTOCOL_ASC_ADMIN_SERVER_ANNOUNCE_REQ );
	SendPacket.WriteData( &i32Type,		sizeof(INT32));
	SendPacket.WriteData( &i32Len,		sizeof(INT32));
	SendPacket.WriteData( strNotice,	sizeof(TCHAR)*i32Len );
	m_pControlSocket->SendPacketMessage( &SendPacket );

	m_dwLoginTime = GetTickCount();		// 체킹 타임 초기화
}

void CControlToolView::OnNoticeEmergencrynotice()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CDlgNotice dlgNotice;
	if( IDCANCEL == dlgNotice.DoModal() )		return;

	INT32 i32Len = dlgNotice.m_strNotice.GetLength()+1;
	if( NOTICE_STRING_MAX_COUNT < i32Len )
	{
		MessageBox( _T("Long length of the message does not send."), _T("Warning"), MB_OK); 
		return; 
	}

	INT32 i32Type = NOTICE_TYPE_EMERGENCY;
	TCHAR strNotice[ NOTICE_STRING_MAX_COUNT ];
	_tcscpy_s( strNotice, NOTICE_STRING_MAX_COUNT, dlgNotice.m_strNotice.GetBuffer() );

	CPacket SendPacket( PROTOCOL_ASC_ADMIN_SERVER_ANNOUNCE_REQ );
	SendPacket.WriteData( &i32Type,		sizeof(INT32));
	SendPacket.WriteData( &i32Len,		sizeof(INT32));
	SendPacket.WriteData( strNotice,	sizeof(TCHAR)*i32Len );
	m_pControlSocket->SendPacketMessage( &SendPacket );

	m_dwLoginTime = GetTickCount();		// 체킹 타임 초기화
}

void	CControlToolView::OnModifyStyleLarge()
{
	GetListCtrl().ModifyStyle( LVS_TYPEMASK, LVS_ICON );
}

void	CControlToolView::OnModifyStyleSmall()
{
	GetListCtrl().ModifyStyle( LVS_TYPEMASK, LVS_SMALLICON );
}

void	CControlToolView::OnModifyStyleList()
{
	GetListCtrl().ModifyStyle( LVS_TYPEMASK, LVS_LIST );
}

void	CControlToolView::OnModifyStyleDetail()
{
	GetListCtrl().ModifyStyle( LVS_TYPEMASK, LVS_REPORT );
}

void	CControlToolView::OnAllWarningClear()
{
	if( SERVICE_AUTH_ADMIN > m_i8AuthService )	return;
	if( IDCANCEL == MessageBox( _T("Warning Clear all server. \nReally?"), _T("Warning"), MB_OKCANCEL | MB_DEFBUTTON2 ) ) return;
	if( FALSE == CheckAuth() )	return;

	for( INT32 i = 0 ; i < m_i32AllServerCount ; i++ )
	{
		m_pi32WarningIcon[ i ] = WARNING_LEVEL_NO;
	}

	OutputAddString( 0, _T("Warning Level Clear All Success") );

	m_dwLoginTime = GetTickCount();		// 체킹 타임 초기화
}

void	CControlToolView::GetMACAddress( UINT64 * pMAC,INT32 i32NICCount )
{
	INT32 i32AdapterCount = 0;

	if( 16 < i32NICCount) i32NICCount = 16;

	IP_ADAPTER_INFO AdapterInfo[16];	// NICs 16개까지	제한
	PIP_ADAPTER_INFO pAdapter = NULL;
	ULONG ulOutBufLen = sizeof(AdapterInfo);

	if( ::GetAdaptersInfo(AdapterInfo, &ulOutBufLen) == ERROR_SUCCESS ) 
	{
		pAdapter = AdapterInfo;
		while(pAdapter && i32AdapterCount < i32NICCount)
		{
			i32AdapterCount++;

			memcpy(pMAC, pAdapter->Address, sizeof(INT64)); // sizeof(char) * MAX_ADAPTER_ADDRESS_LENGTH

			pAdapter = pAdapter->Next;
			pMAC += sizeof(INT64);	// sizeof(char) * MAX_ADAPTER_ADDRESS_LENGTH;
		}
	}
}

BOOL	CControlToolView::CheckAuth()
{
	DWORD dwNowTime = GetTickCount();
	// 한번 로그인 한 후 일정 시간 동안은 무조건 인증으로 처리
	if( m_dwLoginTime + AUTH_TIME > dwNowTime )	return TRUE;

	CDlgAuth dlgAuth;
	if( IDCANCEL == dlgAuth.DoModal() )		return FALSE;
	
	if( ( 0 != dlgAuth.m_strID.Compare( m_strID ) ) || ( 0 != dlgAuth.m_strPass.Compare( m_strPass ) ) )
	{
		MessageBox( _T("Authentication Failed."), _T("Warning"), MB_OK); 
		return FALSE;
	}

	m_dwLoginTime = dwNowTime;
	return TRUE;
}

void	CControlToolView::OutputAddString( INT32 i32Type, TCHAR* strText )
{
	((CMainFrame*)GetParentFrame())->AddString( i32Type, strText );
}


//SCM으로 부터 받는 Packet들 처리
INT32	CControlToolView::PacketParsing_SCM()
{
	CPacket* pPacket = NULL;

	if(m_pSCMSocket != NULL)
	{
		if(m_pSCMSocket->IsConnected())
			pPacket = m_pSCMSocket->GetPacket();
		else
			return DISCONNECT;
	}
			
	if( pPacket == NULL )	return CONNECTED;

	switch( pPacket->GetProtocolID() )
	{
	//Control Tool이 접속가능한 갯수를 초과한 경우 종료.
	case PROTOCOL_SC_CONTROLTOOL_FULL:
		{
			MessageBox( _T("SCM Connection is Full. To exit Program.") );
			m_bStopApp = TRUE;
		}
		break;

	/*
	SCM으로부터 서버의 상태를 받음.
	Control Server의 상태만 받음.
	다른서버들의 정보는 Control Server에 접속하면 받게됨.
	*/
	case PROTOCOL_SC_SERVER_STATUS_ACK:
		{
			P_COMMON_SERVER_STATUS_ACK	ServerState;

			pPacket->ReadData(&ServerState, sizeof(P_COMMON_SERVER_STATUS_ACK));

			//컨트롤서버만
			//나머지 서버정보들은 Control Server로 부터.
			if(ServerState._i8ServerType == 0)
			{
				//컨트롤서버 상태 저장.
				SetControlServerInfo(ServerState._i8ServerStatus);
				m_i8CServerState = ServerState._i8ServerStatus;
			}
		}
		break;

	case PROTOCOL_SC_UPDATE_SERVER_NOTI:
		{
			if(m_pDlgUpdate == NULL)
			{
				m_pDlgUpdate->Create(IDD_DLG_UPDATE);
				m_pDlgUpdate->ShowWindow(SW_SHOW);
			}

			m_pDlgUpdate->m_ctrlCurUpdate.SetWindowText( _T("Updating") );
			
			P_SC_UPDATE_SERVER_NOTI NotiInfo;
			pPacket->ReadData(&NotiInfo, sizeof(P_SC_UPDATE_SERVER_NOTI));

			if(NotiInfo._ui8Percent <= 0)
				NotiInfo._ui8Percent = 0;
			if(NotiInfo._ui8TotalPercent <= 0)
				NotiInfo._ui8TotalPercent = 0;

			TCHAR strTemp[MAX_PATH];
			_stprintf_s( strTemp, _T("Current % : %d && ToTal % :%d && IP : %ls --- Count : %d"), NotiInfo._ui8Percent, NotiInfo._ui8TotalPercent, NotiInfo._strIp, g_i32Num);
			OutputAddString( 0, strTemp);
			g_i32Num++;

			CString		strProgressMessage;
			strProgressMessage.Format(_T("Updating SC: %ls "), NotiInfo._strIp);
			m_pDlgUpdate->m_ctrlCurUpdate.SetWindowText( strProgressMessage );
			m_pDlgUpdate->IncCurProgressBar(NotiInfo._ui8Percent, NotiInfo._ui8TotalPercent);

		}
		break;

	case PROTOCOL_SC_UPDATE_CONTROLTOOL_ACK:
		{
			WCHAR						RecvData[MD5_SIZE] = {0, };
			TCHAR						CurFilePath[MAX_PATH];
			CVersionCreate				OrgVersion;
			
			pPacket->ReadData(&RecvData, sizeof(WCHAR) * MD5_SIZE);

			GetModuleFileName(NULL, CurFilePath, MAX_PATH);

			//현재 실행위치 저장.
			_tcscpy_s( m_strCurFilePath, CurFilePath );
			
			OrgVersion.MakeMD5Value_onlyfile(CurFilePath);
			
			//버젼 체크해서 다를경우 업데이트 시작.
			if(!OrgVersion.CheckVersion_onlyfile(RecvData))
			{
				CString				UpdateFilePath;

				//업데이트 받을 파일 이름 설정.
				UpdateFilePath.Format(_T("%s_Update"), CurFilePath);

				//업데이트 파일 경로 저장.
				_tcscpy_s(m_strUpdateFilePath, UpdateFilePath);

				//다운받을 파일 생성.
				m_hFile = ::CreateFile( UpdateFilePath, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

				CPacket SendPacket(PROTOCOL_SC_TOOLSEND_REQ);
				m_pSCMSocket->SendPacketMessage( &SendPacket );

				OutputAddString( 0, _T("Control Tool Update file is down Load start.") );
			}
			else
				OutputAddString( 0, _T("This is the Latest Version.") );
		}
		break;

	case PROTOCOL_SC_TOOLSEND_HEAD:
		{
			P_SC_TOOLSEND_HEAD	File_Head;

			pPacket->ReadData(&File_Head, sizeof(P_SC_TOOLSEND_HEAD));
			m_i32UpdateFileSize = File_Head._ui32FileSize;

			//파일 사이즈가 0보다 작거나 같은경우 && 파일이 열려 있는경우
			if(m_i32UpdateFileSize <= 0 && m_hFile == INVALID_HANDLE_VALUE)
			{
				//잘못된 정보라고 판단하고 파일을 닫고 삭제.
				CloseHandle(m_hFile);
				m_hFile = INVALID_HANDLE_VALUE;
				_tremove( m_strUpdateFilePath );

				MessageBox( _T("Control Tool Update is Fail."), _T("Error"), MB_OK);

				//컨트롤툴 업데이트 다시 시작한다. -음....???
				CPacket SendPacket( PROTOCOL_SC_TOOLSEND_REQ );
				m_pSCMSocket->SendPacketMessage( &SendPacket );	
			}
		}
		break;

	case PROTOCOL_SC_TOOLSEND_BODY:
		{
			P_SC_FILESEND_BODY	File_Body;
			WCHAR				pBuffer[MAXIMUM_FILE_PACKET_SIZE];

			pPacket->ReadData( &File_Body, sizeof(P_SC_FILESEND_BODY) );
			pPacket->ReadData( pBuffer, File_Body._ui32PacketSize);


			DWORD dwReturnSize;
			if( FALSE == ::WriteFile( m_hFile, pBuffer, File_Body._ui32PacketSize, &dwReturnSize, NULL))
			{
				MessageBox( _T("File Write Error") );
			}

			m_i32CurUpdateFileSize += File_Body._ui32PacketSize;

			if(File_Body._bIsFileEnd)
			{
				if(m_i32CurUpdateFileSize == m_i32UpdateFileSize)
				{
					
					CloseHandle(m_hFile);
					m_hFile = INVALID_HANDLE_VALUE;

					OutputAddString( 0, _T("Update file downloading finish.") );

					ReStartTool();
				}
			}
				
		}
		break;
	}

	return CONNECTED;
}


INT32	CControlToolView::PacketParsing()
{
	CPacket* pPacket = NULL;
	if(m_pControlSocket != NULL)
	{
		if(m_pControlSocket->IsConnected())
			pPacket = m_pControlSocket->GetPacket();
		else
			return DISCONNECT;
	}
		
	if( pPacket == NULL )	return CONNECTED;

	switch( pPacket->GetProtocolID() )
	{
	case PROTOCOL_ASC_NOTALLOW_ROOMCREATION_NTF:
		{
			INT8	i8DediServerIndex = -1;

			pPacket->ReadData(&i8DediServerIndex, sizeof(INT8));

			if(i8DediServerIndex < 0) return WRONG_PACKET;

			if(this->m_pBattleServerList[i8DediServerIndex]._i32IsActive)
				m_pNotAllowRoomCreationList[i8DediServerIndex] = 1;

		}break;
	case PROTOCOL_ASC_CHANGE_HACKCHECKRATIO_NTF:
	{
		ChangedHackCheckRatio sHackCheckRatio;

		pPacket->ReadData(&sHackCheckRatio, sizeof(ChangedHackCheckRatio));

		switch (sHackCheckRatio.m_ui8ServerType)
		{
		case ASC_SERVER_GAME:
		{
			m_pGameHackCheckRatio[sHackCheckRatio.m_ui8ServerIdx].m_aui8HackCheckRatio[sHackCheckRatio.m_ui8HackCheckType] = sHackCheckRatio.m_ui8ChangedCheckRatio;
		}
		break;
		case ASC_SERVER_BATTLE:
		{
			// save changed hack check ratio
			m_pBattleHackCheckRatio[sHackCheckRatio.m_ui8ServerIdx].m_aui8HackCheckRatio[sHackCheckRatio.m_ui8HackCheckType] = sHackCheckRatio.m_ui8ChangedCheckRatio;
		}
		break;
		}

		// Update Hack Check ratio UI
		UpdateHackCheckRatioUI((ASC_SERVER_TYPE)sHackCheckRatio.m_ui8ServerType, sHackCheckRatio.m_ui8ServerIdx);
	}
	break;
	case PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS	://연결되었음
		{

			pPacket->ReadData( &m_i32UpdateTime, sizeof(UINT32) );
			pPacket->ReadData( &m_ControlVer, sizeof(SS_VERSION));

			// 컨트롤서버와 버전 비교
			/*SS_VERSION	ToolVer;
			if ( (ToolVer._ui8VerGame != m_ControlVer._ui8VerGame) || (ToolVer._ui16VerBuild != m_ControlVer._ui16VerBuild) || (ToolVer._ui16VerSS != m_ControlVer._ui16VerSS) )
			{
				MessageBox( "It does not match the Control server version.", "Warning", MB_OK );
				PostQuitMessage(0);
			}*/
			

			//처음 보내는 패킷을 보낸다. 		
			CPacket SendPacket( PROTOCOL_ASC_CONFIRMATION_REQ ); 
			INT32 ConnectType= ASC_CONNECT_TYPE_ADMIN;	//관제툴
			SendPacket.WriteData( &ConnectType,			sizeof(INT32) );
			SendPacket.WriteData( m_strID,				sizeof(TCHAR)*NET_ID_SIZE );
			SendPacket.WriteData( m_strPass,			sizeof(TCHAR)*NET_PW_SIZE );
			SendPacket.WriteData( &m_ui64MacAddress,	sizeof(UINT64) );
			m_pControlSocket->SendPacketMessage( &SendPacket );
		}

		
		break; 
	case PROTOCOL_ASC_ADMIN_SERVER_START_ACK:
		{
			INT32 i32Rv;
			TCHAR strMessage[ MAX_PATH ];

			pPacket->ReadData( &i32Rv, sizeof(INT32) ); 
			if( -1 < i32Rv )	_tcscpy_s( strMessage, MAX_PATH, _T("Starting the server success.") );
			else				_tcscpy_s( strMessage, MAX_PATH, _T("Starting the server fail.") );

			((CMainFrame*)GetParentFrame())->AddString( 0, strMessage );
		}
		break;
	case PROTOCOL_ASC_ADMIN_SERVER_STOP_ACK:
		{
			INT32 i32Rv;
			TCHAR strMessage[ MAX_PATH ];

			pPacket->ReadData( &i32Rv, sizeof(INT32) ); 
			if( -1 < i32Rv )	_tcscpy_s( strMessage, MAX_PATH, _T("Stop the server success.") );
			else				_tcscpy_s( strMessage, MAX_PATH, _T("Stop the server fail.") );

			((CMainFrame*)GetParentFrame())->AddString( 0, strMessage );
		}
		break;
	case PROTOCOL_ASC_ADMIN_CONTROL_ANNOUNCE_REQ:
		{
			INT32 i32Len;
			TCHAR strMessage[ NOTICE_STRING_MAX_COUNT ];

			pPacket->ReadData( &i32Len,		sizeof(INT32) );	if( i32Len > MAX_PATH ) i32Len = MAX_PATH;
			pPacket->ReadData( strMessage,	sizeof(TCHAR)*i32Len );
			OutputAddString( 0, strMessage );
		}
		break;
	case PROTOCOL_ASC_CONFIRMATION_ACK:
		{
			INT32 i32Rv;
			pPacket->ReadData( &i32Rv,						sizeof(INT32) );
			if( -1 < i32Rv )
			{
				m_dwLoginTime = GetTickCount();

				pPacket->ReadData( &m_iCommonServerCount,	sizeof(INT32) );
				pPacket->ReadData( &m_iGameServerCount,		sizeof(INT32) );
				pPacket->ReadData( &m_iBattleServerCount,	sizeof(INT32) );
				pPacket->ReadData( &m_i32CastServerCount,	sizeof(INT32) );
				pPacket->ReadData( &m_i32UpdateServerCount,	sizeof(INT32) );
				pPacket->ReadData( &m_i8AuthService,		sizeof(INT8) );	
				pPacket->ReadData(m_pBattleHackCheckRatio, sizeof(ServerHackCheckRatio) * m_iBattleServerCount);
				pPacket->ReadData( m_pNotAllowRoomCreationList,  sizeof(INT8) * m_iBattleServerCount);
				pPacket->ReadData(m_pGameHackCheckRatio, sizeof(GSHackCheckRatio) * m_iGameServerCount);

				m_i32AllServerCount = m_iCommonServerCount + m_i32UpdateServerCount + m_iGameServerCount + m_i32CastServerCount + m_iBattleServerCount + m_i32Separator;
				m_pi32WarningIcon	= new UINT32[ m_i32AllServerCount ];
				m_pCommonServerList	= new SERVER_INFO_COMMON[ m_iCommonServerCount		];
				m_pGameServerList	= new SERVER_INFO_COMMON[ m_iGameServerCount		];
				m_pBattleServerList	= new SERVER_INFO_COMMON[ m_iBattleServerCount		];
				m_pCastServerList	= new SERVER_INFO_COMMON[ m_i32CastServerCount		];
				m_pUpdateServerList	= new SERVER_INFO_COMMON[ m_i32UpdateServerCount	];

			/*	for (INT32 k = 0; k < m_iGameServerCount; ++k)
				{
					UpdateHackCheckRatioUI(ASC_SERVER_GAME, k);
				}

				for (INT32 h = 0; h < m_iBattleServerCount; ++h)
				{
					UpdateHackCheckRatioUI(ASC_SERVER_BATTLE, h);
				}*/

				INT32 i32Idx;
				for( INT32 i = 0; i < m_i32AllServerCount; i++ )
				{
					LV_ITEM lvitem; 
					lvitem.mask		= LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM; 
					lvitem.iItem	= i + 1;			//Control Server 상태 추가를 위해서 +1
					lvitem.iSubItem	= 0; 
					lvitem.pszText	= _T(""); 
					lvitem.iImage	= -1; 
					i32Idx  = GetListCtrl().InsertItem(&lvitem);

					GetListCtrl().SetItemText( i32Idx, 0, _T("") );
					GetListCtrl().SetItemText( i32Idx, 1, _T("") );
					GetListCtrl().SetItemText( i32Idx, 2, _T("") );
					GetListCtrl().SetItemText( i32Idx, 3, _T("") );
					GetListCtrl().SetItemText( i32Idx, 4, _T("") );
					GetListCtrl().SetItemText( i32Idx, 5, _T("") );

					m_pi32WarningIcon[i] = WARNING_LEVEL_NO;
				}

				if( SERVICE_AUTH_ADMIN <= m_i8AuthService )
				{
					CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
					pFrame->m_bAdmin = TRUE;
					pFrame->m_bAdmin_Update = FALSE;
				}
			}
			else
			{
				m_bStopApp = TRUE;
			}
		}
		break;
	case PROTOCOL_ASC_GET_MD5_KEY_ACK:
		{
			TCHAR strMD5Key[2][ NET_MD5_KEY_SIZE+1 ];

			pPacket->ReadData( strMD5Key[0],	NET_MD5_KEY_SIZE );
			pPacket->ReadData( strMD5Key[1],	NET_MD5_KEY_SIZE );

			strMD5Key[0][ NET_MD5_KEY_SIZE ] = '\0';
			strMD5Key[1][ NET_MD5_KEY_SIZE ] = '\0';

			CDlgMd5Key dlgMd5Key;
			if( IDCANCEL == dlgMd5Key.DoModalEx( strMD5Key ) )		return WRONG_PACKET;
			_tcscpy_s( strMD5Key[0], NET_MD5_KEY_SIZE+1, dlgMd5Key.m_strMd5Key00.GetBuffer() );
			_tcscpy_s( strMD5Key[1], NET_MD5_KEY_SIZE+1, dlgMd5Key.m_strMd5Key01.GetBuffer() );
			strMD5Key[0][NET_MD5_KEY_SIZE] = '\0';
			strMD5Key[1][NET_MD5_KEY_SIZE] = '\0';

			CPacket SendPacket( PROTOCOL_ASC_SET_MD5_KEY_REQ );
			SendPacket.WriteData( strMD5Key[0],	NET_MD5_KEY_SIZE );
			SendPacket.WriteData( strMD5Key[1],	NET_MD5_KEY_SIZE );
			m_pControlSocket->SendPacketMessage( &SendPacket );
		}
		break;
	case PROTOCOL_ASC_STATE_ACK:
		{
			pPacket->ReadData( &m_i32MaxCCU,			sizeof(INT32) );
			pPacket->ReadData( &m_i32MaxCCUTime,		sizeof(UINT32) );
			pPacket->ReadData( &m_i32NowCCU,			sizeof(INT32) );
			pPacket->ReadData( &m_i32NowCCUTime,		sizeof(UINT32) );
			pPacket->ReadData( m_pCommonServerList,		( sizeof(SERVER_INFO_COMMON)	* m_iCommonServerCount ) ); 
			pPacket->ReadData( m_pGameServerList,		( sizeof(SERVER_INFO_COMMON)	* m_iGameServerCount   ) ); 
			pPacket->ReadData( m_pBattleServerList,		( sizeof(SERVER_INFO_COMMON)	* m_iBattleServerCount ) ); 
			pPacket->ReadData( m_pCastServerList,		( sizeof(SERVER_INFO_COMMON)	* m_i32CastServerCount ) ); 
			pPacket->ReadData( m_pUpdateServerList,		( sizeof(SERVER_INFO_COMMON)	* m_i32UpdateServerCount ) ); 
			pPacket->ReadData( m_pHackCount,			(UINT16)(sizeof(UINT32) * HACK_TYPE_MAX) );

			UINT32 HackCount = 0; 
			for(INT32 i = 0; i < HACK_TYPE_MAX; i++)
			{
				HackCount = HackCount + m_pHackCount[i]; 
			}
			m_HackCount = HackCount; 
			((CMainFrame*)GetParentFrame())->UpdateHackList( m_pHackCount );

			m_i32ServerIdx = 0;
			SetCaption();
			m_bCommonServer = TRUE;
			SetCommonServerInfo(m_pCommonServerList,	m_iCommonServerCount);
			m_bCommonServer = FALSE;

			SetCommonServerInfo(m_pUpdateServerList,	m_i32UpdateServerCount);
			SetCommonServerInfo(m_pCastServerList,		m_i32CastServerCount);
			SetCommonServerInfo(m_pGameServerList,		m_iGameServerCount);			

			for(int i=0; i<m_iBattleServerCount; i++)
			{
				if(m_pBattleServerList[i]._i32IsActive == SERVER_ACTIVE_STATE_DISCONNECT )
					m_pNotAllowRoomCreationList[i] = 0;

				if(m_pNotAllowRoomCreationList[i] == 1)
					m_pBattleServerList[i]._i32IsActive = SERVER_ACTIVE_STATE_NOT_ALLOW_CREATE_ROOM;
			}

			SetCommonServerInfo(m_pBattleServerList,	m_iBattleServerCount); 
		}
		break;

	case PROTOCOL_ASC_SERVER_LOADING_RESULT:
		{
			INT32	i32ServerIdx;
			UINT64	ui64LoadingResult;
			char	strErrorName[30];
			INT32	i32DestroyReason;

			pPacket->ReadData(&i32ServerIdx, sizeof(INT32));
			pPacket->ReadData(&i32DestroyReason, sizeof(INT32));

			if (i32DestroyReason > SERVER_ERROR_NO)
			{
				TCHAR strServerName[SERVICE_NAME_SIZE];

				switch(i32ServerIdx)
				{
				case ASC_SERVER_TRANS		: _stprintf_s( strServerName, _T("TransServer") );		break;
				case ASC_SERVER_CLAN		: _stprintf_s( strServerName, _T("ClanServer") );		break;
				case ASC_SERVER_MESSENGER	: _stprintf_s( strServerName, _T("MessengerServer") );	break;
				case ASC_SERVER_BATTLE		: _stprintf_s( strServerName, _T("BattleServer") );		break;
				case ASC_SERVER_ZLOG		: _stprintf_s( strServerName, _T("ZlogServer") );		break;
				case ASC_SERVER_SIA			: _stprintf_s( strServerName, _T("SIAServer") );		break;
				case ASC_SERVER_GAME		: _stprintf_s( strServerName, _T("GameServer") );		break;
				case ASC_SERVER_CAST		: _stprintf_s( strServerName, _T("CastServer") );		break;
				default						: _stprintf_s( strServerName, _T("UnKnown Error") );	break;
				}

				CString ErrorMessage;
				ErrorMessage.Format( _T("Server Name : %s \nReason : %s "), strServerName, g_DestroyServerMessage[i32DestroyReason]); 
				MessageBox( ErrorMessage, _T("Warning"), MB_OK );
			}
		}
		break;
	}

	return CONNECTED;
}

void CControlToolView::SetCaption(void)
{
	time_t timer;
	timer = time(NULL);
	struct tm *timeVal;
	timeVal = localtime(&timer);
	
	TCHAR	strTemp[ 1024 ];
	_stprintf_s( strTemp, MAX_PATH, _T("Server Ver : %d.%d, Daily MAX CCU : %d, MAX CCU Time : %2dH : %2dM, NOW CCU : %d   ::: Hack Count = %d"), 
		m_ControlVer._ui8VerGame, m_ControlVer._ui16VerSS, m_i32MaxCCU, (m_i32MaxCCUTime/10000)%100, (m_i32MaxCCUTime/100)%100, m_i32NowCCU, m_HackCount );

	((CMainFrame*)GetParentFrame())->SetCaption( strTemp );

	if(m_i32OldTime != timeVal->tm_hour)
	{
		((CMainFrame*)::AfxGetMainWnd())->SetGraph(m_i32MaxCCU);
		m_i32OldTime = timeVal->tm_hour;
	}

	return; 
}


void CControlToolView::SetControlServerInfo(INT8 CurState)
{
	LV_ITEM lvitem;
	TCHAR	strTemp[MAX_PATH];

	lvitem.mask		= LVIF_TEXT | LVIF_IMAGE;
	
	if(CurState == 1)
	{
		lvitem.iImage	= 1;
	}
	else
	{
		lvitem.iImage	= CurState;
	}

	lvitem.iSubItem = 0;
	lvitem.pszText	= _T("Control");
	lvitem.iItem	= 1;

	//GetListCtrl().DeleteItem(0);
	GetListCtrl().DeleteAllItems();
	GetListCtrl().InsertItem(&lvitem);

	GetIP( m_dwControlSVRIp, strTemp );
	GetListCtrl().SetItemText( 0, 2, strTemp );

	m_i8CServerState = CurState;
}


void CControlToolView::SetCommonServerInfo( SERVER_INFO_COMMON * pList, INT32 iCount)
{
	if (iCount <= 0) return;

	struct	in_addr	ConIp;
	TCHAR	strTemp[ MAX_PATH ];

	if(m_bCommonServer)
		m_i32ServerIdx	= m_i32ServerIdx + 1;

	LV_ITEM lvitem;
	lvitem.mask		= LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	lvitem.iSubItem = 0;

	for( INT32 i = 0 ; i < iCount ; i++ )
	{
		m_pi32WarningIcon[m_i32ServerIdx] = m_pi32WarningIcon[m_i32ServerIdx] | pList[i]._ui32WarningInfo[WARNING_TYPE_NOTICE]; 

		//View Warning 
		if ( 0 == m_i32WarningSwitch )m_pi32WarningIcon[m_i32ServerIdx] = WARNING_LEVEL_NO; 

		//Set Icon 
		lvitem.iImage = pList[i]._i32IsActive;
		if( WARNING_LEVEL_NO != m_pi32WarningIcon[m_i32ServerIdx] )
		{
			lvitem.iImage = WARINING_IMAGE;
		}
		
		switch( pList[i]._ui8Type )
		{
		case ASC_SERVER_TRANS		: _tcscpy_s( strTemp, MAX_PATH, _T("TRANS") );				break; 
		case ASC_SERVER_CLAN		: _tcscpy_s( strTemp, MAX_PATH, _T("CLAN") );				break; 
		case ASC_SERVER_MESSENGER	: _tcscpy_s( strTemp, MAX_PATH, _T("MESS") );				break; 
		case ASC_SERVER_GATEWAY		: _tcscpy_s( strTemp, MAX_PATH, _T("GATE") );				break; 
		case ASC_SERVER_SIA			: _tcscpy_s( strTemp, MAX_PATH, _T("SIA") );				break; 
		case ASC_SERVER_ZLOG		: _tcscpy_s( strTemp, MAX_PATH, _T("ZLOG") );				break; 
		//case ASC_SERVER_BILLING		: _tcscpy_s( strTemp, MAX_PATH, _T("BILLING") );		break; 
		case ASC_SERVER_UPDATER		: _stprintf_s( strTemp, MAX_PATH, _T("UPDATER[%03d]"), i);	break;		
		case ASC_SERVER_GAME		: 
		{
			_stprintf_s(strTemp, MAX_PATH, _T("GAME[%03d]"), i);
			UpdateHackCheckRatioUI((ASC_SERVER_TYPE)pList[i]._ui8Type, i);
		}
		break;
		case ASC_SERVER_CAST		: _stprintf_s( strTemp, MAX_PATH, _T("Cast[%03d]"), i);		break;
		case ASC_SERVER_BATTLE		: 
			_stprintf_s( strTemp, MAX_PATH, _T("Battle[%03d]"), i );		// Update UI Column Information
			UpdateHackCheckRatioUI((ASC_SERVER_TYPE)pList[i]._ui8Type, i);
			break;  
		default						: _stprintf_s( strTemp, MAX_PATH, _T("???????") );			break; 
		}

		//Output MessageBox에 메시지 출력 
		if(0 != m_i32WarningSwitch && pList[i]._ui32WarningInfo[WARNING_TYPE_NOTICE] != WARNING_LEVEL_NO)
		{
			TCHAR strMessage[MAX_PATH];
			SYSTEMTIME	SystemTime;	
			::GetLocalTime( &SystemTime );
			_stprintf_s(strMessage, MAX_PATH, _T("[%d-%d:%d:%d]%s Warning Detected OX%x"), SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, strTemp, pList[i]._ui32WarningInfo); 
			OutputAddString( 0, strMessage );
		}

		lvitem.pszText	= strTemp;
		lvitem.lParam	= SET_SERVER_FLAG( i, pList[i]._ui8Type );

		lvitem.iItem	= m_i32ServerIdx;
		GetListCtrl().SetItem(&lvitem);

		CString strDest;
		strDest.Format( _T("%d.%d."), pList[i]._ServerVersion._ui8VerGame, pList[i]._ServerVersion._ui16VerSS);
		GetListCtrl().SetItemText( m_i32ServerIdx, 1, strDest );

		GetIP( pList[i]._ui32Ip, strTemp );
		GetListCtrl().SetItemText( m_i32ServerIdx, 2, strTemp );

		//Is Activate Server 
		if( SERVER_ACTIVE_STATE_DISCONNECT == pList[i]._i32IsActive )
		{
			GetListCtrl().SetItemText( m_i32ServerIdx, 1, _T("") );
			GetListCtrl().SetItemText( m_i32ServerIdx, 3, _T("") );
			GetListCtrl().SetItemText( m_i32ServerIdx, 4, _T("") );
			GetListCtrl().SetItemText( m_i32ServerIdx, 5, _T("") );
			GetListCtrl().SetItemText( m_i32ServerIdx, 6, _T("") );

			// 데디접속시 컨트롤 서버에서 hack ratio를 보내는데
			// 이 정보를 삭제하기 때문에 막습니다.
			//if(pList[i]._Type == ASC_SERVER_BATTLE) 
			//{
			//	ClearHackCheckRatio(i);
			//}

			m_i32ServerIdx++;  
			continue; 
		}
		
		_stprintf_s( strTemp, MAX_PATH, _T("[Cpu %4d Handle %7d ]"), pList[i]._i32Cpu, pList[i]._i32Handle );
		GetListCtrl().SetItemText( m_i32ServerIdx, 3, strTemp );

		TCHAR strName[5][64];
		strName[2][0] = strName[3][0] = strName[4][0] = 0; 

		switch( pList[i]._ui8Type )
		{
		case ASC_SERVER_TRANS		: _tcscpy_s( strName[0], 64, _T("Server") );	_tcscpy_s(strName[1], 64, _T("User") );																													break; 
		case ASC_SERVER_CLAN		: _tcscpy_s( strName[0], 64, _T("Server") );	_tcscpy_s(strName[1], 64, _T("Clan") );																													break; 
		case ASC_SERVER_MESSENGER	: _tcscpy_s( strName[0], 64, _T("Server") );	_tcscpy_s(strName[1], 64, _T("User") );																													break;  
		case ASC_SERVER_GATEWAY		: _tcscpy_s( strName[0], 64, _T("User") );		_tcscpy_s(strName[1], 64, _T("Active") );																												break; 
		case ASC_SERVER_UPDATER		: _tcscpy_s( strName[0], 64, _T("User") );		_tcscpy_s(strName[1], 64, _T("Admin ") );																												break;  
		case ASC_SERVER_GAME		: _tcscpy_s( strName[0], 64, _T("User") );		_tcscpy_s(strName[1], 64, _T("Room  ") );			_tcscpy_s( strName[2], 64, _T("Packet Count(sec)") );				_tcscpy_s( strName[3], 64, _T("Bandwith Recv(sec)") );	break; 
		case ASC_SERVER_BATTLE		: _tcscpy_s( strName[0], 64, _T("Room") );		_tcscpy_s(strName[1], 64, _T("Bandwith Recv") );	_tcscpy_s( strName[2], 64, _T("Bandwith Send") );													break; 
		default						: _tcscpy_s( strName[0], 64, _T("") );			_tcscpy_s(strName[1], 64, _T("") );																														break; 
		}

		//OutputAddString( 0, "Warning Level Clear All Success" );
		_stprintf_s( strTemp, MAX_PATH, _T("[%s :%8d, %s :%8d, %s :%8d, %s :%8d, %s :%8d]"),
			strName[0], pList[i]._i32Arg0,
			strName[1], pList[i]._i32Arg1,
			strName[2], pList[i]._i32Arg2,
			strName[3], pList[i]._i32Arg3,
			strName[4], pList[i]._i32Arg4);

		GetListCtrl().SetItemText( m_i32ServerIdx, 5, strTemp );

		SetWarningText( strTemp, m_pi32WarningIcon[m_i32ServerIdx] ); 
		GetListCtrl().SetItemText( m_i32ServerIdx, 4, strTemp );

		m_i32ServerIdx++;  
	}

	lvitem.mask		= LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	lvitem.iSubItem = 0; 

	lvitem.iItem	= m_i32ServerIdx;
	lvitem.pszText	= _T("");
	lvitem.lParam	= -1;
	lvitem.iImage	= -1;

	GetListCtrl().SetItem(&lvitem);
	GetListCtrl().SetItemText( m_i32ServerIdx, 1, _T("") );
	GetListCtrl().SetItemText( m_i32ServerIdx, 2, _T("") );
	GetListCtrl().SetItemText( m_i32ServerIdx, 3, _T("") );
	GetListCtrl().SetItemText( m_i32ServerIdx, 4, _T("") );
	GetListCtrl().SetItemText( m_i32ServerIdx, 5, _T("") );

	m_i32ServerIdx++; 

	return; 
}

void CControlToolView::UpdateHackCheckRatioUI(ASC_SERVER_TYPE eServerType, int iBattleServerIdx)
{
	CString strHackCheckRatio;

	switch (eServerType)
	{
	case ASC_SERVER_GAME:
	{
		strHackCheckRatio.Format(_T("DX:%d%%"),
			m_pGameHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio[GS_HACK_TYPE_DLL_CHEAT] * 10
			);
	}
	break;
	case ASC_SERVER_BATTLE:
	{
		strHackCheckRatio.Format(_T("SP:%d ES:%d WL:%d GV:%d RA:%d TH:%d BU:%d RC:%d WP:%d TR:%d FS:%d GM:%d RT:%d RP:%d C4:%d CT:%d HD:%d IT:%d PA:%d WE:%d UD:%d MV:%d SH:%d RG: %d"),
			m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio[HACK_TYPE_SPEED],
			m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio[HACK_TYPE_ESCAPE],
			m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio[HACK_TYPE_WALLSHOT],
			m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio[HACK_TYPE_GRAVITY],
			m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio[HACK_TYPE_RANGE],
			m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio[HACK_TYPE_GRENADE],
			m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio[NOT_USE_HACK_TYPE_MAGAZINE],
			m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio[HACK_TYPE_RECALL],
			m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio[HACK_TYPE_WEAPON],

			m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio[HACK_TYPE_TRAINING],
			m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio[HACK_TYPE_FIRE_SPEED],
			m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio[HACK_TYPE_GRAVITY_MOVE],
			m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio[HACK_TYPE_RESPAWN_TIME],
			m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio[HACK_TYPE_RESPAWN_POSITION],
			m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio[HACK_TYPE_INSTALL_C4],
			m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio[HACK_TYPE_CLIENT_TIME],
			m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio[HACK_TYPE_HEADSHOT],
			m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio[HACK_TYPE_ITEM],
			m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio[HACK_TYPE_WEAPON_PARAM],
			m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio[HACK_TYPE_WEAPON_NOT_FOUND],
			m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio[HACK_TYPE_UDP_VERSION],
			m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio[HACK_TYPE_PAUSE_MOVE],
			m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio[HACK_TYPE_PAUSE_SHOOT],
			m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio[HACK_TYPE_RPG_CHEAT]
			);
	}
	break;
	}



	GetListCtrl().SetItemText(m_i32ServerIdx, 6, strHackCheckRatio.GetBuffer(strHackCheckRatio.GetLength()));

	UpdateData(FALSE);
}

void CControlToolView::ClearHackCheckRatio(int iBattleServerIdx)
{	
	memset(m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio, 0, sizeof(m_pBattleHackCheckRatio[iBattleServerIdx].m_aui8HackCheckRatio));
}

UINT32	CControlToolView::GetFlagWarningLevel( INT32 i32WarningLevel )
{
	if( WARNING_LEVEL_NO >= i32WarningLevel )		return 0;
	if( WARNING_LEVEL_COUNT - 1 < i32WarningLevel )	return 0;

	return (0x1 << (i32WarningLevel - 1));
}

void CControlToolView::SetWarningText( TCHAR * strString, UINT32 iWarningValue)
{
	if( iWarningValue == WARNING_LEVEL_NO) 
	{
		_tcscpy_s(strString, MAX_PATH, _T("") );
		return; 
	}

	_tcscpy_s( strString, MAX_PATH, _T("") );
	if(GetFlagWarningLevel( WARNING_LEVEL_PERFOMANCE )		& iWarningValue)	_tcscat_s( strString, MAX_PATH, _T("PERFOMANCE | ") ); 
	if(GetFlagWarningLevel( WARNING_LEVEL_NETWORK)			& iWarningValue)	_tcscat_s( strString, MAX_PATH, _T("NETWORK | ") ); 
	if(GetFlagWarningLevel( WARNING_LEVEL_DB )				& iWarningValue)	_tcscat_s( strString, MAX_PATH, _T("DB | ") ); 
	if(GetFlagWarningLevel( WARNING_LEVEL_GAME_GUARD )		& iWarningValue)	_tcscat_s( strString, MAX_PATH, _T("GAMEGUARD | ") ); 
	if(GetFlagWarningLevel( WARNING_LEVEL_CRASH )			& iWarningValue)	_tcscat_s( strString, MAX_PATH, _T("CRASH ") ); 

	return; 
}

void CControlToolView::OnPopNotallowroomcreation()
{
	if( IDCANCEL == MessageBox( _T("Do you want to really apply?"), _T("Warning"), MB_OKCANCEL ) ) 
	{
		return;
	}

	INT32 i32SelectIdx = -1;
	POSITION posList = GetListCtrl().GetFirstSelectedItemPosition();
	if( NULL == posList ) 
	{
		MessageBox( _T("Please select a server from a list."), _T("Error"), MB_OK );
		return;
	}

	while( posList )
	{
		i32SelectIdx = GetListCtrl().GetNextSelectedItem( posList );
		INT32 i32Type = (INT32)GetListCtrl().GetItemData( i32SelectIdx );
		if( -1 == i32Type ) continue;

		UINT8 ui8Type = (UINT8)i32Type;

		UINT8 ui8ServerIdx = (UINT8)SET_SERVER_IDX( i32Type );
 
		// Send Data
		CPacket SendPacket( PROTOCOL_ASC_NOTALLOW_ROOMCREATION_NTF  );
		SendPacket.WriteData( &ui8ServerIdx,	sizeof(UINT8) );
		m_pControlSocket->SendPacketMessage( &SendPacket );

		TCHAR strLog[MAX_PATH];
		_stprintf_s( strLog, MAX_PATH, _T("Send - Dedi Not Allow Room Creation / Server Idx : %d"), ui8ServerIdx );
		OutputAddString( 0, strLog );
	}


	for(int i=0; i<m_iBattleServerCount; i++)
	{
		if(m_pNotAllowRoomCreationList[i] == 1)
			m_pBattleServerList[i]._i32IsActive = SERVER_ACTIVE_STATE_NOT_ALLOW_CREATE_ROOM;
	}

	//SetCommonServerInfo(m_pBattleServerList,	m_iBattleServerCount); 

	m_dwLoginTime = GetTickCount();		// 체킹 타임 초기화
}


void CControlToolView::SendChangedHackRatio(INT32 i32HackCheckType, UINT8 ui8ChangedRatio, ASC_SERVER_TYPE eServerType)
{
	INT32 i32SelectIdx = -1;
	POSITION posList = GetListCtrl().GetFirstSelectedItemPosition();
	if (NULL == posList)
	{
		MessageBox(_T("Please select a server from a list."), _T("Error"), MB_OK);
		return;
	}

	OutputAddString(0, _T("Please wait for a response from the ControlServer."));

	while (posList)
	{
		i32SelectIdx = GetListCtrl().GetNextSelectedItem(posList);
		INT32 i32Type = (INT32)GetListCtrl().GetItemData(i32SelectIdx);
		if (-1 == i32Type) continue;

		// Set Data
		ChangedHackCheckRatio sHackCheckRatio;
		sHackCheckRatio.m_ui8ServerType = (UINT8)eServerType;
		sHackCheckRatio.m_ui8ServerIdx = (UINT8)SET_SERVER_IDX(i32Type);
		sHackCheckRatio.m_ui8HackCheckType = i32HackCheckType;
		sHackCheckRatio.m_ui8ChangedCheckRatio = ui8ChangedRatio;

		// Send Data
		CPacket SendPacket(PROTOCOL_ASC_CHANGE_HACKCHECKRATIO_NTF);
		SendPacket.WriteData(&sHackCheckRatio, sizeof(ChangedHackCheckRatio));
		m_pControlSocket->SendPacketMessage(&SendPacket);

		// save changed hack check ratio
		switch (eServerType)
		{
		case ASC_SERVER_GAME:
		{
			m_pGameHackCheckRatio[sHackCheckRatio.m_ui8ServerIdx].m_aui8HackCheckRatio[i32HackCheckType] = ui8ChangedRatio;
		}
		break;
		case ASC_SERVER_BATTLE:
		{
			// save changed hack check ratio
			m_pBattleHackCheckRatio[sHackCheckRatio.m_ui8ServerIdx].m_aui8HackCheckRatio[i32HackCheckType] = ui8ChangedRatio;
		}
		break;
		}

		// Update Hack Check ratio UI
		UpdateHackCheckRatioUI(eServerType, sHackCheckRatio.m_ui8ServerIdx);
	}
}

void CControlToolView::OnPopHackChange( UINT i32Param )
{
	INT32 i32HackType	= (i32Param >> 4) & 0x00FF;
	INT32 i32RaitIdx	= (i32Param & 0x000F) * 10;

	TRACE( "%d, %d\n", i32HackType, i32RaitIdx );

	SendChangedHackRatio( (DS_HACK_TYPE)i32HackType, (UINT8)i32RaitIdx );
}

void CControlToolView::OnDllcheat(UINT i32Param)
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	SendChangedHackRatio(GS_HACK_TYPE_DLL_CHEAT, i32Param - ID_DLLCHEAT_0, ASC_SERVER_GAME);
}

void CControlToolView::OnDisableAllHackCheck()
{
	/*
	모든 핵 체크를 해제 합니다.
	현재 사용중인 핵 체크는 20개 임으로 상수 20으로 셋팅.

	HACK_TYPE_MAX 는 HACK_TYPE_WEAPON_NOT_FOUND, HACK_TYPE_UDP_VERSION를 포함하고 있음으로 사용안함.
	0번은 HACK_TYPE_NO 임으로 처리 안함.
	*/

	for(INT32 HackTypeNum = HACK_TYPE_GRAVITY; HackTypeNum < HACK_TYPE_MAX ; HackTypeNum++)
	{
		SendChangedHackRatio( (DS_HACK_TYPE)HackTypeNum, 0 );
	}
}

BOOL CControlToolView::SendUpdatePacket(UINT32 ui32Port, char *strServiceName)
{
	if(ui32Port <= 0 || strServiceName == NULL)
		return FALSE;

	return TRUE;
}


BOOL CControlToolView::CheckSocketState()
{
	//종료
	if(m_bStopApp == TRUE)
	{
		m_bStopApp = FALSE;
		return FALSE;
	}

	//컨트롤 서버 패킷 체크
	INT32 i32ControlSVRState = 0;
	i32ControlSVRState = PacketParsing();
	
	switch(i32ControlSVRState)
	{
	case WRONG_PACKET:
		{
			OutputAddString( 0, _T("Received Packet is Wrong") );
		}
		break;

	case DISCONNECT:
		{
			MessageBox( _T("ControlServer is down\nStart Control Server first."), _T("Error"), MB_OK);
			SetControlServerState(SERVER_STATUS_OFF);

			m_pControlSocket->OnDestroy();
			m_pControlSocket = NULL;

			Initialization();	//임시
		}
		break;

	default:
		break;

		::Sleep( 1 );
	}
	
	INT32 i32SCMState = 0;
	i32SCMState = PacketParsing_SCM();
	
	switch(i32SCMState)
	{
	case DISCONNECT :
		{
			MessageBox( _T("Service Control Manager is down. \nCheck the Service Control Manager first.\nTo exit the program"), _T("Error"), MB_OK);
			ConnectSCM();
		}
		break;
	
	default:
		break;

		::Sleep( 1 );
	}
	
	return TRUE;
}

void CControlToolView::GetIP( UINT32 ui32IP, TCHAR* wstrIP )
{
	INT32 i32Count = 0;
	INT32 i32Count2;
	UINT32 ui32Temp;
	char strTemp[ 4 ];

	for( INT32 i = 0 ; i < 4 ; i++ )
	{
		ui32Temp = (ui32IP >> (8 * i)) & 0x000000FF;

		i32Count2 = 0;
		do
		{
			strTemp[ i32Count2 ] = (ui32Temp % 10) + L'0';
			ui32Temp /= 10;
			i32Count2++;
		} while( 0 < ui32Temp );
		
		do
		{
			i32Count2--;
			wstrIP[ i32Count ] = strTemp[ i32Count2 ];
			i32Count++;
		} while( 0 < i32Count2 );
		wstrIP[ i32Count ] = '.';
		i32Count++;
	}
	wstrIP[ i32Count-1 ] = L'\0';
}