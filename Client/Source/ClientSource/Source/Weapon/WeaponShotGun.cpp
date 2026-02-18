#include "pch.h"
#include "WeaponShotGun.h"
#include "GameCharaBase.h"
#include "GameCharaActionContext.h"
#include "GameCharaAnimContext.h"
#include "WeaponFunction.h"


I3_CLASS_INSTANCE( WeaponShotGun);//, WeaponBase);

WeaponShotGun::WeaponShotGun(void) : WeaponBase()
{
}

WeaponShotGun::~WeaponShotGun(void)
{
}

void WeaponShotGun::_OnInitVairable( void)
{
	WeaponBase::_OnInitVairable();

	m_iShotgunMasterLoadMagCnt = 0;
	m_aiiLoadB_Master1PV = -1;
	m_aiiLoadB_Master3PV = -1;
	m_aiiLoadC_Master1PV = -1;
	m_aiiLoadC_Master3PV = -1;
	m_aiiLoadD_Master1PV = -1;
	m_aiiLoadD_Master3PV = -1;

	memset(m_pBullet, 0, sizeof(i3Node*)*g_i870MCS_MasterBulletCount);

}

void WeaponShotGun::OnBindResource( void)
{
	WeaponBase::OnBindResource();

	m_aiiLoadB_Master1PV = (INT8) m_pAICtx->FindAIStateByName( "LoadB_Master1PV");
	m_aiiLoadB_Master3PV = (INT8) m_pAICtx->FindAIStateByName( "LoadB_Master3PV");
	m_aiiLoadC_Master1PV = (INT8) m_pAICtx->FindAIStateByName( "LoadC_Master1PV");
	m_aiiLoadC_Master3PV = (INT8) m_pAICtx->FindAIStateByName( "LoadC_Master3PV");
	m_aiiLoadD_Master1PV = (INT8) m_pAICtx->FindAIStateByName( "LoadD_Master1PV");
	m_aiiLoadD_Master3PV = (INT8) m_pAICtx->FindAIStateByName( "LoadD_Master3PV");

	m_pBullet[0] = i3Scene::FindNodeByName(getSceneObject(), "Model_VOL01", true);
	m_pBullet[1] = i3Scene::FindNodeByName(getSceneObject(), "Model_VOL02", true);
	m_pBullet[2] = i3Scene::FindNodeByName(getSceneObject(), "Model_VOL03", true);
	m_pBullet[3] = i3Scene::FindNodeByName(getSceneObject(), "Model_VOL", true);
}

void WeaponShotGun::Reset( void)
{
	WeaponBase::Reset();
	m_iShotgunMasterLoadMagCnt = 0;
}

void WeaponShotGun::_OnPlayWeaponFireAnim()
{
	INT8 AIIndex = getAIIndex( WEAPON::AI_FIRE);

	if( (getAIIndex( WEAPON::AI_FIREMALE) != -1) && (getAIIndex( WEAPON::AI_FIREFEMALE) != -1))		// 우선
	{
		if(  m_pOwner != nullptr && (m_pOwner->isFemale() == true))
			AIIndex = getAIIndex( WEAPON::AI_FIREFEMALE);
		else if(  m_pOwner != nullptr && (m_pOwner->isFemale() == false))
			AIIndex = getAIIndex( WEAPON::AI_FIREMALE);
	}

	i3AIState * pAIState = m_pAICtx->getAIState( AIIndex );
	I3ASSERT( pAIState != nullptr);
	REAL32 rDelay = GetFireDelay();
	I3ASSERT( rDelay != 0.f);
	REAL32 rTimeScale = pAIState->getDuration() / rDelay;
	WF::PlayWeaponAnim( this, AIIndex, rTimeScale );
}

REAL32 WeaponShotGun::GetAnimFireScaleTime( CHARA_ACTION_UPPER upperState, ID_UPPER_AI ai )
{
	REAL32 rTimeScale = 1.f;
	if( m_pOwner != nullptr)
	{
		CGameCharaAnimContext * pAnimCtx = m_pOwner->getAnimContext();

		i3AIContext * pAIUpper = pAnimCtx->getAIUpper();

		#if defined( AI_MOVE_TO_WEAPON)
			INT32 nID = getCharaAIIndex( m_pOwner->getAnimSet(), m_pOwner->getActionContext()->getBodyUpper(), ai);
		#else
			INT32 nID = pAnimCtx->getAIUpperID( m_pWeaponInfo->GetTypeClass(), m_pWeaponInfo->GetNumber(), ai);
		#endif

		if( pAIUpper != nullptr )
		{
			if( nID == -1)
			{
				I3PRINTLOG(I3LOG_NOTICE, "nID == -1");
				return 1.f;
			}
			I3ASSERT( nID < pAIUpper->getAIStateCount());
			i3AIState * pNewAIState = pAIUpper->getAIState( nID );
			if( pNewAIState != nullptr )
			{
				CGameFramework::LoadAIStateAnimation( pNewAIState);

				REAL32 rDelay = GetFireDelay();

				if ( ai == ID_UPPER_SECONDARY_ATTACK )
					rDelay = 1.f;

				rTimeScale = pNewAIState->getAnimDuration() / rDelay;
			}
		}
	}

	return rTimeScale;
}

void WeaponShotGun::OnSetVisibleExtension( bool bVal)
{
	INT32 i = 0;

	if( bVal)
	{
		for(i=0; i<g_i870MCS_MasterBulletCount; i++)
		{
			if(m_pBullet[i] != nullptr)
			{
				m_pBullet[i]->RemoveFlag( I3_NODEFLAG_INVISIBLE );
			}
		}
	}
	else
	{
		for(i=0; i<g_i870MCS_MasterBulletCount; i++)
		{
			if(m_pBullet[i] != nullptr)
			{
				m_pBullet[i]->AddFlag( I3_NODEFLAG_INVISIBLE );
			}
		}
	}
}

void WeaponShotGun::OnLoadC_MasterRun(i3AIContext * pCtx, REAL32 tm)
{
	RepeatLoadMagShotgunMaster(pCtx, tm, ESMLMO_C);
}

void WeaponShotGun::OnLoadB_MasterRun(i3AIContext * pCtx, REAL32 tm)
{
	RepeatLoadMagShotgunMaster(pCtx, tm, ESMLMO_B);
}

void WeaponShotGun::RepeatLoadMagShotgunMaster(i3AIContext * pCtx, REAL32 tm, EShotgunMasterLoadMagOrder eMagOrder)
{
	// bool bDoneLoadMag = false;

	if( m_pOwner == nullptr) 
		return ;//false;

	if( pCtx->getLocalTime() == 0.0f)
	{
		if( m_pOwner->isLocal())
			addWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_FIRE_DISABLE);
		setWeaponState( WEAPON::STATE_LOADMAG);

		removeWeaponStateMask( WSM_WAIT_FOR_LOADMAG | WSM_EMPTY_CARTRIDGE);

		if( (getWeaponInfo()->GetTypeTrigger() == WEAPON::TRIGGER_RELOADING_SHOT) &&
			isWeaponStateMask( WSM_LOADED_BULLET) )
		{
			removeWeaponStateMask( WSM_FIRE_DISABLE);
		}

		if(eMagOrder == ESMLMO_C)
		{
			m_iShotgunMasterLoadMagCnt = 0;
			int i = 0;

			for(i=0; i<g_i870MCS_MasterBulletCount; i++)
			{
				if(m_pBullet[i] != nullptr)
				{
					m_pBullet[i]->RemoveFlag( I3_NODEFLAG_INVISIBLE );
				}
			}
		}
	}

	if( isWeaponStateMask( WSM_WAIT_FOR_LOADMAG) == false)
	{
		REAL32 rReloadTime = 1.f;

		i3AIState * pAIState = pCtx->getCurrentAIState();

		if( pAIState != nullptr)
			rReloadTime = pAIState->getActualDuration();
		
		if( (pCtx->getLocalTime() + tm)  > rReloadTime )
		{
			INT32 addCount;

			PB_VM_S

			INT32 iBullet = getLoadedBulletCount();
			INT32 iTotal = getTotalBulletCount();

			addCount = MIN( m_pWeaponInfo->GetReloadBulletCount(), iTotal);

			if(( iBullet + addCount) > GetLoadBulletWithCashItem())
				addCount -= (iBullet + addCount) - GetLoadBulletWithCashItem();

			if(eMagOrder == ESMLMO_B)
			{
				iBullet += addCount;
#ifdef ENABLE_TEST_COMMAND
				if( !g_pWeaponManager->COMMAND_IsUnlimitGun())		//	Use Cheat
#endif
				{
					iTotal -= addCount;
				}
			}
			else if(eMagOrder == ESMLMO_C)
			{
				setWeaponState( WEAPON::STATE_NEEDTO_LOAD_B_MAG);
			}

			setLoadedBulletCount( iBullet);
			setTotalBulletCount( iTotal);

			PB_VM_E

			// 재장전했음에도 아직 완전히 탄창이 채워지지 않았다면
			// 다시 Load Mag을 요구한다.
			// 샷건이 적용되는 대표적인 예.
			if( (iBullet < GetLoadBulletWithCashItem()) && (iTotal > 0))
			{
				
				if(eMagOrder == ESMLMO_B)
				{
					m_iShotgunMasterLoadMagCnt++;

				}

				if(m_iShotgunMasterLoadMagCnt >= g_i870MCS_MasterBulletCount)
				{
					setWeaponState( WEAPON::STATE_NEEDTO_LOADMAG);
				}
				else 
				{
					setWeaponState( WEAPON::STATE_NEEDTO_LOAD_B_MAG);
				}

				if(m_iShotgunMasterLoadMagCnt <= g_i870MCS_MasterBulletCount)
				{
					int i = 0;

					for(i=0; i<m_iShotgunMasterLoadMagCnt; i++)
					{
						if(m_pBullet[i] != nullptr)
						{
							m_pBullet[i]->AddFlag( I3_NODEFLAG_INVISIBLE );
						}
					}
				}
			}
			else
			{
				if(getWeaponInfo()->GetPumpAction())	//	펌프액션 방식이면
				{
					if( m_pWeaponInfo->GetNeedToLoadBullet() )
					{
						setWeaponState( WEAPON::STATE_NEEDTO_LOADBULLET);
						removeWeaponStateMask( WSM_LOADBULLET_DISABLE);
					}
				}
				else
				{
					// 더 이상 장탄할 총알이 없음.
					// 그냥 Idle 상태로 돌린다.
					setWeaponState( WEAPON::STATE_READY);
					_OnPlayWeaponIdleAnim();
				}
			}

			removeWeaponStateMask( WSM_LOADMAG_DISABLE);
			addWeaponStateMask( WSM_WAIT_FOR_LOADMAG);
		}
	}

	// return bDoneLoadMag;
}

void WeaponShotGun::OnPlayLoadBulletAnimation( void)
{
	bool bPlayAnim = false;

	if( EnablePlayMasterAnim())
	{
		if( getWeaponInfo()->GetNeedToLoadBullet() == 0)
		{
			I3PRINTLOG(I3LOG_WARN,  "OnPlayLoadBulletAnimation이 호출되면 안됩니다.");
		}
		else
		{
			if((m_pOwner != nullptr) && m_pOwner->is1PV())
			{
				bPlayAnim = _PlayWeaponAnimWithApplyTimeScale( m_aiiLoadD_Master1PV);
			}
			else if((m_pOwner != nullptr) && m_pOwner->is3PV())
			{
				bPlayAnim = _PlayWeaponAnimWithApplyTimeScale( m_aiiLoadD_Master3PV);
			}
		}
	}
	
	if( bPlayAnim == false)
	{
		_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADBULLET));
	}
}

void WeaponShotGun::OnPlayLoadMagAnimation( void)
{
	bool bPlayAnim = false;

	if(EnablePlayMasterAnim())
	{
		if(getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_SHOTGUN)
		{
			if( getWeaponInfo()->GetNeedToLoadBullet() == 0)
			{
				if( m_pOwner->is1PV())
				{
					bPlayAnim = _PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER1PV));
				}
				else
				{
					bPlayAnim = _PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER3PV) );
				}
			}
			else
			{
				if( m_pOwner->is1PV())
				{
					if(getWeaponState() == WEAPON::STATE_NEEDTO_LOAD_B_MAG)
					{
						bPlayAnim = _PlayWeaponAnimWithApplyTimeScale( m_aiiLoadB_Master1PV );
					}
					else
					{
						bPlayAnim = _PlayWeaponAnimWithApplyTimeScale( m_aiiLoadC_Master1PV );
					}
					
				}
				else
				{
					if(getWeaponState() == WEAPON::STATE_NEEDTO_LOAD_B_MAG)
					{
						bPlayAnim = _PlayWeaponAnimWithApplyTimeScale( m_aiiLoadB_Master3PV );
					}
					else
					{
						bPlayAnim = _PlayWeaponAnimWithApplyTimeScale( m_aiiLoadC_Master3PV );
					}
					
				}
			}
		}
		else 
		{
			I3PRINTLOG(I3LOG_WARN,  "WEAPON_CLASS_SHOTGUN이 아닙니다.");
		}
	}
	
	if( bPlayAnim == false)
	{
		_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE) );
	}

	setWeaponState( WEAPON::STATE_LOADMAG);
}