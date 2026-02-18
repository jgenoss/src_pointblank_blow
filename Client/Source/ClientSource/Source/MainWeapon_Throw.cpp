#include "pch.h"

#include "MainWeapon_Throw.h"
#include "GameCharaWeaponContext.h"
#include "Weapon/WeaponItemID.h"

void	Reset_WeaponThrowData(WeaponThrowData* outData)
{
	outData->m_bCheckThrow = false;
	outData->m_rSpeed = 0.f;
	outData->m_rTracerUpdateTime = 0.f;
	outData->m_rElapsedThrowTime = 0.f;
	
	outData->m_BombState = 0;

	i3Vector::Zero( &outData->m_vTracerOldPos);
	i3Vector::Zero( &outData->m_vOldPos);
	i3Vector::Zero( &outData->m_vStartPos);		//시작 위치
	i3Vector::Zero( &outData->m_vDirection);	//방향
}



MainWeapon_Throw::MainWeapon_Throw(WeaponBase* p) : MainWeapon(p), m_nTraceEffectTickCount(0)
{
	Reset_WeaponThrowData(&m_ThrowData);	
}


void	MainWeapon_Throw::OnInitVariable()
{
	Reset_WeaponThrowData(&m_ThrowData);	

	m_nTraceEffectTickCount = 0;
}

void	MainWeapon_Throw::Reset()
{
	Reset_WeaponThrowData(&m_ThrowData);	

	m_nTraceEffectTickCount = 0;
}


void	MainWeapon_Throw::OnFire_Grenade( i3AIContext * pCtx, REAL32 tm)
{

	REAL32	LocalTime = pCtx->getLocalTime();

	if( m_pWeaponBase->getOwner() == nullptr )
		return;
	
	//던지기 동작 시작
	if( LocalTime == 0.0f)
	{	
		// 연이어 발사하지 못하도록...
		m_pWeaponBase->setWeaponState( WEAPON::STATE_FIRE);

		if( (m_pWeaponBase->getOwner() != nullptr) && m_pWeaponBase->getOwner()->isLocal())
			m_pWeaponBase->addWeaponStateMask( WSM_FIRE_DISABLE);
	}
	//던지기 동작
	else
	{
		if( !m_ThrowData.m_bCheckThrow )
		{
			m_ThrowData.m_bCheckThrow = this->_CheckThrow(LocalTime);
		}
	}

	if( m_ThrowData.m_bCheckThrow && LocalTime > m_pWeaponBase->GetFireDelay() )
	{
		m_pWeaponBase->SetEnable( true);

		m_pWeaponBase->_OnPlayWeaponIdleAnim();

		m_pWeaponBase->removeWeaponStateMask( WSM_FIRING | WSM_FIRE_DISABLE);
	}

}


void	MainWeapon_Throw::OnEndFire( i3AIContext * pCtx, REAL32 tm)
{
	m_pWeaponBase->SetEnable( true);
	m_pWeaponBase->_OnPlayWeaponIdleAnim();
	m_pWeaponBase->removeWeaponStateMask( WSM_FIRING | WSM_FIRE_DISABLE);
	m_ThrowData.m_bCheckThrow = false;
}

void MainWeapon_Throw::CheckChainExplosion( VEC3D * pStartPos, REAL32 rRange, REAL32 rRadian/* = I3_PI*/)
{
	if( m_pWeaponBase->IsDedicatedHost())
		return;

	VEC3D vDir;
	i3CollideeLine line;
	line.SetStart( pStartPos );
	UINT32 max_count_throwing1 = WEAPON::getItemCount(WEAPON_CLASS_THROWING_GRENADE);		// 현재는 7이다..

	// C5, Claymore 검사
	UINT32 ItemCount = WEAPON::getSettableItemCount();

	for ( UINT32 i = 0 ; i < ItemCount ; ++i )
	{
		WEAPON::RES_ID nIndex = WEAPON::getSettableItemIndex( i );

		// 버그로 의심되는데, 일단은 갯수 문제를 표시하고 넘길수 있게 처리함..
		if ( nIndex >= max_count_throwing1 )
		{
			I3PRINTLOG(I3LOG_WARN, "getSettableItemIndex의 결과값이 m_pThrowWeaponList배열 크기를 넘어섰습니다.");
			continue;
		}

		INT32 nCount = g_pWeaponManager->getThrowWeaponCount( nIndex );

		for( INT32 idx = 0; idx < nCount; ++idx )
		{
			WeaponBase * pThrow = static_cast<WeaponBase*>( g_pWeaponManager->getThrowWeapon( nIndex, idx) );

			if( ! i3::same_of<WeaponC5*, WeaponClaymore*>(pThrow) )
				continue;

			if( pThrow == nullptr)					continue;
			if( pThrow == m_pWeaponBase)		continue;
			if( !pThrow->isEnable() )			continue;
			if( !pThrow->getOwner()->isLocal())	continue;

			MainWeapon_Throw * pMainWeapon = static_cast<MainWeapon_Throw*>( pThrow->GetMainWeapon());

			if( !pMainWeapon->isBombState( WEAPON_BOMBSTATE_THROWING) )	continue;
			if( pMainWeapon->isBombState(WEAPON_BOMBSTATE_EXPLOSION) )	continue;

			if( !m_pWeaponBase->IsValidExplosionArea(pThrow->GetPos(), rRange, rRadian, false) )	continue;

			i3Vector::Sub( &vDir, pThrow->GetPos(), pStartPos );
			REAL32 rLen = i3Vector::Length( &vDir);

			if( rLen < rRange )
			{
				line.SetDir( &vDir);

				I3_PHYSIX_HIT_RESULT * pResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK);
				if( pResult != nullptr)
				{
					if( pResult->m_pDestShapeSet->getShapeGroup() != CGRP_ITEM_HIT)
					{
						// 해당 오브젝트보다 가까운 거리에 무엇인가 있으면 스킵
						if( pResult->m_T < rLen)
							continue;
					}
				}

				// 자동 폭발하도록 커맨드를 줍니다.
				// 거리에 따라 딜레이 타임을 줍니다.
				/*GAMETOCONTEXT_INFO_WEAPON_THROW		throwInfo(pThrow->getWeaponInfo()->GetItemID(), pThrow->GetPos(), NET_GRENADE_STATE_EXPLOSION);

				throwInfo._pNormal			= pMainWeapon->GetThrowInfo_Normal();

				GAMEEVENT::Write_ThrowingWeapon( pThrow->getNetworkIdx(), &throwInfo );*/

				CHARA_DAMAGEINFO damageInfo;

				pThrow->GetDamageInfo(&damageInfo);

				if (m_pWeaponBase->IsGrenade())
					damageInfo._DeathType = CHARA_DEATH_B;

				damageInfo._HitType = GHT_ITEM_C5;
				damageInfo._nVictimIdx = pThrow->getNetworkIdx();
				damageInfo._rDamage = pThrow->GetDamage(false);

				GAMEEVENT::Write_Chara_Hit(m_pWeaponBase->getOwner(), &damageInfo);

				//pMainWeapon->ChainExplosion();
			}
		}
	}
}

void MainWeapon_Throw::UpdateTraceEffect(REAL32 fElapsedTime)
{
	REAL32 len;
	VEC3D vDir;
	COLOR	color;

	CWeaponGlobal * pGlobalInfo = g_pWeaponInfoDataBase->getGlobal();

	REAL32 fThickness = 1.0f;
	REAL32 fLifeTime = 0.0f;
	i3Vector::Sub( &vDir, m_pWeaponBase->GetPos(), &m_ThrowData.m_vTracerOldPos); 
	len = i3Vector::Length( &vDir); 
	i3Vector::Normalize( &vDir, &vDir);

	//	궤적 분류
	TRACER_TYPE		type	= m_pWeaponBase->getWeaponInfo()->GetTracerType();

	if( type == TRACER_TYPE_UNKNOWN )
	{
		I3PRINTLOG(I3LOG_FATAL,  "throw weapon invalid type.");
		type	= TRACER_TYPE_HAND_GRENADE;
	}

	switch( type)
	{
	case TRACER_TYPE_HAND_GRENADE:			
		{
			//	궤적 두께
			fThickness = pGlobalInfo->m_rTracerHandGrenadeThickness;
			fLifeTime = pGlobalInfo->m_rTracerHandGrenadePieceLifeTime;

			//	궤적 색상
			i3Color::Copy(&color, &pGlobalInfo->m_TracerHandGrenadeColor);
		}
		break;
	case TRACER_TYPE_SMOKE:			
		{
			//	궤적 두께
			fThickness = pGlobalInfo->m_rTracerSmokeThickness;	
			fLifeTime = pGlobalInfo->m_rTracerSmokePieceLifeTime;

			//	궤적 색상
			i3Color::Copy(&color, &pGlobalInfo->m_TracerSmokeColor);
		}
		break;
	case TRACER_TYPE_FLASH:		
		{
			//	궤적 두께
			fThickness = pGlobalInfo->m_rTracerFlashThickness;
			fLifeTime = pGlobalInfo->m_rTracerFlashPieceLifeTime;

			//	궤적 색상
			i3Color::Copy(&color, &pGlobalInfo->m_TracerFlashColor);
		}
		break;

	}

	//	궤적 발사
	{
		if (m_nTraceEffectTickCount == 0)
		{
			i3Vector::Copy( &m_ThrowData.m_vTracerOldPos, m_pWeaponBase->GetPos());
			m_nTraceEffectTickCount++;
		}

		TracerBulletInfo tracerInfo;
		tracerInfo._Type = type;
		tracerInfo._LifeTime = fLifeTime;
		i3Vector::Copy( &tracerInfo._vBeginPos, &m_ThrowData.m_vTracerOldPos);
		i3Vector::Copy( &tracerInfo._vDestPos, m_pWeaponBase->GetPos());
		tracerInfo._Length = len;
		tracerInfo._Thickness = fThickness;
		tracerInfo._EndSpeed = 0;
		i3Color::Copy( &tracerInfo._color, &color);

		g_pTracerBulletManager->Fire( &tracerInfo);
	}
	
	//VEC3D vPos = *GetPos();
	//I3TRACE("보이는 궤적 : OLD: %f, %f, %f DEST: %f, %f, %f\n", m_vTracerOldPos.x, m_vTracerOldPos.y, m_vTracerOldPos.z, vPos.x, vPos.y, vPos.z);

	i3Vector::Copy( &m_ThrowData.m_vTracerOldPos, m_pWeaponBase->GetPos());

	//I3TRACE( "Grenade Tracer : Update Time %.4f, Length : %.4f, Speed : %.4f\n", m_rTracerUpdateTime, len, speed);
}
