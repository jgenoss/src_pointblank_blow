#include "pch.h"
#include "MainWeapon_ThrowKnife.h"
#include "BattleObjContext.h"
#include "BattleSlotContext.h"
#include "GameCharaCameraContext.h"
#include "Weapon/WeaponItemID.h"
#include "Weapon/WeaponInfo/MeleeWeaponInfo.h"

#include "LocaleValue.h"

MainWeapon_ThrowKnife::MainWeapon_ThrowKnife(WeaponBase* p) : MainWeapon_Throw(p) 
{
	m_pAttachObjectTransform	= nullptr;
	m_pAttachObject				= nullptr;
	m_rBoundObjectLen			= 0.f;
	m_bSettedChecker			= false;
	m_nStateFlag				= -1;

	i3Vector::Zero(&m_vObjectForce);
}

void	MainWeapon_ThrowKnife::Reset()
{
	MainWeapon_Throw::Reset();

	
	i3Vector::Zero( &m_vRightDir);

	m_pAttachObjectTransform	= nullptr;
	m_pAttachObject				= nullptr;
	m_rBoundObjectLen			= 0.f;
	m_bSettedChecker			= false;

	i3Matrix::Identity( m_pWeaponBase->GetMatrix());

	m_nStateFlag = -1;
	m_vThrowKnifeStateFlag.clear();

	if (m_pWeaponBase->getNetworkIdx() != -1)
	{
		GAMEINFO_THROW_WEAPON * pThrowInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( m_pWeaponBase->getNetworkIdx() );
		pThrowInfo->_tNetData.resetPrevPos();
	}

	i3Vector::Zero(&m_vObjectForce);
}

// revision 36102 미니 액스 오브젝트 데미지 안들어가던 문제 수정
void MainWeapon_ThrowKnife::_ProcessHitWorld( I3_PHYSIX_HIT_RESULT * pResult, i3CollideeLine* pLine)
{
	I3ASSERT( pResult != nullptr);

	// Normal 방향대로 무기를 박아버립니다.
	{
		VEC3D vUp;
		VEC3D vRight;
		VEC3D vAt;

		// Up 벡터 구하기
		REAL32 rRotate = static_cast<CMeleeWeaponInfo*>(m_pWeaponBase->getWeaponInfo())->GetRotateFront();
		i3Vector::Set(&vUp, getX(&pResult->m_Normal), getY(&pResult->m_Normal), getZ(&pResult->m_Normal));
		// 보정해야 합니다.
		if (vUp.y + I3_EPS >= 1.0f || vUp.y - I3_EPS <= -1.0f)
		{
			// Up 벡터 다시 계산
			if (rRotate == 1.f)
			{
				i3Vector::Set(&vUp, 0.0f, vUp.y, 0.f);

				i3Vector::Cross(&vRight, &m_ThrowData.m_vDirection, &I3_YAXIS);
			}
			else
			{
				i3Vector::Set(&vUp, 0.0f, -vUp.y, 0.f);

				i3Vector::Cross(&vRight, &I3_YAXIS, &m_ThrowData.m_vDirection);
			}

			i3Vector::Cross(&vAt, &vUp, &vRight);
		}
		else
		{
			if (rRotate == 1.f)
			{
				// Right 벡터 구하기
				i3Vector::Cross(&vRight, &vUp, &I3_YAXIS);

				// At 벡터 구하기
				i3Vector::Cross(&vAt, &vUp, &vRight);
			}
			else
			{
				// 방향을 바꾸고 벡터의 순서를 바꾸어 주어야 정상적으로 보입니다.
				vUp *= rRotate;

				// Right 벡터 구하기
				i3Vector::Cross(&vRight, &I3_YAXIS, &vUp);

				// At 벡터 구하기
				i3Vector::Cross(&vAt, &vUp, &vRight);
			}
		}
		
		// 세팅합니다.
		i3Vector::Normalize(&vUp, &vUp);
		i3Vector::Normalize(&vRight, &vRight);
		i3Vector::Normalize(&vAt, &vAt);

		i3Matrix::SetUp(m_pWeaponBase->GetMatrix(),		&vUp);
		i3Matrix::SetRight(m_pWeaponBase->GetMatrix(),  &vRight);
		i3Matrix::SetAt(m_pWeaponBase->GetMatrix(),		&vAt);

		/*if (rRotate != 1.f)
		{
			i3Matrix::PostRotateXDeg(m_pWeaponBase->GetMatrix(), 180.f);
			i3Matrix::PostRotateZDeg(m_pWeaponBase->GetMatrix(), 180.f);
		}*/
	}

	/*VEC3D vNormal;
	REAL32 rDotX = i3Vector::Dot( &pResult->m_Normal, &I3_YAXIS);
	REAL32 rRotate = static_cast<CMeleeWeaponInfo*>(m_pWeaponBase->getWeaponInfo())->GetRotateFront();

	i3Matrix::SetRotateZ( m_pWeaponBase->GetMatrix(), I3_PI);
	i3Matrix::PostRotateX( m_pWeaponBase->GetMatrix(),(rRotate * (1.f - rDotX)) * I3_PI2);
	
	i3Vector::Set( &vNormal, getX( &pResult->m_Normal), 0.f, getZ( &pResult->m_Normal));
	i3Vector::Normalize( &vNormal, &vNormal);
	REAL32 fTheta = i3Math::atan2( -getX( &vNormal), -getZ( &vNormal));
	i3Matrix::PostRotateY( m_pWeaponBase->GetMatrix(), fTheta);
	
	{
		VEC3D vDir;
		REAL32 fFactor = i3Vector::Dot( &pResult->m_Normal, &I3_YAXIS);
		REAL32 fRotateRadVal = 0.f;
		i3Vector::Set( &vDir, getX( &m_ThrowData.m_vDirection), 0.f, getZ( &m_ThrowData.m_vDirection));
		i3Vector::Normalize( &vDir, &vDir);
		REAL32 fTheta2 = i3Math::atan2( getX( &vDir), getZ( &vDir));

		fRotateRadVal = (fTheta2 - I3_PI) * fFactor ;
		i3Matrix::PostRotateY( m_pWeaponBase->GetMatrix(), -fRotateRadVal);
	}*/
	
	VEC3D vRealPos;
	REAL32 rDistance = static_cast<CMeleeWeaponInfo*>(m_pWeaponBase->getWeaponInfo())->GetWallBetweenDistance();
	i3Vector::Scale( &vRealPos, &pResult->m_Normal, rDistance);
	i3Vector::Add( &vRealPos, &vRealPos, &pResult->m_Intersect);
	i3Matrix::SetPos( m_pWeaponBase->GetMatrix(), &vRealPos);

	if( pResult->m_pTerrain != nullptr)
	{
		g_pEffectManager->AddBlowEffect( (I3_TERRAIN_TYPE) pResult->m_pTerrain->m_Terrain, WEAPON_CLASS_KNIFE,
			static_cast<BULLET_EFFECT_TYPE>(m_pWeaponBase->getWeaponInfo()->GetLoadBulletType()),
			&pResult->m_Intersect, &pResult->m_Normal);

		m_pWeaponBase->_AddTerrainEffectSound( (I3_TERRAIN_TYPE) pResult->m_pTerrain->m_Terrain, 0.01f, &pResult->m_Intersect);
	}

	if( OBJECT_COLLISION_BY_WEAPON_HIT( pResult->m_pDestShapeSet->getShapeGroup()) )
	{
		VEC3D	vDir;
		i3Vector::Sub( &vDir, &pResult->m_Intersect, pLine->GetStart() );
		REAL32	rLength = i3Vector::Normalize( &vDir, &vDir );
		m_pWeaponBase->_CalcHitObject( pResult, pLine, &vDir, rLength, true);

		if (static_cast<CMeleeWeaponInfo*>(m_pWeaponBase->getWeaponInfo())->IsEnableBounce() == false)
		{
			addBombState( WEAPON_BOMBSTATE_EXPLOSION);
			m_ThrowData.m_rSpeed = 0.f;
		}
	}
}

bool MainWeapon_ThrowKnife::SuccessiveStateFlagCheck(INT32 nState)
{
	if (false == m_vThrowKnifeStateFlag.empty())
	{
		UINT32 nSize = m_vThrowKnifeStateFlag.size();

		//폭팔, 정지 상태 이후엔 이하의 상황이 들어올수 없다.
		if (nState == NET_GRENADE_STATE_FLY || nState == NET_GRENADE_STATE_BOUND)
		{
			//바로앞 상태를 찾는다.
			UINT8 usPrevState = (UINT8)m_vThrowKnifeStateFlag[nSize - 1];
			if (usPrevState == NET_GRENADE_STATE_EXPLOSION || usPrevState == NET_GRENADE_STATE_SETTLED)
			{
				//패킷을 보내지 않는다.
				return false;
			}
		}
	}

	m_vThrowKnifeStateFlag.push_back(nState);
	return true;
}

void MainWeapon_ThrowKnife::ReturnInstance()
{
	
	m_ThrowData.m_BombState		= 0xcdcdcdcd;

	m_pAttachObjectTransform	= nullptr;
	m_pAttachObject				= nullptr;

	MainWeapon_Throw::ReturnInstance();
}

void MainWeapon_ThrowKnife::ThrowArrow( VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, bool bOrigin, INT32 nNetSlotIdx, WeaponThrowKnife * pNewWeapon)
{
	////////////////////////////////////////////////////////////////////////////////////////
	//	던지는 원본 (화면에 날라가는 개체가 아니다. 캐릭터가 아직 들고 있을 경우 인스턴스로 생성해서 월드에 던집니다.)
	if( bOrigin)
	{
		if( pNewWeapon == nullptr )
		{
			// 인스턴스가 없습니다..?
			//I3PRINTLOG(I3LOG_FATAL,  "no instance resource." );
			return;
		}
		
		MainWeapon_ThrowKnife* mainWeapon_new = static_cast<MainWeapon_ThrowKnife*>(pNewWeapon->GetMainWeapon());

		pNewWeapon->SetOwner( m_pWeaponBase->getOwner());	// 먼저 세팅해야됩니다.	
		pNewWeapon->Reset();
		mainWeapon_new->Throw( vStart, vDir, rSpeed, nNetSlotIdx);
		pNewWeapon->SetFireOrder( WEAPON::PRIMARY_FIRE);

		VEC3D vDirection;
		i3Vector::Set( &vDirection, getX( mainWeapon_new->getDirection()), 0.f, getZ( mainWeapon_new->getDirection()));
		i3Vector::Normalize( &vDirection, &vDirection);
		REAL32 rRot = i3Vector::Dot( &vDirection, &I3_ZAXIS);

		i3Matrix::SetRotateX( pNewWeapon->GetMatrix(), I3_PI);

		if( getX( &vDirection) >= 0.f)
			i3Matrix::SetRotateY( pNewWeapon->GetMatrix(), (1.f - rRot) * I3_PI2 + I3_PI);
		else
			i3Matrix::SetRotateY( pNewWeapon->GetMatrix(), -(1.f - rRot) * I3_PI2 + I3_PI);

		i3Vector::Cross( mainWeapon_new->getRightVec(), &I3_YAXIS, &vDirection);
		i3Vector::Normalize( mainWeapon_new->getRightVec(), mainWeapon_new->getRightVec());
		
		// Network ID
		pNewWeapon->setNetworkIdx( (INT16)nNetSlotIdx);
		if( nNetSlotIdx == -1)
		{
			const GAMEINFO_CHARACTER * pSendBuffer = BattleSlotContext::i()->getGameInfo_Chara( m_pWeaponBase->getOwner()->getCharaNetIndex());
			pNewWeapon->setNetworkIdx( pSendBuffer->_tNetData._tFireData.getNetSlotIndex());
		}
		else if( nNetSlotIdx < MAX_THROWWEAPON_COUNT)
		{
			GAMEINFO_THROW_WEAPON * pThrowInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( nNetSlotIdx );
			pThrowInfo->_tNetData.SetDir( *( mainWeapon_new->getDirection() ) );
		}

		VEC3D	vTemp;
		m_pWeaponBase->getOwner()->getCameraContext()->getVirtualCamPosition( &vTemp );
		GAMEEVENT::BindNetGrenadeInfo( pNewWeapon->getNetworkIdx(), pNewWeapon, &vTemp );
	}
	////////////////////////////////////////////////////////////////////////////////////////
	//	던져지는 개체 (인스턴스로 생성되어 날아가는 녀석이다.)
	else
	{
		Throw( vStart, vDir, rSpeed, nNetSlotIdx);
	}


}


void MainWeapon_ThrowKnife::Throw( VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, INT32 nNetSlotIdx)
{
	addBombState( WEAPON_BOMBSTATE_THROWING);

	m_ThrowData.m_rElapsedThrowTime		= 0.f;

	#if !defined( NO_GRENADE_TRACE_EFFECT)
	m_ThrowData.m_rTracerUpdateTime		= 0.f;
	#endif

	m_ThrowData.m_rSpeed				= rSpeed;

	i3Vector::Copy( &m_ThrowData.m_vStartPos, vStart);
	i3Vector::Copy( &m_ThrowData.m_vDirection, vDir);
	i3Vector::Copy( &m_ThrowData.m_vOldPos, vStart);
	i3Vector::Copy( &m_ThrowData.m_vTracerOldPos, vStart);
}


void MainWeapon_ThrowKnife::UpdateFlying( REAL32 rDeltaSeconds, REAL32 rExplosionRange)
{
	if( m_ThrowData.m_rSpeed < 0.15f )
		return;
	
	if( rDeltaSeconds > 0.5f)rDeltaSeconds = 0.016f;
	
	if( m_pWeaponBase->IsDedicatedHost() )
	{
		this->CheckWorldCollision( rDeltaSeconds);
	}
	else
	{	// 네트워크 처리입니다.
		this->UpdateNetworkData( rDeltaSeconds);
	}

#if !defined( NO_GRENADE_TRACE_EFFECT)

	//GlobalFunc::PB_TRACE("UpdateFlying isVisible() %d m_rElapsedThrowTime %f m_rTracerUpdateTime %f %d m_BombState %x", isVisible(), m_rElapsedThrowTime, m_rTracerUpdateTime, !i3Vector::IsSame( GetPos(), &m_vTracerOldPos), m_BombState);

	//	수류탄 궤적(꼬리)을 만든다.
	if( m_pWeaponBase->isVisible() && (m_ThrowData.m_rElapsedThrowTime > 0.1f && m_ThrowData.m_rTracerUpdateTime <= 0.f) &&
		!i3Vector::IsSame( m_pWeaponBase->GetPos(), &m_ThrowData.m_vTracerOldPos) && !m_bSettedChecker )
	{	
		REAL32 len;
		VEC3D vDir;
		COLOR	color;

		CWeaponGlobal * pGlobalInfo = g_pWeaponInfoDataBase->getGlobal();

		i3Vector::Sub( &vDir, m_pWeaponBase->GetPos(), &m_ThrowData.m_vTracerOldPos); 
		len = i3Vector::Length( &vDir); 
		i3Vector::Normalize( &vDir, &vDir);

		//GlobalFunc::PB_TRACE("UpdateFlying vDir %f %f %f m_rElapsedThrowTime %f m_rTracerUpdateTime %f m_BombState %x", vDir.x, vDir.y, vDir.z, m_rElapsedThrowTime, m_rTracerUpdateTime, m_BombState);

		
		//	궤적 두께
		REAL32 fThickness = pGlobalInfo->m_rTracerHandGrenadeThickness;
		REAL32 fLifeTime = pGlobalInfo->m_rTracerHandGrenadePieceLifeTime;

		//	다음 궤적 시간 (작을수록 tracer를 많이 만든다.)
		m_ThrowData.m_rTracerUpdateTime = pGlobalInfo->m_rTracerHandGrenadePieceCreateTime; 
		
		//	궤적 색상
		i3Color::Copy(&color, &pGlobalInfo->m_TracerHandGrenadeColor);
		

		//	궤적 발사
	//	if( m_rElapsedExplosionTime < rExplosionRange - 0.1f)	//	폭발하기 직전에 궤적은 불필요하므로 끊는다.
		{
			TracerBulletInfo tracerInfo;
			tracerInfo._Type = TRACER_TYPE_HAND_GRENADE;
			tracerInfo._LifeTime = fLifeTime;
			i3Vector::Copy( &tracerInfo._vBeginPos, &m_ThrowData.m_vTracerOldPos);
			i3Vector::Copy( &tracerInfo._vDestPos, m_pWeaponBase->GetPos());
			tracerInfo._Length = len;
			tracerInfo._Thickness = fThickness;
			tracerInfo._EndSpeed = 0;
			i3Color::Copy( &tracerInfo._color, &color);

			g_pTracerBulletManager->Fire( &tracerInfo);
		}

		i3Vector::Copy( &m_ThrowData.m_vTracerOldPos, m_pWeaponBase->GetPos());

		//I3TRACE( "Grenade Tracer : Update Time %.4f, Length : %.4f, Speed : %.4f\n", m_rTracerUpdateTime, len, speed);
	}
	
	m_ThrowData.m_rTracerUpdateTime -= rDeltaSeconds;
#endif	
}



void MainWeapon_ThrowKnife::CheckWorldCollision( REAL32 rDeltaSeconds)
{
	VEC3D	vTemp;
	REAL32	rLength;
	i3CollideeLine ColLine;
	I3_PHYSIX_HIT_RESULT * pResult;

	VEC3D	vNew;

	i3Vector::Scale( &vNew, &m_ThrowData.m_vDirection, m_ThrowData.m_rSpeed * m_ThrowData.m_rElapsedThrowTime);
	i3Vector::Add( &vNew, &m_ThrowData.m_vStartPos, &vNew);
	addY( &vNew, -9.81f * m_pWeaponBase->getWeaponInfo()->GetWeight() * m_ThrowData.m_rElapsedThrowTime * m_ThrowData.m_rElapsedThrowTime);

	ColLine.SetStart( &m_ThrowData.m_vOldPos);
	i3Vector::Sub( &vTemp,  &vNew, &m_ThrowData.m_vOldPos);
	rLength = i3Vector::Normalize( &vTemp, &vTemp);

	// 이 위치가 내가 날리는 방향
	REAL32 rTheta = i3Math::atan2( getX( &vTemp), getZ( &vTemp));
	REAL32 rRho = i3Math::acos( getY( &vTemp) );
	i3Matrix::SetRotateY( m_pWeaponBase->GetMatrix(), rTheta);
	i3Matrix::PreRotateX( m_pWeaponBase->GetMatrix(), rRho);
	i3Matrix::SetPos( m_pWeaponBase->GetMatrix(), &vNew);

	if( rLength <= 0.0f )
	{	// 이동 속도가 0
		if (static_cast<CMeleeWeaponInfo*>(m_pWeaponBase->getWeaponInfo())->IsEnableBounce())
		{
			_ProcessBoundObjectForce();					
		}
		return;
	}

	ColLine.SetEnd( m_pWeaponBase->GetPos());
	ColLine.SetDir( &vTemp);

	pResult = g_pFramework->raycastClosest( &ColLine, CGRP_MASKGROUP_ATTACK );
	
	if( (pResult != nullptr) )
	{
		g_pFramework->reserveRaycastResult( 1);
		
		if(static_cast<CMeleeWeaponInfo*>(m_pWeaponBase->getWeaponInfo())->IsEnableBounce() == false && rLength > pResult->m_T)
		{
			rLength = pResult->m_T;
			_ProcessHitWorld( pResult, &ColLine);
			m_bSettedChecker = true;
			m_ThrowData.m_rSpeed = 0.f;
		}
		// 헤어드라이기, 프라이팬 다중 공격을 방지 하기 위하여 // 2014.06.12 ENG_김대영
		else if(static_cast<CMeleeWeaponInfo*>(m_pWeaponBase->getWeaponInfo())->IsEnableBounce() == true && rLength > pResult->m_T )
		{
			rLength = pResult->m_T;
			_ProcessHitWorld( pResult, &ColLine);
			m_bSettedChecker = true;
			addBombState(WEAPON_BOMBSTATE_HITBOUND);
		}
	}

	// 헤어드라이기, 프라이팬 다중 공격을 방지 하기 위하여 // 2014.06.12 ENG_김대영
	// WEAPON_BOMBSTATE_HITBOUND 상태를 가지고 있을 경우 캐릭터와의 충돌 체크를 하지 않게 수정하였습니다.
	
	I3_PHYSIX_HIT_RESULT * pCharaResult = nullptr;

	if((this->isBombState( WEAPON_BOMBSTATE_HITBOUND)) == false)
	{
		UINT32 nColGroup = CGRP_MASKGROUP_CHARA_BLUE;
		if( m_pWeaponBase->getOwner()->getCharaTeam() & CHARACTER_TEAM_BLUE)
			nColGroup = CGRP_MASKGROUP_CHARA_RED;

		pCharaResult = g_pFramework->raycastClosest( &ColLine, nColGroup );
	}

	// 2014.06.12 ENG_김대영

	if( pCharaResult != nullptr)
	{
		i3EventReceiver * pRecv = pCharaResult->m_pDestShapeSet->getEventReceiver();
		if( i3::kind_of<CGameCharaBase*>(pRecv))
		{
			if( !((CGameCharaBase *)pRecv)->isCharaStateMask( CHARA_STATE_DEATH))
			{
				if( rLength > pCharaResult->m_T)
				{
					m_pWeaponBase->_CalcHitChara( pCharaResult, &ColLine, ColLine.GetDir(), rLength);

					if (static_cast<CMeleeWeaponInfo*>(m_pWeaponBase->getWeaponInfo())->IsEnableBounce() == false)
					{
						m_ThrowData.m_rSpeed = 0.f;
						m_nStateFlag = NET_GRENADE_STATE_EXPLOSION;
					}
					else
					{
						addBombState(WEAPON_BOMBSTATE_HITBOUND); // 헤어드라이기, 프라이팬 다중 공격을 방지 하기 위하여 // 2014.06.12 ENG_김대영
					}
					m_bSettedChecker = true;
				}
			}
		}
	}

	if (pResult && static_cast<CMeleeWeaponInfo*>(m_pWeaponBase->getWeaponInfo())->IsEnableBounce())
	{
		VEC3D	vecNew, vecNewPos, vecNormal;
		REAL32	rDot1 = 0.0f;
		INT32	nBoundType = 0;

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

		//2014. 06. 11 ENG_김대영
		//[탈취모드] 기획팀 요청으로 헤어드라이기와 프라잉팬만 맨처음 바운드 속도 조절 
		//( 코드상 35이 기본이나 수류탄 기본속도인 18.5로 하드코딩)
		WEAPON::RES_ID weaponNum = m_pWeaponBase->getWeaponInfo()->GetNumber();
		if ( (weaponNum == WEAPON::getItemIndex(WEAPON::HAIRDRYER_DUMMY) 
			|| weaponNum == WEAPON::getItemIndex(WEAPON::FRYINGPAN_DUMMY))
			 && m_pWeaponBase->getWeaponInfo()->GetThrowSpeed() == Speed ) 
		{
			Speed = 18.5f;
		}
		//2014. 06. 11 ENG_김대영
			

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

		// 타 클라이언트에서도 튕긴 탄환(더미)가 보일 수 있게 Throw()에서 NET_Bound()로 변경 // 2014.6.13 ENG_김대영
		NET_Bound( m_pWeaponBase->GetPos(), &vecNew, Speed, nBoundType );
	
	}

	g_pFramework->resetRaycastResult();
}

REAL32 MainWeapon_ThrowKnife::_BoundObject( I3_PHYSIX_HIT_RESULT * pHitResult, VEC3D * pDir)
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

void MainWeapon_ThrowKnife::_ProcessBoundObjectForce()
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

void MainWeapon_ThrowKnife::UpdateNetworkData( REAL32 rDeltatime)
{
	REAL32	flerp;
	VEC3D	vDir;

	//I3ASSERT( m_pWeaponBase->IsDedicatedHost() == false);

	if(  m_pWeaponBase->getNetworkIdx() < 0 || m_pWeaponBase->getNetworkIdx() >= MAX_THROWWEAPON_COUNT)		return;

	GAMEINFO_THROW_WEAPON * pThrowInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( m_pWeaponBase->getNetworkIdx() );
	GAMEINFO_THROW_WEAPON_ASSIST * pAssistInfo = &pThrowInfo->_tAssistData;
	GAMEINFO_THROW_WEAPON_NETDATA * pNetInfo = &pThrowInfo->_tNetData;

	VEC3D vNetPos = pNetInfo->GetPos();

	if( !isBombState( WEAPON_BOMBSTATE_SETTLED) && !isBombState(WEAPON_BOMBSTATE_EXPLOSION) )
	{
		pAssistInfo->_fInterpolateTime += rDeltatime;
		flerp	= pAssistInfo->_fInterpolateTime;

		REAL32 rInterpolTime = pThrowInfo->_fBroadcastCheckSyncTime + pThrowInfo->_fBroadcastCheckSyncTime * 0.2f;

		if( flerp > rInterpolTime)
			flerp = rInterpolTime;

		flerp /= rInterpolTime;

		i3Vector::Sub(&vDir, &vNetPos, &pAssistInfo->_vPostPos);
		i3Vector::Normalize(&vDir, &vDir);

		REAL32 rTheta = i3Math::atan2( getX( &vDir), getZ( &vDir));
		REAL32 rRho = i3Math::acos( getY( &vDir) );
		i3Matrix::SetRotateY( m_pWeaponBase->GetMatrix(), rTheta);
		i3Matrix::PreRotateX( m_pWeaponBase->GetMatrix(), rRho);

		i3Vector::Lerp( m_pWeaponBase->GetPos(), &pAssistInfo->_vPostPos, &vNetPos, flerp );
	}

	if( pThrowInfo->_bRecv )
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
		case NET_GRENADE_STATE_C5_SETTLED:
		case NET_GRENADE_STATE_SLEEP:
			break;
		case NET_GRENADE_STATE_SETTLED:
			{
				if( !isBombState( WEAPON_BOMBSTATE_SETTLED ) )
				{
					addBombState( WEAPON_BOMBSTATE_SETTLED);

					// 네트워크로는 월드에 대한 정보가 없습니다.
					// Raycast를 하여 월드에 대한 정보를 얻어옵니다.
					i3CollideeLine ColLine;

					ColLine.SetStart( &vNetPos);

					i3Vector::Sub( &vDir, &vNetPos, &pAssistInfo->_vPostPos);
					ColLine.SetDir( &vDir);

					I3_PHYSIX_HIT_RESULT * pResult = g_pFramework->raycastClosest( &ColLine, CGRP_MASKGROUP_ATTACK);
					if( pResult != nullptr)
					{
						_ProcessHitWorld( pResult, &ColLine);
						m_pWeaponBase->SetVisible( true );
					}
				}
				else
				{
					m_bSettedChecker = true;
				}

				m_pWeaponBase->SetPos(&vNetPos);
			}
			break;
		case NET_GRENADE_STATE_TAKEOBJECT:
		case NET_GRENADE_STATE_CLICKFIRE:
			break;
		case NET_GRENADE_STATE_EXPLOSION:
			{
				g_pWeaponManager->PushThrowKnife( m_pWeaponBase);
			}
			break;
		}

		pThrowInfo->_bRecv = false;
	}
}

void		 MainWeapon_ThrowKnife::OnUpdate_ThrowKnife( REAL32 rDeltaSeconds)
{
	if( isThrowing())
	{
		// 유저가 있는지 체크해서 없으면 반환시켜줍니다.
		if( _CheckProcessUserLeave() == false)
			return;

		m_nStateFlag = NET_GRENADE_STATE_FLY;

		if( m_pWeaponBase->IsDedicatedHost())
		{
			if( m_ThrowData.m_rSpeed < 0.2f && !isBombState( WEAPON_BOMBSTATE_SETTLED) )
			{
				addBombState( WEAPON_BOMBSTATE_SETTLED);
				m_nStateFlag = NET_GRENADE_STATE_SETTLED;
			}

			if( isBombState( WEAPON_BOMBSTATE_EXPLOSION) )
			{
				g_pWeaponManager->PushThrowKnife( m_pWeaponBase);
				m_nStateFlag = NET_GRENADE_STATE_EXPLOSION;
				return;
			}
		}

		REAL32 rExplosionRange = m_pWeaponBase->GetExplosionTime();

		UpdateFlying( rDeltaSeconds, rExplosionRange );

		m_pWeaponBase->_TransformWorldSpace( rDeltaSeconds);

		m_pWeaponBase->ProcessSound( rDeltaSeconds);

		i3Vector::Copy( &m_ThrowData.m_vOldPos, m_pWeaponBase->GetPos());

		if( m_pWeaponBase->getNetworkIdx() != -1)
		{
			m_ThrowData.m_rElapsedThrowTime += rDeltaSeconds;

			REAL32 hideTime = ((CMeleeWeaponInfo*)m_pWeaponBase->getWeaponInfo())->GetHideTime();
			if( m_ThrowData.m_rElapsedThrowTime > 5.0f ||
				(isBombState( WEAPON_BOMBSTATE_SETTLED) && (m_ThrowData.m_rElapsedThrowTime > hideTime) ) )
			{
				if(m_pWeaponBase->IsDedicatedHost())
				{
					m_nStateFlag = NET_GRENADE_STATE_EXPLOSION;
				}
				else
				{
					GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon( m_pWeaponBase->getNetworkIdx() );
					pInfo->Reset();
					g_pWeaponManager->PushThrowKnife(m_pWeaponBase); 
				}
			}

			if( m_pWeaponBase->IsDedicatedHost() )
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
				g_pWeaponManager->PushThrowKnife(m_pWeaponBase);
		}
	}

	
}


bool MainWeapon_ThrowKnife::_CheckProcessUserLeave( void)
{	
	bool	bOwnerInvalid = true;

	// 유저가 나간경우 체크	
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

	if( bOwnerInvalid )
	{
		g_pWeaponManager->PushThrowKnife( m_pWeaponBase);
		return false;
	}

	return true;
}

void	MainWeapon_ThrowKnife::NET_Bound( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType )
{
	//그레네이드를 따온 ThrowKnife 에는 튕기는 무기에 대해 네트워크로 재 발사하는 함수가 없었기 때문에 추가함.
	//2014-06-13 ENG_김대영

	bool bHost = m_pWeaponBase->IsDedicatedHost();

	if( bHost && m_pWeaponBase->getNetworkIdx() != -1)
	{
		// 네트워크로 보내줍니다.
		// ThrowKnife 같은 경우 NET_GRENADE_STATE_FLY 로 해야 그려집니다. (그 외에는 EXPOLISON 상태로 바로 가고 PUSH해버림)
		// 수류탄은 NET_GRENADE_STATE_BOUND로 되어 있습니다.
		GAMETOCONTEXT_INFO_WEAPON_THROW throwInfo(m_pWeaponBase->getWeaponInfo()->GetItemID(), pStartPos, NET_GRENADE_STATE_FLY);

		throwInfo._nBoundType			= nBoundType;

		if (SuccessiveStateFlagCheck(throwInfo._nAddStateFlag))
			GAMEEVENT::Write_ThrowingWeapon( m_pWeaponBase->getNetworkIdx(), &throwInfo);
	}

	if( fSpeed > 0.5f)	//	약한 바운드는 궤적이 안나온다.
	{
#if !defined( NO_GRENADE_TRACE_EFFECT)

		m_ThrowData.m_rTracerUpdateTime = 0.0f;
#endif
		//_PlayBoundSound( (I3_TERRAIN_TYPE) nBoundType ); 아직 헤어드라이기나 후라이팬은 전용 바운드 사운드가 없으므로 주석처리.
	}
	else
	{
#if !defined( NO_GRENADE_TRACE_EFFECT)
		m_ThrowData.m_rTracerUpdateTime = 0.2f;
#endif
	}

	Throw( pStartPos, pDir, fSpeed , -1 );
}
