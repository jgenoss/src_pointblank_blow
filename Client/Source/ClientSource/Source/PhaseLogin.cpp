#include "pch.h"
#include "PhaseLogin.h"
#include "StageReady.h"
#include "StageDef.h"
#include "GlobalVariables.h"
#include "Messenger.h"

I3_CLASS_INSTANCE(CPhaseLogin, CPhaseBase);

CPhaseLogin::CPhaseLogin()
{
	m_pStaticBg = NULL;
	m_pStaticCarret = NULL;
	m_pButtonJoin = NULL;
	m_pButtonLogin = NULL;
	m_pEditBoxId = NULL;
	m_pEditBoxPw = NULL;
	m_pCheckBox = NULL;

	m_rConnectWaitTimeStamp = 0.0f;
	m_bConnectStart = FALSE;

	m_pmbLoginWait = NULL;
}

CPhaseLogin::~CPhaseLogin()
{
	if (m_pmbLoginWait)
	{
		g_pFramework->CloseMessageBox(m_pmbLoginWait);
	}
}

void CPhaseLogin::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
	case GCI_LGP_B_JOIN:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED) OnJoinButtonClick();
		break;
	case GCI_LGP_B_LOGIN:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED) OnLoginButtonClick();
		break;
	}
}

void CPhaseLogin::OnGameEvent(INT32 event,INT32 arg)
{
	switch(event)
	{
	case EVENT_CONNECT_SERVER:
		if( EV_SUCCESSED(arg))
		{
			g_pFramework->SaveServerIdx(m_idxPhysicalServer);
			
			if (g_pGameContext->isWebLogin())
			{
				g_pGameContext->SetEvent(EVENT_LOG_IN_WEBKEY);
			}
			else
			{
				g_pGameContext->SetEvent(EVENT_LOG_IN);
			}
		}
		else
		{
			_ResetWaitConnect();
			if( EV_IS_ERROR(arg, EVENT_ERROR_FAIL)) EhFailConnect();
			else EhStageError(event,arg);
		}
		break;		
	case EVENT_LOG_IN:
		if( EV_SUCCESSED(arg))
		{
			g_pGameContext->SetEvent(EVENT_GET_CHANNELLIST);
		}
		else
		{
			_ResetWaitConnect();

			if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_LOG_IN_ALEADY_LOGIN))			EhAlreadyLogin();
			else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_LOG_IN_INVALID_ACCOUNT))	EhInvalidAccount();
			else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_LOG_IN_TIME_OUT1))			EhTimeOut1();				
			else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_LOG_IN_TIME_OUT2))			EhTimeOut2();
			else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_LOG_IN_BLOCK_ACCOUNT))		EhBlockAccount();
			else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_LOG_IN_UNKNOWN))			EhUnknownLogin();
			else if((arg & EVENT_ERROR_EVENT_LOG_IN_BLOCK_GAME) == EVENT_ERROR_EVENT_LOG_IN_BLOCK_GAME)		EhBlockGame(arg);
			else if((arg & EVENT_ERROR_EVENT_LOG_IN_BLOCK_INNER) == EVENT_ERROR_EVENT_LOG_IN_BLOCK_INNER)	EhBlockInner(arg);
			else if((arg & EVENT_ERROR_EVENT_LOG_IN_BLOCK_OUTER) == EVENT_ERROR_EVENT_LOG_IN_BLOCK_OUTER)	EhBlockOuter(arg);
			else EhStageError(event,arg);

			g_pGameContext->SetEvent(EVENT_DISCONNECT);
		}
		break;
	case EVENT_GET_CHANNELLIST:			
		if( EV_SUCCESSED(arg))
		{
			g_pFramework->GotoNextStage("Server");
			_ResetWaitConnect();

			// 메신저 연결
			g_pMessenger->Login(g_pGameContext->m_DBIdx);
		}
		else
		{	
			_ResetWaitConnect();

			if( EV_IS_ERROR(arg, EVENT_ERROR_FAIL)) EhFailChannel();
			else EhStageError(event,arg);
		}
		break;
	case EVENT_LOG_OUT:
		_ResetWaitConnect();

		// 성공, 실패 공통
		EhWaitDisconnect();

		if( EV_FAILED(arg))
		{
			EhStageError(event,arg);
		}
		break;
	case EVENT_DRIVE_AWAY_M:
		_ResetWaitConnect();
		CPhaseBase::DriveAway(arg);
		break;
	case EVENT_ERROR_M:
		_ResetWaitConnect();
		// ! praptor - 에러 종료 파악 할 것
		SYSTEM_POPUP_C(STBL_IDX_EP_GAME_EXIT_ASSERT_E, CbExitNormal);
		break;
	case EVENT_DISCONNECTION_M:
		_ResetWaitConnect();
		SYSTEM_POPUP_C(STBL_IDX_EP_GAME_EXIT_ASSERT_E, CbExitNormal);			
		break;
	case EVENT_DESTROYED_NETWORK_M:	// 네트워크가 끊어졌다
		_ResetWaitConnect();
		SYSTEM_POPUP_C(STBL_IDX_EP_GAME_DESTROYED_NETWORK_E, CbExitNormal);
		break;
	default:
		{
			_ResetWaitConnect();
			g_pGameContext->SetEvent(EVENT_DISCONNECT);

			//I3ASSERT_0;	// 처리못한 EVENT가 존재합니다. nEvent값을 알려주세요 - praptor
			char bug[256];
			sprintf(bug, "디버그 팝업: EVENT_ERROR(0x%x)\n담당자: praptor", event);
			g_pFramework->MessageBox(bug, MBF_GAME_OK);
		}			
		break;
	}
}

BOOL CPhaseLogin::OnLinkControl(i3GuiControl* pControl)
{
	LINK_STATIC_EX("Login_Bg", m_pStaticBg, -1);
	LINK_STATIC_EX("Carret", m_pStaticCarret, -1);
	
	LINK_BUTTON_EX("Join_Button", m_pButtonJoin, GCI_LGP_B_JOIN);
	LINK_BUTTON_EX("Login_Button", m_pButtonLogin, GCI_LGP_B_LOGIN);
	
	LINK_EDIT_EX("Login_ID", m_pEditBoxId, -1);
	LINK_EDIT_EX("Login_PW", m_pEditBoxPw, -1);

	LINK_CHECKBOX_EX("Login_CheckBox", m_pCheckBox, -1);

	return FALSE;
}

void CPhaseLogin::OnInitControl(void)
{
	INIT_BUTTON(m_pButtonJoin);
	INIT_BUTTON(m_pButtonLogin);

	// Bold Font
	i3Font* pFont = g_pFramework->getFont( FONT_KOREAN_COMMON);

	// Login EditBox
	CREATE_IME_ELEMENT ime;
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = 16;	// NC 웹 기준 4~16
	m_pEditBoxId->ReCreateText( pFont, FONT_COUNT_32, GAME_SYSFONT_DEFAULT_SIZE, TRUE, &ime);
	m_pEditBoxId->SetIMEMode( TRUE, IME_MODE_ALPHANUMERIC);

	ime._nTextLimitCount = 16;	// NC 웹 기준 6~16
	m_pEditBoxPw->ReCreateText( pFont, FONT_COUNT_32, GAME_SYSFONT_DEFAULT_SIZE, TRUE, &ime);
	m_pEditBoxPw->SetIMEMode( TRUE, IME_MODE_ALPHANUMERIC);
	m_pEditBoxPw->setPasswardEnable( TRUE);
}

void CPhaseLogin::OnUpdate(REAL32 rDeltaSeconds)
{
	CPhaseBase::OnUpdate( rDeltaSeconds);

	if (g_pGameContext->isWebLogin())
	{
		static BOOL bFirst = TRUE;

		if (bFirst && i3Framework::STATE_SCENE == g_pFramework->GetState())
		{
			g_pConfig->m_bNetworking = TRUE;
			m_bConnectStart = TRUE;

			g_pGameContext->SetEvent(EVENT_CONNECT_SERVER);	// 자동 접속/로그인

			bFirst = FALSE;	// 1회만 실행되도록
		}
	}

	_WaitConnectMessageBox(rDeltaSeconds);

	// Button 처리	
	if( GUI_STROKE(GAME_KEY_CMD_CHAT) )
	{
		OnLoginButtonClick();
	}
	else if( (g_pFramework->getSysKeyStroke() & GAME_KEY_TAB) == GAME_KEY_TAB )
	{	
		if( m_pEditBoxId->getIsFocus())
		{
			m_pEditBoxPw->SetFocus( TRUE);
		}
		else if( m_pEditBoxPw->getIsFocus())
		{
			m_pEditBoxId->SetFocus( TRUE);
		}
		else
		{
			m_pEditBoxId->SetFocus( TRUE);
		}
	}
}

void CPhaseLogin::OnEntranceStart(void)
{
	// Server IP 설정
	if( !_ReadyFirstServerIP())
	{
		SYSTEM_POPUP_C(STBL_IDX_EP_GAME_NOT_FOUND_FILE_SERVER_INFO_E, CbExitNormal);
		return;
	}

	UINT8 SessionKey[NET_SESSION_KEY_SIZE];
	INT64 MacAddress;
	char strServerAddress[MAX_STRING_COUNT];
	
	if (GetEnvSessKey(SessionKey) && GetEnvMacAddr(&MacAddress) && GetEnvServerAddr(strServerAddress))	
	{
		g_pGameContext->SetSessionKey(SessionKey);
		g_pGameContext->SetMacAddress(&MacAddress);
		
		for(INT32 i = 0; i < g_pFramework->GetServerIPCount(); i++)
		{
			if (0 == i3String::Compare(strServerAddress, g_pFramework->GetServerIP(i)))
			{
				m_idxLastConnectedServer = m_idxPhysicalServer = i;
				m_bCheckedLastServer = FALSE;
				_SetServerIP();
			}
			else
			{
				strcpy( g_pConfig->m_Ip, strServerAddress);
				g_pGameContext->m_nServerIP = ::inet_addr(g_pConfig->m_Ip);

				//// 환경변수의 서버IP가 등록되어있지 않다.
				//SYSTEM_POPUP_C(STBL_IDX_EP_GAME_NOT_FOUND_FILE_SERVER_INFO_E, CbExitNormal);
				//return;
			}
		}
		
		g_pGameContext->setWebLogin(true);
		g_pGUIRoot->SetEnable(FALSE);
	}
	else
	{
		// ID 로드
		LoadID( "Script/etc/Log.log");

	#if defined (I3_DEBUG)
		if( i3String::Length( m_pEditBoxId->GetText()) > 0)
		{
			m_pEditBoxPw->SetIMEText( "aaaa1111");
		}
	#endif

		m_pStaticCarret->SetEnable(FALSE);
	}
}

BOOL CPhaseLogin::SaveID( char* pszPath)
{
	if( pszPath == NULL)	return FALSE;

	CFileFind finder;
	i3FileStream FileStream;
	char	szTemp[MAX_STRING_COUNT] = {0,};
	char	szSaveDirectory[MAX_STRING_COUNT] = {0,};

	// 로그 삭제
	GetCurrentDirectory( MAX_STRING_COUNT, szTemp);

	// 만약 세이브할 디렉토리가 없다면 디렉토리를 생성합니다.
	i3String::Copy( szSaveDirectory, szTemp);
	i3String::Concat(szSaveDirectory, "\\Script\\Etc\\*.*");	
	BOOL bWorking = finder.FindFile(szSaveDirectory);	
	if( !bWorking)
	{
		i3mem::FillZero( szSaveDirectory, sizeof(char)*MAX_STRING_COUNT);
		i3String::Copy( szSaveDirectory, szTemp);
		i3String::Concat(szSaveDirectory, "\\Script\\Etc");	

		CreateDirectory( szSaveDirectory, NULL);
	}

	if( !FileStream.Open( pszPath, STREAM_WRITE))
	{		
		if( !FileStream.Create( pszPath, STREAM_WRITE))
		{
			return FALSE;
		}
	}

	if( m_pCheckBox->getCheckBoxState())
	{
		char szID[MAX_STRING_COUNT] = {0,};
		i3String::Copy( szID, m_pEditBoxId->getIMETextBuf());
		FileStream.Write( szID, sizeof(char)*MAX_STRING_COUNT);
	}
	else
	{
		FileStream.Close();

		// 파일을 저장합니다.
		i3String::Concat( szTemp, "\\Script\\Etc\\Log.log");		
		DeleteFile( szTemp);
	}

	return TRUE;
}

void CPhaseLogin::LoadID(char* pszPath)
{
	if( pszPath == NULL)	return;

	char	szTemp[MAX_STRING_COUNT] = {0,};
	i3FileStream FileStream;
	if( !FileStream.Open( pszPath, STREAM_READ))
	{
		m_pEditBoxId->SetFocus( TRUE);
		g_pGUIRoot->setFocusControl( m_pEditBoxId);
		return;
	}
	
	FileStream.Read( szTemp, MAX_STRING_COUNT);

	if( i3String::Length( szTemp) < 16)
	{
		m_pEditBoxId->SetIMEText( szTemp);
		m_pCheckBox->setCheckBoxState( TRUE);
		m_pEditBoxPw->SetFocus( TRUE);
	}
}

void CPhaseLogin::OnJoinButtonClick(void)
{
	//	SetWindowPos( 1280, 1024);

#if defined (I3_DEBUG)
	g_pConfig->m_bNetworking = FALSE;

	g_pFramework->SetWeaponListDummy();
	USER_INFO_EQUIPMENT_WEAPON* pInfo = g_pGameContext->GetMyUserInfoEquipmentWeapon();
	
	pInfo->_noprim = MAKE_WEAPON_FLAG(WEAPON_USAGE_PRIMARY, g_DebugWeaponSet[0][0], g_DebugWeaponSet[0][1]);
	pInfo->_nosub = MAKE_WEAPON_FLAG(WEAPON_USAGE_SECONDARY, g_DebugWeaponSet[1][0], g_DebugWeaponSet[1][1]);
	pInfo->_nomelee = MAKE_WEAPON_FLAG(WEAPON_USAGE_MELEE, g_DebugWeaponSet[2][0], g_DebugWeaponSet[2][1]);
	pInfo->_nothrow = MAKE_WEAPON_FLAG(WEAPON_USAGE_THROWING, g_DebugWeaponSet[3][0], g_DebugWeaponSet[3][1]);
	pInfo->_noitem = MAKE_WEAPON_FLAG(WEAPON_USAGE_ITEM, g_DebugWeaponSet[4][0], g_DebugWeaponSet[4][1]);

	switch( g_pGameContext->getMyRoomInfo()->_MapIdx)
	{
	case 0:
		g_pFramework->GotoNextStage( "NPort");		
		break;
	case 1:
		g_pFramework->GotoNextStage( "NStation");
		break;
	case 2:
		g_pFramework->GotoNextStage( "Construction");
		break;
	case 3:
		g_pFramework->GotoNextStage( "Library");
		break;
	case 4:
		g_pFramework->GotoNextStage( "Downtown");
		break;
	case 5:
		g_pFramework->GotoNextStage( "NPort_Test01");
		break;
	case 6:
		g_pFramework->GotoNextStage( "NPort_Test02");
		break;
	case 7:
		g_pFramework->GotoNextStage( "NPort_Test03");
		break;
	case 8:
		g_pFramework->GotoNextStage( "NPort_Test04");
		break;
	case 9:
		g_pFramework->GotoNextStage( "NPort_Test05");
		break;
	case 10:
		g_pFramework->GotoNextStage( "NPort_Test06");
		break;
	case 11:
		g_pFramework->GotoNextStage( "Downtown_Test01");
		break;
	case 12:
		g_pFramework->GotoNextStage( "Downtown_Test02");
		break;
	case 13:
		g_pFramework->GotoNextStage( "Downtown_Test03");
		break;
	case 14:
		g_pFramework->GotoNextStage( "Library_Test01");
		break;
	case 15:
		g_pFramework->GotoNextStage( "Library_Test02");
		break;
	case 16:
		g_pFramework->GotoNextStage( "Library_Test03");
		break;
	case 17:
		g_pFramework->GotoNextStage( "Downtown_Show");
	}
#endif
}

void CPhaseLogin::OnLoginButtonClick(void)
{
	if( TRUE == m_bConnectStart )
	{
		return;
	}

	if( i3String::Length( m_pEditBoxId->getIMETextBuf()) <= 0)
	{			
		EhNonID();
		return;
	}

	g_pConfig->m_bNetworking = TRUE;

	// ID
	i3String::Copy(g_pGameContext->getUserID(), m_pEditBoxId->getIMETextBuf());

	// Password
	i3String::Copy(g_pGameContext->getUserPW(), m_pEditBoxPw->getIMETextBuf());

	// ID저장
	if( SaveID( "Script/etc/Log.log"))
	{
		_SetServerIP();

		g_pGameContext->SetEvent( EVENT_CONNECT_SERVER);
		m_bConnectStart = TRUE;
	}
	else
	{
		ERROR_POPUP_C(STBL_IDX_EP_LOGIN_FAIL_SAVE_ID_C,CbFailSaveID);
		// 저장 실패 메세지 띄움
		// 확인 누르면 컨넥트 서버 이벤트 날림
	}

}

void CPhaseLogin::EhNonID(void)
{
	ERROR_POPUP( STBL_IDX_EP_LOGIN_NONE_ID);
	m_pEditBoxId->SetFocus(TRUE);
}

void CPhaseLogin::EhNonPW(void)
{
	ERROR_POPUP( STBL_IDX_EP_LOGIN_NONE_PW);
	
	m_pEditBoxPw->SetFocus(TRUE);
}

void CPhaseLogin::EhInvalidAccount(void)
{
	if (g_pGameContext->isWebLogin())
	{
		ERROR_POPUP_C(STBL_IDX_EP_LOGIN_INVALID_ACCOUNT, CbExitNormal);
		return;
	}

	ERROR_POPUP( STBL_IDX_EP_LOGIN_INVALID_ACCOUNT);

//	m_pEditBoxId->SetIMEText("");
	m_pEditBoxPw->SetIMEText( "");
	m_pEditBoxId->SetFocus(TRUE);	
}

void CPhaseLogin::EhAlreadyLogin(void)
{	
	if (g_pGameContext->isWebLogin())
	{
		ERROR_POPUP_C(STBL_IDX_EP_LOGIN_ALREADY_LOGIN_WEB_E, CbExitNormal);
		return;
	}
	
	//ERROR_POPUP_Q(STBL_IDX_EP_LOGIN_ALREADY_LOGIN_Q, CbAlreadyLogin);
	ERROR_POPUP_C(STBL_IDX_EP_LOGIN_ALREADY_LOGIN_Q, CbAlreadyLogin);

//	m_pEditBoxId->SetIMEText("");
	m_pEditBoxPw->SetIMEText( "");
	m_pEditBoxId->SetFocus(TRUE);
}

void CPhaseLogin::EhTimeOut1(void)
{
	if (g_pGameContext->isWebLogin())
	{
		ERROR_POPUP_C(STBL_IDX_EP_LOGIN_TIME_OUT_1, CbExitNormal);
		return;
	}	

	ERROR_POPUP(STBL_IDX_EP_LOGIN_TIME_OUT_1);

//	m_pEditBoxId->SetIMEText("");
	m_pEditBoxPw->SetIMEText( "");
	m_pEditBoxId->SetFocus(TRUE);
}

void CPhaseLogin::EhTimeOut2(void)
{
	if (g_pGameContext->isWebLogin())
	{
		ERROR_POPUP_C(STBL_IDX_EP_LOGIN_TIME_OUT_2, CbExitNormal);
		return;
	}	

	ERROR_POPUP(STBL_IDX_EP_LOGIN_TIME_OUT_2);

//	m_pEditBoxId->SetIMEText("");
	m_pEditBoxPw->SetIMEText( "");
	m_pEditBoxId->SetFocus(TRUE);
}

void CPhaseLogin::EhBlockAccount(void)
{
	if (g_pGameContext->isWebLogin())
	{
		ERROR_POPUP_C(STBL_IDX_EP_LOGIN_BLOCK_ACCOUNT, CbExitNormal);
		return;
	}		

	ERROR_POPUP(STBL_IDX_EP_LOGIN_BLOCK_ACCOUNT);

	// m_pEditBoxId->SetIMEText("");	
	m_pEditBoxPw->SetIMEText("");
	m_pEditBoxId->SetFocus(TRUE);
}

void CPhaseLogin::EhUnknownLogin(void)
{
#if defined I3_DEBUG

	char strBuf[MAX_STRING_COUNT];
	sprintf(strBuf, "%s (0x%x)\n", STR_TBL(STBL_IDX_EP_LOGIN_UNKNOWN), EVENT_ERROR_EVENT_LOG_IN_UNKNOWN);
	
	STRING_POPUP_C(strBuf, CbExitNormal);

#else

	ERROR_POPUP_C(STBL_IDX_EP_LOGIN_UNKNOWN, CbExitNormal);

#endif
}

void CPhaseLogin::EhWaitDisconnect(void)
{
	ERROR_POPUP( STBL_IDX_EP_LOGIN_WAIT_DISCONNECT);	
}

void CPhaseLogin::EhFailConnect(void)
{
	if( _MakeLinearIPIdx(TRUE) )
	{
		_SetServerIP();
		g_pGameContext->SetEvent( EVENT_CONNECT_SERVER);
		m_bConnectStart = TRUE;		
	}
	else
	{
		SYSTEM_POPUP_C( STBL_IDX_EP_LOGIN_FAIL_CONNECT_E, CbExitNormal);
		m_pEditBoxId->SetFocus(TRUE);
	}
}

void CPhaseLogin::EhFailChannel(void)
{
	if( _MakeLinearIPIdx(TRUE) )
	{
		_SetServerIP();
		g_pGameContext->SetEvent( EVENT_CONNECT_SERVER);
		m_bConnectStart = TRUE;		
	}
	else
	{
		SYSTEM_POPUP_C( STBL_IDX_EP_SERVER_FAIL_REFRESH_C, CbExitNormal);
		m_pEditBoxId->SetFocus(TRUE);
	}
}

void CPhaseLogin::EhStageError(INT32 Event,INT32 Arg)
{
	//I3ASSERT_0;	// ! 현재로선 들어올 Error가 없습니다. 들어오면 Arg를 알려주세요. - praptor
	char bug[256];
	sprintf(bug, "디버그 팝업: Event(%d), Arg(0x%x)\n담당자: praptor", Event, Arg);
	g_pFramework->MessageBox(bug, MBF_GAME_OK);
}

void CPhaseLogin::EhBlockGame(INT32 Arg)
{
	Arg = Arg & EVENT_ERROR_EVENT_LOG_IN_BLOCK_MASK;
	
	// 정보가 없다면 일반 블럭 메세지 출력
	if (0 == Arg)
	{
		EhBlockAccount();
		return;
	}
	
	// 에러 메세지 출력 우선순위는 하위 비트 순
	for(UINT32 i = STBL_IDX_BLOCK_GAME_0, nBit = 0x00000001; i <= STBL_IDX_BLOCK_GAME_13; i++, nBit = nBit << 1)
	{
		if (Arg & nBit)
		{
			ERROR_POPUP_FONT((STRING_TABLE_IDX)i, 8);
			return;
		}		
	}	
}

void CPhaseLogin::EhBlockInner(INT32 Arg)
{
	Arg = Arg & EVENT_ERROR_EVENT_LOG_IN_BLOCK_MASK;
	
	// 정보가 없다면 일반 블럭 메세지 출력
	if (0 == Arg)
	{
		EhBlockAccount();
		return;
	}
	
	// 에러 메세지 출력 우선순위는 하위 비트 순
	for(UINT32 i = STBL_IDX_BLOCK_INNER_0, nBit = 0x00000001; i <= STBL_IDX_BLOCK_INNER_4; i++, nBit = nBit << 1)
	{
		if (Arg & nBit)
		{
			ERROR_POPUP_FONT((STRING_TABLE_IDX)i, 8);
			return;
		}		
	}	
}

void CPhaseLogin::EhBlockOuter(INT32 Arg)
{
	Arg = Arg & EVENT_ERROR_EVENT_LOG_IN_BLOCK_MASK;
	
	// 정보가 없다면 일반 블럭 메세지 출력
	if (0 == Arg)
	{
		EhBlockAccount();
		return;
	}
	
	// 에러 메세지 출력 우선순위는 하위 비트 순
	for(UINT32 i = STBL_IDX_BLOCK_OUTER_0, nBit = 0x00000001; i <= STBL_IDX_BLOCK_OUTER_5; i++, nBit = nBit << 1)
	{
		if (Arg & nBit)
		{
			ERROR_POPUP_FONT((STRING_TABLE_IDX)i, 8);
			return;
		}		
	}	
}

void CPhaseLogin::_WaitConnectMessageBox(REAL32 rDeltaSeconds)
{
	if( !m_bConnectStart) return;

	if( m_rConnectWaitTimeStamp > 1.0f)	// 1초
	{	
		if( !m_pmbLoginWait )
		{
			m_pmbLoginWait = ERROR_POPUP_W(STBL_IDX_EP_LOGIN_WAIT_CONNECT_W);
		}
	}
	else
	{
		m_rConnectWaitTimeStamp += rDeltaSeconds;
	}	
}

void CPhaseLogin::_ResetWaitConnect(void)
{
	m_bConnectStart = FALSE;
	m_rConnectWaitTimeStamp = 0.0f;

	if( m_pmbLoginWait)
	{
		g_pFramework->CloseMessageBox(m_pmbLoginWait);
		m_pmbLoginWait = NULL;
	}
}

void CPhaseLogin::CbFailSaveID(void* pThis,INT32 nParam)
{
	g_pGameContext->SetEvent( EVENT_CONNECT_SERVER);
	((CPhaseLogin*)pThis)->m_bConnectStart = TRUE;
}

void CPhaseLogin::CbAlreadyLogin(void* pThis,INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		//g_pGameContext->SetEvent( EVENT_LOG_OUT);
		((CPhaseLogin*)(g_pFramework->GetCurrentStage()))->EhWaitDisconnect(); // EVENT_LOG_OUT 활성화 전까지 임시
	}
}

BOOL CPhaseLogin::GetEnvSessKey(UINT8* pSessKey)
{
	I3ASSERT(pSessKey);

	char strBuf[MAX_STRING_COUNT];

	if (0 == ::GetEnvironmentVariable("SessKey", strBuf, 256))
	{
		return FALSE;
	}

	char* desc;
	desc = (char*)pSessKey;
	*pSessKey = 0;
	
	char* ptr;
	ptr = strBuf;

	// 문자열 bytes값을 수치로 복원한다
	for(INT32 i = 0; i < NET_SESSION_KEY_SIZE; i++)
	{
		char chByte;
		chByte = HexToInt(*ptr) * 16 + HexToInt(*(ptr + 1));
		ptr += 2;
	
		*desc = chByte;
		desc++;
	}

	return TRUE;
}

BOOL CPhaseLogin::GetEnvMacAddr(INT64* pMacAddr)
{
	I3ASSERT(pMacAddr);

	char strBuf[MAX_STRING_COUNT];

	if (0 == ::GetEnvironmentVariable("MacAddr", strBuf, 256))
	{
		return FALSE;		
	}
	
	char* desc;
	desc = (char*)pMacAddr;
	*pMacAddr = 0;
	
	char* ptr;
	ptr = strBuf;

	// 문자열 bytes값을 수치로 복원한다
	for(INT32 i = 0; i < 6; i++)
	{
		char chByte;
		chByte = HexToInt(*ptr) * 16 + HexToInt(*(ptr + 1));
		ptr += 2;
	
		*desc = chByte;
		desc++;
	}
	
	return TRUE;
}

BOOL CPhaseLogin::GetEnvServerAddr(char* strServerAddr)
{
	char strBuf[MAX_STRING_COUNT];

	if (0 == ::GetEnvironmentVariable("ServerAddr", strBuf, 256))
	{
		return FALSE;
	}

	i3String::Copy(strServerAddr, strBuf);

	return TRUE;
}

INT8 CPhaseLogin::HexToInt(char chHex)
{
	if (chHex >= '0' && chHex <= '9')
	{
		return chHex - '0';
	}
	else if (chHex >= 'a' && chHex <= 'f')
	{
		return chHex - 'a' + 10;
	}
	else if (chHex >= 'A' && chHex <= 'F')
	{
		return chHex - 'A' + 10;
	}

	return 0;
}
