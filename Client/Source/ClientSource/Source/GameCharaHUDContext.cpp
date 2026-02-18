#include "pch.h"
#include "GameCharaHUDContext.h"
#include "GameCharaContexts.h"
#include "StageBattle.h"

#include "Camera/GameCamera_OnObject.h"

#include "StageObject/ObjectWithWeapon.h"
#include "StageObject/ObjectRidingHelicopter.h"
#include "StageObject/ObjectRespawnHelicopter.h"

#include "./Character/GameCharaHUDControl.h"
#include "./Character/GameCharaHUDControlDino.h"
#include "./Character/GameCharaHUDControlTRex.h"
#include "./Character/GameCharaHUDControlRaptor.h"
#include "./Character/GameCharaHUDControlSting.h"
#include "./Character/GameCharaHUDControlElite.h"

#include "BattleHud.h"
#include "BattleHUD_Scope.h"
#include "BattleSlotContext.h"
#include "UserInfoContext.h"
#include "HackContext.h"
#include "UI/UIBattleFrame.h"
#include "UI/UIHudObserver.h"

I3_CLASS_INSTANCE( CGameCharaHUDContext);//, CGameCharaContextBase);

CGameCharaHUDContext::~CGameCharaHUDContext()
{
	OnDestroy();
}

/*virtual*/ void CGameCharaHUDContext::OnCreate( CGameCharaBase * pOwner, INT32 defaultView /*= 0*/, INT32 defaultNetwork /*= 0*/)
{
	m_pHUDControl = CGameCharaHUDControl::new_object();
	m_pHUDControl->BindOwner( pOwner);

	m_pCurrentControl = m_pHUDControl;

	m_pStingHUDControl = CGameCharaHUDControlSting::new_object();
	m_pStingHUDControl->BindOwner( pOwner);

	m_pEliteHUDControl = CGameCharaHUDControlElite::new_object();
	m_pEliteHUDControl->BindOwner( pOwner);


	m_pRaptorHUDControl = CGameCharaHUDControlRaptor::new_object();
	m_pRaptorHUDControl->BindOwner( pOwner);

	m_pTRexHUDControl = CGameCharaHUDControlTRex::new_object();
	m_pTRexHUDControl->BindOwner( pOwner);

	CGameCharaContextBase::OnCreate( pOwner, defaultView, defaultNetwork);
}

/*virtual*/ void CGameCharaHUDContext::OnDestroy( void)
{
	m_pCurrentControl = nullptr;
	I3_SAFE_RELEASE( m_pHUDControl);
	I3_SAFE_RELEASE( m_pRaptorHUDControl);
	I3_SAFE_RELEASE( m_pTRexHUDControl);
	I3_SAFE_RELEASE( m_pStingHUDControl);
	I3_SAFE_RELEASE( m_pEliteHUDControl);
}

/*virtual*/ void CGameCharaHUDContext::OnChange1PV( void)
{	
	if(m_pOwner->IsDino())
	{
		if(m_pOwner->IsRex())
		{
			OnChangeTrex();
			return;
		}
		else if (m_pOwner->IsSting())
		{
			OnChangeSting();
			return;
		}
		else if (m_pOwner->IsElite())
		{
			OnChangeElite();
			return;
		}
		else if (m_pOwner->IsCCRaptor())
		{
			OnChangeCCRaptor();
			return;
		}
		else if (m_pOwner->IsCCSting())
		{
			OnChangeCCSting();
			return;
		}
		else if (m_pOwner->IsCCAcid())
		{
			OnChangeCCAcid();
			return;
		}
		else if (m_pOwner->IsRaptorMercury())
		{
			OnChangeRaptorMercury();
			return;
		}
		else if (m_pOwner->IsStingMars())
		{
			OnChangeStingMars();
			return;
		}
		else if (m_pOwner->IsAcidVulcan())
		{
			OnChangeAcidVulcan();
			return;
		}
		else
		{
			OnChangeRaptor();
			return;
		}
	}

	m_pCurrentControl = m_pHUDControl;

	if( g_pFramework->IsBattleStage() )
	{
		if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM ) &&
			(m_pOwner->GetCurrentCharaWeapon() != nullptr) )
		{
			SetScope( true, m_pOwner->GetCurrentCharaWeapon());
		}
	}
}
/*virtual*/ void CGameCharaHUDContext::OnChangeSting()
{
	m_pCurrentControl = m_pStingHUDControl;
}
/*virtual*/ void CGameCharaHUDContext::OnChangeTrex( )
{
	m_pCurrentControl = m_pTRexHUDControl;
}
/*virtual*/ void CGameCharaHUDContext::OnChangeRaptor()
{
	m_pCurrentControl = m_pRaptorHUDControl;
}
/*virtual*/ void CGameCharaHUDContext::OnChangeElite()
{
	m_pCurrentControl = m_pEliteHUDControl;
}
/*virtual*/ void CGameCharaHUDContext::OnChangeCCRaptor()
{
	m_pCurrentControl = m_pRaptorHUDControl;
}

/*virtual*/ void CGameCharaHUDContext::OnChangeCCSting()
{
	m_pCurrentControl = m_pStingHUDControl;
}

/*virtual*/ void CGameCharaHUDContext::OnChangeCCAcid()
{
	m_pCurrentControl = m_pRaptorHUDControl;
}

/*virtual*/ void CGameCharaHUDContext::OnChangeRaptorMercury()
{
	m_pCurrentControl = m_pRaptorHUDControl;
}

/*virtual*/ void CGameCharaHUDContext::OnChangeStingMars()
{
	m_pCurrentControl = m_pStingHUDControl;
}

/*virtual*/ void CGameCharaHUDContext::OnChangeAcidVulcan()
{
	m_pCurrentControl = m_pRaptorHUDControl;
}

/*virtual*/ void CGameCharaHUDContext::OnReset( void)
{
	i3Color::Set( &m_EFFECT_ScreenColor, (UINT8) 0, 0, 0, 255);
	m_fBlurStart				= 0.f;
	m_fBlurEnd					= 0.f;
	m_nScrEffectType			= SCR_EFFECT_NONE;

	m_rDamageElapseTime			= 0.f;
	m_rFlashElapseTime			= 0.0f;
	m_rFlashEffectiveTime		= 0.0f;
}

/*virtual*/ void CGameCharaHUDContext::OnUpdate( REAL32 rDeltatime)
{
	i3PostProcessMotionBlur * pMotionBlur = g_pFramework->GetMotionBlur();
	I3ASSERT( pMotionBlur != nullptr);

	CHud * pHud = CHud::i();
	
	bool	bActiveEffect = false;
	INT32	Tex2DAlpha = 0;
	
	switch( m_nScrEffectType)
	{	
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//	워프 이펙트 효과
	case SCR_EFFECT_JUMPER:
		{
			if(pMotionBlur->IsEnable())
			{
				bActiveEffect = true;

				REAL32 fTargetFOV = g_pCamera->GetCameraDefaultFOV() * g_pCamera->getZoomRatio();
				REAL32 maxTime = pMotionBlur->GetBlurTime();

/*				REAL32 fEffectFOV = 0.f;
				REAL32 gap = 0.f;
				REAL32 procTime = 0.f;
				REAL32 fCurFOV = 0.f;  

				if(pRasterizeBlur->GetProcessingTime() < pRasterizeBlur->GetBlurTime() * 0.7f) 
				{
					fEffectFOV = 100.f;
					gap = fEffectFOV - fDefFOV;
					procTime = maxTime - (pRasterizeBlur->GetProcessingTime() * 1.3f);
					fCurFOV = fDefFOV + ((procTime / maxTime) * gap);
				}
				else
				{
					fEffectFOV = fDefFOV - 30.f;
					gap = fDefFOV - fEffectFOV;
					procTime = pRasterizeBlur->GetProcessingTime();
					fCurFOV = fEffectFOV + ((procTime / maxTime) * gap);
				}*/

#if 1 
				REAL32 fEffectFOV = fTargetFOV + 25.f;
				REAL32 gap = fEffectFOV - fTargetFOV;
				REAL32 procTime = maxTime - pMotionBlur->GetProcessingTime();
				REAL32 ratio = procTime / maxTime;
				REAL32 accel = ratio * ratio * ratio * ratio;
				REAL32 fCurFOV = fTargetFOV + (ratio * gap * accel);
#else
				REAL32 fEffectFOV = 25.f;
				REAL32 gap = fTargetFOV - fEffectFOV;
				REAL32 procTime = pRasterizeBlur->GetProcessingTime();
				REAL32 fCurFOV = fEffectFOV + ((procTime / maxTime) * gap);

#endif
				g_pCamera->SetCameraFOV( fCurFOV);

				//char temp[256] = "";
				//sprintf_s(temp, "effect %f (%f)\n", fCurFOV, fTargetFOV);
				//I3TRACE(temp);
			}
			else
			{
				bActiveEffect = false;
				m_nScrEffectType = SCR_EFFECT_NONE;

				REAL32 fTargetFOV = g_pCamera->GetCameraDefaultFOV() * g_pCamera->getZoomRatio();

				g_pCamera->SetCameraFOV(fTargetFOV);

				pMotionBlur->EndMotionBlur();
			}
		}
		break;
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//	데미지 스크린 이펙트
	case SCR_EFFECT_DAMAGE:
		{
			bActiveEffect = true;
			m_rDamageElapseTime -= rDeltatime;

			if( m_rDamageElapseTime <= 0.0f)
			{	//	블러 효과(?)가 서서히 사라진다.
				if( m_fBlurStart > m_fBlurEnd)
				{			
					m_fBlurStart -= 0.02f;
		
					//	색상의 알파가 사라진다.
					INT32 a = (INT32) i3Color::GetA( &m_EFFECT_ScreenColor);
					a -= 2;
					if( a > 255)	a = 255; 
					else if( a < 0) a = 0;
					i3Color::SetA( &m_EFFECT_ScreenColor, (UINT8) a);

					I3TRACE( "데미지 감소중..(blur %.4f / alpha %d)\n", m_fBlurStart, i3Color::GetA( &m_EFFECT_ScreenColor));
				}
				else
				{
					bActiveEffect = false;
				
					m_fBlurStart = m_fBlurEnd = 0.0f;
					m_rDamageElapseTime = 0.0f;

					m_nScrEffectType = SCR_EFFECT_NONE;

					I3TRACE( "데미지 종료..\n");
				}
			}
			else
			{
				I3TRACE( "데미지 진행중..(blur %.4f / alpha %d)\n", m_fBlurStart, i3Color::GetA( &m_EFFECT_ScreenColor));
			}
		}
		break;
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//	섬광 이펙트 시간 계산 (섬광탄에서 사용)
	case SCR_EFFECT_FLASHBANG:
		{	
			Tex2DAlpha = 150;
			bActiveEffect = true;
			m_rFlashElapseTime -= rDeltatime;

			if( m_rFlashElapseTime <= 0.f)
			{	
				Tex2DAlpha = (INT32) i3Color::GetA( &m_EFFECT_ScreenColor);		
				Tex2DAlpha -= (INT32) (rDeltatime * 255.0f);
				if( Tex2DAlpha < 0)	Tex2DAlpha = 0;

				//	이펙트가 끝나고 서서히 없어진다.
				if( Tex2DAlpha <= 0)
				{
					m_rFlashElapseTime = 0.0f;
					m_rFlashEffectiveTime = 0.0f;

					bActiveEffect = false;
					m_nScrEffectType = SCR_EFFECT_NONE;

					g_pSndMng->setOutStandingEffect( false, 0.0f);

					//I3TRACE( "플뱅 종료..\n");
				}
				else
				{
					i3Color::Set( &m_EFFECT_ScreenColor, 255, 255, 255, (UINT8) Tex2DAlpha);	

					//I3TRACE( "플뱅 감소중..(alpha %d)\n", Tex2DAlpha);
				}
			}
			else	//	이펙트 중
			{
				REAL32 fMinReduceAlpha = 150.0f;
				REAL32 fMinAlpha = 255.0f - fMinReduceAlpha;
				REAL32 fGapTime = m_rFlashEffectiveTime - m_rFlashElapseTime;
				INT32 alpha = 0;

				if( fGapTime <= 0.5f)		//	갑자기 하얀화면으로 페이드 된다.
				{
					alpha = (INT32) (fGapTime * 2.0f * 255.f);
					if( alpha > 255) alpha = 255;
				}	
				else if( fGapTime <= 1.0f)	//	하얀화면이 지속된다.
				{
					alpha = 255;
				}					 
				else						//	하얀화면이 울렁(?) 거린다.
				{			
					REAL32 fAlpha = i3Math::abs( i3Math::sin( m_rFlashElapseTime)) * fMinReduceAlpha;
					alpha = (INT32) (255.f - fAlpha);
					if( alpha < (INT32) fMinAlpha)		alpha = (INT32) fMinAlpha;
				}

				i3Color::Set( &m_EFFECT_ScreenColor, 255, 255, 255, (UINT8) alpha);	

				//I3TRACE( "플뱅 진행중..(alpha %d)\n", alpha);
			}			
		}
		break;
	case SCR_EFFECT_HOWL:
		{
			if(pMotionBlur->IsEnable())
			{
				bActiveEffect		= true;
			}
			else
			{
				bActiveEffect		= false;
				m_nScrEffectType	= SCR_EFFECT_NONE;

				pMotionBlur->EndMotionBlur();
			}
		}
		break;

	case SCR_EFFECT_DEATHBLOW :
		{	
			Tex2DAlpha = 150;
			bActiveEffect = true;
			m_rBlowElapseTime -= rDeltatime;

			if( m_rBlowElapseTime <= 0.f)
			{	
				Tex2DAlpha = (INT32) i3Color::GetA( &m_EFFECT_ScreenColor);		
				Tex2DAlpha -= (INT32) (rDeltatime * 255.0f);
				if( Tex2DAlpha < 0)	Tex2DAlpha = 0;

				Tex2DAlpha = (INT32) i3Color::GetA( &m_EFFECT_ScreenColor);		
				Tex2DAlpha -= (INT32) (rDeltatime * 255.0f);
				if( Tex2DAlpha < 0)	Tex2DAlpha = 0;

				//	이펙트가 끝나고 서서히 없어진다.
				if( Tex2DAlpha <= 0)
				{
					m_rFlashElapseTime = 0.0f;
					m_rFlashEffectiveTime = 0.0f;

					bActiveEffect = false;
					m_nScrEffectType = SCR_EFFECT_NONE;
					REAL32 fTargetFOV = g_pCamera->GetCameraDefaultFOV() * g_pCamera->getZoomRatio();
					g_pCamera->SetCameraFOV(fTargetFOV);
					pMotionBlur->EndMotionBlur();
				
				}
				else
				{
					i3Color::Set( &m_EFFECT_ScreenColor, 255, 255, 255, (UINT8) Tex2DAlpha);	
				}
			}
			else	//	이펙트 중
			{
				REAL32 fMinReduceAlpha = 150.0f;
				REAL32 fMinAlpha = 255.0f - fMinReduceAlpha;
				REAL32 fGapTime = m_rBlowEffectiveTime - m_rBlowElapseTime;
				INT32 alpha = 0;

				if( fGapTime <= 0.5f)		
				{
					alpha = (INT32) (fGapTime * 2.0f * 255.f);
					if( alpha > 255) alpha = 255;
				}	
				else if( fGapTime <= 1.0f)	
				{
					alpha = 255;
				}					 
				else						
				{			
					REAL32 fAlpha = i3Math::abs( i3Math::sin( m_rBlowElapseTime)) * fMinReduceAlpha;
					alpha = (INT32) (255.f - fAlpha);
					if( alpha < (INT32) fMinAlpha)		alpha = (INT32) fMinAlpha;
				}

				i3Color::Set( &m_EFFECT_ScreenColor, 70, 24, 0, (UINT8) alpha);	
				if(pMotionBlur->IsEnable())
				{
					bActiveEffect		= true;

					REAL32 fTargetFOV = g_pCamera->GetCameraDefaultFOV() * g_pCamera->getZoomRatio();
					REAL32 maxTime = pMotionBlur->GetBlurTime();

#if 1 
					REAL32 fEffectFOV = fTargetFOV + 25.f;
					REAL32 gap = fEffectFOV - fTargetFOV;
					REAL32 procTime = maxTime - pMotionBlur->GetProcessingTime();
					REAL32 ratio = procTime / maxTime;
					REAL32 accel = ratio * ratio * ratio * ratio;
					REAL32 fCurFOV = fTargetFOV + (ratio * gap * accel);
#else
					REAL32 fEffectFOV = 25.f;
					REAL32 gap = fTargetFOV - fEffectFOV;
					REAL32 procTime = pRasterizeBlur->GetProcessingTime();
					REAL32 fCurFOV = fEffectFOV + ((procTime / maxTime) * gap);

#endif
					g_pCamera->SetCameraFOV( fCurFOV);

				}
			}
		}




		break;
	default:
		{
			bActiveEffect = false;

			i3Color::Set( &m_EFFECT_ScreenColor, (UINT8) 0, 0, 0, 0);
			m_fBlurStart		= 0.0f;
			m_fBlurEnd			= 0.0f;
		}
		break;
	};

	//	배틀중에서만 체크합니다.
	if (g_pFramework->IsBattleStage())
	{
		if( g_pCamera != nullptr && g_pCamera->getCurTarget() == m_pOwner)
		{
			//	1인칭 카메라에서만 작동함 (3인칭 카메라는 스크린 이펙트에 영향을 받지 않는다.)
			if( IsCamFPS() || IsCamKill())
			{	
				//	이펙트 활성화
				if( bActiveEffect)
				{
					if( m_nScrEffectType == SCR_EFFECT_DAMAGE)
					{
						pMotionBlur->SetEnable( false);					
					}
					else if( m_nScrEffectType == SCR_EFFECT_FLASHBANG)
					{
						if( pHud != nullptr)
						{
							pHud->SetScreenTex2DEnable( 0, true);
							pHud->SetScreenTex2DColor( 0, 255, 255, 255, (UINT8) Tex2DAlpha);
							
							for( INT32 i = 1; i < SCR_BACKBUFFER_COUNT; i++)
							{	//	첫번째 스크린만 빼고 안나오게
								pHud->SetScreenTex2DEnable( i, false);
							}
						}
					}

					if( pHud != nullptr)
					{
						pHud->SetScreenEffectEnable( true);
						pHud->SetScreenEffectColor( &m_EFFECT_ScreenColor);	
					}
				}
				//	이펙트 비활성화
				else
				{		
					ClearScreenEffect();
				} 
			}
			//	3인칭 카메라
			else
			{
				ClearScreenEffect();
			}
		}
	}
}

bool CGameCharaHUDContext::IsEffectAvailable( void)
{
	if (g_pFramework->IsBattleStage())
	{				
//		if( g_pCameraManager != nullptr && g_pCamera->getCurTarget() == m_pOwner)
		if ( CGameCameraManager::i() != nullptr && g_pCamera->getCurTarget() == m_pOwner)
		{
			if( IsCamFPS() || IsCamKill())
				return true;
		}
	}

	return false;
}

void CGameCharaHUDContext::ClearScreenEffect( void)
{
	i3PostProcessMotionBlur * pMotionBlur = g_pFramework->GetMotionBlur();
	I3ASSERT( pMotionBlur != nullptr);

	pMotionBlur->EndMotionBlur();

	i3Color::Set( &m_EFFECT_ScreenColor, (UINT8) 0, 0, 0, 0);
	m_fBlurStart		= 0.0f;
	m_fBlurEnd			= 0.0f;

	m_rFlashElapseTime = m_rFlashEffectiveTime = 0.0f;
	m_rDamageElapseTime = 0.0f;

	m_nScrEffectType	= SCR_EFFECT_NONE;

	if (false == g_pFramework->IsBattleStage()) return;

	CHud * pHud = CHud::i();
	if( pHud == nullptr)	return;

	if (pHud->GetScreenEffect())
	{
		pHud->SetScreenEffectEnable( false);
		pHud->SetScreenTex2DEnable( false);
	}
	else
		TRACE("CGameCharaHUDContext::ClearScreenEffect pHud->GetScreenEffect() = nullptr\n");
	
}

bool CGameCharaHUDContext::ScreenEffect_Flashbang( VEC3D * pVecWeaponPos, REAL32 rEffectiveTime, REAL32 rExplosionRange, REAL32 rExplosionSlope, REAL32 rDamagedLength)
{
	I3ASSERT( pVecWeaponPos != nullptr);

	CHud * pHud = CHud::i();

	if( rEffectiveTime == 0.0f) return false;

	i3Color::Set( &m_EFFECT_ScreenColor, (UINT8) 255, 255, 255, 255);

	if( g_pFramework->IsBattleStage() && pHud != nullptr)
	{
		if( IsCamFPS() && g_pCamera->getCurTarget() == m_pOwner)
		{
			for( INT32 i = 0; i < SCR_BACKBUFFER_COUNT; i++)
				pHud->SetCurrentScreenImage( i, 255, 255, 255, 255);
		}
	}
	
	// 거리에 따른 효과 증감율( 1.0f가 Default입니다.)
	REAL32  rValidRate	= 1.0f - ((1.0f / rExplosionRange) * rDamagedLength) * rExplosionSlope;
	if( rValidRate < I3_EPS)	rValidRate = I3_EPS;

	//	바라보는 각도에 따라 영향을 받는다.
	{					
		VEC3D vCharaDir, vFlashDir;					
		i3Vector::Copy( &vCharaDir, i3Matrix::GetAt( m_pOwner->GetMatrix()));
		i3Vector::Sub( &vFlashDir, pVecWeaponPos, m_pOwner->GetPos());
		i3Vector::Normalize( &vFlashDir, &vFlashDir);

		REAL32 fCharaFlashAngle = i3Vector::Dot( &vCharaDir, &vFlashDir);
		fCharaFlashAngle = I3_RAD2DEG( fCharaFlashAngle);
		if( fCharaFlashAngle <= 0.0f)
			fCharaFlashAngle = i3Math::abs( fCharaFlashAngle) + 90.f; 					
							
		//	완전히 영향 받는 각도 0 ~ 70도 
		if( fCharaFlashAngle > 110.0f)		// 1/4 영향 받는 각도 110 ~ 180
		{
			rValidRate *= 0.25f;
		}
		else if( fCharaFlashAngle > 70.f)	// 1/2 영향 받는 각도 70 ~ 110
		{
			rValidRate *= 0.5f;
		}
	}	

	//	지속 시간 계산
	m_rFlashEffectiveTime = rEffectiveTime * rValidRate;

	////-----------------	Anti Flashbang Mask
	//	사용자가 사용중이면 플뱅 지속시간에 영향을 준다. (자신 사용)
	if( CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_ANTI_FLASHBANG_MASK) == true )
	{
		REAL32 ratio = ((REAL32)VALUE_CASH_ITEM_ANTI_FLASHBANG_MASK * 0.01f);
		ratio += (ratio > 0 ? I3_EPS : -I3_EPS);

		m_rFlashEffectiveTime += (m_rFlashEffectiveTime * ratio);
	}

	m_rFlashElapseTime = m_rFlashEffectiveTime;
	m_nScrEffectType = SCR_EFFECT_FLASHBANG;

	g_pSndMng->setOutStandingEffect( true, m_rFlashElapseTime);

	return true;	
}



void CGameCharaHUDContext::ScreenEffect_Blow( REAL32 rProcessTime)
{
	CHud * pHud = CHud::i();

	i3Color::Set( &m_EFFECT_ScreenColor, (UINT8) 255, 0, 0, 0);

	if (g_pFramework->IsBattleStage() && pHud != nullptr)
	{
		if( IsCamFPS() && g_pCamera->getCurTarget() == m_pOwner)
		{
			for( INT32 i = 0; i < SCR_BACKBUFFER_COUNT; i++)
				pHud->SetCurrentScreenImage( i, 255, 255, 255, 0);
		}
	}
	m_rBlowEffectiveTime = rProcessTime;
	m_rBlowElapseTime = m_rBlowEffectiveTime;

	i3PostProcessMotionBlur * pMotionBlur = g_pFramework->GetMotionBlur();
	I3ASSERT( pMotionBlur != nullptr);

	pMotionBlur->StartMotionBlur( rProcessTime, 0.9f, 0.3f); 

	m_nScrEffectType = SCR_EFFECT_DEATHBLOW;

}
void CGameCharaHUDContext::ScreenEffect_Jumper( REAL32 rProcessTime)
{
	i3PostProcessMotionBlur * pMotionBlur = g_pFramework->GetMotionBlur();
	I3ASSERT( pMotionBlur != nullptr);

	pMotionBlur->StartMotionBlur( rProcessTime, 0.9f, 0.3f); 

	m_nScrEffectType = SCR_EFFECT_JUMPER;
}

bool CGameCharaHUDContext::ScreenEffect_Howl( REAL32 rEffectTime)
{
	if( rEffectTime == 0.f) return false;

	i3PostProcessMotionBlur * pMotionBlur = g_pFramework->GetMotionBlur();
	I3ASSERT( pMotionBlur != nullptr);

	pMotionBlur->StartMotionBlur(rEffectTime * 2.0f, 0.9f, 0.3f); 

	//	지속 시간 계산
	m_rFlashElapseTime = m_rFlashEffectiveTime = rEffectTime;
	
	m_nScrEffectType = SCR_EFFECT_HOWL;

	g_pSndMng->setOutStandingEffect( true, rEffectTime);

	return true;
}


bool CGameCharaHUDContext::__CalcProjectionPos( REAL32 * pOutX, REAL32 * pOutY, VEC3D * pIn )
{
	VEC3D	vTemp;
	VEC3D	vTempIn;
	MATRIX	CamMat;
	VEC3D*	pvCamAt;
	VEC3D	vSub;
	REAL32	Height = 1.9f;

	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))	return false;

	pvCamAt = i3Matrix::GetAt( g_pCamera->getCamMatrix());

	i3Vector::Sub( &vSub, pIn, m_pOwner->GetPos() );
	i3Vector::Normalize( &vSub, &vSub );

	if( i3Vector::Dot( pvCamAt, &vSub) < 0.f ) 
		return false;
	
	i3Matrix::Mul( &CamMat, g_pCamera->getViewMatrix(), g_pCamera->getProjectionMatrix());

	i3Vector::Set( &vTempIn, i3Vector::GetX( m_pOwner->GetPos() ), i3Vector::GetY( m_pOwner->GetPos() ) + Height, i3Vector::GetZ( m_pOwner->GetPos() ) );
	
	i3Vector::TransformCoord( &vTemp, &vTempIn, &CamMat );

	*pOutX	= i3Vector::GetX( &vTemp );
	*pOutY	= i3Vector::GetY( &vTemp );

	return true;
}

void CGameCharaHUDContext::ShowCharacterNick( bool bFriendly)
{
	if( CHud::i() == nullptr)
		return;

	CHud * pHud = CHud::i();

	REAL32 rPosX, rPosY;

	VEC3D * pPos = i3Matrix::GetPos( g_pCamera->getCamMatrix());
	
	if( i3::same_of<CGameCamera_OnObject*>(g_pCamera))
	{
		i3Object * pObj = ((CGameCamera_OnObject*)g_pCamera)->getOnObject();
		if( pObj != nullptr)
		{
			if( i3::same_of<CGameObjectRidingHelicopter*>(pObj))
			{
				pPos = i3Matrix::GetPos( ((CGameObjectRidingHelicopter*)pObj)->getRootDummy()->GetCacheMatrix());
			}
			else if( i3::same_of<CGameObjectRespawnHelicopter*>(pObj))
			{
				pPos = i3Matrix::GetPos( ((CGameObjectRespawnHelicopter*)pObj)->getRootDummy()->GetCacheMatrix());
			}
			else
			{
				I3PRINTLOG(I3LOG_WARN,  "invalid object type");
				return;
			}
		}
	}

    if( __CalcProjectionPos( &rPosX, &rPosY, pPos ))
	{
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
		if (!m_pOwner)
		{
			I3PRINTLOG(I3LOG_NOTICE, " Owner is nullptr ");
			return;
		}
#endif

 		rPosX *= 0.5f;		rPosX += 0.5f;
		rPosY *= -0.5f;		rPosY += 0.5f;
		
		VEC3D vTemp;
	//	char szTemp[MAX_STRING_COUNT] = {0,};
		i3::stack_wstring wstrStackTemp;

		i3Vector::Sub( &vTemp, pPos, m_pOwner->GetPos());
		INT32 nLength = (INT32)i3Vector::Length(&vTemp);
		if( nLength > 9000 || nLength < -9000) nLength = 0;

		INT32 slot = m_pOwner->getCharaNetIndex();

		if( HackContext::i()->isHackUserCaution( slot) && !(m_pOwner->getCharaInfo()->GetAIToClient()))
		{
//내부버전 핵체크를 위한..
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)

			switch(HackContext::i()->getHackUsertype(slot))
			{
			case HACK_CHECK_TYPE_CLIENT :
				i3::sprintf( wstrStackTemp, L"체크타입 : 클라 사유 : %d \n%s", HackContext::i()->getHackNumber(slot), BattleSlotContext::i()->getNickForSlot( slot ) );
				break;
			case HACK_CHECK_TYPE_SERVER_D :
				i3::sprintf( wstrStackTemp, L"체크타입 : 데디 사유 : %d \n%s", HackContext::i()->getHackNumber(slot), BattleSlotContext::i()->getNickForSlot( slot ) );
				break;
			default:
				i3::sprintf( wstrStackTemp, L"%s\n%s", GAME_RCSTRING("STR_HACKER_ID" ), BattleSlotContext::i()->getNickForSlot( slot ) );
			}
#endif
		}
		else
		{
			

			const GLOBAL_CHARA_INFO* pCharaInfo = m_pOwner->getCharaInfo();
			i3::rc_wstring wstrCharaNickName = BattleSlotContext::i()->getNickForSlot( slot, CHARA::Type2BufferType( pCharaInfo->GetType()));

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)

			i3::sprintf( wstrStackTemp, L"%s [%dm]-%dHP", wstrCharaNickName, nLength, m_pOwner->getCurHP());

			if( Domination_Util::enable_domi_nick == false && pCharaInfo->GetType() == CHARA::TYPE_DOMI_FULLBODY_AI)
				wstrStackTemp.clear();

#else
			wstrStackTemp = wstrCharaNickName;

			if( pCharaInfo->GetType() == CHARA::TYPE_DOMI_FULLBODY_AI)
				wstrStackTemp.clear();
#endif
		}

#if defined( DEF_OBSERVER_MODE)
		if( BattleSlotContext::i()->isObserverGM())
#else
		if( UserInfoContext::i()->IsGM_Observer())
#endif
		{		
			//StringCchPrintf(szTemp, MAX_STRING_COUNT, "%s [%dm]-%dHP", BattleSlotContext::i()->getNickForSlot( m_pOwner->getCharaNetIndex()), nLength, m_pOwner->getCurHP());
			i3::sprintf(wstrStackTemp, L"%s -%dHP", 
				BattleSlotContext::i()->getNickForSlot( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType()), 
				m_pOwner->getCurHP());

			pHud->SetCharaNickForGM( wstrStackTemp, m_pOwner->getCharaNetIndex(),
				BattleSlotContext::i()->IsRedTeam( m_pOwner->getCharaNetIndex()), 
				rPosX, rPosY, (REAL32) nLength);
		}
		else {
			// Friendly
			pHud->SetCharaNick( wstrStackTemp, bFriendly, rPosX, rPosY, (REAL32) nLength);
		}
	}
}

void CGameCharaHUDContext::SetScope( bool bEnable, WeaponBase * pWeapon)
{
	if( bEnable == false )
	{
		CHUD_Scope::instance()->SetSniperScope( bEnable, 0, pWeapon); //false 일 경우엔, 두번째 매개인자는 불필요한 존재가 된다.
	}
	else
	{
		if( pWeapon == 0 ) return;

		CWeaponInfo * pInfo = pWeapon->getWeaponInfo();
		if( pInfo == 0 ) return;

		CHUD_Scope::instance()->SetSniperScope( bEnable, pInfo, pWeapon);
	}
}

void CGameCharaHUDContext::ShowMissionNotice( const i3::rc_wstring& wstrMessage, REAL32 lifeTime /*= 3.f*/, INT32 offsetX /*= 0*/, INT32 offsetY /*= 0*/)
{
	if( CHud::i())
		CHud::i()->SetMissionNoticeString( wstrMessage, lifeTime, offsetX, offsetY);
}


void CGameCharaHUDContext::_ApplyCrosshair( void)
{	
	m_pCurrentControl->OnApplyCrosshair();
}

void CGameCharaHUDContext::ChangeWeapon( void)
{
	m_pCurrentControl->OnChangeWeapon();

	if (m_pOwner)
		observer_hud::Context::i()->Notify(observer_hud::EVT_WEAPON_CHANGED, m_pOwner->getCharaNetIndex());
}

void CGameCharaHUDContext::_Damage( VEC3D * pDir /*= nullptr*/, bool bDirectionView /*= false*/, INT32 filter_index /*=-1*/ )
{
	if( m_pOwner->is1PV() )
		m_pCurrentControl->OnDamage( pDir, m_pOwner->getBoneContext()->getTheta(), bDirectionView, filter_index );
}



