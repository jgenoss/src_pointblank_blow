#include "pch.h"
#include "MainWeapon_MedicalKit.h"
#include "BattleObjContext.h"
#include "WeaponFunction.h"
#include "GameCharaBoneContext.h"
#include "BattleServerContext.h"

#define MEDICAL_POS_ASSISTTIME		0.18f
#define MEDICAL_POS_ASSISTTIMEINV		5.5555555555555555555555555555555f

MainWeapon_MedicalKit::MainWeapon_MedicalKit(WeaponBase* p) : MainWeapon_Grenade(p),
	m_rRotateY(0.f), 	m_bFirstBound(false), m_rThrowLen(0.f), 
	m_rElapsedPickTime(0.f), m_bCalcEllipsedTime(false)
{
}

void	MainWeapon_MedicalKit::OnInitVariable() 
{
	MainWeapon_Grenade::OnInitVariable();

	i3Vector::Zero(&m_vAttachNormal);

	i3Matrix::Identity(&m_AttachLocalMatrix);

	m_rRotateY				= 0.0f;

	m_rThrowLen				= 0.0f;	
	m_rElapsedPickTime		= 0.0f;


	m_bFirstBound			= false;

	m_bCalcEllipsedTime		= false;
}

void	MainWeapon_MedicalKit::Reset()
{
	MainWeapon_Grenade::Reset();
	
	i3Vector::Zero(&m_vAttachNormal);

	i3Matrix::Identity(&m_AttachLocalMatrix);

	m_rRotateY				= 0.0f;

	m_rThrowLen				= 0.0f;	
	m_rElapsedPickTime		= 0.0f;


	m_bFirstBound			= false;

	m_bCalcEllipsedTime		= false;
}



void	MainWeapon_MedicalKit::OnCreate(bool bCreateInWeapon)
{
	MainWeapon_Grenade::OnCreate(bCreateInWeapon);

}


void	MainWeapon_MedicalKit::CheckWorldCollision(REAL32 rDeltaSeconds)
{
	VEC3D	vTemp;
	REAL32	rLength;
	i3CollideeLine ColLine;
	I3_PHYSIX_HIT_RESULT * pResult = nullptr;
	bool	bCharaCollision = false;

	INT32	nBoundType = 0;

	VEC3D	vNew;
	i3Vector::Scale( &vNew, &m_ThrowData.m_vDirection , m_ThrowData.m_rSpeed * m_ThrowData.m_rElapsedThrowTime );
	i3Vector::Add( m_pWeaponBase->GetPos(), &m_ThrowData.m_vStartPos, &vNew);
	REAL32	rGravity = -5.0f * m_pWeaponBase->getWeaponInfo()->GetWeight() * m_ThrowData.m_rElapsedThrowTime  * m_ThrowData.m_rElapsedThrowTime;
	addY( m_pWeaponBase->GetPos(), rGravity);

	m_rRotateY -= I3_DEG2RAD( rDeltaSeconds * 360.f);
	i3Matrix::PreRotateY( m_pWeaponBase->GetMatrix(), - I3_DEG2RAD( rDeltaSeconds * 360.f));


	if(!m_bFirstBound)
	{
		i3Matrix::PreRotateX( m_pWeaponBase->GetMatrix(), - I3_DEG2RAD( rDeltaSeconds * 360.f));
	}

	ColLine.SetStart(&m_ThrowData.m_vOldPos);
	i3Vector::Sub( &vTemp,  m_pWeaponBase->GetPos(), &m_ThrowData.m_vOldPos );
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

					//I3TRACE( "CharaCol %d %.4f\n", pTarget->getCharaNetIndex(), rMinLen);
				}
			}
		}
	}

	VEC3D	vecNew, vecNewPos, vecNormal;
	REAL32	rDot1 = 0.0f;
	
	//이동 방향 벡터 와 이동거리
	i3Vector::Sub( &vecNew, &pResult->m_Intersect, &m_ThrowData.m_vOldPos );
	REAL32 rTestLen = i3Vector::Normalize( &vecNew, &vecNew); 

	// 움직이는 오브젝트라면 힘을 계산해서 반사량을 더해줘야합니다. by dfly79 080117
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
	i3Vector::Scale( &vecRevert, &vecNew, - 0.03f);

	if( getX( &m_vObjectForce) != 0.f || getY( &m_vObjectForce) != 0.f || getZ( &m_vObjectForce) != 0.f)
	{
		VEC3D vAddForce;
		REAL32 rHeightGravity = -rGravity * getY( &m_vObjectForce);
		i3Vector::Scale( &vAddForce, &m_vObjectForce, 4.f);
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
		i3Matrix::SetUp( m_pWeaponBase->GetMatrix(), 0.f, -1.f, 0.f, 0.f);
		i3Matrix::SetAt( m_pWeaponBase->GetMatrix(), getX( &pResult->m_Normal), getY( &pResult->m_Normal), getZ( &pResult->m_Normal), 0.f);

		VEC3D vCross;
		i3Vector::Cross( &vCross, &I3_YAXIS, &pResult->m_Normal);

		i3Matrix::SetRight( m_pWeaponBase->GetMatrix(), getX( &vCross), getY( &vCross), getZ( &vCross), 0.f);

	//	i3Matrix::PreRotateX( m_pWeaponBase->GetMatrix(), -I3_PI2);
		// revision 61755
		i3Matrix::PreRotateX( m_pWeaponBase->GetMatrix(), m_pWeaponBase->getWeaponInfo()->GetRotateAngle(WEAPON::WALL_X)); // KEYWORD : ROTATE_GRENADE
		i3Matrix::PreRotateY( m_pWeaponBase->GetMatrix(), m_pWeaponBase->getWeaponInfo()->GetRotateAngle(WEAPON::WALL_Y));


		m_pWeaponBase->SetPos( &vecNew);
		m_ThrowData.m_rSpeed = 0.f;

		i3Vector::Copy( &m_vAttachNormal, &pResult->m_Normal);
		g_pSndMng->StartWeaponStateSnd( GTBWS_ON_C5, 1, m_pWeaponBase->GetPos(), false);

		addBombState( WEAPON_BOMBSTATE_ATTACHWALL);

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
		i3Vector::Add( &m_ThrowData.m_vOldPos , &m_ThrowData.m_vOldPos, &vecRevert);

		i3Vector::Add( &vecNewPos, &pResult->m_Intersect, &vecNew);	//NewPosition
		rDot1 = i3Vector::Dot( &vecNew, &pResult->m_Normal);

		i3Vector::Scale( &vecNormal, &pResult->m_Normal, - 2.0f * rDot1);
		i3Vector::Add( &vecNewPos, &vecNewPos, &vecNormal);

		i3Vector::Sub( &vecNew, &vecNewPos, &pResult->m_Intersect);
		i3Vector::Normalize( &vecNew, &vecNew);


		VEC3D vPos;
		i3Vector::Copy( &vPos, m_pWeaponBase->GetPos());
		i3Matrix::Identity( m_pWeaponBase->GetMatrix());

		// revision 61755
		i3Matrix::SetRotateX( m_pWeaponBase->GetMatrix(), 
			m_pWeaponBase->getWeaponInfo()->GetRotateAngle(WEAPON::FLOOR_X)); // KEYWORD : ROTATE_GRENADE

		i3Matrix::PreRotateY( m_pWeaponBase->GetMatrix(), m_rRotateY);

		m_pWeaponBase->SetPos( &vPos);

		//반사 속도 계산
		REAL32	Speed = m_ThrowData.m_rSpeed ;

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

		//재 투척
		NET_Bound( m_pWeaponBase->GetPos(), &vecNew, Speed, nBoundType );

		i3Vector::Set( &m_vAttachNormal, 0.f, 1.f, 0.f);
	}

	g_pFramework->resetRaycastResult();

}

/*virtual*/ void MainWeapon_MedicalKit::_PlayBoundSound(I3_TERRAIN_TYPE nTerrainType)
{
	I3ASSERT( g_pViewer != nullptr);
	g_pSndMng->StartWeaponStateSnd( GTBWS_ON_C5, 1, m_pWeaponBase->GetPos(), false);
}

void MainWeapon_MedicalKit::UpdateFlying( REAL32 rDeltaSeconds, REAL32 rExplosionRange)
{

	if( m_ThrowData.m_rSpeed < 0.15f)
	{
		this->_ProcessTakeObject();
		return;
	}

	if( rDeltaSeconds > 0.5f)
		rDeltaSeconds = 0.016f;

	if( m_pWeaponBase->IsDedicatedHost() )
	{
		CheckWorldCollision( rDeltaSeconds);
	}
	else
	{	// 네트워크 처리입니다.
		UpdateNetworkData( rDeltaSeconds);
	}

	m_ThrowData.m_rElapsedThrowTime += rDeltaSeconds;
}

void MainWeapon_MedicalKit::UpdateNetworkData(REAL32 rDeltatime)
{
	REAL32	flerp;
	VEC3D	vTemp, vDir, vAxis;

	I3ASSERT( m_pWeaponBase->IsDedicatedHost() == false);

	if( m_pWeaponBase->getNetworkIdx() < 0 || 
		m_pWeaponBase->getNetworkIdx() >= MAX_THROWWEAPON_COUNT)	return;

	GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( m_pWeaponBase->getNetworkIdx() );
	GAMEINFO_THROW_WEAPON_ASSIST * pAssistInfo = &pInfo->_tAssistData;
	GAMEINFO_THROW_WEAPON_NETDATA * pNetInfo = &pInfo->_tNetData;

	VEC3D vNetPos = pNetInfo->GetPos();
	VEC3D vNetDir = pNetInfo->GetDir();

	pAssistInfo->_fInterpolateTime += rDeltatime;
	flerp	= pAssistInfo->_fInterpolateTime;

	// revision 54573 C5, 메디컬 킷 투척시 3인칭 시점에서 끊겨 보이는 현상 수정(hansoft. 4172)
	if(isBombState(WEAPON_BOMBSTATE_SETTLED) == false)
	{
		if(flerp > MEDICAL_POS_ASSISTTIME)
		{
			if(pNetInfo->GetState() & NET_GRENADE_STATE_FLY)
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
				flerp = MEDICAL_POS_ASSISTTIME;
		}

		flerp *= MEDICAL_POS_ASSISTTIMEINV;


		// 클라이언트 상에서 임의로 회전시켜줍니다.
		m_rRotateY -= I3_DEG2RAD( rDeltatime * 360.f);

		i3Vector::Lerp( m_pWeaponBase->GetPos(), &pAssistInfo->_vPostPos, &vNetPos, flerp );
		i3Vector::Sub( &vTemp, &pAssistInfo->_vPostPos, &vNetPos );
		if( i3Vector::LengthSq( &vTemp ) > I3_EPS )
		{
			i3Matrix::PreRotateX( m_pWeaponBase->GetMatrix(), -I3_DEG2RAD( rDeltatime * 360.f));
			i3Matrix::PreRotateY( m_pWeaponBase->GetMatrix(), -I3_DEG2RAD( rDeltatime * 360.f));
		}
	}

	if( pInfo->_bRecv )
	{
		if( pNetInfo->GetState() & NET_GRENADE_STATE_FLY)
			m_pWeaponBase->SetVisible( true );

		if( (this->isBombState( WEAPON_BOMBSTATE_SETTLED) == false) )
		{
			// 월드에 고정됩니다.
			if( (pNetInfo->GetState() & NET_GRENADE_STATE_SETTLED) )
			{
				m_pWeaponBase->SetVisible( true );
				this->addBombState( WEAPON_BOMBSTATE_SETTLED);

				// 축을 맞춥니다.
				i3Vector::Set( &vDir, getX( &vNetDir), getY( &vNetDir) , getZ( &vNetDir));
				i3Matrix::MakeWithAt( m_pWeaponBase->GetMatrix(), &vDir, &I3_YAXIS);
				i3Vector::Copy( &vAxis, i3Matrix::GetRight( m_pWeaponBase->GetMatrix()));
				if( getY( &vNetDir) < 0.5f)
				{	// 모델링의 Z축으로한 회전값은 벽에 붙이면 돌려줘야합니다.
					i3Vector::Copy( &vAxis, i3Matrix::GetAt( m_pWeaponBase->GetMatrix()));

					// revision 61755
					i3Matrix::PreRotateX( m_pWeaponBase->GetMatrix(), 
						m_pWeaponBase->getWeaponInfo()->GetRotateAngle(WEAPON::WALL_X)); // KEYWORD : ROTATE_GRENADE
					i3Matrix::PreRotateY( m_pWeaponBase->GetMatrix(), 
						m_pWeaponBase->getWeaponInfo()->GetRotateAngle(WEAPON::WALL_Y));

				//	i3Matrix::PreRotateX( m_pWeaponBase->GetMatrix(), I3_PI2);
				}
				else
				{	// 바닥에 고정되는 경우입니다.
					i3Matrix::Identity( m_pWeaponBase->GetMatrix());
					i3Matrix::PreRotateY( m_pWeaponBase->GetMatrix(), m_rRotateY);
					i3Matrix::PreRotateX( m_pWeaponBase->GetMatrix(), m_pWeaponBase->getWeaponInfo()->GetRotateAngle(WEAPON::FLOOR_X));
				}

				// 좌표를 맞춥니다.
				m_pWeaponBase->SetPos( &vNetPos);
			}
		}
		else
		{
			if( this->isBombState( WEAPON_BOMBSTATE_ATTACHOBJECT) &&
				!(pNetInfo->GetState() & NET_GRENADE_STATE_TAKEOBJECT))
			{
				this->removeBombState( WEAPON_BOMBSTATE_SETTLED);
			}
		}

		if( pNetInfo->GetState() & NET_GRENADE_STATE_BOUND )
		{// 바운딩 사운드
			g_pSndMng->StartWeaponStateSnd( GTBWS_ON_C5, 1, m_pWeaponBase->GetPos(), false);
			// 바운드 후 플라잉 모드로 바꿔줘야 합니다.
			UINT8 tState = pNetInfo->GetState();
			tState	&= ~NET_GRENADE_STATE_BOUND;
			pNetInfo->SetState(tState);
		}

		pInfo->_bRecv = false;

		if( pNetInfo->GetState() & NET_GRENADE_STATE_EXPLOSION )
		{
			m_pWeaponBase->SetPos( &vNetPos);
			if( this->isBombState( WEAPON_BOMBSTATE_EXPLOSION) == false)
				Explosion();
		}
	}
}


void	MainWeapon_MedicalKit::OnUpdate_Grenade(REAL32 rDeltaSeconds)
{
	if( this->isThrowing())
	{	// 월드에 던져진 수류탄입니다.
		// 유저가 있는지 체크해서 없으면 반환시켜줍니다.
		if( _CheckProcessUserLeave() == false)
			return;

		WF::PlayWeaponAnim(m_pWeaponBase, m_pWeaponBase->getAIIndex(WEAPON::AI_LOADBULLET));

		bool	bSettled = false;
		bool	bExplosion = false;
		bool	bHost = m_pWeaponBase->IsDedicatedHost();

		if( bHost )
		{
			if( m_ThrowData.m_rSpeed < 0.2f && !this->isBombState( WEAPON_BOMBSTATE_SETTLED) )
			{
				//this->addBombState( WEAPON_BOMBSTATE_SETTLED);
				m_bCalcEllipsedTime = true;
				bSettled = true;
			}
		}

	//	if( this->isSettled())
	// revision 63118 C5, Medical Kit 의 Onwer가 죽었을 경우 삭제.
		REAL32 rExplosionRange = m_pWeaponBase->GetExplosionTime();
		if( this->isSettled() || m_bCalcEllipsedTime)
		{
			m_rElapsedExplosionTime += rDeltaSeconds;

			//owner가 죽은 경우 삭제. Indonesia Hansoft.264
			if( m_pWeaponBase->getOwner()->isCharaStateMask( CHARA_STATE_DEATH)) 
				m_rElapsedExplosionTime += rExplosionRange;
		}

		this->UpdateFlying( rDeltaSeconds, rExplosionRange );

		m_pWeaponBase->_TransformWorldSpace( rDeltaSeconds);

		i3Vector::Copy( &m_ThrowData.m_vOldPos , m_pWeaponBase->GetPos());

		if( m_pWeaponBase->getNetworkIdx() == -1)
			return;

		if( this->GetElapsedExplosionTime() >= rExplosionRange)
		{
			GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( m_pWeaponBase->getNetworkIdx() );
			pInfo->Reset();

			m_pWeaponBase->_ResetGrenadeInfo();

			g_pWeaponManager->PushThrowWeapon(m_pWeaponBase);
		}
		else
		{
			for(INT32 i = 0; i < g_pCharaManager->getCharaCount(); ++i)
			{
				CGameCharaBase * pChara = g_pCharaManager->getChara( i);

				if( pChara != nullptr)
				{
					// 투척 후 자신은 일정시간은 획득 불가능 
					//(사용자가 던지는 순간 획득할 수 있으므로 시간제어)
					if(pChara == m_pWeaponBase->getOwner() && m_rElapsedPickTime < 0.5f)
					{
						m_rElapsedPickTime += rDeltaSeconds;
						continue;
					}
					if( pChara->isAIToClient()) continue;

					bExplosion = this->RecoveryHP(pChara);

					if(bExplosion)
						break;
				}
			}
		}
			

		if( m_ThrowData.m_rElapsedThrowTime > 15.f/*NET_GRENADE_OFFTIME*/)
		{// 네트워로 받는 경우 15초까지 패킷을 못 받으면 그냥 없애줍니다.
			if( bHost )	bExplosion = true;
			else
			{
				GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon(	m_pWeaponBase->getNetworkIdx() );
				pInfo->Reset();

				m_pWeaponBase->_ResetGrenadeInfo();

				g_pWeaponManager->PushThrowWeapon(m_pWeaponBase); 
			}
		}

		if( bHost )
		{
			// 네트워크로 보내줍니다.
			GAMETOCONTEXT_INFO_WEAPON_THROW throwInfo(m_pWeaponBase->getWeaponInfo()->GetItemID(), &m_ThrowData.m_vOldPos, NET_GRENADE_STATE_FLY);
			throwInfo._pNormal				= GetThrowInfo_Normal();
			if( bExplosion )	throwInfo._nAddStateFlag	= NET_GRENADE_STATE_EXPLOSION;
			else if( bSettled )		throwInfo._nAddStateFlag	= NET_GRENADE_STATE_SETTLED;
		
			if( this->isBombState( WEAPON_BOMBSTATE_ATTACHOBJECT) && (m_pAttachObject != nullptr) )
			{
				throwInfo._nAddStateFlag	|= NET_GRENADE_STATE_TAKEOBJECT;
				throwInfo._nBoundType		= m_pAttachObject->getNetIndex();
			}

			GAMEEVENT::Write_ThrowingWeapon( m_pWeaponBase->getNetworkIdx(), &throwInfo );
		}

		if( bExplosion && !this->isBombState( WEAPON_BOMBSTATE_EXPLOSION))
			Explosion();
	}

}



void MainWeapon_MedicalKit::_TakeObject( I3_PHYSIX_HIT_RESULT * pResult )
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

void MainWeapon_MedicalKit::_TakeObject( i3Object * pObject)
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
	}
	else
	{
		// 애니메이션 싱크를 맞추지 않는다면 오브젝트의 로컬좌표로 붙여줍니다.
		i3Matrix::Inverse( &mtxInv, 0, pObject->GetMatrix());
		i3Matrix::Mul( &m_AttachLocalMatrix, m_pWeaponBase->GetMatrix(), &mtxInv);
	}

	addBombState( WEAPON_BOMBSTATE_ATTACHOBJECT);
}


void MainWeapon_MedicalKit::Explosion(void)
{
	g_pEffectManager->AddEffect( m_pWeaponBase->getFireEffectIndex(), 
		m_pWeaponBase->GetPos(), &I3_YAXIS);
	g_pWeaponManager->PushThrowWeapon(m_pWeaponBase);
} 


bool MainWeapon_MedicalKit::RecoveryHP(CGameCharaBase * pChara)
{
	// 체력이 이미 최대치 이거나 0일때 사용이 불가능
	if(pChara->getFullHP() == pChara->getCurHP() ||	 pChara->getCurHP() == 0)
		return false;


	// revision 33359
	// 거리 체크는 2가지의 case를 체크합니다.
	// 1. 바닥에 떨어진 오브젝트
	//	- 무기와 캐릭터간의 순수 거리로 체크
	// 2. 벽에 붙은 오브젝트
	//	- 무기와 캐릭터의 x,z 좌표 거리와 무기의 y값이 캐릭터의 높이 값 범위 Check


	VEC3D	vDir;
//	REAL32	rLength;

	// 첫번째 Check
	i3Vector::Sub( &vDir, m_pWeaponBase->GetPos(), pChara->GetPos());
	REAL32 rLength = i3Vector::LengthSq( &vDir );

	// 두번째 Check	
	vDir.y = 0.0f;
	REAL32 rLength2 = i3Vector::LengthSq( &vDir );
	bool bPickofWall = false;


	{
		// 캐릭터 최대(Head Y Value), 최소(Original Y Value) 높이를 구함 
		REAL32 rCharMinHeight = i3Vector::GetY( pChara->GetPos());
		REAL32 rCharMaxHeight = i3Vector::GetY( i3Matrix::GetPos( pChara->getBoneContext()->getBoneMatrix(BONE_HEAD)));
		
		// 메디컬킷의 높이
		REAL32 rObjHeight = i3Vector::GetY(m_pWeaponBase->GetPos());
	
		// 메디컬킷의 Y값이 캐릭터의 높이값 범위내 없으면 획득하지 못함
		if( rObjHeight <= rCharMaxHeight && rObjHeight >= rCharMinHeight)
			bPickofWall = true;
	}

//	i3Vector::Sub( &vDir, m_pWeaponBase->GetPos(), pChara->GetPos());
//	vDir.y = 0.0f;
//	rLength = i3Vector::LengthSq( &vDir );

	REAL32 rRange = m_pWeaponBase->getWeaponInfo()->GetExplosion_Range();

// revision 33359
	// 2가지 거리 체크중 1개의 조건이라도 만족하면 획득
	if( (rLength < rRange) || 
		((rLength2 < rRange) && (bPickofWall == true)))
	{	
		// 데디케이션 서버 일때 셋팅된 아이템 획득 시 2번 정보가 가는것을 막기위한 코드
		// 던진사람이 아니면 네트워크로 전송하지 않는다.
		if(BattleServerContext::i()->IsUseDedicationMode())
		{
			if(m_pWeaponBase->getOwner()->is3PV())
			{
				return false;
			}
		}

		REAL32 rDamage = m_pWeaponBase->GetDamage(false);
		//REAL32 rEffectiveTime = m_pWeaponBase->getWeaponInfo()->GetEffectiveTime(); 

		i3Vector::Sub( &vDir, pChara->GetPos(), m_pWeaponBase->GetPos());

		CHARA_DAMAGEINFO DamageInfo;

		// revision 46408 MedicalKit 계열 넘버값이 MedicalKit으로만 되던 문제 수정 (hansoft.3145)
		m_pWeaponBase->getWeaponInfo()->GetDamageInfo( &DamageInfo);
		DamageInfo.SetWeaponItemID(m_pWeaponBase);

		DamageInfo._DeathType			= CHARA_DEATH_BUFF;
		DamageInfo._rDamage				= rDamage;
		DamageInfo._pWallHitResult		= nullptr;
		DamageInfo._IsOwner				= pChara->isLocal();

		DamageInfo._WallShotReduceRate	= 0;
		DamageInfo._HitType				= GHT_CHARA;
		DamageInfo.SetHitPart( CHARA_HIT_ROOT );
		DamageInfo._nVictimIdx			= pChara->getCharaNetIndex();
		DamageInfo._nKillerIdx			= m_pWeaponBase->getOwner()->getCharaNetIndex();

		DamageInfo._pVecDir				= &vDir;
		DamageInfo._pVecPos				= m_pWeaponBase->GetPos();
		i3Vector::Copy( &DamageInfo._vStartPos,m_pWeaponBase->GetPos());
		DamageInfo._nNetidx				= m_pWeaponBase->getNetworkIdx();
		DamageInfo._AttackType			= m_pWeaponBase->getWeaponInfo()->GetAttackType(m_pWeaponBase->getFireOrder(), m_pWeaponBase->getAutoFireCount() );

		return pChara->RecoveryHP(&DamageInfo);
	}

	return false;
}

