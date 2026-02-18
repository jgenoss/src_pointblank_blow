#include "pch.h"
#include "WeaponBase.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "BattleHud.h"
#include "TracerBulletManager.h"
#include "Crosshair.h"
#include "GameCharaAnimContext.h"
#include "GameCharaBoneContext.h"
#include "WeaponGlobal.h"
#include "GameCharaCameraContext.h"
#include "GameCharaCollisionContext.h"
#include "GameCharaMoveContext.h"
#include "GameCharaWeaponContext.h"
#include "GameCharaInputContext.h"
#include "GameCharaEquipContext.h"
#include "GameObject.h"
#include "WeaponM197.h"
#include "WeaponDino.h"
#include "GameCharaActionContext.h"
#include "../Character/InputCtrlAI_ForHuman.h"
#include "TutorialMode.h"
#include "WeaponItemID.h"
#include "CrosshairMgr.h"
#include "GameMode.h"
#include "ObjExplosion.h"
#include "Weaponinfo/MeleeWeaponInfo.h"

#ifdef DOMI_DINO
#include "../Character/Fullbody/GameCharaDomiSting.h"
#include "../Mode/Domination/Domination_Defines.h"
#include "../Mode/Domination/DomiUserSkillMgr.h"
#include "../Mode/Domination/DomiSkillProperties.h"
#endif

#include "i3Base/string/ext/snprintf.h"

#include "../StageBattle/UserContext.h"
#include "BattleServerContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "BattleObjContext.h"
#include "UserInfoContext.h"
#include "GameMissionManager.h"
#include "WeaponFunction.h"
#include "MainWeapon_C5.h"
#include "MainWeapon_Claymore.h"
#include "MainWeapon_M14.h"
#include "Designation/DesigApplyInfo.h"

#include "i3Base/string/ext/utf16_to_mb.h"

#if 0
#define CASHITEM_TRACE	I3TRACE
#else
#define CASHITEM_TRACE	__noop
#endif

namespace
{
	enum PENALTY_TYPE	{ PENALTY_HORZRECOIL = 0, PENALTY_VERTRECOIL, PENALTY_DAMAGE, PENALTY_DEVIATION };

	// Calculate Endurance Penalty
	void CalEndurancePenalty( PENALTY_TYPE Type, UINT8 ui8Endurance, REAL32& rVal )
	{
		EndurancePenalty*	pPenalty	= g_pConfigEx->GetEndurancePenalty( ui8Endurance );

		if( pPenalty == nullptr ) return;

		switch( Type )
		{
		case	PENALTY_HORZRECOIL :	rVal	*= pPenalty->HorzRecoil;	break;
		case	PENALTY_VERTRECOIL :	rVal	*= pPenalty->VertRecoil;	break;
		case	PENALTY_DAMAGE :		rVal	*= pPenalty->Damage;		break;
		case	PENALTY_DEVIATION :		rVal	*= pPenalty->Deviation;		break;
		}

		return;
	}
}

I3_CLASS_INSTANCE( WeaponBase); //, i3Chara);


WeaponBase::WeaponBase( void)
{
	_OnInitVairable();		
}	

WeaponBase::~WeaponBase( void)
{
	_StopFireSound();
	_StopSwapSound();
	_StopIdleSound();

	for( INT32 i = 0; i < WEAPON::FIRE_SOUND_COUNT; i++)
	{
//		I3_SAFE_RELEASE( m_pFireSound[ i]);
		I3_SAFE_RELEASE( m_pFireSoundPlayInfo[ i]);
	}

	I3_SAFE_RELEASE( m_pSwapSoundPlayInfo);
	I3_SAFE_RELEASE( m_pIdleSoundPlayInfo);

	if( m_pLight != nullptr)
	{
		g_pFramework->removeLight( m_pLight);

		I3ASSERT( m_pLight->GetRefCount() == 1);
		I3_SAFE_RELEASE( m_pLight);
	}

	I3_SAFE_RELEASE( m_pContactHitBox);
	I3_SAFE_DELETE(m_pDamage);

	#if defined( AI_MOVE_TO_WEAPON)
	I3_SAFE_RELEASE( m_pCharaAINode);
	#endif

	i3::cu::for_each_delete(m_listEmptyBulletSound);
	i3::cu::for_each_delete(m_listTerrainSound);

	I3_SAFE_RELEASE( m_pFXLastTrans );

	I3_SAFE_DELETE(m_StartPos);
	I3_SAFE_DELETE(m_TargetPos);

}

void WeaponBase::_OnInitVairable( void)
{

	m_pWeaponInfo		= nullptr;
	m_pAbsoluteWeaponInfo = nullptr;
	m_pBody				= nullptr;

	#if defined( AI_MOVE_TO_WEAPON)
	m_pCharaAINode		= nullptr;
	#endif

	m_pExtWeaponInfo	= nullptr;

	m_pAICtx			= nullptr;

	i3mem::Fill( m_AIIndex, -1, sizeof( m_AIIndex));
	
	m_idxFXBone					= -1;
	m_idxFXIdleBone				= -1;

	i3mem::Fill(m_idxFireEffect, -1, sizeof(m_idxFireEffect));

	m_idxFXBullet				= -1;
	m_idxBulletEffect			= -1;
	m_idxFireEffect_1PV_Zoom	= -1;
	m_idxFXLaserSight			= -1;

	m_rLastIdleEffectTime = 0.0f;
	
	m_pAttrSet			= nullptr;
	m_pMaterialAttr		= nullptr;
	m_pBlendEnable		= nullptr;	
	m_fAlphaVal			= 0.0f;

	m_timeLight			= 0.0f;

#if defined( TEST_COLLISION_LINE )
	m_pVA			= nullptr;
#endif

	for(INT32 i = 0; i < WEAPON::FIRE_SOUND_COUNT; i++)
	{
		m_pFireSoundPlayInfo[ i]	= nullptr;		
	}
	m_pFireSoundPlayState	= nullptr;
	m_pSwapSoundPlayInfo	= nullptr;
	m_pSwapSoundPlayState	= nullptr;
	m_pIdleSoundPlayInfo	= nullptr;
	m_pIdleSoundPlayState	= nullptr;
	
	m_Idx				= -1;						// 리셋되면 안됩니다.
	m_NetIdx			= -1;						// 리셋되면 안됩니다.
	setWeaponState( WEAPON::STATE_READY);
	m_StateMaskWeapon	= 0;
	m_FireOrder			= WEAPON::PRIMARY_FIRE;
	m_AttachGrip		= WEAPON_ATTACH_NONE;

	m_WeaponFlag		= 0;
	
	m_DualMagazineMode			= WEAPON_DUAL_MAGAZINE_NONE;
	setLoadedBulletDualCount(0);
	
	setLoadedBulletCount(0);
	m_nMaxBullet		= 0;
    setAutoFireCount(0);
	setAutoPointShotCount(0);
	setRemainAutoFireCount(0);

	m_timeAutoFire		= 0.0f;

	m_bSetStart			= false;
	i3Vector::Zero( m_StartPos);
	i3Vector::Zero( m_TargetPos);

	addWeaponStateMask( WSM_EMPTY_CARTRIDGE | WSM_LOADED_BULLET);

	ResetSound();
	
	SetHitGroup( false);
	m_rPenetrateLength = 0.0f;
	
	m_pOwner		= nullptr;
	m_pParentGame	= nullptr;

	m_pGunDummy			= nullptr;
	m_pGunDummyLeftHand = nullptr;
	m_pGunDummyBack = nullptr;
	i3Matrix::Identity( &m_mtxGunDummy);
	i3Matrix::Identity( &m_mtxGunDummyLeftHand);
	
//	m_nContactReportCount = 0;
//	m_nContactReportReadPos = 0;
	m_vecContactReport.clear();

	setDamageRate( 1.f);

	I3_SAFE_RELEASE( m_pContactHitBox);
	
	if( m_pLight != nullptr)
	{
		g_pFramework->removeLight( m_pLight);

		I3ASSERT( m_pLight->GetRefCount() == 1);
		I3_SAFE_RELEASE( m_pLight);
	}
	
	m_timeAfterZoom = 10.0f;

	i3mem::Fill(m_nCommandBuf, -1, sizeof(m_nCommandBuf));

	m_nPushCommandCount = 0;
	m_nPopCommandCount = 0;

	m_pPairWeapon	= nullptr;						// 리셋되면 안됩니다.

	m_bTestRoughCollision	= false;
	m_ui8Endurance			= 100;

	addWeaponFlag( WEAPON_FLAG_ENABLE_FIRE_SOUND);		//	default
	m_bOnceJump = false;
	m_bOnceSit = false;

	m_rFireTime = 0.f;
	m_rReloadTime = 0.f;

	m_tDropInfo.Reset();

//
	if (m_mainWeapon)
		m_mainWeapon->OnInitVariable();
//
}

void WeaponBase::_ResetGrenadeInfo(void)
{
	if(m_pOwner->getCharaNetIndex() == BattleSlotContext::i()->getMySlotIdx())
	{	
		INT32 GrenadeIdx =  getNetworkIdx() % MAX_THROWWEAPON_SLOP;
		if (GrenadeIdx > -1 && GrenadeIdx <= MAX_THROWWEAPON_SLOP)
		{
			GrenadeContext::i()->m_GrenadeCheck[GrenadeIdx].Reset();
		}
	}
}

void WeaponBase::Reset( void)
{

	m_StateMaskWeapon	= 0;
	m_FireOrder		= WEAPON::PRIMARY_FIRE;

	addWeaponStateMask( WSM_EMPTY_CARTRIDGE | WSM_LOADED_BULLET);

	m_CurrentExtensionType = m_pWeaponInfo->getExtensionType(0); //	첫번째 익스텐션으로 초기화 설정. 중간에 바뀔수 있는 값
	setLoadedBulletCount( GetLoadBulletWithCashItem());
	setLoadedBulletDualCount(0);
	
	INT32 nMaxBullet = GetMaxBulletWithCashItem();

	setWeaponState( WEAPON::STATE_READY);
	SetEnable( true);
	SetMatrix( &I3_IDENTITYMATRIX );			//	원위치로
		
	ResetSound();

	_OnPlayWeaponIdleAnim();

	ISNETWORK
	{
		if( m_pOwner )
		{
#if defined( USE_WEAPON_ENDURANCE)
			//	AI 캐릭터는 적용되면 안됨. komet
			if( m_pOwner->getCharaInfo() != nullptr && (m_pOwner->getCharaNetIndex() != -1) && (m_pOwner->isAI() == false) && 				
				m_pOwner->isAIToClient() == false &&
				(m_pWeaponInfo->GetTypeClass() != WEAPON_CLASS_ROCKET_LAUNCHER) &&
				(m_pWeaponInfo->GetTypeClass() != WEAPON_CLASS_OBJECT_GUN) &&
				(m_pWeaponInfo->GetTypeClass() != WEAPON_CLASS_MISSION) &&
				(m_pWeaponInfo->GetTypeClass() != WEAPON_CLASS_DINO) &&
				(m_pWeaponInfo->GetTypeClass() != WEAPON_CLASS_DINO_LEFT_SCRATCH))
			{
				const NET_CHARA_INFO * pCharaInfo = BattleSlotContext::i()->getNetCharaInfo( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());

				// 기본 지급무기 경우 내구도 처리를 하지 않도록 합니다.
				if( m_pWeaponInfo->IsExceptionEndurance() )
					SetEndurance( 100 );		// 내구도 무시 무기는 최대값 100
				else								
				{
					if( m_pWeaponInfo->GetTypeUsage() == WEAPON_SLOT_UNKNOWN )
					{
						I3TRACE("Weapon Slot Type Unknown : %d\n", m_pWeaponInfo->GetTypeUsage() );
					}
					else if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
					{
						SetEndurance( 100 );
					}
					else
					{
						WEAPON_SLOT_TYPE slotType =	m_pWeaponInfo->GetTypeUsage();

						if (slotType > WEAPON_SLOT_UNKNOWN &&  slotType < WEAPON_SLOT_COUNT)
						{
							WEAPON_SLOT_TYPE weaponSlotType = m_pWeaponInfo->GetTypeUsage();
							I3ASSERT_RETURN(weaponSlotType >= 0);
							SetEndurance(pCharaInfo->getEndurance(weaponSlotType));
						}
					}
				}
			}
#endif
		}
	}

	{
		setTotalBulletCount( nMaxBullet - getLoadedBulletCount());
	}

	//	Material
	if( m_pMaterialAttr != nullptr )
	{
		COLORREAL col;

		i3Color::Set( &col, 1.0f, 1.0f, 1.0f, 1.0f);
		m_pMaterialAttr->SetDiffuse( &col);
		m_pMaterialAttr->SetSpecular( &col);
		m_pMaterialAttr->SetShininess( 64.8f);		

		OnSetBlendAlpha( 1.0f);
	}
	
	ResetCommand();

	// Hit Group
	SetHitGroup( false);

	//	기본으로 오른손에 잡도록
	ChangeGunDummy( WEAPON_ATTACH_GRIP);

//	m_nContactReportCount = 0;
//	m_nContactReportReadPos = 0;
	m_vecContactReport.clear();

	m_bTestRoughCollision	= false;

	m_tDropInfo.Reset();

	//	xgkick
	setDynamicActiveState( true, false);
	SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC, true );

	m_mainWeapon->Reset();
}

void WeaponBase::SetExtWeaponInfo()
{
	// 익스텐션 기능 무기의 익스텐션 정보
	if( m_pWeaponInfo->isExtensible() && m_pWeaponInfo->hasExtensionInfo() )
	{
		m_pExtWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( m_pWeaponInfo->GetSubExtensionItemID() );
	}
}

void WeaponBase::Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon)
{
	
	// 미리 생성해두고, 사전조작을 할수 있도록 하기 위해..
	// 널조건검사를 추가함.
	if (!m_mainWeapon)
		m_mainWeapon.reset( this->CreateMainWeapon() );
	
	
	WEAPON_CLASS_TYPE classType = WEAPON::ItemID2Class( Itemid );
	WEAPON::RES_ID number = WEAPON::ItemID2ResID( Itemid );

	m_u8EncryptKey = (UINT8) ((i3Math::Rand() % 7) + 1);

	_OnInitVairable();

	m_pOwner = pOwner;

//	char szFireEffect[100];

	if( g_pWeaponInfoDataBase != nullptr)
	{
		//	공용 데이터베이스에 있는 무기 정보를 가져온다.
		m_pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( Itemid);
		if(m_pWeaponInfo == nullptr)
		{
			I3PRINTLOG(I3LOG_FATAL, "m_pWeaponInfo 가 nullptr임 Itemid %d \n  \
				PEF 파일에 무기 정보가 등록되어 있는지, 또는\n등록된 class 번호, number가 헤더와 일치하는지 확인하세요.", Itemid);
			return;
		}

		m_pAbsoluteWeaponInfo = m_pWeaponInfo;		//	처음 초기화때만 셋팅되는 메인 정보이다.

		m_CurrentExtensionType = m_pWeaponInfo->getExtensionType(0); //	첫번째 익스텐션으로 초기화 설정. 중간에 바뀔수 있는 값

		UINT32 flag = WEAPON_EXT_MASK_NEED_TO_ACTIVATE | WEAPON_EXT_MASK_DIRECT_USE;
		if( (m_pWeaponInfo->getExtensionType(0) & flag) && 
			(m_pWeaponInfo->getExtensionType(1) & flag) )
		{
			addWeaponFlag( WEAPON_FLAG_MULTIPLE_EXTENSION);
		}
		else
		{
			removeWeaponFlag( WEAPON_FLAG_MULTIPLE_EXTENSION);
		}

		SetExtWeaponInfo();

		//	무기 매니저에서 리소스를 받아온다.
		// 0.5ms
		i3GameResChara * pRes = g_pWeaponManager->QueryResource( classType, number);
		if( pRes == nullptr)
		{	
			i3::rc_wstring wstrName;
			//무기가 등록이 안된 경우 무기 이름 표시로 변경
			m_pWeaponInfo->GetWeaponName(wstrName);

			i3::string strName;	i3::utf16_to_mb(wstrName, strName);

			I3PRINTLOG(I3LOG_FATAL, "i3GameResChara 가 nullptr임 (Itemid %d ) - %s\n  \
						RSC 파일에 무기 I3CHR 파일을 등록했는지 확인하세요.", Itemid, strName.c_str() );
			return;
		}

		if( pRes->getSceneGraphInfo() == nullptr)
		{
			i3::rc_wstring wstrName;
			//무기가 등록이 안된 경우 무기 이름 표시로 변경
			m_pWeaponInfo->GetWeaponName(wstrName);

			i3::string strName;		i3::utf16_to_mb(wstrName, strName);

			I3PRINTLOG(I3LOG_FATAL, "i3SceneGraphInfo 가 nullptr임 (Itemid %d ) - %s\n  \
						 무기의 i3S 확인하세요.", Itemid, strName.c_str() );
			return;
		}

		// 2ms
		i3Chara::Create( g_pFramework, pRes);

		// 3ms
		Instantiate();

		//	노드 상위에 알파처리를 위한 AttrSet을 붙인다.
		{
			m_pAttrSet = i3AttrSet::new_object_ref();
			m_pAttrSet->SetPriority( 0);	

			while( getAttachNode()->GetChildCount() > 0)
			{
				i3Node * pChild = getAttachNode()->GetChild( 0);
				pChild->ChangeParent( m_pAttrSet);
			}	

			getAttachNode()->AddChild( m_pAttrSet);		

			m_pBlendEnable = i3BlendEnableAttr::new_object_ref();
			m_pBlendEnable->Set( false);
			m_pAttrSet->AddAttr( m_pBlendEnable);

			i3BlendModeAttr * pBlendMode = i3BlendModeAttr::new_object_ref();
			pBlendMode->SetSource( I3G_BLEND_SRCALPHA);
			pBlendMode->SetDestination( I3G_BLEND_INVSRCALPHA);
			m_pAttrSet->AddAttr( pBlendMode);

			i3FaceCullModeAttr * pFaceCullModeAttr =i3FaceCullModeAttr::new_object_ref();

			pFaceCullModeAttr->Set( I3G_FACE_CULL_NONE);

			m_pAttrSet->AddAttr( pFaceCullModeAttr);

			m_pMaterialAttr = i3MaterialAttr::new_object_ref();
			m_pAttrSet->AddAttr( m_pMaterialAttr);

			COLORREAL col;

			i3Color::Set( &col, 1.0f, 1.0f, 1.0f, 1.0f);
			m_pMaterialAttr->SetDiffuse( &col);
			m_pMaterialAttr->SetSpecular( &col);
			m_pMaterialAttr->SetShininess( 128.0f);
		}
    
		//	Set Update Mode
		m_pBody = getSceneObject()->GetBody();
		I3ASSERT( m_pBody != nullptr);

		//	임의로 변경하기 쉽게 하기 위해 강제로 LOD Slope값을 지정합니다. (기존에 CharaTool에서 지정한 Slope값은 사용하지 않습니다. komet)
		m_pBody->setSlope( WEAPON_LOD_SLOPE);
		m_pBody->setConst( 0.0f);
		m_pBody->removeStyle( I3BS_LOD_AUTO);
//		m_pBody->AddFlag( I3_NODEFLAG_MUSTBEPROCESS , true);

		{
			// 5.5ms
			_LoadAllSound();

			// FX를 붙일 더미의 Index
			m_idxFXBone = getSceneObject()->GetBody()->FindBoneByName( "FXDummy");
			if( m_idxFXBone == -1)
			{
				m_idxFXBone = getSceneObject()->GetBody()->FindBoneByName( "FxDummy");
			}

			if(LocaleValue::GetInt32("WeaponFxDummy") == 1)
			{
				// Body에서 FXDummy용 BoneRef를 찾아 대표 Node로 변환
				i3Node * pTempFX = nullptr;

				for( UINT32 i = 0; i < getSceneObject()->GetBody()->getLODCount(); i++)
				{
					i3Node * pTempNode = i3Scene::FindNodeByName( getSceneObject()->GetBody()->getLOD( i)->getShapeNode(), "FXDummy");
					if( pTempNode != nullptr)
					{
						I3_REF_CHANGE( pTempFX, pTempNode);
						pTempNode->RemoveFromParent();
					}
				}

				if( pTempFX != nullptr)
				{
					getSceneObject()->GetBody()->AddChild( pTempFX);
					pTempFX->Release();
				}

				// FXDummy Boneref가 있는지 검사
				i3Node * pTempBone = i3Scene::FindNodeByName( getSceneObject(), "FXDummy");
				if( i3::same_of<i3BoneRef*>(pTempBone) && (pTempBone->GetChildCount() > 0) )
				{
					i3Node * pChild = pTempBone->GetChild(0);
					if( i3::kind_of<i3Transform*>( pChild) )
						m_pFXTrans = static_cast<i3Transform*>(pChild);
				}
			}

			// 발사 무기는 반드시 FXDummy가 있어야 한다.
#if defined( I3_DEBUG)
			if( getWeaponInfo()->GetTypeFire() == WEAPON::FIRE_RANGE
				&& classType != WEAPON_CLASS_DINO && classType != WEAPON_CLASS_BOW && classType != WEAPON_SUBCLASS_BOW)
			{
				for(UINT32 i=0; i<getSceneObject()->GetBody()->getLODCount(); i++)	
				{
					if( getSceneObject()->GetBody()->getLOD( i)->getSkeleton()->FindBoneByName( "FXDummy") == -1)
					{
						if( getSceneObject()->GetBody()->getLOD( i)->getSkeleton()->FindBoneByName( "FxDummy") == -1)
						{
							I3PRINTLOG(I3LOG_FATAL, "Must have FXDummy. please check LOD\r\nLOD %d classType %d number %d", i, classType, number);
						}
					}
				}
			}
#endif

			m_idxFXBullet = getSceneObject()->GetBody()->FindBoneByName( "EmptyCartridgeFXDummy");
				
			//IdleEffect에서 사용할 Bone의 인덱스 번호를 구한다
			m_pGunDummy = i3Scene::FindNodeByName(getSceneObject()->GetBody()->getFrameLOD()->getShapeNode(), "GunDummy");
			if( m_pGunDummy != nullptr)
			{
				if (i3::same_of<i3BoneRef*>(m_pGunDummy))
				{
					m_idxFXIdleBone = ((i3BoneRef*)m_pGunDummy)->getBoneIndex();
				}
			}
			

			//	Left-Hand Gun Dummy
			m_pGunDummyLeftHand = i3Scene::FindNodeByName( getSceneObject()->GetBody()->getFrameLOD()->getShapeNode(), "LHandGunDummy");

			if( m_pGunDummyLeftHand != nullptr)
			{
				if( i3::same_of<i3BoneRef*>(m_pGunDummyLeftHand))
				{
					INT32 nBoneIdx = ((i3BoneRef*)m_pGunDummyLeftHand)->getBoneIndex();
					i3Bone * pBone = getSceneObject()->GetBody()->getFrameLOD()->getSkeleton()->getBone( nBoneIdx);

					i3Matrix::Copy( &m_mtxGunDummyLeftHand, pBone->getMatrix());

					i3Matrix::PostRotateXDeg( &m_mtxGunDummyLeftHand, 90.f );
					i3Matrix::PostRotateZDeg( &m_mtxGunDummyLeftHand, 90.f );	

					i3Matrix::Inverse( &m_mtxGunDummyLeftHand, 0, &m_mtxGunDummyLeftHand );

					m_idxFXIdleBone = nBoneIdx;
				}
			}
			else
			{
				//LHandGunDummy라는 이름대신 WholeAniDummy라는 이름으로 들어간 무기도 있음
				m_pGunDummyLeftHand = i3Scene::FindNodeByName(getSceneObject()->GetBody()->getFrameLOD()->getShapeNode(), "WholeAniDummy");
				if (m_pGunDummyLeftHand != nullptr)
				{
					if (i3::same_of<i3BoneRef*>(m_pGunDummyLeftHand))
					{
						m_idxFXIdleBone = ((i3BoneRef*)m_pGunDummyLeftHand)->getBoneIndex();
					}
				}
			}

			// 발사 이펙트
			if( m_pWeaponInfo->hasFireEffect())
			{
				i3::rc_wstring wstrFireEffect;
				i3::string strFireEffect;

				for(INT32 i=0; i<MAX_FIRE_EFFECT; i++)
				{
					m_pWeaponInfo->GetFireEffect( wstrFireEffect, i);
					if( i3::generic_string_size( wstrFireEffect) > 0)
					{
						i3::utf16_to_mb(wstrFireEffect, strFireEffect);
						m_idxFireEffect[i] = g_pEffectManager->FindEffect(strFireEffect.c_str());
					}
				}

				//	Steady Aim일 경우 '_1st' 이펙트일 경우 앞부분에 작게 보이기 때문에 예외처리. komet
				if( GetCurrentExtensionType() != WEAPON::EXT_STEADY_AIM)
				{
					char conv[MAX_PATH];
					m_pWeaponInfo->GetFireEffect(wstrFireEffect);
					i3::utf16_to_mb(wstrFireEffect, strFireEffect);

					i3::snprintf( conv, MAX_PATH, "%s%s", strFireEffect, "_1st");

					m_idxFireEffect_1PV_Zoom = g_pEffectManager->FindEffect( conv);
				}

				m_idxFireEffect_1PV_Zoom = ( m_idxFireEffect_1PV_Zoom == -1) ? m_idxFireEffect[0] : m_idxFireEffect_1PV_Zoom;
			}
			else
			{
				m_idxFireEffect[0] = g_pEffectManager->FindEffect( "Fir_Ar_M4A1");
				m_idxFireEffect_1PV_Zoom = m_idxFireEffect[0];
			}

			// 탄피 이펙트
			{
				char * pszEffName = nullptr;

				switch( m_pWeaponInfo->GetTypeClass())
				{
					case WEAPON_CLASS_CIC			:
					case WEAPON_CLASS_DUALHANDGUN	:
					case WEAPON_CLASS_HANDGUN		:	pszEffName = "Bul_Hg_MK23";		break;
					case WEAPON_SUBCLASS_ASSAULT	:
					case WEAPON_CLASS_ASSAULT		:	pszEffName = "Bul_Ar_M4A1";		break;	
					case WEAPON_SUBCLASS_SMG		:
					case WEAPON_CLASS_SMG			:			
					case WEAPON_CLASS_DUALSMG		:	pszEffName = "Bul_Smg_MP5K";	break;
					case WEAPON_SUBCLASS_SNIPER		:
					case WEAPON_CLASS_SNIPER		:	pszEffName = "Bul_Sr_HKPSG1";	break;
					case WEAPON_SUBCLASS_SHOTGUN	:
					case WEAPON_CLASS_SHOTGUN		:		
					case WEAPON_CLASS_DUALSHOTGUN	:	pszEffName = "Bul_Sg_870MCS";	break;
					case WEAPON_CLASS_MG			:	pszEffName = "Bul_Ar_M4A1";		break;
				}

				if( pszEffName != nullptr)
				{
					m_idxBulletEffect = g_pEffectManager->FindEffect( pszEffName);

					if( m_idxBulletEffect == -1)
					{
						#if defined( I3_DEBUG)//디버그 덤프
							g_pEffectManager->Dump();
						#endif

						m_idxBulletEffect = g_pEffectManager->FindEffect( pszEffName);
					}

					I3ASSERT( m_idxBulletEffect != -1);
				}
				else
				{
					m_idxBulletEffect = -1;
				}
			}


			if( m_pWeaponInfo->GetEnableExtFireEffectOffSet() )
			{
				i3BoneRef * pFXBoneRef = i3BoneRef::new_object_ref();
				pFXBoneRef->setBoneIndex( m_idxFXBone );	
				m_pFXLastTrans = i3Transform::new_object();
				pFXBoneRef->AddChild( m_pFXLastTrans);
				
				MATRIX mtx;		
				
				m_pWeaponInfo->GetPosExtFireEffectOffSet( i3Matrix::GetPos(&mtx) );
				m_pFXLastTrans->SetMatrix(&mtx);
				
				getSceneObject()->GetBody()->AddChild( pFXBoneRef );
			}

			//히트 이펙트
			i3::rc_wstring	wsObjHitEffect;
			m_pWeaponInfo->GetObjHitEffect(wsObjHitEffect);
			i3::string strEffect;	
			i3::utf16_to_mb(wsObjHitEffect, strEffect);
			ObjHitEffIdx = g_pEffectManager->FindEffect(strEffect.c_str());
			if(ObjHitEffIdx == -1)
				ObjHitEffIdx = -2;


			i3::rc_wstring	wsCharaHitEffect;
			m_pWeaponInfo->GetCharaHitEffect(wsCharaHitEffect);
			i3::utf16_to_mb(wsCharaHitEffect, strEffect);
			CharaHitEffIdx = g_pEffectManager->FindEffect(strEffect.c_str());
			if(CharaHitEffIdx == -1)
				CharaHitEffIdx = -2;

			i3::rc_wstring	wsCharaHitEffect1PV;
			m_pWeaponInfo->GetCharaHitEffect1PV(wsCharaHitEffect1PV);
			i3::utf16_to_mb(wsCharaHitEffect1PV, strEffect);
			CharaHitEff1PVIdx = g_pEffectManager->FindEffect(strEffect.c_str());
			if(CharaHitEff1PVIdx == -1)
				CharaHitEff1PVIdx = -2;

			i3::rc_wstring	wsDecalEffectExt;
			m_pWeaponInfo->GetDecalEffectExt(wsDecalEffectExt);
			i3::utf16_to_mb(wsDecalEffectExt, strEffect);
			DecalEffExtIdx = g_pEffectManager->FindEffect(strEffect.c_str());
			if(DecalEffExtIdx == -1)
				DecalEffExtIdx = -2;

			i3::rc_wstring	wsIdleEffect;
			m_pWeaponInfo->GetIdleEffect(wsIdleEffect);
			i3::utf16_to_mb(wsIdleEffect, strEffect);
			IdleEffIdx = g_pEffectManager->FindEffect(strEffect.c_str());
			if (IdleEffIdx == -1)
				IdleEffIdx = -2;


			if(m_pWeaponInfo->GetUnBlowEffFlag() != 0)
				UnBlowHitEffFlag = true;

			if(m_pWeaponInfo->GetUnDecalEffFlag() != 0)
				UnDecalHitEffFlag = true;



			// LaserSight
			{
				m_idxFXLaserSight = getSceneObject()->GetBody()->FindBoneByName( "FXLaserSightDummy");
			}

			// Damage
			{
				if(m_pDamage == nullptr)
					m_pDamage = new weapon::Damage( this);
			}
		}

	}
	
	if ( m_pOwner == nullptr)
	{
		m_idxColorForFireEffect = 0;
	}
	else
	{
		INT32 SlotIdx = pOwner->getCharaInfo()->GetNetIdx();

		if (m_pWeaponInfo && m_pWeaponInfo->GetUnColorEffectFlag())
		{
			m_idxColorForFireEffect = 0;
		}
		else if(SlotIdx == BattleSlotContext::i()->getMySlotIdx())
		{
			USER_INFO_INVITEM_DATA Data;
			Data.Reset();
			UserInfoContext::i()->GetMyUserInfoInvItemData(&Data);
			m_idxColorForFireEffect = Data._ui16MuzzleFlashColor;
		}
		else
		{
			m_idxColorForFireEffect = BattleSlotContext::i()->getFireMuzzleColorForSlot(SlotIdx);
		}
	}

	{
		I3ASSERT( m_pLight == nullptr);
		m_pLight = i3Light::new_object();

		m_pLight->SetLightType( I3G_LIGHT_POINT);

		m_pLight->addState( I3G_LIGHT_STATE_DISABLE);
	}

#if defined( TEST_COLLISION_LINE )
	COLORREAL Col;
	i3Color::Set( &Col, 0.8f, 0.8f, 0.0f, 0.8f);
	i3Node * pNode = (i3Node*) i3SceneUtil::BuildLine( m_pVA, 1, &Col);
	g_pFramework->Get3DRoot()->AddChild( pNode);
#endif

	// 0.1ms
	{
		// Init
		_InitParameters();

		_InitCollideeGroup();
		
		// 기본적으로 오브젝트를 어딘가에 붙여줍니다.
		setDynamicActiveState( true, false);
		SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC, true);
	}
	
	if( CHud::i() != nullptr )
	{
		INT32 temp_index = m_pWeaponInfo->GetWeaponSpecialHitHudIndex();
		if( temp_index > -1 )
		{
			CHud::i()->InsertSpecialDamageFilter( temp_index );
		}

		temp_index = m_pWeaponInfo->GetWeaponSpecialDieHudIndex();
		if (temp_index > -1)
		{
			CHud::i()->InsertSpecialDieFilter(temp_index);
		}

		temp_index = m_pWeaponInfo->GetScopeType();
		if (temp_index > -1)
		{
			CHud::i()->InsertScopeHudTexture(temp_index);
		}
	}

	//
	m_mainWeapon->OnCreate(bCreateInWeapon);
	PostCreate();

	this->EnableUpdateAABB(true);

	this->EnableRenderAABB(true);

}

void WeaponBase::PostCreate( void)
{
}

MainWeapon*	WeaponBase::CreateMainWeapon() 
{
	MainWeapon * p = new MainWeapon(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon));
	return p;
}

void WeaponBase::PreCreateMainWeaponBeforeCreateCall()
{
	if (m_mainWeapon) return;	// 기생성된 경우 무시하면 됨..
	m_mainWeapon.reset( this->CreateMainWeapon() );
}

void WeaponBase::_InitCollideeGroup( void)
{
//	InitColliderSet( CGRP_ITEM, CGRP_MASK_ITEM );
	WF::SetCollideeGroup_NormalWeapon(this);
}


void WeaponBase::OnSetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular)
{
	if( m_pMaterialAttr != nullptr )
	{
		if( pfDiffuse != nullptr)	
		{
			REAL32 fDiffuseAlpha = (REAL32) i3Color::GetA( m_pMaterialAttr->GetDiffuse());
			m_pMaterialAttr->SetDiffuse( pfDiffuse->r, pfDiffuse->g, pfDiffuse->b, fDiffuseAlpha);
		}

		if( pfSpecular != nullptr)	
		{
			REAL32 fSepcularAlpha = (REAL32) i3Color::GetA( m_pMaterialAttr->GetSpecular());
			m_pMaterialAttr->SetSpecular( pfSpecular->r, pfSpecular->g, pfSpecular->b, fSepcularAlpha);
		}
	}

	m_mainWeapon->OnSetMaterialColor(pfDiffuse, pfSpecular);
}

void WeaponBase::CheckAlphaUsed( void)
{
	I3ASSERT( m_pBlendEnable != nullptr );

	bool bCheck = false;
	bool bOldBlend = m_pBlendEnable->Get();

	bCheck = getWeaponInfo()->IsAlphablendUsed();

	if( bOldBlend )
	{
		if( m_tDropInfo._bDropping )
		{
			m_pBlendEnable->Set( false);
		}
		else if( !bCheck )
		{
			m_pBlendEnable->Set( false);
		}
	}
	else
	{
		if( m_tDropInfo._bDropping )
		{
			// 네트워크 또는 월드에 있는 무기
		}
		else if( bCheck )
		{	// 자신이 들고 있는 무기중에서 블랜드를 끄면 안되는 무기들
			m_pBlendEnable->Set( true);
		}
	}
}

void WeaponBase::OnSetBlendAlpha( REAL32 Val)
{
	REAL32 val = i3Math::Clamp( Val, 0.0f, 1.0f);

	if( (m_fAlphaVal != val) && (m_pMaterialAttr != nullptr) )
	{
		i3Color::SetA( m_pMaterialAttr->GetDiffuse(), val);
		i3Color::SetA( m_pMaterialAttr->GetSpecular(), val);
		
		if( val == 1.0f)	CheckAlphaUsed();
		else
		{
			if( m_pBlendEnable->Get() == false)
				m_pBlendEnable->Set( true);
		}

		m_fAlphaVal = val;
	}

	m_mainWeapon->OnSetBlendAlpha(Val);
}

void WeaponBase::_LoadAllSound(void)
{
	//배틀이 아닌 경우에 효과음이 비활성화면 로딩안해도 될 듯...
	//배틀일 경우엔 시작시 로딩하기 때문에 효과음 설정 유무와 상관없이 로딩..
	if( g_pFramework->IsBattleStage() == false && g_pEnvSet->m_bEnableEffect == false ) return;

	_LoadFireSound();
	_LoadSwapSound();
	_LoadIdleSound();
}

bool WeaponBase::GenerateWeaponSndName(char* buf, INT32 bufSiz, CWeaponInfo* winfo, 
									   WEAPON::FIRE_SOUND_TYPE type)
{	
//	char szGetFireSound[256];
//	char szGetFireSoundExt[256];

	i3::rc_wstring wstrGetFireSound;
	i3::rc_wstring wstrGetFireSoundExt;

	winfo->GetFireSound(wstrGetFireSound);
	winfo->GetFireSoundExt(wstrGetFireSoundExt);

	INT32 length = i3::generic_string_size(wstrGetFireSound);
	if( length <= 0)
		return false;

	i3::string strGetFireSound;			i3::utf16_to_mb(wstrGetFireSound, strGetFireSound);
	i3::string strGetFireSoundExt;		i3::utf16_to_mb(wstrGetFireSoundExt, strGetFireSoundExt);

	switch((WEAPON::FIRE_SOUND_TYPE)type)
	{
	case  WEAPON::FIRE_SOUND_NORMAL:		//	일반 사격 사운드
		{				
			i3::snprintf(buf, bufSiz, "%s.wav", strGetFireSound);
		}
		break;

	case WEAPON::FIRE_SOUND_NORMAL_2:
		{
			i3::snprintf(buf, bufSiz, "%s_Secondary.wav", strGetFireSound);
		}
		break;

	case WEAPON::FIRE_SOUND_MUFFLER:
		{
			i3::snprintf(buf, bufSiz, "%s_Muffler.wav", strGetFireSound);
		}
		break;

	case WEAPON::FIRE_SOUND_SCOPE:		//	저격 사운드
		{
			//if( getWeaponInfo()->m_TypeClass == WEAPON_CLASS_SNIPER)
			{
				i3::safe_string_copy( buf, strGetFireSound, bufSiz );

				INT32	nLength = i3::generic_string_size( buf);
				INT32	nPos	= nLength - 4;
				char * file = &buf[nPos];
				i3::snprintf( file, bufSiz - (nPos + 1), "%s", "Zoom_Fire.wav");
			}
		}
		break;

	case WEAPON::FIRE_SOUND_EXT :
		{
			// 먼저 FireSoundExt 에 설정되어 있다면 우선
			if( i3::generic_string_size( strGetFireSoundExt) > 0)
				i3::snprintf(buf, bufSiz, "%s.wav", strGetFireSoundExt);
			// m_pExtWeaponInfo에 설정된 FireSound는 차선
			else
			{

				// 익스텐션 기능 무기의 익스텐션 정보
				winfo->GetFireSound( wstrGetFireSoundExt);
				i3::utf16_to_mb(wstrGetFireSoundExt, strGetFireSoundExt);

				i3::snprintf(buf, bufSiz, "%s.wav", strGetFireSoundExt);

			}
		}
		break;

	case WEAPON::FIRE_SOUND_NORMAL_3PV:
		{
			i3::snprintf(buf, bufSiz, "%s_3pv.wav", strGetFireSound);
		}
		break;

	default:
		return false;
	}

	return true;
}

void WeaponBase::_LoadFireSound()
{
	I3ASSERT_RETURN( m_pWeaponInfo != nullptr);
	
//	char szGetFireSound[256];
//	char szGetFireSoundExt[256];
	i3::rc_wstring wstrGetFireSound, wstrGetFireSoundExt;

	m_pWeaponInfo->GetFireSound(wstrGetFireSound);
	m_pWeaponInfo->GetFireSoundExt(wstrGetFireSoundExt);

	INT32 length = i3::generic_string_size( wstrGetFireSound);
	if( length <= 0)	return;

	i3::string strGetFireSound, strGetFireSoundExt;
	i3::utf16_to_mb(wstrGetFireSound, strGetFireSound);
	i3::utf16_to_mb(wstrGetFireSoundExt, strGetFireSoundExt);

	for( INT32 i = 0; i < WEAPON::FIRE_SOUND_COUNT; i++)
	{
		char conv[MAX_PATH];
	
		switch( (WEAPON::FIRE_SOUND_TYPE) i)
		{
		case  WEAPON::FIRE_SOUND_NORMAL:		//	일반 사격 사운드
			{				
				i3::snprintf( conv, MAX_PATH, "%s.wav", strGetFireSound);
			}
			break;
		case WEAPON::FIRE_SOUND_NORMAL_2:
			{
				i3::snprintf( conv, MAX_PATH, "%s_Secondary.wav", strGetFireSound);
			}
			break;
		case WEAPON::FIRE_SOUND_MUFFLER:
			{
				i3::snprintf( conv, MAX_PATH, "%s_Muffler.wav", strGetFireSound);
			}
			break;
		case WEAPON::FIRE_SOUND_SCOPE:		//	저격 사운드
			{
				//if( getWeaponInfo()->m_TypeClass == WEAPON_CLASS_SNIPER)
				{
					i3::safe_string_copy( conv, strGetFireSound, MAX_PATH );

					INT32	nLength = i3::generic_string_size( conv);
					INT32	nPos	= nLength - 4;
					char * file = &conv[nPos];
					i3::snprintf( file, MAX_PATH - (nPos + 1), "%s", "Zoom_Fire.wav");
				}
			}
			break;
		case WEAPON::FIRE_SOUND_EXT :
			{
				// 먼저 FireSoundExt 에 설정되어 있다면 우선
				if( i3::generic_string_size( strGetFireSoundExt) > 0)
					i3::snprintf( conv, MAX_PATH, "%s.wav",strGetFireSoundExt);
				// m_pExtWeaponInfo에 설정된 FireSound는 차선
				else
				{
					if( m_pExtWeaponInfo)
					{
						// 익스텐션 기능 무기의 익스텐션 정보
						m_pExtWeaponInfo->GetFireSound( wstrGetFireSoundExt);
						i3::utf16_to_mb(wstrGetFireSoundExt, strGetFireSoundExt);

						i3::snprintf( conv, MAX_PATH, "%s.wav", strGetFireSoundExt);
					}
				}
			}
			break;

		case WEAPON::FIRE_SOUND_NORMAL_3PV:
			{
				i3::snprintf( conv, MAX_PATH, "%s_3pv.wav", strGetFireSound);
			}
			break;
		}		

		i3GameResSound * pSoundRes = (i3GameResSound *) g_pFramework->QuaryResource( conv);
		
		if( pSoundRes != nullptr)
		{
			I3_SAFE_RELEASE( m_pFireSoundPlayInfo[ i]);

			i3SoundPlayInfo * pSrcInfo = pSoundRes->getSound();
			
			m_pFireSoundPlayInfo[ i] = i3SoundPlayInfo::new_object();

			pSrcInfo->CopyTo( m_pFireSoundPlayInfo[ i], I3_COPY_INSTANCE);
			m_pFireSoundPlayInfo[ i]->setMuteAtMaxDistance( true);

			if((m_pWeaponInfo->GetTypeClass() == WEAPON_CLASS_OBJECT_GUN) && (WEAPON::FIRE_SOUND_NORMAL == (WEAPON::FIRE_SOUND_TYPE)i))
			{
				m_pFireSoundPlayInfo[ i]->setLoopMode( I3SND_RT_LOOPMODE_LOOPING);
			}
		}
	}
}

void WeaponBase::_LoadSwapSound( void)
{
	
	I3ASSERT( m_pWeaponInfo != nullptr);
	i3::rc_wstring wstrSwapSound;
	m_pWeaponInfo->GetSwapSound(wstrSwapSound);
	INT32 length = i3::generic_string_size(wstrSwapSound);
	char conv[MAX_PATH] = "";
	if( length > 0)
	{
		i3::string strSwapSound;
		i3::utf16_to_mb(wstrSwapSound, strSwapSound);
		i3::snprintf( conv, MAX_PATH, "%s.wav", strSwapSound);

		i3GameResSound * pSoundRes = (i3GameResSound *) g_pFramework->QuaryResource( conv);

		if( pSoundRes != nullptr)
		{
			I3_SAFE_RELEASE( m_pSwapSoundPlayInfo);
			i3SoundPlayInfo * pSrcInfo = pSoundRes->getSound();
			m_pSwapSoundPlayInfo = i3SoundPlayInfo::new_object();
			pSrcInfo->CopyTo( m_pSwapSoundPlayInfo, I3_COPY_INSTANCE);
			m_pSwapSoundPlayInfo->setMuteAtMaxDistance( true);
		}
#if defined( I3_DEBUG)
		else
		{
			char szWeaponName[MAX_STRING_COUNT];
			char tmp[256] = "";
			
			INT32 len = i3::generic_string_size( m_pWeaponInfo->GetName());

			if( len > 0 )
			{
				I3ASSERT( len < MAX_STRING_COUNT);
				GlobalFunc::DecryptValue( szWeaponName, m_pWeaponInfo->GetName(), len, ENC_SHIFT_WEAPONINFO_RESNAME);
				sprintf_s( tmp, "Could not load weapon (%s) swap sound file (%s)", szWeaponName, conv);
				I3PRINTLOG(I3LOG_WARN,  tmp);
			}
		}
#endif
	}
}

void WeaponBase::_LoadIdleSound()
{
	I3ASSERT(m_pWeaponInfo != nullptr);
	i3::rc_wstring wstrIdleSound;
	m_pWeaponInfo->GetIdleSound(wstrIdleSound);
	INT32 length = i3::generic_string_size(wstrIdleSound);
	char conv[MAX_PATH] = "";
	if (length > 0)
	{
		i3::string strIdleSound;
		i3::utf16_to_mb(wstrIdleSound, strIdleSound);
		i3::snprintf(conv, MAX_PATH, "%s.wav", strIdleSound);

		i3GameResSound * pSoundRes = (i3GameResSound *)g_pFramework->QuaryResource(conv);

		if (pSoundRes != nullptr)
		{
			I3_SAFE_RELEASE(m_pIdleSoundPlayInfo);
			i3SoundPlayInfo * pSrcInfo = pSoundRes->getSound();
			m_pIdleSoundPlayInfo = i3SoundPlayInfo::new_object();
			pSrcInfo->CopyTo(m_pIdleSoundPlayInfo, I3_COPY_INSTANCE);
			m_pIdleSoundPlayInfo->setMuteAtMaxDistance(true);
			m_pIdleSoundPlayInfo->setLoopMode(I3SND_RT_LOOPMODE_LOOPING);
		}
#if defined( I3_DEBUG)
		else
		{
			char szWeaponName[MAX_STRING_COUNT];
			char tmp[256] = "";

			INT32 len = i3::generic_string_size(m_pWeaponInfo->GetName());

			if (len > 0)
			{
				I3ASSERT(len < MAX_STRING_COUNT);
				GlobalFunc::DecryptValue(szWeaponName, m_pWeaponInfo->GetName(), len, ENC_SHIFT_WEAPONINFO_RESNAME);
				sprintf_s(tmp, "Could not load weapon (%s) Idle sound file (%s)", szWeaponName, conv);
				I3PRINTLOG(I3LOG_WARN, tmp);
			}
		}
#endif
	}
}

void WeaponBase::_PlayFireSound( WEAPON::FIRE_SOUND_TYPE type)
{
	I3ASSERT( g_pViewer != nullptr);

	// 사운드 플레이할 필요 없다.
	if( isWeaponFlag( WEAPON_FLAG_ENABLE_FIRE_SOUND) == false) return;

	i3SoundContext * pSndCtx = g_pViewer->GetSoundContext();
	if( pSndCtx == nullptr)	return;

	bool bSameInfo = true;


	if( m_pOwner->is3PV() )
	{
		if( m_pWeaponInfo->GetTypeClass() == WEAPON_CLASS_SNIPER &&
			m_pWeaponInfo->GetNumber() == WEAPON::getItemIndex( WEAPON::SNIPER_SVU)  )
			type = WEAPON::FIRE_SOUND_NORMAL_3PV;
	}

	i3SoundPlayInfo * pPlayInfo = m_pFireSoundPlayInfo[ type];
	if( pPlayInfo == nullptr)
	{
		I3TRACE( "[WeaponBase::_PlayFireSound] WEAPON::FIRE_SOUND_TYPE (%d) sound not have. now play default fire sound.\n", type);

		pPlayInfo = m_pFireSoundPlayInfo[ WEAPON::FIRE_SOUND_NORMAL];
	}

	if( pPlayInfo == nullptr)			
	{
		char szWeaponName[ MAX_STRING_COUNT];
		char temp[1024] = "";
		const char * pName = m_pWeaponInfo->GetName();

		INT32 len = i3::generic_string_size( pName);

		if( len > 0)
		{
			I3ASSERT( len < MAX_STRING_COUNT);
			GlobalFunc::DecryptValue( szWeaponName, pName, len, ENC_SHIFT_WEAPONINFO_RESNAME);

			sprintf_s(temp, "[_PlayFireSound] %s (TYPE %d) 사운드를 플레이할 수 없습니다. 지정된 사운드 파일이 없거나 읽지 못했습니다. 확인해 주세요.\n", szWeaponName, type); 

			I3TRACE( temp);
		}

		return;
	}

	if( m_pFireSoundPlayState != nullptr)
	{
		if( m_pFireSoundPlayState->getPlayInfo() != pPlayInfo)
			bSameInfo = false;
	}

	CGameCharaBase * pOwner = m_pOwner;
	
	VEC3D * pPos = nullptr;
	if( getFXMatrix() != nullptr)
	{
		pPos = i3Matrix::GetPos( getFXMatrix());
	}
	else if( pOwner != nullptr )
	{
		i3Node * pNode = pOwner->getWeaponContext()->getAttachRoot();
		if( i3::same_of<i3Transform*>(pNode))
		{
			i3Transform * pTrans = ( i3Transform *) pNode;

			I3ASSERT( i3::same_of<i3Transform*>(pTrans));

			pPos = i3Matrix::GetPos( pTrans->GetCacheMatrix());
		}
		else if( i3::same_of<i3BoneRef*>(pNode))
		{
			pPos = i3Matrix::GetPos( pOwner->getBoneContext()->getBoneMatrix( ((i3BoneRef*)pNode)->getBoneIndex() ) );
		}
		else
		{
			I3PRINTLOG(I3LOG_FATAL,  "Weapon attach node is invalid.");
			return;
		}
	}

	if( bSameInfo && m_pFireSoundPlayState != nullptr)
	{
		if( m_pWeaponInfo->GetTypeClass()  != WEAPON_CLASS_OBJECT_GUN)
		{
			m_pFireSoundPlayState = pSndCtx->RePlay( m_pFireSoundPlayState, pPlayInfo, pPos);
		}
		else
		{
			MATRIX mtx;
			i3Matrix::SetTranslate( &mtx, pPos);

			m_pFireSoundPlayState->UpdatePos( &mtx);
		}
	}
	else
	{
		if( pOwner != nullptr && pOwner->is1PV())
		{
			pPlayInfo->set3DMode( I3SND_RT_3DMODE_DISABLE);
		}
		else
		{
			pPlayInfo->set3DMode( I3SND_RT_3DMODE_WORLD_SPACE);
		}

		m_pFireSoundPlayState = pSndCtx->Play( pPlayInfo, pPos);		
	}
}

void WeaponBase::_PlaySwapSound( void)
{
	I3ASSERT( g_pViewer != nullptr);

	i3SoundPlayInfo * pPlayInfo = m_pSwapSoundPlayInfo;
	if( pPlayInfo == nullptr)			
	{
		/*char temp[1024] = "";
		i3::rc_wstring wstrWeaponName;
		i3::rc_wstring wstrSwapSound;

		getWeaponInfo()->GetSwapSound(wstrSwapSound);
		getWeaponInfo()->GetWeaponName(wstrWeaponName);

		i3::snprintf(temp, 256, "[_PlaySwapSound] (%s) %s 사운드를 플레이할 수 없습니다. 지정된 사운드 파일이 없거나 읽지 못했습니다. 확인해 주세요.\n", 
						wstrWeaponName, wstrSwapSound); 

		I3PRINTLOG(I3LOG_WARN,  temp);*/	// 실제 파일이 없는 무기가 있다.
		return;
	}

	i3SoundContext * pSndCtx = g_pViewer->GetSoundContext();
	if( pSndCtx == nullptr)	return;

	bool bSameInfo = true;

	if( m_pSwapSoundPlayState != nullptr)
	{
		if( m_pSwapSoundPlayState->getPlayInfo() != pPlayInfo)
			bSameInfo = false;
	}

	VEC3D * pPos = nullptr;
	if( getFXMatrix() != nullptr)
	{
		pPos = i3Matrix::GetPos( getFXMatrix());
	}
	else if( m_pOwner != nullptr )
	{
		i3Node * pNode = m_pOwner->getWeaponContext()->getAttachRoot();
		if( i3::same_of<i3Transform*>(pNode))
		{
			i3Transform * pTrans = ( i3Transform *) pNode;

			I3ASSERT( i3::same_of<i3Transform*>(pTrans));

			pPos = i3Matrix::GetPos( pTrans->GetCacheMatrix());
		}
		else if( i3::same_of<i3BoneRef*>(pNode))
		{
			pPos = i3Matrix::GetPos( m_pOwner->getBoneContext()->getBoneMatrix( ((i3BoneRef*)pNode)->getBoneIndex() ) );
		}
		else
		{
			I3PRINTLOG(I3LOG_FATAL,  "Weapon attach node is invalid.");
			return;
		}
	}
	else
	{
		I3PRINTLOG(I3LOG_FATAL, "m_pOwner == nullptr && Weapon attach node is invalid.");
		return;
	}

	if( bSameInfo && m_pSwapSoundPlayState != nullptr)
	{
		if( m_pWeaponInfo->GetTypeClass()  != WEAPON_CLASS_OBJECT_GUN)
		{
			m_pSwapSoundPlayState = pSndCtx->RePlay( m_pSwapSoundPlayState, pPlayInfo, pPos);
		}
		else
		{
			MATRIX mtx;
			i3Matrix::SetTranslate( &mtx, pPos);

			m_pSwapSoundPlayState->UpdatePos( &mtx);
		}
	}
	else
	{
		if( m_pOwner != nullptr && m_pOwner->is1PV())
		{
			pPlayInfo->set3DMode( I3SND_RT_3DMODE_DISABLE);
		}
		else
		{
			pPlayInfo->set3DMode( I3SND_RT_3DMODE_WORLD_SPACE);
		}

		m_pSwapSoundPlayState = pSndCtx->Play( pPlayInfo, pPos);
	}
}

void WeaponBase::_PlayIdleSound(void)
{
	I3ASSERT(g_pViewer != nullptr);

	i3SoundPlayInfo * pPlayInfo = m_pIdleSoundPlayInfo;
	if (pPlayInfo == nullptr)
	{
		return;
	}

	i3SoundContext * pSndCtx = g_pViewer->GetSoundContext();
	if (pSndCtx == nullptr)	return;

	bool bSameInfo = true;

	if (m_pIdleSoundPlayState != nullptr)
	{
		if (m_pIdleSoundPlayState->getPlayInfo() != pPlayInfo)
			bSameInfo = false;
	}

	VEC3D * pPos = nullptr;
	if (getFXMatrix() != nullptr)
	{
		pPos = i3Matrix::GetPos(getFXMatrix());
	}
	else if (m_pOwner != nullptr)
	{
		i3Node * pNode = m_pOwner->getWeaponContext()->getAttachRoot();
		if (i3::same_of<i3Transform*>(pNode))
		{
			i3Transform * pTrans = (i3Transform *)pNode;

			I3ASSERT(i3::same_of<i3Transform*>(pTrans));

			pPos = i3Matrix::GetPos(pTrans->GetCacheMatrix());
		}
		else if (i3::same_of<i3BoneRef*>(pNode))
		{
			pPos = i3Matrix::GetPos(m_pOwner->getBoneContext()->getBoneMatrix(((i3BoneRef*)pNode)->getBoneIndex()));
		}
		else
		{
			I3PRINTLOG(I3LOG_FATAL, "Weapon attach node is invalid.");
			return;
		}
	}

	if (bSameInfo && m_pIdleSoundPlayState != nullptr)
	{
		if (m_pWeaponInfo->GetTypeClass() != WEAPON_CLASS_OBJECT_GUN)
		{
			m_pIdleSoundPlayState = pSndCtx->RePlay(m_pIdleSoundPlayState, pPlayInfo, pPos);
		}
		else
		{
			MATRIX mtx;
			i3Matrix::SetTranslate(&mtx, pPos);

			m_pIdleSoundPlayState->UpdatePos(&mtx);
		}
	}
	else
	{
		if (m_pOwner != nullptr && m_pOwner->is1PV())
		{
			pPlayInfo->set3DMode(I3SND_RT_3DMODE_DISABLE);
		}
		else
		{
			pPlayInfo->set3DMode(I3SND_RT_3DMODE_WORLD_SPACE);
		}

		m_pIdleSoundPlayState = pSndCtx->Play(pPlayInfo, pPos);
	}
}

void WeaponBase::_UpdateSound(REAL32 rDeltaSeconds)
{
	if( m_pFireSoundPlayState != nullptr)
	{
		//if( m_pFireSoundPlayState->isFree() || (m_pFireSoundPlayState->getRequest() & I3SND_RT_REQ_STOP))
		if( m_pFireSoundPlayState->isFree())
		{
			m_pFireSoundPlayState = nullptr;
		}
		else
		{
			/*
			if( !m_pOwner->is1PV())
			{
				if( getFXMatrix() != nullptr)
					m_pFireSoundPlayState->Req_UpdatePos( i3Matrix::GetPos( getFXMatrix()), rDeltaSeconds);
				else
					m_pFireSoundPlayState->Req_UpdatePos( i3Matrix::GetPos( GetMatrix()), rDeltaSeconds);
			}
			*/
		}
	}

	if( m_pSwapSoundPlayState != nullptr)
	{
		if( m_pSwapSoundPlayState->isFree())
		{
			m_pSwapSoundPlayState = nullptr;
		}
	}

	if( m_pIdleSoundPlayState != nullptr)
	{
		if(m_pIdleSoundPlayState->isFree())
		{
			m_pIdleSoundPlayState = nullptr;
		}
	}
}

void WeaponBase::SetSwapSoundPlayState(const UINT32 nState)
{
	if (m_pSwapSoundPlayState)
		m_pSwapSoundPlayState->SetPlayState(nState);
}

void WeaponBase::_StopFireSound()
{
	if( m_pFireSoundPlayState != nullptr)
	{
		m_pFireSoundPlayState->Stop();
		m_pFireSoundPlayState = nullptr;
	}
}

void WeaponBase::_StopSwapSound()
{
	if( m_pSwapSoundPlayState != nullptr)
	{
		m_pSwapSoundPlayState->Stop();
		m_pSwapSoundPlayState = nullptr;
	}
}

void WeaponBase::_StopIdleSound(void)
{
	if (m_pIdleSoundPlayState != nullptr)
	{
		m_pIdleSoundPlayState->Stop();
		m_pIdleSoundPlayState = nullptr;
	}
}

void WeaponBase::_InitParameters(void)
{
	CWeaponInfo * pInfo = getWeaponInfo();
	I3ASSERT( pInfo != nullptr);

	setLoadedBulletDualCount( i3Math::softAdd( GetLoadBulletWithCashItem(), 0));
	setLoadedBulletCount( i3Math::softAdd( GetLoadBulletWithCashItem(), 0));
	setTotalBulletCount( i3Math::softAdd( GetMaxBulletWithCashItem(), 0));
}

void WeaponBase::_TransformWorldSpace( REAL32 tm)
{
	i3SceneTracer * pTracer = g_pFramework->GetViewer()->GetSceneTracer();
	I3ASSERT( pTracer != nullptr);

	pTracer->GetModelViewMatrixStack()->Reset();
	pTracer->SetTime( tm);
	pTracer->SetViewMatrix( g_pCamera->getCamMatrix(), g_pCamera->getCamMatrix());

	getSceneObject()->OnUpdate( pTracer);
}


void WeaponBase::FireReady( void)
{	//	발사 준비 상태
	if( m_pOwner != nullptr && (m_pOwner->isLocal() || m_pOwner->isAI()) )
		addWeaponStateMask( WSM_FIRING | WSM_FIRE_DISABLE);

	m_mainWeapon->FireReady();
}

bool WeaponBase::Fire( WEAPON::FIRE_ORDER type)
{
	if( getLoadedBulletCount() <= 0)
		return false;

	m_FireOrder = type;

	// 시점이 향하고 있는 목표 지점을 구한다.
	GetShotTargetPos( m_TargetPos);

	NET_Fire( nullptr, m_TargetPos, m_FireOrder );

	return true;
}

void WeaponBase::FireEnd(void)
{
	removeWeaponStateMask( WSM_FIRING | WSM_WAIT_AUTOFIREDELAY | WSM_FIRE_DISABLE);
	
	if(getWeaponInfo()->GetPumpAction() && !isWeaponStateMask( WSM_LOADED_BULLET))
	{
		addWeaponStateMask( WSM_FIRE_DISABLE);
	}

	setAutoFireCount(0);
	setRemainAutoFireCount(0);
}

void WeaponBase::FireCollision( VEC3D * pStartPos, VEC3D * pTargetPos)
{	
	VEC3D target, start, dir, vXAxis, vYAxis;
	REAL32 length;
	INT32 i, collGroup;

	if( m_pWeaponInfo->GetTypeUsage() == WEAPON_SLOT_THROWING1 )
		return;

	if( m_pOwner == nullptr)
		return;

	i3Vector::Copy( &target, pTargetPos);

	if( pStartPos == nullptr )
	{
		// 총알의 발사 시작점을 (1인칭의 경우에는) Camera의 Position으로,
		// (3인칭의 경우에는) 총구 지점으로 한다.
		if( m_pOwner->is1PV() || m_pOwner->isAI())
		{
			// 현재는 Camera의 지점
			m_pOwner->getCameraContext()->getVirtualCamPosition( &start );
		}
		else
		{
			i3Vector::Copy( &start, i3Matrix::GetPos( getFXMatrix()));
		}
	}
	else
	{
		i3Vector::Copy( &start, pStartPos );
	}

	collGroup = CGRP_MASKGROUP_ATTACK;
	if( m_pOwner->getCharaTeam() & CHARACTER_TEAM_RED)
		collGroup = CGRP_MASKGROUP_CHARA_BLUE;
	else
		collGroup = CGRP_MASKGROUP_CHARA_RED;

	// 네트워크로 보내줍니다.
	NET_FireSync( &target );

	i3Vector::Sub( &dir, &target, &start);
	length = i3Vector::Normalize( &dir, &dir);

	i3Vector::Cross( &vXAxis, &dir, &I3_YAXIS);
	i3Vector::Normalize( &vXAxis, &vXAxis);

	i3Vector::Cross( &vYAxis, &dir, &vXAxis);
	i3Vector::Normalize( &vYAxis, &vYAxis);

	REAL32 deviation;

	// 스나이퍼 종류의 총이며, Zoom을 한 상태에서 편차가 0이라면 편차를 적용하지 않음.
	// PSG1과 같이 연사가 가능한 스나이퍼 총에 대해 페널티를 부여하기 위함.
	/*
	if( ( m_pWeaponInfo->m_TypeClass == WEAPON_CLASS_SNIPER) &&
		( m_pOwner->isWeaponStateMask( CHARA_STATE_ZOOM)) &&
		( m_pOwner->getFireDeviation() == m_pOwner->getLastDeviation()))
	{
		deviation = 0.0f;
	}
	else
	{
		// 일반적인 편차치(Deviation) 적용.
		deviation = m_pOwner->getDeviationAngle();
	}
	*/

	if((m_pWeaponInfo->isEnableFastZoom()) && (isWeaponStateMask( WSM_FIRST_ZOOM)))
		deviation = 0.0f;
	else if (m_pOwner->IsSting())
	{
		deviation = m_pWeaponInfo->GetDeviation();
		//deviation = 15.0f;
	}
	else if (m_pOwner->IsCCSting() || m_pOwner->IsStingMars())
	{
		deviation = m_pWeaponInfo->GetDeviation();
	}
	else
		deviation = m_pOwner->getBoneContext()->getFireDeviation();

	if( m_pOwner->isAI() && m_pOwner->IsDino() == false)
	{
		REAL32 deviationRate = ((CInputCtrlAI_ForHuman*)m_pOwner->getInputContext()->getControl())->GetTargetRate();

		deviation = (1.0f + deviation) * deviationRate * 0.5f;

		deviation = MIN( deviation, 45.0f);
	}

	//I3TRACE("%f deviation %f in FireCollision \n", g_pFramework->GetAccTime(), deviation);

	m_bTestRoughCollision = false;
	for( i = 0; i < m_pWeaponInfo->GetFireBulletCount(); i++)
	{
		REAL32 r, t;
		VEC3D tTarget, tdir;
		
		REAL32 orgDeviation = deviation;

		r = i3Math::Randf2();

		if (!m_pOwner->IsDomiDino())
		{
			
			//	UseITEM 정확도 감소 (deviation 증가)
			REAL32 ratio = BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_ACCURACY);
			CASHITEM_TRACE("CASHITEMSKILL_ACCURACY = %f\n", BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_ACCURACY));
			deviation += (orgDeviation * ratio);

			////------------------	SKill 명중률 Apply
			const NET_CHARA_INFO * pCharaInfo = BattleSlotContext::i()->getNetCharaInfo( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());

			if( pCharaInfo != nullptr)
			{
				REAL32 ratio_inSkill = pCharaInfo->getWeaponSkillValue( getWeaponInfo()->GetTypeClass(), SKILLLIST_ACCURACY);
				ratio_inSkill += (ratio_inSkill > 0 ? I3_EPS : -I3_EPS);
				deviation += ratio_inSkill;
			}
		}
		
		r *= deviation * 0.5f * 0.5f;
		t = i3Math::Randf2() * deviation * 0.5f * 0.5f;

		MATRIX tmp;
		i3Matrix::SetRotateAxisDeg( &tmp, &vXAxis, r);
		i3Matrix::PostRotateAxisDeg( &tmp, &vYAxis, t);

		i3Vector::TransformNormal( &tdir, &dir, &tmp);

		i3Vector::Scale( &tdir, &tdir, length);
		i3Vector::Add( &tTarget, &start, &tdir);

		DoLineCollision( &start, &tTarget, collGroup);
#if defined( TEST_COLLISION_LINE )
		{
			m_pVA->Lock();
			m_pVA->SetPosition(0, &start);
			m_pVA->SetPosition(1, &tTarget);
			m_pVA->Unlock();
		}
#endif
	}

	INT32 autoFireCount = getAutoFireCount();

	autoFireCount++;

	setAutoFireCount( autoFireCount);
}

I3_PHYSIX_HIT_RESULT * WeaponBase::_CollisionTestByHitBox( i3CollideeLine * pline, REAL32 rMinLength, VEC3D* pVec3)
{
	INT32 i;
	MATRIX mat;
	VEC3D * pShapeCenterPos;
	VEC3D vCenter;
	VEC3D vDir, vCamPos, vShapeDir;
	VEC3D vMovements;
	MATRIX camMtx;
	VEC4D vecRow1, vecRow2, vecRow3;

	I3_PHYSIX_HIT_RESULT * pResult = nullptr;
	if( m_pOwner == nullptr)
		return nullptr;
	
	m_pOwner->getCameraContext()->getVirtualCamPosition( &vCamPos, &camMtx );

	

	i3Vector::Normalize( &vDir, pline->GetDir());
	if(pVec3 == nullptr)
	{
		REAL32 rRange = GetRange();
		i3Vector::Scale( &vMovements, &vDir, rRange * 0.5f);
		m_HitBox.extents.x = rRange * 0.5f;
		m_HitBox.extents.y = 0.01f;
		m_HitBox.extents.z = rRange * 0.5f;
	}
	else 
	{
		i3Vector::Scale( &vMovements, &vDir, pVec3->z);
		m_HitBox.extents.x = pVec3->x;
		m_HitBox.extents.y = pVec3->y;
		m_HitBox.extents.z = pVec3->z;
	}
	
	i3Vector::Add( &vCenter, pline->GetStart(), &vMovements);
	
	i3Matrix::GetRow( &camMtx, 0, &vecRow1);
	i3Matrix::GetRow( &camMtx, 1, &vecRow2);
	i3Matrix::GetRow( &camMtx, 2, &vecRow3);

	NxVec3 row1( i3Vector::GetX( &vecRow1), i3Vector::GetY( &vecRow1), i3Vector::GetZ( &vecRow1));
	NxVec3 row2( i3Vector::GetX( &vecRow2), i3Vector::GetY( &vecRow2), i3Vector::GetZ( &vecRow2));
	NxVec3 row3( i3Vector::GetX( &vecRow3), i3Vector::GetY( &vecRow3), i3Vector::GetZ( &vecRow3));
	
	m_HitBox.center.x = getX( &vCenter);
	m_HitBox.center.y = getY( &vCenter);
	m_HitBox.center.z = getZ( &vCenter);

	m_HitBox.rot.setRow( 0, row1);
	m_HitBox.rot.setRow( 1, row2);
	m_HitBox.rot.setRow( 2, row3);

	INT32 collisionGroup = CGRP_TEAM_RED;
	if( m_pOwner->getCharaTeam() & CHARACTER_TEAM_RED )
		collisionGroup = CGRP_TEAM_BLUE;
	
	NxScene * pNxScene = g_pPhysixCtx->getScene();

	REAL32 rMaxDot = 0.9f;

	NxShape * pShapeList[ 32];

	INT32 nOverlapCount = pNxScene->overlapOBBShapes( m_HitBox, NX_DYNAMIC_SHAPES, 32, pShapeList, nullptr, (1<<collisionGroup) );

	for( i = 0; i < nOverlapCount; ++i)
	{
		if( pShapeList[ i]->userData == nullptr)	continue;

		if( !i3::same_of<i3PhysixShapeSet*>( (i3EventReceiver*)pShapeList[ i]->userData) )
			continue;

		i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*)pShapeList[ i]->userData;
		if(!i3::kind_of<CGameCharaBase*>(pShapeSet->getEventReceiver()))
		{
			continue;	
		}

		if( pShapeSet->getShapeGroup() != collisionGroup)
			continue;

		pShapeSet->getShapeGlobalPose( &mat);

		pShapeCenterPos = i3Matrix::GetPos( &mat);
		i3Vector::Sub( &vShapeDir, pShapeCenterPos, &vCamPos);
		REAL32 rLen = i3Vector::Normalize( &vShapeDir, &vShapeDir);

		if( rLen < rMinLength )
		{
			REAL32 rDot = i3Vector::Dot( &vDir, &vShapeDir);
			
			if( rMaxDot < rDot)
			{
				if(IsThereWallBetween(vCamPos, *pShapeCenterPos, rMinLength) == false)
				{
					pResult = &m_LastHitResult;
					i3Vector::Copy( &pResult->m_Intersect, pShapeCenterPos);
					i3Vector::Scale( &pResult->m_Normal, i3Matrix::GetAt( m_pOwner->getBoneContext()->getHeadMatrix()), -1.f );
					pResult->m_pDestShapeSet = pShapeSet;
					pResult->m_pHitShape = pShapeSet->getNxShape( 0);
					pResult->m_T = rLen;

					rMaxDot = rDot;
				}
			}
		}
	}

	if( pResult != nullptr)
		_OnHit( pResult, pline, false);
	

	return pResult;
}

bool WeaponBase::IsThereWallBetween(VEC3D& v3StartPos, VEC3D& v3EndPos, REAL32 rLength)
{
	i3CollideeLine ColLine;

	ColLine.SetStart(&v3StartPos);
	ColLine.SetEnd(&v3EndPos);

	//World에 대해서 한번 테스트
	// 월드 컬리젼 거리보다 작으면.
	return _CheckHitWorld( ColLine, rLength );
}

I3_PHYSIX_HIT_RESULT * WeaponBase::_DoFireCollisionKnife( i3CollideeLine * pline, INT32 idxGroup, bool *bOwnerWarn)
{
	I3_PHYSIX_HIT_RESULT *	pResult, * pLastResult = nullptr;
	UINT32			nResultCount, nLastResultCount, nRoughResultCount;
	REAL32			rMinLength = GetRange();

	if( m_pOwner == nullptr )
		return nullptr;

	m_rPenetrateLength = rMinLength;

	nResultCount = nLastResultCount = nRoughResultCount = 0;

	m_pHitWorld = m_pHitObject = nullptr;
	// 버그 수정 버퍼 카운트를 초기화 해야합니다.
	g_pFramework->reserveRaycastResult( 0);

	//World & object
	pResult = g_pFramework->raycastClosest( pline, CGRP_MASKGROUP_ATTACK);
	if( pResult != nullptr )
	{
		nResultCount = 1;
		g_pFramework->reserveRaycastResult( nResultCount );

		if( pResult->m_T < rMinLength )
		{
			switch( pResult->m_pDestShapeSet->getShapeGroup() )
			{
			case CGRP_WORLD_ATTACK :
				{
					m_pHitWorld = pResult;
					rMinLength = pResult->m_T;
				}
				break;
			case CGRP_OBJECT_ALL :
			case CGRP_OBJECT_HIT :
			case CGRP_OBJECT_WHOLE :
			case CGRP_OBJECT_WHOLE_DYNAMIC :
				{
					m_pHitObject = pResult;
					rMinLength = pResult->m_T;
				}
				break;
			case CGRP_ITEM_HIT :
				{
					if( _OnHit( pResult, pline, false ) )
					{
						pLastResult = pResult;
						return pLastResult;
					}
				}
				break;
			}
		}
	}

	INT32 RoughCollGroup = CGRP_MASKGROUP_CHARA_ROUGH_BLUE;
	if( m_pOwner->getCharaTeam() & CHARACTER_TEAM_BLUE)
	{
		RoughCollGroup = CGRP_MASKGROUP_CHARA_ROUGH_RED;
	}

	// 캐릭터 컬리젼
	SetHitGroup( false );
	pResult = g_pFramework->raycastClosest( pline, idxGroup );

	i3EventReceiver * pLastEvent = nullptr;

	if( pResult )
	{
		nLastResultCount = 1;
		g_pFramework->reserveRaycastResult( nResultCount + nLastResultCount + nRoughResultCount);
		
		if( pResult->m_T < rMinLength )
		{	
			pLastEvent = pResult->m_pDestShapeSet->getEventReceiver();

			const char * pszName = pResult->m_pDestShapeSet->GetName();

			if( CheckHitPart( pszName ) )
			{
				rMinLength = pResult->m_T;
				if( _OnHit( pResult, pline, false ) )
				{
					pLastResult = pResult;
				}
			}
		}
	}
	// 위에서 충돌 판정이 나지 않았으면 박스로 처리 한다.
	if(pLastEvent == nullptr)
	{
		VEC3D v3Temp;
		REAL32 rTemp = 0.f;

		i3Vector::Set(&v3Temp, 0.f, 0.f, 0.f);
		if(i3::same_of<CMeleeWeaponInfo*>(m_pWeaponInfo))
		{
			((CMeleeWeaponInfo*)m_pWeaponInfo)->GetCollisionBoxExtent(&v3Temp);
		}
		rTemp = v3Temp.x + v3Temp.y + v3Temp.z;

		// Extent가 있는 거 먼저 처리를 한다.
		//WEAPON::RES_ID num = m_pWeaponInfo->GetRealNumber();
		if(rTemp > 0)
		{
			rMinLength = v3Temp.z * 2; // v3Temp.z 은 Extent이고 박스의 한변의 길이는 Extent * 2 이다.
			pLastResult = _CollisionTestByHitBox( pline, rMinLength, &v3Temp);
		}
		else
		{
			bool bProcess = false;
			if( (m_FireOrder == WEAPON::PRIMARY_FIRE) ||
				(i3::same_of<WeaponDualKnife*>(this) && getAutoFireCount() < 2) )
				bProcess = true;

			if( m_FireOrder == WEAPON::SECONDARY_FIRE)
			{
				if( i3::same_of<WeaponKnife*>(this) &&
					((CMeleeWeaponInfo*)m_pWeaponInfo)->IsSpraySecAttack())
					bProcess = true;

				if( i3::same_of<WeaponGunWithKnife*>(this))
					bProcess = true;
			}

			if( bProcess)
				pLastResult = _CollisionTestByHitBox( pline, rMinLength);
		}
		
	}
	
	if( pLastResult == nullptr )
	{
		// world hit effect
		if( m_pHitWorld != nullptr )
		{
			if( _OnHit( m_pHitWorld, pline, false ) )
			{
				pLastResult = m_pHitWorld;
			}
		}

		if( m_pHitObject != nullptr )
		{
			_OnHit( m_pHitObject, pline, false );
			if( m_pHitWorld == nullptr )
				pLastResult = m_pHitObject;
		}

		if( m_pHitItem != nullptr )
		{
			_OnHit( m_pHitItem, pline, false );
			if( m_pHitItem == nullptr )
				pLastResult = m_pHitItem;
		}

	}

	g_pFramework->resetRaycastResult();

	return pLastResult;
}

void WeaponBase::_CalcPhysixForce( i3PhysixShapeSet * pShapeSet, VEC3D * pHitDir, VEC3D * pHitPos, REAL32 rForce, REAL32 rLength)
{
	I3ASSERT( pShapeSet != nullptr);
	I3ASSERT( pHitDir != nullptr);
	I3ASSERT( g_pPhysixInfoDataBase != nullptr);

	MATRIX mat;
	VEC3D vForce, vTorque, vR;

	if( pShapeSet->getShapeGroup() == CGRP_OBJECT_HIT)
	{
		// 히트컬리젼의 액터는 시뮬레이션을 하지 않습니다.
		// 상위 노드에서 시뮬레이션을 하는 i3Transform2를 찾아서 세팅해줘야 합니다.

		I3ASSERT( i3::kind_of<i3Object*>(pShapeSet->getEventReceiver()));
			
		i3Object * pObj = (i3Object*) pShapeSet->getEventReceiver();
		i3Transform2 * pTrans = (i3Transform2*) i3Scene::FindNodeByExactType( pObj->getSceneObject(), i3Transform2::static_meta() );
		if( pTrans != nullptr)
			pShapeSet = pTrans->getShapeSet();
	}

	// 물리량을 계산합니다.
	PHYSIXINFODATA_WEAPON * pPhysixData = &g_pPhysixInfoDataBase->getInfo()->m_WeaponObject;

	switch( m_pWeaponInfo->GetTypeClass() )
	{
	case WEAPON_CLASS_KNIFE				:		
	case WEAPON_CLASS_KNUCKLE			:	rForce *= pPhysixData->m_rWeapon_Knife; 	break;
	case WEAPON_CLASS_HANDGUN			:	rForce *= pPhysixData->m_rWeapon_Pistol;	break;
	case WEAPON_SUBCLASS_ASSAULT		:
	case WEAPON_CLASS_ASSAULT			:	rForce *= pPhysixData->m_rWeapon_Assult;	break;
	case WEAPON_SUBCLASS_BOW			:	
	case WEAPON_CLASS_BOW				:	rForce *= pPhysixData->m_rWeapon_Assult;	break;
	case WEAPON_SUBCLASS_SMG			:
	case WEAPON_CLASS_SMG				:	rForce *= pPhysixData->m_rWeapon_SMG;		break;
	case WEAPON_SUBCLASS_SNIPER			:
	case WEAPON_CLASS_SNIPER			:	rForce *= pPhysixData->m_rWeapon_Rifle;		break;
	case WEAPON_SUBCLASS_SHOTGUN		:
	case WEAPON_CLASS_SHOTGUN			:		
	case WEAPON_CLASS_DUALSHOTGUN		:	rForce *= pPhysixData->m_rWeapon_Shotgun;	break;
	case WEAPON_CLASS_THROWING_GRENADE	:
	case WEAPON_CLASS_MISSION			:	rForce *= pPhysixData->m_rWeapon_Grenade;	break;
	case WEAPON_CLASS_MG				:	rForce *= pPhysixData->m_rWeapon_Assult;	break;
	case WEAPON_CLASS_DUALKNIFE			:	rForce *= pPhysixData->m_rWeapon_Knife;		break;
	case WEAPON_CLASS_DUALHANDGUN		:
	case WEAPON_CLASS_CIC				:	rForce *= pPhysixData->m_rWeapon_Pistol;	break;
	case WEAPON_CLASS_GRENADESHELL		:	rForce *= pPhysixData->m_rWeapon_Grenade;	break;
	case WEAPON_SUBCLASS_G_LAUNCHER		:
	case WEAPON_CLASS_ROCKET_LAUNCHER	:	rForce *= pPhysixData->m_rWeapon_Grenade;	break;
	case WEAPON_CLASS_OBJECT_GUN		:	rForce *= pPhysixData->m_rWeapon_Assult;	break;
	default :		
		// 순구씨~!!!!!!!!!!!!
		I3TRACE( "Invalid Weapon class type: %d", m_pWeaponInfo->GetTypeClass());
		rForce *= pPhysixData->m_rWeapon_Grenade;
		break;
	}

	I3ASSERT( m_pWeaponInfo != nullptr);

	if( m_pWeaponInfo->isThrowingClass() )
	{
		REAL32 rExplosionRange = getWeaponInfo()->GetExplosion_Range();
		I3ASSERT( rExplosionRange > I3_EPS);
		rLength = (rExplosionRange - rLength + 0.5f) / rExplosionRange;

		rForce *= MINMAX( 0.f, rLength, 1.f);
	}
	
	// 이동힘을 구합니다.
	i3Vector::Scale( &vForce, pHitDir, rForce * g_pPhysixInfoDataBase->getInfo()->m_rObjectLinSlop );
	pShapeSet->addForce( &vForce);

	// 회전량을 구합니다.
	pShapeSet->getShapeGlobalPose( &mat );
	i3Vector::Sub( &vR, pHitPos, i3Matrix::GetPos( &mat ) );
	i3Vector::Cross( &vTorque, &vR, &vForce );
	i3Vector::Scale( &vTorque, &vTorque, g_pPhysixInfoDataBase->getInfo()->m_rObjectAngSlop );
	pShapeSet->addTorque( &vTorque);
}

void WeaponBase::_CalcPhysixForce( I3_PHYSIX_HIT_RESULT * pResult, VEC3D * pHitDir, REAL32 rDamage)
{
	_CalcPhysixForce( pResult->m_pDestShapeSet, pHitDir, &pResult->m_Intersect, rDamage, 0.f);
}
void WeaponBase::PlayAnimation_AttackReady(void)
{
	if (m_pOwner->is1PV())
	{
		if( getAIIndex( WEAPON::AI_ATTACKREADY_1PV) != -1 )
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_ATTACKREADY_1PV) );
	}
	else
	{
		if( getAIIndex( WEAPON::AI_ATTACKREADY_3PV) != -1 )
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_ATTACKREADY_3PV) );
	}
}

void WeaponBase::PlayAnimation_JumpEnd( void)
{
	if( getAIIndex( WEAPON::AI_JUMPEND) != -1 )
		WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_JUMPEND) );
}

void WeaponBase::PlayAnimation_Damage( void)
{
	if( getAIIndex( WEAPON::AI_DAMAGE) != -1)
		WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_DAMAGE));
}

void WeaponBase::LoadMagazine( void)
{
	if( m_pOwner != nullptr && (m_pOwner->isLocal() || m_pOwner->isAI()))
	{
		if( getWeaponInfo()->GetTypeTrigger() != WEAPON::TRIGGER_RELOADING_SHOT)
			addWeaponStateMask( WSM_FIRE_DISABLE);
	}

	OnPlayLoadMagAnimation();

	m_mainWeapon->LoadMagazine();
}

void WeaponBase::LoadBullet(void)
{
	if( m_pOwner != nullptr && (m_pOwner->isLocal() || m_pOwner->isAI()))
	{
		if( getWeaponInfo()->GetTypeTrigger() != WEAPON::TRIGGER_RELOADING_SHOT)
			addWeaponStateMask( WSM_FIRE_DISABLE);
	}

	OnPlayLoadBulletAnimation();

	setWeaponState( WEAPON::STATE_LOADBULLET);
}

void WeaponBase::WeaponChange( bool bPlaySound /*= true*/)
{
	I3ASSERT_RETURN(m_pOwner != nullptr);

	CHARA_VIEW_TYPE type = CHARA_VIEW_1PV;


	if( m_pOwner->isLocal() || m_pOwner->isAI())
		addWeaponStateMask( WSM_FIRE_DISABLE);

	type = m_pOwner->getViewType();
	
	getSceneObject()->GetBody()->StopAnim();

	//이거 추가 안하면 스왑 속도 증가 파츠 착용 시 무기는 체인지 애니메이션 상체 애니메이션 보다 느려집니다.
	REAL32 rTimeScale = _CalcSwapTimeScaleToVersion();

	OnPlayChangeAni( rTimeScale);

	if( bPlaySound )
		OnPlayChangeSound();
}

void		WeaponBase::OnPlayChangeAni( REAL32 rTimeScale )	// 비가상함수로 만들고 구현함수를 가상으로 처리..
{
	m_mainWeapon->OnPlayChangeAni(rTimeScale);
}

void		WeaponBase::OnPlayChangeAniImpl_Default( REAL32 rTimeScale )	// 이것도 일단 비가상으로 따로 둔다..
{
	if( isWeaponStateMask( WSM_ACTIVATE_EXTENSION) )
	{
		if( getAIIndex( WEAPON::AI_IDLE_EXTENSION) != -1 )
		{
			WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_IDLE_EXTENSION), rTimeScale );
			return;
		}
	}

	if( m_pOwner != nullptr && g_pFramework->IsBattleStage())
	{
		if( m_pOwner->is1PV() && (getAIIndex( WEAPON::AI_CHANGE1PV) != -1) )		
		{
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_CHANGE1PV), rTimeScale );
			m_pOwner->Cmd_Only_ChangeWeaponAction();	// revision 35847  인게임에 처음 입장할 때 총기를 꺼내는 애니메이션이 어색한 버그수정
			return;
		}
		else if( m_pOwner->is3PV() && (getAIIndex( WEAPON::AI_CHANGE3PV) != -1) )	
		{	
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_CHANGE3PV), rTimeScale );
			return;
		}
		else if( getAIIndex( WEAPON::AI_CHANGE) != -1)
		{
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_CHANGE), rTimeScale);
			return;
		}else if( getAIIndex(WEAPON::AI_IDLE_FOLDED_CHANGE) != -1)
		{
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_IDLE_FOLDED_CHANGE), rTimeScale);
			return;
		}
	}	

	_OnPlayWeaponIdleAnim();
}

void		WeaponBase::OnPlayChangeAniImpl( REAL32 rTimeScale )
{
	WeaponBase::OnPlayChangeAniImpl_Default(rTimeScale);
}

/* virtual  void WeaponBase::OnPlayChangeAnimation( REAL32 rTimeScale  = 1.f )
{
	if( isWeaponStateMask( WSM_ACTIVATE_EXTENSION) )
	{
		if( getAIIndex( WEAPON::AI_IDLE_EXTENSION) != -1 )
		{
			_PlayWeaponAnim( getAIIndex( WEAPON::AI_IDLE_EXTENSION), rTimeScale );
			return;
		}
	}

	if( m_pOwner != nullptr && g_pFramework->IsBattleStage())
	{
		if( m_pOwner->is1PV() && (getAIIndex( WEAPON::AI_CHANGE1PV) != -1) )		
		{
			_PlayWeaponAnim( getAIIndex( WEAPON::AI_CHANGE1PV), rTimeScale );
			return;
		}
		else if( m_pOwner->is3PV() && (getAIIndex( WEAPON::AI_CHANGE3PV) != -1) )	
		{	
			_PlayWeaponAnim( getAIIndex( WEAPON::AI_CHANGE3PV), rTimeScale );
			return;
		}
		else if( getAIIndex( WEAPON::AI_CHANGE) != -1)
		{
			_PlayWeaponAnim( getAIIndex( WEAPON::AI_CHANGE), rTimeScale);
			return;
		}
	}	

	_OnPlayWeaponIdleAnim();
}
*/

/*virtual*/ void WeaponBase::OnPlayChangeSound()
{
	// 무기 교체 사운드
	_PlaySwapSound();
}

void WeaponBase::SetGachaBrightness(void)
{
	if( m_pMaterialAttr != nullptr )
	{
		COLORREAL col;

		REAL32 rBrightness = m_pWeaponInfo->GetWeaponBrightness();
		i3Color::Set( &col, rBrightness, rBrightness, rBrightness, 1.0f);
		m_pMaterialAttr->SetDiffuse( &col);
		m_pMaterialAttr->SetSpecular( &col);
		m_pMaterialAttr->SetShininess( 64.8f);		

		OnSetBlendAlpha( 1.0f);
	}
}

bool WeaponBase::ExtensionActivate( bool bVal, bool bAnimPlay)
{
	if( OnCheckExtensionPossible( bVal) )
	{
		setExtensionActivate( bVal);	// revision 42335 위치 변경 SS1-R5 익스텐션 변환시 총기 애니메이션 안바뀌는 버그 수정 (redmine.5406)
		OnPlayExtensionAnimation( bVal, bAnimPlay);
		OnProcessExtension( bVal);

		//	무기 Extension 활성화 / 비활성화 중에는 무기는 발사 못한다.	
		if( (m_pOwner != nullptr) && (m_pOwner->isLocal() || m_pOwner->isAI()))
		{
			addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
		}
		return true;
	}

	return false;
}

static INT32 s_WeaponCount = 0;

static void s_Weapon_SetShapePosition( WeaponBase * pWeapon )
{
	VEC3D vec;
	i3Vector::Set( &vec, (REAL32)s_WeaponCount - 1000.f , 12000.f, (REAL32)s_WeaponCount - 1000.f );
	pWeapon->SetPos( &vec );
	s_WeaponCount++;
}

void WeaponBase::ReturnInstance()
{

	m_mainWeapon->ReturnInstance();

	s_Weapon_SetShapePosition( this );

	_StopFireSound();
	_StopSwapSound();
	_StopIdleSound();

	for( INT32 i = 0; i < WEAPON::FIRE_SOUND_COUNT; i++)
	{
		//g_pSndMng->ReleaseSoundFromPlayInfo( m_pFireSoundPlayInfo[ i]);
		I3_SAFE_RELEASE( m_pFireSoundPlayInfo[ i]);
	}

	//g_pSndMng->ReleaseSoundFromPlayInfo( m_pSwapSoundPlayInfo);
	I3_SAFE_RELEASE( m_pSwapSoundPlayInfo);
	I3_SAFE_RELEASE( m_pIdleSoundPlayInfo);

	getAttachNode()->RemoveAllChild();

	RemoveAllChild();	

	DetachNode();		//	GameNode , SceneNode 제거

	_OnInitVairable();

	I3_SAFE_RELEASE( m_pFXLastTrans );

	I3_SAFE_RELEASE( m_pSgInfo);
	m_mainWeapon.reset();
	
	//2014-07-24 싱말 폭파미션후 프리징 버그 해결. v2에도 적용
	//ENG_김대영
//	if( GetRefCount() > 1)
//		i3Chara::ReturnInstance();

	i3Chara::ReturnInstance();
}

void WeaponBase::ReturnToReady( void)
{
	if( m_pWeaponInfo == nullptr )
		return;

	//	 강제로 Ready 상태로 돌아가기 위해 필요합니다. (예: 장전 중에 무기를 교체하면...)
	getSceneObject()->RemoveFlag( I3_NODEFLAG_DISABLE );

	OnSetVisibleExtension( true );

	_OnPlayWeaponIdleAnim();

	// 강제 처리입니다..
	// 소음기, 듀얼탄창은 무기 스왑에서 익스텐션 상태를 그대로 유지합니다.
	bool bNoExtensionOff = false;

	switch( m_pWeaponInfo->GetTypeClass() )
	{
	case WEAPON_CLASS_SMG:
		{
			// KrissSuperV 스킵
			if( i3::same_of<WeaponKrissSuperV*>(this))
				bNoExtensionOff = true;
		}		
		break;
	case WEAPON_CLASS_SHOTGUN :
		{
			if( i3::same_of<WeaponShotGunGenderWithDual*>(this) )
				bNoExtensionOff = true;
		}
		break;
	case WEAPON_CLASS_DUALSMG :
	case WEAPON_CLASS_DUALSHOTGUN : 
		{
			bNoExtensionOff = true;			// DualSMG, DualShotGun은 스킵
		}
		break;
	case WEAPON_CLASS_KNIFE :
		{
			bNoExtensionOff = m_pWeaponInfo->isNoOffExtAtChange();
		}
		break;
	case WEAPON_CLASS_KNUCKLE :
		break;
	}
	

	if( (GetCurrentExtensionType() != WEAPON::EXT_SILENCER) &&
		(GetCurrentExtensionType() != WEAPON::EXT_DUAL_MAG) &&
		!i3::same_of<WeaponGunWithKnife*>(this) &&
		!bNoExtensionOff )
	{
		ExtensionActivate( false, false);
		
		// 커스텀 M4 경우 익스텐션 모드 활성화시 무기 변경시 익스텐션을 끈다.
		if( isMultipleExtension())
		{				
			m_pWeaponInfo = m_pAbsoluteWeaponInfo;		//	백업된 주무기 정보로 다시 전환

			m_CurrentExtensionType = m_pWeaponInfo->getExtensionType(0); //	첫번째 익스텐션으로 초기화 설정. 중간에 바뀔수 있는 값
		}	
	}

	// state clear	
	setWeaponState( WEAPON::STATE_READY);
	removeWeaponStateMask( WSM_PLAYED_FIRE_SOUND | 0xFF);
	
	//연사 기능 관련 초기화.
	m_timeAutoFire = 0.0f;
	setAutoFireCount(0);
	setRemainAutoFireCount(0);
	removeWeaponStateMask(WSM_WAIT_AUTOFIREDELAY | WSM_FIRE_DISABLE | WSM_POINTSHOTING);

	//	기본으로 오른손에 잡도록
	if( getOwner() != nullptr)
		getOwner()->getWeaponContext()->setAttachHand( CHARA_WEAPON_GRIP_RIGHT_HAND);	// revision 45937 - 무기 교체 시 무기 오른손에 잡도록 수정
	ChangeGunDummy( WEAPON_ATTACH_GRIP);

	// xgkick_thai
	m_tDropInfo.ReturnToReady();

	if( i3::same_of<WeaponDualKnife*>(this)
		&& m_pFireSoundPlayState != nullptr)
	{
		m_pFireSoundPlayState->Stop();
		m_pFireSoundPlayState = nullptr;
	}

	_StopSwapSound();
	_StopIdleSound();
	_RemoveIdleEffect();
///
	m_mainWeapon->ReturnToReady();
///

}

////////////////////////////////////////////////////////////////////////////////////////
//

// 실제 총알에 대한 Collision Test가 아니라, 시점 상에 목표 지점을 구하기 위해
// Collision Test를 한다.
// 이때, Character는 충돌 테스트 대상에 포함되지 않으며, World만을 대상으로
// 해야한다.
void WeaponBase::GetShotTargetPos( VEC3D * pDest, VEC3D * pStart)
{
	MATRIX mtx;
	VEC3D pos;
	VEC3D at, *pFXPos;
	REAL32	fRange = 400.f;

	if( m_pOwner == nullptr )
		return;
	
	if( m_idxFXBone == -1)	//	FX가 없는 무기일 경우
		pFXPos = pStart;
	else
		pFXPos = i3Matrix::GetPos( getFXMatrix());

	if( m_pWeaponInfo->GetTypeFire() == WEAPON::FIRE_MELEE )
		fRange = 400.f;
	else
		fRange = GetRange();

	m_pOwner->getCameraContext()->getVirtualCamPosition( &pos, &mtx);
	i3Vector::Scale( &at, i3Matrix::GetAt( &mtx), - fRange);

	i3Vector::Add( pDest, &at, &pos);


	if( m_pOwner->is1PV() == false)
	{
		i3CollideeLine line;

		line.SetStart( i3Matrix::GetPos( g_pCamera->getCamMatrix()));
		line.SetEnd( pDest);

		I3_PHYSIX_HIT_RESULT * pResult = g_pFramework->raycastClosest( &line, CGRP_WORLD_ATTACK );
		if( pResult != nullptr )
		{
			i3Vector::Sub( &at, &pResult->m_Intersect, pFXPos);
			i3Vector::Normalize( &at, &at);
			i3Vector::Scale( pDest, &at, GetRange());
			i3Vector::Add( pDest, pDest, pFXPos);
		}
	}
}

WEAPON_BULLET_HIT_STATE WeaponBase::_Raycast( i3CollideeLine * pLine, REAL32 & rMinLength, UINT32 & nWorldHitCount, UINT32 & nPenetrateCount)
{
	i3CollideeLine Line;

	//World & object
	//bool bPenetrate = false;
	WEAPON_BULLET_HIT_STATE iHitState = WEAPON_BULLET_HIT_STATE_HIT;

	I3_PHYSIX_HIT_RESULT * pResult = g_pFramework->raycastClosest( pLine, CGRP_MASKGROUP_ATTACK);

	if( pResult == nullptr)			return WEAPON_BULLET_HIT_STATE_NO_HIT;
	if( rMinLength < pResult->m_T)	return WEAPON_BULLET_HIT_STATE_LESSRANGE;

	m_bHitWorld = false;

	nWorldHitCount = 1;
	g_pFramework->reserveRaycastResult( nWorldHitCount);

	// 관통 테스트
	REAL32 rLen = _CheckWorldHitLength( pResult, rMinLength );

	if( rLen == rMinLength )
	{	// 관통 되었습니다.
		//bPenetrate = true;

		VEC3D vAdd;
		i3Vector::Normalize( &vAdd, pLine->GetDir() );
		i3Vector::Scale( &vAdd, &vAdd, 0.01f );
		i3Vector::Add( &vAdd, &pResult->m_Intersect, &vAdd );
		Line.SetStart( &vAdd );
		Line.SetEnd( pLine->GetEnd() );

		iHitState = WEAPON_BULLET_HIT_STATE_PENETRATE;

		// 관통된 거리
		m_rPenetrateLength = pResult->m_T;
		switch( pResult->m_pDestShapeSet->getShapeGroup() )
		{
		case CGRP_WORLD_ATTACK :			m_pHitWorldPenetrate = pResult;		break;
		case CGRP_OBJECT_ALL :
		case CGRP_OBJECT_HIT :
		case CGRP_OBJECT_WHOLE :
		case CGRP_OBJECT_WHOLE_DYNAMIC :	m_pHitObjectPenetrate = pResult;	break;
		}

		// 관통된 벽부터 다시 체크합니다.
		pResult = g_pFramework->raycastClosest( &Line, CGRP_MASKGROUP_ATTACK);
		if( pResult )
		{
			nPenetrateCount = 1;
			g_pFramework->reserveRaycastResult( nWorldHitCount + nPenetrateCount );

			switch( pResult->m_pDestShapeSet->getShapeGroup() )
			{
			case CGRP_WORLD_ATTACK :
				if( (rMinLength - m_rPenetrateLength) * 0.5f > pResult->m_T )
				{
					rMinLength = pResult->m_T + m_rPenetrateLength;
					m_bHitWorld = true;
				}
				m_pHitWorld = pResult;
				break;
			case CGRP_OBJECT_ALL :
			case CGRP_OBJECT_HIT :
			case CGRP_OBJECT_WHOLE :
			case CGRP_OBJECT_WHOLE_DYNAMIC :
				if( (rMinLength - m_rPenetrateLength) * 0.5f > pResult->m_T )
				{
					rMinLength = pResult->m_T + m_rPenetrateLength;
					m_bHitWorld = true;
				}
				m_pHitObject = pResult;
				break;
			case CGRP_ITEM_HIT :
				if( (rMinLength - m_rPenetrateLength) * 0.5f > pResult->m_T )
				{
					rMinLength = pResult->m_T + m_rPenetrateLength;
					m_pHitItem = pResult;
				}
				break;
			}
		}
	}
	else
	{
		m_bHitWorld = true;
		rMinLength = rLen;
		switch( pResult->m_pDestShapeSet->getShapeGroup() )
		{
		case CGRP_WORLD_ATTACK :			m_pHitWorld = pResult;	break;
		case CGRP_OBJECT_ALL :
		case CGRP_OBJECT_HIT :
		case CGRP_OBJECT_WHOLE :
		case CGRP_OBJECT_WHOLE_DYNAMIC :	m_pHitObject = pResult;	break;
		case CGRP_ITEM_HIT :				m_pHitItem = pResult;	break;
		}
	}

	//return bPenetrate;
	return iHitState;
}

I3_PHYSIX_HIT_RESULT * WeaponBase::DoLineCollision2( i3CollideeLine * pline, INT32 idxGroup, bool *bOwnerWarn)
{
	VEC3D	WarningIntersectPos;
	I3_PHYSIX_HIT_RESULT *	pResult, * pLastResult = nullptr;
	UINT32			nResultCount, nPenetResultCount, nLastResultCount, nRoughResultCount, i;
	REAL32			rTestRange = GetRange();
	REAL32			rMinLength = rTestRange;
	WEAPON_BULLET_HIT_STATE	iHitState = WEAPON_BULLET_HIT_STATE_NONE;

	i3CollideeLine	Line;

	if( m_pOwner == nullptr )
		return nullptr;

	CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());

	m_rPenetrateLength = rMinLength;

	nResultCount = nPenetResultCount = nLastResultCount = nRoughResultCount = 0;

	m_pHitWorld = m_pHitWorldPenetrate = m_pHitObject = m_pHitObjectPenetrate = m_pHitItem = nullptr;

	iHitState = _Raycast( pline, rMinLength, nResultCount, nPenetResultCount);

	// Error 검출
	if( iHitState == WEAPON_BULLET_HIT_STATE_NONE)
		return nullptr;

	if( rMinLength == rTestRange)
	{	// Hit거리가 사거리라면 맞지 않은 상태입니다.
		if( (iHitState != WEAPON_BULLET_HIT_STATE_NO_HIT) && (iHitState != WEAPON_BULLET_HIT_STATE_LESSRANGE) &&
			(iHitState != WEAPON_BULLET_HIT_STATE_PENETRATE))
		{
			// 맞지 않은 경우, 모자라는 경우, 관통은 되었지만 다음 도달 거리가 아닌 경우 외에는 잘못된 상태
			return nullptr;
		}
	}

	// Warning Sound용 스피어 체크
	if( m_bTestRoughCollision == false)
	{
		INT32 RoughCollGroup = CGRP_MASKGROUP_CHARA_ROUGH_BLUE;
		if( m_pOwner->getCharaTeam() & CHARACTER_TEAM_BLUE)
			RoughCollGroup = CGRP_MASKGROUP_CHARA_ROUGH_RED;

		nRoughResultCount = g_pFramework->raycast( pline, RoughCollGroup );
		if( nRoughResultCount > 0)
		{
			//컬리전 테스트를 새로 수행하게되면 Result값이 사라지므로, 테스트를 수행할 receiver를 미리 점유해 둔다.
			I3_PHYSIX_HIT_RESULT * pRoughResult = g_pFramework->getRaycastResult();

			g_pFramework->reserveRaycastResult( nResultCount + nPenetResultCount + nRoughResultCount );
			
			for( i = 0; i < nRoughResultCount; ++i)
			{
				i3EventReceiver * pRecv = (i3EventReceiver*)pRoughResult[i].m_pDestShapeSet->getEventReceiver();

				if( (pRecv == pPlayer) && (rMinLength > pRoughResult[ i].m_T))
				{
					*bOwnerWarn = true;
					i3Vector::Copy( &WarningIntersectPos, &pRoughResult[i].m_Intersect);
					break;
				}
			}
		}
		m_bTestRoughCollision = true;
	}

	// 캐릭터 컬리젼
	i3EventReceiver * pLastEvent = nullptr;
	i3EventReceiver * pTempEvent[128] = {nullptr, };

	if( getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_SNIPER )
	{
		// 관통 무기 
		nLastResultCount = g_pFramework->raycast( pline, idxGroup );
		if( nLastResultCount > 0 )
		{
			pResult = g_pFramework->getRaycastResult();
			g_pFramework->reserveRaycastResult( nResultCount + nPenetResultCount + nLastResultCount + nRoughResultCount);

			// 데미지율에 따른 정렬 - 2013.02.22 양승천
			struct cmpByDamageRate
			{
				bool operator()( const I3_PHYSIX_HIT_RESULT& p1, const I3_PHYSIX_HIT_RESULT& p2)
				{
					CGameCharaBase * chara1 = i3::kind_cast<CGameCharaBase*>(p1.m_pDestShapeSet->getEventReceiver());
					CGameCharaBase * chara2 = i3::kind_cast<CGameCharaBase*>(p2.m_pDestShapeSet->getEventReceiver());

					if(chara1 != nullptr && chara2 != nullptr)
					{
						CHARA_HIT_PART ePart1 = chara1->getCollisionContext()->GetHitPart( p1.m_pDestShapeSet);
						CHARA_HIT_PART ePart2 = chara2->getCollisionContext()->GetHitPart( p2.m_pDestShapeSet);

						// TRex이고 눈에 맞은 경우면 return true를 해줘야 함. RU.638
						bool is_hit_trex_eye1 = check_trex_hit_eye(chara1, ePart1);
						bool is_hit_trex_eye2 = check_trex_hit_eye(chara2, ePart2);

						if( is_hit_trex_eye1 || is_hit_trex_eye2 )
							return ( is_hit_trex_eye1 && !is_hit_trex_eye2 ) ? true : false;	// return (true && false) ? true : false;

						CHARA::eDAMAGEPART damagePart1 = CHARA::GetDamagePartByHit( ePart1);
						CHARA::eDAMAGEPART damagePart2 = CHARA::GetDamagePartByHit( ePart2);

						CCharaGlobal * pGlobalInfo = g_pCharaInfoDataBase->getGlobal();
						REAL32 rPart1 = chara1->getDBInfo()->GetPartDamageRate( damagePart1) * pGlobalInfo->GetDamageRateByMode( damagePart1);
						REAL32 rPart2 = chara2->getDBInfo()->GetPartDamageRate( damagePart2) * pGlobalInfo->GetDamageRateByMode( damagePart2);

						return  rPart1 > rPart2;
					}
					return ( chara1 == nullptr && chara2 != nullptr ) ? true : false; // return (nullptr && !nullptr) ? true : false;
				}

				bool check_trex_hit_eye( CGameCharaBase * pChara , CHARA_HIT_PART hit_part )
				{
					if( i3::same_of<CGameCharaDinoTRex*>(pChara) )
					{
						if( hit_part == CHARA_HIT_VITAL_EYE )
							return true;
					}

					return false;
				}
			};
			i3::sort( pResult, pResult + nLastResultCount, cmpByDamageRate());


			for( i = 0; i < nLastResultCount; i++ )
			{
				bool isEvent = false;

				if( rMinLength < pResult[ i ].m_T )
					continue;

				for(UINT32 j = 0; j < i; j++)
				{
					if( pTempEvent[j] == pResult[ i ].m_pDestShapeSet->getEventReceiver())
					{
						//저격소총으로 TRex 사격 시, 데스 블로우가 일어나지 않는 문제.
						//TRex는 여러번 관통되어도 죽지 않으므로, 눈이 관통되었는지만 확인하면 된다.
						CGameCharaBase * tempChara = i3::kind_cast<CGameCharaBase*>(pResult[ i ].m_pDestShapeSet->getEventReceiver());
						if(!tempChara->IsRex())
						{
							isEvent = true;
						}
					}
				}

				if(isEvent == true)
					continue;

				// 맞는 유저가 다르다면 부위판별을 리셋해줍니다.
				if( pLastEvent != pResult[ i ].m_pDestShapeSet->getEventReceiver() )
				{
					SetHitGroup( false );
					pLastEvent = pResult[ i ].m_pDestShapeSet->getEventReceiver();
				}

				//GlobalFunc::PB_TRACE(" i %d GetName %s", i, pResult[ i ].m_pDestShapeSet->GetName());
				// 맞은 부위별로 체크합니다.
				// 같은 부위라면 스킵
				if( CheckHitPart( pResult[ i ].m_pDestShapeSet->GetName() ) )
				{
					if( _OnHit( &pResult[ i ], pline, false ) )
					{
						pLastResult = &pResult[ i ];
						pTempEvent[i] = pLastEvent;
					}
				}
			}
		}
	}
	else
	{	// 일반 무기
		pResult = g_pFramework->raycastClosest( pline, idxGroup );
		if( pResult != nullptr )
		{
			nLastResultCount = 1;
			g_pFramework->reserveRaycastResult( nResultCount + nPenetResultCount + nLastResultCount + nRoughResultCount);

			if( rMinLength > pResult->m_T)
			{
				_OnHit( pResult, pline, false );
				pLastResult = pResult;
				rMinLength = pResult->m_T;
			}
		}
	}
	
	if( pLastResult == nullptr )
	{
		// world hit effect
		if( m_pHitWorld != nullptr && m_bHitWorld)
		{
			if( _OnHit( m_pHitWorld, pline, false ) )
				pLastResult = m_pHitWorld;
		}

		if( m_pHitObject != nullptr && m_bHitWorld)
		{
			if( _OnHit( m_pHitObject, pline, false ))
				pLastResult = m_pHitObject;
		}

		if( m_pHitItem != nullptr)
		{
			if( _OnHit( m_pHitItem, pline, false))
				pLastResult = m_pHitItem;
		}
	}

	// 관통이 되었다면 뒷면도 체크
	if( iHitState == WEAPON_BULLET_HIT_STATE_PENETRATE )
	{
		if( m_pHitWorldPenetrate != nullptr )
		{
			_OnHit( m_pHitWorldPenetrate, pline, false );
			if( pLastResult == nullptr)
				pLastResult = m_pHitWorldPenetrate;
		}
		if( m_pHitObjectPenetrate != nullptr )
		{
			_OnHit( m_pHitObjectPenetrate, pline, false );
			if( pLastResult == nullptr)
				pLastResult = m_pHitObjectPenetrate;
		}

		i3CollideeLine BackLine;
		VEC3D	vAdd, vDir;
		i3Vector::Normalize( &vDir, pline->GetDir() );
		i3Vector::Scale( &vAdd, &vDir, -0.01f );

		if( m_pHitWorld != nullptr)		i3Vector::Add( &vAdd, &m_pHitWorld->m_Intersect, &vAdd );
		else if( m_pHitObject != nullptr)	i3Vector::Add( &vAdd, &m_pHitObject->m_Intersect, &vAdd);
		else							i3Vector::Add( &vAdd, pline->GetEnd(), &vAdd);

		BackLine.SetStart( &vAdd );
		BackLine.SetEnd( pline->GetStart() );
		

		// revision 44306
		// 2012.09.07 박기성
		// 고스트타운 컬리젼 관련 코드 주석처리
		/*UINT32 nBackCount = 0;
		pResult = g_pFramework->raycastClosest( &BackLine, CGRP_MASKGROUP_ATTACK);
		if( pResult != nullptr )
		{
			nBackCount = 1;
			g_pFramework->reserveRaycastResult( nResultCount + nPenetResultCount + nLastResultCount + nRoughResultCount + nBackCount);
			_OnHit( pResult, &BackLine, false, false );
		}*/
	}

	if(*bOwnerWarn)
	{
		if (m_pWeaponInfo->GetTypeClass() == WEAPON_CLASS_DINO)
		{
			if( m_pWeaponInfo->GetTypeFire() == WEAPON::FIRE_RANGE)
			{
				g_pSndMng->ReserveSoundPlay( IT_WEAPON_STATE, GTBWS_ON_HIT_DINO_STING, i3Math::Rand()%3, &WarningIntersectPos, m_pOwner->is1PV());
			}
		}
		else if(m_pWeaponInfo->isGun())
			g_pSndMng->ReserveSoundPlay( IT_WEAPON_STATE, GTBWS_ON_GRAZE_BULLET, i3Math::Rand()%3, &WarningIntersectPos, m_pOwner->is1PV());
	}

	g_pFramework->resetRaycastResult();

	return pLastResult;
}

I3_PHYSIX_HIT_RESULT * WeaponBase::DoLineCollision3( i3CollideeLine * pline, INT32 idxGroup, bool *bOwnerWarn)
{	
	UINT32 i;

	VEC3D	WarningIntersectPos;
	I3_PHYSIX_HIT_RESULT *	pResult, * pLastResult = nullptr;
	UINT32			nResultCount, nPenetResultCount, nLastResultCount, nRoughResultCount;
	REAL32			rMinLength = GetRange();
	bool	bPenetrate = false;

	i3CollideeLine	Line;

	if( m_pOwner == nullptr )
		return nullptr;

	m_rPenetrateLength = rMinLength;

	nResultCount = nPenetResultCount = nLastResultCount = nRoughResultCount = 0;

	m_pHitWorld = m_pHitWorldPenetrate = m_pHitObject = m_pHitObjectPenetrate = m_pHitItem = nullptr;

	//bPenetrate = _Raycast( pline, rMinLength, nResultCount, nPenetResultCount);
	UINT32	nCollisionGroup = CGRP_MASKGROUP_ALLATTACK_BLUE;

	if( m_pOwner->getCharaTeam() & CHARACTER_TEAM_BLUE)
		nCollisionGroup = CGRP_MASKGROUP_ALLATTACK_RED;

	m_bHitWorld = false;

	//World & object
	UINT32 iHitCount = g_pFramework->raycast( pline, nCollisionGroup);

	if( iHitCount == 0)
		return nullptr;

	I3_PHYSIX_HIT_RESULT * HitChara[ 128];
	i3mem::FillZero( HitChara, sizeof( HitChara));

	UINT32 iHitCharaCount = 0;

	pResult = g_pFramework->getRaycastResult();
	g_pFramework->reserveRaycastResult( iHitCount);

	REAL32 rMyRoughDist = rMinLength;
	REAL32 rMinCharaDist = rMinLength;
	UINT32 iMinCharaIdx = 0;

	for( i = 0; i < iHitCount; i++)
	{
		// 관통 테스트
		REAL32 rLen = rMinLength;
		switch( pResult[ i].m_pDestShapeSet->getShapeGroup() )
		{
		case CGRP_WORLD_ATTACK :
			if( rMinLength > pResult[ i].m_T)
			{
				m_bHitWorld	= true;
				if( m_pHitWorld == nullptr)						m_pHitWorld = &pResult[ i];
				else if( m_pHitWorld->m_T > pResult[ i].m_T)	m_pHitWorld = &pResult[ i];
				
				rLen = _CheckWorldHitLength( &pResult[ i], rMinLength );
				if( rLen == rMinLength )
				{	// 관통 되었습니다.
					if( m_rPenetrateLength > pResult[ i].m_T )
					{
						bPenetrate = true;
						VEC3D	vAdd;
						i3Vector::Normalize( &vAdd, pline->GetDir() );
						i3Vector::Scale( &vAdd, &vAdd, 0.01f );
						i3Vector::Add( &vAdd, &pResult[ i].m_Intersect, &vAdd );
						Line.SetStart( &vAdd );
						Line.SetEnd( pline->GetEnd() );

						// 관통된 거리
						m_rPenetrateLength = pResult[ i].m_T;
						m_pHitWorldPenetrate = &pResult[ i];
						m_pHitObjectPenetrate = nullptr;
					}
				}
				else
				{
					m_pHitObject = nullptr;
					m_pHitItem = nullptr;
				}
				rMinLength = rLen;
			}
			break;

		case CGRP_OBJECT_ALL :
		case CGRP_OBJECT_HIT :
		case CGRP_OBJECT_WHOLE :
		case CGRP_OBJECT_WHOLE_DYNAMIC :
			if( rMinLength > pResult[ i].m_T)
			{
				if( m_pHitObject == nullptr)						m_pHitObject = &pResult[ i];
				else if( m_pHitObject->m_T > pResult[ i].m_T)	m_pHitObject = &pResult[ i];
				
				rLen = _CheckWorldHitLength( &pResult[ i], rMinLength );
				if( rLen == rMinLength )
				{	// 관통 되었습니다.
					if( m_rPenetrateLength > pResult[ i].m_T )
					{
						bPenetrate = true;
						VEC3D	vAdd;
						i3Vector::Normalize( &vAdd, pline->GetDir() );
						i3Vector::Scale( &vAdd, &vAdd, 0.01f );
						i3Vector::Add( &vAdd, &pResult[ i].m_Intersect, &vAdd );
						Line.SetStart( &vAdd );
						Line.SetEnd( pline->GetEnd() );

						m_rPenetrateLength = pResult[ i].m_T;
						m_pHitObjectPenetrate = &pResult[ i];
						m_pHitWorldPenetrate = nullptr;
					}
				}
				else
				{
					m_pHitWorld = nullptr;
					m_pHitItem = nullptr;
				}
				rMinLength = rLen;
			}
			break;

		case CGRP_ITEM_HIT :
			if( rMinLength > pResult[ i].m_T)
			{
				if( m_pHitItem == nullptr)						m_pHitItem = &pResult[ i];
				else if( m_pHitItem->m_T > pResult[ i].m_T)	m_pHitItem = &pResult[ i];

				rMinLength = pResult[ i].m_T;

				m_pHitWorld = nullptr;
				m_pHitObject = nullptr;
			}
			break;

		case CGRP_TEAM_BLUE_ROUGH :
		case CGRP_TEAM_RED_ROUGH :
			// Warning Sound용 스피어 체크
			{
				i3EventReceiver * pRecv = (i3EventReceiver*)pResult[i].m_pDestShapeSet->getEventReceiver();

				CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());

				if( (pRecv == pPlayer))
				{
					*bOwnerWarn = true;
					rMyRoughDist = pResult[ i].m_T;
					i3Vector::Copy( &WarningIntersectPos, &pResult[i].m_Intersect);
				}
			}
			break;
		case CGRP_TEAM_BLUE :
		case CGRP_TEAM_RED :

			if (iHitCharaCount < 128)
			{
				HitChara[iHitCharaCount] = &pResult[i];

				if (rMinCharaDist > pResult[i].m_T)
				{
					rMinCharaDist = rMinLength;
					iMinCharaIdx = iHitCharaCount;
				}

				iHitCharaCount++;

				if (iHitCharaCount >= 128)
				{
					I3PRINTLOG(I3LOG_FATAL, "chara hit buffer overflow.");
					// 문제가 있지만, 이미 표시가 되며, 계속 굴리는게 목적이라면....이 루프가 끝날때는 iHitCharaCount를 고정해야한다!
				}
			}
			break;
		}
	}


	if( bPenetrate )
	{	// 관통 되었습니다.
		// 관통된 벽부터 다시 체크합니다.
		pResult = g_pFramework->raycastClosest( &Line, CGRP_MASKGROUP_ATTACK);
		if( pResult )
		{
			g_pFramework->reserveRaycastResult( iHitCount + 1 );

			switch( pResult->m_pDestShapeSet->getShapeGroup() )
			{
			case CGRP_WORLD_ATTACK :
				if( (rMinLength - m_rPenetrateLength) * 0.5f > pResult->m_T )
				{
					rMinLength = pResult->m_T + m_rPenetrateLength;
					m_bHitWorld = true;
				}
				m_pHitWorld = pResult;
				break;
			case CGRP_OBJECT_ALL :
			case CGRP_OBJECT_HIT :
			case CGRP_OBJECT_WHOLE :
			case CGRP_OBJECT_WHOLE_DYNAMIC :
				if( (rMinLength - m_rPenetrateLength) * 0.5f > pResult->m_T )
				{
					rMinLength = pResult->m_T + m_rPenetrateLength;
					m_bHitWorld = true;
				}
				m_pHitObject = pResult;
				break;
			case CGRP_ITEM_HIT :
				if( (rMinLength - m_rPenetrateLength) * 0.5f > pResult->m_T )
				{
					rMinLength = pResult->m_T + m_rPenetrateLength;
					m_pHitItem = pResult;
				}
				break;
			}
		}
	}

	// 캐릭터 컬리젼
	i3EventReceiver * pLastEvent = nullptr;

	if( getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_SNIPER )
	{
		// 관통 무기 
		for( i = 0; i < iHitCharaCount; i++)
		{
			if( rMinLength < HitChara[ i ]->m_T )
				continue;

			// 맞는 유저가 다르다면 부위판별을 리셋해줍니다.
			if( pLastEvent != HitChara[ i ]->m_pDestShapeSet->getEventReceiver() )
			{
				SetHitGroup( false );
				pLastEvent = HitChara[ i ]->m_pDestShapeSet->getEventReceiver();
			}

			// 맞은 부위별로 체크합니다.
			// 같은 부위라면 스킵
			if( CheckHitPart( HitChara[ i ]->m_pDestShapeSet->GetName() ) )
			{
				if( _OnHit( HitChara[ i ], pline, false ) )
					pLastResult = HitChara[ i ];
			}
		}
	}
	else
	{	// 일반 무기
		if( iHitCharaCount > 0 )
		{
			if( rMinLength > HitChara[ iMinCharaIdx]->m_T)
			{
				_OnHit( HitChara[ iMinCharaIdx], pline, false );
				pLastResult = HitChara[ iMinCharaIdx];
			}
		}
	}
	
	
	if( pLastResult == nullptr )
	{
		// world hit effect
		if( m_pHitWorld != nullptr)
		{
			if( _OnHit( m_pHitWorld, pline, false ) )
				pLastResult = m_pHitWorld;
		}

		if( m_pHitObject != nullptr)
		{
			if( _OnHit( m_pHitObject, pline, false ))
				pLastResult = m_pHitObject;
		}

		if( m_pHitItem != nullptr)
		{
			if( _OnHit( m_pHitItem, pline, false))
				pLastResult = m_pHitItem;
		}
	}

	// 관통이 되었다면 뒷면도 체크
	if( bPenetrate )
	{
		if( m_pHitWorldPenetrate != nullptr )		_OnHit( m_pHitWorldPenetrate, pline, false );
		if( m_pHitObjectPenetrate != nullptr )		_OnHit( m_pHitObjectPenetrate, pline, false );

		i3CollideeLine BackLine;
		VEC3D	vAdd, vDir;
		i3Vector::Normalize( &vDir, pline->GetDir() );
		i3Vector::Scale( &vAdd, &vDir, -0.01f );

		if( m_pHitWorld != nullptr)		i3Vector::Add( &vAdd, &m_pHitWorld->m_Intersect, &vAdd );
		else if( m_pHitObject != nullptr)	i3Vector::Add( &vAdd, &m_pHitObject->m_Intersect, &vAdd);

		BackLine.SetStart( &vAdd );
		BackLine.SetEnd( pline->GetStart() );
		
		UINT32 nBackCount = 0;
		pResult = g_pFramework->raycastClosest( &BackLine, CGRP_MASKGROUP_ATTACK);
		if( pResult != nullptr )
		{
			nBackCount = 1;
			g_pFramework->reserveRaycastResult( nResultCount + nPenetResultCount + nLastResultCount + nRoughResultCount + nBackCount);
			_OnHit( pResult, &BackLine, false, false );
		}
	}

	if( rMyRoughDist > rMinLength )
		*bOwnerWarn = false;

	if( *bOwnerWarn)
	{
		if( getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_DINO )
		{
			if( getWeaponInfo()->GetTypeFire() == WEAPON::FIRE_RANGE)
				g_pSndMng->ReserveSoundPlay( IT_WEAPON_STATE, GTBWS_ON_HIT_DINO_STING, i3Math::Rand()%3, &WarningIntersectPos, m_pOwner->is1PV());
		}
		else if(m_pWeaponInfo->isGun())
			g_pSndMng->ReserveSoundPlay( IT_WEAPON_STATE, GTBWS_ON_GRAZE_BULLET, i3Math::Rand()%3, &WarningIntersectPos, m_pOwner->is1PV());
	}

	g_pFramework->resetRaycastResult();

	return pLastResult;
}

bool WeaponBase::CheckHitPart( const char * pszPart)
{
	bool bRet = false;

	switch( pszPart[0])
	{
		case 'H':
			if( pszPart[1] == 'E')
			{
				if( !m_bHitGroup[HIT_CHARA_PART_VITAL])
				{
					m_bHitGroup[HIT_CHARA_PART_VITAL] = true;
					bRet = true;
				}
			}
			else if( pszPart[1] == '_')			// Head
			{
				if( pszPart[ 5 ] != 'H' )
				{
					// Hit!
					if( !m_bHitGroup[HIT_CHARA_PART_HEAD])
					{
						m_bHitGroup[HIT_CHARA_PART_HEAD] = true;
						bRet = true;
					}
				}
			}	
			else if( pszPart[1] == 'J')		// Arm
			{
				// Hit!
				if( !m_bHitGroup[HIT_CHARA_PART_HEAD])
				{
					m_bHitGroup[HIT_CHARA_PART_HEAD] = true;
					bRet = true;
				}
			}
			else if( pszPart[1] == 'N')		// Arm
			{
				// Hit!
				if( !m_bHitGroup[HIT_CHARA_PART_ARM])
				{
					m_bHitGroup[HIT_CHARA_PART_ARM] = true;
					bRet = true;
				}
			}
			else if( pszPart[1] == 'u')
			{
				// Hull_Helmet
				if( pszPart[5] == 'H' )
				{
					if( !m_bHitGroup[HIT_CHARA_PART_HELMET])
					{
						m_bHitGroup[HIT_CHARA_PART_HELMET] = true;
						//bRet = true;
						bRet = false;	// 헬멧은 히트 계산을 하지 않습니다. 머리에 직접 맞아야 합니다.
					}
				}
				else if( !m_bHitGroup[HIT_CHARA_PART_WEAPON])	// Weapon
				{					
					m_bHitGroup[HIT_CHARA_PART_WEAPON] = true;
					bRet = false;
				}
			}
			else if( pszPart[1] == 'e')		// Helmet
			{
				if( !m_bHitGroup[HIT_CHARA_PART_HELMET])
				{
					m_bHitGroup[HIT_CHARA_PART_HELMET] = true;
					bRet = false;	// 헬멧은 히트 계산을 하지 않습니다. 머리에 직접 맞아야 합니다.
				}
			}
			break;

		// Body
		case 'T':		// 공룡 꼬리는 바디로 처리
		case 'B':
			if( !m_bHitGroup[HIT_CHARA_PART_VITAL])
			{
				if( pszPart[2] == 'N')
				{
					if( pszPart[8] == 'L')
					{
						m_bHitGroup[ HIT_CHARA_PART_VITAL] = true;
						bRet = true;
					}
				}
			}

			// GIRAN NPC
			if( !m_bHitGroup[ HIT_CHARA_PART_HEAD])
			{
				if( pszPart[ 1] == 'i' && pszPart[ 6] == 'H')
				{
					m_bHitGroup[ HIT_CHARA_PART_HEAD] = true;
					bRet = true;
				}
			}
			// 랩터
			if( pszPart[8] == 'L') {	// B_Neck0_Lower
				if( !m_bHitGroup[HIT_CHARA_PART_NECK]) {
					m_bHitGroup[HIT_CHARA_PART_NECK] = true;
					bRet = true;
				}
			}

            if( !bRet && !m_bHitGroup[HIT_CHARA_PART_BODY])
			{	// body
				m_bHitGroup[HIT_CHARA_PART_BODY] = true;
				bRet = true;
			}
			
			break;

		// Arm
		case 'A':
			// Hit!
			if( !m_bHitGroup[HIT_CHARA_PART_ARM])
			{
				m_bHitGroup[HIT_CHARA_PART_ARM] = true;
				bRet = true;
			}
			break;

		// Leg
		case 'L':
		case 'F':
			// Hit!
			if( !m_bHitGroup[HIT_CHARA_PART_LEG])
			{
				m_bHitGroup[HIT_CHARA_PART_LEG] = true;
				bRet = true;
			}
			break;
		default:
			bRet = false;
			break;
	}

	return bRet;
}

void WeaponBase::SetHitGroup( bool bFlag)
{
	INT32 i = 0;

	for( i=0; i<HIT_CHARA_PART_COUNT; i++)
	{
		m_bHitGroup[i] = bFlag;
	}
}

/*
I3_TERRAIN_NA,							// Not-Assigned
I3_TERRAIN_CONCRETE = 1,				// 콘크리트
I3_TERRAIN_STEEL,						// 철제
I3_TERRAIN_GROUND,						// 흙
I3_TERRAIN_WOOD,						// 나무
I3_TERRAIN_SNOW,						// 눈
I3_TERRAIN_WATER_DEEP,					// 물 (깊은)
I3_TERRAIN_WATER_SHALLOW,				// 물 (얕은)
I3_TERRAIN_WET_CLAY,					// 진흙
I3_TERRAIN_GRASS,						// 잔디
I3_TERRAIN_MARBLE,						// 대리석
I3_TERRAIN_FALLLEAF,
I3_TERRAIN_CONCRETE_THIN,
I3_TERRAIN_STEEL_THIN,
I3_TERRAIN_WOOD_THIN,
I3_TERRAIN_MARBLE_THIN,
*/
bool WeaponBase::_CheckPenetration( I3_PHYSIX_HIT_RESULT * pResult, REAL32 rFireRange, REAL32 rWeaponRange )
{
	if( pResult->m_pTerrain == nullptr )
	{
		return false;
	}

	UINT16 nTerrainType = pResult->m_pTerrain->m_Terrain;

	if( nTerrainType < I3_TERRAIN_CONCRETE_THIN )
	{
		return false;
	}

	if( ((nTerrainType > I3_TERRAIN_MARBLE_THIN) &&	(nTerrainType < I3_TERRAIN_GROUND_THIN)) ||
		((nTerrainType > I3_TERRAIN_BARBEDWIRE) && (nTerrainType < I3_TERRAIN_CLOTH_NO_DECAL)) )
	{
		return false;
	}

	// 두께가 아래보다 작으면 무조건 관통
	if(pResult->m_pTerrain->m_Thickness <= 10)
	{
		return true;
	}

	INT32 iPenentratePercent = 0;
	CWeaponInfo * pWeaponInfo = getWeaponInfo();

#ifdef BUILD_RELEASE_QA_VERSION
	I3ASSERT( pWeaponInfo != nullptr);
#else
	if(pWeaponInfo == nullptr) return false;
#endif
	REAL32 Penetrate = GetWeaponPenetrate(false);
	REAL32 rTemp = Penetrate * pWeaponInfo->GetBulletWeight() * rWeaponRange;

	if(rTemp > I3_EPS)
	{
		iPenentratePercent = (INT32)(100 - (( pResult->m_pTerrain->m_Thickness * rFireRange) / rTemp));
		if(i3Math::Rand() % 100 <= iPenentratePercent)
		{
			return true;
		}
	}

	return false;



	// 관통되었습니다
	// return true;
}


bool WeaponBase::_CheckHit( i3CollideeLine& ColLine, REAL32 rLength, INT32 HitType )
{
	UINT32	nCount	= g_pFramework->raycast( &ColLine, HitType );
	if( nCount > 0 )
	{
		I3_PHYSIX_HIT_RESULT*	pResult	= g_pFramework->getRaycastResult();
		for( UINT32 i = 0; i < nCount; i++ )
		{
			if( rLength > pResult[i].m_T )
				rLength = _CheckWorldHitLength( &pResult[i], rLength );
		}

		if( rLength <= i3Vector::Length( ColLine.GetDir()) )
			return true;
	}
	return false;
}

bool WeaponBase::_CheckHitWorld( i3CollideeLine& ColLine, REAL32 rLength )
{	// revision 45500
	UINT32	nCount	= g_pFramework->raycast( &ColLine, CGRP_MASKGROUP_ENVIRONMENT );
	if( nCount > 0 )
	{
		I3_PHYSIX_HIT_RESULT*	pResult	= g_pFramework->getRaycastResult();
		for( UINT32 i = 0; i < nCount; i++ )
		{
			if( rLength > pResult[i].m_T )
			{
				i3EventReceiver * pReceiver = pResult->m_pDestShapeSet->getEventReceiver();
				//헬기 오브젝트는 폭파범위 무시.
				if(i3::same_of<CGameObjectRespawnHelicopter*>(pReceiver) ||
				   i3::same_of<CGameObjectRidingHelicopter*>(pReceiver))
				{
					continue;
				}

				rLength = _CheckWorldHitLength( &pResult[i], rLength );
			}
		}

		if( rLength < i3Vector::Length( ColLine.GetDir()) )
			return false;
	}

	return true;
}

REAL32 WeaponBase::_CheckWorldHitLength( I3_PHYSIX_HIT_RESULT * pResult, REAL32	rLength )
{
	if( getWeaponInfo()->isNotPenetrate())
	{// 관통이 안됩니다.
	}
	else if( _CheckPenetration( pResult, pResult->m_T,  rLength) )
	{
		return rLength;
	}

	// 관통이 안되면 가까운 히트 지점까지의 거리를 리턴시켜줍니다
	if( pResult->m_T < rLength )
		rLength = pResult->m_T;

	return rLength;
}

struct FORCE_INFO
{
	i3Object *	_pObj;
	VEC3D *		_pImpulse;
	VEC3D *		_pIntersect;
	REAL32		_rTorque;
	i3PhysixShapeSet * _pHitShape;
} ;

static I3SG_TRAVERSAL_RESULT _SetLinearForceProc( i3Node * pNode, void * pData, i3MatrixStack * pStack)
{
	i3PhysixShapeSet * pShape;
	MATRIX	mat;
	VEC3D	vR, vTorque;
	FORCE_INFO * pInfo = (FORCE_INFO *) pData;

	if( !i3::same_of<i3Transform2*>(pNode))
		return I3SG_TRAVERSAL_CONTINUE;

	i3Transform2 * pTrans2 = (i3Transform2 *) pNode;

	pShape = pTrans2->getShapeSet();
	I3ASSERT( pShape != nullptr );

	if( pInfo->_pHitShape != nullptr && pShape != pInfo->_pHitShape )
		return I3SG_TRAVERSAL_CONTINUE;

	pShape->getShapeGlobalPose( &mat );
	i3Vector::Sub( &vR, pInfo->_pIntersect, i3Matrix::GetPos( &mat ) );
	i3Vector::Cross( &vTorque, &vR, pInfo->_pImpulse );
	pShape->addForce( pInfo->_pImpulse);

	i3Vector::Scale( &vTorque, &vTorque, pInfo->_rTorque );
	pShape->addTorque( &vTorque);

	return I3SG_TRAVERSAL_CONTINUE;
}

WEAPON_CLASS_TYPE WeaponBase::_GetDinoTerrainHitWeaponClass( bool & bAddSound)
{
	WEAPON_CLASS_TYPE weaponClass = WEAPON_CLASS_UNKNOWN;
	EDinoAttackType attackType = ((CGameCharaDino*)m_pOwner)->GetNowDinoAttackType();

	if(m_pOwner->IsRaptor() || m_pOwner->IsElite() ||
		m_pOwner->IsCCRaptor() || m_pOwner->IsRaptorMercury()) 
	{
		if( attackType == DAT_RAPTOR_ATTACK_A)
		{
			if( getAutoFireCount() >= m_pWeaponInfo->GetAutoFireCount() - 1 )
			{
				weaponClass = WEAPON_CLASS_DINO_LEFT_SCRATCH;
			}
			else 
			{
				weaponClass = m_pWeaponInfo->GetTypeClass();
			}
		}
		else if( attackType == DAT_ELITE_SCRATCH) 
		{	
			if( getAutoFireCount() >= m_pWeaponInfo->GetAutoFireCount() - 1 )
			{
				weaponClass = WEAPON_CLASS_ELITE_LEFT_SCRATCH;
			}
			else 
			{
				weaponClass = WEAPON_CLASS_ELITE_RIGHT_SCRATCH;
			}
		}
		else if( attackType == DAT_RAPTOR_ATTACK_B) 
		{
			weaponClass = m_pWeaponInfo->GetTypeClass();
		}
		else if( attackType == DAT_ELITE_HOWL)
		{
		}
		else
		{
			I3PRINTLOG(I3LOG_FATAL, "invalid Dino attackType : %d", attackType);
		}
	}
	else if (m_pOwner->IsRex())
	{
		if( attackType == DAT_REX_ATTACK1) 
		{
			weaponClass = WEAPON_CLASS_TREX;
			bAddSound = false;
		}
	}
	else if (m_pOwner->IsSting())
	{
		if( attackType == DAT_STING_ATTACK1 || attackType == DAT_STING_ATTACK2) 
		{
			weaponClass = m_pWeaponInfo->GetTypeClass();
		}
		else
		{
			I3PRINTLOG(I3LOG_FATAL, "invalid Dino attackType : %d", attackType);
		}
	}
	else if (m_pOwner->IsTank())
	{
	}
	else if (m_pOwner->IsCCSting() || m_pOwner->IsStingMars())
	{
		if( attackType == DAT_STING_ATTACK1)
		{
			weaponClass = m_pWeaponInfo->GetTypeClass();
		}
		else if( attackType == DAT_STING_ATTACK2) 
		{
			weaponClass = WEAPON_CLASS_STING;
		}
		else
		{
			I3PRINTLOG(I3LOG_FATAL, "invalid Dino attackType : %d", attackType);
		}
	}
	else if (m_pOwner->IsDomiSting() || m_pOwner->IsDomiStingMars())
	{
		weaponClass = WEAPON_CLASS_STING;
	}

	return weaponClass;
}

void WeaponBase::_CalcHitWorld( I3_PHYSIX_HIT_RESULT * pResult, VEC3D * pDir)
{	
	if (m_pOwner == nullptr) return;
	if (m_pOwner->getBoneContext() == nullptr) return;

	I3_PHYSIX_TERRAIN * pTerrain = pResult->m_pTerrain;

	//I3TRACE( "HIT_WORLD +++++++++++++++++ TERRAIN_TYPE : %02d\n" , (pResult->m_pTerrain->m_Terrain) );

	I3ASSERT( pTerrain != nullptr );

	//	캐릭터 시야를 중심으로 하는 탄흔 이펙트를 회전해야 하는 값을 구한다. komet
	REAL32 fRotateRadVal = 0.f;
	REAL32	fFactor = i3Vector::Dot( &pResult->m_Normal, i3Matrix::GetUp( m_pOwner->GetMatrix() ) ); //&I3_YAXIS );
	
	if( fFactor > 0.001f )
	{
		fRotateRadVal = m_pOwner->getBoneContext()->getTheta() * fFactor;
	}
	else if( fFactor < -0.001f)
	{
		if( fFactor > -1.f )
		{
			fRotateRadVal = m_pOwner->getBoneContext()->getTheta() * (fFactor) + I3_PI;
		}
		else
		{
			fRotateRadVal = m_pOwner->getBoneContext()->getTheta() * fFactor - I3_PI2;
		}
	}

	bool bAddSound = true;
	WEAPON_CLASS_TYPE weaponClass = WEAPON_CLASS_UNKNOWN;

	if( m_pOwner->IsDino())
	{
		weaponClass = _GetDinoTerrainHitWeaponClass( bAddSound);
	}

	if( weaponClass != WEAPON_CLASS_UNKNOWN)
	{
		g_pEffectManager->AddTerrainEffect( (I3_TERRAIN_TYPE) pTerrain->m_Terrain, weaponClass, 
									static_cast<BULLET_EFFECT_TYPE>(m_pWeaponInfo->GetLoadBulletType()),
									&pResult->m_Intersect, &pResult->m_Normal, pDir, fRotateRadVal );
	}
	else
	{
		WEAPON_TERRAIN_EFFECT_INFO tEffectInfo;
		tEffectInfo._terrainType	= (I3_TERRAIN_TYPE) pTerrain->m_Terrain;
		tEffectInfo._pPos			= &pResult->m_Intersect;
		tEffectInfo._pNormal		= &pResult->m_Normal;
		tEffectInfo._pDir			= pDir;
		tEffectInfo._rRotate		= fRotateRadVal;
		tEffectInfo._pObj			= nullptr;

		_OnAddTerrainEffect( &tEffectInfo);
	}
	

	REAL32 rDelay = 0.f;

	if( m_pOwner->is1PV())
	{
		VEC3D vDir, vPos;
		m_pOwner->getCameraContext()->getVirtualCamPosition( &vPos);
		i3Vector::Sub( &vDir, &pResult->m_Intersect, &vPos );
		rDelay = i3Vector::Length( &vDir) * 0.02f;
		if( rDelay > 0.1f)
			rDelay = 0.1f;
	}

	if( bAddSound == true)
	{
		_AddTerrainEffectSound( (I3_TERRAIN_TYPE) pTerrain->m_Terrain, rDelay, &pResult->m_Intersect);
	}
}

void WeaponBase::_CalcHitObject( I3_PHYSIX_HIT_RESULT * pResult, i3CollideeLine * pLine, VEC3D * pDir, REAL32 rLength, bool bDamage )
{
	I3ASSERT( pResult != nullptr);

	if( !g_pFramework->IsBattleStage() )
		return;

	if( m_pOwner == nullptr)
		return;

	I3ASSERT( pResult->m_pDestShapeSet != nullptr);

	VEC3D vHitPos;
	I3_PHYSIX_HIT_RESULT * pLastResult = (I3_PHYSIX_HIT_RESULT *)i3MemAlloc(sizeof(I3_PHYSIX_HIT_RESULT));

	i3Object * pObject = (i3Object*) pResult->m_pDestShapeSet->getEventReceiver();
	// 오브젝트는 RandomDamage 미적용
	REAL32	rDamage = GetDamage( false);
	I3_PHYSIX_HIT_RESULT * pPenetrate = nullptr;

	if( pObject == nullptr )
	{
		I3PRINTLOG(I3LOG_WARN,  "오브젝트가 없어요~" );
		return;
	}

	if( i3::same_of<CGameObjectHatchling*>(pObject))
	{	// 기란맵 헤츨링 Damage 동작
		((CGameObjectHatchling*)pObject)->SetControledObjectState( 1);
	}
	else if( i3::same_of<CGameObjectRidingHelicopter*>(pObject))
	{	// OutPost 헬기는 공중에 뜬 경우에만 데미지를 받습니다.
		bDamage = (((CGameObjectRidingHelicopter*)pObject)->isEnableDamage() == true);
		if( i3::same_of<WeaponM197*>(this))
		{
			bDamage = false;
		}
	}

	if( bDamage)
	{
		// Wallshot
		if( m_rPenetrateLength < pResult->m_T )
		{ // 관통된 벽 뒤에 있는 경우
			if( m_pHitWorldPenetrate != nullptr && m_pHitWorldPenetrate->m_pTerrain != nullptr )
				pPenetrate = m_pHitWorldPenetrate;
			else if( m_pHitObjectPenetrate != nullptr && m_pHitObjectPenetrate->m_pTerrain != nullptr )
				pPenetrate = m_pHitObjectPenetrate;

			if( pPenetrate != nullptr )
			{
		#if 0
				rDamage	*= (1.f - (REAL32)pPenetrate->m_pTerrain->m_Thickness * 0.01f);
		#else
				rDamage *= 0.5f;
		#endif
			}
		}

		// 나이프는 오브젝트에 대해 1/2 데미지를 준다.
		if( getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_KNIFE || getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_KNUCKLE)
			rDamage *= 0.5f;

		// revision 62269 오브젝트 폭파 컬리전 체크 수정 및 파괴미션 헬기 버그 수정
		//컬리전 체크 시 기존 결과가 사라지는 것 때문에, memcpy로 기존의 정보를 보존한다.
		//컬리전 체크가 끝난 후 다시 결과를 복사해줌.
		memcpy(pLastResult, pResult, sizeof(I3_PHYSIX_HIT_RESULT));
		_CalcHitObjectDamage( pResult, pLine, rDamage);
		memcpy(pResult, pLastResult, sizeof(I3_PHYSIX_HIT_RESULT));
		I3MEM_SAFE_FREE(pLastResult);

		if( pObject->getDynamicState() == I3GAME_DYNAMIC_STATE_DYNAMIC )
		{	// Dynamic Object에 대한 힘 계산
			if( BattleServerContext::i()->IsP2PHost() ||
				(pObject->getNetworkSyncMode() == I3_NETWORK_GAMEOBJECT_TYPE_NONE) ||		// 네트워크 모드가 없는 경우이거나
				(pObject->getNetworkSyncMode() == I3_NETWORK_GAMEOBJECT_TYPE_STATIC) )		// 스테틱으로 세팅된 오브젝트의 경우 처리
				_CalcPhysixForce( pResult, pDir, rDamage);
		}
	}

	if( !getWeaponInfo()->isThrowingClass() )
	{
		I3_PHYSIX_TERRAIN * pTerrain = pResult->m_pTerrain;
		I3_TERRAIN_TYPE terrain = I3_TERRAIN_CONCRETE;

		if( pTerrain != nullptr)
		{
			terrain = (I3_TERRAIN_TYPE) pTerrain->m_Terrain;
		}

		REAL32 fRotateRadVal = 0.f;
		REAL32	fFactor = i3Vector::Dot( &pResult->m_Normal, &I3_YAXIS );
		REAL32 fTheta = m_pOwner->getBoneContext()->getTheta();
		
		i3Vector::Copy( &vHitPos, &pResult->m_Intersect);
		
		if( fFactor > 0.001f )
		{
			fRotateRadVal = fTheta * fFactor;
		}
		else if( fFactor < -0.001f)
		{
			if( fFactor > -1.f )
			{
				fRotateRadVal = fTheta - I3_PI2;
			}
			else
			{
				fRotateRadVal = fTheta * fFactor - I3_PI2;
			}
		}

		// revision 48169 오브젝트 이펙트에 공룡 예외처리
		bool bAddSound = true;
		WEAPON_CLASS_TYPE weaponClass = WEAPON_CLASS_UNKNOWN;

		if( m_pOwner->IsDino())
		{
			weaponClass = _GetDinoTerrainHitWeaponClass( bAddSound);
		}

		if( weaponClass != WEAPON_CLASS_UNKNOWN)
		{
			g_pEffectManager->AddTerrainEffect( terrain, weaponClass, 
				static_cast<BULLET_EFFECT_TYPE>(m_pWeaponInfo->GetLoadBulletType()),
				&vHitPos, &pResult->m_Normal, pDir, fRotateRadVal, pObject);
		}
		else
		{
			WEAPON_TERRAIN_EFFECT_INFO tEffectInfo;
			tEffectInfo._terrainType	= terrain;
			tEffectInfo._pPos			= &vHitPos;
			tEffectInfo._pNormal		= &pResult->m_Normal;
			tEffectInfo._pDir			= pDir;
			tEffectInfo._rRotate		= fRotateRadVal;
			tEffectInfo._pObj			= pObject;

			_OnAddTerrainEffect( &tEffectInfo);
		}

		REAL32 rDelay = 0.f;

		if( m_pOwner->is1PV() )
		{
			VEC3D vDir, vPos;
			m_pOwner->getCameraContext()->getVirtualCamPosition( &vPos);
			i3Vector::Sub( &vDir, &pResult->m_Intersect, &vPos );
			rDelay = i3Vector::Length( &vDir) * 0.02f;
			if( rDelay > 0.1f)
				rDelay = 0.1f;
		}

		if( bAddSound)
			_AddTerrainEffectSound( (I3_TERRAIN_TYPE) pTerrain->m_Terrain, rDelay, &pResult->m_Intersect);
	}
}

void WeaponBase::_CalcHitObjectDamage(  I3_PHYSIX_HIT_RESULT * pResult, i3CollideeLine * pLine,i3Object * pObject, i3PhysixShapeSet * pShapeSet, REAL32 rDamage)
{
	CGameMissionManager * pMissionManager = CGameMissionManager::i();
	
	CHud * pHud = CHud::i();
	if( CHud::i() == nullptr)
		return;
	
	I3ASSERT( m_pWeaponInfo != nullptr);

	if( m_pOwner == nullptr)
		return;

	bool	bMissionObj = false;
	bool	bDamageTo = false;

	if( rDamage < 1.f)
		rDamage = 1.f;

	STAGE_MODE stageType = MyRoomInfoContext::i()->getStage()->GetStageMode();
	switch( stageType)
	{
	case STAGE_MODE_ESCAPE:
	case STAGE_MODE_DESTROY:
		{
			if( pObject == pMissionManager->getRedObject() )
			{
				bMissionObj = true;

				if( isDamageToMissionObject() &&
					(m_pOwner->getCharaTeam() & CHARACTER_TEAM_BLUE) )
					bDamageTo = true;
			}
			else if( pObject == pMissionManager->getBlueObject() )
			{
				bMissionObj = true;

				if( isDamageToMissionObject() &&
					(m_pOwner->getCharaTeam() & CHARACTER_TEAM_RED) )
					bDamageTo = true;
			}
			else
				bDamageTo = true;
		}
		break;

	case STAGE_MODE_DEFENCE:
		{	// 현재는 Red team만 공격하는 형태입니다.
			if( (pObject == pMissionManager->getDefenceObj( 0)) ||
				(pObject == pMissionManager->getDefenceObj( 1)) )
			{
				bMissionObj = true;

				if( isDamageToMissionObject() &&
					(m_pOwner->getCharaTeam() & CHARACTER_TEAM_RED) )
					bDamageTo = true;
			}
			else
				bDamageTo = true;
		}
		break;
	default:
		bDamageTo = true;
		break;
	}

	if( BattleServerContext::i()->IsP2PHost() )
	{
		INT32 nTempState = pObject->getCurState();

		if( bMissionObj)
		{
			if( bDamageTo )
			{
				REAL32 rPreHP = pObject->getCurrentHP();
				pObject->Damage( rDamage, this, pShapeSet);
				//I3TRACE( "Obj damage by %s\n", this->getWeaponInfo()->GetName());

				if( pObject->getCurrentHP() != rPreHP)
				{
					pMissionManager->DamageObjectForUser( m_pOwner->getCharaNetIndex(), rDamage, pObject, (bool)(nTempState != pObject->getCurState()));

					//공격중인 캐릭터를 표시
					pHud->SetDetectedEnemyInfo( m_pOwner->getCharaNetIndex());
				}
			}
		}
		else
		{
			pObject->Damage( rDamage, this, pShapeSet);
			//I3TRACE( "Obj damage by %s\n", this->getWeaponInfo()->GetName());
		}
	}
	else
	{
		if( bMissionObj )
		{	// 파괴 오브젝트를 공격하는 케릭터가 적군인경우 표시
			//공격중인 케릭터를 표시
			if( bDamageTo && pObject->getCurrentHP() != 0.f)
			{
				pHud->SetDetectedEnemyInfo( m_pOwner->getCharaNetIndex());
			}
		}
		else if( (pObject->getNetworkSyncMode() == I3_NETWORK_GAMEOBJECT_TYPE_NONE) )
		{	// Client는 싱크모드가 없는 오브젝트만 데미지 계산
			pObject->Damage( rDamage, this, pShapeSet);
			//I3TRACE( "Obj damage by %s\n", this->getWeaponInfo()->GetName());
		}
		
		if( BattleServerContext::i()->IsUseDedicationMode())
		{
			// 오브젝트 확인
			if( pObject->getNetworkSyncMode() == I3_NETWORK_GAMEOBJECT_TYPE_ANIM)
			{	// 애니메이션 오브젝트
				if( !i3::kind_of<CGameControledObject*>(pObject)
					|| (((CGameControledObject*)pObject)->isManualUpdate() == false) )
				{	// 컨트롤 되지 않는 것은 데미지를 보내지 않습니다.
					bDamageTo = false;
				}
			}

			// revision  47188 
			//히트 카운터가 10개 이상 되기 때문에 //트리 오브젝트 제외 시킵니다.
			INT32 isTreeObject  = -1;
			isTreeObject = i3::contain_string( pObject->getSgRes()->getSceneGraphInfo()->GetName(), "Christmas");

			if( bDamageTo && isTreeObject < 0 )
			{// 공격 가능한지
				if( getOwner()->isLocal() || getOwner()->isAI() )
				{	// 자신이 맞춘경우입니다.
					// Network로 맞췄음을 보낸다.
					CHARA_DAMAGEINFO damageInfo;

					GetDamageInfo( &damageInfo);
					
					if( IsGrenade() )	// revision 39201
						damageInfo._DeathType = CHARA_DEATH_B;						

					damageInfo._HitType = GHT_OBJECT;
					damageInfo._nVictimIdx = pObject->getNetIndex();
					damageInfo._ObjNetType = pObject->getNetworkSyncMode();
					damageInfo._rDamage = rDamage;

					if (pLine != nullptr && pResult != nullptr)
					{
						i3Vector::Copy( &damageInfo._vStartPos, pLine->GetStart());
						i3Vector::Copy( &damageInfo._vHitPos, &pResult->m_Intersect);
#if (defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)) && 0
						I3TRACE("오브젝트 총구방향 x : %f  y : %f  z : %f  \n",damageInfo._vStartPos.x,damageInfo._vStartPos.y,damageInfo._vStartPos.z);
						I3TRACE("오브젝트 피격방향 x : %f  y : %f  z : %f  \n",damageInfo._vHitPos.x,damageInfo._vHitPos.y,damageInfo._vHitPos.z);
#endif
					}

					// revision 62135
					obj_explosion::i()->call_for_dedi(getOwner(), damageInfo, pObject);

					GAMEEVENT::Write_Chara_Hit( getOwner(), &damageInfo);
				}
			}
		}
	}
}

void WeaponBase::_CalcHitObjectDamage( I3_PHYSIX_HIT_RESULT * pResult,i3CollideeLine * pLine,REAL32 rDamage)
{
	if( g_pFramework->IsBattleStage() == false)
		return;

	i3Object * pObject = (i3Object*) pResult->m_pDestShapeSet->getEventReceiver();
	I3ASSERT( i3::kind_of<i3Object*>(pObject));

	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
	{
		CStageBattle* pStage = g_pFramework->GetStageBattle();
		I3ASSERT(pStage);

		if(pStage != nullptr && gmode::i()->GetTutorialMode() != nullptr)
		{
			if( gmode::i()->GetTutorialMode()->IsAvailableTargetOnTutorialStateOnWeapon(pObject->getNetIndex(), this) == false)
			{
				return;
			}
		}
		else
		{
			I3ASSERT_0;
		}
	}

	_CalcHitObjectDamage( pResult, pLine, pObject,pResult->m_pDestShapeSet, rDamage);
}

void WeaponBase::_CalcHitChara( I3_PHYSIX_HIT_RESULT * pResult, i3CollideeLine * pLine, VEC3D * pDir, REAL32 rLength)
{
	VEC3D vPos;
	i3PhysixShapeSet *	pShapeSet = pResult->m_pDestShapeSet;
	I3ASSERT( pShapeSet != nullptr);
	
	CGameCharaBase * pHitChara = nullptr;
	CGameCharaEquip * pEquip = nullptr;

	CHARA_DAMAGEINFO DamageInfo;

	i3EventReceiver * pReceiver = pShapeSet->getEventReceiver();

	if(pReceiver == nullptr)
	{
		I3PRINTLOG(I3LOG_NOTICE,"ShapeSet Receiver == nullptr");
		return;
	}

	if( i3::same_of<CGameCharaEquip*>(pReceiver))
	{
		pEquip = (CGameCharaEquip*) pReceiver;
		if( i3::same_of<CGameCharaBase*>(pEquip->GetOwner()))
			pHitChara = (CGameCharaBase*) pEquip->GetOwner();
	}
	else if( i3::kind_of<CGameCharaBase*>(pReceiver))
		pHitChara = (CGameCharaBase*) pReceiver;
	else
	{
		I3PRINTLOG(I3LOG_FATAL,  "알 수 없는 Hit." );
	}
	
	
	if( pHitChara != nullptr)
	{
		if( pHitChara->getCharaNetIndex() == -1)
			return;

		if( pHitChara->getCollisionContext() == nullptr)	// 막 해제된 경우 들어올수 있다.
			return;

		if( !pHitChara->isAIToClient() && pHitChara->getCharaInfo()->GetType() != CHARA::TYPE_DOMI_FULLBODY_AI)
		{
			if( BattleSlotContext::i()->getSlotInfo( pHitChara->getCharaNetIndex())->_State != SLOT_STATE_BATTLE)
				return;
		}

		VEC3D vecDir = VEC3D( 0.f, 0.f, 0.f);
		
		GetDamageInfo( &DamageInfo);

		// GetDamageInfo 안에서 getAP()로 처리되고 있다.
		// 원래의 코드는 아래 코드이기에 주석으로 남겨둠.
		//DamageInfo._rDamage			= GetDamage();
		DamageInfo._nKillerType		= m_pOwner->getCharaInfo()->GetType();
		DamageInfo._pHitResult		= pResult;
		
		DamageInfo._HitType	= GHT_CHARA;

		// 관통량
		if( m_rPenetrateLength < pResult->m_T )
		{ // 관통된 벽 뒤에 있는 경우
			if( (m_pHitWorldPenetrate != nullptr) && (m_pHitWorldPenetrate->m_pTerrain != nullptr) )
				DamageInfo._WallShotReduceRate = m_pHitWorldPenetrate->m_pTerrain->m_Thickness;
			else if( (m_pHitObjectPenetrate != nullptr) && (m_pHitObjectPenetrate->m_pTerrain != nullptr) )
				DamageInfo._WallShotReduceRate = m_pHitObjectPenetrate->m_pTerrain->m_Thickness;

			if( (DamageInfo._WallShotReduceRate < 0) || (DamageInfo._WallShotReduceRate > 100) )
			{
				I3TRACE( "Wall thickness is invalid..\n" );
				DamageInfo._WallShotReduceRate = 50;
			}
		}


		VEC3D diff;
		i3Vector::Sub( &diff, pHitChara->GetPos(), m_pOwner->GetPos());
		DamageInfo._rDistance = i3Vector::Length( &diff);


		if( g_pEnvSet->m_bEnableBlood )
		{
			VEC3D	vRand, vAdd;
			i3CollideeLine BloodLine;
			I3_PHYSIX_HIT_RESULT * pWallHitResultTemp = nullptr;
			REAL32 rBloodLength;

			if( m_pHitWorld != nullptr )
			{
				rBloodLength = m_pHitWorld->m_T - pResult->m_T;
				if( rBloodLength > 0.f && rBloodLength < BLOODEFFECT_HIT_LENGTH )
				{
					pWallHitResultTemp = m_pHitWorld;
					BloodLine.SetStart( pLine->GetStart() );
				}
			}
			if( m_pHitWorldPenetrate != nullptr )
			{	
				if( m_pHitWorldPenetrate->m_T < pResult->m_T )
				{	// 관통된 벽 뒤에 있으면 벽에서부터 다시 검사
					i3Vector::Scale( &vAdd, &m_pHitWorldPenetrate->m_Normal, -0.01f );
					i3Vector::Add( &vAdd, &vAdd, &m_pHitWorldPenetrate->m_Intersect );
					BloodLine.SetStart( &vAdd );
				}
				else
				{	// 벽의 앞이라면 관통된 벽을 히트 컬리젼으로 한다.
					pWallHitResultTemp = m_pHitWorldPenetrate;
					BloodLine.SetStart( pLine->GetStart() );
				}
			}

			if( pWallHitResultTemp != nullptr )
			{
				// calc Random position
				i3Vector::Set( &vAdd, 1.f, 1.f, 1.f );
				i3Vector::Sub( &vAdd, &vAdd, &pWallHitResultTemp->m_Normal );
				i3Vector::Set( &vRand, (REAL32)((i3Math::Rand() % 100) - 50) * 0.001f, (REAL32)((i3Math::Rand() % 100) - 50) * 0.001f, (REAL32)((i3Math::Rand() % 100) - 50) * 0.001f );
				i3Vector::Mul( &vAdd, &vAdd, &vRand );
				i3Vector::Add( &vAdd, &vAdd, &pWallHitResultTemp->m_Intersect );
				BloodLine.SetEnd( &vAdd );

				UINT32 nResultCount = g_pFramework->raycast( &BloodLine, CGRP_MASKGROUP_ATTACK );
				if( nResultCount > 0 )
				{
					pWallHitResultTemp = g_pFramework->getRaycastResult();

					for( UINT32 i = 0; i < nResultCount; i++ )
					{
						if( pWallHitResultTemp[ i ].m_T - pResult->m_T < BLOODEFFECT_HIT_LENGTH )
							DamageInfo._pWallHitResult = &pWallHitResultTemp[ i ];
					}
				}
			}
		}

		CHARA_HIT_PART part = pHitChara->getCollisionContext()->GetHitPart( pResult->m_pDestShapeSet);
		if( part == CHARA_HIT_UNKNOWN)
			return;	// 장비에 맞은 경우는 무시

		if( m_pWeaponInfo->isExplosionPhysics())
		{	//	폭발물에 죽음
			DamageInfo._DeathType	= CHARA_DEATH_B;
		}
		else
		{
			//	헤드샷 죽음				
			if( part == CHARA_HIT_HEAD)
				DamageInfo._DeathType = CHARA_DEATH_H;
			//	보통 죽음
			else
				DamageInfo._DeathType = CHARA_DEATH_A;	//	CHARA_DEATH_A 는 임시이므로 다른 Death 동작이 필요하면 변경이 필요합니다.			
			
			if( m_pOwner != nullptr)
			{
				i3Vector::Set( &vPos, getX( m_pOwner->GetPos() ), getY( m_pOwner->GetPos() ) + m_pOwner->getCameraContext()->getIKHeightForCam(), getZ( m_pOwner->GetPos() ) );
			}

			DamageInfo._pVecPos		=  &vPos;
		}

		CalcHitDir( pResult, DamageInfo._pVecPos, &vecDir);
		DamageInfo._pVecDir		= &vecDir;
		DamageInfo.SetHitPart( part );
		DamageInfo._IsNPC = false;
		DamageInfo._nVictimIdx		= pHitChara->getCharaNetIndex();

		if(i3::same_of<WeaponThrowKnife*>(DamageInfo._pWeapon))
		{	
			//Throwing Knife 계열의 무기라면, 피격자 위치를 보낸다.
			//차후 Throwing Knife 계열 무기 갯수가 증가할 경우, 따로 처리하는 것이 맞을듯.
			i3Vector::Copy( &DamageInfo._vStartPos, DamageInfo._pVecPos);
			// 던져진 객체는 현재 들고 있는 무기에 상관 없이 extension이 꺼진 것으로 인식
			DamageInfo._bExtension = false;
		}
		else
		{
			i3Vector::Copy( &DamageInfo._vStartPos, pLine->GetStart());
		}
		
		i3Vector::Copy( &DamageInfo._vHitPos, &pResult->m_Intersect);

		I3ASSERT( DamageInfo.isValid() );

		// 캐릭터의 HP 및 이펙트 처리
		pHitChara->OnHit( &DamageInfo, m_pOwner);
	}
	else if( pEquip != nullptr )
	{	// 헬멧 자체가 맞은 경우(월드 헬멧)
		pShapeSet->setSleeping( false );

		VEC3D vLin, vTorque, vRoll;
		MATRIX mat;
		i3Vector::Normalize( &vLin, pLine->GetDir() );
		i3Vector::Scale( &vLin, &vLin, getAP() * 0.5f );
		
		pShapeSet->getShapeGlobalPose( &mat );
		i3Vector::Sub( &vRoll, &pResult->m_Intersect, i3Matrix::GetPos( &mat ) );
		i3Vector::Cross( &vTorque, &vRoll, &vLin );
		pShapeSet->setLinearVelocity( &vLin );

		pShapeSet->setAngularVelocity( &vTorque );

		g_pEffectManager->AddEquipHitEffect( m_pWeaponInfo->GetTypeClass(),	
			static_cast<BULLET_EFFECT_TYPE>(m_pWeaponInfo->GetLoadBulletType()),
			&pResult->m_Intersect, &pResult->m_Normal, m_pOwner->is1PV() );
	}

	// 크로스헤어 이펙트
	if( g_pEnvSet->m_bCrossHairEffect && (m_pOwner != nullptr) &&
		m_pOwner->isLocal() )
	{
		if( m_pWeaponInfo->isEnableCrossHairEffect() )
		{
			if (i3::kind_of<WeaponSentryGun*>(this) == false)
			{
				if ( CrosshairMgr::i()->getCrosshair()->isEnable())
				{
					CrosshairMgr::i()->getCrosshair()->SetEnableHitEffect(true);
				}
			}

		}
	}

	// 헬멧의 경우가 있는거 같아서 pHitChara 널검사만 추가하는 것으로만 일단 수정..
 	if( pHitChara && pHitChara->IsRex() && m_pOwner != nullptr && m_pOwner->isLocal() && DamageInfo.GetHitPart() == CHARA_HIT_VITAL_EYE)
	{
		if( pHitChara->isDeathblowState() == false)
		{
			CHud		*	pHud = CHud::i();

			if( pHud != nullptr)
			{
				pHud->AddDeathBlow();
				pHitChara->SetDeathblowState( true);

				DEATHBLOW_INFO deathBlowInfo;
				deathBlowInfo._nTrexIdx = (UINT8) pHitChara->getCharaNetIndex();
				deathBlowInfo._WeaponInfo = DamageInfo._ui32BaseWeaponID;

				GameEventSender::i()->SetEvent(EVENT_BATTLE_DEATHBLOW ,&deathBlowInfo);
			}
		}
	}
}

void WeaponBase::_CalcHitItem( I3_PHYSIX_HIT_RESULT * pResult, VEC3D * pDir)
{
	I3ASSERT( pResult != nullptr);

	i3PhysixShapeSet * pShapeSet = pResult->m_pDestShapeSet;
	i3EventReceiver * pRecv = pShapeSet->getEventReceiver();
	I3ASSERT( pRecv != nullptr);


	if( i3::same_of<WeaponC5*>(pRecv) || i3::same_of<WeaponClaymore*>(pRecv) || i3::same_of<WeaponM14*>(pRecv))
	{
		//근접 공격으로 맞았으면 C5는 처리하지 않도록 합니다. 여기에는 깊은 사연이 있어요...
		if(  getUsedWeapon()->getWeaponInfo()->GetTypeFire() == WEAPON::FIRE_MELEE )
			return;

		WeaponBase * pThrow = (WeaponBase*) pRecv;

		if( getOwner()->isLocal() || getOwner()->isAI())
		{	// 자신이 맞춘 경우입니다.
			// Network로 맞췄음을 보낸다.
			CHARA_DAMAGEINFO damageInfo;

			pThrow->GetDamageInfo( &damageInfo);

			if( IsGrenade() )
				damageInfo._DeathType	= CHARA_DEATH_B;

			damageInfo._HitType = GHT_ITEM_C5;
			damageInfo._nVictimIdx = pThrow->getNetworkIdx();
			damageInfo._rDamage = pThrow->GetDamage( false);

			GAMEEVENT::Write_Chara_Hit( getOwner(), &damageInfo);

			if( BattleServerContext::i()->IsP2PHost() )
			{
				MainWeapon_Throw * pMainWeapon = static_cast<MainWeapon_Throw*>(pThrow->GetMainWeapon());

				if( i3::same_of<WeaponC5*>(pThrow) )
				{
					static_cast<MainWeapon_C5*>(pMainWeapon)->Explosion_C5();
				}
				else if( i3::same_of<WeaponClaymore*>(pThrow) )
				{
					static_cast<MainWeapon_Claymore*>(pMainWeapon)->Explosion();
				}
				else if (i3::same_of<WeaponM14*>(pThrow))
				{
					static_cast<MainWeapon_M14*>(pMainWeapon)->Explosion();
				}
			}
		}
	}

	if( i3::same_of<WeaponDinoDummy*>(pRecv))
	{
		WeaponDinoDummy * pDummy = (WeaponDinoDummy*) pRecv;

		if( getOwner()->isLocal() || getOwner()->isAI())
		{	// 자신이 맞춘 경우입니다.
			// Network로 맞췄음을 보낸다.

			//서버에서 처리하기로 했습니다.


			CHARA_DAMAGEINFO damageInfo;

			GetDamageInfo( &damageInfo);

			damageInfo._HitType = GHT_OBJECT; // 더미히트
			damageInfo._nVictimIdx = pDummy->getNetworkIdx();
			damageInfo._rDamage = /*pDummy->*/GetDamage( false);
			damageInfo._ObjNetType = I3_NETWORK_GAMEOBJECT_TYPE_STATIC;

			GAMEEVENT::Write_Chara_Hit( getOwner(), &damageInfo);

		}

		if( BattleServerContext::i()->IsP2PHost() )
		{
			//	pDummy->Uninstall( pDummy->getNetworkIdx());
		}
	}

}


namespace 
{
	template<class T>
	i3PhysixShapeSet* FindValidDomiDinoShapeSet(const T idx, const T max, i3Skeleton* skel, CGameCharaBase* chara)
	{
		i3PhysixShapeSet* shapeSet = skel->getShapeSet(chara->getBoneContext()->getBoneIndex(idx));
		if (shapeSet) return shapeSet;

		for (INT32 i=0; i<static_cast<INT32>(max); i++)
		{
			shapeSet = skel->getShapeSet(chara->getBoneContext()->getBoneIndex(i));
			if (shapeSet) break;
		}

		return shapeSet;
	}
}
void WeaponBase::_OnHitExplosionChara( CGameCharaBase * pChara, CHARA_HIT_PART part, VEC3D * pDir)
{
	VEC3D	vDir;
	I3_PHYSIX_HIT_RESULT	tempHitResult;
	I3_PHYSIX_HIT_RESULT *	pWallHitResultTemp;

	I3ASSERT( pChara != nullptr );
	I3ASSERT( pChara->getSceneObject()->GetBody() != nullptr);
	I3ASSERT( pChara->getSceneObject()->GetBody()->getFrameLOD() != nullptr);

	i3Skeleton * pSkel = pChara->getSceneObject()->GetBody()->getFrameLOD()->getSkeleton();

	I3ASSERT( pSkel != nullptr);
	
	CHARA_DAMAGEINFO DamageInfo;

	tempHitResult.m_T			= i3Vector::Normalize( &tempHitResult.m_Normal, pDir);
	tempHitResult.m_pDestShapeSet = nullptr;

	if( part == CHARA_HIT_HEAD )
	{
		i3Vector::Copy( &tempHitResult.m_Intersect, i3Matrix::GetPos( pChara->getBoneContext()->getHeadMatrix()) );
		if( pChara->IsRex())
		{	
			tempHitResult.m_pDestShapeSet = pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( TREX_BONE_HEAD) );
		}
		else if( pChara->IsRaptorBase())
		{
			tempHitResult.m_pDestShapeSet = pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( RAPTOR_BONE_HEAD) );
		}
#ifdef DOMI_DINO
		else if (pChara->IsDomiRaptorBoneBase())
		{
			tempHitResult.m_pDestShapeSet = pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( DOMI_RAPTOR_BONE_HEAD) );
		}
		else if ( pChara->IsDomiMutantRex())
		{
			tempHitResult.m_pDestShapeSet = pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( DOMI_MUTANTREX_BONE_HEAD) );
		}
#endif
		else
		{
			tempHitResult.m_pDestShapeSet = pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( BONE_HEAD) );
		}
	}
	else if( part == CHARA_HIT_ROOT)
	{
		i3Vector::Copy( &tempHitResult.m_Intersect, i3Matrix::GetPos( pChara->getBoneContext()->getIKMatrix()) );

		if( pChara->IsRex())
		{	
			tempHitResult.m_pDestShapeSet = pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( TREX_BONE_PELVIS) );
		}
		else if( pChara->IsRaptorBase())
		{
			tempHitResult.m_pDestShapeSet = pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( RAPTOR_BONE_ROOT) );
		}
#ifdef DOMI_DINO
		else if( pChara->IsDomiRaptorBoneBase())
		{
			tempHitResult.m_pDestShapeSet 
				= FindValidDomiDinoShapeSet<DOMI_RAPTOR_BONES>(DOMI_RAPTOR_BONE_ROOT, DOMI_RAPTOR_BONE_MAX, pSkel, pChara);
		}
		else if (pChara->IsDomiMutantRex())
		{
			tempHitResult.m_pDestShapeSet 
				= FindValidDomiDinoShapeSet<MUTANT_TREX_BONES>(DOMI_MUTANTREX_BONE_PELVIS, DOMI_MUTANTREX_BONE_MAX, pSkel, pChara);
		}
#endif
		else
		{
			tempHitResult.m_pDestShapeSet = pSkel->getShapeSet( pChara->getBoneContext()->getBoneIndex( BONE_ROOT) );
		}
	}

	I3ASSERT( tempHitResult.m_pDestShapeSet != nullptr);
	
	i3Vector::Scale( &tempHitResult.m_Normal, &tempHitResult.m_Normal, -1.f);

	GetDamageInfo( &DamageInfo);

	DamageInfo._pHitResult		= &tempHitResult;
	DamageInfo._HitType	= GHT_CHARA;
	DamageInfo.SetHitPart( part );
	DamageInfo._rDistance		= tempHitResult.m_T;

	if( g_pEnvSet->m_bEnableBlood )
	{
		VEC3D	vAdd;
		i3CollideeLine BloodLine;

		if( part == CHARA_HIT_HEAD )
		{
			BloodLine.SetStart( i3Matrix::GetPos( pChara->getBoneContext()->getHeadMatrix()) );
		}
		else if( part == CHARA_HIT_ROOT)
		{
			BloodLine.SetStart( i3Matrix::GetPos( pChara->getBoneContext()->getIKMatrix()) );
		}
		
		i3Vector::Add( &vAdd, BloodLine.GetStart(), pDir);
		BloodLine.SetEnd( &vAdd );

		pWallHitResultTemp = g_pFramework->raycastClosest( &BloodLine, CGRP_MASKGROUP_ATTACK );
		if( pWallHitResultTemp != nullptr )
		{
			if( pWallHitResultTemp->m_T < BLOODEFFECT_HIT_LENGTH )
				DamageInfo._pWallHitResult = pWallHitResultTemp;
		}
	}

	//	폭발물에 죽음
	if((m_pWeaponInfo->GetTypeClass() == WEAPON_CLASS_GRENADESHELL) &&
		((m_pWeaponInfo->GetNumber() == WEAPON::getItemIndex(WEAPON::GSHELL_POISON))))
	{
	
			DamageInfo._DeathType	= CHARA_DEATH_POISON;

	}
	else
	DamageInfo._DeathType	= CHARA_DEATH_B;

	i3Vector::Scale( &vDir, &tempHitResult.m_Normal, -1.f);
	DamageInfo._pVecDir		= &vDir;
	DamageInfo._nVictimIdx	= pChara->getCharaNetIndex();

	i3Vector::Copy( &DamageInfo._vStartPos, GetPos() );
	i3Vector::Copy( &DamageInfo._vHitPos, &tempHitResult.m_Intersect);

	I3ASSERT( DamageInfo.isValid() );

	// 캐릭터의 HP 및 이펙트 처리
	pChara->OnHit( &DamageInfo);
}

REAL32 WeaponBase::_CheckExplosionObjectCollision( REAL32 rRange, REAL32 rRadian)
{
	INT32		i, j;
	VEC3D		vDir, vPos, vTorqPos;
	i3Object *	pGameObj;
	i3CollideeLine	ColLine;

	I3ASSERT( rRange > I3_EPS);
	
	CStageBattle * pStage = g_pFramework->GetStageBattle();
	I3ASSERT_RETURN( pStage != nullptr, 0.f);
	
	StageObjectListPtr pObjectList = pStage->GetStageObjectList();

	if( pObjectList == nullptr )
		return 0.f;

	UINT8	nHitMap[ OBJ_TOTAL_COUNT ] = {0,};
	REAL32 rLength = 0.f;
	REAL32	rDamage = getAP();

	ColLine.SetStart( GetPos() );

	NxScene * pNxScene = g_pPhysixCtx->getScene();
	NxShape* pShapeList[ 256];
	
	NxSphere nxSphere;
	nxSphere.center.x = getX( GetPos());
	nxSphere.center.y = getY( GetPos());
	nxSphere.center.z = getZ( GetPos());

	//if( i3::same_of<WeaponRocketLauncherShell*>(this))
		//rRange += 4.f;

	nxSphere.radius = rRange;

	INT32 iActiveGrp = (1<<CGRP_OBJECT_HIT) | (1<<CGRP_OBJECT_ALL) | (1<<CGRP_OBJECT_WHOLE) | (1<<CGRP_OBJECT_WHOLE_DYNAMIC);

	INT32 nOverlapCount = pNxScene->overlapSphereShapes( nxSphere, NX_DYNAMIC_SHAPES, 256, pShapeList, nullptr, iActiveGrp);
	//I3TRACE( "overlapcount : %d\n", nOverlapCount);
	
	for( i = 0; i < nOverlapCount; i++ )
	{
		i3EventReceiver * pReceiver = (i3EventReceiver*) pShapeList[ i]->userData;
		if( pReceiver == nullptr)
			continue;
		if( !i3::same_of<i3PhysixShapeSet*>(pReceiver))
			continue;

		i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) pReceiver;
		if( !i3::kind_of<i3Object*>(pShapeSet->getEventReceiver()))
			continue;

		pGameObj = (i3Object*)pShapeSet->getEventReceiver();
		I3ASSERT( pGameObj->getNetIndex() > -1);

		// 이미 등록되어 있는지 확인합니다.
		if( nHitMap[ pGameObj->getNetIndex() ] != 0 )
			continue;

		// 중심점을 기준으로 계산합니다.
		MATRIX mat;
		if( pGameObj->getSceneObject()->getCurrentAnim() != nullptr)
		{
			pShapeSet->getShapeGlobalPose( &mat);
			i3Vector::Copy( &vPos, i3Matrix::GetPos( &mat));
		}
		else
		{	
			pShapeSet->getShapeMassCenterPos( &vPos);
		}

		if( IsValidExplosionArea(&vPos, rRange, rRadian, false) == false )
			continue;

		if( i3::same_of<CGameObjectHatchling*>(pGameObj))
		{	// 기란맵 헤츨링 Damage 동작
			((CGameObjectHatchling*)pGameObj)->SetControledObjectState( 1);
		}
		else if( i3::same_of<CGameObjectRidingHelicopter*>(pGameObj))
		{	// OutPost 헬기는 공중에 뜬 경우에만 데미지를 받습니다.
			if( ((CGameObjectRidingHelicopter*)pGameObj)->isEnableDamage() == false)
				continue;
			else
			{
				for( j = 0; j < g_pCharaManager->getCharaCount(); j++)
				{
					CGameCharaBase * pChara = g_pCharaManager->getChara( j);

					if( pChara == nullptr)
						continue;
					if( (pChara->getAttachedObject() == pGameObj) && pChara->is1PV() && (g_pCamera != nullptr) )
					{
						g_pCamera->SetTremble( 5.0f, 15.0f, 0.2f, 2.0f);
					}
				}
			}
		}
		else if( i3::same_of<CGameObjectRespawnHelicopter*>(pGameObj))
		{
			for( j = 0; j < g_pCharaManager->getCharaCount(); j++)
			{
				CGameCharaBase * pChara = g_pCharaManager->getChara( j);		

				if( pChara == nullptr)
					continue;

				if( (pChara->getAttachedObject() == pGameObj) && pChara->is1PV() && (g_pCamera != nullptr) )
				{
					g_pCamera->SetTremble( 5.0f, 15.0f, 0.2f, 2.0f);
				}
			}
		}
		
		// HP가 있는 오브젝트는 0이면 스킵
		if( (pGameObj->getStateCount() > 1) && (pGameObj->getCurrentHP() == 0.f) )
			continue;

		//World에 대해서 한번 테스트
		ColLine.SetEnd( &vPos );

		// revision 63631	폭발위치와 오브젝트 사이에 장애물이 있으면 오브젝트는 데미지를 입지 않음 - 2013. 8. 27 상훈
		// revision 64633
		/*if( pResult != nullptr && pResult->m_pDestShapeSet != pShapeSet)
			return 0;*/

		// revision 52397 폭발 오브젝트에서 월드 컬리전 체크 코드 삭제
		/*if( _CheckHitWorld( ColLine, m_pWeaponInfo->GetExplosion_Range() ) == false )
			continue;*/

		i3Vector::Sub( &vDir, &vPos, GetPos() );
		rLength = i3Vector::Normalize( &vDir, &vDir );

		// 토크를 주기위한 위치를 계산합니다.
		i3Vector::Scale( &vTorqPos, &vDir, MINMAX( 0.f, 1.f - (rLength / rRange), 1.f) );
		
		if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
		{
			if( gmode::i()->GetTutorialMode() != nullptr)
			{
				if(gmode::i()->GetTutorialMode()->IsAvailableTargetOnTutorialStateOnWeapon(pGameObj->getNetIndex(), this) == false)
				{
					continue;
				}
			}
		}

		//2014.3.27 박기성
		//GrenadeShell 오브젝트 데미지를 위한 컬리전 계산.
		I3_PHYSIX_HIT_RESULT* pResult = g_pFramework->raycastClosest( &ColLine, CGRP_MASKGROUP_ATTACK);
		_CalcHitObjectDamage( pResult, &ColLine, pGameObj, pShapeSet, rDamage);
		nHitMap[ pGameObj->getNetIndex() ] = 1;

		if( pGameObj->getDynamicState() == I3GAME_DYNAMIC_STATE_DYNAMIC )
		{	// Dynamic Object에 대한 힘 계산
			if( BattleServerContext::i()->IsP2PHost() ||
				((pGameObj->getNetworkSyncMode() != I3_NETWORK_GAMEOBJECT_TYPE_DYNAMIC) &&
				(pGameObj->getNetworkSyncMode() != I3_NETWORK_GAMEOBJECT_TYPE_MOVE)) )
			{
				_CalcPhysixForce( pShapeSet, &vDir, &vTorqPos, rDamage, rLength);
			}
		}

		//I3TRACE( "HitObject : idx %d (colGroup %d) len : %.4f damage : %.4f\n", pGameObj->getNetIndex(), pDestShapeSet->getShapeGroup(), pNextResult->m_T, rDamage);
	}

	return rLength;
}

void WeaponBase::_CheckExplosionWeapon( REAL32 rRange, REAL32 rRadian )
{
	VEC3D vDir;
	i3CollideeLine line;

	line.SetStart( GetPos());

	// C5만 검사
	UINT32 ItemCount = WEAPON::getSettableItemCount();
	for ( UINT32 Index = 0 ; Index < ItemCount; ++Index)
	{
		WEAPON::RES_ID resID = WEAPON::getSettableItemIndex(Index);
		if( resID >= WEAPON::getItemCount(WEAPON_CLASS_THROWING_GRENADE))
			continue;	// 등록 안된 무기

		INT32 nCount = g_pWeaponManager->getThrowWeaponCount( resID );

		for( INT32 i = 0; i < nCount; i++)
		{
			WeaponBase * pThrow = static_cast<WeaponBase*>( g_pWeaponManager->getThrowWeapon( resID, i));

			if( pThrow == this)			continue;
			if( !pThrow->isEnable() )	continue;

			MainWeapon_Throw * pMainweapon = static_cast<MainWeapon_Throw*>( pThrow->GetMainWeapon());

			if( !pMainweapon->isBombState( WEAPON_BOMBSTATE_THROWING) )				continue;
			if( pMainweapon->isBombState(WEAPON_BOMBSTATE_EXPLOSION) )				continue;

			if( !IsValidExplosionArea(pThrow->GetPos(), rRange, rRadian, false) )	continue;

			i3Vector::Sub( &vDir, pThrow->GetPos(), GetPos());
			REAL32 rLen = i3Vector::Length( &vDir);

			if( rLen < rRange)
			{
				line.SetDir( &vDir);

				I3_PHYSIX_HIT_RESULT * pResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK);
				if( pResult != nullptr)
				{
					if( pResult->m_pDestShapeSet->getShapeGroup() != CGRP_ITEM_HIT)
					{
						// 해당 오브젝트보다 가까운 거리에 무엇인가 있으면 스킵
						if( pResult->m_T < rLen)
							continue;
					}
				}

				// 자동 폭발하도록 커맨드를 줍니다.
				// 거리에 따라 딜레이 타임을 줍니다.
				if( IsDedicatedHost() )
				{
					//이젠 Host에서 처리하는 부분이 아니라..데디에서 처리하므로..
					//이곳에서 바로 히트 패킷을 보낸다.
					CHARA_DAMAGEINFO damageInfo;

					pThrow->GetDamageInfo( &damageInfo);

					/*if( IsGrenade())
						damageInfo._DeathType	= CHARA_DEATH_B;*/

					damageInfo._HitType = GHT_ITEM_C5;
					damageInfo._nVictimIdx = pThrow->getNetworkIdx();
					damageInfo._rDamage = pThrow->GetDamage( false);

					GAMEEVENT::Write_Chara_Hit( getOwner(), &damageInfo);
				}
			}
		}
	}
}

UINT32 WeaponBase::_OnHitBloodMark( I3_PHYSIX_HIT_RESULT ** pResultOut, VEC3D * pStartPos, VEC3D * pDir)
{
	UINT32	i;

	i3CollideeLine BloodLine;
	I3_PHYSIX_HIT_RESULT * pWallHitResultTemp = nullptr;
	REAL32	rMinLen = BLOODEFFECT_HIT_LENGTH;

	BloodLine.SetStart( pStartPos);

	{
		VEC3D	vAdd;
		i3Vector::Add(&vAdd, BloodLine.GetStart(), pDir);
		BloodLine.SetEnd(&vAdd);
	}

	UINT32 nResultCount = g_pFramework->raycast( &BloodLine, CGRP_MASKGROUP_ATTACK );
	if( nResultCount > 0 )
	{
		pWallHitResultTemp = g_pFramework->getRaycastResult();
		g_pFramework->reserveRaycastResult( nResultCount);

		for( i = 0; i < nResultCount; i++ )
		{
			if( pWallHitResultTemp[ i ].m_T < rMinLen )
			{
				*pResultOut = &pWallHitResultTemp[ i ];
				rMinLen = pWallHitResultTemp[ i].m_T;
			}
		}
	}

	return nResultCount;
}

void WeaponBase::_OnHitCharaByExplosion( CGameCharaBase * pHitUser, CHARA_HIT_PART part, VEC3D * pDir,VEC3D * pStart)
{	
	MATRIX mat;
	I3_PHYSIX_HIT_RESULT	tempHitResult;


	I3ASSERT( pHitUser != nullptr );
	I3ASSERT( m_pOwner != nullptr);
	I3_BOUNDCHK( part, CHARA_HIT_MAXCOUNT);

	if(pHitUser->isEnable() == false)
	{
		return;
	}

	const PHYSIX_SHAPESET_INFO * pShapeSetInfo = pHitUser->getPhysixShapeSetInfo( part);

	CHARA_DAMAGEINFO DamageInfo;

	GetDamageInfo( &DamageInfo);
	
	tempHitResult.m_T			= i3Vector::Normalize( &tempHitResult.m_Normal, pDir);
	pShapeSetInfo->_pShapeSet->getShapeGlobalPose( &mat);
	i3Vector::Copy( &tempHitResult.m_Intersect, i3Matrix::GetPos( &mat));
	tempHitResult.m_pDestShapeSet = pShapeSetInfo->_pShapeSet;
	i3Vector::Scale( &tempHitResult.m_Normal, &tempHitResult.m_Normal, -1.f);

	DamageInfo._rExplosionRange = GetExplosionRange();
	DamageInfo._pHitResult		= &tempHitResult;
	DamageInfo._HitType	= GHT_CHARA;
	DamageInfo.SetHitPart( part );
	i3Vector::Copy( &DamageInfo._vStartPos,pStart);
	i3Vector::Copy( &DamageInfo._vHitPos, &tempHitResult.m_Intersect);

	if (DamageInfo.isFireWeaponType(WEAPON_CLASS_GRENADESHELL) &&
		DamageInfo.isFireWeaponNumber( WEAPON::GSHELL_POISON , true) )
	{
		DamageInfo._DeathType		= CHARA_DEATH_POISON;
	}
	else
		DamageInfo._DeathType		= CHARA_DEATH_B;

	DamageInfo._pVecDir			= pDir;
	DamageInfo._rDistance		= tempHitResult.m_T;
	DamageInfo._nVictimIdx		= pHitUser->getCharaNetIndex();
	I3ASSERT( DamageInfo.isValid() );
	// 캐릭터의 HP 및 이펙트 처리
	pHitUser->OnHit( &DamageInfo, m_pOwner);

	if( g_pEnvSet->m_bEnableBlood )
	{
		_OnHitBloodMark( &DamageInfo._pHitResult, i3Matrix::GetPos( &mat), pDir);
		g_pFramework->resetRaycastResult();
	}
}

bool WeaponBase::_OnHit( I3_PHYSIX_HIT_RESULT * pResult, i3CollideeLine * pLine, bool bWarnOwner, bool bDamage )
{	
	I3ASSERT( pResult != nullptr );
	I3ASSERT( m_pOwner != nullptr);

	bool bRet = true;

	i3PhysixShapeSet *	pShapeSet = pResult->m_pDestShapeSet;

	if( pShapeSet == nullptr )
	{
		I3PRINTLOG(I3LOG_WARN,  "WeaponBase:_OnHit() pDestShapeSet nullptr." );
		return false;
	}

	VEC3D	vDir;
	i3Vector::Sub( &vDir, &pResult->m_Intersect, pLine->GetStart() );
	REAL32	rLength = i3Vector::Normalize( &vDir, &vDir );

	switch( pShapeSet->getShapeGroup() )
	{
	case CGRP_WORLD_ATTACK :
		{	// World에 맞았다.
			_CalcHitWorld( pResult, &vDir);
		}
		break;
	case CGRP_OBJECT_ALL :
	case CGRP_OBJECT_HIT :
	case CGRP_OBJECT_WHOLE :
	case CGRP_OBJECT_WHOLE_DYNAMIC :
		{	// Object에 맞았다.
			_CalcHitObject( pResult,pLine, &vDir, rLength, bDamage);
		}
		break;
	case CGRP_WORLD :
	case CGRP_TEAM_BLUE_ROUGH :
	case CGRP_TEAM_RED_ROUGH :
		break;

	case CGRP_ITEM_HIT:
		_CalcHitItem( pResult, &vDir);
		break;

	case CGRP_TEAM_BLUE :
	case CGRP_TEAM_RED :
		{
			_CalcHitChara( pResult, pLine, &vDir, rLength);
		}
		break;

#ifdef DOMINATION
	case CGRP_ITEM_WHOLE:
		{

		}
		break;
#endif
	}

	return bRet;
}

void WeaponBase::_OnCheckCollisionByExplosion( void)
{
	if( m_pOwner == nullptr)
		return;

	//------------------------------------------------------------------------------------------//
	//										Character Collision	
	UINT32 nColGroup = CGRP_MASKGROUP_ATTACK;
	UINT32 nSelfColGroup = nColGroup;
	if( m_pOwner->getCharaTeam() & CHARACTER_TEAM_RED)
	{
		nColGroup		= CGRP_MASKGROUP_CHARA_BLUE;
		nSelfColGroup	= CGRP_MASKGROUP_CHARA_RED;
	}
	else
	{
		nColGroup		= CGRP_MASKGROUP_CHARA_RED;
		nSelfColGroup	= CGRP_MASKGROUP_CHARA_BLUE;
	}

	INT32 i;
	VEC3D vLength;

	bool bHost = BattleServerContext::i()->IsP2PHost();

	if( BattleServerContext::i()->IsUseDedicationMode())
	{
		// Dedicated는 자신만 하면 된다. ( revision : 14380 참고 )
		if( m_pOwner->isLocal())
			bHost = true;
		else
			bHost = false;
	}

	REAL32 rExplosionRange = GetExplosionRange();

	for( i = 0; i < g_pCharaManager->getCharaCount(); ++i)
	{
		CGameCharaBase * pChara = g_pCharaManager->getChara( i);
		if( pChara == nullptr)
			continue;

		i3Vector::Sub( &vLength, GetPos(), i3Matrix::GetPos( pChara->getBoneContext()->getIKMatrix()));
		REAL32 rLength = i3Vector::Length( &vLength );
		
		if( pChara->IsRex()) rExplosionRange *= 2.0f;

		// 방장만 캐릭터 컬리젼을 하도록 합니다.
		if( (rLength < rExplosionRange))
		{
			if( bHost)
			{
				// 특수 무기
				if((m_pWeaponInfo->GetTypeClass() == WEAPON_CLASS_GRENADESHELL) &&
					((m_pWeaponInfo->GetNumber() == WEAPON::getItemIndex(WEAPON::GSHELL_POISON))))
				{
					if( (m_pOwner->getCharaTeam() != pChara->getCharaTeam()))
						_OnCheckCharaCollisionByExplosion( pChara, nColGroup, rExplosionRange);
				}
				else if ((m_pWeaponInfo->GetTypeClass() == WEAPON_CLASS_MISSION) &&
					(((m_pWeaponInfo->GetNumber() > 6) && (m_pWeaponInfo->GetNumber() < 11)))) //더미는 7번인덱스부터 시작됩니다.
				{
					//더미는 같은편을 공격하지 않도록 합니다.
					if( (m_pOwner->getCharaTeam() != pChara->getCharaTeam()))
						_OnCheckCharaCollisionByExplosion( pChara, nColGroup, rExplosionRange);
				}
				else if( (m_pOwner->getCharaTeam() != pChara->getCharaTeam()))	_OnCheckCharaCollisionByExplosion( pChara, nColGroup, rExplosionRange);
				else if( m_pOwner == pChara)								_OnCheckCharaCollisionByExplosion( pChara, nSelfColGroup, rExplosionRange);
			}
		}

		//수류탄이 터지는 근처에 Owner가 위치해있는경우 카메라 흔들림 효과
		if( (g_pCamera != nullptr) && pChara->is1PV() && !pChara->isCharaStateMask( CHARA_STATE_DEATH) )
		{	
			REAL32 MaxLength	= rExplosionRange * 2.0f;
			if( rLength < MaxLength)
			{
				REAL32 TrembAmpRate = 1.0f - rLength/MaxLength;
				g_pCamera->SetTremble( 15.0f * TrembAmpRate * TrembAmpRate, 10.0f * TrembAmpRate, 0.2f, 1.0f *	TrembAmpRate);
			}
		}
	}

	if( BattleServerContext::i()->IsP2PHost() )
	{
		// 수류탄으로 죽은 캐릭터를 우선적으로 보내줍니다.
		UserContext::i()->SendDeathChara();
	}

	//	Object Collision
	_CheckExplosionObjectCollision( GetExplosionRange());
}

bool WeaponBase::_OnCheckCharaCollisionByExplosion( CGameCharaBase * pChara, UINT32 nColGroup, REAL32 rRange)
{
	bool					bValidCol = false;
	i3CollideeLine			ColLine;
	MATRIX *				pMtx;
	bool					bCharaAlive = !pChara->isCharaStateMask( CHARA_STATE_DEATH) ;
	INT32					nCurHP	= pChara->getCurHP();

	ColLine.SetStart( GetPos());

	addY( ColLine.GetStart(), 0.1f);


	if( bCharaAlive )
	{
		if(pChara->IsDino()) {
			pMtx	= pChara->getBoneContext()->getIKMatrix();
		}
		else {
			pMtx	= pChara->getBoneContext()->getHeadMatrix();
		}
		
		ColLine.SetEnd( i3Matrix::GetPos( pMtx));

		//World에 대해서 한번 테스트
		// 월드 컬리젼 거리보다 작으면.
		if(  _CheckHitWorld( ColLine, rRange ) )
		{
			_OnHitCharaByExplosion( pChara, CHARA_HIT_HEAD, ColLine.GetDir(),ColLine.GetStart() );
			bValidCol = true;
		}
	}

	// 계속 살아 있던가 아니면 죽어 있는 경우 처리
	if( bCharaAlive == !pChara->isCharaStateMask( CHARA_STATE_DEATH) )
	{
		pMtx = pChara->getBoneContext()->getIKMatrix();
		ColLine.SetEnd( i3Matrix::GetPos( pMtx));
		
		//World에 대해서 한번 테스트
		if( _CheckHitWorld( ColLine, rRange ) )
		{
			_OnHitCharaByExplosion( pChara, CHARA_HIT_ROOT, ColLine.GetDir(),ColLine.GetStart());
			bValidCol = true;
		}
	}

	if( bValidCol && pChara->isLocal() && (nCurHP > pChara->getCurHP()))
	{	// Hit only Host
		g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_THROW, 0, GetPos(), true);
	}

	return bValidCol;
}

void WeaponBase::CalcHitDir( I3_PHYSIX_HIT_RESULT * pHitResult, VEC3D * pPos, VEC3D * pOutDir)
{
	I3ASSERT( pHitResult != nullptr && pPos != nullptr && pOutDir != nullptr);

	VEC3D shapePos;
	i3PhysixShapeSet*		pDestShapeSet = pHitResult->m_pDestShapeSet;
	I3ASSERT( pDestShapeSet != nullptr);
	
	*((NxVec3 *) &shapePos) = pDestShapeSet->getNxActor()->getGlobalPosition();

	i3Vector::Sub( pOutDir, &shapePos, pPos );
	i3Vector::Normalize( pOutDir, pOutDir);
}

VEC3D* WeaponBase::GetBulletTraceStartPos()
{
	if(getFXMatrix() != nullptr)
		return i3Matrix::GetPos( getFXMatrix());
	return nullptr;
}
VEC3D* WeaponBase::GetSinoBulletTraceStartPos()
{
	return  i3Matrix::GetPos(((CGameCharaDino*)m_pOwner)->getFXMatrix());
}


void WeaponBase::AddBulletTrace(I3_PHYSIX_HIT_RESULT* pBulletHit)
{
	if( g_pTracerBulletManager == nullptr)
		return;

	TracerBulletInfo bulletInfo;
	CWeaponGlobal * pGlobalInfo = g_pWeaponInfoDataBase->getGlobal();
	I3ASSERT( pGlobalInfo != nullptr);

	CWeaponInfo * pWeaponInfo = getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);

	{	// 예광탄 궤적
		bulletInfo._Type = TRACER_TYPE_BULLET;
		//스팅:궤적
		if( (m_pOwner != nullptr) && (m_pOwner->IsSting() || m_pOwner->IsCCSting() || m_pOwner->IsStingMars()))
			i3Vector::Copy( &bulletInfo._vBeginPos, GetSinoBulletTraceStartPos());
	#ifdef DOMI_DINO
		else if ( (m_pOwner != nullptr) && ( m_pOwner->IsDomiSting() || m_pOwner->IsDomiStingMars() ) )
		{
			VEC3D* rp = i3Matrix::GetPos(static_cast<CGameCharaDomiSting*>(m_pOwner)->GetMatWeaponReleasePoint());
			i3Vector::Copy(&bulletInfo._vBeginPos, rp);
		}
	#endif
		else
		{
			VEC3D* pPos = GetBulletTraceStartPos();

			if(pPos != nullptr)
				i3Vector::Copy(&bulletInfo._vBeginPos, GetBulletTraceStartPos());
			else
			{
				i3::rc_wstring wstrWeaponName;
				pWeaponInfo->GetWeaponName(wstrWeaponName);
				I3PRINTLOG(I3LOG_NOTICE, "GetBulletTraceStartPos == nullptr %s", wstrWeaponName);
			}
		}

		i3Vector::Copy( &bulletInfo._vDestPos, m_TargetPos);
		bulletInfo._Length = pGlobalInfo->m_rTracerBulletLength;
		bulletInfo._Thickness = pGlobalInfo->m_rTracerBulletThickness;
		bulletInfo._EndSpeed = pGlobalInfo->m_rTracerBulletSpeed;
		i3Color::Set( &bulletInfo._color, (UINT8) 255, 255, 255, 255);
	}

	TracerBulletInfo bulletSmokeInfo;

	{// 탄연기 궤적
		bulletSmokeInfo._Type = TRACER_TYPE_BULLET_SMOKE;

		i3Vector::Copy( &bulletSmokeInfo._vBeginPos, &bulletInfo._vBeginPos);
		i3Vector::Copy( &bulletSmokeInfo._vDestPos, m_TargetPos);

		// 탄연기의 속성은 무기에서 얻어온다.
		pWeaponInfo->SetBulletSmokeInfo(&bulletSmokeInfo);
	}

	bool bEnable = false;
	if( (i3Math::Rand() % 100) < pGlobalInfo->m_iTracerBulletRate)
		bEnable = true;

	if( getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_SNIPER || getWeaponInfo()->GetTypeClass() == WEAPON_SUBCLASS_BOW)
		bEnable = true;	// 스나이퍼 총은 100% 나오게

	if( getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_DINO)
	{
		if( getWeaponInfo()->GetTypeFire() == WEAPON::FIRE_RANGE)
		{
			bEnable = true;
		}
	}

	if( bEnable)
	{
		if( pBulletHit != nullptr && pBulletHit->m_pDestShapeSet != nullptr )
		{
			VEC3D * pIntersect = &pBulletHit->m_Intersect;

			//	예광탄
			i3Vector::Copy( &bulletInfo._vDestPos, pIntersect);
			i3Vector::Copy( &bulletSmokeInfo._vDestPos, pIntersect);
		}

		//	예광탄
		g_pTracerBulletManager->Fire(&bulletInfo);

		if(pWeaponInfo->GetBulletSmokeLifeTime() > 0.0f)
		{	//	탄 연기
			g_pTracerBulletManager->Fire(&bulletSmokeInfo);
		}
	}
}

void WeaponBase::DoLineCollision( VEC3D * pStart, VEC3D * pEnd, INT32 idxGroup)
{
	bool						bWarningOwner = false;
	I3_PHYSIX_HIT_RESULT *		pBulletHit;
	
	I3ASSERT( pStart != nullptr);
	I3ASSERT( pEnd != nullptr);

	m_CollideeLine.SetStart( pStart);
	m_CollideeLine.SetEnd( pEnd);
	//Sting의 TargetPos은 받아온걸로 셋팅합니다.
	if(m_pOwner->IsSting() || m_pOwner->IsCCSting() || m_pOwner->IsStingMars() ||
		m_pOwner->IsDomiSting() || m_pOwner->IsDomiStingMars() )
	{
		i3Vector::Copy( m_TargetPos, pEnd);
	}

	pBulletHit = DoLineCollision2( &m_CollideeLine, idxGroup, &bWarningOwner);

	AddBulletTrace(pBulletHit);
}

void WeaponBase::_FireEffect( MATRIX * pStart, INT32 idxEffect, bool bAttachWorld)
{
	MATRIX * pMtx = pStart;
	
	if( pMtx == nullptr )
		return;
	
	
	// Light
	if( g_pEnvSet->m_bDynamicLight)
	{
		COLORREAL col;

		i3Color::Set( &col, 1.4f, 1.3f, 1.2f, 1.0f);
		m_pLight->SetDiffuse( &col);

		m_pLight->SetSpecular( &col);
		m_pLight->SetLightType( I3G_LIGHT_POINT);
		m_pLight->setRange( 3.0f);

		if( m_pLight->isState( I3G_LIGHT_STATE_DISABLE))
		{
			g_pFramework->addLight( m_pLight);
		}

		m_pLight->removeState( I3G_LIGHT_STATE_DISABLE);
		m_pLight->SetWorldSpaceTransform( pMtx);
		m_timeLight = 0.0f;
	}

	VEC3D Pos;
	VEC3D norm;

	i3Vector::Copy( &norm, i3Matrix::GetUp(pMtx));
	i3Vector::Copy( &Pos, i3Matrix::GetPos(pMtx));

	if( bAttachWorld)
	{
		pMtx = nullptr;
	}

	i3TimeStamp * pEffect = g_pEffectManager->AddEffect( idxEffect, &Pos, &norm, pMtx);
	

	// Effect가 살아있는 동안, 해당 무기를 메모리 해제하지 않도록 하기 위한 조치
	if( pEffect != nullptr)
	{
		pEffect->setRefObject( getSceneObject()->GetBody());
	
		SetColorFireEffect(pEffect); // ColorFireEffect (ColorFireMuzzleFlash)
	}
}

void WeaponBase::_IdleEffect(REAL32 rDeltaSeconds)
{
	if ( m_pOwner == nullptr)
		return;

	if (!m_pWeaponInfo->hasIdleEffect())
		return;

	if (!g_pFramework->IsBattleStage())
		return;
	
	if (!isAttached() || IsDroped())
		return;

	if ( m_pOwner->getWeaponContext()->getWeapon(WEAPON_SLOT_PRIMARY) != nullptr
		&& MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
	{
		if ( !m_pOwner->getWeaponContext()->getWeapon(WEAPON_SLOT_PRIMARY)->getWeaponInfo()->hasIdleEffect())
			return;
	}
	
	//업데이트에서 Owner, sceneObject, WeaponInfo가 사라지는 경우가 있음
	I3ASSERT(m_pBody != nullptr);
	I3ASSERT(m_pWeaponInfo != nullptr);
	
	//줌했을때 이펙트 생성 중단
	if (isWeaponStateMask(WSM_ZOOM))
		return;

	//이펙트 루프 타임 계산
	const REAL32 rLoopTime = m_pWeaponInfo->GetIdleEffectLoopTime();
	m_rLastIdleEffectTime += rDeltaSeconds;

	if (rLoopTime < m_rLastIdleEffectTime)
	{
		//Gundummy의 행렬을 받음
		MATRIX* pMtx = nullptr;
		if (m_idxFXIdleBone != -1)
			pMtx = m_pBody->getCacheMatrix(m_idxFXIdleBone);
		if (pMtx == nullptr)
			return;

		VEC3D Pos;
		VEC3D norm;

		i3Vector::Copy(&norm, i3Matrix::GetUp(pMtx));
		i3Vector::Copy(&Pos, i3Matrix::GetPos(pMtx));

		i3TimeStamp* pEffect = g_pEffectManager->AddEffect(IdleEffIdx, &Pos, &norm, pMtx);

		// Effect가 살아있는 동안, 해당 무기를 메모리 해제하지 않도록 하기 위한 조치
		if (pEffect != nullptr)
		{
			pEffect->setRefObject(m_pBody);
		}
		m_rLastIdleEffectTime = 0.0f;
	}

}

void WeaponBase::_RemoveIdleEffect()
{
	if (m_pWeaponInfo->hasIdleEffect())
	{
		if (m_pBody == nullptr)
			return;

		i3EffectTypeInfo* effectType = g_pEffectManager->getEffectType(IdleEffIdx);
		i3TimeStamp* pChild = nullptr;

		for (int i = 0; i < effectType->getRoot()->GetChildCount(); i++)
		{
			pChild = (i3TimeStamp*)effectType->getRoot()->GetChild(i);

			//내 무기에서 나온 이펙트이면 삭제
			if (pChild->getRefObject() == m_pBody)
			{
				WEAPON_EFFECT_FORCE_DELETE(pChild);
				//pChild->RemoveFromParent();
			}
		}
	}
}

void WeaponBase::SetColorFireEffect(i3TimeStamp * pEffect)
{

	for (int i = 0; i < pEffect->GetChildCount(); ++i)
	{
		i3Node *pNode = pEffect->GetChild(i);
		for (int j = 0; j < pNode->GetChildCount(); ++j)
		{
			if (i3::same_of<i3Particle*>(pNode->GetChild(j)))
			{
				i3Particle* pParticle = static_cast<i3Particle*>(pNode->GetChild(j));
				
				i3ParticleInfo* pCustomInfo =	pParticle->GetCustomParticleInfo();

				if ( m_idxColorForFireEffect < 1 )
				{
					if (pCustomInfo != nullptr)
					{
						pParticle->EnableCustomParticleInfo(false);
					}
				}
				else
				{
					if (pCustomInfo == nullptr)
					{
						pCustomInfo = i3ParticleInfo::new_object_ref();
						pParticle->SetCustomParticleInfo(pCustomInfo);
					
						i3ParticleInfo* origInfo = pParticle->GetOrigParticleInfo();

						origInfo->CopyTo(pCustomInfo, I3_COPY_INSTANCE);
					}

					pParticle->EnableCustomParticleInfo(true);

					I3COLOR* pColor = const_cast<I3COLOR*>(GetColorbyIdx(0, m_idxColorForFireEffect));

					pCustomInfo->SetColorStyle(0);
					// 소음기 이펙트는 FUNC_CONSTANT였기 때문에 색상 설정이 안됨

					if (pCustomInfo->GetRGBFunc() != i3ParticleInfo::FUNC_TABLE)
					{
						pCustomInfo->SetRGBFunc(i3ParticleInfo::FUNC_TABLE);
					}

					if (pCustomInfo->GetAlphaTable() != nullptr &&
						pCustomInfo->GetRTable() != nullptr  && 
						pCustomInfo->GetGTable() != nullptr &&
						pCustomInfo->GetBTable() != nullptr)
					{
						*pCustomInfo->GetRTable()->GetTable() = pColor->r;
						*pCustomInfo->GetGTable()->GetTable() = pColor->g;
						*pCustomInfo->GetBTable()->GetTable() = pColor->b;
						*pCustomInfo->GetAlphaTable()->GetTable() = pColor->a;
					}

				}
			}
		}
	} // Color Setting END

	

}

void WeaponBase::_RemoveLight(void)
{
	if( m_pLight != nullptr && (m_pLight->isState( I3G_LIGHT_STATE_DISABLE) == false) )
	{
		m_pLight->addState( I3G_LIGHT_STATE_DISABLE);
		g_pFramework->removeLight( m_pLight);
	}
}

void WeaponBase::_EmptyCartridgeEffect( INT32 mtxIdx, INT32 effectIdx)
{
	// 탄피 Effect
	if( (mtxIdx != -1) && ( effectIdx != -1))
	{
		MATRIX * pMtx = getSceneObject()->GetBody()->getCacheMatrix( mtxIdx);
		VEC3D norm = VEC3D( 1.0f, 0.0f, 0.0f );

		if( m_pOwner->isLeftHand())
			i3Vector::Scale( &norm, i3Matrix::GetUp(pMtx), -1.0f);
		else
			i3Vector::Copy( &norm, i3Matrix::GetUp(pMtx));

		g_pEffectManager->AddEffect( effectIdx, i3Matrix::GetPos(pMtx), &norm, (MATRIX *) nullptr);
	}
}

void WeaponBase::_PlayEmptyCartridgeSound( I3_TERRAIN_TYPE nTerrainType, VEC3D * pPos )
{
	if( (m_pOwner != nullptr) && getWeaponInfo()->isGun())
	{
		switch( getWeaponInfo()->GetTypeClass() )
		{
		case WEAPON_CLASS_SHOTGUN		:
		case WEAPON_SUBCLASS_SHOTGUN	:
		case WEAPON_CLASS_DUALSHOTGUN	:
			{
				g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, nTerrainType, GTBT_WPN_SHOTGUN_EMPTYCATRIDGE, pPos, m_pOwner->is1PV());
			}
			break;
		default	:
			{
				g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, nTerrainType, GTBT_WPN_EMPTYCATRIDGE_1, pPos, m_pOwner->is1PV());
			}
			break;
		}
	}
}

void WeaponBase::_OnPlayTerrainHitSound( WEAPON::FIRE_ORDER fireOrder, UINT32 type, VEC3D * pPos)
{
	//Hit Sound
	if(m_pWeaponInfo->GetLoadBulletType() == 1)
	{
		g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, GTBT_WATERGUN_HIT, pPos, false);
		return;
	}
	//Hit Sound
	switch( m_pWeaponInfo->GetTypeClass() )
	{
	case WEAPON_SUBCLASS_BOW:
	case WEAPON_CLASS_KNIFE :
		{
			WEAPON::RES_ID weaponNum = m_pWeaponInfo->GetRealNumber();
			if( weaponNum == WEAPON::getItemIndex( WEAPON::CANDYCANE))
			{
				//캔디케인은 이펙트 사운드에 의존합니다.
			}
			else if( weaponNum == WEAPON::getItemIndex(WEAPON::GH5007))
			{
				//	GH5007는 두번 컬리전되어 두번 플레이되는 것을 피하기위해 첫번째 컬리전 될때만 나오도록한다.
				if( fireOrder == WEAPON::SECONDARY_FIRE)
				{
					if( getAutoFireCount() == 1)
						g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, GTBT_WPN_GH5007_SEC1_HIT, pPos, false);
					else
						g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, GTBT_WPN_GH5007_SEC2_HIT, pPos, false);
				}
				else 
					g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, GTBT_WPN_MELEE_HIT, pPos, false);
			}
			else if( weaponNum == WEAPON::getItemIndex(WEAPON::FRYINGPAN)
				|| weaponNum == WEAPON::getItemIndex(WEAPON::FRYINGPAN_DUMMY)
				|| weaponNum == WEAPON::getItemIndex(WEAPON::HAIRDRYER)
				|| weaponNum == WEAPON::getItemIndex(WEAPON::HAIRDRYER_DUMMY) )
			{
				g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, GTBT_SUMMEREVENT_WEAPON_HIT, pPos, false);
			}
			else
			{
				INT32 idx = m_pWeaponInfo->GetTerrainHitSoundIndex();
				if( idx > -1)
					g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, idx, pPos, false);
				else
					g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, GTBT_WPN_MELEE_HIT, pPos, false);
			}
		}
		break;
	case WEAPON_CLASS_KNUCKLE :
		{
			INT32 idx = m_pWeaponInfo->GetTerrainHitSoundIndex();
			if( idx > -1)
				g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, idx, pPos, false);
			else
				g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, GTBT_WPN_BONEKNIFE_HIT, pPos, false);
		}
		break;
	case WEAPON_CLASS_DUALKNIFE :
		{
			INT32 idx = m_pWeaponInfo->GetTerrainHitSoundIndex();
			if( idx > -1)
				g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, idx, pPos, false);
			else
				g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, GTBT_WPN_MELEE_HIT, pPos, false);
		}
		break;

	case WEAPON_SUBCLASS_SHOTGUN	:
	case WEAPON_CLASS_SHOTGUN		:
	case WEAPON_CLASS_DUALSHOTGUN	:
		g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, GTBT_WPN_SHOTGUN_HIT_1, pPos, false);
		break;
	case WEAPON_CLASS_DINO:
	case WEAPON_CLASS_DINO_LEFT_SCRATCH:
	case WEAPON_CLASS_ELITE_LEFT_SCRATCH:
	case WEAPON_CLASS_ELITE_RIGHT_SCRATCH:
		g_pSndMng->ReserveSoundPlay( IT_DINO_TERRAIN_TYPE, type, GTBT_WPN_DINO_HIT, pPos, false);
		break;
	default :
		g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, type, GTBT_WPN_HIT_1 + i3Math::Rand() % 2, pPos, false);
		break;
	}
}

void WeaponBase::_AddEmptyCartridgeSound( I3_TERRAIN_TYPE nTerrainType, REAL32 delay, VEC3D * pPos )
{
	while ( m_listEmptyBulletSound.size() >= MAX_EMPTYBULLETSOUND_COUNT)		// 사이즈가 넘치면 앞에걸 강제로 비운다.
	{
		delete m_listEmptyBulletSound.front();
		m_listEmptyBulletSound.pop_front();
	}

	if( delay < I3_EPS)	delay = I3_EPS;

	WEAPON_EMPTYBULLET_SOUND* new_obj = new WEAPON_EMPTYBULLET_SOUND(WEAPON::PRIMARY_FIRE, nTerrainType, delay, *pPos);
	m_listEmptyBulletSound.push_back(new_obj);
}

void WeaponBase::_AddTerrainEffectSound( I3_TERRAIN_TYPE nTerrainType, REAL32 delay, VEC3D * pPos)
{
	while ( m_listTerrainSound.size() >= MAX_TERRAINSOUND_COUNT)		// 사이즈가 넘치면 앞에걸 강제로 비운다.
	{
		delete m_listTerrainSound.front();
		m_listTerrainSound.pop_front();
	}


	switch( m_pWeaponInfo->GetTypeClass() )
	{
	case WEAPON_CLASS_SHOTGUN		:
	case WEAPON_SUBCLASS_SHOTGUN	:
	case WEAPON_CLASS_DUALSHOTGUN	:	delay = 0.f;	break;
	default							:	break;
	}

	if( delay < I3_EPS)	delay = I3_EPS;

	WEAPON_TERRAIN_SOUND* new_obj = new WEAPON_TERRAIN_SOUND( getFireOrder(), nTerrainType, delay, *pPos);
	m_listTerrainSound.push_back(new_obj);

}

void WeaponBase::ProcessSound( REAL32 tm )
{
	typedef i3::list<WEAPON_EMPTYBULLET_SOUND*>::iterator   iterator1;
	typedef i3::list<WEAPON_TERRAIN_SOUND*>::iterator		iterator2;

	for ( iterator1 it = m_listEmptyBulletSound.begin(), it_end = m_listEmptyBulletSound.end() ; it != it_end ; )
	{
		WEAPON_EMPTYBULLET_SOUND* obj = *it;

		if ( obj == nullptr)
		{
			++it;
			continue;
		}

		obj->_rDelay -= tm;
		if( obj->_rDelay <= 0.f )
		{
			_PlayEmptyCartridgeSound( obj->_nTerrainType, &obj->_vPos);
			delete obj;
			it = m_listEmptyBulletSound.erase(it);
		}
		else 
		{
			++it;
		}
	}

	for ( iterator2 it = m_listTerrainSound.begin(), it_end = m_listTerrainSound.end() ; it != it_end ; )
	{
		WEAPON_TERRAIN_SOUND* obj = *it;

		if (obj == nullptr)
		{
			++it;
			continue;
		}

		obj->_rDelay -= tm;
		if( obj->_rDelay <= 0.f )
		{
			_OnPlayTerrainHitSound( obj->_nFireOrder, obj->_nTerrainType, &obj->_vPos );
			delete obj;
			it = m_listTerrainSound.erase(it);
		}
		else 
		{
			++it;
		}
	}

}

void WeaponBase::ResetSound()
{
	i3::cu::for_each_delete_clear(m_listEmptyBulletSound);
	i3::cu::for_each_delete_clear(m_listTerrainSound);
}

void WeaponBase::OnChangeFPSView(void)
{
	m_pBody->removeStyle(I3BS_LOD_AUTO);
	m_pBody->setCurrentLevel(0);

	// 그림자를 끕니다.
	m_pBody->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER );

	// 알파가 있는 총기인 경우 알파를 켜줍니다.
	OnSetBlendAlpha( 1.f);

	SetOwner( m_pOwner);

	m_mainWeapon->OnChangeFPSView();
}

void WeaponBase::OnChangeObserverView(void)
{
	if( (m_pOwner != nullptr) && !m_pOwner->isProfile() )
		m_pBody->addStyle(I3BS_LOD_AUTO);

	// 그림자가 나오도록 합니다.
	m_pBody->RemoveFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER );

	// 알파가 있던 총기인 경우 알파를 꺼줍니다.
	OnSetBlendAlpha( 1.f);

	SetOwner( m_pOwner);

	m_mainWeapon->OnChangeObserverView();
}

void WeaponBase::OnBindResource(void)
{
	i3Chara::OnBindResource();

	m_pAICtx = this->FindAI( "Default");

	if( m_pAICtx == nullptr)
		return;

	i3AnimationResManager * pManager = static_cast<i3AnimationResManager*>(i3NamedResourceManager::FindByMeta( i3AnimationResManager::static_meta()));

	for( INT32 i = 0; i < m_pAICtx->getAIStateCount(); i++)
	{
		i3AIState * pAIState = m_pAICtx->getAIState( i);
		if( pAIState->getAnim() == nullptr )
		{
			if( pAIState->getAnimName()[0] != 0)
			{
				i3Animation * pAnim = (i3Animation*) pManager->GetResource( nullptr, nullptr, pAIState->getAnimName());
				if( pAnim != nullptr)
				{
					pAIState->SetAnim( pAnim);
				}
				else
				{
					g_pCharaManager->LoadWeaponAnimation( pAIState, pAIState->getAnimName());
				}
			}
		}
	}

	setAIIndex( WEAPON::AI_FIRE1PV, (INT8) m_pAICtx->FindAIStateByName( "Fire1PV"));
	setAIIndex( WEAPON::AI_FIRE3PV, (INT8) m_pAICtx->FindAIStateByName( "Fire3PV"));

	setAIIndex( WEAPON::AI_FIRE, (INT8) m_pAICtx->FindAIStateByName( "Fire"));

	//if( getAIIndex( WEAPON::AI_FIRE1PV) == -1 && getAIIndex( WEAPON::AI_FIRE3PV) == -1)
	//	I3ASSERT( getAIIndex( WEAPON::AI_FIRE) != -1);	

	setAIIndex( WEAPON::AI_SECFIRE, (INT8) m_pAICtx->FindAIStateByName( "SecondaryFire"));

	setAIIndex( WEAPON::AI_SECFIRE3PV, (INT8) m_pAICtx->FindAIStateByName( "SecondaryFire_3PV"));
	
	if( getAIIndex( WEAPON::AI_SECFIRE) == -1)
		setAIIndex( WEAPON::AI_SECFIRE, getAIIndex( WEAPON::AI_FIRE));
	
	setAIIndex( WEAPON::AI_LOADMAGAZINE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag"));

	setAIIndex( WEAPON::AI_LOADBULLET, (INT8) m_pAICtx->FindAIStateByName( "LoadBullet"));
	setAIIndex( WEAPON::AI_LOADBULLET1PV, (INT8) m_pAICtx->FindAIStateByName( "LoadBullet1PV"));
	setAIIndex( WEAPON::AI_LOADBULLET3PV, (INT8) m_pAICtx->FindAIStateByName( "LoadBullet3PV"));

	setAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER1PV, (INT8) m_pAICtx->FindAIStateByName( "LoadMagMaster1PV"));
	setAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER3PV, (INT8) m_pAICtx->FindAIStateByName( "LoadMagMaster3PV"));
	setAIIndex( WEAPON::AI_LOADMAGAZINE_RIGHT_MASTER1PV, (INT8) m_pAICtx->FindAIStateByName( "LoadMag_RightAllMaster1PV"));
	setAIIndex( WEAPON::AI_LOADMAGAZINE_RIGHT_MASTER3PV, (INT8) m_pAICtx->FindAIStateByName( "LoadMag_RightAllMaster3PV"));

	setAIIndex( WEAPON::AI_LOADMAGAZINE_1PV, (INT8) m_pAICtx->FindAIStateByName( "LoadMag1PV"));
	setAIIndex( WEAPON::AI_LOADMAGAZINE_3PV, (INT8) m_pAICtx->FindAIStateByName( "LoadMag3PV"));

	setAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL, (INT8) m_pAICtx->FindAIStateByName( "LoadMag_RightAll"));
	setAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_1PV_MALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag_RightAll_1PV_MALE"));
	setAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_1PV_FEMALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag_RightAll_1PV_FEMALE"));
	setAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_3PV_MALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag_RightAll_3PV_MALE"));
	setAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_3PV_FEMALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag_RightAll_3PV_FEMALE"));

	setAIIndex( WEAPON::AI_IDLE1PV, (INT8) m_pAICtx->FindAIStateByName( "Idle1PV"));
	setAIIndex( WEAPON::AI_IDLE3PV, (INT8) m_pAICtx->FindAIStateByName( "Idle3PV"));

	setAIIndex( WEAPON::AI_IDLE, (INT8) m_pAICtx->FindAIStateByName( "Idle"));

	// IDLE 이름으로 없는 무기도 있음 (Brass Knuckle)
	//if( getAIIndex( WEAPON::AI_IDLE1PV) == -1 || getAIIndex( WEAPON::AI_IDLE3PV) == -1)
	//	I3ASSERT( getAIIndex( WEAPON::AI_IDLE) != -1);

	// Dual 무기
	setAIIndex( WEAPON::AI_LOADMAGAZINEDUAL, (INT8) m_pAICtx->FindAIStateByName( "LoadMagDualGun"));
	setAIIndex( WEAPON::AI_LOADMAGAZINEDUAL1PV_LEFT, (INT8) m_pAICtx->FindAIStateByName( "LoadMag1PV_DualGun_Left"));
	setAIIndex( WEAPON::AI_LOADMAGAZINEDUAL1PV_RIGHT, (INT8) m_pAICtx->FindAIStateByName( "LoadMag1PV_DualGun_Right"));
	setAIIndex( WEAPON::AI_LOADMAGAZINEDUAL3PV_LEFT, (INT8) m_pAICtx->FindAIStateByName( "LoadMag3PV_DualGun_Left"));
	setAIIndex( WEAPON::AI_LOADMAGAZINEDUAL3PV_RIGHT, (INT8) m_pAICtx->FindAIStateByName( "LoadMag3PV_DualGun_Right"));

	setAIIndex( WEAPON::AI_CHANGEDUAL, (INT8) m_pAICtx->FindAIStateByName( "ChangeDualGun"));
	setAIIndex( WEAPON::AI_CHANGEDUAL1PV_LEFT, (INT8) m_pAICtx->FindAIStateByName( "Change1PV_DualGun_Left"));
	setAIIndex( WEAPON::AI_CHANGEDUAL1PV_RIGHT, (INT8) m_pAICtx->FindAIStateByName( "Change1PV_DualGun_Right"));
	setAIIndex( WEAPON::AI_CHANGEDUAL3PV_LEFT, (INT8) m_pAICtx->FindAIStateByName( "Change3PV_DualGun_Left"));
	setAIIndex( WEAPON::AI_CHANGEDUAL3PV_RIGHT, (INT8) m_pAICtx->FindAIStateByName( "Change3PV_DualGun_Right"));



	setAIIndex( WEAPON::AI_LOADMAGAZINEDUAL1PV_SWAP,   (INT8) m_pAICtx->FindAIStateByName("LoadMag1PVSwapShell"));
	setAIIndex( WEAPON::AI_LOADMAGAZINEDUAL3PV_SWAP,   (INT8) m_pAICtx->FindAIStateByName("LoadMag3PVSwapShell"));
	setAIIndex( WEAPON::AI_LOADMAGAZINEDUAL1PV_RELOAD, (INT8) m_pAICtx->FindAIStateByName("LoadMag1PVReloadShell"));
	setAIIndex( WEAPON::AI_LOADMAGAZINEDUAL3PV_RELOAD, (INT8) m_pAICtx->FindAIStateByName("LoadMag3PVReloadShell"));

	setAIIndex( WEAPON::AI_EXTENSION_ACTIVATE, (INT8) m_pAICtx->FindAIStateByName( "ExtensionActivate"));
	setAIIndex( WEAPON::AI_EXTENSION_DEACTIVATE, (INT8) m_pAICtx->FindAIStateByName( "ExtensionDeactivate"));

	
	{
		// Extension Activate weapons
		setAIIndex( WEAPON::AI_IDLE_EXTENSION, (INT8) m_pAICtx->FindAIStateByName( "Idle_Ext"));
		setAIIndex( WEAPON::AI_FIRE_EXTENSION, (INT8) m_pAICtx->FindAIStateByName( "Fire_Ext"));
		setAIIndex( WEAPON::AI_LOADBULLET_EXTENSION, (INT8) m_pAICtx->FindAIStateByName( "LoadBullet_Ext"));
		setAIIndex( WEAPON::AI_LOADMAGAZINE_EXTENSION, (INT8) m_pAICtx->FindAIStateByName( "LoadMag_Ext"));
		
		setAIIndex( WEAPON::AI_LOADMAGAZINE1PV_EXTENSION, (INT8) m_pAICtx->FindAIStateByName( "LoadMag1PV_Ext"));
		setAIIndex( WEAPON::AI_LOADMAGAZINE3PV_EXTENSION, (INT8) m_pAICtx->FindAIStateByName( "LoadMag3PV_Ext"));

		setAIIndex( WEAPON::AI_EXTENSION_ACTIVATE3PV, (INT8) m_pAICtx->FindAIStateByName( "ExtensionActivate3PV"));
		setAIIndex( WEAPON::AI_EXTENSION_DEACTIVATE3PV, (INT8) m_pAICtx->FindAIStateByName( "ExtensionDeactivate3PV"));
		

	}

	// MachineGun
	{
		setAIIndex( WEAPON::AI_MOVE, (INT8) m_pAICtx->FindAIStateByName( "Move"));
		setAIIndex( WEAPON::AI_CHANGE, (INT8) m_pAICtx->FindAIStateByName( "Change"));
		setAIIndex( WEAPON::AI_CHANGE1PV, (INT8) m_pAICtx->FindAIStateByName( "Change1PV"));
		setAIIndex( WEAPON::AI_CHANGE3PV, (INT8) m_pAICtx->FindAIStateByName( "Change3PV"));
		setAIIndex( WEAPON::AI_DAMAGE, (INT8) m_pAICtx->FindAIStateByName( "Damage"));
		setAIIndex( WEAPON::AI_JUMPEND, (INT8) m_pAICtx->FindAIStateByName( "JumpEnd"));
	}

	// Folded weapon
	{
		setAIIndex( WEAPON::AI_IDLE_FOLDED, (INT8) m_pAICtx->FindAIStateByName( "Idle_Folded"));
		setAIIndex( WEAPON::AI_FIRE_FOLDED, (INT8) m_pAICtx->FindAIStateByName( "Fire_Folded"));
		setAIIndex( WEAPON::AI_IDLE_FOLDED_CHANGE, (INT8) m_pAICtx->FindAIStateByName( "Idle_Folded_Change"));
	}



	// 남녀캐릭구분
	{
		//일반로드
		{
			//남
			setAIIndex( WEAPON::AI_LOADMAGAZINE_1PV_MALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag1PVMale"));
			setAIIndex( WEAPON::AI_LOADMAGAZINE_3PV_MALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag3PVMale"));
			//여
			setAIIndex( WEAPON::AI_LOADMAGAZINE_1PV_FEMALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag1PVFemale"));
			setAIIndex( WEAPON::AI_LOADMAGAZINE_3PV_FEMALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag3PVFemale"));
		}
		//익스텐션 상태 로드
		{
			//남
			setAIIndex( WEAPON::AI_LOADMAGAZINE1PV_EXTENSION_MALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag1PV_Ext_Male"));
			setAIIndex( WEAPON::AI_LOADMAGAZINE3PV_EXTENSION_MALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag3PV_Ext_Male"));
			//여
			setAIIndex( WEAPON::AI_LOADMAGAZINE1PV_EXTENSION_FEMALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag1PV_Ext_Female"));
			setAIIndex( WEAPON::AI_LOADMAGAZINE3PV_EXTENSION_FEMALE, (INT8) m_pAICtx->FindAIStateByName( "LoadMag3PV_Ext_Female"));

		}
		
		//익스텐션 리로드 마스터
		{
			//남
			setAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER1PV_EXT, (INT8) m_pAICtx->FindAIStateByName( "LoadMagMaster1PV_Ext"));
			setAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER3PV_EXT, (INT8) m_pAICtx->FindAIStateByName( "LoadMagMaster3PV_Ext"));
		
		}
		//로드뷸렛
		{
			//남
			setAIIndex( WEAPON::AI_LOADBULLET1PV_MALE, (INT8) m_pAICtx->FindAIStateByName( "LoadBullet1PVMale"));
			setAIIndex( WEAPON::AI_LOADBULLET3PV_MALE, (INT8) m_pAICtx->FindAIStateByName( "LoadBullet3PVMale"));
			//여
			setAIIndex( WEAPON::AI_LOADBULLET1PV_FEMALE, (INT8) m_pAICtx->FindAIStateByName( "LoadBullet1PVFemale"));
			setAIIndex( WEAPON::AI_LOADBULLET3PV_FEMALE, (INT8) m_pAICtx->FindAIStateByName( "LoadBullet3PVFemale"));
		}
		// 발사후 볼트액션
		{
			//남
			setAIIndex( WEAPON::AI_FIREBULLET1PV_MALE, (INT8) m_pAICtx->FindAIStateByName( "FireBullet1PVMale"));
			setAIIndex( WEAPON::AI_FIREBULLET3PV_MALE, (INT8) m_pAICtx->FindAIStateByName( "FireBullet3PVMale"));
			//여
			setAIIndex( WEAPON::AI_FIREBULLET1PV_FEMALE, (INT8) m_pAICtx->FindAIStateByName( "FireBullet1PVFemale"));
			setAIIndex( WEAPON::AI_FIREBULLET3PV_FEMALE, (INT8) m_pAICtx->FindAIStateByName( "FireBullet3PVFemale"));

		}
		//공격준비 상태
		{
			setAIIndex( WEAPON::AI_ATTACKREADY_1PV, (INT8) m_pAICtx->FindAIStateByName( "AttackReady1PV"));
			setAIIndex( WEAPON::AI_ATTACKREADY_3PV, (INT8) m_pAICtx->FindAIStateByName( "AttackReady3PV"));

			setAIIndex( WEAPON::AI_GRENADEATTACKREADY_1PV, (INT8) m_pAICtx->FindAIStateByName( "GrenadeAttackReady1PV"));
			setAIIndex( WEAPON::AI_GRENADEATTACKREADY_3PV, (INT8) m_pAICtx->FindAIStateByName( "GrenadeAttackReady3PV"));
			
		}

	}

	#if defined( AI_MOVE_TO_WEAPON)
	// Chara-AI-State의 검색
	{
		m_pCharaAINode = FindAI( "CharaAI");

		if( m_pCharaAINode != nullptr)
			m_pCharaAINode->AddRef();

		m_AIIndexTable.Init( this);
	}
	#endif

	m_mainWeapon->OnBindResource();

}

void WeaponBase::_ProcessContactHitBox( void)
{
	INT32 i;
	MATRIX mat;

	if( m_pContactHitBox != nullptr )
	{	
		i3Skeleton * pSkel = getSceneObject()->GetBody()->getFrameLOD()->getSkeleton();
		for( i = 0; i < pSkel->getBoneCount(); i++)
		{
			if( pSkel->isShapeSet( i) == false) continue;
			if( pSkel->getShapeSet( i) != nullptr )
			{
				pSkel->getShapeSet( i)->getShapeGlobalPose( &mat);
				m_pContactHitBox->setShapeGlobalPose( &mat);
				break;
			}
		}
	}
}

void WeaponBase::_ProcessDrop( void)
{	
	if( BattleServerContext::i()->IsP2PActivate() == false)
		return;

	if( getNetworkIdx() == -1)
		return;

	if( IsDedicatedHost() )
	{
		if( m_tDropInfo._bDropping && !m_tDropInfo._bDroped && !m_tDropInfo._bContact)
		{
			VEC3D vec;
			i3Skeleton * pSkel = getSceneObject()->GetBody()->getFrameLOD()->getSkeleton();
			for( INT32 i = 0; i < pSkel->getBoneCount(); i++)
			{
				if( pSkel->isShapeSet( i) == false) continue;
				if( pSkel->getShapeSet( i) == nullptr)
					continue;

				pSkel->getShapeSet( i)->getLinearVelocity( &vec);
				if( i3Vector::LengthSq( &vec) < 0.01f)
				{
					m_tDropInfo._bDroped = true;
					m_tDropInfo._bContact = true;

					/*
					// Emit dust effect
					if( g_pEnvSet->m_nEffectLevel != 0)
					{
						i3Vector::Set( &vec, i3Math::Randf2(), i3Math::Randf() + 0.1f, i3Math::Randf2() );
						i3Vector::Normalize( &vec, &vec);
						pSkel->getShapeSet( i)->getShapeGlobalPose( &mat);
						g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_WEAPON_DOWN_COMMON, i3Matrix::GetPos( &mat ), &vec );
					}
					*/

					// Network information
					{
						GAMEINFO_DROPED_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_DropedWeapon( getNetworkIdx() );
						pInfo->setState( NET_WEAPONDROPED_BOUND);
						pInfo->setDroped( true);
					}

					// Fire Effect Color Index Initialize
					{
						m_idxColorForFireEffect = 0;
					}
				}
			}
		}
	}
}

void WeaponBase::OnUpdate( REAL32 rDeltaSeconds)
{
	i3Chara::OnUpdate( rDeltaSeconds);

	if( g_pEnvSet->m_nEffectLevel <= 1)
	{
		_ProcessDrop();
	}
	else
	{
		_ProcessContactHitBox();
		_ParseContactReport();
	}
	#ifdef ENABLE_TEST_COMMAND
		if(g_pWeaponManager && g_pWeaponManager->COMMAND_GetGunBullet() >= 0)
		{
			setTotalBulletCount(g_pWeaponManager->COMMAND_GetGunBullet());
			g_pWeaponManager->COMMAND_SetGunBullet(-1);
		}
	#endif
	if( m_tDropInfo._bContact )
	{	
		if( IsDedicatedHost() && getNetworkIdx() != -1 )
		{
			REAL32 sleepTime = 1.f;

			if (MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
				sleepTime = 3.f;

			m_tDropInfo._rSleepTimer += rDeltaSeconds;
			if( m_tDropInfo._rSleepTimer > sleepTime)
			{
				PHYSIX_SetSleeping( true );
				m_tDropInfo._rSleepTimer = 0.f;
				m_tDropInfo._bContact = false;

				// Network Information
				GAMEINFO_DROPED_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_DropedWeapon( getNetworkIdx() );
				if( pInfo->_pWeaponRef != nullptr && pInfo->getState() != NET_WEAPONDROPED_SLEEP)
				{
					pInfo->setState( NET_WEAPONDROPED_SLEEP);
					// 슬립시키고 최초 1회는 바로 보내줘야 합니다.
					pInfo->_fSendTime = 1.0f;
				}
			}
		}
	}

	//	월드에 떨어진 무기는 자신이 직접 LOD 계산을 합니다.
	if( m_tDropInfo._bDroped)
	{
		//	Zoom시 FOV 비율 계산
		REAL32 fRatio = g_pCamera->GetCameraDefaultFOV() / g_pCamera->GetCameraFOV();

		calcLOD( GetPos(), i3Matrix::GetPos( g_pCamera->getCamMatrix()), fRatio);
	}

	// Light
	if( m_pLight != nullptr && (m_pLight->isState( I3G_LIGHT_STATE_DISABLE) == false) )
	{
		m_timeLight += rDeltaSeconds;
		if( m_timeLight > 0.05f)
		{
			m_pLight->addState( I3G_LIGHT_STATE_DISABLE);
			g_pFramework->removeLight( m_pLight);
		}
	}
	
	_IdleEffect(rDeltaSeconds);
	_UpdateSound(rDeltaSeconds);

	// Dragonov를 위한 시간 처리
	//if( isStateMask( WSM_ZOOM))
	{
		m_timeAfterZoom += rDeltaSeconds;
	}

	UpdateAfterAnim( rDeltaSeconds);
}

void WeaponBase::PlayLoadMagazine( CHARA::eSEX_TYPE nArg, bool b1PV)
{	
	switch(nArg)
	{
	case CHARA::eSEX_TYPE_MALE :
		if( b1PV)
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV_MALE));
		else
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV_MALE));
		break;
	case CHARA::eSEX_TYPE_FEMALE:
		if( b1PV)
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV_FEMALE));
		else
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV_FEMALE));
		break;
	case CHARA::eSEX_TYPE_DINO:
		break;
	default:
		break;

	}

}

void WeaponBase::PlayLoadMagazineExt( CHARA::eSEX_TYPE nArg, bool b1PV)
{
	switch(nArg)
	{
	case CHARA::eSEX_TYPE_MALE :
		if( b1PV)
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE1PV_EXTENSION_MALE));
		else
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE3PV_EXTENSION_MALE));
		break;
	case CHARA::eSEX_TYPE_FEMALE:
		if( b1PV)
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE1PV_EXTENSION_FEMALE));
		else
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE3PV_EXTENSION_FEMALE));
		break;
	case CHARA::eSEX_TYPE_DINO:
		break;
	default:
		break;

	}
}

void WeaponBase::PlayLoadMagazineDualMaz( CHARA::eSEX_TYPE nArg, bool b1PV)
{	
	switch(nArg)
	{
	case CHARA::eSEX_TYPE_MALE :
		if( b1PV)
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_1PV_MALE));
		else
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_3PV_MALE));
		break;
	case CHARA::eSEX_TYPE_FEMALE:
		if( b1PV)
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_1PV_FEMALE));
		else
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_3PV_FEMALE));
		break;
	case CHARA::eSEX_TYPE_DINO:
		break;
	default:
		break;
	}
}

bool WeaponBase::NotMasterLoadMagAnim(INT32 idx)
{

	if( (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE)) || 
		(idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_EXTENSION))  ||
		(idx == getAIIndex( WEAPON::AI_LOADMAGAZINE1PV_EXTENSION)) || (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE3PV_EXTENSION)) ||
		(idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV)) || (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV)) ||
		(idx == getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL)) ||
		(idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL)) || (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_1PV_MALE)) ||
		 (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_1PV_FEMALE)) || (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_3PV_MALE)) ||
		  (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_3PV_FEMALE)) ||
		(idx == getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL1PV_LEFT)) || (idx == getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL1PV_RIGHT)) ||
		(idx == getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL3PV_LEFT)) || (idx == getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL3PV_RIGHT)) ||
		(idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV_MALE)) || (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV_FEMALE)) ||
		(idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV_MALE)) || (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV_FEMALE)))

	{
		return true;
	}
	else
	{
		return false;
	}
}

bool WeaponBase::isReloadLoopAnimation()
{
	return getWeaponInfo()->IsReloadLoopAnimation();
}

bool WeaponBase::_PlayWeaponAnimWithApplyTimeScale( INT8 idx)
{	
	if( m_pAICtx == nullptr)	return false;
	if( idx == -1)			return false;
	if( idx >= m_pAICtx->getAIStateCount()) return false;

	REAL32 rTimeScale = 1.f;
	i3AIState * pAIState = m_pAICtx->getAIState( idx);
	CWeaponInfo * pInfo = getWeaponInfo();

	REAL32 rReloadTime = GetReloadTime();
	
	CGameFramework::LoadAIStateAnimation(pAIState);

	// 일반 리로드
	if( NotMasterLoadMagAnim(idx) )
	{
		rTimeScale = _CalcReloadTimeScaleToVersion( pAIState, rReloadTime);
	}
	// 마스터 칭호 빠른 리로드
	else if( (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER1PV)) ||
			 (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER3PV)) ||
			 (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_RIGHT_MASTER1PV)) ||
			 (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_RIGHT_MASTER3PV)) ||
			 (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER1PV_EXT)) ||
			 (idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER3PV_EXT))
			 )
	
	{
		// 빠른 재장전 사용 안하는 무기는 TimeScale을 증가.
		if ( !IsFastReloadAnimWeapon() )
		{
			rTimeScale = _CalcReloadTimeScaleToVersion( pAIState, rReloadTime);
		}
	}		
	// LoadBullet
	else if( (idx == getAIIndex( WEAPON::AI_LOADBULLET)) ||
			(idx == getAIIndex( WEAPON::AI_LOADBULLET_EXTENSION))||
			(idx == getAIIndex( WEAPON::AI_LOADBULLET1PV)) ||
			(idx == getAIIndex( WEAPON::AI_LOADBULLET3PV)) ||
			(idx == getAIIndex(WEAPON::AI_LOADBULLET1PV_MALE)) ||
			(idx == getAIIndex(WEAPON::AI_LOADBULLET1PV_FEMALE)) ||
			(idx == getAIIndex(WEAPON::AI_LOADBULLET3PV_MALE)) ||
			(idx == getAIIndex(WEAPON::AI_LOADBULLET3PV_FEMALE)) ||
			(idx == getAIIndex(WEAPON::AI_FIREBULLET1PV_MALE)) ||
			(idx == getAIIndex(WEAPON::AI_FIREBULLET1PV_FEMALE)) ||
			(idx == getAIIndex(WEAPON::AI_FIREBULLET3PV_MALE)) ||
			(idx == getAIIndex(WEAPON::AI_FIREBULLET3PV_FEMALE)) || 
			(idx == getAIIndex(WEAPON::AI_LOADMAGAZINEDUAL1PV_SWAP)) || 
			(idx == getAIIndex(WEAPON::AI_LOADMAGAZINEDUAL3PV_SWAP)))
	{
		REAL32 rLoadBulletTime = pInfo->GetLoadBulletTime();

		if( pInfo->GetTypeClass() != WEAPON_CLASS_SNIPER && rLoadBulletTime > 0.0f)
			rTimeScale = _CalcReloadTimeScaleToVersion(pAIState, rLoadBulletTime, 1.0f);
 	}	
	// 마스터 칭호 빠른 리로드
	else if( idx == getAIIndex( WEAPON::AI_LOADBULLET_EXTENSION) || 
			 idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_EXTENSION))
	{
		if( EnablePlayMasterAnim() )
			rTimeScale = _CalcReloadTimeScaleToVersion( nullptr, 1.0f, MASTER_RELOAD_SPEED_RATIO);
	}
	else if ( EnablePlayMasterAnim() )
	{
		// 빠른 재장전 사용 안하는 무기는 TimeScale을 증가.
		if ( !IsFastReloadAnimWeapon() )
			rTimeScale = _CalcReloadTimeScaleToVersion(pAIState, rReloadTime);
	}


	if( idx == getAIIndex( WEAPON::AI_LOADMAGAZINE) || 
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_1PV_MALE) || 
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_1PV_FEMALE) || 
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_3PV_MALE) || 
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL_3PV_FEMALE) || 
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV) || idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV) ||
		idx == getAIIndex( WEAPON::AI_LOADBULLET) ||
		idx == getAIIndex( WEAPON::AI_LOADBULLET1PV) || idx == getAIIndex( WEAPON::AI_LOADBULLET3PV) ||
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER1PV) || idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER3PV) ||
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_RIGHT_MASTER1PV) || idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_RIGHT_MASTER3PV) ||
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER1PV_EXT) || idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER3PV_EXT) ||
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL) ||
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL1PV_LEFT) || idx == getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL1PV_RIGHT) ||		
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL3PV_LEFT) || idx == getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL3PV_RIGHT) ||		
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_DUALMAG_ALL) ||
		idx == getAIIndex( WEAPON::AI_LOADBULLET_EXTENSION) || idx == getAIIndex( WEAPON::AI_LOADMAGAZINE_EXTENSION)	||
		idx == getAIIndex( WEAPON::AI_LOADMAGAZINE1PV_EXTENSION) || idx == getAIIndex( WEAPON::AI_LOADMAGAZINE3PV_EXTENSION) ||
		idx == getAIIndex(WEAPON::AI_LOADMAGAZINE_1PV_MALE) || idx == getAIIndex(WEAPON::AI_LOADMAGAZINE_3PV_MALE)||
		idx == getAIIndex(WEAPON::AI_LOADMAGAZINE_1PV_FEMALE) || idx == getAIIndex(WEAPON::AI_LOADMAGAZINE_3PV_FEMALE)||
		idx == getAIIndex(WEAPON::AI_LOADMAGAZINE1PV_EXTENSION_MALE) || idx == getAIIndex(WEAPON::AI_LOADMAGAZINE3PV_EXTENSION_MALE)||
		idx == getAIIndex(WEAPON::AI_LOADMAGAZINE1PV_EXTENSION_FEMALE) || idx == getAIIndex(WEAPON::AI_LOADMAGAZINE3PV_EXTENSION_FEMALE)||
		idx == getAIIndex(WEAPON::AI_LOADBULLET1PV_MALE) || idx == getAIIndex(WEAPON::AI_LOADBULLET3PV_MALE)||
		idx == getAIIndex(WEAPON::AI_LOADBULLET1PV_FEMALE) || idx == getAIIndex(WEAPON::AI_LOADBULLET3PV_FEMALE)||
		idx == getAIIndex(WEAPON::AI_LOADBULLET_LEFT_MALE) || idx == getAIIndex(WEAPON::AI_LOADBULLET_RIGHT_MALE)||
		idx == getAIIndex(WEAPON::AI_LOADBULLET_LEFT_FEMALE) || idx == getAIIndex(WEAPON::AI_LOADBULLET_RIGHT_FEMALE)
	)
	{
		// 초반에 그냥 Time Scale을 1로 상태 전환.
		if (isReloadLoopAnimation())
		{
			rTimeScale = 1.f;

			// 이상하게 이곳저곳에 적용되어 있으니... 여기도 어쩔수 없이...
			if (g_pEnvSet->IsV2Version() )
			{
				REAL32 ratio = BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_REBULLET_RATE);
				CASHITEM_TRACE("CASHITEMSKILL_REBULLET_RATE = %f\n", ratio);
				rTimeScale += (rTimeScale * ratio);
			}
		}

		// 스나이퍼 총 발사할때 LoadBullet 동작은 아이템 사용과 관계없다.
		if( pInfo->GetTypeClass() == WEAPON_CLASS_SNIPER &&
			(idx == getAIIndex( WEAPON::AI_LOADBULLET) || idx == getAIIndex( WEAPON::AI_LOADBULLET_EXTENSION) ||
		     idx == getAIIndex( WEAPON::AI_LOADBULLET1PV) || idx == getAIIndex( WEAPON::AI_LOADBULLET3PV) ||
			 idx == getAIIndex(WEAPON::AI_LOADBULLET1PV_MALE) || idx == getAIIndex(WEAPON::AI_LOADBULLET1PV_FEMALE) ||
			 idx == getAIIndex(WEAPON::AI_LOADBULLET3PV_MALE) || idx == getAIIndex(WEAPON::AI_LOADBULLET3PV_FEMALE) ||
			 idx == getAIIndex(WEAPON::AI_FIREBULLET1PV_MALE) || idx == getAIIndex(WEAPON::AI_FIREBULLET1PV_FEMALE) ||
			 idx == getAIIndex(WEAPON::AI_FIREBULLET3PV_MALE) || idx == getAIIndex(WEAPON::AI_FIREBULLET3PV_FEMALE)))
		{
			// no use
		}
		// MG 장전할때 LoadBullet 동작은 아이템 사용과 관계없다.
		else if( pInfo->GetTypeClass() == WEAPON_CLASS_MG &&
			(idx == getAIIndex( WEAPON::AI_LOADBULLET) || idx == getAIIndex( WEAPON::AI_LOADBULLET_EXTENSION) ||
			idx == getAIIndex( WEAPON::AI_LOADBULLET1PV) || idx == getAIIndex( WEAPON::AI_LOADBULLET3PV) ||
			idx == getAIIndex(WEAPON::AI_LOADBULLET1PV_MALE) || idx == getAIIndex(WEAPON::AI_LOADBULLET1PV_FEMALE) ||
			idx == getAIIndex(WEAPON::AI_LOADBULLET3PV_MALE) || idx == getAIIndex(WEAPON::AI_LOADBULLET3PV_FEMALE) ||
			idx == getAIIndex(WEAPON::AI_FIREBULLET1PV_MALE) || idx == getAIIndex(WEAPON::AI_FIREBULLET1PV_FEMALE) ||
			idx == getAIIndex(WEAPON::AI_FIREBULLET3PV_MALE) || idx == getAIIndex(WEAPON::AI_FIREBULLET3PV_FEMALE)))
		{
			// no use
		}
		//	빠른 장전 아이템 사용시 시간 감소. 
		else
		{
			if (g_pEnvSet->IsV2Version() == false)
			{
				REAL32 ratio = BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_REBULLET_RATE);
				CASHITEM_TRACE("CASHITEMSKILL_REBULLET_RATE = %f\n", ratio);
				rTimeScale += (rTimeScale * ratio);
			}
		}
	}

	if( rTimeScale < I3_EPS)
	{
		rTimeScale = I3_EPS + 0.00001f;
	}

#if defined( USE_RELOAD_NEWANIMATION)
	WF::PlayWeaponAnimDirect( this, (INT32) idx, rTimeScale);
#else
	WF::PlayWeaponAnim( this, (INT32) idx, rTimeScale);
#endif
	setAIIndex( WEAPON::AI_CURRENT, idx);

	//I3PRINTLOG(I3LOG_NOTICE,"Weapon rTimeScale: %f \n", rTimeScale);
	return true;
}

void WeaponBase::_OnPlayWeaponIdleAnim(void)
{	
	if( isWeaponStateMask( WSM_ACTIVATE_EXTENSION) )
	{
		if( getAIIndex( WEAPON::AI_IDLE_EXTENSION) != -1 )
		{
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_IDLE_EXTENSION) );
			return;
		}
	}

	if( getAIIndex( WEAPON::AI_IDLE_FOLDED) != -1)
	{
		if( i3::kind_of<WeaponDualSMG*>(this) )
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_IDLE_FOLDED));
		else if( i3::kind_of<WeaponDualSMG*>(getPairWeapon()))
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_IDLE_FOLDED));
		else
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_IDLE) );
	}
	else
	{
		if( (getAIIndex( WEAPON::AI_IDLE1PV) != -1) && (getAIIndex( WEAPON::AI_IDLE3PV) != -1))		// 우선
		{
			if(  m_pOwner != nullptr && m_pOwner->is1PV())
				WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_IDLE1PV) );
			else if(  m_pOwner != nullptr && m_pOwner->is3PV())
				WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_IDLE3PV) );
			else if( getAIIndex( WEAPON::AI_IDLE) != -1)
				WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_IDLE) );
		}
		else if( getAIIndex( WEAPON::AI_IDLE) != -1)
		{
			WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_IDLE) );
		}
	}

	//setWeaponState( WEAPON::STATE_READY);
}

void WeaponBase::_OnPlayWeaponFireAnim(void)
{	
	if( isWeaponStateMask( WSM_ACTIVATE_EXTENSION) )
	{
		if( getAIIndex( WEAPON::AI_FIRE_EXTENSION) != -1 )
		{
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_FIRE_EXTENSION) );
			return;
		}
	}
	
	if( getAIIndex( WEAPON::AI_IDLE_FOLDED) != -1)
	{
		if( i3::kind_of<WeaponDualSMG*>(this))
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_FIRE_FOLDED));
		else if( i3::kind_of<WeaponDualSMG*>(getPairWeapon()))
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_FIRE_FOLDED));
		else
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_FIRE) );
	}
	else
	{
		if( (getAIIndex( WEAPON::AI_FIRE1PV) != -1) && (getAIIndex( WEAPON::AI_FIRE3PV) != -1))		// 우선
		{
			if(  m_pOwner != nullptr && m_pOwner->is1PV())
				WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_FIRE1PV) );
			else if(  m_pOwner != nullptr && m_pOwner->is3PV())
				WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_FIRE3PV) );
			else if( getAIIndex( WEAPON::AI_FIRE) != -1)
				WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_FIRE) );
		}
		else
		{
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_FIRE) );
		}
	}
}

void WeaponBase::_OnPlayWeaponSecondFireAnim( void)
{
	if ( getAIIndex( WEAPON::AI_SECFIRE3PV) != -1)
	{
		if ( m_pOwner != nullptr && m_pOwner->is1PV())
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_SECFIRE));
		else if ( m_pOwner != nullptr && m_pOwner->is3PV())
			WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_SECFIRE3PV));
		else
			WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_SECFIRE));
	}
	else
	{
		WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_SECFIRE));
	}
}


static bool _SetSoundNodeProc( i3GameNode * pNode, i3GameNodeTraversal * pTraversal, void * pUserData)
{
	if( !i3::same_of<i3AIContext*>(pNode))
		return true;

	bool * isOwner = (bool*)pUserData;

	i3AIContext * pCtx = (i3AIContext *) pNode;
	i3AI * pAI = pCtx->getAI();
	INT32 i, j;

	for( i = 0; i < pAI->getAIStateCount(); i++)
	{
		i3AIState * pState = pAI->getAIState( i);
		i3TimeSequence * pTimeSeq = pState->getTimeSeq();

		if( pTimeSeq == nullptr)
			continue;

		for( j = 0; j < pTimeSeq->GetSequenceCount(); j++)
		{
			i3TimeSequenceInfo * pInfo = pTimeSeq->GetSequence( j);

			if( i3::kind_of<i3SoundNode*>(pInfo->GetNode()))
			{
				i3SoundNode * pSnd = (i3SoundNode *) pInfo->GetNode();
				if( pSnd == nullptr) 
					continue;

				i3SoundPlayInfo * pPlayInfo = pSnd->getSoundPlayInfo();
				if( pPlayInfo == nullptr)
					continue;

				if( pPlayInfo->GetSoundType() == I3SND_PROP_TYPE_VOICE)
				{
					pPlayInfo->RemoveStyle( I3SND_PROP_TYPE_VOICE);
					pPlayInfo->AddStyle( I3SND_PROP_TYPE_EFFECT);
				}

				UINT32 n3DMode = I3SND_RT_3DMODE_WORLD_SPACE;
				//Listener Sound의 속성을 가지고 있으면서 Owner속성인경우만 positional sound를 disable시킨다.
				if( *isOwner == true)
				{
					n3DMode = I3SND_RT_3DMODE_DISABLE;
				}

				pPlayInfo->set3DMode( n3DMode);
			}
		}
	}

	return true;
}

void WeaponBase::SetOwner( CGameCharaBase * pOwner)
{


	if (pOwner == nullptr)
	{
		m_idxColorForFireEffect = 0;
	}
	else
	{
		INT32 SlotIdx = pOwner->getCharaInfo()->GetNetIdx();
		// 무기 소유자가 자기 자신일 경우는 자신의 InvItemData를 받아오면 됩니다
		if ( m_pWeaponInfo != nullptr && m_pWeaponInfo->GetUnColorEffectFlag())
		{
			m_idxColorForFireEffect = 0;
		}
		else
			if(SlotIdx == BattleSlotContext::i()->getMySlotIdx())
			{
				USER_INFO_INVITEM_DATA Data;
				Data.Reset();
				UserInfoContext::i()->GetMyUserInfoInvItemData(&Data);
				m_idxColorForFireEffect = Data._ui16MuzzleFlashColor;
			}
			else
			{
				m_idxColorForFireEffect = BattleSlotContext::i()->getFireMuzzleColorForSlot(SlotIdx);
			}
	}


	m_pOwner = pOwner;

	if( m_pOwner != nullptr )
	{
		bool bOwner = m_pOwner->is1PV();
		//Sound에 Owner에따라서 3dmode를 실행 할런지 말런지 결정한다.
		{
			//I3TRACE(">>>WEAPON_MANAGER : START<<<%s>>>>>>>>>>\n", pOwner->GetName());

			i3GameNodeTraversal Traverse;

			Traverse.setUserData( &bOwner);
			Traverse.setUserProc( _SetSoundNodeProc);

			Traverse.Traverse( this);

			//I3_SAFE_RELEASE( pTraverse);

			//I3TRACE(">>>WEAPON_MANAGER : END<<<%s>>>>>>>>>>\n", pOwner->GetName());
		}

		for( INT32 i = 0; i < WEAPON::FIRE_SOUND_COUNT; i++)
		{
			if( m_pFireSoundPlayInfo[ i] != nullptr)
			{
				if( bOwner)
		 			m_pFireSoundPlayInfo[ i]->set3DMode( I3SND_RT_3DMODE_DISABLE);
				else
					m_pFireSoundPlayInfo[ i]->set3DMode( I3SND_RT_3DMODE_WORLD_SPACE);
			}
		}

		addWeaponFlag( WEAPON_FLAG_TAKE_CHARA);
	}
	else
	{
		m_ui8Endurance		= 100;
		removeWeaponFlag( WEAPON_FLAG_TAKE_CHARA);
	}
	
	if( m_mainWeapon )
		m_mainWeapon->SetOwner(pOwner);
}

void WeaponBase::NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type )
{
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

	setWeaponState( WEAPON::STATE_FIRE);

	//	발사 타입
	if( type == WEAPON::PRIMARY_FIRE)			_OnPlayWeaponFireAnim();
	else if( type == WEAPON::SECONDARY_FIRE)		_OnPlayWeaponSecondFireAnim();
	else
	{
		I3PRINTLOG(I3LOG_FATAL,  "weapon fire type invalid.");
	}

	m_FireOrder = type;

	if( m_pOwner != nullptr && (m_pOwner->isLocal() || m_pOwner->isAI()) )
	{
		addWeaponStateMask( WSM_FIRING | WSM_FIRE_DISABLE);
	}
	removeWeaponStateMask( WSM_WAIT_AUTOFIREDELAY);

	//////////////////////////////
	if( isWeaponStateMask( WSM_ZOOM))
		m_timeAfterZoom = 0.0f;

	removeWeaponStateMask( WSM_FIRST_ZOOM);
}

void WeaponBase::ReloadAnimation(CHARA_VIEW_TYPE eCAT)
{
	if( (m_pPairWeapon != nullptr) && m_pPairWeapon->isDualGun())
	{
		if( getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL) != -1)
		{
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL));
		}
		else
		{
			if( m_pOwner->is1PV() && ( getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL1PV_LEFT) != -1) )
			{
				_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL1PV_LEFT));
			}
			else if( getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL3PV_LEFT) != -1)
			{	
				_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINEDUAL3PV_LEFT));
			}
			else
			{
				// 로드 애니메이션이 1, 3인칭 구별없이 되어 있는 경우.
				if( getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV) == -1)
				{
					_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE));
				}
				// 구별이 되어 있는 경우 2008. 10. 28. 이후로 대부분의 무기는 1, 3인칭 구별이 된다.
				else
				{
					if(CHARA_VIEW_1PV == eCAT)
					{
						_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV));
					}
					else
					{
						_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV));
					}
				}
			}
		}		
	}
	else if(EnablePlayMasterAnim())
	{
		if(IsFastReloadAnimWeapon())
		{
			if(CHARA_VIEW_1PV == eCAT)
			{

				if( getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER1PV_EXT) != -1  && isWeaponStateMask( WSM_ACTIVATE_EXTENSION))
				{
					_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER1PV_EXT));
				}
				else
					_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER1PV));
			
			}
			else
			{
				if( getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER3PV_EXT) != -1  && isWeaponStateMask( WSM_ACTIVATE_EXTENSION))
				{
					_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER3PV_EXT));
				}
				else
					_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_MASTER3PV));
			}
		}
		// 그냥 애니메이션만 빠르게 해 준다.
		else
		{
			// 로드 애니메이션이 1, 3인칭 구별없이 되어 있는 경우.
			if( getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV) == -1&&
				getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV_MALE) == -1)
			{
				_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE));
			}
			// 구별이 되어 있는 경우 2008. 10. 28. 이후로 대부분의 무기는 1, 3인칭 구별이 된다.
			else
			{
				bool b1PV = (bool)(CHARA_VIEW_1PV == eCAT);

				if( b1PV)
				{
					if( getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV_MALE) != -1)
					{
						PlayLoadMagazine( m_pOwner->getDBInfo()->GetCharaSexTypePHYSIX(), b1PV );
					}
					else
					{
						_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV));
					}
					
				}
				else
				{	
					if( getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV_MALE) != -1)
					{
						PlayLoadMagazine( m_pOwner->getDBInfo()->GetCharaSexTypePHYSIX(), b1PV );
					}
					else
					{
						_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV));
					}
					
				}
			}
		}
	}
	else
	{
		// 로드 애니메이션이 1, 3인칭 구별없이 되어 있는 경우.
		if( getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV) == -1 &&				
			getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV_MALE) == -1)
		{
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE) );
		}
		// 구별이 되어 있는 경우 2008. 10. 28. 이후로 대부분의 무기는 1, 3인칭 구별이 된다.
		else
		{
			bool b1PV = (bool) (CHARA_VIEW_1PV == eCAT);
			if( b1PV)
			{
				if( getAIIndex( WEAPON::AI_LOADMAGAZINE1PV_EXTENSION_MALE) != -1 && isWeaponStateMask( WSM_ACTIVATE_EXTENSION) )
				{
					PlayLoadMagazineExt( m_pOwner->getDBInfo()->GetCharaSexTypePHYSIX(), b1PV);
				}
				else if(getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV_MALE) != -1)
				{
					PlayLoadMagazine( m_pOwner->getDBInfo()->GetCharaSexTypePHYSIX(), b1PV);
				}
				else
				{
					_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_1PV));
				}
			}
			else
			{
				if(getAIIndex( WEAPON::AI_LOADMAGAZINE1PV_EXTENSION_MALE) != -1 && isWeaponStateMask( WSM_ACTIVATE_EXTENSION) )
				{
					PlayLoadMagazineExt( m_pOwner->getDBInfo()->GetCharaSexTypePHYSIX(), b1PV);
				}
				else if(getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV_MALE) != -1)
				{
					PlayLoadMagazine( m_pOwner->getDBInfo()->GetCharaSexTypePHYSIX(), b1PV);
				}
				else
				{
					_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_3PV));
				}
			
			}
		}
	}
}



void  WeaponBase::ChangeDualAnimation( CHARA_VIEW_TYPE eCAT, REAL32 rTimeScale)
{
	if( (m_pPairWeapon != nullptr) && m_pPairWeapon->isDualGun())
	{
		if( getAIIndex( WEAPON::AI_CHANGEDUAL) != -1)
		{
			// revision 28684
			WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_CHANGEDUAL), rTimeScale );
		}
		else
		{
			if( eCAT == CHARA_VIEW_1PV )
			{
				if( getAIIndex( WEAPON::AI_CHANGEDUAL1PV_LEFT) != -1 )
				{
					WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_CHANGEDUAL1PV_LEFT), rTimeScale );
				}
			}
			else if( getAIIndex( WEAPON::AI_CHANGEDUAL3PV_LEFT) != -1)
			{
				WF::PlayWeaponAnim( this, getAIIndex( WEAPON::AI_CHANGEDUAL3PV_LEFT), rTimeScale );
			}
		}		
	}
}


void WeaponBase::OnPlayLoadMagAnimation( void)
{	
#if defined(BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG)
	I3ASSERT(m_pOwner);
#else
	if(m_pOwner == nullptr) return;
#endif

	if( isWeaponStateMask( WSM_ACTIVATE_EXTENSION) )
	{
		if( getAIIndex( WEAPON::AI_LOADMAGAZINE_EXTENSION) != -1 )
		{
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE_EXTENSION));
		}
		else if ( getAIIndex( WEAPON::AI_LOADMAGAZINE1PV_EXTENSION) != -1)
		{
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE1PV_EXTENSION));
		}
		else if ( getAIIndex( WEAPON::AI_LOADMAGAZINE3PV_EXTENSION) != -1)
		{
			_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADMAGAZINE3PV_EXTENSION));
		}
	    else
		{
			ReloadAnimation( m_pOwner->getViewType());
		}
	}
	else
	{
		ReloadAnimation( m_pOwner->getViewType());
	}

	setWeaponState( WEAPON::STATE_LOADMAG);
}

bool WeaponBase::EnablePlayMasterAnim()
{
	return _IsEnableMasterAnimToVersion();
}

bool WeaponBase::IsFastReloadAnimWeapon()
{
	I3ASSERT(getWeaponInfo());

	return getWeaponInfo()->IsFastReloadAnimation();
}

void WeaponBase::OnPlayLoadBulletAnimation( void)
{
	bool bPlay = false;

	// extension animation을 먼저 플레이해본다.
	if( isWeaponStateMask( WSM_ACTIVATE_EXTENSION))
	{
		bPlay = _PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADBULLET_EXTENSION) );
	}
	
	if( bPlay == false)
	{	// extension anim이 없다.
		if( m_pOwner->is1PV())
		{
			// 1인칭용 anim 플레이
			bPlay = _PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADBULLET1PV) );
			
			if( bPlay == false)
			{	// 1인칭 anim이 없다.
				// 각 성별 anim을 플레이해본다.
				switch( m_pOwner->getDBInfo()->GetCharaSexTypePHYSIX())
				{
				case CHARA::eSEX_TYPE_MALE :	
					bPlay = _PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADBULLET1PV_MALE));		
					break;

				case CHARA::eSEX_TYPE_FEMALE:	
					bPlay = _PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADBULLET1PV_FEMALE));	
					break;
				}
			}
		}
		else if( m_pOwner->is3PV())
		{
			bPlay = _PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADBULLET3PV) );
			
			if( bPlay == false)
			{
				switch( m_pOwner->getDBInfo()->GetCharaSexTypePHYSIX())
				{
				case CHARA::eSEX_TYPE_MALE :	
					bPlay = _PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADBULLET3PV_MALE));		
					break;

				case CHARA::eSEX_TYPE_FEMALE:	
					bPlay = _PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADBULLET3PV_FEMALE));	
					break;
				}
			}
		}
	}

	if( bPlay == false)
	{// 기본 anim을 플레이하자
		_PlayWeaponAnimWithApplyTimeScale( getAIIndex( WEAPON::AI_LOADBULLET));
	}
}

void WeaponBase::OnPlayExtensionAnimation( bool bVal, bool bAnimPlay)
{	
	if( bAnimPlay )
	{
		if( bVal)
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_EXTENSION_ACTIVATE));
		else
			WF::PlayWeaponAnim(this, getAIIndex( WEAPON::AI_EXTENSION_DEACTIVATE));
	}
	else
		_OnPlayWeaponIdleAnim();
}

void WeaponBase::NET_FireSync( VEC3D * pTargetPos, WEAPON::FIRE_ORDER fireOrder )
{
	INT32	 nR;

	if( g_pRand->CheckCreation() )
	{	
		if( m_pOwner != nullptr && !m_pOwner->isNetwork())
		{	// 자신의 캐릭터입니다.
			g_pRand->GetPreCode();
			nR = g_pRand->GetRand();
			srand( nR );

			GAMEEVENT::Write_Chara_Fire( m_pOwner, pTargetPos, this);
		}
		else
		{
			// 네트워크 캐릭터 입니다.
			nR = g_pRand->GetRand( g_pRand->GetCode() );
			srand( nR );
		}
	}
}

#ifdef DOMI_STING_POISON_GRENADE
void WeaponBase::NET_FireSync_DomiDinoGrenade(VEC3D* pTargetPos, WEAPON::FIRE_ORDER fireOrder)
{
	GAMEEVENT::Write_Chara_Fire_DomiDinoGrenade( m_pOwner, pTargetPos, this);
}
#endif

void WeaponBase::NET_Bound( VEC3D * pStartPos, VEC3D * pDir, REAL32 fSpeed, INT32 nBoundType )
{
}

void WeaponBase::AttachNode( i3Node * pSceneNode, i3GameNode * pOwner )
{
	i3Node * pAttachNode = getSceneObject();
	
	//	렌더링될 곳에 붙인다.	
	if( pAttachNode->GetParent() != pSceneNode && pSceneNode != nullptr)
	{
		I3_MUST_ADDREF( pAttachNode);
		if( pAttachNode->GetParent() != nullptr)
			pAttachNode->RemoveFromParent();
				
		pSceneNode->AddChild( pAttachNode);
		I3_MUST_RELEASE( pAttachNode);
	}

	//	업데이트될 곳에 붙인다.
	if( m_pParentGame != pOwner && pOwner != nullptr)
	{
		if( m_pParentGame != nullptr)
			m_pParentGame->RemoveChild( this);

		m_pParentGame = pOwner;
		I3_GAMENODE_ADDCHILDTOHEAD( m_pParentGame, this );
	}

	if( pSceneNode != nullptr && m_pParentGame != nullptr)
		addWeaponFlag( WEAPON_FLAG_ATTACHED);

	#if defined( AI_MOVE_TO_WEAPON)
	if( i3::kind_of<i3Chara*>(pOwner))
	{
		if( (m_AttachGrip == WEAPON_ATTACH_GRIP) && (m_pCharaAINode != nullptr))
		{
			// KEYWORD : AI_MOVE_TO_WEAPON
			// 무기를 손에 쥐는 경우...
			AttachCharaAIToChara( (i3Chara *) pOwner);

			_Rec_BindWeaponAnim( m_pCharaAINode);
		}
	}
	#endif

	m_mainWeapon->AttachNode(pSceneNode, pOwner);

}

void WeaponBase::DetachNode( void )
{
	i3Node * pAttachNode = getSceneObject();
	if( ( pAttachNode != nullptr) && (pAttachNode->GetParent() != nullptr) )
	{
		pAttachNode->RemoveFromParent();
	}

	if( m_pParentGame != nullptr)
	{
		if( m_pParentGame->getFirstChild() != nullptr)
			m_pParentGame->RemoveChild( this);
		m_pParentGame = nullptr;
	}

	// 무기 변경시 Light가 있을 경우 꺼준다
	if( m_pLight != nullptr && (m_pLight->isState( I3G_LIGHT_STATE_DISABLE) == false) )
	{
		m_pLight->addState( I3G_LIGHT_STATE_DISABLE);
		g_pFramework->removeLight( m_pLight);
	}

	removeWeaponFlag( WEAPON_FLAG_ATTACHED);
	ChangeGunDummy( WEAPON_ATTACH_NONE);

	#if defined( AI_MOVE_TO_WEAPON)
	if( m_pCharaAINode != nullptr)
	{
		// KEYWORD : AI_MOVE_TO_WEAPON
		DetachCharaAIFromChara();
	}
	#endif

	if (m_mainWeapon)
		m_mainWeapon->DetachNode();

}

void WeaponBase::OnDrop(void)
{	// 무기를 떨어뜨리면 익스텐션을 끈다.
	setExtensionActivate(false);
	m_pWeaponInfo = m_pAbsoluteWeaponInfo;		//	백업된 주무기 정보로 다시 전환

	m_CurrentExtensionType = m_pWeaponInfo->getExtensionType(0); //	첫번째 익스텐션으로 초기화 설정. 중간에 바뀔수 있는 값

}

void WeaponBase::GetDamageInfo( CHARA_DAMAGEINFO * pOut)
{
	WeaponBase * pThis = getUsedWeapon();

	if( getPairWeapon() != nullptr )
	{
		// 크리스 계열은 듀얼의 Pair가 원본 무기이다.
		WeaponBase * pPairWeapon = getPairWeapon();
		if( pPairWeapon->getPairWeapon() != nullptr)
			pPairWeapon = pPairWeapon->getPairWeapon();

		pPairWeapon->getWeaponInfo()->GetDamageInfo( pOut);

		if( i3::same_of<WeaponGrenadeShell*>(this))
			pOut->_bExtension = true;
		else
			pOut->_bExtension = pPairWeapon->isExtensionActivate();

		if( pOut->_bExtension)
		{
			// 익스텐션 상태라면 현재 무기(this가 익스텐션 무기다)를 설정
			pThis = this;
		}
		else
			pThis = pPairWeapon->getUsedWeapon();
	}
	else
	{
		m_pWeaponInfo->GetDamageInfo( pOut);
		pOut->_bExtension = isExtensionActivate();
	}

	I3ASSERT( pThis != nullptr);

	pOut->SetWeaponItemID(this);

	if( m_pOwner != nullptr)
	{
		pOut->_IsOwner			= m_pOwner->isLocal();
		pOut->_nKillerIdx		= m_pOwner->getCharaNetIndex();
	}

	{
		pOut->_FireOrder		= pThis->getFireOrder();
		pOut->_AttackType		= pThis->getWeaponInfo()->GetAttackType( pThis->getFireOrder(), pThis->getAutoFireCount() );
	}

	pOut->_rDamage			= getAP();
	pOut->_nNetidx			= getNetworkIdx();
	pOut->_pWeapon			= this;
	pOut->_pVecPos			= GetPos();

	if( m_pWeaponInfo != nullptr)
	{
		pOut->_pWeaponInfo		= m_pWeaponInfo;
		pOut->_DamageType		= m_pWeaponInfo->GetTypeDamage();
		pOut->_rExplosionRange	= m_pWeaponInfo->GetExplosion_Range();
		pOut->_rExplosionSlope	= m_pWeaponInfo->GetExplosion_EffectiveSlope();
		pOut->_rEffectiveTime	= m_pWeaponInfo->GetEffectiveTime();
	}
}

WEAPON::ATTACK_TYPE	WeaponBase::GetAttackType( void)
{
	return m_pWeaponInfo->GetAttackType( getFireOrder(), getAutoFireCount());
}

void WeaponBase::ResetColideeGroup(void)
{
	_InitCollideeGroup();
}

// xgkick_thai
void WeaponBase::setTeamTag( INT32 team)
{
	m_tDropInfo._nUserTeam = team;
}

void WeaponBase::Transmission( i3Node * pNewSceneNode, i3GameNode * pNewOwner, VEC3D * pPos, bool bToWorld)
{
	m_tDropInfo._bDropping = bToWorld;

	DetachNode();
	AttachNode( pNewSceneNode, pNewOwner);

	if( pPos != nullptr)
	{
		SetPos( pPos);
	}

	// Alpha check 알파가 필요한 경우 켜줍니다.
	CheckAlphaUsed();
}

void WeaponBase::ChangeAttachHand( CHARA_WEAPON_ATTACH_HAND nAttachHand, WEAPON_ATTACH_TYPE nWeaponAttach)
{
	if( getOwner() == nullptr) 
		return;

	getOwner()->getWeaponContext()->setAttachHand( nAttachHand);

	//	현재 들고 있는 무기가 있다면 무기가 붙는 곳을 바꾼다. (렌더링 되는 곳만 바꾼다)
	if (getOwner()->getWeaponContext()->getAttachRoot())
	{
		I3ASSERT( getOwner()->getWeaponContext()->getAttachRoot() != nullptr );
		AttachNode( getOwner()->getWeaponContext()->getAttachRoot(), getOwner());
	}
	else
		TRACE("WeaponBase::ChangeAttachHand getOwner()->getWeaponContext()->getAttachRoot()=nullptr\n");
	

	ChangeGunDummy( nWeaponAttach);
}

// KEYWORD : WEAPON_DUMMY
void WeaponBase::ChangeGunDummy( WEAPON_ATTACH_TYPE nAttachType)
{	
	//	총 손잡이 
	switch( nAttachType)
	{
	case WEAPON_ATTACH_GRIP:	getSceneObject()->SetMatrix( &m_mtxGunDummy);		break;
	case WEAPON_ATTACH_BARREL:
#if defined( USE_RELOAD_NEWANIMATION)
		getSceneObject()->SetMatrix( &m_mtxGunDummy);		break;
#else
		if( m_pGunDummyLeftHand != nullptr)
		{
			getSceneObject()->SetMatrix( &m_mtxGunDummyLeftHand );
		}
#endif
		break;
	}

	m_AttachGrip = nAttachType;
}


/**********************************************************************************************//**
 * @fn	void WeaponBase::MoveNodeForBow()
 *
 * @brief	Move node for bow.
 * @desc	활처럼 백더미에 붙는 모양이 다른 무기를 위해 만든 함수
 * @author	Ran
 * @date	2011-09-22
 **************************************************************************************************/

void WeaponBase::MoveNodeForBow()
{

	i3Transform *				m_pTransformDummy;

	m_pGunDummyBack = i3Scene::FindNodeByName( getSceneObject(), "Back");
	if( m_pGunDummyBack != nullptr)
	{				
		i3Scene::MoveChildren(m_pGunDummyBack, getSceneObject()->GetBody() );
		m_pTransformDummy =(i3Transform*) m_pGunDummyBack;
		getSceneObject()->SetMatrix( m_pTransformDummy->GetMatrix());	
		
	}

}
void WeaponBase::SetCurrentAttachHand(void)
{
	/*if( getOwner()->getWeaponContext()->getBackpack() != this && 
		getOwner()->getWeaponContext()->getSidepack() != this &&
		getOwner()->GetCurrentCharaWeapon() == this)*/

	/*I3ASSERT( getOwner()->getWeaponContext()->getBackpack() != this && 
		getOwner()->getWeaponContext()->getSidepack() != this );*/

	if( getOwner() == nullptr)
		return;

	WeaponBase * pWeapon = getOwner()->GetCurrentCharaWeapon();
	if( pWeapon == this)
	{
		ChangeAttachHand( CHARA_WEAPON_GRIP_RIGHT_HAND, WEAPON_ATTACH_GRIP);
	}
}

void WeaponBase::SetCurrentAttachLeftHand(void)
{
	if( getOwner() == nullptr)
	{
		//I3PRINTLOG(I3LOG_NOTICE,"/*- Exception process -*/");
		return;
	}

	WeaponBase * pWeapon = getOwner()->GetCurrentCharaWeapon();
	if( pWeapon == this)
	{
		ChangeAttachHand( CHARA_WEAPON_GRIP_LEFT_HAND, WEAPON_ATTACH_GRIP);
	}
}

// KEYWORD : WEAPON_DUMMY
bool WeaponBase::IsLoadMagazineLeftBarrel( void)
{	

	if( EnablePlayMasterAnim() && IsFastReloadAnimWeapon() )
	{
		return false;
	}

	bool bRv = (m_pWeaponInfo->IsLoadMagazineLeftBarrel()==true);

	// 유탄 발사기 부착형
	if( m_pWeaponInfo->availableExtType( WEAPON::EXT_GLAUNCHER))
	{
		if( !isExtensionActivate() && bRv )	bRv = true;
		else								bRv = false;
	}

	return bRv;
}

// KEYWORD : WEAPON_DUMMY
bool WeaponBase::IsLoadBulletLeftBarrel( void)
{	// true : 노리쇠 전후진시 왼손 더미에 붙는 무기
	return getWeaponInfo()->IsLoadBulletLeftBarrel();
}

bool WeaponBase::IsExtensionWeapon( void)
{ 
	CWeaponInfo * pWeaponInfo = getAbsoluteWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);

	if( pWeaponInfo->GetTypeClass() == WEAPON_CLASS_KNIFE)
	{
		if( pWeaponInfo->IsThrowWeapon())
			return true;
		if( pWeaponInfo->availableExtType( WEAPON::EXT_THROW))
			return true;
	}

	return pWeaponInfo->isExtensible();
}

bool WeaponBase::IsFireBulletWeapon( void)
{	// 발사후 볼트액션 동작이 따로 있을 경우 
	// WEAPON::STATE_NEEDTO_LOADBULLET에서 LoadBullet 모션을 사용해야 할 경우 여기서 false 이여야 LoadBullet 모션을 사용한다.
	if( getAIIndex( WEAPON::AI_FIREBULLET1PV_MALE) != -1 || getAIIndex( WEAPON::AI_FIREBULLET3PV_MALE) != -1 ||
		getAIIndex( WEAPON::AI_FIREBULLET1PV_FEMALE) != -1 || getAIIndex( WEAPON::AI_FIREBULLET3PV_FEMALE) != -1)
	{
		return true;
	}

	return false;
}

void WeaponBase::_QueContactReport( const I3_PHYSIX_CONTACT_REPORT_PTR& spReport )
{
//	UINT32 nCount = m_nContactReportCount - m_nContactReportReadPos;
	if( m_vecContactReport.size() >= PHYSIX_CONTACT_REPORT_COUNT )
	{
		//I3TRACE( "WeaponBase::_QueContactReport() - Contact report buffer over flow.\n" );
		return;
	}
	
	m_vecContactReport.push_back(spReport);

//	UINT32 i = m_nContactReportCount % PHYSIX_CONTACT_REPORT_COUNT;
//	i3Physix::ContactReportCopyTo( &m_ContactReportBuf[ i ], pReport );
//	m_nContactReportCount++;
}

void WeaponBase::_ParseContactReport( void )
{
	UINT32 i, j;
	const UINT32 nCount = (UINT32)m_vecContactReport.size();	// m_nContactReportCount - m_nContactReportReadPos;

	if( nCount == 0 )	return;

	if( getNetworkIdx() == -1 )
		return;

	for( i = 0; i < nCount; i++ )
	{
//		UINT32 nPos = (m_nContactReportReadPos + i) % PHYSIX_CONTACT_REPORT_COUNT;
//		I3_PHYSIX_CONTACT_REPORT * pReport = &m_ContactReportBuf[ nPos ];
		I3_PHYSIX_CONTACT_REPORT* pReport = m_vecContactReport[i].get();

		switch( pReport->m_Events)
		{
		case NX_NOTIFY_ON_END_TOUCH :
			{
				m_tDropInfo._bContact = false;
			
				GAMEINFO_DROPED_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_DropedWeapon( getNetworkIdx() );
				if( pInfo->_pWeaponRef == nullptr )	continue;
				if( pInfo->getState() == NET_WEAPONDROPED_NONE )		continue;

				pInfo->setState( NET_WEAPONDROPED_FLY);
			}
			break;
		
		case NX_NOTIFY_ON_START_TOUCH :
			if( pReport->m_ContactList.size() > 0)
			{
				GAMEINFO_DROPED_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_DropedWeapon( getNetworkIdx() );
				if( pInfo->_pWeaponRef == nullptr )
					continue;
				if( pInfo->getState() == NET_WEAPONDROPED_NONE )
					continue;

				pInfo->setState( NET_WEAPONDROPED_BOUND);
				pInfo->setDroped( true);

				for( j = 0; j < pReport->m_ContactList.size(); j++ )
				{
					I3_PHYSIX_CONTACT_INFO * pPointInfo = pReport->m_ContactList.at(j);

					if( !m_tDropInfo._bContact && ( i3Vector::Dot( &pPointInfo->m_Normal, &I3_YAXIS ) > I3_EPS + 0.3f ))
					{
						m_tDropInfo._bContact = true;
						m_tDropInfo._bDroped = true;

						i3EventReceiver * pReceiver = pReport->m_pDestShapeSet->getEventReceiver();
						if( i3::kind_of<i3Object*>(pReceiver))
						{
							i3Transform2 * pTrans = (i3Transform2*) i3Scene::FindNodeByExactType( ((i3Object*)pReceiver)->getSceneObject(), i3Transform2::static_meta() );
							if( (pTrans != nullptr) && pTrans->IsAnimated())
							{
								m_tDropInfo._bContact = false;
							}
						}
					}
				}
			}
			break;
		}
	}

	m_vecContactReport.clear();

//	m_nContactReportReadPos += nCount;
}

bool WeaponBase::OnEvent( UINT32 event, i3ElementBase *pObj, UINT32 param, I3_EVT_CODE code)
{	
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		switch( event )
		{
		case I3_EVT_CONTACT :
			if( i3::same_of<i3PhysixShapeSet*>(pObj))
			{
				i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) pObj;
				
				for( INT32 i = 0; i < pShapeSet->getContactCount(); i++ )
				{
					const I3_PHYSIX_CONTACT_REPORT_PTR& spReport = pShapeSet->getContactReport( i );
					
					_QueContactReport( spReport );
				}

				return true;
			}
			break;
		}
	}

	return i3GameObj::OnEvent( event, pObj, param, code);
}

// Contact report
void WeaponBase::CreateContactHitBox( INT32 nUserIndex)
{	
	MATRIX mat;
	VEC3D vDef, vSize;
	INT32 i;

	if( !BattleServerContext::i()->IsP2PActivate() )
		return;

	if( getWeaponInfo() != nullptr && getWeaponInfo()->isNotNeedContactBox())
		return;

	i3Stage * pStage = g_pFramework->GetCurrentStage();
	i3World * pWorld = pStage->getWorld();
	I3ASSERT( pWorld != nullptr );

	i3Skeleton * pSkel = m_pBody->getFrameLOD()->getSkeleton();
	I3ASSERT( pSkel != nullptr );

	i3Matrix::SetPos( &mat, (REAL32)nUserIndex, 10100.f, (REAL32) getWeaponInfo()->GetTypeClass(), 1.f);

	m_pContactHitBox = i3PhysixShapeSet::new_object();

	for( i = 0; i < pSkel->getBoneCount(); i++)
	{
		if( pSkel->isShapeSet( i) == false) continue;
		i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i );
		if( pShapeSet == nullptr)	continue;

		NxShape * pShape = pShapeSet->getNxShape(0);
		const NxConvexShape * pConvexShape = pShape->isConvexMesh();
		if( pConvexShape != nullptr)
		{
			UINT32 nCount = pConvexShape->getConvexMesh().getCount( 0, NX_ARRAY_VERTICES);
			VEC3D * pVerts = (VEC3D*) pConvexShape->getConvexMesh().getBase( 0, NX_ARRAY_VERTICES );

			for( UINT32 j = 0; j < nCount; j++)
			{
				if( getX( &pVerts[j]) > getX( &vSize))
				{
					setX( &vSize, getX( &pVerts[j]));
				}

				if( getY( &pVerts[j]) > getY( &vSize))
				{
					setY( &vSize, getY( &pVerts[j]));
				}
				
				if( getZ( &pVerts[j]) > getZ( &vSize))
				{
					setZ( &vSize, getZ( &pVerts[j]));
				}
			}
		}
	}

	i3Vector::Add( &vSize, 0.01f, 0.01f, 0.01f );
	
	m_pContactHitBox->CreateBoxShape( &vDef, &vSize, CGRP_ITEM, true, &mat);
	m_pContactHitBox->modifyDynamicState( I3_PHYSIX_STATE_NO_GRAVITY | I3_PHYSIX_STATE_NO_RESPONSE | I3_PHYSIX_STATE_NO_RAYCAST | I3_PHYSIX_STATE_NO_COLLISION, 0);

#if defined( I3_DEBUG)
	m_pContactHitBox->SetName( "WeaponContactShapeSet" );
	m_pContactHitBox->getNxShape(0)->setName( "WeaponContactShape");
#endif
	m_pContactHitBox->setEventReceiver( this);

	i3Scene::SetActorPairFlag( pWorld->getWorldCollision()->getInstanceSg(), m_pContactHitBox->getNxActor(), NX_NOTIFY_ON_START_TOUCH );

	if( pStage->GetStageObjectList() != nullptr )
	{
		for( i = 0; i < (INT32)pStage->GetStageObjectList()->size(); i++ )
		{
			i3Object * pObj = (*pStage->GetStageObjectList())[i];
			i3Scene::SetActorPairFlag( pObj->getSceneObject(), m_pContactHitBox->getNxActor(), NX_NOTIFY_ON_START_TOUCH );
		}
	}
}

REAL32 WeaponBase::getCrosshairSpeedFactor(void)
{
	CWeaponInfo * pInfo = GetCurrentWeaponInfo();
	if( pInfo->isEnableFastZoom())
	{
		// Normal 상태에서 Zoom 상태로 바뀐 이후, 시간이 충분히 지났다면
		// SpeedFactor를 순줌이 가능할 수준으로 올려준다.

		if( isWeaponStateMask( WSM_FIRST_ZOOM))
			return 0.0f;
	}
	return pInfo->GetCrossHairSpeedFactor();
}

REAL32	WeaponBase::getCrossHairDeviationTargetTime(CWeaponInfo * pInfo)
{
	// m_pWeaponInfo을 사용하지 않고 WeaponInfo를 따로 받는 이유는
	// Extension무기 때문인듯 합니다.
	I3ASSERT(pInfo != nullptr);

	// 다중무기 교체 시 m_pOwner가 NULL인 경우 발생
	//I3ASSERT(m_pOwner != nullptr);

	REAL32 SpeedFactorEx = 0.f;
	
	if ( m_pOwner != nullptr)
		SpeedFactorEx = m_pOwner->getEquipContext()->GetAbility(EQUIP::eABILITY_CROSSHAIR_SPEEDFACTOR);

	if ( SpeedFactorEx > 0.f)
		return getCrosshairSpeedFactor() * pInfo->GetCrossHairContractTime() * SpeedFactorEx;
	else
		return getCrosshairSpeedFactor() * pInfo->GetCrossHairContractTime();
}

bool WeaponBase::getScopeCrosshairEnable(void)
{
	if( m_pWeaponInfo->isEnableFastZoom())
	{
		// Zoom 되지 않은 상황에서 처음 Zoom을 할 때는 Crosshair를 표시하지 않는다.
		if( isWeaponStateMask( WSM_FIRST_ZOOM))
			return false;
	}

	return true;
}

INT32 WeaponBase::getAutoPointShotCount(void)
{
#if defined( MEM_ENCRYPTION_LV1)
	return DECRYPT_INTVALUE(m_AutoPointShotCount, m_u8EncryptKey);
#else
	return m_AutoPointShotCount;
#endif
}

void WeaponBase::setAutoPointShotCount(INT32 count)
{
#if defined( MEM_ENCRYPTION_LV1)
	GlobalFunc::EncryptValue(&count, m_u8EncryptKey);
#endif

	m_AutoPointShotCount = count;
}

//======================================================================================================//
// 암호화 2010.08.11 순구
INT32 WeaponBase::getAutoFireCount( void)
{
	return m_AutoFireCount;
}

void WeaponBase::setAutoFireCount( INT32 count)
{
	m_AutoFireCount = count;
}

INT32 WeaponBase::getRemainAutoFireCount(void)
{
#if defined( MEM_ENCRYPTION_LV1)
	return DECRYPT_INTVALUE(m_RemainAutoFireCount, m_u8EncryptKey);
#else
	return m_RemainAutoFireCount;
#endif
}

void WeaponBase::setRemainAutoFireCount(INT32 count)
{
#if defined( MEM_ENCRYPTION_LV1)
	GlobalFunc::EncryptValue(&count, m_u8EncryptKey);
#endif

	m_RemainAutoFireCount = count;
}

/*virtual*/ INT32 WeaponBase::getLoadedBulletDualCount( void)
{
	return static_cast<INT32>(m_nLoadedBulletDual);
}

/*virtual*/ void WeaponBase::setLoadedBulletDualCount( INT32 nCount )
{
	REAL32 temp = static_cast<REAL32>(nCount);

	m_nLoadedBulletDual = temp;
}

INT32 WeaponBase::getLoadedBulletCount(void)
{
	return static_cast<INT32>(m_nLoadedBullet);
}

void WeaponBase::setLoadedBulletCount( INT32 nCount )
{
	REAL32 temp = static_cast<REAL32>(nCount);

	m_nLoadedBullet = temp;
}

INT32 WeaponBase::getTotalBulletCount(void)
{
	return m_nMaxBullet;
}

void WeaponBase::setTotalBulletCount( INT32 nCount )
{
	m_nMaxBullet = nCount;
}

///////////////////////REAL32용으로 만듭니다. 
/*virtual*/ REAL32 WeaponBase::getLoadedBulletDualCountF( void)
{
	return m_nLoadedBulletDual;
}

/*virtual*/ void WeaponBase::setLoadedBulletDualCountF( REAL32 nCount )
{
	m_nLoadedBulletDual = nCount;
}

REAL32 WeaponBase::getLoadedBulletCountF(void)
{
	return m_nLoadedBullet;
}

void WeaponBase::setLoadedBulletCountF( REAL32 nCount )
{
	m_nLoadedBullet = nCount;
}


REAL32 WeaponBase::getDamageRate( void)
{
	return m_DamageRate;
}

void WeaponBase::setDamageRate( REAL32 rDamage)
{	
	m_DamageRate = rDamage;
}

void WeaponBase::setZoom( bool bFlag)
{
	if( bFlag)
	{
		removeWeaponStateMask( WSM_FIRST_ZOOM);

		if( isWeaponStateMask( WSM_ZOOM) == false)
		{
			if( m_timeAfterZoom > 1.0f)
			{
				addWeaponStateMask( WSM_FIRST_ZOOM);
			}
		}

		addWeaponStateMask( WSM_ZOOM);
		_RemoveIdleEffect();
	}
	else
	{
		removeWeaponStateMask( WSM_ZOOM);
	}	
}

void WeaponBase::_OnEmitFireEffect( void)
{
	//	발사 Effect
	if( m_pOwner->is1PV() && m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
	{	// 1인칭 뷰의 이팩트를 출력
		_FireEffect( getFXMatrix(), getFireEffectIndex1PVZoom());
	}
	else
	{
		bool bAttachWorld = false;

		_FireEffect( getFXMatrix(), getFireEffectIndex(), bAttachWorld);
	}
}

void WeaponBase::_OnPlayFireSound()
{
	if( m_pOwner->is1PV() && m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM) && getWeaponInfo()->GetTypeZoom() == WEAPON::ZOOM_SCOPE)
		PlayFireSoundByScope();
	else 
		PlayFireSoundByOrder( getFireOrder());
}

INT32 WeaponBase::_OnProcessFireBulletCount( void)
{
	PB_VM_S

	INT32 bullet = getLoadedBulletCount();

	bullet -= m_pWeaponInfo->GetFireDecBulletCount();

	if( bullet < 0)
		bullet = 0;

	if( getWeaponInfo()->GetGaugeReload() == 1)
	{
		if(bullet == 0)
			SetEndurance(0);
	}

	setLoadedBulletCount( bullet);

	return bullet;

	PB_VM_E
}

void WeaponBase::PlayFireSoundByOrder( WEAPON::FIRE_ORDER type /*= WEAPON::PRIMARY_FIRE*/)
{
	// xgkick
	if( getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_MISSION )
		return;

	if( type == WEAPON::SECONDARY_FIRE )
	{
		_PlayFireSound( WEAPON::FIRE_SOUND_NORMAL_2);	//	SECONDARY 사운드
	}
	else
	{
		_PlayFireSound( WEAPON::FIRE_SOUND_NORMAL);		//	일반 사운드
	}
}

void WeaponBase::PlayFireSoundByScope(void)
{
	// xgkick
	if( getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_MISSION )
		return;

	_PlayFireSound( WEAPON::FIRE_SOUND_SCOPE);			//	저격 사운드
}


void WeaponBase::_OnAddTerrainEffect( WEAPON_TERRAIN_EFFECT_INFO * pInfo)
{
	I3ASSERT( pInfo->_pPos != nullptr);
	I3ASSERT( pInfo->_pNormal != nullptr);
	I3ASSERT( pInfo->_pDir != nullptr);

	I3ASSERT_RETURN(m_pWeaponInfo);
	
	if(ObjHitEffIdx == -1) // 가상 함수구조로 크레이트 함수 이상한데다 붙이면 HIT EFF IDX 로드 안될수도있는경우 예외처리
	{
		//히트 이펙트
		i3::rc_wstring	wsObjHitEffect;
		m_pWeaponInfo->GetObjHitEffect(wsObjHitEffect);
		i3::string strEffect;	
		i3::utf16_to_mb(wsObjHitEffect, strEffect);
		ObjHitEffIdx = g_pEffectManager->FindEffect(strEffect.c_str());
		if(ObjHitEffIdx == -1)
			ObjHitEffIdx = -2;


		i3::rc_wstring	wsCharaHitEffect;
		m_pWeaponInfo->GetCharaHitEffect(wsCharaHitEffect);
		i3::utf16_to_mb(wsCharaHitEffect, strEffect);
		CharaHitEffIdx = g_pEffectManager->FindEffect(strEffect.c_str());
		if(CharaHitEffIdx == -1)
			CharaHitEffIdx = -2;

		i3::rc_wstring	wsCharaHitEffect1PV;
		m_pWeaponInfo->GetCharaHitEffect1PV(wsCharaHitEffect1PV);
		i3::utf16_to_mb(wsCharaHitEffect1PV, strEffect);
		CharaHitEff1PVIdx = g_pEffectManager->FindEffect(strEffect.c_str());
		if(CharaHitEff1PVIdx == -1)
			CharaHitEff1PVIdx = -2;

		i3::rc_wstring	wsDecalEffectExt;
		m_pWeaponInfo->GetDecalEffectExt(wsDecalEffectExt);
		i3::utf16_to_mb(wsDecalEffectExt, strEffect);
		DecalEffExtIdx = g_pEffectManager->FindEffect(strEffect.c_str());
		if(DecalEffExtIdx == -1)
			DecalEffExtIdx = -2;

		i3::rc_wstring	wsIdleEffect;
		m_pWeaponInfo->GetIdleEffect(wsIdleEffect);
		i3::utf16_to_mb(wsIdleEffect, strEffect);
		IdleEffIdx = g_pEffectManager->FindEffect(strEffect.c_str());
		if (IdleEffIdx == -1)
			IdleEffIdx = -2;

		if(m_pWeaponInfo->GetUnBlowEffFlag() != 0)
			UnBlowHitEffFlag = true;

		if(m_pWeaponInfo->GetUnDecalEffFlag() != 0)
			UnDecalHitEffFlag = true;

	}

	T_ItemID itemID = m_pWeaponInfo->GetItemID();

	BULLET_EFFECT_TYPE effectType = static_cast<BULLET_EFFECT_TYPE>(m_pWeaponInfo->GetLoadBulletType());

	if (8209011 <= itemID && itemID <= 8209014)
	{
		g_pEffectManager->AddTerrainEffect( pInfo->_terrainType, WEAPON_CLASS_ASSAULT,
			effectType, pInfo->_pPos, pInfo->_pNormal, pInfo->_pDir, pInfo->_rRotate, pInfo->_pObj, UnBlowHitEffFlag, UnDecalHitEffFlag );
	}
	else
	{
		if ( getFireOrder() == WEAPON::PRIMARY_FIRE )
		{
			g_pEffectManager->AddTerrainEffect( pInfo->_terrainType, m_pWeaponInfo->GetTypeClass(),
				effectType, pInfo->_pPos, pInfo->_pNormal, pInfo->_pDir, pInfo->_rRotate, pInfo->_pObj, UnBlowHitEffFlag, UnDecalHitEffFlag );
		}
		else
		{	// revision 60217 칼 부착 가능 총기 ( Weapon With Knife ) 류의 월드 컬리전 이펙트 남기도록 ( 칼 이펙트 )
			g_pEffectManager->AddTerrainEffect( pInfo->_terrainType, WEAPON_CLASS_KNIFE,
				effectType, pInfo->_pPos, pInfo->_pNormal, pInfo->_pDir, pInfo->_rRotate, pInfo->_pObj, UnBlowHitEffFlag, UnDecalHitEffFlag );
		}

		if(DecalEffExtIdx > -1)
		{
			g_pEffectManager->AddDecal( DecalEffExtIdx, pInfo->_pPos, pInfo->_pNormal, pInfo->_pDir, pInfo->_rRotate, pInfo->_pObj);
		}
		if(ObjHitEffIdx > -1)
		{
			g_pEffectManager->AddEffect( ObjHitEffIdx, pInfo->_pPos, pInfo->_pNormal);
		}
		
	}
}

REAL32 WeaponBase::GetPhysixForce( WEAPON_CLASS_TYPE classType)
{
	PHYSIXINFODATA_WEAPON * pPhysixData = &g_pPhysixInfoDataBase->getInfo()->m_WeaponDeath;
	REAL32 rForce = 1.f;

	switch( classType )
	{
	case WEAPON_CLASS_KNIFE			:	rForce *= pPhysixData->m_rWeapon_Knife * 2.5f;	break;
	case WEAPON_CLASS_KNUCKLE		:	rForce *= pPhysixData->m_rWeapon_Knife * 2.5f;	break;
	case WEAPON_CLASS_HANDGUN		:	rForce *= pPhysixData->m_rWeapon_Pistol;		break;
	case WEAPON_SUBCLASS_ASSAULT	:
	case WEAPON_CLASS_ASSAULT		:	rForce *= pPhysixData->m_rWeapon_Assult;		break;
	case WEAPON_SUBCLASS_SMG		:
	case WEAPON_CLASS_SMG			:	rForce *= pPhysixData->m_rWeapon_SMG;			break;
	case WEAPON_SUBCLASS_SNIPER		:
	case WEAPON_CLASS_SNIPER		:	rForce *= pPhysixData->m_rWeapon_Rifle;			break;
	case WEAPON_SUBCLASS_SHOTGUN	:
	case WEAPON_CLASS_SHOTGUN		:	
	case WEAPON_CLASS_DUALSHOTGUN	:	rForce *= pPhysixData->m_rWeapon_Shotgun;		break;
	case WEAPON_CLASS_MG			:	rForce *= pPhysixData->m_rWeapon_Assult;		break;
	case WEAPON_SUBCLASS_BOW		:
	case WEAPON_CLASS_BOW			:	rForce *= pPhysixData->m_rWeapon_Assult;		break;		
	case WEAPON_CLASS_DUALKNIFE		:	rForce *= pPhysixData->m_rWeapon_Knife * 2.5f;	break;
	case WEAPON_CLASS_DUALHANDGUN	:	rForce *= pPhysixData->m_rWeapon_Pistol;		break;
	default :	break;
	}

	return rForce;
}

void WeaponBase::ResetCommand( void)
{
	m_nPushCommandCount = m_nPopCommandCount = 0;
}

INT32 WeaponBase::CommandCount( void)
{
	INT32 nCount = m_nPushCommandCount - m_nPopCommandCount;
	
	return nCount;
}

void WeaponBase::PushCommnad( INT32 nCmd)
{
	if( m_nPushCommandCount - m_nPopCommandCount > MAX_WEAPON_COMMAND_COUNT)
		return;

	INT32 nCur = m_nPushCommandCount % MAX_WEAPON_COMMAND_COUNT;
	m_nCommandBuf[ nCur] = nCmd;
	m_nPushCommandCount++;
}

INT32 WeaponBase::PopCommand( void)
{
	INT32 nCur = m_nPopCommandCount % MAX_WEAPON_COMMAND_COUNT;
	m_nPopCommandCount++;
	return m_nCommandBuf[ nCur];
}

void WeaponBase::GetNetBullet( GAMEINFO_BULLET* pBullet )
{
	pBullet->Reset();
	pBullet->SetLoadBulletCount(	WEAPON::EXTENSION_OFF,	WEAPON::HAND_TYPE_LEFT,	(UINT8)getLoadedBulletCount() );
	pBullet->SetStandbyBulletCount( WEAPON::EXTENSION_OFF,	WEAPON::HAND_TYPE_LEFT,	(UINT16)getTotalBulletCount( ) );
	pBullet->SetLoadBulletCount(	WEAPON::EXTENSION_ON,	WEAPON::HAND_TYPE_LEFT,	(UINT8)getLoadedBulletDualCount( ) );
}

void WeaponBase::SetNetBullet( GAMEINFO_BULLET* pBullet )
{
	setLoadedBulletCount( pBullet->GetLoadBulletCount(		WEAPON::EXTENSION_OFF,	WEAPON::HAND_TYPE_LEFT ) );
	setTotalBulletCount( pBullet->GetStandbyBulletCount(	WEAPON::EXTENSION_OFF,	WEAPON::HAND_TYPE_LEFT ) );
	setLoadedBulletDualCount( pBullet->GetLoadBulletCount(	WEAPON::EXTENSION_ON,	WEAPON::HAND_TYPE_LEFT ) );
}

REAL32 WeaponBase::GetMoveSpeed( WEAPON_MOVE_SPEED_TYPE type)
{
	REAL32 rVal = 0.f;
	CWeaponInfo * pWeaponInfo = m_pWeaponInfo;

	if( isExtensionActivate())
	{
		I3ASSERT( this->getUsedWeapon() != nullptr);
		pWeaponInfo = this->getUsedWeapon()->getWeaponInfo();
	}

	if( i3::kind_of<WeaponGrenadeLauncher*>(this) )
	{
		if( GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER && isExtensionActivate() )
			pWeaponInfo = getExtensionWeaponInfo();
	}

	switch( type)
	{
	case WEAPON_MOVE_SPEED_NORMAL :			rVal = pWeaponInfo->GetMoveNormalStand();		break;
	case WEAPON_MOVE_SPEED_FIRE :			rVal = pWeaponInfo->GetMoveNormalFire();		break;
	case WEAPON_MOVE_SPEED_SIT :			rVal = pWeaponInfo->GetMoveNormalSit();			break;
	case WEAPON_MOVE_SPEED_ZOOM:			rVal = pWeaponInfo->GetMoveNormalZoom();		break;
	}

	if( rVal < 1.f)
		rVal = 1.f;
	
	// 호칭 weaponMoveSpeed 적용
	if (m_pOwner != nullptr)
	{
		const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());
		const DesigApplyInfo* desig_apply_info = pNetInfo->GetDesigApplyInfo();
		if (desig_apply_info )
			rVal += desig_apply_info->desigWeapon.weaponMoveSpeed;
	}
	
	return rVal;
}

REAL32 WeaponBase::GetFireDelay()
{
	// FireDelay가 작은 수치로 변경되도 적용이 안됩니다.
	// 화면 주사율과 네트워크와 관련이 있기 때문입니다.
	// 총을 쏘는 패킷은 몇프레임당 전송이 되고
	// 화면 주사율을 초당 60프레임을 기준으로 한다면 0.015초당 한번씩
	// FireDelay에 대해서 검사를 하기때문에 0.001초 같은 작은 수치는 반영이 안됩니다.
	if( getPairWeapon() != nullptr
		&& getPairWeapon()->getWeaponInfo()->UsePairData() == true )
	{
		return getPairWeapon()->GetFireDelay();
	}

	REAL32 rVal = 0.f;
	
	// revision 35995  스카 스나이퍼의 경우 줌상태와 일반상태의 연사방식을 다르게 수정함
	if (m_pWeaponInfo->GetTypeClass() == WEAPON_CLASS_SNIPER &&
		m_pWeaponInfo->GetNumber() == WEAPON::getItemIndex(WEAPON::SNIPER_SCAR_LB))
	{
		if( !m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
			rVal = m_pWeaponInfo->GetFireDelay();
		else
			rVal = m_pWeaponInfo->GetExtFireDelay();

		//
		// rVal = _CalcFireDelayForDesignation(rVal);	// 호칭에 의한 발사 지연시간. ( 추가적 코드)
		// 12월 버전은 SCAR 스나이퍼에서 줌상태가 아니면 특성치적용 없이 WeaponInfo->GetFireDelay()로 바로 리턴되고..
		// 줌 상태인 경우에는 호칭 특성치 적용만 2배로 적용됨....   ( 어느쪽이 버그인지 알수가 없어서 이건 이대로 방치합니다.)
		// (2015.05.22.수빈)
		//
	}
	else
	{
		rVal = m_pWeaponInfo->GetFireDelay();
	}
	
	return _CalcFireDelayToVersion(rVal);
}

REAL32 WeaponBase::GetFireDelay2()
{
	// FireDelay가 작은 수치로 변경되도 적용이 안됩니다.
	// 화면 주사율과 네트워크와 관련이 있기 때문입니다.
	// 총을 쏘는 패킷은 몇프레임당 전송이 되고
	// 화면 주사율을 초당 60프레임을 기준으로 한다면 0.015초당 한번씩
	// FireDelay에 대해서 검사를 하기때문에 0.001초 같은 작은 수치는 반영이 안됩니다.
	REAL32 rVal = m_pWeaponInfo->GetFireDelay2();

	return _CalcFireDelayToVersion(rVal);
}


REAL32 WeaponBase::GetReloadTime()
{
	I3ASSERT( m_pWeaponInfo != nullptr);

	REAL32 rVal = m_pWeaponInfo->GetReloadTime();

	// Dual 무기일 경우 기준을 오른쪽 무기 (PairWeapon)로 한다.
	if( getPairWeapon() != nullptr)
		rVal = getPairWeapon()->getWeaponInfo()->GetReloadTime();

	return _CalcReloadTimeToVersion(rVal);
}

REAL32 WeaponBase::GetRecoilHorzAngle()
{	
	REAL32 fRate = 0.0f;
	REAL32 rVal = m_pWeaponInfo->GetRecoilHorzAngle();
	if (m_pOwner == nullptr)
	{
		return rVal;
	}
	
	// 호칭 적용 여부를 잘 모르겠다....실제 호칭에는 없긴 한데...일단 구현해놓는다...( 0.01을 곱하면 너무 작고, 그렇지 않은 경우 다소 큰값이라서 문제이긴 하다..)
	if (m_pOwner)
	{
		const NET_CHARA_INFO* netCharaInfo = BattleSlotContext::i()->getNetCharaInfo(m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType() );
		if (netCharaInfo->GetDesigApplyInfo())
		{
			fRate = netCharaInfo->GetDesigApplyInfo()->desigWeapon.weaponHorzReaction;
			fRate += netCharaInfo->GetDesigApplyInfo()->desigWeapon.weaponReaction * 0.01f;
		}
	}

	rVal += (rVal * fRate); //*0.01f);

#if defined( USE_WEAPON_ENDURANCE)
	if( UserInfoContext::i()->IsNewRepair() )
	{
		if( !getOwner()->isAI())
			CalEndurancePenalty( PENALTY_HORZRECOIL, m_ui8Endurance, rVal );
	}
	else
	{
		if( IsEnduranceZero() && !getOwner()->isAI() )
			rVal = CGameWeaponManager::WeaponEnduranceZeroHorzRecoil( rVal);
	}
#endif

	return rVal;
}


REAL32 WeaponBase::GetRecoilVertAngle()
{
	REAL32 fRate = 0.0f;
	REAL32 rVal = m_pWeaponInfo->GetRecoilVertAngle();
	if( m_pOwner == nullptr)
	{
		return rVal;
	}

	// 호칭 적용 여부를 잘 모르겠다....일단 구현해놓는다...실제 호칭에는 없긴 한데...일단 구현해놓는다...( 0.01을 곱하면 너무 작고, 그렇지 않은 경우 다소 큰값이라서 문제이긴 하다..)
	if (m_pOwner)
	{
		const NET_CHARA_INFO* netCharaInfo = BattleSlotContext::i()->getNetCharaInfo(m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType() );
		if (netCharaInfo->GetDesigApplyInfo())
		{
			fRate = netCharaInfo->GetDesigApplyInfo()->desigWeapon.weaponVertReaction;
			fRate += netCharaInfo->GetDesigApplyInfo()->desigWeapon.weaponReaction * 0.01f;
		}
	}

	rVal = rVal + (rVal * fRate);

#if defined( USE_WEAPON_ENDURANCE)
	if( UserInfoContext::i()->IsNewRepair() )
	{
		if( !getOwner()->isAI())
			CalEndurancePenalty( PENALTY_VERTRECOIL, m_ui8Endurance, rVal );
	}
	else
	{
		if( IsEnduranceZero() && !getOwner()->isAI() )
			rVal = CGameWeaponManager::WeaponEnduranceZeroVertRecoil( rVal);
	}
#endif

	return rVal;
}

REAL32 WeaponBase::GetDeviation()
{
	REAL32 rVal = m_pWeaponInfo->GetDeviation();

	if( m_pOwner != nullptr)
	{
		// 기획 협의후, pef 초기값 기준으로 사다리 상태에서 디비에이션 값에 페널티 비율을 곱한다...
		
		if (m_pOwner->isCharaStateMask(CHARA_STATE_CLIMB) )
		{
			rVal *= g_pConfigEx->GetLadderPenaltyRate_Deviation();			
		}

		// SMG류는 지정된 거리 이하에서 다른 캐릭터를 조준한 상태라면
		// 편차를 감소 처리해야 한다.
		if( m_pWeaponInfo->GetTypeClass() == WEAPON_CLASS_SMG)
		{
			REAL32 factor = m_pWeaponInfo->GetDeviation_Decrement();

			if( factor > 0.0f)
			{
				if( (m_pOwner->getCollisionContext() != nullptr) && (m_pOwner->getCollisionContext()->getCrossHitDistance() > 0.0f))
				{
					REAL32 dist = m_pOwner->getCollisionContext()->getCrossHitDistance();

					if( dist <= m_pWeaponInfo->GetDeviation_DiminutionDistance())
					{
						rVal *= factor;
					}
				}
			}
		}
		
		rVal = _CalcDeviationToVersion(rVal);
		//I3TRACE( "Deviation %.4f + %.4f\n", m_pWeaponInfo->GetDeviation(), rVal - m_pWeaponInfo->GetDeviation());

		
#if defined( USE_WEAPON_ENDURANCE)
		if (UserInfoContext::i()->IsNewRepair())
		{
			if (!getOwner()->isAI())
				CalEndurancePenalty(PENALTY_DEVIATION, m_ui8Endurance, rVal);
		}
#endif

	}
	
	return rVal;
}

REAL32 WeaponBase::GetRange()
{
	if( getPairWeapon() != nullptr
		&& getPairWeapon()->getWeaponInfo()->UsePairData() == true )
	{
		return getPairWeapon()->GetRange();
	}

	REAL32 rVal = m_pWeaponInfo->GetRange();

	if( m_pOwner != nullptr )
	{
		// 장비 속성
		if ( m_pOwner->getEquipContext() )
			rVal += (rVal * m_pOwner->getEquipContext()->GetAbility( EQUIP::eABILITY_RANGE));

		// 스킬 속성.
		const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());
		REAL32 ratio = pNetInfo->getWeaponSkillValue(getWeaponInfo()->GetTypeClass(), SKILLLIST_RANGE);
		rVal += ratio;
		
		// 호칭 적용
		const DesigApplyInfo* desig_apply_info = pNetInfo->GetDesigApplyInfo();
		if (desig_apply_info)
			rVal += desig_apply_info->desigWeapon.weaponAttackRange;
	}

	return rVal;
}

REAL32 WeaponBase::GetDPD_Rate()
{
	REAL32 rVal = m_pWeaponInfo->GetDPD_Rate();

	return rVal;
}

REAL32 WeaponBase::GetDamage( bool bRandomDamage)
{
	I3ASSERT_RETURN( m_pOwner != nullptr, 0.f);
	
	REAL32 AddDamage = 0.f;

	if(m_pOwner->getEquipContext() != nullptr)
	{
		if( !i3::same_of<WeaponWPSmoke*>(this) )	// revision 36503 // WP Smoke 계열은 마스크 데미지를 적용시키지 않습니다.
			AddDamage = m_pOwner->getEquipContext()->GetAbility( EQUIP::eABILITY_DAMAGE);
	}

	REAL32 rVal =0.f;
	// 36005 스카 익스텐션 상태시 데미지 값을 변경하도록 함
	if (m_pWeaponInfo->GetTypeClass() == WEAPON_CLASS_SNIPER &&
		m_pWeaponInfo->GetNumber() == WEAPON::getItemIndex(WEAPON::SNIPER_SCAR_LB))
	{
		if( !m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
			rVal =	(REAL32) m_pWeaponInfo->GetDamage() + AddDamage;
		else
			rVal =	(REAL32) m_pWeaponInfo->GetExtDamage() + AddDamage;
	}
	else
	{
		rVal =	(REAL32) m_pWeaponInfo->GetDamage() + AddDamage;
	}

	REAL32 rBasisDamage = rVal;

	WEAPON_CLASS_TYPE nWeaponClass = m_pWeaponInfo->GetTypeClass();

	switch( nWeaponClass)
	{	
	// 칼은 랜덤데미지 적용되지 않습니다.
	case WEAPON_CLASS_KNIFE:
	case WEAPON_CLASS_DUALKNIFE:
	case WEAPON_CLASS_KNUCKLE:
		{						
			switch( m_FireOrder)
			{
			case WEAPON::PRIMARY_FIRE:
				{
					rVal = m_pWeaponInfo->GetDamageA( getAutoFireCount());		// 왼쪽 공격 데미지
				}
				break;
			case WEAPON::SECONDARY_FIRE :
				{
					rVal = m_pWeaponInfo->GetDamageB( getAutoFireCount());		//	오른쪽 공격 데미지
				}
				break;
			}
		} break;
	//	폭발류 또는 투척류도 랜덤 데미지에서 제외.
	case WEAPON_CLASS_THROWING_GRENADE:
	case WEAPON_CLASS_THROWING_CONTAIN:
	case WEAPON_CLASS_MISSION:
	case WEAPON_CLASS_GRENADESHELL:
	case WEAPON_CLASS_ROCKET_LAUNCHER:
	case WEAPON_CLASS_THROWING_ATTACK:
	case WEAPON_CLASS_THROWING_HEAL:
		{
			//	기존 데미지로...
		} break;
	////////////////////////////////////////////	그외 총기류는 모두 RandomDamage 적용.
	default:
		{
			if( m_pDamage)
				rVal = m_pDamage->Get( rVal, AddDamage, bRandomDamage);
		} break;
	}

	//	총알 발사 계열 무기만 Ammo 캐시 아이템이 적용되도록
	if( (nWeaponClass == WEAPON_CLASS_CIC && m_FireOrder == WEAPON::PRIMARY_FIRE)	//	CIC 일경우 SECONDARY는 Knife 공격이기 때문에
		|| m_pWeaponInfo->isGun())
	{
		//아이템 데미지 증가
		REAL32 ratio = BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_DAMAGE);
		CASHITEM_TRACE("CASHITEMSKILL_DAMAGE = %f\n", ratio);
		rVal += (rBasisDamage * ratio);

		// PBTN Mask 예외처리. (V1에서 중복 계산중이였습니다.)
		{
			const CEquipInfo * pFaceInfo = m_pOwner->getEquipContext()->getEquipInfo(EQUIP::ePART_FACEGEAR);
			if (pFaceInfo != nullptr && pFaceInfo->GetITEMID() == 800169)
			{
				ratio = pFaceInfo->GetAbilityValue(EQUIP::eABILITY_DAMAGERATE);
				ratio += (ratio > 0 ? I3_EPS : -I3_EPS);
				rVal += (rBasisDamage * ratio);
			}
		}
	}

	if( m_pOwner->getCharaNetIndex() != -1)
	{	
		const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());

		//// designation ( V1용 잔존코드로 복구. 다만, 사용하는 호칭이 없어 주석 처리합니다.)
		//{
		//	if (pNetInfo->GetDesigApplyInfo())
		//		rVal += pNetInfo->GetDesigApplyInfo()->desigWeapon.weaponDamage;
		//}

		// skill
		{
			REAL32 ratio = pNetInfo->getWeaponSkillValue(getWeaponInfo()->GetTypeClass(), SKILLLIST_DAMAGE);
			rVal += rVal * ratio;
		}

		if( m_pOwner->getEquipContext() != nullptr)
		{
			rVal += (rVal * m_pOwner->getEquipContext()->GetAbility( EQUIP::eABILITY_DAMAGERATE));
		}
	}

	//I3TRACE( "Damage %.4f + %.4f\n", m_pWeaponInfo->GetDamage(), rVal - m_pWeaponInfo->GetDamage());



#if defined( USE_WEAPON_ENDURANCE)
	if( UserInfoContext::i()->IsNewRepair() )
	{
		if( !getOwner()->isAI())
			CalEndurancePenalty( PENALTY_DAMAGE, m_ui8Endurance, rVal );
	}
	else
	{
		if( IsEnduranceZero() && !getOwner()->isAI())
			rVal = CGameWeaponManager::WeaponEnduranceZeroDamage( rVal);
	}
#endif

	return rVal;
}

REAL32 WeaponBase::GetScopeFOV()
{
	return m_pWeaponInfo->GetScopeFOV();
}

REAL32 WeaponBase::GetScopeFOV2()
{
	return m_pWeaponInfo->GetScopeFOV2();
}

REAL32 WeaponBase::GetThrowSpeed()
{
	I3ASSERT( m_pWeaponInfo != nullptr);
	REAL32 rVal = 0.f;
	rVal = m_pWeaponInfo->GetThrowSpeed();

	const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());

	REAL32 ratio = pNetInfo->getWeaponSkillValue(getWeaponInfo()->GetTypeClass(), SKILLLIST_BOOMLENGTH);
	rVal += ratio; // rVal * ratio;			// 곱하지 않고 그대로 더하는 것으로 변경..(기획 요청)..2015.05.22.수빈...

	if (pNetInfo->GetDesigApplyInfo() )
	{
		const DesigApplyInfo* desig_apply_info = pNetInfo->GetDesigApplyInfo();
		rVal += desig_apply_info->desigWeapon.weaponThrowSpeed;				// 12월버전에서는 그냥 더하게 되있는데..그 코드 그대로 반영함..
	}

	return rVal;
}

REAL32 WeaponBase::GetExThrowSpeed()
{
	CWeaponInfo * pWeaponInfo = m_pWeaponInfo;
	if( i3::kind_of<WeaponGrenadeLauncher*>(this) )
		pWeaponInfo = getExtensionWeaponInfo();

	REAL32 rVal = pWeaponInfo->GetExThrowSpeed();
	if( m_pOwner != nullptr)
	{
		const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());
		
		REAL32 ratio = pNetInfo->getWeaponSkillValue(getWeaponInfo()->GetTypeClass(), SKILLLIST_BOOMLENGTH);
		rVal += ratio; // rVal * ratio;			// 곱하지 않고 그대로 더하는 것으로 변경..(기획 요청)..2015.05.22.수빈...

//		if (pNetInfo->getSkillsystem(0) != nullptr)
//		{
//			rVal += pNetInfo->getSkillsystem(0)->getWeaponThrowSpeed();
//			rVal += pNetInfo->getSkillsystem(1)->getWeaponThrowSpeed();
//			rVal += pNetInfo->getSkillsystem(2)->getWeaponThrowSpeed();				
//		}
		
		if ( pNetInfo->GetDesigApplyInfo() )
		{
			const DesigApplyInfo* desig_apply_info = pNetInfo->GetDesigApplyInfo();
			rVal += desig_apply_info->desigWeapon.weaponThrowSpeed;				// 12월버전에서는 그냥 더하게 되있는데..그 코드 그대로 반영함..
		}

	}
	return rVal;
}

REAL32 WeaponBase::GetExplosionTime()
{
	REAL32 rVal = m_pWeaponInfo->GetExplosion_Time();

	if( m_pOwner != nullptr)
	{
		const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());
		REAL32 ratio = pNetInfo->getWeaponSkillValue(getWeaponInfo()->GetTypeClass(), SKILLLIST_BOOMTIME);
		rVal -= rVal * ratio;
		
		// 호칭 적용...
		if (pNetInfo->GetDesigApplyInfo())
			rVal += pNetInfo->GetDesigApplyInfo()->desigWeapon.weaponBombTime;
	}

	return MINMAX(0.f, rVal, 100.f);
}

REAL32 WeaponBase::GetExplosionRange()
{
	REAL32 rVal = m_pWeaponInfo->GetExplosion_Range();

	if( m_pOwner != nullptr)
	{
		const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());
		REAL32 ratio = pNetInfo->getWeaponSkillValue(getWeaponInfo()->GetTypeClass(), SKILLLIST_BOOMRANGE);
		rVal += ratio;

		//호칭 적용..
		if (pNetInfo->GetDesigApplyInfo())
			rVal += pNetInfo->GetDesigApplyInfo()->desigWeapon.weaponBombRange;
	}

	I3ASSERT( rVal > 0.f);
	return rVal;
}

REAL32 WeaponBase::GetExplosionNearRange()
{
	return m_pWeaponInfo->GetExplosion_NearRange();
}

REAL32 WeaponBase::GetDetectedRange()
{
	return m_pWeaponInfo->GetDetected_Range();
}

INT32 WeaponBase::GetMaxBulletWithCashItem(void)
{
	CWeaponInfo * pInfo = getWeaponInfo();
	I3ASSERT( pInfo != nullptr);

	INT32 count = pInfo->GetMaxBullet();
	REAL32 ability = 1.0f;

	// 탄수 증가 공식 아이템부터 계산 뒤 파츠 능력치 계산
	if( m_pOwner != nullptr)
	{
		// 보유탄수 증가권이 있는지를 확인하여 보유탄수 40% 증가 (자신 사용)
		if(	BattleSlotContext::i()->isUseCashItemInSlot( m_pOwner->getCharaNetIndex(), SLOT_ITEM_AMMO_UP) == true )
		{
			// 주무기일때만 적용
			if( m_pWeaponInfo->GetTypeUsage() == WEAPON_SLOT_PRIMARY )
			{
				// 일반 KrissS.V.에서 듀얼로 전환되는 Dual KrissS.V.는 기획상 적용에서 제외.
				if( !(i3::same_of<WeaponShotGunGenderWithDual*>(getPairWeapon()) == true || i3::same_of<WeaponKrissSuperV*>(getPairWeapon()) == true) )
				{				
					ability += WEAPON_BULLET_INCREASING_COUNT_40;
				}
			}
		}

		// Grenade 탄수 증가 아이템 계산
		if (pInfo->GetTypeUsage() == WEAPON_SLOT_THROWING1)
		{
			if ( m_pOwner && BattleSlotContext::i()->isUseCashItemInSlot( m_pOwner->getCharaNetIndex(), SLOT_ITEM_INCREASE_GRENADE_SLOT))
			{
				count += pInfo->GetIncreaseCount();
			}
		}
		else if (pInfo->GetTypeUsage() == WEAPON_SLOT_THROWING2)
		{
			if (m_pOwner && BattleSlotContext::i()->isUseCashItemInSlot(m_pOwner->getCharaNetIndex(), SLOT_ITEM_INCREASE_THR2_SLOT))
			{
				count += pInfo->GetIncreaseCount();
			}
		}

		// 파츠 능력치 추가
		switch( m_pWeaponInfo->GetTypeClass() )
		{
			//case WEAPON_CLASS_HANDGUN :
		case WEAPON_SUBCLASS_ASSAULT	:
		case WEAPON_CLASS_ASSAULT		:
		case WEAPON_SUBCLASS_SMG		:
		case WEAPON_CLASS_SMG			:
		case WEAPON_SUBCLASS_SNIPER		:
		case WEAPON_CLASS_SNIPER		:
		case WEAPON_SUBCLASS_SHOTGUN	:
		case WEAPON_CLASS_SHOTGUN		:
		case WEAPON_CLASS_MG			:
			//case WEAPON_CLASS_CIC :
			{
				//count = count + (INT32) (m_pOwner->getEquipContext()->GetAbility( EQUIP::eABILITY_MAXBULLET) * count);
				ability += m_pOwner->getEquipContext()->GetAbility( EQUIP::eABILITY_MAXBULLET);
			}
			break;

			//case WEAPON_CLASS_DUALHANDGUN :
		case WEAPON_CLASS_DUALSMG :
			{
				// 일반 KrissS.V.에서 듀얼로 전환되는 Dual KrissS.V.는 기획상 적용에서 제외.
				if( i3::same_of<WeaponKrissSuperV*>(getPairWeapon()))
					break;

				// 듀얼무기는 각 무기별로 증가율을 계산. (전체 발수 * 0.5 적용)
				INT32 nHarfCount = static_cast<INT32>(count * 0.5f);

				// 증가율
				ability += m_pOwner->getEquipContext()->GetAbility( EQUIP::eABILITY_MAXBULLET);

				// 정해진 무기별 탄수에, 듀얼을 적용 (무기별 증가 탄수 * 2 적용)
				count = static_cast<INT32>( nHarfCount * ability ) * 2;

				// Fix된 Bullet Count
				ability = 1.0f;
			}
			break;

		case WEAPON_CLASS_THROWING_GRENADE :
			{
				if( m_pWeaponInfo->GetDual())
					count = count + (INT32) (m_pOwner->getEquipContext()->GetAbility( EQUIP::eABILITY_THROWCOUNT) * 2);
				else
					count = count + (INT32) m_pOwner->getEquipContext()->GetAbility( EQUIP::eABILITY_THROWCOUNT);
			}
			break;

		default :	// 다른 무기는 적용하지 않음.
			break;
		}
	}

	return static_cast<INT32>(count * ability);
}

INT32 WeaponBase::GetMaxBulletWithSkill(void)
{
	CWeaponInfo * pInfo = getWeaponInfo();
	I3ASSERT( pInfo != nullptr);
	INT32 count = pInfo->GetMaxBullet();

	if( m_pOwner && m_pOwner->isLocal())
	{
		// 주무기일때만 적용
		if( m_pWeaponInfo->GetTypeUsage() == WEAPON_SLOT_PRIMARY )
		{
			if( i3::same_of<WeaponKrissSuperV*>(getPairWeapon()) )
			{
				// 일반 KrissS.V.에서 듀얼로 전환되는 Dual KrissS.V.는 기획상 적용에서 제외.
			}
		}
	}


	return count;
}

INT32 WeaponBase::GetLoadBulletWithCashItem(void)
{
	CWeaponInfo * pInfo = getWeaponInfo();
	I3ASSERT( pInfo != nullptr);

	INT32 count = pInfo->GetLoadBullet();

	// 월드에 붙는 수류탄인 경우 그냥 원래 값으로 리턴
	if( m_pOwner != nullptr 
#ifdef DOMI_STING_POISON_GRENADE
		&& !(i3::kind_of<CGameCharaFullbody*>(m_pOwner))
#endif
		)
	{
		if( m_pOwner->getCharaNetIndex() == 0xffffffff)
			return count;

		//11766 노현서, 수류탄은 Max Count 에만 +1 증가 처리합니다 
		if( IsWeaponByIncreaseGrenadeSlot( pInfo->GetTypeClass(), pInfo->GetRealNumber()))
		{
			count += static_cast<INT32>(BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_GRENADE_ADD));
			CASHITEM_TRACE("CASHITEMSKILL_GRENADE_ADD = %f\n", BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_GRENADE_ADD));
		}
		else if( IsWeaponByIncreaseDualGrenadeSlot( pInfo->GetTypeClass(), pInfo->GetRealNumber()))
		{
			count += static_cast<INT32>(BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_GRENADE_ADD))*2;
			CASHITEM_TRACE("CASHITEMSKILL_GRENADE_ADD = %f\n", BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_GRENADE_ADD));
		}
	}

	return count;
}

void WeaponBase::_OnProcessFireFirstAnim()
{
	// 연이어 발사하지 못하도록...
	if( m_pOwner->isLocal() || m_pOwner->isAI())
	{
		// 재장전 뒤 바로 Fire를 하게 되면 여기서 초기화가 이루어 져야 함.
		m_pOwner->ResetNetworkAttackCount();
		addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
	
		if( m_bSetStart )	FireCollision( m_StartPos, m_TargetPos);
		else				FireCollision( nullptr, m_TargetPos);

#if defined( FIREDELAY_COST)
		if( !isWeaponStateMask( WSM_MORE_FIRING))
		{
			m_rFireTime = 0.f;
		}

		removeWeaponStateMask( WSM_MORE_FIRING);
#endif
	}
	// 발사 이팩트
	_OnEmitFireEffect();
	
	// 발사 사운드
	_OnPlayFireSound();
	// 탄환을 계산합니다.
	// 탄환 체크 코드 추가 2010.09.07 순구
	INT32 bullet = getLoadedBulletCount();

	if( GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER && isExtensionActivate())
	{
		bullet = ((WeaponShotGunLauncher*)this)->getLoadedShellCount();
	}
	else if( getMagazineType() == WEAPON_DUAL_MAGAZINE_RIGHT)
	{
		bullet = getLoadedBulletDualCount();
	}

	if( bullet <= _OnProcessFireBulletCount())
	{
		if( (bullet > 0) && (m_pOwner != nullptr) && (m_pOwner->isLocal()))
		{
			REAL32 rError = 51.f;
			UINT32 nRemainBattleTime = (UINT32)UserContext::i()->fRemainingBattleTime;

			GameEventSender::i()->SetEvent( EVENT_BATTLE_REMAINTIME_N, &nRemainBattleTime, &rError);
		}
	}

	// 발사 한 경우에 Load Mag을 했다는 flag를 끕니다. 그래야 탄창이 안찹니다.
	removeWeaponStateMask(WSM_EMPTY_CARTRIDGE | WSM_LOADMAG_TO_NEEDBULLET);
}

void WeaponBase::_OnProcessFireEndAnim( REAL32 rDelay)
{
	I3ASSERT( m_pOwner != nullptr);
	I3ASSERT( m_pWeaponInfo != nullptr);

	if( m_pWeaponInfo->GetPumpAction())	//	펌프액션 방식이면
	{
		if( getLoadedBulletCount() > 0 )
		{
			addWeaponStateMask( WSM_LOADED_BULLET);
			removeWeaponStateMask( WSM_FIRE_DISABLE);

			// 발사후 볼트액션 동작이 있으면
			if( IsFireBulletWeapon())  
			{
				setWeaponState( WEAPON::STATE_NEEDTO_LOADBULLET);
				removeWeaponStateMask( WSM_LOADBULLET_DISABLE);			
			}
		}
		else
		{
			removeWeaponStateMask( WSM_LOADED_BULLET);
			if( (m_pOwner->isLocal() || m_pOwner->isAI()))	addWeaponStateMask( WSM_FIRE_DISABLE);
		}

		_OnPlayWeaponIdleAnim();
	}
	else
	{
		if( m_pWeaponInfo->GetNeedToLoadBullet() && (getLoadedBulletCount() > 0))  
		{
			setWeaponState( WEAPON::STATE_NEEDTO_LOADBULLET);
			removeWeaponStateMask( WSM_LOADBULLET_DISABLE);			
		}
		else
		{	
			if( m_pWeaponInfo->GetNeedToLoadBullet() && (getLoadedBulletCount() > 0))  
			{
				setWeaponState( WEAPON::STATE_NEEDTO_LOADBULLET);
				removeWeaponStateMask( WSM_LOADBULLET_DISABLE);			
			}
			else
			{	
				if( isWeaponStateMask( WSM_FIRING ) ||
					rDelay >= m_pOwner->getAnimContext()->getUpperDuration() )
				{
					_OnPlayWeaponIdleAnim();

					if (getLoadedBulletCount() <= 0)
					{
						if (getTotalBulletCount() > 0)
						{
							setWeaponState(WEAPON::STATE_NEEDTO_LOADMAG);		// 자동 재장전
							removeWeaponStateMask(WSM_LOADMAG_DISABLE);
						}
						else
							setWeaponState(WEAPON::STATE_EMPTY_MAGAZINE);

						if (m_pOwner->isLocal()) addWeaponStateMask(WSM_FIRE_DISABLE);
					}
					else
					{
						setWeaponState(WEAPON::STATE_READY);
						//2015-03-18 ENG_김대영 
						//기존 코드에서 이부분에 연발 사격 가능 총기는 강제적으로 웨폰을 FIRE 상태로 바꾸는 코드가 적혀져 있었음.
						//추정되는 이유는 GameCharaAnimControl에서 OnAttackRun함수에서 애니메이션 시간이 끝나면 바로 Cmd_Return_Upper 함수 호출 해버림.
						//그렇게 될 경우 연발사격이 불가능해짐.( OnAttackRun에서 발사 함수 호출하기전 해당 상태가 바뀌어서 OnAttackRun 함수가 호출되지 않음.) 
						//따라서 기존에 강제로 상태를 바꾸어 연발이 가능하도록 했었는데, 해당 방법은 무기가 계속해서 파이어 상태로 유지 되어버려 다른 루틴에서 정상적으로 작동 안 할수 있는 문제가 있어 바꿉니다. 
						//(OnAttackRun도 연발 무기만 예외처리하도록 수정)
					}

				}
				else
				{
					if( getLoadedBulletCount() <= 0)
					{
						setWeaponState( WEAPON::STATE_READY );
						_OnPlayWeaponIdleAnim();
					}
					else
					{
						removeWeaponStateMask( WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
						setWeaponState( WEAPON::STATE_FIRING );
					}
				}
			}
		}
	}
}

REAL32	WeaponBase::_CalcFireDelayForDesignation(REAL32 rDelay)
{
	if( m_pOwner != nullptr)
	{
		const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());	
		
		if ( pNetInfo->GetDesigApplyInfo() )
			rDelay += pNetInfo->GetDesigApplyInfo()->desigWeapon.weaponFireSpeed;
	}

	return rDelay;
}

REAL32 WeaponBase::_CalcFireDelayForSkill(REAL32 rDelay)
{
	if( m_pOwner != nullptr)
	{
		const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());
		REAL32 ratio = pNetInfo->getWeaponSkillValue(getWeaponInfo()->GetTypeClass(), SKILLLIST_FIRESPEED);
		rDelay -= rDelay * ratio;
	}

	return rDelay;
}

REAL32 WeaponBase::_CalcFireDelayForEquip(REAL32 rDelay)
{
	if(m_pOwner != nullptr && m_pOwner->getEquipContext() != nullptr)
	{
		REAL32 ratio = m_pOwner->getEquipContext()->GetAbility( EQUIP::eABILITY_FIREDELAY);
		rDelay -= rDelay * ratio;
	}

	return rDelay;
}

void WeaponBase::_OnProcessFireMiddleAnim( REAL32 rDelay)
{
	I3ASSERT( m_pOwner != nullptr);

	//	탄피 Effect. 볼트 액션식이 아닌 일반 탄창식 총기일 경우
	//	샷건과 볼트 액션식 총기를 제외한 일반 총기는 발사하자마자 바로 탄피 발사
	if( !isWeaponStateMask( WSM_EMPTY_CARTRIDGE) && (m_idxBulletEffect != -1) && (m_idxFXBullet != -1) )
	{
		if( m_pWeaponInfo->GetNeedToLoadBullet() == false )				addWeaponStateMask( WSM_EMPTY_CARTRIDGE);
		else if( m_pWeaponInfo->GetPumpAction() && rDelay > 0.7f )		addWeaponStateMask( WSM_EMPTY_CARTRIDGE);

		//기획팀 요청으로 잭해머는 탄피이펙트를 사용하지 않습니다.
		if( isWeaponStateMask( WSM_EMPTY_CARTRIDGE) &&
			!m_pWeaponInfo->isNoExjectShell())
		{
			MATRIX * pMtx = getSceneObject()->GetBody()->getCacheMatrix( m_idxFXBullet);

			_EmptyCartridgeEffect( m_idxFXBullet, m_idxBulletEffect);
			I3ASSERT( m_pOwner->getMoveContext() != nullptr);
			_AddEmptyCartridgeSound( m_pOwner->getMoveContext()->getFloorTerrainType(), DELAY_EMPTYBULLETSOUND, i3Matrix::GetPos( pMtx) );
		}
	}
}

/*virtual*/ void WeaponBase::SetBulletCountForNetwork( INT32 iLoadedCount, INT32 iDualLoadedCount, INT32 iMaxCount)
{
	I3ASSERT( m_pWeaponInfo != nullptr);

	INT32 iMaxLoadCount =  GetMaxBulletWithCashItem();
	
	if( GetLoadBulletWithCashItem() < iLoadedCount)
		iLoadedCount = GetLoadBulletWithCashItem();
	setLoadedBulletCount( iLoadedCount );

	if( GetLoadBulletWithCashItem() < iDualLoadedCount)
		iDualLoadedCount = GetLoadBulletWithCashItem();
	setLoadedBulletDualCount( iDualLoadedCount);

	if( iMaxLoadCount < iMaxCount )
		iMaxCount = iMaxLoadCount;
	setTotalBulletCount( iMaxCount);
}

/*virtual*/ void WeaponBase::ApplyBulletCountWithCashItem()
{
	I3ASSERT( m_pWeaponInfo != nullptr);

	setLoadedBulletCount(getLoadedBulletCount());
	setLoadedBulletDualCount(getLoadedBulletDualCount());

	INT32 iMaxLoadCountDefault	= m_pWeaponInfo->GetMaxBullet();
	INT32 iMaxLoadCountWithCash = GetMaxBulletWithCashItem();
	INT32 iIncreaseCount		= iMaxLoadCountWithCash - iMaxLoadCountDefault;

	setTotalBulletCount(getTotalBulletCount() + iIncreaseCount);
}

/*virtual*/ void WeaponBase::ApplyBulletCountWithSkill()
{
	I3ASSERT( m_pWeaponInfo != nullptr);

	setLoadedBulletCount(getLoadedBulletCount());
	setLoadedBulletDualCount(getLoadedBulletDualCount());

//	INT32 iMaxLoadCountDefault	= m_pWeaponInfo->GetMaxBullet();
//	INT32 iMaxLoadCountWithSkill = GetMaxBulletWithSkill();
//	INT32 iIncreaseCount		= iMaxLoadCountWithSkill - iMaxLoadCountDefault;
	
	setTotalBulletCount(getTotalBulletCount() + 300);
}

/*virtual*/ void WeaponBase::ApplyBulletWithDropItem(REAL32 rRate)
{
	I3ASSERT( m_pWeaponInfo != nullptr);

	CGameCharaBase* chara = getOwner();
	I3ASSERT( chara != nullptr);

	INT32 lv = DomiUserSkillMgr::i()->GetLevel(chara->getCharaNetIndex(), USER_SKILL_MAGAZINE_UP);
	REAL32 addupStat = domi::SkillProp::i()->GetUserSkill(USER_SKILL_MAGAZINE_UP).GetIncrRatio(lv);


	//유탄 계열은 25% 탄약을 채울 경우 증가량이 0.xx 이기 때문에 올림 함. hansoft.4640
	struct round_up
	{	
		REAL32 operator()(REAL32 f)
		{
			return ((INT32)((f + 0.9f)/1.f))*1.f;
		}
	}rounder;


	INT32 max = m_pWeaponInfo->GetMaxBullet();			//최대 수.
	INT32 increase = (INT32)rounder((max*rRate));		//증가 수.
	INT32 current = getTotalBulletCount();				//현재 수.

	increase += current;
	if( increase > static_cast<INT32>(GetMaxBulletWithCashItem() * addupStat))
		increase = static_cast<INT32>(GetMaxBulletWithCashItem() * addupStat);

	setTotalBulletCount( increase);
}

void WeaponBase::SetEnable( bool bTrue, REAL32 tm)
{
	i3GameNode::SetEnable( bTrue, tm);

	m_mainWeapon->SetEnable(bTrue, tm);
}

void WeaponBase::Instantiate( void)
{
	MATRIX mat;
	I3_ONINSTANCING_INFO info;
	info._pBase = this;
	info._pMatrixStack = g_pFramework->GetViewer()->GetSceneTracer()->GetModelViewMatrixStack();
	
	i3Matrix::SetPos( &mat, 0.f, g_pConfigEx->GetPhysix().HeightWeapon, 0.f);
	info._pMatrixStack->Push( &mat, nullptr);
	OnInstancing( &info);
	info._pMatrixStack->Pop();
}

void WeaponBase::DumpState( void)
{
	i3::rc_wstring wstrWeaponName;
	getWeaponInfo()->GetWeaponName(wstrWeaponName);
	
	I3PRINTLOG(I3LOG_NOTICE, "<< Dump Weapon >>");
	I3PRINTLOG(I3LOG_NOTICE, "- Name : %s (Class %d, No. %d)", wstrWeaponName, (INT32) getWeaponInfo()->GetTypeClass(), getWeaponInfo()->GetNumber());
	I3PRINTLOG(I3LOG_NOTICE, "- Visible : %d", isVisible());
	I3PRINTLOG(I3LOG_NOTICE, "- Enable : %d", isEnable());
	I3PRINTLOG(I3LOG_NOTICE, "- Owner Addr : 0x%p", m_pOwner);
	I3PRINTLOG(I3LOG_NOTICE, "- Current Pos : %.4f, %.4f, %.4f", GetPos()->x, GetPos()->y, GetPos()->z);

	char * pszState = "(none)";
	switch( getWeaponState())
	{
	case WEAPON::STATE_READY:				pszState = "WEAPON::STATE_READY";			break;
	case WEAPON::STATE_WAIT:					pszState = "WEAPON::STATE_WAIT";				break;
	case WEAPON::STATE_FIRE:					pszState = "WEAPON::STATE_FIRE";				break;
	case WEAPON::STATE_LOADBULLET:			pszState = "WEAPON::STATE_LOADBULLET";		break;
	case WEAPON::STATE_LOADMAG:				pszState = "WEAPON::STATE_LOADMAG";			break;
	case WEAPON::STATE_NEEDTO_FIRE:			pszState = "WEAPON::STATE_NEEDTO_FIRE";		break;
	case WEAPON::STATE_NEEDTO_LOADBULLET:	pszState = "WEAPON::STATE_NEEDTO_LOADBULLET";break;
	case WEAPON::STATE_NEEDTO_LOADMAG:		pszState = "WEAPON::STATE_NEEDTO_LOADMAG";	break;
	case WEAPON::STATE_EMPTY_MAGAZINE:		pszState = "WEAPON::STATE_EMPTY_MAGAZINE";	break;
	}

	switch( m_AttachGrip)
	{
	case WEAPON_ATTACH_NONE :		I3PRINTLOG(I3LOG_NOTICE, "- Weapon no attach");					break;
	case WEAPON_ATTACH_GRIP:		I3PRINTLOG(I3LOG_NOTICE, "- Weapon Attach Grip");					break;
	case WEAPON_ATTACH_BARREL:		I3PRINTLOG(I3LOG_NOTICE, "- Weapon Attach Barrel");				break;
	}

	I3PRINTLOG(I3LOG_NOTICE, "- Current State : %s", pszState);
	I3PRINTLOG(I3LOG_NOTICE, "- State Mask : 0x%x", m_StateMaskWeapon);
	if( isWeaponStateMask( WSM_FIRE_DISABLE))
	{
		I3PRINTLOG(I3LOG_NOTICE, "- On : WSM_FIRE_DISABLE");	
	}
	if( isWeaponStateMask( WSM_LOADMAG_DISABLE))
	{
		I3PRINTLOG(I3LOG_NOTICE, "- On : WSM_LOADMAG_DISABLE");	
	}
	if( isWeaponStateMask( WSM_LOADBULLET_DISABLE))
	{
		I3PRINTLOG(I3LOG_NOTICE, "- On : WSM_LOADBULLET_DISABLE");	
	}
	if( isWeaponStateMask( WSM_FIRING))
	{
		I3PRINTLOG(I3LOG_NOTICE, "- On : WSM_FIRING");	
	}	
	if( isWeaponStateMask( WSM_WAIT_AUTOFIREDELAY))
	{
		I3PRINTLOG(I3LOG_NOTICE, "- On : WSM_WAIT_AUTOFIREDELAY");	
	}
}


REAL32	WeaponBase::GetWeaponPenetrate(bool IsStrengthen)
{
	I3ASSERT(m_pWeaponInfo != nullptr);
	I3ASSERT(m_pOwner != nullptr);

	REAL32 Value = 0.0f;


	if ( IsStrengthen )
		Value = m_pWeaponInfo->GetHelmetPenetrate_Str();
	else
		Value = m_pWeaponInfo->GetHelmetPenetrate();

	if( m_pOwner->getEquipContext()) 
	{
		REAL32 penetrate = m_pOwner->getEquipContext()->GetAbility( EQUIP::eABILITY_PENETRATE);
		//I3TRACE2("penetrate - 1:%f, 2:%f\n\n", Value, penetrate);
		Value += penetrate;
	}

	const NET_CHARA_INFO * pNetChara;
	
	pNetChara = BattleSlotContext::i()->getNetCharaInfo(m_pOwner->getCharaNetIndex() );		// 안제압..
	
	REAL32 ratio = pNetChara->getWeaponSkillValue(getWeaponInfo()->GetTypeClass(), SKILLLIST_PENETRATE);
	Value += ratio;

	return Value;
}

REAL32	WeaponBase::GetCrossHairMin()
{
	I3ASSERT(m_pOwner != nullptr);

	CWeaponInfo *	pInfo = GetCurrentWeaponInfo();

	REAL32 Value = pInfo->GetCrossHairMin();

	return _CalcCrossHairMinToVersion(Value);
}


INT32	WeaponBase::GetRecoilHorzCount(CWeaponInfo * pInfo, bool bApplyExt)
{
	// 다중무기 교체 시 m_pOwner가 NULL인 경우 발생
	//I3ASSERT(m_pOwner != nullptr);

	INT32	Count = 0;

	if( bApplyExt)
		Count = pInfo->GetExtRecoilHorzCount();
	else
		Count = pInfo->GetRecoilHorzCount();

	if ( m_pOwner != nullptr)
	{
		Count += static_cast<INT32>(m_pOwner->getEquipContext()->GetAbility(EQUIP::eABILITY_RECOILHORZCOUNT));
	}

	return Count;
}

INT32 WeaponBase::GetRecoilVertCount(CWeaponInfo * pInfo, bool bApplyExt)
{
	I3ASSERT(m_pOwner != nullptr);

	INT32	Count = 0;

	if (bApplyExt)
		Count = pInfo->GetExtRecoilVertCount();
	else
		Count = pInfo->GetRecoilVertCount();

	return Count;
}

bool WeaponBase::GetOneVertEnable(CWeaponInfo * pInfo, bool bApplyExt)
{
	I3ASSERT(m_pOwner != nullptr);
	
	if (bApplyExt)
		return pInfo->GetExtOneVertEnable() ? true : false;
	else
		return pInfo->GetOneVertEnable() ? true : false;
}

bool	WeaponBase::GetOneHorzEnable(CWeaponInfo * pInfo, bool bApplyExt)
{
	// 다중무기 교체 시 m_pOwner가 NULL인 경우 발생
	// I3ASSERT(m_pOwner != nullptr);

	if( bApplyExt)
		return pInfo->GetExtOneHorzEnable() ? true : false;
	else
		return pInfo->GetOneHorzEnable() ? true : false;
}

CWeaponInfo *	WeaponBase::GetCurrentWeaponInfo(void)
{
	CWeaponInfo * pInfo = getWeaponInfo();

	if( i3::kind_of<WeaponGrenadeLauncher*>(this)  ||
		i3::kind_of<WeaponShotGunLauncher*>(this) )
	{
		if( isExtensionActivate())
			pInfo = getExtensionWeaponInfo();
	}
	else if( i3::same_of<WeaponKrissSuperV*>(this))
	{
		if( isExtensionActivate() && ((WeaponKrissSuperV*)this)->getDualWeapon())
		{
			pInfo = ((WeaponKrissSuperV*)this)->getDualWeapon()->getWeaponInfo();
		}
	}
	else if( i3::same_of<WeaponShotGunGenderWithDual*>(this) )
	{
		if( isExtensionActivate() )
		{
			pInfo = ((WeaponShotGunGenderWithDual*)this)->getDualWeapon()->getWeaponInfo();
		}
	}

	return pInfo;
}

void WeaponBase::SwapExtensionType(void)
{
	WEAPON::EXT_TYPE type0 = m_pWeaponInfo->getExtensionType(0);
	WEAPON::EXT_TYPE type1 = m_pWeaponInfo->getExtensionType(1);

	if( m_CurrentExtensionType == type0)
		m_CurrentExtensionType = type1;
	else if( m_CurrentExtensionType == type1)
		m_CurrentExtensionType = type0;
}
WEAPON::EXT_TYPE WeaponBase::setExtensionType(WEAPON::EXT_TYPE type)
{
	m_CurrentExtensionType = type;

	return m_CurrentExtensionType;
}
WEAPON::EXT_TYPE	WeaponBase::GetNextExtensionType(void)
{
	WEAPON::EXT_TYPE type0 = getWeaponInfo()->getExtensionType(0);
	WEAPON::EXT_TYPE type1 = getWeaponInfo()->getExtensionType(1);
	WEAPON::EXT_TYPE nextType = WEAPON::EXT_NONE;

	if( GetCurrentExtensionType() == type0)
		nextType = type1;
	else 
		nextType = type0;

	return nextType;
}

bool WeaponBase::isPossibleTurnOffWithDotLaser(void)
{	// 기획팀 요청으로 레이저 사이트를 off 불가능한 무기가 있다. (예. 커스텀 M4) komet
	CWeaponInfo * pWeaponInfo = GetCurrentWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);
	return !pWeaponInfo->isLockedDotLaser();
}

#if defined( AI_MOVE_TO_WEAPON)
void WeaponBase::AttachCharaAIToChara( i3Chara * pOwner)
{
	if((m_pCharaAINode != nullptr) && (m_pCharaAINode->getParent() == this))
	{
		m_pCharaAINode->AddRef();

		this->RemoveChild( m_pCharaAINode);

		// Character의 Child로 추가.
		I3_GAMENODE_ADDCHILD( pOwner, m_pCharaAINode);
		m_pCharaAINode->SetOwnerToAllChildren( pOwner);

		I3TRACE( "ATTACH - AI:%p - Owner:%p\n", m_pCharaAINode, pOwner);

		m_pCharaAINode->Release();
		m_pCharaAINode->SetExclusiveEnable( true, false);

		m_pAICtx->SetEnable( true);
	}
}

void WeaponBase::DetachCharaAIFromChara(void)
{
	if((m_pCharaAINode != nullptr) && (m_pCharaAINode->getParent() != this))
	{
		m_pCharaAINode->AddRef();

		// 기존 Parent(Character)로 부터 제거.
		i3GameNode * pParent = m_pCharaAINode->getParent();
		I3ASSERT( pParent != nullptr);
		pParent->RemoveChild( m_pCharaAINode);

		// 무기로 옮겨 붙인다.
		I3_GAMENODE_ADDCHILD( this, m_pCharaAINode);
		m_pCharaAINode->SetOwnerToAllChildren( (i3Chara *) this);

		I3TRACE( "DETACH - AI:%p - Owner:%p\n", m_pCharaAINode, this);

		m_pCharaAINode->Release();
		m_pCharaAINode->SetExclusiveEnable( false, false);

		m_pAICtx->SetEnable( false);
	}
}

void WeaponBase::_MakeAnimPath( char * pszDest, INT32 len, const char * pszSrc, bool bCommon)
{
	CCharaInfo * pInfo = (CCharaInfo *) m_pOwner->getDBInfo();
	const char * pszCharaName = nullptr;

	if( bCommon)
	{
		if( m_pOwner->isFemale())
			pszCharaName = "Base_FM_KnEys";
		else
			pszCharaName = "Base_M_AcdPl";
	}
	else
	{
		pszCharaName = pInfo->getAnimationName();
	}

	i3::safe_string_copy( pszDest, pszSrc);

//	i3String::ReplaceNoCase( pszDest, len, "%c", pszCharaName);
	i3::generic_string_ireplace_all(pszDest, "%c", pszCharaName);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	변경된 무기에 맞추어 해당 캐릭터의 Animation을 검색해 설정하는 함수. (Recursive fasion) </summary>
///
/// <remarks>	이진균, 2010-12-02. </remarks>
///
/// <param name="pAICtx">	[in,out] If non-null, context for a i. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

void WeaponBase::_Rec_BindWeaponAnim( i3AIContext * pAICtx)
{
	INT32 i;
	char szName[MAX_PATH];

	for( i = 0; i < pAICtx->getAIStateCount(); i++)
	{
		i3AIState * pState = pAICtx->getAIState( i);

		if( (pState->getStyle() & I3_AISTATE_STYLE_ANIM_TEMPLATE) == 0)
			continue;

		_MakeAnimPath( szName, sizeof(szName), pState->getAnimName(), false);

		i3Animation * pAnim = (i3Animation *) g_pFramework->FindResourceFromManager( szName, i3Animation::static_meta());

		if( pAnim == nullptr)
		{
			_MakeAnimPath( szName, sizeof(szName), pState->getAnimName(), true);

			pAnim = (i3Animation *) g_pFramework->FindResourceFromManager( szName, i3Animation::static_meta());

			if( pAnim == nullptr)
			{
				I3PRINTLOG(I3LOG_FATAL,  "%s Animation을 찾을 수 없음!", szName);
			}
		}

		pState->SetAnim( pAnim);
	}

	{
		// Child를 모두 처리...
		i3GameNode * pChild = pAICtx->getFirstChild();

		while( pChild != nullptr)
		{
			if( i3::same_of<i3AIContext*>(pChild))
			{
				_Rec_BindWeaponAnim( (i3AIContext *) pChild);
			}

			pChild = pChild->getNext();
		}
	}
}
#endif

bool WeaponBase::isWorldWeapon( void)
{
	if( m_pWeaponInfo == nullptr)
		return false;

	return m_pWeaponInfo->IsWorldWeapon();
}


bool WeaponBase::isDamageToMissionObject( void)
{
	if( m_pWeaponInfo == nullptr)
		return false;

	if( m_pWeaponInfo->GetTypeUsage() == WEAPON_SLOT_MELEE)
		return false;

	// 발사 무기지만 근접 공격이 있는 무기
	if( m_CurrentExtensionType == WEAPON::EXT_DIRECT_MELEE)
	{
		// 밀리 공격
		if( getFireOrder() == WEAPON::SECONDARY_FIRE)
			return false;
	}

	return true;
}

bool WeaponBase::IsDedicatedHost( void)
{
	bool bHost = BattleServerContext::i()->IsP2PHost();

	ISTEST_RETURN( bHost);

	if( BattleServerContext::i()->IsUseDedicationMode())
	{
		if( (getOwner() != nullptr) && (getOwner()->isLocal() || getOwner()->isAI()) )
		{	// Dedicated Server에 접속한 경우는 자신의 무기라면 자신이 처리하도록 한다.
			bHost = true;
		}
		else
		{
			bHost = false;
		}
	}

	return bHost;
}

void WeaponBase::setWeaponState( WEAPON::eSTATE state)			
{ 
	// GlobalFunc::PB_TRACE("WEAPON_STATE %d", state);
	m_StateWeapon = state; 
}

bool WeaponBase::IsLocalOrAI()
{
	if( (getOwner() != nullptr) && (getOwner()->isLocal() || getOwner()->isAI()) )
	{
		return true;
	}

	return false;
}

REAL32 WeaponBase::_CalcReloadTimeToVersion(REAL32 rReloadTime)
{
	// V1 버전은 별도의 계산처리 안함.
	if (g_pEnvSet->IsV2Version() == false)
		return rReloadTime;

	// 소유자가 없을때 별도 계산 처리 안함.
	if (m_pOwner == nullptr)
		return rReloadTime;

	// 장비 ability
	if (m_pOwner->getEquipContext() != nullptr && m_pWeaponInfo->GetTypeUsage() == WEAPON_SLOT_PRIMARY)
	{
		REAL32 ratio = m_pOwner->getEquipContext()->GetAbility(EQUIP::eABILITY_RELOADTIME);
		CGameCharaEquip * Headgear = m_pOwner->getEquipContext()->getEquip(EQUIP::ePART_HEADGEAR);
		
		if(  Headgear != nullptr)
		{
			// 이건 슈퍼 베렛만 예외처리 합니다.
			if (Headgear->GetID() == 9)
			{
				ratio += 0.25f;
			}
		}

		rReloadTime -= rReloadTime * ratio;
	}

	//	빠른 장전 아이템 사용시 시간 감소.
	REAL32 ratio = BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_REBULLET_RATE);
	CASHITEM_TRACE("CASHITEMSKILL_REBULLET_RATE = %f\n", ratio);
	rReloadTime -= (rReloadTime * ratio);

	//  주특기 빠른 장전 스킬 적용
	{
		const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo(m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());
		REAL32 ratio_weapon_skill = pNetInfo->getWeaponSkillValue(getWeaponInfo()->GetTypeClass(), SKILLLIST_RELOAD);
		rReloadTime -= ratio_weapon_skill;

		// 호칭 적용.. (12월에는 아예 이 기능이 불활성화된 이상 상태라서...확인이 필요할지도 모른다.. 15.05.22.수빈)
		const DesigApplyInfo* desig_apply_info = pNetInfo->GetDesigApplyInfo();
		if (desig_apply_info)
		{
			//	rVal += desig_apply_info->desigWeapon.weaponReloadTime;				
			REAL32 fRate = m_pWeaponInfo->GetReloadTime() * 0.01f;		// 으응?? 이유는?? 퍼센티지 표시일래나...
			rReloadTime += desig_apply_info->desigWeapon.weaponReloadTime * fRate;		// 일단 반영분은 다른 누적없는 WeaponInfo에서의 비율로 계산..
		}
	}

	return MINMAX(0.1f, rReloadTime, 100.f); ;
}

REAL32 WeaponBase::_CalcReloadTimeScaleToVersion(i3AIState * pAIState/* = nullptr*/, REAL32 rReloadTime/* = 1.0f*/, REAL32 rSpeedRatio/* = 0.0f*/)
{
	// 재장전 감소율이 디폴트 인자면, 디폴트 값을 재설정해줍니다.
	if (rSpeedRatio == 0.0f)
	{
		rSpeedRatio = 1.0f;

		if( (g_pEnvSet->IsV2Version() == false) && EnablePlayMasterAnim() )
			rSpeedRatio = MASTER_RELOAD_SPEED_RATIO;
	}

	REAL32 rTimeScale = 1.0f;

	if (pAIState != nullptr)
	{
		rTimeScale = (pAIState->getAnimDuration() / rReloadTime);
	}

	return rTimeScale * rSpeedRatio;
}

REAL32 WeaponBase::_CalcSwapTimeScaleToVersion(void)
{
	if (m_pOwner == nullptr)
		return 1.0f;

	REAL32 rTimeScale = 1.0f;

	if (g_pEnvSet->IsV2Version())
	{
		rTimeScale = m_pOwner->GetWeaponSwapTimeScale();
	}
	else
	{
		// PBTN Beret 예외처리
		{
			const CEquipInfo * pBeretInfo = m_pOwner->getEquipContext()->getEquipInfo(EQUIP::GetePartToBeret());

			if (pBeretInfo != nullptr && pBeretInfo->GetITEMID() == 2700013)
			{
				REAL32 ratio = 0.5f + I3_EPS;
				rTimeScale += (rTimeScale * ratio);
			}
		}
	}

	//	빠른 무기 변경 아이템 사용시 시간 감소.
	REAL32 ratio = BattleSlotContext::i()->GetUseCashItemValue(m_pOwner->getCharaNetIndex(), CASHITEMSKILL_SWAPWEAPON_RATE);
	CASHITEM_TRACE("CASHITEMSKILL_SWAPWEAPON_RATE = %f\n", ratio);
	rTimeScale += (rTimeScale * ratio);

	return rTimeScale;
}


REAL32 WeaponBase::_CalcFireDelayToVersion(REAL32 rFireDelay)
{
	if (g_pEnvSet->IsV2Version())
	{
		rFireDelay = _CalcFireDelayForDesignation(rFireDelay);	// 호칭에 의한 발사 지연시간.
		rFireDelay = _CalcFireDelayForSkill(rFireDelay);		// 스킬에 대한 발사 지연시간 계산
		rFireDelay = _CalcFireDelayForEquip(rFireDelay);		// 장비에 대한 발사 지연시간 계산

		rFireDelay = MINMAX(I3_EPS, rFireDelay, 100.f);
	}
	else
	{
		rFireDelay = _CalcFireDelayForDesignation(rFireDelay);	// 호칭에 대한 FireDelay 계산

		// 장비에 대한 FireDelay 계산 ( V1은 비율 감소가 아닌 절대값 수치 감소.)
		rFireDelay += m_pOwner->getEquipContext()->GetAbility(EQUIP::eABILITY_FIREDELAY);
	}

	return rFireDelay;
}

REAL32 WeaponBase::_CalcCrossHairMinToVersion(REAL32 rCrossHairMin)
{
	if (g_pEnvSet->IsV2Version())
	{
		// 이값은 빼는 것이 좋아집니다.
		rCrossHairMin -= (rCrossHairMin * m_pOwner->getEquipContext()->GetAbility(EQUIP::eABILITY_ACCURACY));

		const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo(m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());
		REAL32 ratio = pNetInfo->getWeaponSkillValue(getWeaponInfo()->GetTypeClass(), SKILLLIST_ACCURACY);
		rCrossHairMin -= rCrossHairMin * ratio;
		rCrossHairMin = MINMAX(0.f, rCrossHairMin, 100.f);
	}
	else
	{
		// 장비에 대한 FireDelay 계산 ( V1은 비율 감소가 아닌 절대값 수치 감소.)
		rCrossHairMin += m_pOwner->getEquipContext()->GetAbility(EQUIP::eABILITY_ACCURACY);

		if (rCrossHairMin <= 0.0f)
			rCrossHairMin = 0.01f;
	}

	return rCrossHairMin;
}

REAL32 WeaponBase::_CalcDeviationToVersion(REAL32 rDeviation)
{
	REAL32 rRate = 1.0f;

	if( g_pEnvSet->IsV2Version() )
	{
		// Equipment Ability
		// 편차각은 작을 수록 좋아진다.
		rRate -= (rRate * m_pOwner->getEquipContext()->GetAbility(EQUIP::eABILITY_ACCURACY));

		// Skill ability
		{
			const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo(m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());
			REAL32 ratio = pNetInfo->getWeaponSkillValue(getWeaponInfo()->GetTypeClass(), SKILLLIST_ACCURACY);
			rRate -= ratio;

			// 호칭 속성 반영 ...
			if (pNetInfo->GetDesigApplyInfo())
			{
				const DesigApplyInfo* desig_apply_info = pNetInfo->GetDesigApplyInfo();
				rRate += desig_apply_info->desigWeapon.weaponDeclination;
			}
			// 
		}

		rDeviation *= rRate;
		rDeviation = MINMAX(0.f, rDeviation, 100.f);

	}
	else
	{
		rRate = 0.0f;

		// 호칭 속성 반영 ...
		const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo(m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());
		if (pNetInfo->GetDesigApplyInfo())
		{
			const DesigApplyInfo* desig_apply_info = pNetInfo->GetDesigApplyInfo();
			rRate += desig_apply_info->desigWeapon.weaponDeclination;
		}

		rDeviation += (rDeviation * rRate);
		rDeviation = MINMAX(0.f, rDeviation, 100.f);
	}

	return rDeviation;

}

bool WeaponBase::_IsEnableMasterAnimToVersion(void)
{
	I3ASSERT(m_pOwner);

	if (g_pEnvSet->IsV2Version())
	{
		REAL32 reloadTime = 0.0f;

		const CEquipInfo * pHeadInfo = m_pOwner->getEquipContext()->getEquipInfo(EQUIP::ePART_HEADGEAR);
		reloadTime += pHeadInfo->GetAbilityValue(EQUIP::eABILITY_RELOADTIME);

		//if (m_pOwner->getEquipContext()->GetAbility(EQUIP::eABILITY_RELOADTIME) > 0.0f)

		if (reloadTime != 0.f && reloadTime > 0.f)
			return true;

		return false;
	}
	else
	{
		const CEquipInfo * pBeretInfo = m_pOwner->getEquipContext()->getEquipInfo(EQUIP::GetePartToBeret());

		if (getWeaponInfo() == nullptr ||
			getWeaponInfo()->GetGaugeReload() == 1 ||  // 로드타입이 펌프일경우 고정합니다. 
			pBeretInfo == nullptr)
		{
			return false;
		}

		T_ItemID beret_id = pBeretInfo->GetITEMID();
		WEAPON_CLASS_TYPE eWeaponClass = getWeaponInfo()->GetTypeClass();

		//// Shotgun 중에서 KelTec은 빠른 재장전 없음
		//if (eWeaponClass == WEAPON_CLASS_SHOTGUN &&
		//	(getWeaponInfo()->IsFastReloadAnimation() == false))
		//{
		//	return false;
		//}

		switch (beret_id)
		{
		case 2700006:	// Kopassus
		case 2700005:	// CrossSword
		case 2700012:	// PBNC5
		{
			if (eWeaponClass == WEAPON_CLASS_ASSAULT ||
				eWeaponClass == WEAPON_CLASS_SMG ||
				eWeaponClass == WEAPON_CLASS_DUALSMG)
			{
				return true;
			}
		}
		break;
		case 2700014:	// Title_Assault
		case 2700007:	// Star-Red
		{
			if (eWeaponClass == WEAPON_CLASS_ASSAULT)
			{
				return true;
			}
		}
		break;
		case 2700017:	// Title_Submachine
		{
			if (eWeaponClass == WEAPON_CLASS_SMG ||
				eWeaponClass == WEAPON_CLASS_DUALSMG)
			{
				return true;
			}
		}
		break;
		case 2700016:	// Title_Sniper
		case 2700011:	// Star-Yellow
		{
			if (eWeaponClass == WEAPON_CLASS_SNIPER)
			{
				return true;
			}
		}
		break;
		case 2700018:	// Title_ShotGun
		{
			if (eWeaponClass == WEAPON_CLASS_SHOTGUN ||
				eWeaponClass == WEAPON_CLASS_DUALSHOTGUN)
			{
				return true;
			}
		}
		break;
		case 2700015:	// Title_HandGun
		{
			if (eWeaponClass == WEAPON_CLASS_HANDGUN ||
				eWeaponClass == WEAPON_CLASS_DUALHANDGUN ||
				eWeaponClass == WEAPON_CLASS_CIC)
			{
				return true;
			}
		}
		break;
		case 2700001:	// Black
		case 2700003:	// BlackPB
		case 2700013:	// PBTN
		case 2700004:	// Bope
		case 2700008:	// S-General
		case 2700002:	// Brazil
		case 2700019:	// VeraCruz2016
		{
			return true;
		}
		break;
		}
	}

	return false;
}

