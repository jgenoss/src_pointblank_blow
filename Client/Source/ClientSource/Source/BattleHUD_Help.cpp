#include "pch.h"
#include "BattleHUD_Help.h"

#include "ConfigRenderBattle.h"
#include "UserInfoContext.h"

CHUD_Help::CHUD_Help(i3AttrSet* pHudRoot)
{
	m_pSpriteHelper = nullptr;
	m_pSpriteHelper_Observer = nullptr;
	m_pTextureHelper = nullptr;
	m_pTextureHelper_Observer = nullptr;

	i3Viewer * pViewer = g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();
	INT32	nScreenHeight	= pViewer->GetViewHeight();

	// HUD Helper

	m_pTextureHelper = crb::i()->LoadHudTexture(HUD_TEXTURE_HELP);

	m_pSpriteHelper = i3Sprite2D::new_object();
	m_pSpriteHelper->Create( 1, true, true );
	m_pSpriteHelper->SetTexture(m_pTextureHelper);
	m_pSpriteHelper->SetEnable( false);
	m_pSpriteHelper->SetRect( 0, (REAL32)(nScreenWidth/2 - 326), (REAL32)(nScreenHeight/2 - 256), (652.f), (512.0f));
	m_pSpriteHelper->SetColor( 0, 255,255,255,255);
	m_pSpriteHelper->SetTextureCoord( 0, (0.f), (0.f), (652.0f), (512.0f));


	pHudRoot->AddChild( m_pSpriteHelper);

	m_pTextureHelper_Observer = crb::i()->LoadHudTexture(HUD_TEXTURE_HELP_OBSERVER);
	m_pSpriteHelper_Observer = i3Sprite2D::new_object();
	m_pSpriteHelper_Observer->Create(1, TRUE, TRUE);
	m_pSpriteHelper_Observer->SetTexture(m_pTextureHelper_Observer);
	m_pSpriteHelper_Observer->SetEnable(FALSE);
	m_pSpriteHelper_Observer->SetRect(0, (REAL32)(nScreenWidth / 2 - 326), (REAL32)(nScreenHeight / 2 - 256), (652.f), (512.0f));
	m_pSpriteHelper_Observer->SetColor(0, 255, 255, 255, 255);
	m_pSpriteHelper_Observer->SetTextureCoord(0, (0.f), (0.f), (652.0f), (512.0f));


	pHudRoot->AddChild(m_pSpriteHelper_Observer);
}

CHUD_Help::~CHUD_Help(void)
{
	I3_SAFE_NODE_RELEASE( m_pSpriteHelper );
	I3_SAFE_NODE_RELEASE(m_pSpriteHelper_Observer);
	I3_SAFE_RELEASE(m_pTextureHelper);
	I3_SAFE_RELEASE(m_pTextureHelper_Observer);
}

void CHUD_Help::OnUpdate( REAL32 rDeltaSeconds )
{
	if( g_pFramework ->getKeyboard()->GetPressState( I3I_KEY_F1 ) )
	{
		if (UserInfoContext::i()->IsGM_Observer())
		{
			m_pSpriteHelper_Observer->SetEnable(TRUE);
		}
		else
		{
			m_pSpriteHelper->SetEnable(TRUE);
		}
	}
	else
	{
		if (UserInfoContext::i()->IsGM_Observer())
		{
			m_pSpriteHelper_Observer->SetEnable(FALSE);
		}
		else
		{
			m_pSpriteHelper->SetEnable(FALSE);
		}
	}
}