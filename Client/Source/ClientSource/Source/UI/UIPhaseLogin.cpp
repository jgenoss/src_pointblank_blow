#include "pch.h"
#include "UIPhaseLogin.h"

#include "UILogin.h"
#include "UIMainFrame.h"
#include "UIUtil.h"
#include "../TimeUtil.h"

#if defined(BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG)
#include "../UnitTest/GameUnitTest.h"
#include "BattleSlotContext.h"
#endif

#include "GameLoginContext.h"
#include "GMAuthorityContext.h"
#include "../StageBattle/NetworkContext.h"
#include "UserInfoContext.h"
#include "Network/GameNetworkManager.h"
#include "Console/PBConsoleClientCtx.h"
#include "MyRoomInfoContext.h"
#include "ClientStageInfoUtil.h"

#include "i3Base/string/stack_string.h"
#include "i3Base/string/ext/format_string.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/contain_string_head.h"
#include "i3Base/string/ext/generic_string_copy.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/generic_string_cat.h"
#include "i3Base/string/algorithm/to_lower.h"

#include "SteamContext.h"


#define CONNECTMESSAGE_WAIT_TIME		1.f			// 1초 후 Message를 띄운다.

static bool bFirst = true;

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
UIPhaseLogin * GetThis( void)
{
	i3Stage * pStage = g_pFramework->GetCurrentStage();
	I3ASSERT( i3::same_of<UILogin*>(pStage));

	return (UIPhaseLogin*)(((UILogin*)pStage)->GetUIPhaseLogin());
}

extern "C" {
	int UIPhaseLogin_ClickJoinBtn( lua_State * L)
	{
	#if ENABLE_UNIT_TEST || defined( I3_RELEASE_PROFILE)	//가입하기 버튼 클릭시 동작하기
		if( UnitTest::get_unit_test() != nullptr)
		{
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_LOGIN_RUN_UNIT_TEXT1"));/*Unit 테스트가 실행중입니다.\n실행파일의 커맨드 라인 인자를 참고하세요.\n \n테스트중에는 임의로 실행할 수 없습니다.*/
			return 0;
		}

		g_pConfig->m_bNetworking = false;

		BattleSlotContext::i()->InitRoomData();
		UserInfoContext::i()->ResetForSingleMode();

		PBCONSOLE_CREATE_FOR_DEBUG_RELEASEQA

		CInvenList::i()->SetWeaponListDummy();		//	인벤 무기 구성

		//	장비할 무기 셋팅
		USER_INFO_EQUIPMENT_WEAPON	InfoEquipWeapon;

		UserInfoContext::i()->GetMyUserInfoEquipmentWeapon(InfoEquipWeapon);

		InfoEquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_PRIMARY ]	= MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE,	ITEM_TYPE_PRIMARY,		g_DebugWeaponSet[0][0], g_DebugWeaponSet[0][1]);
		InfoEquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_SECONDARY ]	= MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE,	ITEM_TYPE_SECONDARY,	g_DebugWeaponSet[1][0], g_DebugWeaponSet[1][1]);
		InfoEquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_MELEE ]		= MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE,	ITEM_TYPE_MELEE,		g_DebugWeaponSet[2][0], g_DebugWeaponSet[2][1]);
		InfoEquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_THROWING1 ]	= MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE,	ITEM_TYPE_THROWING1,	g_DebugWeaponSet[3][0], g_DebugWeaponSet[3][1]);
		InfoEquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_THROWING2 ]	= MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE,	ITEM_TYPE_THROWING2,	g_DebugWeaponSet[4][0], g_DebugWeaponSet[4][1]);

		UserInfoContext::i()->SetMyUserInfoEquipmentWeapon(InfoEquipWeapon);

		
		CSI_MAP* dummy_map		= CStageInfoUtil::GetMapData(g_DebugStartingMap.c_str());
		UINT32	 dummy_stage_id	= StageID::MakeStageID(STAGE_MODE_DEATHMATCH, WEAPON_ALL, AI_USE_NO, false, false, g_DebugSpaceMode ? 8 : 0, (STAGE_UID) (dummy_map != nullptr ? dummy_map->ui8StageUID : 1) );
				
		MyRoomInfoContext::i()->setDummyStage(dummy_stage_id);

		g_pFramework->GotoNextStage( g_DebugStartingMap.c_str() );
		

	#else
		//	라이브 버전에서 가입하기 누르면 지정된 웹사이트를 띄운다.
		if( true != g_pConfigEx->GetMisc().JoinAddress.empty() )
		{
			#if defined (LOCALE_RUSSIA)
				//@eunil. PBRU-1065 러시아 요청사항 사용자 지정 웹브라우저로 띄운다
				g_pFramework->OpenWebBrowserCurrSet(g_pConfigEx->GetMisc().JoinAddress.c_str());
			#else
			//	가입하기 사이트
			g_pFramework->OpenWebBrowser(g_pConfigEx->GetMisc().JoinAddress.c_str(), nullptr, true);
			#endif
		}
	#endif

		return 0;
	}

	int UIPhaseLogin_ClickLoginBtn( lua_State * L)
	{
		if(  UIPhaseLogin::m_rDelayWait > 0)
		{
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_LOGINWAIT_DELAY"));
			return 0;
		}
#if ENABLE_UNIT_TEST
		if( UnitTest::get_unit_test() != nullptr)
		{
			/*Unit 테스트가 실행중입니다.\n실행파일의 커맨드 라인 인자를 참고하세요.
				\n \n테스트중에는 로그인할 수 없습니다.*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_LOGIN_RUN_UNIT_TEXT2"));
			return 0;
		}
#endif

		if( UIPhaseLogin::m_bConnecting )
			return 0;

		g_pConfig->m_bNetworking = true;

	#if defined (LOCALE_INDONESIA) || defined (LOCALE_TURKEY)
		// 인도네시아만 아이디를 소문자로 변경해서 보냅니다!
		// 크레온에서 협의없이 아이디를 소문자로 바꿨기 때문입니다!
		const char* ID = i3Lua::GetStringArg( L, 1, nullptr, 0, "");

		char sID[ MAX_PATH] = {};
				
		i3::to_lower_copy(sID, ID);

//		::to_lower( sID, MAX_PATH, ID);
	#else
		const char* sID = i3Lua::GetStringArg( L, 1, nullptr, 0, "");	// UTF8
		I3ASSERT_RETURN(sID, 0);
	#endif
		
//		char szPW[ MAX_PATH] = {0,};
		const char*	sPW = i3Lua::GetStringArg( L, 2, nullptr, 0, "");	// UTF8

		// ID UTF-8 -> Multibyte로 변환
		i3::string strID;
		i3::utf8_to_mb(sID, strID);

		i3::string strPW;
		i3UIEditBox * pPwBox = GetThis()->GetPwBox();
		if( pPwBox && pPwBox->getPassword())
		{	
			// Password UTF-16 -> Multibyte로 변환
			i3::utf16_to_mb(pPwBox->getPassword(), strPW);
//			i3String::CopyWCHARtoASCII( szPW, sizeof( szPW), pPw->getPassword(), i3::generic_string_size( pPw->getText()));
		}

		bool bCheckSave = (i3Lua::GetBooleanArg( L, 3) == true);

		if( sPW != nullptr )
		{

			if( i3::generic_string_size( sID) <= 0)
			{
				GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOGIN_NONE_ID"), nullptr, MAKE_CALLBACK(UIPhaseLogin::CbPopupOK), GetThis());
				return 0;
			}
		
			// ID저장
			char szLog[MAX_STRING_COUNT] = "";
			i3::snprintf( szLog, MAX_STRING_COUNT, "/%s/Log.log", GAME_TITLE_NAME_A);

			if( UIPhaseLogin::SaveID( szLog, sID, bCheckSave) )
			{
				GameLoginContext::i()->setUserID( strID.c_str());
				GameLoginContext::i()->setUserPW( strPW.c_str());

				GetThis()->TryConnectLoginServer();

				UIPhaseLogin::m_bConnecting = true;
			}
		}

		return 0;
	}

	int UIPhaseLogin_ClickExitBtn( lua_State * L)
	{
		GetThis()->ClickExit();
		return 0;
	}

	int UIPhaseLogin_ClickFindIDBtn( lua_State * L)
	{
		GetThis()->ClickFindID();
		return 0;
	}

	int UIPhaseLogin_ClickFidnPwBtn( lua_State* L)
	{
		GetThis()->ClickFindPw();
		return 0;
	}

}	// extern "C"

LuaFuncReg PhaseLogin_Glue[] = 
{
	{"Glue_Join",				UIPhaseLogin_ClickJoinBtn},		// 가입 하기
	{"Glue_Login",				UIPhaseLogin_ClickLoginBtn},	// Login
	{"Glue_Exit",				UIPhaseLogin_ClickExitBtn},		// 게임 종료
	{"Glue_FindID",				UIPhaseLogin_ClickFindIDBtn},	// ID 찾기
	{"Glue_FindPW",				UIPhaseLogin_ClickFidnPwBtn},	// 비번찾기 

	{nullptr, nullptr}
};



void OnExitCB()
{
}

I3_CLASS_INSTANCE( UIPhaseLogin);//, UIPhaseBase);

bool UIPhaseLogin::m_bConnecting = false;

REAL32 UIPhaseLogin::m_rDelayWait = 0;

UIPhaseLogin::UIPhaseLogin()
{
	m_rConnectWaitTimeStamp = 0.f;
	m_bConnectStart = false;
	m_pmbLoginWait = nullptr;
	m_LoginError = false;
	m_LogInConnectFailedCount = 0;
	m_GameStartTime = 0.f;
	m_ForceExit = false;
	m_RNFlag = false;

	m_pIDBox = nullptr;
	m_pPwBox = nullptr;
	m_pImage_IDbox = nullptr;
	m_pImage_PWbox = nullptr;

	m_iWaitCount		= 0;
	m_rWaitTime			= 0.f;
	m_rDelayWait		= 0;
	m_pWaitText			= nullptr;

	//FCM_INFO_ACK 실명제 적용여부(Yes or No) 패킷 날라옴 by 하동익
	RegisterGameEventFunc( EVENT_RECEIVE_REALNAME,				&UIPhaseLogin::ReceiveGameEvent_RealName);
	RegisterGameEventFunc( EVENT_CONNECT_SERVER_GATEWAY,		&UIPhaseLogin::ReceiveGameEvent_Connect_Server_Gateway);
	RegisterGameEventFunc( EVENT_LOG_IN,						&UIPhaseLogin::ReceiveGameEvent_Login);
	RegisterGameEventFunc( EVENT_LOG_OUT,						&UIPhaseLogin::ReceiveGameEvent_Logout);
	RegisterGameEventFunc( EVENT_M_DRIVE_AWAY,					&UIPhaseLogin::ReceiveGameEvent_Drive_Away);
	RegisterGameEventFunc( EVENT_M_BLOCK_ACCOUNT,				&UIPhaseLogin::ReceiveGameEvent_Block_Account);
	RegisterGameEventFunc( EVENT_SHUTDOWN_TIME_LOGIN,			&UIPhaseLogin::ReceiveGameEvent_ShutDown_Time_Login);
	RegisterGameEventFunc( EVENT_M_ERROR,						&UIPhaseLogin::ReceiveGameEvent_Error);
	RegisterGameEventFunc( EVENT_M_GAMEGUARD_ERROR,				&UIPhaseLogin::ReceiveGameEvent_GameGaurd_Error);
	RegisterGameEventFunc( EVENT_M_DISCONNECTION,				&UIPhaseLogin::ReceiveGameEvent_Disconnection);
	RegisterGameEventFunc( EVENT_M_DESTROYED_NETWORK,			&UIPhaseLogin::ReceiveGameEvent_Destroyed_Network);
	RegisterGameEventFunc( EVENT_LOG_IN_WAIT,					&UIPhaseLogin::ReceiveGameEvent_Login_Wait);
}

UIPhaseLogin::~UIPhaseLogin()
{
	if( m_pmbLoginWait )
	{
		GameUI::CloseMsgBox( m_pmbLoginWait);
		m_pmbLoginWait = nullptr;
	}

	if( m_pWaitText )
	{
		GameUI::CloseMsgBox( m_pWaitText);
		m_pWaitText = nullptr;
	}

}

void UIPhaseLogin::__CheckTimeOut( void)
{
#if defined( I3_DEBUG)
	return;
#else

	if (GameLoginContext::i()->IsWebLogin()) return;

	// 강제종료 안내팝업을 띄웠다면 넘긴다.
	if ( m_ForceExit ) return;

	// 게임에서 로그인 처리를 할 경우처리.
	// 게임 실행 후 일정시간 이상 로그인을 안하고 있다면 사용자 입력이 있다면 강제 종료를 한다.
	// 업데이트를 빗겨나가는 문제때문이다.
	REAL32	ElapsedTime = g_pFramework->GetAccTime() - m_GameStartTime;
	if ( ElapsedTime > g_pConfigEx->GetNetwork().GameRunWaitTime )
	{
		I3TRACE1("LogIn idle time out : %f\n", ElapsedTime);
		GameUI::MsgBox( MSG_TYPE_SYSTEM_OK, GAME_RCSTRING("STBL_IDX_EP_NOT_LOGIN_TIMEOUT"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
		m_ForceExit = true;
	}
#endif
}

void UIPhaseLogin::__WaitConnectMessageBox(REAL32 rDeltaSeconds)
{
	if( !m_bConnectStart)
		return;
	m_rConnectWaitTimeStamp += rDeltaSeconds;

	if ( m_LoginError )
		return;

	if(m_RNFlag == false){
		if( m_rConnectWaitTimeStamp > CONNECTMESSAGE_WAIT_TIME )	// 1초
		{
			if(( m_pmbLoginWait == nullptr) && (m_pWaitText == nullptr))
			{
				m_pmbLoginWait = GameUI::MsgBox( MSG_TYPE_SYSTEM_WAIT, GAME_RCSTRING("STBL_IDX_EP_LOGIN_WAIT_CONNECT_W"));
			}
		}
	}
}

void UIPhaseLogin::__ResetWaitConnect( void)
{
	m_bConnectStart = false;
	m_rConnectWaitTimeStamp = 0.0f;

	if( m_pmbLoginWait)
	{
		GameUI::CloseMsgBox( m_pmbLoginWait);
		m_pmbLoginWait = nullptr;
	}

	if( m_pWaitText )
	{
		GameUI::CloseMsgBox( m_pWaitText);
		m_pWaitText = nullptr;
	}
}

void UIPhaseLogin::enable_sound()
{
	i3SoundContext * pSndCtx = g_pViewer->GetSoundContext();
	if( pSndCtx != nullptr)
		pSndCtx->SetTypeVolume( I3SND_PROP_TYPE_EFFECT, 60);
}

/*virtual*/ void UIPhaseLogin::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPhaseBase::_InitializeAtLoad( pScene);

	m_join_btn = (i3UIButton*) pScene->FindChildByName( "Btn_Join");
	m_login_btn = (i3UIButton*) pScene->FindChildByName( "Btn_Login");

	m_pIDBox = (i3UIEditBox*) pScene->FindChildByName( "EditBox_ID");
	I3ASSERT( m_pIDBox != nullptr);
	m_pIDBox->setAlphaNumericOnly(true);

	m_pImage_IDbox = (i3UIImageBox*) pScene->FindChildByName("i3UIImageBox1");
	I3ASSERT(m_pImage_IDbox);
	m_pImage_PWbox = (i3UIImageBox*) pScene->FindChildByName("i3UiImageBox3");
	I3ASSERT(m_pImage_PWbox);

	m_pImage_PWbox->SetVisible(false);
	m_pImage_PWbox->SetVisible(false);

#if defined(LOCALE_TURKEY)
	m_pIDBox->SetLetterLimit( 64);
#else
	m_pIDBox->SetLetterLimit( 16);	// NC 웹 기준 4~16
#endif

	m_pPwBox = (i3UIEditBox*) pScene->FindChildByName( "EditBox_PW");
	I3ASSERT( m_pPwBox != nullptr);
	m_pPwBox->setAlphaNumericOnly(true);

	m_pPwBox->SetLetterLimit( 16);

	i3UIFrameWnd* pLoginBackGround = (i3UIFrameWnd *) pScene->FindChildByName( "i3UIFrameWnd_Login");
	I3ASSERT( pLoginBackGround != nullptr);

	i3Texture * pTexture = g_pFramework->GetResourceManager()->LoadTexture("Gui/Loading/Login.i3i");
	I3ASSERT(pTexture);

	m_pBG = i3UIImageBoxEx::new_object();
	I3ASSERT( m_pBG != nullptr);
	m_pBG->AddTexture( pTexture, 1024.f, 576.f);

	I3_MUST_RELEASE(pTexture);

	m_pBG->CreateBaseSceneObjects();
	pLoginBackGround->AddChildToHead(m_pBG);
	m_pBG->SetCurrentImage(0);

	_Resize();

	char szLoadID[MAX_STRING_COUNT] = "";
	i3::snprintf( szLoadID, MAX_STRING_COUNT, "\\%s\\Log.log", GAME_TITLE_NAME_A);
	// ID 로드
	LoadID( szLoadID); 

#if defined(BUILD_RELEASE_QA_VERSION)
//	char id[ MAX_STRING_COUNT] = {0,};
//	WCHAR16 pw[ MAX_STRING_COUNT] = L"";

//	INT32 lengthW = i3::generic_string_size( m_pIDBox->getText());
	
	i3::string strID;
	i3::utf16_to_mb(m_pIDBox->getText(), strID);
//	i3String::CopyWCHARtoASCII( id, MAX_STRING_COUNT, m_pIDBox->getText(), lengthW);

	if( i3::contain_string_head( strID, "gptest") )		// if (i3::Contain() == 0) 이렇게 처리하면 너무 멍청한 루틴이다..
	{													// 리턴값이 1~n인것을 찾을 이유가 하나도 없다..그렇게 하지말것..(2013.05.16.수빈)
		m_pPwBox->SetText( m_pIDBox->getText());
	}
	else if( i3::contain_string_head( strID, "angel") )
	{
		m_pPwBox->SetText( L"aaaa1111");
	}
	else if( i3::contain_string_head( strID, "gametest") )
	{
	//	const char * pID = id;
		i3::const_char_range pID = i3::as_literal(strID);

		char szNumber[256];
		char szTemp[256];
		pID.advance_begin(8);	//	pID+=8;
		
	//	i3::string_ncopy_nullpad( szNumber, pID , lengthW - 8 );
		i3::generic_string_copy(szNumber, pID);

		sprintf_s( szTemp, "g4m3t3st%s", szNumber);
		
		i3::wstring wstrPW;
		i3::mb_to_utf16(szTemp, wstrPW);
	//	i3String::CopyASCIItoWCHAR( pw, szTemp);
		m_pPwBox->SetText( wstrPW.c_str());
	}
#endif

}

/*virtual*/ void UIPhaseLogin::_ClearAtUnload( i3UIScene * pScene)
{
	UIPhaseBase::_ClearAtUnload( pScene);

	if( g_pSndMng != nullptr)
	{
		g_pSndMng->StopBGM(0.5f);
	}
}

void UIPhaseLogin::_Init()
{
	GameLoginContext::i()->setWebLogin(false);

#if defined(LOCALE_RUSSIA) && defined(NC_BUILD)
	GameLoginContext::i()->setWebLogin(true);
#endif

#if defined(LOCALE_LATIN_AMERICA) && defined(NC_BUILD)
	GameLoginContext::i()->setWebLogin(true);
#endif
	
#if defined(LOCALE_NORTHAMERICA) && defined(NC_BUILD)
	if(SteamContext::i()->IsSteamUser())
		GameLoginContext::i()->setWebLogin(true);
#endif

#if defined(LOCALE_SINGMAL) && defined(NC_BUILD)
	GameLoginContext::i()->setWebLogin(true);
#endif

#if defined (LOCALE_PHILIPPINES) && defined(NC_BUILD)
	GameLoginContext::i()->setWebLogin(true);
#endif

#if defined(LOCALE_VIETNAM) && defined(NC_BUILD)
	GameLoginContext::i()->setWebLogin(true);
#endif

#if defined(LOCALE_JAPAN) && defined(NC_BUILD)
	GameLoginContext::i()->setWebLogin(true);
#endif

#if defined(LOCALE_KOREA) && defined(NC_BUILD)
	GameLoginContext::i()->setWebLogin(true);
#endif
	
	UINT8 SessionKey[NET_SESSION_KEY_SIZE];
	INT64 MacAddress;
	char strServerAddress[MAX_STRING_COUNT];

	if (_GetEnvSessKey(SessionKey) && _GetEnvMacAddr(&MacAddress)
		&& _GetEnvServerAddr(strServerAddress, MAX_STRING_COUNT))
	{
		GameLoginContext::i()->SetSessionKey(SessionKey);
		GameLoginContext::i()->SetMacAddress(&MacAddress);
		GameLoginContext::i()->setWebLogin(true);
	}

}

bool UIPhaseLogin::_GetEnvSessKey( UINT8 * pSessKey)
{
	I3ASSERT(pSessKey);

	char strBuf[NET_SESSION_KEY_SIZE];

	if (0 == ::GetEnvironmentVariable("SessKey", strBuf, NET_SESSION_KEY_SIZE))
	{
		return false;
	}

	// TODO: strBuf를 직접 저장한다. 32byte + 1byte(nullptr) - praptor to komet
	GameLoginContext::i()->SetSessionID( strBuf);

	char* desc;
	desc = (char*)pSessKey;
	*pSessKey = 0;

	char* ptr;
	ptr = strBuf;

	// 문자열 bytes값을 수치로 복원한다

	// 와 ..이거 매크로 때문에 왕창 망했었네요..... 16.07.04 이후로 이틀동안 친 커버리티 중에 젤 당황시런 버그...

	for(INT32 i = 0; i < NET_SESSION_KEY_SIZE / 2; i++)
	{
		char chByte;
		chByte = HexToInt(*ptr) * 16 + HexToInt(*(ptr + 1));
		ptr += 2;

		*desc = chByte;
		desc++;
	}

	return true;
}

bool UIPhaseLogin::_GetEnvMacAddr( INT64* pMacAddr)
{
	I3ASSERT(pMacAddr);

	char strBuf[MAX_STRING_COUNT];

	if (0 == ::GetEnvironmentVariable("MacAddr", strBuf, 256))
	{
		return false;
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

	return true;
}

bool UIPhaseLogin::_GetEnvServerAddr( char* strServerAddr, const size_t sizeServerAddrStrMax)
{
	char strBuf[MAX_STRING_COUNT];

	if (0 == ::GetEnvironmentVariable("ServerAddr", strBuf, 256))
	{
		return false;
	}

	i3::safe_string_copy(strServerAddr, strBuf, sizeServerAddrStrMax);

	return true;
}

bool UIPhaseLogin::OnRevive( i3RenderContext * pCtx)
{
	if( UIPhaseBase::OnRevive( pCtx) == false)
		return false;

	_Resize();
	return true;
}

//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//

/*virtual*/ void UIPhaseLogin::OnCreate( i3GameNode * pParent)
{
	UIPhaseBase::OnCreate( pParent);

	_Init();

	m_bConnecting = false;

	// UI Scene을 등록합니다.
	if( GameLoginContext::i()->IsWebLogin() == false )
	{
		AddScene("Scene/Main/PointBlankRe_Login.i3UIs", PhaseLogin_Glue);
	}

	enable_sound();
}

/*virtual*/ void UIPhaseLogin::OnUpdate( REAL32 rDeltaSeconds)
{
	UIPhaseBase::OnUpdate( rDeltaSeconds);

	UpdateWaitTime(rDeltaSeconds);

	//m_pExitMessageBox->isEnable()로 판단해도 되지만,,, open_exit_popup변수를 사용한 이유는
	//m_pExitMessageBox->isEnable() 함수는 평상시에도 호출된다. 그래서 exit popup 에 활성화 상태 후만 검사하기 위해 넣었다.
	if( open_exit_popup == true ) 
	{
	
		if( m_pExitMessageBox && m_pExitMessageBox->isEnable() == true )
		{
			//ExitMessageBox 를 활성화 하면서 인풋을 막았기에 여기서 자체 검사를 해야 된다.
			if( GameUI::IsKeyboardStroked(I3I_KEY_ENTER) == true )
			{
				m_pExitMessageBox->OnKey_Enter();
			}
		}
		else
		{
			open_exit_popup = false;
			UIBase::SetInputDisable(false);
		}
		
	}


#if defined(WINDOW_MODE_DISABLE)
	/*if (g_DidThisInactive)
	{
		 GOTO_EXIT_STAGE();
	}*/
#endif

	if (GameLoginContext::i()->IsWebLogin())
	{
		if (bFirst && i3Framework::STATE_SCENE == g_pFramework->GetState())
		{
			g_pConfig->m_bNetworking = true;
			GameEventSender::i()->SetEvent(EVENT_CONNECT_SERVER_GATEWAY);
			m_bConnectStart	= true;

			bFirst = false;	// 1회만 실행되도록
		}
	}

	__CheckTimeOut();
	__WaitConnectMessageBox( rDeltaSeconds);

	ProcessGameEvent( rDeltaSeconds);
}

/*virtual*/ bool UIPhaseLogin::OnEntranceStart( void * pArg1, void * pArg2)
{
	UIBase::OnEntranceStart(pArg1, pArg2);			// 아.....일단 리턴 스킵의 고의성을 모르므로 체크리턴여부를 모르겠다...커버리티에서 
													// intentional로 돌리겠음....어쨋거나...이렇게 짜면 안된다... false이면 리턴하거나, 
	open_exit_popup = false;						// 원래는 전체 컨텐츠 구현에서 가상함수구현시 부모 구현 물려받고 자식구현 덧붙이는거 자체를 하지를 말아야한다..

	m_GameStartTime = g_pFramework->GetAccTime();

	bFirst = true;
	m_bConnectStart = false;
	m_bConnecting = false;

	return true;
}

/*virtual*/ bool UIPhaseLogin::OnPlayClickSound(i3UIControl * pCtrl)
{
	if(i3::same_of<i3UIEditBox*>(pCtrl))
	{
		return PlaySound(UISND_LIST_OVER);
	}
	else if( pCtrl == m_join_btn )
	{
		return PlaySound(UISND_ITEM_CLICK_OK);
	}

	return UIPhaseBase::OnPlayClickSound(pCtrl);
}

void UIPhaseLogin::ProcessGameEvent( REAL32 rDeltaSeconds)
{	
	INT32 arg;
	GAME_EVENT nEvent;
	i3::user_data	UserData;

	while ( GameEventReceiver::i()->PopRecvData(nEvent, arg, UserData) )
	{	
		

		UIBase::ProcessGameEvent( nEvent, arg, UserData);

		i3::unordered_map< GAME_EVENT, call_func >::iterator it;
		it = m_EventFuncMap.find( nEvent);

		if ( it != m_EventFuncMap.end() )
		{
			call_func & mf = it->second;
			(this->*mf)( arg, UserData);
		}
	}
}

void UIPhaseLogin::TryConnectLoginServer( void)
{
	m_bConnectStart	= true;

	SetInputDisable( true);

	GameEventSender::i()->SetEvent(EVENT_CONNECT_SERVER_GATEWAY);
}


void UIPhaseLogin::OnClickLoginChina(void)
{

}

void UIPhaseLogin::OnClickLoginBtn( void)
{
	LuaState * L = _CallLuaFunction( "ClickLoginBtn");
	_EndLuaFunction( L, 0);
}

void UIPhaseLogin::OnLoginOK( void)
{
}

void UIPhaseLogin::OnLoginFailed( void)
{
}

void UIPhaseLogin::ClickExit( void)
{
	OnEscapeKey();

	m_pExitMessageBox->SetCBDocumentComplete(OnExitCB);
	open_exit_popup = g_pFramework->GetMessageBoxManager()->IsTopMsgBox(m_pExitMessageBox);
	if( open_exit_popup == true )
	{
		SetInputDisable(true);
	}
}

void UIPhaseLogin::ClickFindID( void)
{
	if( true != g_pConfigEx->GetMisc().FindIDAddress.empty())
	{
		//	ID 찾기 연결
		g_pFramework->OpenWebBrowser(g_pConfigEx->GetMisc().FindIDAddress.c_str(), nullptr, true);
	}
}

void UIPhaseLogin::ClickFindPw( void)
{
	// ID찾기와 동일하게 일단 연결해둠니당 .. 
	if( true != g_pConfigEx->GetMisc().FindIDAddress.empty())
	{
		g_pFramework->OpenWebBrowser(g_pConfigEx->GetMisc().FindIDAddress.c_str(), nullptr, true);
	}
}

/*virtual*/ bool UIPhaseLogin::OnKeyInput( i3InputDeviceManager * pInputMng)
{
	if( UIPhaseBase::OnKeyInput( pInputMng))
		return true;

	i3InputKeyboard* pKeyboard = pInputMng->GetKeyboard();
	I3ASSERT( pKeyboard != nullptr);

	// 문자가 입력된 상태이면 Edit Box의 농도가 진해져야 함
	if( m_pImage_PWbox)
	{
		if(m_pPwBox->getEditState() == EDIT_STATE_CHANGE_INPUT_CHAR ||
			m_pPwBox->getTextLength() > 0)
			m_pImage_PWbox->SetVisible(true);
		else
			m_pImage_PWbox->SetVisible(false);
	}

	if( m_pImage_IDbox)
	{
		if(m_pIDBox->getEditState() == EDIT_STATE_CHANGE_INPUT_CHAR 
			|| m_pIDBox->getTextLength() > 0)
			m_pImage_IDbox->SetVisible(true);
		else
			m_pImage_IDbox->SetVisible(false);
	}

	if( pKeyboard->GetStrokeState(I3I_KEY_TAB))
	{
		if( m_pIDBox->isFocused())
			g_pFramework->SetFocus( m_pPwBox);
		else if( m_pPwBox->isFocused())
			g_pFramework->SetFocus( m_pIDBox);
		else
			g_pFramework->SetFocus( m_pIDBox);

		return true;
	}

	return false;
}

/*virtual*/ bool UIPhaseLogin::OnKey_Enter( void)
{
	OnClickLoginBtn();

	return true;
}

bool UIPhaseLogin::OnKey_Escape()
{
	//UIPhaseBase::OnKey_Escape() //이 함수를 호출 하면 안된다. ClickExit() 함수가 호출하며 중복 호출이 된다.
	ClickExit();

	return false;
}

void UIPhaseLogin::OnCompleteLoad( void)
{
	if( m_pIDBox->isFocused())
	{
		g_pFramework->SetFocus( nullptr);
		g_pFramework->SetFocus( m_pIDBox);
	}
	else if( m_pPwBox->isFocused())
	{
		g_pFramework->SetFocus( nullptr);
		g_pFramework->SetFocus( m_pPwBox);
	}

}

bool UIPhaseLogin::SaveID( const char * pszPath, const char * pID, bool bChkSave)
{
	if( pszPath == nullptr)
		return false;

	
	CFileFind finder;
	i3FileStream FileStream;

	char	szTemp[MAX_STRING_COUNT] = {0,};		// adddata directory 
	char	szFile[MAX_STRING_COUNT] = {0,};		// full file directory
	char	szSaveDirectory[MAX_STRING_COUNT] = {0,};

	CString appdata = getWritableFolder();

	// 로그 삭제
	i3::safe_string_copy( szTemp, appdata, MAX_STRING_COUNT );
	i3::safe_string_copy( szFile, appdata, MAX_STRING_COUNT );
	i3::generic_string_cat( szFile, pszPath );

	// 만약 세이브할 디렉토리가 없다면 디렉토리를 생성합니다.
	i3::safe_string_copy( szSaveDirectory, appdata, MAX_STRING_COUNT );
	i3::generic_string_cat(szSaveDirectory, "\\");
	i3::generic_string_cat(szSaveDirectory, GAME_TITLE_NAME_A);	
	i3::generic_string_cat(szSaveDirectory, "\\*.*");

	BOOL bWorking = finder.FindFile(szSaveDirectory);
	if( bWorking == false)
	{
		i3mem::FillZero( szSaveDirectory, sizeof(char)*MAX_STRING_COUNT);

		i3::safe_string_copy( szSaveDirectory, appdata, MAX_STRING_COUNT );
		i3::generic_string_cat(szSaveDirectory, "\\");
		i3::generic_string_cat(szSaveDirectory, GAME_TITLE_NAME_A);	

		CreateDirectory( szSaveDirectory, nullptr);
	}

	if( !FileStream.Open( szFile, STREAM_WRITE))		// pszPath
	{
		if( !FileStream.Create( szFile, STREAM_WRITE))	// pszPath
		{
			return false;
		}
	}

	if( bChkSave)
	{
		FileStream.Write( pID, sizeof(char)*MAX_STRING_COUNT);
	}
	else
	{
		FileStream.Close();
		DeleteFile( szFile );
	}

	__setlogId( pID );
	g_pFramework->PrintVersion();

	return true;
}

void UIPhaseLogin::LoadID( const char * pszPath)
{
	if( pszPath == nullptr)
		return;

	CString appdata = getWritableFolder();
	char	szFile[MAX_STRING_COUNT] = {0,};
	char	szTemp[MAX_STRING_COUNT] = {0,};

	i3::safe_string_copy( szFile, appdata, MAX_STRING_COUNT );
	i3::generic_string_cat( szFile, pszPath );
	
	i3FileStream FileStream;
	if( !FileStream.Open( szFile, STREAM_READ | STREAM_SHAREREAD))
	{
		g_pFramework->SetFocus( m_pIDBox);
		return;
	}
	
	FileStream.Read( szTemp, MAX_STRING_COUNT);

	szTemp[MAX_STRING_COUNT-1] = 0;

	g_pFramework->SetFocus( m_pPwBox);

	LuaState* L = _CallLuaFunction( "LoadID");				// TODO : ID는 영문자로 일단 판단했기 때문에 딱히 wchar_t로 변경할지 미지수로 둔다. (2014.07.23.수빈)
	if( L != nullptr)																// 만약 변경해야한다면  파일 포맷 자체가 수정되어야한다.
	{
		i3Lua::PushString( L, szTemp);									// 영문자로 간주한다면 UTF-8로 간주해준다. (2014.07.23.수빈)
		i3Lua::PushInteger( L, 1);
		_EndLuaFunction( L, 2);
	}
}

void UIPhaseLogin::FocusToControl( void)
{
	m_bConnectStart = false;
	m_bConnecting = false;

	//i3UI::setDisableUIInput( false);
	SetInputDisable( false);
}

void UIPhaseLogin::SetPBLoginModule(bool Sig)
{
	LuaState * L = _CallLuaFunction("SetLoginModule");
	i3Lua::PushBoolean(L, Sig);
	_EndLuaFunction(L,1);
}

void UIPhaseLogin::EhNonID(void)
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOGIN_NONE_ID"), nullptr, MAKE_CALLBACK(CbPopupOK), this);
}

void UIPhaseLogin::EhNonPW(void)
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOGIN_NONE_PW"), nullptr, MAKE_CALLBACK(CbPopupOK), this);
}

void UIPhaseLogin::EhInvalidAccount(void)
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOGIN_INVALID_ACCOUNT"), nullptr, MAKE_CALLBACK(GetErrorHandler()), this);

	// Password 를 삭제 해줍니다.
	if( m_pPwBox != nullptr)
		m_pPwBox->SetText( L"");
}

void	UIPhaseLogin::EhInvalidID(void)
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOGIN_INVALID_ID"), nullptr, MAKE_CALLBACK(GetErrorHandler()), this);

	// Password 를 삭제 해줍니다.
	if( m_pPwBox != nullptr)
		m_pPwBox->SetText( L"");
}

void	UIPhaseLogin::EhInvalidPassword(void)
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOGIN_INVALID_PASSWORD"), nullptr, MAKE_CALLBACK(GetErrorHandler()), this);

	// Password 를 삭제 해줍니다.
	if( m_pPwBox != nullptr)
		m_pPwBox->SetText( L"");
}

void UIPhaseLogin::EhLogouting()
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOGIN_LOGOUTING"), nullptr, MAKE_CALLBACK(GetErrorHandler()), this);

	// Password 를 삭제 해줍니다.
	if( m_pPwBox != nullptr)
		m_pPwBox->SetText( L"");
}

void UIPhaseLogin::EhDBBufferFull()
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOGIN_DB_BUFFER_FULL"), nullptr, MAKE_CALLBACK(GetErrorHandler()), this);

	// Password 를 삭제 해줍니다.
	if( m_pPwBox != nullptr)
		m_pPwBox->SetText( L"");
}

void UIPhaseLogin::EhDeletedAccount()
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOGIN_DELETE_ACCOUNT"), nullptr, MAKE_CALLBACK(GetErrorHandler()), this);

	// Password 를 삭제 해줍니다.
	if( m_pPwBox != nullptr)
		m_pPwBox->SetText( L"");
}

void UIPhaseLogin::EhEmailAuthError()
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOGIN_EMAIL_AUTH_ERROR"), nullptr, MAKE_CALLBACK(GetErrorHandler()), this);

	// Password 를 삭제 해줍니다.
	if( m_pPwBox != nullptr)
		m_pPwBox->SetText( L"");
}

void UIPhaseLogin::EhBlockIP()
{
	if(GameLoginContext::i()->IsWebLogin())
	{
		GameUI::MsgBox(MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOGIN_BLOCK_IP"), nullptr, EXIT_CALLBACK(CbExitNormal, true), this);
	}
	else
	{
		GameUI::MsgBox(MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOGIN_BLOCK_IP"), nullptr, MAKE_CALLBACK(GetErrorHandler()), this);
	}


	// Password 를 삭제 해줍니다.
	if( m_pPwBox != nullptr)
		m_pPwBox->SetText( L"");
}

// 북미 Error Handler 추가 (2011.06.10 양승천)
void UIPhaseLogin::EhInvalidAccount(const i3::rc_wstring& wstrMsg)
{
	if(i3::generic_string_size(wstrMsg) > 0)
		GameUI::MsgBox( MSG_TYPE_ERROR, wstrMsg, nullptr, MAKE_CALLBACK(GetErrorHandler()), this);
	else
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOGIN_INVALID_ACCOUNT"), nullptr, MAKE_CALLBACK(GetErrorHandler()), this);
}

void UIPhaseLogin::EhInvalidCountry()
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOGIN_BLOCK_COUNTRY"), nullptr, MAKE_CALLBACK(GetErrorHandler()), this);
}


void UIPhaseLogin::EhAlreadyLogin(void)
{
#if defined(I3_DEBUG)
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOGIN_ALREADY_LOGIN_Q"), nullptr, MAKE_CALLBACK(CbAlreadyLogin), this);
#else
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOGIN_ALREADY_LOGIN_WEB_E"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
	Sleep(1000*10);
#endif

}

void UIPhaseLogin::EhTimeOut1(void)
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOGIN_TIME_OUT_1"), nullptr, MAKE_CALLBACK(GetErrorHandler()), this);
}

void UIPhaseLogin::EhTimeOut2(void)
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOGIN_TIME_OUT_2"), nullptr, MAKE_CALLBACK(GetErrorHandler()), this);
}

void UIPhaseLogin::EhBlockAccount(INT32 arg)
{
	i3::wstring	wstr = GAME_RCSTRING("STBL_IDX_EP_LOGIN_BLOCK_ACCOUNT");

	if ( arg != 0 )
	{
		wstr += i3::format_string(L"(error:%d)", arg);
	}

	GameUI::MsgBox( MSG_TYPE_ERROR, wstr, nullptr, MAKE_CALLBACK(GetErrorHandler()), this);
}

void UIPhaseLogin::EhUnknownLogin(INT32 Event,INT32 arg)
{
#if defined( I3_DEBUG)//에러 핸들러 관련 정의되지 않은 에러코드

	i3::rc_wstring wstrBuf;
	i3::sprintf(wstrBuf, L"%s Event(%d), arg(0x%x)\n", GAME_RCSTRING("STBL_IDX_EP_LOGIN_UNKNOWN"), Event, arg);

	GameUI::MsgBox( MSG_TYPE_GAME_OK, wstrBuf, nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
#else
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOGIN_UNKNOWN"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
#endif
}

void UIPhaseLogin::EhInventoryFail(void)
{
	/*창고 목록을 받아오는데 실패하였습니다.\n창고 목록이 없으면 게임을 진행할 수 없어 종료합니다.*/
	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_LOGIN_FAIL_GET_FROM_STORE_HOUSE"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
}

void UIPhaseLogin::EhWaitDisconnect(void)
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOGIN_WAIT_DISCONNECT"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
}

void UIPhaseLogin::EhFailConnect(void)
{
	if ( m_LogInConnectFailedCount++ < g_pConfigEx->GetNetwork().ServerReconnectCount )
	{	// try reconnect
		TryConnectLoginServer();
	}
	else
	{	// Connect fail message
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOGIN_FAIL_CONNECT_E"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
	}
}

void UIPhaseLogin::EhStageError(INT32 Event,INT32 arg)
{
	//I3ASSERT_0;	// ! 현재로선 들어올 Error가 없습니다. 들어오면 arg를 알려주세요. - praptor
	i3::rc_wstring wstr_bug;
	i3::sprintf(wstr_bug, L"ERROR: Event(%d), arg(0x%x)", Event, arg);

	GameUI::MsgBox( MSG_TYPE_GAME_OK, wstr_bug);
}

void UIPhaseLogin::EhBlockGame(INT32 arg)
{
	arg = arg & EVENT_ERROR_EVENT_LOG_IN_BLOCK_MASK;

	// 정보가 없다면 일반 블럭 메시지 출력
	if (0 == arg)
	{
		EhBlockAccount(arg);
		return;
	}

	// 에러 메시지 출력 우선순위는 하위 비트 순
	for(UINT32 i = 0, nBit = 0x00000001; i <= 13; i++, nBit = nBit << 1)
	{
		if (arg & nBit)
		{
			char szTemp[256];

			_snprintf(szTemp, 256, "STBL_IDX_BLOCK_GAME_%d", i);
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING(szTemp), nullptr, EXIT_CALLBACK(CbExitNormal,true), this, 8);
			return;
		}
	}
}

void UIPhaseLogin::EhBlockInner(INT32 arg)
{
	arg = arg & EVENT_ERROR_EVENT_LOG_IN_BLOCK_MASK;

	// 정보가 없다면 일반 블럭 메시지 출력
	if (0 == arg)
	{
		EhBlockAccount(arg);
		return;
	}

	// 에러 메시지 출력 우선순위는 하위 비트 순
	for(UINT32 i = 0, nBit = 0x00000001; i <= 4; i++, nBit = nBit << 1)
	{
		if (arg & nBit)
		{
			char szTemp[256];
			_snprintf(szTemp, 256, "STBL_IDX_BLOCK_INNER_%d", i);
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING(szTemp), nullptr, nullptr, nullptr, 8);
			return;
		}
	}
}

void UIPhaseLogin::EhBlockOuter(INT32 arg)
{
	arg = arg & EVENT_ERROR_EVENT_LOG_IN_BLOCK_MASK;

	// 정보가 없다면 일반 블럭 메시지 출력
	if (0 == arg)
	{
		EhBlockAccount(arg);
		return;
	}

	// 에러 메시지 출력 우선순위는 하위 비트 순
	for(UINT32 i = 0, nBit = 0x00000001; i <= 5; i++, nBit = nBit << 1)
	{
		if (arg & nBit)
		{
			char szTemp[256];
			_snprintf(szTemp, 256, "STBL_IDX_BLOCK_OUTER_%d", i);
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING(szTemp), nullptr, nullptr, nullptr, 8);
			return;
		}
	}
}

ErrMessageHandler	UIPhaseLogin::GetErrorHandler()
{
	if (GameLoginContext::i()->IsWebLogin())
	{
		return CbExitNormal;
	}

	return CbPopupOK;
}

void UIPhaseLogin::CbNation(void* pThis, INT32 nParam)
{
	if (nParam == MBR_OK)
	{
		// Focus를 돌려준다.
		((UIPhaseLogin*)pThis)->FocusToControl();
		i3Stage * pStage = g_pFramework->GetCurrentStage();
		I3ASSERT(i3::same_of<UILogin*>(pStage));
		((UILogin*)pStage)->MovePhase(LOGIN_UIPHASE_NATION);
	}
}


void UIPhaseLogin::CbPopupOK(void* pThis,INT32 nParam)
{
	// Focus를 돌려준다.
	((UIPhaseLogin*)pThis)->FocusToControl();
}

void UIPhaseLogin::CbFailSaveID(void* pThis,INT32 nParam)
{
	((UIPhaseLogin*)pThis)->TryConnectLoginServer();
	// Focus를 돌려준다.
	((UIPhaseLogin*)pThis)->FocusToControl();
}

void UIPhaseLogin::CbAlreadyLogin(void* pThis,INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		((UIPhaseLogin*)pThis)->EhWaitDisconnect(); // EVENT_LOG_OUT 활성화 전까지 임시
		// Focus를 돌려준다.
		((UIPhaseLogin*)pThis)->FocusToControl();
	}
}
void UIPhaseLogin::UpdateWaitTime(REAL32 rdeltaSeconds)
{
	if( m_rDelayWait > 0)
		m_rDelayWait -= rdeltaSeconds;

	if((m_pWaitText != nullptr) && (m_rWaitTime > 0))
	{
		INT32 hour, minute, second, itemp = 0;
		hour = INT32( (m_rWaitTime)/3600.f );
		itemp =(INT32)(m_rWaitTime)%3600;
		minute = itemp/60;
		second = itemp%60;
 
	
		i3::stack_wstring TempText1;
		i3::stack_wstring TempText2;
		i3::stack_wstring TempText3;
		i3::stack_wstring TempText4;
		i3::stack_wstring TempText5;

		m_rWaitTime -= rdeltaSeconds;

		i3::sprintf( TempText1, GAME_RCSTRING("STBL_IDX_LOGINWAIT_CURRENTLY_RANK"), m_iWaitCount);	//현재 대기 순위 : %d \n 

		if(hour > 0)
			i3::sprintf( TempText2, GAME_RCSTRING("STBL_IDX_LOGINWAIT_CURRENTLY_HR"), hour);
		if(minute > 0)
			i3::sprintf( TempText3, GAME_RCSTRING("STBL_IDX_LOGINWAIT_CURRENTLY_MIN"), minute);
		if(second > 0)
			i3::sprintf( TempText4, GAME_RCSTRING("STBL_IDX_LOGINWAIT_CURRENTLY_SEC"), second);
		else
			i3::sprintf( TempText4, GAME_RCSTRING("STBL_IDX_LOGINWAIT_CURRENTLY_SEC"), 0);

		i3::sprintf( TempText5, GAME_RCSTRING("STBL_IDX_LOGINWAIT_FORECAST_TIME"), TempText2, TempText3, TempText4);	//예상 대기시간: %s %s %s

		i3::rc_wstring wstrMessageText;
		i3::sprintf( wstrMessageText, L"%s\n%s%s", GAME_RCSTRING("STBL_IDX_LOGINWAIT_TITLE"), TempText1, TempText5);

		m_pWaitText->ChangeMessage( wstrMessageText, nullptr);
	}
}

void UIPhaseLogin::CbWaitOK(void* pThis,INT32 nParam)
{
	((UIPhaseLogin*)pThis)->m_pWaitText = nullptr;

	if( GameLoginContext::i()->IsWebLogin() == true)
	{
		GameUI::MsgBox( MSG_TYPE_SYSTEM_OK, GAME_RCSTRING("STBL_IDX_LOGINWAIT_EXIT"), nullptr, EXIT_CALLBACK(CbExitNormal, true), pThis);
	}
	else
	{
		((UIPhaseLogin*)pThis)->m_rDelayWait = 3;
		((UIPhaseLogin*)pThis)->m_RNFlag = true;
		CGameNetworkManager::i()->DisconnectTCP();
		((UIPhaseLogin*)pThis)->FocusToControl();
	}
}

void UIPhaseLogin::WaitMsgbox(INT32 count, REAL32 time)
{
	if(m_pWaitText == nullptr)
	{
		if( m_pmbLoginWait != nullptr)
		{
			GameUI::CloseMsgBox( m_pmbLoginWait);
			m_pmbLoginWait = nullptr;
		}

		m_pWaitText = GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("asdf"), nullptr, MAKE_CALLBACK(CbWaitOK), this);
	}
	m_iWaitCount = count;
	m_rWaitTime = static_cast<REAL32>(time);
}

void UIPhaseLogin::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert( i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UIPhaseLogin::ReceiveGameEvent_RealName( INT32 arg, const i3::user_data&)
{

}

void UIPhaseLogin::ReceiveGameEvent_Connect_Server_Gateway( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg))
	{
		NetworkContext::i()->ChangeSocket();

#ifdef JUST_RENDEZVOUS_SERVER
		GameEventSender::i()->SetEvent( EVENT_RENDEZVOUS_CONNECT );
#else
		if (GameLoginContext::i()->IsWebLogin())
		{
			// Steam빌드면 내부에서 EVENT_LOG_IN_WEBKEY 를 처리합니다
			if (!SteamContext::i()->RequestLoginTicket())
				GameEventSender::i()->SetEvent(EVENT_LOG_IN_WEBKEY);
		}
		else
		{
			GameEventSender::i()->SetEvent(EVENT_LOG_IN);
		}
#endif	// JUST_RENDEZVOUS_SERVER
	}
	else
	{
		__ResetWaitConnect();
		if( EV_IS_ERROR(arg, EVENT_ERROR_FAIL))
			EhFailConnect();
		else
			EhStageError( EVENT_CONNECT_SERVER_GATEWAY,arg);
	}
}

void UIPhaseLogin::ReceiveGameEvent_Login( INT32 arg, const i3::user_data& UserData)
{
	if( EV_SUCCESSED( arg))
	{
		//	2011-03-07 중국 실명제
		//  중국이 아닌 국가는 다음 스테이지, 중국은 실명제 정보 ACK올때까지 대기
		I3PRINTLOG(I3LOG_NOTICE,"Event Login Success.");
		if( m_pWaitText != nullptr)
		{
			GameUI::CloseMsgBox( m_pWaitText);
			m_pWaitText = nullptr;
		}

		CGameFramework::JumpToReady();

		//
		// 콘솔사용처리 - Distribute버전에서는 인증된 유저만가능합니다.
		//
		{
			//콘솔사용가능한 유저인지 확인후처리
		
#if defined(BUILD_RELEASE_QA_VERSION) || defined(DEBUG)
			static const bool confirmed = true;
#else
			bool confirmed = UserInfoContext::i()->CheckSpecialAbility(SA_CHEAT)!=false;
#endif
			//콘솔 실제생성은 첫번째 콘솔열기키(ctrl+')를 눌렀을떄 생성됩니다.
			//콘솔의사용이 전혀없는 경우(라이브 유저) 어떠한 처리도 하지않기 위함입니다.
			extern i3ClassMeta* GetPBConsoleClientCtxMeta();
			i3UIManager::InitConsole(confirmed ? true : false, GetPBConsoleClientCtxMeta());
			if( confirmed == true)
				PBConsoleManager::CreateInstance();
		}

	}
	else
	{
		I3PRINTLOG(I3LOG_NOTICE,"Event Login Fail.");
		__ResetWaitConnect();

		if ( IsLogInBlockError(arg) )
		{
			UINT32 ArgHead = arg & EVENT_ERROR_EVENT_LOG_IN_BLOCK_HEADER_MASK;
			switch (ArgHead)
			{
			case EVENT_ERROR_EVENT_LOG_IN_BLOCK_INNER:		EhBlockInner(arg);		break;
			case EVENT_ERROR_EVENT_LOG_IN_BLOCK_OUTER:		EhBlockOuter(arg);		break;
			case EVENT_ERROR_EVENT_LOG_IN_BLOCK_GAME:		EhBlockGame(arg);		break;
			}
		}
		else
		{
			switch (arg & EVENT_ERROR_MASK_FAIL)
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
			case EVENT_ERROR_EVENT_LOG_IN_CUSTOM_MESSAGE: 
				EhInvalidAccount(i3::value_cast<RecvData_EVENT_LOG_IN>(UserData).wstrCustomMsg ); break;
			case EVENT_ERROR_EVENT_LOG_IN_MAXUSER:
				GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_SERVER_USER_FULL_C"), nullptr, MAKE_CALLBACK( CbPopupOK), this);
				break;

			case EVENT_ERROR_EVENT_LOG_IN_UNKNOWN:
			case EVENT_ERROR_EVENT_LOG_IN_HTTP:
			case EVENT_ERROR_LOGIN_BREAK_SESSION:
			default:
				EhUnknownLogin( EVENT_LOG_IN, arg);
				break;
			}
		}
#if 0
		if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_LOG_IN_ALEADY_LOGIN))			EhAlreadyLogin();
		else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_LOG_IN_INVALID_ACCOUNT))	EhInvalidAccount();
		else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_LOG_IN_ID_FAIL))			EhInvalidID();
		else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_LOG_IN_PW_FAIL))			EhInvalidPassword();
		else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_LOGOUTING))					EhLogouting();
		else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_LOG_IN_TIME_OUT1))			EhTimeOut1();
		else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_LOG_IN_TIME_OUT2))			EhTimeOut2();
		else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_LOG_IN_BLOCK_ACCOUNT))		EhBlockAccount();
		else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_LOG_IN_UNKNOWN))			EhUnknownLogin(nEvent, arg);
		else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_LOG_IN_HTTP))				EhUnknownLogin(nEvent, arg);
		else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_LOG_IN_USER_ITEM_FAIL))		EhInventoryFail();
		else if( EV_IS_ERROR(arg, EVENT_ERROR_LOGIN_BREAK_SESSION))				EhUnknownLogin(nEvent, arg);
		else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_LOG_IN_MAXUSER))			GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_SERVER_USER_FULL_C"));
		else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_LOG_IN_BLOCK_GAME))			EhBlockGame(arg);
		else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_LOG_IN_BLOCK_INNER))		EhBlockInner(arg);
		else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_LOG_IN_BLOCK_OUTER))		EhBlockOuter(arg);
		else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_LOG_IN_DB_BUFFER_FULL))		EhDBBufferFull();
		else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_LOG_IN_DELETE_ACCOUNT))		EhDeletedAccount();
		else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_LOG_IN_EMAIL_AUTH_ERROR))		EhEmailAuthError();
		else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_LOG_IN_BLOCK_IP))		EhBlockIP();
		else EhStageError( EVENT_LOG_IN,arg);
#endif

		GameEventSender::i()->SetEvent(EVENT_DISCONNECT);
	}
}

void UIPhaseLogin::ReceiveGameEvent_Block_Account( INT32 arg, const i3::user_data&)
{
	if ( GMAuthorityContext::i()->GetBlockDate() == BLOCK_TIME_LIMIT)
	{
		GameUI::MsgBox( MSG_TYPE_SYSTEM_OK, GAME_RCSTRING("STR_GM_POPUP_BANMESSAGE"), nullptr, EXIT_CALLBACK(nullptr,true), this);
	}
	else
	{
		UINT32 _GapSecond = GMAuthorityContext::i()->GetBlockDate();

		INT32 _Second = _GapSecond%60;
		INT32 _Minute = (_GapSecond/60)%60;
		INT32 _Hour = ((_GapSecond/60)/60)%24;
		INT32 _Day = ((_GapSecond/60)/60)/24;

		i3::rc_wstring wstrblockMessage;
		i3::sprintf(wstrblockMessage, GAME_RCSTRING("STBL_IDX_EP_GAME_INVALID_TIME"), _Day, _Hour, _Minute, _Second);

		GameUI::MsgBox( MSG_TYPE_BLOCK_NOTICE, wstrblockMessage, nullptr, EXIT_CALLBACK(nullptr,true), this);
	}

	GameEventSender::i()->SetEvent(EVENT_DISCONNECT);
}

void UIPhaseLogin::ReceiveGameEvent_ShutDown_Time_Login( INT32 arg, const i3::user_data&)
{
	GameUI::MsgBox( MSG_TYPE_SYSTEM_OK, GAME_RCSTRING("STR_SHUTDOWN_POPUP_ACCESSPROHIBITION"), nullptr, EXIT_CALLBACK(nullptr,true), this);
}

void UIPhaseLogin::ReceiveGameEvent_Logout( INT32 arg, const i3::user_data&)
{
	__ResetWaitConnect();

	// 성공, 실패 공통
	EhWaitDisconnect();

	if( EV_FAILED( arg))
	{
		EhStageError( EVENT_LOG_OUT,arg);
	}
}

void UIPhaseLogin::ReceiveGameEvent_Drive_Away( INT32 arg, const i3::user_data& UserData)
{
	__ResetWaitConnect();

	UIPhaseBase::ReceiveGameEvent_DriveAway(arg, UserData);
}

void UIPhaseLogin::ReceiveGameEvent_Error( INT32 arg, const i3::user_data&)
{
	__ResetWaitConnect();
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_GAME_EXIT_ASSERT_E"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
}

void UIPhaseLogin::ReceiveGameEvent_GameGaurd_Error( INT32 arg, const i3::user_data&)
{
	__ResetWaitConnect();
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_GAMEGUARD_ERROR"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
}

void UIPhaseLogin::ReceiveGameEvent_Disconnection( INT32 arg, const i3::user_data&)
{
	__ResetWaitConnect();

	switch(arg)
	{
	case EVENT_ERROR_LOGIN_INVALID_VERSION:
		{
			GameUI::MsgBox( MSG_TYPE_ERROR, L"Invalid Version. Please check your client.", nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
		}
		break;
	default:
		{
			GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_GAME_EXIT_ASSERT_E"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
		}
		break;
	}
}

void UIPhaseLogin::ReceiveGameEvent_Destroyed_Network( INT32 arg, const i3::user_data&)
{
	// 네트워크가 끊어졌다
	__ResetWaitConnect();
	EhFailConnect();
}

void UIPhaseLogin::ReceiveGameEvent_Login_Wait( INT32 arg, const i3::user_data& UserData)
{
	const RecvData_EVENT_LOG_IN_WAIT& data = i3::value_cast<RecvData_EVENT_LOG_IN_WAIT>(UserData);
	WaitMsgbox(data.WaitCount, (REAL32)data.WaitLoginTime);
}

void UIPhaseLogin::ReceiveGameEvent_Unknown( GAME_EVENT evt, INT32 arg)
{
	__ResetWaitConnect();
	GameEventSender::i()->SetEvent(EVENT_DISCONNECT);
	
	// 처리못한 EVENT가 존재합니다.
	i3::rc_wstring wstr_bug;
	i3::sprintf( wstr_bug, L"EVENT_ERROR(0x%x)", evt);
	GameUI::MsgBox( MSG_TYPE_GAME_OK, wstr_bug);
}
