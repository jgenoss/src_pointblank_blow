#include "pch.h"
#include "Crosshair_CrossDot.h"
#include "WeaponBase.h"
#include "GlobalVariables.h"
#include "GameCharaBoneContext.h"
#include "GameCharaWeaponContext.h"
#include "UserInfoContext.h"

I3_CLASS_INSTANCE( Crosshair_CrossDot);//, Crosshair);

#define		CR_R		0
#define		CR_G		255
#define		CR_B		0

Crosshair_CrossDot::Crosshair_CrossDot(void)
{
	m_pSprite = nullptr;

	m_CR_R = CR_R;
	m_CR_Org_R = CR_R;
	m_CR_G = CR_G;
	m_CR_Org_G = CR_G;
	m_CR_B = CR_B;
	m_CR_Org_B = CR_B;
	m_CR_A = 255;
	m_CR_Org_A = 255;

	if( CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_COLOR_CROSSHAIR) == true ) // 칼라 크로스 헤어체크
	{	
		USER_INFO_INVITEM_DATA MyInvenData;
		UserInfoContext::i()->GetMyUserInfoInvItemData(&MyInvenData);

		UINT8 color = MyInvenData._ui8CrossHairColor;
		const I3COLOR* cashColor = GetColorCashItem(color);

		m_CR_Org_R = cashColor->r;
		m_CR_Org_G = cashColor->g;
		m_CR_Org_B = cashColor->b;
	}
}

Crosshair_CrossDot::~Crosshair_CrossDot(void)
{
	I3_SAFE_NODE_RELEASE( m_pSprite);
	I3_SAFE_NODE_RELEASE( m_pHitSprite);
	I3_SAFE_RELEASE( m_pHitTexture);
}

void Crosshair_CrossDot::Create( void)
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
	m_pSprite->Create( 5, false, true);
	m_pSprite->SetSrcBlend( I3G_BLEND_SRCALPHA);
	m_pSprite->SetDestBlend( I3G_BLEND_ONE);

	_UpdatePos();

	m_pSg->AddChild( m_pSprite);
}

void Crosshair_CrossDot::OnUpdate( REAL32 tm)
{
	_UpdatePos();

	Crosshair::OnUpdate( tm);
}

void Crosshair_CrossDot::_UpdatePos(void)
{
	REAL32 rmin, len;
	REAL32 scrCX, scrCY;

	CWeaponInfo * pWeaponInfo = _GetCurrentWeaponInfo();
	if( pWeaponInfo == nullptr)
		return;

	{
		i3RenderTarget * pRT = g_pFramework->GetViewer()->GetRenderContext()->GetRenderTarget( 0);

		scrCX = (REAL32) pRT->GetWidth() * 0.5f;
		scrCY = (REAL32) pRT->GetHeight() * 0.5f;
	}

	CalcRminLen(rmin, len, scrCY, pWeaponInfo);
	len *= 0.5f;

	// 세로 Crosshair
	if( !m_pSprite->GetEnable( 0))	m_pSprite->SetEnable(	0, true);
	m_pSprite->SetCenter(	0, scrCX, scrCY + rmin + len, 0.0f);
	m_pSprite->SetSize(		0, 1.0f, len * 2.0f + m_CrossSizeOffset);
	m_pSprite->SetColor(	0, m_CR_R, m_CR_G, m_CR_B, m_CR_A);

	if( !m_pSprite->GetEnable( 1))	m_pSprite->SetEnable(	1, true);
	m_pSprite->SetCenter(	1, scrCX, scrCY - rmin - len, 0.0f);
	m_pSprite->SetSize(		1, 1.0f, len * 2.0f + m_CrossSizeOffset);
	m_pSprite->SetColor(	1, m_CR_R, m_CR_G, m_CR_B, m_CR_A);

	// 가로 Crosshair
	if( !m_pSprite->GetEnable( 2))	m_pSprite->SetEnable(	2, true);
	m_pSprite->SetCenter(	2, scrCX + rmin + len, scrCY, 0.0f);
	m_pSprite->SetSize(		2, len * 2.0f + m_CrossSizeOffset, 1.0f);
	m_pSprite->SetColor(	2, m_CR_R, m_CR_G, m_CR_B, m_CR_A);

	if( !m_pSprite->GetEnable( 3))	m_pSprite->SetEnable(	3, true);
	m_pSprite->SetCenter(	3, scrCX - rmin - len, scrCY, 0.0f);
	m_pSprite->SetSize(		3, len * 2.0f + m_CrossSizeOffset, 1.0f);
	m_pSprite->SetColor(	3, m_CR_R, m_CR_G, m_CR_B, m_CR_A);

	m_pSprite->SetEnable(	4, true);
	m_pSprite->SetCenter(	4, scrCX, scrCY, 0.0f);
	m_pSprite->SetSize(		4, 1.0f, 1.0f);
	m_pSprite->SetColor(	4, m_CR_R, m_CR_G, m_CR_B, m_CR_A);
}
