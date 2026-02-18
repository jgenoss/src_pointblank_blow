#include "pch.h"
#include "UIPhaseProfile_V11.h"

#include "UITabProfileDesignation_V11.h"
#include "UITabProfileMissionCard_V11.h"
//#include "UITabProfileRecord.h"
//#include "UITabProfileWeaponRecord.h"
#include "UIFloatChat_V11.h"
#include "UITopMenu_V11.h"
#include "UI/UIMainFrame_V11.h"

static UIPhaseProfile_V11 * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);

	return (UIPhaseProfile_V11*)pFrame->GetSubPhase( USP_PROFILE);
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int PhaseProfile_ChangeTab_V11( LuaState * L)
	{
		PROFILETAB tab = (PROFILETAB) i3Lua::GetIntegerArg( L, 1);

		GetThis()->ChangeTab( tab);
		return 0;
	}
	/*
	int PhaseProfile_ChangeCardPack( LuaState * L)
	{
		INT32 pack = i3Lua::GetIntegerArg( L, 1);

		GetThis()->ClickMissionCard( pack);

		return 0;
	}

	int PhaseProfile_ClickDesignation( LuaState * L)
	{
		INT32 slot = i3Lua::GetIntegerArg( L, 1);

		GetThis()->ClickDesignation( slot);
		return 0;
	}
	*/
}

LuaFuncReg PhaseProfile_V11_Glue[] =
{
	{"ChangeTab",				PhaseProfile_ChangeTab_V11			},
//	{"ClickMissionCard",		PhaseProfile_ChangeCardPack		},
//	{"ClickDesignation",		PhaseProfile_ClickDesignation	},
	{nullptr,		nullptr}
};


I3_CLASS_INSTANCE( UIPhaseProfile_V11);

UIPhaseProfile_V11::UIPhaseProfile_V11() : m_pBtnGeneral( nullptr )
{
	m_pDesignation = nullptr;
	m_pMissionCard = nullptr;
	m_pRecord = nullptr;
	m_pWeaponRecord = nullptr;

}

UIPhaseProfile_V11::~UIPhaseProfile_V11()
{
	if( m_pDesignation)
	{
		m_pDesignation->Destroy();
	}

	if( m_pMissionCard)
	{
		m_pMissionCard->Destroy();
	}

//	if( m_pRecord)
	//{
//		m_pRecord->Destroy();
	//}

//	if( m_pWeaponRecord)
	//{
	//	m_pWeaponRecord->Destroy();
	//}
}

//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//
/*virtual*/ void UIPhaseProfile_V11::OnCreate( i3GameNode * pParent)
{
	UIPhaseSubBase::OnCreate( pParent);

	// Load Scene
 	AddScene( "Scene/Main/PointBlankRe_MyInfomation.i3UIs", PhaseProfile_V11_Glue);

	// Create Tab
	m_pDesignation = UITabProfileDesignation_V11::new_object();
	I3ASSERT( m_pDesignation != nullptr);
	m_pDesignation->OnCreate( this);

	m_pMissionCard = UITabProfileMissionCard_V11::new_object();
	I3ASSERT( m_pMissionCard != nullptr);
	m_pMissionCard->OnCreate( this);

	//m_pRecord = UITabProfileRecord::new_object();
	//I3ASSERT( m_pRecord != nullptr);
//	m_pRecord->OnCreate( this);

//	m_pWeaponRecord = UITabProfileWeaponRecord::new_object();
//	I3ASSERT( m_pWeaponRecord != nullptr);
//	m_pWeaponRecord->OnCreate( this);
}

/*virtual*/ void UIPhaseProfile_V11::_InitializeAtLoad( i3UIScene * pScene )
{
	UIPhaseBase::_InitializeAtLoad(pScene);

	I3ASSERT_RETURN(pScene);

	m_pBtnGeneral = (i3UIButton*)pScene->FindChildByName( "i3UIButtonComposed3_MIM" );

	m_slide.AddFrameWnds(pScene, UISlide::R_TO_L);
	
}

/*virtual*/ void UIPhaseProfile_V11::_ClearAtUnload( i3UIScene * pScene )
{
	UIPhaseBase::_ClearAtUnload(pScene);

	m_pBtnGeneral	= nullptr;
}
/*virtual*/ void UIPhaseProfile_V11::SetInputDisable(bool bDisable)
{
	UIBase::SetInputDisable(bDisable);
	m_pMissionCard->SetInputDisable(bDisable);
	m_pDesignation->SetInputDisable(bDisable);

	if( m_pBtnGeneral ) m_pBtnGeneral->setInputDisable(true);	// 강제로 버튼 인풋 막음
}

void UIPhaseProfile_V11::ProcessGameEvent(GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
{
	//UIPhaseSubBase::ProcessGameEvent( event, arg);
	//재정의한 탭 ProcessGameEvent는 PhaseProfile에서 case별로 이벤트 호출 
	switch( m_CurrentTab)
	{
	case PROFILETAB_MISSIONCARD :
		{
			m_pMissionCard->ProcessGameEvent( event, arg, UserData);
		}
		break;

	case PROFILETAB_DESIGNATION :
		{
			m_pDesignation->ProcessGameEvent( event, arg, UserData);
		}
		break;
	}
}

/*virtual*/ bool UIPhaseProfile_V11::OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /* = I3_EVT_CODE_ACTIVATE */)
{
	//재정의한 탭 ProcessGameEvent는 PhaseProfile에서 case별로 이벤트 호출 
	UIPhaseBase::OnEvent(event,pObj,param2, code);
	switch( m_CurrentTab)
	{
	case PROFILETAB_MISSIONCARD :
		{
			m_pMissionCard->OnEvent(event,pObj,param2,code);
		}
		break;

	case PROFILETAB_DESIGNATION :
		{
			m_pDesignation->OnEvent(event,pObj,param2,code);
		}
		break;
	}
	return true;

}

bool	UIPhaseProfile_V11::OnExitStart()
{
	switch( m_CurrentTab)
	{
	case PROFILETAB_MISSIONCARD :
		m_pMissionCard->GetSlide()->SetTargetTime(false, 1.f / UI_SLIDEOUT_SLOP);
		m_pMissionCard->OnExitStart();
	break;
	case PROFILETAB_DESIGNATION :
		m_pDesignation->GetSlide()->SetTargetTime(false, 1.f / UI_SLIDEOUT_SLOP);
		m_pDesignation->OnExitStart();
	break;
	}

	return UIPhaseSubBase::OnExitStart();
}


void UIPhaseProfile_V11::OnExitEnd( void)
{
	UIPhaseSubBase::OnExitEnd();

	// 모두 꺼준다.
	switch( m_CurrentTab)
	{
	case PROFILETAB_MISSIONCARD :
		m_pMissionCard->GetSlide()->ForceEnd();
		break;
	case PROFILETAB_DESIGNATION :
		m_pDesignation->GetSlide()->ForceEnd();
		break;
	}

	//m_pRecord->SetEnable( false);
	//m_pWeaponRecord->SetEnable( false);
}

/*virtual*/ void UIPhaseProfile_V11::Leave( void)
{
	// 차후 연동 작업 시 로비로 돌아갈 때 사용
	GameEventSender::i()->SetEventOnce( EVENT_LEAVE_INFO);
}

/*virtual*/ bool UIPhaseProfile_V11::OnClick( i3UIControl * pControl)
{
	if( UIPhaseSubBase::OnClick( pControl) == false)
		return false;

	return true;
}

/*virtual*/ void UIPhaseProfile_V11::OnLoadAllScenes()
{
	UIPhaseBase::OnLoadAllScenes();
	InitTab();
	if ( g_pEnvSet->m_nUIVersion == UFV_1_1)
	{
		UIMainFrame_V11* pParent = (UIMainFrame_V11*)g_pFramework->GetUIMainframe();

		I3ASSERT(pParent != nullptr);

		pParent->GetTopMenu_V11()->ClearTabSelection();
	}
}

void UIPhaseProfile_V11::OnEntranceEnd()
{
	UIPhaseBase::OnEntranceEnd();
}

void UIPhaseProfile_V11::InitTab()
{
	m_pMissionCard->GetSlide()->SetTargetTime(true, 1.f / UI_SLIDEIN_SLOP);
	m_pMissionCard->OnEntranceStart();		// 이 함수로 대체시도. 11.06.13.수빈.
	
	m_CurrentTab = PROFILETAB_MISSIONCARD;

	LuaState * L = _CallLuaFunction( "InitTab");
	i3Lua::PushInteger(L, 0);
	_EndLuaFunction( L, 1);

	UIFloatChat_V11* pFC = static_cast<UIFloatChat_V11*>(g_pFramework->GetUIMainframe()->GetFloating(UFW_CHAT));
	pFC->OnMinimizeStart();
}


void UIPhaseProfile_V11::ChangeTab( PROFILETAB tab)
{
	m_CurrentTab = tab;
	UIFloatChat_V11* pFC = static_cast<UIFloatChat_V11*>(g_pFramework->GetUIMainframe()->GetFloating(UFW_CHAT));
	pFC->OnMinimizeStart();
	switch( tab)
	{
	case PROFILETAB_MISSIONCARD :
		{
			m_pDesignation->SetEnable(false);
			m_pMissionCard->GetSlide()->SetTargetTime(true, 0.f);
			m_pMissionCard->OnEntranceStart();		// 이 함수로 대체시도. 11.06.13.수빈.
		}
		break;

	case PROFILETAB_DESIGNATION :
		{
			m_pMissionCard->SetEnable(false);
			m_pDesignation->GetSlide()->SetTargetTime(true, 0.f);
			m_pDesignation->OnEntranceStart();		// 이 함수로 대체시도. 11.06.13.수빈.
		}
		break;
	}
}


