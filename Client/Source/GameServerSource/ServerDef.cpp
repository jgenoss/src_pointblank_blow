#include "pch.h"
#include "ggsrv30.h"
#include "ServerDef.h"
#include "DataBase.h"
#include "ServerContext.h"
#include "UdpHolePunching.h"
#include "UserManager.h"
#include "RoomManager.h"
#include "ModuleControl.h"
#include "ModuleTrans.h"
#include "ModuleClan.h"
#include "ModuleMessenger.h"
#include "ModuleContextNc.h"

char			g_strCCurrentPath[]		= {0,}; 
SERVER_STATE	g_eServerState			= SERVER_STATE_NONE;
UINT32			g_ui32StartTime			= 0;

UINT8	g_ui8ConBuildVerMonth	= 0;
UINT8	g_ui8ConBuildVerDay		= 0;

INT32	g_ai32trainingKillPoint[TRAINING_DIFFICULTY_LEVEL_HIGH + 1] =   {0, 5, 7, 10, 15, 20, 25, 35, 45, 60, 80};

INT8	g_ai8SlotSequenceforOdd[SLOT_MAX_COUNT] = { 1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14 };
INT8	g_ai8SlotSequenceforEven[SLOT_MAX_COUNT] = { 1, 0, 3, 2, 4, 5, 7, 6, 8, 9, 11, 10, 12, 13, 15, 14 };

static UINT32		s_LastLogUpdateTime_State		= 0; 
static UINT32		s_LastLogUpdateTime_Channel		= 0; 
static UINT32		s_LastLogUpdateTime_GameGuard	= 0; 

static HQUERY		s_hCpuQuery = NULL;
static HCOUNTER     s_hCpuCounter;

static HQUERY		s_hHandleQuery = NULL;
static HCOUNTER     s_hHandleCounter;

#ifdef USE_AHNHS
AHNHS_SERVER_HANDLE	g_hAhnhsServer = NULL;
#endif
#ifdef USE_XTRAP
unsigned char		g_XTrapMap[ CSFILE_NUM ][ XTRAP_CS4_BUFSIZE_MAP ] ;	// Map File Store
CXTrapModule*		g_pXTrapModule = NULL;
#endif
#ifdef USE_GPK
SGPK::IGPKSvrDynCode*		g_pSvrDynCode = NULL;

#endif

// 진급 Point

 
#ifdef LOCALE_INDONESIA
const INT32		g_ai32RankUpBonus[] = 
{	
	0,		2000,	3200,	4000,	6000,	8000,	0,		0,		10000,	0,
	0,		0,		12000,	0,		0,		0,		0,		14000,	0,		0,
	0,		16000,	0,		0,		0,		0,		18000,	0,		0,		0,
	0,		20000,	0,		0,		0,		0,		22000,	0,		0,		0,
	0,		24000,	0,		0,		0,		0,		0,		0,		0,		0,
	0,		0
};

#else
const INT32		g_ai32RankUpBonus[] = 
{	
	0,		2000,	3200,	4000,	6000,	8000,	8000,	8000,	10000,	10000,
	10000,	10000,	12000,	12000,	12000,	12000,	12000,	14000,	14000,	14000,
	14000,	16000,	16000,	16000,	16000,	16000,	18000,	18000,	18000,	18000,
	18000,	20000,	20000,	20000,	20000,	20000,	22000,	22000,	22000,	22000,
	22000,	24000,	24000,	24000,	24000,	24000,	28000,	32000,	36000,	40000,
	60000,  1000000
};
#endif

/*
// 진급 Point
const INT32		g_RankUpBonus[] = 
{	
	0,		2000,	3200,	4000,	6000,	8000,	8000,	8000,	10000,	10000,
	10000,	10000,	12000,	12000,	12000,	12000,	12000,	14000,	14000,	14000,
	14000,	16000,	16000,	16000,	16000,	16000,	18000,	18000,	18000,	18000,
	18000,	20000,	20000,	20000,	20000,	20000,	22000,	22000,	22000,	22000,
	22000,	24000,	24000,	24000,	24000,	24000,	28000,	0,		0,		0,
	0
};
*/

void g_SetCurrentPath(char * pPath)
{
	//Set Char 
	i3String::Copy(g_strCCurrentPath, pPath, sizeof(g_strCCurrentPath)); 
}

INT32 g_InitServer(void)
{
	char strPath[SERVER_STRING_COUNT]; 

	//Error Handle Init
	i3Error::Init(); 

	// Expection 생성
	i3String::Format( strPath, SERVER_STRING_COUNT, "%s\\%s", g_strCCurrentPath, "PBGames.dmp" );
	i3Error::SetExceptionHandler( strPath );

	//Network Init
	if( FALSE == i3Net::Init() )													return SERVER_ERROR_NETWORK;

	//Load ContextFile 
	g_pContextMain = CContextMain::NewObject();
	if( NULL == g_pContextMain )													return SERVER_ERROR_CONFIG;
	if ( FALSE == g_pContextMain->InitShop())										return SERVER_ERROR_SHOP_CONTEXT;

	i3String::Copy( strPath, g_strCCurrentPath, SERVER_STRING_COUNT); 		
	i3String::Concat(strPath, "\\Config.ini"); 		
	if( FALSE == g_pContextMain->LoadConfigFile(strPath) )							return SERVER_ERROR_CONFIG; 

	//Create LogFile Module 
	g_pContextMain->RenewTime();
	{
		g_pModuleLogFile = CModuleLogFile::NewObject(); 
		if( FALSE == g_pModuleLogFile->OnCreate( g_pContextMain->m_ui32ThreadCount ))
		{
			return SERVER_ERROR_LOGFILE; 
		}
	}

	// 로그가 생성되지 않은상태에서 FALSE를 리턴하면 크래쉬하여 임시로 옮겼습니다.
	// 적당한자리로 옮겨야합니다.
	if( FALSE == g_pContextMain->LoadEventQuestFile() ) 
	{
		g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[LoadConfigFile] LoadEventQuestFile Error!! ");
		return SERVER_ERROR_LOAD_QUEST;
	}


	g_pContextMain->RenewTime();
	{//Create DataBase
		//pUdlFilePath "File Name=C:\\BcDataBase.udl;" <- 이런 형식으로 보내줘야 합니다. 
		char strPath[SERVER_STRING_COUNT]; 
		i3String::Copy( strPath, "File Name=", SERVER_STRING_COUNT); 
		i3String::Concat(strPath, g_strCCurrentPath); 
		i3String::Concat(strPath, "\\BcDataBase.udl;"); 

		g_pDataBase = CDataBase::NewObject(); 
		if( FALSE == g_pDataBase->OnCreate(g_pContextMain->m_ui32ThreadCount, strPath) )
		{
			g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[INIT] Error Create Database");
			return SERVER_ERROR_DATABASE; 
		}
		g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[INIT] Create Database ");
	}

	g_pContextMain->RenewTime();
	{	//Control Module 
		g_ui8Active = ASC_ICON_LOADING;
		g_pModuleControl = CModuleControl::NewObject();
		if( FALSE == g_pModuleControl->OnCreate( g_pContextMain->m_ui32ThreadCount ))	
		{
			g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[INIT] Error Create Control Module");
			return SERVER_ERROR_CONTROL; 
		}
		g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[INIT] Create Control Module");
	}

	//g_bUpdate = TRUE; 
	g_eServerState		= SERVER_STATE_LOADING;
	g_ui32StartTime		= g_pContextMain->GetServerTime();

	g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "---------------- START GAME SERVER ---------------------");

	I3BCLOG(LOCATION, "[%02d%02d%02d][%02d:%02d:%02d]---------Start--------------------", g_pContextMain->m_i32Year, g_pContextMain->m_i32Month, g_pContextMain->m_i32Day, g_pContextMain->m_i32Hour, g_pContextMain->m_i32Minute, g_pContextMain->m_i32Second);		

	return EVENT_ERROR_SUCCESS;
}

INT32 g_StartServer(void)
{
	g_eServerState = SERVER_STATE_INITING;

	// Create CPU Speed
	g_pContextMain->RenewTime();
	if( g_pModuleControl->InitSystemLog() )
	{
		g_pModuleLogFile->Write_M_Log( MAIN_THREAD_IDX, NULL, "[SUCC] Start Control SystemLog" );
	}
	else
	{
		g_pModuleLogFile->Write_M_Log( MAIN_THREAD_IDX, NULL, "[FAIL] Start Control SystemLog" );
	}

	g_pContextMain->RenewTime();
	{//Trans
		g_pModuleTrans	= CModuleTrans::NewObject();
		INT32 i32ModuleRv = g_pModuleTrans->OnCreate( g_pContextMain->m_ui32ThreadCount ); 
		if( i32ModuleRv !=  EVENT_ERROR_SUCCESS)
		{
			g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[START] Error Create Module Trans");
			return i32ModuleRv; 
		}
		g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[START] Create Module Trans");
	}
	g_pContextMain->RenewTime();
	{
		g_pModuleClan = CModuleClan::NewObject(); 
		INT32 i32ModuleRv = g_pModuleClan->OnCreate( g_pContextMain->m_ui32ThreadCount ); 
		if( i32ModuleRv !=  EVENT_ERROR_SUCCESS)
		{
			g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[START] Error Create Module Clan");
			return i32ModuleRv; 
		}
		g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[START] Create Module Clan");
	}
	g_pContextMain->RenewTime();
	{
		g_pModuleMessenger = CModuleMessenger::NewObject(); 
		INT32 i32ModuleRv = g_pModuleMessenger->OnCreate( g_pContextMain->m_ui32ThreadCount ); 
		if( i32ModuleRv !=  EVENT_ERROR_SUCCESS)
		{
			g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[START] Error Create Module Messenger");
			return i32ModuleRv; 
		}
		g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[START] Create Module Messenger");
	}

	g_pContextMain->RenewTime();
	{//Load ChallengeFile
		if( FALSE == g_pContextMain->LoadQuestFile( g_strCCurrentPath ) )
		{
			g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[START] Error Load Quest");
			return SERVER_ERROR_LOAD_QUEST;
		}
		g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[START] Load Quest");
	}
	g_pContextMain->RenewTime();
	{//Nc Module 
		g_pModuleContextNC = CModuleContextNC::NewObject(); 
		INT32 i32ModuleRv = g_pModuleContextNC->OnCreate( g_pContextMain->m_ui32ThreadCount ); 
		if( i32ModuleRv !=  EVENT_ERROR_SUCCESS)
		{
			g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[START] Error Create NC Module");
			return i32ModuleRv; 
		}
		g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[START] Create NC Module");
	}
	// GameGuard
	g_pContextMain->RenewTime();
	switch( g_pContextMain->m_i32GameGuard )
	{
		case GAMEGUARD_NPROTECT:		// nProtect
			{
#ifdef USE_NPROTECT
				char strFilePath[1024];
				i3String::Format( strFilePath,"%s\\nProtect",g_strCCurrentPath );
				UINT32 ui32Return = InitCSAuth3( strFilePath );
				if( ui32Return != ERROR_SUCCESS )
				{
					char strErrorLog[1024];
					i3String::Format( strErrorLog,"[START] Error GameGuard NProtect - %d",ui32Return );
					g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, strErrorLog );
					return SERVER_ERROR_GAMEGUARD; 
				}
				g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[START] Success GameGuard NProtect");
#endif
			}
			break;
		case GAMEGUARD_HACKSHIELD:		// Ahn HackShield
			{
#ifdef USE_AHNHS
				char strPath[ MAX_PATH];
				i3String::Copy( strPath, g_strCCurrentPath, sizeof(strPath)); 		
				i3String::Concat(strPath, "\\HackShield\\AntiCpX.hsb");

				g_hAhnhsServer = _AhnHS_CreateServerObject ( strPath ); // .hsb 파일 경로

				if ( g_hAhnhsServer == ANTICPX_INVALID_HANDLE_VALUE )
				{
					g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[START] Error GameGuard Ahn HackShield");
					return SERVER_ERROR_GAMEGUARD;
				}
				g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[START] Success GameGuard Ahn HackShield");
#endif
			}
			break;
// Xtrap
		case GAMEGUARD_XTRAP:		// Xtrap
			{
#ifdef USE_XTRAP
				char strPath[ MAX_PATH];
				// Map File1 Load
				i3String::Copy( strPath, g_strCCurrentPath,MAX_PATH);
				i3String::Concat(strPath, "\\XTrap\\PointBlank.CS3");

				g_pXTrapModule = new CXTrapModule;
				if( FALSE == g_pXTrapModule->OnCreate( strPath ) )
				{
					g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[START] Error GameGuard Xtrap");
					return SERVER_ERROR_GAMEGUARD;
				}
				g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[START] Success GameGuard Xtrap");
#endif
			}
			break;
		case GAMEGUARD_GPK:
			{
		#ifdef USE_GPK
				g_pSvrDynCode = SGPK::GPKCreateSvrDynCode();
				if(NULL == g_pSvrDynCode)
				{
					g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[START][GPK] Create SvrDynCode component failed");
					return SERVER_ERROR_GAMEGUARD;
				}

				INT32 i32BinCount;

				i32BinCount = LoadGPK();

				char strTemp[1024];
				if(0 == i32BinCount || -1 == i32BinCount)
				{					
					i3String::Format( strTemp, "[START][GPK][%d] LoadBinary Fail",i32BinCount);
					g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, strTemp);
					return SERVER_ERROR_GAMEGUARD;
				}

				i3String::Format( strTemp, "[START][%d] Success GameGuard GPK",i32BinCount);
				g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, strTemp);
		#endif
			}
			break;
	}
	g_pContextMain->RenewTime();
	{//Creat Server Context 
		g_pServerContext = CServerContext::NewObject(); 
		if( !g_pServerContext->OnCreate(g_pContextMain->m_ui8AddressCount, g_pContextMain->m_pui32AddressIp, g_pContextMain->m_pui16AddressPort, g_pContextMain->m_pui8AddressTimeOut, g_pContextMain->m_ui32ThreadCount, NULL) )
		{
			g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[START] Error Create ServerContext");
			return SERVER_ERROR_CONTEXT; 
		}
		g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[START] Create ServerContext");
	}
	g_pContextMain->RenewTime();

	// 컨트롤 서버와 빌드 버전 비교후 다르다면 서버 종료. (New)
	if ( (g_pContextMain->m_ControlVersion._ui8VerGame != g_pContextMain->m_GameVersion._ui8VerGame) || (g_pContextMain->m_ControlVersion._ui16VerBuild != g_pContextMain->m_GameVersion._ui16VerBuild) || g_pContextMain->m_ControlVersion._ui16VerSS != g_pContextMain->m_GameVersion._ui16VerSS ) 
	{
		char strTemp[MAX_PATH];
		i3String::Format( strTemp, MAX_PATH, "Build version is different with Control Server - Control Server : %d.%d.%d. , Game Server : %d.%d.%d.",
			g_pContextMain->m_ControlVersion._ui8VerGame, g_pContextMain->m_ControlVersion._ui16VerBuild, g_pContextMain->m_ControlVersion._ui16VerSS,
			g_pContextMain->m_GameVersion._ui8VerGame, g_pContextMain->m_GameVersion._ui16VerBuild, g_pContextMain->m_GameVersion._ui16VerSS);
		g_pModuleLogFile->Write_M_Log( MAIN_THREAD_IDX, NULL, strTemp );

		return SERVER_ERROR_DIFFERENT_BUILD_VERSION; 
	}


	// 서버 로딩 완료
	g_ui8Active = ASC_ICON_CONNECTION;

	g_eServerState = SERVER_STATE_START;

	return EVENT_ERROR_SUCCESS;
}

void g_UpdateServer(void)
{	
	g_pContextMain->RenewTime();

	switch( g_eServerState )
	{
	case SERVER_STATE_INIT:
		{
			INT32 i32Rv = g_StartServer();
			if( i32Rv != EVENT_ERROR_SUCCESS )
			{
				// 서버 로딩 결과 전달
				i3NetworkPacket		SendErrorPacket;
				SendErrorPacket.SetProtocol(PROTOCOL_ASC_SERVER_LOADING_RESULT);
				SendErrorPacket.WriteData(&i32Rv, sizeof(INT32));
				g_pModuleControl->SendPacketMessage(&SendErrorPacket);

				g_DestroyServer();
				i3mem::Destroy();
				exit(0);
			}
		}
		break;
	case SERVER_STATE_START:
		{
			//순서 중요합니다. 먼저 룸을 업데이트 하고 유저를 업데이트합니다.		
			g_pContextMain->OnUpdate(); 		
			g_pServerContext->OnUpdate( SERVER_UPDATE_ROOM );
			g_pServerContext->OnUpdate( SERVER_UPDATE_PLAYER );

			//Update Channel Idx
			g_pContextMain->m_ui32UpdateChannel++; 
			if(g_pContextMain->m_ui32UpdateChannel == GAME_CHANNEL_COUNT)g_pContextMain->m_ui32UpdateChannel = 0;

			 /////////////////////////////////////////////////////////////////////////////////////////////////
			//로그 : 서버 상태 초당 동접, 채널별 (시간으로 제어) 10초마다 
			if( (g_pContextMain->GetServerTime() - s_LastLogUpdateTime_State ) > LOG_UPDATE_TIME_STATE)
			{
				PDH_STATUS				pdhStatus;		
				DWORD					ctrType;
				PDH_FMT_COUNTERVALUE	fmtValue;

				s_LastLogUpdateTime_State = g_pContextMain->GetServerTime(); 				
			}

			//////////////////////////////////////////////////////////////////////////////////////////////
			//로그 : 1초마다 한번씩 한채널의 정보를 보낸다. 
			if( (g_pContextMain->GetServerTime() - s_LastLogUpdateTime_Channel) > LOG_UPDATE_TIME_CHANNEL)
			{
				//For Next 
				s_LastLogUpdateTime_Channel = g_pContextMain->GetServerTime(); 
			}
		}
		break;
	default:
		{
			// Control 서버와 연결 안됨
			if( g_ui32StartTime + 60 < g_pContextMain->GetServerTime() )
			{
				g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[INIT_LOG]--ERROR-- CONTROL SERVER Connect Fail");
				// 게임 종료
				g_DestroyServer();
				i3mem::Destroy();
				exit(0);
			}
		}
		break;
	}

	return; 
}

void g_DestroyServer(void)
{
	// Control 서버에 정상 종료 패킷 전송.
	i3NetworkPacket SendPacket( PROTOCOL_ASC_NORMAL_DISCONNECT_REQ );
	if ( g_pModuleControl )
		g_pModuleControl->SendPacketMessage( &SendPacket );

	Sleep(1000);

	if( NULL != g_pModuleContextNC )
	{
		g_pModuleContextNC->SendZLog();
		Sleep(100);
	}

	if(g_pModuleLogFile		!= NULL) g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "--START SERVER DESTROY--");

	switch( g_pContextMain->m_i32GameGuard )
	{
		case GAMEGUARD_NPROTECT:
		#ifdef USE_NPROTECT
			CloseCSAuth3();
		#endif
			break;
		case GAMEGUARD_HACKSHIELD:
		#ifdef USE_AHNHS
			_AhnHS_CloseServerHandle( g_hAhnhsServer );
		#endif
			break;
		case GAMEGUARD_XTRAP:
		#ifdef USE_XTRAP
			I3_SAFE_DELETE( g_pXTrapModule );
		#endif
		case GAMEGUARD_GPK:
		#ifdef USE_GPK
			g_pSvrDynCode->Release();
		#endif
			break;
	}

	if(g_pModuleLogFile		!= NULL) g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "--DESTROY GameGuard--");

	I3_SAFE_RELEASE( g_pServerContext	);	if(g_pModuleLogFile		!= NULL) g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[SUCC] DESTROY g_pServerContext");
	I3_SAFE_RELEASE( g_pModuleControl	);	if(g_pModuleLogFile		!= NULL) g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[SUCC] DESTROY g_pModuleControl");
	I3_SAFE_RELEASE( g_pModuleTrans		);	if(g_pModuleLogFile		!= NULL) g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[SUCC] DESTROY g_pModuleTrans");
	I3_SAFE_RELEASE( g_pModuleClan		);	if(g_pModuleLogFile		!= NULL) g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[SUCC] DESTROY g_pModuleClan");
	I3_SAFE_RELEASE( g_pModuleMessenger	);	if(g_pModuleLogFile		!= NULL) g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[SUCC] DESTROY g_pModuleMessenger");
	I3_SAFE_RELEASE( g_pDataBase		);	if(g_pModuleLogFile		!= NULL) g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[SUCC] DESTROY g_pDataBase");
	I3_SAFE_RELEASE( g_pModuleContextNC );	if(g_pModuleLogFile		!= NULL) g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[SUCC] DESTROY g_pModuleContextNC");

	if(g_pModuleLogFile		!= NULL) g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "--DESTROY CloseQuery--");

	i3Net::Destroy(); 

	if(g_pModuleLogFile != NULL) g_pModuleLogFile->Write_M_Log( MAIN_THREAD_IDX, NULL, "--------------------- Game Server End ---------------------" );
	I3_SAFE_RELEASE( g_pModuleLogFile );
	I3_SAFE_RELEASE( g_pContextMain );	  
	
	return; 
}

UINT32			g_GetWeaponClass_ForMission(UINT32 ui32WeaponClass)
{
	// 원래 웨폰 클래스와 비교하는 클래스가 다른 경우가 있어 함수에서 확인
	switch( ui32WeaponClass )
	{
		case WEAPON_CLASS_UNKNOWN:				return WEAPON_CLASS_UNKNOWN;

		case WEAPON_CLASS_KNIFE:				return WEAPON_CLASS_KNIFE;			//	나이프
		case WEAPON_CLASS_HANDGUN:				return WEAPON_CLASS_HANDGUN;		//	권총
		case WEAPON_CLASS_ASSAULT:				return WEAPON_CLASS_ASSAULT;		//	소총
		case WEAPON_CLASS_SMG:					return WEAPON_CLASS_SMG;			//	서브머신건
		case WEAPON_CLASS_SNIPER:				return WEAPON_CLASS_SNIPER;			//	저격총
		case WEAPON_CLASS_SHOTGUN:				return WEAPON_CLASS_SHOTGUN;		//	산탄총
		case WEAPON_CLASS_THROWING:				return WEAPON_CLASS_THROWING;		//	투척 무기
		case WEAPON_CLASS_ITEM:					return WEAPON_CLASS_ITEM;			//	아이템
		case WEAPON_CLASS_MISSION:				return WEAPON_CLASS_MISSION;		//	미션 아이템
		case WEAPON_CLASS_MG:					return WEAPON_CLASS_MG;				//	머신건

		case WEAPON_CLASS_GRENADESHELL:			return WEAPON_CLASS_GRENADESHELL;	// 유탄
		case WEAPON_CLASS_BOMBTRIGGER:			return WEAPON_CLASS_BOMBTRIGGER;	// 폭탄 스위치

		case WEAPON_CLASS_CIC:					return WEAPON_CLASS_HANDGUN;
		case WEAPON_CLASS_DUALHANDGUN:			return WEAPON_CLASS_HANDGUN;
		case WEAPON_CLASS_DUALKNIFE:			return WEAPON_CLASS_KNIFE;

		case WEAPON_CLASS_ROCKET_LAUNCHER:		return WEAPON_CLASS_ROCKET_LAUNCHER;
		case WEAPON_CLASS_OBJECT_GUN:			return WEAPON_CLASS_OBJECT_GUN;	// 특정 오브젝트에 붙어 있는 총 ( M197....)

		case WEAPON_CLASS_DUALSMG:				return WEAPON_CLASS_SMG;

		case WEAPON_CLASS_DINO:					return WEAPON_CLASS_DINO;
		case WEAPON_CLASS_DINO_LEFT_SCRATCH:	return WEAPON_CLASS_DINO;	// 왼쪽 손 할퀴기 이펙트를 위해 만듬.
		case WEAPON_CLASS_TREX:					return WEAPON_CLASS_DINO;	// 왼쪽 손 할퀴기 이펙트를 위해 만듬.
		case WEAPON_CLASS_STING:				return WEAPON_CLASS_DINO;

		case WEAPON_CLASS_COUNT:				return WEAPON_CLASS_COUNT;

		case WEAPON_CLASS_KNUCKLE:				return WEAPON_CLASS_KNUCKLE;

		default:								return WEAPON_CLASS_UNKNOWN;
	}
}

STAGE_ORDINAL_ID	g_GetStageID_ForMission( STAGE_ORDINAL_ID StageID )
{
	// 같은 맵이지만 모드가 달라 ID가 다른 경우 같은 맵으로 맞춰주기 위해 필요
	switch( StageID )
	{
	case STAGE_ORD_D_DOWNTOWN:					return STAGE_ORD_DOWNTOWN;
	case STAGE_ORD_D_LUXVILLE:					return STAGE_ORD_LUXVILLE;
	case STAGE_ORD_D_BLOWCITY:					return STAGE_ORD_BLOWCITY;
	case STAGE_ORD_D_STORMTUBE:					return STAGE_ORD_STORMTUBE;
	case STAGE_ORD_D_GIRAN2:					return STAGE_ORD_GIRAN2;
	case STAGE_ORD_D_BREAKDOWN:					return STAGE_ORD_BREAKDOWN;
	case STAGE_ORD_D_SENTRYBASE:				return STAGE_ORD_SENTRYBASE;
	case STAGE_ORD_D_SANDSTORM:					return STAGE_ORD_SANDSTORM;
	case STAGE_ORD_D_SHOPPINGCENTER:			return STAGE_ORD_SHOPPINGCENTER;
	default:									return StageID;
	}
}

// 어뷰징 체크시 사용되는 값 . [기준(초)]
UINT32		g_ui32AbusingCalValue[] = 
{	
	300,275,250,225,	// 일반 제재
	250,225,200,175		// 강화 제재
};

#ifdef USE_GPK
INT32	LoadGPK()
{
	INT32 i32BinCount = 0;
	//load dynamic code database
#if defined( I3_DEBUG)
	char szPath1[ MAX_PATH ];
	char szPath2[ MAX_PATH ];
	i3String::Copy( szPath1, g_strCCurrentPath, MAX_PATH); 
	i3String::Concat(szPath1, "GPK\\Redist\\Debug\\Server\\DynCode\\Server");
	i3String::Copy( szPath2, g_strCCurrentPath, MAX_PATH); 
	i3String::Concat(szPath2, "GPK\\Redist\\Debug\\Server\\DynCode\\Client");
#else
	char szPath1[ MAX_PATH ];
	char szPath2[ MAX_PATH ];
	i3String::Copy( szPath1, g_strCCurrentPath, MAX_PATH); 
	i3String::Concat(szPath1, "\\GPK\\Redist\\Release\\Server\\DynCodeBin\\Server");
	i3String::Copy( szPath2, g_strCCurrentPath, MAX_PATH); 
	i3String::Concat(szPath2, "\\GPK\\Redist\\Release\\Server\\DynCodeBin\\Client");
#endif

	i32BinCount = (INT32)g_pSvrDynCode->LoadBinary(szPath1, szPath2);

	char strTemp[1024];
	i3String::Format( strTemp, "[START][GPK][%s][%s]",szPath1, szPath2);
	g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, strTemp);

	return i32BinCount;
}
#endif