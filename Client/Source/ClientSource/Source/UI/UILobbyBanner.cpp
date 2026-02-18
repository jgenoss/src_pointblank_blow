#include "pch.h"
#include "UILobbyBanner.h"

#include "UINewAndHotBanner.h"

extern "C" {
	int UILobbyBanner_ClickTab( LuaState * L)
	{
		return 0;
	}
}

LuaFuncReg UILobbyBanner_Glue[] = {
	{ "ClickTab",				UILobbyBanner_ClickTab},
	{ nullptr,						nullptr}
};


I3_CLASS_INSTANCE( UILobbyBanner);//, UIFloatingBase);

UILobbyBanner::UILobbyBanner() : m_pImageBox(nullptr), m_pTex(nullptr)
{
	m_AlignedPos = UIFLOATING_POS_LEFTTOP;

#if defined( I3_DEBUG)
	SetName( "LobbyBanner");
#endif
}

UILobbyBanner::~UILobbyBanner()
{
	I3_SAFE_RELEASE( m_pTex);
}

/*virtual*/ void UILobbyBanner::_InitializeAtLoad( i3UIScene * pScene)
{
	UIFloatingBase::_InitializeAtLoad( pScene);

	m_pImageBox = (i3UIImageBoxEx*) pScene->FindChildByName( "i3UIImageBoxEx");
	I3ASSERT( m_pImageBox != nullptr);
	
	if( m_pTex!= nullptr)
	{
		m_pImageBox->AddTexture( m_pTex);
		m_pImageBox->CreateBaseSceneObjects();
	}
}

/*virtual*/ void UILobbyBanner::_ClearAtUnload( i3UIScene * pScene)
{
	I3_SAFE_RELEASE( m_pTex);
	m_pImageBox = nullptr;

	UIFloatingBase::_ClearAtUnload( pScene);
}

/*virtual*/ void UILobbyBanner::OnCreate( i3GameNode * pParent)
{
	UIFloatingBase::OnCreate( pParent);

	// Load Scene
	AddScene( "PointBlankRe_Lobby_Banner.i3UIs", UILobbyBanner_Glue);
}

void	UILobbyBanner::OnLoadAllScenes()
{
	UIFloatingBase::OnLoadAllScenes();
	m_slide.AddFrameWnd(GetFirstFrameWnd(), UISlide::L_TO_R);
}

/*virtual*/ bool UILobbyBanner::OnEntranceStart( void * pArg1, void * pArg2)
{
	m_pTex = g_pFramework->GetResourceManager()->LoadTexture( "Gui/Image/banner_01.i3i");
	if( m_pTex == nullptr)
		return false;

	if( m_pImageBox != nullptr)
	{
		LuaState * L = _CallLuaFunction( "InitTab");
		if( L != nullptr)
		{
			i3Lua::PushInteger( L, 0);
			_EndLuaFunction( L, 1);
		}

		m_pImageBox->SetCurrentImage( 0);
	}

	return UIFloatingBase::OnEntranceStart( pArg1, pArg2);
}


