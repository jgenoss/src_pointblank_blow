#include "pch.h"
#include "BattleHUD_CountDown.h"

#include "ui/UIMath.h"

I3_CLASS_INSTANCE(CHUD_CountDown);

namespace inner { CHUD_CountDown* this_ptr = 0; }
CHUD_CountDown* CHUD_CountDown::instance() { return inner::this_ptr; }

CHUD_CountDown::CHUD_CountDown(void)
{
	inner::this_ptr = this;

	pf_update = &CHUD_CountDown::update_none_impl;
}

CHUD_CountDown::~CHUD_CountDown(void)
{
	inner::this_ptr = 0;
	for( INT32 i = 0; i < COUNTDOWN; i++)
		I3_SAFE_RELEASE(m_pCountDownSound[i]);
}

void CHUD_CountDown::CreateHUD(i3AttrSet* pHudRoot)
{
	INT32	nScreenWidth	= g_pFramework->GetViewer()->GetViewWidth();
	INT32	nScreenHeight	= g_pFramework->GetViewer()->GetViewHeight();

	m_origin_center_x = (REAL32)(nScreenWidth * 0.5f);
	m_origin_center_y = (REAL32)(nScreenHeight * 0.3f);

	m_BgSprite.sprite = i3Sprite2D::new_object();
	m_BgSprite.sprite->Create( 1, true, true );
	m_BgSprite.sprite->SetRect( 0, 0.f, 0.f, (REAL32)nScreenWidth, (REAL32)nScreenHeight);
	m_BgSprite.sprite->SetColor( 0, 0, 0, 0, 153); //¾ËÆÄ°ª 60%
	m_BgSprite.sprite->SetEnable( false);

	m_CountSprite.texture = g_pFramework->GetResourceManager()->LoadTexture("Gui/Ingame/count_110.i3i");
	m_CountSprite.sprite = i3Sprite2D::new_object();
	m_CountSprite.sprite->Create( CS_NumMax, true, true );
	m_CountSprite.sprite->SetTexture( m_CountSprite.texture );
	
	for(INT32 i=0; i<CS_NumMax;++i)
	{
		m_CountSprite.sprite->SetColor(i, 255,255,255,255);
		m_CountSprite.sprite->SetRect(i, m_origin_center_x - (IS_Width * 0.2f), m_origin_center_y - (IS_Width * 0.2f), (REAL32)(IS_Width), (REAL32)(IS_Height));
		m_CountSprite.sprite->SetTextureCoord(i, (REAL32)(IS_Width*i), 0.f, (REAL32)(IS_Width*i)+(REAL32)(IS_Width), (REAL32)(IS_Height));
	}
	m_CountSprite.sprite->SetEnable( false);

	pHudRoot->AddChild( m_BgSprite.sprite);
	pHudRoot->AddChild( m_CountSprite.sprite);

	i3mem::FillZero(m_pCountDownSound, sizeof(m_pCountDownSound));

	const char* soundFileName[COUNTDOWN] =
	{
		"UI_Mission_CountDown.wav",
		"UI_Mission_CountDown_2.wav",
		"UI_Mission_CountDown_3.wav",
	};

	for( INT32 i = 0; i < COUNTDOWN; i++)
	{
		i3GameResSound* pResSound = (i3GameResSound*)g_pFramework->QuaryResource( soundFileName[i]);
		I3ASSERT(pResSound != nullptr);
		I3_REF_CHANGE( m_pCountDownSound[i], pResSound->getSound());
	}

	m_pSoundContext = g_pViewer->GetSoundContext();
}

void CHUD_CountDown::OnUpdate( REAL32 rDeltaSeconds )
{
	(this->*pf_update)(rDeltaSeconds);
}

void CHUD_CountDown::update_count_down_impl(REAL32 rDeltaSeconds)
{
	m_elapse_time -= rDeltaSeconds;
	if( m_elapse_time < 0.f ) 
	{
		m_CountSprite.sprite->SetEnable( m_curr_count_num, false );
		m_curr_count_num -= 1;

		if( m_curr_count_num  < CS_Num1 )
		{ 
			stop_count_down(); 
			return;
		}
		else
		{
			if( m_pSoundContext != nullptr)
			{
				m_pSoundContext->Play(m_pCountDownSound[m_curr_count_num]);
			}
		}

		m_CountSprite.sprite->SetEnable( m_curr_count_num, true );
		m_elapse_time = 1.f;
	}

	REAL32 factor = uiMath::inverse_interpolate(m_elapse_time, 0.9f, 1.0f);
	factor = uiMath::limited(factor, 0.f, 1.f);

	modify_count_down_size(factor, false);
	modify_count_down_alpha(factor, false);
}


void CHUD_CountDown::start_count_down()
{
	modify_count_down_size(1.f, true);
	modify_count_down_alpha(1.f, true);

	m_elapse_time = 1.f;
	m_curr_count_num = CS_Num3;

	m_BgSprite.sprite->SetEnable( true );
	m_CountSprite.sprite->SetEnable( CS_Num3, true );

	if( m_pSoundContext != nullptr)
		m_pSoundContext->Play(m_pCountDownSound[2]);

	pf_update = &CHUD_CountDown::update_count_down_impl;
}
void CHUD_CountDown::stop_count_down()
{
	m_BgSprite.sprite->SetEnable( false );
	m_CountSprite.sprite->SetEnable( false );

	pf_update = &CHUD_CountDown::update_none_impl;
}

void CHUD_CountDown::modify_count_down_size(REAL32 factor, bool all_shape)
{
	factor *= 10.f;
	factor += 1.f;

	REAL32 factor_width = (REAL32)(IS_Width * (factor));
	REAL32 factor_height = (REAL32)(IS_Height * (factor));

	REAL32 x = m_origin_center_x - (factor_width * 0.5f);
	REAL32 y = m_origin_center_y - (factor_height * 0.5f);
	REAL32 u = factor_width;
	REAL32 v = factor_height;

	if(all_shape == true)
	{
		for(INT32 i=0; i<CS_NumMax;++i)
		{
			m_CountSprite.sprite->SetRect(i, x, y, u, v);
		}
	}
	else
	{
		m_CountSprite.sprite->SetRect(m_curr_count_num, x, y, u, v);
	}
}

void CHUD_CountDown::modify_count_down_alpha(REAL32 factor, bool all_shape)
{
	UINT8 alpha = 255 - (UINT8)(255 * factor);

	if(all_shape == true)
	{
		for(INT32 i=0; i<CS_NumMax;++i)
		{
			m_CountSprite.sprite->SetColor(i, 255, 255, 255, alpha);
		}
	}
	else
	{
		m_CountSprite.sprite->SetColor(m_curr_count_num, 255, 255, 255, alpha);
	}
}