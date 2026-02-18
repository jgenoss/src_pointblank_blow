#include "pch.h"
#include "WeaponDualKnife.h"
#include "GameCharaBase.h"
#include "GameCharaAnimContext.h"
#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"
#include "WeaponKnife.h"
#include "WeaponFunction.h"
#include "WeaponItemID.h"
#include "MainWeapon_DualKnife.h"


I3_CLASS_INSTANCE( WeaponDualKnife);//, WeaponDual);

WeaponDualKnife::WeaponDualKnife()
{

}

WeaponDualKnife::~WeaponDualKnife()
{
}


MainWeapon*	WeaponDualKnife::CreateMainWeapon()
{
	MainWeapon_DualKnife * p = new MainWeapon_DualKnife(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_DualKnife));
	return p;
}

/*virtual*/void WeaponDualKnife::_InitCollideeGroup( void)
{
	WF::SetCollideeGroup_NoUseCollidee(this);
}

/*virtual*/void WeaponDualKnife::OnBindResource( void)
{
	WeaponBase::OnBindResource();
}

/*virtual*/void WeaponDualKnife::ReturnToReady( void)
{
	WeaponBase::ReturnToReady();
}

/*virtual*/bool WeaponDualKnife::Fire( WEAPON::FIRE_ORDER type )
{
	m_FireOrder = type;

	// 시점이 향하고 있는 목표 지점을 구한다.
	//	Head와 IK 중간 지점에 시작
	NET_Fire( nullptr, nullptr, m_FireOrder );

	addWeaponStateMask( WSM_WAIT_AUTOFIREDELAY | WSM_NEED_SEND_FIREACTION | WSM_FIRE_DISABLE | WSM_FIRING );

	setAutoFireCount( 0);
	
	return true;
}

/*virtual*/void WeaponDualKnife::FireEnd( void)
{
}

/*virtual*/ void WeaponDualKnife::FireCollision( VEC3D * pStartPos, VEC3D * pTargetPos )
{	
	INT32 collGroup;

	if( getOwner() == nullptr )	return;
	
	collGroup = CGRP_MASKGROUP_ATTACK;
	if( getOwner()->getCharaTeam() & CHARACTER_TEAM_RED)
		collGroup = CGRP_MASKGROUP_CHARA_BLUE;
	else
		collGroup = CGRP_MASKGROUP_CHARA_RED;
	
	if( pStartPos == nullptr )
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
		i3Vector::Copy( m_StartPos, pStartPos );
	}

	if( getOwner() != nullptr && getOwner()->isLocal())
		GetShotTargetPos( m_TargetPos, m_StartPos);
	else
		i3Vector::Copy( m_TargetPos, pTargetPos );

	// Network Random Sync
	NET_FireSync( m_TargetPos, getFireOrder() );
	
	//	가운데 충돌 체크(첫번째)
	{
		i3CollideeLine col;
		col.SetStart( m_StartPos);
		col.SetEnd( m_TargetPos );
		
		DoFireCollision( &col, collGroup);
	}
	return ;
}

bool WeaponDualKnife::DoFireCollision( i3CollideeLine * pCol, INT32 idxGroup)
{
	bool	bWarningOwner = false;

	if( _DoFireCollisionKnife( pCol, idxGroup, &bWarningOwner) == nullptr)
	{
		// 아무데도 충돌하지 않았다.
		return false;
	}

	return true;
}

//virtual
/* void WeaponDualKnife::OnPlayChangeAniImpl( REAL32 rTimeScale // = 1.f) 
{
	if( g_pFramework->IsBattleStage() )
	{
		WF::PlayWeaponAnim(this, m_aiiChangeRight, rTimeScale );

		if( m_pLeftHandWeapon)
			m_pLeftHandWeapon->OnPlayChangeAni( rTimeScale);
	}
}
*/


void WeaponDualKnife::NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type)
{
	MainWeapon_DualKnife* mainWeapon = static_cast<MainWeapon_DualKnife*>(GetMainWeapon());
	mainWeapon->NET_Fire_DualKnife(pStartPos, pTargetPos, type);
}


void WeaponDualKnife::PlayAnim_UI_IdleB( void)
{
	//if( m_pOwner != nullptr)
	//{
	//	switch( m_pOwner->getCharaInfo()->GetResType())
	//	{
	//	case CHARARES_REBEL_MALE				:
	//	case CHARARES_D_FOX						:
	//		{
	//			_PlayWeaponAnim( m_aiiUI_RebelMale_Right );
	//			((WeaponKnife*)m_pLeftHandWeapon)->PlayAnim_UI_RebelMale_Left();			
	//		}
	//		break;
	//	case CHARARES_REBEL_FEMALE				:
	//	case CHARARES_BELLA						:
	//		{
	//			_PlayWeaponAnim( m_aiiUI_RebelFemale_Right );
	//			((WeaponKnife*)m_pLeftHandWeapon)->PlayAnim_UI_RebelFemale_Left();
	//		}
	//		break;
	//	case CHARARES_REBEL_VIPERRED			:		// 그래픽 팀에 요청에 의해 바이퍼레드만 예외처리
	//		{
	//			_PlayWeaponAnim( m_aiiUI_RebelFemale_Right2 );
	//			((WeaponKnife*)m_pLeftHandWeapon)->PlayAnim_UI_RebelFemale_Left2();
	//		}
	//		break;
	//	}
	//}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//				animation callback function

/*virtual*/ void WeaponDualKnife::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	if( getOwner() != nullptr && getOwner()->isLocal())
	{
		addWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
		removeWeaponStateMask( WSM_FIRE_DISABLE);
	}
}

/*virtual*/ void WeaponDualKnife::OnFire( i3AIContext * pCtx, REAL32 tm)
{
	if( m_pOwner == nullptr)
	{
		ReturnToReady();
		return;
	}

	setWeaponState( WEAPON::STATE_FIRE);

	INT32 nCollisionCount = 1;
	REAL32 fAttackTiming = 0.f;
	REAL32 fAttackDelay = m_pOwner->getAnimContext()->getUpperDuration();

	INT32 autoFireCount = getAutoFireCount();

	//	공격 방식에 따라 충돌시간이 변한다.	
	switch( m_FireOrder)
	{
	case WEAPON::PRIMARY_FIRE:
		{
			nCollisionCount = m_pWeaponInfo->GetAutoFireCount();
			fAttackTiming = m_pWeaponInfo->GetAttackTimeA( autoFireCount);
		}
		break;
	case WEAPON::SECONDARY_FIRE:
		{
			nCollisionCount = m_pWeaponInfo->GetAutoFireCount2();
			fAttackTiming = m_pWeaponInfo->GetAttackTimeB( autoFireCount);
		}
		break;
	default:	I3PRINTLOG(I3LOG_FATAL,  "AttackType is invalid.");
	}

	// 오른쪽 클릭 공격 사운드는 시작시 텀이 있다.
	if( (m_pWeaponInfo->GetTypeClass() == WEAPON_CLASS_KNUCKLE) &&
		(m_pWeaponInfo->GetRealNumber() == WEAPON::getItemIndex(WEAPON::BRASS_KNUCKLE)) &&
		(m_FireOrder == WEAPON::SECONDARY_FIRE))
	{
		if( !isWeaponStateMask( WSM_PLAYED_FIRE_SOUND) && pCtx->getLocalTime() + tm > fAttackTiming - 0.4f )
		{
			_OnPlayFireSound();
			addWeaponStateMask( WSM_PLAYED_FIRE_SOUND);
		}
	}
	
	//	공격 시작부터 타격 시간이 되면 조준점까지 한번 컬리전 한다.
	if( ( autoFireCount < nCollisionCount) && pCtx->getLocalTime() > fAttackTiming )
	{
		if ( m_pOwner->isLocal())
			FireCollision(nullptr, nullptr);

		autoFireCount++;
		//I3TRACE( "-- Knife Attacked!!\n");
	}

	setAutoFireCount( autoFireCount);

	// 공격 지연시간이 충분히 지났다면...Ready 상태로
	if( pCtx->getLocalTime() + tm > fAttackDelay )
	{
		ReturnToReady();
	}
}

/*virtual*/void WeaponDualKnife::OnSecondaryFire( i3AIContext * pCtx, REAL32 tm)
{
	
}

/*virtual*/ void WeaponDualKnife::OnSwaping( i3AIContext *pCtx, REAL32 tm)
{
	WeaponBase::OnSwaping( pCtx, tm);
}

void WeaponDualKnife::OnUI_ShowWeapon( i3AIContext * pCtx, REAL32 tm)
{
	REAL32 localTime = pCtx->getLocalTime();
	
	i3AIState * pAIState = pCtx->getCurrentAIState();
	I3ASSERT( pAIState != nullptr);

	if( localTime + tm > pAIState->getDuration())
	{
		_OnPlayWeaponIdleAnim();
		setWeaponState( WEAPON::STATE_READY);
	}
}



