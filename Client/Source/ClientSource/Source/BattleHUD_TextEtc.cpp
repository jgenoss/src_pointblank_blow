#include "pch.h"
#include "BattleHUD_TextEtc.h"

#include "i3Base/profile/profile.h"

I3_CLASS_INSTANCE(CHUD_TextEtc);

namespace inner { CHUD_TextEtc* this_ptr = 0; }
CHUD_TextEtc* CHUD_TextEtc::instance() { return inner::this_ptr; }

CHUD_TextEtc::CHUD_TextEtc(void)
{
	inner::this_ptr = this;

	m_pTextEtc = nullptr;
	//m_pSpriteEtcMessageBG = nullptr;
	m_fEtcTextTime = 0.0f;
}

CHUD_TextEtc::~CHUD_TextEtc(void)
{
	inner::this_ptr = 0;

	I3_SAFE_NODE_RELEASE( m_pTextEtc );
	//I3_SAFE_NODE_RELEASE(m_pSpriteEtcMessageBG);
}

void CHUD_TextEtc::CreateHUD(i3AttrSet* pHudRoot, i3Texture* pTexture)
{
	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();
	INT32	nScreenHeight	= pViewer->GetViewHeight();

	m_pTextEtc = i3TextNodeDX2::new_object();
#if defined ( LOCALE_TAIWAN ) // revision 55598 [대만] 마우스 감도 조절 UI 항목 글자수 초과
	m_pTextEtc->Create(  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_SIZE_MOUSE, FW_NORMAL, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
#else
	//m_pTextEtc->Create(  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_SMALL_SIZE, FW_NORMAL, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
	const INT32 fontSize = 10; // 한소프트[7065] 관련 기획자가 폰트사이즈 +2 원함.
	m_pTextEtc->Create(  GetDefaultFontName(), FONT_COUNT_64, fontSize, FW_NORMAL, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
#endif
	m_pTextEtc->SetAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTextEtc->setPos( nScreenWidth - 222, nScreenHeight - 198);
	m_pTextEtc->setSize( 200, 50);
	m_pTextEtc->SetColor( 255, 255, 255, 255);	
	pHudRoot->AddChild( m_pTextEtc);

	/*INT32 nPosX = nScreenWidth - 185;
	INT32 nPosY = nScreenHeight - 109;

	m_pSpriteEtcMessageBG = i3Sprite2D::new_object();
	m_pSpriteEtcMessageBG->SetTexture( pTexture );
	m_pSpriteEtcMessageBG->Create(1, true, true);

	m_pSpriteEtcMessageBG->SetEnable( 0, false);
	m_pSpriteEtcMessageBG->SetRect( 0, (REAL32)(nPosX - 2), (REAL32)(nPosY - 30), (115.0f), (30.0f));
	m_pSpriteEtcMessageBG->SetColor( 0, 255,255,255,255);
	m_pSpriteEtcMessageBG->SetTextureCoord( 0, 817.0f, 0.0f, 932.0f, 30.0f);

	pHudRoot->AddChild(m_pSpriteEtcMessageBG);*/
}

static const REAL32 ETC_TEXT_TIME = 3.0f;
void CHUD_TextEtc::OnUpdate( REAL32 rDeltaSeconds )
{
	if( m_pTextEtc->GetEnable() == false ) return;

	m_fEtcTextTime += rDeltaSeconds;

	if( m_fEtcTextTime > ETC_TEXT_TIME)
	{
		m_fEtcTextTime = 0.0f;
		m_pTextEtc->SetEnable( false);
		//m_pSpriteEtcMessageBG->SetEnable( 0, false);
	}
}

void CHUD_TextEtc::SetEtcText(const i3::wliteral_range& wTextRng)
{
	i3_prof_func();

	m_fEtcTextTime = 0.0f;
	m_pTextEtc->SetText( wTextRng);
	m_pTextEtc->SetEnable( true);
	//m_pSpriteEtcMessageBG->SetEnable( 0, true);
}