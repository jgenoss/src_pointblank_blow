#include "pch.h"
#include "MainWeapon_RLShell.h"

#include "GameCharaBoneContext.h"
#include "StageObject/ObjectRespawnHelicopter.h"
#include "GameMissionManager.h"
#include "BattleServerContext.h"
#include "BattleObjContext.h"
#include "Weapon/WeaponInfo/GrenadeGunInfo.h"

#define NET_GRENADESHELL_OFFTIME		15.f				// 네트워크로 15초간 못 받으면 삭제

#define GRENADESHELL_POS_ASSISTTIME		0.1f
#define GRENADESHELL_POS_ASSISTTIMEINV	10.f

MainWeapon_RLShell::MainWeapon_RLShell(WeaponBase* p) : MainWeapon_Throw(p)
{
	
	i3mem::FillZero( m_vRandDirection, sizeof( m_vRandDirection));
	i3mem::FillZero( m_vRandSub, sizeof( m_vRandSub));
	i3mem::FillZero( m_vRandTangent, sizeof( m_vRandTangent));

	m_bExplosion = false;
	m_bGrazeSoundPlayed = false;
	m_fSmokeLocalTime = 0.f;
	m_nFlyingStep = 1;
	m_rElapsedExplosionTime = 0.f;
	m_rElapsedFlyingTime = 0.f;
}

void	MainWeapon_RLShell::OnInitVariable()
{
	MainWeapon_Throw::OnInitVariable();

	m_rElapsedExplosionTime = 0.0f;
	m_fSmokeLocalTime		= 0.0f;
	m_rElapsedFlyingTime	= 0.0f;
	
	m_bExplosion = false;
	m_nFlyingStep = 0;
	m_bGrazeSoundPlayed = false;
}

void	MainWeapon_RLShell::Reset()
{
	MainWeapon_Throw::Reset();

	m_pWeaponBase->setWeaponStateMask(0);

	m_rElapsedExplosionTime = 0.0f;
	m_fSmokeLocalTime		= 0.0f;
	m_rElapsedFlyingTime	= 0.0f;

	m_bExplosion			= false;
	m_nFlyingStep = 0;
	m_bGrazeSoundPlayed = false;

	m_pWeaponBase->SetEnable( true); //SetVisible( true);
}

void MainWeapon_RLShell::Throw( VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, bool bOrigin)
{	
	addBombState( GRENADESHELL_STATE_LAUNCH);

	m_ThrowData.m_rElapsedThrowTime		= 0.f;
	m_fSmokeLocalTime		= 0.f;
	
	// 로켓 발사 경과 시간을 초기화 합니다.
	m_rElapsedFlyingTime = 0.0f;

	// 발사 스텝을 초기화합니다.
	m_nFlyingStep = 0;

	m_ThrowData.m_rSpeed				= rSpeed;

	i3Vector::Copy( &m_ThrowData.m_vStartPos, vStart);
	i3Vector::Copy( &m_ThrowData.m_vDirection, vDir);

	i3Vector::Copy( &m_ThrowData.m_vOldPos, vStart);
	i3Vector::Copy( &m_ThrowData.m_vTracerOldPos, vStart);

	// Random한 지역 4개를 구합니다.
	{			
		VEC2D vec2D;

		REAL32 deviation = m_pWeaponBase->getOwner()->getBoneContext()->getFireDeviation();

		if( deviation <= 0.0f) deviation = 0.01f;

		for( INT32 i=0; i<4; i++)
		{	
			// 처음 랜덤 구역은 발사위치와 동일하게 합니다.
			if( i == 0)
			{
				i3Vector::Set( &vec2D, i3Vector::GetX( &m_ThrowData.m_vStartPos), i3Vector::GetY( &m_ThrowData.m_vStartPos));
			}
			else if( i == 3)	// 마지막 랜덤 구역역시 발사위치와 거의 동일하게 합니다(명중률 향상을 위해.. 하지만 약간의 랜덤값은 적용.)
			{
				do
				{
					REAL32 xRand = i3Math::Randf2() * (deviation * 0.05f);
					REAL32 yRand = i3Math::Randf2() * (deviation * 0.05f);
					i3Vector::Set( &vec2D, i3Vector::GetX( &m_ThrowData.m_vStartPos) + xRand, i3Vector::GetY( &m_ThrowData.m_vStartPos) + yRand );
				}while( isBeing( &vec2D));
			}
			else
			{
				do
				{
					REAL32 xRand = i3Math::Randf2() * (deviation * 0.25f);
					REAL32 yRand = i3Math::Randf2() * (deviation * 0.25f);
					i3Vector::Set( &vec2D, i3Vector::GetX( &m_ThrowData.m_vStartPos) + xRand, i3Vector::GetY( &m_ThrowData.m_vStartPos) + yRand );
				}while( isBeing( &vec2D));
			}

			i3Vector::Copy( &m_vRandDirection[i], &vec2D);
		}
	}

	//  로켓이 날라가는 첫번째 궤적 연산을 합니다.
	i3Vector::Sub( &m_vRandSub[0], &m_vRandDirection[1], &m_vRandDirection[0]);			
	i3Vector::Normalize( &m_vRandTangent[0], &m_vRandSub[0]);

	i3Vector::Sub( &m_vRandSub[1], &m_vRandDirection[2], &m_vRandDirection[1]);			
	i3Vector::Add( &m_vRandTangent[1], &m_vRandSub[0], &m_vRandSub[1]);
	i3Vector::Normalize( &m_vRandTangent[1], &m_vRandTangent[1]);

}

void MainWeapon_RLShell::UpdateFlying( REAL32 rDeltaSeconds)
{
	if( rDeltaSeconds > 0.5f)
		rDeltaSeconds = 0.016f;

	m_ThrowData.m_rElapsedThrowTime += rDeltaSeconds;

	if( m_pWeaponBase->IsDedicatedHost() )
	{
		CheckWorldCollision( rDeltaSeconds);
	}
	else
	{	// 네트워크 처리입니다.
		UpdateNetworkData( rDeltaSeconds);
	}

	m_fSmokeLocalTime += rDeltaSeconds;

	if( m_ThrowData.m_rElapsedThrowTime < 1.0f)
	{
		if( m_fSmokeLocalTime > 0.03f)
		{
			m_fSmokeLocalTime = 0.0f;			
			// 로켓뒤에 달리는 이펙트
			g_pEffectManager->AddEnvironmentEffect(ENV_EFFECT_RPG7_SMK_COMMON, m_pWeaponBase->GetPos(), i3Matrix::GetUp(m_pWeaponBase->GetMatrix()));
		}
	}
	else if( m_ThrowData.m_rElapsedThrowTime < 1.8f)
	{
		if( m_fSmokeLocalTime > 0.075f)
		{
			m_fSmokeLocalTime = 0.0f;
			// 로켓뒤에 달리는 이펙트
			g_pEffectManager->AddEnvironmentEffect(ENV_EFFECT_RPG7_SMK_COMMON, m_pWeaponBase->GetPos(), i3Matrix::GetUp(m_pWeaponBase->GetMatrix()));
		}
	}
	else if( m_ThrowData.m_rElapsedThrowTime < 3.0f)
	{
		if( m_fSmokeLocalTime > 0.15f)
		{
			m_fSmokeLocalTime = 0.0f;
			// 로켓뒤에 달리는 이펙트
			g_pEffectManager->AddEnvironmentEffect(ENV_EFFECT_RPG7_SMK_COMMON, m_pWeaponBase->GetPos(), i3Matrix::GetUp(m_pWeaponBase->GetMatrix()));
		}
	}
	else if( m_ThrowData.m_rElapsedThrowTime < 6.0f)
	{
		if( m_fSmokeLocalTime > 0.4f)
		{
			m_fSmokeLocalTime = 0.0f;
			// 로켓뒤에 달리는 이펙트
			g_pEffectManager->AddEnvironmentEffect(ENV_EFFECT_RPG7_SMK_COMMON, m_pWeaponBase->GetPos(), i3Matrix::GetUp(m_pWeaponBase->GetMatrix()));
		}
	}
}



void MainWeapon_RLShell::CheckWorldCollision( REAL32 rDeltaSeconds)
{
	VEC3D	vTemp;
	REAL32	rLength;
	i3CollideeLine ColLine;
	I3_PHYSIX_HIT_RESULT * pResult;
	VEC3D	vNew;	
	D3DXVECTOR2 d3dVec2;

	m_rElapsedFlyingTime += rDeltaSeconds * 2.0f;

	if( m_rElapsedFlyingTime >= 1.0f) 
	{
		m_rElapsedFlyingTime = 0.0f;
		m_nFlyingStep++;

		switch( m_nFlyingStep)
		{
		case 0:
			{
				i3Vector::Sub( &m_vRandSub[0], &m_vRandDirection[1], &m_vRandDirection[0]);			
				i3Vector::Normalize( &m_vRandTangent[0], &m_vRandSub[0]);

				i3Vector::Sub( &m_vRandSub[1], &m_vRandDirection[2], &m_vRandDirection[1]);			
				i3Vector::Add( &m_vRandTangent[1], &m_vRandSub[0], &m_vRandSub[1]);
				i3Vector::Normalize( &m_vRandTangent[1], &m_vRandTangent[1]);
			}
			break;
		case 1:
			{
				i3Vector::Sub( &m_vRandSub[0], &m_vRandDirection[1], &m_vRandDirection[0]);	
				i3Vector::Sub( &m_vRandSub[1], &m_vRandDirection[2], &m_vRandDirection[1]);	
				i3Vector::Add( &m_vRandTangent[0], &m_vRandSub[0], &m_vRandSub[1]);
				i3Vector::Normalize( &m_vRandTangent[0], &m_vRandTangent[0]);

				i3Vector::Sub( &m_vRandSub[0], &m_vRandDirection[2], &m_vRandDirection[1]);			
				i3Vector::Sub( &m_vRandSub[1], &m_vRandDirection[3], &m_vRandDirection[2]);			
				i3Vector::Add( &m_vRandTangent[1], &m_vRandSub[0], &m_vRandSub[1]);
				i3Vector::Normalize( &m_vRandTangent[1], &m_vRandTangent[1]);
			}
			break;
		case 2:
			{
				i3Vector::Sub( &m_vRandSub[0], &m_vRandDirection[2], &m_vRandDirection[1]);	
				i3Vector::Sub( &m_vRandSub[1], &m_vRandDirection[3], &m_vRandDirection[2]);	
				i3Vector::Add( &m_vRandTangent[0], &m_vRandSub[0], &m_vRandSub[1]);
				i3Vector::Normalize( &m_vRandTangent[0], &m_vRandTangent[0]);

				i3Vector::Sub( &m_vRandSub[0], &m_vRandDirection[3], &m_vRandDirection[2]);
				i3Vector::Sub( &m_vRandSub[1], &m_vRandDirection[1], &m_vRandDirection[3]);			
				i3Vector::Add( &m_vRandTangent[1], &m_vRandSub[0], &m_vRandSub[1]);
				i3Vector::Normalize( &m_vRandTangent[1], &m_vRandTangent[1]);
			}
			break;
		default:
			{
				i3Vector::Sub( &m_vRandSub[0], &m_vRandDirection[3], &m_vRandDirection[2]);	
				i3Vector::Sub( &m_vRandSub[1], &m_vRandDirection[1], &m_vRandDirection[3]);	
				i3Vector::Add( &m_vRandTangent[0], &m_vRandSub[0], &m_vRandSub[1]);
				i3Vector::Normalize( &m_vRandTangent[0], &m_vRandTangent[0]);

				i3Vector::Sub( &m_vRandSub[0], &m_vRandDirection[3], &m_vRandDirection[2]);
				i3Vector::Sub( &m_vRandSub[1], &m_vRandDirection[1], &m_vRandDirection[0]);			
				i3Vector::Add( &m_vRandTangent[1], &m_vRandSub[0], &m_vRandSub[1]);
				i3Vector::Normalize( &m_vRandTangent[1], &m_vRandTangent[1]);
			}
			break;
		}
	}
	
	switch( m_nFlyingStep)
	{
	case 0:
		D3DXVec2Hermite( &d3dVec2, (const D3DXVECTOR2 *)&m_vRandDirection[0], (const D3DXVECTOR2 *)&m_vRandTangent[0], (const D3DXVECTOR2 *)&m_vRandDirection[1], (const D3DXVECTOR2 *)&m_vRandTangent[1], m_rElapsedFlyingTime);
		break;
	case 1:
		D3DXVec2Hermite( &d3dVec2, (const D3DXVECTOR2 *)&m_vRandDirection[1], (const D3DXVECTOR2 *)&m_vRandTangent[0], (const D3DXVECTOR2 *)&m_vRandDirection[2], (const D3DXVECTOR2 *)&m_vRandTangent[1], m_rElapsedFlyingTime);
		break;
	case 2:
		D3DXVec2Hermite( &d3dVec2, (const D3DXVECTOR2 *)&m_vRandDirection[2], (const D3DXVECTOR2 *)&m_vRandTangent[0], (const D3DXVECTOR2 *)&m_vRandDirection[3], (const D3DXVECTOR2 *)&m_vRandTangent[1], m_rElapsedFlyingTime);
		break;		
	default:
		D3DXVec2Hermite( &d3dVec2, (const D3DXVECTOR2 *)&m_vRandDirection[2], (const D3DXVECTOR2 *)&m_vRandTangent[0], (const D3DXVECTOR2 *)&m_vRandDirection[3], (const D3DXVECTOR2 *)&m_vRandTangent[1], 1.0f);
		break;
	}		

	i3Vector::SetX( &m_ThrowData.m_vStartPos, d3dVec2.x);
	i3Vector::SetY( &m_ThrowData.m_vStartPos, d3dVec2.y);

	// 유도탄 기능
	if( m_pWeaponBase->getOwner() != nullptr)
	{
		CGameMissionManager * pMissionMng = CGameMissionManager::i(); 
		VEC3D dir, axis, * pObjPos;
		MATRIX rotMtx;
		i3Object * pObj;
		CGrenadeGunInfo * pInfo = (CGrenadeGunInfo *) m_pWeaponBase->getWeaponInfo();

		I3ASSERT( i3::same_of<CGrenadeGunInfo*>(pInfo));

		// 목표물을 정한다.
		if( m_pWeaponBase->getOwner()->getCharaTeam() & CHARACTER_TEAM_RED)
			pObj = pMissionMng->getBlueObject();
		else
			pObj = pMissionMng->getRedObject();

		// 대상 Object가 있고, BC.pef에서 Homing Property의 값이 0보다 크다면..
		if(( pObj != nullptr) && (pInfo->getHoming() > 0.0f))
		{
			if( i3::same_of<CGameObjectRespawnHelicopter*>(pObj))
			{
				pObjPos = i3Matrix::GetPos( ((CGameObjectRespawnHelicopter*)pObj)->getRootDummy()->GetCacheMatrix());
			}
			else
			{
				pObjPos = pObj->GetPos();
			}

			// 목표 방향
			i3Vector::Sub( &dir, m_pWeaponBase->GetPos(), pObjPos);
			i3Vector::Normalize( &dir, &dir);

			// 방향 오차가 일정이상 크다면...
			if( i3Math::abs( i3Vector::Dot( &dir, &m_ThrowData.m_vDirection)) < 0.97f)
			{
				// 회전축 계산
				i3Vector::Cross( &axis, &dir, &m_ThrowData.m_vDirection);

				// 회전 Matrix 설정
				i3Matrix::SetRotateAxisDeg( &rotMtx, &axis, pInfo->getHoming());

				// 진행방향(m_vDirection)을 Rotate시킨다.
				i3Vector::TransformNormal( &m_ThrowData.m_vDirection, &m_ThrowData.m_vDirection, &rotMtx);
			}
		}
	}

	i3Vector::Normalize( &m_ThrowData.m_vDirection, &m_ThrowData.m_vDirection);

	i3Vector::Scale( &vNew, &m_ThrowData.m_vDirection, m_ThrowData.m_rSpeed * m_ThrowData.m_rElapsedThrowTime);
	i3Vector::Add( &vNew, &m_ThrowData.m_vStartPos, &vNew);
	
	ColLine.SetStart( &m_ThrowData.m_vOldPos);
	i3Vector::Sub( &vTemp,  &vNew, &m_ThrowData.m_vOldPos);
	rLength = i3Vector::Normalize( &vTemp, &vTemp);

	REAL32 rTheta = i3Math::atan2( getX( &vTemp), getZ( &vTemp));
	REAL32 rRho = i3Math::acos( getY( &vTemp) );
	i3Matrix::SetRotateY( m_pWeaponBase->GetMatrix(), rTheta);
	i3Matrix::PreRotateX( m_pWeaponBase->GetMatrix(), rRho);
	i3Matrix::SetPos( m_pWeaponBase->GetMatrix(), &vNew);

	if( rLength <= 0.0f )
	{	// 이동 속도가 0
		return;
	}

	ColLine.SetEnd( m_pWeaponBase->GetPos());
	ColLine.SetDir( &vTemp);

	// 첫번째 raycast는 Chara들의 Rough Collision을 검사합니다.
	pResult = g_pFramework->raycastClosest( &ColLine, CGRP_MASKGROUP_GRENADE );
	if( pResult != nullptr)
	{
		i3EventReceiver * pRecv = pResult->m_pDestShapeSet->getEventReceiver();

		g_pFramework->reserveRaycastResult( 1);

		if( i3::kind_of<CGameCharaBase*>(pRecv))
		{
			UINT32 nColGroup = 0xffffffff;
			CGameCharaBase * pTarget = (CGameCharaBase *)pRecv;

			if( pTarget == m_pWeaponBase->getOwner() )
			{
				pResult = nullptr;
			}
			else
			{
				if( pTarget->getCharaTeam() & CHARACTER_TEAM_RED)
				{
					nColGroup = CGRP_MASKGROUP_CHARA_RED;
				}
				else {
					nColGroup = CGRP_MASKGROUP_CHARA_BLUE;
				}

				pResult = g_pFramework->raycastClosest( &ColLine, nColGroup );
				if( pResult != nullptr)
				{
					pRecv = pResult->m_pDestShapeSet->getEventReceiver();
					if( i3::kind_of<CGameCharaBase*>(pRecv))
					{
						pTarget = (CGameCharaBase *) pRecv;
						if( pTarget->isCharaStateMask( CHARA_STATE_DEATH))
						{
							pResult = nullptr;
						}
					}
				}
			}
		}
	}

	if( pResult == nullptr)
	{
		// Rough Hit Collision과 Chara Hit결과가 없으면 World와 Object에 대하여 테스트를 합니다.
		pResult = g_pFramework->raycastClosest( &ColLine, CGRP_MASKGROUP_ATTACK );
		if( pResult == nullptr)
			return;

		g_pFramework->reserveRaycastResult( 1);
	}

	{
		VEC3D	vecNew;
		//이동 방향 벡터 와 이동거리
		i3Vector::Sub( &vecNew, &pResult->m_Intersect, &m_ThrowData.m_vOldPos);
		REAL32 rTestLen = i3Vector::Normalize( &vecNew, &vecNew);

		if( rLength + 0.05f < rTestLen )
		{	
			//유효한 거리내의 충돌이 없심..
			return;
		}

		VEC3D vecRevert;
		i3Vector::Scale( &vecRevert, &vecNew, -0.1f);
		i3Vector::Add( m_pWeaponBase->GetPos(), &pResult->m_Intersect, &vecRevert);

		m_bExplosion = true;
	}

	g_pFramework->resetRaycastResult();
}


bool MainWeapon_RLShell::isBeing(VEC2D* pVec)
{
	bool bRet = false;
	INT32 i = 0;

	for( i=0; i<4; i++)
	{
		if( (i3Vector::GetX( &m_vRandDirection[i]) == i3Vector::GetX( pVec)) &&	(i3Vector::GetY( &m_vRandDirection[i]) == i3Vector::GetY( pVec)) )
		{
			bRet = true;
		}
	}

	return bRet;
}

void MainWeapon_RLShell::PlayGrazeSound(void)
{
	g_pSndMng->StartWeaponStateSnd( GTBWS_ON_GRAZE_RPG7, 0, m_pWeaponBase->GetPos(), false);
	m_bGrazeSoundPlayed = true;
}

void MainWeapon_RLShell::OnUpdate_RLShell( REAL32 rDeltaSeconds)
{
	if( isLaunching())
	{
		// 유저가 나간경우
		bool bOwnerInvalid = true;
		for( INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)
		{
			if (g_pCharaManager->getChara( i) == nullptr)
				continue;

			if( g_pCharaManager->getChara( i) == m_pWeaponBase->getOwner() )
			{
				bOwnerInvalid = false;
				break;
			}
		}
		if( m_pWeaponBase->getOwner() == nullptr || bOwnerInvalid )
		{
			g_pWeaponManager->PushRocketLauncherWeapon( static_cast<WeaponRocketLauncherShell*>(m_pWeaponBase) );
			return;
		}

		m_rElapsedExplosionTime += rDeltaSeconds;

		UpdateFlying( rDeltaSeconds );

		m_pWeaponBase->_TransformWorldSpace( rDeltaSeconds);

		i3Vector::Copy( &m_ThrowData.m_vOldPos, m_pWeaponBase->GetPos());

		
		if( m_pWeaponBase->IsDedicatedHost() && m_pWeaponBase->getNetworkIdx() != -1)
		{
			GAMETOCONTEXT_INFO_WEAPON_THROW throwInfo(m_pWeaponBase->getWeaponInfo()->GetItemID(), &m_ThrowData.m_vOldPos, NET_GRENADE_STATE_FLY);

			if( m_bExplosion )
				throwInfo._nAddStateFlag	= NET_GRENADE_STATE_EXPLOSION;

			GAMEEVENT::Write_ThrowingWeapon( m_pWeaponBase->getNetworkIdx(), &throwInfo );
		}

		if( m_bExplosion)
			this->Explosion();
		
		if( m_ThrowData.m_rElapsedThrowTime >= NET_ROCKETLAUNCHER_OFFTIME)
		{// 15초까지 충돌이 없거나 패킷을 못 받으면 그냥 없애줍니다.
			m_bExplosion = true;
		}
	}
	
}

void	MainWeapon_RLShell::NET_Bound_RLShell( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType )
{
	

	if( m_pWeaponBase->IsDedicatedHost() && m_pWeaponBase->getNetworkIdx() != -1)
	{
		// 네트워크로 보내줍니다.
		GAMETOCONTEXT_INFO_WEAPON_THROW throwInfo(m_pWeaponBase->getWeaponInfo()->GetItemID(), pStartPos, NET_GRENADE_STATE_BOUND);

		throwInfo._nBoundType		= nBoundType;

		GAMEEVENT::Write_ThrowingWeapon( m_pWeaponBase->getNetworkIdx(), &throwInfo);
	}

	if( fSpeed > 0.5f)
	{
		_PlayBoundSound( (I3_TERRAIN_TYPE) nBoundType );
	}

	Throw( pStartPos, pDir, fSpeed , false );
	
}


void MainWeapon_RLShell::UpdateNetworkData( REAL32 rDeltatime)
{
	REAL32	flerp;
	VEC3D	vTemp, vDir;

	I3ASSERT( BattleServerContext::i()->IsP2PHost() == false);

	if( m_pWeaponBase->getNetworkIdx() < 0 ||  m_pWeaponBase->getNetworkIdx() >= MAX_THROWWEAPON_COUNT)	return;

	GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( m_pWeaponBase->getNetworkIdx() );
	GAMEINFO_THROW_WEAPON_ASSIST * pAssistInfo = &pInfo->_tAssistData;
	GAMEINFO_THROW_WEAPON_NETDATA * pNetInfo = &pInfo->_tNetData;

	VEC3D vNetPos = pNetInfo->GetPos();

	flerp = pAssistInfo->_fInterpolateTime += rDeltatime;

	if( pInfo->_bRecv )
	{
		i3Vector::Sub( &pAssistInfo->_vDir, &vNetPos, &pAssistInfo->_vPostPos);

		if( i3Vector::Length( &pAssistInfo->_vDir) > I3_EPS)
		{	
			i3Vector::Normalize( &vDir, &pAssistInfo->_vDir);
			i3Matrix::MakeWithAt( m_pWeaponBase->GetMatrix(), &vDir, &I3_YAXIS);
			i3Matrix::PreRotateX( m_pWeaponBase->GetMatrix(), I3_PI2);
		}
		else
		{
			if( m_pWeaponBase->getOwner() != nullptr)
			{
				m_pWeaponBase->SetMatrix( m_pWeaponBase->getOwner()->GetMatrix());
				i3Matrix::PreRotateX( m_pWeaponBase->GetMatrix(), I3_PI2);
			}
		}
	}

	if( flerp > GRENADESHELL_POS_ASSISTTIME )
	{	
		if( (pNetInfo->GetState() & 0x0F) == NET_GRENADE_STATE_FLY )
		{
			i3Vector::Copy( &pAssistInfo->_vPostPos, m_pWeaponBase->GetPos());
			i3Vector::Add( &vTemp, &pAssistInfo->_vPostPos, &pAssistInfo->_vDir );
			pNetInfo->SetPos( vTemp );
			vNetPos = vTemp;
			
			flerp = rDeltatime;
		}
		else
			flerp = GRENADESHELL_POS_ASSISTTIME;
	}

	flerp *= GRENADESHELL_POS_ASSISTTIMEINV;

	i3Vector::Lerp( m_pWeaponBase->GetPos(), &pAssistInfo->_vPostPos, &vNetPos, flerp );
	
	if( pInfo->_bRecv )
	{
		pInfo->_bRecv = false;

		if( pNetInfo->GetState() & NET_GRENADE_STATE_FLY)
			m_pWeaponBase->SetVisible( true );

		if( pNetInfo->GetState() & NET_GRENADE_STATE_EXPLOSION )
		{
			m_pWeaponBase->SetPos( &vNetPos);
			Explosion();
		}
	}
}

void MainWeapon_RLShell::Explosion()
{
	//BOOM
	m_pWeaponBase->_OnCheckCollisionByExplosion();

	{
		// 폭발 이팩트
		g_pEffectManager->AddEffect( m_pWeaponBase->getFireEffectIndex(), m_pWeaponBase->GetPos(), &I3_YAXIS);
		//I3TRACE("WeaponGrenadeShell::Explosion \n "); 

		g_pWeaponManager->PushRocketLauncherWeapon( static_cast<WeaponRocketLauncherShell*>(m_pWeaponBase) );
	}
}



void MainWeapon_RLShell::_PlayBoundSound(I3_TERRAIN_TYPE nTerrainType)
{
	I3ASSERT( g_pViewer != nullptr);
	g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, nTerrainType, GTBT_BOUND_THROW, m_pWeaponBase->GetPos(), false);
}
