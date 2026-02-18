#include "pch.h"
#include "GameContext.h"

#include "HackContext.h"
#include "MyRoomInfoContext.h"
#include "UserInfoContext.h"
#include "ShutDownContext.h"
#include "Mode/Domination/Domination_Defines.h"
#include "Mode/Domination/DomiStageProperties.h"
#include "Mode/Domination/DomiSkillProperties.h"
#include "Network/GameNetworkManager.h"
#include "BattleServerContext.h"
#include "GameMissionManager.h"
#include "./StageBattle/IntrudeContext.h"
#include "./StageBattle/UserContext.h"
#include "GameMaster.h"

I3_CLASS_INSTANCE( CGameContext);

CGameContext::CGameContext(void)
{
	HackContext::i()->InitHackFlag();
}

CGameContext::~CGameContext(void)
{
	OnDestroy();
}

bool CGameContext::OnCreate(void)
{
	S2MOCreate();
	GameEventSender::i()->OnCreate();
	MyRoomInfoContext::i()->InitMyRoomInfo();
	CGameNetworkManager::i()->CreateTCPSocket();
	BattleServerContext::i()->InitP2PData();		// P2P Data 및 Network Data 초기화 (생성자에서 m_pNetworkManager 인스턴스 접근을 위해 이리로 일단 이동)
	BattleServerContext::i()->InitRoundData();
	return true;
}


bool CGameContext::OnDestroy(void)
{
	//Dump
#if defined( I3_DEBUG)
	I3PRINTLOG(I3LOG_NOTICE,"Connect Time %d", (UINT32)g_pFramework->GetAccTime());
	CGameNetworkManager::i()->Dump();
#endif

	CGameNetworkManager::i()->DestroyUDPSocket();
	CGameNetworkManager::i()->DestroyTCPSocket();

	S2MODestory();

	return true;
}


void CGameContext::OnUpdate( REAL32 rDeltatime )
{
	ISTEST_S
	{
		CGameMissionManager::i()->OnUpdate( rDeltatime );
		HackContext::i()->penaltyUpdate( rDeltatime); 
		return;
	}
	ISTEST_E

	//
#if defined( LOCALE_KOREA )
	ShutDownTimer();
#endif // LOCALE_KOREA

	INT64	time1;// = i3Timer::GetProfileTime();
	QueryPerformanceCounter( (LARGE_INTEGER *) &time1);

	CGameNetworkManager::i()->Update( rDeltatime);

	//로딩완료후부터 배틀까지
	if( BattleServerContext::i()->IsP2PActivate() )
	{
		// 난입 처리입니다.
		IntrudeContext::i()->ProcessIntrude();
		
		ROOM_INFO_BASIC	roomInfo;
		MyRoomInfoContext::i()->getMyRoomInfo(&roomInfo);

		//KEYWORD : STAGE_MODE_ADD
		if(roomInfo._State == ROOM_STATE_BATTLE)	//배틀만
		{
			// revision 31898
			BattleServerContext::i()->OnUpdate( rDeltatime);
			HackContext::i()->penaltyUpdate( rDeltatime);

			if ( !CGameMaster::i()->IsPaused() )
			{
				UserContext::i()->fRemainingBattleTime -= rDeltatime;
				CGameMissionManager::i()->OnUpdate( rDeltatime );
			}
		}
	}

	// TODO : Heart Bit
#if !defined(THREAD_TCP)
	CGameNetworkManager::i()->UpdateHeartbeat( g_pFramework->GetAccTime() );
#endif

	return;
}

// 커스텀 메세지 설정 [북미 로그인 추가 - 2011.06.10 양승천] 
/*
void CGameContext::SetCustomMSG(const char* Msg)
{
	if(i3::generic_string_size(Msg) > 0)
	{
		i3::safe_string_copy(m_strCustomMsg , Msg, NORTH_CUSTOM_MSG_SIZE);
	}
}
*/

void CGameContext::ShutDownTimer()
{
	UINT16 _OldPlayTime = ShutDownContext::i()->GetPlayTime();

	//ShutDownContext::i()->SetPlayTime((UINT16)(g_pFramework->GetAccTime() / 10));
	ShutDownContext::i()->SetPlayTime((UINT16)(g_pFramework->GetAccTime() / 3600));

	if ( _OldPlayTime != ShutDownContext::i()->GetPlayTime())
	{
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_PLAYTIME_ALARM);
	}

	//I3TRACE("AccTime : %f, PlayTime : %d\n", acctime, _PlayTime);

	if(ShutDownContext::i()->IsShutDown())
	{
		if(UserInfoContext::i()->getAge() < 16)
		{
			SYSTEMTIME sysTime;
			GetLocalTime(&sysTime);

			if ( sysTime.wHour == 23 )
			{
				if (sysTime.wMinute == 30 && m_bShutDownTimeAlarm == true)
				{
					m_bShutDownTimeAlarm = false;
					ShutDownContext::i()->SetFnishTime(30);
					GameEventReceiver::i()->PushReceivedGameEvent(EVENT_SHUTDOWNTIME_ALARM);
				}
				else if (sysTime.wMinute == 55 && m_bShutDownTimeAlarm == true)
				{
					m_bShutDownTimeAlarm = false;
					ShutDownContext::i()->SetFnishTime(5);
					GameEventReceiver::i()->PushReceivedGameEvent(EVENT_SHUTDOWNTIME_ALARM);
				}
				else if (sysTime.wMinute == 30 || sysTime.wMinute == 55)
					m_bShutDownTimeAlarm = true;
			}
		}
	}
}