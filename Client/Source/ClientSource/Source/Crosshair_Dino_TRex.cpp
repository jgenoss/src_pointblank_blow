#include "pch.h"
#include "Crosshair_Dino_TRex.h"
#include "WeaponBase.h"
#include "GlobalVariables.h"
#include "GameCharaBoneContext.h"
#include "GameCharaWeaponContext.h"
#include "StageBattle.h"

const INT32	TREX_CH_UP_CX			= 65;
const INT32 TREX_CH_UP_CY			= 35;

const INT32	TREX_CH_DN_CX			= 65;
const INT32 TREX_CH_DN_CY			= 23;

I3_CLASS_INSTANCE( Crosshair_Dino_TRex);//, Crosshair_Dino);

Crosshair_Dino_TRex::Crosshair_Dino_TRex(void)
{
}

Crosshair_Dino_TRex::~Crosshair_Dino_TRex(void)
{
}

void Crosshair_Dino_TRex::Create(void)
{
	Crosshair_Dino::Create();

	m_pSprite->SetTextureCoord( 0, 954.0f, 265.0f, 954.0f + TREX_CH_UP_CX, 265.0f + TREX_CH_UP_CY);
	m_pSprite->SetTextureCoord( 1, 954.0f, 314.0f, 954.0f + TREX_CH_DN_CX, 314.0f + TREX_CH_DN_CY);
	m_pSprite->SetTextureCoord( 2, 954.0f, 265.0f, 954.0f + TREX_CH_UP_CX, 265.0f + TREX_CH_UP_CY);
	m_pSprite->SetTextureCoord( 3, 954.0f, 314.0f, 954.0f + TREX_CH_DN_CX, 314.0f + TREX_CH_DN_CY);
}

void Crosshair_Dino_TRex::_Update( REAL32 tm)
{
	REAL32 scrCX, scrCY;

	{
		i3RenderTarget * pRT = g_pFramework->GetViewer()->GetRenderContext()->GetRenderTarget( 0);

		scrCX = (REAL32) pRT->GetWidth() * 0.5f;
		scrCY = (REAL32) pRT->GetHeight() * 0.5f;
	}

	REAL32 x1, y1, w1, h1;
	REAL32 x2, y2, w2, h2;
	REAL32 scale = 1.0f;
	REAL32 t;
	REAL32 dist = 6.0f;

	m_timeLocal += tm;

	w1 = TREX_CH_UP_CX * scale;
	h1 = TREX_CH_UP_CY * scale;
	x1 = scrCX - (w1 * 0.5f);
	y1 = scrCY - (h1 * 0.5f) - (dist * 0.5f);
	
	w2 = TREX_CH_DN_CX * scale;
	h2 = TREX_CH_DN_CY * scale;
	x2 = scrCX - (w2 * 0.5f);
	y2 = scrCY + (h2 * 0.5f) + (dist * 0.5f);

	m_pSprite->SetRect( 0, x1, y1, w1, h1);
	m_pSprite->SetRect( 1, x2, y2, w2, h2);
	m_pSprite->SetColor( 0, m_CR_R, m_CR_G, m_CR_B, m_CR_A);
	m_pSprite->SetColor( 1, m_CR_R, m_CR_G, m_CR_B, m_CR_A);

	#if 1
	if( m_bBite)
	{


		UINT8 a;

		m_timeLocal = min( m_timeLocal, m_timeTarget);

		scale = 5.0f;
		dist = 100.0f;

		t = m_timeLocal / m_timeTarget;
		a = (UINT8)(i3Math::vcos( t) * 255.0f);

		w1 = TREX_CH_UP_CX * scale;
		h1 = TREX_CH_UP_CY * scale;
		x1 = scrCX - (w1 * 0.5f);
		y1 = scrCY - (h1 * 0.5f) - (dist * 0.5f);
		
		w2 = TREX_CH_DN_CX * scale;
		h2 = TREX_CH_DN_CY * scale;
		x2 = scrCX - (w2 * 0.5f);
		y2 = scrCY + (h2 * 0.5f) + (dist * 0.5f);

		m_pSprite->SetRect( 2, x1, y1, w1, h1);
		m_pSprite->SetRect( 3, x2, y2, w2, h2);
		m_pSprite->SetColor( 2, 255, 0, 0, a);
		m_pSprite->SetColor( 3, 255, 0, 0, a);

		if( m_timeLocal >= m_timeTarget)
			m_bBite = false;
	}

	m_pSprite->SetEnable( 2, m_bBite == true);
	m_pSprite->SetEnable( 3, m_bBite == true);
	#endif
}
