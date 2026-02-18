#include "pch.h"
#include "WeaponKnife.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "BattleHud.h"
#include "GameCharaActionContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaWeaponContext.h"
#include "WeaponItemID.h"
#include "MainWeapon_Knife.h"
#include "WeaponFunction.h"
#include "WeaponInfo/MeleeWeaponInfo.h"

#define WEAPON_KNIFE_SEC_ATTACK_TIME	0.31f

I3_CLASS_INSTANCE( WeaponKnife);//, WeaponBase);

WeaponKnife::WeaponKnife( void)
{

}

WeaponKnife::~WeaponKnife( void)
{
	if((getOwner() != nullptr) && (getOwner()->isLocal()))
	{
		// 1인칭용 칼로 나오고 있었다면 궤적에 대한 관리를 해야함.
//		g_pEdgeTrail->Reset();
	}
}


MainWeapon*	WeaponKnife::CreateMainWeapon() 
{
	MainWeapon_Knife * p = new MainWeapon_Knife(this);	
	MEMDUMP_NEW( p, sizeof( MainWeapon_Knife));
	return p;
}

void WeaponKnife::_InitCollideeGroup( void)
{
	WF::SetCollideeGroup_NoUseCollidee(this);
}

void WeaponKnife::Reset( void)
{
	WeaponBase::Reset();	// revision 54702 기존에 생성 된 Knife 재 사용 시 Reset (hansoft.4544)
	ReturnToReady();
}

void WeaponKnife::ReturnToReady( void)
{
	WeaponBase::ReturnToReady();

	m_StateMaskWeapon	= WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE;

	setAutoFireCount( 0);

}

void WeaponKnife::OnPlayChangeAniImpl( REAL32 rTimeScale /*= 1.f*/)
{
	// 임시 구현...(나중에 재 교체 해야할것이다..)
	// 가상함수 동작이 아닌 구체 함수 호출을 위해 범위연산 지정..
	((MainWeapon_Knife*)m_mainWeapon.get())->MainWeapon_Knife::OnPlayChangeAni(rTimeScale);
	//

/*
	if( (m_pPairWeapon != nullptr) && (m_aiiChangeLeft != -1) )
	{// paired weapon이면 왼손잡이용 애니메이션을 한다.
		if( g_pFramework->IsBattleStage() )
			_PlayWeaponAnim( m_aiiChangeLeft , rTimeScale );
	}
	else
	{
		WeaponBase::OnPlayChangeAniImpl( rTimeScale);
	}
*/
}

bool WeaponKnife::Fire( WEAPON::FIRE_ORDER type)
{
	m_FireOrder = type;

	// 시점이 향하고 있는 목표 지점을 구한다.

	//	Head와 IK 중간 지점에 시작
	NET_Fire( nullptr, nullptr, m_FireOrder );

	addWeaponStateMask( WSM_WAIT_AUTOFIREDELAY | WSM_NEED_SEND_FIREACTION);

	setAutoFireCount( 0);

	return true;
}

void WeaponKnife::NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type )
{	//	현재 Local, 네크워크 상관없이 TargetPos를 설정합니다. 네트워크일 경우 컬리전 시점을 구현해야 합니다.

	setWeaponState( WEAPON::STATE_FIRE);		//	공격!!
	m_FireOrder = type;

	setAutoFireCount( 0);

	if( m_pPairWeapon == nullptr)
	{	// 일반적인 나이프 입니다.
		WeaponBase::NET_Fire( nullptr, nullptr, type );
	}
	else
	{
		// KEYWORD : WEAPON_CIC
		// CIC용 나이프는 호출해줘야 합니다.
		//	CIC 총기 추가되면 반드시 인덱스 추가 필요합니다.
		if( m_pPairWeapon->getWeaponInfo()->isGun() 
			|| i3::kind_of<WeaponMiniAxe*>(m_pPairWeapon) )	// CIC는 왼손이 밀리 오른손이 반드시 피스톨류이다
			WeaponBase::NET_Fire( nullptr, nullptr, type );
	}

	if( pStartPos != nullptr )
	{
		i3Vector::Copy( m_StartPos, pStartPos );
		m_bSetStart	= true;
	}
	else
	{
		m_bSetStart	= false;
	}
	
	if( pTargetPos != nullptr )
		i3Vector::Copy( m_TargetPos, pTargetPos);

	if( getOwner() != nullptr && (getOwner()->isLocal() || getOwner()->isAI()) )
		addWeaponStateMask( WSM_FIRING | WSM_FIRE_DISABLE);
	removeWeaponStateMask( WSM_WAIT_AUTOFIREDELAY);

	if( ((CMeleeWeaponInfo*)m_pWeaponInfo)->isDelayedFireSound() == false)
	{
		_OnPlayFireSound();
	}

	MainWeapon_Knife* mainWeapon = static_cast<MainWeapon_Knife*>(m_mainWeapon.get());

	if( type == WEAPON::PRIMARY_FIRE)
	{
		// KEYWORD : WEAPON_DUALKNIFE
		if( (m_pPairWeapon != nullptr) && (mainWeapon->GetAttackA_Left() != -1)
			&& (i3::kind_of<WeaponMiniAxe*>(m_pPairWeapon) == false) )
			WF::PlayWeaponAnim(this, mainWeapon->GetAttackA_Left() );
	}
	else if( type == WEAPON::SECONDARY_FIRE)
	{
		// KEYWORD : WEAPON_DUALKNIFE
		if( (m_pPairWeapon != nullptr) && (mainWeapon->GetAttackB_Left() != -1)
			&& (i3::kind_of<WeaponMiniAxe*>(m_pPairWeapon) == false) )
			WF::PlayWeaponAnim(this, mainWeapon->GetAttackB_Left() );
		else
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_SECFIRE));
	}
}

void WeaponKnife::FireEnd(void)
{	
}

void WeaponKnife::FireCollision( VEC3D * pStartPos, VEC3D * pTargetPos )
{	
	INT32 collGroup;

	if( getOwner() == nullptr )		return;
	
	collGroup = CGRP_MASKGROUP_ATTACK;
	if( getOwner()->getCharaTeam() & CHARACTER_TEAM_RED)	collGroup = CGRP_MASKGROUP_CHARA_BLUE;
	else													collGroup = CGRP_MASKGROUP_CHARA_RED;
		
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

	if( getOwner() != nullptr && (getOwner()->isLocal() || getOwner()->isAI()))
	{
		GetShotTargetPos( m_TargetPos, m_StartPos);
	}
	else
	{
		i3Vector::Copy( m_TargetPos, pTargetPos );
	}

	// Network Random Sync
	// KEYWORD : WEAPON_CIC
	if( i3::same_of<WeaponDualCIC*>(getPairWeapon()) )
	{	// CIC의 나이프 공격이면 나이프 데이터가 아닌 CIC정보를 네트워크로 보냅니다.
		getPairWeapon()->NET_FireSync( m_TargetPos, WEAPON::SECONDARY_FIRE);
	}
	else
	{
		NET_FireSync( m_TargetPos, getFireOrder() );
	}
	
	/*GlobalFunc::PB_TRACE("m_StartPos %f %f %f m_TargetPos %f %f %f GetPos %f %f %f", m_StartPos.x, m_StartPos.y, m_StartPos.z, m_TargetPos.x, m_TargetPos.y, m_TargetPos.z,
												m_pOwner->GetPos()->x, m_pOwner->GetPos()->y, m_pOwner->GetPos()->z);*/
	//	collision line
	{	
		i3CollideeLine col;

		col.SetStart( m_StartPos);
		col.SetEnd( m_TargetPos );
		
		DoFireCollision( &col, collGroup);
	}
}

bool WeaponKnife::DoFireCollision( i3CollideeLine * pCol, INT32 idxGroup)
{
	bool	bWarningOwner = false;

	I3_PHYSIX_HIT_RESULT * pLastHitResult;

	pLastHitResult = _DoFireCollisionKnife( pCol, idxGroup, &bWarningOwner);
	if( pLastHitResult == nullptr )
	{
		// 아무데도 충돌하지 않았다.
		return false;
	}

	if (m_pWeaponInfo->GetNumber() == WEAPON::getItemIndex( WEAPON::CANDYCANE))
	{
		MainWeapon_Knife* mainWeapon = static_cast<MainWeapon_Knife*>(m_mainWeapon.get());

		// revision 47817 캔디케인 이펙트 터지는 위치 무기 -> 타격 지점으로 수정 - hansoft.3362
		MATRIX mtx;
		
		i3Matrix::SetUp(&mtx, &pLastHitResult->m_Normal);
		i3Matrix::SetPos(&mtx, &pLastHitResult->m_Intersect);

		_FireEffect( &mtx, mainWeapon->GetCandyEffectIdx(), true);
	}

	I3ASSERT( m_pOwner != nullptr);

	//GlobalFunc::PB_TRACE("DoFireCollision");

	if( pLastHitResult->m_pDestShapeSet != nullptr)
	{
		if( (m_FireOrder == WEAPON::PRIMARY_FIRE) &&
			((CMeleeWeaponInfo*)m_pWeaponInfo)->UseHitedAnimation() )
		{
			m_pOwner->Cmd_Attack( WEAPON::HITED_FIRE);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void WeaponKnife::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	setAutoFireCount( 0);

	if( getOwner() != nullptr && (getOwner()->isLocal() || getOwner()->isAI()))
	{
		addWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
		removeWeaponStateMask( WSM_FIRE_DISABLE);
	}
}

void WeaponKnife::OnFire( i3AIContext * pCtx, REAL32 tm)
{
	if( getOwner() != nullptr && (getOwner()->isLocal() || getOwner()->isAI()))
	{
		addWeaponStateMask( WSM_FIRE_DISABLE | WSM_FIRING | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
	}

	REAL32 fAttackTiming = 0.f;

	if( m_pPairWeapon != nullptr)
	{
		// KEYWORD : WEAPON_DUALKNIFE
		// 듀얼 나이프의 왼쪽 나이프는 WeaponDualKnife에서 컬리젼을 합니다.
		if( i3::same_of<WeaponDualKnife*>(m_pPairWeapon))
		{
			if( pCtx->getLocalTime() + tm > pCtx->getCurrentAIState()->getActualDuration())
				ReturnToReady();

			return;
		}
	}

	{
		//	공격 방식에 따라 충돌시간이 변한다.
		MainWeapon_Knife* mainWeapon = static_cast<MainWeapon_Knife*>(m_mainWeapon.get());

		WEAPON_KNIFE_ATTACK_TYPE attack_type = mainWeapon->GetKnifeAttackType();

		switch( attack_type)
		{
		case WEAPON_KNIFE_ATTACK:		fAttackTiming = m_pWeaponInfo->GetAttackTimeA(0);		break;
		case WEAPON_KNIFE_ATTACK2:		fAttackTiming = m_pWeaponInfo->GetAttackTimeA(1);		break;	
		default:	I3PRINTLOG(I3LOG_FATAL,  "AttackType is invalid.");
		}
	}
	
	if( ((CMeleeWeaponInfo*)m_pWeaponInfo)->isDelayedFireSound())
	{
		if( !isWeaponStateMask( WSM_PLAYED_FIRE_SOUND) && pCtx->getLocalTime() + tm > fAttackTiming - 0.1f )
		{
			_OnPlayFireSound();
			addWeaponStateMask( WSM_PLAYED_FIRE_SOUND);
		}
	}

	INT32 autoFireCount = getAutoFireCount();

	//	공격 시작부터 타격 시간이 되면 조준점까지 한번 컬리전 한다.
	if( autoFireCount < m_pWeaponInfo->GetAutoFireCount() && pCtx->getLocalTime() > fAttackTiming)
	{
		if( getOwner() != nullptr && (getOwner()->isLocal() || getOwner()->isAI()))
		{
			FireCollision( nullptr, nullptr );
		}
		autoFireCount++;
	}
	
	
	setAutoFireCount( autoFireCount);

	// 공격 지연시간이 충분히 지났다면...Ready 상태로
	//if( pCtx->getLocalTime() + tm > (m_pWeaponInfo->GetFireDelay()))
	REAL32 rDelay = pCtx->getLocalTime() + tm;
	if( rDelay > GetFireDelay())
	{
		_OnProcessFireEndAnim( rDelay);
	}
}

void WeaponKnife::OnSecondaryFire( i3AIContext * pCtx, REAL32 tm)
{
	INT32 autoFireCount = getAutoFireCount();
	REAL32 fAttackTiming = m_pWeaponInfo->GetAttackTimeB( autoFireCount);

	if( ((CMeleeWeaponInfo*)m_pWeaponInfo)->isDelayedFireSound() )
	{
		if( !isWeaponStateMask(WSM_PLAYED_FIRE_SOUND) && pCtx->getLocalTime() + tm > fAttackTiming - 0.2f )
		{
			_OnPlayFireSound();
			addWeaponStateMask( WSM_PLAYED_FIRE_SOUND);
		}
	}

	//	공격 시작부터 타격 시간이 되면 조준점까지 한번 컬리전 한다.
	if( autoFireCount < m_pWeaponInfo->GetAutoFireCount2() && (pCtx->getLocalTime() + tm > fAttackTiming) )
	{
		if( getOwner() != nullptr && (getOwner()->isLocal() || getOwner()->isAI()))// 네트워크는 패킷으로 처리합니다.
		{
			FireCollision( nullptr, nullptr );
		}

		autoFireCount++;
	}

	setAutoFireCount( autoFireCount);

	// 공격 지연시간이 충분히 지났다면...Ready 상태로	
	if( pCtx->getLocalTime() >= (GetFireDelay2() - tm))
	{
		ReturnToReady();
	}
}

void WeaponKnife::OnEndFire( i3AIContext * pCtx, REAL32 tm)
{	//	애니메이션이 없기 때문에 수동으로 호출해줘야 합니다. 캐릭터에서 Fire 종료시 자동으로 호출합니다.
	if( getWeaponState() == WEAPON::STATE_FIRE)
		return;

	ReturnToReady();
}

void WeaponKnife::OnUI_ShowWeapon( i3AIContext * pCtx, REAL32 tm)
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


void WeaponKnife::OnDrop()
{
	 I3ASSERT_0;
}

void WeaponKnife::PlayAnim_UI_IdleA( void)
{
	if( m_pOwner != nullptr)
	{
		((MainWeapon_Knife*)m_mainWeapon.get())->PlayAnim_UI_Swat_Common();
	}
}

void WeaponKnife::PlayAnim_UI_IdleB( void)
{
	if( m_pOwner != nullptr)
	{
		((MainWeapon_Knife*)m_mainWeapon.get())->PlayAnim_UI_Swat_Common();
	}
}


void WeaponKnife::_OnProcessFireEndAnim( REAL32 rDelay)
{	
	if( (m_pOwner != nullptr) && (m_pOwner->getAnimContext()->getAIUpper() != nullptr) )
	{ 
		i3AIState * pAIState = m_pOwner->getAnimContext()->getAIUpper()->getCurrentAIState();
		if( pAIState != nullptr )
		{
			if( rDelay > pAIState->getActualDuration() )
				ReturnToReady();
			else
				setWeaponState( WEAPON::STATE_FIRING);

			return;
		}
	}
	
	ReturnToReady();
}
