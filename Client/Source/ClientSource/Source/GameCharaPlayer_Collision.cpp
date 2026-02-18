#include "pch.h"
#include "GameChara.h"
#include "StageBattle.h"
#include "GlobalVariables.h"
#include "Weapon.h"

#define			RADIUS_NETWORK	0.45f
#define			TALL_NETWORK	1.5f//0.78f


#if defined( I3_DEBUG)
//#define SHOW_CHARCOL_MESH
#endif 

/*
class PlayerController : public i3PhysixController
{
	I3_CLASS_DEFINE( PlayerController);

protected:
	CGameCharaPlayer * m_pOwner;

public:
	PlayerController(void);
	virtual ~PlayerController(void);

	void	setOwner( CGameCharaPlayer * pPlayer){	I3_REF_CHANGE( m_pOwner, pPlayer); }
	CGameCharaPlayer * getOwner(void)			 {	return m_pOwner;}

	virtual NxControllerAction		OnShapeHit( const NxControllerShapeHit * pHit)
	{
		// 오브젝트와 컬리젼 되면 오브젝트의 위치가 변했는지 확인합니다.
		if( pHit->shape != NULL )
		{
			m_HitNormal.x = pHit->worldNormal.x; m_HitNormal.y = pHit->worldNormal.y; m_HitNormal.z = pHit->worldNormal.z;

			if( pHit->shape->getGroup() == CGRP_OBJECT || pHit->shape->getGroup() == CGRP_OBJECT_WHOLE )
			{
				//m_pOwner->CalculateLadderEnable((void*) pHit);
				
				m_HitDir.x = pHit->dir.x; m_HitDir.y = pHit->dir.y; m_HitDir.z = pHit->dir.z;

				m_rLength = pHit->length;

				m_pHitShape = pHit->shape;
			}
			else
			{
				m_pHitShape = NULL;
			}

			m_HitPos.x = pHit->worldPos.x; m_HitPos.y = pHit->worldPos.y; m_HitPos.z = pHit->worldPos.z;
		}

		return NX_ACTION_NONE;
	}

	virtual	NxControllerAction		OnControllerHit( const NxControllersHit * pHit)
	{
		// 컨트롤러 같이 히트가 되면 밀어내야합니다.
		// 충돌 처리 후 위치를 다시 계산할 필요가 있습니다..
		/*
		if( pHit->controller != NULL )
		{
			NxExtendedVec3 pos;

			pos = pHit->controller->getPosition();

			VEC3D vecOther;
			VEC3D vDir;
			VEC3D * pVecMyPos = g_pPlayer->GetPos();

			i3PhysixController * pController = (i3PhysixController*) pHit->controller->getUserData();

			setX( &vecOther, (REAL32) pos.x);
			setY( &vecOther, (REAL32) pos.y - pController->getHeight() );
			setZ( &vecOther, (REAL32) pos.z);

			i3Vector::Sub( &vDir, pVecMyPos, &vecOther );
			REAL32 rLength = i3Vector::Normalize( &vDir, &vDir );
			if( rLength < 0.7f )
			{
				rLength = (1.f - rLength);
				i3Vector::Scale( &vDir, &vDir, rLength );
				i3Vector::Add( &vDir, &vDir, pVecMyPos );
				g_pPlayer->forceSetPos( &vDir );
			}
		}
		
		//I3TRACE( "CONTROLLER HIT\n");
		return NX_ACTION_NONE;
	}
};


I3_CLASS_INSTANCE( PlayerController, i3PhysixController);
PlayerController::PlayerController(void)
{
	m_pOwner = NULL;
}

PlayerController::~PlayerController(void)
{
	I3_SAFE_RELEASE( m_pOwner);
}


/////////////////////////////////////////////////////////////////////////////////
// Collision
void CGameCharaPlayer::_AttachWorldCollision()
{
	//m_pMoveContext->CreateController();
	
	////////
	// Character Controller
	INT32 i;
	
	{
		NxCapsuleControllerDesc capsuledesc;
		NxBoxControllerDesc boxdesc;

		NxControllerDesc * pDesc;

		REAL32 rRadius = RADIUS;
		REAL32 rTall = TALL;
		REAL32 rSkinWidth = 0.025f;
		if( isNetwork() )
		{
			rRadius = RADIUS_NETWORK;
			rTall = TALL_NETWORK;
			rSkinWidth = 0.4f;

			capsuledesc.position.set( 0.0f, 0.3f, 0.0f);
		}
		else
		{
			capsuledesc.position.set( 0.0f, 0.0f, 0.0f);
		}

		capsuledesc.radius = rRadius;
		capsuledesc.height = max( 0.1f, rTall );
		//capsuledesc.climbingMode = CLIMB_EASY;		// Ageia 2.7.3 

		pDesc = &capsuledesc;

		pDesc->skinWidth = rSkinWidth;
		pDesc->slopeLimit = i3Math::cos( I3_DEG2RAD( 45));	// 경사각이 45이상이면 이동하지 못하도록 합니다.
		pDesc->stepOffset = 0.4f;							// 턱의 높이가 40센티 이상이면 넘지 못합니다.

		I3ASSERT( pDesc->isValid());

		m_pController = PlayerController::NewObject();
		I3ASSERT( m_pController != NULL);

		if( !isNetwork() )
			m_pController->Create( CGRP_MYCHARA_BOUND, pDesc);
		else
			m_pController->Create( CGRP_CHARA_BOUND, pDesc);

		m_pController->setHeight( (rTall * 0.5f) + rRadius );
		((PlayerController*)m_pController)->setOwner( this);

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

		pAct->raiseBodyFlag( NX_BF_DISABLE_GRAVITY );
	}

	// 이팩트용 컬리젼입니다.
	if( m_pContactHitList != NULL && g_pEnvSet->m_nEffectLevel > 1)
	{
		i3Stage * pStage = g_pFramework->GetCurrentStage();
		i3World * pWorld = pStage->getWorld();

		if (g_pFramework->IsBattleStage())
		{
			VEC3D vec, vCenter;
			i3PhysixShapeSet * pShapeSet = NULL;
			
			VEC3D vZero;
			i3Vector::Zero( &vZero);

			i3Skeleton * pSkel = m_pBody->getFrameLOD()->getSkeleton();
			
			// Root shape
			pShapeSet = i3PhysixShapeSet::NewObject();
			pShapeSet->CreateSphereShape( &vZero, 0.25f, CGRP_ITEM);
			pShapeSet->SetName( "ContactShapeRoot");
			pShapeSet->getNxActor()->setGroup( CGRP_ITEM);
			pShapeSet->getNxShape(0)->setName( "ContactShape_Root");
			pShapeSet->getNxShape(0)->setFlag( NX_SF_POINT_CONTACT_FORCE, true);
			pShapeSet->setEventReceiver( this);
			pShapeSet->modifyDynamicState( I3_PHYSIX_STATE_NO_GRAVITY | I3_PHYSIX_STATE_NO_COLLISION | I3_PHYSIX_STATE_NO_RAYCAST, 0);

			
			if( pWorld->getWorldCollision() != NULL)
			{
				i3Scene::SetActorPairFlag( pWorld->getWorldCollision()->getInstanceSg(), pShapeSet->getNxActor(), NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_END_TOUCH);
			}

			if( pStage->GetStageObjectList() != NULL )
			{
				for( i = 0; i < pStage->GetStageObjectList()->GetCount(); i++ )
				{
					i3Object * pObj = (i3Object*) pStage->GetStageObjectList()->GetItem( i );
					i3Scene::SetActorPairFlag( pObj->GetSceneObject(), pShapeSet->getNxActor(), NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_END_TOUCH );
				}
			}

			m_pContactHitList->Add( pShapeSet);

			// Spine3 shape
			pShapeSet = i3PhysixShapeSet::NewObject();
			pShapeSet->CreateSphereShape( &vZero, 0.25f, CGRP_ITEM);
			pShapeSet->SetName( "ContactShapeSpine3");
			pShapeSet->getNxActor()->setGroup( CGRP_ITEM);
			pShapeSet->getNxShape(0)->setName( "ContactShape_Spine3");
			pShapeSet->getNxShape(0)->setFlag( NX_SF_POINT_CONTACT_FORCE, true);
			pShapeSet->setEventReceiver( this);
			pShapeSet->modifyDynamicState( I3_PHYSIX_STATE_NO_GRAVITY | I3_PHYSIX_STATE_NO_COLLISION | I3_PHYSIX_STATE_NO_RAYCAST, 0);

			if( pWorld->getWorldCollision() != NULL)
			{
				i3Scene::SetActorPairFlag( pWorld->getWorldCollision()->getInstanceSg(), pShapeSet->getNxActor(), NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_END_TOUCH);
			}
			if( pStage->GetStageObjectList() != NULL )
			{
				for( i = 0; i < pStage->GetStageObjectList()->GetCount(); i++ )
				{
					i3Object * pObj = (i3Object*) pStage->GetStageObjectList()->GetItem( i );
					i3Scene::SetActorPairFlag( pObj->GetSceneObject(), pShapeSet->getNxActor(), NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_END_TOUCH );
				}
			}
			m_pContactHitList->Add( pShapeSet);


			if( g_pEnvSet->m_nEffectLevel > 2)
			{	// L Foot
				i3PhysixShapeSet * pShapeSetRoot = pSkel->getShapeSet( m_idxBone[BONE_LFOOT]);
				pShapeSetRoot->getShapeMassCenterPos( &vec);
				i3Vector::Set( &vCenter, -getX( &vec) * 0.5f, -getY( &vec), 0.f );

				pShapeSet = i3PhysixShapeSet::NewObject();
				pShapeSet->CreateSphereShape( &vCenter, 0.1f, CGRP_ITEM);
				pShapeSet->SetName( "ContactShapeLFoot");
				pShapeSet->getNxActor()->setGroup( CGRP_ITEM);
				pShapeSet->getNxShape(0)->setName( "ContactShape_LFoot");
				pShapeSet->setEventReceiver( this);
				pShapeSet->modifyDynamicState( I3_PHYSIX_STATE_NO_GRAVITY | I3_PHYSIX_STATE_NO_RESPONSE | I3_PHYSIX_STATE_NO_RAYCAST, 0);

				if( pWorld->getWorldCollision() != NULL)
				{
					i3Scene::SetActorPairFlag( pWorld->getWorldCollision()->getInstanceSg(), pShapeSet->getNxActor(), NX_NOTIFY_ON_START_TOUCH);
				}
				if( pStage->GetStageObjectList() != NULL )
				{
					for( i = 0; i < pStage->GetStageObjectList()->GetCount(); i++ )
					{
						i3Object * pObj = (i3Object*) pStage->GetStageObjectList()->GetItem( i );
						i3Scene::SetActorPairFlag( pObj->GetSceneObject(), pShapeSet->getNxActor(), NX_NOTIFY_ON_START_TOUCH );
					}
				}
				m_pContactHitList->Add( pShapeSet);

				// R Foot
				pShapeSetRoot = pSkel->getShapeSet( m_idxBone[BONE_RFOOT]);
				pShapeSetRoot->getShapeMassCenterPos( &vec);
				i3Vector::Set( &vCenter, getX( &vec) * 0.5f, -getY( &vec), 0.f );
			
				pShapeSet = i3PhysixShapeSet::NewObject();
				pShapeSet->CreateSphereShape( &vCenter, 0.1f, CGRP_ITEM);
				pShapeSet->SetName( "ContactShapeRFoot");
				pShapeSet->getNxActor()->setGroup( CGRP_ITEM);
				pShapeSet->getNxShape(0)->setName( "ContactShape_RFoot");
				pShapeSet->setEventReceiver( this);
				pShapeSet->modifyDynamicState( I3_PHYSIX_STATE_NO_GRAVITY | I3_PHYSIX_STATE_NO_RESPONSE | I3_PHYSIX_STATE_NO_RAYCAST, 0);

				if( pWorld->getWorldCollision() != NULL)
				{
					i3Scene::SetActorPairFlag( pWorld->getWorldCollision()->getInstanceSg(), pShapeSet->getNxActor(), NX_NOTIFY_ON_START_TOUCH);
				}
				if( pStage->GetStageObjectList() != NULL )
				{
					for( i = 0; i < pStage->GetStageObjectList()->GetCount(); i++ )
					{
						i3Object * pObj = (i3Object*) pStage->GetStageObjectList()->GetItem( i );
						i3Scene::SetActorPairFlag( pObj->GetSceneObject(), pShapeSet->getNxActor(), NX_NOTIFY_ON_START_TOUCH );
					}
				}
				m_pContactHitList->Add( pShapeSet);
			}
		}
	}
}

void CGameCharaPlayer::_AttachRoughHitCollision()
{
	UINT32 nColGroup = CGRP_TEAM_BLUE_ROUGH;
	if( m_pCharaInfo->_Team & CHARACTER_TEAM_RED)
		nColGroup = CGRP_TEAM_RED_ROUGH;
	
	m_pRoughHitTrans = i3Transform2::NewObjectRef();
	#if defined( I3_DEBUG)
	m_pRoughHitTrans->SetName( "Rough_Coll_Sphere");
	#endif

	m_pSceneObj->AddChild( m_pRoughHitTrans);

	{
		i3PhysixShapeSet *	pShapeSet = i3PhysixShapeSet::NewObjectRef();

		VEC3D	vZero;
		i3Vector::Set( &vZero, 0.0f, 0.f, 0.f);

		MATRIX mat;
		i3Matrix::Identity( &mat);
		{	
			i3Matrix::SetPos( &mat, 0.f, 1.f, 0.f, 1.f );
		}
		
		m_pRoughHitTrans->SetMatrix( &mat );
		pShapeSet->CreateSphereShape( &vZero, m_rColRadius, nColGroup);
		pShapeSet->setEventReceiver( this);

		m_pRoughHitTrans->setShapeSet( pShapeSet);
		m_pRoughHitTrans->modifyDynamicState( I3_PHYSIX_STATE_KINEMATIC | I3_PHYSIX_STATE_NO_GRAVITY | I3_PHYSIX_STATE_NO_RESPONSE , 0);
	}
    
	//렌더링용 충돌 Mesh생성
	#if defined( I3_DEBUG) && defined( SHOW_CHARCOL_MESH)
	//m_pRenderableRoughHitCol = i3SceneUtil::BuildRendableCollider( m_pRoughHitColSet, m_pSceneObj, TRUE, TRUE);
	#endif
}


void CGameCharaPlayer::AttachCollisionNode( i3Node * pParentNode, UINT32 nColGroupIndex)
{
	DetachCollisionNode();

	if( m_pCharaInfo->_Team & CHARACTER_TEAM_RED)
		SetCollisionGroup( CGRP_TEAM_RED, CGRP_MASK_TEAM_RED );
	else
		SetCollisionGroup( CGRP_TEAM_BLUE, CGRP_MASK_TEAM_BLUE );

	//World Collision
	//if( this == g_pPlayer)
	_AttachWorldCollision();

	//HitCollision Init
	_AttachRoughHitCollision();
}

void CGameCharaPlayer::_DetachWorldCollision()
{
	if( m_pMoveContext != NULL)
		m_pMoveContext->ReleaseController();

	//렌더링용 충돌 Mesh제거
#if defined( I3_DEBUG) && defined( SHOW_CHARCOL_MESH)
	m_pSceneObj->RemoveChild( m_pRenderableWorldCol);
#endif
}
/*
void CGameCharaPlayer::_DetachRoughHitCollision()
{
	if( m_pRoughHitTrans != NULL)
	{
		m_pSceneObj->RemoveChild( m_pRoughHitTrans);
	}
}


void CGameCharaPlayer::_ChangeControllerHeight( REAL32 rTall )
{	
	I3ASSERT( m_pController != NULL );

	if( isNetwork() )
		return;

	m_pController->setHeight( (rTall * 0.5f) + RADIUS);
	
	if( m_pController->getNxController()->getType() == NX_CONTROLLER_BOX )
	{
	}
	else
	{
		NxCapsuleController * pNxController = (NxCapsuleController*)m_pController->getNxController();
		pNxController->setHeight( rTall );
	}
	m_pController->setPosition( GetPos() );
}


void CGameCharaPlayer::DetachCollisionNode()
{
	if( m_pSceneObj == NULL)	return;

	_DetachWorldCollision();

	//_DetachRoughHitCollision();
}

//void CGameCharaPlayer::SetCollisionGroup( UINT16 nGroup, UINT32 nMask)
//{	
//	m_nCollisionGroup = nGroup;
//	
//	InitColliderSet( nGroup, nMask );
//}


void CGameCharaPlayer::EnableCollisionGroup( BOOL bEnable)
{
	INT32 i;
	
	i3Skeleton * pSkel = GetSceneObject()->GetBody()->getFrameLOD()->getSkeleton();

	// Team 별로 Collision Group을 설정.
	if( !bEnable )
	{
		for( i = 0; i < pSkel->getBoneCount(); i++ )
		{
			i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i );
			if( pShapeSet != NULL )
			{
				pShapeSet->setShapeGroup( CGRP_DUMMY_CHARA );
			}
		}
	}
	else
	{
		for( i = 0; i < pSkel->getBoneCount(); i++ )
		{
			i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i );
			if( pShapeSet != NULL )
			{
				pShapeSet->setShapeGroup( m_nCollisionGroup );
			}
		}
	}
}
*/

typedef struct __tag_ColTemp
{
	REAL32	r_T;
	VEC3D	r_Normal;
}COLLISION_INFO_FORCOMARE;

/*
void CGameCharaPlayer::_CheckWorldCollision( REAL32 tm)
{
	m_pMoveContext->CheckWorldCollision( tm);

	
	if( isNetwork() )	return;
	if( isAlive() == FALSE && isStateMask( CHARA_STATE_FLOOR) ) return;
	if( !isAlive() )	return;

	if( m_pController == NULL )
	{
		I3WARN( "m_pController is NULL." );
		return;
	}

	REAL32	y		= m_Vel.y;
	m_Vel.y			= 0.0f;

	if( i3Vector::LengthSq( &m_Accel) > I3_EPS)
	{
		VEC3D accl, vVel, vAccel;

		i3Vector::Normalize( &vAccel, &m_Accel);

		REAL32 rVel		= i3Vector::Normalize( &vVel, &m_Vel);
		REAL32 rDrive	= i3Vector::Dot( &vVel, &vAccel);

		if( !getActionContext()->isLower( CHARA_LOWER_JUMP))
		{
			if( rVel > I3_EPS)//이동 하던 중이었다면
			{
				m_tmAccelElapsed += tm * ( rDrive -  m_pDBInfo->m_rAccelRateBackVsFront);
			}
			else
			{
				m_tmAccelElapsed += tm;	//속도가 매우 작은경우와 가속도가 매우 작은경우를 위해 += 로 사용.
			}
		}

		m_tmAccelElapsed = MINMAX( 0.0f, m_tmAccelElapsed, m_pDBInfo->m_rAccelerationTime);

		i3Vector::Scale( &accl, &m_Accel, tm);
		i3Vector::Add( &m_Vel, &m_Vel, &accl);

		i3Vector::Normalize( &m_Vel, &m_Vel);

		REAL32 maxSpeed = m_rMaxMoveSpeed;
		if( isEnabledSpeedReduction())
		{
			maxSpeed *= m_rSpeedReductionRate;
		}

		i3Vector::Scale( &m_Vel, &m_Vel, maxSpeed * tm * i3Math::sin( (m_tmAccelElapsed / m_pDBInfo->m_rAccelerationTime) * I3_PI2));
	}
	else
	{
		if( !getActionContext()->isLower( CHARA_LOWER_JUMP))
		{
			m_tmAccelElapsed -= tm;

			if( m_tmAccelElapsed > 0.0f)
			{
				i3Vector::Scale( &m_Vel, &m_Vel, i3Math::sin( (m_tmAccelElapsed / m_pDBInfo->m_rAccelerationTime) * I3_PI2));

				REAL32 len = i3Vector::Normalize( &m_Vel, &m_Vel);

				len = MIN( len, m_rMaxMoveSpeed * tm);

				i3Vector::Scale( &m_Vel, &m_Vel, len);
			}
			else
			{
				i3Vector::Zero( &m_Vel);

				m_tmAccelElapsed = 0.0f;
			}
		}
	}

	if( isStateMask( CHARA_STATE_CLIMB))
	{
		_CalcClimbingLadder( &m_Vel);
	}
	else
	{
		m_Vel.y = y;
	}
}

void CGameCharaPlayer::_CheckWallCollision(REAL32 tm)
{

}

void CGameCharaPlayer::_ResetJumping()
{
	i3Vector::Copy( &m_TakeOffPos, GetPos());

	m_rElapsedTakeOffTime	= 0.0f;
	m_rCurJumpHeight		= m_pDBInfo->m_rJumpHeight;
	m_rCurJumpTime			= m_pDBInfo->m_rJumpTime * 0.5f;

	removeStateMask( CHARA_STATE_JUMP);

	m_Vel.y =  - 0.1f;
	
}


void CGameCharaPlayer::_UpdateJumping(REAL32 tm)
{	
	I3ASSERT( m_pDBInfo != NULL);

	if( isStateMask( CHARA_STATE_JUMP) && (m_rElapsedTakeOffTime == 0.0f))
	{
		m_Vel.y = i3Math::sin( (tm / m_rCurJumpTime) * I3_PI2) * m_rCurJumpHeight;

		i3Vector::Copy( &m_TakeOffPos, GetPos());

		m_rElapsedTakeOffTime = tm;

		return ;
	}

	if( isStateMask( CHARA_STATE_JUMP ) && (m_rElapsedTakeOffTime < m_rCurJumpTime && ((m_rElapsedTakeOffTime + tm) > m_rCurJumpTime)))
	{
		_ResetJumping();
	}
	else
	{
		if( getCurHP() > 0)
		{
			m_rElapsedTakeOffTime += tm;
		}
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Process 낙하
	if( isStateMask( CHARA_STATE_FLOOR))
	{
		//	낙하 데미지 계산
		if( !isNetwork() && !isInvincible() && m_rElapsedTakeOffTime > 0.1f
			&& (g_pGameContext->getMissionManager()->getMissionState() != GAME_MISSION_STATE_ENDBATTLE
			&& g_pGameContext->getMissionManager()->getMissionState() != GAME_MISSION_STATE_GAMEOVER) )
		{	// 게임이 종료되면 계산하지 말아야 합니다.
			INT32 nDamage = _CalcFallDamage( &m_TakeOffPos);

			//	HP 감소
			if( nDamage > 0)
			{	
				if( g_pGameContext->IsP2PHost() )
				{
					m_pCharaInfo->_Hp -= nDamage;
					if( m_pCharaInfo->_Hp <= 0)
					{
						VEC3D vDir;
						i3Vector::Set( &vDir, 0.f, 0.f, 1.f );
						m_pCharaInfo->_Hp = 0;

						g_pCharaManager->DeathChara( getCharaNetIndex(), getCharaNetIndex(), &vDir, CHARA_DEATH_A );
						g_pGameContext->WriteCharaDeath( getCharaNetIndex(), getCharaNetIndex(), WEAPON_USAGE_MELEE,
							WEAPON_CLASS_KNIFE, WEAPON_NUM_KNIFE_M7, CHARA_DEATH_F, CHARA_HIT_BODY1, &vDir );
					}

					GAMEINFO_RECV * pGameInfo	= g_pGameContext->getRecvGameInfo( getCharaNetIndex() );
					pGameInfo->_tDefData._nHP	= (UINT8) m_pCharaInfo->_Hp;
				}

				g_pGameContext->WriteMyCharaDecentDamage( (UINT8) nDamage );

				ACTION_FallDamage();	//	데미지 동작 및 화면 이펙트

				if( !isNetwork())
				{
					EnableSpeedReduction( (UINT8) nDamage);
				}
			}

			if( m_rElapsedTakeOffTime > 0.1f && m_rElapsedTakeOffTime > tm)
			{
				g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, m_CurFloorTerrainType, GTBT_JUMP_END, GetPos(), is1PV());
			}
		}

		_ResetJumping();
	}
	else
	{
		//if( isStateMask( CHARA_STATE_CLIMB))
		//{
		//	_ResetJumping();
		//	return;
		//}

		REAL32 rNextHeight = 0.0f;
		REAL32	rFac = (m_rElapsedTakeOffTime / m_rCurJumpTime) * I3_PI2;
		if( isStateMask( CHARA_STATE_JUMP))
		{
			rNextHeight = getY( &m_TakeOffPos) + (i3Math::sin(rFac) * m_rCurJumpHeight);
		}
		else
		{
			if( m_rElapsedTakeOffTime >= m_rCurJumpTime)
			{
				REAL32 ac = m_rElapsedTakeOffTime/ m_rCurJumpTime;

				rNextHeight = getY( &m_TakeOffPos) - (ac * (m_rCurJumpHeight * ac));
			}
			else
			{
				rNextHeight = getY( &m_TakeOffPos) - (((1.0f - i3Math::cos( rFac)) * m_rCurJumpHeight));
			}
		}

		{	//이전 프레임에서 계산했던 값이 GetPos()에는 현재시점에서 반영되지 못하는 경우가 종종 있으며
			//현재 getPosition( &newPos)에서도 이전vel값에의한 이동치가 반영되었다고 장담할수는 없습니다.
			VEC3D newPos;
			m_pController->getPosition( &newPos);

			m_Vel.y = rNextHeight - getY( &newPos);

			//if( !isNetwork())
			//{
			//	I3TRACE("vel: %f, curY:%f, nexthY: %f, hitmask: %08X\n", m_Vel.y, getY(&newPos), rNextHeight, m_hitMask);
			//}
		}
	}
}

bool CGameCharaPlayer::CalculateLadderEnable( void * pHit)
{
	NxControllerShapeHit * pHitData = (NxControllerShapeHit*)pHit;
	bool bClimbEnable = false;

	//충돌 Shape
	NxU32 nClosest = pHitData->dir.closestAxis();
	if( nClosest != 1 )//Y축이면.
	{
		bClimbEnable = true;
	}

	setClimbingLadderEnable( bClimbEnable);
	
	if( bClimbEnable)
	{
		setLadderContactPosition( (REAL32)pHitData->worldPos.x, (REAL32)pHitData->worldPos.y, (REAL32)pHitData->worldPos.z);
	}

	return bClimbEnable;
}

void CGameCharaPlayer::EnableSpeedReduction( UINT8 nDamage)
{
    m_rElapsedSpeedReduction = g_pCharaInfoDataBase->getGlobal()->m_rSpeedReductionTime;

	REAL32 rMaxReduce = g_pCharaInfoDataBase->getGlobal()->m_rSpeedReductionMaxRate * 0.01f;

	REAL32 nReductionRate = 1.0f - ( MINMAX( 0.0f , (rMaxReduce * ((REAL32)nDamage / 100.0f)), rMaxReduce));
	if( m_rElapsedSpeedReduction > 0.0f)
	{
		m_rSpeedReductionRate = ( m_rSpeedReductionRate > nReductionRate ) ? m_rSpeedReductionRate : nReductionRate;
	}
	else
	{
		m_rSpeedReductionRate = nReductionRate;
	}

	{
		g_pCamera->SetTremble( 2.0f, 2.0f, 0.0f, 0.1f);
	}
}


void CGameCharaPlayer::UpdateSpeedReduction(REAL32 rDeltaSeconds)
{
	m_rElapsedSpeedReduction -= rDeltaSeconds;
}

*/


