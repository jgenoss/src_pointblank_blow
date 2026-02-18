#include "pch.h"
#include "RecLiveIcon.h"

#include "i3Framework/i3UI/i3UIFilePathMgr.h"

I3_CLASS_INSTANCE( CRecLiveIcon);

CRecLiveIcon::CRecLiveIcon(void) : m_fNotifyDelay(0.f), m_bEnable(false)
{
	m_pImageBox1	= nullptr;			// µÕ±Ù ºû ²¨Á®ÀÖ´Â ÀÌ¹ÌÁö
	m_pImageBox2	= nullptr;			// µÕ±Ù ºû ÄÑÁ®ÀÖ´Â ÀÌ¹ÌÁö
	m_pStaticText	= nullptr;			// REC, LIVE, REC & LIVE	
	m_pScene		= nullptr;
}

CRecLiveIcon::~CRecLiveIcon(void)
{
}

void CRecLiveIcon::InitIcon()
{
	VEC3D vZero;
	VEC2D vSize;

	i3::rc_string strCommonUIFolder = i3UIFilePathMgr::i()->Retrieve_CommonUIRootDirectory(false);

	i3::stack_string strFullPath;
	i3::sprintf(strFullPath, "%s/Scene/HUD/PBRe_HUD_Live_Rec.i3UIs", strCommonUIFolder);
	i3UIScene* pUIScene = i3UI::getManager()->LoadUIScene(strFullPath.c_str());
	i3UITemplate * s_pToolTipTemplate = nullptr;
	i3UILibrary * pLib = i3UI::getUILibrary();
	if( pLib != nullptr)
	{
		s_pToolTipTemplate = pLib->FindByName( "ToolTip");
		
		if (s_pToolTipTemplate != nullptr)
		{
			i3UIToolTip * pToolTip = (i3UIToolTip*)i3UI::CreateControl(s_pToolTipTemplate, &vZero, &vSize, pUIScene);
			pUIScene->setDefToolTip(pToolTip);
			pUIScene->Attach(true);
			pUIScene->EnableScene(false);
		}
	}
	
	m_pImageBox1	= (i3UIImageBox*) pUIScene->FindChildByName("i3UIImageBox0");
	m_pImageBox2	= (i3UIImageBox*) pUIScene->FindChildByName("i3UIImageBox1");
	m_pStaticText	= (i3UIStaticText *) pUIScene->FindChildByName("i3UIStaticText");
	
	m_pImageBox1->SetVisible(true);
	m_pImageBox2->SetVisible(false);

	m_pScene = pUIScene;
}

void CRecLiveIcon::OnUpdate( bool isTwitchStreaming, bool isVideoCapture )
{
	if( !isTwitchStreaming && !isVideoCapture )
	{
		if(m_bEnable)
		{
			m_pScene->EnableScene( false );
			g_pFramework->RemovePostDrawUIScene( m_pScene );
			m_bEnable = false;
		}
		return;
	}
	
	if(!m_bEnable)
	{
		m_pScene->EnableScene( true );
		g_pFramework->AddPostDrawUIScene( m_pScene );
		m_bEnable = true;
	}

	if( isTwitchStreaming )
		m_pStaticText->SetText(GAME_STRING("STR_OPT_RECORDING_LIVE"));
	if ( isVideoCapture )
		m_pStaticText->SetText(GAME_STRING("STR_OPT_RECORDING_REC"));
	if( isTwitchStreaming && isVideoCapture )
		m_pStaticText->SetText(GAME_STRING("STR_OPT_RECORDING_BOTH"));

	REAL32 rDelta = g_pViewer->getDeltaTime();
	
	m_fNotifyDelay += rDelta;

	if(m_fNotifyDelay > 0.5f)
	{
		if(m_pImageBox1->isVisible())
		{
			m_pImageBox1->SetVisible(false);
			m_pImageBox2->SetVisible(true);
		}
		else
		{
			m_pImageBox1->SetVisible(true);
			m_pImageBox2->SetVisible(false);
		}
		m_fNotifyDelay = 0.0f;
	}
}