#include "pch.h"
#include "ObjectStormTubeRepairTrain.h"
#include "GlobalVariables.h"
#include "GlobalFunc.h"
#include "GameCharaMoveContext.h"
#include "BattleServerContext.h"
#include "BattleSlotContext.h"

#define STRT_SEND_PACKET_LIMIT_COUNT			5		

const char * s_AnimName_RepairTrain[] =
{
	"Object/moving/Train_Repair_Start.i3a",
	"Object/moving/Train_Repair_Move.i3a",
	"Object/moving/Train_Repair_End.i3a",
};

const char * s_SoundName_RepairTrain[ RTSND_STATE_COUNT] =
{
	"OB_Stormtube_RepairTrain.wav",
	"OB_Stormtube_RepairTrain_move.wav",
};

I3_CLASS_INSTANCE( CGameObjectStormTubeRepairTrain);//, CGameControledObject);

CGameObjectStormTubeRepairTrain::CGameObjectStormTubeRepairTrain(void)
{
	for (INT32 i = 0; i < RTA_COUNT; i++)
		m_pAnim[i] = nullptr;

	InitMemberVaribleSTRT();
	
	m_pRootDummy = nullptr;
	m_rAnimPlayTime = 0.f;

	for( INT32 i = 0; i < RTSND_STATE_COUNT; i++)
		m_pSoundNode[ i ] = nullptr;

	m_nSoundState = (RTSND_STATE) -1;
}

CGameObjectStormTubeRepairTrain::~CGameObjectStormTubeRepairTrain(void)
{
	for( INT32 i = 0; i < RTSND_STATE_COUNT; i++)
	{
		if( m_pSoundNode[ i] != nullptr)
		{
			m_pSoundNode[ i]->DeactivateNode();
			m_pSoundNode[ i]->RemoveFromParent();
		}

		I3_SAFE_RELEASE( m_pSoundNode[ i] );
	}

	for( INT32 i = 0; i < RTA_COUNT; i++)
	{
		I3_SAFE_RELEASE( m_pAnim[i]);
	}
}

void CGameObjectStormTubeRepairTrain::InitMemberVaribleSTRT()
{
	m_rMovelTime = 0.f;
	m_iSendPacketCount = 0;
	m_eRTA = RTA_START;
	m_rAnimPlayTime = 0.f;
}

void CGameObjectStormTubeRepairTrain::LoadAnimSTRT()
{
	for( INT32 i = 0; i < RTA_COUNT; i++)
	{
		if( s_AnimName_RepairTrain[i] != nullptr)
		{
			i3Animation * pAnimation = i3Animation::LoadFromFile( s_AnimName_RepairTrain[i]);

			if( pAnimation == nullptr)
			{
				I3PRINTLOG(I3LOG_WARN, "Couldn't load Animation : %s", s_AnimName_RepairTrain[i]);
			}

			I3_REF_CHANGE( m_pAnim[i], pAnimation);
			I3_SAFE_RELEASE(pAnimation);  // LoadFromFile()은 레퍼런스카운팅을 갖고 있음.(2012.10.18.수빈)
		}
	}

	m_pSceneObj->BuildBoneList();
}

void CGameObjectStormTubeRepairTrain::InitObject()
{
	InitMemberVaribleSTRT();
	LoadAnimSTRT();

	m_pRootDummy = i3Scene::FindNodeByName( getSceneObject(), "Root");
	I3ASSERT( m_pRootDummy != nullptr);

	for( INT32 i = 0; i < RTSND_STATE_COUNT; i++)
	{
		i3GameResSound * pRes = (i3GameResSound*) g_pFramework->QuaryResource( s_SoundName_RepairTrain[ i] );
		I3ASSERT( pRes != nullptr);
		i3SoundPlayInfo * pInfo = pRes->getSound();
		pInfo->setLoopMode( I3SND_RT_LOOPMODE_LOOPING);
		pInfo->set3DMode( I3SND_RT_3DMODE_WORLD_SPACE);

		m_pSoundNode[ i] = i3SoundNode::new_object();
		I3ASSERT( m_pSoundNode[ i] != nullptr);
		m_pSoundNode[ i]->setSoundPlayInfo( pInfo);
	}
	
	m_bManualUpdate = true;
}

void CGameObjectStormTubeRepairTrain::ResetObject()
{	
	InitMemberVaribleSTRT();
	SetAnimSTRT( m_eRTA);
}

void CGameObjectStormTubeRepairTrain::ProcessTest( REAL32 rDeltaSeconds)
{
	i3Transform2 * pTrans = (i3Transform2*) i3Scene::FindNodeByExactType( getSceneObject(), i3Transform2::static_meta() );

	MATRIX mat;
	GlobalFunc::GetNowFrameMatrix(&mat, pTrans);
}

VEC3D* CGameObjectStormTubeRepairTrain::GetVelocityTrain()
{
	MATRIX mat;
	i3Transform2 * pTrans = (i3Transform2*) i3Scene::FindNodeByExactType( getSceneObject(), i3Transform2::static_meta() );

	MATRIX* pMat = pTrans->GetCacheMatrix();
	GlobalFunc::GetNowFrameMatrix(&mat, pTrans);
	
	i3Vector::Sub( &m_v3VelTrain, i3Matrix::GetPos(&mat), i3Matrix::GetPos(pMat));
	
	return &m_v3VelTrain;
}

//void CGameObjectStormTubeRepairTrain::TraceDebugInfo()
//{
//	VEC3D v3Vel;
//	MATRIX mat;
//	
//	i3Transform2 * pTrans = (i3Transform2*) i3Scene::FindNodeByExactType( getSceneObject(), i3Transform2::static_meta() );
//
//
//	MATRIX* pMat = pTrans->GetCacheMatrix();
//	GlobalFunc::GetNowFrameMatrix(&mat, pTrans);
//
//	
//
//	
//	i3Vector::Sub( &v3Vel, i3Matrix::GetPos(&mat), i3Matrix::GetPos(pMat));
//
//	I3TRACE("%f TraceDebugInfo() v3Vel %f %f %f mat %f %f %f GetCacheMatrix %f %f %f  \n", 
//		g_pFramework->GetAccTime(), v3Vel.x, v3Vel.y, v3Vel.z, mat.m30, mat.m31,mat.m32, pMat->m30, pMat->m31, pMat->m32);
//
//
//	i3PhysixShapeSet* pPhysixShapeSet = pTrans->getShapeSet();
//	int i = 0;
//	for(i=0; i<1; i++)
//	{
//		NxShape* pShape = pPhysixShapeSet->getNxShape(i);
//		NxActor* pActor = &(pShape->getActor());
//		m_v3NowPos[i] = pShape->getGlobalPosition();
//
//		
//		NxVec3 vnxVel; 
//		vnxVel.subtract(m_v3NowPos[0], m_v3OldPos[0]);
//
//		GlobalFunc::PB_TRACE("TraceDebugInfo() %s v3Vel %f %f %f  v3Pos %f %f %f m_v3OldPos %f %f %f\n", 
//			pActor->getName(), vnxVel.x, vnxVel.y, vnxVel.z, m_v3NowPos[0].x, m_v3NowPos[0].y, m_v3NowPos[0].z, m_v3OldPos[0].x, m_v3OldPos[0].y, m_v3OldPos[0].z );
//	}
//	 
//	i3ShapeSetContainer * pi3ShapeSet = (i3ShapeSetContainer*) i3Scene::FindNodeByExactType( pTrans, i3ShapeSetContainer::static_meta() );
//	pPhysixShapeSet = pi3ShapeSet->getShapeSet();
//	{
//		NxShape* pShape = pPhysixShapeSet->getNxShape(0);
//		NxActor* pActor = &(pShape->getActor());
//		m_v3NowPos[1] = pShape->getGlobalPosition();
//
//		
//		NxVec3 vnxVel; 
//		vnxVel.subtract(m_v3NowPos[1], m_v3OldPos[1]);
//
//		GlobalFunc::PB_TRACE("TraceDebugInfo() %s v3Vel %f %f %f  v3Pos %f %f %f m_v3OldPos %f %f %f\n", 
//			pActor->getName(), vnxVel.x, vnxVel.y, vnxVel.z, m_v3NowPos[1].x, m_v3NowPos[1].y, m_v3NowPos[1].z, m_v3OldPos[1].x, m_v3OldPos[1].y, m_v3OldPos[1].z );
//	}
//}

void CGameObjectStormTubeRepairTrain::ProcessObject( REAL32 rDeltaSeconds)
{
	CGameControledObject::ProcessObject(rDeltaSeconds);
	if(m_eRTA == RTA_MOVE)
	{
		MoveProcess(rDeltaSeconds);
	}

	// 오브젝트 사운드 플레이
	if( m_eRTA == RTA_MOVE && m_nSoundState != RTSND_STATE_MOVE)
	{
		if( m_nSoundState != -1)
		{
			m_pSoundNode[ m_nSoundState]->DeactivateNode();
			m_pSoundNode[ m_nSoundState]->RemoveFromParent();
		}

		m_pRootDummy->AddChild( m_pSoundNode[ RTSND_STATE_MOVE]);
		m_pSoundNode[ RTSND_STATE_MOVE]->ActivateNode();
		m_nSoundState = RTSND_STATE_MOVE;
	}

	// TraceDebugInfo();

	/*int i = 0;
	for(i=0; i<10; i++)
	{
		m_v3OldPos[i] = m_v3NowPos[i];
	}*/
}

void CGameObjectStormTubeRepairTrain::MoveProcess(REAL32 rDeltaSeconds)
{
	m_rMovelTime += rDeltaSeconds;

	I3ASSERT(m_pAnim[m_eRTA]);
	REAL32 rDuration = m_pAnim[m_eRTA]->GetDuration();

	// GlobalFunc::PB_TRACE("MoveProcess m_eRTA %d m_rMovelTime + rDeltaSeconds %f rDuration %f \n", m_eRTA, (m_rMovelTime + rDeltaSeconds), rDuration);

	if( m_rMovelTime + rDeltaSeconds > rDuration)
	{
		CGameCharaBase * pMyChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());

		if( pMyChara != nullptr && pMyChara->getMoveContext() != nullptr)
		{
			pMyChara->getMoveContext()->OverlapPush();
		}
		//GlobalFunc::PB_TRACE("RTA_END m_eRTA %d m_rMovelTime + rDeltaSeconds %f rDuration %f \n", m_eRTA, (m_rMovelTime + rDeltaSeconds), rDuration);
		SetAnimSTRT( RTA_END);
	}
}

void CGameObjectStormTubeRepairTrain::MoveHostProcess(REAL32 rDeltaSeconds)
{
	I3ASSERT( BattleServerContext::i()->IsP2PHost());

	m_iSendPacketCount++;
	if( m_iSendPacketCount < STRT_SEND_PACKET_LIMIT_COUNT)
	{
		// EventSender 추가 2011.04.20 swoongoo
		INT32 EventSender = BattleSlotContext::i()->getMySlotIdx();
		if( i3::kind_of<CGameCharaBase*>(getEventSender()))
		{
			CGameCharaBase * pES = (CGameCharaBase*) getEventSender();
			EventSender = pES->getCharaNetIndex();
		}

		GAMEEVENT::Write_Object_AnimateForOutpost( getNetIndex(), EventSender, (UINT8) m_eRTA, (UINT16) getCurrentHP(), m_rMovelTime, true);
	}
}

void CGameObjectStormTubeRepairTrain::SetMoveAnimSTRT()
{
	SetAnimSTRT(RTA_MOVE);
}

void CGameObjectStormTubeRepairTrain::SetAnimSTRT(REPAIR_TRAIN_ANIM animKey)
{
	I3ASSERT(animKey < RTA_COUNT);

	if(animKey == RTA_MOVE)
	{
		m_rMovelTime = 0.f;
	}

	m_eRTA = animKey;

	// GlobalFunc::PB_TRACE("animKey %d \n", animKey);

	I3ASSERT(m_pAnim[m_eRTA]);

	m_pSceneObj->PlayAnim(m_pAnim[m_eRTA], I3_ANIM_BLEND_MODE_REPLACE, 0.0f, 0.0f, 1.0f, 1.0f, I3_ANIM_PLAY_MODE_CLAMP);

	// m_pSceneObj->PlayAnim(m_pAnim[m_eRTA]);
	

	m_rMovelTime = 0;
	m_iSendPacketCount = 0;

	if( m_eRTA != RTA_MOVE)
	{
		if( m_nSoundState != -1)
		{
			m_pSoundNode[ m_nSoundState]->DeactivateNode();
			m_pSoundNode[ m_nSoundState]->RemoveFromParent();
		}
		m_pRootDummy->AddChild( m_pSoundNode[ RTSND_STATE_IDLE]);
		m_pSoundNode[ RTSND_STATE_IDLE]->ActivateNode();
		m_nSoundState = RTSND_STATE_IDLE;
	}
}
