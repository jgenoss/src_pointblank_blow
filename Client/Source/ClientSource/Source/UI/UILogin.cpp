#include "pch.h"
#include "UILogin.h"

#include "UIPhaseLogin.h"
#include "UIPhaseNation.h"
#include "../UI_V11/UIPhaseLogin_V11.h"
#include "../UI_V10/UIPhaseLogin_V10.h"


I3_CLASS_INSTANCE( UILogin);//, i3Stage);

UILogin::UILogin(): m_phaseState(PS_START), m_currentPhase(LOGIN_UIPHASE_START), m_lastPhase(LOGIN_UIPHASE_START)
{
	m_PhaseQueue.clear();
	m_LoginPhase.clear();

}

UILogin::~UILogin()
{

}

void UILogin::__UpdatePhase(REAL32 rDeltaSeconds)
{

	// phase					// MustBeFix
	switch (m_phaseState)
	{
	case PS_IDLE:
		if (m_PhaseQueue.size() > 0)
		{
			LOGIN_UIPHASE MovePhase = m_PhaseQueue[0];
			i3::vu::erase_index(m_PhaseQueue, 0);

			StartPhase(MovePhase);

			m_phaseState = PS_ENTRANCE_START;
		}
		break;
	case PS_ENTRANCE_START:
		if (GetCurrentPhase()->OnEntranceStart())
		{
			m_phaseState = PS_ENTRANCING;
		}

		break;
	case PS_ENTRANCING:
		m_phaseState = PS_ENTRANCE_END;
		break;
	case PS_ENTRANCE_END:
		m_phaseState = PS_PLAYING;
		break;
	case PS_PLAYING:
	{
		if (m_PhaseQueue.size() > 0)
		{
			m_phaseState = PS_EXIT_START;
		}
	}
	break;

	case PS_EXIT_START:
	{
		EndPhase();
		m_phaseState = PS_EXITING;
	}
	break;
	case PS_EXITING:
		m_phaseState = PS_EXIT_END;
		break;
	case PS_EXIT_END:
		m_phaseState = PS_IDLE;
		break;
	}
}

/*virtual*/ void UILogin::OnCreate( void)
{
	i3Stage::OnCreate();

}

/*virtual*/ void UILogin::OnLoadEnd(void)
{
	i3Stage::OnLoadEnd();

	i3ClassMeta * pMeta = nullptr;

	// Create Phase
	m_LoginPhase.resize(LOGIN_UIPHASE_MAX);
	for (INT32 i = 0; i < LOGIN_UIPHASE_MAX; i++)
	{
		switch (i)
		{
		case LOGIN_UIPHASE_LOGIN:
		{
			if (g_pEnvSet->m_nUIVersion == UFV_1_1)
			{
				pMeta = UIPhaseLogin_V11::static_meta();
			}
			else if (g_pEnvSet->IsBaseOnUIVersion_1_0())
			{
				pMeta = UIPhaseLogin_V10::static_meta();
			}
			else	//UFV_1_5
			{
				pMeta = UIPhaseLogin::static_meta();
			}
		}
		break;
		case LOGIN_UIPHASE_NATION:
			pMeta = UIPhaseNation::static_meta();
			break;
			/*
		default:
			pMeta = nullptr;
			I3PRINTLOG(I3LOG_NOTICE, "Not Setted phase class...!!!");
			break;
			*/
		}

		if (pMeta != nullptr)
		{
			m_LoginPhase[i] = (UIPhaseBase*)pMeta->create_instance(); //CREATEINSTANCE( pMeta);
			I3ASSERT(m_LoginPhase[i]->same_of(pMeta));

			m_LoginPhase[i]->OnCreate(this);
			I3_MUST_ADDREF(m_LoginPhase[i]);
		}
	}
	// Phase 기본 설정
	if (m_lastPhase == LOGIN_UIPHASE_START)
	{
		if( g_pServerInfo->GetUseIPCount() > 1)
			MovePhase(LOGIN_UIPHASE_NATION);
		else
			MovePhase(LOGIN_UIPHASE_LOGIN);
	}
	else
	{
		m_PhaseQueue.push_back(m_lastPhase);
		m_currentPhase = m_lastPhase;
	}
}

/*virtual*/ bool UILogin::OnFinish( void)
{
	for (size_t i = 0; i < m_LoginPhase.size(); i++)
	{
		if (m_LoginPhase[i])
		{
			m_LoginPhase[i]->Destroy();
			m_LoginPhase[i] = nullptr;
		}
	}
	return true;
}

/*virtual*/ void UILogin::OnUpdate(REAL32 rDeltaSeconds)
{
	__UpdatePhase(rDeltaSeconds);
	i3Stage::OnUpdate(rDeltaSeconds);
}
/*virtual*/ bool UILogin::OnEvent( UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if (GetCurrentPhase() != nullptr)
	{
		if (GetCurrentPhase()->OnEvent(event, pObj, param2, code))
			return true;
	}

	return i3Stage::OnEvent( event, pObj, param2, code);
}
//
void UILogin::OnEscape( void)
{
	if( GetCurrentPhase())
	{
		GetCurrentPhase()->ClickExit();
	}
}


void UILogin::MovePhase(LOGIN_UIPHASE Phase)
{
	if (m_currentPhase != Phase)
	{
		m_PhaseQueue.push_back(Phase);
		m_lastPhase = m_currentPhase;
	}
}

/*virtual*/void UILogin::StartPhase(LOGIN_UIPHASE MovePhase)
{
	//ChannelType channeltype;
	m_currentPhase = MovePhase;
	m_lastPhase = m_currentPhase;

}

/*virtual*/void UILogin::EndPhase(void)
{
	LOGIN_UIPHASE NextPhase = LOGIN_UIPHASE_START;

	if (m_PhaseQueue.size() > 0)
		NextPhase = m_PhaseQueue[0];

	GetCurrentPhase()->OnExitStart();

}