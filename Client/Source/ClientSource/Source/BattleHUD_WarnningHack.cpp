#include "pch.h"
#include "BattleHud_WarnningHack.h"

#include "BattleHud.h"
#include "StageBattle.h"

I3_CLASS_INSTANCE( CHud_WarnningHack);//, i3GameObjBase);

namespace inner { CHud_WarnningHack* this_ptr = 0; }
CHud_WarnningHack* CHud_WarnningHack::instance() { return inner::this_ptr; }

CHud_WarnningHack::CHud_WarnningHack(void)
{
	inner::this_ptr = this;

	m_pHud = nullptr;
	m_pTexture = nullptr;
	m_pSpriteBG = nullptr;	
	m_nPosX = 0;
	m_nPosY = 0;

	m_fLocalTime = 0.0f;
	m_bActivate = false;
}

CHud_WarnningHack::~CHud_WarnningHack(void)
{	
	inner::this_ptr = 0;

	I3_SAFE_RELEASE( m_pTexture);
	I3_SAFE_NODE_RELEASE( m_pSpriteBG);
}

void CHud_WarnningHack::Create(CHud * pHud)
{
	m_pHud = pHud;
}

void CHud_WarnningHack::CreateResource(void)
{
	if( m_pSpriteBG != nullptr) return;

	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= 0;	

	if( pViewer != nullptr)
	{
		nScreenWidth	= pViewer->GetViewWidth();	
	}


	m_nPosX = (INT32)((nScreenWidth * 0.5f) - (830.0f * i3GuiRoot::getWidthRatioBy768() * 0.5f));
	m_nPosY = (INT32)(60.0f * i3GuiRoot::getHeightRatioBy768());


	m_pTexture = g_pFramework->GetResourceManager()->LoadTexture("Gui/Ingame/pb-auto.i3i");

	m_pSpriteBG = i3Sprite2D::new_object();	
	I3ASSERT( m_pSpriteBG != nullptr);
	m_pSpriteBG->Create(1, true, true);
	m_pSpriteBG->SetTexture( m_pTexture );

	// Body
	{
		m_pSpriteBG->SetEnable( 0, false);
		m_pSpriteBG->SetRect( 0, (REAL32)m_nPosX, (REAL32)m_nPosY, (830.0f * i3GuiRoot::getWidthRatioBy768()), (590.0f * i3GuiRoot::getHeightRatioBy768()));
		m_pSpriteBG->SetColor( 0, 255,255,255,255);
		m_pSpriteBG->SetTextureCoord( 0, 0.0f, 0.0f, 830.0f, 590.0f);
	}

	m_pHud->getHUDRoot()->AddChild( m_pSpriteBG);

	m_pSpriteBG->SetEnable( true);
}

void CHud_WarnningHack::SetActivate(void)
{
	m_bActivate = true;

	CreateResource();
}

void CHud_WarnningHack::OnUpdate( REAL32 rDeltaSeconds)
{
	i3GameObjBase::OnUpdate( rDeltaSeconds );

	if( m_bActivate)
	{
		m_fLocalTime += rDeltaSeconds;

		if( m_fLocalTime > 10.0f)
		{
			// 강제종료..
			// GOTO_EXIT_STAGE();
			GameEventSender::i()->SetEvent( EVENT_GAME_END ); // 2012.03.10 준영. 로그아웃 요청 패킷 전송
			m_bActivate = false;	
		}
	}
}
