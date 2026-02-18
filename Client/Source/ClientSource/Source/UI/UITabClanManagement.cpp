#include "pch.h"

#include "UIMainFrame.h"
#include "UIPhaseClanMember.h"
#include "UITabClanManagement.h"

#include "UISubTabClanMgt_Join.h"
#include "UISubTabClanMgt_Member.h"
#include "UISubTabClanMgt_Info.h"
#include "UISubTabClanMgt_Etc.h"
#include "UISubTabClanMgt_Control.h"

#include "UIUtil.h"

#include "../GameGUI.h"

static UITabClanManagement * GetThis( void)
{
	UIPhaseClanMember* pPhase =	static_cast<UIPhaseClanMember*>(GameUI::GetPopup(UPW_CLAN_MEMBER));
	if( pPhase != nullptr)
	{
		return static_cast<UITabClanManagement*>(pPhase->GetTab(UTC_MGT));
	}
	return nullptr;
}

//----------------------------------------------------------------------------------//
//								Lua Glue Function									//
//----------------------------------------------------------------------------------//
extern "C" {
	INT32 UITabClanManagement_SelectTab(LuaState* L)
	{
		INT32 tab = i3Lua::GetIntegerArg( L, 1);
		UITabClanManagement * pTab = GetThis();
		if( pTab != nullptr)
			pTab->ChangeSubTab((UISubTabClanMgtType) tab);
		return 0;
	}
}

LuaFuncReg UITabClanManagement_Glue[] = {
	{"SelectTab",			UITabClanManagement_SelectTab},
	{ nullptr,					nullptr}
};

I3_CLASS_INSTANCE( UITabClanManagement);//, UITabClan_Normal);

UITabClanManagement::UITabClanManagement()
{
	m_eCurrentTab = USTC_MGT_JOIN;
	m_eNextTab = USTC_MGT_NONE;

	for( INT32 i = 0; i < USTC_MGT_MAX; ++i)
		m_pClanMgtTab[i] = nullptr;
}

UITabClanManagement::~UITabClanManagement()
{
	for( INT32 i = 0; i < USTC_MGT_MAX; ++i)
		if( m_pClanMgtTab[i]) m_pClanMgtTab[ i]->Destroy();
}

/*virtual*/ void UITabClanManagement::OnCreate( i3GameNode * pParent)
{
	UITabClan_Normal::OnCreate( pParent);

	// add scene
	AddScene("Scene/Clan/PointBlankRe_Clan_Management_Main.i3UIs", UITabClanManagement_Glue);

	i3ClassMeta * pMeta = nullptr;

	for (INT32 i = 0 ; i < USTC_MGT_MAX ; ++i)
	{
		switch( i)
		{	
		case USTC_MGT_JOIN :	pMeta = UISubTabClanMgt_Join::static_meta();	break;
		case USTC_MGT_GRADE :	pMeta = UISubTabClanMgt_Member::static_meta();	break;
		case USTC_MGT_INFO :	pMeta = UISubTabClanMgt_Info::static_meta();	break;
		case USTC_MGT_CONTROL :	pMeta = UISubTabClanMgt_Control::static_meta();	break;
			/*
		default :
			pMeta = nullptr;
			I3PRINTLOG(I3LOG_FATAL,  "Not Setted SubTab class...!!!");
			break;
			*/
		}

		if( pMeta != nullptr)
		{
			m_pClanMgtTab[ i] = (UISubTabClan_Mgt*) pMeta->create_instance(); //CREATEINSTANCE( pMeta);
		//	I3ASSERT( m_pClanMgtTab[ i] != nullptr);
			I3ASSERT( m_pClanMgtTab[ i]->same_of( pMeta));

			m_pClanMgtTab[i]->OnCreate( this);
			I3_MUST_ADDREF(m_pClanMgtTab[i]);
		}
	}
}

/*virtual*/ bool UITabClanManagement::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UITabClan_Normal::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	m_eCurrentTab = USTC_MGT_JOIN;

	bool resTab = GetCurrentTab()->OnEntranceStart(pArg1, pArg2);

	I3ASSERT(resTab);

	return true;
}

/*virtual*/ bool UITabClanManagement::OnExitStart(void)
{
	m_eNextTab = USTC_MGT_NONE;

	GetCurrentTab()->GetSlide()->SetTargetTime(false, 0.f);
	GetCurrentTab()->OnExitStart();

	for (INT32 i=0; i<USTC_MGT_MAX; i++)
	{
		if (m_pClanMgtTab[i])
		{
			m_pClanMgtTab[i]->GetSlide()->ForceEnd();
			m_pClanMgtTab[i]->SetEnable(false);
		}
	}

	return UITabClan_Normal::OnExitStart();
}

/*virtual*/ void UITabClanManagement::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
{
	UITabClan_Normal::ProcessGameEvent( event, arg, UserData);

	GetCurrentTab()->ProcessGameEvent(event, arg, UserData);
}

void UITabClanManagement::ChangeSubTab( UISubTabClanMgtType tab)
{
	if(m_eCurrentTab == tab)
		return;

	CClanContext * ClanContext;
	ClanContext = g_pFramework->GetClanContext();

	m_eNextTab = tab;

	GetCurrentTab()->GetSlide()->SetTargetTime(false, 0.f);
	GetCurrentTab()->OnExitStart();			
}

void UITabClanManagement::ProcessNextTab( void)
{
	if(m_eNextTab != USTC_MGT_NONE)
	{
		m_eCurrentTab = m_eNextTab;
		m_eNextTab = USTC_MGT_NONE;

		if( GetCurrentTab() )
		{
			GetCurrentTab()->GetSlide()->SetTargetTime( true, 0.f );				
			bool resTab = GetCurrentTab()->OnEntranceStart();
			I3ASSERT(resTab);
		}
	}
}