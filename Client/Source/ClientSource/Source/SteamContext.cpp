#include "pch.h"
#include "SteamContext.h"
#include "UI/UIServerAndChannelBanner.h"
#include "UI/UIUtil.h"
#include "../Steam/include/steam_api.h"

SteamContext::SteamContext() : m_bRecvSteamCallback(false), m_pSteamUser(nullptr), m_bEnableSteam(false), m_CallbackGameOverlayActivated(this, &SteamContext::OnGameOverlayActivated)
{
	m_hSteamDll = LoadLibrary(_T("steam_api.dll"));

	if (m_hSteamDll)
	{
		m_CallbackGameOverlayActivated.SetDLL(m_hSteamDll);
		m_CallbackGameOverlayActivated.Register(this, &SteamContext::OnGameOverlayActivated);

		I3PRINTLOG(I3LOG_NOTICE, "[STEAM] Steam_API.dll okay");
	}
	else
	{
		I3PRINTLOG(I3LOG_NOTICE, "[STEAM] Steam_API.dll Load Fail");
	}
}

SteamContext::~SteamContext()
{
	if (m_hSteamDll)
	{
		m_CallbackGameOverlayActivated.Unregister();
		m_CallbackGameOverlayActivated.SetDLL(nullptr);
		FreeLibrary(m_hSteamDll);
	}
}

bool SteamContext::IsSteamUser()
{
	if (!m_hSteamDll)
		return false;

	m_pSteamUser = (ISteamUser*(*)())GetProcAddress(m_hSteamDll, "SteamUser");
	if (!m_pSteamUser)
		return false;

	return true;
}

bool SteamContext::SteamAPI_Init(LPTSTR lpCmdLine)
{
	int iLocaleCode = atoi(lpCmdLine);

	I3PRINTLOG(I3LOG_NOTICE, "lpCmdLine : %s", lpCmdLine);

	// Cmdline이 일치하지 않는다면 DLL이 있어도 스팀빌드로 처리하지 않습니다.
	if (!(iLocaleCode > LOCALE_CODE_NONE && iLocaleCode < LOCALE_CODE_MAX))
	{
		I3PRINTLOG(I3LOG_NOTICE, "[STEAM] LocaleCod Error");
		m_hSteamDll = false; //위의 주석을 이유로, 뒤에 로그인시 관리자 권한을 체크하기 때문에 dll도 꺼준다
		return true;
	}

	// DLL 존재여부 검사
	// 이 경우에서 steam_api.dll 이 없다면 스팀빌드가 아니기 때문에
	// 일반 빌드로 진행하기 위해서 true를 반환합니다.
	if (!m_hSteamDll)
	{
		I3PRINTLOG(I3LOG_NOTICE, "[STEAM] SteamDll did not Load");
		return true;
	}

	// CmdLine도 일치. DLL도 존재한다면 반드시 SteamBuild 이기 때문에 조건 불충족시 게임을 종료시켜야합니다.
	bool(*pSteam_Init)();
	pSteam_Init = (bool(*)())GetProcAddress(m_hSteamDll, "SteamAPI_Init");

	if (!pSteam_Init)
	{
		I3PRINTLOG(I3LOG_NOTICE, "[STEAM] pSteam_Init Fail");
		return false;
	}

	// SteamAPI_init() 함수 실행.
	// 스팀런처 미구동 시 false 를 받습니다. 
	// Dev빌드의 경우 steam_appid.txt 도 존재해야 true가 반환됩니다.
	m_bEnableSteam = (*pSteam_Init)();

	I3PRINTLOG(I3LOG_NOTICE, "[STEAM] m_bEnableSteam %d", m_bEnableSteam);
	return m_bEnableSteam;
}

// 특별히 큰 의미는 없지만 RequestEncryptedAppTicket에 사용됩니다.
uint32 k_unSecretData = 0x5444;

void SteamContext::SteamAPI_RunCallbacks()
{
	void(*pSteam_RunCallback)();

	pSteam_RunCallback = (void(*)())GetProcAddress(m_hSteamDll, "SteamAPI_RunCallbacks");
	if (pSteam_RunCallback)
	{
		(*pSteam_RunCallback)();
	}
}

bool SteamContext::RequestLoginTicket()
{
	if (!m_hSteamDll)
		return false;

	if (!m_bEnableSteam)
	{
		ShowWindow(g_hWnd, SW_HIDE);

		//Msg 박스 출력
		//Run STEAM client as administratorPlease run STEAM client as administrator
		::MessageBox(NULL, "Please run STEAM client as an administrator.", "Error", MB_OK);
		I3PRINTLOG(I3LOG_NOTICE, "[STEAM] Login Error : Not Enable Steam");

		GameEventSender::i()->SetEventOnce(EVENT_GAME_END);
		g_pFramework->GotoNextStage("Exit");

		return true;
		//
	}

	// SteamUser 정보 검사
	ISteamUser* (*pSteamUser)(void);
	pSteamUser = (ISteamUser*(*)())GetProcAddress(m_hSteamDll, "SteamUser");

	if (!pSteamUser)
		return false;

	//I3PRINTLOG(I3LOG_NOTICE, "[STEAM] Login : %x", pSteamUser);

	//// AppTicekt 요청하는 CallBack 등록
	SteamAPICall_t cbRequestAppTicket = (*pSteamUser)()->RequestEncryptedAppTicket(&k_unSecretData, sizeof(k_unSecretData));
	m_EncryptedTicketResult.Set(cbRequestAppTicket, this, &SteamContext::OnRequestEncryptedAppTicket, m_hSteamDll);

	// 콜백 Recv 여부.
	m_bRecvSteamCallback = false;

	// 약 1초에 한번씩 약 10회 시도합니다. 
	for (int i = 0; i < 10; ++i)
	{
		// 반드시 약간의 딜레이 후 호출되어야 합니다. 
		// SteamAPI_RunCallbacks() 을 호출한 후, Callback 결과값이 있어야 OnRequestEncryptedAppTicket이 실행되기 때문입니다.
		Sleep(1000);
		// SteamAPI_RunCallbacks() 호출. 
		SteamAPI_RunCallbacks();

		// 결과상관없이 콜백을 받았다면 escape. 
		if (m_bRecvSteamCallback)
			break;
	}

	// 콜백을 받지 못했다면 클라이언트 종료합니다
	// steam_api.dll이 존재함에도 불구하고 Steam 로그인 자체에 실패했다는 의미이기 때문입니다.
	if (!m_bRecvSteamCallback)
	{
		I3PRINTLOG(I3LOG_NOTICE, "[STEAM] Login Error : cb"); 
		GameEventSender::i()->SetEventOnce(EVENT_GAME_END);
		g_pFramework->GotoNextStage("Exit");
		return false;
	}

	return true;
}

void SteamContext::OnGameOverlayActivated(GameOverlayActivated_t *pOverlay)
{
	bool bOverlay = pOverlay->m_bActive ? true : false;

	UIServerAndChannelBanner * pBanner = static_cast<UIServerAndChannelBanner*>(GameUI::GetFloating(UFW_SCBANNER));
	if(pBanner != nullptr) 	pBanner->ShowHideWebAunnounce(!bOverlay);
}

void SteamContext::OnRequestEncryptedAppTicket(EncryptedAppTicketResponse_t *pEncryptedAppTicketResponse, bool bIOFailure)
{
	if (bIOFailure)
		return;

	// 결과가 어찌되었건 콜백을 받았으니 true로 바꿔줌
	m_bRecvSteamCallback = true;

	if (pEncryptedAppTicketResponse->m_eResult == k_EResultOK)
	{
		GameEventSender::i()->SetEvent(EVENT_LOG_IN_WEBKEY);
	}
	else 
	{
		// 콜백의 결과값이 성공이 아닌 경우입니다.
		// 아래 주석을 해제하여 원인을 알 수 있습니다.
		//{
		//	if (!m_hSteamDll)
		//		return;

		//	ISteamUtils* (*pSteamUtils)(void);
		//	pSteamUtils = (ISteamUtils*(*)())GetProcAddress(m_hSteamDll, "SteamUtils");
		//	if (!pSteamUtils)
		//		return;

		//	ESteamAPICallFailure result = (*pSteamUtils)()->GetAPICallFailureReason(m_cbRequestAppTicket);
		//	
		//	if (result == k_ESteamAPICallFailureSteamGone)
		//	{
		//		// the local Steam process has gone away
		//		// . . . "isteamutils.h"  을 열어서 확인할 수 있습니다.
		//	}
		//}

		I3TRACE("[STEAM] Enc. Login ERROR");
		GameEventSender::i()->SetEventOnce(EVENT_GAME_END);
		g_pFramework->GotoNextStage("Exit");
	}
}

ISteamUser* SteamContext::GetSteamUser()
{
	return (*m_pSteamUser)();
}

bool SteamContext::SetAchievement(const char* NameKey)
{
	if (!m_hSteamDll)
		return false;

	ISteamUserStats* (*pSteam_UserStats)(void); 

	pSteam_UserStats = (ISteamUserStats*(*)())GetProcAddress(m_hSteamDll, "SteamUserStats");
	if (!pSteam_UserStats)
		return false;

	return (*pSteam_UserStats)()->SetAchievement(NameKey);
}

bool SteamContext::ActivateWebPage(const char* strURL)
{
	if (!m_hSteamDll)
		return false;

	ISteamFriends* (*pSteam_ActivateWebPage)(void);

	pSteam_ActivateWebPage = (ISteamFriends*(*)())GetProcAddress(m_hSteamDll, "SteamFriends");
	if ( !pSteam_ActivateWebPage )
		return false;

	(*pSteam_ActivateWebPage)()->ActivateGameOverlayToWebPage(strURL);

	return true;
}