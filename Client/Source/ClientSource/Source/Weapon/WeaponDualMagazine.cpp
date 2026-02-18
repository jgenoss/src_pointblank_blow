#include "pch.h"
#include "WeaponDualMagazine.h"
#include "GlobalVariables.h"
#include "GameCharaAnimContext.h"
#include "WeaponFunction.h"

I3_CLASS_INSTANCE( WeaponDualMagazine);//, WeaponBase);

WeaponDualMagazine::WeaponDualMagazine()
{
	m_aiiFire_Dual				= -1;
	m_aiiIdle_Dual				= -1;
	m_aiiLoadMagazine_Dual		= -1;
	m_aiiLoadMagazine_DualShift = -1;
}

WeaponDualMagazine::~WeaponDualMagazine()
{
}

void WeaponDualMagazine::_OnPlayWeaponIdleAnim( void)
{
	if( (m_DualMagazineMode == WEAPON_DUAL_MAGAZINE_RIGHT)  )
	{	// Dual magazine
		WF::PlayWeaponAnim(this, m_aiiIdle_Dual );
	}
	else
	{
		WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_IDLE) );
	}
}

void WeaponDualMagazine::_OnPlayWeaponFireAnim( void)
{
	if( (m_DualMagazineMode == WEAPON_DUAL_MAGAZINE_RIGHT) )
	{
		WF::PlayWeaponAnim(this, m_aiiFire_Dual);
	}
	else
	{
		WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_FIRE) );
	}
}

void WeaponDualMagazine::Reset( void)
{
	WeaponBase::Reset();

	INT32 bullet = GetLoadBulletWithCashItem();

	setLoadedBulletCount( bullet);
	setLoadedBulletDualCount( bullet);
	m_DualMagazineMode = WEAPON_DUAL_MAGAZINE_IDLE;

	INT32 nMaxBullet = GetMaxBulletWithCashItem();

	setTotalBulletCount( nMaxBullet - (bullet + bullet));

}

void WeaponDualMagazine::OnBindResource( void)
{
	WeaponBase::OnBindResource();

	m_aiiFire_Dual = (INT8) m_pAICtx->FindAIStateByName( "Fire_Right");
	m_aiiIdle_Dual = (INT8) m_pAICtx->FindAIStateByName( "Idle_Right");
	m_aiiLoadMagazine_Dual = (INT8) m_pAICtx->FindAIStateByName( "LoadMag_Right" );
	m_aiiLoadMagazine_DualShift = (INT8) m_pAICtx->FindAIStateByName( "LoadMag_RightShift" );

	m_DualMagazineMode = WEAPON_DUAL_MAGAZINE_IDLE;
}

bool WeaponDualMagazine::Fire( WEAPON::FIRE_ORDER type)
{
	if( m_DualMagazineMode == WEAPON_DUAL_MAGAZINE_RIGHT )
	{
		if( getLoadedBulletDualCount() <= 0 )	return false;
	}
	else
	{
		if( getLoadedBulletCount() <= 0)	return false;
	}

	m_FireOrder = type;

	// 시점이 향하고 있는 목표 지점을 구한다.
	GetShotTargetPos( m_TargetPos);

	NET_Fire( nullptr, m_TargetPos, m_FireOrder );

	return true;
}

void WeaponDualMagazine::OnPlayLoadMagAnimation( void)
{
	if( m_pAICtx == nullptr)
		return;

	if( (m_DualMagazineMode == WEAPON_DUAL_MAGAZINE_RIGHT))
	{	// 탄창이 우측에서 좌측으로 전체 교체입니다.
		
		if(EnablePlayMasterAnim())
		{
			if( m_pOwner->is1PV())
				_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_RIGHT_MASTER1PV));
			if( m_pOwner->is3PV())
				_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_RIGHT_MASTER3PV));
		}
		else
		{
			if (getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_1PV_MALE) != -1)
			{
				PlayLoadMagazineDualMaz( m_pOwner->getDBInfo()->GetCharaSexTypePHYSIX(), m_pOwner->is1PV());
			}
			else
			{
				_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL));
			}
		}
			
	}
	else
	{
		
		if(EnablePlayMasterAnim())
		{
			if( m_pOwner->is1PV())
			{
				_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER1PV));
			}
			else
			{
				_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER3PV));
			}
		}
		else
		{

			if (getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV_MALE) != -1)
			{	
				PlayLoadMagazine( m_pOwner->getDBInfo()->GetCharaSexTypePHYSIX(), m_pOwner->is1PV());
			}
			else
				_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE) );
		}
	}

	setWeaponState( WEAPON::STATE_LOADMAG);
}

void WeaponDualMagazine::OnPlayChangeAniImpl( REAL32 rTimeScale /*= 1.f*/)
{
	if( (m_DualMagazineMode == WEAPON_DUAL_MAGAZINE_RIGHT) )
		WF::PlayWeaponAnim(this, m_aiiIdle_Dual );
	else
		_OnPlayWeaponIdleAnim();
}

/*virtual*/ bool WeaponDualMagazine::OnCheckExtensionPossible( bool bVal)
{
	if( WeaponBase::OnCheckExtensionPossible( bVal))
	{
		if( GetCurrentExtensionType() == WEAPON::EXT_DUAL_MAG)
		{// 듀얼 탄창을 교체할 수 있는가?
			if( getTotalBulletCount() <= 0 && getLoadedBulletDualCount() <= 0)
				return false;
		}

		return true;
	}

	return false;
}

void WeaponDualMagazine::OnPlayExtensionAnimation( bool bVal, bool bAnimPlay)
{
	if( m_DualMagazineMode == WEAPON_DUAL_MAGAZINE_IDLE )
	{	
		if( bAnimPlay)
		{
			I3ASSERT( m_aiiLoadMagazine_Dual != -1);
			i3AIState * pAIState = m_pAICtx->getAIState( m_aiiLoadMagazine_Dual);
			I3ASSERT( pAIState != nullptr);
			WF::PlayWeaponAnim( this, m_aiiLoadMagazine_Dual, pAIState->getActualDuration() );
		}
		else
			m_DualMagazineMode = WEAPON_DUAL_MAGAZINE_RIGHT;
	}
	else if( m_DualMagazineMode == WEAPON_DUAL_MAGAZINE_RIGHT )
	{	
		if( bAnimPlay)
		{
			I3ASSERT( m_aiiLoadMagazine_DualShift != -1);
			i3AIState * pAIState = m_pAICtx->getAIState( m_aiiLoadMagazine_DualShift);
			I3ASSERT( pAIState != nullptr);
			WF::PlayWeaponAnim(this, m_aiiLoadMagazine_DualShift, pAIState->getActualDuration() );
		}
		else
			m_DualMagazineMode = WEAPON_DUAL_MAGAZINE_IDLE;
	}
}


void WeaponDualMagazine::OnIdle( i3AIContext * pCtx, REAL32 tm)
{	
	if( m_pOwner == nullptr)	return;

	bool bLoaded = true;

	INT32 iBullet = getLoadedBulletCount();
	INT32 iDualBullet = getLoadedBulletDualCount();
	INT32 iTotal = getTotalBulletCount();

	SetCurrentAttachHand();

	// 자동 재장전
	switch( m_DualMagazineMode )
	{
	case WEAPON_DUAL_MAGAZINE_NONE :
	case WEAPON_DUAL_MAGAZINE_IDLE :
		if( getLoadedBulletCount() <= 0 )
		{
			if( iTotal > 0)
			{
				setWeaponState( WEAPON::STATE_NEEDTO_LOADMAG);		// 자동 재장전
				removeWeaponStateMask( WSM_LOADMAG_DISABLE);
			}
			else if( iDualBullet > 0 )
			{	// 잔탄을 RightMagazine으로
				setTotalBulletCount( iDualBullet);
				setLoadedBulletDualCount( 0);
				setWeaponState( WEAPON::STATE_NEEDTO_LOADMAG);
				removeWeaponStateMask( WSM_LOADMAG_DISABLE);
			}
			else
				setWeaponState( WEAPON::STATE_EMPTY_MAGAZINE);

			if( m_pOwner->isLocal()) addWeaponStateMask( WSM_FIRE_DISABLE);
			bLoaded = false;
		}
		break;

	case WEAPON_DUAL_MAGAZINE_RIGHT :
		if( getLoadedBulletDualCount() <= 0)	// 현재 장전된 탄환이 없다.
		{
			if( iTotal > 0)
			{
				setWeaponState( WEAPON::STATE_NEEDTO_LOADMAG);		// 자동 재장전
				removeWeaponStateMask( WSM_LOADMAG_DISABLE);
			}
			else if( iBullet > 0 )
			{// 잔탄을 LeftMagazine으로
				setTotalBulletCount( iBullet);
				setLoadedBulletCount( 0);
				setWeaponState( WEAPON::STATE_NEEDTO_LOADMAG);
				removeWeaponStateMask( WSM_LOADMAG_DISABLE);
			}
			else
				setWeaponState( WEAPON::STATE_EMPTY_MAGAZINE);

			if( getOwner() != nullptr && getOwner()->isLocal())
				addWeaponStateMask( WSM_FIRE_DISABLE);
			bLoaded = false;
		}
		break;
	}

	if( bLoaded )
	{
		removeWeaponStateMask( WSM_FIRE_DISABLE);

		if( iBullet < GetLoadBulletWithCashItem() || iDualBullet < GetLoadBulletWithCashItem() )
			removeWeaponStateMask( WSM_LOADMAG_DISABLE);
		else if( m_pOwner->isLocal())
			addWeaponStateMask( WSM_LOADMAG_DISABLE);

		if( isWeaponStateMask( WSM_FIRING) == false)
		{	// 발사중이 아니면
			setWeaponState( WEAPON::STATE_READY);
#if defined( FIREDELAY_COST)
			removeWeaponStateMask( WSM_MORE_FIRING);
#endif
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

#if defined( FIREDELAY_COST)
				addWeaponStateMask( WSM_MORE_FIRING);

				// Idle 상태에서 발사 체크시 타이머를 계산한다.
				// 60fps에 맞춰진 오차를 계산하여 Idle 타임을 계산
				m_rFireTime = m_rFireTime - (0.016f - tm);
#endif
				break;

			case 1 :		// 단발
				setWeaponState( WEAPON::STATE_READY);
				if( m_pOwner->isLocal()) addWeaponStateMask( WSM_FIRE_DISABLE);
				break;

			default :		// 점사
				// 연사 중...
				if (autopointshot != 0)
				{
					ProcessAutoPoint(tm);
				}
				else
				{
					if (isWeaponStateMask(WSM_WAIT_AUTOFIREDELAY))
					{
						// 연사 사이의 간격을 기다리고 있음.
						m_timeAutoFire += tm;
						if (m_pOwner->isLocal()) addWeaponStateMask(WSM_FIRE_DISABLE);

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
							if (m_pOwner->isLocal()) addWeaponStateMask(WSM_WAIT_AUTOFIREDELAY | WSM_FIRE_DISABLE);
							m_timeAutoFire = 0.0f;
							setWeaponState(WEAPON::STATE_WAIT);
						}
						else
							setWeaponState(WEAPON::STATE_NEEDTO_FIRE);
					}
					removeWeaponStateMask(WSM_FIRE_DISABLE);
				}
				break;
			}				
		}
	}
}

INT32 WeaponDualMagazine::_OnProcessFireBulletCount( void)
{
	PB_VM_S

		if( m_DualMagazineMode == WEAPON_DUAL_MAGAZINE_RIGHT )
		{
			INT32 bullet = getLoadedBulletDualCount();

			bullet -= m_pWeaponInfo->GetFireDecBulletCount();

			setLoadedBulletDualCount( bullet);

			return bullet;
		}
		else
		{
			INT32 bullet = getLoadedBulletCount();

			bullet -= m_pWeaponInfo->GetFireDecBulletCount();
			setLoadedBulletCount( bullet);

			return bullet;
		}

		PB_VM_E
}
bool WeaponDualMagazine::OnLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	bool bDoneLoadMag = false;

	if( pCtx->getLocalTime() == 0.0f)
	{
		if( getOwner() != nullptr && getOwner()->isLocal())
			addWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_FIRE_DISABLE);
		setWeaponState( WEAPON::STATE_LOADMAG);

		removeWeaponStateMask( WSM_WAIT_FOR_LOADMAG | WSM_EMPTY_CARTRIDGE);

		if( GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER &&
			isExtensionActivate() )
		{
			// none
		}
		else
		{
			// KEYWORD : WEAPON_DUMMY
			if( IsLoadMagazineLeftBarrel() )
			{
				ChangeAttachHand( CHARA_WEAPON_BARREL_LEFT_HAND, WEAPON_ATTACH_BARREL);
			}
		}
	}

	if( isWeaponStateMask( WSM_WAIT_FOR_LOADMAG) == false)
	{
		REAL32 rReloadTime = 1.f;

		I3ASSERT( getAIIndex( WEAPON::AI_CURRENT) != -1);
		i3AIState * pAIState = pCtx->getAIState( getAIIndex( WEAPON::AI_CURRENT) );

		if( pAIState != nullptr)
			rReloadTime = pAIState->getActualDuration();
		
		if( pCtx->getLocalTime() > rReloadTime )
		{
			INT32 addCount;

			INT32 iBullet = getLoadedBulletCount();

			bDoneLoadMag = true;

			INT32 iTotal = getTotalBulletCount();

			iTotal += getLoadedBulletDualCount();

			setLoadedBulletDualCount( 0);

			addCount = MIN( m_pWeaponInfo->GetReloadBulletCount(), iTotal);

			if((iBullet + addCount) > GetLoadBulletWithCashItem())
				addCount -= (iBullet + addCount) - GetLoadBulletWithCashItem();

			setLoadedBulletCount( i3Math::softAdd( iBullet, addCount));

	#ifdef ENABLE_TEST_COMMAND
			if( !g_pWeaponManager->COMMAND_IsUnlimitGun())		//	Use Cheat
	#endif
			{
				iTotal = i3Math::softAdd( iTotal, -addCount);
			}

			addCount = MIN( m_pWeaponInfo->GetReloadBulletCount(), iTotal);

			if( addCount > GetLoadBulletWithCashItem() )
				addCount -= addCount - GetLoadBulletWithCashItem();

			
			setLoadedBulletDualCount( i3Math::softAdd( 0, addCount));

	#ifdef ENABLE_TEST_COMMAND
			if( !g_pWeaponManager->COMMAND_IsUnlimitGun())		//	Use Cheat
	#endif
			{
				iTotal = i3Math::softAdd( iTotal, -addCount);
			}

			setTotalBulletCount( iTotal);

			m_DualMagazineMode = WEAPON_DUAL_MAGAZINE_IDLE;

			// 바로 READY 상태로 만든다.
			setWeaponState( WEAPON::STATE_READY);
			_OnPlayWeaponIdleAnim();
			removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE);
			addWeaponStateMask( WSM_WAIT_FOR_LOADMAG);
		}
	}

	return bDoneLoadMag;
}

void WeaponDualMagazine::OnExtensionActivate( i3AIContext * pCtx, REAL32 tm)
{	
	I3ASSERT( getAIIndex( WEAPON::AI_CURRENT) != -1);
	i3AIState * pAI = pCtx->getAIState( getAIIndex( WEAPON::AI_CURRENT));

	//	동작이 종료되면
	if( pCtx->getLocalTime() + tm > pAI->getDuration() )
	{
		removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);

		if( m_DualMagazineMode == WEAPON_DUAL_MAGAZINE_IDLE )
		{
			addWeaponStateMask( WSM_ACTIVATE_EXTENSION);
			m_DualMagazineMode = WEAPON_DUAL_MAGAZINE_RIGHT;
		}
		else
		{
			removeWeaponStateMask( WSM_ACTIVATE_EXTENSION);
			m_DualMagazineMode = WEAPON_DUAL_MAGAZINE_IDLE;
		}

		_OnPlayWeaponIdleAnim();
	}
	else
	{
		if( (m_pOwner != nullptr) && m_pOwner->isLocal())
			addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);

		removeWeaponStateMask( WSM_ACTIVATE_EXTENSION);
	}
}

void WeaponDualMagazine::OnQuickLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	if( pCtx->getLocalTime() == 0.0f)
	{
		if( getOwner() != nullptr && getOwner()->isLocal())
			addWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_FIRE_DISABLE);

		setWeaponState( WEAPON::STATE_LOADMAG);
		removeWeaponStateMask( WSM_WAIT_FOR_LOADMAG);
	}

	if( isWeaponStateMask( WSM_WAIT_FOR_LOADMAG) == false)
	{
		I3ASSERT( getAIIndex( WEAPON::AI_CURRENT) != -1);
		i3AIState * pAI = pCtx->getAIState( getAIIndex( WEAPON::AI_CURRENT));

		if( pCtx->getLocalTime() + tm > pAI->getActualDuration())
		{
			if( m_DualMagazineMode == WEAPON_DUAL_MAGAZINE_IDLE )		m_DualMagazineMode = WEAPON_DUAL_MAGAZINE_RIGHT;
			else														m_DualMagazineMode = WEAPON_DUAL_MAGAZINE_IDLE;

			setWeaponState( WEAPON::STATE_READY);
			_OnPlayWeaponIdleAnim();
			removeWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_FIRE_DISABLE);
			addWeaponStateMask( WSM_WAIT_FOR_LOADMAG);
		}
	}
}

void WeaponDualMagazine::OnEndQuickLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
}

void WeaponDualMagazine::_OnProcessFireEndAnim( REAL32 rDelay)
{
	if( isWeaponStateMask( WSM_FIRING ) ||
		rDelay >= m_pOwner->getAnimContext()->getUpperDuration() )
	{
		_OnPlayWeaponIdleAnim();
		if( m_pWeaponInfo->GetAutoFireCount() == 0)
		{	// 연사, 또는 점사
			removeWeaponStateMask( WSM_FIRE_DISABLE);
		}
	}
	else
	{
		switch( m_DualMagazineMode )
		{
		case WEAPON_DUAL_MAGAZINE_RIGHT :
			// 현재 장전된 탄환이 없다.
			if( getLoadedBulletDualCount() <= 0)	_OnPlayWeaponIdleAnim();
			else
			{
				removeWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
				setWeaponState( WEAPON::STATE_FIRING);
			}
			break;
		case WEAPON_DUAL_MAGAZINE_IDLE :
		default :
			if( getLoadedBulletCount() <= 0)	_OnPlayWeaponIdleAnim();
			else
			{
				removeWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
				setWeaponState( WEAPON::STATE_FIRING);
			}
			break;
		}
	}
}


