#include "pch.h"
#include "GlobalVariables.h"
#include "WeaponGunWithKnife.h"
#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaActionContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaBoneContext.h"
#include "GameCharaMoveContext.h"
#include "GameCharaWeaponContext.h"
#include "WeaponFunction.h"

I3_CLASS_INSTANCE( WeaponGunWithKnife);//, WeaponBase);

WeaponGunWithKnife::WeaponGunWithKnife()
{

	
}

void WeaponGunWithKnife::_OnInitVairable( void)
{
	WeaponBase::_OnInitVairable();

	m_KnifeMaterialList.clear();
}

WeaponGunWithKnife::~WeaponGunWithKnife()
{
	
}

void WeaponGunWithKnife::PostCreate( void)
{
	m_KnifeMaterialList.clear();

	// 각 LOD에서 해당 Material을 찾습니다. 장착되는 Extension에 대해 Visible을 판별하기 위해서입니다.
	i3Body * pBody = getSceneObject()->GetBody();
	I3ASSERT( pBody != nullptr);

	i3::rc_wstring wsCharaAI;

	getWeaponInfo()->GetLinkedToCharaAI(wsCharaAI);

	i3::rc_wstring wsNodeName;
	i3::rc_wstring wsNodeName_Body;
	i3::string     sNodeName;
	i3::string     sNodeName_Body;
	
	wsNodeName		= wsCharaAI + L"_Knife";
	wsNodeName_Body = wsCharaAI + L"_Knife_body";

	i3::utf16_to_mb(wsNodeName, sNodeName);
	i3::utf16_to_mb(wsNodeName_Body, sNodeName_Body);
	
	for( UINT32 i = 0; i < pBody->getLODCount(); i++ )
	{
		i3LOD * pLOD = pBody->getLOD( i );

				i3Scene::FindNodeByName( pLOD->getShapeNode(), sNodeName.c_str(), m_KnifeMaterialList );
		i3Scene::FindNodeByName( pLOD->getShapeNode(), sNodeName_Body.c_str(), m_KnifeMaterialList);
		i3Scene::FindNodeByName( pLOD->getShapeNode(), "M_7AniDummy", m_KnifeMaterialList);
	}

	if( m_KnifeMaterialList.size() > 0 )
	{
		OnSetVisibleExtension( false);
	}
	
	
}

void WeaponGunWithKnife::Reset( void)
{
	WeaponBase::Reset();

}

void WeaponGunWithKnife::OnSetVisibleExtension( bool bVal)
{
	WeaponBase::OnSetVisibleExtension( bVal);

	// Extension 장비
	if( m_KnifeMaterialList.empty() == true )
		return;

	if( bVal)
	{	
		
		for( size_t i = 0; i < m_KnifeMaterialList.size(); i++ )
		{
			i3AttrSet * pAttrSet = (i3AttrSet*) m_KnifeMaterialList[i];
			pAttrSet->RemoveFlag( I3_NODEFLAG_DISABLE );
		}
	}
	else
	{
		for( size_t i = 0; i < m_KnifeMaterialList.size(); i++ )
		{
			i3AttrSet * pAttrSet = (i3AttrSet*) m_KnifeMaterialList[i];
			pAttrSet->AddFlag( I3_NODEFLAG_DISABLE );
		}
	}
}
void WeaponGunWithKnife::SwapExtensionType(void)
{
	WeaponBase::SwapExtensionType();		//	WEAPON::EXT_TYPE 전환
}
void WeaponGunWithKnife::FireEnd( void)
{
	removeWeaponStateMask( WSM_FIRING | WSM_WAIT_AUTOFIREDELAY | WSM_FIRE_DISABLE);

	if( getFireOrder() == WEAPON::PRIMARY_FIRE)
	{
		setAutoFireCount( 0);
	}
}

bool WeaponGunWithKnife::Fire( WEAPON::FIRE_ORDER type )
{
	if( !isExtensionActivate() )
	{
		if( getLoadedBulletCount() <= 0)
			return false;

		m_FireOrder = type;

		// 시점이 향하고 있는 목표 지점을 구한다.
		GetShotTargetPos( m_TargetPos);

		NET_Fire( nullptr, m_TargetPos, m_FireOrder );
	}
	else
	{
		if( (type == WEAPON::PRIMARY_FIRE) && (getLoadedBulletCount() <= 0))
			return false;

		m_FireOrder = type;

		if( m_FireOrder == WEAPON::SECONDARY_FIRE)
		{
			addWeaponStateMask( WSM_NEED_SEND_FIREACTION);
		}

		// 시점이 향하고 있는 목표 지점을 구한다.
		GetShotTargetPos( m_TargetPos);

		NET_Fire( nullptr, m_TargetPos, m_FireOrder );

		return true;
	
	}

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////
//		Animation Callback
void WeaponGunWithKnife::OnIdle( i3AIContext * pCtx, REAL32 tm)
{	
	
	bool bLoaded = true;

	if( getOwner() == nullptr )
		return;

	SetCurrentAttachHand();

	// 로컬 캐릭터의 무기가 아니라면 들어오면 안됩니다.
	if( !getOwner()->isLocal() && !getOwner()->isAI() )
		return;

	// 자동 재장전
	if( isExtensionActivate() )
	{
		{
			setWeaponState( WEAPON::STATE_READY);
			removeWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE | WSM_EXT_DISABLE);
			// 캐릭터의 익스텐션을 직접 호출하기 때문에 캐릭터의 상태를 초기화 해주어야 한다.
			m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE | CHARA_STATE_DISABLE_DROPWEAPON);
		}
	
			addWeaponStateMask( WSM_FIRE_DISABLE);	
			
	}
	{
		INT32 iBullet = getLoadedBulletCount();

		if( iBullet <= 0)
		{
			if( getTotalBulletCount() > 0)
			{
				setWeaponState( WEAPON::STATE_NEEDTO_LOADMAG);		// 자동 재장전
				removeWeaponStateMask( WSM_LOADMAG_DISABLE );
			}
			else
				setWeaponState( WEAPON::STATE_EMPTY_MAGAZINE);

			addWeaponStateMask( WSM_FIRE_DISABLE);
			bLoaded = false;
		}
		else
		{
			removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_EXT_DISABLE);

			if( iBullet < GetLoadBulletWithCashItem() || getLoadedBulletDualCount() < GetLoadBulletWithCashItem() )
				removeWeaponStateMask( WSM_LOADMAG_DISABLE);
			else
				addWeaponStateMask( WSM_LOADMAG_DISABLE);

			if( isWeaponStateMask( WSM_FIRING) == false)
			{// 발사중이 아니면
				if(getWeaponInfo()->GetPumpAction() == false)	//	펌프액션 방식이면
				{
					setWeaponState( WEAPON::STATE_READY);
					removeWeaponStateMask( WSM_FIRE_DISABLE);
				}
			}
			else
			{
				INT32 autoCnt = 0;
				INT32 autopointshot = 0;
				
				if (getOwner()->isCharaStateMask(CHARA_STATE_ZOOM) == false)
					autoCnt = m_pWeaponInfo->GetAutoFireCount();
				else
					autoCnt = m_pWeaponInfo->GetExtAutoFireCount();

				if (getOwner()->isCharaStateMask(CHARA_STATE_ZOOM) == false)
					autopointshot = m_pWeaponInfo->GetAutoPointShotEnable();
				else
					autopointshot = m_pWeaponInfo->GetExtAutoPointShotEnable();

				switch( autoCnt)
				{
				case 0 :		// 연발
					setWeaponState( WEAPON::STATE_NEEDTO_FIRE);

					removeWeaponStateMask( WSM_FIRE_DISABLE);

#if defined( FIREDELAY_COST)
					addWeaponStateMask( WSM_MORE_FIRING);

					// Idle 상태에서 발사 체크시 타이머를 계산한다.
					// 60fps에 맞춰진 오차를 계산하여 Idle 타임을 계산
					m_rFireTime = m_rFireTime - (0.016f - tm);
#endif
					break;

				case 1 :		// 단발
					setWeaponState( WEAPON::STATE_READY);

					addWeaponStateMask( WSM_FIRE_DISABLE);
					break;

				default:		// 점사

					if (autopointshot != 0)
					{
						ProcessAutoPoint(tm);
					}
					else
					{
						// 연사 중...
						if (isWeaponStateMask(WSM_WAIT_AUTOFIREDELAY))
						{
							// 연사 사이의 간격을 기다리고 있음.
							m_timeAutoFire += tm;
							addWeaponStateMask(WSM_FIRE_DISABLE);

							if (m_timeAutoFire >= (m_pWeaponInfo->GetAutoFireDelay() - tm))
							{
								m_timeAutoFire = 0.0f;
								setAutoFireCount(0);
								setWeaponState(WEAPON::STATE_NEEDTO_FIRE);
								removeWeaponStateMask(WSM_WAIT_AUTOFIREDELAY | WSM_FIRE_DISABLE);
							}
						}
						else
						{
							// 발사한다.
							if (getAutoFireCount() > autoCnt)
							{
								// 발사 개수가 점사 수만큼...
								addWeaponStateMask(WSM_WAIT_AUTOFIREDELAY | WSM_FIRE_DISABLE);

								m_timeAutoFire = 0.0f;
								setWeaponState(WEAPON::STATE_WAIT);
							}
							else
							{
								setWeaponState(WEAPON::STATE_NEEDTO_FIRE);
							}
						}
						removeWeaponStateMask(WSM_FIRE_DISABLE);
					}
					break;
				}
			}
		}
	}
}
void WeaponGunWithKnife::OnSecondaryFire( i3AIContext * pCtx, REAL32 tm)
{
	if( m_pOwner == nullptr)
	{
		ReturnToReady();
		return;
	}
	setWeaponState( WEAPON::STATE_FIRE);

	
	if( getFireOrder() == WEAPON::SECONDARY_FIRE)
	{
		addWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);		
	}

	REAL32 fAttackDelay = m_pOwner->getAnimContext()->getUpperActualDuration();

	INT32 autoFireCount = getAutoFireCount();

	if( pCtx->getLocalTime() == 0.f )
	{
		// 발사 사운드
		_OnPlayFireSound();
		autoFireCount = 0;
	}

	//	공격 시작부터 타격 시간이 되면 조준점까지 한번 컬리전 한다.
	if( autoFireCount == 0 && pCtx->getLocalTime() > m_pWeaponInfo->GetExtAttackTime() )
	{
		if( m_pOwner->isLocal())
			MeleeCollision( nullptr, nullptr );

		autoFireCount++;
	}

	setAutoFireCount( autoFireCount);
	// 공격 지연시간이 충분히 지났다면...Ready 상태로
	if( pCtx->getLocalTime() + tm > fAttackDelay )
	{
		
		if( getFireOrder() == WEAPON::SECONDARY_FIRE)
		{
			removeWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
		}

		getSceneObject()->RemoveFlag(I3_NODEFLAG_DISABLE);
		OnSetVisibleExtension(true);
		_OnPlayWeaponIdleAnim();

		setWeaponState(WEAPON::STATE_READY);
		removeWeaponStateMask(WSM_PLAYED_FIRE_SOUND | 0xFF);

		m_timeAutoFire = 0.0f;
		setAutoFireCount(0);
		setRemainAutoFireCount(0);
		removeWeaponStateMask(WSM_WAIT_AUTOFIREDELAY | WSM_FIRE_DISABLE | WSM_POINTSHOTING);

		if (getOwner() != nullptr)
			getOwner()->getWeaponContext()->setAttachHand(CHARA_WEAPON_GRIP_RIGHT_HAND);	// revision 45937 - 무기 교체 시 무기 오른손에 잡도록 수정
		ChangeGunDummy(WEAPON_ATTACH_GRIP);

		m_FireOrder = WEAPON::PRIMARY_FIRE;
	}
}


REAL32 WeaponGunWithKnife::GetRange()
{
	if( getFireOrder() == WEAPON::PRIMARY_FIRE)
		return WeaponBase::GetRange();

	return  m_pWeaponInfo->GetExtRange();
}

REAL32 WeaponGunWithKnife::GetDPD_Rate()	
{	
	if( getFireOrder() == WEAPON::PRIMARY_FIRE)
		return WeaponBase::GetDPD_Rate();

	return  m_pWeaponInfo->GetExt_DPD_Rate();
}
void WeaponGunWithKnife::_OnAddTerrainEffect( WEAPON_TERRAIN_EFFECT_INFO * pInfo)
{
	I3ASSERT( pInfo->_pPos != nullptr);
	I3ASSERT( pInfo->_pNormal != nullptr);
	I3ASSERT( pInfo->_pDir != nullptr);

	WeaponBase::_OnAddTerrainEffect( pInfo);
	// revision 60217
}
void WeaponGunWithKnife::_OnPlayTerrainHitSound( WEAPON::FIRE_ORDER fireOrder, UINT32 type, VEC3D * pPos)
{
	if( fireOrder == WEAPON::PRIMARY_FIRE)
		WeaponBase::_OnPlayTerrainHitSound( fireOrder, type, pPos);
	else
	{	// melee attack hit sound
		g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, GTBT_WPN_MELEE_HIT, pPos, false);
	}
}


REAL32 WeaponGunWithKnife::GetDamage( bool bRandomDamage)
{
	if( getFireOrder() == WEAPON::PRIMARY_FIRE)
		return WeaponBase::GetDamage( bRandomDamage);

	return m_pWeaponInfo->GetExtDamage();
}

/*virtual*/ REAL32 WeaponGunWithKnife::GetPhysixForce( WEAPON_CLASS_TYPE classType )
{
	if( getFireOrder() == WEAPON::PRIMARY_FIRE)
		return WeaponBase::GetPhysixForce( classType);

	return WeaponBase::GetPhysixForce( WEAPON_CLASS_KNIFE);
}

void WeaponGunWithKnife::MeleeCollision(  VEC3D * pStartPos, VEC3D * pTargetPos)
{
	INT32 collGroup;

	if( m_pOwner == nullptr )		return;
	
	collGroup = CGRP_MASKGROUP_ATTACK;
	if( m_pOwner->getCharaTeam() & CHARACTER_TEAM_RED)	collGroup = CGRP_MASKGROUP_CHARA_BLUE;
	else												collGroup = CGRP_MASKGROUP_CHARA_RED;

	m_FireOrder = WEAPON::SECONDARY_FIRE;
	
	if( pStartPos == nullptr)
	{
		if( m_pOwner->is1PV())
		{
			// 1인칭에서는 현재는 Camera의 지점
			m_pOwner->getCameraContext()->getVirtualCamPosition( m_StartPos );
		}
		else
		{
			//	3인칭에서는 캐릭터 좌표(정확하게는 캐릭터의 IK Bone)의 위치를 시작지점으로 카메라 방향으로
			// 스타트 포지션
			i3Vector::Sub( m_StartPos, i3Matrix::GetPos( m_pOwner->getBoneContext()->getHeadMatrix()), i3Matrix::GetPos( m_pOwner->getBoneContext()->getIKMatrix()));
			i3Vector::Scale( m_StartPos, m_StartPos, 0.5f);
			i3Vector::Sub( m_StartPos, i3Matrix::GetPos( m_pOwner->getBoneContext()->getHeadMatrix()), m_StartPos);
		}
	}
	else
	{
		i3Vector::Copy( m_StartPos, pStartPos);
	}

	if( m_pOwner->isLocal() || m_pOwner->isAI() )
	{
		GetShotTargetPos( m_TargetPos, m_StartPos);
	}
	else
	{
		i3Vector::Copy( m_TargetPos, pTargetPos );
	}

	NET_FireSync( m_TargetPos, getFireOrder() );
	
	//	collision line
	{	
		i3CollideeLine col;

		col.SetStart( m_StartPos);
		col.SetEnd( m_TargetPos );

		bool	bWarningOwner = false;

		_DoFireCollisionKnife( &col, collGroup, &bWarningOwner);
		
	}
}

void WeaponGunWithKnife::OnExtensionActivate( i3AIContext * pCtx, REAL32 tm)
{
	if( m_pOwner == nullptr ) return;

	i3AIState * pAI;
	if(m_pOwner->is1PV())
	{
		pAI = pCtx->getAIState( getAIIndex( WEAPON::AI_EXTENSION_ACTIVATE));
	}
	else
	{
		pAI = pCtx->getAIState( getAIIndex( WEAPON::AI_EXTENSION_ACTIVATE3PV));
	}
	
	addWeaponStateMask( WSM_ACTIVATE_EXTENSION);

	//	동작이 종료되면
	if( pCtx->getLocalTime() + tm >= pAI->getDuration() )
	{
		removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
	
		
	
		_OnPlayWeaponIdleAnim();
	}
	else
	{
		if( m_pOwner->isLocal())
		{
			addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
		}

	}

	OnSetVisibleExtension( true);
}

/*virtual*/void WeaponGunWithKnife::OnPlayExtensionAnimation( bool bVal, bool bAnimPlay)
{	
	if( bAnimPlay )
	{
		if( bVal)
		{
			if (m_pOwner->is1PV())
				WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_EXTENSION_ACTIVATE));
			else
				WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_EXTENSION_ACTIVATE3PV));
		}
	
		else
		{
			if (m_pOwner->is1PV())
				WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_EXTENSION_DEACTIVATE));
			else
				WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_EXTENSION_DEACTIVATE3PV));

		}
	}
	else
	{
		_OnPlayWeaponIdleAnim();
		OnSetVisibleExtension( bVal);
	}
}


void WeaponGunWithKnife::OnExtensionDeactivate( i3AIContext * pCtx, REAL32 tm)
{

	i3AIState * pAI;
	if(m_pOwner->is1PV())
	{
		pAI = pCtx->getAIState( getAIIndex( WEAPON::AI_EXTENSION_DEACTIVATE));
	}
	else
	{
		pAI = pCtx->getAIState( getAIIndex( WEAPON::AI_EXTENSION_DEACTIVATE3PV));
	}
		
	removeWeaponStateMask( WSM_ACTIVATE_EXTENSION);

	//	동작이 종료되면
	if( pCtx->getLocalTime() + tm >= pAI->getDuration() )
	{
		removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);

		_OnPlayWeaponIdleAnim();

		OnSetVisibleExtension( false);
	}
	else
	{
		if( getOwner() != nullptr && getOwner()->isLocal())
			addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);

	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//		Animation Callback


void WeaponGunWithKnife::GetTargetDir( VEC3D * pDir)
{
	VEC3D vDir;
				
	REAL32 r, t;
	VEC3D vXAxis, vYAxis;
	
	i3Vector::Sub( &vDir, m_TargetPos, m_StartPos);
	i3Vector::Normalize( &vDir, &vDir);

	i3Vector::Cross( &vXAxis, &vDir, &I3_YAXIS);
	i3Vector::Normalize( &vXAxis, &vXAxis);

	i3Vector::Cross( &vYAxis, &vDir, &vXAxis);
	i3Vector::Normalize( &vYAxis, &vYAxis);

	REAL32 deviation = m_pOwner->getBoneContext()->getFireDeviation();

	r = i3Math::Randf2();
	
	r *= deviation * 0.5f * 0.5f;
	t = i3Math::Randf2() * deviation * 0.5f * 0.5f;

	MATRIX tmp;
	
	i3Matrix::SetRotateAxisDeg( &tmp, &vXAxis, r);
	i3Matrix::PostRotateAxisDeg( &tmp, &vYAxis, t);

	i3Vector::TransformNormal( pDir, &vDir, &tmp);
}

