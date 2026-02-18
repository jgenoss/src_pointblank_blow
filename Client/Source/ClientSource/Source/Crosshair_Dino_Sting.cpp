#include "pch.h"
#include "Crosshair_Dino_Sting.h"
#include "WeaponBase.h"
#include "GlobalVariables.h"
#include "GameCharaBoneContext.h"
#include "GameCharaWeaponContext.h"
#include "StageBattle.h"

const INT32	STING_CH_UP_CX			= 39;
const INT32 STING_CH_UP_CY			= 42;


const INT32	RAPTOR_CH_UP_CX			= 41;
const INT32 RAPTOR_CH_UP_CY			= 26;

const INT32	RAPTOR_CH_DN_CX			= 41;
const INT32 RAPTOR_CH_DN_CY			= 17;


const INT32	RAPTOR_SC_UP_CX			= 50;
const INT32 RAPTOR_SC_UP_CY			= 51;


I3_CLASS_INSTANCE( Crosshair_Dino_Sting);//, Crosshair_Dino);

Crosshair_Dino_Sting::Crosshair_Dino_Sting(void)
{
}

Crosshair_Dino_Sting::~Crosshair_Dino_Sting(void)
{
}

void Crosshair_Dino_Sting::Create(void)
{
	Crosshair_Dino::Create();

	m_pSprite->SetTextureCoord( 0, 969.0f, 398.0f, 969.0f + STING_CH_UP_CX, 398.0f + STING_CH_UP_CY);
	m_pSprite->SetTextureCoord( 2, 904.0f, 264.0f, 904.0f + RAPTOR_CH_UP_CX, 264.0f + RAPTOR_CH_UP_CY);
	m_pSprite->SetTextureCoord( 3, 904.0f, 291.0f, 904.0f + RAPTOR_CH_DN_CX, 291.0f + RAPTOR_CH_DN_CY);


}

void Crosshair_Dino_Sting::_Update( REAL32 tm)
{
	REAL32 scrCX, scrCY;

	{
		i3RenderTarget * pRT = g_pFramework->GetViewer()->GetRenderContext()->GetRenderTarget( 0);

		scrCX = (REAL32) pRT->GetWidth() * 0.5f;
		scrCY = (REAL32) pRT->GetHeight() * 0.5f;
	}

	REAL32 x1, y1, w1, h1;
	REAL32 scale = 1.0f;
	REAL32 t;
	REAL32 dist = 6.0f;

	m_timeLocal += tm;

	w1 = STING_CH_UP_CX * scale;
	h1 = STING_CH_UP_CY * scale;
	x1 = scrCX - (w1 * 0.5f);
	y1 = scrCY - (h1 * 0.5f) - (dist * 0.5f);
	
	m_pSprite->SetRect( 0, x1, y1, w1, h1);
	m_pSprite->SetColor( 0, m_CR_R, m_CR_G, m_CR_B, m_CR_A);
	
#if 1
	if(m_bSpit)
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
			y1 = scrCY - (h1 * 0.5f) - (dist * 0.5f);


			m_pSprite->SetRect( 4, x1, y1, w1, h1);
			m_pSprite->SetColor( 4, 255, 0, 0, a);

			m_pSprite->SetColor( 3, 255, 0, 0, a);

			if( m_timeLocal >= m_timeTarget)
				m_bScratch = false;

	}
	
	m_pSprite->SetEnable( 2, m_bSpit == true);
	m_pSprite->SetEnable( 3, m_bSpit == true);
#endif
	
}
