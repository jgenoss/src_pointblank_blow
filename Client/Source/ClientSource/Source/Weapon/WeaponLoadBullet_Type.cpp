#include "pch.h"
#include "WeaponLoadBullet_Type.h"
#include "GameCharaActionContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaMoveContext.h"
#include "GameCharaWeaponContext.h"

I3_CLASS_INSTANCE( WeaponLoadBullet_Type);//, WeaponBase);

WeaponLoadBullet_Type::WeaponLoadBullet_Type()
{
	i3mem::Fill( m_aii_LoadMag_Ready, -1, sizeof( m_aii_LoadMag_Ready));
	i3mem::Fill( m_aii_FireBullet, -1, sizeof( m_aii_FireBullet));

	m_bFirstLoadMag = false;

	m_iLoadedBulletCount = 0;
	m_iLoadableBulletCount = 0;
}

WeaponLoadBullet_Type::~WeaponLoadBullet_Type()
{
}


void WeaponLoadBullet_Type::_OnInitVairable()
{
	WeaponBase::_OnInitVairable();

	for(INT32 view = 0; view<CHARA_VIEW_MAX; view++)
	{
		for(INT32 human = 0; human<HUMAN_MAX; human++)
		{
			for(INT32 action = 0; action<WEAPON_ACTION_MAX; action++)
			{
				m_aii_LoadMag_Ready[view][human][action] = -1;
				m_aii_FireBullet[view][human][action] = -1;
			}
		}
	}

	m_iLoadableBulletCount	= 0;
	m_bFirstLoadMag			= false;
}

void WeaponLoadBullet_Type::_OnProcessFireMiddleAnim( REAL32 rDelay)
{
	I3ASSERT( m_pOwner != nullptr);

	if( !isWeaponStateMask( WSM_EMPTY_CARTRIDGE) && (m_idxBulletEffect != -1) && (m_idxFXBullet != -1) )
	{
		if( rDelay > 0.6f )		addWeaponStateMask( WSM_EMPTY_CARTRIDGE);

		if( isWeaponStateMask( WSM_EMPTY_CARTRIDGE) )
		{
			MATRIX * pMtx = getSceneObject()->GetBody()->getCacheMatrix( m_idxFXBullet);

			_EmptyCartridgeEffect( m_idxFXBullet, m_idxBulletEffect);
			I3ASSERT( m_pOwner->getMoveContext() != nullptr);
			_AddEmptyCartridgeSound( m_pOwner->getMoveContext()->getFloorTerrainType(), DELAY_EMPTYBULLETSOUND, i3Matrix::GetPos( pMtx) );
		}
	}
}

void WeaponLoadBullet_Type::_OnPlayWeaponSecondFireAnim( void)
{
	WeaponBase::_OnPlayWeaponSecondFireAnim();
}

void WeaponLoadBullet_Type::_OnPlayTerrainHitSound( WEAPON::FIRE_ORDER fireOrder, UINT32 type, VEC3D * pPos)
{
	if( fireOrder == WEAPON::PRIMARY_FIRE)
		WeaponBase::_OnPlayTerrainHitSound( fireOrder, type, pPos);
	else
	{	// melee attack hit sound
		g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, GTBT_WPN_M1887_HIT, pPos, false);
	}
}

void WeaponLoadBullet_Type::_OnAddTerrainEffect( WEAPON_TERRAIN_EFFECT_INFO * pInfo)
{
	I3ASSERT( pInfo->_pPos != nullptr);
	I3ASSERT( pInfo->_pNormal != nullptr);
	I3ASSERT( pInfo->_pDir != nullptr);

	if( getFireOrder() == WEAPON::PRIMARY_FIRE)
	{
		WeaponBase::_OnAddTerrainEffect( pInfo);
	}
	else
	{
		g_pEffectManager->AddBlowEffect( pInfo->_terrainType, WEAPON_CLASS_KNIFE,
			static_cast<BULLET_EFFECT_TYPE>(getWeaponInfo()->GetLoadBulletType()),
			pInfo->_pPos, pInfo->_pNormal);
	}
}

void WeaponLoadBullet_Type::OnBindResource()
{
	WeaponBase::OnBindResource();

	m_aii_LoadMag_Ready[CHARA_VIEW_1PV][HUMAN_MALE][WEAPON_ACTION_NORMAL] = (INT8) m_pAICtx->FindAIStateByName( "LoadMagReady1PVMale");
	m_aii_LoadMag_Ready[CHARA_VIEW_1PV][HUMAN_FEMALE][WEAPON_ACTION_NORMAL] = (INT8) m_pAICtx->FindAIStateByName( "LoadMagReady1PVFemale");
	m_aii_LoadMag_Ready[CHARA_VIEW_1PV][HUMAN_MALE][WEAPON_ACTION_MASTER] = (INT8) m_pAICtx->FindAIStateByName( "LoadMagReadyMaster1PVMale");
	m_aii_LoadMag_Ready[CHARA_VIEW_1PV][HUMAN_FEMALE][WEAPON_ACTION_MASTER] = (INT8) m_pAICtx->FindAIStateByName( "LoadMagReadyMaster1PVFemale");

	m_aii_LoadMag_Ready[CHARA_VIEW_3PV][HUMAN_MALE][WEAPON_ACTION_NORMAL] = (INT8) m_pAICtx->FindAIStateByName( "LoadMagReady3PVMale");
	m_aii_LoadMag_Ready[CHARA_VIEW_3PV][HUMAN_FEMALE][WEAPON_ACTION_NORMAL] = (INT8) m_pAICtx->FindAIStateByName( "LoadMagReady3PVFemale");
	m_aii_LoadMag_Ready[CHARA_VIEW_3PV][HUMAN_MALE][WEAPON_ACTION_MASTER] = (INT8) m_pAICtx->FindAIStateByName( "LoadMagReadyMaster3PVMale");
	m_aii_LoadMag_Ready[CHARA_VIEW_3PV][HUMAN_FEMALE][WEAPON_ACTION_MASTER] = (INT8) m_pAICtx->FindAIStateByName( "LoadMagReadyMaster3PVFemale");
}

void WeaponLoadBullet_Type::OnSetVisibleExtension( bool bVal)
{
	WeaponBase::OnSetVisibleExtension( bVal);
}

bool WeaponLoadBullet_Type::NotMasterLoadMagAnim(INT32 idx)
{
	return (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV_MALE)) || (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV_MALE)) ||
		   (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV_FEMALE)) || (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV_FEMALE));
}

bool WeaponLoadBullet_Type::Fire( WEAPON::FIRE_ORDER type)
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

void WeaponLoadBullet_Type::FireEnd( void)
{
	removeWeaponStateMask( WSM_FIRING | WSM_WAIT_AUTOFIREDELAY | WSM_FIRE_DISABLE);
	
	if( getFireOrder() == WEAPON::PRIMARY_FIRE)
	{
		if(getWeaponInfo()->GetPumpAction() && !isWeaponStateMask( WSM_LOADED_BULLET))
		{
			addWeaponStateMask( WSM_FIRE_DISABLE);
		}

		setAutoFireCount( 0);
	}
}

void WeaponLoadBullet_Type::LoadMagazine_Ready( void)
{
	if( m_pOwner != nullptr && (m_pOwner->isLocal() || m_pOwner->isAI()))
	{
		if( getWeaponInfo()->GetTypeTrigger() != WEAPON::TRIGGER_RELOADING_SHOT)
			addWeaponStateMask( WSM_FIRE_DISABLE);

		addWeaponStateMask( WSM_LOADMAG_DISABLE );
	}

	OnPlayLoadMagReadyAnimation();
}

void WeaponLoadBullet_Type::OnPlayLoadBulletAnimation( void)
{
	INT8 aiidx = WEAPON::AI_CURRENT;

	HUMAN_TYPE type = HUMAN_MALE;
	if( m_pOwner->isFemale())	type = HUMAN_FEMALE;

	CHARA_VIEW_TYPE view = m_pOwner->getViewType();

	WEAPON::AI idx[2][CHARA_VIEW_MAX][HUMAN_MAX] = {
		{ {WEAPON::AI_LOADBULLET1PV_MALE, WEAPON::AI_LOADBULLET1PV_FEMALE}, {WEAPON::AI_LOADBULLET3PV_MALE, WEAPON::AI_LOADBULLET3PV_FEMALE} },
		{ {WEAPON::AI_FIREBULLET1PV_MALE, WEAPON::AI_FIREBULLET1PV_FEMALE}, {WEAPON::AI_FIREBULLET3PV_MALE, WEAPON::AI_FIREBULLET3PV_FEMALE} },
	};

	// 재장전 모션이 아닌 발사후 볼트액션 모션일 경우
	if( getOwner()->getActionContext()->getUpper() == CHARA_UPPER_FIRE)
		aiidx = getAIIndex(idx[1][view][type]);		//	WEAPON::AI_FIREBULLET1PV_MALE
	else
		aiidx = getAIIndex(idx[0][view][type]);		//	WEAPON::AI_LOADBULLET1PV_MALE
	
	_PlayWeaponAnimWithApplyTimeScale( aiidx);
}

void WeaponLoadBullet_Type::OnPlayLoadMagAnimation( void)
{
	WeaponBase::OnPlayLoadMagAnimation();
}

void WeaponLoadBullet_Type::OnPlayChangeAniImpl( REAL32 rTimeScale /*= 1.f*/)
{
	WeaponBase::OnPlayChangeAniImpl( rTimeScale);
}

void WeaponLoadBullet_Type::OnPlayLoadMagReadyAnimation( void)
{
	HUMAN_TYPE type = HUMAN_MALE;
	CHARA_VIEW_TYPE view = CHARA_VIEW_1PV;

	if( m_pOwner->isFemale())	type = HUMAN_FEMALE;

	view = m_pOwner->getViewType();

	INT8 idx = m_aii_LoadMag_Ready[view][type][WEAPON_ACTION_NORMAL];

	_PlayWeaponAnimWithApplyTimeScale( idx);
}

void WeaponLoadBullet_Type::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnIdle( pCtx, tm);

	m_bFirstLoadMag = true;

	removeWeaponStateMask( WSM_READY_TO_LOADMAG);
	m_FireOrder = WEAPON::PRIMARY_FIRE;
}

void WeaponLoadBullet_Type::OnLoadBullet( i3AIContext * pCtx, REAL32 tm)
{
	_LoadBullet( pCtx, tm);
}

bool WeaponLoadBullet_Type::OnLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	return WeaponBase::OnLoadMag( pCtx, tm);
}

void WeaponLoadBullet_Type::OnSecondaryFire( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnSecondaryFire( pCtx, tm);	
}

void WeaponLoadBullet_Type::OnSwaping( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnSwaping( pCtx, tm);
}

void WeaponLoadBullet_Type::OnFireBullet( i3AIContext * pCtx, REAL32 tm)
{	
	if( pCtx->getLocalTime() == 0.0f)
	{
		removeWeaponStateMask( WSM_EMPTY_CARTRIDGE);
	}

	//	탄피 Effect. 탄창식 일반 총기가 아닌 볼트 액션식 총기만 (볼트액션 총기)
	if(	(pCtx->getLocalTime() > 0.5f) && !isWeaponStateMask( WSM_EMPTY_CARTRIDGE) && getWeaponInfo()->GetNeedToLoadBullet() &&
		(m_idxFXBullet != -1) && (m_idxBulletEffect != -1))			
	{
		_EmptyCartridgeEffect( m_idxFXBullet, m_idxBulletEffect);
		addWeaponStateMask( WSM_EMPTY_CARTRIDGE);

		MATRIX * pMtx = getSceneObject()->GetBody()->getCacheMatrix( m_idxFXBullet);
		_AddEmptyCartridgeSound( m_pOwner->getMoveContext()->getFloorTerrainType(), DELAY_EMPTYBULLETSOUND, i3Matrix::GetPos( pMtx) );
	}

	_LoadBullet( pCtx, tm);
}

void WeaponLoadBullet_Type::OnEndFireBullet( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnEndFireBullet( pCtx, tm);
}

void WeaponLoadBullet_Type::OnLoadMag_Ready( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnLoadMag_Ready( pCtx, tm);

	I3ASSERT( pCtx != nullptr);
	REAL32 localTime = pCtx->getLocalTime();

	i3AIState * pAIState = pCtx->getCurrentAIState();
	I3ASSERT( pAIState != nullptr);

	if( pCtx->getLocalTime() == 0.0f)
	{
		removeWeaponStateMask( WSM_EMPTY_CARTRIDGE);
	}
	
	if( localTime + tm > pAIState->getDuration() )
	{
		setWeaponState( WEAPON::STATE_NEEDTO_LOADMAG);
		addWeaponStateMask( WSM_READY_TO_LOADMAG);
	}

	//	탄피 Effect. 탄창식 일반 총기가 아닌 볼트 액션식 총기만 (볼트액션 총기)
	if( (pCtx->getLocalTime() > 0.65f) && !isWeaponStateMask( WSM_EMPTY_CARTRIDGE) && 
		(m_idxBulletEffect != -1) && (m_idxFXBullet != -1) )
	{
		_EmptyCartridgeEffect( m_idxFXBullet, m_idxBulletEffect);
		addWeaponStateMask( WSM_EMPTY_CARTRIDGE);

		MATRIX * pMtx = getSceneObject()->GetBody()->getCacheMatrix( m_idxFXBullet);
		_AddEmptyCartridgeSound( m_pOwner->getMoveContext()->getFloorTerrainType(), 0.8f, i3Matrix::GetPos( pMtx) );
	}
}

void WeaponLoadBullet_Type::PlayAnim_UI_IdleB()
{
	WeaponBase::PlayAnim_UI_IdleB();
}

void WeaponLoadBullet_Type::MeleeCollision(  VEC3D * pStartPos, VEC3D * pTargetPos)
{
	
}

REAL32 WeaponLoadBullet_Type::GetAnimFireScaleTime( CHARA_ACTION_UPPER upperState, ID_UPPER_AI ai)
{
	REAL32 rTimeScale = 1.f;
	if( (m_pOwner != nullptr) && (upperState != CHARA_UPPER_SECONDARY_FIRE) )
	{
		REAL32 rFireDelay = GetFireDelay();
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
				I3PRINTLOG(I3LOG_WARN,  "nID == -1");
				return 1.f;
			}
			I3ASSERT( nID < pAIUpper->getAIStateCount());
			i3AIState * pNewAIState = pAIUpper->getAIState( nID );
			if( (pNewAIState != nullptr) && (rFireDelay > 0.f) )
			{
				CGameFramework::LoadAIStateAnimation( pNewAIState);

				rTimeScale = pNewAIState->getAnimDuration() / rFireDelay;
			}
		}
	}

	return rTimeScale;
}

REAL32 WeaponLoadBullet_Type::GetAnimLoadBulletScaleTime( CHARA_VIEW_TYPE eCAT)
{
	REAL32 rTimeScale = 1.f;
	INT8 idx = -1;

	HUMAN_TYPE type = HUMAN_MALE;

	if( m_pOwner->isFemale())	type = HUMAN_FEMALE;

	switch( eCAT)
	{
	case CHARA_VIEW_1PV:
		{
			if( m_pOwner->isFemale())
				idx = getAIIndex( WEAPON::AI_LOADBULLET1PV_FEMALE);
			else
				idx = getAIIndex( WEAPON::AI_LOADBULLET1PV_MALE);
		}
		break;
	case CHARA_VIEW_3PV:
		{
			if( m_pOwner->isFemale())
				idx = getAIIndex( WEAPON::AI_LOADBULLET3PV_FEMALE);
			else
				idx = getAIIndex( WEAPON::AI_LOADBULLET3PV_MALE);
		}
	}

	if( idx != -1)
	{
		i3AIState * pAIState = m_pAICtx->getAIState( idx);
		if( pAIState != nullptr)
		{
			CGameFramework::LoadAIStateAnimation( pAIState);

			rTimeScale = pAIState->getAnimDuration() / m_pWeaponInfo->GetLoadBulletTime();
		}
	}
	
	return rTimeScale;
}

REAL32 WeaponLoadBullet_Type::GetMoveSpeed( WEAPON_MOVE_SPEED_TYPE type)
{
	if( getFireOrder() == WEAPON::PRIMARY_FIRE)
		return WeaponBase::GetMoveSpeed( type);

	if( type == WEAPON_MOVE_SPEED_FIRE)
		type = WEAPON_MOVE_SPEED_NORMAL;

	return WeaponBase::GetMoveSpeed( type);
}

REAL32 WeaponLoadBullet_Type::GetRange()
{
	if( getFireOrder() == WEAPON::PRIMARY_FIRE)
		return WeaponBase::GetRange();
	
	return  m_pWeaponInfo->GetExtRange();
}

REAL32 WeaponLoadBullet_Type::GetDPD_Rate()	
{	// M1887 의 개머리판 공격에 DPD_Rate 값이 적용되어 CalcDamage 양이 극도로 적어지는 
	// 버그를 방지하기 위해 Ext(개머리판 공격) DPD Rate를 적용한다.
	if( getFireOrder() == WEAPON::PRIMARY_FIRE)
		return WeaponBase::GetDPD_Rate();
	
	return  m_pWeaponInfo->GetExt_DPD_Rate();
}

REAL32 WeaponLoadBullet_Type::GetDamage( bool bRandomDamage)
{
	if( getFireOrder() == WEAPON::PRIMARY_FIRE)
		return WeaponBase::GetDamage( bRandomDamage);

	return m_pWeaponInfo->GetExtDamage();
}

void WeaponLoadBullet_Type::_LoadBullet( i3AIContext * pCtx, REAL32 tm)
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

	if( pCtx->getLocalTime() > pLoadBullet->getActualDuration())
	{
		setWeaponState( WEAPON::STATE_READY);

		_OnPlayWeaponIdleAnim();

		addWeaponStateMask( WSM_LOADED_BULLET);
	}
}



