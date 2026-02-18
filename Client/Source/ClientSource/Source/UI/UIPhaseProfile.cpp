#include 	"pch.h"
#include 	"UIPhaseProfile.h"

#include 	"UITabProfileBase.h"
#include 	"UITabProfileInfo.h"
#include	"UITabProfileSkill.h"
#include 	"UITabProfileRecord.h"
#include 	"UITabProfileWeaponRecord.h"
#include 	"UITabProfileMedal.h"
#include 	"UIFloatChat.h"
#include 	"UIMainFrame.h"
#include	"UIUtil.h"

extern "C" {
	int PhaseProfile_ChangeTab( LuaState * L)
	{
		INT32 tab = i3Lua::GetIntegerArg( L, 1 );
		UIPhaseProfile * pThis = static_cast<UIPhaseProfile*>(GameUI::GetPopup( UPW_PROFILE));
		if( pThis != nullptr)
		{
			bool bRes = pThis->ChangeTab( (UIPhaseProfile::PROFILETAB) tab );
			i3Lua::PushBoolean( L, bRes ? true : false );
			return 1;
		}
		return 0;
	}

	int PhaseProfile_CloseProfile( LuaState * L)
	{
		UIPhaseProfile * pThis = static_cast<UIPhaseProfile*>(GameUI::GetPopup( UPW_PROFILE));
		if( pThis != nullptr)
			pThis->OnEscapeKey();
		return 0;
	}

	int PhaseProfile_GotoBackTab( LuaState * L)
	{
		UIPhaseProfile * pThis = static_cast<UIPhaseProfile*>(GameUI::GetPopup( UPW_PROFILE));
		if( pThis != nullptr)
			pThis->GotoBackTab();
		return 0;
	}
}

LuaFuncReg PhaseProfile_Glue[] =
{
	{ "ChangeTab",		PhaseProfile_ChangeTab },
	{ "CloseProfile",	PhaseProfile_CloseProfile },

	{nullptr, nullptr}
};


I3_CLASS_INSTANCE( UIPhaseProfile);//, UIPhaseSubBase);

UIPhaseProfile::UIPhaseProfile() : m_eTab( TAB_NONE ), m_eNextTab( TAB_NONE )
{
	for( int i = 0; i < TAB_MAX; ++i )
	{
		m_pTab[i] = nullptr;
		m_pUITab[i] = nullptr;
	}
	m_EscapeInput	= true;
}

UIPhaseProfile::~UIPhaseProfile()
{
	_DestroyTab();
}

void UIPhaseProfile::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad( pScene);

	m_pUITab[ TAB_INFO] = static_cast<i3UITab*>(pScene->FindChildByName( "tab_myinfo"));
	I3ASSERT( m_pUITab[ TAB_INFO] != nullptr);
	m_pUITab[ TAB_SKILL] = static_cast<i3UITab*>(pScene->FindChildByName( "tab_skill"));
	I3ASSERT( m_pUITab[ TAB_SKILL] != nullptr);
	m_pUITab[ TAB_MEDAL] = static_cast<i3UITab*>(pScene->FindChildByName( "tab_quest"));
	I3ASSERT( m_pUITab[ TAB_MEDAL] != nullptr);
}

void UIPhaseProfile::_ClearAtUnload( i3UIScene * pScene)
{
	UIPopupBase::_ClearAtUnload( pScene);
	for( INT32 i = 0; i < TAB_MAX; ++i )
		m_pUITab[i] = nullptr;
}

void UIPhaseProfile::SetCurrentTab( PROFILETAB tab)
{
	LuaState * L = _CallLuaFunction( "UpdateTabButton");
	if( L != nullptr)
	{
		I3ASSERT( tab > TAB_NONE && tab < TAB_MAX);
		i3Lua::PushInteger(L, (INT32) tab);
		i3Lua::PushUserData(L, m_pUITab[ tab]);
		_EndLuaFunction(L , 2);
	}
}

bool	UIPhaseProfile::ChangeTab( PROFILETAB eTab )
{
	//if( m_eTab == eTab )
	if( m_eNextTab == eTab )	// hansoft 11846. 탭 상태 동기화 처리.
	{
		return false;
	}

	m_EscapeInput = false;
	if( GetCurrentTab() )
	{
		if( GetCurrentTab()->RequestChangeTab() )
		{
			m_eNextTab = eTab;
		}
		else
		{
			m_EscapeInput = true;
			return false;
		}
	}
	else
	{		
		m_eNextTab = eTab;		
		NextTabProcess();
	}

	return true;
}

void	UIPhaseProfile::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);
	AddScene( "Scene/Main/PointBlankRe_Profile_Menu.i3UIs", PhaseProfile_Glue );

	_CreateTab();
}

bool	UIPhaseProfile::OnEntranceStart( void * pArg1, void * pArg2 )
{
	if(UIPopupBase::OnEntranceStart(pArg1, pArg2) == false)
		return false;

	m_eTab = TAB_INFO;
	m_eNextTab = TAB_NONE;
	m_EscapeInput = true;
	SetBackFrameEnable(false);

	if( GetCurrentTab() )
	{
		UISlide* slide = GetCurrentTab()->GetSlide();
		if( slide )
			slide->SetTargetTime( true, 0.f);

		bool resTab = GetCurrentTab()->OnEntranceStart();
		I3ASSERT_RETURN(resTab == true, false);
	}

	LuaState * L = _CallLuaFunction("InitTabState");
	if(L != nullptr)
	{
		// 자기 정보 = true , 타인 정보 = false
		i3Lua::PushBoolean(L, CCommunityContext::i()->BeCommunityUserInfo() == true ? false : true);
		// 퀘스트 오픈 여부
		i3Lua::PushBoolean(L, false);
		_EndLuaFunction(L , 2);
	}

	return true;
}

bool	UIPhaseProfile::OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /* = I3_EVT_CODE_ACTIVATE */)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if( event == I3_EVT_INPUT)
		{
			i3InputDeviceManager* pMgr = (i3InputDeviceManager*)pObj;
			i3InputKeyboard* pKeyboard = pMgr->GetKeyboard();

			if (pKeyboard->GetStrokeState(I3I_KEY_ESC))
			{
				if(m_EscapeInput)
				{
					OnEscapeKey();
					return true;
				}
			}
		}
	}

	return UIPopupBase::OnEvent(event, pObj, param2, code);
}

/*vurtial*/ bool UIPhaseProfile::OnPlayClickSound(i3UIControl * pCtrl)
{
	if( i3::same_of<i3UIButton*>(pCtrl) && 
		i3::generic_is_iequal(pCtrl->GetName() , "btn_Close")  )
	{
		return PlaySound(UISND_ITEM_CLICK_CANCEL);
	}

	return UIPopupBase::OnPlayClickSound(pCtrl);
}

void	UIPhaseProfile::SetInputDisable(bool bDisable)
{
	UIPopupBase::SetInputDisable(bDisable);
	for( int i = 0; i < TAB_MAX; ++i )
	{
		if( m_pTab[i] )
			m_pTab[i]->SetInputDisable( bDisable );
	}
}

void	UIPhaseProfile::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData )
{
	if( GetCurrentTab() )
		GetCurrentTab()->ProcessGameEvent( event, arg , UserData);
}

void	UIPhaseProfile::Leave( void)
{
	if(CCommunityContext::i()->BeCommunityUserInfo() == false)
	{
		//GameEventSender::i()->SetEventOnce(EVENT_LEAVE_INFO);
	}
	else
	{
		GetMainFrame()->EnterDelayedEventSubPhase();
	}

	CCommunityContext::i()->SetCommunityUserInfo(false);
}

void	UIPhaseProfile::_CreateTab()
{
	i3ClassMeta* pMeta = nullptr;

	for( int i = 0; i < TAB_MAX; ++i )
	{
		pMeta = nullptr;

		switch( i )
		{
		case	TAB_INFO:
				pMeta = UITabProfileInfo::static_meta();
				break;

		case	TAB_SKILL:
				pMeta = UITabProfileSkill::static_meta();
				break;

		case	TAB_MEDAL:
				pMeta = UITabProfileMedal::static_meta();
				break;
				/*
		default:
			pMeta = nullptr;
			I3PRINTLOG(I3LOG_FATAL,  "Not Setted phase class...!!!");
			break;
			*/
		}

		if( nullptr != pMeta )
		{
			m_pTab[i] = (UITabProfileBase*) pMeta->create_instance();	//CREATEINSTANCE( pMeta );
			I3ASSERT( nullptr != m_pTab[i] );	

			if( m_pTab[i] )
			{
				m_pTab[i]->OnCreate( this );
				m_pTab[i]->SetNextProcess( &UIPhaseProfile::cbNextTabProcessEvent, (void*)this );
				I3_MUST_ADDREF(m_pTab[i]);

				I3ASSERT( m_pTab[i]->same_of( pMeta ) );
			}
		}
	}
}

void	UIPhaseProfile::NextTabProcess()
{
	m_eTab = m_eNextTab;
	m_eNextTab = TAB_NONE;

	if(m_eTab == TAB_INFO)
		SetBackFrameEnable(false);
	else
		SetBackFrameEnable(true);

	if( GetCurrentTab() )
	{
		GetCurrentTab()->GetSlide()->SetTargetTime( true, 0.f );				
		bool resTab = GetCurrentTab()->OnEntranceStart();
		I3ASSERT(resTab);
	}
}


void	UIPhaseProfile::cbNextTabProcessEvent( void* ptr )
{
	if( nullptr == ptr )
	{
		return;
	}

	UIPhaseProfile* phase = (UIPhaseProfile*) ptr;

	if( phase )
		phase->NextTabProcess();
}

bool	UIPhaseProfile::OnExitStart()
{
	for(INT32 i = 0 ; i < TAB_MAX; i ++)
	{
		if( m_pTab[i]->isEnable() == true)
		{
			m_pTab[i]->GetSlide()->SetTargetTime( false, 0.0f);
			m_pTab[i]->OnExitStart();
		}
	}
	m_eTab = TAB_NONE;
	m_eNextTab = TAB_NONE;

	GetSlide()->SetTargetTime(false, 0.0f);
	return UIPopupBase::OnExitStart();
}

void	UIPhaseProfile::OnExitEnd()
{
	UITabProfileBase* tab = GetCurrentTab();

	if( tab )
	{
		tab->GetSlide()->ForceEnd();

		tab->SetEnable(false); // 원래 이 시점에 tab이 사라져야 함이 정상인데, 프레임 차이로 살아있는경우... 없애준다.
	}


	return UIPopupBase::OnExitEnd();
}

void	UIPhaseProfile::OnEscapeKey()
{
	bool bExit = true;

	if( GetCurrentTab() )
	{
		bExit = GetCurrentTab()->RequestExitTab();
	}
	
	GetMainFrame()->GetTopMenu()->ClickPopupBackButton(true);

	CCommunityContext::i()->SetCommunityUserInfo(false);
}

void	UIPhaseProfile::OnEntranceEnd()
{
	UIPopupBase::OnEntranceEnd();

	// Key : User Guide System
	{
		UserGuideMgr::i()->OnLoadGuideTargetInfo(this);
	}
}

void	UIPhaseProfile::_DestroyTab()
{
	for( int i = 0; i < TAB_MAX; ++i )
	{
		if( nullptr == m_pTab[i] )
		{
			continue;
		}

		m_pTab[i]->Destroy();
		m_pTab[i] = nullptr;
	}
}

void	UIPhaseProfile::GotoBackTab()
{
	GetCurrentTab()->GotoBackTab();
}

void	UIPhaseProfile::SetBackFrameEnable(bool bEnable)
{
	LuaState * L = _CallLuaFunction("EnableBackFrame");

	if(L != nullptr)
	{
		i3Lua::PushBoolean(L , bEnable == true ? true : false);
		_EndLuaFunction(L , 1);
	}
}