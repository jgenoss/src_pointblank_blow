#include "pch.h"
#include "Game.h"
#include "StageLogin.h"
#include "shellapi.h"
#include "GlobalVariables.h"
#include "ServerInfo.h"
#include "SecurityHeader.h"

#if defined (LOCALE_CHINA)
#include "../SDOALink/SDOALink.h"
#include "CHttpFCMInfo.h"
#endif


static bool CheckIsAlphaNumericStr(const char* str)
{
	if(!str)
		return false;

	while(*str)
	{
		if(!isalnum(*str))
			return false;

		str++;
	}

	return true;
}


BOOL IsWindowsXP()
{
   OSVERSIONINFOEX osvi;
   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   BOOL bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi);
   if( ! bOsVersionInfoEx )
   {
       osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

       if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) )
           return TRUE;
   }

   if ( osvi.dwMajorVersion < 6 )
   {
       return TRUE;
   }
   else
   {
       return FALSE;
   }
}

#if defined(LOCALE_CHINA)

BOOL GetHostIP(char *addr, unsigned int maxlen)
{  
	char szHostName[50];
	char szIp[16];
	IN_ADDR in;
	PHOSTENT phostent;
	if (maxlen <= 0)
	{  
		return FALSE;
	}
	memset (szHostName, 0, sizeof(szHostName));
	memset (szIp, 0, sizeof(szIp));
	if (gethostname(szHostName, 50) != 0)
	{
		return FALSE;
	}
	else
	{
		if ((phostent = gethostbyname(szHostName)) == NULL)
		{
			return FALSE;
		}
		else
		{
			memcpy (&in, phostent->h_addr, 4);
			wsprintf (szIp, "%s", inet_ntoa(in));
			i3String::Copy (addr, szIp, maxlen);  
			return TRUE;  
		}
	}
	return TRUE;
}
#endif

typedef UINT (CALLBACK* LPFNDLLFUNC) (GUID& rfid, DWORD dwFlags, HANDLE hToken, PWSTR *ppszPath);
CString GetWritableFolder(void)
{ 
	CString strPath = _T("");
	TCHAR szPath[ MAX_PATH] = {0,};

	if( !IsWindowsXP() )
	{ 
		PWSTR       path = NULL;
		HINSTANCE   hInst;
		LPFNDLLFUNC pGetKnownFldPathFnPtr;

		// Vista 이상에서 사용자/AppData/Local 폴더 경로 {F1B32785-6FBA-4FCF-9D55-7B8E7F157091}, CSIDL_LOCAL_APPDATA 
		GUID folderid_localappdata = {0xF1B32785, 0x6FBA, 0x4FCF, {0x9D, 0x55, 0x7B, 0x8E, 0x7F, 0x15, 0x70, 0x91}};

		hInst = LoadLibrary(_T("shell32.dll"));

		pGetKnownFldPathFnPtr = (LPFNDLLFUNC)GetProcAddress(hInst, "SHGetKnownFolderPath");

		if ( pGetKnownFldPathFnPtr && SUCCEEDED( pGetKnownFldPathFnPtr( folderid_localappdata, 0, NULL, &path) ) )
		{

#ifndef _UNICODE
			WideCharToMultiByte( CP_ACP, 0, path, wcslen(path), szPath, MAX_PATH, NULL, NULL );
			strPath = szPath;
#else
			strPath = path;

#endif
			CoTaskMemFree( path );
		}

		FreeLibrary( hInst );
	}
	else
	{
		SHGetSpecialFolderPath( NULL, szPath, CSIDL_APPDATA, TRUE );
		strPath = szPath;
	}

	return strPath;
}

I3_CLASS_INSTANCE( CStageLogin, CStageGUI);

CStageLogin::CStageLogin(void)
{
	m_pStaticBG			= NULL;
	m_pStaticCarret		= NULL;
	m_pStaticBGAccount	= NULL;
	m_pEditBoxID		= NULL;
	m_pEditBoxPW		= NULL;
	m_pCheckBox			= NULL;
	m_pButtonJoin		= NULL;
	m_pButtonJoinText	= NULL;
	m_pButtonLogin		= NULL;
	m_pButtonLoginText	= NULL;
	m_pFocusedControl	= NULL;

#if defined(LOCALE_CHINA)
	m_pmbRNWait			= NULL;
#endif

	m_rConnectWaitTimeStamp	= 0.0f;
	m_bConnectStart			= FALSE;

	m_pmbLoginWait		= NULL;
	m_LoginError		= FALSE;

	m_LogInConnectFailedCount	= 0;
	m_GameStartTime				= 0.0f;
	m_ForceExit					= FALSE;

#if defined(LOCALE_CHINA)
	// 2011-03-07 하동익
	//실명제 인증을 위한 새로운 UI변수 추가
	m_pStaticRNBG			= NULL;
	m_pStaticSRNName			= NULL;
	m_pStaticSRNSnum	= NULL;
	m_pStaticSRNEmail		= NULL;
	m_pStaticRNName		= NULL;
	m_pStaticRNSnum		= NULL;
	m_pStaticRNEmail		= NULL;
	m_pStaticRNNotice		= NULL;
	m_pButtonRNConfirm		= NULL;

	m_pUserLicenseWindow	= NULL;
	m_pUserLicenseScrollFrame = NULL;
	m_pUserLicenseList		= NULL;
	m_pUserLicenseText		= NULL;
	m_pUserLicenseConfirm	= NULL;

	m_iLineCnt = 0;
	m_LoginWindowFlag = TRUE;
#if defiend(USE_SDOA_LOGIN)
	g_pSDOA_Link->SetbRanderFlag(TRUE);//중국 CBT 임시 로그인
#else
	g_pSDOA_Link->SetbRanderFlag(FALSE);//중국 CBT 임시 로그인
#endif

#endif

	m_RNFlag = FALSE;
}

CStageLogin::~CStageLogin(void)
{
	if( m_pmbLoginWait )
	{
		g_pFramework->CloseMessageBox(m_pmbLoginWait);
	}
}

void CStageLogin::OnCreate()
{
	CStageGUI::OnCreate();

	//============================================================
	// Test Code로써, 가장 처음의 Window는 항상 Update되게 합니다.
	((i3GuiWindow*)(m_pGUIRoot->getFirstChild()))->setState(I3GUI_WINDOW_STATE_ALLWAYS_FOCUS);

	//=================================
	// Control객체를 변수와 연결합니다.
	LinkControlByName();

	//=================================
	// Control객체를 초기화합니다.
	Init();

	m_LogInConnectFailedCount	= 0;
	m_GameStartTime				= g_pGameContext->GetAccTime();
	I3TRACE1("Game Start Time : %f\n", m_GameStartTime);
}

void CStageLogin::SetAnimation()
{
	//m_pButtonLogin->
}

BOOL CStageLogin::OnQueryLoad( INT32 numStage)
{
	LPCTSTR pFileName = "Gui/Project/Login.gui";
#if defined(LOCALE_TURKEY)
	pFileName = "Gui/Project/Login_turkey.gui";
#endif
#if defined(LOCALE_CHINA)
	pFileName = "Gui/Project/Login_China.gui";
#endif

	i3String::Copy( m_szUIFileName, pFileName, MAX_PATH );

	return CStageGUI::OnQueryLoad( numStage);
}

void CStageLogin::LinkControlByName()
{
	i3GuiWindow* pWindow = ((i3GuiWindow*)(m_pGUIRoot->getFirstChild()));
	i3List	ControlBackUpList;

	if(pWindow)
	{
		i3GuiControl * pControl = (i3GuiControl*)pWindow->getFirstChild();
		/* 하동익 2011-03-07 실명제 UI에 Control 링크
		*/
		while(pControl)
		{
			//============================================
			// 이곳에 Control을 링크합니다.
			if(pControl->hasName())
			{
				// static
				if( i3String::Compare( pControl->GetName(), "Login_BG") == 0)
				{
					m_pStaticBG = (i3GuiStatic*)pControl;
				}
				else if( i3String::Compare( pControl->GetName(), "Login_InputBG") == 0)
				{
					m_pStaticBox = (i3GuiStatic*)pControl;
				}
				else if( i3String::Compare( pControl->GetName(), "Login_InputBG_Account") == 0)
				{
					m_pStaticBGAccount = (i3GuiStatic*)pControl;
				}
				else if( i3String::Compare( pControl->GetName(), "Carret") == 0)
				{
					m_pStaticCarret = (i3GuiStatic*)pControl;
				}
				// Button
				else if( i3String::Compare( pControl->GetName(), "Join_Button") == 0)
				{
					m_pButtonJoin = (i3GuiButton*)pControl;
					m_pButtonJoin->SetControlID(GUI_ID_LOGIN_BUTTON_JOIN);
				}
				else if( i3String::Compare( pControl->GetName(), "Login_Button") == 0)
				{
					m_pButtonLogin = (i3GuiButton*)pControl;
					m_pButtonLogin->SetControlID(GUI_ID_LOGIN_BUTTON_LOGIN);
				}
				// Edit Box
				else if( i3String::Compare( pControl->GetName(), "Login_ID") == 0)
				{
					m_pEditBoxID = (i3GuiEditBox*)pControl;
				}
				else if( i3String::Compare( pControl->GetName(), "Login_PW") == 0)
				{
					m_pEditBoxPW = (i3GuiEditBox*)pControl;
				}
				else if( i3String::Compare( pControl->GetName(), "Join_Button_Text") == 0)
				{
					m_pButtonJoinText = (i3GuiEditBox*)pControl;
				}
				else if( i3String::Compare( pControl->GetName(), "Login_Button_Text") == 0)
				{
					m_pButtonLoginText = (i3GuiEditBox*)pControl;
				}
				// Check Box
				else if( i3String::Compare( pControl->GetName(), "Login_CheckBox") == 0)
				{
					m_pCheckBox = (i3GuiCheckBox*)pControl;
				}

#if defined(LOCALE_CHINA)
				//하동익 2011-03-07 새로운 UI 링크 실명제용 UI
				else if( i3String::Compare( pControl->GetName(), "window") == 0)
				{
					m_pStaticRNBG = (i3GuiStatic*)pControl;
				}
				else if( i3String::Compare( pControl->GetName(), "static_realname_name") == 0)
				{
					m_pStaticSRNName = (i3GuiEditBox*)pControl;
				}
				else if( i3String::Compare( pControl->GetName(), "static_realname_number") == 0)
				{
					m_pStaticSRNSnum = (i3GuiEditBox*)pControl;
				}
				else if( i3String::Compare( pControl->GetName(), "static_realname_email") == 0)
				{
					m_pStaticSRNEmail = (i3GuiEditBox*)pControl;
				}
				else if( i3String::Compare( pControl->GetName(), "input_realname_name") == 0)
				{
					m_pStaticRNName = (i3GuiEditBox*)pControl;
				}
				else if( i3String::Compare( pControl->GetName(), "input_realname_number") == 0)
				{
					m_pStaticRNSnum = (i3GuiEditBox*)pControl;
				}
				else if( i3String::Compare( pControl->GetName(), "input_realname_email") == 0)
				{
					m_pStaticRNEmail = (i3GuiEditBox*)pControl;
				}
				else if( i3String::Compare( pControl->GetName(), "notice") == 0)
				{
					m_pStaticRNNotice = (i3GuiEditBox*)pControl;
				}
				else if( i3String::Compare( pControl->GetName(), "okok") == 0)
				{
					m_pButtonRNConfirm = (i3GuiButton*)pControl;
					m_pButtonRNConfirm->SetControlID(GUI_ID_REALNAME_BUTTON_CONFIRM);
				}
				else if( i3String::Compare( pControl->GetName(), "UserLicense") == 0)
				{
					m_pUserLicenseWindow = (i3GuiStatic*)pControl;
				}
				else if( i3String::Compare( pControl->GetName(), "LicenseScrollFrame") == 0)
				{
					m_pUserLicenseScrollFrame = (i3GuiStatic*)pControl;
				}
				else if( i3String::Compare( pControl->GetName(), "LicenseScrollList") == 0)
				{
					m_pUserLicenseList = (i3GuiList*)pControl;
					m_pUserLicenseList->SetControlID(GCI_SSP_S_LICENSE_SCROLL);
				}
				else if( i3String::Compare( pControl->GetName(), "LicenseText") == 0)
				{
					m_pUserLicenseText = (i3GuiEditBox*)pControl;
				}
				else if( i3String::Compare( pControl->GetName(), "LicenseConfirm") == 0)
				{
					m_pUserLicenseConfirm = (i3GuiButton*)pControl;
					m_pUserLicenseConfirm->SetControlID(GUI_ID_REALNAME_LICENSE_CONFIRM);
				}
#endif
			}
			//============================================

			if( pControl->getFirstChild() != NULL)
			{
				if((i3GuiControl*)pControl->getNext() != NULL)
				{
					ControlBackUpList.Add( (i3GuiControl*)pControl->getNext());
				}
				pControl = (i3GuiControl*)pControl->getFirstChild();
			}
			else
			{
				pControl = (i3GuiControl*)pControl->getNext();
				if(pControl == NULL)
				{
					if( ControlBackUpList.GetCount() != 0)
					{
						pControl = (i3GuiControl*)ControlBackUpList.GetLast();
						ControlBackUpList.Delete(ControlBackUpList.GetCount());
					}
				}
			}
		}

	}
}

void CStageLogin::Init()
{
	// Login Button
	//하동익 2011-03-07 실명제 버튼 설정
#if defined(LOCALE_CHINA)
	m_pButtonRNConfirm->setClickAction( TRUE);
	m_pButtonRNConfirm->SetUserDefine( I3GUI_CONTROL_STATE_NORMAL, 0);
	m_pButtonRNConfirm->SetUserDefine( I3GUI_CONTROL_STATE_ONMOUSE, 1);
	m_pButtonRNConfirm->setInputMode( I3GUI_INPUT_MODE_PRESS);

	m_pUserLicenseConfirm->setClickAction( TRUE);
	m_pUserLicenseConfirm->SetUserDefine( I3GUI_CONTROL_STATE_NORMAL, 0);
	m_pUserLicenseConfirm->SetUserDefine( I3GUI_CONTROL_STATE_ONMOUSE, 1);
	m_pUserLicenseConfirm->setInputMode( I3GUI_INPUT_MODE_PRESS);

	m_pUserLicenseList->CreateTextEx(1,  GetDefaultFontName());
	m_pUserLicenseList->SetSelColor(0, 0, 0, 0);

#endif

	m_pButtonJoin->setClickAction( TRUE);
	m_pButtonJoin->SetUserDefine( I3GUI_CONTROL_STATE_NORMAL, 0);
	m_pButtonJoin->SetUserDefine( I3GUI_CONTROL_STATE_ONMOUSE, 1);
	m_pButtonJoin->setInputMode( I3GUI_INPUT_MODE_PRESS);
	m_pButtonLogin->setClickAction( TRUE);
	m_pButtonLogin->SetUserDefine( I3GUI_CONTROL_STATE_NORMAL, 0);
	m_pButtonLogin->SetUserDefine( I3GUI_CONTROL_STATE_ONMOUSE, 1);
	m_pButtonLogin->setInputMode( I3GUI_INPUT_MODE_PRESS);

#if !defined( BUILD_RELEASE_QA_VERSION)
	#if defined(LOCALE_INDONESIA)
	m_pButtonJoin->SetEnable(FALSE);
	#endif
#endif
#if defined(ENABLE_NEW_LOGIN_UI)
	{
#if defined(LOCALE_TURKEY)
		REAL32	Rate_BGMain		= 1.85f;
		REAL32	Rate_BGAccount	= 1.9f;
		REAL32	Rate_Edit		= 2.6f;
#else
		REAL32	Rate_BGMain		= 1.f;
		REAL32	Rate_BGAccount	= 1.f;
		REAL32	Rate_Edit		= 1.f;
#endif
		m_pButtonJoinText->setInputDisable(TRUE);
		m_pButtonLoginText->setInputDisable(TRUE);

		REAL32 resolutionWidth = i3GuiRoot::getGuiResolutionWidth();
		REAL32	_x			= m_pStaticBox->getPositionX();
		REAL32	_width		= m_pStaticBox->getWidth();
		REAL32	CenterPos	= (_x + _width / resolutionWidth / 2);
		REAL32	NewWidth	= (_width * Rate_BGMain);
		REAL32	NewPos		= (CenterPos - (NewWidth / resolutionWidth / 2 ));
		m_pStaticBox->setPositionX(NewPos);

		GameGUI::ResizingForm	Form;
		Form.Set(0,0,0);
		Form.Set(0,1,1);
		Form.Set(0,2,2);
		Form.Set(1,0,3);
		Form.Set(1,1,8);
		Form.Set(1,2,4);
		Form.Set(2,0,5);
		Form.Set(2,1,6);
		Form.Set(2,2,7);
		ResizingControl(m_pStaticBox, &Form, NewWidth, m_pStaticBox->getHeight());


		REAL32	EditSize = (m_pEditBoxID->getWidth() * Rate_Edit);
		ResizingControl(m_pEditBoxID, &Form, EditSize, m_pEditBoxID->getHeight());
		ResizingControl(m_pEditBoxPW, &Form, EditSize, m_pEditBoxPW->getHeight());


		Form.Reset();
		Form.Set(1,1,0);
		REAL32	BGAccountSize = (m_pStaticBGAccount->getWidth() * Rate_BGAccount);
		ResizingControl(m_pStaticBGAccount, &Form, BGAccountSize, m_pStaticBGAccount->getHeight());
	}

#endif
	// Lobby EditBox
	CREATE_IME_ELEMENT ime;
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
#if defined(LOCALE_TURKEY)
	ime._nTextLimitCount = 64;
#else
	ime._nTextLimitCount = 16;	// NC 웹 기준 4~16
#endif

	//ime._StyleIME = IME_STYLE_IGNORE_KEYINPUT_IME_MODE;
	m_pEditBoxID->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pEditBoxID->SetAlphaNumericOnly( true);
	m_pEditBoxID->SetTextAutoWrap( FALSE);
	m_pEditBoxID->SetOutCharaProcess( TRUE);

	ime._nTextLimitCount = 16;	// NC 웹 기준 6~16
	//ime._StyleIME = IME_STYLE_IGNORE_KEYINPUT_IME_MODE;
	m_pEditBoxPW->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pEditBoxPW->SetAlphaNumericOnly( true);
	m_pEditBoxPW->setPasswardEnable( TRUE);

#if defined(LOCALE_CHINA)
	CREATE_IME_ELEMENT ime2;
	ime2._hWnd = g_hWnd;
	ime2._hInst = g_hInstance;
	ime2._nTextLimitCount = 64;
	ime2._StyleIME = 0;
	m_pStaticRNName->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_DEFAULT_SIZE, TRUE, &ime2);
	m_pStaticRNName->SetTextColor(GetColor(GCT_DEFAULT));
	m_pStaticRNName->SetIMEMode( IME_MODE_NATIVE);
	m_pStaticRNName->SetTextAutoWrap( FALSE);
	m_pStaticRNName->SetOffsetPos(3.5f, 2.2f);
	m_pStaticRNName->setCaretPosOffset(2.5f, 4.0f);


	ime._nTextLimitCount = 64;
	//ime._StyleIME = IME_STYLE_IGNORE_KEYINPUT_IME_MODE;
	//ime._StyleIME = IME_STYLE_LIMIT_CHARACTER;
	//하동익 중국 GUI
	/*
	m_pStaticRNName->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pStaticRNName->SetAlphaNumericOnly( true);
	m_pStaticRNName->SetTextAutoWrap( FALSE);
	m_pStaticRNName->SetOutCharaProcess( TRUE);
	*/
	m_pStaticRNSnum->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pStaticRNSnum->SetAlphaNumericOnly( true);
	m_pStaticRNSnum->SetTextAutoWrap( FALSE);
	m_pStaticRNSnum->SetOutCharaProcess( TRUE);
	m_pStaticRNSnum->SetOffsetPos(3.5f, 2.2f);
	m_pStaticRNSnum->setCaretPosOffset(2.5f, 4.0f);

	m_pStaticRNEmail->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pStaticRNEmail->SetAlphaNumericOnly( false);
	m_pStaticRNEmail->SetTextAutoWrap( FALSE);
	m_pStaticRNEmail->SetOutCharaProcess( TRUE);
	m_pStaticRNEmail->SetOffsetPos(3.5f, 2.2f);
	m_pStaticRNEmail->setCaretPosOffset(2.5f, 4.0f);

	m_pStaticSRNName->SetText(GAME_STRING("STR_CHINA_REALNAME_NAME"));
	m_pStaticSRNSnum->SetText(GAME_STRING("STR_CHINA_REALNAME_NUM"));
	m_pStaticSRNEmail->SetText(GAME_STRING("STR_CHINA_REALNAME_EMAIL"));

	m_pStaticRNNotice->setInputDisable(TRUE);
	m_pStaticRNNotice->SetText(GAME_STRING("STR_CHINA_NOTICE"));
	
	m_pUserLicenseWindow->SetEnable(FALSE);//
	LoadUserCheck("config\\china\\License_China.txt");

	//중국 로그인 연동 대기용 Flag
	m_RNFlag = 0;
#endif

#if defined(ENABLE_NEW_LOGIN_UI)
	m_pEditBoxID->SetOffsetPos(5, 0);
	m_pEditBoxID->setCaretPosOffset(5, 3);
	m_pEditBoxID->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pEditBoxPW->SetOffsetPos(5, 0);
	m_pEditBoxPW->setCaretPosOffset(5, 3);
	m_pEditBoxPW->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
#endif

	UINT8 SessionKey[NET_SESSION_KEY_SIZE];
	INT64 MacAddress;
	char strServerAddress[MAX_STRING_COUNT];

#if defined(LOCALE_RUSSIA) && defined(NC_BUILD)
//#if defined(LOCALE_RUSSIA)
	g_pGameContext->setWebLogin(true);
	g_pGUIRoot->SetEnable(FALSE);
#endif

#if defined(LOCALE_LATIN_AMERICA) && defined(NC_BUILD)
	g_pGameContext->setWebLogin(true);
	g_pGUIRoot->SetEnable(FALSE);
#endif

#if defined(LOCALE_KOREA) && defined(NC_BUILD)
	// 한국 새로운 엔씨 런처를 통할 경우 더이상 환경변수를 사용하지 않고 오직 매개변수만 사용하도록 변경되었습니다.
	const UINT8 * pSession = g_pGameContext->GetSessionKey();
	if( *pSession != 0)		//	세션값을 런처에게 받았다면
	{
		g_pGameContext->setWebLogin(true);
		g_pGUIRoot->SetEnable(FALSE);
	}
#else
	if (GetEnvSessKey(SessionKey) && GetEnvMacAddr(&MacAddress) 
		&& GetEnvServerAddr(strServerAddress, MAX_STRING_COUNT))
	{
		g_pGameContext->SetSessionKey(SessionKey);
		g_pGameContext->SetMacAddress(&MacAddress);
		g_pGameContext->setWebLogin(true);
		g_pGUIRoot->SetEnable(FALSE);
	}
#endif
	else
	{
		char szLoadID[MAX_STRING_COUNT] = "";
		i3String::Format( szLoadID, MAX_STRING_COUNT, "\\%s\\Log.log", GAME_TITLE_NAME_A);
		// ID 로드
		LoadID( szLoadID); 

#if defined(BUILD_RELEASE_QA_VERSION)

		if( i3String::Contain( m_pEditBoxID->GetText(), "gptest") == 0)
		{
			m_pEditBoxPW->SetIMEText( m_pEditBoxID->GetText());
		}
		else if( i3String::Contain( m_pEditBoxID->GetText(), "angel") == 0)
		{
			m_pEditBoxPW->SetIMEText( "aaaa1111");
		}
		else if( i3String::Contain( m_pEditBoxID->GetText(), "gametest") == 0)
		{
			const char * pID = m_pEditBoxID->GetText();
			char szNumber[256];
			char szTemp[256];
			pID+=8;
			i3String::NCopy( szNumber, pID , i3String::Length(m_pEditBoxID->GetText()) - 8 );
			sprintf_s( szTemp, "g4m3t3st%s", szNumber);
			m_pEditBoxPW->SetIMEText( szTemp);
		}


		//m_pEditBoxID->SetIMEText("123456789-123456789-123456789-123456789-123456789-123456789-");
#endif

		m_pStaticCarret->SetEnable(FALSE);
	}

	// Battle에서 에러팝업 후 퇴장 시 처리
	if( g_pFramework->IsMessageBoxEnabled() )
	{
		g_pGUIRoot->setGuiDisableInput(TRUE);
	}

	// Server IP 설정
	g_pFramework->PrintVersion();

	m_pStaticBox->SetEnable(TRUE);
	m_pStaticCarret->SetEnable(TRUE);
	m_pEditBoxID->SetEnable(TRUE);
	m_pEditBoxPW->SetEnable(TRUE);
	m_pCheckBox->SetEnable(TRUE);
	m_pButtonJoin->SetEnable(TRUE);
	m_pButtonLogin->SetEnable(TRUE);	

#if defined (LOCALE_CHINA)
#ifdef USE_SDOA_LOGIN
	//20110407하동익 테스트
	m_pStaticBox->SetEnable(FALSE);
	m_pStaticCarret->SetEnable(FALSE);
	m_pEditBoxID->SetEnable(FALSE);
	m_pEditBoxPW->SetEnable(FALSE);
	m_pCheckBox->SetEnable(FALSE);
	m_pButtonJoin->SetEnable(FALSE);
	m_pButtonLogin->SetEnable(FALSE);

	m_pStaticRNBG->SetEnable(FALSE);
	m_pStaticSRNName->SetEnable(FALSE);
	m_pStaticSRNSnum->SetEnable(FALSE);
	m_pStaticSRNEmail->SetEnable(FALSE);
	m_pStaticRNName->SetEnable(FALSE);
	m_pStaticRNSnum->SetEnable(FALSE);
	m_pStaticRNEmail->SetEnable(FALSE);
	m_pButtonRNConfirm->SetEnable(FALSE);

	//GetSDOAVersion();
	char szTemp[256];
	char China_Version[256];
	i3FileStream FileStream;
	int t_index = 0;
	if( FALSE == FileStream.Open( "Version.dat", STREAM_READ | STREAM_SHAREREAD) )
	{
		I3NOTICE("Version.dat Reading Fail. Launcher working fail or your datafile changed\n");
		return;
	}
	I3NOTICE("Open China Version data....\n");
	while(FileStream.ReadLine(szTemp,256) != NULL)
	{
		//FileStream.ReadLine(szTemp,256);
		if(i3String::Contain(szTemp, "[Common]") != -1)
		{
			FileStream.ReadLine(szTemp,256);
			I3NOTICE("Version Reading Success.\n");
			break;
		}
	}
	for(int i = 0; i<strlen(szTemp); i++)
	{
		if(isdigit(szTemp[i]) || szTemp[i] == '.')
			China_Version[t_index++] = szTemp[i];
	}
	China_Version[t_index++] = '\0';
	I3NOTICE("SDOA Initialize Information\n");
	I3NOTICE("Version Info : %s", China_Version);
	I3NOTICE("AreaCode Info : %s", g_pConfigEx->m_AreaCode);
	g_pSDOA_Link->Create(g_hWnd, g_pD3DDevice, i3String::ToInt(g_pConfigEx->m_AreaCode), China_Version);

	CALLBACK_EXTERNAL_RENDER m_Func1 = g_pSDOA_Link->GetOnExternalRender();
	CALLBACK_SDOA_MSG_PROC m_Func2 = g_pSDOA_Link->GetMsgProc();
	CALLBACK_SDOA_LOSTDEVICE lostDeviceHandler = g_pSDOA_Link->getLostDevHandler();
	CALLBACK_SDOA_RESETDEVICE resetDeviceHandler = g_pSDOA_Link->getResetDeviceHandler();

	g_pViewer->setExternalRender(m_Func1);
	g_pFramework->setSDOAMsgProc(m_Func2);
	g_pViewer->setExternalRenderLostDeviceHandler( lostDeviceHandler);
	g_pViewer->setExternalRenderReviveHandler( resetDeviceHandler);

#else
	m_pStaticBox->SetEnable(TRUE);
	m_pStaticCarret->SetEnable(TRUE);
	m_pEditBoxID->SetEnable(TRUE);
	m_pEditBoxPW->SetEnable(TRUE);
	m_pCheckBox->SetEnable(TRUE);
	m_pButtonJoin->SetEnable(TRUE);
	m_pButtonLogin->SetEnable(TRUE);

	m_pStaticRNBG->SetEnable(FALSE);
	m_pStaticSRNName->SetEnable(FALSE);
	m_pStaticSRNSnum->SetEnable(FALSE);
	m_pStaticSRNEmail->SetEnable(FALSE);
	m_pStaticRNName->SetEnable(FALSE);
	m_pStaticRNSnum->SetEnable(FALSE);
	m_pStaticRNEmail->SetEnable(FALSE);
	m_pButtonRNConfirm->SetEnable(FALSE);

#endif
#endif

}

void CStageLogin::OnUpdate( REAL32 rDeltaSeconds)
{
	CStageGUI::OnUpdate( rDeltaSeconds);

#if defined(WINDOW_MODE_DISABLE)
	/*if (g_DidThisInactive)
	{
		GOTO_EXIT_STAGE();
	}*/
#endif

#if defined(LOCALE_CHINA) && defined(USE_SDOA_LOGIN)
	if(TRUE == g_pSDOA_Link->GetbLoginFlag())
	{
		OnLoginButtonClick();
		g_pSDOA_Link->SetbLoginFlag(FALSE);
	}
#endif

	if (g_pGameContext->IsWebLogin())
	{
		static BOOL bFirst = TRUE;

		if (bFirst && i3Framework::STATE_SCENE == g_pFramework->GetState())
		{
			g_pConfig->m_bNetworking = TRUE;
			ConnectLogInServer();

			bFirst = FALSE;	// 1회만 실행되도록
		}
	}

	_CheckGameRunTimeOut();

	//==================================
	// Game Context처리
	ProcessGameContext( rDeltaSeconds);
	_WaitConnectMessageBox(rDeltaSeconds);
	CStageLogin::SetAnimation();
}

BOOL CStageLogin::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if (I3_EVT_INPUT == event)
		{
			i3InputDeviceManager* pMgr = (i3InputDeviceManager*)pObj;
			i3InputKeyboard* pKeyboard = pMgr->GetKeyboard();
			#if !defined(LOCALE_CHINA)
			if (pKeyboard->GetStrokeState(I3I_KEY_ENTER))
			{
				OnLoginButtonClick();
				return TRUE;
			}
			#endif

			if (pKeyboard->GetStrokeState(I3I_KEY_TAB))
			{
				if( m_pEditBoxID->getIsFocus())
				{
					m_pEditBoxPW->SetFocus( TRUE);
				}
				else if( m_pEditBoxPW->getIsFocus())
				{
					m_pEditBoxID->SetFocus( TRUE);
				}
				else
				{
					m_pEditBoxID->SetFocus( TRUE);
				}

				return TRUE;
			}
		}
	}

	return CStageGUI::OnEvent(event, pObj, param2, code);
}

BOOL CStageLogin::SaveID( char* pszPath)
{
	if( pszPath == NULL)	return FALSE;

	CString appdata = GetWritableFolder();
	CFileFind finder;
	i3FileStream FileStream;
	char	szTemp[MAX_STRING_COUNT] = {0,};		// adddata directory 
	char	szFile[MAX_STRING_COUNT] = {0,};		// full file directory
	char	szSaveDirectory[MAX_STRING_COUNT] = {0,};

	// 로그 삭제
	//GetCurrentDirectory( MAX_STRING_COUNT, szTemp);
	i3String::Copy( szTemp, appdata, MAX_STRING_COUNT );
	i3String::Copy( szFile, appdata, MAX_STRING_COUNT );
	i3String::Concat( szFile, pszPath );

	// 만약 세이브할 디렉토리가 없다면 디렉토리를 생성합니다.
	//i3String::Copy( szSaveDirectory, szTemp);
	//i3String::Concat(szSaveDirectory, "\\Script\\Etc\\*.*");

	i3String::Copy( szSaveDirectory, appdata, MAX_STRING_COUNT );
	i3String::Concat(szSaveDirectory, "\\");
	i3String::Concat(szSaveDirectory, GAME_TITLE_NAME_A);	
	i3String::Concat(szSaveDirectory, "\\*.*");

	BOOL bWorking = finder.FindFile(szSaveDirectory);
	if( !bWorking)
	{
		i3mem::FillZero( szSaveDirectory, sizeof(char)*MAX_STRING_COUNT);
		//i3String::Copy( szSaveDirectory, szTemp);
		//i3String::Concat(szSaveDirectory, "\\Script\\Etc");

		i3String::Copy( szSaveDirectory, appdata, MAX_STRING_COUNT );
		i3String::Concat(szSaveDirectory, "\\");
		i3String::Concat(szSaveDirectory, GAME_TITLE_NAME_A);	

		CreateDirectory( szSaveDirectory, NULL);
	}

	if( !FileStream.Open( szFile, STREAM_WRITE))		// pszPath
	{
		if( !FileStream.Create( szFile, STREAM_WRITE))	// pszPath
		{
			return FALSE;
		}
	}

	if( m_pCheckBox->getCheckBoxState())
	{
		char szID[MAX_STRING_COUNT] = {0,};
		i3String::Copy( szID, m_pEditBoxID->getIMETextBuf(), MAX_STRING_COUNT );
		FileStream.Write( szID, sizeof(char)*MAX_STRING_COUNT);

#if defined( I3_DEBUG)
	__setlogId( szID ); 
#endif
	}
	else
	{
		FileStream.Close();

		// 파일을 저장합니다.
		//i3String::Concat( szTemp, "\\Script\\Etc\\Log.log");
		//i3String::Concat( szTemp, "\\PointBlank\\Log.log");
		DeleteFile( szFile );
	}

	return TRUE;
}

void CStageLogin::LoadID( char* pszPath)
{
	if( pszPath == NULL)
	{
		return;
	}

	CString appdata = GetWritableFolder();
	char	szFile[MAX_STRING_COUNT] = {0,};
	char	szTemp[MAX_STRING_COUNT] = {0,};

	i3String::Copy( szFile, appdata, MAX_STRING_COUNT );
	i3String::Concat( szFile, pszPath );
	
	i3FileStream FileStream;
	if( !FileStream.Open( szFile, STREAM_READ | STREAM_SHAREREAD))
	{
		m_pEditBoxID->SetFocus( TRUE);
		m_pGUIRoot->setFocusControl( m_pEditBoxID);
		return;
	}
	
	FileStream.Read( szTemp, MAX_STRING_COUNT);

	szTemp[MAX_STRING_COUNT-1] = NULL;
	m_pEditBoxID->SetIMEText( szTemp);
	m_pCheckBox->setCheckBoxState( TRUE);
	m_pEditBoxPW->SetFocus( TRUE);
}

#if defined(LOCALE_CHINA)

void CStageLogin::LoadUserCheck( char* pszPath)
{

	if( pszPath == NULL)
	{
		return;
	}

	i3FileStream FileStream;
	
	if( FALSE == FileStream.Open( pszPath, STREAM_READ | STREAM_SHAREREAD) )
		return;

	WCHAR16 wchTemp;
	//	유니코드 파일인지 검사
	FileStream.Read( &wchTemp, sizeof( WCHAR16));


//	여기에 걸리면 스크립트 파일이 유니코드가 아닙니다. 반드시 유니코드 파일이어야 합니다. komet
	if( !CHECK_UNICODE(wchTemp))
	{
		I3FATAL( "String Filter have not unicode !! (%s)", pszPath);
		return;
	}

	// 앞의 유니코드 문자는 빼 준다.
	UINT32 uiTextCnt = FileStream.GetSize()/2 - 1;
	UINT32 uiTextsSize = FileStream.GetSize() - 2;

	// 뒤에 널 문자 더해 준다.
	void* wszTexts = new char [uiTextsSize+2];
	char* szTexts = new char [uiTextsSize+1];

	i3mem::FillZero( wszTexts, uiTextsSize+2);
	i3mem::FillZero( szTexts, uiTextsSize+1);


	FileStream.Read(wszTexts, uiTextsSize);

	INT32 iLen = wcslen((WCHAR16*)wszTexts);
	//	유니코드 문자를 멀티바이트 문자로 변환
	WideCharToMultiByte( GetCodePage(), 0, (WCHAR16*)wszTexts,iLen, szTexts, uiTextsSize, NULL, NULL);


	FileStream.Close();

	m_pUserLicenseText->ReCreateTextEx( GetDefaultFontName(), iLen, GAME_FONT_DEFAULT_SIZE, FALSE, NULL);
	m_pUserLicenseText->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pUserLicenseText->SetTextColor(GetColor(GCT_VALUE));
	m_pUserLicenseText->SetEnable( TRUE);
	m_pUserLicenseText->SetText( szTexts);
	m_pUserLicenseText->SetTextAutoWrap( TRUE);
	m_pUserLicenseText->SetWordLineFeed( FALSE);
	m_pUserLicenseText->SetOutCharaProcess( TRUE);
	m_pUserLicenseText->SetOnlyEditOffsetPos(0.f, 7.f);
	

	m_pUserLicenseText->getTextNodeDX()->getTextAttrDX()->removeState(I3_TEXT_STATE_UPDATE_POS | I3_TEXT_STATE_UPDATE_ALIGN | I3_TEXT_STATE_UPDATE_COLOR | I3_TEXT_STATE_UPDATE_TEX);
	
	m_iLineCnt = m_pUserLicenseText->getTextNodeDX()->getTextAttrDX()->getVisibleLineCount();

	m_pUserLicenseList->Clear();
	m_pUserLicenseList->SetSliderPos(0);
	m_pUserLicenseList->setInputDisable( FALSE);
	m_pUserLicenseList->CreateTextEx(14,  GetDefaultFontName(), FONT_COUNT_256);
	m_pUserLicenseList->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pUserLicenseList->SetSelColor(0, 0, 0, 0);
	m_pUserLicenseList->SetTextColor(GetColor(GCT_DEFAULT));
	
	for(INT32 i = 0; i < m_iLineCnt; i++)
	{
		m_pUserLicenseList->AddElement("");
	}

	delete [] wszTexts;
	delete [] szTexts;
}

void	CStageLogin::OnLicenseListScroll()
{
	REAL32 rCurScrollValue = m_pUserLicenseList->getCurValue();
	INT32 iLineCnt = m_pUserLicenseText->getTextNodeDX()->getTextAttrDX()->getLineCount();
	INT32 iCurValue = (INT32)(rCurScrollValue * (m_iLineCnt-14)*0.01f);

	I3TRACE("rCurScrollValue %f iCurValue %d\n", rCurScrollValue, iCurValue);

	INT32 iTemp = m_pUserLicenseText->getTextNodeDX()->getTextAttrDX()->getLineHeight();
	m_pUserLicenseText->SetOnlyTextOffsetPos(0.f, -(iTemp*iCurValue));
	if(rCurScrollValue >= 99){
		m_pUserLicenseConfirm->SetEnable(TRUE);
	}
	else{
		m_pUserLicenseConfirm->SetEnable(FALSE);

	}
}

void	CStageLogin::OnWheelUpdate(INT32 scrollby)
{
	if (0 < scrollby)
	{
		for ( INT32 i = 0 ; i < scrollby ; ++i )
			m_pUserLicenseList->MovePrev();
	}
	else
	{
		for ( INT32 i = 0 ; i > scrollby ; --i )
			m_pUserLicenseList->MoveNext();
	}
	OnLicenseListScroll();
}

#endif

#if defined(_HACK_SHIELD_) && defined(USING_HSMS)
extern AhnHS_SetUserId_Proc	g_pHS_SetUserID;
#endif

void CStageLogin::ProcessGameContext( REAL32 rDeltaSeconds)
{
	for(INT32 i = 0; i < g_pGameContext->GetRecvEventCount(); i++)
	{
		INT32 Arg = 0;
		INT32 nEvent = g_pGameContext->GetEventGame(&Arg);

		switch(nEvent)
		{
#if defined(LOCALE_CHINA)
		case EVENT_RECEIVE_REALNAME: //FCM_INFO_ACK 실명제 적용여부(Yes or No) 패킷 날라옴 by 하동익
			
			if( EV_SUCCESSED(Arg))
			{
				g_pSDOA_Link->SetbRanderFlag(TRUE);	 //피로도 적용

				CGameFramework::JumpToReady();
			}
			else //한번 클리어 & 다이얼로그 Close
			{
				if ( m_pmbLoginWait )
				{
					g_pFramework->CloseMessageBox(m_pmbLoginWait);
					m_pmbLoginWait = NULL;
				}

				m_pStaticRNBG->SetEnable(TRUE);
				m_pStaticSRNName->SetEnable(TRUE);
				m_pStaticSRNSnum->SetEnable(TRUE);
				m_pStaticSRNEmail->SetEnable(TRUE);
				m_pStaticRNName->SetEnable(TRUE);
				m_pStaticRNSnum->SetEnable(TRUE);
				m_pStaticRNEmail->SetEnable(TRUE);
				m_pButtonRNConfirm->SetEnable(TRUE);
				g_pSDOA_Link->SetbRanderFlag(FALSE);
				m_LoginWindowFlag = FALSE;
				m_RNFlag = 1;
			}
			break;
#endif
		case EVENT_CONNECT_SERVER_GATEWAY:
			if( EV_SUCCESSED(Arg))
			{
#if defined(CHANGE_SERVER_MOVE)
				g_pGameContext->ChangeSocket();
#endif
#ifdef JUST_RENDEZVOUS_SERVER
				g_pGameContext->SetEvent( EVENT_RENDEZVOUS_CONNECT );
#else
				if (g_pGameContext->IsWebLogin())
				{
					g_pGameContext->SetEvent(EVENT_LOG_IN_WEBKEY);
				}
				else
				{
					g_pGameContext->SetEvent(EVENT_LOG_IN);
				}

#if defined(_HACK_SHIELD_) && defined(USING_HSMS)
				if (g_pHS_SetUserID)
				{
					g_pHS_SetUserID(g_pGameContext->getUserID());
				}
#endif
#endif
			}
			else
			{
				_ResetWaitConnect();
				if( EV_IS_ERROR(Arg, EVENT_ERROR_FAIL))
					EhFailConnect();
				else
					EhStageError(nEvent,Arg);
			}
			break;
		case EVENT_LOG_IN:
			if( EV_SUCCESSED(Arg))
			{
				//	2011-03-07 중국 실명제
				//  중국이 아닌 국가는 다음 스테이지, 중국은 실명제 정보 ACK올때까지 대기
				//g_pGameContext->SetMyUID( Arg);
				
				//20110407하동익
				#if !defined(LOCALE_CHINA)
					CGameFramework::JumpToReady();
				#else
					#if !defined(USE_SDOA_LOGIN)
						CGameFramework::JumpToReady();
					#endif
				#endif
			}
			else
			{
				_ResetWaitConnect();

				if ( IsLogInBlockError(Arg) )
				{
					UINT32 ArgHead = Arg & EVENT_ERROR_EVENT_LOG_IN_BLOCK_HEADER_MASK;
					switch (ArgHead)
					{
					case EVENT_ERROR_EVENT_LOG_IN_BLOCK_INNER:		EhBlockInner(Arg);		break;
					case EVENT_ERROR_EVENT_LOG_IN_BLOCK_OUTER:		EhBlockOuter(Arg);		break;
					case EVENT_ERROR_EVENT_LOG_IN_BLOCK_GAME:		EhBlockGame(Arg);		break;
					}
				}
				else
				{
					switch (Arg & EVENT_ERROR_MASK_FAIL)
					{
					case EVENT_ERROR_EVENT_LOG_IN_ALEADY_LOGIN:		EhAlreadyLogin();		break;
					case EVENT_ERROR_EVENT_LOG_IN_INVALID_ACCOUNT:	EhInvalidAccount();		break;
					case EVENT_ERROR_EVENT_LOGOUTING:				EhLogouting();			break;
					case EVENT_ERROR_EVENT_LOG_IN_TIME_OUT1:		EhTimeOut1();			break;
					case EVENT_ERROR_EVENT_LOG_IN_TIME_OUT2:		EhTimeOut2();			break;
					case EVENT_ERROR_EVENT_LOG_IN_BLOCK_ACCOUNT:	EhBlockAccount();		break;

					case EVENT_ERROR_EVENT_LOG_IN_DB_BUFFER_FULL:	EhDBBufferFull();		break;

					case EVENT_ERROR_EVENT_LOG_IN_ID_FAIL:			EhInvalidID();			break;
					case EVENT_ERROR_EVENT_LOG_IN_PW_FAIL:			EhInvalidPassword();	break;
					case EVENT_ERROR_EVENT_LOG_IN_DELETE_ACCOUNT:	EhDeletedAccount();		break;
					case EVENT_ERROR_EVENT_LOG_IN_EMAIL_AUTH_ERROR:	EhEmailAuthError();		break;
					case EVENT_ERROR_EVENT_LOG_IN_BLOCK_IP:			EhBlockIP();			break;

					case EVENT_ERROR_EVENT_LOG_IN_USER_ITEM_FAIL:	EhInventoryFail();		break;

					case EVENT_ERROR_EVENT_LOG_IN_BLOCK_COUNTRY : EhInvalidCountry();	break;
					case EVENT_ERROR_EVENT_LOG_IN_CUSTOM_MESSAGE: EhInvalidAccount(g_pGameContext->GetCustomMSG()); break;

					case EVENT_ERROR_EVENT_LOG_IN_MAXUSER:
						ERROR_POPUP(GAME_STRING("STBL_IDX_EP_SERVER_USER_FULL_C"));
						break;

					case EVENT_ERROR_EVENT_LOG_IN_UNKNOWN:
					case EVENT_ERROR_EVENT_LOG_IN_HTTP:
					case EVENT_ERROR_LOGIN_BREAK_SESSION:
					default:
						EhUnknownLogin(nEvent, Arg);
						break;
					}

				}
#if 0
				if( EV_IS_ERROR(Arg, EVENT_ERROR_EVENT_LOG_IN_ALEADY_LOGIN))			EhAlreadyLogin();
				else if( EV_IS_ERROR(Arg, EVENT_ERROR_EVENT_LOG_IN_INVALID_ACCOUNT))	EhInvalidAccount();
				else if( EV_IS_ERROR(Arg, EVENT_ERROR_EVENT_LOG_IN_ID_FAIL))			EhInvalidID();
				else if( EV_IS_ERROR(Arg, EVENT_ERROR_EVENT_LOG_IN_PW_FAIL))			EhInvalidPassword();
				else if( EV_IS_ERROR(Arg, EVENT_ERROR_EVENT_LOGOUTING))					EhLogouting();
				else if( EV_IS_ERROR(Arg, EVENT_ERROR_EVENT_LOG_IN_TIME_OUT1))			EhTimeOut1();
				else if( EV_IS_ERROR(Arg, EVENT_ERROR_EVENT_LOG_IN_TIME_OUT2))			EhTimeOut2();
				else if( EV_IS_ERROR(Arg, EVENT_ERROR_EVENT_LOG_IN_BLOCK_ACCOUNT))		EhBlockAccount();
				else if( EV_IS_ERROR(Arg, EVENT_ERROR_EVENT_LOG_IN_UNKNOWN))			EhUnknownLogin(nEvent, Arg);
				else if( EV_IS_ERROR(Arg, EVENT_ERROR_EVENT_LOG_IN_HTTP))				EhUnknownLogin(nEvent, Arg);
				else if( EV_IS_ERROR(Arg, EVENT_ERROR_EVENT_LOG_IN_USER_ITEM_FAIL))		EhInventoryFail();
				else if( EV_IS_ERROR(Arg, EVENT_ERROR_LOGIN_BREAK_SESSION))				EhUnknownLogin(nEvent, Arg);
				else if( EV_IS_ERROR(Arg, EVENT_ERROR_EVENT_LOG_IN_MAXUSER))			{ERROR_POPUP(GAME_STRING("STBL_IDX_EP_SERVER_USER_FULL_C"));}
				else if( EV_IS_ERROR(Arg, EVENT_ERROR_EVENT_LOG_IN_BLOCK_GAME))			EhBlockGame(Arg);
				else if( EV_IS_ERROR(Arg, EVENT_ERROR_EVENT_LOG_IN_BLOCK_INNER))		EhBlockInner(Arg);
				else if( EV_IS_ERROR(Arg, EVENT_ERROR_EVENT_LOG_IN_BLOCK_OUTER))		EhBlockOuter(Arg);
				else if( EV_IS_ERROR(Arg, EVENT_ERROR_EVENT_LOG_IN_DB_BUFFER_FULL))		EhDBBufferFull();
				else if( EV_IS_ERROR(Arg, EVENT_ERROR_EVENT_LOG_IN_DELETE_ACCOUNT))		EhDeletedAccount();
				else if( EV_IS_ERROR(Arg, EVENT_ERROR_EVENT_LOG_IN_EMAIL_AUTH_ERROR))		EhEmailAuthError();
				else if( EV_IS_ERROR(Arg, EVENT_ERROR_EVENT_LOG_IN_BLOCK_IP))		EhBlockIP();
				else EhStageError(nEvent,Arg);
#endif

			g_pGameContext->SetEvent(EVENT_DISCONNECT);
			#if defined (LOCALE_CHINA) && defined(USE_SDOA_LOGIN)
				g_pSDOA_Link->ShowLoginDlg();
			#endif
			}
			break;
		case EVENT_LOG_OUT:
			_ResetWaitConnect();

			// 성공, 실패 공통
			EhWaitDisconnect();

			if( EV_FAILED(Arg))
			{
				EhStageError(nEvent,Arg);
			}
			break;
		case EVENT_M_DRIVE_AWAY:
			_ResetWaitConnect();
			CStageGUI::DriveAway(Arg);
			break;
		case EVENT_M_ERROR:
			_ResetWaitConnect();
			break;
		case EVENT_M_GAMEGUARD_ERROR:
			_ResetWaitConnect();
#if defined(LOCALE_CHINA)
			g_pSDOA_Link->SetbLoginFlag(FALSE);
#endif
			SYSTEM_POPUP_C(GAME_STRING("STBL_IDX_EP_GAMEGUARD_ERROR"), EXIT_CALLBACK(CbExitNormal), this);
			break;
		case EVENT_M_DISCONNECTION:
			_ResetWaitConnect();
#if defined(LOCALE_CHINA)
			g_pSDOA_Link->SetbLoginFlag(FALSE);
#endif
			SYSTEM_POPUP_C(GAME_STRING("STBL_IDX_EP_GAME_EXIT_ASSERT_E"), EXIT_CALLBACK(CbExitNormal), this);
			break;
		case EVENT_M_DESTROYED_NETWORK:	// 네트워크가 끊어졌다
			_ResetWaitConnect();
			EhFailConnect();
			I3TRACE("EVENT_M_DESTROYED_NETWORK\n");
			break;

	/*	case EVENT_FCM_INFO_RETRY:
			g_pGameContext->SetEvent(EVENT_FCM_INFO_RETRY);

			break;*/
		default:
			{
				_ResetWaitConnect();
				g_pGameContext->SetEvent(EVENT_DISCONNECT);

				//I3ASSERT_0;	// 처리못한 EVENT가 존재합니다. nEvent값을 알려주세요 - praptor
				char bug[256];
				sprintf_s(bug, "EVENT_ERROR(0x%x)", nEvent);
				g_pFramework->MessageBox(bug, NULL, MBF_GAME_OK);
			}
			break;
		}
	}
}

void CStageLogin::OnControlNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch( pNotify->m_nID)
	{
	case GUI_ID_LOGIN_BUTTON_JOIN:
		if( pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
			OnJoinButtonClick();
		break;
	case GUI_ID_LOGIN_BUTTON_LOGIN:
		if( pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
			OnLoginButtonClick();
		break;
#if defined(LOCALE_CHINA)
	case GUI_ID_REALNAME_BUTTON_CONFIRM:
		if( pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
			OnRealnameButtonClick();
		break;
	case GUI_ID_REALNAME_LICENSE_CONFIRM:
		if( pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
			OnRealnameLicenseButtonClick();
		break;
	case GCI_SSP_S_LICENSE_SCROLL:
		if ( I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent )
			OnLicenseListScroll();
		break;
#endif
	}

	CStageGUI::OnControlNotify(pNotify);
}


void	CStageLogin::ConnectLogInServer()
{
	g_pGameContext->SetEvent(EVENT_CONNECT_SERVER_GATEWAY);
	m_bConnectStart	= TRUE;
}


void CStageLogin::OnJoinButtonClick(void)
{
#if defined( BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG) || defined( I3_RELEASE_PROFILE)	//가입하기 버튼 클릭시 동작하기
	if( g_pFramework->getUnitTest() != NULL)
	{
		STRING_POPUP(GAME_STRING("STR_TBL_GUI_LOGIN_RUN_UNIT_TEXT1"));/*Unit 테스트가 실행중입니다.\n실행파일의 커맨드 라인 인자를 참고하세요.\n \n테스트중에는 임의로 실행할 수 없습니다.*/
		return;
	}

	g_pConfig->m_bNetworking = FALSE;

	g_pFramework->SetWeaponListDummy();		//	인벤 무기 구성

	//	장비할 무기 셋팅
	USER_INFO_EQUIPMENT_WEAPON	InfoEquipWeapon;
	g_pGameContext->GetMyUserInfoEquipmentWeapon(InfoEquipWeapon);
	ITEM_TYPE ItemClass = GetItemType_TempFunc((WEAPON_CLASS_TYPE)g_DebugWeaponSet[0][0]);

	InfoEquipWeapon._noprim	= MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE,	ItemClass,			 g_DebugWeaponSet[0][0], g_DebugWeaponSet[0][1]);
	InfoEquipWeapon._nosub	= MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE,	ITEM_TYPE_SECONDARY, g_DebugWeaponSet[1][0], g_DebugWeaponSet[1][1]);
	InfoEquipWeapon._nomelee = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE,	ITEM_TYPE_MELEE,	 g_DebugWeaponSet[2][0], g_DebugWeaponSet[2][1]);
	InfoEquipWeapon._nothrow = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE,	ITEM_TYPE_THROWING1,	 g_DebugWeaponSet[3][0], g_DebugWeaponSet[3][1]);
	InfoEquipWeapon._noitem	= MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE,	ITEM_TYPE_THROWING2,	 g_DebugWeaponSet[4][0], g_DebugWeaponSet[4][1]);

	g_pGameContext->SetMyUserInfoEquipmentWeapon(InfoEquipWeapon);

	//	현재 장비된 무기의 유효성 검사...여기서 걸리면 인벤에 없는 무기를 장비한 것임. (인벤토리를 구성한 후여야 함)
	if( !g_pFramework->GetInvenList()->CheckAvariableMyWeapon())
	{
		STRING_POPUP(GAME_STRING("STR_TBL_GUI_LOGIN_FAIL_WRONG_WEAPON"));/*더이상 진행할 수 없습니다.\n 인벤토리에 없는 무기가 장비되어 있습니다.\n 장비한 무기를 확인해 주세요*/
		return;
	}
	
	g_pFramework->GotoNextStage( g_DebugStartingMap.c_str() );

#else
	//	라이브 버전에서 가입하기 누르면 지정된 웹사이트를 띄운다.
	if( true != g_pConfigEx->GetMisc().JoinAddress.empty() )
	{
		//	가입하기 사이트
		g_pFramework->OpenWebBrowser(g_pConfigEx->GetMisc().JoinAddress.c_str(), NULL, true);
	}
#endif
}

void CStageLogin::OnLoginButtonClick(void)
{
	if( g_pFramework->getUnitTest() != NULL)
	{
		/*Unit 테스트가 실행중입니다.\n실행파일의 커맨드 라인 인자를 참고하세요.\n \n테스트중에는 로그인할 수 없습니다.*/
		STRING_POPUP(GAME_STRING("STR_TBL_GUI_LOGIN_RUN_UNIT_TEXT2"));
		return;
	}

	if (m_bConnectStart )
	{
		return;
	}
	#if !defined (LOCALE_CHINA) 
	if( i3String::Length( m_pEditBoxID->getIMETextBuf()) <= 0)
	{
		EhNonID();
		return;
	}
	#endif

	g_pConfig->m_bNetworking = TRUE;

	BOOL bIsWrongID = FALSE;

#if defined (LOCALE_INDONESIA) || defined (LOCALE_TURKEY)
	// 인도네시아만 아이디를 소문자로 변경해서 보냅니다!
	// 크레온에서 협의없이 아이디를 소문자로 바꿨기 때문입니다!
	char ID[MAX_PATH] = {};
	i3String::ToLower(ID, MAX_PATH, m_pEditBoxID->getIMETextBuf());
#elif defined (LOCALE_CHINA) 
	#if defined(USE_SDOA_LOGIN)
		const char * ID = g_pSDOA_Link->GetLoingResult()->szSndaid;
		const char * SndaSessionKey = g_pSDOA_Link->GetLoingResult()->szSessionId;
		g_pGameContext->SetSDOASndaID(ID);
		g_pGameContext->SetSDOASessionID(SndaSessionKey);
	#else
		//20110407하동익 샨다 로그인 모드 비활성화
		const char * ID = m_pEditBoxID->getIMETextBuf();
	#endif
#else
	const char * ID = m_pEditBoxID->getIMETextBuf();
#endif

	//브라질의 경우, 아이디 입력시부터 소문자/특수문자 체크를 함. 
	//아이디 입력시 특수문자가 들어있다면 그냥 꺼버린다.
#if (defined(LOCALE_BRAZIL) && !defined(BUILD_RELEASE_QA_VERSION))
	if(!g_pFramework->CheckValidCharacterByLocale(ID))
	{
		bIsWrongID = TRUE;
	}
#elif defined(LOCALE_INDONESIA)
	// 인도네시아의경우 알파벳대소문자,숫자만허용합니다.(닉네임은 모두가능)
	if(!CheckIsAlphaNumericStr(ID))
	{
		bIsWrongID = TRUE;
	}
#endif
	
	if(!bIsWrongID)
	{
		// ID
		g_pGameContext->setUserID(ID);
		g_pGameContext->setUserPW(m_pEditBoxPW->getIMETextBuf());

		// ID저장
		char szLog[MAX_STRING_COUNT] = "";
		i3String::Format( szLog, MAX_STRING_COUNT, "/%s/Log.log", GAME_TITLE_NAME_A);
		if( SaveID( szLog) )
		{
			ConnectLogInServer();
		}
		else
		{
			ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_LOGIN_FAIL_SAVE_ID_C"), MAKE_CALLBACK(CbFailSaveID), this);
			// 저장 실패 메시지 띄움
			// 확인 누르면 컨넥트 서버 이벤트 날림
		}
	}
	else
	{
#if defined(LOCALE_INDONESIA)
		STRING_POPUP_C(GAME_STRING("STBL_IDX_EP_LOGIN_INVALID_ID"), MAKE_CALLBACK(MsgBoxCallBack_LoginFailWithBadID), this);
#else
		ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_LOGIN_FAIL_SAVE_ID_C"), MAKE_CALLBACK(CbFailSaveID), this);
		// 저장 실패 메시지 띄움
		// 확인 누르면 컨넥트 서버 이벤트 날림
#endif
	}
}

#if defined(LOCALE_CHINA)
//중국 하동익 2011-03-07
void CStageLogin::OnRealnameButtonClick(void)
{
	if(!strlen(m_pStaticRNName->getIMETextBuf()) || !strlen(m_pStaticRNSnum->getIMETextBuf()) || !strlen(m_pStaticRNEmail->getIMETextBuf()))
	{
		STRING_POPUP(GAME_STRING("STR_CHINA_REALNAME_INFO_LEAK")); //정보 입력이 부족합니다.
		m_pStaticRNName->SetIMEText("");
		m_pStaticRNSnum->SetIMEText("");
		m_pStaticRNEmail->SetIMEText("");
		return;
	}


	g_pConfig->m_bNetworking = TRUE;
	if( g_pFramework->getUnitTest() != NULL)
	{
		/*GameContext에 서버에 전송할 이름,시큐리티넘버,이메일 전송*/
		STRING_POPUP(GAME_STRING("STR_TBL_GUI_LOGIN_RUN_UNIT_TEXT2"));
		return;
	}
	RETURN_VALUE URL_VALUE;
	
	i3String::Copy( URL_VALUE.username, m_pStaticRNName->getIMETextBuf(), 50 );
	i3String::Copy( URL_VALUE.idCard, m_pStaticRNSnum->getIMETextBuf(), 100 );
	i3String::Copy( URL_VALUE.Email, m_pStaticRNEmail->getIMETextBuf(), 100 );
	i3String::Copy( URL_VALUE.appArea, "999", 20 );
	i3String::Copy( URL_VALUE.appId, "631", 20 );
	GetHostIP(URL_VALUE.IP,100);

	CHttpFCMInfo URL_Data;
	
	if(URL_Data.ConnectionCreate(g_pGameContext->Receive_URL)){
		if(EVENT_ERROR_SUCCESS == URL_Data._WorkProcess(g_pGameContext->Receive_URL,&URL_VALUE)){
			//g_pFramework->GotoNextStage("Ready");
			//클라이언트 -> 서버 이벤트(Request) : 하동익 소스정리 필요


			m_pStaticRNBG->SetEnable(FALSE);
			m_pStaticSRNName->SetEnable(FALSE);
			m_pStaticSRNSnum->SetEnable(FALSE);
			m_pStaticSRNEmail->SetEnable(FALSE);
			m_pStaticRNName->SetEnable(FALSE);
			m_pStaticRNSnum->SetEnable(FALSE);
			m_pStaticRNEmail->SetEnable(FALSE);
			m_pButtonRNConfirm->SetEnable(FALSE);

			m_pUserLicenseWindow->SetEnable(TRUE);
			m_pUserLicenseScrollFrame->SetEnable(TRUE);
			m_pUserLicenseText->SetEnable(TRUE);
			m_pUserLicenseList->SetEnable(TRUE);
			m_pUserLicenseConfirm->SetEnable(FALSE);
			//1. FALSE -> 인증성공후 OnUpdate의 로그인에 안빠진다.
			//2. TRUE라면 바로 로그인을 시도한다. - 로그인창으로 가지 않는다.(방식 어떻게 할지 상의해야함)
			
			//m_bConnectStart = FALSE;
			//1의 방식이라면 로그인 창 다시 호출
			
			m_rConnectWaitTimeStamp	= 0.0f;
			URL_Data.ConnectionDestroy();
		}
		else{
			//실명인증 Clear후 재입력 요구, 메시지 박스 호출(인증이 되지 않았습니다)
			STRING_POPUP(GAME_STRING("STR_CHINA_REALNAME_FAIL"));
			g_pSDOA_Link->SetbLoginFlag(FALSE);//FALSE -> OnUpdate의 로그인에 안빠진다.
			m_bConnectStart = FALSE;
			m_pStaticRNName->SetIMEText("");
			m_pStaticRNSnum->SetIMEText("");
			m_pStaticRNEmail->SetIMEText("");
			URL_Data.ConnectionDestroy();
		}
	}
}

void CStageLogin::OnRealnameLicenseButtonClick(void)
{
	g_pGameContext->SetEvent(EVENT_FCM_INFO_RETRY);
	g_pSDOA_Link->SetbRanderFlag(TRUE);
}


#endif

void CStageLogin::EhNonID(void)
{
	ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_LOGIN_NONE_ID"), MAKE_CALLBACK(CbPopupOK), this);

	m_pFocusedControl = m_pEditBoxID;
}

void CStageLogin::EhNonPW(void)
{
	ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_LOGIN_NONE_PW"), MAKE_CALLBACK(CbPopupOK), this);

	m_pFocusedControl = m_pEditBoxPW;
}

void CStageLogin::EhInvalidAccount(void)
{

	ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_LOGIN_INVALID_ACCOUNT"), MAKE_CALLBACK(GetErrorHandler()), this);

	m_pEditBoxPW->SetIMEText( "");
	m_pFocusedControl = m_pEditBoxID;
}

void	CStageLogin::EhInvalidID(void)
{
	ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_LOGIN_INVALID_ID"), MAKE_CALLBACK(GetErrorHandler()), this);

	m_pEditBoxPW->SetIMEText( "");
	m_pFocusedControl = m_pEditBoxID;
}

void	CStageLogin::EhInvalidPassword(void)
{
	ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_LOGIN_INVALID_PASSWORD"), MAKE_CALLBACK(GetErrorHandler()), this);

	m_pEditBoxPW->SetIMEText( "");
	m_pFocusedControl = m_pEditBoxID;
}

void CStageLogin::EhLogouting()
{
	ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_LOGIN_LOGOUTING"), MAKE_CALLBACK(GetErrorHandler()), this);

	m_pEditBoxPW->SetIMEText( "");
	m_pFocusedControl = m_pEditBoxID;
}

void CStageLogin::EhDBBufferFull()
{
	ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_LOGIN_DB_BUFFER_FULL"), MAKE_CALLBACK(GetErrorHandler()), this);

	m_pEditBoxPW->SetIMEText( "");
	m_pFocusedControl = m_pEditBoxID;
}

void CStageLogin::EhDeletedAccount()
{
	ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_LOGIN_DELETE_ACCOUNT"), MAKE_CALLBACK(GetErrorHandler()), this);

	m_pEditBoxPW->SetIMEText( "");
	m_pFocusedControl = m_pEditBoxID;
}

void CStageLogin::EhEmailAuthError()
{
	ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_LOGIN_EMAIL_AUTH_ERROR"), MAKE_CALLBACK(GetErrorHandler()), this);

	m_pEditBoxPW->SetIMEText( "");
	m_pFocusedControl = m_pEditBoxID;
}

void CStageLogin::EhBlockIP()
{
	ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_LOGIN_BLOCK_IP"), MAKE_CALLBACK(GetErrorHandler()), this);

	m_pEditBoxPW->SetIMEText( "");
	m_pFocusedControl = m_pEditBoxID;
}

// 북미 Error Handler 추가 (2011.06.10 양승천)

void CStageLogin::EhInvalidAccount(const char *Msg)
{
	if(i3String::Length(Msg) > 0)
		ERROR_POPUP_C(Msg, MAKE_CALLBACK(GetErrorHandler()), this);
	else
		ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_LOGIN_INVALID_ACCOUNT"), MAKE_CALLBACK(GetErrorHandler()), this);
}

void CStageLogin::EhInvalidCountry()
{
	ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_LOGIN_BLOCK_COUNTRY"), MAKE_CALLBACK(GetErrorHandler()), this);
}



void CStageLogin::EhAlreadyLogin(void)
{
#if defined(I3_DEBUG)
	ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_LOGIN_ALREADY_LOGIN_Q"), MAKE_CALLBACK(CbAlreadyLogin), this);
	m_pFocusedControl = m_pEditBoxID;

#else
#if defined(LOCALE_CHINA)
	g_pSDOA_Link->SetbLoginFlag(FALSE);
#endif
	ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_LOGIN_ALREADY_LOGIN_WEB_E"), EXIT_CALLBACK(CbExitNormal), this);
	Sleep(1000*10);
#endif

}

void CStageLogin::EhTimeOut1(void)
{
	ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_LOGIN_TIME_OUT_1"), MAKE_CALLBACK(GetErrorHandler()), this);
	m_pFocusedControl = m_pEditBoxID;
}

void CStageLogin::EhTimeOut2(void)
{
	ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_LOGIN_TIME_OUT_2"), MAKE_CALLBACK(GetErrorHandler()), this);
	m_pFocusedControl = m_pEditBoxID;
}

void CStageLogin::EhBlockAccount(INT32 Arg)
{
	string	str = GAME_STRING("STBL_IDX_EP_LOGIN_BLOCK_ACCOUNT");

	if ( Arg != 0 )
	{
		str += BOOST_FORMAT1("(error:%d)", Arg);
	}

	ERROR_POPUP_C(str.c_str(), MAKE_CALLBACK(GetErrorHandler()), this);
	m_pFocusedControl = m_pEditBoxID;
}

void CStageLogin::EhUnknownLogin(INT32 Event,INT32 Arg)
{
#if defined( I3_DEBUG)//에러 핸들러 관련 정의되지 않은 에러코드

	char strBuf[MAX_STRING_COUNT];
	sprintf_s(strBuf, "%s Event(%d), Arg(0x%x)\n", GAME_STRING("STBL_IDX_EP_LOGIN_UNKNOWN"), Event, Arg);

	STRING_POPUP_C(strBuf, EXIT_CALLBACK(CbExitNormal), this);

#else
#if defined(LOCALE_CHINA)
	g_pSDOA_Link->SetbLoginFlag(FALSE);
#endif
	ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_LOGIN_UNKNOWN"), EXIT_CALLBACK(CbExitNormal), this);

#endif
}

void CStageLogin::EhInventoryFail(void)
{
#if defined(LOCALE_CHINA)
	g_pSDOA_Link->SetbLoginFlag(FALSE);
#endif
	STRING_POPUP_C(GAME_STRING("STR_TBL_GUI_LOGIN_FAIL_GET_FROM_STORE_HOUSE"), EXIT_CALLBACK(CbExitNormal), this);/*창고 목록을 받아오는데 실패하였습니다.\n창고 목록이 없으면 게임을 진행할 수 없어 종료합니다.*/
}

void CStageLogin::EhWaitDisconnect(void)
{
	ERROR_POPUP(GAME_STRING("STBL_IDX_EP_LOGIN_WAIT_DISCONNECT"));
}

void CStageLogin::EhFailConnect(void)
{
	if ( m_LogInConnectFailedCount++ < g_pConfigEx->GetNetwork().ServerReconnectCount )
	{
		ConnectLogInServer();
	}
	else
	{
#if defined(LOCALE_CHINA)
		g_pSDOA_Link->SetbLoginFlag(FALSE);
#endif
		SYSTEM_POPUP_C(GAME_STRING("STBL_IDX_EP_LOGIN_FAIL_CONNECT_E"), EXIT_CALLBACK(CbExitNormal), this);

		m_pFocusedControl = m_pEditBoxID;
	}
}


void CStageLogin::_WaitConnectMessageBox(REAL32 rDeltaSeconds)
{
	if( !m_bConnectStart)
		return;
//#if !defined(LOCALE_CHINA)
	m_rConnectWaitTimeStamp += rDeltaSeconds;

	if ( m_LoginError )
		return;

	if(m_RNFlag == 0){
		if( m_rConnectWaitTimeStamp > 1.0f )	// 1초
		{
			if( !m_pmbLoginWait )
			{
				m_pmbLoginWait = ERROR_POPUP_W(GAME_STRING("STBL_IDX_EP_LOGIN_WAIT_CONNECT_W"));
			}
		}

		if ( m_rConnectWaitTimeStamp > g_pConfigEx->GetNetwork().LogInWaitTime_Sec )
		{
			if ( m_pmbLoginWait )
			{
				g_pFramework->CloseMessageBox(m_pmbLoginWait);
				m_pmbLoginWait = NULL;
			}
			I3NOTICE("ConnectLoginServer() -> Perfect Login Fail CallbAck Exit\n");
			#if defined(LOCALE_CHINA) && defined(USE_SDOA_LOGIN)
				g_pSDOA_Link->SetbLoginFlag(FALSE);
			#endif
			m_LoginError = TRUE;
			SYSTEM_POPUP_C(GAME_STRING("STBL_IDX_EP_LOGIN_FAIL_CONNECT_E"), EXIT_CALLBACK(CbExitNormal), this);
		}
	}
//#endif
}

void CStageLogin::_ResetWaitConnect(void)
{
	m_bConnectStart = FALSE;
	m_rConnectWaitTimeStamp = 0.0f;

	if( m_pmbLoginWait)
	{
		g_pFramework->CloseMessageBox(m_pmbLoginWait);
		m_pmbLoginWait = NULL;
	}
}

void	CStageLogin::_CheckGameRunTimeOut(void)
{
#if defined(I3_DEBUG)
	return;
#else
	if (g_pGameContext->IsWebLogin())
		return;

	// 강제종료 안내팝업을 띄웠다면 넘긴다.
	if ( m_ForceExit )
		return;

	// 게임에서 로그인 처리를 할 경우처리.
	// 게임 실행 후 일정시간 이상 로그인을 안하고 있다면 사용자 입력이 있다면 강제 종료를 한다.
	// 업데이트를 빗겨나가는 문제때문이다.

	REAL32	ElapsedTime = g_pGameContext->GetAccTime() - m_GameStartTime;
	if ( ElapsedTime > g_pConfigEx->GetNetwork().GameRunWaitTime )
	{
		I3TRACE1("LogIn idle time out : %f\n", ElapsedTime);
#if defined(LOCALE_CHINA)
		g_pSDOA_Link->SetbLoginFlag(FALSE);
#endif
		SYSTEM_POPUP_C(GAME_STRING("STBL_IDX_EP_NOT_LOGIN_TIMEOUT"), EXIT_CALLBACK(CbExitNormal), this);
		m_ForceExit = TRUE;
	}
#endif
}

void CStageLogin::EhStageError(INT32 Event,INT32 Arg)
{
	//I3ASSERT_0;	// ! 현재로선 들어올 Error가 없습니다. 들어오면 Arg를 알려주세요. - praptor
	char bug[256];
	sprintf_s(bug, "ERROR: Event(%d), Arg(0x%x)", Event, Arg);
	g_pFramework->MessageBox(bug, NULL, MBF_GAME_OK);
}

void CStageLogin::EhBlockGame(INT32 Arg)
{
	Arg = Arg & EVENT_ERROR_EVENT_LOG_IN_BLOCK_MASK;

	// 정보가 없다면 일반 블럭 메시지 출력
	if (0 == Arg)
	{
		EhBlockAccount(Arg);
		return;
	}

	// 에러 메시지 출력 우선순위는 하위 비트 순
	for(UINT32 i = 0, nBit = 0x00000001; i <= 13; i++, nBit = nBit << 1)
	{
		if (Arg & nBit)
		{
			char szTemp[256];

			_snprintf(szTemp, 256, "STBL_IDX_BLOCK_GAME_%d", i);
			g_pFramework->MessageBox(GAME_STRING(szTemp), NULL, MBF_GAME_OK, EXIT_CALLBACK(CbExitNormal), this, 8);
			return;
		}
	}
}

void CStageLogin::EhBlockInner(INT32 Arg)
{
	Arg = Arg & EVENT_ERROR_EVENT_LOG_IN_BLOCK_MASK;

	// 정보가 없다면 일반 블럭 메시지 출력
	if (0 == Arg)
	{
		EhBlockAccount(Arg);
		return;
	}

	// 에러 메시지 출력 우선순위는 하위 비트 순
	for(UINT32 i = 0, nBit = 0x00000001; i <= 4; i++, nBit = nBit << 1)
	{
		if (Arg & nBit)
		{
			char szTemp[256];
			_snprintf(szTemp, 256, "STBL_IDX_BLOCK_INNER_%d", i);
			ERROR_POPUP_FONT(GAME_STRING(szTemp), 8);
			return;
		}
	}
}

void CStageLogin::EhBlockOuter(INT32 Arg)
{
	Arg = Arg & EVENT_ERROR_EVENT_LOG_IN_BLOCK_MASK;

	// 정보가 없다면 일반 블럭 메시지 출력
	if (0 == Arg)
	{
		EhBlockAccount(Arg);
		return;
	}

	// 에러 메시지 출력 우선순위는 하위 비트 순
	for(UINT32 i = 0, nBit = 0x00000001; i <= 5; i++, nBit = nBit << 1)
	{
		if (Arg & nBit)
		{
			char szTemp[256];
			_snprintf(szTemp, 256, "STBL_IDX_BLOCK_OUTER_%d", i);
			ERROR_POPUP_FONT(GAME_STRING(szTemp), 8);
			return;
		}
	}
}

ErrorMsgHandler	CStageLogin::GetErrorHandler()
{
	if (g_pGameContext->IsWebLogin())
	{
		return CbExitNormal;
	}

	return CbPopupOK;

}

void CStageLogin::CbPopupOK(void* pThis,INT32 nParam)
{
	((CStageLogin*)pThis)->FocusToControl();
}

void CStageLogin::CbFailSaveID(void* pThis,INT32 nParam)
{
	((CStageLogin*)pThis)->ConnectLogInServer();
	((CStageLogin*)pThis)->FocusToControl();
}

void CStageLogin::CbAlreadyLogin(void* pThis,INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		((CStageLogin*)(g_pFramework->GetCurrentStage()))->EhWaitDisconnect(); // EVENT_LOG_OUT 활성화 전까지 임시
		((CStageLogin*)pThis)->FocusToControl();
	}
}


BOOL CStageLogin::GetEnvSessKey(UINT8* pSessKey)
{
	I3ASSERT(pSessKey);

	char strBuf[MAX_STRING_COUNT];

	if (0 == ::GetEnvironmentVariable("SessKey", strBuf, 256))
	{
		return FALSE;
	}

	// TODO: strBuf를 직접 저장한다. 32byte + 1byte(NULL) - praptor to komet
	g_pGameContext->SetSessionID( strBuf);

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

BOOL CStageLogin::GetEnvMacAddr(INT64* pMacAddr)
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

BOOL CStageLogin::GetEnvServerAddr(char* strServerAddr, const size_t sizeServerAddrStrMax)
{
	char strBuf[MAX_STRING_COUNT];

	if (0 == ::GetEnvironmentVariable("ServerAddr", strBuf, 256))
	{
		return FALSE;
	}

	i3String::Copy(strServerAddr, strBuf, sizeServerAddrStrMax);

	return TRUE;
}
#if defined (LOCALE_CHINA)
void CStageLogin::OnRevive( i3RenderContext * pCtx)
{
	CStageGUI::OnRevive( pCtx);

	g_pSDOA_Link->Create(g_hWnd, g_pD3DDevice, i3String::ToInt(g_pConfigEx->m_AreaCode), "sdasd");

	CALLBACK_EXTERNAL_RENDER m_Func1 = g_pSDOA_Link->GetOnExternalRender();
	CALLBACK_SDOA_MSG_PROC m_Func2 = g_pSDOA_Link->GetMsgProc();
	CALLBACK_SDOA_LOSTDEVICE lostDeviceHandler = g_pSDOA_Link->getLostDevHandler();
	CALLBACK_SDOA_RESETDEVICE resetDeviceHandler = g_pSDOA_Link->getResetDeviceHandler();

	g_pViewer->setExternalRender(m_Func1);
	g_pFramework->setSDOAMsgProc(m_Func2);
	g_pViewer->setExternalRenderLostDeviceHandler( lostDeviceHandler);
	g_pViewer->setExternalRenderReviveHandler( resetDeviceHandler);

}
#endif

void CStageLogin::MsgBoxCallBack_LoginFailWithBadID(void* pThis,INT32 nParam)
{
	if(((CStageLogin*)pThis)->m_pEditBoxID)
		((CStageLogin*)pThis)->m_pEditBoxID->SetFocus(TRUE);
}