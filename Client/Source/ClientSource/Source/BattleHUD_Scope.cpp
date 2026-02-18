#include "pch.h"
#include "BattleHUD_Scope.h"

#include "GameCharaMoveContext.h"
#include "ConfigRenderBattle.h"

#include "Camera/GameCamera_OnObject.h"
#include "CrosshairMgr.h"
#include "UserInfoContext.h"

I3_CLASS_INSTANCE( CHUD_Scope);

namespace inner { CHUD_Scope* this_ptr = 0; }
CHUD_Scope* CHUD_Scope::instance() { return inner::this_ptr; }

CHUD_Scope::CHUD_Scope()
{
	inner::this_ptr = this;

	m_pDefaultTex			= nullptr;
	m_pSpriteScope			= nullptr;

	m_bEnabledCenterImage			= false;
	m_bEnabledSniperScopeHUD		= false;
	m_bEnablebScopeBreath			= false;
	m_rElapsedScopeEnable			= 0.0f;

	m_nAspectScreenWidth	= 0;
	m_nAspectScreenHeight	= 0;
	m_nAspectOffsetWidth	= 0;

	m_pDefaultTex = g_pFramework->GetResourceManager()->LoadTexture("Gui/Hud/scope.i3i");
}

CHUD_Scope::~CHUD_Scope()
{
	inner::this_ptr = 0;
	i3::cu::for_each_safe_release_clear(m_vecScopeHudTexture, &i3Texture::Release);
	I3_SAFE_RELEASE(m_pDefaultTex);
	I3_SAFE_NODE_RELEASE(m_pSpriteScope);
}	

void CHUD_Scope::CreateScopeHUD( i3Node * pRoot)
{
	I3ASSERT( pRoot != nullptr);

	REAL32 fImageWidth = 1024;
	REAL32 fImageHeight = 768; 

	i3Viewer * pViewer		= g_pFramework->GetViewer();

	m_nAspectScreenWidth	= pViewer->GetViewWidth();
	m_nAspectScreenHeight	= pViewer->GetViewHeight();	
	
	REAL32 aspect = (REAL32) m_nAspectScreenWidth / (REAL32) m_nAspectScreenHeight;

	// Sniper Scope Sprite
	// Sprite = 0: 메인(왼쪽) / 1: 메인(오른쪽) / 2: 왼쪽땜질 / 3: 오른쪽 떔질 / 4: 가운데 점( 사용이 안될 수 도 있음)
	// 리소스의 절약을 위해 기존 메인부분을 반으로 나눈 리소스를 사용.  	
	m_pSpriteScope = i3Sprite2D::new_object();	
	m_pSpriteScope->Create( 5, true, false);
	m_pSpriteScope->SetEnable(true);	
	m_pSpriteScope->SetTexture(m_pDefaultTex);
	m_pSpriteScope->SetTextureCoord( 0, 0.0f, 0.0f, fImageWidth*0.5f, fImageHeight);
	m_pSpriteScope->SetTextureCoord( 1, fImageWidth*0.5f, 0.0f, 0.0f, fImageHeight);
	m_pSpriteScope->SetTextureCoord( 2, 0.0f, 0.0f, 1.0f, fImageHeight); 
	m_pSpriteScope->SetTextureCoord( 3, 1.0f, 0.0f, 0.0f, fImageHeight);	
	m_pSpriteScope->SetTextureCoord( 4, 4.0f, 773.0f, 50.0f, 819.0f);	

	//+offset을 해준 이유는 픽셀은 정수 단위, 계산값은 실수단위로 값이 0.5f보다(반 내림) 적으면 갈라지는 형상이 생긴다.
	INT32 offset = 0;
	if( m_nAspectScreenWidth == 1600 && m_nAspectScreenHeight == 1024 )
		offset = 1;

	// 와이드 화면일 경우 Scope 위치 계산
	if( aspect > (4.0f/3.0f))
	{
		m_nAspectScreenWidth = (INT32)((REAL32) m_nAspectScreenHeight * (4.0f/3.0f));	//	이미지의 4:3로 맞춤

		m_nAspectOffsetWidth = (INT32)((pViewer->GetViewWidth() - m_nAspectScreenWidth) * 0.5f);	//	땜질 넓이
	}

	//	화면 가운데 HUD(왼쪽)
	m_pSpriteScope->SetRect( 0, (REAL32)m_nAspectOffsetWidth, 0.0f, 
		(REAL32)(m_nAspectScreenWidth*0.5f) + offset,
		(REAL32) m_nAspectScreenHeight);

	//	화면 가운데 HUD(오른쪽)
	m_pSpriteScope->SetRect( 1, (REAL32)(pViewer->GetViewWidth()*0.5f), 0.f, 
		(REAL32)(m_nAspectScreenWidth*0.5f) + offset, 
		(REAL32) m_nAspectScreenHeight);

	//	화면 외쪽 HUD
	m_pSpriteScope->SetRect( 2, 0.0f, 0.0f, 
								(REAL32)m_nAspectOffsetWidth, (REAL32) m_nAspectScreenHeight);

	//	화면 오른쪽 HUD
	m_pSpriteScope->SetRect( 3, (REAL32)(pViewer->GetViewWidth() - m_nAspectOffsetWidth), 0.0f, 
								(REAL32)m_nAspectOffsetWidth, (REAL32)m_nAspectScreenHeight);

	//	화면 중앙 Dot HUD
	m_pSpriteScope->SetRect( 4, m_nAspectOffsetWidth+m_nAspectScreenWidth * 0.5f - 23.0f, m_nAspectScreenHeight * 0.5f - 23.0f, 
								46.0f, 46.0f);
	m_pSpriteScope->SetEnable( 4, false);

	pRoot->AddChild( m_pSpriteScope);

	SetSniperScope( false);
}

void CHUD_Scope::SetSniperScope( bool bVisible, CWeaponInfo * pWeaponInfo, WeaponBase * pWeapon)					
{
	if( bVisible)
	{
		m_bEnablebScopeBreath = false;

		if (!pWeaponInfo)
		{
			TRACE("CHUD_Scope::SetSniperScope pWeaponInfo = nullptr\n");
			return;
		}

		I3ASSERT( pWeaponInfo != nullptr);
		if( pWeaponInfo->GetScopeType() > -1 )
		{
			INT32 nScopeType	= pWeaponInfo->GetScopeType();
			INT32 nSubScopeType = pWeaponInfo->GetScopeSubType();

			const SUB_SCOPE_INFO* sub_scope_info = nullptr;
			if (nSubScopeType > -1)
				sub_scope_info = g_pWeaponInfoDataBase->GetSubScopeInfo(nSubScopeType);

			if(	sub_scope_info->m_bOnScopeSound	)
				m_bEnablebScopeBreath = true;

			//texture setting
			if ((m_vecScopeHudTexture.size() > (size_t)nScopeType && nScopeType >= 0) &&
				m_vecScopeHudTexture[nScopeType] != nullptr)
			{
				REAL32 fImageWidth = 1024;
				REAL32 fImageHeight = 768;

				m_pSpriteScope->SetEnable(true);
				m_pSpriteScope->SetTexture(m_vecScopeHudTexture[nScopeType]);
				m_pSpriteScope->RemoveFlag(I3_NODEFLAG_DISABLE);

				m_pSpriteScope->SetTextureCoord(0, 0.0f, 0.0f, fImageWidth*0.5f, fImageHeight);
				m_pSpriteScope->SetTextureCoord(1, fImageWidth*0.5f, 0.0f, 0.0f, fImageHeight);
				m_pSpriteScope->SetTextureCoord(2, 0.0f, 0.0f, 1.0f, fImageHeight);
				m_pSpriteScope->SetTextureCoord(3, 0.0f, 0.0f, 1.0f, fImageHeight);
				m_pSpriteScope->SetEnable(4, false);
			}
			
			//subtexture setting
			if (sub_scope_info != nullptr)
			{
				REAL32 texture_u	= sub_scope_info->m_vec2DotStartPos.x;
				REAL32 texture_v	= sub_scope_info->m_vec2DotStartPos.y;
				REAL32 width		= sub_scope_info->m_vec2DotWidthHeight.x;
				REAL32 height		= sub_scope_info->m_vec2DotWidthHeight.y;

				if (!(width == 0.0f || height == 0.0f) && !UserInfoContext::i()->IsGM_Observer())
				{
					m_pSpriteScope->SetTextureCoord(4, texture_u, texture_v, texture_u + width, texture_v + height);
					m_pSpriteScope->SetRect(4, m_nAspectOffsetWidth + m_nAspectScreenWidth * 0.5f - (width / 2.0f), m_nAspectScreenHeight * 0.5f - (height / 2.0f),
											width, height);
					m_pSpriteScope->SetEnable(4, true);
					m_bEnabledCenterImage = true;
				}
				else
				{
					m_pSpriteScope->SetEnable(4, false);
					m_bEnabledCenterImage = false;
				}
			}
			m_bEnabledSniperScopeHUD = true;
		}

	}
	else
	{
		m_pSpriteScope->AddFlag( I3_NODEFLAG_DISABLE);
		m_bEnabledSniperScopeHUD = false;
	}
}

void CHUD_Scope::UpdateSniperScope( REAL32 rDeltaTime)
{
	if( m_bEnabledSniperScopeHUD && m_bEnablebScopeBreath)
	{
		CGameCharaBase * pChara = g_pCamera->getCurTarget();
		if( pChara != nullptr && (pChara->getMoveContext() != nullptr) && ( pChara->getMoveContext()->getAccelElapsed() <= 0.0f))
		{
			if( m_rElapsedScopeEnable < 2.0f && m_rElapsedScopeEnable + rDeltaTime >= 2.0f)
			{
				if( pChara->getCharaInfo() != nullptr)
				{
					g_pSndMng->StartCharacterActionSnd( GTBCA_ON_ZOOMBREATH, pChara->getDBInfo()->GetCharaSexTypeDISPLAY(), nullptr, true);
				}
			}

			m_rElapsedScopeEnable += rDeltaTime;
		}
		else
		{
			m_rElapsedScopeEnable = 0.0f;
		}
	}
	else
	{
		m_rElapsedScopeEnable = 0.0f;
	}
}

void CHUD_Scope::SetEnableSinperScope( bool bFlag)
{
	m_pSpriteScope->SetEnable(bFlag);
	
	if( bFlag && !m_bEnabledCenterImage)
		m_pSpriteScope->SetEnable(4, false);
}
	
void CHUD_Scope::InsertScopeHudTexture(INT32 index)
{
	if (!i3::vu::is_value_valid(m_vecScopeHudTexture, index))
	{
		// 텍스쳐
		i3::rc_wstring texture_path;
		g_pWeaponInfoDataBase->GetScopeHUDTexturePath(index, texture_path);

		i3::string conv;
		i3::utf16_to_mb(texture_path, conv);
		i3Texture * pTex = g_pFramework->GetResourceManager()->LoadTexture(conv.c_str());
		i3::vu::set_value_force(m_vecScopeHudTexture, index, pTex);
	}
}