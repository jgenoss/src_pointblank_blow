#include "pch.h"
#include "Crosshair_Dino_Elite.h"
#include "WeaponBase.h"
#include "GlobalVariables.h"
#include "GameCharaBoneContext.h"
#include "GameCharaWeaponContext.h"
#include "StageBattle.h"

const INT32	RAPTOR_CH_UP_CX			= 41;
const INT32 RAPTOR_CH_UP_CY			= 26;

const INT32	RAPTOR_CH_DN_CX			= 41;
const INT32 RAPTOR_CH_DN_CY			= 17;

const INT32	RAPTOR_SC_UP_CX			= 40;
const INT32 RAPTOR_SC_UP_CY			= 100;


I3_CLASS_INSTANCE( Crosshair_Dino_Elite);//, Crosshair_Dino);

Crosshair_Dino_Elite::Crosshair_Dino_Elite(void)
{
}

Crosshair_Dino_Elite::~Crosshair_Dino_Elite(void)
{
}

void Crosshair_Dino_Elite::Create(void)
{
	Crosshair_Dino::Create();

	m_pSprite->SetTextureCoord( 0, 904.0f, 264.0f, 904.0f + RAPTOR_CH_UP_CX, 264.0f + RAPTOR_CH_UP_CY);
	m_pSprite->SetTextureCoord( 1, 904.0f, 291.0f, 904.0f + RAPTOR_CH_DN_CX, 291.0f + RAPTOR_CH_DN_CY);

	m_pSprite2->SetTextureCoord( 0, 841.0f, 6.0f, 841.0f + RAPTOR_SC_UP_CX, 6.0f+RAPTOR_SC_UP_CY);
	
}

void Crosshair_Dino_Elite::_Update( REAL32 tm)
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


	w1 = RAPTOR_CH_UP_CX * scale;
	h1 = RAPTOR_CH_UP_CY * scale;
	x1 = scrCX - (w1 * 0.5f);
	y1 = scrCY - (h1 * 0.5f) - (dist * 0.5f);

	w2 = RAPTOR_CH_DN_CX * scale;
	h2 = RAPTOR_CH_DN_CY * scale;
	x2 = scrCX - (w2 * 0.5f);
	y2 = scrCY + (h2 * 0.5f) + (dist * 0.5f);

	m_pSprite->SetRect( 0, x1, y1, w1, h1);
	m_pSprite->SetRect( 1, x2, y2, w2, h2);
	m_pSprite->SetColor( 0, m_CR_R, m_CR_G, m_CR_B, m_CR_A);
	m_pSprite->SetColor( 1, m_CR_R, m_CR_G, m_CR_B, m_CR_A);


#if 1
	
	if (m_bScratch)
	{
		UINT8 a;

		m_timeLocal = min( m_timeLocal, m_timeTarget);

		scale = 5.0f;
		dist = 100.0f;

		t = m_timeLocal / m_timeTarget;
		a = (UINT8)(i3Math::vcos( t) * 255.0f);

		w1 = RAPTOR_SC_UP_CX * scale;
		h1 = RAPTOR_SC_UP_CY * scale;
		x1 = scrCX - (w1 * 0.5f);
		y1 = scrCY - (h1 * 0.3f) - (dist * 0.3f);

	
		m_pSprite2->SetRect( 0, x1, y1, w1, h1);
		m_pSprite2->SetColor( 0, 255, 0, 0, a);
		
	
		if( m_timeLocal >= m_timeTarget)
			m_bScratch = false;
		
	}
	m_pSprite2->SetEnable( 0, m_bScratch == true);
	
#endif
}
