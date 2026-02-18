#include "pch.h"
#include "GameCharaDino.h"

#include "GameCharaDef.h"
#include "GameCharaContexts.h"
#include "Character/GameDinoContexts.h"
#include "DinoInfo.h"
#include "WeaponDino.h"
#include "StageBattle.h"
#include "Crosshair_Dino.h"
#include "CrosshairMgr.h"

#include "ui/UIHudDefine.h"
#include "ui/UIHudUtil.h"

#include "BattleServerContext.h"
#include "BattleSlotContext.h"
#include "i3Base/string/ext/generic_string_cat.h"

static const char * s_szNameLowerDinoAI [ ID_LOWER_DINO_AI_MAX] =
{
	"Idle",					//	ID_LOWER_IDLE

	"Front",				//	ID_LOWER_MOVE_FRONT,
	"FrontLeft",			//	ID_LOWER_MOVE_FRONT_LEFT,
	"FrontRight",			//	ID_LOWER_MOVE_FRONT_RIGHT,
	"Left",					//	ID_LOWER_MOVE_LEFT,
	"Right",				//	ID_LOWER_MOVE_RIGHT,
	"Back",					//	ID_LOWER_MOVE_BACK,
	"BackLeft",				//	ID_LOWER_MOVE_BACK_LEFT,
	"BackRight",			//	ID_LOWER_MOVE_BACK_RIGHT,

	"WalkFront",			//	ID_LOWER_WALK_FRONT,
	"WalkFrontLeft",		//	ID_LOWER_WALK_FRONT_LEFT,
	"WalkFrontRight",		//	ID_LOWER_WALK_FRONT_RIGHT,
	"WalkLeft",				//	ID_LOWER_WALK_LEFT,
	"WalkRight",			//	ID_LOWER_WALK_RIGHT,
	"WalkBack",				//	ID_LOWER_WALK_BACK,
	"WalkBackLeft",			//	ID_LOWER_WALK_BACK_LEFT,
	"WalkBackRight",		//	ID_LOWER_WALK_BACK_RIGHT,

	"JumpStart",			//	ID_LOWER_JUMP_START,
	"JumpEnd",				//	ID_LOWER_JUMP_END,

	"DeathFrontA1",			//	ID_LOWER_DEATH_FRONT_A1,
	"DeathFrontA2",			//	ID_LOWER_DEATH_FRONT_A2,
	"DeathFrontA3",			//	ID_LOWER_DEATH_FRONT_A3,

	"DeathLeftA",			//	ID_LOWER_DEATH_LEFT_A,
	"DeathRightA",			//	ID_LOWER_DEATH_RIGHT_A,
	"DeathBackA",			//	ID_LOWER_DEATH_BACK_A,

	"DeathFrontB",			//	ID_LOWER_DEATH_FRONT_B,
	"DeathLeftB",			//	ID_LOWER_DEATH_LEFT_B,
	"DeathRightB",			//	ID_LOWER_DEATH_RIGHT_B,
	"DeathBackB",			//	ID_LOWER_DEATH_BACK_B,

	"DeathFrontH1",			//	ID_LOWER_DEATH_FRONT_H1,
	"DeathFrontH2",			//	ID_LOWER_DEATH_FRONT_H2,

	"DeathLeftH",			//	ID_LOWER_DEATH_LEFT_H,
	"DeathRightH",			//	ID_LOWER_DEATH_RIGHT_H,
	"DeathBackH",			//	ID_LOWER_DEATH_BACK_H,
	"JumpStartMove",		//	ID_LOWER_DEATH_RIGHT_H,
	"JumpEndMove",			//	ID_LOWER_DEATH_BACK_H,
};

I3_CLASS_INSTANCE( CGameCharaDino);

CGameCharaDino::CGameCharaDino() : m_rElapsedBreathTime(15.0f)
{
	for(INT32 i = 0 ; i < DINO_WEAPON_COUNT ; ++i)
		m_arDinoWeapon[i] = nullptr;
}

CGameCharaDino::~CGameCharaDino()
{
	for(INT32 i = 0 ; i < DINO_WEAPON_COUNT ; ++i)
	{
		I3_SAFE_RELEASE(m_arDinoWeapon[i]);
	}
}

void CGameCharaDino::_PlayDownfallSound( void)
{
	g_pSndMng->StartDinoActionSound( 0, GTBDA_ON_DAMAGE_DOWNFALL, GetPos(), is1PV());
}

void CGameCharaDino::ResetChara()
{
	CGameCharaBase::ResetChara();

	SetNowAttackFirstFrame(false);
}

/*virtual*/ void CGameCharaDino::OnBindResource( void)
{
	i3Chara::OnBindResource();

	if( m_pCharaInfo != nullptr && m_pCharaInfo->GetType() != CHARA::TYPE_NPC)
	{
		//	게임에서 사용될 AIState의 ID를 찾아 미리 저장해둡니다. 나중에 AIState 변경은 이곳 배열에 저장된 ID를 사용하게 됩니다.
		//	Lower ID
		_PrepareLowerAI();

		//	Upper ID
		_PrepareUpperAI();

		// Common ID
		_PrepareCommonAI();
	}
}

void CGameCharaDino::Create( GLOBAL_CHARA_INFO * pCharaInfo, const CCharaInfo * pDBInfo)
{
	CGameCharaBase::Create( pCharaInfo, pDBInfo);

	i3Node * pFindNode = i3Scene::FindNodeByName( getSceneObject()->GetBody(), "FXDummy");

	if( pFindNode != nullptr )
	{	
		if( i3::kind_of<i3BoneRef*>(pFindNode))
		{
			m_pFXDummy = (i3Transform*)i3Scene::FindNodeByExactType( pFindNode, i3Transform::static_meta());
			I3ASSERT( m_pFXDummy);
		}
		else if( i3::kind_of<i3Transform*>(pFindNode))
		{
			m_pFXDummy =(i3Transform*) pFindNode;
		}
	}

	_CreateDino();

	setFullHP();	
}

void CGameCharaDino::CreateFirst( const CCharaInfo * pDBInfo, bool bInLobby)
{
	m_bReturnedInstance = false;

	Destroy();

	I3ASSERT( pDBInfo != nullptr);
	m_pDBInfo = const_cast<CCharaInfo*>(pDBInfo);

	// AnimContext는 먼저 생성해주어야 합니다.
	{
		// 2ms
		m_pAnimContext = CGameDinoAnimContext::new_object();
		I3ASSERT( m_pAnimContext != nullptr);
		m_pAnimContext->OnCreate( this);
	}


	I3ASSERT( m_pCharaInfo != nullptr);

	i3GameResChara * pRes = (i3GameResChara*)g_pCharaManager->QueryCharaResource( getCharaInfo()->GetResType());
	I3ASSERT( pRes != nullptr);

	i3Chara::Create( g_pFramework, pRes);

	// 0.06ms
	OnBindResource();

	// 해당 모델링의 AttrSet을 찾아 리스트에 넣습니다.
	// 0.1ms
	m_pMaterialContext = CGameCharaMaterialContext::new_object();
	m_pMaterialContext->OnCreate(this);

	// 본의 인덱스 리스트 작성. 리소스 로딩 후에 호출
	m_pAnimContext->BindBoneList();

	m_pWeaponContext = nullptr;
	m_pEquipContext = nullptr;

	//	임의로 변경하기 쉽게 하기 위해 강제로 LOD Slope값을 지정합니다. (기존에 CharaTool에서 지정한 Slope값은 사용하지 않습니다. komet)
	if( IsRex() || IsDomiMutantRex())
		getSceneObject()->GetBody()->setSlope( CHARA_LOD_SLOPE * 0.3f);
	else
		getSceneObject()->GetBody()->setSlope( CHARA_LOD_SLOPE);

	// LOD(0)은 shadow가 나오지 않아야 한다.
	i3LOD * pLOD = getSceneObject()->GetBody()->getLOD( 0);
	if( bInLobby )
		pLOD->getShapeNode()->RemoveFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);
	else
		pLOD->getShapeNode()->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);
//	getSceneObject()->GetBody()->AddFlag( I3_NODEFLAG_MUSTBEPROCESS, true);

	m_pUpdateContext = CGameCharaUpdateContext::new_object();
	I3ASSERT( m_pUpdateContext != nullptr);
	m_pUpdateContext->OnCreate( this);

	m_pActionContext = CGameCharaActionContext::new_object();
	I3ASSERT( m_pActionContext != nullptr);
	m_pActionContext->OnCreate( this);

	m_pMoveContext = CGameCharaMoveContext::new_object();
	I3ASSERT( m_pMoveContext != nullptr);
	m_pMoveContext->OnCreate(  this);

	m_pInputContext = CGameCharaInputContext::new_object();
	I3ASSERT( m_pInputContext != nullptr);
	m_pInputContext->OnCreate(  this);

	m_pHUDContext = CGameCharaHUDContext::new_object();
	I3ASSERT( m_pHUDContext != nullptr);
	m_pHUDContext->OnCreate( this);

	m_pCameraContext = CGameCharaCameraContext::new_object();
	I3ASSERT( m_pCameraContext != nullptr);
	m_pCameraContext->OnCreate(  this);

	m_pBoneContext = CGameDinoBoneContext::new_object();
	I3ASSERT( m_pBoneContext != nullptr);
	m_pBoneContext->OnCreate(  this);

	m_pSoundContext = CGameCharaSoundContext::new_object();
	I3ASSERT( m_pSoundContext != nullptr);
	m_pSoundContext->OnCreate(  this);

	m_pCollisionContext = CGameDinoCollisionContext::new_object();
	I3ASSERT( m_pCollisionContext != nullptr);
	m_pCollisionContext->OnCreate(  this);


	// 0.5ms
	{
		_SetControlsByCharaType();

		// Update Timing을 조정하기 위해, 수동으로 Animation, World-Transform 등을 하도록 한다.
		{
			// Animation Update와 World-Space Cache Matrix에 대한 시간차를
			// 없애기 위해, 수동으로 Update를 한다.
			if( BattleServerContext::i()->IsP2PActivate() )
			{
				getSceneObject()->GetBody()->addStyle( I3BS_MANUAL_ANIM_UPDATE | I3BS_MANUAL_TRANSFORM);
				getSceneObject()->AddFlag( I3_NODEFLAG_MANUAL_UPDATE);
			}

			// 이 때, Animation Update와 World-Space Transform 사이에 LOD 계산에서
			// Current Level이 다를 수 있기 때문에...
			// 수작업으로 LOD를 계산한다.
			getSceneObject()->GetBody()->removeStyle( I3BS_LOD_AUTO);
		}

		ResetChara();		//	최종 클래스에서 불러야 한다.
	}

	_PresetResName();
}

void CGameCharaDino::ChangeDinoWeapon(WEAPON::FIRE_ORDER order)
{
	I3ASSERT( order < DINO_WEAPON_COUNT);
	WeaponBase * pWeapon = m_pCurDinoWeapon;
	
	m_pCurDinoWeapon = i3::kind_cast<WeaponBase*>(m_arDinoWeapon[order]);
	I3ASSERT(m_pCurDinoWeapon != nullptr);

	if(pWeapon != m_pCurDinoWeapon) {
		SwapWeaponDino();
	}
}

void CGameCharaDino::SwapWeaponDino()
{
	if(IsDino()) 
	{
		if(getActionContext()->getUpper() == CHARA_UPPER_IDLE) 
		{
			((CGameDinoAnimContext*)getAnimContext())->_Play_Anim_DinoUpper( ID_UPPER_DINO_IDLE);
		}
		else if(getActionContext()->getUpper() == CHARA_UPPER_MOVE) 
		{
			((CGameDinoAnimContext*)getAnimContext())->_Play_Anim_DinoUpper( ID_UPPER_DINO_MOVE);
		}
	}
}

void CGameCharaDino::_PrepareLowerAI( void)
{
	i3AIContext * pAICtx = nullptr;
	char szAIName[256];

	I3ASSERT( m_pAnimContext != nullptr);

	for( INT32 i = 0; i < CHARA_BODYLOWER_MAXCOUNT; i++)
	{
		i3::safe_string_copy( szAIName, "Lower:", 256 );

		switch( (CHARA_ACTION_BODYLOWER) i)
		{
		case CHARA_BODYLOWER_STAND:			i3::generic_string_cat( szAIName, "Stand");		break;
		case CHARA_BODYLOWER_CROUCH:		i3::generic_string_cat( szAIName, "Sit");			break;
		}

		//	AI Context
		pAICtx = this->FindAI( szAIName);
		I3ASSERT( pAICtx != nullptr);

		//	AI Id
		for( INT32 id = 0; id < ID_LOWER_DINO_AI_MAX; id++)
		{
			if( s_szNameLowerDinoAI[ id] != nullptr)
			{
				INT32 Idx = pAICtx->FindAIStateByName( s_szNameLowerDinoAI[ id] );

				//EscapeMission : 일단 새로운 AI형태 때문에 ASSERT막아 놓습니다.
				if( !IsRex() ) { I3ASSERT(Idx != -1); }

				CGameCharaAnimContext * pAnimContext = m_pAnimContext;
				((CGameDinoAnimContext*)pAnimContext)->setDinoAILowerID( (CHARA_ACTION_BODYLOWER)i, (ID_LOWER_DINO_AI)id, Idx);
			}
		}
	}
}

void CGameCharaDino::_PrepareUpperAI( void)
{
	i3AIContext * pAICtx = nullptr;

	char szAIName[256] = "";
	char * pszAnimSet = "";

	//	AnimSet
	for( INT32 nAnimCnt = 0; nAnimCnt < CHARA_VIEW_MAX; nAnimCnt++)
	{
		switch( (CHARA_VIEW_TYPE) nAnimCnt)
		{
		case CHARA_VIEW_1PV:			pszAnimSet = "1PV";		break;
		case CHARA_VIEW_3PV:			pszAnimSet = "3PV";		break;
		}

		//	Weapon Class
		i3::safe_string_copy( szAIName, "Upper:", 256 );
		i3::generic_string_cat( szAIName, pszAnimSet);

		//	AI Context
		pAICtx = FindAI( szAIName);
		if( pAICtx == nullptr)		continue;		//	Zoom이 없는 무기도 있으므로

		//	AI Id
		for( INT32 id = 0; id < ID_UPPER_DINO_AI_MAX; id++)
		{	
			INT32 nRet = pAICtx->FindAIStateByName( g_szNameUpperAI_Dino[ id] );

			CGameCharaAnimContext * pAnimContext = m_pAnimContext;
			((CGameDinoAnimContext*)pAnimContext)->setDinoAIUpperID( (CHARA_VIEW_TYPE)nAnimCnt, (ID_UPPER_DINO_AI)id, nRet);
		}
	}
}

void CGameCharaDino::_CreateDino(void)
{
	for(INT32 i = 0 ; i < DINO_WEAPON_COUNT ; ++i)
	{
		// 새 무기를 항상 생성하기 때문에, 이전에 있던 무기정보는 해제합니다.
		I3_SAFE_RELEASE( m_arDinoWeapon[i] );

		T_ItemID ItemID = getDBInfo()->GetDinoWeaponID(i);
		m_arDinoWeapon[i] = i3::kind_cast<WeaponDino*>( g_pWeaponManager->_CreateDinoWeapon(ItemID, ItemID, this) );
		I3ASSERT(m_arDinoWeapon[i] != nullptr);
	}	

	m_pCurDinoWeapon = i3::kind_cast<WeaponBase*>(m_arDinoWeapon[WEAPON::PRIMARY_FIRE]);
	I3ASSERT(m_pCurDinoWeapon != nullptr);
}

void CGameCharaDino::_PlayDinoAnimUpper_Fire(WEAPON::FIRE_ORDER fireOrder, ID_UPPER_DINO_AI DinoAI)
{
	SetNowAttackFirstFrame(true);

	switch(fireOrder)
	{
	case WEAPON::PRIMARY_FIRE:
		{
			getActionContext()->setUpper( CHARA_UPPER_FIRE);
		}
		break;
	case WEAPON::SECONDARY_FIRE:
		{
			getActionContext()->setUpper( CHARA_UPPER_SECONDARY_FIRE);
		}
		break;
	default:	I3ASSERT_0;
	}

	((CGameDinoAnimContext*)getAnimContext())->_Play_Anim_DinoUpper( DinoAI);

	m_arDinoWeapon[fireOrder]->setAutoFireCount(0);

	_PlayDinoSound_Fire(fireOrder);
}

ID_UPPER_DINO_AI CGameCharaDino::_FindDinoUpperAttackAI(WEAPON::FIRE_ORDER fireOrder)
{
	m_eNowDinoAttackType = GetDinoAttackType(fireOrder);

	switch(fireOrder)
	{
	case WEAPON::PRIMARY_FIRE:
		{
			if( isRun(fireOrder) || isWalk(fireOrder) )
				return ID_UPPER_DINO_MOVE_ATTACK_A;
			else
				return ID_UPPER_DINO_ATTACK_A;
		}
		break;
	case WEAPON::SECONDARY_FIRE:
		{
			if( isRun(fireOrder) || isWalk(fireOrder) )
				return ID_UPPER_DINO_MOVE_ATTACK_B;
			else
				return ID_UPPER_DINO_ATTACK_B;
		}
		break;
	default:	I3ASSERT_0;
	}

	return ID_UPPER_DINO_UNKNOWN;
}


void CGameCharaDino::OnUpdate( REAL32 rDeltaSeconds)
{
	CGameCharaBase::OnUpdate( rDeltaSeconds);

	for( INT32 i = 0;i < DINO_WEAPON_COUNT; ++i)
	{
		WeaponDino*	pWeaponDino = m_arDinoWeapon[i];
		if( pWeaponDino != nullptr)
		{
			pWeaponDino->OnUpdate( rDeltaSeconds);
		}
	}

	//티렉스가 idle상태일때만 가동. 적정?!! 시간마다 숨소리를 낸다.
	if( getActionContext()->isLower( CHARA_LOWER_IDLE) && getActionContext()->isUpper( CHARA_UPPER_IDLE))
	{
		m_rElapsedBreathTime	-= rDeltaSeconds;
		if( m_rElapsedBreathTime < 0.0f)
		{
			m_rElapsedBreathTime = 5.0f;

			_PlayBreathSnd();
		}
	}

}

bool CGameCharaDino::NET_HitForHost( CHARA_DAMAGEINFO * pDamageInfo, REAL32 rDamage)
{
	return CGameCharaBase::NET_HitForHost(pDamageInfo, rDamage);
}

/*virtual*/ void CGameCharaDino::SetDinoFireStartPos(VEC3D * pSrc)
{
	if(is1PV())
	{
		getCameraContext()->getVirtualCamPosition(pSrc);
	}
	else
	{
		if( IsRaptorBase())
		{
			i3Vector::Sub( pSrc, i3Matrix::GetPos( getBoneContext()->getHeadMatrix()), i3Matrix::GetPos( getBoneContext()->getIKMatrix()));
			i3Vector::Scale( pSrc, pSrc, 0.5f);
			i3Vector::Sub( pSrc, i3Matrix::GetPos( getBoneContext()->getHeadMatrix()), pSrc);
		}
	}
}

void CGameCharaDino::PlayDinoCrossHair(void)
{
	Crosshair* pCH = CrosshairMgr::i()->getCrosshair();

	if( i3::kind_of<Crosshair_Dino*>(pCH))
	{
		if(m_eNowDinoAttackType == DAT_RAPTOR_ATTACK_B || m_eNowDinoAttackType == DAT_REX_ATTACK1)
		{
			((Crosshair_Dino *)pCH)->Bite();
		}
		else if(m_eNowDinoAttackType == DAT_RAPTOR_ATTACK_A  || m_eNowDinoAttackType == DAT_ELITE_SCRATCH)
		{
			((Crosshair_Dino *)pCH)->Scratch();
		}
	}
}

/*virtual*/ REAL32 CGameCharaDino::GetDinoSkillDuration(INT32 idx) const
{
	return m_arDinoWeapon[idx]->getWeaponInfo()->GetSkillDuration();
}

/*virtual*/ REAL32 CGameCharaDino::GetDinoSkillCoolTime(INT32 idx) const
{
	return m_arDinoWeapon[idx]->getWeaponInfo()->GetSkillCoolTime();
}

/*virtual*/ REAL32 CGameCharaDino::GetDinoSkillMovementRate(INT32 idx) const
{
	return m_arDinoWeapon[idx]->getWeaponInfo()->GetSkillSkillMovementRate();
}

//

bool CGameCharaDino::isRun(INT32 idx)
{
	return getActionContext()->isLower(CHARA_LOWER_RUN);
}

/*virtual*/ bool CGameCharaDino::isWalk(INT32 idx)
{
	return getActionContext()->isLower(CHARA_LOWER_WALK);
}

WeaponBase* CGameCharaDino::GetDinoWeapon(const WEAPON::FIRE_ORDER order) const
{
	I3ASSERT(0 <= order && order < DINO_WEAPON_COUNT);
	return m_arDinoWeapon[order];
}

using namespace DinoSkill;

Buff::Buff(const BuffInfo& info) : m_started(false),
								   m_timeCurrent(0.0f)
{
	m_info.owner = info.owner;
	m_info.effectIdx = info.effectIdx;
	m_info.speedRate = info.speedRate;
	m_info.cooldown = info.cooldown;
	m_info.buffSustain = info.buffSustain;
}

Buff::~Buff() 
{
}

void Buff::Start()
{
	m_started = true;
	m_timeCurrent = m_info.cooldown;

	if (m_info.owner->isLocal())
	{
		CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
		myPlayer->getMoveContext()->EnableSpeedReduction(m_info.speedRate, m_info.buffSustain);
	}
}

void Buff::Stop()
{
	m_started = false;
	m_timeCurrent = 0.0f;
}

void Buff::Update(REAL32 deltaSeconds)
{
	if (!m_started) return;

	REAL32 timeBuffEnd = m_info.cooldown - m_info.buffSustain;

	m_timeCurrent -= deltaSeconds;

	if (m_timeCurrent < 0.0f)
		Stop();

	if (m_info.owner->isLocal())
		_UpdateGuage();

	// 버프 이펙트
	if ((timeBuffEnd < m_timeCurrent) && (m_timeCurrent <= m_info.cooldown))
	{
		MATRIX* ik = m_info.owner->getBoneContext()->getIKMatrix();
		((WeaponDino*)m_info.owner->GetDinoWeapon())->_FireEffect(ik, m_info.effectIdx, true);
	}

	// 버프 종료 이펙트 : 하울링 스크린 이펙트로 대체 사용
	if (m_info.owner->isLocal() &&
		(timeBuffEnd <= m_timeCurrent) && (m_timeCurrent <= timeBuffEnd+1.0f))
	{
		CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
		CGameCharaHUDContext* hud = myPlayer->getHUDContext();
		if (hud->GetCurScreenEffect() != SCR_EFFECT_HOWL)
			hud->ScreenEffect_Howl(1.0f);
	}

	// 버프 종료 이펙트 해제
	if (m_info.owner->isLocal() && 
		(timeBuffEnd-1.0f <= m_timeCurrent) && (m_timeCurrent <= timeBuffEnd))
	{
		CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
		myPlayer->getHUDContext()->ClearScreenEffect();
	}
}

void Buff::_UpdateGuage()
{
	hu_dino::set_dino_raio( CooldownRate() );
}

REAL32 Buff::CooldownRate()
{
	REAL32 max = MINMAX(0.1f, m_info.cooldown, 100.0f);
	REAL32 rate = MINMAX(0.0f, m_timeCurrent/max, 1.0f);

	return (1.0f - rate);
}