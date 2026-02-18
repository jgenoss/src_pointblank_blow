#include "pch.h"
#include "Crosshair_Dino.h"
#include "WeaponBase.h"
#include "GlobalVariables.h"
#include "GameCharaBoneContext.h"
#include "GameCharaWeaponContext.h"
#include "ConfigRenderBattle.h"

//I3_ABSTRACT_CLASS_INSTANCE( Crosshair_Dino, Crosshair);
I3_CLASS_INSTANCE( Crosshair_Dino);

Crosshair_Dino::Crosshair_Dino(void) : m_timeLocal(0.0f)
									, m_timeTarget(0.0f)
{
	m_pSprite = nullptr;
	m_pSprite2 = nullptr;
	m_bBite = false;
	m_bScratch = false;
	m_bSpit =false;
}

Crosshair_Dino::~Crosshair_Dino(void)
{
	I3_SAFE_NODE_RELEASE( m_pSprite);
	I3_SAFE_NODE_RELEASE( m_pSprite2);
}

void Crosshair_Dino::Create( void)
{
	Crosshair::Create();

	m_pSprite = i3Sprite2D::new_object();

	m_pSprite->Create( 5, true, true);
	m_pSprite->SetSrcBlend( I3G_BLEND_SRCALPHA);
	m_pSprite->SetDestBlend( I3G_BLEND_ONE);
	
	m_pSprite2 = i3Sprite2D::new_object();
	m_pSprite2->Create(1, true, true);
	m_pSprite2->SetSrcBlend( I3G_BLEND_SRCALPHA);
	m_pSprite2->SetDestBlend( I3G_BLEND_ONE);
	
	{
		i3TextureFilterAttr * pAttr = i3TextureFilterAttr::new_object_ref();

		pAttr->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR);

		m_pSprite->AddAttr( pAttr);
		m_pSprite2->AddAttr( pAttr);
	}

	m_pSprite->SetEnable( 0, true);
	m_pSprite->SetEnable( 1, true);
	m_pSprite->SetEnable( 2, false);
	m_pSprite->SetEnable( 3, false);
	m_pSprite->SetEnable( 4, false);
	
	{
		i3Texture * pTex =nullptr;
		i3Texture * pTex2 =nullptr;

		 pTex2 = crb::i()->LoadHudTexture( HUD_TEXTURE_HUD3);
		 pTex = crb::i()->LoadHudTexture( HUD_TEXTURE_HUD2);
		
		

		m_pSprite->SetTexture( pTex);
		m_pSprite2->SetTexture( pTex2);

		I3_MUST_RELEASE(pTex);
		I3_MUST_RELEASE(pTex2);
	}

	m_pSg->AddChild( m_pSprite);
	m_pSg->AddChild( m_pSprite2);
}

void Crosshair_Dino::OnUpdate( REAL32 tm)
{
	_Update( tm);

	Crosshair::OnUpdate( tm);
}

void Crosshair_Dino::StartBite(void)
{
}

void Crosshair_Dino::Bite(void)
{
	m_bBite = true;
	m_timeTarget = 1.0f;
	m_timeLocal = 0.0f;
}
void Crosshair_Dino::Spit(void)
{
	m_bSpit = true;
	m_timeTarget = 1.0f;
	m_timeLocal = 0.0f;
}
void Crosshair_Dino::Scratch(void)
{
	m_bScratch = true;
	m_timeTarget = 1.0f;
	m_timeLocal = 0.0f;
}
void Crosshair_Dino::ResetCrosshair()
{
	m_bBite = false;
}

