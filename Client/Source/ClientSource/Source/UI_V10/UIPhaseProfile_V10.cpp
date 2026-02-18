#include "pch.h"
#include "UIPhaseProfile_V10.h"

#include "UITabProfileDesignation_V10.h"
#include "UITabProfileMissionCard_V10.h"
#include "UIFloatChat_V10.h"
#include "UITopMenu_V10.h"

#include "Designation/DesigUtil.h"
#include "Designation/UserDesigInfo.h"
#include "Designation/DesigAttr.h"
#include "Designation/DesigSet.h"

#include "UI/UIShopFunction.h"
#include "UserInfoContext.h"

#include "UI/UIMainFrame_V10.h"

static UIPhaseProfile_V10 * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);

	return (UIPhaseProfile_V10*)pFrame->GetSubPhase( USP_PROFILE);
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int PhaseProfile_ChangeTab_V10( LuaState * L)
	{
		PROFILETAB_V10 tab = (PROFILETAB_V10) i3Lua::GetIntegerArg( L, 1);

		GetThis()->ChangeTab( tab);
		return 0;
	}
	
	int PhaseProfile_ClickExit( LuaState * L)
	{
		GetThis()->OnKey_Escape();
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

LuaFuncReg PhaseProfile_V10_Glue[] =
{
	{"ChangeTab",				PhaseProfile_ChangeTab_V10			},
	{"ClickExit",				PhaseProfile_ClickExit				},
	//	{"ClickDesignation",		PhaseProfile_ClickDesignation	},
	{nullptr,		nullptr}
};


I3_CLASS_INSTANCE( UIPhaseProfile_V10);

UIPhaseProfile_V10::UIPhaseProfile_V10() : m_pBtnGeneral( nullptr )
{
	m_pDesignation = nullptr;
}

UIPhaseProfile_V10::~UIPhaseProfile_V10()
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
/*virtual*/ void UIPhaseProfile_V10::OnCreate( i3GameNode * pParent)
{
	UIPhaseSubBase::OnCreate( pParent);

	// Load Scene
	AddScene( "Scene/Main/PointBlankRe_MyInfomation.i3UIs", PhaseProfile_V10_Glue);

	// Create Tab
	m_pDesignation = UITabProfileDesignation_V10::new_object();
	I3ASSERT( m_pDesignation != nullptr);
	m_pDesignation->OnCreate( this);

	m_pMissionCard = UITabProfileMissionCard_V10::new_object();
	I3ASSERT( m_pMissionCard != nullptr);
	m_pMissionCard->OnCreate( this);

	//m_pRecord = UITabProfileRecord::new_object();
	//I3ASSERT( m_pRecord != nullptr);
	//	m_pRecord->OnCreate( this);

	//	m_pWeaponRecord = UITabProfileWeaponRecord::new_object();
	//	I3ASSERT( m_pWeaponRecord != nullptr);
	//	m_pWeaponRecord->OnCreate( this);
}

/*virtual*/ void UIPhaseProfile_V10::_InitializeAtLoad( i3UIScene * pScene )
{
	UIPhaseBase::_InitializeAtLoad(pScene);

	//m_pDesignation->BindScene(pScene);
}

/*virtual*/ void UIPhaseProfile_V10::_ClearAtUnload( i3UIScene * pScene )
{
	UIPhaseBase::_ClearAtUnload(pScene);

	m_pBtnGeneral	= nullptr;
}

/*virtual*/ void UIPhaseProfile_V10::SetInputDisable(bool bDisable)
{
	UIBase::SetInputDisable(bDisable);
	m_pMissionCard->SetInputDisable(bDisable);
	m_pDesignation->SetInputDisable(bDisable);

	if( m_pBtnGeneral ) m_pBtnGeneral->setInputDisable(true);	// 강제로 버튼 인풋 막음
}

void UIPhaseProfile_V10::ProcessGameEvent(GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
{
	//UIPhaseSubBase::ProcessGameEvent( event, arg);
	//재정의한 탭 ProcessGameEvent는 PhaseProfile에서 case별로 이벤트 호출 
	switch( m_CurrentTab)
	{
	case PROFILETAB_MISSIONCARD_V10 :
		{
			m_pMissionCard->ProcessGameEvent( event, arg, UserData);
		}
		break;

	case PROFILETAB_DESIGNATION_V10 :
		{
			m_pDesignation->ProcessGameEvent( event, arg, UserData);
		}
		break;
	}
}

/*virtual*/ bool UIPhaseProfile_V10::OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /* = I3_EVT_CODE_ACTIVATE */)
{
	//재정의한 탭 ProcessGameEvent는 PhaseProfile에서 case별로 이벤트 호출 
	UIPhaseBase::OnEvent(event,pObj,param2, code);
	switch( m_CurrentTab)
	{
	case PROFILETAB_MISSIONCARD_V10 :
		{
			m_pMissionCard->OnEvent(event,pObj,param2,code);
		}
		break;

	case PROFILETAB_DESIGNATION_V10 :
		{
			m_pDesignation->OnEvent(event,pObj,param2,code);
		}
		break;
	}

	return true;
}

bool	UIPhaseProfile_V10::OnExitStart()
{
	switch( m_CurrentTab)
	{
	case PROFILETAB_MISSIONCARD_V10 :
		m_pMissionCard->GetSlide()->SetTargetTime(false, 1.f / UI_SLIDEOUT_SLOP);
		m_pMissionCard->OnExitStart();
		break;
	case PROFILETAB_DESIGNATION_V10 :
		m_pDesignation->GetSlide()->SetTargetTime(false, 1.f / UI_SLIDEOUT_SLOP);
		m_pDesignation->OnExitStart();
		break;
	}

	return UIPhaseSubBase::OnExitStart();
}

void UIPhaseProfile_V10::OnExitEnd( void)
{
	UIPhaseSubBase::OnExitEnd();

	// 모두 꺼준다.
	switch( m_CurrentTab)
	{
	case PROFILETAB_MISSIONCARD_V10 :
		m_pMissionCard->GetSlide()->ForceEnd();
		break;
	case PROFILETAB_DESIGNATION_V10 :
		m_pDesignation->GetSlide()->ForceEnd();
		break;
	}

	//m_pRecord->SetEnable( false);
	//m_pWeaponRecord->SetEnable( false);
}

/*virtual*/ void UIPhaseProfile_V10::Leave( void)
{
	UIMainFrame_V10 * pUIMainFrame = ((UIMainFrame_V10*)g_pFramework->GetUIMainframe());
	I3ASSERT( i3::kind_of<UIMainFrame_V10*>(pUIMainFrame));

	pUIMainFrame->GetTopMenu_V10()->AllMenuBtnShapeToNormal();

	// 차후 연동 작업 시 로비로 돌아갈 때 사용
	GameEventSender::i()->SetEventOnce( EVENT_LEAVE_INFO);
}

/*virtual*/ bool UIPhaseProfile_V10::OnClick( i3UIControl * pControl)
{
	if( UIPhaseSubBase::OnClick( pControl) == false)
		return false;

	return true;
}


/*virtual*/ void UIPhaseProfile_V10::OnLoadAllScenes()
{
	UIPhaseBase::OnLoadAllScenes();

	m_slide.AddFrameWnd(GetFirstFrameWnd(), UISlide::R_TO_L);
	InitTab();
	
}

void UIPhaseProfile_V10::OnEntranceEnd()
{
	UIPhaseBase::OnEntranceEnd();
}

/*virtual*/ bool UIPhaseProfile_V10::OnKey_Escape()
{
	if( i3UI::getDisableUIInput() == true)
		return false;
	
	// 차후 연동 작업 시 로비로 돌아갈 때 사용
	GetMainFrame()->ExclusedEnterSubPhase( USP_PROFILE);		

	return true;
}

void UIPhaseProfile_V10::InitTab()
{
	m_pMissionCard->GetSlide()->SetTargetTime(true, 1.f / UI_SLIDEIN_SLOP);
	m_pMissionCard->OnEntranceStart();		// 이 함수로 대체시도. 11.06.13.수빈.

	m_CurrentTab = PROFILETAB_MISSIONCARD_V10;

	LuaState * L = _CallLuaFunction( "InitTab");
	i3Lua::PushInteger(L, 0);
	_EndLuaFunction( L, 1);

	/*m_pDesignation->GetSlide()->SetTargetTime(true, 1.f / UI_SLIDEIN_SLOP);
	m_pDesignation->OnEntranceStart();*/
	//m_CurrentTab = PROFILETAB_DESIGNATION_V10; 

}


void UIPhaseProfile_V10::ChangeTab( PROFILETAB_V10 tab)
{
	m_CurrentTab = tab;

	switch( tab)
	{
	case PROFILETAB_MISSIONCARD_V10 :
		{
			m_pDesignation->SetEnable(false);
			m_pMissionCard->GetSlide()->SetTargetTime(true, 0.f);
			m_pMissionCard->OnEntranceStart();
		}
		break;

	case PROFILETAB_DESIGNATION_V10 :
		{
			m_pMissionCard->SetEnable(false);
			m_pDesignation->GetSlide()->SetTargetTime(true, 0.f);
			m_pDesignation->OnEntranceStart();		// 이 함수로 대체시도. 11.06.13.수빈.
		}
		break;
	}
}
