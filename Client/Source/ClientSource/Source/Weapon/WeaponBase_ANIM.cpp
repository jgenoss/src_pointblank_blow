#include "pch.h"
#include "WeaponBase.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "BattleHud.h"
#include "GameCharaAnimContext.h"
#include "GameCharaMoveContext.h"
#include "BattleSlotContext.h"

//#define FIREDELAY_COST

///////////////////////////////////////////////////////////////////////////////////////////
void WeaponBase::OnIdle( i3AIContext * pCtx, REAL32 tm)
{	
	if( m_pOwner == nullptr)	return;
	
	bool bLoaded = true;

	if( pCtx->getLocalTime() == 0.0f)
	{
		SetCurrentAttachHand();
		
		if (g_pFramework->IsBattleStage() == true && m_pOwner->is1PV())
		{
			if (getDropInfo()->_bDropping == false && getDropInfo()->_bDroped == false)
			{
				// 인게임 일 경우에만 사운드를 사용합니다.
				// 드랍중이거나 드랍되었을 때가 아닐 경우에만 실행
				_PlayIdleSound();
			}
		}
	}

	// 자동 재장전
	INT32 iBullet = getLoadedBulletCount();

	if( iBullet <= 0)
	{
		if( getTotalBulletCount() > 0)
		{
			setWeaponState( WEAPON::STATE_NEEDTO_LOADMAG);		// 자동 재장전
			removeWeaponStateMask( WSM_LOADMAG_DISABLE);
		}
		else
			setWeaponState( WEAPON::STATE_EMPTY_MAGAZINE);

		if( m_pOwner->isLocal()) addWeaponStateMask( WSM_FIRE_DISABLE);
		bLoaded = false;
	}

	// 로드타입이 펌프 일경우 탄창을 리로드 
	if(	m_pOwner != nullptr
		&& getWeaponInfo()->GetGaugeReload() == 1 )
	{
//		INT32 iBullet = getLoadedBulletCount();
		INT32 iTotal = getTotalBulletCount();
		INT32 iMaxBullet = GetLoadBulletWithCashItem();

		if(	(iBullet < iMaxBullet) && (iTotal > 0))
		{
			if( getWeaponInfo()->IsLoadMagazineReady() 
				&& isReadyToLoadMagazine() == false)
			{
				setWeaponState( WEAPON::STATE_NEEDTO_LOADMAG);
				addWeaponStateMask( WSM_READY_TO_LOADMAG);
			}
		}
	}

	if( bLoaded )
	{
		if(getWeaponInfo()->GetPumpAction())	//	펌프액션이면
		{
			addWeaponStateMask( WSM_LOADED_BULLET);
			removeWeaponStateMask( WSM_FIRE_DISABLE);
		}
		else
		{
			removeWeaponStateMask( WSM_FIRE_DISABLE);
		}

		if( iBullet < GetLoadBulletWithCashItem() )	removeWeaponStateMask( WSM_LOADMAG_DISABLE);
		else if( m_pOwner->isLocal())			addWeaponStateMask( WSM_LOADMAG_DISABLE);

		if( isWeaponStateMask( WSM_FIRING) == false)
		{	// 발사중이 아니면
			if(getWeaponInfo()->GetPumpAction() == false)
			{
				setWeaponState( WEAPON::STATE_READY);
				removeWeaponStateMask( WSM_FIRE_DISABLE
#if defined( FIREDELAY_COST)
					| WSM_MORE_FIRING
#endif
					);
			}
		}
		else
		{
			INT32 autoCnt;
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
							removeWeaponStateMask(WSM_WAIT_AUTOFIREDELAY | WSM_FIRE_DISABLE );
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

void WeaponBase::ProcessAutoPoint(REAL32 tm)
{
	INT32 autoCnt = 0;

	if (getRemainAutoFireCount() != 0)
	{
		autoCnt = getRemainAutoFireCount();
	}
	else
	{
		if (getOwner()->isCharaStateMask(CHARA_STATE_ZOOM) == false)
			autoCnt = m_pWeaponInfo->GetAutoFireCount();
		else
			autoCnt = m_pWeaponInfo->GetExtAutoFireCount();

	}

	if (isWeaponStateMask(WSM_POINTSHOTING) && getAutoFireCount() <= autoCnt)
	{
		if (m_rFireTime >= m_pWeaponInfo->GetFireDelay())
			m_pOwner->Cmd_Attack();

		m_rFireTime = m_rFireTime + tm;

		if (getAutoFireCount() > autoCnt)
		{
			m_timeAutoFire = 0.0f;
			setWeaponState(WEAPON::STATE_WAIT);
		}
	}
	else if (getAutoFireCount() > autoCnt)
	{
		m_timeAutoFire += tm;
		addWeaponStateMask(WSM_FIRE_DISABLE);

		if (m_timeAutoFire >= (m_pWeaponInfo->GetAutoFireDelay() - tm))
		{
			addWeaponStateMask(WSM_FIRE_DISABLE);
			removeWeaponStateMask(WSM_POINTSHOTING);
			m_timeAutoFire = 0.0f;
		}
	}
}

void WeaponBase::OnEndIdle(i3AIContext * pCtx, REAL32 tm)
{
	I3ASSERT(m_pWeaponInfo != nullptr);

	if (m_pOwner == nullptr)	return;

	_StopIdleSound();
}

void WeaponBase::OnFire( i3AIContext * pCtx, REAL32 tm)
{
	I3ASSERT( m_pWeaponInfo != nullptr);

	if( m_pOwner == nullptr)	return;

	if( pCtx->getLocalTime() == 0.0f)
	{
		SetCurrentAttachHand();

		_OnProcessFireFirstAnim();
#if defined( FIREDELAY_COST)
		m_rFireTime = m_rFireTime + tm;
#endif
	}
	else
	{	
		//REAL32 rDelay = pCtx->getLocalTime() + tm;
#if defined( FIREDELAY_COST)
		m_rFireTime = m_rFireTime + tm;
#else
		m_rFireTime = pCtx->getLocalTime() + tm;
#endif	

		_OnProcessFireMiddleAnim( m_rFireTime);

		REAL32 rDelay = GetFireDelay();
		rDelay = MINMAX(0.06f, rDelay, 3.f);

		if( m_rFireTime > rDelay )
		{// 발사 지연시간이 충분히 지났다면...
			_OnProcessFireEndAnim( pCtx->getLocalTime() + tm);

			// 누적시킨다.
#if defined( FIREDELAY_COST)
			m_rFireTime = m_rFireTime - rDelay;
#endif
		}
	}
}

void WeaponBase::OnEndFire( i3AIContext * pCtx, REAL32 tm)
{
	removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE
#if defined( FIREDELAY_COST)
		| WSM_MORE_FIRING
#endif
		);

	if( pCtx->getLocalTime() == 0.0f)
		SetCurrentAttachHand();

	if(getWeaponInfo()->GetPumpAction())	//	펌프액션 방식이면
	{
		if( getLoadedBulletCount() > 0)
		{
			setWeaponState( WEAPON::STATE_READY);
			addWeaponStateMask( WSM_LOADED_BULLET);
		}
		else
		{
			removeWeaponStateMask( WSM_LOADED_BULLET);
			if( getOwner() != nullptr && getOwner()->isLocal())
				addWeaponStateMask( WSM_FIRE_DISABLE);
		}
	}

	m_mainWeapon->OnEndFire(pCtx, tm);
}

void WeaponBase::OnExtensionActivate( i3AIContext * pCtx, REAL32 tm)
{
	if( getAIIndex( WEAPON::AI_EXTENSION_ACTIVATE) == -1)
		return;

	if( pCtx->getLocalTime() == 0.0f)
		SetCurrentAttachHand();

	i3AIState * pAI = pCtx->getAIState( getAIIndex( WEAPON::AI_EXTENSION_ACTIVATE));

	//	동작이 종료되면
	if( pCtx->getLocalTime() + tm >= pAI->getDuration() ||
		(m_pWeaponInfo->GetTypeZoom() > WEAPON::ZOOM_NONE && pCtx->getLocalTime() + tm >= 0.5f) )
	{
		removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);

		addWeaponStateMask( WSM_ACTIVATE_EXTENSION);

		_OnPlayWeaponIdleAnim();
	}
	else
	{
		if( (m_pOwner != nullptr) && m_pOwner->isLocal())
		{
			addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
		}

		removeWeaponStateMask( WSM_ACTIVATE_EXTENSION);
	}

	OnSetVisibleExtension( true);
}

void WeaponBase::OnExtensionDeactivate( i3AIContext * pCtx, REAL32 tm)
{
	if( getAIIndex( WEAPON::AI_EXTENSION_DEACTIVATE) == -1)
		return;

	if( pCtx->getLocalTime() == 0.0f)
		SetCurrentAttachHand();

	i3AIState * pAI = pCtx->getAIState( getAIIndex( WEAPON::AI_EXTENSION_DEACTIVATE));

	REAL32 rElaps = pCtx->getLocalTime() + tm;
	//	동작이 종료되면
	if( rElaps >= pAI->getDuration() ||
		(m_pWeaponInfo->GetTypeZoom() > WEAPON::ZOOM_NONE && rElaps + tm >= 0.5f) )
	{
		removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE | WSM_ACTIVATE_EXTENSION);

		_OnPlayWeaponIdleAnim();

		OnSetVisibleExtension( false);
	}
	else
	{
		if( getOwner() != nullptr && getOwner()->isLocal())
			addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);

		addWeaponStateMask( WSM_ACTIVATE_EXTENSION);
	}
}

bool WeaponBase::_OnNormalReload(i3AIContext * pCtx, REAL32 tm)
{
	bool bDoneLoadMag = false;

	REAL32 rReloadTime = 1.f;

	i3AIState * pAIState = pCtx->getCurrentAIState();

	if (isDualGun() && getAIIndex(WEAPON::AI_LOADMAGAZINEDUAL) != -1)
		pAIState = pCtx->getAIState(getAIIndex(WEAPON::AI_LOADMAGAZINEDUAL));

	if (m_pPairWeapon != nullptr &&  getAIIndex(WEAPON::AI_LOADMAGAZINEDUAL) != -1)
		pAIState = pCtx->getAIState(getAIIndex(WEAPON::AI_LOADMAGAZINEDUAL));

	if (pAIState != nullptr)
		rReloadTime = pAIState->getActualDuration();

	//	탄피 Effect. 탄창식 일반 총기가 아닌 볼트 액션식 총기만 (볼트액션 총기)
	if ((pCtx->getLocalTime() > 2.f) && !isWeaponStateMask(WSM_EMPTY_CARTRIDGE) &&
		getWeaponInfo()->GetNeedToLoadBullet() && (getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_SNIPER) &&
		(m_idxBulletEffect != -1) && (m_idxFXBullet != -1))
	{
		_EmptyCartridgeEffect(m_idxFXBullet, m_idxBulletEffect);
		addWeaponStateMask(WSM_EMPTY_CARTRIDGE);

		MATRIX * pMtx = getSceneObject()->GetBody()->getCacheMatrix(m_idxFXBullet);
		_AddEmptyCartridgeSound(m_pOwner->getMoveContext()->getFloorTerrainType(), 0.8f, i3Matrix::GetPos(pMtx));
	}


	if ((getWeaponInfo()->GetGaugeReload() == 0)
		&& (pCtx->getLocalTime() + tm > rReloadTime))
	{
		if (getWeaponInfo()->GetLoadMagToLoadBullet() == false)
			AddNomalBullet();
	}
	else if (getWeaponInfo()->GetGaugeReload() == 1)
	{
		AddPumpBullet(tm);
	}

	if (pCtx->getLocalTime() + tm > rReloadTime)
	{

		bDoneLoadMag = true;

		INT32 iBullet = getLoadedBulletCount();
		INT32 iTotal = getTotalBulletCount();
		INT32 iMaxBullet = GetLoadBulletWithCashItem();

		// Load Magaine 이후에 진행 되어 야 하는 것은 
		// Bullet에서 처리 하도록 변경해야 합니다.
		if (getWeaponInfo()->GetLoadMagToLoadBullet() == true)
		{
			if (m_pWeaponInfo->GetNeedToLoadBullet())
			{
				setWeaponState(WEAPON::STATE_NEEDTO_LOADBULLET);
				removeWeaponStateMask(WSM_LOADBULLET_DISABLE);

				addWeaponStateMask(WSM_LOADMAG_TO_NEEDBULLET);
			}
		}
		// 재장전했음에도 아직 완전히 탄창이 채워지지 않았다면
		// 다시 Load Mag을 요구한다.
		// 샷건이 적용되는 대표적인 예.
		else if ((iBullet < iMaxBullet) && (iTotal > 0))
		{
			setWeaponState(WEAPON::STATE_NEEDTO_LOADMAG);
		}
		else
		{
			//	펌프액션 방식과 Mag하고 난 뒤에 LoadBullet 이 필요한 무기 일 경우에는 사용합니다.
			if (getWeaponInfo()->GetPumpAction())
			{
				if (m_pWeaponInfo->GetNeedToLoadBullet())
				{
					setWeaponState(WEAPON::STATE_NEEDTO_LOADBULLET);
					removeWeaponStateMask(WSM_LOADBULLET_DISABLE);
				}
			}
			else
			{
				// 더 이상 장탄할 총알이 없음.
				// 그냥 Idle 상태로 돌린다.
				setWeaponState(WEAPON::STATE_READY);
				_OnPlayWeaponIdleAnim();
			}

			//버그일수도 있어서 예외 처리 합니다. 
			iMaxBullet = MIN(iBullet, iMaxBullet);
			setLoadedBulletCount(iMaxBullet);
		}

		removeWeaponStateMask(WSM_LOADMAG_DISABLE);
		addWeaponStateMask(WSM_WAIT_FOR_LOADMAG);
	}

	return bDoneLoadMag;
}

bool WeaponBase::_OnLoopAnimationReload(i3AIContext * pCtx, REAL32 tm)
{
	bool bDoneLoadMag = false;
	
	// Time을 따로 계산합니다.
	m_rReloadTime += tm;

	REAL32 rWeaponReloadTime = getWeaponInfo()->GetReloadTime();

	// 여기에 재장전 속도 증가 들어가야 함.
	REAL32 ratio = BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_REBULLET_RATE);
	rWeaponReloadTime -= (rWeaponReloadTime * ratio);

	//	탄피 Effect. 탄창식 일반 총기가 아닌 볼트 액션식 총기만 (볼트액션 총기)
	if ((pCtx->getLocalTime() > 2.f) && !isWeaponStateMask(WSM_EMPTY_CARTRIDGE) &&
		getWeaponInfo()->GetNeedToLoadBullet() && (getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_SNIPER) &&
		(m_idxBulletEffect != -1) && (m_idxFXBullet != -1))
	{
		_EmptyCartridgeEffect(m_idxFXBullet, m_idxBulletEffect);
		addWeaponStateMask(WSM_EMPTY_CARTRIDGE);

		MATRIX * pMtx = getSceneObject()->GetBody()->getCacheMatrix(m_idxFXBullet);
		_AddEmptyCartridgeSound(m_pOwner->getMoveContext()->getFloorTerrainType(), 0.8f, i3Matrix::GetPos(pMtx));
	}
	
	if ((getWeaponInfo()->GetGaugeReload() == 0)
		&& (m_rReloadTime > rWeaponReloadTime))
	{
		if (getWeaponInfo()->GetLoadMagToLoadBullet() == false)
			AddNomalBullet();
	}
	else if (getWeaponInfo()->GetGaugeReload() == 1)
	{
		AddPumpBullet(tm);
	}

	if (m_rReloadTime > rWeaponReloadTime)
	{

		bDoneLoadMag = true;

		INT32 iBullet = getLoadedBulletCount();
		INT32 iTotal = getTotalBulletCount();
		INT32 iMaxBullet = GetLoadBulletWithCashItem();

		// Load Magaine 이후에 진행 되어 야 하는 것은 
		// Bullet에서 처리 하도록 변경해야 합니다.
		if (getWeaponInfo()->GetLoadMagToLoadBullet() == true)
		{
			if (m_pWeaponInfo->GetNeedToLoadBullet())
			{
				setWeaponState(WEAPON::STATE_NEEDTO_LOADBULLET);
				removeWeaponStateMask(WSM_LOADBULLET_DISABLE);

				addWeaponStateMask(WSM_LOADMAG_TO_NEEDBULLET);
			}
		}
		// 재장전했음에도 아직 완전히 탄창이 채워지지 않았다면
		// 다시 Load Mag을 요구한다.
		// 샷건이 적용되는 대표적인 예.
		else if ((iBullet < iMaxBullet) && (iTotal > 0))
		{
			setWeaponState(WEAPON::STATE_NEEDTO_LOADMAG);
		}
		else
		{
			//	펌프액션 방식과 Mag하고 난 뒤에 LoadBullet 이 필요한 무기 일 경우에는 사용합니다.
			if (getWeaponInfo()->GetPumpAction())
			{
				if (m_pWeaponInfo->GetNeedToLoadBullet())
				{
					setWeaponState(WEAPON::STATE_NEEDTO_LOADBULLET);
					removeWeaponStateMask(WSM_LOADBULLET_DISABLE);
				}
			}
			else
			{
				// 더 이상 장탄할 총알이 없음.
				// 그냥 Idle 상태로 돌린다.
				setWeaponState(WEAPON::STATE_READY);
				_OnPlayWeaponIdleAnim();
			}

			//버그일수도 있어서 예외 처리 합니다. 
			iMaxBullet = MIN(iBullet, iMaxBullet);
			setLoadedBulletCount(iMaxBullet);
		}

		removeWeaponStateMask(WSM_LOADMAG_DISABLE);
		addWeaponStateMask(WSM_WAIT_FOR_LOADMAG);
	}

	return bDoneLoadMag;
}

bool WeaponBase::OnLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	bool bDoneLoadMag = false;

	if( m_pOwner == nullptr) 
		return false;

	if( pCtx->getLocalTime() == 0.0f)
	{
		if( m_pOwner->isLocal())
			addWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_FIRE_DISABLE);
		setWeaponState( WEAPON::STATE_LOADMAG);

		m_rReloadTime = 0.f;
		removeWeaponStateMask( WSM_WAIT_FOR_LOADMAG | WSM_EMPTY_CARTRIDGE);

		if(( getWeaponInfo()->GetTypeTrigger() == WEAPON::TRIGGER_RELOADING_SHOT) &&
			isWeaponStateMask( WSM_LOADED_BULLET) )
		{
			removeWeaponStateMask( WSM_FIRE_DISABLE);
		}

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

	if( isWeaponStateMask(WSM_WAIT_FOR_LOADMAG) == false)
	{
		// Loop와 Non Loop를 구분합니다.
		if (isReloadLoopAnimation())
		{
			bDoneLoadMag = _OnLoopAnimationReload(pCtx, tm);
		}
		else
		{
			bDoneLoadMag = _OnNormalReload(pCtx, tm);
		}
	}

	return bDoneLoadMag;
}

void WeaponBase::AddNomalBullet()
{
	INT32 addCount;

	INT32 iBullet = getLoadedBulletCount();
	INT32 iTotal = getTotalBulletCount();

	PB_VM_S

		addCount = MIN( m_pWeaponInfo->GetReloadBulletCount(), iTotal);

	if((iBullet + addCount) > GetLoadBulletWithCashItem())
		addCount -= (iBullet + addCount) - GetLoadBulletWithCashItem();

	iBullet += addCount;

	setLoadedBulletCount( iBullet);

#ifdef ENABLE_TEST_COMMAND
	if( !g_pWeaponManager->COMMAND_IsUnlimitGun())		//	Use Cheat
#endif
	{
		iTotal -= addCount;
	}

	setTotalBulletCount( iTotal);

	PB_VM_E
}

void WeaponBase::AddPumpBullet(REAL32 tm)
{
	REAL32 addCount = 0.f;
	REAL32 iBullet = getLoadedBulletCountF();
	REAL32 iTotal = static_cast<REAL32>(getTotalBulletCount());
	INT32 iMaxBullet = GetLoadBulletWithCashItem();
	REAL32 ratio = BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_REBULLET_RATE);

	addCount = MIN( m_pWeaponInfo->GetReloadBulletCount(), iTotal);
	addCount = (tm / (addCount));
	addCount += (addCount*ratio);

	iBullet += addCount;


	//iBullet = MIN( iBullet, iTotal);

	if( iMaxBullet <= iBullet)
		SetEndurance(100);
	setLoadedBulletCountF( iBullet);

	//setTotalBulletCount( iTotal);


}
void WeaponBase::OnEndLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	removeWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);

	if(getWeaponInfo()->GetPumpAction())	//	펌프액션 방식이면
	{
		if( (isWeaponStateMask( WSM_LOADED_BULLET) == false) && (getOwner() != nullptr && getOwner()->isLocal()))
			addWeaponStateMask( WSM_FIRE_DISABLE);
	}

	SetCurrentAttachHand();

	m_mainWeapon->OnEndLoadMag(pCtx, tm);
}

void WeaponBase::OnLoadBullet( i3AIContext * pCtx, REAL32 tm)
{	
	i3AIState * pLoadBullet = pCtx->getCurrentAIState();

	I3ASSERT( pLoadBullet != nullptr);


	if( pCtx->getLocalTime() == 0.f )
	{
		if( getOwner() != nullptr && getOwner()->isLocal())
			addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);

		// KEYWORD : WEAPON_DUMMY
		if( IsLoadBulletLeftBarrel() )
		{	// MG는 탄약 장전시에만 왼손 파지
			ChangeAttachHand( CHARA_WEAPON_BARREL_LEFT_HAND, WEAPON_ATTACH_BARREL);
		}		
	}

	//	탄피 Effect. 탄창식 일반 총기가 아닌 볼트 액션식 총기만 (볼트액션 총기)
	if( (getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_SNIPER) &&
		(pCtx->getLocalTime() > 0.5f) && !isWeaponStateMask( WSM_EMPTY_CARTRIDGE) && getWeaponInfo()->GetNeedToLoadBullet() &&
		(m_idxFXBullet != -1) && (m_idxBulletEffect != -1))			
	{
		_EmptyCartridgeEffect( m_idxFXBullet, m_idxBulletEffect);
		addWeaponStateMask( WSM_EMPTY_CARTRIDGE);

		MATRIX * pMtx = getSceneObject()->GetBody()->getCacheMatrix( m_idxFXBullet);
		_AddEmptyCartridgeSound( m_pOwner->getMoveContext()->getFloorTerrainType(), DELAY_EMPTYBULLETSOUND, i3Matrix::GetPos( pMtx) );
	}
	
	if( pCtx->getLocalTime() > pLoadBullet->getActualDuration())
	{
		addWeaponStateMask( WSM_LOADED_BULLET);

		// 이 위치에서 탄창 추가!
		if (isWeaponStateMask(WSM_LOADMAG_TO_NEEDBULLET) == true)
		{
			// 탄 부터 채웁니다.
			AddNomalBullet();

			INT32 iBullet = getLoadedBulletCount();
			INT32 iTotal = getTotalBulletCount();
			INT32 iMaxBullet = GetLoadBulletWithCashItem();

			// 재장전했음에도 아직 완전히 탄창이 채워지지 않았다면
			// 다시 Load Mag을 요구한다.
			// 샷건이 적용되는 대표적인 예.
			if ((iBullet < iMaxBullet) && (iTotal > 0))
			{
				setWeaponState(WEAPON::STATE_NEEDTO_LOADMAG);
			}
			else
			{
				removeWeaponStateMask(WSM_LOADMAG_TO_NEEDBULLET);

				setWeaponState(WEAPON::STATE_READY);

				_OnPlayWeaponIdleAnim();
			}
		}
		else
		{
			setWeaponState(WEAPON::STATE_READY);

			_OnPlayWeaponIdleAnim();
		}
	}
}

void WeaponBase::OnEndLoadBullet( i3AIContext * pCtx, REAL32 tm)
{
	removeWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE | WSM_FIRE_DISABLE);

	if( pCtx->getLocalTime() == 0.f )
		SetCurrentAttachHand();
}

void WeaponBase::OnEndFireBullet( i3AIContext * pCtx, REAL32 tm)
{
	removeWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE | WSM_FIRE_DISABLE);

	if( pCtx->getLocalTime() == 0.f )
		SetCurrentAttachHand();
}

void WeaponBase::OnSwaping( i3AIContext * pCtx, REAL32 tm)
{
	I3ASSERT( pCtx != nullptr);

	REAL32 localTime = pCtx->getLocalTime();
	
	i3AIState * pAIState = pCtx->getCurrentAIState();
	I3ASSERT( pAIState != nullptr);

	if( pCtx->getLocalTime() == 0.f )
		SetCurrentAttachHand();

	if( localTime + tm > pAIState->getDuration())
	{
		_OnPlayWeaponIdleAnim();
		setWeaponState( WEAPON::STATE_READY);
		removeWeaponStateMask( WSM_FIRE_DISABLE);
	}
}

void WeaponBase::OnLoadMag_Ready( i3AIContext * pCtx, REAL32 tm)
{
	if( pCtx->getLocalTime() == 0.0f)
	{
		// KEYWORD : WEAPON_DUMMY
		if( IsLoadMagazineLeftBarrel() )
		{
			ChangeAttachHand( CHARA_WEAPON_BARREL_LEFT_HAND, WEAPON_ATTACH_BARREL);
		}		
	}
}
