#include "pch.h"

#if 지워질_것들

#include "GlobalVariables.h"
#include "BattleHud_Helicopter.h"
#include "BattleHud.h"
//#include "StageGui/Popup/PopupOption.h"
#include "ConfigRenderBattle.h"

I3_CLASS_INSTANCE( CHUD_DinoHP);//, i3GameObjBase);

CHUD_DinoHP::CHUD_DinoHP(void)
{
	m_pHud = NULL;
	m_pSprite = NULL;

	m_fLocalTime = 0.0f;
	//m_fGageLength = 208.0f;
	m_fGageLength = 472.0f;
	m_nPosX = 0;
	m_nPosY = 0;
	m_fClanOffsetY = 0.0f;
	m_bUpdateGage = FALSE;
}

CHUD_DinoHP::~CHUD_DinoHP(void)
{	
	I3_SAFE_NODE_RELEASE( m_pSprite);
}

void CHUD_DinoHP::Create(CHud * pHud)
{
	m_pHud = pHud;

	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();	

	if( g_pGameContext->GetBattleChannel())
	{
		m_fClanOffsetY = 18.0f;
	}

	m_nPosX = (INT32)((nScreenWidth * 0.5f) - 264.0f);
	m_nPosY = (INT32)(50.0f + m_fClanOffsetY);

	i3Texture * pTex = crb::i()->LoadHudTexture( HUD_TEXTURE_HUD);

	m_pSprite = i3Sprite2D::new_object();	
	I3ASSERT( m_pSprite != NULL);
	m_pSprite->Create( DINOHP_SPR_COUNT, TRUE, TRUE);
	m_pSprite->SetTexture( pTex );

	//crb::i()->LoadHudTexture은 이미 addref를 한상태의 instance를 넘겨주기때문에, 가지고 있지 않는다면 refcount를 하나 감소시켜줘야한다.
	pTex->Release();

	// Body
	{
		m_pSprite->SetEnable( DINOHP_SPR_BODY_LEFT, FALSE);
		m_pSprite->SetRect( DINOHP_SPR_BODY_LEFT, (REAL32)m_nPosX, (REAL32)m_nPosY, (66.0f), (43.0f));
		m_pSprite->SetColor( DINOHP_SPR_BODY_LEFT, 255,255,255,255);
		m_pSprite->SetTextureCoord( DINOHP_SPR_BODY_LEFT, 786.0f, 235.0f, 852.0f, 278.0f);

		m_pSprite->SetEnable( DINOHP_SPR_BODY_CENTER, FALSE);
		m_pSprite->SetRect( DINOHP_SPR_BODY_CENTER, (REAL32)(m_nPosX+66), (REAL32)m_nPosY, (435.0f), (43.0f));
		m_pSprite->SetColor( DINOHP_SPR_BODY_CENTER, 255,255,255,255);
		m_pSprite->SetTextureCoord( DINOHP_SPR_BODY_CENTER, 855.0f, 235.0f, 879.0f, 278.0f);

		m_pSprite->SetEnable( DINOHP_SPR_BODY_RIGHT, FALSE);
		m_pSprite->SetRect( DINOHP_SPR_BODY_RIGHT, (REAL32)(m_nPosX+501), (REAL32)m_nPosY, (26.0f), (43.0f));
		m_pSprite->SetColor( DINOHP_SPR_BODY_RIGHT, 255,255,255,255);
		m_pSprite->SetTextureCoord( DINOHP_SPR_BODY_RIGHT, 882.0f, 235.0f, 908.0f, 278.0f);
	}

	// TRex
	m_pSprite->SetEnable( DINOHP_SPR_TREX, FALSE);
	m_pSprite->SetRect( DINOHP_SPR_TREX, (REAL32)(m_nPosX + 9), (REAL32)(m_nPosY + 8), (34.0f), (30.0f));
	m_pSprite->SetColor( DINOHP_SPR_TREX, 255,255,255,255);
	m_pSprite->SetTextureCoord( DINOHP_SPR_TREX, 894.0f, 279.0f, 894.0f + 34.0f, 279.0f + 30.0f);

	// Gage
	m_pSprite->SetEnable( DINOHP_SPR_GAGE, FALSE);
	m_pSprite->SetRect( DINOHP_SPR_GAGE, (REAL32)(m_nPosX+49), (REAL32)(m_nPosY + 16), m_fGageLength, (12.0f));
	m_pSprite->SetColor( DINOHP_SPR_GAGE, 255,255,255,255);
	m_pSprite->SetTextureCoord( DINOHP_SPR_GAGE, 953.0f, 245.0f, 983.0f, 257.0f);

	m_pHud->getHUDRoot()->AddChild( m_pSprite);

	
}

void CHUD_DinoHP::OnUpdate( REAL32 rDeltaSeconds)
{
	i3GameObjBase::OnUpdate( rDeltaSeconds );

	m_fLocalTime += rDeltaSeconds;

	CGameCharaDinoTRex * pTRex = g_pCharaManager->GetTRex();

	if( pTRex != NULL)
		UpdateHP( (REAL32) pTRex->getCurHP());

	if( !m_bUpdateGage)
	{
		m_pSprite->SetEnable( DINOHP_SPR_BODY_LEFT, FALSE);
		m_pSprite->SetEnable( DINOHP_SPR_BODY_CENTER, FALSE);
		m_pSprite->SetEnable( DINOHP_SPR_BODY_RIGHT, FALSE);
		m_pSprite->SetEnable( DINOHP_SPR_TREX, FALSE);
		m_pSprite->SetEnable( DINOHP_SPR_GAGE, FALSE);
	}
	else
	{
		m_bUpdateGage = FALSE;
	}
}

void CHUD_DinoHP::UpdateHP(REAL32 fHp)
{
	INT32 nValue = 255;

	// 비활성화 되어있다면, 활성화 합니다.
	if( !m_pSprite->GetEnable( DINOHP_SPR_BODY_LEFT))
	{
		m_pSprite->SetEnable( DINOHP_SPR_BODY_LEFT, TRUE);
		m_pSprite->SetEnable( DINOHP_SPR_BODY_CENTER, TRUE);
		m_pSprite->SetEnable( DINOHP_SPR_BODY_RIGHT, TRUE);
		m_pSprite->SetEnable( DINOHP_SPR_TREX, TRUE);
		m_pSprite->SetEnable( DINOHP_SPR_GAGE, TRUE);
	}

    // gage를 조정합니다.	
	REAL32 fScale = fHp * (1.0f / nValue);//0.00392156862745098f;
	m_pSprite->SetRect( DINOHP_SPR_GAGE, (REAL32)(m_nPosX+49), (REAL32)(m_nPosY + 16), m_fGageLength*fScale, (12.0f));
	//GlobalFunc::PB_TRACE("m_fGageLength %f fScale %f", m_fGageLength, fScale);

	// 현재 HP에 따라서 색상을 변경합니다.
	COLOR col;
	if( fScale >= 0.9f)		i3Color::Set( &col, (UINT8) 255, 255, 255, 255);
	else if( fScale >= 0.8f)	i3Color::Set( &col, (UINT8) 255, 190, 190, 255);
	else if( fScale >= 0.7f)	i3Color::Set( &col, (UINT8) 255, 160, 160, 255);
	else if( fScale >= 0.6f)	i3Color::Set( &col, (UINT8) 255, 130, 130, 255);
	else if( fScale >= 0.5f)	i3Color::Set( &col, (UINT8) 255, 100, 100, 255);
	else if( fScale >= 0.4f)	i3Color::Set( &col, (UINT8) 255, 70, 70, 255);
	else if( fScale > 0.3f)	i3Color::Set( &col, (UINT8) 255, 30, 30, 255);
	else					
	{
		i3Color::Set( &col, 255, 0, 0, (UINT8)nValue);		

		// HP가 일정이상 낮게 되었을경우에 사용하는 변수를 설정합니다.
		nValue = 100 + (INT32)(155.0f * (i3Math::sin(m_fLocalTime*10.0f)+1.0f));
		if( nValue > 255) nValue = 255;
		else if( nValue < 0) nValue = 0;
	}

	m_pSprite->SetColor( DINOHP_SPR_TREX, 255,255,255,(UINT8)nValue);
	m_pSprite->SetColor( DINOHP_SPR_BODY_LEFT, 255,(UINT8)nValue,(UINT8)nValue,255);
	m_pSprite->SetColor( DINOHP_SPR_BODY_CENTER, 255,(UINT8)nValue,(UINT8)nValue,255);
	m_pSprite->SetColor( DINOHP_SPR_BODY_RIGHT, 255,(UINT8)nValue,(UINT8)nValue,255);

	// 변경된 색상을 게이지에 적용합니다.
	m_pSprite->SetColor( DINOHP_SPR_GAGE, &col);

	/*
	if( fHp <= 30.0f)
	{
		char szTemp[256];
		sprintf_s( szTemp, "\"%s\"", GAME_STRING("STR_TBL_INGAME_HUD_NOTICE_WARNING_HELI"));
		m_pHud->SetMissionNoticeString( szTemp, 0.1f, 0, 45);
	}
	*/

	

	m_bUpdateGage = TRUE;
}
#endif