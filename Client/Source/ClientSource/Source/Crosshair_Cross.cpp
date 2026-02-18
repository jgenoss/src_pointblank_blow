#include "pch.h"
#include "Crosshair_Cross.h"
#include "WeaponBase.h"
#include "GlobalVariables.h"
#include "GameCharaBoneContext.h"
#include "GameCharaWeaponContext.h"

I3_CLASS_INSTANCE( Crosshair_Cross);//, Crosshair);

Crosshair_Cross::Crosshair_Cross(void)
{
	m_pSprite = nullptr;
}

Crosshair_Cross::~Crosshair_Cross(void)
{
	I3_SAFE_NODE_RELEASE( m_pSprite);
}

void Crosshair_Cross::Create( void)
{
	REAL32 scrCX, scrCY;

	{
		i3RenderTarget * pRT = g_pFramework->GetViewer()->GetRenderContext()->GetRenderTarget( 0);

		scrCX = (REAL32) pRT->GetWidth() * 0.5f;
		scrCY = (REAL32) pRT->GetHeight() * 0.5f;
	}

	Crosshair::Create();

	m_pSprite = i3Sprite2D::new_object();

	//m_pSprite->Create( 8, false, true);
	m_pSprite->Create( 4, false, true);
	m_pSprite->SetSrcBlend( I3G_BLEND_SRCALPHA);
	m_pSprite->SetDestBlend( I3G_BLEND_ONE);

	_UpdatePos();

	m_pSg->AddChild( m_pSprite);
}

void Crosshair_Cross::OnUpdate( REAL32 tm)
{
	_UpdatePos();

	Crosshair::OnUpdate( tm);
}

void Crosshair_Cross::_UpdatePos(void)
{
	CWeaponInfo * pWeaponInfo = _GetCurrentWeaponInfo();
	if( pWeaponInfo == nullptr)
		return;

	REAL32 rmin, len;
	REAL32 scrCX, scrCY;

	{
		i3RenderTarget * pRT = g_pFramework->GetViewer()->GetRenderContext()->GetRenderTarget( 0);

		scrCX = (REAL32) pRT->GetWidth() * 0.5f;
		scrCY = (REAL32) pRT->GetHeight() * 0.5f;
	}

	CalcRminLen(rmin, len, scrCY, pWeaponInfo);
	len *= 0.5f;
	len += (len * (m_CrossSizeOffset*0.01f));

	// 세로 Crosshair
	if( !m_pSprite->GetEnable( 0))	m_pSprite->SetEnable(	0, true);
	m_pSprite->SetCenter(	0, scrCX, scrCY + rmin + len, 0.0f);
	m_pSprite->SetSize(		0, 1.0f, len * 2.0f);
	m_pSprite->SetColor(	0, m_CR_R, m_CR_G, m_CR_B, m_CR_A);

	if( !m_pSprite->GetEnable( 1))	m_pSprite->SetEnable(	1, true);
	m_pSprite->SetCenter(	1, scrCX, scrCY - rmin - len, 0.0f);
	m_pSprite->SetSize(		1, 1.0f, len * 2.0f);
	m_pSprite->SetColor(	1, m_CR_R, m_CR_G, m_CR_B, m_CR_A);

	// 가로 Crosshair
	if( !m_pSprite->GetEnable( 2))	m_pSprite->SetEnable(	2, true);
	m_pSprite->SetCenter(	2, scrCX + rmin + len, scrCY, 0.0f);
	m_pSprite->SetSize(		2, len * 2.0f, 1.0f);
	m_pSprite->SetColor(	2, m_CR_R, m_CR_G, m_CR_B, m_CR_A);

	if( !m_pSprite->GetEnable( 3))	m_pSprite->SetEnable(	3, true);
	m_pSprite->SetCenter(	3, scrCX - rmin - len, scrCY, 0.0f);
	m_pSprite->SetSize(		3, len * 2.0f, 1.0f);
	m_pSprite->SetColor(	3, m_CR_R, m_CR_G, m_CR_B, m_CR_A);
}
