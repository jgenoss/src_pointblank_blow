#include "pch.h"
#include "PopupOption.h"
#include "StageReady.h"
#include "GlobalVariables.h"

#include "UI/UIUtil.h"
#include "StageBattle.h"

#define CRITICAL_NOTIFY		GAME_STRING("STR_POPUP_Q_MESSAGE_PERFORMANCE_VERY_SLOW_DOWN")		//	"변경하신 항목은 사용중인 하드웨어에 따라 성능이 급격히 저하됩니다.\n변경 하시겠습니까?"
#define NORMAL_NOTIFY		GAME_STRING("STR_POPUP_Q_MESSAGE_PERFORMANCE_SLOW_DOWN")			//	"변경하신 항목은 사용중인 하드웨어에 따라 성능이 저하될 수 있습니다.\n변경 하시겠습니까?"


/** \brief 임시 옵션창에 가려져 옆으로 옮깁니다. */
static void		RePositionMsgBox( UIMessageBox * pBox)
{
	if( pBox != NULL)
	{
		if( g_pFramework->IsBattleStage())
			pBox->SetPosition( pBox->GetPositionX() - 100, pBox->GetPositionY() + 100);
		else
			pBox->SetPosition( pBox->GetPositionX() - 100, pBox->GetPositionY());
	}
}

void CPopupOption::_ResetPerformance(void)
{
	// Shadow
	_UpdateShadow();

	// 총알궤적
	m_pCapacityBullet->setCheckBoxState( m_bEnableBulletTrace);
	
	if( !m_bEnableBulletTrace)
	{
		COLOR col;
		i3Color::Set( &col, (UINT8) 100, 100, 100, 255);

		m_pCapacityBulletSmoke->setCheckBoxState( FALSE);
		m_pCapacityBulletSmoke->setControlDisable( TRUE);
		m_bEnableBulletSmoke= FALSE;
		m_pCapacityBulletSmokeKey->SetTextColor(&col);
	}
	else
	{
		// 총알 연기 궤적
		m_pCapacityBulletSmoke->setCheckBoxState( m_bEnableBulletSmoke);
	}

	// 동적 라이팅
	m_pCapacityLight->setCheckBoxState( m_bDynamicLight);

	// 노말맵
	m_pCapacityNormal->setCheckBoxState( m_bEnableNormalMap);

	// 텍스쳐 퀄러티
	switch( m_nTextureQualityType)
	{
	case 0:
		m_pCapacityTextureCombo->SetText( GAME_STRING("STBL_IDX_QUALITY_TYPE1"));	// 상
		break;
	case 1:
		m_pCapacityTextureCombo->SetText( GAME_STRING("STBL_IDX_QUALITY_TYPE2"));	// 중
		break;
	case 2:
		m_pCapacityTextureCombo->SetText( GAME_STRING("STBL_IDX_QUALITY_TYPE3"));	// 하
		break;
	default:
		m_pCapacityTextureCombo->SetText( GAME_STRING("STBL_IDX_QUALITY_TYPE2"));	// 중
		break;
	}

	// Terrain Effect
	m_pCapacityTerrain->setCheckBoxState( m_bEnableTerrainEffect);

	// 16bit Deapth buffer
	m_pCapacityDepth->setCheckBoxState( m_b16BitsDepth);

	// Bloom효과
	m_pCapacityBloom->setCheckBoxState( m_bEnableBloom);

	// PhysX
	m_pCapacityPhysX->setCheckBoxState( m_bEnablePhysX);

	// 빛반사효과
	switch(m_nSpecularQualityType)
	{
	case 2:
		m_pCapacitySpecularCombo->SetText(GAME_STRING("STBL_IDX_QUALITY_TYPE1"));
		break;
	case 1:
		m_pCapacitySpecularCombo->SetText(GAME_STRING("STBL_IDX_QUALITY_TYPE2"));
		break;
	case 0:
		m_pCapacitySpecularCombo->SetText(GAME_STRING("STBL_IDX_QUALITY_TYPE3"));
		break;
	default:
		m_pCapacitySpecularCombo->SetText(GAME_STRING("STBL_IDX_QUALITY_TYPE2"));
		break;
	}

	// 이펙트 품질
	switch(m_nEffectQuality)
	{
	case 2:
		m_pCapacityEffectCombo->SetText(GAME_STRING("STBL_IDX_QUALITY_TYPE1"));
		break;
	case 1:
		m_pCapacityEffectCombo->SetText(GAME_STRING("STBL_IDX_QUALITY_TYPE2"));
		break;
	case 0:
		m_pCapacityEffectCombo->SetText(GAME_STRING("STBL_IDX_QUALITY_TYPE3"));
		break;
	default:
		m_pCapacityEffectCombo->SetText(GAME_STRING("STBL_IDX_QUALITY_TYPE2"));
		break;
	}
}

void CPopupOption::_InitPerformance(void)
{
#if defined(GUI_ADJUST_RUSSIA)
	{
		i3GuiEditBox * pEditBoxes[] =
		{
			m_pCapacityTextureKey, m_pCapacityShadowKey,
			m_pCapacitySpecularKey, m_pCapacityEffectKey,
		};
		i3GuiEditBox * pComboBoxes[] =
		{
			m_pCapacityTextureCombo, m_pCapacityShadowCombo,
			m_pCapacitySpecularCombo, m_pCapacityEffectCombo,
		};

		I3ASSERT(sizeof(pEditBoxes) == sizeof(pComboBoxes));

		INT32 BoxCount = GET_ARRAY_COUNT(pEditBoxes);
		const i3TextFontSetInfo * pFontName = GetDefaultFontName();

		for ( INT32 i = 0 ; i < BoxCount ; ++i )
			pEditBoxes[i]->ReCreateTextEx(pFontName, FONT_COUNT_256, 7);
	}
#endif

	m_pCapacityBulletKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCapacityBulletSmokeKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCapacityLightKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCapacityNormalKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCapacityTerrainKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCapacityDepthKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCapacityBloomKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCapacityPhysXKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCapacityShadowKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCapacityTextureKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCapacitySpecularKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCapacityEffectKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);

	m_pCapacityBulletKey->SetTextColor(GetColor(GCT_STATIC));
	m_pCapacityBulletSmokeKey->SetTextColor(GetColor(GCT_STATIC));
	m_pCapacityLightKey->SetTextColor(GetColor(GCT_STATIC));
	m_pCapacityNormalKey->SetTextColor(GetColor(GCT_STATIC));
	m_pCapacityTerrainKey->SetTextColor(GetColor(GCT_STATIC));
	m_pCapacityDepthKey->SetTextColor(GetColor(GCT_STATIC));
	m_pCapacityBloomKey->SetTextColor(GetColor(GCT_STATIC));
	m_pCapacityPhysXKey->SetTextColor(GetColor(GCT_STATIC));
	m_pCapacityShadowKey->SetTextColor(GetColor(GCT_STATIC));
	m_pCapacityTextureKey->SetTextColor(GetColor(GCT_STATIC));
	m_pCapacitySpecularKey->SetTextColor(GetColor(GCT_STATIC));
	m_pCapacityEffectKey->SetTextColor(GetColor(GCT_STATIC));

	m_pCapacityBulletKey->SetText(GAME_STRING("STR_POPUP_TRACER_BULLET"));/*총알궤적*/
	m_pCapacityBulletSmokeKey->SetText(GAME_STRING("STR_POPUP_TRACER_BULLET_EFFECT"));/*총알궤적 이펙트*/
	m_pCapacityLightKey->SetText(GAME_STRING("STR_POPUP_DYNAMIC_LIGHT"));/*Dynamic Light*/
	m_pCapacityNormalKey->SetText(GAME_STRING("STR_POPUP_NORMAL_MAP"));/*Normal Map*/
	m_pCapacityTerrainKey->SetText(GAME_STRING("STR_POPUP_TERRAIN_EFFECT"));/*지형 이펙트*/
	m_pCapacityShadowKey->SetText(GAME_STRING("STR_POPUP_SHADOW_EFFECT"));/*그림자효과*/
	m_pCapacityTextureKey->SetText(GAME_STRING("STR_POPUP_TEXTURE_QUALITY"));/*텍스쳐 퀄리티*/	
	m_pCapacityDepthKey->SetText(GAME_STRING("STR_POPUP_16BIT_DEPTH_BUFFER"));/*16비트 깊이 버퍼*/
	m_pCapacityBloomKey->SetText(GAME_STRING("STR_POPUP_GLOW_EFFECT"));/*글로우 효과*/
	m_pCapacityPhysXKey->SetText(GAME_STRING("STR_POPUP_PHYSICS_EFFECT"));/*물리효과*/
	m_pCapacitySpecularKey->SetText(GAME_STRING("STR_POPUP_LIGHT_REFLECTION"));/*빛반사 효과*/
	m_pCapacityEffectKey->SetText(GAME_STRING("STR_POPUP_EFFECT_QUALITY"));/*이펙트 품질*/
	
	INIT_COMBO_EX(m_pCapacityShadowButton);
	INIT_COMBO_EX(m_pCapacityTextureButton);
	INIT_COMBO_EX(m_pCapacitySpecularButton);
	INIT_COMBO_EX(m_pCapacityEffectButton);

	m_pCapacityShadowCombo->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCapacityTextureCombo->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCapacitySpecularCombo->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCapacityEffectCombo->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCapacityShadowCombo->SetTextColor(GetColor(GCT_STATIC));
	m_pCapacityTextureCombo->SetTextColor(GetColor(GCT_STATIC));

	m_pCapacitySpecularCombo->SetTextColor(GetColor(GCT_STATIC));	
	m_pCapacityEffectCombo->SetTextColor(GetColor(GCT_STATIC));	

	m_pCapacityShadowPopup->CreateTextEx(4,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pCapacityShadowPopup->SetSelColor(100, 100, 100, 100);
	m_pCapacityShadowPopup->SetTextColor(GetColor(GCT_STATIC));
	m_pCapacityShadowPopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCapacityShadowPopup->SetExclusiveEnableControl(TRUE);
	m_pCapacityShadowPopup->AddElement(GAME_STRING("STR_POPUP_HIGH_QUALITY"));/*상*/
	m_pCapacityShadowPopup->AddElement(GAME_STRING("STR_POPUP_MID_QUALITY"));/*중*/
	m_pCapacityShadowPopup->AddElement(GAME_STRING("STR_POPUP_LOW_QUALITY"));/*하*/
	m_pCapacityShadowPopup->AddElement(GAME_STRING("STR_POPUP_UNUSED"));/*사용안함*/
	m_pCapacityShadowPopup->SetEnable(FALSE);
	
	m_pCapacityTexturePopup->CreateTextEx(3,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pCapacityTexturePopup->SetSelColor(100, 100, 100, 100);	
	m_pCapacityTexturePopup->SetTextColor(GetColor(GCT_STATIC));
	m_pCapacityTexturePopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCapacityTexturePopup->SetExclusiveEnableControl(TRUE);
	m_pCapacityTexturePopup->AddElement(GAME_STRING("STR_POPUP_HIGH_QUALITY"));/*상*/
	m_pCapacityTexturePopup->AddElement(GAME_STRING("STR_POPUP_MID_QUALITY"));/*중*/
	m_pCapacityTexturePopup->AddElement(GAME_STRING("STR_POPUP_LOW_QUALITY"));/*하*/
	m_pCapacityTexturePopup->SetEnable(FALSE);

	m_pCapacitySpecularPopup->CreateTextEx(3,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pCapacitySpecularPopup->SetSelColor(100, 100, 100, 100);
	m_pCapacitySpecularPopup->SetTextColor(GetColor(GCT_STATIC));
	m_pCapacitySpecularPopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCapacitySpecularPopup->SetExclusiveEnableControl(TRUE);
	m_pCapacitySpecularPopup->AddElement(GAME_STRING("STBL_IDX_QUALITY_TYPE1"));
	m_pCapacitySpecularPopup->AddElement(GAME_STRING("STBL_IDX_QUALITY_TYPE2"));
	m_pCapacitySpecularPopup->AddElement(GAME_STRING("STBL_IDX_QUALITY_TYPE3"));
	m_pCapacitySpecularPopup->SetEnable(FALSE);

	m_pCapacityEffectPopup->CreateTextEx(3,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pCapacityEffectPopup->SetSelColor(100, 100, 100, 100);
	m_pCapacityEffectPopup->SetTextColor(GetColor(GCT_STATIC));
	m_pCapacityEffectPopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCapacityEffectPopup->SetExclusiveEnableControl(TRUE);
	m_pCapacityEffectPopup->AddElement(GAME_STRING("STBL_IDX_QUALITY_TYPE1"));
	m_pCapacityEffectPopup->AddElement(GAME_STRING("STBL_IDX_QUALITY_TYPE2"));
	m_pCapacityEffectPopup->AddElement(GAME_STRING("STBL_IDX_QUALITY_TYPE3"));
	m_pCapacityEffectPopup->SetEnable(FALSE);

	// 아직 구현 안된것들은 비활성화 합니다.
	//m_pCapacityLight->setControlState(I3GUI_CONTROL_STATE_DISABLED);

	// 16비트 깊이버퍼는 항상 FALSE로 합니다.
	m_pCapacityDepth->SetEnable( FALSE);
	m_pCapacityDepthKey->SetEnable( FALSE);

#ifndef BLOOM_ENABLE
	m_pCapacityBloom->SetEnable( FALSE);
	m_pCapacityBloomKey->SetEnable( FALSE);
#endif

	// In Game Option에서 사용되지 않는 항목들을 비활성화 합니다.
	if( m_bInGame)
	{
		COLOR col;
		i3Color::Set( &col, (UINT8) 180, 180, 180, 100);

		m_pCapacityLightKey->SetTextColor(&col);
		m_pCapacityNormalKey->SetTextColor(&col);
		m_pCapacityTerrainKey->SetTextColor(&col);
		m_pCapacityShadowKey->SetTextColor(&col);
		m_pCapacityTextureKey->SetTextColor(&col);	
		m_pCapacityDepthKey->SetTextColor(&col);
		m_pCapacityBloomKey->SetTextColor(&col);
		m_pCapacitySpecularKey->SetTextColor(&col);
		m_pCapacityEffectKey->SetTextColor(&col);

		m_pCapacityShadowButton->setInputDisable( TRUE);
		m_pCapacityTextureButton->setInputDisable( TRUE);
		m_pCapacitySpecularButton->setInputDisable( TRUE);
		m_pCapacityEffectButton->setInputDisable( TRUE);

		m_pCapacityShadowCombo->SetTextColor( &col);
		m_pCapacityTextureCombo->SetTextColor( &col);
		m_pCapacitySpecularCombo->SetTextColor( &col);
		m_pCapacityEffectCombo->SetTextColor( &col);

		m_pCapacityLight->setInputDisable( TRUE);
		m_pCapacityLight->SetColor( I3GUI_CONTROL_STATE_NORMAL, &col);

		m_pCapacityNormal->setInputDisable( TRUE);
		m_pCapacityNormal->SetColor( I3GUI_CONTROL_STATE_NORMAL, &col);

		m_pCapacityTerrain->setInputDisable( TRUE);
		m_pCapacityTerrain->SetColor( I3GUI_CONTROL_STATE_NORMAL, &col);

		m_pCapacityDepth->setInputDisable( TRUE);
		m_pCapacityDepth->SetColor( I3GUI_CONTROL_STATE_NORMAL, &col);

#ifndef BLOOM_ENABLE
		m_pCapacityBloom->setInputDisable( TRUE);
		m_pCapacityBloom->SetColor( I3GUI_CONTROL_STATE_NORMAL, &col);
#endif

#ifndef RAGDOLL_OPTION_ENABLE
		m_pCapacityPhysX->setInputDisable( TRUE);
		m_pCapacityPhysX->SetColor( I3GUI_CONTROL_STATE_NORMAL, &col);
#endif
	}
}

////////////////////////////////////////////////////////////////
// 게임내에서의 성능설정
////////////////////////////////////////////////////////////////
void	CPopupOption::_UpdateShadow(void)
{
	// Shadow UI에서는 Shadow를 지원하지 않는다.
	CStageBattle * pStage = g_pFramework->GetStageBattle();

	if( pStage != NULL)
		g_pFramework->setShadowQuality( m_nShadowQualityType);
	else
		g_pFramework->setShadowQuality(0);

	switch( m_nShadowQualityType)
	{
	case 3:	m_pCapacityShadowCombo->SetText( GAME_STRING("STBL_IDX_QUALITY_TYPE1"));	// 상
		break;

	case 2:
		m_pCapacityShadowCombo->SetText( GAME_STRING("STBL_IDX_QUALITY_TYPE2"));	// 중
		break;

	case 1:
		m_pCapacityShadowCombo->SetText( GAME_STRING("STBL_IDX_QUALITY_TYPE3"));	// 하
		break;

	default:
		m_pCapacityShadowCombo->SetText( GAME_STRING("STBL_IDX_QUALITY_NONE"));	// 사용안함
		break;
	}
}
void	CPopupOption::_SetPerformanceShadow(UINT32 idxShadow)
{
	switch( idxShadow)
	{
		case 0 :		// 상
			m_nShadowQualityType = 3;
			break;

		case 1 :		// 중
			m_nShadowQualityType = 2;
			break;

		case 2 :		// 하
			m_nShadowQualityType = 1;
			break;

		default :
			m_nShadowQualityType = 0;		// 사용안함
			break;
	}

	_UpdateShadow();

	m_bNeedToValidate = true;		// Shader가 해당 GPU에서 사용 가능한지 확인이 필요!!
}
void	CPopupOption::_SetPerformanceBulletTrace(void)
{
	m_bEnableBulletTrace = m_pCapacityBullet->getCheckBoxState();

	// 상위 총알궤적이 비활성화 된다면, 하위 총알궤적도 같이 비활성화 됩니다.
	if( m_bEnableBulletTrace == FALSE) 
	{
		COLOR col;
		i3Color::Set( &col, (UINT8) 100, 100, 100, 255);

		m_pCapacityBulletSmoke->setCheckBoxState( FALSE);
		m_pCapacityBulletSmoke->setControlDisable( TRUE);
		m_bEnableBulletSmoke= FALSE;
		m_pCapacityBulletSmokeKey->SetTextColor(&col);
	}
	else
	{
		m_pCapacityBulletSmoke->setControlDisable( FALSE);
		m_pCapacityBulletSmokeKey->SetTextColor(GetColor(GCT_STATIC));
	}
}

void	CPopupOption::_SetPerformanceBulletSmokeTrace(void)
{
	m_bEnableBulletSmoke = m_pCapacityBulletSmoke->getCheckBoxState();
}

void	CPopupOption::_SetPerformanceDynamicLight(void)
{
	m_bDynamicLight = m_pCapacityLight->getCheckBoxState();

	m_bNeedToValidate = true;		// Shader가 해당 GPU에서 사용 가능한지 확인이 필요!!
}

void	CPopupOption::_SetPerformanceNormalMap(void)
{
	// 다시 게임을 시작하셔야만\n\n변경사항이 적용됩니다.
	UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STBL_IDX_EP_OPTION_RESTART_MESSAGE"));
	RePositionMsgBox( pBox);

	m_bEnableNormalMap = m_pCapacityNormal->getCheckBoxState();
	
	m_bNeedToValidate = true;		// Shader가 해당 GPU에서 사용 가능한지 확인이 필요!!
}

void	CPopupOption::_SetPerformanceTerrainEffect(void)
{
	m_bEnableTerrainEffect = m_pCapacityTerrain->getCheckBoxState();
}

void	CPopupOption::_SetPerformanceTextureQuality(UINT32 idxTexture)
{
	m_nTextureQualityType  = idxTexture;

	switch( m_nTextureQualityType)
	{
	case 0:
		m_pCapacityTextureCombo->SetText(GAME_STRING("STBL_IDX_QUALITY_TYPE1"));	// 상
		break;
	case 1:
		m_pCapacityTextureCombo->SetText( GAME_STRING("STBL_IDX_QUALITY_TYPE2"));	// 중
		break;
	case 2:
		m_pCapacityTextureCombo->SetText( GAME_STRING("STBL_IDX_QUALITY_TYPE3"));	// 하
		break;
	default:
		m_pCapacityTextureCombo->SetText( GAME_STRING("STBL_IDX_QUALITY_TYPE2"));	// 중
		break;
	}

	// 다시 게임을 시작하셔야만\n\n변경사항이 적용됩니다.
	UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STBL_IDX_EP_OPTION_RESTART_MESSAGE"));
	RePositionMsgBox( pBox);
}

void CPopupOption::_SetPerformanceSpecularQuality(UINT32 idxSpecular)
{
	
	switch(idxSpecular)
	{
	case 0:
		m_pCapacitySpecularCombo->SetText(GAME_STRING("STBL_IDX_QUALITY_TYPE1"));
		m_nSpecularQualityType = 2;
		break;
	case 1:
		m_pCapacitySpecularCombo->SetText(GAME_STRING("STBL_IDX_QUALITY_TYPE2"));
		m_nSpecularQualityType = 1;
		break;
	case 2:
		m_pCapacitySpecularCombo->SetText(GAME_STRING("STBL_IDX_QUALITY_TYPE3"));
		m_nSpecularQualityType = 0;
		break;
	default:
		m_pCapacitySpecularCombo->SetText(GAME_STRING("STBL_IDX_QUALITY_TYPE2"));
		break;
	}

	UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STBL_IDX_EP_OPTION_RESTART_MESSAGE"));
	RePositionMsgBox( pBox);
}

void CPopupOption::_SetPerformanceEffectQuality(UINT32 idxEffect)
{
	
	switch(idxEffect)
	{
	case 0:
		m_pCapacityEffectCombo->SetText(GAME_STRING("STBL_IDX_QUALITY_TYPE1"));
		m_nEffectQuality = 2;
		break;
	case 1:
		m_pCapacityEffectCombo->SetText(GAME_STRING("STBL_IDX_QUALITY_TYPE2"));
		m_nEffectQuality = 1;
		break;
	case 2:
		m_pCapacityEffectCombo->SetText(GAME_STRING("STBL_IDX_QUALITY_TYPE3"));
		m_nEffectQuality = 0;
		break;
	default:
		m_pCapacityEffectCombo->SetText(GAME_STRING("STBL_IDX_QUALITY_TYPE2"));
		break;
	}

	UIMessageBox * pBox = GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STBL_IDX_EP_OPTION_RESTART_MESSAGE"));
	RePositionMsgBox( pBox);
}

void CPopupOption::OnCapacityBullet(void)
{	
    _SetPerformanceBulletTrace();
}

void CPopupOption::OnCapacityBulletSmoke(void)
{	
    _SetPerformanceBulletSmokeTrace();
}

void CPopupOption::OnCapacityLight(void)
{
	i3GfxCaps * pCaps = i3RenderContext::GetCaps();

	if( m_pCapacityLight->getCheckBoxState())
	{
		switch( pCaps->getGPUClass())
		{
			case I3G_GPU_CLASS_F :
			case I3G_GPU_CLASS_E :
				if( m_bEnableNormalMap)
				{
					GameUI::MsgBox( MSG_TYPE_GAME_QUERY, CRITICAL_NOTIFY, NULL, MAKE_CALLBACK(_CbConfirmDynamicLight), this);
				}
				else
				{
					GameUI::MsgBox( MSG_TYPE_GAME_QUERY, NORMAL_NOTIFY, NULL, MAKE_CALLBACK(_CbConfirmDynamicLight), this);
				}
				break;

			case I3G_GPU_CLASS_D :
				GameUI::MsgBox( MSG_TYPE_GAME_QUERY, NORMAL_NOTIFY, NULL, MAKE_CALLBACK(_CbConfirmDynamicLight), this);
				break;

			default :
				_SetPerformanceDynamicLight();
		}
	}
	else
	{
		_SetPerformanceDynamicLight();
	}
}

void CPopupOption::_CbConfirmDynamicLight( void * pThis, INT32 nParam)
{
	CPopupOption * pDlg = (CPopupOption *) pThis;

	if( nParam == MBR_OK || g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ENTER ))
	{
		pDlg->_SetPerformanceDynamicLight();
	}
	else if( nParam == MBR_CANCEL || g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ESC ))
	{
		pDlg->m_pCapacityLight->setCheckBoxState( FALSE);
	}
}

void CPopupOption::OnCapacityNormal(void)
{
	i3GfxCaps * pCaps = i3RenderContext::GetCaps();

	if( m_pCapacityNormal->getCheckBoxState())
	{
		switch( pCaps->getGPUClass())
		{
			case I3G_GPU_CLASS_F :
			case I3G_GPU_CLASS_E :
			case I3G_GPU_CLASS_D :
				GameUI::MsgBox( MSG_TYPE_GAME_QUERY, CRITICAL_NOTIFY, NULL, MAKE_CALLBACK(_CbConfirmNormalMap), this);
				break;

			case I3G_GPU_CLASS_C :
				GameUI::MsgBox( MSG_TYPE_GAME_QUERY, NORMAL_NOTIFY, NULL, MAKE_CALLBACK(_CbConfirmNormalMap), this);
				break;

			default :
				_SetPerformanceNormalMap();
		}
	}
	else
	{
		_SetPerformanceNormalMap();
	}
}

void CPopupOption::_CbConfirmNormalMap( void * pThis, INT32 nParam)
{
	CPopupOption * pDlg = (CPopupOption *) pThis;

	if( nParam == MBR_OK || g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ENTER ))
	{
		pDlg->_SetPerformanceNormalMap();
	}
	else if( nParam == MBR_CANCEL || g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ESC ))
	{
		pDlg->m_pCapacityNormal->setCheckBoxState( FALSE);
	}
}

void CPopupOption::OnCapacityTerrain(void)
{
	_SetPerformanceTerrainEffect();
}

void CPopupOption::OnCapacityDepth(void)
{
	m_b16BitsDepth = m_pCapacityDepth->getCheckBoxState();
}

void CPopupOption::OnCapacityBloom(void)
{
	m_bEnableBloom = m_pCapacityBloom->getCheckBoxState();

	m_bNeedToValidate = true;
}

void CPopupOption::OnCapacityPhysX(void)
{
	m_bEnablePhysX = m_pCapacityPhysX->getCheckBoxState();

	m_bNeedToValidate = true;
}

void CPopupOption::OnShadowCombo(void)
{
	ExclusionPopup(m_pCapacityShadowPopup);
}

void CPopupOption::OnShadowPopup(UINT32 idxShadow)
{
	i3GfxCaps * pCaps = i3RenderContext::GetCaps();

	if( idxShadow != 3)
	{
		switch( pCaps->getGPUClass())
		{
			case I3G_GPU_CLASS_F :
			case I3G_GPU_CLASS_E :
			case I3G_GPU_CLASS_D :
				GameUI::MsgBox( MSG_TYPE_GAME_QUERY, CRITICAL_NOTIFY, NULL, MAKE_CALLBACK(_CbConfirmShadow), this);
				break;

			case I3G_GPU_CLASS_C :
			case I3G_GPU_CLASS_B :
				GameUI::MsgBox( MSG_TYPE_GAME_QUERY, NORMAL_NOTIFY, NULL, MAKE_CALLBACK(_CbConfirmShadow), this);
				break;

			default :
				_SetPerformanceShadow(idxShadow);
		}
	}
	else
	{
		_SetPerformanceShadow(idxShadow);
	}
}

void CPopupOption::_CbConfirmShadow( void * pThis, INT32 nParam)
{
	CPopupOption * pDlg = (CPopupOption *) pThis;

	if( nParam == MBR_OK || g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ENTER ))
	{
		pDlg->_SetPerformanceShadow( pDlg->m_pCapacityShadowPopup->getSelectedElement());
	}
	else if( nParam == MBR_CANCEL || g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ESC ))
	{
		pDlg->m_pCapacityShadowPopup->SetSelectedElement( 3, FALSE);
	}
}

void CPopupOption::OnTextureCombo(void)
{
	ExclusionPopup(m_pCapacityTexturePopup);
}

void CPopupOption::OnSpecularCombo(void)
{
	ExclusionPopup(m_pCapacitySpecularPopup);
}

void CPopupOption::OnEffectCombo(void)
{
	ExclusionPopup(m_pCapacityEffectPopup);
}

void CPopupOption::OnTexturePopup(UINT32 idxTexture)
{
	i3GfxCaps * pCaps = i3RenderContext::GetCaps();

	if( idxTexture == 1)
	{
		switch( pCaps->getGPUClass())
		{
			case I3G_GPU_CLASS_F :
			case I3G_GPU_CLASS_E :
				GameUI::MsgBox( MSG_TYPE_GAME_QUERY, NORMAL_NOTIFY, NULL, MAKE_CALLBACK(_CbConfirmShadow), this);
				break;

			default :
				_SetPerformanceTextureQuality(idxTexture);
		}
	}
	else
	{
		_SetPerformanceTextureQuality(idxTexture);
	}
}

void CPopupOption::OnSpecularPopup(UINT32 idxSpecular)
{
	if (idxSpecular == 1)
	{
		_SetPerformanceSpecularQuality(idxSpecular);
	}
	else
	{
		_SetPerformanceSpecularQuality(idxSpecular);
	}
}

void CPopupOption::OnEffectPopup(UINT32 idxEffect)
{
	_SetPerformanceEffectQuality(idxEffect);
}

void CPopupOption::_CbConfirmTextureQuality( void * pThis, INT32 nParam)
{
	CPopupOption * pDlg = (CPopupOption *) pThis;

	if( nParam == MBR_OK || g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ENTER ))
	{
		pDlg->_SetPerformanceTextureQuality( pDlg->m_pCapacityTexturePopup->getSelectedElement());
	}
	else if( nParam == MBR_CANCEL || g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ESC ))
	{
		pDlg->m_pCapacityTexturePopup->SetSelectedElement( 1, FALSE);
	}
}

void CPopupOption::_CbConfirmSpecularQuality(void* pThis,INT32 nParam)
{
	CPopupOption * pDlg = (CPopupOption *) pThis;

	if( nParam == MBR_OK || g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ENTER ))
	{
		pDlg->_SetPerformanceSpecularQuality( pDlg->m_pCapacitySpecularPopup->getSelectedElement());
	}
	else if( nParam == MBR_CANCEL || g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ESC ))
	{
		pDlg->m_pCapacitySpecularPopup->SetSelectedElement( 1, FALSE);
	}
}

void CPopupOption::_PopupNotifyValidateFail(void)
{
	/*변경하신 항목 중 일부 항목이 하드웨어 특성에 따라 자동으로 변경되었습니다.*/
	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_MESSAGE_AUTO_CHANGE_BY_HARDWARE"), NULL, MAKE_CALLBACK(_CbNotifyValidateFail), this);
}

void CPopupOption::_CbNotifyValidateFail( void * pThis, INT32 nParam)
{
	CPopupOption * pDlg = (CPopupOption *) pThis;

	if( nParam == MBR_OK || g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ENTER ))
	{
		if( pDlg->_NeedToUserVerify())
		{
			pDlg->_PopupUserVerify();
		}
		else
		{
			// 사용자의 확인 불필요한 변경이라면 그냥 설정하고 나감.
			((CPopupOption*)pThis)->OptionSave();		
			((CPopupOption*)pThis)->OnClosePopup();
			((CPopupOption*)pThis)->m_bEnableConfirm = FALSE;
		}
	}
}

bool CPopupOption::_ValidateShader(void)
{
	i3ShaderCode code;

	code.setDiffuseMapEnable( true);
	code.setReflectMapEnable( true);
	code.setReflectMaskMapEnable( true);
	code.setPPLShader( false);
	code.setLightModel( I3G_SHADER_FLAG_LIGHT_MODEL_PHONG);
	code.setLightType( 0, I3G_SHADER_FLAG_LIGHT_TYPE_DIRECTIONAL);
	code.setLightingEnable( true);

#if defined(BLOOM_ENABLE)
	// Bloom 효과
	if( m_bEnableBloom)
	{
		g_pFramework->CreateBloom();		
	}
	g_pFramework->SetBloom( m_bEnableBloom);
#endif

	// 설정된 Option에 따라 Shader 코드를 정한다.
	if( m_bEnableNormalMap)
	{
		code.setNormalMapEnable( true);
		code.setPPLShader( true);
	}

	switch( g_pEnvSet->m_nSpecularQualityType)
	{
		case 0 :
			break;

		case 1 :
			code.setSpecularMapEnable( true);
			break;

		case 2 :
			code.setSpecularMapEnable( true);
			code.setFresnelEnable( true);
			break;
	}

	if( m_bDynamicLight)
	{
		code.setLightType( 1, I3G_SHADER_FLAG_LIGHT_TYPE_POINT);
	}

	if( m_nShadowQualityType != 0)
	{
		code.setShadowType( 0, I3G_SHADER_FLAG_SHADOW_SHADOWMAP);
	}

	i3RenderContext * pCtx = g_pViewer->GetRenderContext();

	if( pCtx->ValidateShader( &code) == false)
	{
		// 해당 Shader가 활성화가 되지 않는다.

		// code에는 Downgrading된 Shader Code가 담겨 있다.
		// 이것을 바탕으로 역으로 Option을 재설정 해둔다.

		if( m_bEnableNormalMap && (code.getNormalMapEnable() == false))
		{
			// Normal Map 옵션이 변경되었다.
			g_pEnvSet->m_bEnableNormalMap = m_bEnableNormalMap = FALSE;

			m_pCapacityNormal->setCheckBoxState( false);
		}

		if( (m_nSpecularQualityType > 0) && (code.getSpecularMapEnable() == false))
		{
			// Specular Map이 강제 Off되었다.
			g_pEnvSet->m_nSpecularQualityType = m_nSpecularQualityType = 0;
		}

		if( (m_nSpecularQualityType > 1) && (code.getFresnelEnable() == false))
		{
			// Fresnel이 강제 Off되었다.
			if( code.getSpecularMapEnable())
				g_pEnvSet->m_nSpecularQualityType = m_nSpecularQualityType = 1;
			else
				g_pEnvSet->m_nSpecularQualityType = m_nSpecularQualityType = 0;
		}


		if((m_nShadowQualityType != 0) && (code.getShadowType( 0) != I3G_SHADER_FLAG_SHADOW_SHADOWMAP))
		{
			// Shadow가 강제 Off되었다
			g_pEnvSet->m_nShadowQualityType = m_nShadowQualityType = 0;

			_SetPerformanceShadow( 3);
		}

		_PopupNotifyValidateFail();

		return false;
	}

	return true;
}
