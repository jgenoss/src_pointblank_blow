#include "pch.h"

#if 지워질_것들

#include "GlobalVariables.h"
#include "BattleHUD_WeaponHeatGauge.h"
#include "StageBattle.h"
#include "BattleHud.h"

I3_CLASS_INSTANCE( CHUD_WeaponHeatGauge);//, i3GameObjBase);

CHUD_WeaponHeatGauge::CHUD_WeaponHeatGauge()
{
	m_pHud = NULL;
	m_pSprite	= NULL;
	m_pTextGage = NULL;

	m_fLocalTime = 0.0f;
	m_fGageLength = 86.0f;
	m_nPosX = 0;
	m_nPosY = 0;
	m_bUpdateGage	= FALSE;
	m_bShowIcon		= true;
}

CHUD_WeaponHeatGauge::~CHUD_WeaponHeatGauge()
{
	I3_SAFE_NODE_RELEASE( m_pSprite);
	I3_SAFE_NODE_RELEASE( m_pTextGage);
}

// Create()는 반드시 호출되어야 합니다.
void CHUD_WeaponHeatGauge::Create( CHud * pHud, bool bShowIcon)
{
	m_pHud = pHud;

	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= 0;	
	INT32	nScreenHeight	= 0;

	if( pViewer != NULL)
	{
		nScreenWidth	= pViewer->GetViewWidth();	
		nScreenHeight	= pViewer->GetViewHeight();
	}
	 
	m_nPosX = (INT32)((nScreenWidth ) - 156.f);
	m_nPosY = (INT32)(nScreenHeight - 68.f );

	i3Texture * pTex = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD);

	m_pSprite = i3Sprite2D::new_object();	
	I3ASSERT( m_pSprite != NULL);
	m_pSprite->Create( 2, TRUE, TRUE);
	m_pSprite->SetTexture( pTex );
	pTex->Release();

	// Gage
	m_pSprite->SetEnable( FALSE);
	m_pSprite->SetRect( 0, (REAL32)(m_nPosX+49), (REAL32)(m_nPosY + 16), m_fGageLength, (16.0f));
	m_pSprite->SetColor( 0, 255,255,255,255);
	m_pSprite->SetTextureCoord( 0, 953.0f, 245.0f, 983.0f, 257.0f);

	// M197 Image
	m_pSprite->SetEnable( FALSE);
	m_pSprite->SetRect( 1, (REAL32)(m_nPosX+53), (REAL32)(m_nPosY - 13), 82.0f, 22.0f);
	m_pSprite->SetColor( 1, 255,255,255,255);
	m_pSprite->SetTextureCoord( 1, 682.0f, 220.0f, 682.0f + 82.0f, 220.0f + 22.0f);

	m_bShowIcon = bShowIcon;

	m_pHud->getHUDRoot()->AddChild( m_pSprite);
}

void CHUD_WeaponHeatGauge::OnUpdate( REAL32 rDeltaSeconds )
{	
	i3GameObjBase::OnUpdate( rDeltaSeconds );

	m_fLocalTime += rDeltaSeconds;

	if( !m_bUpdateGage)
	{
		if( m_bShowIcon)	m_pSprite->SetEnable( FALSE);
		else				m_pSprite->SetEnable( 0, FALSE);
	}
	else				m_bUpdateGage = FALSE;
}

void CHUD_WeaponHeatGauge::SetRidingMark(BOOL bEnable)
{
}

void CHUD_WeaponHeatGauge::UpdateGauge( REAL32 fGaugeValue)
{
	  // gage를 조정합니다.	
	REAL32 fScale = fGaugeValue * 0.01f;
	m_pSprite->SetRect( 0, (REAL32)(m_nPosX+49), (REAL32)(m_nPosY + 16), m_fGageLength*fScale, (16.0f));

	// 현재 HP에 따라서 색상을 변경합니다.
	COLOR col;
	if( fGaugeValue >= 90.0f)		i3Color::Set( &col, (UINT8) 255, 255, 255, 255);
	else if( fGaugeValue >= 80.0f)	i3Color::Set( &col, (UINT8) 255, 190, 190, 255);
	else if( fGaugeValue >= 70.0f)	i3Color::Set( &col, (UINT8) 255, 160, 160, 255);
	else if( fGaugeValue >= 60.0f)	i3Color::Set( &col, (UINT8) 255, 130, 130, 255);
	else if( fGaugeValue >= 50.0f)	i3Color::Set( &col, (UINT8) 255, 100, 100, 255);
	else if( fGaugeValue >= 40.0f)	i3Color::Set( &col, (UINT8) 255, 70, 70, 255);
	else if( fGaugeValue > 30.0f)	i3Color::Set( &col, (UINT8) 255, 30, 30, 255);
	else					
	{
		i3Color::Set( &col, (UINT8) 255, 0, 0, 255);		

		// HP가 일정이상 낮게 되었을경우에 사용하는 변수를 설정합니다.
		//nValue = 100 + (INT32)(155.0f * (i3Math::sin(m_fLocalTime*10.0f)+1.0f));
		//if( nValue > 255) nValue = 255;
		//else if( nValue < 0) nValue = 0;
	}

	m_pSprite->SetColor( 0, &col);

	if( m_bShowIcon)	m_pSprite->SetEnable( TRUE);
	else				m_pSprite->SetEnable( 0, TRUE);

	m_bUpdateGage = TRUE;
}
#endif