#include "pch.h"
#include "WeaponShotGun3.h"
#include "GameCharaActionContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaMoveContext.h"
#include "GameCharaWeaponContext.h"
#include "WeaponFunction.h"

I3_CLASS_INSTANCE( WeaponShotGun3);//, WeaponShotGun);

WeaponShotGun3::WeaponShotGun3()
{
	m_aii_Change_1PV = -1;
	m_aii_Change_3PV = -1;
	m_aii_LoadBullet_1PV = -1;
	m_aii_LoadBullet_3PV = -1;
	m_aii_LoadMag_Ready_1PV = -1;
	m_aii_LoadMag_Ready_3PV = -1;
	m_aii_SecondaryAttack_1PV = -1;
	m_aii_SecondaryAttack_3PV = -1;
	m_aii_UI_Rebel_DFox = -1;
	m_aii_UI_Rebel_Female  = -1;
	m_aii_UI_Rebel_Male = -1;
	m_aii_UI_Rebel_ViperRed = -1;
	m_aii_UI_SWAT_Female = -1;
	m_aii_UI_SWAT_Hide = -1;
	m_aii_UI_SWAT_Leopard = -1;
	m_aii_UI_SWAT_Male = -1;
	m_bFirstLoadMag =0;
	m_iLoadableBulletCount = -1;
	m_iLoadedBulletCount = -1;
}

WeaponShotGun3::~WeaponShotGun3()
{
}


void WeaponShotGun3::_OnInitVairable()
{
	INT32 i, j;

	WeaponShotGun::_OnInitVairable();

	m_aii_LoadMag_Ready_1PV	= -1;
	m_aii_LoadMag_Ready_3PV	= -1;
	m_aii_LoadBullet_1PV	= -1;
	m_aii_LoadBullet_3PV	= -1;
	m_aii_Change_1PV		= -1;
	m_aii_Change_3PV		= -1;
	m_aii_SecondaryAttack_1PV = -1;
	m_aii_SecondaryAttack_3PV = -1;

	m_aii_UI_Rebel_Male		= -1;
	m_aii_UI_Rebel_Female	= -1;
	m_aii_UI_Rebel_DFox		= -1;
	m_aii_UI_Rebel_ViperRed	= -1;
	m_aii_UI_SWAT_Male		= -1;
	m_aii_UI_SWAT_Female	= -1;
	m_aii_UI_SWAT_Leopard	= -1;
	m_aii_UI_SWAT_Hide		= -1;

	for( i = 0; i < BULLETCOUNT_M1887; i++)
	{
		for( j = 0; j < 3; j++)
			m_pBulletForM1887[ i][ j] = nullptr;
	}

	m_iLoadableBulletCount	= 0;
	m_bFirstLoadMag			= false;
}

void WeaponShotGun3::_OnProcessFireMiddleAnim( REAL32 rDelay)
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

void WeaponShotGun3::_OnPlayWeaponSecondFireAnim( void)
{
	if( m_pOwner != nullptr)
	{
		if( m_pOwner->is1PV())		WF::PlayWeaponAnim(this, m_aii_SecondaryAttack_1PV);
		else						WF::PlayWeaponAnim(this, m_aii_SecondaryAttack_3PV);

		return;
	}

	WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_SECFIRE));
}

void WeaponShotGun3::_OnPlayTerrainHitSound( WEAPON::FIRE_ORDER fireOrder, UINT32 type, VEC3D * pPos)
{
	if( fireOrder == WEAPON::PRIMARY_FIRE)
		WeaponBase::_OnPlayTerrainHitSound( fireOrder, type, pPos);
	else
	{	// melee attack hit sound
		g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, GTBT_WPN_M1887_HIT, pPos, false);
	}
}

void WeaponShotGun3::_OnAddTerrainEffect( WEAPON_TERRAIN_EFFECT_INFO * pInfo)
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

void WeaponShotGun3::OnBindResource()
{
	INT32 i, j;

	WeaponBase::OnBindResource();

	m_aii_LoadMag_Ready_1PV = (INT8) m_pAICtx->FindAIStateByName( "LoadMag_Ready_1PV");
	m_aii_LoadMag_Ready_3PV = (INT8) m_pAICtx->FindAIStateByName( "LoadMag_Ready_3PV");
	setAIIndex( WEAPON::AI_LOADMAGAZINE_1PV, (INT8) m_pAICtx->FindAIStateByName( "LoadMag_1PV"));
	setAIIndex( WEAPON::AI_LOADMAGAZINE_3PV, (INT8) m_pAICtx->FindAIStateByName( "LoadMag_3PV"));
	m_aii_LoadBullet_1PV	= (INT8) m_pAICtx->FindAIStateByName( "LoadBullet_1PV");
	m_aii_LoadBullet_3PV	= (INT8) m_pAICtx->FindAIStateByName( "LoadBullet_3PV");
	m_aii_Change_1PV		= (INT8) m_pAICtx->FindAIStateByName( "Change_1PV");
	m_aii_Change_3PV		= (INT8) m_pAICtx->FindAIStateByName( "Change_3PV");
	m_aii_SecondaryAttack_1PV = (INT8) m_pAICtx->FindAIStateByName( "Fire_Sec_1PV");
	m_aii_SecondaryAttack_3PV = (INT8) m_pAICtx->FindAIStateByName( "Fire_Sec_3PV");

	m_aii_UI_Rebel_Male		= (INT8) m_pAICtx->FindAIStateByName( "UI_Rebel_Male");
	m_aii_UI_Rebel_Female	= (INT8) m_pAICtx->FindAIStateByName( "UI_Rebel_Female");
	m_aii_UI_Rebel_DFox		= (INT8) m_pAICtx->FindAIStateByName( "UI_Rebel_D_Fox");
	m_aii_UI_Rebel_ViperRed	= (INT8) m_pAICtx->FindAIStateByName( "UI_Rebel_ViperRed");
	m_aii_UI_SWAT_Male		= (INT8) m_pAICtx->FindAIStateByName( "UI_SWAT_Male");
	m_aii_UI_SWAT_Female	= (INT8) m_pAICtx->FindAIStateByName( "UI_SWAT_Female");
	m_aii_UI_SWAT_Leopard	= (INT8) m_pAICtx->FindAIStateByName( "UI_SWAT_Leopard");
	m_aii_UI_SWAT_Hide		= (INT8) m_pAICtx->FindAIStateByName( "UI_SWAT_Hide");

	i3::stack_string strTemp;
	for( i = 0; i < BULLETCOUNT_M1887; i++)
	{
//		if( i < BULLETCOUNT_M1887)	sprintf_s( szTemp, "Model_VOL0%d", i + 1);
//		else sprintf_s( szTemp, "Model_VOL%d", i + 1);

		i3::sprintf(strTemp, "Model_VOL0%d", i + 1);

		for( j = 0; j < 3; j++)
		{
			if( getSceneObject()->GetBody()->getLOD( j) != nullptr)
			{
				m_pBulletForM1887[ i][ j] = i3Scene::FindNodeByName( getSceneObject()->GetBody()->getLOD( j)->getShapeNode(), strTemp.c_str());
				
				if (m_pBulletForM1887[i][j] == nullptr)
				{
					i3::stack_string strTemp2;
					i3::sprintf(strTemp2, "Model_VOL%d", i + 1);
					m_pBulletForM1887[i][j] = i3Scene::FindNodeByName(getSceneObject()->GetBody()->getLOD(j)->getShapeNode(), strTemp2.c_str());
				}
			}
		}
	}
}

void WeaponShotGun3::_VisibleBullet( bool bEnable)
{
	INT32 i, j;

	if( bEnable)
	{
		for( i = 0; i < BULLETCOUNT_M1887; i++)
		{
			for( j = 0; j < 3; j++)
			{
				if( m_pBulletForM1887[ i][ j] != nullptr)
				{
					m_pBulletForM1887[ i][ j]->RemoveFlag( I3_NODEFLAG_INVISIBLE);
				}
			}
		}
	}
	else
	{
		for( i = 0; i < BULLETCOUNT_M1887; i++)
		{
			for( j = 0; j < 3; j++)
			{
				if( m_pBulletForM1887[ i][ j] != nullptr)
				{
					m_pBulletForM1887[ i][ j]->AddFlag( I3_NODEFLAG_INVISIBLE);
				}
			}
		}
	}
}

void WeaponShotGun3::OnSetVisibleExtension( bool bVal)
{
	_VisibleBullet( bVal);
}

bool WeaponShotGun3::NotMasterLoadMagAnim(INT32 idx)
{
	return (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV)) || (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV));
}

bool WeaponShotGun3::Fire( WEAPON::FIRE_ORDER type)
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

void WeaponShotGun3::FireEnd( void)
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

void WeaponShotGun3::LoadMagazine_Ready( void)
{
	if( m_pOwner != nullptr && (m_pOwner->isLocal() || m_pOwner->isAI()))
	{
		if( getWeaponInfo()->GetTypeTrigger() != WEAPON::TRIGGER_RELOADING_SHOT)
			addWeaponStateMask( WSM_FIRE_DISABLE);

		addWeaponStateMask( WSM_LOADMAG_DISABLE );
	}

	OnPlayLoadMagReadyAnimation();
}

void WeaponShotGun3::OnPlayLoadBulletAnimation( void)
{
	INT8 idx = m_aii_LoadBullet_1PV;

	if( m_pOwner->is1PV())	idx = m_aii_LoadBullet_1PV;
	else					idx = m_aii_LoadBullet_3PV;

	_PlayWeaponAnimWithApplyTimeScale( idx);
}

void WeaponShotGun3::OnPlayLoadMagAnimation( void)
{
	//if( eCAT == CHARA_VIEW_1PV)	_PlayWeaponAnimWithApplyTimeScale( m_aii_LoadMagazine_1PV );
	//else							_PlayWeaponAnimWithApplyTimeScale( m_aii_LoadMagazine_3PV );

	//setWeaponState( WEAPON::STATE_LOADMAG);
	WeaponBase::OnPlayLoadMagAnimation();
}

void WeaponShotGun3::OnPlayChangeAniImpl( REAL32 rTimeScale /*= 1.f*/)
{
	if( g_pFramework->IsBattleStage() )
	{
		CHARA_VIEW_TYPE type = CHARA_VIEW_1PV;

		if( m_pOwner != nullptr)
			type = m_pOwner->getViewType();
		
		if( type == CHARA_VIEW_1PV)
		{
			if (m_pOwner)
				m_pOwner->Cmd_Only_ChangeWeaponAction();	// revision 35847 인게임에 처음 입장할 때 총기를 꺼내는 애니메이션이 어색한 버그수정
			WF::PlayWeaponAnim( this, m_aii_Change_1PV, rTimeScale );
		}
		else						WF::PlayWeaponAnim( this, m_aii_Change_3PV, rTimeScale );
	}
}

void WeaponShotGun3::OnPlayLoadMagReadyAnimation( void)
{
	if( m_pOwner->is1PV())	_PlayWeaponAnimWithApplyTimeScale( m_aii_LoadMag_Ready_1PV);
	else					_PlayWeaponAnimWithApplyTimeScale( m_aii_LoadMag_Ready_3PV);
}

void WeaponShotGun3::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	WeaponShotGun::OnIdle( pCtx, tm);

	m_bFirstLoadMag = true;
	removeWeaponStateMask( WSM_READY_TO_LOADMAG);
	m_FireOrder = WEAPON::PRIMARY_FIRE;

}

void WeaponShotGun3::OnLoadBullet( i3AIContext * pCtx, REAL32 tm)
{
	WeaponShotGun::OnLoadBullet( pCtx, tm);
}

bool WeaponShotGun3::OnLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	bool bLoaded = false;

	bLoaded = WeaponShotGun::OnLoadMag( pCtx, tm);

	if( pCtx->getLocalTime() == 0.f && m_bFirstLoadMag)
	{
		m_iLoadedBulletCount = -1;

		m_iLoadableBulletCount = GetLoadBulletWithCashItem() - getLoadedBulletCount();
		if( m_iLoadableBulletCount > BULLETCOUNT_M1887)
			m_iLoadableBulletCount = BULLETCOUNT_M1887;

		_VisibleBullet( true);
		/*
		for(i = 0; i < BULLETCOUNT_M1887; i++)
		{
			for( j = 0; j < 3; j++)
			{
				if( m_pBulletForM1887[i][ j] != nullptr)
				{
					m_pBulletForM1887[i][ j]->AddFlag( I3_NODEFLAG_INVISIBLE );
				}
			}
		}
		*/

		m_bFirstLoadMag = false;
	}
	else if( pCtx->getLocalTime() > 0.15f)
	{
		if( m_iLoadedBulletCount == -1)
		{
			/*
			for(i = 0; i < m_iLoadableBulletCount; i++)
			{
				for( j = 0; j < 3; j++)
				{
					if( m_pBulletForM1887[i][ j] != nullptr)
					{
						m_pBulletForM1887[i][ j]->RemoveFlag( I3_NODEFLAG_INVISIBLE );
					}
				}
			}
			*/
			m_iLoadedBulletCount = 0;
		}

		/*
		for( i = BULLETCOUNT_M1887 - 1; i > m_iLoadedBulletCount; i--)
		{
			for( j = 0; j < 3; j++)
			{
				if(m_pBulletForM1887[i][ j] != nullptr)
				{
					m_pBulletForM1887[i][ j]->AddFlag( I3_NODEFLAG_INVISIBLE );
				}
			}
		}
		*/
	}

	if( bLoaded && m_iLoadedBulletCount != -1)
	{
		m_iLoadedBulletCount++;
	}

	return bLoaded;
}

void WeaponShotGun3::OnSecondaryFire( i3AIContext * pCtx, REAL32 tm)
{
	if( m_pOwner == nullptr)
	{
		ReturnToReady();
		return;
	}

	setWeaponState( WEAPON::STATE_FIRE);

	// 개머리판 공격중에는 장전 안됨.
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
		// 개머리판 공격중에는 장전 안됨.
		if( getFireOrder() == WEAPON::SECONDARY_FIRE)
		{
			removeWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
		}

		ReturnToReady();

		m_FireOrder = WEAPON::PRIMARY_FIRE;
	}
}

void WeaponShotGun3::OnSwaping( i3AIContext * pCtx, REAL32 tm)
{
	addWeaponStateMask( WSM_LOADED_BULLET);

	WeaponBase::OnSwaping( pCtx, tm);
}

void WeaponShotGun3::OnLoadMag_Ready( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnLoadMag_Ready( pCtx, tm);

	I3ASSERT( pCtx != nullptr);
	REAL32 localTime = pCtx->getLocalTime();

	i3AIState * pAIState = pCtx->getCurrentAIState();
	I3ASSERT( pAIState != nullptr);
	
	if( localTime + tm > pAIState->getDuration() )
	{
		setWeaponState( WEAPON::STATE_NEEDTO_LOADMAG);
		addWeaponStateMask( WSM_READY_TO_LOADMAG);
	}
}

void WeaponShotGun3::MeleeCollision(  VEC3D * pStartPos, VEC3D * pTargetPos)
{
	INT32 collGroup;

	if( m_pOwner == nullptr )		return;
	
	collGroup = CGRP_MASKGROUP_ATTACK;
	if( m_pOwner->getCharaTeam() & CHARACTER_TEAM_RED)	collGroup = CGRP_MASKGROUP_CHARA_BLUE;
	else												collGroup = CGRP_MASKGROUP_CHARA_RED;

	// 네트워크 싱크를 맞추기 위해 강제로 보조 공격으로 세팅합니다. by 정순구 2009.04.23
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
	
	/*GlobalFunc::PB_TRACE("m_StartPos %f %f %f m_TargetPos %f %f %f GetPos %f %f %f", m_StartPos.x, m_StartPos.y, m_StartPos.z, m_TargetPos.x, m_TargetPos.y, m_TargetPos.z,
												m_pOwner->GetPos()->x, m_pOwner->GetPos()->y, m_pOwner->GetPos()->z);*/
	//	collision line
	{	
		i3CollideeLine col;

		col.SetStart( m_StartPos);
		col.SetEnd( m_TargetPos );

		bool	bWarningOwner = false;

		_DoFireCollisionKnife( &col, collGroup, &bWarningOwner);
		
	}
}

REAL32 WeaponShotGun3::GetAnimFireScaleTime( CHARA_ACTION_UPPER upperState, ID_UPPER_AI ai)
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

REAL32 WeaponShotGun3::GetAnimLoadBulletScaleTime( CHARA_VIEW_TYPE eCAT)
{
	REAL32 rTimeScale = 1.f;
	INT32 idx = m_aii_LoadBullet_1PV;

	if( eCAT == CHARA_VIEW_1PV)	idx = m_aii_LoadBullet_1PV;
	else							idx = m_aii_LoadBullet_3PV;

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

REAL32 WeaponShotGun3::GetMoveSpeed( WEAPON_MOVE_SPEED_TYPE type)
{
	if( getFireOrder() == WEAPON::PRIMARY_FIRE)
		return WeaponBase::GetMoveSpeed( type);

	if( type == WEAPON_MOVE_SPEED_FIRE)
		type = WEAPON_MOVE_SPEED_NORMAL;

	return WeaponBase::GetMoveSpeed( type);
}

REAL32 WeaponShotGun3::GetRange()
{
	if( getFireOrder() == WEAPON::PRIMARY_FIRE)
		return WeaponBase::GetRange();
	
	return  m_pWeaponInfo->GetExtRange();
}

REAL32 WeaponShotGun3::GetDPD_Rate()	
{	// M1887 의 개머리판 공격에 DPD_Rate 값이 적용되어 CalcDamage 양이 극도로 적어지는 
	// 버그를 방지하기 위해 Ext(개머리판 공격) DPD Rate를 적용한다.
	if( getFireOrder() == WEAPON::PRIMARY_FIRE)
		return WeaponBase::GetDPD_Rate();
	
	return  m_pWeaponInfo->GetExt_DPD_Rate();
}

REAL32 WeaponShotGun3::GetDamage( bool bRandomDamage)
{
	if( getFireOrder() == WEAPON::PRIMARY_FIRE)
		return WeaponBase::GetDamage( bRandomDamage);

	return m_pWeaponInfo->GetExtDamage();
}



