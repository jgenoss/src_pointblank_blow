#include "pch.h"
#include "UILobbyBanner_V11.h"

extern "C" {
	int UILobbyBanner_V11_ClickTab( LuaState * L)
	{
		return 0;
	}
}

LuaFuncReg UILobbyBanner_V11_Glue[] = {
	{ "ClickTab",				UILobbyBanner_V11_ClickTab},
	{ nullptr,						nullptr}
};


I3_CLASS_INSTANCE( UILobbyBanner_V11);

UILobbyBanner_V11::UILobbyBanner_V11()
{
	m_pImageBox		= nullptr;
	m_pTex			= nullptr;
	m_AlignedPos	= UIFLOATING_POS_LEFTTOP;

#if defined( I3_DEBUG)
	SetName( "LobbyBanner");
#endif
}

UILobbyBanner_V11::~UILobbyBanner_V11()
{
	I3_SAFE_RELEASE( m_pTex);
}

/*virtual*/ void UILobbyBanner_V11::_InitializeAtLoad( i3UIScene * pScene)
{
	UIFloatingBase::_InitializeAtLoad( pScene);

	m_pImageBox = (i3UIImageBoxEx*) pScene->FindChildByName( "i3UIImageBoxEx");
	I3ASSERT( m_pImageBox != nullptr);
	
	if( m_pTex!= nullptr)
	{
		m_pImageBox->AddTexture( m_pTex);

		m_pImageBox->CreateBaseSceneObjects();

		LuaState * L = _CallLuaFunction( "InitTab");
		if( L != nullptr)
		{
			i3Lua::PushInteger( L, 0);
			_EndLuaFunction( L, 1);
		}

		m_pImageBox->SetCurrentImage( 0);
	}
}

/*virtual*/ void UILobbyBanner_V11::_ClearAtUnload( i3UIScene * pScene)
{
	I3_SAFE_RELEASE( m_pTex);
	m_pImageBox = nullptr;

	UIFloatingBase::_ClearAtUnload( pScene);
}

/*virtual*/ void UILobbyBanner_V11::OnCreate( i3GameNode * pParent)
{
	UIFloatingBase::OnCreate( pParent);

	// Load Scene
	AddScene( "Scene/Popup/PointBlankRe_Lobby_Banner.i3UIs", UILobbyBanner_V11_Glue);
}

void	UILobbyBanner_V11::OnLoadAllScenes()
{
	UIFloatingBase::OnLoadAllScenes();
	m_slide.AddFrameWnd(GetFirstFrameWnd(), UISlide::L_TO_R);
}

/*virtual*/ void UILobbyBanner_V11::SetEnable( bool bTrue /*= true*/, REAL32 tm /*= 0.f*/)
{
	UIFloatingBase::SetEnable( bTrue, tm);
}

/*virtual*/ bool UILobbyBanner_V11::OnEntranceStart( void * pArg1, void * pArg2)
{
	m_pTex = g_pFramework->GetResourceManager()->LoadTexture( "Gui/Image/banner_01.i3i");
	if( m_pTex == nullptr)
		return false;

	return UIFloatingBase::OnEntranceStart( pArg1, pArg2);
}

/*virtual*/ void UILobbyBanner_V11::OnSlideIn( void)
{
	UIFloatingBase::OnSlideIn();
}

/*virtual*/ void UILobbyBanner_V11::OnSlideOut( void)
{
	UIFloatingBase::OnSlideOut();
}

