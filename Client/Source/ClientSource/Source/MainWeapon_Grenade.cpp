#include "pch.h"
#include "MainWeapon_Grenade.h"

#include "BattleObjContext.h"

#include "BattleSlotContext.h"

#include "MyRoomInfoContext.h"
#include "GameCharaCameraContext.h"
#include "BattleServerContext.h"
#include "GameContextUtil.h"

#include "HackContext.h"
#include "GameMode.h"
#include "TutorialMode.h"

#define GRENADE_POS_ASSISTTIME		0.2f
#define GRENADE_POS_ASSISTTIMEINV	5.0f

MainWeapon_Grenade::MainWeapon_Grenade(WeaponBase* p) : MainWeapon_Throw(p)
									, m_rElapsedExplosionTime(0.0f)
									, m_uCount(0)
									, m_bCheckFloor(false)
									, m_rBoundObjectLen(0.0f)
									, m_nFireEffectCount(0)
									, m_nStateFlag(-1)
									, m_bResetBullet(false)
{
	m_pAttachObjectTransform = nullptr;
	m_pAttachObject = nullptr;
}

void MainWeapon_Grenade::OnInitVariable()
{
	MainWeapon_Throw::OnInitVariable();

	m_rElapsedExplosionTime = 0.0f;

	i3Vector::Zero( &m_vObjectForce);

	m_bCheckFloor	= false;
	m_pAttachObjectTransform = nullptr;

	m_rBoundObjectLen = 0.f;

	m_uCount = (i3Math::Rand() & 0x0000FFFF);
	m_uCount |= (g_pRand->GetRand( m_uCount) & 0x00000FFF) << 20;

	m_pAttachObject	= nullptr;
}

void	MainWeapon_Grenade::OnCreate( bool bCreateInWeapon)
{
	m_nFireEffectCount = 0; 

	for(INT32 i=0; i<MAX_FIRE_EFFECT; i++)
	{
		if( m_pWeaponBase->getFireEffectIndex(i) != -1)
			m_nFireEffectCount++;
	}
	
}

void	MainWeapon_Grenade::Reset()
{
	MainWeapon_Throw::Reset();

	m_pWeaponBase->setWeaponStateMask( WSM_LOADBULLET_DISABLE | WSM_LOADMAG_DISABLE);
//	m_StateMaskWeapon		= WSM_LOADBULLET_DISABLE | WSM_LOADMAG_DISABLE;

	m_rElapsedExplosionTime = 0.0f;

	m_bResetBullet = true;
	m_bCheckFloor			= false;
	m_pAttachObjectTransform = nullptr;

	i3Vector::Zero( &m_vObjectForce);

	m_rBoundObjectLen = 0.f;
	m_pAttachObject = nullptr;

	m_uCount = (i3Math::Rand() & 0x0000FFFF);
	m_uCount |= (g_pRand->GetRand( m_uCount) & 0x00000FFF) << 20;

	m_pWeaponBase->SetEnable( true);

	m_nStateFlag = -1;
	m_vGrenadeStateFlag.clear();

	if (  m_pWeaponBase->getNetworkIdx() != -1)
	{
		GAMEINFO_THROW_WEAPON * pGrenadeInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( m_pWeaponBase->getNetworkIdx());
		pGrenadeInfo->_tNetData.resetPrevPos();
	}	
}

/*virtual*/ void MainWeapon_Grenade::OnEndFire(i3AIContext * pCtx, REAL32 tm)
{
	// 여기로 옮깁니다.
	if(m_bResetBullet == false)
		CheckBullet();

	MainWeapon_Throw::OnEndFire(pCtx, tm);
}

/*virtual*/ void MainWeapon_Grenade::ChainExplosion(void)
{
	Explosion();
}

namespace
{
	void GrenadeHackDetection(UINT32 curCnt)
	{
		UINT32 ra = i3Math::Rand();
		curCnt = ((((curCnt & 0x000F0000)>>16) + 1) <<16) | (ra & 0x0000FFFF) | (g_pRand->GetRand(ra&0x0000FFFF)<<20);
		if( ((curCnt & 0x000F0000) >> 16) > (UINT32) LocaleValue::GetInt32("FireWall/GrenadeCount") ||
			((curCnt & 0xFFF00000)>>20) != (UINT32) (g_pRand->GetRand(curCnt&0x0000FFFF) & 0x00000FFF) )
		{
			HackContext::i()->HackingDetected( CC_MANY_GRENADE_COUNT * 1.f ,C_MANY_GRENADE_COUNT);
		}
	}
}

	
void	MainWeapon_Grenade::_ProcessTakeObject() // 일단 public으로..(옮겨야함)
{
	// 멈추면 바닥을 한번 체크합니다. Object 위에 있는 경우 오브젝트 matrix로 세팅할 필요가 있습니다.
	if(!m_bCheckFloor)
		_TakeObject();
	else
		_UpdateTakeObjectTransform();
}


REAL32 MainWeapon_Grenade::_BoundObject( I3_PHYSIX_HIT_RESULT * pHitResult, VEC3D * pDir)
{
	I3ASSERT( pHitResult != nullptr);
	I3ASSERT( pHitResult->m_pDestShapeSet != nullptr);
	
	if( !i3::kind_of<i3Object*>(pHitResult->m_pDestShapeSet->getEventReceiver()))
		return 0.f;

	i3Object * pObj = (i3Object*) pHitResult->m_pDestShapeSet->getEventReceiver();
	i3Transform * pTrans = GlobalFunc::FindTransformFromPhysXSceneObj( pObj->getSceneObject() );

	if( pTrans == nullptr )
		return 0.f;

	if( pTrans->IsAnimated() )
	{
		// 애니메이션하는 오브젝트
		MATRIX	matTop;
		
		i3SceneTracer * pTracer;

		pTracer = g_pFramework->GetViewer()->GetSceneTracer();

		I3ASSERT( pTracer != nullptr);

		pTracer->GetModelViewMatrixStack()->Reset();
		pTracer->SetTime( g_pViewer->getDeltaTime());
		pTracer->SetViewMatrix( g_pCamera->getCamMatrix(), g_pCamera->getCamMatrix());
		
		pTrans->CalcUpdatedMatrix( &matTop, pTracer);

		VEC3D vNewPosDir;
		i3Vector::Sub( &vNewPosDir, i3Matrix::GetPos( &matTop ), i3Matrix::GetPos( pTrans->GetCacheMatrix() ) );

		if( i3Vector::Dot( &vNewPosDir, &pHitResult->m_Normal ) > 0)
		{
			i3Vector::Scale( &m_vObjectForce, &vNewPosDir, 1.3f );

			if( pDir != nullptr)
			{
				VEC3D vDir;
				i3Vector::Normalize( &vDir, pDir);
				REAL32 rDot = i3Vector::Dot( &pHitResult->m_Normal, &vDir) * I3_PI;
				REAL32 angle = i3Math::sin( I3_PI2 - rDot);

				if( angle > I3_EPS)
				{
					REAL32 rLen = i3Vector::Length( &vNewPosDir);
					REAL32 rBoundVal = rLen / angle;
					if( rBoundVal > rLen)
						rBoundVal = rLen;

					return rBoundVal;
				}
			}
		}
	}

	return 0.f;
}

void MainWeapon_Grenade::_ProcessBoundObjectForce()
{
	i3CollideeLine line;
	VEC3D vStartPos, vEndPos;

	if( getX( &m_vObjectForce) != 0.f || getY( &m_vObjectForce) != 0.f || getZ( &m_vObjectForce) != 0.f)
	{
		// 한번더 반대편으로 컬리젼 오브젝트가 있다면 밀어낸다.
		i3Vector::Scale( &vStartPos, &m_vObjectForce, 2.f );
		i3Vector::Add( &vStartPos, &vStartPos, &m_ThrowData.m_vOldPos );

		line.SetStart( &vStartPos);

		i3Vector::Scale( &vEndPos, &m_vObjectForce, -2.f);
		i3Vector::Add( &vEndPos, &m_ThrowData.m_vOldPos, &vEndPos);

		line.SetEnd( &vEndPos);
		
		I3_PHYSIX_HIT_RESULT * pHitResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK);

		if( pHitResult != nullptr )
		{
			I3ASSERT( pHitResult->m_pDestShapeSet != nullptr);
			if( i3::kind_of<i3Object*>(pHitResult->m_pDestShapeSet->getEventReceiver()))
			{
				_BoundObject( pHitResult, nullptr );
				
				i3Vector::Add( m_pWeaponBase->GetPos(), m_pWeaponBase->GetPos(), &m_vObjectForce);
				i3Vector::Add( &m_ThrowData.m_vOldPos, &m_ThrowData.m_vOldPos, &m_vObjectForce);
				i3Vector::Add( &m_ThrowData.m_vStartPos, &m_ThrowData.m_vStartPos, &m_vObjectForce);

				return;
			}
		}

		i3Vector::Zero( &m_vObjectForce);
	}
}

void MainWeapon_Grenade::CheckBullet()
{
	INT32 total_count = m_pWeaponBase->getTotalBulletCount();
	INT32 load_count = m_pWeaponBase->getLoadedBulletCount();

	if (load_count > 0)
	{
		load_count -= m_pWeaponBase->getWeaponInfo()->GetFireDecBulletCount();
	}
	else if (total_count > 0)
	{
		m_pWeaponBase->setLoadedBulletCount(m_pWeaponBase->getWeaponInfo()->GetIncreaseCount());
		m_pWeaponBase->setTotalBulletCount(MIN(0, total_count - m_pWeaponBase->getWeaponInfo()->GetFireDecBulletCount()));
	}
}

void MainWeapon_Grenade::UpdateFlying( REAL32 rDeltaSeconds, REAL32 rExplosionRange)
{
	if( m_ThrowData.m_rSpeed < 0.15f)
	{
		_ProcessTakeObject();
		return;
	}

	if( rDeltaSeconds > 0.5f)
		rDeltaSeconds = 0.016f;

	if( m_pWeaponBase->IsDedicatedHost() )
	{
		this->CheckWorldCollision( rDeltaSeconds);
	}
	else
	{	// 네트워크 처리입니다.
		this->UpdateNetworkData( rDeltaSeconds);
	}

#if !defined( NO_GRENADE_TRACE_EFFECT)
	//	수류탄 궤적을 사용하는 경우에 수류탄 궤적(꼬리)을 만든다.
	if( isUseTraceEffect() && m_pWeaponBase->isVisible() &&
		false == i3Vector::IsSame( m_pWeaponBase->GetPos(), &m_ThrowData.m_vTracerOldPos ) )
	{
		if( m_rElapsedExplosionTime < rExplosionRange - 0.1f && //	폭발하기 직전에 궤적은 불필요하므로 끊는다.
			m_ThrowData.m_rElapsedThrowTime > 0.1f &&
			m_ThrowData.m_rTracerUpdateTime <= 0.f )
		{
			UpdateTraceEffect(m_ThrowData.m_rElapsedThrowTime);
		}
	}

	m_ThrowData.m_rTracerUpdateTime -= rDeltaSeconds;

#endif
	
	m_ThrowData.m_rElapsedThrowTime += rDeltaSeconds;	
}


void	MainWeapon_Grenade::CheckWorldCollision( REAL32 rDeltaSeconds)
{
	VEC3D	vTemp;
	REAL32	rLength;
	i3CollideeLine ColLine;
	I3_PHYSIX_HIT_RESULT * pResult;

	INT32	nBoundType = 0;

	VEC3D	vNew;
	i3Vector::Scale( &vNew, &m_ThrowData.m_vDirection , m_ThrowData.m_rSpeed * m_ThrowData.m_rElapsedThrowTime);
	i3Vector::Add( m_pWeaponBase->GetPos(), &m_ThrowData.m_vStartPos , &vNew);
	addY( m_pWeaponBase->GetPos(), - 5.0f * m_ThrowData.m_rElapsedThrowTime  * m_ThrowData.m_rElapsedThrowTime );
	i3Matrix::PreRotateX( m_pWeaponBase->GetMatrix(), - I3_DEG2RAD( rDeltaSeconds * 360.f));

	ColLine.SetStart(&m_ThrowData.m_vOldPos );
	i3Vector::Sub( &vTemp,  m_pWeaponBase->GetPos(),&m_ThrowData.m_vOldPos );
	rLength = i3Vector::Normalize( &vTemp, &vTemp);

	if( rLength <= 0.0f )
	{
		_ProcessBoundObjectForce();
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
			UINT32 nColGroup = CGRP_MASKGROUP_CHARA_BLUE;
			CGameCharaBase * pTarget = (CGameCharaBase *)pRecv;

			if( pTarget == m_pWeaponBase->getOwner() ) //m_pOwner )
			{
				pResult = nullptr;
			}
			else
			{
				if( pTarget->getCharaTeam() & CHARACTER_TEAM_RED)
				{
					nColGroup = CGRP_MASKGROUP_CHARA_RED;
				}

				pResult = g_pFramework->raycastClosest( &ColLine, nColGroup );
				if( pResult != nullptr)
				{
					pRecv = pResult->m_pDestShapeSet->getEventReceiver();
					if( i3::kind_of<CGameCharaBase*>(pRecv))
					{
						pTarget = (CGameCharaBase *) pRecv;
						if( pTarget->isCharaStateMask( CHARA_STATE_DEATH))
							pResult = nullptr;
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
		VEC3D	vecNew, vecNewPos, vecNormal;
		REAL32	rDot1 = 0.0f;

		//이동 방향 벡터 와 이동거리
		i3Vector::Sub( &vecNew, &pResult->m_Intersect, &m_ThrowData.m_vOldPos );
		REAL32 rTestLen = i3Vector::Normalize( &vecNew, &vecNew);

		// 움직이는 오브젝트라면 힘을 계산해서 반사량을 더해줘야합니다. by dfly79 080117
		m_rBoundObjectLen = _BoundObject( pResult, ColLine.GetDir());

		if( rLength + 0.05f < rTestLen + m_rBoundObjectLen) //rTestLen * rTestLen)
		{	
			//유효한 거리내의 충돌이 없심..
			_ProcessBoundObjectForce();
			return;
		}

		// 들어간 만큼 뒤로 무른다.
		// Step1. 수류탄의 위치를 충돌이 일어난 위치로 이동( 수류탄의 Radius를 포함)
		VEC3D vecRevert;
		i3Vector::Scale( &vecRevert, &vecNew, - 0.05f);

		if( getX( &m_vObjectForce) != 0.f || getY( &m_vObjectForce) != 0.f || getZ( &m_vObjectForce) != 0.f)
		{
			VEC3D vAddForce;
			i3Vector::Scale( &vAddForce, &m_vObjectForce, 3.f);
			i3Vector::Add( &vecRevert, &vecRevert, &vAddForce);
		}

		VEC3D vGrenadeSize;
		i3Vector::Scale( &vGrenadeSize, &pResult->m_Normal, 0.05f);
		
		i3Vector::Add( &vecRevert, &vecRevert, &vGrenadeSize);
		i3Vector::Add( m_pWeaponBase->GetPos(), &pResult->m_Intersect, &vecRevert);	//	들어온 반대 방향으로 무른다.
		i3Vector::Add( &m_ThrowData.m_vOldPos , &m_ThrowData.m_vOldPos, &vecRevert);

		i3Vector::Add( &vecNewPos, &pResult->m_Intersect, &vecNew);	//NewPosition
		rDot1 = i3Vector::Dot( &vecNew, &pResult->m_Normal);

		i3Vector::Scale( &vecNormal, &pResult->m_Normal, - 2.0f * rDot1);
		i3Vector::Add( &vecNewPos, &vecNewPos, &vecNormal);

		i3Vector::Sub( &vecNew, &vecNewPos, &pResult->m_Intersect);
		i3Vector::Normalize( &vecNew, &vecNew);

		//반사 속도 계산
		REAL32	Speed = m_ThrowData.m_rSpeed;

		rDot1 = i3Vector::Dot( &vecNew, &m_ThrowData.m_vDirection ); 
		if( i3Vector::Dot( &I3_YAXIS, &pResult->m_Normal) > 0.5f)
		{
			Speed = ((rDot1 + 1.0f) * Speed * 0.1f) + (Speed * 0.165f);
		}
		else
		{
			Speed = ((rDot1 + 1.0f) * Speed * 0.11f) + (Speed * 0.165f);
		}

		if( pResult->m_pTerrain != nullptr )
			nBoundType = pResult->m_pTerrain->m_Terrain;

		//재 투척 (네트워크 처리됩니다. by 정순구 2006. 10. 16 )
		NET_Bound( m_pWeaponBase->GetPos(), &vecNew, Speed, nBoundType );
	}

	g_pFramework->resetRaycastResult();	
	
	
}



void MainWeapon_Grenade::Explosion()
{
	//BOOM
	m_pWeaponBase->_OnCheckCollisionByExplosion();
	
	{
		i3Object * pObj = nullptr;
		i3Transform * pTrans = nullptr;
		I3_PHYSIX_HIT_RESULT * pResult;
		i3CollideeLine line;
		VEC3D vDir;

		i3Vector::Set( &vDir, 0.f, -1.f, 0.f);
		line.SetStart( m_pWeaponBase->GetPos());
		line.SetDir( &vDir);
		pResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK);
		if( pResult != nullptr)
		{
			UINT16 shapeGroup = pResult->m_pDestShapeSet->getShapeGroup();
				
			if( OBJECT_COLLISION_BY_WEAPON_HIT( shapeGroup ) )
			{
				i3EventReceiver * pReceiver = pResult->m_pDestShapeSet->getEventReceiver();
			
				I3ASSERT( i3::kind_of<i3Object*>(pReceiver));

				pObj = (i3Object*) pReceiver;
				pTrans = GlobalFunc::FindTransformFromPhysXSceneObj( pObj->getSceneObject() );
			}
		}

		INT32 nFireIndex = 0;	//	default
		if( m_nFireEffectCount > 1)
		{
			// 여러개 이텍트가 있는 경우 랜덤으로 선택 플레이한다.
			nFireIndex = (i3Math::Rand() % 3);
			if( nFireIndex < 0)						nFireIndex = 0;
			if( nFireIndex >= m_nFireEffectCount)	nFireIndex = m_nFireEffectCount-1;
		}

		// 폭발 이팩트
		if( pTrans != nullptr)
			g_pEffectManager->AddEffect( m_pWeaponBase->getFireEffectIndex( nFireIndex), m_pWeaponBase->GetPos(), &I3_YAXIS, pTrans->GetCacheMatrix(), 0.f, pObj);
		else
			g_pEffectManager->AddEffect( m_pWeaponBase->getFireEffectIndex( nFireIndex), m_pWeaponBase->GetPos(), &I3_YAXIS);
		//I3TRACE("WeaponGrenade::Explosion \n "); 

		g_pWeaponManager->PushThrowWeapon(m_pWeaponBase);
	}

	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
	{
		if( (g_pFramework->IsBattleStage() != false) && (gmode::i()->GetTutorialMode() != nullptr) )
		{
			VEC3D v3Pos;

			i3Vector::Set(&v3Pos,	m_ThrowData.m_vOldPos.x, 
									m_ThrowData.m_vOldPos.y,
									m_ThrowData.m_vOldPos.z);
							
			gmode::i()->GetTutorialMode()->CheckGrenadeInTutorial(v3Pos, m_pWeaponBase->getWeaponInfo()->GetNumber());
		}
	}

}

bool MainWeapon_Grenade::SuccessiveStateFlagCheck(INT32 nState)
{
	if (false == m_vGrenadeStateFlag.empty())
	{
		UINT32 nSize = m_vGrenadeStateFlag.size();

		//폭팔, 정지 상태 이후엔 이하의 상황이 들어올수 없다.
		if (nState == NET_GRENADE_STATE_FLY || nState == NET_GRENADE_STATE_BOUND)
		{
			//바로앞 상태를 찾는다.
			UINT8 usPrevState = (UINT8)m_vGrenadeStateFlag[nSize - 1];

			switch( usPrevState )
			{
			case NET_GRENADE_STATE_EXPLOSION	:
			case NET_GRENADE_STATE_SETTLED		:
			case NET_GRENADE_STATE_CLICKFIRE	:
				{
					//패킷을 보내지 않는다.
					return false;
				}
			}
		}
	}

	m_vGrenadeStateFlag.push_back(nState);
	return true;
}

void	MainWeapon_Grenade::OnUpdate_Grenade( REAL32 rDeltaSeconds)
{
	if( isThrowing() )
	{	// 월드에 던져진 수류탄입니다.
		// 유저가 있는지 체크해서 없으면 반환시켜줍니다.
		if( _CheckProcessUserLeave() == false)
			return;

		m_nStateFlag = NET_GRENADE_STATE_FLY;

		bool	bHost = m_pWeaponBase->IsDedicatedHost();

		if( bHost )
		{
			if( m_ThrowData.m_rSpeed < 0.2f && !isBombState( WEAPON_BOMBSTATE_SETTLED) )
			{
				addBombState( WEAPON_BOMBSTATE_SETTLED);
				m_nStateFlag = NET_GRENADE_STATE_SETTLED;
			}
		}

		if( isSettled())
			m_rElapsedExplosionTime += rDeltaSeconds;

		REAL32 rExplosionRange = m_pWeaponBase->GetExplosionTime();

		this->UpdateFlying( rDeltaSeconds, rExplosionRange );

		m_pWeaponBase->_TransformWorldSpace( rDeltaSeconds);

		i3Vector::Copy(&m_ThrowData.m_vOldPos , m_pWeaponBase->GetPos());

		if( m_pWeaponBase->getNetworkIdx() != -1)
		{
			if( m_rElapsedExplosionTime > rExplosionRange)
			{
				m_nStateFlag = NET_GRENADE_STATE_EXPLOSION;
				m_pWeaponBase->setSkillOnce_Jump(false);
				m_pWeaponBase->setSkillOnce_Sit(false);
			}

			if( m_ThrowData.m_rElapsedThrowTime > NET_GRENADE_OFFTIME)
			{// 네트워로 받는 경우 15초까지 패킷을 못 받으면 그냥 없애줍니다.
				if( bHost )
				{
					m_nStateFlag = NET_GRENADE_STATE_EXPLOSION;
				}
				else
				{
					GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( m_pWeaponBase->getNetworkIdx());
					pInfo->Reset();

					m_pWeaponBase->_ResetGrenadeInfo();
					g_pWeaponManager->PushThrowWeapon(m_pWeaponBase); 
				}
			}

			if( bHost )
			{
				// 네트워크로 보내줍니다.
				I3ASSERT( m_nStateFlag < 256);
				GAMETOCONTEXT_INFO_WEAPON_THROW throwInfo(m_pWeaponBase->getWeaponInfo()->GetItemID(), &m_ThrowData.m_vOldPos, (UINT8)m_nStateFlag);
				
				if (SuccessiveStateFlagCheck(m_nStateFlag))
				{
					GAMEEVENT::Write_ThrowingWeapon( m_pWeaponBase->getNetworkIdx(), &throwInfo );
				}
			}

			if( m_nStateFlag == NET_GRENADE_STATE_EXPLOSION  && !isBombState( WEAPON_BOMBSTATE_EXPLOSION) )
				this->Explosion();
		}
	}

}

void	MainWeapon_Grenade::SetReceiveStates(GAMEINFO_THROW_WEAPON * pGrenadeInfo, GAMEINFO_THROW_WEAPON_NETDATA * pNetInfo)
{
	
	switch(pNetInfo->GetState())
	{
	case NET_GRENADE_STATE_FLY:
		{
			m_nStateFlag = NET_GRENADE_STATE_FLY;
			m_pWeaponBase->SetVisible( true );
		}
		break;
	case NET_GRENADE_STATE_BOUND:
		{
			if( pNetInfo->GetBoundType() < I3_TERRAIN_TYPE_COUNT )
			{
				_PlayBoundSound( (I3_TERRAIN_TYPE) pNetInfo->GetBoundType() );
			}
			// 바운드 후 플라잉 모드로 바꿔줘야 합니다.
			pNetInfo->SetState( NET_GRENADE_STATE_FLY );
		}
		break;
	case NET_GRENADE_STATE_C5_SETTLED:
		break;
	case NET_GRENADE_STATE_SLEEP:	
		break;
	case NET_GRENADE_STATE_SETTLED:
		{
			addBombState( WEAPON_BOMBSTATE_SETTLED);

			// 클라이언트도 오브젝트 위에 있는 경우를 계산합니다.
			_ProcessTakeObject();
		}
		break;
	case NET_GRENADE_STATE_TAKEOBJECT:
		break;
	case NET_GRENADE_STATE_CLICKFIRE:
		break;
	case NET_GRENADE_STATE_EXPLOSION:
		{
			VEC3D vPos = pNetInfo->GetPos();
			m_pWeaponBase->SetPos( &vPos);
			if( isBombState( WEAPON_BOMBSTATE_EXPLOSION) == false)
				Explosion();
		}
		break;
	}

	pGrenadeInfo->_bRecv = false;
}

void	MainWeapon_Grenade::NET_Bound( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType )
{
	bool bHost = m_pWeaponBase->IsDedicatedHost();

	if( bHost && m_pWeaponBase->getNetworkIdx() != -1)
	{
		// 네트워크로 보내줍니다.
		GAMETOCONTEXT_INFO_WEAPON_THROW throwInfo(m_pWeaponBase->getWeaponInfo()->GetItemID(), pStartPos, NET_GRENADE_STATE_BOUND);

		throwInfo._nBoundType			= nBoundType;

		if (SuccessiveStateFlagCheck(throwInfo._nAddStateFlag))
			GAMEEVENT::Write_ThrowingWeapon( m_pWeaponBase->getNetworkIdx(), &throwInfo);
	}

	if( fSpeed > 0.5f)	//	약한 바운드는 궤적이 안나온다.
	{
#if !defined( NO_GRENADE_TRACE_EFFECT)

		m_ThrowData.m_rTracerUpdateTime = 0.0f;
#endif
		_PlayBoundSound( (I3_TERRAIN_TYPE) nBoundType );
	}
	else
	{
#if !defined( NO_GRENADE_TRACE_EFFECT)
		m_ThrowData.m_rTracerUpdateTime = 0.2f;
#endif
	}

	Throw( pStartPos, pDir, fSpeed , false, -1 );
	
	
}


void MainWeapon_Grenade::UpdateNetworkData( REAL32 rDeltatime)
{
	REAL32	flerp;
	VEC3D	vTemp;

	REAL32	rRotate = 360.0f;

	I3ASSERT( m_pWeaponBase->IsDedicatedHost() == false);

	if( m_pWeaponBase->getNetworkIdx() < 0 || m_pWeaponBase->getNetworkIdx() >= MAX_THROWWEAPON_COUNT)		return;

	GAMEINFO_THROW_WEAPON * pGrenadeInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( m_pWeaponBase->getNetworkIdx());
	GAMEINFO_THROW_WEAPON_ASSIST * pAssistInfo = &pGrenadeInfo->_tAssistData;
	GAMEINFO_THROW_WEAPON_NETDATA * pNetInfo = &pGrenadeInfo->_tNetData;

	VEC3D vNetPos = pNetInfo->GetPos();

	pAssistInfo->_fInterpolateTime += rDeltatime;
	flerp	= pAssistInfo->_fInterpolateTime;

	if( flerp > GRENADE_POS_ASSISTTIME )
	{	
		if( (pNetInfo->GetState() & 0x0F) == NET_GRENADE_STATE_FLY )
		{
			VEC3D vDir;

			i3Vector::Sub( &vDir, &vNetPos, &pAssistInfo->_vPostPos);
			i3Vector::Copy( &pAssistInfo->_vPostPos, m_pWeaponBase->GetPos() );
			
			i3Vector::Add( &vTemp, &pAssistInfo->_vPostPos, &vDir );
			pNetInfo->SetPos( vTemp );
			vNetPos = vTemp;
			
			flerp = rDeltatime;
		}
		else
			flerp = GRENADE_POS_ASSISTTIME;
	}

	flerp *= GRENADE_POS_ASSISTTIMEINV;

	i3Vector::Lerp( m_pWeaponBase->GetPos(), &pAssistInfo->_vPostPos, &vNetPos, flerp );
	i3Vector::Sub( &vTemp, &pAssistInfo->_vPostPos, &vNetPos );

	if (i3Vector::LengthSq(&vTemp) > 0.0001f)
	{
		if (flerp >= 1.0f)
		{
			//pAssistInfo에 현재 포지션을 넣어, 갱신되지 않은 값으로 얻은 vTemp으로 인해 계속 회전하는것을 방지
			i3Vector::Copy(&pAssistInfo->_vPostPos, m_pWeaponBase->GetPos());
		}

		i3Matrix::PreRotateX(m_pWeaponBase->GetMatrix(), -I3_DEG2RAD(rDeltatime * rRotate));
	}
	
	if( pGrenadeInfo->_bRecv )
	{
		SetReceiveStates(pGrenadeInfo, pNetInfo);		// 이 함수의 유일한 호출위치임..
	}
}


bool MainWeapon_Grenade::_CheckProcessUserLeave( void)
{	
	bool	bOwnerInvalid = true;

	// 유저가 나간경우 체크	
	for( INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)
	{
		CGameCharaBase * pChara = g_pCharaManager->getChara( i);

		if( pChara == nullptr)
			continue;
		if( g_pCharaManager->getChara( i) == m_pWeaponBase->getOwner() )
		{
			bOwnerInvalid = false;
			break;
		}
	}

	if( bOwnerInvalid )
	{
		if( m_pWeaponBase->getNetworkIdx() != -1)
		{
			GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( m_pWeaponBase->getNetworkIdx() );
			pInfo->Reset();
		}

		g_pWeaponManager->PushThrowWeapon( m_pWeaponBase);
		return false;
	}

	return true;
}

void MainWeapon_Grenade::_PlayBoundSound(I3_TERRAIN_TYPE nTerrainType)
{
	I3ASSERT( g_pViewer != nullptr);
	g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, nTerrainType, GTBT_BOUND_THROW, m_pWeaponBase->GetPos(), false);
}

/*virtual*/ WeaponGrenade * MainWeapon_Grenade::_CreateThrowWepaon(VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, INT32 nNetSlotIdx)
{
	I3ASSERT( m_pWeaponBase->getWeaponInfo()->GetTypeFire() == WEAPON::FIRE_THROWING);

	WeaponGrenade * pNewWeapon = (WeaponGrenade*) g_pWeaponManager->GetFreeThrowWeapon(
		m_pWeaponBase->getWeaponInfo()->GetTypeClass(), m_pWeaponBase->getWeaponInfo()->GetNumber() );
	if( pNewWeapon == nullptr )
	{
		// 인스턴스가 없습니다..?
		//I3PRINTLOG(I3LOG_FATAL,  "no instance resource." );
		return nullptr;
	}
	// Reset 하면 Bullet을 깎으면 안된다.
	m_bResetBullet = false;

	pNewWeapon->SetOwner(  m_pWeaponBase->getOwner());	// 먼저 세팅해야됩니다.	
	pNewWeapon->Reset();
	pNewWeapon->Throw( vStart, vDir, rSpeed, false, nNetSlotIdx);

	// Network ID
	pNewWeapon->setNetworkIdx( (INT16)nNetSlotIdx);
	if( nNetSlotIdx == -1)
	{
		const GAMEINFO_CHARACTER * pSendBuffer = BattleSlotContext::i()->getGameInfo_Chara(  m_pWeaponBase->getOwner()->getCharaNetIndex());
		pNewWeapon->setNetworkIdx( pSendBuffer->_tNetData._tFireData.getNetSlotIndex());
	}

	VEC3D	vTemp;
	m_pWeaponBase->getOwner()->getCameraContext()->getVirtualCamPosition( &vTemp );
	GAMEEVENT::BindNetGrenadeInfo( pNewWeapon->getNetworkIdx(), pNewWeapon, &vTemp );

	if( m_pWeaponBase->IsDedicatedHost() == false )
	{	
		pNewWeapon->SetPos( &vTemp);			
		pNewWeapon->SetVisible( false );
	}

	if( m_pWeaponBase->getOwner() != nullptr && m_pWeaponBase->getOwner()->isLocal() )
	{
		// Radio Sound 출력 하는 경우에만 처리.
		if( isPlayRadioSound() )  
		{
			if( BattleServerContext::i()->IsP2PHost())
				GameContextUtil::PlayRadioMessageSound( RADIOCHAT_TYPE_AUTO, 1, m_pWeaponBase->getOwner());

			GAMEEVENT::Write_Chara_RadioChat( m_pWeaponBase->getOwner(), 31);
		}

		// 해킹 방지 count
		// KEYWORD : Hack Protect
		GrenadeHackDetection(m_uCount);
	}

	if ( ! i3::same_of<WeaponDualGrenade*>(m_pWeaponBase))
		m_pWeaponBase->SetVisible( false); // 자기 손에 있는 수류탄 

	return pNewWeapon;
}

/*virtual*/ void MainWeapon_Grenade::_SetThrowData(VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed)
{
	addBombState( WEAPON_BOMBSTATE_THROWING);

	m_ThrowData.m_rElapsedThrowTime	= 0.0f;

#if !defined( NO_GRENADE_TRACE_EFFECT)
	m_ThrowData.m_rTracerUpdateTime	= 0.0f;
#endif

	m_ThrowData.m_rSpeed = rSpeed;

	i3Vector::Copy( &m_ThrowData.m_vStartPos, vStart);
	i3Vector::Copy( &m_ThrowData.m_vDirection, vDir);

	i3Vector::Copy( &m_ThrowData.m_vOldPos, vStart);
	i3Vector::Copy( &m_ThrowData.m_vTracerOldPos, vStart);

	m_pWeaponBase->SetHitCollideeGroup();
}

/*virtual*/ void MainWeapon_Grenade::_TakeObject(void)
{
	I3_PHYSIX_HIT_RESULT * pResult;
	i3CollideeLine line;
	VEC3D vDir;
	VEC3D vPos;

	m_bCheckFloor = true;

	i3Vector::Set( &vDir, 0.f, -1.f, 0.f);
	i3Vector::Copy( &vPos, m_pWeaponBase->GetPos());
	i3Vector::AddY( &vPos, 0.05f);
	line.SetStart( &vPos);
	line.SetDir( &vDir);
	pResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK);
	if( pResult == nullptr)
		return;

	UINT16 shapeGroup = pResult->m_pDestShapeSet->getShapeGroup();

	if( OBJECT_COLLISION_BY_WEAPON_HIT( shapeGroup ) )
	{
		i3EventReceiver * pReceiver = pResult->m_pDestShapeSet->getEventReceiver();
		I3ASSERT( i3::kind_of<i3Object*>(pReceiver));

		i3Object * pObj = (i3Object*) pReceiver;
		i3Transform * pTrans = GlobalFunc::FindTransformFromPhysXSceneObj( pObj->getSceneObject() );

		if( pTrans != nullptr )
		{
			// 애니메이션 하는 오브젝트입니다.
			if( pTrans->IsAnimated() )
				m_pAttachObjectTransform = pTrans;

			m_pAttachObject = pObj;
		}
	}
}

/*virtual*/ void MainWeapon_Grenade::_UpdateTakeObjectTransform(void)
{
	// 애니메이션 되는 Transform입니다. Matrix를 여기 맞춰줍니다.
	if( m_pAttachObject && m_pAttachObjectTransform)
	{	
		for(INT32 i = 0 ; i < m_pAttachObject->getStateCount(); i++)
		{
			if( m_pAttachObject->getHP(i) == 0)
				return;
		}

		MATRIX	matTop;
		VEC3D	vNewPosDir;

		i3SceneTracer * pTracer;

		pTracer = g_pFramework->GetViewer()->GetSceneTracer();

		I3ASSERT(pTracer);

		pTracer->GetModelViewMatrixStack()->Reset();
		pTracer->SetTime( g_pViewer->getDeltaTime());
		pTracer->SetViewMatrix( g_pCamera->getCamMatrix(), g_pCamera->getCamMatrix());

		m_pAttachObjectTransform->CalcUpdatedMatrix( &matTop, pTracer);

		// 애니메이션된 좌표에서 이전 프레임의 좌표의 차를 포지션에 더해줍니다.
		i3Vector::Sub( &vNewPosDir, i3Matrix::GetPos( &matTop ), i3Matrix::GetPos( m_pAttachObjectTransform->GetCacheMatrix() ) );

		i3Vector::Add( &vNewPosDir, &vNewPosDir, m_pWeaponBase->GetPos());
		m_pWeaponBase->SetPos( &vNewPosDir);
	}
}

void	MainWeapon_Grenade::Throw( VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, bool bOrigin, INT32 nNetSlotIdx)
{
	////////////////////////////////////////////////////////////////////////////////////////
	//	던지는 원본 (화면에 날라가는 개체가 아니다. 캐릭터가 아직 들고 있을 경우 인스턴스로 생성해서 월드에 던집니다.)
	if( bOrigin)
	{
		_CreateThrowWepaon(vStart, vDir, rSpeed, nNetSlotIdx);
	}
	////////////////////////////////////////////////////////////////////////////////////////
	//	던져지는 개체 (인스턴스로 생성되어 날아가는 녀석이다.)
	else
	{
		_SetThrowData(vStart, vDir, rSpeed);
	}
}


bool MainWeapon_Grenade::_CheckThrow( REAL32 LocalTime )
{
	if( (BattleServerContext::i()->IsP2PHost() && LocalTime > m_pWeaponBase->getWeaponInfo()->GetAttackTimeA(0) ) ||
		(!BattleServerContext::i()->IsP2PHost() && LocalTime > (m_pWeaponBase->getWeaponInfo()->GetAttackTimeA(0) - 0.05f) ) )
	{
		if( !m_pWeaponBase->getOwner()->isNetwork() )// 네트워크는 패킷으로 처리합니다.
		{
			//손에서 떠나가는 시간
			// 시점이 향하고 있는 목표 지점을 구한다.
			VEC3D target;
			m_pWeaponBase->GetShotTargetPos( &target);
			m_pWeaponBase->NET_Throw( nullptr, &target, -1 );
		}

		return true;
	}

	return false;
}
