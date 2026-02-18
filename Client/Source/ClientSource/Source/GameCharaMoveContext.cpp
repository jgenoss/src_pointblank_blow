#include "pch.h"
#include "GlobalVariables.h"
#include "GameObject.h"
#include "GameCharaContexts.h"

#include "./Character/GameCharaMoveControl.h"
#include "PBLog.h"
#include "GlobalFunc.h"
#include "StageBattle.h"
#include "GameWorld.h"
#include "TutorialMode.h"
#include "HackDefense.h"
#include "GameMode.h"
#include "StageBattle/AIModeContext.h"
#include "BattleServerContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "GameMissionManager.h"
#include "Weapon/WeaponItemID.h"
#include "GameMaster.h"
#include "BattleObjContext.h"

#include "i3Base/string/ext/contain_string_head.h"

#define	RADIUS_NETWORK	0.45f
#define	TALL_NETWORK	1.5f//0.78f

#define CHARASKINWIDTH 0.085f


I3_CLASS_INSTANCE( PlayerController);

PlayerController::~PlayerController(void)
{
	I3_SAFE_RELEASE( m_pOwner);
}

void PlayerController::FlushHitShape( void)
{
	for( INT32 i = 0; i < getHitShapeCount(); i++)
	{
		CHitInfo * pHitInfo = GetHitInfo( i);

		if( pHitInfo->GetHitShape() != nullptr)
		{
			i3PhysixShapeSet * pShapeSet = reinterpret_cast<i3PhysixShapeSet*>(pHitInfo->GetHitShape()->userData);
			I3ASSERT( pShapeSet != nullptr);
			pShapeSet->setReleaseCallback( nullptr, nullptr);
		}
	}
}

void PlayerController::setOwner( CGameCharaBase * pPlayer)
{	
	I3_REF_CHANGE( m_pOwner, pPlayer); 
	setEventReceiver( pPlayer);
}

void PlayerController::releaseShape( i3ElementBase * pReceiver, NxShape * pShape)
{
	I3ASSERT( i3::same_of<PlayerController*>(pReceiver));
	PlayerController * pCtrl = static_cast<PlayerController*>(pReceiver);
	// shapeset이 해제되는 경우 버퍼에 있는 것을 삭제해줘야 한다.
	for( INT32 i = 0; i < pCtrl->getHitShapeCount(); i++)
	{
		CHitInfo * pHitInfo = pCtrl->GetHitInfo( i);
		if( pHitInfo->GetHitShape() == pShape)
			pHitInfo->SetHitInfo( nullptr, nullptr);
	}
}

//#define		CHECK_SM_TRESPASS

/*virtual*/ NxControllerAction PlayerController::OnShapeHit( const NxControllerShapeHit * pHit)
{
	#if defined( CHECK_SM_TRESPASS)
	// 이진균
	// SM Collision을 뚫고 들어간 경우를 검사하기 위해,
	// 교점이 발생했을 때, Capsule Controller와의 거리를 확인.
	{
		NxController * pCtrl = pHit->controller;
		REAL32 distViolate;
		VEC3D diff;
		INT32 type;

		if( pCtrl->getType() == NX_CONTROLLER_CAPSULE)
		{
			NxCapsuleController * pCapsule = (NxCapsuleController *) pCtrl;
			VEC3D vmin, vmax, hitPos;
			NxExtendedVec3 pos;

			pos = pCapsule->getPosition();

			vmin.x = vmax.x = (REAL32) pos.x;
			vmin.z = vmax.z = (REAL32) pos.z;
			vmin.y = (REAL32) pos.y - (pCapsule->getHeight() * 0.5f);
			vmax.y = (REAL32) pos.y + (pCapsule->getHeight() * 0.5f);

			hitPos.x = (REAL32) pHit->worldPos.x;
			hitPos.y = (REAL32) pHit->worldPos.y;
			hitPos.z = (REAL32) pHit->worldPos.z;

			// 교점이 vmin.y보다 작거나, vmax보다 크다면 Capsule의 위아래 Sphere 중점과의 거리로 비교
			if( pHit->worldPos.y > vmax.y)
			{
				// 상단 Sphere
				i3Vector::Sub( &diff, &vmax, &hitPos);
				type = 0;
			}
			else if( pHit->worldPos.y < vmin.y)
			{
				// 하단 Sphere
				i3Vector::Sub( &diff, &vmin, &hitPos);
				type = 1;
			}
			else
			{
				REAL32 t;

				// 몸통에 Hit 된 것이기 때문에 각 중점을 잊는 직선과의 거리로 비교
				i3Math::GetPerpendicularPoint( &vmin, &vmax, &hitPos, &diff, &t);

				i3Vector::Sub( &diff, &diff, &hitPos);

				type = 2;
			}

			distViolate = pCapsule->getRadius() - i3Vector::Length( &diff);

			if( i3Math::abs( distViolate) > pCapsule->getRadius())
			{
				I3TRACE( "VIOLATE(%d) : %f\n", type, distViolate);
			}
		}
	}
	#endif

	// 오브젝트와 컬리젼 되면 오브젝트의 위치가 변했는지 확인합니다.
	if( pHit->shape != nullptr && pHit->shape->userData != nullptr)
	{
		i3ElementBase * pOwner = (i3ElementBase *) pHit->shape->userData;

		if( !i3::same_of<i3PhysixShapeSet*>(pOwner))
			return NX_ACTION_NONE;

		((i3PhysixShapeSet *)pOwner)->setReleaseCallback( this, releaseShape);
		
		m_HitNormal.x = pHit->worldNormal.x; m_HitNormal.y = pHit->worldNormal.y; m_HitNormal.z = pHit->worldNormal.z;

		{
			m_HitDir.x = pHit->dir.x; m_HitDir.y = pHit->dir.y; m_HitDir.z = pHit->dir.z;

			//m_rLength = pHit->length;
		}

		if( m_nHitShapeCount < CONTROLLER_HIT_INFO_COUNT)
		{
			m_HitInfo[m_nHitShapeCount].SetHitInfo(pHit->shape, &m_HitNormal);
			m_nHitShapeCount++;
		}
		else
		{
			I3PRINTLOG(I3LOG_WARN, "m_nHitShapeCount >= CONTROLLER_HIT_INFO_COUNT Group %d id %d", pHit->shape->getGroup(), pHit->id);
		}

		m_HitPos.x = (REAL32)pHit->worldPos.x; m_HitPos.y = (REAL32)pHit->worldPos.y; m_HitPos.z = (REAL32)pHit->worldPos.z;

		if( m_nHitWorldPosCount < CONTROLLER_WORLD_HIT_POS_COUNT)
		{
			m_HitWorldPos[m_nHitWorldPosCount].SetWorldHitInfo(&m_HitPos, &m_HitNormal);
			m_nHitWorldPosCount++;
		}
		else
		{
			m_nHitWorldPosCount = 0;
			InitWorldHitInfoArray();
		}
	}

	return NX_ACTION_NONE;
}

/*virtual*/	NxControllerAction PlayerController::OnControllerHit( const NxControllersHit * pHit)
{
	// 컨트롤러 같이 히트가 되면 밀어내야합니다.
	// 충돌 처리 후 위치를 다시 계산할 필요가 있습니다..
	/*
	if( pHit->controller != nullptr )
	{
		NxExtendedVec3 pos;

		pos = pHit->controller->getPosition();

		VEC3D vecOther;
		VEC3D vDir;
		VEC3D * pVecMyPos = g_pPlayer->m_pOwner->GetPos();

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
	*/
	//I3TRACE( "CONTROLLER HIT\n");

	m_pHitUser = nullptr;

	if( ( getOwner()->isLocal() || getOwner()->isAI()) &&
		pHit->controller != nullptr)
	{
		if( i3::same_of<PlayerController*>((i3EventReceiver*)pHit->controller->getUserData()))
		{
			PlayerController * pController = (PlayerController*)pHit->controller->getUserData();
			if( getOwner() != pController->getOwner() )
				m_pHitUser = pController->getOwner();
		}
	}

	return NX_ACTION_NONE;
}


I3_CLASS_INSTANCE( CGameCharaMoveContext);

CGameCharaMoveContext::CGameCharaMoveContext() 
{
	addMoveFlag( CHARA_MOVE_FLAG_TOUCHDOWN); //EscapeMission 연속된 충돌체크로 인한 스코어 중복체크를 막기 위해 설정합니다.

}

CGameCharaMoveContext::~CGameCharaMoveContext()
{
	OnDestroy();
}

INT32 CGameCharaMoveContext::getCharaNetIndex( void)
{
	if( m_pOwner)
		return m_pOwner->getCharaNetIndex();

	return -1;
}

void CGameCharaMoveContext::OnCreate( CGameCharaBase * pOwner, INT32 defaultView, INT32 defaultNetwork)
{
	m_pCurrentMoveControl = nullptr;
	m_pMoveControl = CGameCharaMoveControl::new_object();
	m_pMoveControl->BindOwner( pOwner);

	CGameCharaContextBase::OnCreate( pOwner, defaultView, defaultNetwork);
}

void CGameCharaMoveContext::OnDestroy( void)
{
	CGameCharaContextBase::OnDestroy();

	ReleaseController();

	m_pCurrentMoveControl = nullptr;
	I3_SAFE_RELEASE( m_pMoveControl );

	I3_SAFE_DELETE(m_Accel);
	I3_SAFE_DELETE(m_VelCharacter);
	I3_SAFE_DELETE(m_OldVel);
	I3_SAFE_DELETE(m_TakeOffPos);				// Jump start position
	I3_SAFE_DELETE(m_RideLocalPos);
	I3_SAFE_DELETE(m_RideSyncPos);
	I3_SAFE_DELETE(m_vOldPos);
	I3_SAFE_DELETE(m_vPostPosition);
	I3_SAFE_DELETE(m_vRecvPosition);
	I3_SAFE_DELETE(m_matRideObjectPose);
}

void CGameCharaMoveContext::OnReset( void)
{
	CGameCharaContextBase::OnReset();

	i3Vector::Zero( &m_MoveDir);
	i3Vector::Copy( &m_MoveAt, i3Matrix::GetAt( m_pOwner->getSceneObject()->GetMatrix()));
	i3Vector::Copy( &m_MoveRight, i3Matrix::GetRight( m_pOwner->getSceneObject()->GetMatrix()));
	i3Vector::Zero( m_Accel);
	i3Vector::Zero( m_VelCharacter);
	i3Vector::Zero( m_OldVel );

	m_rVelocity			= 0.f;
	m_tmStopingElapsed  = 0.f;
	m_tmAccelElapsed	= 0.f;
	m_rMaxMoveSpeed		= 0.f;

	m_rElapsedSpeedReduction = 0.f;
	m_rSpeedReductionRate = 0.f;
	m_bSpeedReductionEternal = false;

	i3Vector::Zero( m_TakeOffPos);			// Jump start position
	i3Vector::Zero( &m_ForceJumpDir);			// Jump start speed
	i3Vector::Zero(&m_ForceJumpCharaVel);			// Jump start speed
	m_rForceJumpSpeed = 0.f;
	m_rFallOldPosY = 0.f;
	m_rJumpUpOldTime = 0.f;
	m_rElapsedTakeOffTime	= 0.f;

	if( m_pOwner->getDBInfo() != nullptr)
	{
		m_rCurJumpHeight		= m_pOwner->getDBInfo()->GetJumpHeight();
		m_rCurJumpTime			= m_pOwner->getDBInfo()->GetJumpTime() * 0.5f;
		if( m_rCurJumpTime > 0.f)
			m_rHalfAccelOfGravity	= m_rCurJumpHeight / (m_rCurJumpTime * m_rCurJumpTime);
		else
			m_rHalfAccelOfGravity = 0.f;
	}
	else
	{
		m_rCurJumpHeight = 0.f;
		m_rCurJumpTime = 0.f;
		m_rHalfAccelOfGravity = 0.f;
	}

	m_CurFloorTerrainType	= I3_TERRAIN_NA;
	m_rFloorCheckTimer		= 0.f;

	m_rFloorLength			= 0.f;

	m_pRideObject			= nullptr;
	m_pOldRideObject		= nullptr;
	
	m_pPrevJumpHold			= nullptr;

	m_iRideObjectIndex		= -1;
	i3Matrix::Identity( m_matRideObjectPose);
	//g_pLog->Log(g_pFramework->GetAccTime(), "CGameCharaMoveContext::OnReset()\n");

	ResetController();

	m_rDamageTime = 0.f;
	i3Vector::Zero(&m_v3ContactObjectVelToChara);

	m_bGravityRecovery = false;
}

void CGameCharaMoveContext::OnChangeLocal()
{
	CGameCharaContextBase::OnChangeLocal();

	m_pCurrentMoveControl = m_pMoveControl;
}

void CGameCharaMoveContext::OnChangeNetwork()
{
	CGameCharaContextBase::OnChangeNetwork();

	m_pCurrentMoveControl = m_pMoveControl;
}

void CGameCharaMoveContext::OnChangeNPC( void)
{
	CGameCharaContextBase::OnChangeNPC();
}

void CGameCharaMoveContext::OnChangeAI( void)
{
	m_pCurrentMoveControl = m_pMoveControl;
	
	if( IsNetworkController() )
	{
		ReleaseController();	// 네트워크용 Control에서 AI용으로 변경
		CreateController();
	}
}

void CGameCharaMoveContext::OnChangeFullbody( void)
{
	CGameCharaContextBase::OnChangeFullbody();

	m_pCurrentMoveControl = m_pMoveControl;
}

void CGameCharaMoveContext::OnUpdate( REAL32 rDeltatime)
{
	CGameCharaContextBase::OnUpdate( rDeltatime);

	if( m_pCurrentMoveControl == nullptr)
		return;

	m_pCurrentMoveControl->OnUpdate( rDeltatime);

	//if( m_pController != nullptr)
	//	((PlayerController*)m_pController)->FlushHitShape();
}


//NxCCDSkeleton* CreateCCDSkeleton(REAL32 rSize, REAL32 rHeight)
//{
//	NxU32 triangles[3 * 12] = { 
//		0,1,3,
//		0,3,2,
//		3,7,6,
//		3,6,2,
//		1,5,7,
//		1,7,3,
//		4,6,7,
//		4,7,5,
//		1,0,4,
//		5,1,4,
//		4,0,2,
//		4,2,6
//	};
//
//	NxVec3 points[8];
//
//	// Static mesh
//	points[0].set( size, -size, -size);
//	points[1].set( size, -size,  size);
//	points[2].set( size,  size, -size);
//	points[3].set( size,  size,  size);
//
//	points[4].set(-size, -size, -size);
//	points[5].set(-size, -size,  size);
//	points[6].set(-size,  size, -size);
//	points[7].set(-size,  size,  size);
//
//	NxSimpleTriangleMesh stm;
//	stm.numVertices = 8;
//	stm.numTriangles = 6*2;
//	stm.pointStrideBytes = sizeof(NxVec3);
//	stm.triangleStrideBytes = sizeof(NxU32)*3;
//
//	stm.points = points;
//	stm.triangles = triangles;
//	stm.flags |= NX_MF_FLIPNORMALS;
//	return gPhysicsSDK->createCCDSkeleton(stm);
//}
//
//NxActor* CGameCharaMoveContext::CreateCCDBox(const NxVec3& pos, const NxVec3& boxDim, const NxReal density, bool doDynamicCCD)
//{
//	// Add a single-shape actor to the scene
//	NxActorDesc actorDesc;
//	NxBodyDesc bodyDesc;
//
//	// The actor has one shape, a box
//	NxBoxShapeDesc boxDesc;
//	boxDesc.dimensions.set(boxDim.x,boxDim.y,boxDim.z);
//	boxDesc.localPose.t = NxVec3(0,boxDim.y,0);
//
//	// Set box up with CCD skeleton
//	boxDesc.ccdSkeleton = CreateCCDSkeleton(boxDim.x*0.8f);
//	if (doDynamicCCD)  boxDesc.shapeFlags |= NX_SF_DYNAMIC_DYNAMIC_CCD;  // Activate dynamic-dynamic CCD for this body
//
//	actorDesc.shapes.pushBack(&boxDesc);
//
//	if (density)
//	{
//		actorDesc.body = &bodyDesc;
//		actorDesc.density = density;
//	}
//	else
//	{
//		actorDesc.body = nullptr;
//	}
//	actorDesc.globalPose.t = pos;
//	return gScene->createActor(actorDesc);	
//}


//
void CGameCharaMoveContext::CreateCCD(VEC3D* pv3Pos, VEC3D* pv3Size, int iCGRP)
{
	/*m_pCCD_Box = i3PhysixShapeSet::new_object();
	m_pCCD_Box->CreateBoxShapeCCD( pv3Pos, pv3Size, iCGRP, true);
	m_pCCD_Box->getNxActor()->setGroup( iCGRP);
	m_pCCD_Box->SetName( "test" );
	m_pCCD_Box->getNxShape(0)->setName( "test22");*/
}

bool CGameCharaMoveContext::IsNetworkController( void)
{
	if( m_pController != nullptr)
	{
		NxCapsuleController * pCtrl = (NxCapsuleController*) m_pController->getNxController();
		REAL32 rad = pCtrl->getRadius() - RADIUS_NETWORK;
		if( rad < 0.001f && rad > -0.001f )
		{
			return true;
		}
	}

	return false;
}

void CGameCharaMoveContext::CreateController()
{
	NxCapsuleControllerDesc capsuledesc;

	capsuledesc.setToDefault();

	capsuledesc.stepOffset = 0.27f;	// 턱의 높이가 40센티 이상이면 넘지 못합니다.
	capsuledesc.slopeLimit = i3Math::cos( I3_DEG2RAD( 45));	// 경사각이 45이상이면 이동하지 못하도록 합니다.
	
	if( m_pOwner->IsDino())
	{
		capsuledesc.radius = m_pOwner->getDBInfo()->GetDinoCCRadius();
		capsuledesc.height = max( 0.1f, m_pOwner->getDBInfo()->GetDinoCCTall());
		capsuledesc.stepOffset = m_pOwner->getDBInfo()->GetDinoCCStepOffset();
		capsuledesc.position.set( 0.0f, 0.0f, 0.0f);
		capsuledesc.skinWidth = CHARASKINWIDTH;
	}
	else if(m_pOwner->isLocal()  || m_pOwner->isAI())
	{
		capsuledesc.radius = RADIUS;
		capsuledesc.height = max( 0.1f, CAPSULE_FULL_HEIGHT);
		capsuledesc.skinWidth = CHARASKINWIDTH;
		capsuledesc.stepOffset = 0.4f;	// 턱의 높이가 40센티 이상이면 넘지 못합니다.
		capsuledesc.position.set( 0.0f, 0.0f, 0.0f);
	}
	else if( m_pOwner->isNetwork() )
	{
		capsuledesc.radius = RADIUS_NETWORK;
		capsuledesc.height = max( 0.1f, TALL_NETWORK);
		capsuledesc.skinWidth = 0.4f;
		capsuledesc.position.set( 0.0f, 0.3f, 0.0f);
	}
	else
	{
		capsuledesc.radius = 0.7f;
		capsuledesc.height = 1.8f;
		capsuledesc.skinWidth = 0.4f;
		capsuledesc.position.set( 0.0f, 0.3f, 0.f);
	}

	if( m_pOwner->isAI())
	{
		capsuledesc.stepOffset = 0.25f;						// ai는 턱걸림 판정을 더 엄격하게하여 떨림을 방지합니다.
	}

	I3ASSERT( capsuledesc.isValid());

	m_pController = PlayerController::new_object();	//g_pCharaManager->AllocPhysXController( m_pOwner);
	I3ASSERT( m_pController != nullptr);

	if(m_pOwner->isLocal()  || m_pOwner->isAI())
	{
		if( m_pOwner->IsRex() == false && m_pOwner->IsDomiMutantRex() == false )
		{
			m_pController->Create( CGRP_MYCHARA_BOUND, (NxControllerDesc*)&capsuledesc);
		}
		else
		{
			m_pController->Create( CGRP_TREX_BOUND, (NxControllerDesc*)&capsuledesc);
		}
	}
	else
	{
		if( m_pOwner->IsRex()  || m_pOwner->IsDomiMutantRex())
		{
			m_pController->Create( CGRP_TREX_BOUND, (NxControllerDesc*)&capsuledesc);
		}
		else
		{
			m_pController->Create( CGRP_CHARA_BOUND, (NxControllerDesc*)&capsuledesc);
		}
	}

	((PlayerController*)(i3PhysixController*)m_pController)->setOwner( m_pOwner);

	NxActor * pAct = m_pController->getActor();
	if( pAct == nullptr )
	{
		I3PRINTLOG(I3LOG_WARN,  "Actor is nullptr." );
		return;
	}
	
	if(m_pOwner->isLocal() || m_pOwner->isAI())
	{
		m_pController->MakeCapsuleTrigger();
	}

	pAct->raiseBodyFlag( NX_BF_DISABLE_GRAVITY );
}

void CGameCharaMoveContext::ReleaseController()
{
	if( m_pController != nullptr)
	{
		((PlayerController*)(i3PhysixController*)m_pController)->setOwner(nullptr);	// CharaBase를 해제
	}
	I3_SAFE_RELEASE( m_pController);
}

void CGameCharaMoveContext::ResetController()
{
	if( m_pController == nullptr )
		return;

	((PlayerController*)(i3PhysixController*)m_pController)->setHitShapeCount( 0);
	((PlayerController*)(i3PhysixController*)m_pController)->SetHitWorldShapeCount(0);

	NxActor * pAct = m_pController->getActor();
	if( pAct == nullptr )
	{
		I3PRINTLOG(I3LOG_FATAL,  "Actor is nullptr." );
		return;
	}

	pAct->clearBodyFlag( NX_BF_DISABLE_GRAVITY );
}

void CGameCharaMoveContext::EnableController( bool bVal)
{
#ifdef DOMI_OPTIMIZE
	if (m_pController == nullptr) return;
#endif

	if( bVal )
	{
		if( m_pOwner->IsRex() == false &&  m_pOwner->IsDomiMutantRex() == false)
		{
			if(m_pOwner->isLocal()  || m_pOwner->isAI())
				m_pController->setGroup( CGRP_MYCHARA_BOUND );
			else
				m_pController->setGroup( CGRP_CHARA_BOUND );
		}
		else
		{
			m_pController->setGroup( CGRP_TREX_BOUND );
		}
	}
	else
		m_pController->setGroup( CGRP_DUMMY );
}


void CGameCharaMoveContext::ChangeControllerHeight( REAL32 rHeight )
{	
	I3ASSERT( m_pController != nullptr );

	REAL32 rTall = (rHeight * 0.5f) + RADIUS;

	if( m_pController->getHeight() == rTall )
		return;

	m_pController->SetCenterHeightAndHeight((rHeight * 0.5f) + RADIUS, rHeight);
}

void CGameCharaMoveContext::CheckWorldCollision( REAL32 rDeltatime)
{	
	CStageBattle* pStage = (CStageBattle*)g_pFramework->GetCurrentStage();

	if( m_pController == nullptr )
	{
		I3PRINTLOG(I3LOG_WARN,  "m_pController is nullptr." );
		return;
	}

	REAL32	y		= m_VelCharacter->y;
	m_VelCharacter->y			= 0.0f;

	bool force_process_disable = false;
	if (CHARAMOVE::isCurrentStage_Same_MMT(MMT_SPACE) && m_pOwner->isCharaStateMask(CHARA_STATE_JUMP))
		force_process_disable = true;
		
	if ( i3Vector::LengthSq(m_Accel) > I3_EPS )
	{
		//속도 증가 프로세스들 처리 부분.
		VEC3D accl, vVel, vAccel;
		REAL32 rVel, rDrive, delta_offset_increas, maxSpeed, fSpeed;

		maxSpeed = delta_offset_increas = rDrive = rVel = fSpeed = 0.0f;
		i3Vector::Normalize(&vAccel, m_Accel);

		if( !m_pOwner->isCharaStateMask(CHARA_STATE_FORCEJUMP) )
		{
			//보통 점프 이동, 보통 이동 관련 프로세스.

			rVel = i3Vector::Length(m_VelCharacter);
			if (rVel > I3_EPS)	i3Vector::Normalize(&vVel, m_VelCharacter);
			else				i3Vector::Copy(&vVel, m_VelCharacter);

			rDrive = i3Vector::Dot(&vVel, &vAccel);
			delta_offset_increas = rDeltatime;

			if (pStage != nullptr && CHARAMOVE::isCurrentStage_Same_MMT(MMT_SPACE))
				delta_offset_increas *= pStage->GetStageAddAcceleration_Up_Speed_late();

			if (rVel > I3_EPS)//이동 하던 중이었다면
			{
				m_tmAccelElapsed += delta_offset_increas * (rDrive - m_pOwner->getDBInfo()->GetAccelRateBackVsFront());		//현재 GetAccelRateBackVsFront 이거는 작동 안하는걸 확인했으나, 일단 남겨 둠.
				if (rDrive < 0.0f) m_tmAccelElapsed -= delta_offset_increas;
			}
			else
			{
				m_tmAccelElapsed += delta_offset_increas;	//속도가 매우 작은경우와 가속도가 매우 작은경우를 위해 += 로 사용.
			}

			m_tmAccelElapsed = MINMAX(0.0f, m_tmAccelElapsed, m_pOwner->GetAccelerationTimeAddOffset());

			i3Vector::Scale(&accl, m_Accel, delta_offset_increas);

			if (m_pOwner->isCharaStateMask(CHARA_STATE_CLIMB))
				i3Vector::Copy(m_VelCharacter, &accl);
			else
				i3Vector::Add(m_VelCharacter, m_VelCharacter, &accl);

			i3Vector::Normalize(m_VelCharacter, m_VelCharacter);
			maxSpeed = m_rMaxMoveSpeed;
			if (rVel > I3_EPS && rDrive <= 0.0f && m_rOldMaxMoveSpeed > 0.0f)
				maxSpeed = MIN((REAL32)m_rOldMaxMoveSpeed, (REAL32)m_rMaxMoveSpeed);

			if (isEnabledSpeedReduction())
				maxSpeed *= m_rSpeedReductionRate;

			fSpeed = maxSpeed * i3Math::sin((m_tmAccelElapsed / m_pOwner->GetAccelerationTimeAddOffset()) * I3_PI2);
			i3Vector::Scale(m_VelCharacter, m_VelCharacter, fSpeed * delta_offset_increas);
		}
		else
		{
			//강제 점프 이동 프로세스. 
			//m_ForceJumpCharaVel로 계산하고 저장해서 추가적으로 강제 속도 더해주는 방향으로 구현.
			rVel = i3Vector::Length(&m_ForceJumpCharaVel);
			if (rVel > I3_EPS) i3Vector::Normalize(&vVel, &m_ForceJumpCharaVel);
			else			   i3Vector::Copy(&vVel, &m_ForceJumpCharaVel);

			rDrive = i3Vector::Dot(&vVel, &vAccel);

			if (rVel > I3_EPS)//이동 하던 중이었다면
			{
				m_tmAccelElapsed += rDeltatime * rDrive * 0.5f;
				if (rDrive < 0.0f) m_tmAccelElapsed -= rDeltatime; //둔각
			}
			else
			{
				m_tmAccelElapsed += rDeltatime;	//속도가 매우 작은경우와 가속도가 매우 작은경우를 위해 += 로 사용.
			}

			m_tmAccelElapsed = MINMAX(0.0f, m_tmAccelElapsed, 1.0f);	//일단은 보통 이동 때 쓰는 값으로 사용.
			i3Vector::Scale(&accl, m_Accel, rDeltatime);
			i3Vector::Add(&m_ForceJumpCharaVel, &m_ForceJumpCharaVel, &accl);

			i3Vector::Normalize(&m_ForceJumpCharaVel, &m_ForceJumpCharaVel);
			maxSpeed = m_rMaxMoveSpeed;
			if (rVel > I3_EPS && rDrive <= 0.0f && m_rOldMaxMoveSpeed > 0.0f)
				maxSpeed = MIN((REAL32)m_rOldMaxMoveSpeed, (REAL32)m_rMaxMoveSpeed);
			
			fSpeed = maxSpeed * i3Math::sin((m_tmAccelElapsed / 1.0f) * I3_PI2);			//프레임에 따른 점프 이동 속도 계산 부분.
			i3Vector::Scale(&m_ForceJumpCharaVel, &m_ForceJumpCharaVel, fSpeed * rDeltatime);
			
			REAL32 rFactor = rDeltatime * 60.0f;
			VEC3D force_speed;
			i3Vector::Scale(&force_speed, &m_ForceJumpDir, (m_rForceJumpSpeed * 0.017f * I3_PI2) * rFactor);

			i3Vector::Add( m_VelCharacter, &force_speed, &m_ForceJumpCharaVel);	//실제 캐릭터 속도에 적용 부분.
		}
	}
	else
	{
		if (!force_process_disable)
		{
			//속도 감소 프로세스들 처리 부분.
			REAL32 delta_offset_decreas = rDeltatime;

			if (pStage != nullptr && CHARAMOVE::isCurrentStage_Same_MMT(MMT_SPACE))
				delta_offset_decreas *= pStage->GetStageAddAcceleration_Down_Speed_late();

			m_tmAccelElapsed -= delta_offset_decreas;
			if (m_pOwner->IsRex()) m_tmAccelElapsed -= delta_offset_decreas;

			if (m_tmAccelElapsed > 0.0f)
			{
				i3Vector::Scale(m_VelCharacter, m_VelCharacter, i3Math::sin((m_tmAccelElapsed / m_pOwner->GetAccelerationTimeAddOffset()) * I3_PI2));

				REAL32 len = i3Vector::Length(m_VelCharacter);
				if (len > I3_EPS)
				{
					i3Vector::Normalize(m_VelCharacter, m_VelCharacter);
					//기존 이동 속도 벨런싱 때문에 이렇게 예외처리로 남겨둡니다. 민감해서 안건드리겠습니다.

					if (CHARAMOVE::isCurrentStage_Same_MMT(MMT_SPACE) || pStage->GetStageAddAcceleration_Down_Speed_late() == 1.0f)
						len = MIN(len, m_rMaxMoveSpeed * rDeltatime);

					i3Vector::Scale(m_VelCharacter, m_VelCharacter, len);
				}

				if (len < 0.001)
					m_tmAccelElapsed = 0.0f;
			}
			else
			{
				i3Vector::Zero(m_VelCharacter);
				m_tmAccelElapsed = 0.0f;
			}
		}
	}

	if (m_pOwner->isCharaStateMask(CHARA_STATE_CLIMB) == false )
	{
		m_VelCharacter->y = y;
	}
	else
	if ( IsJumpingOnLadder(m_ladder_info) )
	{
		m_VelCharacter->y = y;
	}
}

void CGameCharaMoveContext::UpdateTouchDown(REAL32 rDeltatime)
{
//EscapeMission 터치다운에 딜레이를 주어 연속적인 득점을 방지함
	if(GetTouchdown()) //터치다운을 할 수 있는 상태이면 리턴 시킨다.
		return;
	
	m_fDelayTime = m_fDelayTime - rDeltatime;

	if( m_fDelayTime <= 0.0f)
	{
		SetTouchdown( true);
		SetDeleyTime( 10.0f);
	}

}
void CGameCharaMoveContext::ResetJumping(void)
{
	m_pController->getPosition( m_TakeOffPos);
	i3Vector::Zero(&m_ForceJumpDir);			// Jump start speed
	i3Vector::Zero(&m_ForceJumpCharaVel);		// Jump start speed

	m_rForceJumpSpeed		= 0.f;
	m_rFallOldPosY			= 0.f;
	m_rJumpUpOldTime		= 0.f;
	m_rElapsedTakeOffTime	= 0.f;

	m_pOwner->removeCharaStateMask( CHARA_STATE_JUMP);
	m_pOwner->removeCharaStateMask(	CHARA_STATE_JUMPING_UP);
	m_pOwner->removeCharaStateMask(	CHARA_STATE_JUMPING_DOWN);
	m_pOwner->removeCharaStateMask( CHARA_STATE_FORCEJUMP);
	
	if (m_pOwner->isLocal() && (m_pOwner->getCharaNetIndex() == BattleSlotContext::i()->GetMySlotIdxEx()))
		m_pPrevJumpHold = nullptr;
}

void CGameCharaMoveContext::UpdateFallDamage( REAL32 rDeltatime)
{
	//	낙하 데미지 계산
	if(EnableKill() && (m_rElapsedTakeOffTime > rDeltatime))
	{
		if( m_rElapsedTakeOffTime > 0.1f)
		{
			INT32 nDamage = _CalcFallingDamage( m_TakeOffPos);

			if( m_pOwner != nullptr )
			{
				if(m_pOwner->isAI() && MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS))
					nDamage = 0;

				// 이 위치에서 Flag를 켜주자.
				if (MyRoomInfoContext::i()->getStage()->IsSpace() && m_bGravityRecovery == false)
					nDamage = 0;
			}

			if( nDamage > 0)
				m_pOwner->Cmd_DamageSelf( nDamage, 0xFFFF, true);
		}
	}

	if( m_rElapsedTakeOffTime > rDeltatime)
	{
		getSoundContext()->_PlayJumpEndSound();
	}

	ResetJumping();
	
	//I3TRACE( "in Floor..%.4f\n", g_pViewer->getDeltaTime());
}

REAL32 CGameCharaMoveContext::CalrFallVel(VEC3D& newPos, REAL32 rFac, REAL32 rDeltatime)
{
	REAL32 rLen = 0.f;
	REAL32 rUp = 0.f;

	if (m_pOwner->isEnableStateMask(CHARA_STATE_DISABLE_GRAVITY) )		// 일단 사다리 트리거 내부에서는 중력작동을 제거하도록 처리...(2014.12.04.수빈)
	{
		rLen = 0;
		rUp = 0.f;
	}
	else
	{
		rLen = m_rHalfAccelOfGravity * m_rElapsedTakeOffTime * m_rElapsedTakeOffTime;
		rUp = m_rFallOldPosY - rLen;
	}

	if(rUp > 0) 
	{
		I3ASSERT_0;
	}

	m_rFallOldPosY = rLen;
	
	return rUp;
}

void CGameCharaMoveContext::UpdateJumping(REAL32 rDeltatime)
{
	I3ASSERT( m_pOwner->getDBInfo() != nullptr);

	REAL32 rNextHeight = 0.0f;
	REAL32 rFac = 0.f;
	VEC3D  newPos;
	m_pController->getPosition(&newPos);
	REAL32 rUp = 0.0f;

	if ( !(MyRoomInfoContext::i()->getStage()->IsSpace()) )
	{
		if (m_pOwner->isAI() && MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_DIEHARD))
		{
			m_rCurJumpHeight = g_pTempConfig->m_JumpHeight;
			m_rCurJumpTime = g_pTempConfig->m_JumpTime * 0.5f;
			m_rHalfAccelOfGravity = m_rCurJumpHeight / (m_rCurJumpTime * m_rCurJumpTime);
		}
		else if (m_pOwner->isAI() && MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS))
		{
			INT32 charaNetIndex = getCharaNetIndex();
			if (charaNetIndex < 0 || charaNetIndex >= SLOT_MAX_COUNT)
			{
				I3_BOUNDCHK(charaNetIndex, SLOT_MAX_COUNT);
			}
			else
			{
				AI_ENTRY *		pAiEntry = &AIModeContext::i()->AIEntry[charaNetIndex];
				AI_BIND_DATA *  pBindData = g_pCharaManager->getAiBindData(pAiEntry->_number);

				m_rCurJumpHeight = pBindData->_Jump_Height;
				m_rCurJumpTime = pBindData->_Jump_Time * 0.5f;
				m_rHalfAccelOfGravity = m_rCurJumpHeight / (m_rCurJumpTime * m_rCurJumpTime);
			}
		}

		if (m_pOwner->isCharaStateMask(CHARA_STATE_JUMP) && (m_rElapsedTakeOffTime == 0.0f))
		{
			m_VelCharacter->y = i3Math::sin((rDeltatime / m_rCurJumpTime) * I3_PI2) * m_rCurJumpHeight;

			GetPosition(m_TakeOffPos);

			m_rElapsedTakeOffTime = rDeltatime;

			return;
		}

		if (m_pOwner->isCharaStateMask(CHARA_STATE_JUMP) && ((m_rElapsedTakeOffTime + rDeltatime) > m_rCurJumpTime))
		{
			ResetJumping();
		}
		else
		{
			if (m_pOwner->isCharaStateMask(CHARA_STATE_DEATH) == false)
			{
				m_rElapsedTakeOffTime = m_rElapsedTakeOffTime + rDeltatime;
			}
		}

		/////////////////////////////////////////////////////////////////////////////////
		// Process 낙하
		if (m_pOwner->isCharaStateMask(CHARA_STATE_FLOOR))
		{
			UpdateFallDamage(rDeltatime);

			setY(m_VelCharacter, -0.1f);
		}
		else
		{
			//이전 프레임에서 계산했던 값이 m_pOwner->GetPos()에는 현재시점에서 반영되지 못하는 경우가 종종 있으며
			//현재 getPosition( &newPos)에서도 이전vel값에의한 이동치가 반영되었다고 장담할수는 없습니다.
			rFac = (m_rElapsedTakeOffTime / m_rCurJumpTime) * I3_PI2;

			if (m_pOwner->isCharaStateMask(CHARA_STATE_JUMP))
			{
				rNextHeight = getY(m_TakeOffPos) + (i3Math::sin(rFac) * m_rCurJumpHeight);
				rUp = MAX(0.0f, rNextHeight - getY(&newPos));
			}
			else
			{
				rUp = CalrFallVel(newPos, rFac, rDeltatime);
			}

			setY(m_VelCharacter, rUp);

			if (EnableKill())
			{
				if (MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_OUTPOST)
				{
					// 맵에서 빠졌을 경우 죽임.
					if (m_pOwner->GetPos()->y < -170.f)
					{
						m_pOwner->Cmd_DamageSelf(255);
					}
				}
				else
				{
					// 맵에서 빠졌을 경우 죽임.
					if (m_pOwner->GetPos()->y < -30.f)
					{
						m_pOwner->Cmd_DamageSelf(255);
					}
				}
			}
		}
	}
	else
	{
		//뉴 점프 프로세스. 스페이스 모드 개발을 위하여 새로 만듭니다.
		//순서가 중요하여 순서 바꾸 실 때는 신중히 바꿔주세요.

		CStageBattle* pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
		if (pStage == nullptr) return;

		m_rHalfAccelOfGravity = pStage->GetStageGravity();
		REAL32 Chara_Up_Gravity_Speed_late = pStage->GetStage_Chara_Up_Gravity_Speed_late();
		REAL32 Chara_Down_Gravity_Speed_late = pStage->GetStage_Chara_Down_Gravity_Speed_late();
		REAL32 chara_fall_gravity_late = pStage->GetStage_Chara_Fall_Gravity_late();

		// 중력복구 아이템 적용 시에만 바꿔줍니다.
		if (m_bGravityRecovery)
		{
			m_rHalfAccelOfGravity = pStage->GetGrageGravityRestore();//m_rCurJumpHeight / (m_rCurJumpTime * m_rCurJumpTime);
			Chara_Up_Gravity_Speed_late = 1.0f;
			Chara_Down_Gravity_Speed_late = 1.0f;
			chara_fall_gravity_late = 1.0f;
		}

		{// 점프 시작 시퀀스. 이 조건에 맞으면 밑에 루틴을 전부 통과하고 다음 프레임부터 시작.
			if (m_pOwner->isCharaStateMask(CHARA_STATE_JUMP) && (m_rElapsedTakeOffTime == 0.0f))
			{
				REAL32 rFactor = rDeltatime * 60.0f;

				if (m_pOwner->isCharaStateMask(CHARA_STATE_FORCEJUMP))
				{
					//프레임에 따른 강제 점프 이동 속도 계산 부분.
					VEC3D force_speed;
					i3Vector::Scale(&force_speed, &m_ForceJumpDir, (m_rForceJumpSpeed * 0.017f * I3_PI2) * rFactor);
					i3Vector::Copy(m_VelCharacter, &force_speed);
				}

				m_VelCharacter->y = (m_rCurJumpHeight * 0.017f * I3_PI2) * rFactor;
				m_rElapsedTakeOffTime = rDeltatime;
				GetPosition(m_TakeOffPos);
				m_pOwner->addCharaStateMask(CHARA_STATE_JUMPING_UP);
					
				return;
			}
		}

		{// 점프 종료 검사 및 업데이트 시간 갱신 부분.
			if (m_pOwner->isCharaStateMask(CHARA_STATE_JUMP) && m_pOwner->isCharaStateMask(CHARA_STATE_FLOOR)) 
			{
				// 점프 종료시에 데미지 처리 한 번 더 합니다....
				UpdateFallDamage(rDeltatime);
				m_bGravityRecovery = false;

				ResetJumping();
			}
			else if (m_pOwner->isCharaStateMask(CHARA_STATE_DEATH) == false)									
				m_rElapsedTakeOffTime = m_rElapsedTakeOffTime + rDeltatime;
		}

		// Process 점프 , Process 낙하, Process 점프 및 낙하 완료
		REAL32 gravity_delta = 0.0f;		//중력 변량
		REAL32 jump_delta = 0.0f;		//힘 변량
		if (m_pOwner->isCharaStateMask(CHARA_STATE_JUMP)) //무엇인가 힘을 준 경우를 점프라 생각 해서 해당 행동 프로세스 처리
		{
			//rFac는 계산에 쓸 시간이라고 생각하자.
			if (m_pOwner->isCharaStateMask(CHARA_STATE_JUMPING_UP))
			{
				rFac = (m_rElapsedTakeOffTime)* I3_PI2 * Chara_Up_Gravity_Speed_late;
			}
			if (m_pOwner->isCharaStateMask(CHARA_STATE_JUMPING_DOWN))
			{
				REAL32 only_down_rFac = (m_rElapsedTakeOffTime - m_rJumpUpOldTime) * I3_PI2 * Chara_Down_Gravity_Speed_late;
				rFac = (m_rJumpUpOldTime)* I3_PI2 * Chara_Up_Gravity_Speed_late + only_down_rFac;
			}

			gravity_delta	= ((m_rHalfAccelOfGravity * rFac * rFac) * 0.5f);
			jump_delta		= (rFac * m_rCurJumpHeight);

			rNextHeight = getY(m_TakeOffPos) - (gravity_delta)+(jump_delta);
			rUp = rNextHeight - getY(&newPos);
			setY(m_VelCharacter, rUp);

			if ( m_pOwner->isCharaStateMask(CHARA_STATE_JUMPING_UP) && 
				 (gravity_delta >= (jump_delta*0.5f)) )
				{//점프 상승 시퀀스에서 하강 시퀀스로 바꾸는 부분.
					m_pOwner->removeCharaStateMask(CHARA_STATE_JUMPING_UP);
					m_pOwner->addCharaStateMask(CHARA_STATE_JUMPING_DOWN);
					m_rJumpUpOldTime = m_rElapsedTakeOffTime;
				}
		}
		else if (!m_pOwner->isCharaStateMask(CHARA_STATE_FLOOR)) //단순히 바닥이 없어 떨어지는 낙하 처리
		{
			//rFac는 계산에 쓸 시간이라고 생각하자.
			rFac = (m_rElapsedTakeOffTime) * I3_PI2 * chara_fall_gravity_late;

			gravity_delta = (((m_rHalfAccelOfGravity * rFac * rFac)) * 0.5f);

			rNextHeight = getY(m_TakeOffPos) - gravity_delta;
			rUp = rNextHeight - getY(&newPos);
			setY(m_VelCharacter, rUp);
		}
		else //완료 처리. 점프도 아니고 낙하도 아니니까요.
		{
			if (m_pOwner->isCharaStateMask(CHARA_STATE_JUMPING_DOWN)) m_pOwner->removeCharaStateMask(CHARA_STATE_JUMPING_DOWN);
			UpdateFallDamage(rDeltatime);
			// 그래비티 적용합니다.
			m_bGravityRecovery = false;
			setY(m_VelCharacter, -0.1f);
		}

		// 맵에서 빠졌을 경우 죽이는 루틴. 어떠한 경우에도 해당 루틴은 검사해야지라고 생각.
		if (EnableKill())
		{
			if ( MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_OUTPOST )
			{
				if (m_pOwner->GetPos()->y < -170.f) m_pOwner->Cmd_DamageSelf(255);
			}
			else
			{
				if (m_pOwner->GetPos()->y < -30.f) m_pOwner->Cmd_DamageSelf(255);
			}
		}

	}

	//I3TRACE("Half Gravity %f\n", m_rHalfAccelOfGravity);
}

void CGameCharaMoveContext::EnableSpeedReduction( REAL32 rRate, REAL32 rTime)
{
	m_rElapsedSpeedReduction = rTime;

	if (rTime == -1)
		m_bSpeedReductionEternal = true;

	m_rSpeedReductionRate = rRate;
}

void CGameCharaMoveContext::EnableSpeedReduction( UINT8 nHitPart)
{
	CHARA_HIT_PART_SIMPLE eCHPS = PartKind(nHitPart);
	const CSpeedReduction* pReDuction = m_pOwner->getDBInfo()->GetSpeedReduction(eCHPS);

	if(pReDuction == nullptr)
	{
		return;
	}

	EnableSpeedReduction( ((100.f - pReDuction->GetRate()) / 100.f), pReDuction->GetTime());
}

CHARA_HIT_PART_SIMPLE CGameCharaMoveContext::PartKind(UINT8 nHitPart)
{
	switch(nHitPart)
	{
	case CHARA_HIT_NECK	:
	case CHARA_HIT_LCLAVICLE :
	case CHARA_HIT_RCLAVICLE :		// 어깨
	case CHARA_HIT_BODY1 :
	case CHARA_HIT_BODY2 :
	case CHARA_HIT_BODY3 :
	case CHARA_HIT_ROOT	:			//	몸통 데미지
		{
			return CHPS_BODY;
		}
		break;

	case CHARA_HIT_RUARM:
	case CHARA_HIT_RDARM:
	case CHARA_HIT_LUARM:
	case CHARA_HIT_LDARM:			//	팔 데미지
		{
			return CHPS_ARM;
		}
		break;
	case CHARA_HIT_LHAND:
	case CHARA_HIT_RHAND:			//	손 데미지
		{
			return CHPS_HAND;
		}
		break;
	case CHARA_HIT_LULEG:
	case CHARA_HIT_LDLEG:
	case CHARA_HIT_RULEG:
	case CHARA_HIT_RDLEG:			//	다리 데미지
		{
			return CHPS_LEG;
		}
		break;
	case CHARA_HIT_LFOOT:
	case CHARA_HIT_RFOOT:			//	발 데미지
		{
			return CHPS_FOOT;
		}
		break;
	default: return CHPS_UNDKNOWN;
	}
	
}

void CGameCharaMoveContext::UpdateSpeedReduction(REAL32 rDeltaSeconds)
{
	if (m_bSpeedReductionEternal) return;

	m_rElapsedSpeedReduction -= rDeltaSeconds;
}

// 오브젝트 타기 계산
void CGameCharaMoveContext::CheckObjectByController( void)
{
	bool bRide = false;
	bool bTouchWorld = false;

	I3ASSERT_RETURN( m_pController != nullptr && m_pOwner != nullptr );

	PlayerController * pCtrl = (PlayerController *)(i3PhysixController*)m_pController;

	for( INT32 i = 0; i < pCtrl->getHitShapeCount(); i++)
	{
		CHitInfo* pHitInfo = pCtrl->GetHitInfo( i);

		NxShape * pShape = pHitInfo->GetHitShape();

		if( pShape == nullptr)
			continue;	// pShapeSet이 해제된 경우 이전 프레임에서 히트된 정보이다. 스킵

		i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet *) pShape->userData;
		I3ASSERT( pShapeSet != nullptr);
		if( pShapeSet == nullptr)
		{	// 이경우가 나온다면 데이터 자체를 의심해봐야 한다.
			continue;
		}

		pShapeSet->setReleaseCallback( nullptr, nullptr);

		i3EventReceiver * pReceiver = pShapeSet->getEventReceiver();
		if( pReceiver == nullptr )
			continue;

		if( i3::same_of<CGameWorld*>(pReceiver))
		{
			bTouchWorld = true;

			FromObjDamageToChara( pShape, pShapeSet);
		}
		else if( i3::kind_of<i3Object*>(pReceiver))
		{
			i3Object * pObj = ((i3Object*)pReceiver);

			FromObjDamageToChara( pShape, pShapeSet);

			if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) )
			{
				if( g_pFramework->IsBattleStage() )
				{
					// "BN_arrow_Green" 문자열 검색으로 녹색 화살표인지 식별
					// Obj type같은 걸 넣어서 정수 비교로 개선할 여지가 있음
					bool head_contain = i3::contain_string_head(pObj->GetName(), "BN_arrow_Green");

					if( head_contain )
					{
						char szTemp[256];
						strncpy(szTemp, pObj->GetName(), 256);
						INT32 iNum = atoi(&szTemp[14]); // BN_arrow_Green를 제외하고 번호를 int 값으로 바꾼다.
						GlobalFunc::PB_TRACE("iNum %d", iNum);
						if(gmode::i()->GetTutorialMode() != nullptr && gmode::i()->GetTutorialMode()->ChangeNextStateByArrowInTutorial(iNum))
						{
							g_pSndMng->StartCharacterActionSnd( GTBCA_ON_WARPGATE, 0, nullptr, true);
							pObj->setDynamicActiveState(false);
							pObj->InvisibleScene();
						}
					}
				}
			}
			
			if( i3::same_of<CGameObjectJumper*>(pObj))
			{
				if (m_pOwner->IsDino())
				{
					continue;
				}
				if( !MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE)
					&& !MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY)
					&& !MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_RUN_AWAY))
				{
					m_pOwner->WarpChara(((CGameObjectJumper*)pObj)->getJumpPos());
				
				}
				if( m_pOwner->isLocal())
				{	
					//Key:EscapeMission 충돌체크가 대면 이벤트를 수행하는 부분입니다.
					if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) )
					{
						I3ASSERT( pObj->getNetIndex() < 65536);
						UINT16 ObjectIdx = (UINT16) pObj->getNetIndex();
					
						if(GetTouchdown())
						{
							GAMEEVENT::Write_Chara_TouchDown( m_pOwner, ObjectIdx );
							//GameEventSender::i()->SetEvent(EVENT_BATTLE_TOUCHDOWN,&SlotIdx);
							SetTouchdown(false);
						
						}
					}
					else if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY))
					{
						I3ASSERT( pObj->getNetIndex() < 65536);

						if( MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON))
						{
							CGameCharaWeaponContext * pWeaponCtx = m_pOwner->getWeaponContext();
							if( pWeaponCtx->getCurrentSlotNum() == WEAPON_SLOT_MISSION)
							{
								UINT16 ObjectIdx = (UINT16) pObj->getNetIndex();
								GAMEEVENT::Write_Chara_TouchDown( m_pOwner, ObjectIdx );
							}
						}
						else
						{
							if(CGameMissionManager::i()->isMyUsurpatio() == true)
							{
								UINT16 ObjectIdx = (UINT16) pObj->getNetIndex();
								GAMEEVENT::Write_Chara_TouchDown( m_pOwner, ObjectIdx );
							}
						}
					}
					else if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_RUN_AWAY))
					{
						if(CGameMissionManager::i()->isMyUsurpatio() == true)
						{
							UINT16 ObjectIdx = (UINT16) pObj->getNetIndex();
							GAMEEVENT::Write_Chara_TouchDown( m_pOwner, ObjectIdx );
						}
					}
					else
					{
						m_pOwner->forceSetPos( ((CGameObjectJumper*)pObj)->getJumpPos());

						//pBattle->InitCheckDataPos();
						hd::check_speedhack::i()->init_pos();
						g_pSndMng->StartCharacterActionSnd( GTBCA_ON_WARPGATE, 0, nullptr, true);

					}
		
				
					
				}
				continue; 
			}
			else if( pObj->getNetworkSyncMode() == I3_NETWORK_GAMEOBJECT_TYPE_ANIM)
			{
				// 물리가 적용되는 AnimationObject의 경우 물리적용시점에서는 계산하면 안됩니다.
				// 물리 적용시점은 HP가 0인 경우의 오브젝트입니다.
				if( m_pOwner->isLocal() )
				{
					if( (pObj->getStateCount() > 1) && (pObj->getCurrentHP() == 0.f) )
					{
						ResetRideObject();
						continue;
					}

					VEC3D vUp;
					i3Vector::Set( &vUp, 0.f, 1.f, 0.f );
					REAL32 rDot = i3Vector::Dot( pHitInfo->GetHitNormal(), &vUp );

					if( rDot > 0.5f )
					{
						// 올라 탄 상태를 설정.
						m_pOwner->AddOnObjectState();
						SetRideObject( pObj);

						bRide = true;

						m_iRideObjectIndex = pObj->getNetIndex();
					}
				}
			}
			else if( i3::same_of<CGameObjectEvent*>(pObj))
			{
				CGameObjectEvent * ObjEvent = (CGameObjectEvent*)pObj;
				BattleObjContext::i()->ObjectHitWriteFunction( ObjEvent->getEventType(), pObj, m_pOwner );
			}
			else if( i3::same_of<CGameObjectWeaponBox*>(pObj))
			{
				// RPG7 Box와 충돌시
				if( m_pOwner->isLocal() && (m_pOwner->IsDino() == false))
				{
					CGameObjectWeaponBox * pWeaponBox = (CGameObjectWeaponBox*)pObj;

					if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY))
					{
						if(MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON))
						{
							CGameCharaWeaponContext * pWeaponCtx = m_pOwner->getWeaponContext();
							if( pWeaponCtx->getCurrentSlotNum() == WEAPON_SLOT_MELEE)
							{
								WeaponBase * pWeapon = pWeaponCtx->getWeapon( WEAPON_SLOT_MELEE);
								// 이미 쌍칼을 들고 있으면 스킵
								if( (pWeapon != nullptr) &&
									(pWeapon->getWeaponInfo()->GetNumber() == WEAPON::getItemIndex( WEAPON::CHINESECLEAVER_SEIZED)
										&& (pWeaponBox->GetWeaponType() != WT_CROWNWATERMELON)) )
								{
									continue;
								}

								// 수박이라면 레드팀만 체크
								if( (pWeaponBox->GetWeaponType() == WT_CROWNWATERMELON) && (m_pOwner->getTeam() != TEAM_RED))
									continue;
							}
							else
								continue;
						}
						else
						{
							if(pWeaponBox->GetWeaponType() != WT_CROWNWATERMELON)
								continue;
						}
					}
					else
					{
						if( ( pWeaponBox->getShellCount() < 1) ||
							( !MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && getWeaponContext()->getWeapon( WEAPON_SLOT_PRIMARY) != nullptr) ||
							( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && m_pOwner->getMultiWeaponContext()->CheckEmptySlot() == false))
						{
							continue;
						}
					}

					if( BattleServerContext::i()->IsP2PHost())
					{	// For Host
						pWeaponBox->HoldToChara( m_pOwner);
					}

					GAMEEVENT::Write_Chara_UseObject( m_pOwner, pObj, NET_ACTIONKEY_USEOBJECT);
				}
			}
			else if (i3::same_of<CGameObjectGiftbox*>(pObj))
			{
				CGameObjectGiftbox* giftbox = (CGameObjectGiftbox*)pObj;
				if (giftbox && m_pOwner->isLocal())
					giftbox->Collide(m_pOwner);
			}
			else if (i3::same_of<CGameObjectJumpHold*>(pObj))
			{
				//속도 차이로 2번이상 충돌 되는 경우가 있음 <- 해당 경우를 방지한다.
				CGameObjectJumpHold* jumphold = (CGameObjectJumpHold*)pObj;
				if ( jumphold && 
					 !( (m_pOwner->isCharaStateMask(CHARA_STATE_FORCEJUMP)) && jumphold == m_pPrevJumpHold ) 
				   )
				{
					if (m_pOwner->isLocal() && (m_pOwner->getCharaNetIndex() == BattleSlotContext::i()->GetMySlotIdxEx()))
					{
						GAMEEVENT::Write_Chara_UseObject(m_pOwner, pObj, NET_ACTIONKEY_USE_MOVE_CHARA_OBJECT);
						jumphold->JumpToChara(pShape, m_pOwner);
						jumphold->PlayJumpAnim();
						g_pSndMng->StartObjectSnd(GSND_OBJECT_JUMPHOLD, jumphold->GetPos());
						m_pPrevJumpHold = jumphold;
					}
				}
			}
			else if (i3::same_of<CGameObjectBattleUseItem*>(pObj))
			{
				// Enable 검사 후 object Action Key만 보냅니다.
				if (
					pObj->isEnable() == TRUE && 
					m_pOwner
					)
				{
					// 나라면 UDP를 보내고 제 3자이면 Enable만 끕니다.
					if (m_pOwner->isLocal() && (m_pOwner->getCharaNetIndex() == BattleSlotContext::i()->GetMySlotIdxEx()))
					{
						GAMEEVENT::Write_Chara_UseObject(m_pOwner, pObj, NET_ACTIONKEY_USEITEMBOX);
						pObj->SetEnable(FALSE);
					}
					else
					{
						pObj->SetEnable(FALSE);
					}
				}
			}
		}
	}

	// CHARA_MOVE_FLAG_STAND_ON_MOVEOBJ 변수는 CheckFloorTerrain에서 Raycast를 해서
	// 현재 움직이는 Object의 위에 서 있는지를 확인해 얻어진 결과이다.
	//
	// 아무것에도 닿지 않았으면서, Raycast 결과에서 Object의 위에 올라서 있지 않은
	// 경우에만 Ride Object 상태를 해제한다.
	//
	// CHARA_MOVE_FLAG_STAND_ON_MOVEOBJ에 대한 결과를 반영하지 않을 경우,
	// 올라 타 있으면서 Jump했을 때, Ride Object State가 풀려
	// 관성이 적용되지 않게 된다.
	if( bRide == false)
	{
		// 움직이는 Object에 닿고 있지 않다.

		// Floor Terrain 검사가 간헐적으로 이루어지기 때문에 CHARA_MOVE_FLAG_STAND_ON_MOVEOBJ 변수의
		// 값만 반영해선 안되며, 배경에 닿은 경우라면 항상 올라타 있음을 해제해주어야 한다.
		if( bTouchWorld == true)
		{
			ResetRideObject();
		}
		else if( isMoveFlag( CHARA_MOVE_FLAG_STAND_ON_MOVEOBJ) == false)
		{
			// 아무런 Object에도 올라타고 있지 않은 경우이기 때문에
			// OnObject 상태를 해제한다.
			ResetRideObject();
		}
	}

	((PlayerController*)(i3PhysixController*)m_pController)->setHitShapeCount( 0);
}

void CGameCharaMoveContext::UpdateRideObject( MATRIX * pNewTx, REAL32 tm, bool bPhysixUpdate)
{
	I3ASSERT( m_pRideObject != nullptr);

	i3Transform * pTrans = GlobalFunc::FindTransformFromPhysXSceneObj( m_pRideObject->getSceneObject() );

	if( pTrans != nullptr )
	{
	
		I3MATRIXINFO Env;
		MATRIX	matTop, matObj;

		i3Node * pParent = pTrans->GetParent();
		
		i3Matrix::Copy( &matObj, pTrans->GetMatrix() );

		// 해당 애니메이션을 계산합니다. 상위 노드의 Matrix를 가져와야 합니다.
		while( pParent != nullptr )
		{	
			if( i3::kind_of<i3Transform*>(pParent))
			{
				i3Matrix::Mul( &matTop, &matTop, ((i3Transform*)pParent)->GetMatrix() );
			}

			pParent = pParent->GetParent();
		}

		if( pTrans->IsAnimated())
		{
			i3SceneTracer * pTracer = g_pViewer->GetSceneTracer();
			Env.m_FrameID = pTracer->getFrameID();
			Env.m_pIn	= &matObj;
			Env.m_pOut	= &matObj;
			Env.m_Time	= g_pViewer->getDeltaTime();
			Env.m_pModelViewMatrix = &matTop;
			pTrans->GetAnimatedMatrix( &Env );
		}

		// 최종 애니메이션된 matrix
		i3Matrix::Mul( pNewTx, &matObj, &matTop );
	}
	else
	{
		I3ASSERT_0;
	}
}

void CGameCharaMoveContext::MoveCharaByObject( VEC3D * pPos)
{	
	if( m_pOwner->isCharaStateMask( CHARA_STATE_ONOBJECT) == true)
	{
		if ( !CGameMaster::i()->IsPaused() )
		{
			REAL32 SyncPosRatio = g_pViewer->getDeltaTime() * 60.f;		// 60Hz 대비 비례식으로 변경한다.. 60Hz일때 1..60넘으면 더 작은값.

			if (MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_OUTPOST)
			{
				m_RideSyncPos->y += 0.04f * SyncPosRatio;
			}
			else
			{
				m_RideSyncPos->y += 0.01f * SyncPosRatio;
			}

			SetPosition(m_RideSyncPos);
		}
	}
}


VEC3D * CGameCharaMoveContext::TransformByRideObject( MATRIX * pAnimatedMtx, VEC3D * pPos, bool bUpdate)
{
	MATRIX inv;
	// 속력을 계산하기위해 이전 좌표를 저장
	VEC3D vVel, vOldLocalPos;
	VEC3D vRealVel;
		
	if( m_pOldRideObject != m_pRideObject)
	{
		// 새로운 Object에 올라탔다.
		// 현재 Frame으로부터 Local-Pos를 구한다.
		m_pOldRideObject = m_pRideObject;

		i3Matrix::Inverse( &inv, nullptr, pAnimatedMtx);	

		// local position
		i3Vector::TransformCoord( m_RideLocalPos, pPos, &inv);
	}
	else
	{
		// Controller로부터 얻어진 Position(vPos)를
		// 이전 Frame에서의 Local-Pos로 역산한다.
		i3Matrix::Inverse( &inv, nullptr, m_matRideObjectPose);
		
		i3Vector::Copy( &vOldLocalPos, m_RideLocalPos);

		// local position
		i3Vector::TransformCoord( m_RideLocalPos, pPos, &inv);
		
		vRealVel.x = pAnimatedMtx->m30 - m_matRideObjectPose->m30;
		vRealVel.y = pAnimatedMtx->m31 - m_matRideObjectPose->m31;
		vRealVel.z = pAnimatedMtx->m32 - m_matRideObjectPose->m32;

		// 애니메이션에 사용할 속력 계산
		i3Vector::Sub( &vVel, &vOldLocalPos, m_RideLocalPos);
		i3Vector::SetY( &vVel, 0.f);
	}

	if( m_pOwner->isLocal() || m_pOwner->isAI())
	{
		REAL32 speed = i3Vector::Length( &vVel);
		setSpeed( speed);
	}

	// 올라 탄 Object의 Local-Space Position을 이번 Animation Transform에
	// 맞춘 World-Space Position으로 변환한다.
	i3Vector::TransformCoord( m_RideSyncPos, m_RideLocalPos, pAnimatedMtx);

	// Animation된 Object만큼 Controller도 강제로 위치 이동시킨다.
	if( bUpdate)
	{
		MoveCharaByObject( &vRealVel);
		RotateThetaByRideObejct( pAnimatedMtx, &inv);
	}

	i3Matrix::Copy( m_matRideObjectPose, pAnimatedMtx);

	return m_RideSyncPos;
}

void CGameCharaMoveContext::setRideObjectLocalPos(VEC3D* pPos)
{
	VEC3D vPos;
	i3Vector::Copy(m_RideLocalPos, pPos);
	i3Vector::TransformCoord( &vPos, m_RideLocalPos, m_matRideObjectPose);

	SetPosition(&vPos);
}

void CGameCharaMoveContext::RotateThetaByRideObejct( MATRIX * pAnimatedMtx, MATRIX * pInvMtx)
{
	MATRIX matAt;
	i3Matrix::Copy( &matAt, m_pOwner->GetMatrix());
	i3Matrix::PostRotateAxis( &matAt, i3Matrix::GetRight( &matAt), getBoneContext()->getRhoValue());
	i3Matrix::Mul( &matAt, &matAt, pInvMtx);
	i3Matrix::Mul( &matAt, &matAt, pAnimatedMtx);
	REAL32 rTheta = i3Math::atan2( getX( i3Matrix::GetAt( &matAt)), getZ( i3Matrix::GetAt( &matAt)));
	getBoneContext()->setTheta( rTheta + getInputContext()->getInputMoveTheta() );
}

void CGameCharaMoveContext::SetRideObject( i3Object * pObj)
{
	m_pRideObject			= pObj;
	m_iRideObjectCountTime	= 0;

 	m_pOwner->setAttachedObject( m_pRideObject);
}

void CGameCharaMoveContext::ResetRideObject( void)
{
	m_pRideObject			= nullptr;
	m_pOldRideObject		= nullptr;
	m_iRideObjectCountTime = 0;
	m_pOwner->setAttachedObject( nullptr);
	m_pOwner->RemoveOnObjectState();
}

void CGameCharaMoveContext::_TestCollisionAtRespawn()
{
	VEC3D vOtherDir;
	for( INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++ )
	{
		CGameCharaBase * pChara = g_pCharaManager->getChara( i );
		
		if (pChara == nullptr)
			continue;

		if( (pChara != m_pOwner) && !pChara->isCharaStateMask( CHARA_STATE_DEATH))
		{
			i3Vector::Sub( &vOtherDir, m_pOwner->GetPos(), pChara->GetPos() );
			REAL32 rLength = i3Vector::Length( &vOtherDir);

			if( rLength < 0.45f )
			{
				if (m_bPushingRandomDir)
				{
					VEC3D randomDir;
					i3Vector::GetRandVector(&randomDir);
					i3Vector::Normalize(&randomDir, &randomDir);
					i3Vector::ScaleAdd(&vOtherDir, &randomDir, rLength);
				}

				i3Vector::Add( m_VelCharacter, m_VelCharacter, &vOtherDir); 
			}
		}
	}

	if (m_bPushingRandomDir == true)
		m_bPushingRandomDir = false;
}

REAL32	CGameCharaMoveContext::_GetMoveVelocity(VEC3D * pVecDir, EMoveSpeedType MoveType)
{
	I3ASSERT(pVecDir != nullptr);
	I3ASSERT(MoveType == MST_RUN || MoveType == MST_WALK || MoveType == MST_JUMP_RUN || MoveType == MST_JUMP_WALK );

	EMoveDirectionType	MoveDirection;

	if( pVecDir->z > 0.0f)
	{
		if( pVecDir->x > 0.0f || pVecDir->x < 0.0f)
			MoveDirection = MDT_DIAGONAL;
		else
			MoveDirection = MDT_FRONT;
	}
	else if( pVecDir->z < 0.0f)
	{
		if( pVecDir->x > 0.0f || pVecDir->x < 0.0f)
			MoveDirection = MDT_DIAGONAL;
		else
			MoveDirection = MDT_BACK;
	}
	else
	{
		MoveDirection = MDT_SIDE;
	}

	return m_pOwner->GetMoveSpeedAddOffset(MoveType, MoveDirection);
}

INT32 CGameCharaMoveContext::_CalcFallingDamage( VEC3D * pTakeOffPos)
{
	I3ASSERT( pTakeOffPos != nullptr);
	I3ASSERT( g_pCharaInfoDataBase->getGlobal() != nullptr);

	INT32 nDamage = 0;

	VEC3D vLen, vDiffPos;
	REAL32 fTakeoffHeight = 0.f;

	i3Vector::Copy( &vDiffPos, m_pOwner->GetPos());
	i3Vector::SetX( &vDiffPos, i3Vector::GetX( pTakeOffPos));
	i3Vector::SetZ( &vDiffPos, i3Vector::GetZ( pTakeOffPos));

	i3Vector::Sub( &vLen, &vDiffPos, pTakeOffPos);		
	
	//// + 값은 데미지 계산을 하지 않는다.
	//if(vLen.y > 10.f)
	//{
	//	return 0;
	//}

	fTakeoffHeight = i3Vector::Length( &vLen);		//	떨어진 높이 계산

	//	낙하 데미지 최소 높이보다 높은 곳에서 떨어지면
	if( fTakeoffHeight >  m_pOwner->GetFallDamageMinHeight() + m_pOwner->GetFallDamageMinOffset() )
	{	
		INT32 nFullHP = m_pOwner->getFullHP();

		INT32 nMinDamage = m_pOwner->GetFallMinDamage();	//	최소 데미지

		//	현재 낙하 데미량 계산 (낙하 데미지 = 최소 데미지 + (현재 높이 * 낙하 Slope damage))
		REAL32 fallDmg = m_pOwner->GetFallDamageSlope();
		fallDmg += m_pOwner->getAbility( CHARA::eABILITY_FALLDAMAGE);
		nDamage = nMinDamage + (INT32) ( fTakeoffHeight * fallDmg);

		nDamage = MINMAX( nMinDamage, nDamage, nFullHP);
	}

	return nDamage;
}

void CGameCharaMoveContext::GetNxCapsule(NxCapsule* pCapsule)
{
	VEC3D vec;

	// i3Vector::Copy( &vec, m_pOwner->GetPos() );
	m_pController->getPosition( &vec );
	// i3Vector::Copy( &vec, m_pOwner->GetPos() );
	i3Vector::Add( &vec, 0.f, RADIUS+0.05f, 0.f );
	NxVec3 v1( getX(&vec), getY(&vec), getZ(&vec) );
	i3Vector::Add( &vec, 0.f, CAPSULE_FULL_HEIGHT-0.05f, 0.f );
	NxVec3 v2( getX(&vec), getY(&vec), getZ(&vec) );
	pCapsule->radius = RADIUS;
	pCapsule->p0 = v1;
	pCapsule->p1 = v2;
	
}

void CGameCharaMoveContext::PushByRaycastResult( const I3_PHYSIX_HIT_RESULT * pResult, const NxCapsule * pCapsule, NxVec3 &vDir, REAL32 skinWidth)
{
	//I3TRACE( "raycast %3f\n", pResult->m_T);
	I3ASSERT( pResult != nullptr);
	I3ASSERT( pCapsule != nullptr);

	REAL32 damp = (pCapsule->radius + skinWidth) - pResult->m_T + 0.01f;
	if( damp > 0.f)
	{
		m_v3ContactObjectVelToChara.x = vDir.x * damp;
		m_v3ContactObjectVelToChara.y = vDir.y * damp;
		m_v3ContactObjectVelToChara.z = vDir.z * damp;
		//I3TRACE( "Damp %3f\n", damp);
	}
}

// x, z축만 밀어 낸다.
// 올라 탔을 경우는 계산하지 않는다.
void CGameCharaMoveContext::CalcOverlapLength(NxShape* pShape, NxCapsule* pCapsule, REAL32 rSkinWidth, NxVec3 &vDir)
{
	/*if( m_pRideObject != nullptr)
	{
		return;
	}*/

	I3ASSERT(pShape);
	I3ASSERT(pCapsule);

	
	NxActor* pActor = nullptr;
	pActor = &(pShape->getActor());
	I3ASSERT(pActor);

	if(i3::generic_is_iequal(pActor->getName(), "__FastObject") )
	{
		return;
	}

	if( isMoveFlag(CHARA_MOVE_FLAG_PENETRATEWALL))
	{
		return;
	}
	
	i3Vector::Zero(&m_v3ContactObjectVelToChara);

	NxShapeType nxType = pShape->getType();

	if( nxType == NX_SHAPE_BOX )
	{
		REAL32 len = vDir.normalize();

		if( len > I3_EPS || len < -I3_EPS)
		{
			i3CollideeLine line;
			VEC3D start, end;

			i3Vector::Set( &start, pCapsule->p0.x, pCapsule->p0.y, pCapsule->p0.z);

			line.SetStart( &start);
			
			end.x = -vDir.x + start.x;
			end.y = -vDir.y + start.y;
			end.z = -vDir.z + start.z;

			line.SetEnd( &end);

			// 오브젝트 스피드 만큼 팅겨낸다.
			rSkinWidth += len * g_pFramework->GetViewer()->getDeltaTime();

			I3_PHYSIX_HIT_RESULT * pResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_OBJECT);

			if( pResult != nullptr)
				PushByRaycastResult( pResult, pCapsule, vDir, rSkinWidth);
			else
			{
				start.y -= pCapsule->radius;
				line.SetStart( &start);
				end.y -= pCapsule->radius;
				line.SetEnd( &end);

				pResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_OBJECT);
				if( pResult != nullptr)
					PushByRaycastResult( pResult, pCapsule, vDir, rSkinWidth);
				else
				{
					start.y = pCapsule->p1.y;
					line.SetStart( &start);
					end.y = pCapsule->p1.y;
					line.SetEnd( &end);

					pResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_OBJECT);
					if( pResult != nullptr)
						PushByRaycastResult( pResult, pCapsule, vDir, rSkinWidth);
					else
					{
						I3TRACE( "걸리는 컬리젼이 없다...\n");
					}
				}
			}
		}
		else
		{
			NxBoxShape* pBoxShape =	pShape->isBox();
			NxVec3 nxExtent = pBoxShape->getDimensions();
			NxVec3 nxPos = pBoxShape->getGlobalPosition();
			NxMat33 nxMat33 = pBoxShape->getGlobalOrientation();

			nxExtent.x = abs(nxExtent.x);
			nxExtent.y = abs(nxExtent.y);
			nxExtent.z = abs(nxExtent.z);
			
			NxVec3 v3CapsulePos;
			v3CapsulePos.x = pCapsule->p0.x;
			v3CapsulePos.z = pCapsule->p0.z;
			v3CapsulePos.y = 0;//pCapsule->p0.y;

			NxMat33 nxInvMat33;
			nxMat33.getInverse(nxInvMat33);
			//nxPos.y = 0;

			v3CapsulePos = v3CapsulePos - nxPos;

			nxInvMat33.multiply(v3CapsulePos, v3CapsulePos);

			nxPos.zero();
			pCapsule->p0.x = v3CapsulePos.x;
			pCapsule->p0.y = v3CapsulePos.y;
			pCapsule->p0.z = v3CapsulePos.z;

			//GlobalFunc::PB_TRACE("nxExtent %f %f %f \n", nxExtent.x, nxExtent.y, nxExtent.z);

			CalcContactObjectVel(nxExtent, nxPos, pCapsule, rSkinWidth);

			nxExtent.x =  m_v3ContactObjectVelToChara.x;
			nxExtent.y =  m_v3ContactObjectVelToChara.y;
			nxExtent.z =  m_v3ContactObjectVelToChara.z;

			//GlobalFunc::PB_TRACE("processed nxExtent %f %f %f \n", nxExtent.x, nxExtent.y, nxExtent.z);

			nxMat33.multiply(nxExtent, nxExtent);
			
			m_v3ContactObjectVelToChara.x = nxExtent.x;
			m_v3ContactObjectVelToChara.y = 0;
			m_v3ContactObjectVelToChara.z = nxExtent.z;
		}

		GlobalFunc::PB_TRACE("INV_ContactObjectVelToChara %f %f %f \n", m_v3ContactObjectVelToChara.x, m_v3ContactObjectVelToChara.y, m_v3ContactObjectVelToChara.z);


	}
	else if(nxType == NX_SHAPE_CONVEX)
	{
		NxBounds3 nxBound3;
		pShape->getWorldBounds(nxBound3);
		
		NxVec3 nxExtent;
		NxVec3 nxPos;
		nxBound3.getCenter(nxPos);
		nxBound3.getExtents(nxExtent);

		CalcContactObjectVelConvex(nxExtent, nxPos, pCapsule, rSkinWidth);
	}
}

void CGameCharaMoveContext::CalcContactObjectVel(NxVec3& nxExtent, NxVec3& nxPos, NxCapsule* pCapsule, REAL32 rSkinWidth)
{
	VEC3D v3BoxPos;
	VEC3D v3CapsulePos;
	VEC3D v3GapPos;

	v3BoxPos.x = nxPos.x;
	v3BoxPos.z = nxPos.z;
	v3BoxPos.y = nxPos.y;

	v3CapsulePos.x = pCapsule->p0.x;
	v3CapsulePos.z = pCapsule->p0.z;
	v3CapsulePos.y = pCapsule->p0.y;

	i3Vector::Sub(&v3GapPos, &v3CapsulePos, &v3BoxPos);

	REAL32 rLen = i3Math::abs(v3GapPos.x);
	REAL32 rGapX = rLen - (pCapsule->radius + nxExtent.x + rSkinWidth);
	VEC3D v3MoveVel;
	
	// GlobalFunc::PB_TRACE("CalcContactObjectVel() rGapX %f rSkinWidth %f \n", rGapX, rSkinWidth);

	if(rGapX < 0)
	{
		if(v3GapPos.x < 0)
		{
			v3MoveVel.x += rGapX;
		}
		else 
		{
			v3MoveVel.x -= rGapX;
		}
	}

	rLen = i3Math::abs(v3GapPos.z);
	REAL32 rGapZ = rLen - (pCapsule->radius + nxExtent.z + rSkinWidth);

	if(rGapZ < 0)
	{
		if(v3GapPos.z < 0)
		{
			v3MoveVel.z += rGapZ;
		}
		else 
		{
			v3MoveVel.z -= rGapZ;
		}
	}

	rLen = i3Math::abs(v3GapPos.y);
	REAL32 rGapY = rLen - (pCapsule->radius + nxExtent.y + rSkinWidth);

	if(rGapY < 0)
	{
		if(v3GapPos.y < 0)
		{
			v3MoveVel.y += rGapY;
		}
		else 
		{
			v3MoveVel.y -= rGapY;
		}
	}

	REAL32 rAbsGapX, rAbsGapZ, rAbsGapY;

	rAbsGapX = abs(rGapX);
	rAbsGapZ = abs(rGapZ);
	rAbsGapY = abs(rGapY);

	m_v3ContactObjectVelToChara.x = rAbsGapX;
	m_v3ContactObjectVelToChara.y = rAbsGapY;
	m_v3ContactObjectVelToChara.z = rAbsGapZ;

	if(m_v3ContactObjectVelToChara.x > m_v3ContactObjectVelToChara.y)
	{
		m_v3ContactObjectVelToChara.x = 0;
		if( m_v3ContactObjectVelToChara.y > m_v3ContactObjectVelToChara.z)
		{
			m_v3ContactObjectVelToChara.y = 0;
			m_v3ContactObjectVelToChara.z = v3MoveVel.z;
		}
		else
		{
			m_v3ContactObjectVelToChara.z = 0;
			m_v3ContactObjectVelToChara.y = v3MoveVel.y;
		}
	}
	else
	{
		m_v3ContactObjectVelToChara.y = 0;
		if( m_v3ContactObjectVelToChara.x > m_v3ContactObjectVelToChara.z)
		{
			m_v3ContactObjectVelToChara.x = 0;
			m_v3ContactObjectVelToChara.z = v3MoveVel.z;
		}
		else
		{
			m_v3ContactObjectVelToChara.z = 0;
			m_v3ContactObjectVelToChara.x = v3MoveVel.x;
		}
	}
}

void CGameCharaMoveContext::CalcContactObjectVelConvex(NxVec3& nxExtent, NxVec3& nxPos, NxCapsule* pCapsule, REAL32 rSkinWidth)
{
	VEC3D v3BoxPos;
	VEC3D v3CapsulePos;
	VEC3D v3GapPos;

	v3BoxPos.x = nxPos.x;
	v3BoxPos.z = nxPos.z;
	v3BoxPos.y = 0;

	v3CapsulePos.x = pCapsule->p0.x;
	v3CapsulePos.z = pCapsule->p0.z;
	v3CapsulePos.y = 0;
	
	i3Vector::Sub(&v3GapPos, &v3CapsulePos, &v3BoxPos);

	REAL32 rLen = i3Math::abs(v3GapPos.x);
	REAL32 rGapX = rLen - (pCapsule->radius + nxExtent.x + rSkinWidth);
	VEC3D v3MoveVel;
	
	// GlobalFunc::PB_TRACE("CalcContactObjectVel() rGapX %f rSkinWidth %f \n", rGapX, rSkinWidth);

	if(rGapX < 0)
	{
		if(v3GapPos.x < 0)
		{
			v3MoveVel.x += rGapX;
		}
		else 
		{
			v3MoveVel.x -= rGapX;
		}
	}

	rLen = i3Math::abs(v3GapPos.z);
	REAL32 rGapZ = rLen - (pCapsule->radius + nxExtent.z + rSkinWidth);

	if(rGapZ < 0)
	{
		if(v3GapPos.z < 0)
		{
			v3MoveVel.z += rGapZ;
		}
		else 
		{
			v3MoveVel.z -= rGapZ;
		}
	}

	rLen = i3Math::abs(v3GapPos.y);
	REAL32 rGapY = rLen - (pCapsule->radius + nxExtent.y + rSkinWidth);

	if(rGapY < 0)
	{
		if(v3GapPos.y < 0)
		{
			v3MoveVel.y += rGapY;
		}
		else 
		{
			v3MoveVel.y -= rGapY;
		}
	}

	REAL32 rAbsGapX, rAbsGapZ, rAbsGapY;

	rAbsGapX = abs(rGapX);
	rAbsGapZ = abs(rGapZ);
	rAbsGapY = abs(rGapY);

	if(rAbsGapX > rAbsGapZ)
	{
		// m_v3ContactObjectVelToChara.z += v3MoveVel.z;
		m_v3ContactObjectVelToChara.z = v3MoveVel.z;
	}
	else 
	{
		// m_v3ContactObjectVelToChara.x += v3MoveVel.x;
		m_v3ContactObjectVelToChara.x = v3MoveVel.x;
	}

	//I3TRACE("%f CalcContactObjectVel() m_v3ContactObjectVelToChara %f %f %f rAbsGap %f %f %f v3MoveVel %f %f %f\n", g_pFramework->GetAccTime(), m_v3ContactObjectVelToChara.x, 
	//	m_v3ContactObjectVelToChara.y, m_v3ContactObjectVelToChara.z, rAbsGapX, rAbsGapY, rAbsGapZ, v3MoveVel.x, v3MoveVel.y, v3MoveVel.z);
}


//void CGameCharaMoveContext::FindDamageObjectAndDamageCharaSelf(NxShape* pShape, i3PhysixShapeSet* pi3Shape)
//{
//	// I3TRACE("FindDamageObjectAndDamageCharaSelf \n");
//
//	if(EnableKill() == false) 
//	{
//		return;
//	}
//
//	I3ASSERT(pShape);
//	I3ASSERT(pi3Shape);
//
//	const char* pChar = nullptr;
//	NxActor* pActor = nullptr;
//	pActor = &(pShape->getActor());
//	
//	I3ASSERT(pActor);
//	pChar = pActor->getName();
//	
//	if(pChar == nullptr) 
//	{
//		return;
//	}
//
//	
//}

void CGameCharaMoveContext::DamageCharaSelf_IfOverlap(NxShape* pShape)
{
	if(EnableKill() == false) 
	{
		return;
	}

	I3ASSERT(pShape);

	const char* pChar = nullptr;
	NxActor* pActor = nullptr;
	pActor = &(pShape->getActor());
	
	I3ASSERT(pActor);
	pChar = pActor->getName();

	if(pChar == nullptr) 
	{
		return;
	}

	//I3TRACE("Overlap %s Actor Flag %d\n", pChar, pActor->readActorFlag(NX_AF_DISABLE_COLLISION));
	NxBoxShape* pBox = pShape->isBox();
	if(pBox)
	{
		//I3TRACE("Overlap Box pos %f %f %f half extent %f %f %f \n", pBox->getGlobalPosition().x, pBox->getGlobalPosition().y, pBox->getGlobalPosition().z, pBox->getDimensions().x, pBox->getDimensions().y, pBox->getDimensions().z);
	}
}

void CGameCharaMoveContext::FromObjDamageToChara(NxShape* pShape, i3PhysixShapeSet* pi3Shape)
{
	if(EnableKill() == false) 
	{
		return;
	}

    I3ASSERT(pShape);

	i3EventReceiver * pReceiver = pi3Shape->getEventReceiver();
	if( pReceiver == nullptr )
		return;

	REAL32 rSpeed = 0.0f;
	
	const char* pChar = nullptr;
	NxActor* pActor = nullptr;
	pActor = &(pShape->getActor());

	VEC3D v3Vel;

	if( pActor->isDynamic())
	{
		// World 같은 Static Actor의 경우에는 getLinearVelocity() 함수를 호출했을 때 Warning이 발생한다.
	
		pi3Shape->getLinearVelocity(&v3Vel);
		
		rSpeed = i3Vector::Length(&v3Vel);
	}
	
	I3ASSERT(pActor);
	pChar = pActor->getName();

	if(pChar == nullptr) 
	{
		return;
	}
	
	if(i3::generic_is_iequal(pChar, "__FastObject") ) 
	{
		if(rSpeed < 0.05f || rSpeed > 100.f)
		{
			//GlobalFunc::PB_TRACE("__FastObject rSpeed %f \n", rSpeed);
			return;
		}

		#if defined( I3_DEBUG)
		if( !i3::kind_of<i3Object*>(pReceiver))
		{
			I3ASSERT_0;
			return;
		}
		#endif

		VEC3D v3ObjectPos, v3CharaPos, v3DirFromObjToChara;
		MATRIX matTemp;
		REAL32 rDot;

		pi3Shape->getShapeGlobalPose(&matTemp);
	
		GetPosition(&v3CharaPos);
		v3ObjectPos.x = matTemp.m30;
		v3ObjectPos.y = matTemp.m31;
		v3ObjectPos.z = matTemp.m32;

		i3Vector::Sub(&v3DirFromObjToChara, &v3CharaPos, &v3ObjectPos);

		i3Vector::Normalize(&v3DirFromObjToChara, &v3DirFromObjToChara);
		i3Vector::Normalize(&v3Vel, &v3Vel);

		rDot = i3Vector::Dot(&v3DirFromObjToChara, &v3Vel);

        
		/*I3TRACE("__FastObject rSpeed %f rDot %f v3DirFromObjToChara %f %f %f v3Vel %f %f %f \n", rSpeed, rDot, v3DirFromObjToChara.x, v3DirFromObjToChara.y, v3DirFromObjToChara.z, 
				v3Vel.x, v3Vel.y, v3Vel.z);*/

		bool bDeath = false;
		I3_BOUNDCHK( ((i3Object*)pReceiver)->getNetIndex(), 65536);

		// 아웃 포스트이고
		if (MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_OUTPOST)
		{
			// 헬기에 타고 있지 않는 상태라면 
			if( ( i3::same_of<CGameObjectRidingHelicopter*>(pReceiver)
				&& m_pOwner->isCharaStateMask(CHARA_STATE_ONOBJECT)) == false)
			{
				// 죽인다.
				bDeath = true;
			}
		}
		else if (MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_HELISPOT)
		{
			// 헬기에 타고 있지 않는 상태라면 
			if( (i3::same_of<CGameObjectRespawnHelicopter*>(pReceiver)
				&& m_pOwner->isCharaStateMask(CHARA_STATE_ONOBJECT)) == false)
			{
				// 죽인다.
				bDeath = true;
			}
		}
		else if (MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_STORMTUBE)
		{
			bDeath = true;
		}
		else
		{
			// 나머지는 방향성에 따라 처리
			if(rDot > 0.f)
			{
				bDeath =true;
			}
		}

		if( bDeath)
			m_pOwner->Cmd_DamageSelf( 255, (UINT16) ((i3Object*)pReceiver)->getNetIndex() );
	}
	else if(i3::generic_is_iequal(pChar, "__DamageObject") ) 
	{
		VEC3D vDir;
		i3Vector::Set( &vDir, 1.f, 0.f, 0.f );

		VEC3D v3Vel2;
		pi3Shape->getLinearVelocity(&v3Vel2);
		REAL32 rMass = pi3Shape->getMass();
		REAL32 rSpeed2 = i3Vector::Length(&v3Vel2);
		//REAL32 rDamage = rMass * rSpeed * 0.005f;
		REAL32 rDamage = rMass * rSpeed2 * 0.1f;
		
		//if(rSpeed < 0.05f)
		if(rSpeed2 < 0.5f)
		{
			//GlobalFunc::PB_TRACE("__DamageObject rSpeed %f \n", rSpeed);
			return;
		}

		if(g_pFramework->GetAccTime() - m_rDamageTime > 1.f)
		{
			if(rDamage < 0.f)
			{
				rDamage = 0.f;
			}
			// revision 60567 스톰튜브 B에 폭탄 설치 후 정비용 전차에는 죽어야 함
			else if(MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_STORMTUBE)
			{
				rDamage = 255.f;
			}
			else if(rDamage > 25.f)
			{
				rDamage = 25.f;
			}

			I3_BOUNDCHK( ((i3Object*) pi3Shape->getEventReceiver())->getNetIndex(), 65536);
			m_pOwner->Cmd_DamageSelf((INT32)rDamage, (UINT16) ((i3Object*) pi3Shape->getEventReceiver())->getNetIndex() );

			m_rDamageTime = g_pFramework->GetAccTime();
		}

	}
	else if(i3::generic_is_iequal(pChar, "__DeathObject") ) 
	{
		m_pOwner->Cmd_DamageSelf( 255);
	}
	
}

// 방장인 경우에만 처리 - 박명진 2008. 7. 23
void CGameCharaMoveContext::CheckOverlapObject()
{
	INT32 i = 0;
	
	NxScene * pNxScene = g_pPhysixCtx->getScene();
	NxShape* pShapeList[32];
	
	NxCapsule nxCapsule;
	GetNxCapsule(&nxCapsule);
	INT32 iActiveGrp = (1<<CGRP_OBJECT_WHOLE) | (1<<CGRP_OBJECT) | (1<<CGRP_OBJECT_ALL);
	

	INT32 nOverlapCount = pNxScene->overlapCapsuleShapes( nxCapsule, NX_DYNAMIC_SHAPES, 32, pShapeList, nullptr, iActiveGrp);//, collisionGroup);//idxGroup, nullptr, true );

	// GlobalFunc::PB_TRACE("CheckOverlapObject p0 %f %f %f nOverlapCount %d\n", nxCapsule.p0.x, nxCapsule.p0.y, nxCapsule.p0.z, nOverlapCount);

	if( nOverlapCount > 0)
	{
		for( i = 0; i < nOverlapCount; ++i)
		{
			i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet*) pShapeList[i]->userData;
			if( pShapeSet == nullptr)
				continue;

			if( i3::same_of<i3PhysixShapeSet*>(pShapeSet) == false)
				continue;
			
			//GlobalFunc::PB_TRACE("CheckOverlapObject %s \n", pShapeList[i]->getActor().getName());

			FromObjDamageToChara(pShapeList[i], pShapeSet);
		}			
	}
}

bool CGameCharaMoveContext::EnableKill()
{
	GAME_MISSION_STATE missionState = CGameMissionManager::i()->getMissionState();

	if( BattleServerContext::i()->IsP2PActivate() &&
		(m_pOwner->isLocal()  || m_pOwner->isAI())&&
		!m_pOwner->isInvincible() &&	// 무적 타임 체크
		(( missionState != GAME_MISSION_STATE_ENDBATTLE) &&		// 라운드 종료가 아닌 경우 체크
			(missionState != GAME_MISSION_STATE_GAMEOVER) &&	// 게임 오버가 아닌 경우 체크
			(missionState != GAME_MISSION_STATE_PREBATTLE) ))	// 게임 준비 단계가 아닌 경우 체크
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CGameCharaMoveContext::MoveController( VEC3D * pMoveVec, REAL32 minDist, UINT32 * pHintFlag, REAL32 sharpness)
{
	m_pController->Move( pMoveVec, minDist, pHintFlag, sharpness);
	// GlobalFunc::PB_TRACE("MoveController pMoveVec %f %f %f \n", pMoveVec->x, pMoveVec->y, pMoveVec->z);

	if( isMoveFlag( CHARA_MOVE_FLAG_PENETRATEWALL))
	{
		if( (*pHintFlag & NXCC_COLLISION_SIDES) == NXCC_COLLISION_SIDES)
		{
			VEC3D vPos;
			m_pController->getPosition( &vPos);
			i3Vector::SetY( pMoveVec, 0.f);
			i3Vector::Add( &vPos, &vPos, pMoveVec);
			m_pController->setPosition( &vPos);
		}
	}
}

void CGameCharaMoveContext::OverlapObjectPush(VEC3D& vVel, REAL32 minDist, UINT32& hitMask)
{
	VEC3D v3Right;
	VEC3D v3Temp;
	VEC3D v3Result;

	if( i3Vector::Normalize(&v3Temp, &m_v3ContactObjectVelToChara) > I3_EPS)
	{
		i3Vector::Cross(&v3Right, &I3_YAXIS, &v3Temp);
		REAL32 rDotResult = i3Vector::Dot(&vVel, &v3Right);
		i3Vector::Scale(&v3Right, &v3Right, rDotResult);
		
		i3Vector::Add(&v3Result, &m_v3ContactObjectVelToChara, &v3Right);
		v3Result.y = vVel.y;
	}
	else
	{
		i3Vector::Copy( &v3Result, &vVel);
	}

	// 무브로 이동
	MoveController( &v3Result, minDist, &hitMask, 1.f);
}

void CGameCharaMoveContext::PostUpdate( REAL32 rDeltatime)
{
	i3Vector::Zero(&m_v3ContactObjectVelToChara);
}


void CGameCharaMoveContext::Move( REAL32 rDeltatime)
{
	VEC3D	vVel;
	VEC3D	vPos;
	UINT32	hitMask = 0;
	MATRIX	mtxCurRideWS;

	hitMask = 0;

	//GlobalFunc::PB_TRACE("getCharaStateMask %x \n", m_pOwner->getCharaStateMask());
	//GlobalFunc::PB_TRACE("Move m_Vel %f %f %f \n", m_Vel.x, m_Vel.y, m_Vel.z);
	
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
		return;
	
	// 땜빵.. 2008. 5. 15
	// 델타타임이 크면 이동거리에 영향을 줍니다.. 일정 시간을 넘게 되면 나머지 시간에 대해서 스킵해서 포지션을 계산합니다.
	// 이동 함수에만 적용합니다.
	if( rDeltatime > 0.1f)
		rDeltatime = 0.016f;

	// revision 59757 난간 옆으로 비스듬히 움직일 시 속도가 빨라져 스피드핵 발생하는 버그 수정
	//i3Vector::Copy( &m_vOldPos, m_pOwner->GetPos());

	if (!m_pOwner->isEnableStateMask(CHARA_STATE_DISABLE_GRAVITY))
		UpdateJumping( rDeltatime);
	else
		ResetJumping();

	CheckWorldCollision( rDeltatime);

	UpdateTouchDown(rDeltatime);

	m_pController->getPosition( &vPos );



	// 히트된 오브젝트 검사
	m_rRideObjectElapseTime = m_rRideObjectElapseTime - rDeltatime;
	if( m_rRideObjectElapseTime < 0.f )
		CheckObjectByController();

	CheckOverlapObject();	

	REAL32 minDist = 0.0001f;
	VEC3D vTest;
	if( m_VelCharacter->y < 0 )
	{
		i3Vector::Set( &vTest, 0.f, -0.01f, 0.f);
		MoveController( &vTest, minDist, &hitMask, 1.f);
	}
	else
	{
		hitMask = 0;
	}

	if( m_pOwner->isCharaStateMask( CHARA_STATE_ONOBJECT) == true 
		&& IsUpTownMoveTrainArea() == false 
		&& (MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_UPTOWN))
	{
		m_pOwner->removeCharaStateMask( CHARA_STATE_ONOBJECT);
	}

	if( m_pRideObject != nullptr)
	{
		// 이번 Frame에 이동할 위치를 Object로부터 얻어온다.
		// 내부에서 실제 PhysixConvex의 위치도 Update된다.
		UpdateRideObject( &mtxCurRideWS, rDeltatime, true);

		// 캐릭터의 좌표를 애니메이션된 위치로 맞춥니다.
		TransformByRideObject( &mtxCurRideWS, &vPos, true);
	}
	else
	{
		// 애니메이션에 사용할 속력 계산
		if ( !m_pOwner->isCharaStateMask(CHARA_STATE_CLIMB) )
		{
			i3Vector::Sub( &vVel, &vPos, m_pOwner->GetPos());
			i3Vector::SetY( &vVel, 0.f);
		}
		else
		{
			VEC3D vPos_Ground = vPos;
			vPos_Ground.y -= CHARASKINWIDTH;		// 함수 하단에 CHARASKINWIDTH를 빼는 코드가 있어서 여기서 옵셋을 맞춤..	
			i3Vector::Sub(&vVel, &vPos_Ground, m_pOwner->GetPos());		// 애니메이션 관련 속력계산을 위한 코드들 위치도 좋은것 같지는 않다.
		}																// (2014.12.03.수빈)

		//I3TRACE("SpeedCal Check 2 : %f\n", i3Vector::Length( &vVel));
		setSpeed( i3Vector::Length( &vVel) );
	}
	
	// 캐릭터 컬리젼 체크
	if( m_pOwner->isInvincible() )		// 리스폰 시에만 합니다.
	{
		_TestCollisionAtRespawn();
	}

	i3Vector::Copy( &vVel, m_VelCharacter);
	i3Vector::Copy( m_OldVel, m_VelCharacter );

	

	UINT32	hitMask2;
	PushProcessFromAnimObj(vVel, minDist, hitMask2);
	
	UINT32	hitResult = hitMask2 | hitMask;
	
	SlipSlope(hitResult, minDist, vVel);

	// revision 54918 2013. 3. 12 박기성
	//대각선 이동시 걸리는 문제 수정
	//if( !(i3Vector::isAlike(&m_vOldPos, &vPos, 1)))	// revision 59757 난간 옆으로 비스듬히 움직일 시 속도가 빨라져 스피드핵 발생하는 버그 수정
	//{
	//	CheckCollisionException(hitMask, vVel);
	//}
	//위코드가 이동시 높낮이 처리에 불필요한 부분으로 판단되어 주석처리 합니다. 16.10.19 김민제

	CheckCollisionException(hitMask, vVel);

	// Move함수 호출후 약간의 이동이 있을 수 있습니다.(Physix의 시뮬레이션이 돌아간 상태인듯 합니다..)
	m_pController->getPosition( &vPos );

	// 점프 상태에서 천정에 부딪히면 점프상태를 리셋
	if( m_pOwner->isCharaStateMask( CHARA_STATE_JUMP) && (hitResult & NXCC_COLLISION_UP))
	{
		ResetJumping();
		i3Vector::Add( &vPos, 0.f, -getY( &vVel), 0.f );
	}
	//// skinwidth 수정으로 높이 위치값 보정 - 박명진 2008. 11. 10
	//// skinwidth 0.025기준으로 보정값 0
	vPos.y -= CHARASKINWIDTH;
	//// 여기까지 (skinwidth 수정으로 높이 위치값 보정 - 박명진 2008. 11. 10)
	i3Vector::Copy( m_pOwner->GetPos(), &vPos);
	i3Vector::Zero(&m_v3ContactObjectVelToChara);
}

void CGameCharaMoveContext::CheckCollisionException(UINT32& hitMask, VEC3D& vVel)
{
	//점프중이라면 무시
	if(!m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR) ) 
	{
		return;
	}

	UINT32 tempMask = 0;
	PlayerController * pCtrl = (PlayerController *)(i3PhysixController*)m_pController;
	VEC3D * pIKPos = i3Matrix::GetPos( getBoneContext()->getIKMatrix());

	for( INT32 i = 0; i < pCtrl->GetHitWorldShapeCount(); i++)
	{
		SWorldHitInfo* pHitInfo = pCtrl->GetHitWorld(i);
		VEC3D* pHitWorldPos = pHitInfo->GetHitWorldPos();

		REAL32 tempHeightCalc = getY(pHitWorldPos) - (getY( pIKPos) - (CAPSULE_FULL_HEIGHT*0.5f) - 0.4f);
		
		if(tempHeightCalc >= 0.11f && tempHeightCalc <= 0.4f)
		{
			//I3TRACE("Height! : %f\n", tempHeightCalc);
			//걸리는 컬리전들.
			i3Vector::Set(&vVel, getX(&vVel) * 0.1f, getY(&vVel) * 0.1f, 0.f);
			pCtrl->InitWorldHitInfoArray();
			MoveController(&vVel, 0.003f, &tempMask);
		}
	}
}

void CGameCharaMoveContext::SlipSlope(UINT32& hitMask, REAL32& minDist, VEC3D& vVel)
{
	UINT32 tempMask = 0;

	if( hitMask & NXCC_COLLISION_DOWN)
	{	
		// 아직 공중에 위치한 상태에서 착지한 경우
		if( !m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR) )
		{
			// 바닥 재질을 검사합니다.
			m_pCurrentMoveControl->CheckFloorTerrain();
			m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
		}

		// 바닥의 경사면에 따라 캐릭터를 미끄러트립니다.
		{
			bool bReApplyMovePos = false;
			VEC3D * pFloorNormal = m_pController->getHitNormal();
			REAL32 rDot = i3Vector::Dot( &I3_YAXIS, pFloorNormal );

			if( rDot < 0.6f && rDot > I3_EPS)
			{	
				REAL32 rFactor = g_pViewer->getDeltaTime() * 60.f;		// 60mhz기준 비율 (헤르쯔가 높은 모니터일수록 수치를 적게 만들것임)
				i3Vector::Set(&vVel, getX(pFloorNormal) * 0.02f, -0.04f, getZ(pFloorNormal) * 0.02f);
				m_pOwner->addCharaStateMask(CHARA_STATE_SLOPESLIDE);

				bReApplyMovePos = true;
				vVel *= rFactor;
				minDist = i3Vector::Length(&vVel) * 0.1f;
			}
			else
				m_pOwner->removeCharaStateMask( CHARA_STATE_SLOPESLIDE);

			if( ((PlayerController*)(i3PhysixController*)m_pController)->getHitUser() == nullptr)
			{
				// 히트된 위치와 아래쪽의 Sphere와의 각도차를 계산하여 캐릭터를 옆으로 떨어뜨려야 합니다.
				// 점프시에 문제 해결
				VEC3D * pHitWorldPos = m_pController->getHitPos();
				VEC3D	vDownSpherePos;
				VEC3D * pIKPos = i3Matrix::GetPos( getBoneContext()->getIKMatrix());

				i3Vector::Set( &vDownSpherePos, getX( pIKPos), getY( pIKPos) - (CAPSULE_FULL_HEIGHT*0.5f), getZ( pIKPos) );
				i3Vector::Sub( &vDownSpherePos, &vDownSpherePos, pHitWorldPos);
				i3Vector::Normalize( &vDownSpherePos, &vDownSpherePos);
				REAL32 rDotAngle = i3Vector::Dot( &vDownSpherePos, &I3_YAXIS);

				if( !m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR)) 
				{	// 아직 점프중인 경우 체크
					if( rDotAngle < 0.7f && rDotAngle > 0.f)
					{
						i3Vector::Set( &vVel, getX( &vDownSpherePos) * 0.2f, 0.f, getZ( &vDownSpherePos) * 0.2f);
						ResetJumping();
						m_pOwner->addCharaStateMask( CHARA_STATE_JUMPPUSH);

						bReApplyMovePos = true;
						minDist = i3Vector::Length( &vVel) - 0.005f;
					}
				}
				else
				{
					if( rDotAngle < 0.45f && rDotAngle > 0.f)
					{	// 경사각이 45이상이면 밀어낸다
						i3Vector::Set( &vVel, getX( &vDownSpherePos) * 0.2f, 0.f, getZ( &vDownSpherePos) * 0.2f);

						bReApplyMovePos = true;
						minDist = i3Vector::Length( &vVel) - 0.005f;
					}
					else if( m_pOwner->isCharaStateMask( CHARA_STATE_JUMPPUSH))
					{	// 점프중 밀어내는 경우이면 한번 밀어내어 경사각이 45이상이되도록하여 자동으로 밀리도록 한다.
						VEC3D * pCharaPos = m_pOwner->GetPos();
						if( m_rFloorLength > (getY( pIKPos) - getY( pCharaPos)) * 0.5f + 0.2f)
						{
							i3Vector::Set( &vVel, getX( &vDownSpherePos) * 0.3f, 0.f, getZ( &vDownSpherePos) * 0.3f);
							m_pOwner->removeCharaStateMask( CHARA_STATE_JUMPPUSH);

							bReApplyMovePos = true;
							minDist = i3Vector::Length( &vVel) - 0.005f;
						}
					}
				}
			}
			else
			{
				CGameCharaBase * pHitUser = ((PlayerController*)(i3PhysixController*)m_pController)->getHitUser();
				i3Vector::Sub( &vVel, m_pOwner->GetPos(), pHitUser->GetPos());
				setY( &vVel, 0.f);

				bReApplyMovePos = true;
				minDist = (RADIUS + RADIUS_NETWORK) - i3Vector::Length( &vVel);
			}

			// 다시 이동처리가 있으면 처리
			if( bReApplyMovePos)
			{	
				if( minDist > 0.3f)						minDist = 0.3f;
				else if( minDist < 0.0001f)				minDist = 0.0001f;

				if(m_pOwner->IsDino() == false) {
					MoveController( &vVel, minDist, &tempMask);
				}
			}
		}

		// 바닥에 닿았다면....
		m_pOwner->addCharaStateMask( CHARA_STATE_FLOOR);

		ResetLadderJump(m_ladder_info);
	}
	else
	{
		// 바닥에서 떯어졌다면
		m_pOwner->removeCharaStateMask( CHARA_STATE_FLOOR );
		m_pOwner->removeCharaStateMask( CHARA_STATE_SLOPESLIDE );
	}
}

void CGameCharaMoveContext::OverlapPush()
{
	REAL32 rObjectSpeed = i3Vector::Length(&m_v3ContactObjectVelToChara);

	if( MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_STORMTUBE &&
		MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) )
	{
		if(rObjectSpeed > I3_EPS)
		{
			VEC3D vPos;
			m_pController->getPosition(&vPos);
			i3Vector::Add(&vPos, &vPos, &m_v3ContactObjectVelToChara);

			SetPosition(&vPos);
		}
	}
}

void CGameCharaMoveContext::PushProcessFromAnimObj(VEC3D& vVel, REAL32 minDist, UINT32& hitMask)
{
	REAL32 rObjectSpeed = i3Vector::Length(&m_v3ContactObjectVelToChara);
	
	if( (MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_STORMTUBE)  ||
		(MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_STORMTUBE)  ||
		(MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_UPTOWN) )
	{
		// 무브 방식
		if(rObjectSpeed > I3_EPS)
		{
			OverlapObjectPush(vVel, minDist, hitMask);
			i3Vector::Zero( m_VelCharacter);	// 이동값을 초기화
		}
		else 
		{
			MoveController( &vVel, minDist, &hitMask, 1.f);
		}
	}
	else
	{
		bool bAddableObjectVel = false;
		if ((MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_PORT_AKABA))
		{
			if (rObjectSpeed > I3_EPS)
				bAddableObjectVel = true;
		}
		else
		{
			if (rObjectSpeed > I3_EPS && m_pRideObject == nullptr)
			{
				bAddableObjectVel = true;
			}
		}

		if (bAddableObjectVel)
		{
			// set으로는 월드를 뚫고 들어갈 수 있어 move에서 처리되도록 속도에 더한다. swoongoo.jung 2014.03.14
			i3Vector::Add(&vVel, &vVel, &m_v3ContactObjectVelToChara);

			if (i3Vector::Length(&m_v3ContactObjectVelToChara) > I3_EPS)
			{	// 캐릭터의 이동속도를 없앤다.
				i3Vector::Zero(m_VelCharacter);
			}
		}

		MoveController(&vVel, minDist, &hitMask, 1.f);
	}
}

void CGameCharaMoveContext::SetPosition( const VEC3D * pPos)
{
	if (m_pController)
		m_pController->setPosition( const_cast<VEC3D*>(pPos) );
}

void CGameCharaMoveContext::GetPosition( VEC3D * pPos)
{
	if (m_pController)
		m_pController->getPosition( pPos);
}


void CGameCharaMoveContext::ProcessAccel( void)
{
	if(m_pOwner->IsDino() == false)
	{
		if( getCurrentWeapon() == nullptr)
		{
			return;
		}
	}

	VEC3D	vDist;
	VEC3D	vDistAt;
	VEC3D	vDistRight;

	REAL32	fMoveVelocity	= 0.f;
	MATRIX * pCharaMat = m_pOwner->GetMatrix();
	VEC3D *	pVecDir = getMoveDir();

	if( (pVecDir->x != 0.0f) ||	(pVecDir->y != 0.0f) ||	(pVecDir->z != 0.0f))
	{
		CHARA_ACTION_BODYLOWER	stateBody = getActionContext()->getBodyLower();
		CHARA_ACTION_UPPER	stateUpper = getActionContext()->getUpper();

		//	캐릭터 뛰기
		if( getActionContext()->isLower( CHARA_LOWER_RUN))
		{
			fMoveVelocity = _GetMoveVelocity(pVecDir, MST_RUN);
		}
		//	캐릭터 걷기
		else if( getActionContext()->isLower( CHARA_LOWER_WALK))
		{
			fMoveVelocity = _GetMoveVelocity(pVecDir, MST_WALK);
		}

		//점프 및 낙하 이동속도 정하는 곳.
		if ( CHARAMOVE::isCurrentStage_Same_MMT(MMT_SPACE) &&	//모드 검사
			 ( (m_pOwner->isCharaStateMask(CHARA_STATE_JUMP)  ) || !m_pOwner->isCharaStateMask(CHARA_STATE_FLOOR) ) //상태 검사
		   )
		{
			if (getActionContext()->isLower(CHARA_LOWER_RUN))
			{
				fMoveVelocity = _GetMoveVelocity(pVecDir, MST_JUMP_RUN);
			}
			//	캐릭터 걷기
			else if (getActionContext()->isLower(CHARA_LOWER_WALK))
			{
				fMoveVelocity = _GetMoveVelocity(pVecDir, MST_JUMP_WALK);
			}
		}

		//	현재 동작에 의해 이동 수치가 영향을 받습니다.
		if(m_pOwner->IsDino() == false)
		{
			WeaponBase * pWeapon = getCurrentWeapon();

			if( pWeapon != nullptr )
			{	
				if( stateBody == CHARA_BODYLOWER_STAND)			//	기본 서있는 상태에서
				{
					fMoveVelocity *= pWeapon->GetMoveSpeed( WEAPON_MOVE_SPEED_NORMAL) * 0.01f;
				}
				else if( stateBody == CHARA_BODYLOWER_CROUCH)		//	기본 앉아있는 상태에서
				{
					fMoveVelocity *= pWeapon->GetMoveSpeed( WEAPON_MOVE_SPEED_SIT) * 0.01f;
				}
				
				if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
				{//	견착 또는 줌 상태일때
					//	줌인 중이지만 장전시에는 적용안되게...
					if( stateUpper != CHARA_UPPER_LOADMAGAZINE && stateUpper != CHARA_UPPER_LOADBULLET &&
						stateUpper != CHARA_UPPER_FIREBULLET)	
					{
						fMoveVelocity *= pWeapon->GetMoveSpeed( WEAPON_MOVE_SPEED_ZOOM) * 0.01f;
					}

				}

				if( stateUpper == CHARA_UPPER_FIRE || stateUpper == CHARA_UPPER_SECONDARY_FIRE)			//	발사 상태일때
				{
					fMoveVelocity *= pWeapon->GetMoveSpeed( WEAPON_MOVE_SPEED_FIRE) * 0.01f;
				}
			}
		}

		if( !getActionContext()->isLower( CHARA_LOWER_JUMP) )
		{
			i3Vector::Copy( getMoveAt(), i3Matrix::GetAt( pCharaMat));
			i3Vector::Copy( getMoveRight(), i3Matrix::GetRight( pCharaMat));

//			FIXED32 test = i3Vector::Dot(getMoveAt(), getMoveRight());

			if( fMoveVelocity != getMaxSpeed())
			{
				m_rOldMaxMoveSpeed = getMaxSpeed();
			}

			if( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_DIEHARD) )
			{
				INT32 charaNetIndex = getCharaNetIndex();

				if (charaNetIndex < 0 || charaNetIndex >= SLOT_MAX_COUNT)
				{
					I3_BOUNDCHK(charaNetIndex, SLOT_MAX_COUNT);
				}
				else
				if( m_pOwner->isAI())
				{
					AI_ENTRY* pAiEntry = &AIModeContext::i()->AIEntry[charaNetIndex];
					I3_BOUNDCHK(pAiEntry->_level,MAX_AI_COUNT+1);
					fMoveVelocity = fMoveVelocity * g_pTempConfig->m_Speed[pAiEntry->_level];				
				}
			}else if(MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS))
			{
				INT32 charaNetIndex = getCharaNetIndex();

				if (charaNetIndex < 0 || charaNetIndex >= SLOT_MAX_COUNT)
				{
					I3_BOUNDCHK(charaNetIndex, SLOT_MAX_COUNT);
				}
				else
				if( m_pOwner->isAI())
				{
					AI_ENTRY* pAiEntry = &AIModeContext::i()->AIEntry[charaNetIndex];
					AI_BIND_DATA * pBindData = g_pCharaManager->getAiBindData( pAiEntry->_number );
					fMoveVelocity = fMoveVelocity * pBindData->_Speed;
				}
			}

			setMaxSpeed( fMoveVelocity);
		}

		if (m_pOwner->isCharaStateMask(CHARA_STATE_CLIMB))									
		{
			vDistAt = *GetCurrLadderAccAxis(m_ladder_info);			// 사다리 올라탄 상태에서 이동방향 변경.
			i3Vector::Scale( &vDistAt, &vDistAt, pVecDir->z);
			vDistRight = *GetCurrLadderRightAxis(m_ladder_info);
			i3Vector::Scale( &vDistRight, &vDistRight, pVecDir->x);
		}
		else
		{
			i3Vector::Normalize( &vDistAt, getMoveAt());			// 바라보는 방향...
			i3Vector::Scale( &vDistAt, &vDistAt, pVecDir->z);
			i3Vector::Normalize( &vDistRight, getMoveRight());
			i3Vector::Scale( &vDistRight, &vDistRight, pVecDir->x);
		}

		i3Vector::Add( &vDist, &vDistAt, &vDistRight);

		i3Vector::Normalize( &vDist, &vDist);

		//i3Vector::Scale( &vDist, &vDist, fMoveVelocity);
	}
	else
	{
		//	이동중이 아니다.
		getAnimContext()->setRunElapseTime( 0.f);
		m_pOwner->removeCharaStateMask( CHARA_STATE_FOOTSTEPRIGHT);
	}

	setAccel( &vDist);
}



void	CGameCharaMoveContext::setLadderStateInfo(const LadderStateInfo* info)
{
	m_ladder_info = info;
}

const LadderStateInfo* CGameCharaMoveContext::getLadderStateInfo() const
{
	return m_ladder_info;
}

bool		CGameCharaMoveContext::isJumpingOnLadder() const
{
	return ::IsJumpingOnLadder(m_ladder_info);
}

