#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaPlayer.h"
#include "StageBattle.h"
#include "Weapon.h"
#include "Crosshair_Cross.h"

//#define PHYSIX_CHARA_SLEEP_TIME		3.f
//#define CAMERA_CHARA_SLEEP_TIME		1.f
//#define PHYSIX_EFFECT_DUTY_FORCE	0.f
//#define HUD_CROSSNICKCHECK_TIME		0.0666f	// 크로스헤어 표시 검색( 초당 15회 )


//#define		SPROFILE(a)			( m_timeProfile[a].m_timeStart = m_pTimer->GetProfileTime())
//#define		EPROFILE(a)			{ m_timeProfile[a].m_timeEnd += m_pTimer->GetProfileTime() - m_timeProfile[a].m_timeStart; m_timeProfile[a].m_bValid = true; }

I3_CLASS_INSTANCE( CGameCharaPlayer, CGameCharaBase );

CGameCharaPlayer::CGameCharaPlayer( void)
{
	OnInitVariable();
}

CGameCharaPlayer::~CGameCharaPlayer( void)
{	
	//DetachCollisionNode();
	//Destroy();
}

//void CGameCharaPlayer::Destroy( void)
//{
//	INT32 i;
//	if( getWeaponContext() != NULL)
//	{
//		for( i = 0; i < MAX_ITEMSLOT_COUNT; i++)
//		{
//			getWeaponContext()->getControl()->OnDeleteWeapon( i);
//		}
//	}

//	if( getEquipContext() != NULL)
//	{
//		for( i = 0; i < EQUIP_CLASS_TYPE_COUNT; i++)
//		{
//			getEquipContext()->getControl()->OnDeleteEquip( (EQUIP_CLASS_TYPE) i);
//		}
//	}

	//for( i = 0; i < CHARA_SOUND_COUNT; i++)
	//{
	//	I3_SAFE_RELEASE( m_pCharaSoundRes[ i]);
	//}

	//if( m_pContactHitList != NULL)
	//{
	//	for( i = 0; i < m_pContactHitList->GetCount(); i++ )
	//	{
	//		i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) m_pContactHitList->GetItem( i);
	//		I3_SAFE_RELEASE( pShapeSet);
	//	}
	//	I3_SAFE_RELEASE( m_pContactHitList);
	//}
//}

//void CGameCharaPlayer::OnInitVariable( void)
//{	
//	CGameCharaBase::OnInitVariable();

	//m_pDBInfo				= NULL;
	//m_pCharaInfo			= NULL;
	
	//for( i = 0; i < EQUIP_CLASS_TYPE_COUNT; i++)
	//{
	//	m_pCharaEquip[ i] = NULL;
	//	m_pEquipInfo[ i] = NULL;
	//}

//	m_Theta					= 0.0f;
//	m_Rho					= 0.0f;

//	m_RecoilData.Reset();
//	m_DeviationData.Reset();
//	m_ScopeWaveData.Reset();

//	m_bValidPivot			= false;		
//	m_PivotYaw				= 0.0f;
//	m_PivotYawTarget		= 0.0f;
//	m_timeYawBlend			= 0.0f;
//	m_timeTargetYawBlend	= 0.0f;
//	m_PivotRoll				= 0.0f;
//	i3Vector::Zero( &m_PivotAxis);

//	i3Matrix::Identity( &m_mtxPivot);
//	for( i = 0; i < BONE_MAX; i++)
//	{
//		m_idxBone[ i] = -1;
//		i3Matrix::Identity( &m_Bone[i]);
//	}

	//m_rIKHeight		= 0.0f;
	
//	m_CurFloorTerrainType		= (I3_TERRAIN_TYPE) 0;
//	i3Physix::HitResultClear( &m_CrossHitResult );
//	m_CrossHitLine.SetStart( i3Matrix::GetPos( GetMatrix() ) );
//	m_CrossHitLine.SetDir( i3Matrix::GetAt( GetMatrix() ) );
//	m_rCrossHitTimer			= 0.f;
//	m_pCrossHitChara			= NULL;

	//m_pContactHitList			= NULL;
	
//	m_rContactTime				= 0.f;
//	m_nContactReportCount		= 0;
//	m_nContactReportReadPos		= 0;
//	for( i = 0; i < PHYSIX_CONTACT_REPORT_COUNT; i++ )
//	{
//		i3Physix::ContactReportClear( &m_ContactReportBuf[ i ] );
//	}
//	for( i = 0; i < 6; i++)
//	{
//		m_bContactBones[ i ]	= FALSE;
	//	i3Vector::Zero( &m_vBonesForceDir[ i]);
	//}
	
	//m_nCollisionGroup			= 0;

	//m_pRoughHitTrans			= NULL;

	//m_DropCount = 0;

	//m_nScrEffectType	= SCR_EFFECT_NONE;
	//i3Color::Set( &m_EFFECT_ScreenColor, 0, 0, 0, 0);
	//m_fBlurStart		= 0.0f;
	//m_fBlurEnd			= 0.0f;

//#if defined( I3_DEBUG )
//	m_fDebugtimeStart	= 0;
//	m_fDebugtimeEnd		= 0;
//	i3Vector::Zero( &m_vDebugPos );
//	i3Vector::Zero( &m_vDebugOriginalPos );
//	i3Vector::Zero( &m_vDebugTargetPos);
//#endif

	//m_nAccessory		= 0;
	//m_fRunDuration		= 0.666f;

//	m_velShove = m_acclShove = m_tShove = 0.0f;
//	i3Matrix::Identity( &m_ShoveMatrix);
//	i3Quat::Identity( &m_quatShoveStart);
//	m_bShoveActive = false;
//}

/*
void CGameCharaPlayer::Create( i3GameResChara * pRes, CHARA_INFO * pCharaInfo, const CCharaInfo * pDBInfo)
{	
	__SLOG2( g_pGameContext->GetNickForSlot( pCharaInfo->_NetIdx));

	CGameCharaBase::Create( pRes, pCharaInfo, pDBInfo);
	//	Add after here

	m_pCharaInfo = pCharaInfo;
	m_pDBInfo = pDBInfo;

	//_InitBones();

	// Update Timing을 조정하기 위해, 수동으로 Animation, World-Transform 등을
	// 하도록 한다.
	{
		// Animation Update와 World-Space Cache Matrix에 대한 시간차를
		// 없애기 위해, 수동으로 Update를 한다.
		
		if( g_pGameContext->IsBattle() )
		{
			m_pBody->addStyle( I3SG_MANUAL_ANIM_UPDATE | I3SG_MANUAL_TRANSFORM);
			GetSceneObject()->AddFlag( I3_NODEFLAG_MANUAL_UPDATE);

			// 이 때, Animation Update와 World-Space Transform 사이에 LOD 계산에서
			// Current Level이 다를 수 있기 때문에...
			// 수작업으로 LOD를 계산한다.
			m_pBody->removeStyle( I3SG_LOD_AUTO);

			//m_pContactHitList = i3List::NewObject();
			//I3ASSERT( m_pContactHitList != NULL);
		}
	}
			
	//_InitCharaSound();		//	사운드 로드	
	
	getCollisionContext()->AttachCollisionNode( GetSceneObject(), 0);

	// 무기 등록	
	// ! praptor - getMySlotIdx -> GetMySlotIdxEx 교체했습니다.
	// 방 이외에 장소에서도 호출될때 idx를 0값으로 설정해 놓지만,
	// 몇몇 경우(RoomInfo초기화 등)에 -1로 설정될지도릅니다.

	NET_CHARA_INFO * pNetCharaInfo;
	if( pCharaInfo->_NetIdx != MAX(0, g_pGameContext->GetMySlotIdxEx()) )		
		pNetCharaInfo  = g_pGameContext->getNetCharaInfo( pCharaInfo->_NetIdx );
	else		
		pNetCharaInfo = g_pGameContext->GetMyCharaInfo();

	g_pCharaManager->SetCharaWeaponSet( this, pNetCharaInfo);

	//	헬멧을 등록합니다.
	INT32 helmet = 0;

#if defined( I3_DEBUG)
	if( !g_pConfig->m_bNetworking)
	{
		switch( pCharaInfo->_ResType )
		{
		case CHARARES_REBEL_MALE :			helmet = EQUIPRES_NUM_HELMET_REBEL_M;		break;
		case CHARARES_SWAT_MALE :			helmet = EQUIPRES_NUM_HELMET_SWAT_M;		break;
		case CHARARES_REBEL_FEMALE :		helmet = EQUIPRES_NUM_HELMET_REBEL_F;		break;
		case CHARARES_SWAT_FEMALE :			helmet = EQUIPRES_NUM_HELMET_SWAT_F;		break;
		}
		m_pEquipContext->setEquipInfo( EQUIP_CLASS_TYPE_HELMET, g_pCharaInfoDataBase->getEquipInfo( EQUIPRES_TYPE_DEFAULT));
		if( m_pEquipContext->getControl()->OnCreateEquip( EQUIP_CLASS_TYPE_HELMET, helmet))
		{
			CGameCharaEquip * pEquip = m_pEquipContext->getControl()->getEquip( EQUIP_CLASS_TYPE_HELMET);
			if( getCharaTeam() & CHARACTER_TEAM_RED )
			{
				pEquip->SetCollisionGroup( CGRP_TEAM_RED, CGRP_MASK_TEAM_RED );
			}
			else
			{
				pEquip->SetCollisionGroup( CGRP_TEAM_BLUE, CGRP_MASK_TEAM_BLUE );
			}
		}
	}
#endif

	EQUIPRES_TYPE equipType = EQUIP_NUMBER( pNetCharaInfo->_EquipCharacter._nohair);
	switch( equipType )
	{
	case EQUIPRES_TYPE_DEFAULT :
		switch( pCharaInfo->_ResType )
		{
		case CHARARES_REBEL_MALE :			helmet = EQUIPRES_NUM_HELMET_REBEL_M;		break;
		case CHARARES_SWAT_MALE :			helmet = EQUIPRES_NUM_HELMET_SWAT_M;		break;
		case CHARARES_REBEL_FEMALE :		helmet = EQUIPRES_NUM_HELMET_REBEL_F;		break;
		case CHARARES_SWAT_FEMALE :			helmet = EQUIPRES_NUM_HELMET_SWAT_F;		break;
		}
		break;
	}

	m_pEquipContext->setEquipInfo( EQUIP_CLASS_TYPE_HELMET, g_pCharaInfoDataBase->getEquipInfo( equipType));
	
	if( m_pEquipContext->getControl()->OnCreateEquip( EQUIP_CLASS_TYPE_HELMET, helmet))
	{
		CGameCharaEquip * pEquip = m_pEquipContext->getControl()->getEquip( EQUIP_CLASS_TYPE_HELMET);
		if( getCharaTeam() & CHARACTER_TEAM_RED )
		{
			pEquip->SetCollisionGroup( CGRP_TEAM_RED, CGRP_MASK_TEAM_RED );
		}
		else
		{
			pEquip->SetCollisionGroup( CGRP_TEAM_BLUE, CGRP_MASK_TEAM_BLUE );
		}
	}

	
	//	클랜 마크 설정
	SetCharaClanTexture( 0);

	ResetChara();		//	최종 클래스에서 불러야 한다.

	__ELOG2( g_pGameContext->GetNickForSlot( pCharaInfo->_NetIdx));
}
*/

//void CGameCharaPlayer::CreateInLobby( i3GameResChara * pRes, CHARA_INFO * pCharaInfo, const CCharaInfo * pDBInfo)
//{
//	Create(pRes, pCharaInfo, pDBInfo);
	
//	m_pUpdateContext->OnChangeProfile();

	//	로비 애니메이션으로 플레이
	//ResetLobbyAnim();
//}

//void CGameCharaPlayer::ResetChara()
//{
//	CGameCharaBase::ResetChara();

//	INT32 i;
	
//	m_Theta			= 0.0f;
//	m_Rho			= 0.0f;
	
//	m_PivotYaw		= 0.0f;
//	m_PivotRoll		= 0.0f;

	//m_fJumpDelayElapsedTime = 0.f;
	//m_rFlashElapseTime		= 0.f;
	//m_rFlashEffectiveTime	= 0.f;
	//m_rDamageElapseTime		= 0.f;
	//m_rRespawnElapseTime	= 0.f;
	//m_rInvincibleElapseTime	= 0.f;
	
	//m_rContactTime			= 0.f;
	//for( i = 0; i < 6; i++)
	//{
	//	m_bContactBones[ i] = FALSE;
	//	i3Vector::Zero( &m_vBonesForceDir[ i]);
	//}

	//m_fLeftStepTime			= 0.0f;
	//m_fRightSteptime		= 0.0f;

	//m_pPurchasePrimWeapon = NULL;

	//m_DropCount = 0;

	//	캐릭터에 붙이기 위해 현재 붙어 있는 모든 무기를 떼어낸다
	//for( i = 0; i < MAX_ITEMSLOT_COUNT; ++i)
	//{
	//	getWeaponContext()->getControl()->OnDetachWeapon( i);
	//}

	//	무기 초기화
	//getWeaponContext()->getControl()->OnResetWeapon();
	//--> WeaponControl
	/*
	{
		BOOL bCheck = FALSE;
		i = 0;

		while( !bCheck )
		{
			if( WEAPON_Select( i) )
			{
				break;
			}

			i++;
#if defined( I3_DEBUG )
			if( i >= MAX_ITEMSLOT_COUNT )
			{
				I3ASSERT_0;	// invalid server infomation for weapon list
			}
#endif
		}

		if( getWeaponContext()->getCurrent() != NULL )
		{
			getWeaponContext()->getCurrent()->ReturnToReady();
		}

		m_nSwapWeaponSlotNum = m_nOldWeaponSlotNum = m_nCurWeaponSlotNum;

		for( i = 0; i < MAX_ITEMSLOT_COUNT; ++i)
		{
			WeaponBase * pWeapon = getWeaponContext()->getWeapon( i);

			if( pWeapon != NULL)
				pWeapon->Reset();
		}

		//	무기는 기본 오른손에 부착
		ChangeWeaponAttachHand( CHARA_WEAPON_ATTACH_RIGHT_HAND);

		getWeaponContext()->getControl()->SetBlendAlpha( getWeaponContext()->getCurrentSlotNum(), 1.0f);
	}

	//	장비 초기화 
	{
		for( i = 0; i < EQUIP_CLASS_TYPE_COUNT; i++)
		{
			EQUIP_AttachChara( (EQUIP_CLASS_TYPE) i, TRUE);
		}

		EQUIP_SetBlendAlphaAll( 1.0f, TRUE);
	}
	*/
	
	//ResetDefaultInfo();					//	Stat 초기화

	//ResetDefaultAnim( !is1PV());		//	기본 디폴트 Idle Anim으로 초기화
	//CALL_ACTION_COMMAND( ACTION_CMD_RESET, NULL);

	//	Meterial Alpha 초기화해야 나옵니다.
	//SetBlendAlpha( 1.f );
	/*
	{
		i3Color::SetA( m_pMaterialAttr->GetAmbient(), 1.0f);
		i3Color::SetA( m_pMaterialAttr->GetDiffuse(), 1.0f);
		i3Color::SetA( m_pMaterialAttr->GetSpecular(), 1.0f);
	}
	*/

	//StopCharaSoundAll();

	//m_bRightStepSound = FALSE;
	//m_fRunElapseTime = 0.0f;

//	m_CurFloorTerrainType = (I3_TERRAIN_TYPE) 0;
	/*
	if( m_pController != NULL )
	{
		NxController * pNxController = m_pController->getNxController();
		if( pNxController == NULL )
		{
			I3WARN( "Controller is NULL." );
			return;
		}

		NxActor * pAct = pNxController->getActor();
		if( pAct == NULL )
		{
			I3WARN( "Actor is NULL." );
			return;
		}

		pAct->clearBodyFlag( NX_BF_DISABLE_GRAVITY );
	}
	*/

	//m_bClimbingLadder	= false;
	
//	m_nContactReportCount = 0;
//	m_nContactReportReadPos = 0;
//}

/*
void CGameCharaPlayer::_ProcessOldWeapon( void)
{
	
	if( m_nSwapWeaponSlotNum == m_nCurWeaponSlotNum)
		return;

	BOOL bInStage = FALSE;
	i3Stage* pStage = g_pFramework->GetCurrentStage();
	if (g_pFramework->IsBattleStage())
	{
		bInStage = TRUE;
	}

	WeaponBase * pOldWeapon = getWeaponContext()->getWeapon( m_nCurWeaponSlotNum );
	pOldWeapon->ReturnToReady();	//	무기 교체시 이전 무기는 Ready 상태로 변경
	pOldWeapon->GetSceneObject()->GetBody()->StopAnim();	// 무기 변경시 애니메이션을 정지시켜야 합니다.
	pOldWeapon->GetSceneObject()->AddFlag( I3_NODEFLAG_DISABLE );
	// 이전 무기는 공중으로 보내줍니다.
	VEC3D vec;
	i3Vector::Set( &vec, (REAL32)(getCharaNetIndex()) * 10.f, PHYSIX_HEIGHT_WEAPON, (REAL32)(m_nCurWeaponSlotNum) * 10.f );
	g_pWeaponManager->MoveShapePosition( pOldWeapon, &vec );
	
	//	애니메이션 하기 전에 먼저 무기 교체(모델링)을 해야 한다.		
	WEAPON_Select( m_nSwapWeaponSlotNum);

	// HUD
	if( bInStage)
	{
		CHud * pHud = ((CStageBattle*)pStage)->getHud();
		if( pHud != NULL )
		{	
			pHud->setOldLoadedBullet( 0 );
		}
	}

	// 주 무기인 경우 백팩에 붙입니다.
	if( !is1PV() )
	{
		WEAPON_AttachToBackpack( m_nOldWeaponSlotNum);
		WEAPON_AttachToSidepack( m_nOldWeaponSlotNum);
	}

	if( isNetwork() )
	{
		if( getWeaponContext()->getCurrent() != NULL )
		{
			getWeaponContext()->getCurrent()->NET_WeaponChange();
		}
	}
	else
	{
		if( getWeaponContext()->getCurrent() != NULL )
		{
			getWeaponContext()->getCurrent()->WeaponChange();		//	무기 교체시 이후 무기는 Change 상태로
		}
	}
}

//void CGameCharaPlayer::OnUpdate( REAL32 tm)
//{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
	// 이하의 처리 순서는 매우 중요하고, 변경할 때는 숙고가 필요하다.
	//CGameCharaBase::OnUpdate( tm);
	//	Add code here

	//m_fDebugtimeStart = g_pViewer->GetTimer()->GetProfileTime();

 	//_ProcessChangePose( tm);	// Animation 적용 처리
	//m_pCameraContext->ProcessChangePose( tm);

	BOOL bInStage = FALSE;

	i3Stage* pStage = g_pFramework->GetCurrentStage();
	if (g_pFramework->IsBattleStage())
	{
		bInStage = TRUE;
	}

	if( !isNetwork() && bInStage)
	{
		m_pMoveContext->Move( tm);
		m_pMoveContext->UpdateSpeedReduction( tm);
	}

	i3Chara::OnUpdate( tm);		// 이동 속도 반영 및 AIState Drive
	
	//	1인칭일 경우에 하체 aistate의경우는 호출되지 않으므로 필요한경우 강제 호출해주도록 하는 함수
	if( is1PV())
	{
		if( !isNetwork())
		{	
			if( getActionContext()->isLower( CHARA_LOWER_JUMP) ||
				!isStateMask( CHARA_STATE_FLOOR) || isStateMask( CHARA_STATE_JUMP))
			{
				OnJumpStartRun( m_pAnimContext->getAILower(), tm);
			}
			else if( getActionContext()->isLower( CHARA_LOWER_MOVE) && isStateMask( CHARA_STATE_FLOOR))
			{
				OnMoveRun( m_pAnimContext->getAILower(), tm);
			}
			else if( getActionContext()->isLower( CHARA_LOWER_WALK) && isStateMask( CHARA_STATE_FLOOR))
			{	
				OnMoveRun( m_pAnimContext->getAILower(), tm);
			}
		}
		else
		{
			if( getActionContext()->isLower( CHARA_LOWER_JUMP) )
			{
				OnJumpStartRun( m_pAnimContext->getAILower(), tm);
			}
			else if( getActionContext()->isLower( CHARA_LOWER_MOVE))
			{
				OnMoveRun( m_pAnimContext->getAILower(), tm);
			}
			else if( getActionContext()->isLower( CHARA_LOWER_WALK))
			{	
				OnMoveRun( m_pAnimContext->getAILower(), tm);
			}
		}
	}

	getWeaponContext()->getControl()->OnUpdate( tm);		// 차후에 업데이트 쪽으로 옮겨야 합니다. dfly 20080130
	
	// Animation과 World-Space Transform 사이에 LOD 계산이 달라질 수 있으며,
	// 그것은 Visual 상에 여러 문제가 될 수 있기 때문에
	// 수동으로 LOD를 계산시킨다.
	if( g_pGameContext->IsP2PActivate() )	
	{
		WeaponBase * pWeapon = getWeaponContext()->getCurrent();
		
		if( is1PV() )//&& (!isNetwork() || bFPSView))
		{	// 1인칭 캐릭터
			m_pBody->setCurrentLevel( 0);

			if( pWeapon != NULL)
			{
				pWeapon->setCurrentLOD( 0);
			}
		}
		else
		{	// 3인칭 캐릭터
			//	Zoom시 FOV 비율 계산
			REAL32 fRatio = g_pCamera->GetCemeraDefaultFOV() / g_pCamera->GetCameraFOV();

			//	모델링 LOD 계산
			calcLOD( GetPos(), i3Matrix::GetPos( g_pCamera->getCamMatrix()), fRatio); 
		}
	}

	{
		m_pBoneContext->UpdateAnim( tm);
		//_UpdateAnim( tm);			// Animation의 Key 값을 적용 (각 Bone의 Cache Matrix에 저장)

		m_pBoneContext->CalcTheta( tm);
		//_calcTheta( tm);				// Position

		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
		// 이하에서는 GetSceneObject()의 Matrix는 변경되어선 안된다.
		// 만약 변경된다면, Bone 계산에서 변경된 만큼의 오차가 날 수 있다.
		//

		// SceneObject, AxisRotate, IK, Spine1, Spine2, Spine3 들의 World-Space Matrix를 계산
		// 

		m_pBoneContext->CalcSpineBones();
		//_calcSpineBones();

		//	Rho 
		m_pBoneContext->CalcRecoil( tm);
		m_pBoneContext->CalcRho( tm);
		//_calcRecoil( tm);			// 독립적
		//_calcRho( tm);				// IK Bone ( IK Cache Matrix)
		
		if( g_pGameContext->IsP2PActivate() )	
		{
			m_pBoneContext->TransformWorldSpace( tm);
			//_TransformWorldSpace( tm);
			m_pMaterialContext->DriveLightVolume( tm);
			//_DriveLightVolume( tm);
			//_CAMERA_UpdateState();
			m_pCameraContext->OnUpdate( tm);
		}
	}

	if( !isAlive() )
	{
		//_ProcessDownEffect();
		ProcessDownEffect();

		if( isStateMask( CHARA_STATE_PHYSIXCONTACT) )
		{
			m_rContactTime += tm;

			if( !isStateMask( CHARA_STATE_PHYSIXFREEZE) && m_rContactTime >= PHYSIX_CHARA_SLEEP_TIME )
			{	
				bool bAllSleep = true;

				for( INT32 i = 0; i < m_PhysixShapeSetList.GetCount(); i++)
				{
					i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) m_PhysixShapeSetList.GetItem( i);

					NxActor * pActor = pShapeSet->getNxActor();
					if( pActor != NULL && pActor->isDynamic() )
					{
						if( !pActor->isSleeping() )
						{
							bAllSleep = false;
							pActor->putToSleep();
						}
					}
				}
				
				if( bAllSleep )
				{
					addStateMask( CHARA_STATE_PHYSIXFREEZE);
				}
			}
		}
	}

	//	캐릭터 모델링 알파 처리
	m_pMaterialContext->ProcessMaterial( tm);
	//_ProcessMaterial( tm);

	//	사용자의 HUD 화면 이펙트 
	//_ProcessScreenEffect( tm);

	//_UpdateCharaSound( tm);

	//char szTemp[ 256 ];
	//m_fDebugtimeEnd = g_pViewer->GetTimer()->GetProfileTime() - m_fDebugtimeStart;

	//sprintf( szTemp, "CharaUpdate %d: %.4f\n", this->getCharaNetIndex(), m_fDebugtimeEnd * i3Timer::GetProfileResolution() );
	//strcat( g_pFramework->GetViewer()->getDebugText(), szTemp );
//}
/*
void CGameCharaPlayer::_ProcessDownEffect( void)
{
	VEC3D vec, vUp;

	i3Vector::Set( &vUp, i3Math::Randf2(), i3Math::Randf()+ 0.1f, i3Math::Randf2());
	i3Vector::Normalize( &vUp, &vUp);

	i3Skeleton * pSkel = m_pBody->getFrameLOD()->getSkeleton();
	i3PhysixShapeSet * pShapeSet = NULL;
	REAL32 force = 0.f;
	REAL32 dot = 0.f;

	if( !m_bContactBones[ 0] )
	{	// Root
		pShapeSet = pSkel->getShapeSet( m_pBoneContext->getBoneIndex( BONE_ROOT) );
		pShapeSet->getLinearVelocity( &vec);

		force = i3Vector::LengthSq( &vec);

		if( force >0.f)
		{
			dot = i3Vector::Dot( &m_vBonesForceDir[ 0], &vec);

			if( dot < 0.3f && force < 0.2f)
			//if( force <= 0.01f && force > 0.f)
			{
				//pShapeSet->getShapeGlobalPose( &mat);
				//g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_CHARA_DOWN_COMMON, i3Matrix::GetPos( &mat ), &vUp );
				addStateMask( CHARA_STATE_PHYSIXCONTACT);
				m_bContactBones[ 0] = TRUE;
			}

			i3Vector::Copy( &m_vBonesForceDir[ 0], &vec);
		}
	}

	if( !m_bContactBones[ 1] )
	{	// Spine3
		pShapeSet = pSkel->getShapeSet( m_pBoneContext->getBoneIndex( BONE_SPINE3) );
		pShapeSet->getLinearVelocity( &vec);

		force = i3Vector::LengthSq( &vec);

		if( force > 0.f )
		{
			dot = i3Vector::Dot( &m_vBonesForceDir[ 1], &vec);
			if( dot < 0.3f && force < 0.2f)
			{
				//pShapeSet->getShapeGlobalPose( &mat);
				//g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_CHARA_DOWN_COMMON, i3Matrix::GetPos( &mat ), &vUp );
				addStateMask( CHARA_STATE_PHYSIXCONTACT);
				m_bContactBones[ 1] = TRUE;
			}

			i3Vector::Copy( &m_vBonesForceDir[ 1], &vec);
		}
	}

	
	if( !m_bContactBones[ 2] )
	{	// L forearm
		pShapeSet = pSkel->getShapeSet( m_idxBone[ BONE_LDARM ] );
		pShapeSet->getLinearVelocity( &vec);
		force = i3Vector::LengthSq( &vec);
		if( force <= 0.01f && force > 0.f)
		{
			pShapeSet->getShapeGlobalPose( &mat);
			g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_CHARA_DOWN_COMMON, i3Matrix::GetPos( &mat ), &vUp );
			m_bContactBones[ 2] = TRUE;
		}
	}

	if( !m_bContactBones[ 3] )
	{	// R forearm
		pShapeSet = pSkel->getShapeSet( m_idxBone[ BONE_RDARM ] );
		pShapeSet->getLinearVelocity( &vec);
		force = i3Vector::LengthSq( &vec);
		if( force <= 0.01f && force > 0.f)
		{
			pShapeSet->getShapeGlobalPose( &mat);
			g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_CHARA_DOWN_COMMON, i3Matrix::GetPos( &mat ), &vUp );
			m_bContactBones[ 3] = TRUE;
		}
	}

	if( !m_bContactBones[ 4] )
	{
		// R forearm
		pShapeSet = pSkel->getShapeSet( m_idxBone[ BONE_LFOOT ] );
		pShapeSet->getLinearVelocity( &vec);
		force = i3Vector::LengthSq( &vec);
		if( force <= 0.01f && force > 0.f)
		{
			pShapeSet->getShapeGlobalPose( &mat);
			g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_CHARA_DOWN_COMMON, i3Matrix::GetPos( &mat ), &vUp );
			m_bContactBones[ 4] = TRUE;
		}
	}

	if( !m_bContactBones[ 5] )
	{	// R forearm
		pShapeSet = pSkel->getShapeSet( m_idxBone[ BONE_RFOOT ] );
		pShapeSet->getLinearVelocity( &vec);
		force = i3Vector::LengthSq( &vec);
		if( force <= 0.01f && force > 0.f)
		{
			pShapeSet->getShapeGlobalPose( &mat);
			g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_CHARA_DOWN_COMMON, i3Matrix::GetPos( &mat ), &vUp );
			m_bContactBones[ 5] = TRUE;
		}
	}
}


#define LOD_SLOPE_VALUE		3.5f

void CGameCharaPlayer::calcLOD( VEC3D * pPos, VEC3D * pCamPos, REAL32 fFOVRatio)
{
	//	캐릭터 LOD
	m_pBody->CalcLOD( pPos, pCamPos, fFOVRatio);

	//	무기 LOD
	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon != NULL)
	{
		pWeapon->calcLOD( pPos, pCamPos, fFOVRatio);
	}

	// 등에 맨 무기 LOD도 처리
	if( getWeaponContext()->getBackpack() != NULL)
	{
		getWeaponContext()->getBackpack()->calcLOD( pPos, pCamPos, fFOVRatio);
	}
	
	// 사이드 웨폰을 달고 있다면
	if( getWeaponContext()->getSidepack() != NULL )
	{
		getWeaponContext()->getSidepack()->calcLOD( pPos, pCamPos, fFOVRatio);
	}

	//	장비 LOD
	for( INT32 i = 1; i < EQUIP_CLASS_TYPE_COUNT; i++)		
	{
		EQUIP_CLASS_TYPE type = (EQUIP_CLASS_TYPE) i;
		
		CGameCharaEquip * pEquip = m_pEquipContext->getEquip( type);
		if( pEquip != NULL)
		{
			if( m_pEquipContext->isAttachOnChara( type))
			{
				pEquip->calcLOD( pPos, pCamPos, fFOVRatio);
			}
		}
	}	
}

void CGameCharaPlayer::CheckCrossHit( )
{	
	VEC3D vAt;

	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL)	return;	//	무기가 없을 수도 있다.

	CWeaponInfo * pInfo = pWeapon->getWeaponInfo();
	if( pInfo == NULL)		return;	//	무기가 없을 수도 있다.

	MATRIX mtx;
	VEC3D pos;
	m_pCameraContext->getVirtualCamPosition( &pos, &mtx);
	i3Vector::Scale( &vAt, i3Matrix::GetAt( &mtx), - 1000.f);

	m_CrossHitLine.SetStart( i3Matrix::GetPos( g_pCamera->getCamMatrix()));
	i3Vector::Add( &vAt, &vAt, &pos);
	m_CrossHitLine.SetEnd( &vAt );

	//World에 대해서 한번 테스트
	//UINT32 nResultCount = g_pFramework->raycast( &m_CrossHitLine, CGRP_ATTACK);
	//if( nResultCount > 0 )
	
	I3_PHYSIX_HIT_RESULT * pResult = g_pFramework->raycastClosest( &m_CrossHitLine, CGRP_ATTACK);
	if( pResult != NULL)
	{
		//I3_PHYSIX_HIT_RESULT * pResult = g_pFramework->getRaycastResult();

		REAL32	rLength = 10000.f;

		//for( UINT32 i = 0; i < nResultCount; i++ )
		{
			//if( pResult[ i ].m_T < rLength )
			
			{
				i3Physix::HitResultCopyTo( &m_CrossHitResult, pResult );
				//rLength = pResult[ i].m_T;
			}
		}
	}

	// 폭탄을 바라보고 있는지 체크
	if( g_pGameContext->IsBombMissionMode() && // 폭파모드인지
		pMissionMng->getMissionState() == GAME_MISSION_STATE_INSTALLED &&	// 설치 후인지
		pMissionMng->getMyMissionObject() == MY_MISSION_OBJECT_DEFENSIVE )	// 방어팀인지
	{
		WeaponBase * pWeapon = g_pWeaponManager->FindDropWeapon( NULL, WEAPON_CLASS_MISSION, WEAPON_NUM_MISSION_C4 );
		if( pWeapon != NULL )
		{
			VEC3D vDir;

			i3Vector::Sub( &vDir, pWeapon->GetPos(), GetPos() );
			i3Vector::Normalize( &vDir, &vDir );

			if( i3Vector::Dot( &vDir, i3Matrix::GetAt( GetMatrix() ) ) > 0.6f )
			{
				g_pGameContext->getMissionManager()->setEnableCrossHit( TRUE);
			}
			else
			{
				g_pGameContext->getMissionManager()->setEnableCrossHit( FALSE);
			}
		}
		else
		{
			g_pGameContext->getMissionManager()->setEnableCrossHit( FALSE);
		}
	}
}


void CGameCharaPlayer::ReturnInstance( void)
{	
	{
		char szTemp[256] = "";
		sprintf( szTemp, "Remove Chara (%s)", g_pGameContext->getNickForSlot(  getCharaInfo()->_NetIdx));
		__MLOG( szTemp);
	}

	//DetachCollisionNode();

	//Destroy();

	CGameCharaBase::ReturnInstance();
}


// AI State가 변경될 때마다 호출되는 함수.
// 느릴 수 있기 때문에 충분히 Optimize되어야 한다.
void CGameCharaPlayer::OnChangeAIState( i3AIContext * pCtx)
{
	i3AIState * pState = pCtx->getCurrentAIState();
	i3Animation * pAnim;
	i3TransformSequence * pTransSeq = NULL;

	I3ASSERT( pState != NULL);

	if( pState->getAnim() == NULL)
	{
		return;
	}

	pAnim = pState->getAnim();

	// 상체 Animation들은 하체의 것과 방향이 안맞을 수 있다.
	// 이것을 보정하기 위해, Pivot Bone이 삽입되어 있으며,
	// Pivot은 항상 전방(Z축)으로 향해야 한다는 가정을 이용해
	// 상체가 비틀어진 것을 보정한다.

	m_pBoneContext->setTimeTargetYawBlend( pState->getBlendTime());
	m_pBoneContext->setTimeYawBlend( 0.f);

	// Pivot Keyframe이 있는지 확인 
	if( i3String::Compare( pAnim->GetTrackBoneName( 0), "IK") == 0)
	{
		//  상체다 .
		if( i3String::Compare( pAnim->GetTrackBoneName( pAnim->GetTrackCount() - 1), "Pivot") == 0)
		{
			//pTransSeq = pAnim->GetTrack( 0);
			//pTransSeq->GetKeyMatrix( 0, &m_mtxPivot[0]);

			pTransSeq = pAnim->GetTrack( pAnim->GetTrackCount() - 1);
			pTransSeq->GetKeyMatrix( 0, m_pBoneContext->getPivotMatrix());

			m_pBoneContext->setValidPivot( true);
			return;
		}
		
		m_pBoneContext->setValidPivot( false);
	}


	m_pBoneContext->setPivotYawTarget( 0.f);
}


void CGameCharaPlayer::ChangeFPSView(BOOL bPlayAnimSync)
{
	//--> MaterialContext
	//m_pTexFilterAttr->SetMinFilter(I3G_TEXTURE_FILTER_LINEAR);
	
	ChangeAnimSet(CHARA_ANIMSET_1PV, bPlayAnimSync, FALSE);
	getSoundContext()->SetSoundOwner();
}

void CGameCharaPlayer::ChangeObserverView(BOOL bPlayAnimSync)
{
	ChangeAnimSet(CHARA_ANIMSET_3PV, bPlayAnimSync, FALSE);
	getSoundContext()->SetSoundOwner();
}

void CGameCharaPlayer::SetViewerChara( BOOL bEnable)
{	//	뷰어 또는 GUI용 3인칭 캐릭터로 고정	
	if( bEnable)
	{
		m_pBody->removeStyle( I3SG_LOD_AUTO);
		m_pBody->setCurrentLevel( 1);

		//	무기 LOD
		for( INT32 i = 0; i < MAX_ITEMSLOT_COUNT; i++)
		{
			WeaponBase * pWeapon = getWeaponContext()->getWeapon( i);

			if( pWeapon != NULL)
			{
				i3Body * pBody = pWeapon->GetSceneObject()->GetBody();
				I3ASSERT( pBody != NULL);

				pBody->removeStyle( I3SG_LOD_AUTO);
				pBody->setCurrentLevel( 0);	
			}
		}

		//	장비 LOD
		for( INT32 i = 0; i < EQUIP_CLASS_TYPE_COUNT; i++)		
		{
			EQUIP_CLASS_TYPE type = (EQUIP_CLASS_TYPE) i;
			
			CGameCharaEquip * pEquip = m_pEquipContext->getEquip( type);
			if( pEquip != NULL)
			{
				i3Body * pBody = pEquip->GetSceneObject()->GetBody();
				I3ASSERT( pBody != NULL);

				pBody->removeStyle( I3SG_LOD_AUTO);
				pBody->setCurrentLevel( 0);	
			}
		}	

		//	텍스처 깍뚜기 현상을 막아요.
		m_pMaterialContext->getTextureFilterAttr()->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR);
		m_pMaterialContext->getTextureFilterAttr()->SetMinFilter( I3G_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR);
		//m_pTexFilterAttr->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR);
		//m_pTexFilterAttr->SetMinFilter( I3G_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR);
	}
	else
	{
		m_pBody->addStyle( I3SG_LOD_AUTO);

		//	무기 LOD
		for( INT32 i = 0; i < MAX_ITEMSLOT_COUNT; i++)
		{
			WeaponBase * pWeapon = getWeaponContext()->getWeapon( i);

			if( pWeapon != NULL)
			{
				i3Body * pBody = pWeapon->GetSceneObject()->GetBody();
				I3ASSERT( pBody != NULL);

				pBody->addStyle( I3SG_LOD_AUTO);
			}
		}

		//	장비 LOD
		for( INT32 i = 0; i < EQUIP_CLASS_TYPE_COUNT; i++)		
		{
			EQUIP_CLASS_TYPE type = (EQUIP_CLASS_TYPE) i;
			
			CGameCharaEquip * pEquip = m_pEquipContext->getEquip( type);
			if( pEquip != NULL)
			{
				i3Body * pBody = pEquip->GetSceneObject()->GetBody();
				I3ASSERT( pBody != NULL);

				pBody->addStyle( I3SG_LOD_AUTO);
			}
		}

		m_pMaterialContext->getTextureFilterAttr()->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR);
		m_pMaterialContext->getTextureFilterAttr()->SetMinFilter( I3G_TEXTURE_FILTER_POINT_MIPMAP_LINEAR);
	//	m_pTexFilterAttr->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR);
	//	m_pTexFilterAttr->SetMinFilter( I3G_TEXTURE_FILTER_POINT_MIPMAP_LINEAR);
	}
}


void CGameCharaPlayer::DumpChara( void)
{	
	I3ASSERT( m_pCharaInfo != NULL );
	INT32 nNetIdx = getCharaNetIndex();

	const char * pszName = g_pGameContext->getNickForSlot( nNetIdx);
	if( strlen( pszName) == 0)
		pszName = "null";

	I3NOTICE("---------------------------------------------------");
	I3NOTICE("<< Dump Character >>");	
	I3NOTICE( "- Chara ID : %s (%d)", pszName, nNetIdx );
	I3NOTICE( "- this : 0x%p", this );
	I3NOTICE( "- InvincibleElapseTime = %.5f", m_rInvincibleElapseTime );
	I3NOTICE( "- CharaTask = %d", g_pCharaManager->getCharaTask( nNetIdx ) );
	I3NOTICE( "- HP : %d", m_pCharaInfo->_Hp);

	//	캐릭터 상태 덤프
	CGameCharaBase::DumpChara();

	//	현재 무기 덤프
	if( getWeaponContext()->getCurrent() != NULL)
	{
		getWeaponContext()->getCurrent()->DumpState();
	}
	else
	{
		I3NOTICE( "NO Weapon\n");
	}

	//	장비품 덤프
	for( INT32 i = 0; i < EQUIP_CLASS_TYPE_COUNT; i++)		
	{
		CGameCharaEquip * pEquip = m_pEquipContext->getEquip( (EQUIP_CLASS_TYPE) i);

		if( pEquip != NULL)
			pEquip->DumpState();
	}

	I3NOTICE("---------------------------------------------------");
}
/*

REAL32 CGameCharaPlayer::GetSyncTimeLower( void)
{
	I3ASSERT( m_pBody != NULL);

	REAL32 fSyncTime = 0.0f;
	if( m_pAnimContext->isAILower())
	{
		//	현재 AI State가 적용됬다. (바뀌는 AI는 NextAI로 등록되고 다음 프레임에 적용되어 AI가 변경됩니다.)
		if( m_pAnimContext->getAILower()->getNextAIStateIndex() == -1)
		{
			//	하체 애니메이션과 맞는 타임 싱크해야 합니다.			
			fSyncTime = m_pBody->getPlayTime( m_pBoneContext->getBoneIndex( BONE_PELVIS));
		}
	}	

	return fSyncTime;
}

REAL32 CGameCharaPlayer::GetSyncTimeUpper( void)
{
	I3ASSERT( m_pBody != NULL);

	REAL32 fSyncTime = 0.0f;
	if( m_pAnimContext->isAIUpper())
	{	//	현재 AI State가 적용됬다. (바뀌는 AI는 NextAI로 등록되고 다음 프레임에 적용되어 AI가 변경됩니다.)
		if( m_pAnimContext->getAIUpper()->getNextAIStateIndex() == -1)
		{
			//	하체 애니메이션과 맞는 타임 싱크해야 합니다.			
			fSyncTime = m_pBody->getPlayTime( m_pBoneContext->getBoneIndex( BONE_IK));
		}
	}	

	return fSyncTime;
}

void CGameCharaPlayer::ResetDefaultInfo( void)
{	//	캐릭터 Stat 정보 리셋

	I3ASSERT( m_pDBInfo != NULL);
	I3ASSERT( m_pCharaInfo != NULL );

	m_pCharaInfo->_Hp = (INT16) m_pDBInfo->m_nHP;				// HP
}

//#define		SLICE		0.033f
//#define		INVINCIBLE_MIN_ALPHA		0.4f


void CGameCharaPlayer::_DriveLightVolume( REAL32 tm)
{	
	m_timeVolLit += tm;

	if( m_timeVolLit < SLICE)
	{
		REAL32 val = i3Math::vsin_libm( m_timeVolLit * (1.0f / SLICE));
		COLORREAL diff, mtl;

		i3Color::Sub( &diff, &m_TargetDiffuse, &m_OldDiffuse);
		i3Color::Scale( &mtl, &diff, val);
		i3Color::Add( &mtl, &mtl, &m_OldDiffuse);

		SetMaterialColor( &mtl, &mtl );
		//m_pMaterialAttr->SetAmbient( bright * 0.8f, bright * 0.8f, bright * 0.8f, 1.0f);
		//m_pMaterialAttr->SetDiffuse( &mtl);
		//m_pMaterialAttr->SetSpecular( &mtl);

		getWeaponContext()->getControl()->SetMaterialColor( getWeaponContext()->getCurrentSlotNum(), &mtl, &mtl);

		EQUIP_SetMaterialColorAll( &mtl, &mtl, TRUE);
	}
	else
	{
		// 반원으로 Collistion Test를 한다. 매 Frame마다 하는 것이 아니라
		// 간헐적으로 한다.
		i3CollideeSphere sphere;

		VEC3D pos;
		INT32 count, i;
		COLORREAL target;

		i3Vector::Copy( &pos, GetPos());
		i3Vector::AddY( &pos, m_rColRadius * 0.7f );
		sphere.SetCenter( &pos);
		sphere.SetRadius( m_rColRadius * 0.7f);

		//I3TRACE( "%f %f %f %f\n", pos.x, pos.y, pos.z, m_rColRadius);
		
		count = g_pFramework->getCollisionContext()->CollisionTest( &sphere, CGRP_CHARA_LIGHTVOL, 0); //I3_COLLISION_STYLE_NEAREST_ONE);

		I3_COLLISION_RESULT * pResult = g_pFramework->getCollisionContext()->GetResult();

		if( count > 0)
		{
			i3Color::Set( &target, 0.0f, 0.0f, 0.0f, 0.0f);

			for( i = 0; i < count; i++)
			{
				REAL32 r, g, b;

				r = (REAL32)pResult->m_pTri->m_Color[0] * (1.0f / 255.0f) * 2.0f;
				g = (REAL32)pResult->m_pTri->m_Color[1] * (1.0f / 255.0f) * 2.0f;
				b = (REAL32)pResult->m_pTri->m_Color[2] * (1.0f / 255.0f) * 2.0f;

				i3Color::Add( &target, r, g, b, 1.0f);
				pResult ++;
			}

			i3Color::Scale( &target, &target, (1.0f / count));
		}
		else
		{
			CStageBattle * pStage = (CStageBattle *) g_pFramework->GetCurrentStage();

			i3Color::Set( &target, pStage->getDefaultDiffuse());
		}

		{
			m_timeVolLit = 0.0f;
			i3Color::Set( &m_OldDiffuse, &m_TargetDiffuse);
			i3Color::Set( &m_TargetDiffuse, &target);
		}
	}
}
*/

/*
void CGameCharaPlayer::_ProcessMaterial( REAL32 tm)
{	
	REAL32 fAlpha = 1.0f;

	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	if( g_pGameContext->IsBattle() )
	{
		//////////////////////////////////////////////////////////////////////
		//	리스폰 타임
		if( m_rRespawnElapseTime > 0.0f)	
		{
			if( pMissionMng->getStageType() == STAGE_TYPE_DEATHMATCH )
			{
				//	죽은 캐릭터는 서서히 사라지는 효과
				I3ASSERT( g_pCharaInfoDataBase->getGlobal()->m_rRespawnTime > I3_EPS );

				fAlpha = m_rRespawnElapseTime / g_pCharaInfoDataBase->getGlobal()->m_rRespawnTime;

				if( !isNetwork() && g_pCamera->IsExactTypeOf( CGameCamera_Kill::GetClassMeta()))
				{	// 자기 자신
					CGameCamera_Kill * pKillCam = (CGameCamera_Kill*)g_pCamera;
					
					if (pKillCam->isFPS())
					{
						SetZWriteEnable(FALSE);
						SetBlendAlpha(0.0f);
					}
					else
					{
						SetBlendAlpha( fAlpha);
					}
				}
				else
					SetBlendAlpha( fAlpha);

				EQUIP_SetBlendAlphaAll( fAlpha, TRUE);
				getWeaponContext()->getControl()->SetBlendAlpha( getWeaponContext()->getCurrentSlotNum(), fAlpha);

				if( getWeaponContext()->getBackpack() != NULL )
					getWeaponContext()->getBackpack()->SetBlendAlpha( fAlpha);
				if( getWeaponContext()->getSidepack() != NULL )
					getWeaponContext()->getSidepack()->SetBlendAlpha( fAlpha);
			}
		}
		else
		{
			if( isAlive())
			{
				//////////////////////////////////////////////////////////////////////////////
				//	무적 타임
				if( m_rInvincibleElapseTime > 0.0f)
				{
					//	무적시간시에는 깜빡이는 효과		      
					fAlpha = i3Math::abs( i3Math::sin( m_rInvincibleElapseTime * 6.0f));

					//if( fAlpha < INVINCIBLE_MIN_ALPHA)		fAlpha = INVINCIBLE_MIN_ALPHA + (INVINCIBLE_MIN_ALPHA - fAlpha);
					//if( fAlpha < INVINCIBLE_MIN_ALPHA)			fAlpha = INVINCIBLE_MIN_ALPHA; 

					SetBlendAlpha( fAlpha);

					EQUIP_SetBlendAlphaAll( fAlpha, TRUE);
					getWeaponContext()->getControl()->SetBlendAlpha( getWeaponContext()->getCurrentSlotNum(), fAlpha);
					if( getWeaponContext()->getBackpack() != NULL )
						getWeaponContext()->getBackpack()->SetBlendAlpha( fAlpha);
					if( getWeaponContext()->getSidepack() != NULL )
						getWeaponContext()->getSidepack()->SetBlendAlpha( fAlpha);

					// debug 리스폰 후 캐릭터의 HP가 0이면 덤프해봅니다. by dfly
#if defined( I3_DEBUG)
					if( getCurHP() == 0)
					{
						DumpChara();
					}
#endif
				}
				else
				{
					//	무적이 아닐때 원본 상태로 설정		
					SetBlendAlpha( 1.0f);
					EQUIP_SetBlendAlphaAll( 1.0f, TRUE);
					getWeaponContext()->getControl()->SetBlendAlpha( getWeaponContext()->getCurrentSlotNum(), 1.0f);
					if( getWeaponContext()->getBackpack() != NULL )
						getWeaponContext()->getBackpack()->SetBlendAlpha( 1.f );
					if( getWeaponContext()->getSidepack() != NULL )
						getWeaponContext()->getSidepack()->SetBlendAlpha( 1.f );
				}
			}
		}
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////////
		//	 캐릭터 Fade효과
		if( m_rFadeElapseTime != 0.0f)
		{
			REAL32 fAlpha = m_rFadeElapseTime / 1.0f;

			I3ASSERT( m_pAnimContext->getAILower() != NULL);
			I3ASSERT( m_pAnimContext->getAIUpper() != NULL);

			if( m_pAnimContext->getAILower()->getCurrentAIState() == NULL || m_pAnimContext->getAIUpper()->getCurrentAIState() == NULL)
				SetVisible( FALSE);
			else
				SetVisible( TRUE);

			SetBlendEnable( TRUE);

			if( m_rFadeElapseTime < 0.0f)
			{
				m_rFadeElapseTime += tm;

				if( m_rFadeElapseTime > 0.0f)	
				{
					m_rFadeElapseTime = 0.0f;
					SetBlendEnable( FALSE);
				}

				fAlpha = i3Math::abs( 1.0f + fAlpha);
			}
			else
			{
				m_rFadeElapseTime -= tm;

				if( m_rFadeElapseTime < 0.0f)	
				{
					m_rFadeElapseTime = 0.0f;
					SetBlendEnable( FALSE);
				}

				fAlpha = i3Math::abs( fAlpha);
			}

			SetBlendAlpha( fAlpha);

			EQUIP_SetBlendAlphaAll( fAlpha, TRUE);
			getWeaponContext()->getControl()->SetBlendAlpha( getWeaponContext()->getCurrentSlotNum(), fAlpha);
		}
	}
}
*/
/*
void CGameCharaPlayer::_ProcessScreenEffect( REAL32 tm)
{
	i3RasterizeChain_Blur * pRasterizeBlur = g_pFramework->GetRasterizeBlur();
	I3ASSERT( pRasterizeBlur != NULL);

	CStageBattle	* pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
	I3ASSERT( pStage != NULL);
	CHud * pHud = pStage->getHud();
	
	BOOL	bActiveEffect = FALSE;
	INT32	Tex2DAlpha = 0;
	
	switch( m_nScrEffectType)
	{	
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//	데미지 스크린 이펙트
	case SCR_EFFECT_DAMAGE:
		{
			bActiveEffect = TRUE;
			m_rDamageElapseTime -= tm;

			if( m_rDamageElapseTime <= 0.0f)
			{	//	블러 효과(?)가 서서히 사라진다.
				if( m_fBlurStart > m_fBlurEnd)
				{			
					m_fBlurStart -= 0.02f;
		
					//	색상의 알파가 사라진다.
					INT32 a = (INT32) i3Color::GetA( &m_EFFECT_ScreenColor);
					a -= 2;
					if( a > 255)	a = 255; 
					else if( a < 0) a = 0;
					i3Color::SetA( &m_EFFECT_ScreenColor, (UINT8) a);

					//I3TRACE( "데미지 감소중..(blur %.4f / alpha %d)\n", m_fBlurStart, i3Color::GetA( &m_EFFECT_ScreenColor));
				}
				else
				{
					bActiveEffect = FALSE;
				
					m_fBlurStart = m_fBlurEnd = 0.0f;
					m_rDamageElapseTime = 0.0f;

					m_nScrEffectType = SCR_EFFECT_NONE;

					//I3TRACE( "데미지 종료..\n");
				}
			}
			else
			{
				//I3TRACE( "데미지 진행중..(blur %.4f / alpha %d)\n", m_fBlurStart, i3Color::GetA( &m_EFFECT_ScreenColor));
			}
		}
		break;
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//	섬광 이펙트 시간 계산 (섬광탄에서 사용)
	case SCR_EFFECT_FLASHBANG:
		{	
			Tex2DAlpha = 150;
			bActiveEffect = TRUE;
			m_rFlashElapseTime -= tm;

			if( m_rFlashElapseTime <= 0.f)
			{	
				Tex2DAlpha = (INT32) i3Color::GetA( &m_EFFECT_ScreenColor);		
				Tex2DAlpha -= (INT32) (tm * 255.0f);
				if( Tex2DAlpha < 0)	Tex2DAlpha = 0;

				//	이펙트가 끝나고 서서히 없어진다.
				if( Tex2DAlpha <= 0)
				{
					m_rFlashElapseTime = 0.0f;
					m_rFlashEffectiveTime = 0.0f;

					bActiveEffect = FALSE;
					m_nScrEffectType = SCR_EFFECT_NONE;

					//I3TRACE( "플뱅 종료..\n");
				}
				else
				{
					i3Color::Set( &m_EFFECT_ScreenColor, 255, 255, 255, (UINT8) Tex2DAlpha);	

					//I3TRACE( "플뱅 감소중..(alpha %d)\n", Tex2DAlpha);
				}
			}
			else	//	이펙트 중
			{
				REAL32 fMinReduceAlpha = 150.0f;
				REAL32 fMinAlpha = 255.0f - fMinReduceAlpha;
				REAL32 fGapTime = m_rFlashEffectiveTime - m_rFlashElapseTime;
				INT32 alpha = 0;

				if( fGapTime <= 0.5f)		//	갑자기 하얀화면으로 페이드 된다.
				{
					alpha = (INT32) (fGapTime * 2.0f * 255.f);
					if( alpha > 255) alpha = 255;
				}	
				else if( fGapTime <= 1.0f)	//	하얀화면이 지속된다.
				{
					alpha = 255;
				}					 
				else						//	하얀화면이 울렁(?) 거린다.
				{			
					REAL32 fAlpha = i3Math::abs( i3Math::sin( m_rFlashElapseTime)) * fMinReduceAlpha;
					alpha = (INT32) (255.f - fAlpha);
					if( alpha < (INT32) fMinAlpha)		alpha = (INT32) fMinAlpha;
				}

				i3Color::Set( &m_EFFECT_ScreenColor, 255, 255, 255, (UINT8) alpha);	

				//I3TRACE( "플뱅 진행중..(alpha %d)\n", alpha);
			}			
		}
		break;
	default:
		{
			bActiveEffect = FALSE;

			i3Color::Set( &m_EFFECT_ScreenColor, 0, 0, 0, 0);
			m_fBlurStart		= 0.0f;
			m_fBlurEnd			= 0.0f;
		}
		break;
	};

	//	배틀중에서만 체크합니다.
	if (g_pFramework->IsBattleStage())
	{
		if(g_pCamera->getCurTarget() == this)
		{
			//	1인칭 카메라에서만 작동함 (3인칭 카메라는 스크린 이펙트에 영향을 받지 않는다.)
			if( IsCamFPS() || IsCamKill())
			{	
				//	이펙트 활성화
				if( bActiveEffect)
				{
					if( m_nScrEffectType == SCR_EFFECT_DAMAGE)
					{
						//pRasterizeBlur->SetEnable( TRUE);
						//pRasterizeBlur->SetBlurAlpha( m_fBlurStart);
						//pRasterizeBlur->SetBlurAlphaMin( m_fBlurEnd);
						pRasterizeBlur->SetEnable( FALSE);					
					}
					else if( m_nScrEffectType == SCR_EFFECT_FLASHBANG)
					{			
						pHud->SetScreenTex2DEnable( 0, TRUE);
						pHud->SetScreenTex2DColor( 0, 255, 255, 255, (UINT8) Tex2DAlpha);
						
						for( INT32 i = 1; i < SCR_BACKBUFFER_COUNT; i++)
						{	//	첫번째 스크린만 빼고 안나오게
							pHud->SetScreenTex2DEnable( i, FALSE);
						}
					}

					pHud->SetScreenEffectEnable( TRUE);
					pHud->SetScreenEffectColor( &m_EFFECT_ScreenColor);	
				}
				//	이펙트 비활성화
				else
				{		
					//	화면 이펙트 비활성화		
					pHud->SetScreenEffectEnable( FALSE);
					pHud->SetScreenTex2DEnable( FALSE);

					//	블러 비활성화
					pRasterizeBlur->SetEnable( FALSE);
				} 
			}
			//	3인칭 카메라
			else
			{
				//	화면 이펙트 비활성화		
				pHud->SetScreenEffectEnable( FALSE);
				pHud->SetScreenTex2DEnable( FALSE);

				//	블러 비활성화
				pRasterizeBlur->SetEnable( FALSE);
			}
		}
	}
}
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Process
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//void CGameCharaPlayer::ProcessFirst( REAL32 tm)
//{	
//	CGameCharaBase::ProcessFirst( tm);
	//	Add code here
//}

/*
void CGameCharaPlayer::_ProcessRespawn( REAL32 tm)
{
	// Owner 캐릭터는 죽은 후 KillCam 모델 안보이게
	if( is1PV() && g_pCamera->IsExactTypeOf( CGameCamera_Kill::GetClassMeta()))
	{
		CGameCamera_Kill * pKillCam = (CGameCamera_Kill*)g_pCamera;
		
		if (pKillCam->isFPS())
		{	
			m_pMaterialContext->SetBlendAlpha(0.0f);
			m_pMaterialContext->SetBlendEnable( TRUE);
		}
	}

	//	 리스폰 타이머 체크	
	if( m_rRespawnElapseTime > 0.0f)
	{	
		m_pMaterialContext->SetBlendEnable( TRUE);

		m_rRespawnElapseTime -= tm;

		//	리스폰이 된다.
		if( m_rRespawnElapseTime <= 0.0f)
		{
			m_pMaterialContext->SetBlendEnable( FALSE);
			m_rRespawnElapseTime = 0.0f;
			
#if defined( I3_DEBUG )
			if( !g_pConfig->m_bNetworking )
			{	
				g_pCharaManager->RespawnChara( getCharaNetIndex() );
				i3Vector::Set( m_pMoveContext->getMoveDir(), (REAL32)(i3Math::Rand() % 2), 0.f, (REAL32)(i3Math::Rand() % 2) );

				MATRIX mat;
				VEC3D	vDir;
				i3Matrix::Identity( &mat);
				i3Matrix::SetRotateY( &mat, m_pBoneContext->getTheta() );

				i3Vector::Copy( &m_vDebugOriginalPos, GetPos() );
				i3Vector::TransformCoord( &vDir, m_pMoveContext->getMoveDir(), &mat );
				i3Vector::Add( &m_vDebugTargetPos, GetPos(), &vDir );
				m_rDebugElp = 1.f;

				//ACTION_MoveLower();

				if( i3Vector::IsSame( getMoveDir(), &I3_ZEROVECTOR) )
				{
					if(!STATE_IS_LOWER( CHARA_LOWER_IDLE))
						CALL_ACTION_COMMAND(ACTION_CMD_STEADYLOWER, NULL);;
				}
				else
				{
					ActionParameter * param = GET_PREPARE_ACTION_PARAM;
					i3Vector::Copy(&param->_dir, getMoveDir());

					CALL_ACTION_COMMAND(ACTION_CMD_RUNLOWER, param);
				}
			}
			else
#endif
			{	// 리스폰 타임 전에 리스폰 위치로 옮기고 캐릭터를 끕니다
				g_pCharaManager->DeathPosition( getCharaNetIndex() );
			}
		}
	}	
}
*/

/*
void CGameCharaPlayer::EQUIP_CheckVisible( void)
{

	if (g_pFramework->IsBattleStage())
	{
		if( g_pCameraManager != NULL && g_pCamera->getCurTarget() == this)
		{
			//	1인칭 카메라에서만 작동함 (3인칭 카메라는 스크린 이펙트에 영향을 받지 않는다.)
			if( IsCamFPS() || IsCamKill())
			{	//	1인칭 일경우 장비는 보일 필요가 없다.
				EQUIP_SetVisibleAll( FALSE, TRUE);	
				return;
			}
		}
	}

	EQUIP_SetVisibleAll( TRUE, TRUE);
}

<<<<<<< .mine

void CGameCharaPlayer::ChangeAnimSet( CHARA_ANIMSET_TYPE type, BOOL bAnimSync, BOOL isFixed3PV)
{
	CGameCharaBase::ChangeAnimSet( type, bAnimSync, isFixed3PV);
=======
//void CGameCharaPlayer::ChangeAnimSet( CHARA_ANIMSET_TYPE type, BOOL bAnimSync, BOOL isFixed3PV)
//{
//	CGameCharaBase::ChangeAnimSet( type, bAnimSync, isFixed3PV);
>>>>>>> .r10962
	
	//EQUIP_CheckVisible();
//}

//void CGameCharaPlayer::ChangeAnimSetLowerSync( CHARA_ANIMSET_TYPE type, BOOL isFixed3PV)
//{
//	CGameCharaBase::ChangeAnimSetLowerSync( type, isFixed3PV);

	//EQUIP_CheckVisible();
//}


void CGameCharaPlayer::ProcessMovement( REAL32 tm )
{
	VEC3D	vDist;
	VEC3D	vDistAt;
	VEC3D	vDistRight;
	
	REAL32	fMoveVelocity	= 0.f;
	MATRIX * pCharaMat = GetSceneObject()->GetMatrix();

	i3Vector::Zero( &vDist);

	if( (m_pMoveContext->getMoveDir()->x != 0.0f) || (m_pMoveContext->getMoveDir()->y != 0.0f) || (m_pMoveContext->getMoveDir()->z != 0.0f))
	{
		CHARA_ACTION_BODY	stateBody = getActionContext()->getBody();
		CHARA_ACTION_UPPER	stateUpper = getActionContext()->getUpper();

		//	캐릭터 뛰기
		if( getActionContext()->isLower( CHARA_LOWER_MOVE))
		{
			if( m_pMoveContext->getMoveDir()->z > 0.0f)
			{
				if( m_pMoveContext->getMoveDir()->x > 0.0f || m_pMoveContext->getMoveDir()->x < 0.0f)
					fMoveVelocity = getDBInfo()->m_rDiagonalMoveSpeed;	//	대각선으로 이동값
				else
					fMoveVelocity = getDBInfo()->m_rFrontMoveSpeed;		//	앞으로 이동값
			}
			else if( m_pMoveContext->getMoveDir()->z < 0.0f)
			{
				if( m_pMoveContext->getMoveDir()->x > 0.0f || m_pMoveContext->getMoveDir()->x < 0.0f)
					fMoveVelocity = getDBInfo()->m_rDiagonalMoveSpeed;	//	대각선으로 이동값
				else
					fMoveVelocity = getDBInfo()->m_rBackMoveSpeed;		//	뒤로 이동값
			}
			else
				fMoveVelocity = getDBInfo()->m_rSideMoveSpeed;		//	좌우로 이동값
		}
		//	캐릭터 걷기
		else if( getActionContext()->isLower( CHARA_LOWER_WALK))
		{
			if( m_pMoveContext->getMoveDir()->z > 0.0f)
			{
				if( m_pMoveContext->getMoveDir()->x > 0.0f || m_pMoveContext->getMoveDir()->x < 0.0f)
					fMoveVelocity = getDBInfo()->m_rWalkDiagonalMove;	//	대각선으로 이동값
				else
					fMoveVelocity = getDBInfo()->m_rWalkFrontMove;		//	앞으로 이동값
			}
			else if( m_pMoveContext->getMoveDir()->z < 0.0f)
			{
				if( m_pMoveContext->getMoveDir()->x > 0.0f || m_pMoveContext->getMoveDir()->x < 0.0f)
					fMoveVelocity = getDBInfo()->m_rWalkDiagonalMove;	//	대각선으로 이동값
				else
					fMoveVelocity = getDBInfo()->m_rWalkBackMove;		//	뒤로 이동값
			}
			else
				fMoveVelocity = getDBInfo()->m_rWalkSideMove;		//	좌우로 이동값
		}

		//	현재 동작에 의해 이동 수치가 영향을 받습니다.
		{
			WeaponBase * pWeapon = getWeaponContext()->getCurrent();
			I3ASSERT( pWeapon != NULL);
			CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();				
			
			if( stateBody == CHARA_BODY_STAND)			//	기본 서있는 상태에서
				fMoveVelocity *= pWeaponInfo->m_rMoveNormalStand * 0.01f;
			else if( stateBody == CHARA_BODY_CROUCH)		//	기본 앉아있는 상태에서
				fMoveVelocity *= pWeaponInfo->m_rMoveNormalSit  * 0.01f;
			
			if( isStateMask( CHARA_STATE_ZOOM))
			{//	견착 또는 줌 상태일때
				if( stateUpper != CHARA_UPPER_LOADMAGAZINE && stateUpper != CHARA_UPPER_LOADBULLET)	//	줌인 중이지만 장전시에는 적용안되게...
					fMoveVelocity *= pWeaponInfo->m_rMoveNormalZoom * 0.01f;
			}

			if( stateUpper == CHARA_UPPER_FIRE ||
				stateUpper == CHARA_UPPER_SECONDARY_FIRE)			//	발사 상태일때
				fMoveVelocity *= pWeaponInfo->m_rMoveNormalFire * 0.01f;
		}

		//	점프중에는 이동방향을 변경할수 없다.
		if( !getActionContext()->isLower( CHARA_LOWER_JUMP))
		{
			i3Vector::Copy( m_pMoveContext->getMoveAt(), i3Matrix::GetAt( pCharaMat));
			i3Vector::Copy( m_pMoveContext->getMoveRight(), i3Matrix::GetRight( pCharaMat));

			m_pMoveContext->setMaxSpeed( fMoveVelocity);
		}

		i3Vector::Normalize( &vDistAt, m_pMoveContext->getMoveAt());
		i3Vector::Scale( &vDistAt, &vDistAt, m_pMoveContext->getMoveDir()->z);

		i3Vector::Normalize( &vDistRight, m_pMoveContext->getMoveRight());
		i3Vector::Scale( &vDistRight, &vDistRight, m_pMoveContext->getMoveDir()->x);

		i3Vector::Add( &vDist, &vDistAt, &vDistRight);

		i3Vector::Normalize( &vDist, &vDist);
		//i3Vector::Scale( &vDist, &vDist, fMoveVelocity);
	}
	else
	{
		//	이동중이 아니다.
		//m_fRunElapseTime = 0.0f;		
		getAnimContext()->setRunElapseTime( 0.f);
		removeStateMask( CHARA_STATE_FOOTSTEPRIGHT);
	}

	m_pMoveContext->setAccel( &vDist);
}



void CGameCharaPlayer::ProcessTimer( REAL32 tm)
{
	//	리스폰 직후 무적시간을 계산해 준다.
	if( m_rInvincibleElapseTime > 0.0f)
	{
		m_rInvincibleElapseTime -= tm;
		
		//	리스폰이 된후 일정시간 지나면 키입력을 풀어준다. (무기 교체 GUI에서 키입력을 막았으므로)
		if( g_pCharaInfoDataBase->getGlobal()->m_rInvincibleTime - 0.5f > m_rInvincibleElapseTime)	
		{	
			EnableControl( TRUE);
		}

		m_pMaterialContext->SetBlendEnable( TRUE);
	}		
	else if( m_rInvincibleElapseTime < 0.0f)		
	{	
		EnableControl( TRUE);
		m_rInvincibleElapseTime = 0.0f;

		m_pMaterialContext->SetBlendEnable( FALSE);
	}

	
	//	점프 딜레이 체크
	//if( m_fJumpDelayElapsedTime > 0.0f)
	//{
	//	m_fJumpDelayElapsedTime -= tm;
	//}
	//else if( m_fJumpDelayElapsedTime < 0.0f)		m_fJumpDelayElapsedTime = 0.0f;
	
	m_fJumpDelayElapsedTime += tm;
}

void CGameCharaPlayer::_ProcessChangePose( RT_REAL32 rDeltaSeconds)
{
	m_rIKHeight = -0.15f;

	if( isStateMask( CHARA_STATE_CHANGEPOSE) )
	{	// 자세 변경중이다.
		REAL32 rDelta	= m_pDBInfo->m_vCamIKHeight.vec[m_nChangeFromState] - m_pDBInfo->m_vCamIKHeight.vec[m_StateForCam];

		if( m_rChangePoseAccTime >= m_rChangePoseTime)
		{// 자세 변경이 완료되었다.
			removeStateMask( CHARA_STATE_CHANGEPOSE);
			//m_bChangePoseEnable = FALSE;
			m_rChangePoseAccTime= 0.0f;
			m_rIKHeight			+= m_pDBInfo->m_vCamIKHeight.vec[m_StateForCam] - m_pDBInfo->m_vCamIKHeight.vec[0];
			return ;
		}

		REAL32 rDiv				= i3Math::div( m_rChangePoseAccTime, m_rChangePoseTime);
		REAL32 rRatio			= MINMAX( 0.0f, ( rDelta > 0.0f) ? i3Math::vsin_libm( rDiv) : i3Math::vcos_libm( rDiv), 1.0f);
		
		REAL32 rHeight			= (rDelta > 0.0f) ? rDelta * ( 1.0f - rRatio) - m_pDBInfo->m_vCamIKHeight.vec[0] : 
									rDelta * (rRatio) - m_pDBInfo->m_vCamIKHeight.vec[0];

		m_rIKHeight				+= m_pDBInfo->m_vCamIKHeight.vec[m_StateForCam] + rHeight;
		m_rChangePoseAccTime	+= rDeltaSeconds;
	}
	else
	{
		m_rIKHeight				+= m_pDBInfo->m_vCamIKHeight.vec[m_StateForCam] - m_pDBInfo->m_vCamIKHeight.vec[0];
	}
}


INT32 CGameCharaPlayer::CalcFallDamage( VEC3D * pTakeOffPos)
{
	I3ASSERT( pTakeOffPos != NULL);
	I3ASSERT( g_pCharaInfoDataBase->getGlobal() != NULL);

	INT32 nDamage = 0;

	VEC3D vLen, vDiffPos;
	REAL32 fTakeoffHeight = 0.f;

	i3Vector::Copy( &vDiffPos, GetPos());
	i3Vector::SetX( &vDiffPos, i3Vector::GetX( pTakeOffPos));
	i3Vector::SetZ( &vDiffPos, i3Vector::GetZ( pTakeOffPos));

	i3Vector::Sub( &vLen, &vDiffPos, pTakeOffPos);		
	fTakeoffHeight = i3Vector::Length( &vLen);		//	떨어진 높이 계산

	//	낙하 데미지 최소 높이보다 높은 곳에서 떨어지면
	if( fTakeoffHeight > g_pCharaInfoDataBase->getGlobal()->m_rFallDamageMin)
	{	
		INT32 nMinDamage = g_pCharaInfoDataBase->getGlobal()->m_nFallDamage;	//	최소 데미지

		//	현재 낙하 데미량 계산 (낙하 데미지 = 최소 데미지 + (현재 높이 * 낙하 Slope))
		nDamage = nMinDamage + (INT32) ( fTakeoffHeight * g_pCharaInfoDataBase->getGlobal()->m_rFallDamageSlope);

		if( nMinDamage > nDamage)	nDamage = nMinDamage;
		if( nDamage > 100)			nDamage = 100;

		if( nDamage > 0 && g_pCamera != NULL && is1PV())
		{
			REAL32 TrembAmpRate = ((REAL32)nDamage) / 100.0f;
			g_pCamera->SetTremble( 2.5f + TrembAmpRate, 1.0f, 0.2f, 0.5f);
		}
	}

	return nDamage;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	EQUIPMENT
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CGameCharaPlayer::EQUIP_SetVisibleAll( BOOL bEnable, BOOL bAttachedOnChara)
{
	for( INT32 i = 0; i < EQUIP_CLASS_TYPE_COUNT; i++)		
	{
		EQUIP_CLASS_TYPE type = (EQUIP_CLASS_TYPE) i;
		
		CGameCharaEquip * pEquip = m_pEquipContext->getEquip( type);
		if( pEquip != NULL)
		{
			if( bAttachedOnChara)
				if( !m_pEquipContext->isAttachOnChara( type))		continue;

			pEquip->SetVisible( bEnable);
		}
	}	
}

void CGameCharaPlayer::EQUIP_SetBlendAlphaAll( REAL32 fVal, BOOL bAttachedOnChara)
{	
	//	캐릭터 알파값에 장착된 장비도 영향을 받는다.
	for( INT32 i = 0; i < EQUIP_CLASS_TYPE_COUNT; i++)		
	{
		EQUIP_SetBlendAlpha( (EQUIP_CLASS_TYPE) i, fVal, bAttachedOnChara);
	}
}

BOOL CGameCharaPlayer::EQUIP_SetBlendAlpha( EQUIP_CLASS_TYPE type, REAL32 fVal, BOOL bAttachedOnChara)
{
	if( bAttachedOnChara == TRUE)
		if( m_pEquipContext->isAttachOnChara( type) == FALSE)	
			return FALSE;

	CGameCharaEquip * pEquip = m_pEquipContext->getEquip( type);

	if( pEquip != NULL)
	{
		pEquip->SetBlendAlpha( fVal);
		return TRUE;
	}

	return FALSE;
}

void CGameCharaPlayer::EQUIP_SetMaterialColorAll( COLORREAL * pfDiffuse, COLORREAL * pfSpecular, BOOL bAttachedOnChara)
{	
	//	캐릭터 알파값에 장착된 장비도 영향을 받는다.
	for( INT32 i = 0; i < EQUIP_CLASS_TYPE_COUNT; i++)		
	{
		EQUIP_SetMaterialColor( (EQUIP_CLASS_TYPE) i, pfDiffuse, pfSpecular, bAttachedOnChara);
	}
}

BOOL CGameCharaPlayer::EQUIP_SetMaterialColor( EQUIP_CLASS_TYPE type, COLORREAL * pfDiffuse, COLORREAL * pfSpecular, BOOL bAttachedOnChara)
{
	if( bAttachedOnChara == TRUE)
		if( m_pEquipContext->isAttachOnChara( type) == FALSE)	
			return FALSE;

	CGameCharaEquip * pEquip = m_pEquipContext->getEquip( type);

	if( pEquip != NULL)
	{
		pEquip->SetMaterialColor( pfDiffuse, pfSpecular);
		return TRUE;
	}

	return FALSE;
}

BOOL CGameCharaPlayer::m_pEquipContext->DropToWorld( EQUIP_CLASS_TYPE equipType, VEC3D * pHitDir )
{
	I3ASSERT( pHitDir != NULL);

	VEC3D vHitDir;
	
	CGameCharaEquip * pEquip = m_pEquipContext->getEquip( equipType );
	if( pEquip != NULL )
	{		
		i3Vector::Scale( &vHitDir, pHitDir, -3.f );
		i3Vector::Add( &vHitDir, 0.f, 3.5f, 0.f );
		
		i3Scene::SetDynamicVelocity( pEquip->GetSceneObject(), NULL, &vHitDir, &vHitDir );
		
		if( ACTION_DropEquip( EQUIP_CLASS_TYPE_HELMET, GetSceneObject()->GetParent(), getParent(), i3Matrix::GetPos( getHeadMatrix()) ) )
		{
			pEquip->setDynamicActiveState( true, false);
			pEquip->SetDynamicState( I3GAME_DYNAMIC_STATE_DYNAMIC, true);
			pEquip->EnableCollisionGroup( FALSE);
		}

		return TRUE;
	}

	return FALSE;
}

void CGameCharaPlayer::EQUIP_setInfo( EQUIP_CLASS_TYPE equipType, CEquipInfo * pInfo)
{
	m_pEquipInfo[ equipType ] = pInfo;
}

CEquipInfo * CGameCharaPlayer::EQUIP_getInfo( EQUIP_CLASS_TYPE equipType)
{
	return (CEquipInfo*) m_pEquipInfo[ equipType];
}

BOOL CGameCharaPlayer::EQUIP_Add( EQUIP_CLASS_TYPE type, INT32 number)
{	
	BOOL ret = FALSE;

	//	Resource
	i3GameResChara * pRes = (i3GameResChara *) g_pCharaManager->QueryEquipResource( type, number);
	I3ASSERT( pRes != NULL);

	//	Create
	CGameCharaEquip * pEquip = CGameCharaEquip::NewObject();	//	장비는 캐릭터에 종속된다.
	I3ASSERT( pEquip != NULL);
    
	pEquip->Create( pRes, type, number);

	MATRIX mat;
	i3Body * pBody = pEquip->GetSceneObject()->GetBody();
	i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();
	i3Matrix::Identity( &mat );
	i3Matrix::SetPos( &mat, (REAL32)(getCharaNetIndex()) * 10.f, PHYSIX_HEIGHT_EQUIP, (REAL32)(type) * 10.f, 1.f );
	for( INT32 i = 0; i < pSkel->getBoneCount(); i++ )
	{
		i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i );
		if( pShapeSet != NULL )
		{
			pShapeSet->setShapeGlobalPose( &mat );
		}
	}

	pEquip->Reset();

	ret = EQUIP_Set( pEquip);
	if( !ret)
	{
		I3TRACE( "[EQUIP_Add] Warning! Failed to add equipment (type: %d, num: %d)\n", type, number);

		//	메모리 제거
		I3_SAFE_RELEASE( pEquip);
	}

	return ret;
}

BOOL CGameCharaPlayer::EQUIP_Set( CGameCharaEquip * pEquip)
{	//	이전장비는 삭제하고 캐릭터에 장착한다.
	I3ASSERT( pEquip != NULL);

	BOOL ret = FALSE;
	EQUIP_CLASS_TYPE type = pEquip->GetType();

	EQUIP_Delete( type);	//	이전 장비 삭제

	ret = EQUIP_Attach( pEquip);	//	현재 장비 부착
	if( !ret)
	{
		I3TRACE( "[EQUIP_Set] Warning! Failed to set equipment (type: %d, num: %d)\n", pEquip->GetType(), pEquip->GetNumber());
	}

	//	1인칭 일경우 장비는 보일 필요가 없다.
	if( m_AnimSet == CHARA_ANIMSET_1PV)
	{
		pEquip->SetVisible( FALSE);	
	}
	else
	{
		pEquip->SetVisible( TRUE);	
	}

	return ret;
}

BOOL CGameCharaPlayer::EQUIP_AttachChara( EQUIP_CLASS_TYPE type, BOOL bReset)
{
	CGameCharaEquip * pEquip = m_pEquipContext->getEquip( type);

	if( pEquip != NULL)
	{
		if( bReset)	
		{
			pEquip->Reset();
		}

		if( pEquip->GetOwner() != this)
		{
			EQUIP_Detach( type);
			
			//pEquip->SetPos( 0.f, 0.f, 0.f);

			return EQUIP_Attach( pEquip);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CGameCharaPlayer::EQUIP_Attach( CGameCharaEquip * pEquip)
{	//	캐릭터에 Attach만 한다.
	i3Node * pAttachBone = NULL;
	INT32 nIdx = (INT32) pEquip->GetType();
	CHARA_EQUIP_PART nEquipPart = CHARA_EQUIP_PART_UNKNOWN;

	I3ASSERT( nIdx < EQUIP_CLASS_TYPE_COUNT );

	if( m_pCharaEquip[ nIdx] != NULL)
	{
		if( m_pCharaEquip[ nIdx]->IsAttached())
		{
			I3FATAL( "Failed! Please detach or delete previous equipmenet. (type: %d)\n", pEquip->GetType());
			return FALSE;
		}
	}

	switch( pEquip->GetType())
	{
	case EQUIP_CLASS_TYPE_HELMET:		//	캐릭터 머리
		pAttachBone = m_pDummyHelmet;
		nEquipPart = CHARA_EQUIP_PART_HEAD;
		break;
	default:
		I3FATAL( "Failed! Unknown equipmenet type. (type: %d)\n", pEquip->GetType());
		return FALSE;
	}			

	I3ASSERT( pAttachBone != NULL);

	//	캐릭터에 붙인다.
	pEquip->Attach( pAttachBone, this, TRUE);
	pEquip->Reset();
	pEquip->SetEquipPart( nEquipPart);

	m_pCharaEquip[ nIdx] = pEquip;

	return TRUE;
}

CGameCharaEquip * CGameCharaPlayer::EQUIP_Detach( EQUIP_CLASS_TYPE type)
{	//	캐릭터에서 Detach만 한다.
	CGameCharaEquip * pEquip = NULL;
	INT32 nIdx = (INT32) type;

	I3ASSERT( nIdx < EQUIP_CLASS_TYPE_COUNT );

	if( m_pCharaEquip[ nIdx] != NULL)
	{
		if( m_pCharaEquip[ nIdx]->IsAttached() )
		{
			if( m_pCharaEquip[ nIdx ]->GetOwner() == this )
				pEquip = m_pCharaEquip[ nIdx];

			m_pCharaEquip[ nIdx]->Detach();	//	부모에서 분리
		}
	}

	return pEquip;
}

BOOL CGameCharaPlayer::EQUIP_Delete( EQUIP_CLASS_TYPE type)
{	//	캐릭터에서 Detach후 메모리 제거한다.
	INT32 nIdx = (INT32) type;

	I3ASSERT( nIdx < EQUIP_CLASS_TYPE_COUNT );

	if( m_pCharaEquip[ nIdx] != NULL)
	{	
		EQUIP_Detach( type);	//	캐릭터에서 떼어낸다

		//	메모리 제거
		I3_SAFE_RELEASE( m_pCharaEquip[ nIdx]);

		return TRUE;
	}

	return FALSE;
}

BOOL CGameCharaPlayer::m_pEquipContext->isAttachOnChara( CGameCharaEquip * pEquip)
{
	if( pEquip != NULL)
	{
		//	현재 캐릭터에 붙어 있는지 여부
		if( pEquip->IsAttached() && (pEquip->GetOwner() == this))
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CGameCharaPlayer::m_pEquipContext->isAttachOnChara( EQUIP_CLASS_TYPE type)
{
	INT32 nIdx = (INT32) type;

	I3ASSERT( nIdx < EQUIP_CLASS_TYPE_COUNT );

	if( m_pCharaEquip[ nIdx] == NULL)	return FALSE;

	//	현재 캐릭터에 붙어 있는지 여부
	if( m_pCharaEquip[ nIdx]->IsAttached() && (m_pCharaEquip[ nIdx]->GetOwner() == this))
	{
		return TRUE;
	}

	return FALSE;
}
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	WEAPON
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
BOOL CGameCharaPlayer::getWeaponContext()->getControl()->SetBlendAlpha( INT32 nSlot, REAL32 fVal)
{	
	WeaponBase * pWeapon = getWeaponContext()->getWeapon( nSlot);

	if( pWeapon != NULL)
	{
		pWeapon->SetBlendAlpha( fVal);
		return TRUE;
	}

	return FALSE;
}

void CGameCharaPlayer::WEAPON_SetBlendAlphaAll( REAL32 fVal)
{
	for( INT32 i = 0; i < MAX_ITEMSLOT_COUNT; ++i)
	{
		getWeaponContext()->getControl()->SetBlendAlpha( i, fVal);
	}
}

BOOL CGameCharaPlayer::WEAPON_SetMaterialColor( INT32 nSlot, COLORREAL * pfDiffuse, COLORREAL * pfSpecular)
{
	WeaponBase * pWeapon = getWeaponContext()->getWeapon( nSlot);

	if( pWeapon != NULL)
	{
		pWeapon->SetMaterialColor( pfDiffuse, pfSpecular);
		return TRUE;
	}

	return FALSE;
}

void CGameCharaPlayer::WEAPON_SetMaterialColorAll( COLORREAL * pfDiffuse, COLORREAL * pfSpecular)
{
	for( INT32 i = 0; i < MAX_ITEMSLOT_COUNT; ++i)
	{
		WEAPON_SetMaterialColor( i, pfDiffuse, pfSpecular);
	}
}


BOOL CGameCharaPlayer::WEAPON_Select( INT32 nSlot)
{	//	캐릭터가 들고 있는 현재 무기 교체

	//--> WeaponControl		dfly	20080130
	WeaponBase * pSwapWeapon = getWeaponContext()->getWeapon( nSlot );
	if( pSwapWeapon != NULL )
	{
		// 현재 무기는 때어낸다.
		WEAPON_Detach( m_nCurWeaponSlotNum );

		//	바뀐 무기로 붙인다.
		WEAPON_Attach( nSlot, getWeaponAttachRoot(), this );

		m_nCurWeaponSlotNum = m_nSwapWeaponSlotNum = nSlot;

		ApplyCrosshair();

		{
			VEC3D vec;

			pSwapWeapon->SetVisible( TRUE);
			i3Vector::Zero( &vec );
			pSwapWeapon->SetPos( &vec );
		}

		return TRUE;
	}
	return FALSE;
}

void CGameCharaPlayer::WEAPON_Attach( INT32 nSlot, i3Node * pSceneNode, i3GameNode * pGameNode )
{
	WeaponBase * pWeapon = getWeaponContext()->getWeapon( nSlot );

	if( pWeapon != NULL )
	{	
		pWeapon->SetVisible( TRUE);

		pWeapon->ChangeGunDummy( WEAPON_ATTACH_RIGHT_GRIP);	//	기본 Root로 변경
		pWeapon->AttachNode( pSceneNode, pGameNode );

		if( is1PV())
		{
			// 1인칭용 무기는 Shadow가 나오지 않도록 설정되어 있다.
			pWeapon->GetSceneObject()->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);
		}

		ApplyCrosshair();
	}
}

void CGameCharaPlayer::WEAPON_Detach( INT32 nSlot )
{
	WeaponBase * pWeapon = getWeaponContext()->getWeapon( nSlot );

	if( pWeapon != NULL )
	{
		pWeapon->DeatchNode();

		if( is1PV())
		{
			// 1인칭용 무기는 Shadow가 나오지 않도록 설정되어 있다.
			pWeapon->GetSceneObject()->RemoveFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);
		}
	}
}

void CGameCharaPlayer::WEAPON_Delete( INT32 nSlot)
{	
	//--> WeaponControl		dfly	20080130
	
	WeaponBase * pWeapon = getWeaponContext()->getWeapon( nSlot);

	if( pWeapon != NULL )
	{
		if( nSlot == m_nCurWeaponSlotNum)
		{// 현재 들고 있는 무기라면 때어내야합니다.
			WEAPON_Detach( nSlot );
		}

		// 등에 매고 있다면 때어낸다.
		if( pWeapon == m_pPurchasePrimWeapon )
		{
			WEAPON_DetachFromBackpack();
		}
		// 권총집에 있다면 때어낸다
		else if( pWeapon == m_pPurchaseSideWeapon )
		{
			WEAPON_DetachFromSidepack();
		}

		//	무기 매니저에서 무기를 삭제하도록 한다.
		g_pWeaponManager->RemoveCharaWeapon( m_pWeapon[nSlot]);
		m_pWeapon[ nSlot] = NULL;
	}
}

//잠시 생각을 다시 해봅시다. 걍 가지고있는거랑. 보이는거랑..어떻게할지
BOOL CGameCharaPlayer::WEAPON_Add(INT32 nSlot, WEAPON_CLASS_TYPE classType, INT32 number)
{	
	//--> WeaponControl		dfly	20080130
	I3ASSERT( number > 0 && (INT32) classType > 0 && (INT32) classType < WEAPON_CLASS_COUNT);
	I3ASSERT( nSlot < MAX_ITEMSLOT_COUNT );

	//	알수 없는 무기일 경우 리턴
	//if( number <= 0 || (INT32) classType <= 0)	return FALSE;

	//	무기 매니저를 통해 Instance를 받아온다.
	if( m_pWeapon[nSlot] == NULL)
	{
		m_pWeapon[nSlot] = g_pWeaponManager->QueryWeapon( this, classType, number);
	}
	else
	{
		CWeaponInfo * pInfo = m_pWeapon[nSlot]->getWeaponInfo();
			
		//	전혀 다른 무기일 경우 다시 받아온다.
		if( pInfo->m_TypeClass == classType)
		{
			if( pInfo->m_nNumber != number)
			{
				WEAPON_Delete( nSlot);

				m_pWeapon[ nSlot] = g_pWeaponManager->QueryWeapon( this, classType, number);
			}
		}
		else
		{
			WEAPON_Delete( nSlot);

			m_pWeapon[ nSlot] = g_pWeaponManager->QueryWeapon( this, classType, number);
		}
	}

	I3ASSERT( m_pWeapon[nSlot] != NULL);

	// 생성 후 공중에 띄워줍니다.
	VEC3D vec;
	i3Vector::Set( &vec, (REAL32)(getCharaNetIndex()) * 10.f, PHYSIX_HEIGHT_WEAPON, (REAL32)(nSlot) * 10.f );
	g_pWeaponManager->MoveShapePosition( m_pWeapon[nSlot], &vec );

	m_pWeapon[nSlot]->SetOwner( this);
	m_pWeapon[nSlot]->Reset();

	// 로컬 캐릭터는 무기의 바운드박스 제거
	if( is1PV() )
	{
		m_pWeapon[ nSlot ]->ChangeFPSView();
	}

	return TRUE;
}


BOOL CGameCharaPlayer::WEAPON_AttachToBackpack( INT32 nSlot)
{	
	//--> WeaponControl		dfly	20080130
	
	WeaponBase * pAttachWeapon = getWeaponContext()->getWeapon( nSlot);
	if( pAttachWeapon == NULL )
		return FALSE;

	WeaponBase * pCurWeapon = getWeaponContext()->getCurrent();
	if( pCurWeapon == NULL)
		return FALSE;

	//	같은 무기를 들고 있으므로 등에서 떼어낸다.
	if( pCurWeapon == m_pPurchasePrimWeapon)
	{
		WEAPON_DetachFromBackpack();
	}
		
	//	무기가 주무기면 등짝에 붙인다 (등에 맨다)
	if( pAttachWeapon->getWeaponInfo()->m_TypeUsage == WEAPON_USAGE_PRIMARY)
	{
		I3ASSERT( m_pDummyPrimPurchase != NULL);
		pAttachWeapon->SetBlendAlpha( 1.f );
		m_pPurchasePrimWeapon = pAttachWeapon;
		WEAPON_Attach( nSlot, m_pDummyPrimPurchase, this);
		//m_pPurchasePrimWeapon->AttachNode( m_pDummyPrimPurchase, this);

		return TRUE;
	}
	else
	{
		if( m_pPurchasePrimWeapon != NULL)
		{
			if( m_pPurchasePrimWeapon->getWeaponInfo() != NULL )
			{
				//	안붙어 있으면 다시 붙인다.
				if( !m_pPurchasePrimWeapon->IsAttachedScene())
					m_pPurchasePrimWeapon->AttachNode( m_pDummyPrimPurchase, this);
			}
			else
			{// 해당 무기는 없는 경우이다.(월드에 버린 경우)
				m_pPurchasePrimWeapon = NULL;
			}
		}
	}
	

	return FALSE;
}


void CGameCharaPlayer::WEAPON_DetachFromBackpack()
{
	//--> WeaponControl		dfly		20080130
	
	if(  m_pPurchasePrimWeapon != NULL )
	{
		if( m_pPurchasePrimWeapon->getParentScene() == m_pDummyPrimPurchase)
			m_pPurchasePrimWeapon->DeatchNode();

		m_pPurchasePrimWeapon = NULL;
	}
}

BOOL CGameCharaPlayer::WEAPON_AttachToSidepack( INT32 nSlot)
{
	//--> WeaponControl		dfly	20080130
	WeaponBase * pAttachWeapon = getWeaponContext()->getWeapon( nSlot);
	if( pAttachWeapon == NULL)
		return FALSE;

	WeaponBase * pCurWeapon = getWeaponContext()->getCurrent();
	if( pCurWeapon == NULL)
		return FALSE;

	//	같은 무기를 들고 있으므로 떼어낸다.
	if( pCurWeapon == m_pPurchaseSideWeapon)
	{
		WEAPON_DetachFromSidepack();
	}
		
	//	무기가 보조주무기면 붙인다 
	if( pAttachWeapon->getWeaponInfo()->m_TypeUsage == WEAPON_USAGE_SECONDARY )
	{
		I3ASSERT( m_pDummySidePurchase != NULL);
		pAttachWeapon->SetBlendAlpha( 1.f );
		m_pPurchaseSideWeapon = pAttachWeapon;
		WEAPON_Attach( nSlot, m_pDummySidePurchase, this);

		return TRUE;
	}
	else
	{
		if( m_pPurchaseSideWeapon != NULL)
		{
			if( m_pPurchaseSideWeapon->getWeaponInfo() != NULL )
			{
				//	안붙어 있으면 다시 붙인다.
				if( !m_pPurchaseSideWeapon->IsAttachedScene())
					m_pPurchaseSideWeapon->AttachNode( m_pDummySidePurchase, this);
			}
			else
			{// 해당 무기는 없는 경우이다.(월드에 버린 경우)
				m_pPurchaseSideWeapon = NULL;
			}
		}
	}

	return FALSE;
}


void CGameCharaPlayer::WEAPON_DetachFromSidepack( void)
{
	//--> WeaponControl		dfly		20080130
	
	if(  m_pPurchaseSideWeapon != NULL )
	{
		if( m_pPurchaseSideWeapon->getParentScene() == m_pDummySidePurchase)
			m_pPurchaseSideWeapon->DeatchNode();

		m_pPurchaseSideWeapon = NULL;
	}
}



BOOL CGameCharaPlayer::WEAPON_IsSame( WeaponBase * pSrc, WeaponBase * pDst)
{
	if( pSrc == NULL || pDst == NULL) return FALSE;

	CWeaponInfo * pSrcWeaponInfo = pSrc->getWeaponInfo();
	CWeaponInfo * pDstWeaponInfo = pDst->getWeaponInfo();

	//	같은 종류의 무기인지 검사
	return ( pSrcWeaponInfo->m_TypeClass == pDstWeaponInfo->m_TypeClass &&
			 pSrcWeaponInfo->m_nNumber == pDstWeaponInfo->m_nNumber);
}


BOOL CGameCharaPlayer::WEAPON_DropToWorld( INT32 nSlot, VEC3D * pPos)
{
	//--> WeaponControl		dfly	20080130
	//	SceneNode -> World Scene Node
	//	GameNode -> World Game Node
	
	WeaponBase * pWeapon = getWeaponContext()->getWeapon( nSlot);
	if( pWeapon != NULL)
	{
		// 무기의 Extension 상태를 초기화해준다.
		pWeapon->setExtensionActivate( FALSE );
		pWeapon->ResetEmptyCatridgeSound();
		pWeapon->GetSceneObject()->GetBody()->StopAnim();
		pWeapon->ReturnToReady();
		//	월드(캐릭터 부모)에 붙는다.
		pWeapon->Transmission( GetSceneObject()->GetParent(), this->getParent(), pPos, TRUE);

		//	이제부터 무기 매니저의 소유이며 관리된다. 해당 캐릭터는 소유권이 없다.
		g_pWeaponManager->MoveWeaponCharaToDrop( pWeapon);	

		//	하지만, 이전 캐릭터로써 소유자를 참조할 수 있다.
		pWeapon->SetOwner( this);

		m_pWeapon[ nSlot] = NULL;

		return TRUE;
	}

	return FALSE;
}

void CGameCharaPlayer::WEAPON_Take( WeaponBase * pWeapon )
{
	INT32	nWeaponSlot = 0;
	I3ASSERT( pWeapon != NULL );

	switch( pWeapon->getWeaponInfo()->m_TypeUsage )
	{
	case WEAPON_USAGE_UNKNOWN	:	break;
	case WEAPON_USAGE_PRIMARY	:	nWeaponSlot = 0; break;
	case WEAPON_USAGE_SECONDARY	:	nWeaponSlot = 1; break;
	case WEAPON_USAGE_MELEE		:
	case WEAPON_USAGE_THROWING	:
	case WEAPON_USAGE_ITEM		:
	case WEAPON_USAGE_MISSION	:
	default :
		I3ASSERT_0;
		break;
	}

	//	새로운 무기를 습득하기 위해서는 해당 무기 슬롯이 반드시 비어 있어야 합니다. 여기서 걸리면 버그임...komet
	if( getWeaponContext()->getWeapon( nWeaponSlot ) != NULL )
	{
		return;
	}


	{
		I3TRACE( "[TakeWeapon] chara : %d, id : %d, idx : %d\n", getCharaNetIndex(), pWeapon->getNetworkID(), pWeapon->getIdx() );

		pWeapon->EnableCollisionGroup( TRUE );		// Raycast용으로 전환

		// Ageia 무기를 Kinematic으로 설정하기전에 컬리젼이 안되도록 세팅(문제가 있어서입니다.)
		VEC3D vec;

		CWeaponInfo *pWeaponInfo = pWeapon->getWeaponInfo();
		I3ASSERT( pWeaponInfo != NULL);
		i3Vector::Set( &vec, (REAL32)(getCharaNetIndex()) * 10.f, PHYSIX_HEIGHT_WEAPON, (REAL32)(pWeaponInfo->m_TypeClass) * 1.f );
		g_pWeaponManager->MoveShapePosition( pWeapon, &vec );

		pWeapon->setDynamicActiveState( true, false );
		pWeapon->SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC, true );

		if( pWeapon->getContactHitBox() != NULL)
		{
			pWeapon->getContactHitBox()->modifyDynamicState( I3_PHYSIX_STATE_NO_COLLISION, 0);
		}

		pWeapon->DeatchNode();
		pWeapon->SetOwner( this);
		pWeapon->setIdx( -1 );
		WEAPON_set( nWeaponSlot, pWeapon);
		//m_pWeapon[ nWeaponSlot ] = pWeapon;

		// 1인칭 뷰 캐릭터라면
		if( is1PV())
		{	// LOD를 세팅하고 바운딩 박스를 제거한다.
			getWeaponContext()->getWeapon( nWeaponSlot )->ChangeFPSView();
		}

		if( g_pGameContext->IsBattle() )
		{
			MATRIX mat;
			i3Matrix::Identity( &mat );
			pWeapon->GetSceneObject()->SetMatrix( &mat );
			pWeapon->GetSceneObject()->AddFlag( I3_NODEFLAG_MANUAL_UPDATE, TRUE );
			if( pWeapon->isExtensionActivate() )
				pWeapon->SetVisibleExtension( TRUE );
		}

		//	월드에서 캐릭터로 옮긴다.
		pWeapon->Transmission( GetSceneObject(), this, NULL, FALSE);			// SceneGraph 노드 구성, 캐릭터 노드로 붙입니다.
		pWeapon->ReturnToReady();
		pWeapon->ChangeGunDummy( WEAPON_ATTACH_RIGHT_GRIP);						// 기본 오른손 그립
		WEAPON_Attach( m_nCurWeaponSlotNum, getWeaponAttachRoot(), this );		// 캐릭터에 GameNode를 붙입니다.
		g_pWeaponManager->MoveWeaponDropToChara( pWeapon);
		
		// 
		if( (nWeaponSlot == 0) || (m_pWeapon[ 0 ] == NULL) )
		{	// 주무기가 비어 있거나 주무기로 얻은 총은 바쭤줍니다.
			ACTION_SwapWeapon( nWeaponSlot );

			ACTION_apply( !is1PV());
		}
		else if( nWeaponSlot == 1 )
		{	
			// 보조무기를 습득한 경우 3인칭 뷰라면 권총집에 붙여줍니다.
			WEAPON_AttachToSidepack( 1 );

			if( is1PV())
			{	
				pWeapon->GetSceneObject()->GetBody()->StopAnim();
				pWeapon->SetVisible( FALSE);//>GetSceneObject()->AddFlag( I3_NODEFLAG_NO_VISABLE);
				//pWeapon->GetSceneObject()->AddFlag( I3_NODEFLAG_DISABLE );
			}
		}
	}
}

void CGameCharaPlayer::WEAPON_CheckTaking( REAL32 rDeltaTime )
{
	INT32	i;
	VEC3D	vDir;
	REAL32	rLength;
	WeaponBase * pWeapon;

	i3CollideeLine ColLine;

	if( g_pWeaponManager == NULL )	return;

	if( !isAlive() ) return;

	if( g_pWeaponManager->GetDropListCount() == 0 ) return;

	m_rDropedWeaponCheckTime += rDeltaTime;
	if( m_rDropedWeaponCheckTime > WEAPON_CHECKTAKING_TIME )
		m_rDropedWeaponCheckTime = 0.f;
	else
		return;

	ColLine.SetStart( i3Matrix::GetPos( getIKMatrix() ) );

	// 주무기 슬롯이 비어있다. 체크
	if( m_pWeapon[ 0 ] == NULL )
	{	
		for( i = 0; i < g_pWeaponManager->GetDropListCount(); i++ )
		{	
			pWeapon = g_pWeaponManager->GetDropWeapon( i );
			if( pWeapon->IsDroped() && pWeapon->getWeaponInfo()->m_TypeUsage == WEAPON_USAGE_PRIMARY)
			{
				if( pWeapon->getTotalBulletCount() > 0 || pWeapon->getLoadedBulletCount() > 0 || pWeapon->getLoadedBulletDualCount() > 0 )
				{	
					i3Vector::Sub( &vDir, pWeapon->GetPos(), GetPos() );//i3Matrix::GetPos( getIKMatrix() ) );
					rLength = i3Vector::LengthSq( &vDir );
					if( rLength < 1.f )//2.25f )
					{	
						// Pickup weapon
						g_pGameContext->WriteMyTakeWeapon( pWeapon, pWeapon->getIdx(), 0 );
						break;
					}
				}
			}
		}
	}

	// 보조 무기 슬롯이 비어있다. 체크
	if( m_pWeapon[ 1 ] == NULL )
	{
		for( i = 0; i < g_pWeaponManager->GetDropListCount(); i++ )
		{
			pWeapon = g_pWeaponManager->GetDropWeapon( i );
			if( pWeapon->IsDroped() && pWeapon->getWeaponInfo()->m_TypeUsage == WEAPON_USAGE_SECONDARY)
			{
				if( pWeapon->getTotalBulletCount() > 0 || pWeapon->getLoadedBulletCount() > 0 )
				{
					i3Vector::Sub( &vDir, pWeapon->GetPos(), GetPos() );//i3Matrix::GetPos( getIKMatrix() ) );
					rLength = i3Vector::LengthSq( &vDir );
					if( rLength < 1.f ) //2.25f )
					{	
						// Pickup weapon
						g_pGameContext->WriteMyTakeWeapon( pWeapon, pWeapon->getIdx(), 1 );
						break;
					}
				}
			}
		}
	}
}


void CGameCharaPlayer::WEAPON_validCheck( WEAPON_CLASS_TYPE nClassType, INT32 WeaponNum )
{
	BOOL bCheck = FALSE;

	INT32 nSlot = 0;
	switch( nClassType )
	{
	case WEAPON_CLASS_KNIFE : nSlot = 2; break;
	case WEAPON_CLASS_HANDGUN : nSlot = 1; break;
	case WEAPON_CLASS_ASSAULT :
	case WEAPON_CLASS_SMG :
	case WEAPON_CLASS_SNIPER :
	case WEAPON_CLASS_SHOTGUN :
	case WEAPON_CLASS_MG :
		nSlot = 0; break;
	case WEAPON_CLASS_THROWING : nSlot = 3;
		if( WeaponNum > 1 )		// 스모크, 플뱅은 스로잉 타입이지만 슬롯은 아이템으로 취급 4번 슬롯으로 사용
			nSlot = 4;
		break;
	case WEAPON_CLASS_ITEM :	nSlot = 4; break;
	case WEAPON_CLASS_MISSION : nSlot = 5; break;
	default :
		I3TRACE( "invalid weapon.\n" );
		return;
	}

	WeaponBase * pWeapon = getWeaponContext()->getWeapon( nSlot );
	
	if( pWeapon != NULL )
	{	
		if( pWeapon->getWeaponInfo()->m_TypeClass != nClassType || pWeapon->getWeaponInfo()->m_nNumber != WeaponNum )
		{
			bCheck = TRUE;
			//pWeapon->ReturnInstance();
			WEAPON_Delete( nSlot );
			//
			I3TRACE( "wrong weapon. recreate.\n" );
		}
	}
	else
	{
		bCheck = TRUE;
	}

	if( bCheck )
	{	
		WEAPON_Add( nSlot, nClassType, WeaponNum );
		WEAPON_Select( nSlot );
	}
}
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	CAMERA
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
void CGameCharaPlayer::_CAMERA_UpdateState(void)
{
	CHARA_STATE_FOR_CAM nNewTargetState = CSFC_UNKNOWN;

	if( getActionContext()->isLower( CHARA_LOWER_MOVE) || getActionContext()->isLower( CHARA_LOWER_WALK))
	{
		if( getActionContext()->isBody( CHARA_BODY_STAND))	//	서 있으면
		{
			nNewTargetState = CSFC_STAND;
		}
		else if( getActionContext()->isBody( CHARA_BODY_CROUCH))	//	앉아 있으면
		{
			nNewTargetState = CSFC_SITMOVE;
		}
	}
	else if( getActionContext()->isLower( CHARA_LOWER_JUMP))
	{
		nNewTargetState = CSFC_JUMP;
	}
	else
	{
		if( getActionContext()->isBody( CHARA_BODY_STAND))	//	서 있으면
		{
			nNewTargetState = CSFC_STAND;
		}
		else if( getActionContext()->isBody( CHARA_BODY_CROUCH))	//	앉아 있으면
		{
			nNewTargetState = CSFC_SIT;
		}
	}

	if( nNewTargetState != m_StateForCam)
	{
		//자세 변경시의 소리
		if( (nNewTargetState == CSFC_STAND) && (m_StateForCam == CSFC_SIT))
		{
			INT32 nRand = i3Math::Rand()%2;

			g_pSndMng->StartCharacterActionSnd( GTBCA_ON_STANDUP, nRand, GetPos(), is1PV());
		}
		else if( (m_StateForCam == CSFC_STAND) && (nNewTargetState == CSFC_SIT))
		{
			INT32 nRand = i3Math::Rand()%2;

			g_pSndMng->StartCharacterActionSnd( GTBCA_ON_SITDOWN, nRand, GetPos(), is1PV());
		}


		BOOL bTrue1PV = FALSE;
		if( g_pCamera && g_pCamera->IsExactTypeOf(CGameCamera_FPS::GetClassMeta()) )
		{	
			CGameCharaPlayer* pCharacter = g_pCamera->getCurTarget();
			if( pCharacter == this ) bTrue1PV = TRUE;
		}
		
		if( bTrue1PV )
		{
			if((m_StateForCam == CSFC_STAND) && (nNewTargetState != CSFC_JUMP))
			{
 				_setStandOn1PV( nNewTargetState, 0.1f);
			}
			else
			{
				_setStandOn1PV( nNewTargetState, 0.3f);
			}
		}
		else
		{
			if( is1PV() )
			{
				if((g_pCamera != NULL) && (nNewTargetState != CSFC_JUMP) && m_StateForCam != CSFC_JUMP)
				{
					g_pCamera->SetInterPolate( 0.2f);
				}
			}
		}
	}
	else
	{

	}

	m_StateForCam = nNewTargetState;

	I3ASSERT( m_StateForCam != CSFC_UNKNOWN);
}

void CGameCharaPlayer::_CAMERA_SetIdle( void)
{
	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	I3ASSERT( pWeapon != NULL);

	BOOL bViewer = FALSE;
	if( g_pCamera && g_pCamera->IsExactTypeOf(CGameCamera_FPS::GetClassMeta()) )
	{	
		CGameCharaPlayer* pCharacter = g_pCamera->getCurTarget();
		if( pCharacter == this ) bViewer = TRUE;
	}	
	else if( g_pCamera->IsExactTypeOf(CGameCamera_Kill::GetClassMeta()))
	{
		CGameCharaPlayer* pCharacter = g_pCamera->getCurTarget();
		if( pCharacter == this ) bViewer = TRUE;
	}
	
	SetVisible( TRUE);
	pWeapon->SetVisible( TRUE);
	pWeapon->setZoom( false);

	if( bViewer )
	{
		//	일반 카메라 상태
		g_pCamera->SetDefaultFOV( 0.15f);			

		CStageBattle	* pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
		I3ASSERT( pStage != NULL);

		CHud * pHud = pStage->getHud();

		if( m_pBoneContext->getScopeWaveData()->m_nCurScopeLevel > 0)
		{
			if( pHud->IsEnabledSniperScope())
			{
				g_pSndMng->StartWeaponStateSnd( GTBWS_ON_SCOPE, 2, GetPos(), is1PV());
			}
			else
			{
				g_pSndMng->StartWeaponStateSnd( GTBWS_ON_ZOOM, 2, GetPos(), is1PV());
			}
		}

		pHud->SetSniperScope( FALSE);

		m_pBoneContext->getScopeWaveData()->m_nCurScopeLevel = 0;
	}

	removeStateMask( CHARA_STATE_ZOOM );

	ApplyCrosshair();
}

void CGameCharaPlayer::_CAMERA_SetZoom( void)
{
	BOOL bViewer = FALSE;
	if( g_pCamera && g_pCamera->IsExactTypeOf(CGameCamera_FPS::GetClassMeta()) )
	{	
		CGameCharaPlayer* pCharacter = g_pCamera->getCurTarget();
		if( pCharacter == this ) bViewer = TRUE;
	}	

	if( bViewer )
	{
		//	견착 카메라 상태
		WeaponBase * pWeapon = getWeaponContext()->getCurrent();
		I3ASSERT( pWeapon != NULL);

		if( g_pCamera->IsExactTypeOf(CGameCamera_FPS::GetClassMeta()) )
		{
			g_pCamera->ZoomIn( pWeapon->getWeaponInfo()->m_rZoomFOV, 0.15f);		
		}
		else
		{
			g_pCamera->ZoomIn( pWeapon->getWeaponInfo()->m_rZoomFOV3PV, 0.15f);		
		}

		m_pBoneContext->getScopeWaveData()->m_nCurScopeLevel = 1;

		g_pSndMng->StartWeaponStateSnd( GTBWS_ON_ZOOM, 0, GetPos(), is1PV());
	}
}

BOOL CGameCharaPlayer::_CAMERA_SetScope( INT32 nScopeLevel)
{	
	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return FALSE;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != NULL );

	BOOL bViewer = FALSE;
	if( g_pCamera != NULL && g_pCamera->IsExactTypeOf( CGameCamera_FPS::GetClassMeta()) )
	{	
		//	Zoom시 캐릭터 팔과 총구가 화면에 삐져나오는 것을 안보이게 하기 위해 Visible Off 시킨다.
		//	Scope 상태이면 화면 HUD만 보이도록 캐릭터와 무기 모델링은 Scene만 off, DynamicState는 On 되어야 한다.
		CGameCharaPlayer* pCharacter = g_pCamera->getCurTarget();
		if( pCharacter == this )
		{
			bViewer = TRUE;
			SetVisible( FALSE);
		}
		else
		{
			SetVisible(TRUE);
		}
	}
	else
	{
		SetVisible(TRUE);
	}

	pWeapon->setZoom( true);
	
	//	스나이퍼 카메라 상태 (스코프 단계가 가능한 상태이면)
	if( nScopeLevel <= pWeaponInfo->m_nMaxScopeLevel)
	{
		if( bViewer )
		{
			REAL32 fScopeFOV = 0.0f;
			switch( nScopeLevel)
			{
			case 1:		fScopeFOV = pWeaponInfo->m_rScopeFOV;		break;		//	1단 Scope
			case 2:		fScopeFOV = pWeaponInfo->m_rScopeFOV2;		break;		//	2단 Scope
			default:	I3ASSERT_0;	//	설정된 이상의 Scope를 할수 없다.
			}	

			g_pCamera->ZoomIn( fScopeFOV, 0.15f);

			CStageBattle	* pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
			I3ASSERT( pStage != NULL);

			CHud * pHud = pStage->getHud();
			pHud->SetSniperScope( TRUE, pWeaponInfo);

			g_pSndMng->StartWeaponStateSnd( GTBWS_ON_SCOPE, 0, GetPos(), is1PV());
		}

		m_pBoneContext->getScopeWaveData()->m_nCurScopeLevel = nScopeLevel;
		//I3TRACE("ScopeLevel : %d\n", m_nCurScopeLevel);

		return TRUE;
	}

	return FALSE;
}
*/

/*
BOOL CGameCharaPlayer::EFFECT_IsAvailable( void)
{	//	현재 스크린 이펙트가 가능한 상태인지 여부
	if (g_pFramework->IsBattleStage())
	{				
		if( g_pCameraManager != NULL && g_pCamera->getCurTarget() == this)
		{
			if( IsCamFPS() || IsCamKill())
				return TRUE;
		}
	}

	return FALSE;
}

void CGameCharaPlayer::EFFECT_Clear( void)
{
	i3Color::Set( &m_EFFECT_ScreenColor, 0, 0, 0, 0);
	m_fBlurStart		= 0.0f;
	m_fBlurEnd			= 0.0f;

	m_rFlashElapseTime = m_rFlashEffectiveTime = 0.0f;
	m_rDamageElapseTime = 0.0f;

	m_nScrEffectType	= SCR_EFFECT_NONE;

	if (FALSE == g_pFramework->IsBattleStage()) return;
	
	//	화면 이펙트 비활성화	
	CStageBattle	* pStage = (CStageBattle*) g_pFramework->GetCurrentStage();
	I3ASSERT( pStage != NULL);

	CHud * pHud = pStage->getHud();
	if( pHud == NULL)
		return;

	i3RasterizeChain_Blur * pRasterizeBlur = g_pFramework->GetRasterizeBlur();
	I3ASSERT( pRasterizeBlur != NULL);

	pHud->SetScreenEffectEnable( FALSE);
	pHud->SetScreenTex2DEnable( FALSE);

	pRasterizeBlur->SetEnable( FALSE);
}

BOOL CGameCharaPlayer::EFFECT_FlashBang( WeaponBase * pWeapon, REAL32 rDamagedLength)
{
	I3ASSERT( pWeapon != NULL);

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != NULL);

	return EFFECT_FlashBang( pWeapon->GetPos(),
							pWeaponInfo->m_rEffectiveTime,
							pWeaponInfo->m_rExplosion_Range,
							pWeaponInfo->m_rExplosion_EffectiveSlope,
							rDamagedLength);
}

BOOL CGameCharaPlayer::EFFECT_FlashBang( CHARA_DAMAGEINFO * pDamageInfo, REAL32 rDamagedLength)
{
	I3ASSERT( pDamageInfo != NULL);
	
	return EFFECT_FlashBang( pDamageInfo->_pVecPos,
							pDamageInfo->_rEffectiveTime,
							pDamageInfo->_rExplosionRange,
							pDamageInfo->_rExplosionSlope,
							rDamagedLength);
}

BOOL CGameCharaPlayer::EFFECT_FlashBang( VEC3D * pVecWeaponPos, REAL32 rEffectiveTime, REAL32 rExplosionRange, REAL32 rExplosionSlope, REAL32 rDamagedLength)
{
	I3ASSERT( pVecWeaponPos != NULL);

	CStageBattle	* pStage = (CStageBattle*) g_pFramework->GetCurrentStage();
	I3ASSERT( pStage != NULL);

	CHud * pHud = pStage->getHud();

	if( rEffectiveTime == 0.0f) return FALSE;

	i3Color::Set( &m_EFFECT_ScreenColor, 255, 255, 255, 255);

	if (g_pFramework->IsBattleStage())
	{
		if( IsCamFPS() && g_pCamera->getCurTarget() == this)
		{
			for( INT32 i = 0; i < SCR_BACKBUFFER_COUNT; i++)
				pHud->SetCurrentScreenImage( i, 255, 255, 255, 255);
		}
	}
	
	// 거리에 따른 효과 증감율( 1.0f가 Default입니다.)
	REAL32  rValidRate	= 1.0f - ((1.0f / rExplosionRange) * rDamagedLength) * rExplosionSlope;
	if( rValidRate < I3_EPS)	rValidRate = I3_EPS;

	//	바라보는 각도에 따라 영향을 받는다.
	{					
		VEC3D vCharaDir, vFlashDir;					
		i3Vector::Copy( &vCharaDir, i3Matrix::GetAt( GetMatrix()));
		i3Vector::Sub( &vFlashDir, pVecWeaponPos, GetPos());
		i3Vector::Normalize( &vFlashDir, &vFlashDir);

		REAL32 fCharaFlashAngle = i3Vector::Dot( &vCharaDir, &vFlashDir);
		fCharaFlashAngle = I3_RAD2DEG( fCharaFlashAngle);
		if( fCharaFlashAngle <= 0.0f)
			fCharaFlashAngle = i3Math::abs( fCharaFlashAngle) + 90.f; 					
							
		//	완전히 영향 받는 각도 0 ~ 70도 
		if( fCharaFlashAngle > 110.0f)		// 1/4 영향 받는 각도 110 ~ 180
		{
			rValidRate *= 0.25f;
		}
		else if( fCharaFlashAngle > 70.f)	// 1/2 영향 받는 각도 70 ~ 110
		{
			rValidRate *= 0.5f;
		}
	}	

	//	지속 시간 계산
	m_rFlashElapseTime = m_rFlashEffectiveTime = rEffectiveTime * rValidRate;
	
	//I3TRACE("EFFECT_FlashBang : cur %.4f / src %.4f (Rate: %.2f%)\n", m_rFlashEffectiveTime, pWeaponInfo->m_rEffectiveTime, rValidRate);

	m_nScrEffectType = SCR_EFFECT_FLASHBANG;

	return TRUE;	
}

BOOL CGameCharaPlayer::EFFECT_Damage( WeaponBase * pWeapon)
{
	I3ASSERT( pWeapon != NULL);

	//	섬광탄이 우선 (섬광 이펙트 진행중이면 스킵)
	if( m_nScrEffectType == SCR_EFFECT_FLASHBANG)	return FALSE;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();

	if( pWeaponInfo->m_rEffectiveTime == 0.0f) return FALSE;

	CWeaponGlobal * pWeaponGlobal = g_pWeaponInfoDataBase->getGlobal();
	
	return EFFECT_Damage( pWeaponInfo->m_rEffectiveTime, 
							&pWeaponGlobal->m_DamageEffectColor, 
							pWeaponGlobal->m_DamageEffectBlurStart,
							pWeaponGlobal->m_DamageEffectBlurEnd);
}

BOOL CGameCharaPlayer::EFFECT_Damage( REAL32 fDamageTime, I3COLOR * pColor, REAL32 fBlurStart, REAL32 fBlurEnd)
{
	// 피격 필터 제거합니다 - praptor
	// 2007.05.17praptor

	////	Local이든 Network 캐릭이든 처리되야 한다.	

	////	섬광탄이 우선 (섬광 이펙트 진행중이면 스킵)
	//if( m_nScrEffectType == SCR_EFFECT_FLASHBANG)	return FALSE;

	//if( fDamageTime > 0.0f)
	//{
	//	m_nScrEffectType = SCR_EFFECT_DAMAGE;
	//	i3Color::Copy( &m_EFFECT_ScreenColor, pColor);
	//	m_fBlurStart = i3Math::Clamp( fBlurStart, 0.f, 1.f);
	//	m_fBlurEnd = i3Math::Clamp( fBlurEnd, 0.f, 1.f);
	//	
	//	m_rDamageElapseTime = fDamageTime;	

	//	return TRUE;
	//}

	return FALSE;
}

void CGameCharaPlayer::EFFECT_FadeChara( REAL32 fFadeTime)
{	//	fFadeTIme이 음수면 FadeIn, 양수면 FadeOut 효과가 나온다.

	m_rFadeElapseTime = fFadeTime;

	if( m_rFadeElapseTime > 0.5f)			m_rFadeElapseTime = 0.5f;
	else if( m_rFadeElapseTime < -0.5f)		m_rFadeElapseTime = -0.5f;

	I3ASSERT( m_pAnimContext->getAILower() != NULL);
	I3ASSERT( m_pAnimContext->getAIUpper() != NULL);

	if( m_pAnimContext->getAILower()->getCurrentAIState() == NULL || m_pAnimContext->getAIUpper()->getCurrentAIState() == NULL)
		SetVisible( FALSE);

	SetBlendEnable( TRUE);

	if( m_rFadeElapseTime < 0.0f)
		SetBlendAlpha( 0.0f);
	else
		SetBlendAlpha( 1.0f);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Hit 처리
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

INT32 CGameCharaPlayer::OnHit( CHARA_DAMAGEINFO * pDamageInfo)
{
	I3ASSERT( pDamageInfo != NULL);
	
	if( isInvincible())		return 0;		// 무적 상태이다.

	CStageBattle * pStage = (CStageBattle*) g_pFramework->GetCurrentStage();
	if( pStage->getStageState() != GAMESTAGE_STATE_BATTLE &&
		pStage->getStageState() != GAMESTAGE_STATE_INTRUDE )
		return 0;

	I3_PHYSIX_HIT_RESULT *	pHitResult = pDamageInfo->_pHitResult;
	i3PhysixShapeSet*		pDestShapeSet = pHitResult->m_pDestShapeSet;
	
	CHARA_HIT_PART	part		= CHARA_HIT_UNKNOWN;	

	REAL32	rDamagedLength		= _CalcDamageLength( pDamageInfo);

	switch( pDamageInfo->_DamageType)
	{
	////////////////////////////////////////////////////////	섬광
	case WEAPON_DAMAGE_FLASH:
		{
			EFFECT_FlashBang( pDamageInfo, rDamagedLength);
		}
		break;	
	////////////////////////////////////////////////////////	HP	감소
	case WEAPON_DAMAGE_HP_LOSS:
		{	
			// Character에 맞았다.
			part = GetHitPart( pDestShapeSet);
						
			if( pDamageInfo->_WeaponType == WEAPON_CLASS_THROWING || pDamageInfo->_DeathType == CHARA_DEATH_O )
			{
				if( isAlive() )
				{
					REAL32 rVaildDamageRate = _CalcDamageRate( pDamageInfo, rDamagedLength);

					if( g_pGameContext->IsP2PHost() )
					{	// 수류탄은 방장이 계산

						NET_Hit( part, pDamageInfo, rVaildDamageRate);
					}
					else
					{
						if( rVaildDamageRate > 0.f)
						{
							//	장비 체크
							if( part == CHARA_HIT_HEAD && m_pEquipContext->isAttachOnChara( EQUIP_CLASS_TYPE_HELMET ) )
							{
								// 헬멧부분에 불꽃이 튀는 효과를 낸다
								g_pEffectManager->AddEquipHitEffect( EQUIP_CLASS_TYPE_HELMET, pDamageInfo->_WeaponType, &pDamageInfo->_pHitResult->m_Intersect, 
									&pDamageInfo->_pHitResult->m_Normal, is1PV());
							}
							else
							{
								ACTION_Hit( pDamageInfo);
							}
						}
					}
				}
				else
				{
					ACTION_Hit( pDamageInfo);
	
					// 죽은 캐릭터면 물리 영향을 준다.
					//_PHYSIX_SetLinearVelocity( pDamageInfo );
				}

				return TRUE;
			}
			else if( pDamageInfo->_WeaponType == WEAPON_CLASS_MISSION )
			{// C4는 모든 클라이언트도 계산
				// Death chara & Physix set
				g_pCharaManager->DeathChara( getCharaNetIndex(), pDamageInfo->_nKillerIdx, pDamageInfo->_pVecDir, pDamageInfo->_DeathType, part );
				return TRUE;
			}

			////////////////////////////////////////////////////////////////////////
			// for Network  --- GameContext에도 코드가 있습니다.
			// desc : 자신이 발사한 것은 네트워크로 CS에 보냅니다.
			
			if( pDamageInfo->_IsOwner )
			{
				if( isAlive() )
				{
					g_pGameContext->WriteMyFireHit( getCharaNetIndex(), (UINT32) part, pDamageInfo->_WallShotReduceRate );
				}
				
				if( g_pGameContext->IsP2PHost() )
				{
					REAL32 rVaildDamageRate = 1.f;
					// 월샷에 대한 두께 기능 추가 by dfly79 20071227
					if( pDamageInfo->_WallShotReduceRate < 100 && pDamageInfo->_WallShotReduceRate > 0)
					{	
						rVaildDamageRate = 1.f - ((REAL32)pDamageInfo->_WallShotReduceRate * 0.01f);
						if( rVaildDamageRate < 0.f)
							rVaildDamageRate = 0.f;
					}

					NET_Hit( part, pDamageInfo, rVaildDamageRate);
				}
				else
				{	//	장비 체크
					if( part == CHARA_HIT_HEAD && m_pEquipContext->isAttachOnChara( EQUIP_CLASS_TYPE_HELMET ) )
					{
						//	불꽃이 튄다.
						g_pEffectManager->AddEquipHitEffect( EQUIP_CLASS_TYPE_HELMET, pDamageInfo->_WeaponType, &pDamageInfo->_pHitResult->m_Intersect, 
							&pDamageInfo->_pHitResult->m_Normal, is1PV());
					}
					else
					{
						ACTION_Hit( pDamageInfo);
					}
				}

				// 장비가 떨어진 경우
				if( pDamageInfo->_pHitResult != NULL )
				{
					i3PhysixShapeSet * pShapeSet = pDamageInfo->_pHitResult->m_pDestShapeSet;
					i3EventReceiver * pReceiver = pShapeSet->getEventReceiver();
					if( pReceiver != NULL && pReceiver->IsExactTypeOf( CGameCharaEquip::GetClassMeta() ) && m_pEquipContext->isAttachOnChara( EQUIP_CLASS_TYPE_HELMET ))
					{
						VEC3D vLin, vTorque, vRoll;
						MATRIX mat;

						i3Vector::Scale( &vLin, pDamageInfo->_pVecDir, pDamageInfo->_rDamage * 0.3f );
						
						pShapeSet->getShapeGlobalPose( &mat );
						i3Vector::Sub( &vRoll, &pDamageInfo->_pHitResult->m_Intersect, i3Matrix::GetPos( &mat ) );
						i3Vector::Cross( &vTorque, &vRoll, &vLin );
						pShapeSet->setLinearVelocity( &vLin );
						pShapeSet->setAngularVelocity( &vTorque );
					}
				}
			}
		}	//	End of case (WEAPON_DAMAGE_HP_LOSS)
		break;
	default:
		{
			I3TRACE("[%s] WARNING! Unknown damage type! (%d)\n", __FUNCTION__, (INT32) pDamageInfo->_DamageType);
		}
	}	//	End of switch

	return 1;
}

BOOL CGameCharaPlayer::CalcHelmetPenetrate( CHARA_DAMAGEINFO * pDamageInfo )
{
	BOOL	bPenetrate = TRUE;
	REAL32 rPenetrateHead = 0.f;

	CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( pDamageInfo->_WeaponType, pDamageInfo->_nWeaponNumber);
	if( pWeaponInfo != NULL )
		rPenetrateHead = pWeaponInfo->m_rHelmetPenetrate;

	if( pDamageInfo->_DeathType == CHARA_DEATH_O )
		rPenetrateHead = 0.5f;

	// 헬멧 방어율 계산
	CEquipInfo * pEquipInfo = (CEquipInfo*) m_pEquipContext->getEquipInfo( EQUIP_CLASS_TYPE_HELMET);
	if( pEquipInfo != NULL )
	{
		if( pEquipInfo->getDefenseRate() > 0.f)
		{
			rPenetrateHead /= pEquipInfo->getDefenseRate();
		}
	}
	
	if( rPenetrateHead < i3Math::Randf() )
	{
		bPenetrate = FALSE;
	}

	return bPenetrate;
}

BOOL CGameCharaPlayer::NET_Hit( CHARA_HIT_PART part, CHARA_DAMAGEINFO * pDamageInfo, REAL32 rValidDamageRate )
{	// Only Host
	BOOL	bDamaged = TRUE;
	BOOL	bDeath = FALSE;
	BOOL	bHelmetDrop = FALSE;
	VEC3D	vHeadDir;

		
	if (g_pFramework->IsBattleStage())
	{	
		i3Stage* pStage = g_pFramework->GetCurrentStage();

		GAMESTAGE_STATE stageState = ((CStageBattle*)pStage)->getStageState();
		if( (stageState != GAMESTAGE_STATE_BATTLE) && (stageState != GAMESTAGE_STATE_INTRUDE))
		{
			return FALSE;
		}

		// 전투 상태이면 처리합니다.
	}

	GAMEINFO_RECV * pGameInfo = g_pGameContext->getRecvGameInfo( getCharaNetIndex() );

	I3ASSERT( pDamageInfo->_nHarmerIdx != WEAPON_USAGE_UNKNOWN );
	I3ASSERT( pDamageInfo->_nWeaponNumber != -1 );

	i3Vector::Scale( &vHeadDir, pDamageInfo->_pVecDir, -1.f );

	if( isAlive())
	{
		///
		if( part == CHARA_HIT_HEAD && m_pEquipContext->isAttachOnChara( EQUIP_CLASS_TYPE_HELMET ) )
		{	
			if( pDamageInfo->_WeaponType != WEAPON_CLASS_THROWING)
			{
				//	헬멧을 쓰고 있다면
				bHelmetDrop = TRUE;
				// 헬멧의 관통 여부 계산
				if( !CalcHelmetPenetrate( pDamageInfo ) )
				{	// 관통이 안 되었습니다. 데미지는 계산하지 않습니다.
					rValidDamageRate = 0.f;
					bDamaged = FALSE;
				}
			}
		}

		if( bDamaged )
		{
			//	먼저 데미지에 따른 HP를 계산(CS)
			INT32 nHP	 = getCurHP();
			INT32 nCurHP = nHP;
			
			// P2P Server 만 계산합니다. C4는 모든 클라이언트도 계산합니다.
			if( g_pGameContext->IsP2PHost() || pDamageInfo->_WeaponType == WEAPON_CLASS_MISSION)
			{
				// DPD_Rate의 (Damage per Distance) 적용
				{
					VEC3D vLen;
					REAL32 rate = 1.0f;
					CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( pDamageInfo->_WeaponType, pDamageInfo->_nWeaponNumber );
					CGameCharaPlayer * pHitUser = g_pCharaManager->getCharaByNetIdx( pDamageInfo->_nKillerIdx );

					if( pHitUser != NULL )
					{
						i3Vector::Sub( &vLen, GetPos(), pHitUser->GetPos() );

						REAL32 rDistance = i3Vector::Length( &vLen);

						rate = rDistance / pWeaponInfo->m_rRange;
					}

					if( rate > 1.0f)	rate = 1.0f;

					rValidDamageRate *= (1.0f - (rate * pWeaponInfo->m_DPD_Rate));
				}

				//수류탄이 머리에 맞은경우 데미지계산만 ROOT에 맞은 데미지로 계산을 합니다. --1214기획팀요청, RAJA수정
				if( pDamageInfo->_WeaponType == WEAPON_CLASS_THROWING && part == CHARA_HIT_HEAD)
				{		
					nCurHP = _CalcHit( CHARA_HIT_ROOT, pDamageInfo->_rDamage, rValidDamageRate);
				}
				else
				{
					nCurHP = _CalcHit( part, pDamageInfo->_rDamage, rValidDamageRate);
				}
			}
			
			if( nHP != nCurHP)
			{
				if( nCurHP == 0)	//	죽으면
				{	
					//	Hit Effect
					ACTION_Hit( pDamageInfo);

					// Network Sync
					if( pDamageInfo->_WeaponType != WEAPON_CLASS_MISSION )
					{	// C4는 네트워크 싱크를 맞추지 않습니다.
						g_pGameContext->WriteCharaDeath( pDamageInfo->_nKillerIdx, getCharaNetIndex(), pDamageInfo->_nHarmerIdx,
							pDamageInfo->_WeaponType, pDamageInfo->_nWeaponNumber, pDamageInfo->_DeathType, part, pDamageInfo->_pVecDir );
					}

					bDeath = TRUE;
				}
				else if( nCurHP > 0 && !isInvincible())		//	데미지
				{
					//	데미지 이펙트
					ACTION_Hit( pDamageInfo);

					//	상체가 데미지 동작이 가능한 상태이면
					//ACTION_Damage( part, pDamageInfo->_pVecDir);

					ActionParameter * param = GET_PREPARE_ACTION_PARAM;
					param->_part = part;
					i3Vector::Copy( &param->_dir, pDamageInfo->_pVecDir);

					CALL_ACTION_COMMAND(ACTION_CMD_DAMAGE, param);

					//	데미지 동작을 안할때도 비명 소리는 나야 한다.
					m_pSoundContext->PlayCharaDamageSound( part, TRUE);
				}
				else
					bHelmetDrop = FALSE;
			}
			else
				bHelmetDrop = FALSE;
		}
		else
		{	// 데미지는 계산하지는 않지만 애니메이션은 취합니다.
			//	상체가 데미지 동작이 가능한 상태이면
			//ACTION_Damage( part, pDamageInfo->_pVecDir );

			ActionParameter * param = GET_PREPARE_ACTION_PARAM;
			param->_part = part;
			i3Vector::Copy( &param->_dir, pDamageInfo->_pVecDir);

			CALL_ACTION_COMMAND(ACTION_CMD_DAMAGE, param);

			m_pSoundContext->PlayCharaDamageSound( part, TRUE);
		}
	}
	else
	{	
		ACTION_Hit( pDamageInfo);

		// 죽은 캐릭터면 물리 영향을 준다.
		//_PHYSIX_SetLinearVelocity( pDamageInfo );
	}

	if( bDamaged )
	{
		// network sync
		if( pDamageInfo->_WeaponType != WEAPON_CLASS_MISSION &&
			(pDamageInfo->_DeathType == CHARA_DEATH_B || pDamageInfo->_DeathType == CHARA_DEATH_O) )
		{
			CMulticastSystem * pMulticast = g_pGameContext->getMulticast();
			
			{
				N_PCINFO_DEF Packet;
				if( bHelmetDrop )
					pGameInfo->_tDefData._nHelmet = HELMET_STATE_DROP;
				g_pGameContext->BuildDefPacket( &Packet, &pGameInfo->_tDefData);
				pMulticast->setOriginalPacketChara_Def( getCharaNetIndex(), &Packet );
			}

			{
				N_PCINFO_HIT Packet;
				g_pGameContext->BuildHitPacket( &Packet, (UINT8) pDamageInfo->_nKillerIdx, (UINT8) part, 0 );
				pMulticast->setOriginalPacketChara_Hit( getCharaNetIndex(), &Packet );
			}

			pMulticast->setOriginalPacketChara_Type( getCharaNetIndex(), CHARA_PACKETTYPE_HIT );
			pMulticast->plusWritePos_Chara( getCharaNetIndex() );
		}
	}

	if( bHelmetDrop )
	{
		m_pEquipContext->DropToWorld( EQUIP_CLASS_TYPE_HELMET, &vHeadDir );

		VEC3D * pVecPos = NULL;
		VEC3D * pNormal = NULL;

		if( pDamageInfo->_pHitResult != NULL)
		{
			pVecPos = &pDamageInfo->_pHitResult->m_Intersect;
			pNormal = &pDamageInfo->_pHitResult->m_Normal;
		}
		else
		{
			pVecPos = i3Matrix::GetPos( m_pBoneContext->getHeadMatrix());
			pNormal = pDamageInfo->_pVecDir;
		}

		//	불꽃이 튄다.
		g_pEffectManager->AddEquipHitEffect( EQUIP_CLASS_TYPE_HELMET, pDamageInfo->_WeaponType, pVecPos, 
					pNormal, is1PV());
	}

	if( bDeath )
	{
		// Death chara & Physix set
		g_pCharaManager->DeathChara( getCharaNetIndex(), pDamageInfo->_nKillerIdx, pDamageInfo->_pVecDir, pDamageInfo->_DeathType, part );
	}

	return bDeath;
}


void CGameCharaPlayer::_PHYSIX_SetLinearVelocity( CHARA_DAMAGEINFO * pInfo )
{
	VEC3D vForce;
	VEC3D vDir;

	i3Vector::Normalize( &vDir, pInfo->_pVecDir );

	if( pInfo->_DeathType == CHARA_DEATH_B || pInfo->_DeathType == CHARA_DEATH_O )
	{
		i3PhysixShapeSet * pShapeSet;
		i3Body * pBody = GetSceneObject()->GetBody();
		i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();

		i3Vector::Scale( &vForce, &vDir, (REAL32)(pInfo->_rDamage) * 0.05f );
		
		for( INT32 i = 0; i < pSkel->getBoneCount(); i++ )
		{
			pShapeSet = pSkel->getShapeSet( i );

			if( pShapeSet == NULL )		continue;

			CHARA_HIT_PART part = GetHitPart( pShapeSet );

			if( part == CHARA_HIT_BODY1 )
			{
				VEC3D vAddLin;
				i3Vector::Scale( &vAddLin, &vForce, 1.5f );
				pShapeSet->setLinearVelocity( &vForce );
			}
			else if( part != CHARA_HIT_UNKNOWN )
			{
				pShapeSet->setLinearVelocity( &vForce );
			}
		}
		return;
	}

	if( pInfo->_pHitResult != NULL )
	{	
		i3Vector::Scale( &vForce, &vDir, (REAL32)(pInfo->_rDamage) * 0.5f );
		pInfo->_pHitResult->m_pDestShapeSet->setLinearVelocity( &vForce );
	}
}
*/

/*
"", "H_Head", "B_Neck", "B_L Clavicle", "B_R Clavicle",
	"B_Spine3", "B_Spine2", "B_Spine1", "B_Root", "AU_L UpperArm", "AD_L ForeArm",
	"AU_R UpperArm", "AD_R ForeArm", "HN_L Hand", "HN_R Hand",
	"LU_L Thigh", "LD_L Calf", "LU_R Thigh", "LD_R Calf",
	"F_L Foot", "F_R Foot" };
	*/

/*
CHARA_HIT_PART CGameCharaPlayer::GetHitPart( i3PhysixShapeSet * pDestShapeSet)
{	
	I3ASSERT( pDestShapeSet != NULL);

	CHARA_HIT_PART	part = CHARA_HIT_UNKNOWN;
	const char * pszCollName = pDestShapeSet->GetName();
	if( pszCollName == NULL ) return part;

	switch( pszCollName[0])
	{
		case 'M' : //Model_Helmet
			if( pszCollName[2] == 'd')
				part = CHARA_HIT_HEAD;
			break;
		case 'H' :
			if( pszCollName[1] == 'N' )
			{
				if( pszCollName[3] == 'L' )		part = CHARA_HIT_LHAND;		// Left Hand
				else							part = CHARA_HIT_RHAND;		// Right Hand
			}
			else
			{
				if( pszCollName[1] == 'E' && !m_pEquipContext->isAttachOnChara( EQUIP_CLASS_TYPE_HELMET ) )
					part = CHARA_HIT_HEAD;
				else
					part = CHARA_HIT_HEAD;									// Head
			}
			break;
		case 'B' :
			if( pszCollName[2] == 'N' )			part = CHARA_HIT_NECK;		// Neck
			else if( pszCollName[2] == 'S' )
			{
				switch( pszCollName[ 7 ] )
				{
				case '3' :	part = CHARA_HIT_BODY1;		break;				// Body1
				case '2' :	part = CHARA_HIT_BODY2;		break;				// Body2
				case '1' :	part = CHARA_HIT_BODY3;		break;				// Body3
				default : part = CHARA_HIT_UNKNOWN;		break;
				}
			}
			else if( pszCollName[4] == 'C' )
			{
				if( pszCollName[2] == 'L' ) part = CHARA_HIT_LCLAVICLE;		// Left Clavicle
				else						part = CHARA_HIT_RCLAVICLE;		// Right Clavicle
			}
			else							part = CHARA_HIT_ROOT;			// Root
			break;
		case 'A' :
			if( pszCollName[1] == 'U' )
			{
				if( pszCollName[3] == 'L' ) part = CHARA_HIT_LUARM;		// Left UpperArm
				else						part = CHARA_HIT_RUARM;		// Right UpperArm
			}
			else
			{
				if( pszCollName[3] == 'L' )	part = CHARA_HIT_LDARM;		// Left ForeArm
				else						part = CHARA_HIT_RDARM;		// Right ForeArm
			}
			break;
		case 'L' :
			if( pszCollName[1] == 'U' )
			{
				if( pszCollName[3] == 'L' )	part = CHARA_HIT_LULEG;		// Left Thigh
				else						part = CHARA_HIT_RULEG;		// Right Thigh
			}
			else
			{
				if( pszCollName[3] == 'L' )	part = CHARA_HIT_LDLEG;		// Left Calf
				else						part = CHARA_HIT_RDLEG;		// Right Calf
			}
			break;
		case 'F' :
			if( pszCollName[2] == 'L' )		part = CHARA_HIT_LFOOT;		// Left Foot
			else							part = CHARA_HIT_RFOOT;		// Right Foot
			break;
		default:
			{
				//I3TRACE( "[%s] WARNING! Invalid Hit Part (%s)\n", __FUNCTION__, pszCollName);
			}
			break;
	}

		
	//I3TRACE( "HIT NAME : %s\n", pszCollName);

	return part;
}

REAL32 CGameCharaPlayer::_CalcDamageLength( CHARA_DAMAGEINFO * pDamageInfo)
{	
	I3ASSERT( pDamageInfo != NULL);

	VEC3D InvVecDir, vShapePos;

	I3_PHYSIX_HIT_RESULT *	pHitResult = pDamageInfo->_pHitResult;
	i3PhysixShapeSet*		pDestShapeSet = pHitResult->m_pDestShapeSet;
	
	*((NxVec3 *) &vShapePos) = pDestShapeSet->getNxActor()->getGlobalPosition();

	i3Vector::Sub( &InvVecDir, &vShapePos, pDamageInfo->_pVecPos );

	return i3Vector::Length( &InvVecDir);
}

REAL32 CGameCharaPlayer::_CalcDamageRate( CHARA_DAMAGEINFO * pDamageInfo, REAL32 rDamagedLength)
{
	I3ASSERT( pDamageInfo != NULL);

	REAL32  rValidDamageRate = 1.0f;

	// 거리에 따른 데미지의 증감율( 1.0f가 Default입니다.)
	rValidDamageRate	= 1.0f - ((1.0f / pDamageInfo->_rExplosionRange) * rDamagedLength) * pDamageInfo->_rExplosionSlope;
 	if( rValidDamageRate < I3_EPS)	rValidDamageRate = I3_EPS;

	return rValidDamageRate;
}

INT32 CGameCharaPlayer::_CalcHit( CHARA_HIT_PART part, REAL32 nDamage, REAL32 rValidDamageRate)
{	
	I3ASSERT( g_pCharaInfoDataBase->getGlobal() != NULL);
	I3ASSERT( m_pCharaInfo != NULL );

#if defined ( I3_DEBUG)
//	char szPart[256] = "\0";
#endif

	if( m_rInvincibleElapseTime > 0.0f + I3_EPS)		//	무적 시간이므로 데미지를 받지 않는다. 
		return -1;

	REAL32 rCurrentDamage = nDamage * rValidDamageRate;
	REAL32 rRateDamage = 0;

	//	각부위에 따른 데미지 연산을 합니다.
	switch( part)
	{
	case CHARA_HIT_HEAD:			//	머리 데미지
		{
			rRateDamage = (REAL32) getDBInfo()->m_rDamageRateHead;
		}
		break;
	case CHARA_HIT_NECK	:
	case CHARA_HIT_LCLAVICLE :
	case CHARA_HIT_RCLAVICLE :		// 어깨
	case CHARA_HIT_BODY1 :
	case CHARA_HIT_BODY2 :
	case CHARA_HIT_BODY3 :
	case CHARA_HIT_ROOT	:			//	몸통 데미지
		{
			rRateDamage = (REAL32) getDBInfo()->m_rDamageRateBody;
		}
		break;

	case CHARA_HIT_RUARM:
	case CHARA_HIT_RDARM:
	case CHARA_HIT_LUARM:
	case CHARA_HIT_LDARM:			//	팔 데미지
		{
			rRateDamage = (REAL32) getDBInfo()->m_rDamageRateArm;
		}
		break;
	case CHARA_HIT_LHAND:
	case CHARA_HIT_RHAND:			//	손 데미지
		{
			rRateDamage = (REAL32) getDBInfo()->m_rDamageRateHand;
		}
		break;
	case CHARA_HIT_LULEG:
	case CHARA_HIT_LDLEG:
	case CHARA_HIT_RULEG:
	case CHARA_HIT_RDLEG:			//	다리 데미지
		{
			rRateDamage = (REAL32) getDBInfo()->m_rDamageRateLeg;
		}
		break;
	case CHARA_HIT_LFOOT:
	case CHARA_HIT_RFOOT:			//	발 데미지
		{
			rRateDamage = (REAL32) getDBInfo()->m_rDamageRateFoot;
		}
		break;
	default: rRateDamage = 0.f;
	}

	//	최종 데미지량 연산
	rCurrentDamage = (rRateDamage) * rCurrentDamage;

	//	HP 감소	
	m_pCharaInfo->_Hp -= (INT32) (rCurrentDamage + 0.5f);
	if( m_pCharaInfo->_Hp <= 0)		m_pCharaInfo->_Hp = 0;

	//I3TRACE( "HP : %d\n", m_pCharaInfo->_Hp);

	GAMEINFO_RECV * pGameInfo	= g_pGameContext->getRecvGameInfo( getCharaNetIndex() );

	{//Hit된경우 이동속도 느려지는 효과
		if( !isNetwork() && ( pGameInfo->_tDefData._nHP > (UINT8) m_pCharaInfo->_Hp))
		{
			m_pMoveContext->EnableSpeedReduction( pGameInfo->_tDefData._nHP - (UINT8) m_pCharaInfo->_Hp);
		}
	}

	pGameInfo->_tDefData._nHP	= (UINT8) m_pCharaInfo->_Hp;

#if defined( I3_DEBUG)
	//I3TRACE("%d %f \n",nCurrentDamage, rRateDamage); 
	//I3TRACE( "Hit! (Part: %s (%d) / HP: %d / Damage %d)\n", szPart, part, m_pCharaInfo->_Hp, nCurrentDamage);
#endif

	return m_pCharaInfo->_Hp;		//	현재 남은 HP 반환
}


void CGameCharaPlayer::DeleteBoundBox( void)
{
	// 캐릭터의 컬링을 끕니다.
	if( m_pBody->GetBound() != NULL )
		m_pBody->SetBound( NULL );

	// 무기의 컬링을 끕니다.
	WeaponBase * pWeapon = NULL;
	for( INT32 i = 0; i < MAX_ITEMSLOT_COUNT; i++)
	{
		pWeapon = getWeaponContext()->getWeapon( i);

		if( pWeapon != NULL)
			pWeapon->DeleteBoundBox();
	}

	//	장비의 컬링을 끕니다.
	CGameCharaEquip * pEquip = NULL;	
	for( INT32 i = 0; i < EQUIP_CLASS_TYPE_COUNT; i++)
	{
		pEquip = m_pEquipContext->getEquip( (EQUIP_CLASS_TYPE) i);

		if( pEquip != NULL)
			pEquip->DeleteBoundBox();
	}	
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void CGameCharaPlayer::EnableControl( BOOL bVal )
//{
//	if( bVal)
//	{
//		removeEnableStateMask( CHARA_STATE_DISABLE_CONTROL);
//	}
//	else
//	{
//		addEnableStateMask( CHARA_STATE_DISABLE_CONTROL);
//	}
//}

void CGameCharaPlayer::EnableCollision( BOOL bVal )
{
	if( bVal )
	{
		if( !isNetwork() )
			m_pController->setGroup( CGRP_MYCHARA_BOUND );
		else
			m_pController->setGroup( CGRP_CHARA_BOUND );
	}
	else
		m_pController->setGroup( CGRP_DUMMY );
}

void CGameCharaPlayer::_CalcClimbingLadder( VEC3D * vMoveDir)
{
	VEC3D vTemp;
	VEC3D vVelNormal;
	
	i3Vector::Sub( &vTemp, getLadderContactPosition(), GetPos());

	setY( &vTemp, 0.0f);
	i3Vector::Normalize( &vTemp, &vTemp);

	i3Vector::Copy( &vVelNormal, vMoveDir);
	setY( &vVelNormal, 0.0f);
	REAL32 rLength = i3Vector::Normalize(&vVelNormal, &vVelNormal);

	REAL32 rMove = i3Vector::Dot( &vVelNormal, &vTemp);

	if( i3Math::abs( rMove) > 0.707f)
	{
		i3Vector::Set( vMoveDir, 0.0f, rMove * rLength, 0.0f);
	}
	else
	{
		setY( vMoveDir, 0.0f);
	}
}


void CGameCharaPlayer::_CalcTakeObject( VEC3D * pFirstPos )
{
	if( m_pController->getHitShape() != NULL )
	{	// 오브젝트와 충돌, 오브젝트가 움직인다면..?
		i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) m_pController->getHitShape()->userData;
		i3EventReceiver * pReceiver = pShapeSet->getEventReceiver();
		if( pReceiver == NULL )
			return;

		VEC3D vUp;
		i3Vector::Set( &vUp, 0.f, 1.f, 0.f );

		if( pReceiver->IsExactTypeOf( i3Object::GetClassMeta() ) )
		{
			i3Object * pObj = ((i3Object*)pReceiver);
			
			i3Transform2 * pTrans = (i3Transform2*) i3Scene::FindNodeByExactType( pObj->GetSceneObject(), i3Transform2::GetClassMeta() );

			if( pTrans == NULL )
				return;

			if( pTrans->IsAnimated() )
			{	
				I3MATRIXINFO Env;
				MATRIX	matTop, matObj;

				VEC3D	vNewPosDir;

				if( i3Vector::Dot( m_pController->getHitNormal(), &vUp ) > I3_EPS )
					m_bTakeObject = TRUE;
				else
					m_bTakeObject = FALSE;

				i3Node * pParent = pTrans->GetParent();
				i3Matrix::Identity( &matTop );
				i3Matrix::Copy( &matObj, pTrans->GetMatrix() );

				// 해당 애니메이션을 계산합니다. 상위 노드의 Matrix를 가져와야 합니다.
				while( pParent != NULL )
				{	
					if( pParent->IsTypeOf( i3Transform::GetClassMeta() ) )
					{
						i3Matrix::Mul( &matTop, &matTop, ((i3Transform*)pParent)->GetMatrix() );
					}

					pParent = pParent->GetParent();
				}

				Env.m_pIn	= &matObj;
				Env.m_pOut	= &matObj;
				Env.m_Time	= g_pViewer->getDeltaTime();
				Env.m_pModelViewMatrix = &matTop;
				pTrans->GetAnimatedMatrix( &Env );

				// 애니메이션을 하고 다시 로컬 타임을 돌려놓습니다.
				pTrans->AddPlayTime( -g_pViewer->getDeltaTime() );

				i3Matrix::Mul( &matTop, &matObj, &matTop );

				// 애니메이션된 좌표에서 이전 프레임의 좌표의 차를 캐릭터 포지션에 더해줍니다.
				i3Vector::Sub( &vNewPosDir, i3Matrix::GetPos( &matTop ), i3Matrix::GetPos( pTrans->GetCacheMatrix() ));
				
				if( m_bTakeObject )
				{
					// 해당 오브젝트에 대한 로컬 좌표를 가져옵니다.
					MATRIX mInv;
					VEC3D vLocalPos;
					i3Matrix::Inverse( &mInv, 0, pTrans->GetCacheMatrix() );
					i3Vector::TransformCoord( &vLocalPos, GetPos(), &mInv );
					g_pGameContext->WriteMyCharaPos( &vLocalPos, getTheta(), getRho(), m_bTakeObject, pObj->getNetIndex() );
				}
				else
				{
					// 디렉션을 체크
					// 충돌하면 이동방향으로 조금더 밀어냅니다. (오브젝트에 걸리는 현상을 방지)
					if( i3Vector::Dot( m_pController->getHitNormal(), &vNewPosDir ) > 0.f )
					{
						REAL32 stepOff = i3Vector::Normalize( &vNewPosDir, &vNewPosDir);
						if( stepOff < 0.04f )
						{
							i3Vector::Scale( &vNewPosDir, &vNewPosDir, 0.04f );
						}
						else
						{
							i3Vector::Scale( &vNewPosDir, &vNewPosDir, stepOff );
						}
					}
					else
					{
						REAL32 stepOff = i3Vector::Normalize( &vNewPosDir, &vNewPosDir);
						if( stepOff < 0.04f )
						{
							i3Vector::Scale( &vNewPosDir, &vNewPosDir, -0.04f );
						}
						else
						{
							i3Vector::Scale( &vNewPosDir, &vNewPosDir, -stepOff );
						}
					}

					if( getActionContext()->isLower( CHARA_LOWER_JUMP) )
					{	// 점프중인 경우 속도를 0으로 합니다.
						setX( m_pMoveContext->getVelocity(), 0.f);
						setZ( m_pMoveContext->getVelocity(), 0.f);
					}
				}
				
				VEC3D vNewPos;
				i3Vector::Add( &vNewPos, &vNewPosDir, pFirstPos );
				forceSetPos( &vNewPos );
			}
		}
	}
	else
	{
		m_bTakeObject = FALSE;
	}
}


void CGameCharaPlayer::CheckFloorTerrain( void )
{
	VEC3D vPos;
	i3CollideeLine ColLine;

	i3Vector::Copy( &vPos, GetPos());
	i3Vector::AddY( &vPos, 0.5f);
	ColLine.SetStart( &vPos);

	i3Vector::AddY( &vPos, - 1.0f);
	ColLine.SetEnd( &vPos);

	I3_PHYSIX_HIT_RESULT * pResult = g_pFramework->raycastClosest( &ColLine, CGRP_ATTACK );
	if((pResult != NULL) && (pResult->m_pTerrain != NULL))
	{
		m_CurFloorTerrainType = (I3_TERRAIN_TYPE)pResult->m_pTerrain->m_Terrain;
	}
}
*/

/*
void CGameCharaPlayer::Move( REAL32 tm )
{
	VEC3D	vVel;
	VEC3D	newPos;
	UINT32	hitMask;

	VEC3D vPos;

	if( isNetwork() )
		return;
	
	_UpdateJumping( tm);

	_CheckWorldCollision( tm);

//	if( isStateMask( CHARA_STATE_CLIMB))
//	{
//		setClimbingLadderEnable( _IsOverlappedBy( CGRP_OBJECT_LADDER));
//	}

	m_pController->getPosition( &vPos );
	
	if( isAlive() )
	{
		// 오브젝트에 올라탔는지 계산합니다.
		_CalcTakeObject( &vPos );
	}

	i3Vector::Copy( &vVel, m_pMoveContext->getVelocity());
	i3Vector::Copy( m_pMoveContext->getOldVelocity(), m_pMoveContext->getVelocity() );

	// 캐릭터 컬리젼 체크
	if( isInvincible() )		// 리스폰 시에만 합니다.
	{
		VEC3D vOtherDir;
		for( INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++ )
		{
			CGameCharaPlayer * pChara = g_pCharaManager->getChara( i );
			if( pChara != this && pChara->isAlive() )
			{
				i3Vector::Sub( &vOtherDir, this->GetPos(), pChara->GetPos() );
				REAL32 rLength = i3Vector::Length( &vOtherDir);//i3Vector::Normalize( &vOtherDir, &vOtherDir );

				if( rLength < 0.45f )
				{
					//rLength = 1.f - rLength;
					//i3Vector::Scale( &vOtherDir, &vOtherDir, rLength );
					i3Vector::Add( &vVel, &vVel, &vOtherDir );
					i3Vector::Add( m_pMoveContext->getVelocity(), m_pMoveContext->getVelocity(), &vOtherDir);
				}
			}
		}
	}

	// 이동 값이 있는지 체크
	if( (vVel.x != 0.0f) || (vVel.y != 0.0f) || (vVel.z != 0.0f))
	{
		REAL32	rSharpness = 0.3f * ( g_pViewer->getDeltaTime() / 0.016666f);
		REAL32	stepOff = i3Vector::Length( &vVel) * 0.2f;
		if( stepOff > 0.3f)
			stepOff = 0.3f;
		else if( stepOff < 0.01f)
			stepOff = 0.01f;
		
		m_pController->Move( &vVel, stepOff, &hitMask, rSharpness);

		m_rFloorCheckTimer += g_pViewer->getDeltaTime();

		if( hitMask & NXCC_COLLISION_DOWN)
		{	
			if( !isStateMask( CHARA_STATE_FLOOR) || m_rFloorCheckTimer > PHYSIX_FLOORCHECK_TIME )
			{	
				CheckFloorTerrain();

				m_rFloorCheckTimer = 0.f;
			}

			// 바닥에 닿았다면....
			addStateMask( CHARA_STATE_FLOOR);

			// 바닥의 경사면에 따라 캐릭터를 미끄러트립니다.
			//NxShape * pShape = m_pController->getHitShape();
			//if( pShape != NULL )
			{
				VEC3D * pFloorNormal = m_pController->getHitNormal();
				REAL32 rDot = i3Vector::Dot( &I3_YAXIS, pFloorNormal );
				if( rDot < 0.5f && rDot > I3_EPS)
				{	
					setX( &vVel, getX( pFloorNormal) * 0.02f );
					setZ( &vVel, getZ( pFloorNormal) * 0.02f );
					setY( &vVel, -0.04f );

					stepOff = i3Vector::Length( &vVel) * 0.1f;
					if( stepOff > 0.3f)
						stepOff = 0.3f;

					m_pController->Move( &vVel, stepOff, &hitMask);

					addStateMask( CHARA_STATE_SLOPESLIDE); //m_bSlide = TRUE;
				}
				else
				{
					removeStateMask( CHARA_STATE_SLOPESLIDE); //m_bSlide = FALSE;
				}

				
				// 히트된 위치와 아래쪽의 Sphere와의 각도차를 계산하여 캐릭터를 옆으로 떨어뜨려야 합니다.
				// 점프시에 문제 해결
				VEC3D * pHitWorldPos = m_pController->getHitPos();
				VEC3D	vDownSpherePos;

				setX( &vDownSpherePos, getX( i3Matrix::GetPos( getIKMatrix())) ); 
				setY( &vDownSpherePos, getY( i3Matrix::GetPos( getIKMatrix())) - (TALL*0.5f) );
				setZ( &vDownSpherePos, getZ( i3Matrix::GetPos( getIKMatrix())) );
				
				i3Vector::Sub( &vDownSpherePos, &vDownSpherePos, pHitWorldPos);
				i3Vector::Normalize( &vDownSpherePos, &vDownSpherePos);
				REAL32 rDotAngle = i3Vector::Dot( &vDownSpherePos, &I3_YAXIS);
				if( rDotAngle < 0.7f && rDotAngle > 0.f)
				{
					setX( &vVel, getX( &vDownSpherePos) * 0.2f);
					setZ( &vVel, getZ( &vDownSpherePos) * 0.2f);


					m_pController->Move( &vVel, stepOff, &hitMask);

					//_ResetJumping();
				}
				
			}
		}
		else
		{
			// 바닥에서 떯어졌다면
			//m_bCollidedFloor = FALSE;
			removeStateMask( CHARA_STATE_FLOOR | CHARA_STATE_SLOPESLIDE);

			if( !isStateMask( CHARA_STATE_JUMP))
			{
		//		I3TRACE("or:%f, vel: %f, new:%f\n", getY( &newPos), vPos.y, vVel.y);
				setY( &newPos, getY( &vPos) + vVel.y);
			}
		}

		m_pController->getPosition( &newPos);

		if( isStateMask( CHARA_STATE_JUMP) && (hitMask & NXCC_COLLISION_UP))
		{
			_ResetJumping();

			i3Vector::Add( &newPos, 0.f, -getY( &vVel), 0.f );
		}
	#if 1
		i3Vector::Copy( GetPos(), &newPos);
	#else
		i3Vector::Sub( m_pMoveContext->getVelocity(), &newPos, GetPos());

		i3Vector::Copy( GetPos(), &newPos);
	#endif	
	}

	//m_rMoveTime -= 0.016f;
	//if( m_rMoveTime >= 0.016f)
		//m_rMoveTime = 0.f;
	
}



void CGameCharaPlayer::MoveStop( void)
{
	i3Vector::Zero( m_pMoveContext->getMoveDir());

	m_pMoveContext->setAccel( 0.0f, 0.0f, 0.0f);
	m_pMoveContext->setVelocity( 0.0f, 0.0f, 0.0f);	
}


void CGameCharaPlayer::forceSetPos( VEC3D * pPos)
{	
	CGameCharaBase::forceSetPos( pPos);
	m_pMoveContext->SetPosition( pPos);
}

void CGameCharaPlayer::GetVirtualCamPosition( VEC3D * pPos, MATRIX * pMat, bool bDump)
{
	I3ASSERT( g_pCameraManager != NULL);

	REAL32	Theta;
	VEC3D	vAt, vPos;
	MATRIX	mtx;

	i3Matrix::Identity( &mtx);
	i3Vector::Copy( &vAt, i3Matrix::GetAt( GetMatrix()));

	Theta = atan2f( getX( &vAt), getZ( &vAt));
	i3Matrix::SetRotateY( &mtx, Theta + I3_DEG2RAD( 180.0f));

	//Rotate
	i3Matrix::PostRotateAxis( &mtx, i3Matrix::GetRight( GetMatrix()), - getRho());
	i3Vector::TransformCoord( &vAt, &m_pDBInfo->m_vFPSCamPos, &mtx);
	
	if( is1PV())
	{
		i3Vector::Copy( &vPos, i3Matrix::GetPos( getBoneMatrix( BONE_NECK)));
	}
	else
	{
		i3Vector::Copy( &vPos, i3Matrix::GetPos( getBoneMatrix( BONE_IK)));
		i3Vector::SetY( &vPos, getY( GetPos()) + m_pCameraContext->getIKHeightForCam());
	}
	//i3Vector::SetY( &vPos, getY( GetPos()) + getIKHeightForCam());
	i3Vector::Add( pPos, &vAt,&vPos);

	if( pMat != NULL )
		i3Matrix::Copy( pMat, &mtx );
}
*/

/*
#define SIT_HEIGHT -0.6f

void CGameChara::_setStandOn1PV( BOOL bStand, REAL32 rTime)
{
	m_rChangePoseTime		= rTime;	//자세 변경시 걸리는 시간
	m_rChangePoseAccTime	= 0.0f;		//자세 변경의 누적 시간
	m_bChangePoseEnable		= TRUE;		//자세 변경이 시작되었음을 알리는 플래그
	m_bChangeToStand		= bStand;	//선자세로 변경인지 앉은자세로 변경인지.
}
*/

/*
void CGameCharaPlayer::_setStandOn1PV( CHARA_STATE_FOR_CAM nState, REAL32 rTime)
{
	if( nState == m_StateForCam)
		return;

	if( isStateMask( CHARA_STATE_CHANGEPOSE))
	{// 자세 변경중이다.
		m_rChangePoseAccTime	= rTime * ( (m_rChangePoseTime - m_rChangePoseAccTime) / m_rChangePoseTime);		//자세 변경의 누적 시간
	}
	else
	{
		m_rChangePoseAccTime	= 0.0f;
		addStateMask( CHARA_STATE_CHANGEPOSE);
		//m_bChangePoseEnable		= TRUE;								//자세 변경이 시작되었음을 알리는 플래그
	}

	if( m_nChangeFromState == CSFC_JUMP || nState == CSFC_JUMP)
	{
		m_rChangePoseAccTime = rTime * 0.1f;
	}

	m_rChangePoseTime		= rTime;								//자세 변경시 걸리는 시간
	m_nChangeFromState		= m_StateForCam;
	m_StateForCam			= nState;

	//자세 변경시 시작부터 컨트롤러를 바꾼다.
	{
		if( m_StateForCam == 2)
		{
			m_pMoveContext->ChangeControllerHeight( HALFTALL);
		}
		else
		{
			m_pMoveContext->ChangeControllerHeight( TALL);
		}
	}
}
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	HUD
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
BOOL CGameCharaPlayer::ApplyCrosshair( void)
{
	CStageBattle * pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
	I3ASSERT( pStage != NULL);

	BOOL bEnableCross = TRUE;			// CH가 나타날지 말지를 결정
	BOOL bScopeCH = FALSE;				// Scope용 CH가 나타날지를 결정

	if( isNetwork())	return FALSE;

	if( g_pCamera != NULL)
	{
		if( g_pCamera->getCurTarget() != this)		return FALSE;	
	}

	if( pStage->getCrosshair() == NULL)			return FALSE;

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
	{
		bEnableCross = FALSE;
	}
	else
	{
		CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
		I3ASSERT( pWeaponInfo != NULL);

		//	Crosshair
		if( isAlive())
		{
			switch( pWeaponInfo->getExtensionType())
			{
				case WEAPON_EXT_SCOPE :
					// Ext.가 활성화 되는 것만으로 Crosshair가 사라져야 함.
					bEnableCross = !pWeapon->isExtensionActivate();
					bScopeCH = isStateMask( CHARA_STATE_ZOOM);
					break;

				case WEAPON_EXT_SCOPE2 :
					bEnableCross = TRUE;
					bScopeCH = isStateMask( CHARA_STATE_ZOOM);
					break;

				case WEAPON_EXT_ZOOM :
				case WEAPON_EXT_DOT_SIGHT :
					bEnableCross = ! isStateMask( CHARA_STATE_ZOOM);
					break;

				//case WEAPON_EXT_DUAL_MAG :
				//case WEAPON_EXT_SILENCER :
				//case WEAPON_EXT_STEADY_AIM :
				//case WEAPON_EXT_NONE :
			}

			if( pWeaponInfo->m_TypeClass == WEAPON_CLASS_SNIPER)
			{
				bEnableCross = isStateMask( CHARA_STATE_ZOOM);
				bScopeCH = TRUE;
			}
		}
		else
		{
			bEnableCross = FALSE;	//	캐릭터가 죽은 경우
		}
		
		if( pWeaponInfo->m_CrossHairInvisible & 0x00000002)
		{
			// 이 후에 강제로 Scope CH가 사라진다고 하더라도, 현재 ScopeCH가 ON이라는 것은
			// Scope 모드이기 때문에 일반 CH는 나타나지 않아야 한다.
			if( bScopeCH == TRUE)
				bEnableCross = FALSE;

			// Scope Crosshair
			bScopeCH = FALSE;
		}

		if( pWeaponInfo->m_CrossHairInvisible & 0x00000001)
		{
			// Normal Crosshair
			if( bScopeCH == FALSE)
				bEnableCross = FALSE;
		}
	}

	if( g_pCamera->IsExactTypeOf( CGameCamera_Observer::GetClassMeta()))
	{
		// 관전 시에는 Crosshair를 항상 Disable 시킨다.
		bEnableCross = FALSE;
	}

	if( g_pGameContext->getUserDeath( g_pPlayer->getCharaNetIndex()))
		bEnableCross = FALSE;

	//bEnableCross = TRUE;

	// Dragonov와 SSG69와 같이 순줌이 되는 무기의 경우엔 Scope_CH를 나타내지 않는다.
	bScopeCH = bScopeCH && pWeapon->getScopeCrosshairEnable();

	//	Apply Crosshair	
	pStage->setScopeCrosshair( bScopeCH == TRUE);
	pStage->enableCrossHair( bEnableCross);

	return TRUE;
}

void CGameCharaPlayer::SetCharaNick(BOOL bFriendly)
{
	CStageBattle	* pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
	I3ASSERT( pStage != NULL);

	CHud * pHud = pStage->getHud();

	REAL32 rPosX, rPosY;

    if( _CalcProjectionPos( &rPosX, &rPosY, GetPos() ))
	{
 		rPosX *= 0.5f;		rPosX += 0.5f;
		rPosY *= -0.5f;		rPosY += 0.5f;
		
		VEC3D vTemp;
		char szTemp[MAX_STRING_COUNT] = {0,};
		i3Vector::Sub( &vTemp, g_pPlayer->GetPos(), GetPos());
		INT32 nLength = (INT32)i3Vector::Length(&vTemp);
		if( nLength > 9000 || nLength < -9000) nLength = 0;
		
		//if( g_pCharaManager->getCharaByNetIdx( getCharaNetIndex()] != NULL )
		{
			if( g_pPlayer->isAlive() )
			{
#if defined(I3_DEBUG)
				sprintf( szTemp, "%s [%dm]-%dHP", g_pGameContext->getNickForSlot(getCharaNetIndex()), nLength, getCurHP());
#else
				sprintf( szTemp, "%s", g_pGameContext->getNickForSlot(getCharaNetIndex()) );
#endif
			}
			else
			{
#if defined(I3_DEBUG)
				sprintf( szTemp, "%s -%dHP", g_pGameContext->getNickForSlot(getCharaNetIndex()), getCurHP());
#else
				sprintf( szTemp, "%s", g_pGameContext->getNickForSlot(getCharaNetIndex()) );
#endif
			}
		}

		// Friendly
		pHud->SetCharaNick( szTemp, bFriendly, rPosX, rPosY, (REAL32) nLength);
	}
}


void CGameCharaPlayer::SetNickOnTarget()
{	
	BOOL	bNeutralColor = TRUE;

	// 캐릭터의 넷 인덱스가 -1인 경우는 해당 캐릭터가 나간경우이다.
	// 업데이트 순서상 이전에 체크된 캐릭터이므로 -1이면 출력하지 말아야합니다. -by dfly 2007.12.20
	if( m_pCrossHitChara != NULL && m_pCrossHitChara->getCharaNetIndex() != -1 && m_pCrossHitChara->isNetwork() )
	{
		m_pCrossHitChara->SetCharaNick( FALSE);
	}

	m_rCrossHitTimer += g_pViewer->getDeltaTime();
	// 초당 15회만 체크
	if( m_rCrossHitTimer < HUD_CROSSNICKCHECK_TIME )
		return;
		
	CStageBattle * pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
	I3ASSERT( pStage != NULL);
	Crosshair * pCrosshair = pStage->getCrosshair();
	
	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )	return;
	CWeaponInfo * pInfo = pWeapon->getWeaponInfo();
	if( pInfo == NULL )	return;

	// 월드 컬리젼
	CheckCrossHit();

	if( pInfo->m_TypeClass == WEAPON_CLASS_THROWING || pInfo->m_TypeClass == WEAPON_CLASS_ITEM ||
		pInfo->m_TypeClass == WEAPON_CLASS_MISSION )
	{
		bNeutralColor = TRUE;
		m_pCrossHitChara = NULL;
	}
	else
	{
		UINT32 roughGroup, collGroup;
		I3_PHYSIX_HIT_RESULT * pRoughResult, * pResult;
		
		if( getCharaTeam() & CHARACTER_TEAM_BLUE )
		{
			roughGroup = CGRP_TEAM_BLUE_ROUGH;
			collGroup = CGRP_TEAM_BLUE_ATTACK;
		}
		else
		{
			roughGroup = CGRP_TEAM_RED_ROUGH;
			collGroup = CGRP_TEAM_RED_ATTACK;
		}

		REAL32 rMinLength = 10000.f;

		// 러프 테스트
		UINT32 nRoughCount = g_pFramework->raycast( &m_CrossHitLine, roughGroup);
		if( nRoughCount > 0)
		{
			pRoughResult = g_pFramework->getRaycastResult();
			g_pFramework->reserveRaycastResult( nRoughCount );

			for( UINT32 i = 0; i < nRoughCount; ++i)
			{
				i3EventReceiver * pObj = pRoughResult[ i ].m_pDestShapeSet->getEventReceiver();

				if( pObj == NULL)	continue;
				
				if( m_CrossHitResult.m_T < pRoughResult[ i ].m_T || pRoughResult[ i].m_T >= rMinLength)
				{	
					m_pCrossHitChara = NULL;
					continue;
				}

				UINT32 nResultCount = g_pFramework->raycast( &m_CrossHitLine, collGroup);
				if( nResultCount == 0 )
				{
					m_pCrossHitChara = NULL;
					continue;
				}
				pResult = g_pFramework->getRaycastResult();

				for( UINT32 j = 0; j < nResultCount; j++ )
				{
					if( pResult[ j ].m_pDestShapeSet->getEventReceiver() != pObj )
						continue;

					if( m_CrossHitResult.m_T < pResult[ j ].m_T )// || pResult[ j].m_T >= rMinLength)
					{
						m_pCrossHitChara = NULL;
						continue;
					}

					if( pResult[ j ].m_T > pInfo->m_rRange )
					{
						m_pCrossHitChara = NULL;
						continue;
					}

					if( !pObj->IsExactTypeOf( CGameCharaBase::GetClassMeta() ) )
					{
						m_pCrossHitChara = NULL;
						continue;
					}

					if( pResult[ j].m_T < rMinLength )
					{
						m_pCrossHitChara = (CGameCharaPlayer *) pObj;
						if( m_pCrossHitChara->isAlive())
						{
							bNeutralColor = FALSE;
						}

						rMinLength = pResult[ j].m_T;
					}
				}
			}

			g_pFramework->resetRaycastResult();
		}
		else
			m_pCrossHitChara = NULL;
	}

	if( bNeutralColor)//보통색
	{
		// 조준된 상대라도 죽엇으면 보통 색상으로 변경
		pCrosshair->SetCrosshairColor( 0, 255, 0);
	}
	else			  //뻘건색
	{	
		pCrosshair->SetCrosshairColor( 255, 0, 0);
	}

	m_rCrossHitTimer = 0.f;
}

BOOL CGameCharaPlayer::_CalcProjectionPos( REAL32 * pOutX, REAL32 * pOutY, VEC3D * pIn )
{
	VEC3D	vTemp;
	VEC3D	vTempIn;
	MATRIX	CamMat;
	VEC3D*	pvCamAt;
	VEC3D	vSub;
	REAL32	Height = 1.9f;

	if( getActionContext()->isLower( CHARA_LOWER_DEATH ) )	return FALSE;

	pvCamAt = i3Matrix::GetAt( g_pCamera->getCamMatrix());

	i3Vector::Sub( &vSub, g_pPlayer->GetPos(), pIn );
	i3Vector::Normalize( &vSub, &vSub );

	if( i3Vector::Dot( pvCamAt, &vSub) < 0.f ) 
		return FALSE;
	
	i3Matrix::Mul( &CamMat, g_pCamera->getViewMatrix(), g_pCamera->getProjectionMatrix());

//	if( STATE_getBody() == CHARA_BODY_CROUCH )
//		Height = 1.25f;

	i3Vector::Set( &vTempIn, i3Vector::GetX( pIn ), i3Vector::GetY( pIn ) + Height, i3Vector::GetZ( pIn ) );
	
	i3Vector::TransformCoord( &vTemp, &vTempIn, &CamMat );

	*pOutX	= i3Vector::GetX( &vTemp );
	*pOutY	= i3Vector::GetY( &vTemp );

	return TRUE;
}

void CGameCharaPlayer::_QueContactReport( I3_PHYSIX_CONTACT_REPORT * pReport )
{
	UINT32 nCount = m_nContactReportCount - m_nContactReportReadPos;
	if( nCount > PHYSIX_CONTACT_REPORT_COUNT )
	{
		I3TRACE( "CGameCharaPlayer::_QueContactReport() - buffer over flow.\n" );
		return;
	}

	UINT32 i = m_nContactReportCount % PHYSIX_CONTACT_REPORT_COUNT;

	i3Physix::ContactReportCopyTo( &m_ContactReportBuf[ i ], pReport );

	m_nContactReportCount++;
}

void CGameCharaPlayer::_ParseContactReport( void )
{
	UINT32 nCount = m_nContactReportCount - m_nContactReportReadPos;
	if( nCount == 0 )
		return;

	UINT32 i;

	BOOL	bCheck = FALSE;
	INT32	nEffAndSndCnt	= 0;
	VEC3D	vUp;

	i3Vector::Set( &vUp, 0.f, 1.f, 0.f );

	for( i = 0; i < nCount; i++ )
	{
		UINT32 nPos = (m_nContactReportReadPos + i) % PHYSIX_CONTACT_REPORT_COUNT;
		I3_PHYSIX_CONTACT_REPORT * pReport = &m_ContactReportBuf[ nPos ];

		i3PhysixShapeSet * pShapeSet = pReport->m_pOwnShapeSet;
		const char * pszName = pShapeSet->GetName();

		if( pszName[12] == 'R' && pszName[15] == 't')
		{
			bCheck = TRUE;
		}

		if( pReport->m_Events == NX_NOTIFY_ON_END_TOUCH )
		{
			if( bCheck )
			{
				removeStateMask( CHARA_STATE_PHYSIXCONTACT);
			}
		}
		else if( (pReport->m_Events == NX_NOTIFY_ON_START_TOUCH) )	//pReport->m_rNormalForce > PHYSIX_EFFECT_DUTY_FORCE
		{
			for( UINT32 j = 0; j < pReport->m_nPointCount; j++ )
			{	
				if( !isStateMask( CHARA_STATE_PHYSIXCONTACT) )
				{
					if( i3Vector::Dot( &pReport->m_Normal[ j ], &vUp ) > I3_EPS + 0.9f )
					{
						addStateMask( CHARA_STATE_PHYSIXCONTACT);
					}
				}

				{
					if( pszName[0] == 'C' && (pszName[12] == 'L' || pszName[12] == 'R') && isAlive() )
						g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_CHARA_WALK_COMMON, &pReport->m_Intersect[ j ], &pReport->m_Normal[ j ] );
					else
						g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_CHARA_DOWN_COMMON, &pReport->m_Intersect[ j ], &pReport->m_Normal[ j ] );

					if( bCheck )
					{
						//if( pReport->m_rNormalForce[ j ] > 0.f && nEffAndSndCnt  <= 0 && pReport->m_pDestShapeSet != NULL)
						if( nEffAndSndCnt  <= 0 && pReport->m_pDestShapeSet != NULL && (m_DropCount == 0 || pReport->m_rNormalForce[ j ] > 0.f) )
						{
							I3_PHYSIX_TERRAIN * pTri = pReport->m_pDestShapeSet->getTerrain(0);
							if( pTri != NULL)
							{
								g_pSndMng->StartCharacterActionSnd( GTBCA_ON_THUD, i3Math::Rand()%2, &pReport->m_Intersect[ j ], is1PV() );
								m_DropCount++;
								//I3TRACE("터어어얼~~~쓰~~~어~~~억~~~..!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
							}

							nEffAndSndCnt++;
						}
					}
				}
			}
		}
	}

	m_nContactReportReadPos += nCount;
}


BOOL CGameCharaPlayer::OnEvent( UINT32 event, i3ElementBase *pObj, UINT32 param )
{
	switch( event )
	{
	case I3_EVT_CONTACT :
		if( pObj->IsExactTypeOf( i3PhysixShapeSet::GetClassMeta()) )
		{
			i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) pObj;
			
			for( INT32 i = 0; i < pShapeSet->getContactCount(); i++ )
			{
				I3_PHYSIX_CONTACT_REPORT * pReport = pShapeSet->getContactReport( i );

				getCollisionContext()->QueContactReport( pReport );
			}

			return TRUE;
		}
		break;
	}

	return i3GameObj::OnEvent( event, pObj, param );
}


void CGameCharaPlayer::ProcessMissionEnd( void )
{
	removeEnableStateMask( CHARA_STATE_DISABLE_SHOT);
	removeEnableStateMask( CHARA_STATE_DISABLE_MOVE);
	removeEnableStateMask( CHARA_STATE_DISABLE_CROUCH);
}


REAL32 CGameCharaPlayer::AnimUpperScale( ID_UPPER_AI ai, CWeaponInfo * pWeaponInfo, REAL32 time)
{
	REAL32 rTimeScale;

	if( m_pAnimContext->getAIUpper() == NULL )
		return 1.f;

	if( time <= 0.f)
		return 1.f;

	I3ASSERT( pWeaponInfo != NULL);
	
	INT32 nID = m_pAnimContext->getAIUpperID( getAnimSet(), pWeaponInfo->m_TypeClass, pWeaponInfo->m_nNumber, getActionContext()->getBody2(), ai);
	i3AIState * pAIState = m_pAnimContext->getAIUpper()->getAIState( nID );

	rTimeScale = pAIState->getDuration() / time;

	return rTimeScale;
}

void CGameCharaPlayer::SetMoveDir( UINT64 key)
{
	if( key & GAME_KEY_MOVE_FORWARD )			setZ( m_pMoveContext->getMoveDir(), 1.f);
	else if( key & GAME_KEY_MOVE_BACKWARD)		setZ( m_pMoveContext->getMoveDir(), -1.f);
	else													setZ( m_pMoveContext->getMoveDir(), 0.f);

	if( key & GAME_KEY_MOVE_LEFT )				setX( m_pMoveContext->getMoveDir(), 1.f);
	else if( key & GAME_KEY_MOVE_RIGHT )		setX( m_pMoveContext->getMoveDir(), -1.f);
	else													setX( m_pMoveContext->getMoveDir(), 0.f);

}

*/

