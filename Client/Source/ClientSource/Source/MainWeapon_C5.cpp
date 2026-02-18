#include "pch.h"
#include "MainWeapon_C5.h"

#include "BattleObjContext.h"
#include "GameCharaWeaponContext.h"

#define GRENADE_DETACHHAND_TIME 0.1f		//손에서 떠나가는 시간

#define C5_POS_ASSISTTIME		0.18f
#define C5_POS_ASSISTTIMEINV	5.5555555555555555555555555555555f

MainWeapon_C5::MainWeapon_C5(WeaponBase* p) : MainWeapon_Throw(p) 
					, m_rThrowLen(0.0f)
					, m_rElapsedExplosionTime(0.0f)
					, m_rExplosionDelay(0.0f)
					, m_rExplosionDelayTimer(0.0f)
					, m_rRotateY(0.0f)
					, m_bFirstBound(false)
					, m_rFloorCheckTime(0.0f)
					, m_bCheckAttachFloor(false)
					, m_rBoundObjectLen(0.0f)
{
	m_pAttachObjectTransform = nullptr;
	m_pAttachObject = nullptr;
	m_pBoundObject = nullptr;	
	m_bAttchedObject = false;
}

void	MainWeapon_C5::OnInitVariable()
{
	MainWeapon_Throw::OnInitVariable();

	m_rElapsedExplosionTime		= 0.f;
	m_rThrowLen					= 0.f;

	i3Vector::Zero( &m_vAttachNormal);
	i3Matrix::Identity( &m_AttachLocalMatrix);
	m_pAttachObject				= nullptr;
	m_pAttachObjectTransform	= nullptr;
	
	m_rExplosionDelay			= 0.f;
	m_rExplosionDelayTimer		= 0.f;
	m_bFirstBound				= false;

	m_rRotateY					= 0.f;
	m_rFloorCheckTime			= 0.f;

	m_bCheckAttachFloor			= false;

	m_rBoundObjectLen			= 0.f;
	i3Vector::Zero( &m_vObjectForce);
	i3Vector::Zero( &m_vObjectForceNormal);
	m_pBoundObject				= nullptr;
	m_bAttchedObject			= false;
}

void	MainWeapon_C5::Reset()
{
	MainWeapon_Throw::Reset();
	
	i3Matrix::Identity( m_pWeaponBase->GetMatrix());

	m_pWeaponBase->setWeaponStateMask(WSM_LOADBULLET_DISABLE | WSM_LOADMAG_DISABLE);

	m_rThrowLen					= 0.f;
	i3Vector::Zero( &m_vAttachNormal);
	m_pAttachObjectTransform	= nullptr;
	m_pAttachObject				= nullptr;
	i3Matrix::Identity( &m_AttachLocalMatrix);

	m_rElapsedExplosionTime		= 0.f;
	m_rExplosionDelay			= 0.f;
	m_rExplosionDelayTimer		= 0.f;
	m_rRotateY = 0.f;

	m_bFirstBound				= false;
	m_rFloorCheckTime			= 0.f;

	m_bCheckAttachFloor			= false;
	m_bAttchedObject			= false;

	i3Vector::Zero( &m_vObjectForce);
	i3Vector::Zero( &m_vObjectForceNormal);
	m_rBoundObjectLen			= 0.f;
	m_pBoundObject				= nullptr;

	m_pWeaponBase->SetEnable( true);
}

REAL32 MainWeapon_C5::_BoundObject( I3_PHYSIX_HIT_RESULT * pHitResult, VEC3D * pDir)
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
		MATRIX mtxInv;
		i3SceneTracer * pTracer;

		pTracer = g_pFramework->GetViewer()->GetSceneTracer();
		I3ASSERT( pTracer != nullptr);

		pTracer->GetModelViewMatrixStack()->Reset();
		pTracer->SetTime( g_pViewer->getDeltaTime());
		pTracer->SetViewMatrix( g_pCamera->getCamMatrix(), g_pCamera->getCamMatrix());
		
		pTrans->CalcUpdatedMatrix( &matTop, pTracer);
		
		i3Matrix::Inverse( &mtxInv, 0, pTrans->GetCacheMatrix());

		VEC3D vNewPosDir;
		i3Vector::Sub( &vNewPosDir, i3Matrix::GetPos( &matTop ), i3Matrix::GetPos( pTrans->GetCacheMatrix() ) );

		REAL32 rDot = i3Vector::Dot( &vNewPosDir, &pHitResult->m_Normal );

		if( rDot > 0.f )
		{
			i3Vector::Scale( &m_vObjectForce, &vNewPosDir, 1.6f );
			i3Vector::Copy( &m_vObjectForceNormal, &pHitResult->m_Normal);

			if( pDir != nullptr)
			{
				VEC3D vDir;
				i3Vector::Normalize( &vDir, pDir);
				REAL32 rDot2 = i3Vector::Dot( &pHitResult->m_Normal, &vDir);

				//마이너스 어쩔
				REAL32 rLen = i3Vector::Length( &m_vObjectForce);
				REAL32 rBoundVal = rLen * rDot2;
				if( rBoundVal > rLen)
					rBoundVal = rLen;

				m_pBoundObject = (i3Object*) pHitResult->m_pDestShapeSet->getEventReceiver();

				return rBoundVal;
			}
		}
	}

	return 0.f;
}

void MainWeapon_C5::_ProcessBoundObjectForce()
{
	i3CollideeLine line;
	VEC3D vStartPos, vDir;

	if( getX( &m_vObjectForce) != 0.f || getY( &m_vObjectForce) != 0.f || getZ( &m_vObjectForce) != 0.f)
	{
		// 한번더 반대편으로 컬리젼 오브젝트가 있다면 밀어낸다.
		i3Vector::Scale( &vStartPos, &m_vObjectForce, 100.f );
		i3Vector::Add( &vStartPos, &vStartPos, &m_ThrowData.m_vOldPos);

		line.SetStart( &vStartPos);

		i3Vector::Normalize( &vDir, &m_vObjectForce);
		i3Vector::Scale( &vDir, &vDir, -1.f);
		line.SetDir( &vDir);
		
		UINT32 nResultCount = g_pFramework->raycast( &line, CGRP_MASKGROUP_ATTACK);
		I3_PHYSIX_HIT_RESULT * pHitResult = g_pFramework->getRaycastResult();

		for( UINT32 i = 0; i < nResultCount; i++)
		{
			if( pHitResult[ i].m_T < i3Math::abs( m_rBoundObjectLen) )
			{
				I3ASSERT( pHitResult[ i].m_pDestShapeSet != nullptr);

				if( i3::kind_of<i3Object*>(pHitResult[ i].m_pDestShapeSet->getEventReceiver()))
				{
					if( m_pBoundObject == pHitResult[ i].m_pDestShapeSet->getEventReceiver())
					{	
						_BoundObject( &pHitResult[ i], nullptr );

						VEC3D vAddPos;
						i3Vector::Scale( &vAddPos, &m_vObjectForce, 1.6f);
						
						i3Vector::Add( m_pWeaponBase->GetPos(), m_pWeaponBase->GetPos(), &vAddPos);
						//i3Vector::Add( &m_vOldPos, &m_vOldPos, &vAddPos);
						//i3Vector::Add( &m_vStartPos, &m_vStartPos, &vAddPos);

						return;
					}
				}
			}
		}

		i3Vector::Zero( &m_vObjectForce);
	}
}

void MainWeapon_C5::Throw( VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed)
{
	addBombState( WEAPON_BOMBSTATE_THROWING);

	m_ThrowData.m_rElapsedThrowTime		= 0.f;
	m_ThrowData.m_rSpeed				= rSpeed;

	i3Vector::Copy( &m_ThrowData.m_vStartPos, vStart);
	i3Vector::Copy( &m_ThrowData.m_vDirection, vDir);

	i3Vector::Copy( &m_ThrowData.m_vOldPos, vStart);

	m_pWeaponBase->SetHitCollideeGroup();

	if( m_pWeaponBase->GetAIContext() != nullptr)
		m_pWeaponBase->GetAIContext()->SetExclusiveEnable( false, false);
}

void MainWeapon_C5::UpdateFlying( REAL32 rDeltaSeconds)
{	
	if( rDeltaSeconds > 0.5f)
		rDeltaSeconds = 0.016f;

	// Dedicated server용이면 자신의 것을 Update
	if( m_pWeaponBase->IsDedicatedHost() )
	{
		if( m_ThrowData.m_rSpeed < 0.2f)
		{
			if( !m_bCheckAttachFloor )
			{
				_CollisionTestForAttach();
				m_bCheckAttachFloor = true;
			}
			return;
		}
		if (!isBombState(WEAPON_BOMBSTATE_SETTLED))
		{
			CheckWorldCollision(rDeltaSeconds);
		}
	}
	else
	{	// 네트워크 처리입니다.
		UpdateNetworkData( rDeltaSeconds);
	}

	m_ThrowData.m_rElapsedThrowTime += rDeltaSeconds;	
}

void MainWeapon_C5::CheckWorldCollision( REAL32 rDeltaSeconds)
{
	VEC3D	vTemp;
	REAL32	rLength;
	i3CollideeLine ColLine;
	I3_PHYSIX_HIT_RESULT * pResult = nullptr;
	bool	bCharaCollision = false;

	INT32	nBoundType = 0;

	VEC3D	vNew;
	i3Vector::Scale( &vNew, &m_ThrowData.m_vDirection, m_ThrowData.m_rSpeed * m_ThrowData.m_rElapsedThrowTime);
	i3Vector::Add( m_pWeaponBase->GetPos(), &m_ThrowData.m_vStartPos, &vNew);
	REAL32	rGravity = -9.81f * m_pWeaponBase->getWeaponInfo()->GetWeight() * m_ThrowData.m_rElapsedThrowTime * m_ThrowData.m_rElapsedThrowTime;
	addY( m_pWeaponBase->GetPos(), rGravity);
	
	m_rRotateY -= I3_DEG2RAD( rDeltaSeconds * 360.f);
	i3Matrix::PreRotateY( m_pWeaponBase->GetMatrix(), -I3_DEG2RAD( rDeltaSeconds * 360.f));

	if( !m_bFirstBound )
		i3Matrix::PreRotateX( m_pWeaponBase->GetMatrix(), - I3_DEG2RAD( rDeltaSeconds * 360.f));

	ColLine.SetStart( &m_ThrowData.m_vOldPos);
	i3Vector::Sub( &vTemp,  m_pWeaponBase->GetPos(), &m_ThrowData.m_vOldPos);
	rLength = i3Vector::Normalize( &vTemp, &vTemp);

	if( rLength <= 0.f )
	{
		_ProcessBoundObjectForce();
		return;
	}

	m_rThrowLen += rLength;

	ColLine.SetEnd( m_pWeaponBase->GetPos());
	ColLine.SetDir( &vTemp);

	REAL32 rMinLen = 10000.f;
	// World collision
	UINT32 nWorldCount = g_pFramework->raycast( &ColLine, CGRP_MASKGROUP_ATTACK );
	if( nWorldCount >0)
	{
		I3_PHYSIX_HIT_RESULT * pWorldResult = g_pFramework->getRaycastResult();
		g_pFramework->reserveRaycastResult( nWorldCount);

		for( UINT32 i = 0; i < nWorldCount; i++)
		{
			if( pWorldResult[ i].m_pDestShapeSet->getShapeGroup() != CGRP_ITEM_HIT)
			{	
				if( pWorldResult[ i].m_T < rMinLen )
				{
					pResult = &pWorldResult[ i];
					rMinLen = pResult->m_T;
				}
			}
		}
	}

	if( pResult == nullptr)
	{
		_ProcessBoundObjectForce();
		return;
	}
	
	// 첫번째 raycast는 Chara들의 Rough Collision을 검사합니다.
	UINT32 nRoughCount = g_pFramework->raycast( &ColLine, CGRP_MASKGROUP_GRENADE );
	if( nRoughCount > 0)
	{
		I3_PHYSIX_HIT_RESULT * pRoughResult = g_pFramework->getRaycastResult();

		g_pFramework->reserveRaycastResult( nRoughCount + nWorldCount);

		for( UINT32 i = 0; i < nRoughCount; i++)
		{
			i3EventReceiver * pRecv = pRoughResult[ i].m_pDestShapeSet->getEventReceiver();

			if( !i3::kind_of<CGameCharaBase*>(pRecv))
				continue;

			UINT32 nColGroup = CGRP_MASKGROUP_CHARA_BLUE;
			CGameCharaBase * pTarget = (CGameCharaBase *)pRecv;

			if( pTarget == m_pWeaponBase->getOwner() )
				continue;

			if( pTarget->getCharaTeam() & CHARACTER_TEAM_RED)
			{
				nColGroup = CGRP_MASKGROUP_CHARA_RED;
			}

			UINT32 nCount = g_pFramework->raycast( &ColLine, nColGroup );
			if( nCount <= 0)
				continue;

			I3_PHYSIX_HIT_RESULT * pHitResult = g_pFramework->getRaycastResult();
			g_pFramework->reserveRaycastResult( nCount + nRoughCount + nWorldCount);

			for( UINT32 j = 0; j < nCount; j++)
			{
				pRecv = pHitResult[ j].m_pDestShapeSet->getEventReceiver();
				if( !i3::kind_of<CGameCharaBase*>(pRecv))
					continue;
				
				pTarget = (CGameCharaBase *) pRecv;
				if( pTarget == m_pWeaponBase->getOwner() )
					continue;

				if( !pTarget->isCharaStateMask( CHARA_STATE_DEATH) &&
					(rMinLen > pHitResult[ j].m_T) )
				{
					pResult = &pHitResult[ j];
					rMinLen = pHitResult[ j].m_T;
					bCharaCollision = true;
				}
			}
		}
	}

	VEC3D	vecNew, vecNewPos, vecNormal;
	REAL32	rDot1 = 0.0f;
	
	//이동 방향 벡터 와 이동거리
	i3Vector::Sub( &vecNew, &pResult->m_Intersect, &m_ThrowData.m_vOldPos);
	REAL32 rTestLen = i3Vector::Normalize( &vecNew, &vecNew);

	//C-5는 부착형이라 할 필요 없어?
	//// 움직이는 오브젝트라면 힘을 계산해서 반사량을 더해줘야합니다. by dfly79 080117
	m_rBoundObjectLen = _BoundObject( pResult, ColLine.GetDir());

	if( (rLength < rTestLen - 0.03f + m_rBoundObjectLen) || (rTestLen == 0.f) )
	{	
		//유효한 거리내의 충돌이 없심..
		_ProcessBoundObjectForce();
		return;
	}

	// 들어간 만큼 뒤로 무른다.
	// Step1. 수류탄의 위치를 충돌이 일어난 위치로 이동( 수류탄의 Radius를 포함)
	VEC3D vecRevert;
	i3Vector::Scale( &vecRevert, &vecNew, -0.03f);

	if( getX( &m_vObjectForce) != 0.f || getY( &m_vObjectForce) != 0.f || getZ( &m_vObjectForce) != 0.f)
	{
		VEC3D vAddForce;
		REAL32 rHeightGravity = -rGravity;// *getY(&m_vObjectForce);
		i3Vector::Scale( &vAddForce, &m_vObjectForce, 3.f);
		i3Vector::Add( &vecRevert, &vecRevert, &vAddForce);
		i3Vector::AddY( &vecRevert, rHeightGravity);
	}

	REAL32 rDot = 1.f;
	
	UINT16 shapeGroup = pResult->m_pDestShapeSet->getShapeGroup();
	if( ( shapeGroup== CGRP_WORLD_ATTACK) || OBJECT_COLLISION_BY_WEAPON_HIT( shapeGroup ) )
	{
		rDot = i3Vector::Dot( &pResult->m_Normal, &I3_YAXIS);
	}

	if( ((rDot < 0.1f) && (rDot > -0.1f)) && (m_rThrowLen < m_pWeaponBase->getWeaponInfo()->GetAttachLength()) )
	{	// 벽면에 붙입니다.
		i3Vector::Scale( &vecNew, &pResult->m_Normal, 0.03f);
		i3Vector::Add( &vecNew, &pResult->m_Intersect, &vecNew);
		i3Matrix::SetUp( m_pWeaponBase->GetMatrix(), 0.f, 1.f, 0.f, 0.f);
		i3Matrix::SetAt( m_pWeaponBase->GetMatrix(), getX( &pResult->m_Normal), getY( &pResult->m_Normal), getZ( &pResult->m_Normal), 0.f);

		VEC3D vCross;
		i3Vector::Cross( &vCross, &I3_YAXIS, &pResult->m_Normal);
		i3Matrix::SetRight( m_pWeaponBase->GetMatrix(), getX( &vCross), getY( &vCross), getZ( &vCross), 0.f);

		// revision 61755 [Weapon] 메디컬킷 및 C5 와 같은 부착형 Grenade의 모델링 방향을 pef값으로 읽어와 회전시키도록 수정
		i3Matrix::PreRotateX( m_pWeaponBase->GetMatrix(), m_pWeaponBase->getWeaponInfo()->GetRotateAngle(WEAPON::WALL_X)); // KEYWORD : ROTATE_GRENADE
//		i3Matrix::PreRotateX( m_pWeaponBase->GetMatrix(), -I3_PI2);

		m_pWeaponBase->SetPos( &vecNew);
		m_ThrowData.m_rSpeed = 0.f;

		i3Vector::Copy( &m_vAttachNormal, &pResult->m_Normal);
		g_pSndMng->StartWeaponStateSnd( GTBWS_ON_C5, 1, m_pWeaponBase->GetPos(), false);

		addBombState( WEAPON_BOMBSTATE_ATTACHWALL);

		m_bCheckAttachFloor = true;

		m_pAttachObject = nullptr;
		_TakeObject( pResult);
	}
	else
	{
		m_bFirstBound = true;

		VEC3D vGrenadeSize;
		i3Vector::Scale( &vGrenadeSize, &pResult->m_Normal, 0.03f);
		
		i3Vector::Add( &vecRevert, &vecRevert, &vGrenadeSize);
		i3Vector::Add( m_pWeaponBase->GetPos(), &pResult->m_Intersect, &vecRevert);	//	들어온 반대 방향으로 무른다.
		i3Vector::Add( &m_ThrowData.m_vOldPos, &m_ThrowData.m_vOldPos, &vecRevert);

		i3Vector::Add( &vecNewPos, &pResult->m_Intersect, &vecNew);	//NewPosition
		rDot1 = i3Vector::Dot( &vecNew, &pResult->m_Normal);

		i3Vector::Scale( &vecNormal, &pResult->m_Normal, - 2.0f * rDot1);
		i3Vector::Add( &vecNewPos, &vecNewPos, &vecNormal);

		i3Vector::Sub( &vecNew, &vecNewPos, &pResult->m_Intersect);
		i3Vector::Normalize( &vecNew, &vecNew);

		VEC3D vPos;
		i3Vector::Copy( &vPos, m_pWeaponBase->GetPos());
		i3Matrix::Identity( m_pWeaponBase->GetMatrix());
//		i3Matrix::SetRotateX( m_pWeaponBase->GetMatrix(), I3_PI);
		i3Matrix::SetRotateX( m_pWeaponBase->GetMatrix(), m_pWeaponBase->getWeaponInfo()->GetRotateAngle(WEAPON::FLOOR_X)); // KEYWORD : ROTATE_GRENADE
		i3Matrix::PreRotateY( m_pWeaponBase->GetMatrix(), m_rRotateY);

		m_pWeaponBase->SetPos( &vPos);

		//반사 속도 계산
		REAL32	Speed = m_ThrowData.m_rSpeed;

		rDot1 = i3Vector::Dot( &vecNew, &m_ThrowData.m_vDirection);
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
		m_pWeaponBase->NET_Bound( m_pWeaponBase->GetPos(), &vecNew, Speed, nBoundType );

		i3Vector::Set( &m_vAttachNormal, 0.f, 1.f, 0.f);
	}

	g_pFramework->resetRaycastResult();
}

void	MainWeapon_C5::OnUpdate_C5(REAL32 rDeltaSeconds)
{
	
	if( isThrowing())
	{
		if( _CheckProcessUserLeave() == false)
			return;

		bool bExplosion = false;
		bool bSettled = false;
		bool bHost = m_pWeaponBase->IsDedicatedHost();


		if( bHost )
		{
			// 커맨드큐 방식으로 처리하는 코드입니다. by dfly79
			// 총 16개까지의 커맨드를 처리가능.
			for( INT32 i = 0; i < m_pWeaponBase->CommandCount(); i++)
			{
				INT32 flag = m_pWeaponBase->PopCommand();
				INT32 cmd = ( flag>> 28) & 0x0F;
				INT32 arg = flag & 0x0FFFFFFF;
				switch( cmd)
				{
				case WEAPON::STATE_FIRE:		// 거리에 따라 폭발 시점이 달라지도록 합니다.
					m_rExplosionDelay = ((REAL32)arg) * 0.005f;
					if( m_rExplosionDelay < 0.1f)
						bExplosion = true;
					break;
				}
			}

			if( m_rExplosionDelay > 0.01f)
			{
				m_rExplosionDelayTimer += rDeltaSeconds;
				if( m_rExplosionDelayTimer > m_rExplosionDelay)
				{
					m_rExplosionDelay = 0.f;
					m_rExplosionDelayTimer = 0.f;
					bExplosion = true;
				}
			}

			if (m_ThrowData.m_rSpeed < 0.2f && !isBombState(WEAPON_BOMBSTATE_SETTLED))
			{
				addBombState(WEAPON_BOMBSTATE_SETTLED);
				bSettled = true;
			}
		}

		UpdateFlying( rDeltaSeconds );

		_ProcessOnTakeObject();

		m_pWeaponBase->_TransformWorldSpace( rDeltaSeconds);

		i3Vector::Copy(&m_ThrowData.m_vOldPos, m_pWeaponBase->GetPos());

		// 격발 상태입니다.. 타이머를 계산하고 폭발시킵니다.
		if( isBombState( WEAPON_BOMBSTATE_TIMESTART))
		{
			m_rElapsedExplosionTime += rDeltaSeconds;
			if( m_rElapsedExplosionTime > m_pWeaponBase->GetExplosionTime() )
			{
				bExplosion = true;
			}
		}

		// 네트워크로 위치를 보내줍니다.( for CS )
		if( m_pWeaponBase->getNetworkIdx() != -1)
		{
			// revision 63118 
			//owner가 죽은 경우 삭제. Indonesia Hansoft.264
			if( isSettled() && m_pWeaponBase->getOwner()->isCharaStateMask( CHARA_STATE_DEATH))
				RemoveInvalidBomb();
			
			if (bHost)
			{
				GAMETOCONTEXT_INFO_WEAPON_THROW		throwInfo( m_pWeaponBase->getWeaponInfo()->GetItemID(), m_pWeaponBase->GetPos(), NET_GRENADE_STATE_FLY);

				throwInfo._pNormal			= GetThrowInfo_Normal();
				throwInfo._AttackType		= WEAPON::ATTACK_TYPE_R1;
				if( bExplosion)
				{	// 폭파되는 C5
					throwInfo._nAddStateFlag	= NET_GRENADE_STATE_EXPLOSION;
				}
				else if( bSettled)
				{	// 바닥에 고정된 경우
					throwInfo._nAddStateFlag	= NET_GRENADE_STATE_SETTLED;

					//@eunil. C-5 오브젝트에 고정되었다.
					if (m_pAttachObject != nullptr) {
						throwInfo._nAddStateFlag |= NET_GRENADE_STATE_TAKEOBJECT;
						
						//움직이는 오브젝트의 경우 현재 좌표도 같이 보내준다.
						if (m_pAttachObjectTransform != nullptr)
							throwInfo._AttachedObjPos = i3Matrix::GetPos(m_pAttachObjectTransform->GetCacheMatrix());
						else
							throwInfo._AttachedObjPos = m_pAttachObject->GetPos();
					}
				}

				if( isBombState( WEAPON_BOMBSTATE_ATTACHOBJECT) && (m_pAttachObject != nullptr) )
				{
					throwInfo._nAddStateFlag	|= NET_GRENADE_STATE_TAKEOBJECT;
					throwInfo._nBoundType		= m_pAttachObject->getNetIndex();
				}

				GAMEEVENT::Write_ThrowingWeapon( m_pWeaponBase->getNetworkIdx(), &throwInfo );
			}
		}

		if( bExplosion)
			this->Explosion_C5();
	}

}

void MainWeapon_C5::_TakeObject( i3Object * pObject)
{
	MATRIX mtxInv;

	I3ASSERT( pObject != nullptr);

	m_pAttachObject = pObject;
	i3Transform * pTrans = nullptr;
	
	// 애니메이션이 있는 경우 애니메이션되는 노드의 좌표를 기준으로 로컬좌표로 붙여줍니다.
	pTrans = GlobalFunc::FindTransformFromPhysXSceneObj( pObject->getSceneObject());

	if( pTrans != nullptr && ( pTrans->IsAnimated() ) )
	{
		m_pAttachObjectTransform = pTrans;

		i3SceneTracer * pTracer;
		MATRIX matTop;

		pTracer = g_pFramework->GetViewer()->GetSceneTracer();

		I3ASSERT(pTracer);

		pTracer->GetModelViewMatrixStack()->Reset();
		pTracer->SetTime( g_pViewer->getDeltaTime());
		pTracer->SetViewMatrix( g_pCamera->getCamMatrix(), g_pCamera->getCamMatrix());

		m_pAttachObjectTransform->CalcUpdatedMatrix( &matTop, pTracer);

		i3Matrix::Inverse( &mtxInv, 0, m_pAttachObjectTransform->GetCacheMatrix() );
		i3Matrix::Mul( &m_AttachLocalMatrix, m_pWeaponBase->GetMatrix(), &mtxInv);

		VEC3D* vSyncPos = m_pWeaponBase->GetPos();

		i3Matrix::Mul(m_pWeaponBase->GetMatrix(), &m_AttachLocalMatrix, m_pAttachObjectTransform->GetCacheMatrix());

		vSyncPos = m_pWeaponBase->GetPos();

		VEC3D	vNewPosDir;
		i3Vector::Sub(&vNewPosDir, i3Matrix::GetPos(&matTop), i3Matrix::GetPos(m_pAttachObjectTransform->GetCacheMatrix()));
		i3Vector::Add(&vNewPosDir, &vNewPosDir, m_pWeaponBase->GetPos());

		m_pWeaponBase->SetPos(&vNewPosDir);		
	}
	else
	{
		// 애니메이션 싱크를 맞추지 않는다면 오브젝트의 로컬좌표로 붙여줍니다.
		i3Matrix::Inverse( &mtxInv, 0, pObject->GetMatrix());
		i3Matrix::Mul( &m_AttachLocalMatrix, m_pWeaponBase->GetMatrix(), &mtxInv);

		i3Matrix::Mul(m_pWeaponBase->GetMatrix(), &m_AttachLocalMatrix, m_pAttachObject->GetMatrix());
	}

	addBombState( WEAPON_BOMBSTATE_ATTACHOBJECT);
}

void MainWeapon_C5::_TakeObject( I3_PHYSIX_HIT_RESULT * pResult )
{	
	I3ASSERT( pResult != nullptr);

	UINT16 shapeGroup = pResult->m_pDestShapeSet->getShapeGroup();

	if( OBJECT_COLLISION_BY_WEAPON_HIT( shapeGroup ) )
	{
		i3EventReceiver * pReceiver = pResult->m_pDestShapeSet->getEventReceiver();

		I3ASSERT( i3::kind_of<i3Object*>(pReceiver)); 

		i3Object * pObj = (i3Object*) pReceiver;

		_TakeObject( pObj);
	}
}

void MainWeapon_C5::_CollisionTestForAttach( void)
{
	i3CollideeLine ColLine;
	I3_PHYSIX_HIT_RESULT * pResult = nullptr;

	VEC3D vDownVec;
	i3Vector::Set( &vDownVec, 0.f, -1.f, 0.f);
	i3Vector::Add( &vDownVec, &vDownVec, m_pWeaponBase->GetPos());

	ColLine.SetStart( m_pWeaponBase->GetPos());
	ColLine.SetEnd( &vDownVec);

	// World collision
	pResult = g_pFramework->raycastClosest( &ColLine, CGRP_MASKGROUP_ATTACK );
	if( pResult != nullptr)
	{
		VEC3D vPos;
		i3Vector::Scale( &vPos, &pResult->m_Normal, 0.03f);
		i3Vector::Add( &vPos, &vPos, &pResult->m_Intersect);
		i3Matrix::SetPos( m_pWeaponBase->GetMatrix(), &vPos);

		_TakeObject( pResult);
	}
}

void MainWeapon_C5::_ProcessOnTakeObject( void)
{
	MATRIX matTop;
	VEC3D vVec;

	if (m_pWeaponBase->IsDedicatedHost() && (m_pAttachObject != nullptr))
	{
		// 오브젝트에 붙어 있는 경우 해당 오브젝트가 파괴될시 떨어지게 합니다.
		if ((m_pAttachObject->getStateCount() > 1) && (m_pAttachObject->getCurrentHP() == 0.f))
		{
			m_bCheckAttachFloor = false;	// revision 52303 C5 | 오브젝트에 붙었을 경우 오브젝트 파괴시 초기화. [PB_SVC] Russia bug . 195
			m_pAttachObject = nullptr;
			m_pAttachObjectTransform = nullptr;

			i3Vector::Set(&vVec, 0.f, -1.f, 0.f);
			Throw(m_pWeaponBase->GetPos(), &vVec, 1.f);
			removeBombState(WEAPON_BOMBSTATE_SETTLED);
		}
	}

	if (isBombState(WEAPON_BOMBSTATE_SETTLED) == false)
		return;

	if( m_pAttachObjectTransform != nullptr)
	{
		i3SceneTracer * pTracer;

		pTracer = g_pFramework->GetViewer()->GetSceneTracer();

		I3ASSERT(pTracer);

		pTracer->GetModelViewMatrixStack()->Reset();
		pTracer->SetTime(g_pViewer->getDeltaTime());
		pTracer->SetViewMatrix(g_pCamera->getCamMatrix(), g_pCamera->getCamMatrix());

		m_pAttachObjectTransform->CalcUpdatedMatrix(&matTop, pTracer);

		VEC3D	vNewPosDir;
		i3Vector::Sub(&vNewPosDir, i3Matrix::GetPos(&matTop), i3Matrix::GetPos(m_pAttachObjectTransform->GetCacheMatrix()));
		i3Vector::Add(&vNewPosDir, &vNewPosDir, m_pWeaponBase->GetPos());

		m_pWeaponBase->SetPos(&vNewPosDir);
	}
	else if( m_pAttachObject != nullptr)
	{	// 물리 오브젝트
		i3Matrix::Mul( m_pWeaponBase->GetMatrix(), &m_AttachLocalMatrix, m_pAttachObject->GetMatrix());
	}
	else
	{	
		if( m_pWeaponBase->IsDedicatedHost() &&
			(isBombState( WEAPON_BOMBSTATE_ATTACHWALL ) == false) &&
			(isBombState( WEAPON_BOMBSTATE_FREEZE) == false) )
		{
			// 벽이 아닌 바닥에 있는 경우
			// 바닥이 변경(오브젝트 파괴)이 있으면 떨어지도록 합니다.
			m_rFloorCheckTime += g_pViewer->getDeltaTime();
			if( m_rFloorCheckTime > 0.2f)
			{
				m_rFloorCheckTime = 0.f;
				i3Vector::Set( &vVec, 0.f, -1.f, 0.f);

				i3CollideeLine line;
				line.SetStart( m_pWeaponBase->GetPos());
				line.SetDir( &vVec);

				I3_PHYSIX_HIT_RESULT * pResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK);

				if( pResult != nullptr)
				{
					if( pResult->m_pDestShapeSet->getShapeGroup() == CGRP_WORLD_ATTACK)
						addBombState( WEAPON_BOMBSTATE_FREEZE);
					else if( pResult->m_T > 0.1f)
					{
						Throw( m_pWeaponBase->GetPos(), &vVec, 1.f );
						removeBombState( WEAPON_BOMBSTATE_SETTLED);
					}
				}
			}
		}
	}
}

void MainWeapon_C5::UpdateNetworkData( REAL32 rDeltatime)
{
	REAL32	flerp;
	VEC3D	vTemp, vDir, vAxis;

	I3ASSERT( m_pWeaponBase->IsDedicatedHost() == false);

	if( m_pWeaponBase->getNetworkIdx() < 0 || m_pWeaponBase->getNetworkIdx() >= MAX_THROWWEAPON_COUNT)	return;

	GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( m_pWeaponBase->getNetworkIdx());
	GAMEINFO_THROW_WEAPON_ASSIST * pAssistInfo = &pInfo->_tAssistData;
	GAMEINFO_THROW_WEAPON_NETDATA * pNetInfo = &pInfo->_tNetData;

	VEC3D	vNetPos = pNetInfo->GetPos();
	VEC3D	vNetDir = pNetInfo->GetDir();

	// revision 54573 C5, 메디컬 킷 투척시 3인칭 시점에서 끊겨 보이는 현상 수정(hansoft. 4172)

		// 공중에서 날라가고 있는 상태
		// 인터폴레이트 합니다.
	pAssistInfo->_fInterpolateTime += rDeltatime;
	flerp	= pAssistInfo->_fInterpolateTime;

	if( (isBombState( WEAPON_BOMBSTATE_SETTLED) == false)  )
	{
		if (flerp > C5_POS_ASSISTTIME )
		{
			if (pNetInfo->GetState() & NET_GRENADE_STATE_FLY)
			{
				VEC3D vDir2;
				i3Vector::Sub( &vDir2, &vNetPos, &pAssistInfo->_vPostPos);
				i3Vector::Copy( &pAssistInfo->_vPostPos, m_pWeaponBase->GetPos() );

				i3Vector::Add( &vTemp, &pAssistInfo->_vPostPos, &vDir2 );
				pNetInfo->SetPos( vTemp );
				vNetPos = vTemp;
				flerp = rDeltatime;
			}
			else
				flerp = C5_POS_ASSISTTIME;
		}

		flerp *= C5_POS_ASSISTTIMEINV;

		m_rRotateY -= I3_DEG2RAD(rDeltatime * 360.f);

		i3Vector::Lerp(m_pWeaponBase->GetPos(), &pAssistInfo->_vPostPos, &vNetPos, flerp);
		i3Vector::Sub(&vTemp, &pAssistInfo->_vPostPos, &vNetPos);

		// 클라이언트 상에서 임의로 회전시켜줍니다.
		if ((isBombState(WEAPON_BOMBSTATE_SETTLED) == false))
		{
			if (i3Vector::LengthSq(&vTemp) > I3_EPS)
			{
				i3Matrix::PreRotateX(m_pWeaponBase->GetMatrix(), -I3_DEG2RAD(rDeltatime * 360.f));
				i3Matrix::PreRotateY(m_pWeaponBase->GetMatrix(), -I3_DEG2RAD(rDeltatime * 360.f));
			}
		}
	}

	if( pInfo->_bRecv )
	{
		if( pNetInfo->GetState() & NET_GRENADE_STATE_FLY)
			m_pWeaponBase->SetVisible( true );

		if( (isBombState( WEAPON_BOMBSTATE_SETTLED) == false) )
		{
			//I3TRACE("Bomb State : %d\n", pNetInfo->GetState());
			// 월드에 고정됩니다.
			if( (pNetInfo->GetState() & NET_GRENADE_STATE_SETTLED) )
			{
				addBombState( WEAPON_BOMBSTATE_SETTLED);

				// 축을 맞춥니다.
				i3Vector::Set( &vDir, getX( &vNetDir), getY( &vNetDir) , getZ( &vNetDir));
				i3Matrix::MakeWithAt( m_pWeaponBase->GetMatrix(), &vDir, &I3_YAXIS);
				i3Vector::Copy( &vAxis, i3Matrix::GetRight( m_pWeaponBase->GetMatrix()));
// 				i3Matrix::PostRotateAxis( m_pWeaponBase->GetMatrix(), &vAxis, -I3_PI2);
				if( getY( &vNetDir) < 0.5f)
				{	// 모델링의 Z축으로한 회전값은 벽에 붙이면 돌려줘야합니다.
					i3Vector::Copy( &vAxis, i3Matrix::GetAt( m_pWeaponBase->GetMatrix()));

					// revision 61755
					i3Matrix::PreRotateX( m_pWeaponBase->GetMatrix(), m_pWeaponBase->getWeaponInfo()->GetRotateAngle(WEAPON::WALL_X));// KEYWORD : ROTATE_GRENADE
					i3Matrix::PreRotateY( m_pWeaponBase->GetMatrix(), m_pWeaponBase->getWeaponInfo()->GetRotateAngle(WEAPON::WALL_Y));

//					i3Matrix::PreRotateAxis( m_pWeaponBase->GetMatrix(), &vAxis, I3_PI);
				}
				else
				{	// 바닥에 고정되는 경우입니다.
					i3Matrix::Identity( m_pWeaponBase->GetMatrix());
					i3Matrix::PreRotateY( m_pWeaponBase->GetMatrix(), m_rRotateY);
					i3Matrix::PreRotateX( m_pWeaponBase->GetMatrix(), m_pWeaponBase->getWeaponInfo()->GetRotateAngle(WEAPON::FLOOR_X));

				//	i3Matrix::PreRotateAxis( m_pWeaponBase->GetMatrix(), &vDir, m_rRotateY);
				}
				
				// 좌표를 맞춥니다.
				m_pWeaponBase->SetPos( &vNetPos);

				// 오브젝트에 고정되어 있는 경우
				if( (pNetInfo->GetState() & NET_GRENADE_STATE_TAKEOBJECT))
				{
					if( pNetInfo->GetBoundType() < OBJ_TOTAL_COUNT)
					{
						const GAMEINFO_OBJECT * pObjInfo = BattleObjContext::i()->getGameInfo_Object( (INT32) pNetInfo->GetBoundType());
						if( (pObjInfo != nullptr) && (pObjInfo->_pObjectRef != nullptr) )
						{
							_TakeObject(pObjInfo->_pObjectRef);
							
							//@eunil. C-5 움직이는 오브젝트 위치 차이만큼 싱크해준다.
							VEC3D* objPos;
							if (m_pAttachObjectTransform != nullptr)
								objPos = i3Matrix::GetPos(m_pAttachObjectTransform->GetCacheMatrix());
							else
								objPos = m_pAttachObject->GetPos();
							
							VEC3D vobjPostPos;
							VEC3D vSubResult;
							VEC3D vSyncPos;
							VEC3D vAttchedObjPos = pNetInfo->GetAttachPos();

							i3Vector::Copy(&vobjPostPos, objPos);
							i3Vector::Sub(&vSubResult, &vobjPostPos, &vAttchedObjPos);
							i3Vector::Add(&vSyncPos, &vSubResult, &vNetPos);

							m_pWeaponBase->SetPos(&vSyncPos);
						}
					}
				}
				else
					addBombState( WEAPON_BOMBSTATE_FREEZE);
			}
		}
		else
		{
			if (isBombState(WEAPON_BOMBSTATE_ATTACHOBJECT) &&
				!(pNetInfo->GetState() & NET_GRENADE_STATE_TAKEOBJECT))
			{
				m_bCheckAttachFloor = false;
				m_pAttachObject = nullptr;
				m_pAttachObjectTransform = nullptr;

				removeBombState(WEAPON_BOMBSTATE_SETTLED);
				removeBombState(WEAPON_BOMBSTATE_ATTACHOBJECT);
			}
		}

		if( pNetInfo->GetState() & NET_GRENADE_STATE_CLICKFIRE)
		{
			if( isBombState( WEAPON_BOMBSTATE_TIMESTART) == false)
			{
				g_pSndMng->StartWeaponStateSnd( GTBWS_ON_C5, 0, m_pWeaponBase->GetPos(), false);
				addBombState( WEAPON_BOMBSTATE_TIMESTART);

				// 던져진 리스트에서 삭제
				if( m_pWeaponBase->getOwner() != nullptr)
				{
					WeaponBase * pUseWeapon = m_pWeaponBase->getOwner()->getWeaponContext()->getWeapon( WEAPON_SLOT_THROWING1);
					if( i3::same_of<WeaponBombTrigger*>(pUseWeapon))
						((WeaponBombTrigger*)pUseWeapon)->removeDropedBomb( m_pWeaponBase);
				}
			}
		}

		if( pNetInfo->GetState() & NET_GRENADE_STATE_BOUND )
		{// 바운딩 사운드
			g_pSndMng->StartWeaponStateSnd( GTBWS_ON_C5, 1, m_pWeaponBase->GetPos(), false);
			// 바운드 후 플라잉 모드로 바꿔줘야 합니다.
			UINT8 tState = pNetInfo->GetState();
			tState &= ~NET_GRENADE_STATE_BOUND;
			pNetInfo->SetState(tState);
		}
		
		if( pNetInfo->GetState() & NET_GRENADE_STATE_EXPLOSION )
		{
			// 던져진 리스트에서 삭제
			if( m_pWeaponBase->getOwner() != nullptr)
			{
				WeaponBase * pUseWeapon = m_pWeaponBase->getOwner()->getWeaponContext()->getWeapon( WEAPON_SLOT_THROWING1);
				if( i3::same_of<WeaponBombTrigger*>(pUseWeapon))
					((WeaponBombTrigger*)pUseWeapon)->removeDropedBomb( m_pWeaponBase);
			}
			m_pWeaponBase->SetPos( &vNetPos);
			Explosion_C5();
		}

		pInfo->_bRecv = false;
	}
}

void	MainWeapon_C5::NET_Bound_C5( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType )		// 일단 비가상처리..WeaponC5의 동명함수로부터 내부 호출된다.
{
	
	if( m_pWeaponBase->IsDedicatedHost() && m_pWeaponBase->getNetworkIdx() != -1)
	{
		// 네트워크로 보내줍니다.
		GAMETOCONTEXT_INFO_WEAPON_THROW throwInfo( m_pWeaponBase->getWeaponInfo()->GetItemID(), pStartPos, NET_GRENADE_STATE_BOUND);
		throwInfo._nBoundType		= nBoundType;

		GAMEEVENT::Write_ThrowingWeapon( m_pWeaponBase->getNetworkIdx(), &throwInfo);
	}

	if( fSpeed > 0.5f)	//	약한 바운드는 궤적이 안나온다.
		g_pSndMng->StartWeaponStateSnd( GTBWS_ON_C5, 1, m_pWeaponBase->GetPos(), false);

	this->Throw( pStartPos, pDir, fSpeed);
}

bool MainWeapon_C5::_CheckProcessUserLeave( void)
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
			// revision 46747 C-5 자살시 3인칭 시점에서 Explosion()함수 호출 안되는 버그 수정
			bOwnerInvalid = false;
			break;
		}
	}

	if( bOwnerInvalid )
	{
		RemoveInvalidBomb();

		return false;
	}

	return true;
}

void	MainWeapon_C5::RemoveInvalidBomb(void)
{
	if( m_pWeaponBase->getNetworkIdx() != -1)
	{
		GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon(  m_pWeaponBase->getNetworkIdx());
		pInfo->Reset();
	}

	g_pWeaponManager->PushThrowWeapon( m_pWeaponBase);
}

void MainWeapon_C5::Explosion_C5( void)
{
	m_pWeaponBase->ResetCommand();

	// revision 45361
	m_pWeaponBase->_OnCheckCollisionByExplosion();
	m_pWeaponBase->_CheckExplosionWeapon( m_pWeaponBase->GetExplosionRange() );
	//CheckChainExplosion(m_pWeaponBase->GetPos(), m_pWeaponBase->GetExplosionRange() );


//	m_pWeaponBase->_CheckExplosionCollision( m_pWeaponBase->GetExplosionRange() );			// 이 호출 함수는 나중에 다시 고쳐야 한다..

	// 폭발 이팩트
	g_pEffectManager->AddEffect( m_pWeaponBase->getFireEffectIndex(), m_pWeaponBase->GetPos(), &I3_YAXIS);

	// 사용된 폭탄은 해제합니다.
	if( m_pWeaponBase->getOwner() != nullptr)
	{
		WeaponBase * pUseWeapon = m_pWeaponBase->getOwner()->getWeaponContext()->getWeapon( WEAPON_SLOT_THROWING1);
		if( i3::same_of<WeaponBombTrigger*>(pUseWeapon))
			((WeaponBombTrigger*)pUseWeapon)->removeDropedBomb( m_pWeaponBase);
	}
	g_pWeaponManager->PushThrowWeapon( m_pWeaponBase);
}

/*virtual*/ void MainWeapon_C5::ChainExplosion(void)
{
	Explosion_C5();
}