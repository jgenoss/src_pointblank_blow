#include "pch.h"


#include "GameEventSender.h"

#include "GameEventHandler.h"

#include "UI/UIMainFrame.h"
#include "UI/UIPhaseBase.h"
#include "UI/UIUtil.h"

#if defined(BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG)
#include "UnitTest/GameUnitTest.h"
#endif


GameEventSender::GameEventSender() : m_pEventHandler(nullptr)
{
	m_BeforeEvent		= 0;
}

GameEventSender::~GameEventSender()
{
	I3_SAFE_RELEASE( m_pEventHandler);
}

void	GameEventSender::OnCreate()
{
	m_pEventHandler = CGameEventHandler::new_object();
}

bool GameEventSender::SetEvent(INT32 Event, const void * pArg, const void * pArg1, const void * pArg2, LPCSTR szFile /*= nullptr*/, int nLine /*= 0*/)
{
#if ENABLE_UNIT_TEST
	// UI Unit Test중인 경우 이벤트를 전송하지 않습니다.
	if(UnitTest::get_unit_test() != nullptr && 
		UnitTest::get_unit_test()->m_UnitTestFlag.is(UNIT_TEST_UI_PHASE | UNIT_TEST_UI_POPUP) )
		return false;
#endif

	ISNETWORKRETURNV(false,true)

	if( (g_pFramework != nullptr) &&
		(g_pFramework->GetUIMainframe() != nullptr) &&
		(g_pFramework->GetUIMainframe()->GetCurrentPhase() != nullptr) )
	{
		if (/*Event != EVENT_ABNORMAL_LOG &&*/ g_pFramework->GetUIMainframe()->GetCurrentPhase()->isMyPhase() == false)
		{
#if defined(I3_DEBUG)
			I3TRACE("SetEvent Other Phase trace : %d\n", Event);
#endif
			return false;
		}
	}

	I3ASSERT( (Event&0xFFF) < EVENT_M_S);

	//요청한 작업을 네트웍전송합니다.
	I3ASSERT( m_pEventHandler != nullptr);
	m_pEventHandler->SendEvent( (GAME_EVENT) Event, pArg, pArg1, pArg2, szFile, nLine);
	
//	if( (Event & EVENT_NORESPONSE) == 0)			// 이정도만 갖고는 쓸모가 없어보여서 주석처리함..(2013.07.15.수빈)
//	{												// 깨끗하게 하려면 m_recvEvent처럼 쌓아놓아야한다..
//		m_ReqEvent		= Event;
//		m_ReqEventTime	= 0.0f;
//	}

	GameUI::ProcessInputDisableChangePhase( Event );

	return true;
}

bool	GameEventSender::SetEventOnce(INT32 Event, const void * pArg, const void * pArg1, const void * pArg2 )
{
	bool bFirst = false;
	if( g_pFramework != nullptr &&
		g_pFramework->GetUIMainframe() != nullptr &&
		g_pFramework->GetUIMainframe()->isLockedInput() == false)
	{
		g_pFramework->GetUIMainframe()->SetLockInput( true);
		m_BeforeEvent = Event;
		bFirst = true;
	}

	if( (!bFirst && m_BeforeEvent != Event) || (bFirst == true))
		return SetEvent(Event, pArg, pArg1, pArg2);

	I3PRINTLOG(I3LOG_NOTICE,"Ignore Same Evt : %d, %d", m_BeforeEvent,Event);
	return false;
}

