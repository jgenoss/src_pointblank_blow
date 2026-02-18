#include "pch.h"
#include "ObjectRidingHelicopter.h"

#include "GameCharaInputContext.h"
#include "StageBattle.h"
#include "ObjectMark_OutPost.h"
#include "Camera/GameCamera_OnObject.h"
#include "WeaponM197.h"
#include "GlobalFunc.h"
#include "TutorialMode.h"
#include "../UI/UIHudManager.h"

#include "BattleHud_Helicopter.h"

#include "BattleServerContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "BattleObjContext.h"

#define HELICOPTER_RESPAWN_TIME				10.f
#define HELICOPTER_SENDCOUNT_TIME			0.5f			// 애니메이션이 바뀌면 0.5초간 보냅니다.

const char * s_PathName_Outpost_Helicopter[ HPA_COUNT] =
{
	"Object/cgame/OutPost_UH60_Idle.i3a",
	"Object/cgame/OutPost_UH60_Path1.i3a",
	"Object/cgame/OutPost_UH60_Path2.i3a",
	"Object/cgame/OutPost_UH60_Path3.i3a",
	"Object/cgame/Helispot/Tu_Heliport_Ground_Idle.i3a",
	"Object/cgame/Helispot/Tu_Heliport_Takeoff.i3a",
	"Object/cgame/Helispot/Tu_Heliport_Air_Idle.i3a",
	"Object/cgame/Helispot/Tu_Helicopter_Land.i3a",
};

const char * s_SoundName_OutPost_Helicopter[ HSND_COUNT] =
{
	"OB_Exp_Uh60_Standing.wav",
	"OB_Exp_Uh60_Landing.wav",
	"OB_Exp_Uh60_Takeoff.wav",
	"OB_Dust_Uh60_Ambient.wav",
};

I3_CLASS_INSTANCE( CGameObjectRidingHelicopter);//, CGameControledObject);

CGameObjectRidingHelicopter::CGameObjectRidingHelicopter(void)
{
	//Animation
	m_PathKey			= (HELICOPTER_PATH_ANIM) -1;
	m_rPathLocalTime	= 0.f;

	i3mem::FillZero( &m_pPathAnim, sizeof( i3Animation*) * HPA_COUNT);
	
	m_pUH60_DustEffect	= nullptr;

	m_bMyEnterObject	= false;
	m_bDestroyed		= false;

	m_rRespawnTime		= 0.f;
	m_pArrowMark		= nullptr;

	i3Vector::Zero( &m_vFieldPos);
	
	m_pRootDummy		= nullptr;

	for( INT32 i = 0; i < HSND_STATE_COUNT; i++)
		m_pStateTimeSequence[ i] = nullptr;

	for( INT32 i = 0; i < HSND_COUNT; i++)
		m_pStateSound[ i]	= nullptr;

	m_nSoundState		= (HELICOPTER_SOUND_STATE) -1;

	InitFlag();

	m_rSendCountTime = 0.f;

	m_bHelicopterRiding = false;

	m_bWarningNotice = false;
}

CGameObjectRidingHelicopter::~CGameObjectRidingHelicopter(void)
{
	INT32 i;

	for( i = 0; i < HPA_COUNT; i++)
	{
		I3_SAFE_RELEASE( m_pPathAnim[ i]);
	}

	for( i = 0; i < HSND_COUNT; i++)
	{
		if(m_pStateSound[ i] == nullptr) {
			continue;
		}
		m_pStateSound[ i]->DeactivateNode();
		m_pStateSound[ i]->RemoveFromParent();
		I3_SAFE_RELEASE( m_pStateSound[ i]);
	}

	for( i = 0; i < HSND_STATE_COUNT; i++)
	{
		if(m_pStateTimeSequence[ i] == nullptr) {
			continue;
		}

		m_pStateTimeSequence[ i]->DeactivateNode();
		m_pStateTimeSequence[ i]->RemoveFromParent();
		I3_SAFE_RELEASE( m_pStateTimeSequence[ i]);
	}
}

void CGameObjectRidingHelicopter::InitObject()
{
	// load animation
	InitAnimation();
	// find arrowmark object
	InitSearchArrowMarkObject();

	i3Transform* pTrans = getSceneObject()->FindBoneByName("WeaponPointDummy");
	T_ItemID Itemid = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY,		WEAPON_CLASS_OBJECT_GUN, 1);
	m_pWeaponOnObject = (WeaponM197*)g_pWeaponManager->QueryWeapon( nullptr, Itemid);
	
	I3ASSERT(m_pWeaponOnObject);
	m_pWeaponOnObject->InitWeaponOnObject();
	m_pWeaponOnObject->AttachNode(pTrans, this);

	m_pRootDummy = getSceneObject()->FindBoneByName( "Root");
	I3ASSERT( m_pRootDummy != nullptr);

	for( INT32 i = 0; i < HSND_COUNT; i++)
	{	
		i3GameResSound * pRes = (i3GameResSound*) g_pFramework->QuaryResource( s_SoundName_OutPost_Helicopter[ i] );
		I3ASSERT( pRes != nullptr);
		i3SoundPlayInfo * pInfo = pRes->getSound();
		if( i == HSND_STANDING || i == HSND_FLYING )
		{
			pInfo->setLoopMode( I3SND_RT_LOOPMODE_LOOPING);
		}
		pInfo->set3DMode( I3SND_RT_3DMODE_WORLD_SPACE);

		m_pStateSound[ i] = i3SoundNode::new_object();
		I3ASSERT( m_pStateSound[ i] != nullptr);
		m_pStateSound[ i]->setSoundPlayInfo( pInfo);
	}

	m_pStateTimeSequence[ HSND_STATE_TAKEOFF] = i3TimeSequence::new_object();
	i3TimeSequenceInfo * pSequenceInfo = i3TimeSequenceInfo::new_object_ref();
	pSequenceInfo->SetStartTime( 0.f);
	pSequenceInfo->SetNode( m_pStateSound[ HSND_TAKEOFF] );
	m_pStateTimeSequence[ HSND_STATE_TAKEOFF]->AddSequence( pSequenceInfo);
	pSequenceInfo = i3TimeSequenceInfo::new_object_ref();
	pSequenceInfo->SetStartTime( 4.f);
	pSequenceInfo->SetNode( m_pStateSound[ HSND_FLYING] );
	m_pStateTimeSequence[ HSND_STATE_TAKEOFF]->AddSequence( pSequenceInfo);

	m_pStateTimeSequence[ HSND_STATE_LANDING] = i3TimeSequence::new_object();
	pSequenceInfo = i3TimeSequenceInfo::new_object_ref();
	pSequenceInfo->SetStartTime( 0.f);
	pSequenceInfo->SetNode( m_pStateSound[ HSND_LANDING] );
	m_pStateTimeSequence[ HSND_STATE_LANDING]->AddSequence( pSequenceInfo);
	pSequenceInfo = i3TimeSequenceInfo::new_object_ref();
	pSequenceInfo->SetStartTime( 4.f);
	pSequenceInfo->SetNode( m_pStateSound[ HSND_STANDING] );
	m_pStateTimeSequence[ HSND_STATE_LANDING]->AddSequence( pSequenceInfo);

	InitFlag();

	m_bManualUpdate = true;

	m_bWarningNotice = true;
}

void CGameObjectRidingHelicopter::_PlayStateSound( HELICOPTER_SOUND_STATE state)
{
	I3ASSERT( m_pRootDummy != nullptr);
	I3_BOUNDCHK( state, HSND_STATE_COUNT);

	if( m_nSoundState != -1)
	{
		m_pStateTimeSequence[ m_nSoundState]->DeactivateNode();
		m_pStateTimeSequence[ m_nSoundState]->RemoveFromParent();
		m_nSoundState = (HELICOPTER_SOUND_STATE) -1;
	}

	if( (getCurrentHP() == 0.f) )
		return;

	if( state != -1)
	{
		m_pRootDummy->AddChild( m_pStateTimeSequence[ state]);
		m_pStateTimeSequence[ state]->OnChangeTime( 0.f);
		m_pStateTimeSequence[ state]->ActivateNode();
	}
	
	m_nSoundState = state;
}

void CGameObjectRidingHelicopter::ResetObject()
{
	//EnableDustEffect( false);
	CGameObjectWithWeapon::ResetObject();
	m_rPathLocalTime	= 0.f;
	m_rRespawnTime		= 0.f;
	m_rSendCountTime	= 0.f;

	if( m_nSoundState != -1)
	{
		m_pStateTimeSequence[ m_nSoundState]->DeactivateNode();
		m_pStateTimeSequence[ m_nSoundState]->RemoveFromParent();
		m_nSoundState = (HELICOPTER_SOUND_STATE) -1;
	}
	m_PathKey			= (HELICOPTER_PATH_ANIM) -1;

	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
	{
		_SetPathAnimKey( HPA_GROUND_IDLE_IN_TUTORIAL);
	}
	else
	{
		_SetPathAnimKey( HPA_NONE);
	}
	

	m_bDestroyed		= false;

	m_bHelicopterRiding = false;
	m_bWarningNotice = true;

	InitFlag();
}

void CGameObjectRidingHelicopter::InitFlag()
{
	m_bUseObject_Flag = false;
	m_iUseObject_Count = 0;

	m_bLeaveWeapon_Flag = false;
	m_iLeaveWeapon_Count = 0;

	m_bEnterWeapon_Flag = false;
	m_iEnterWeapon_Count = 0;
}

void CGameObjectRidingHelicopter::EnableObject()
{
	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
	{
		_SetPathAnimKey( HPA_TAKE_OFF_IN_TUTORIAL);
	}
	else
	{
		if( m_PathKey != HPA_NONE)
			return;

		// select random path
		/*
		INT32 randomIdx = (i3Math::Rand() % 3) + 1;
		_SetPathAnimKey( (HELICOPTER_PATH_ANIM)randomIdx);
		*/

	}
}

bool CGameObjectRidingHelicopter::IsEnableObject()
{ 
	if(m_PathKey == HPA_NONE)
	{
		return false;
	}

	return true;
}

void CGameObjectRidingHelicopter::ProcessObject( REAL32 rDeltaSeconds)
{
	CGameObjectWithWeapon::ProcessObject( rDeltaSeconds);

	Process( rDeltaSeconds);
}

void CGameObjectRidingHelicopter::InitAnimation( void)
{	
	INT32 i;

	for( i = 0; i < HPA_COUNT; i++)
	{
		if( s_PathName_Outpost_Helicopter[ i] != nullptr)
		{
			i3Animation * pAnimation = i3Animation::LoadFromFile( s_PathName_Outpost_Helicopter[ i]);
			if( pAnimation == nullptr)
				continue;

			I3_REF_CHANGE( m_pPathAnim[ i], pAnimation);
			I3_MUST_RELEASE(pAnimation);		// LoadFromFile()은 레퍼런스카운팅을 갖고 있음.(2012.10.18.수빈)
		}
	}

	m_pSceneObj->BuildBoneList();
}

void CGameObjectRidingHelicopter::_SetPathAnimKey( HELICOPTER_PATH_ANIM pathKey, REAL32 rStartTime)
{
	if( m_pPathAnim[ pathKey] != nullptr)
	{
		if( pathKey != HPA_NONE && pathKey != HPA_GROUND_IDLE_IN_TUTORIAL && pathKey != HPA_AIR_IDLE_IN_TUTORIAL)
		{
			m_pSceneObj->PlayAnim( m_pPathAnim[ pathKey], I3_ANIM_BLEND_MODE_REPLACE, rStartTime, 0.f, 1.0f, 1.0f, I3_ANIM_PLAY_MODE_CLAMP, I3_ANIM_PLAY_MODE_STOP);
			if( m_PathKey == HPA_NONE )
				_PlayStateSound( HSND_STATE_TAKEOFF);
		}
		else
		{
			m_pSceneObj->PlayAnim( m_pPathAnim[ pathKey]);

			if( m_PathKey != -1 && m_PathKey != HPA_NONE )
				_PlayStateSound( HSND_STATE_LANDING);
			else if( m_PathKey == -1)
			{
				m_pRootDummy->AddChild( m_pStateTimeSequence[ HSND_STATE_LANDING]);
				m_pStateTimeSequence[ HSND_STATE_LANDING]->OnChangeTime( 1.f);
				m_pStateTimeSequence[ HSND_STATE_LANDING]->ActivateNode();
				m_nSoundState = HSND_STATE_LANDING;
			}
		}
	}

	if( m_pArrowMark != nullptr)
	{
		if( (pathKey != HPA_NONE) || (getCurrentHP() == 0.f))
			m_pArrowMark->SetEnable( false);
		else
			m_pArrowMark->SetEnable( true);
	}

	m_rPathLocalTime	= rStartTime;
	m_PathKey			= pathKey;
	m_rSendCountTime	= 0.f;
//	I3TRACE( "Helicopter Anim NUm : %d\n", pathKey);
}

void CGameObjectRidingHelicopter::_ProcessLanding( REAL32 rDeltaSeconds)
{
	if( !m_bDestroyed )
	{
		m_rPathLocalTime += rDeltaSeconds;
		REAL32 rDuration = m_pPathAnim[ m_PathKey]->GetDuration();

		if( m_rPathLocalTime + rDeltaSeconds > rDuration )
		{
			_SetPathAnimKey( HPA_NONE);
			LeaveObject();

			HUD::instance()->SetParameter(HUD::DASH_BOARD, HUD::DBS_SetRidingHelicopter, false);
			m_bHelicopterRiding = false;
		}
	}
}

void CGameObjectRidingHelicopter::EnableDustEffect( bool bEnable)
{
	if( bEnable)
	{
		if( m_pUH60_DustEffect == nullptr )
		{
			VEC3D vec;
			i3Vector::Set( &vec, 0.f, -1.f, 0.f);

			i3CollideeLine line;
			line.SetStart( GetPos());
			line.SetDir( &vec);

			I3_PHYSIX_HIT_RESULT * pResult;
			pResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK);
			if( pResult != nullptr)
			{
				I3ASSERT( pResult->m_pDestShapeSet != nullptr);
				i3Vector::Copy( &m_vFieldPos, GetPos());
				i3Vector::AddY( &m_vFieldPos, - pResult->m_T);

				m_pUH60_DustEffect = g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_UH60_DUST_COMMON, &m_vFieldPos, &I3_YAXIS);
			}
		}
		else
		{
			if( m_pUH60_DustEffect->IsFlag( I3_NODEFLAG_DISABLE))
			{
				m_pUH60_DustEffect = g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_UH60_DUST_COMMON, &m_vFieldPos, &I3_YAXIS);
			}
		}
	}
	else
	{
		if( m_pUH60_DustEffect != nullptr)
		{	
			m_pUH60_DustEffect->DeactivateNode();
			m_pUH60_DustEffect = nullptr;
		}
	}
}


void CGameObjectRidingHelicopter::Process( REAL32 rDeltaSeconds)
{
	// 제압모드에서 스테이지 점프시 문제가 발생할 경우가 있음
	if( m_PathKey < 0 )
		return;

	CStageBattle * pStage = g_pFramework->GetStageBattle();
	if( pStage == nullptr)
		return;

	GAMEINFO_OBJECT * pObjectInfo = BattleObjContext::i()->setGameInfo_Object( getNetIndex());

	bool bHost = BattleServerContext::i()->IsP2PHost();

	if( getCurrentHP() == 0.f)
	{	// 파괴되면 보내줘야 합니다.
		if( m_nSoundState != -1)
		{
			m_pStateTimeSequence[ m_nSoundState]->DeactivateNode();
			m_pStateTimeSequence[ m_nSoundState]->RemoveFromParent();
			m_nSoundState = (HELICOPTER_SOUND_STATE) -1;
		}
	}

	if( bHost == false)
	{
		T_OBJ objType = pObjectInfo->_tNetData.getObjectType();
		if( objType & UDP_OBJECT_TYPE_HP)
		{
			REAL32 rDamage = getCurrentHP() - (REAL32) pObjectInfo->_tNetData.getHP();
			if( rDamage > 0.f)			Damage( rDamage, nullptr, nullptr);

			if( getCurrentHP() <= 0.f)
				m_bDestroyed = true;
		}

		if( m_bDestroyed == false)
		{
			UINT8 path = (UINT8) m_PathKey;
			if( objType & UDP_OBJECT_TYPE_ANIMPATH)
				path = pObjectInfo->_tNetData.getPathKey();

			REAL32 keyframe = m_rPathLocalTime;
			if( objType & UDP_OBJECT_TYPE_ANIMKEY)
			{
				keyframe = pObjectInfo->_tNetData.getKeyFrame();
			}

			if( (path != m_PathKey) ||
				(i3Math::abs( keyframe - m_rPathLocalTime) > 0.5f) )
			{
				_SetPathAnimKey( (HELICOPTER_PATH_ANIM) path, keyframe);
			}
		}
	}

	UINT64 keyStroke = g_pFramework->getKeyStroke();

	CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());

	switch( m_PathKey)
	{
	case HPA_NONE :
	case HPA_GROUND_IDLE_IN_TUTORIAL :
		// isLocal
		{
			if( m_bMyEnterObject )
			{
				CHud_Helicopter::instance()->SetRidingMark( ECM_FLY_AND_CONTROL_M197);
				
				if( EnableOnOffObject(keyStroke) )
				{
					I3ASSERT( pPlayer != nullptr);
					// Enable helicopter
					if( bHost)
					{	// 방장은 바로 오브젝트를 활성화 합니다.
						// Heli위에 타고 있어야만 탑승 가능하도록 설정합니다.
						if (IsPossibleEnterLeaveInObject())
						{
							EnableObject();
							EnterObject(pPlayer);
						}
						// 위에 없을 경우 패킷 전송도 하면 안되서 break; 처리
						else break;
					}

					GAMEEVENT::Write_Chara_UseObject( pPlayer, this, NET_ACTIONKEY_USEOBJECT );
					GAMEEVENT::Write_Chara_SyncUseObject( pPlayer, this, NET_SYNCKEY_ON_OBJECT );
					m_bUseObject_Flag = true;
					// 클라이언트는 방장에게 이벤트를 보냅니다.
				}

			}
			else
			{
				if( IsPossibleEnterLeaveInObject())
					EnterLeaveProcess(keyStroke, true);

				if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) == false ||
					(MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_OUTPOST) == false )
				{
					CHud_Helicopter::instance()->SetRidingMark( ECM_NONE);
				}
			}
		}
		break;
	case HPA_TAKE_OFF_IN_TUTORIAL :
		{
			m_rPathLocalTime += rDeltaSeconds;
			REAL32 rDuration = m_pPathAnim[ m_PathKey]->GetDuration();

			if( m_rPathLocalTime + rDeltaSeconds > rDuration )
			{
				_SetPathAnimKey( HPA_AIR_IDLE_IN_TUTORIAL);
			}
			if(IsPossibleEnterLeaveInObject())
			{
				EnterLeaveProcess(keyStroke);
				SelectRidingMark(pStage);
			}
			else
			{
				CHud_Helicopter::instance()->SetRidingMark( ECM_NONE);
			}
		}
		break;
	case HPA_AIR_IDLE_IN_TUTORIAL :
		if(IsPossibleEnterLeaveInObject())
		{
			EnterLeaveProcess(keyStroke);
			SelectRidingMark(pStage);
		}
		else
		{
			_SetPathAnimKey( HPA_LAND_IN_TUTORIAL);
			CHud_Helicopter::instance()->SetRidingMark( ECM_NONE);
		}
		break;
	case HPA_LAND_IN_TUTORIAL :
		{
			m_rPathLocalTime += rDeltaSeconds;
			REAL32 rDuration = m_pPathAnim[ m_PathKey]->GetDuration();

			if( m_rPathLocalTime + rDeltaSeconds > rDuration )
			{
				_SetPathAnimKey( HPA_GROUND_IDLE_IN_TUTORIAL);
			}
		}
		break;
	default :
		if(IsPossibleEnterLeaveInObject())
		{	// 탑승하고 있는 경우 처리
			if ((getMaxHP() > 0.f && (getCurrentHP() / getMaxHP()*100.f <= 30.f)) && 
			   (MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_OUTPOST))
			{
				if (m_bWarningNotice)
				{
					m_bWarningNotice = false;
					CHud_Helicopter::instance()->UpdateWarningNotice();
				}
			}

			if (m_bHelicopterRiding == false)
			{
				HUD::instance()->SetParameter(HUD::DASH_BOARD, HUD::DBS_SetRidingHelicopter, true);
				m_bHelicopterRiding = true;
			}

			HUD::instance()->SetParameter(HUD::DASH_BOARD, HUD::DBS_SetUnderAttack, (INT32)(getCurrentHP()));

			EnterLeaveProcess(keyStroke);

			SelectRidingMark(pStage);
		}
		else
		{
			if (m_bHelicopterRiding == true)
			{
				HUD::instance()->SetParameter(HUD::DASH_BOARD, HUD::DBS_SetRidingHelicopter, false);
				m_bHelicopterRiding = false;
			}
		}
		
		if( m_pPathAnim[ m_PathKey] != nullptr )
		{
			// 네트워크 싱크를 맞춥니다.
			if( bHost )
			{
				bool bSend = false;

				m_rSendCountTime+= rDeltaSeconds;
				if( m_rSendCountTime < HELICOPTER_SENDCOUNT_TIME)
					bSend = true;

				// EventSender 추가 2011.04.20 swoongoo
				INT32 EventSender = BattleSlotContext::i()->getMySlotIdx();

				if( i3::kind_of<CGameCharaBase*>(getEventSender()))
				{
						CGameCharaBase * pES = (CGameCharaBase*) getEventSender();
						EventSender = pES->getCharaNetIndex();
				}

				GAMEEVENT::Write_Object_AnimateForOutpost( getNetIndex(), EventSender, (UINT8) m_PathKey, (UINT16) getCurrentHP(), m_rPathLocalTime, bSend);
			}

			// Landing process
			_ProcessLanding( rDeltaSeconds);
		}
		break;
	}

	if(IsEnableTime() == false)
	{
		CHud_Helicopter::instance()->SetRidingMark( ECM_NONE);
	}
	
	if( pPlayer != nullptr)
		SendLeaveSyncUH60();
}

bool CGameObjectRidingHelicopter::IsPossibleEnterLeaveInObject()
{
	CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());

	if(pPlayer != nullptr && pPlayer->isCharaStateMask( CHARA_STATE_ONOBJECT) &&
		(pPlayer->isCharaStateMask( CHARA_STATE_DEATH) == false) && (pPlayer->getAttachedObject() == this) )
	{
		return true;
	}

	return false;
}

void CGameObjectRidingHelicopter::SelectRidingMark(CStageBattle* pStage)
{
	CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());

	if( pPlayer != nullptr)
	{
		if( IsEnableEnter(pPlayer))
			CHud_Helicopter::instance()->SetRidingMark( ECM_CONTROL_M197);
		else if( pPlayer->getCharaNetIndex() == GetEnterCharaIdx())
			CHud_Helicopter::instance()->SetRidingMark( ECM_UNCONTROL_M197);
		else
			CHud_Helicopter::instance()->SetRidingMark( ECM_NONE);
	}
	else
		CHud_Helicopter::instance()->SetRidingMark( ECM_NONE);
}

void CGameObjectRidingHelicopter::EnterLeaveProcess(UINT64 keyStroke, bool bOnlyLeave)
{
	bool bHost = BattleServerContext::i()->IsP2PHost();

	CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
	if (pPlayer == nullptr)
		return;

	if (EnableOnOffObject(keyStroke))
	{
		if (GetWeaponOnObject()->IsControlUser())
		{
			if (bHost)
			{
				LeaveObject();
			}

			GAMEEVENT::Write_Chara_UseObject(pPlayer, this, NET_ACTIONKEY_LEAVE_WEAPON_ON_OBJECT );
			m_bLeaveWeapon_Flag = true;
		}
		else if (IsEnableEnter(pPlayer) && bOnlyLeave == false)
		{
			if (bHost)
			{
				EnterObject(pPlayer);
			}

			GAMEEVENT::Write_Chara_UseObject(pPlayer, this, NET_ACTIONKEY_ENTER_WEAPON_ON_OBJECT);
			GAMEEVENT::Write_Chara_SyncUseObject(pPlayer, this, NET_SYNCKEY_ON_OBJECT);
			m_bEnterWeapon_Flag = true;
		}
	}
}

void CGameObjectRidingHelicopter::SendLeaveSyncUH60()
{
	CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	I3ASSERT( pPlayer != nullptr);

	if( GetEnterCharaIdx() != pPlayer->getCharaNetIndex() )
	{
		GAMEEVENT::Write_Chara_SyncUseObject( pPlayer, this, NET_SYNCKEY_OFF_OBJECT);
	}
}

void CGameObjectRidingHelicopter::SetControledObjectState( INT32 state)
{
}

void CGameObjectRidingHelicopter::ProcessIntrude( const GAMEINFO_OBJECT_NETDATA * pNetData)
{
	CGameObjectWithWeapon::ProcessIntrude( pNetData);

	if( m_pArrowMark != nullptr)
	{
		if( (getCurrentHP() == 0.f))
			m_pArrowMark->SetEnable( false);
	}
}

void CGameObjectRidingHelicopter::InitSearchArrowMarkObject( void)
{
	if( g_pFramework->IsBattleStage())
	{
		i3Stage * pStage = g_pFramework->GetCurrentStage();
		StageObjectListPtr pList = pStage->GetStageObjectList();

		if( pList != nullptr)
		{
			for( size_t i = 0; i < pList->size(); i++)
			{
				i3Object * pObj = (*pList)[i];
				if( i3::same_of<CGameObjectMark_OutPost*>(pObj))
				{
					m_pArrowMark = pObj;
					break;
				}
			}
		}
	}
}

