#if !defined( __I3_PHYSIX_CONTEXT_H)
#define __I3_PHYSIX_CONTEXT_H

#if defined ( I3_PHYSX)

#include "i3Collidee.h"
#include "i3CollideeLine.h"
#include "i3CollideeSphere.h"
#include "i3PhysixDefine.h"
#include "i3PhysixUtil.h"
#include "i3PhysixShapeSet.h"
#include "i3PhysixMemoryPool.h"

enum I3_PHYSIX_CMD
{
	I3_PHYSIX_CMD_UPDATE_DYNAMIC_STATE = 0,
	I3_PHYSIX_CMD_RELEASE_ACTOR,
	I3_PHYSIX_CMD_RELEASE_JOINT,
	I3_PHYSIX_CMD_RELEASE_CONTROLLER,
	I3_PHYSIX_CMD_UPDATE_GLOBAL_POSE,
	I3_PHYSIX_CMD_SET_LINEAR_VELOCITY,
	I3_PHYSIX_CMD_SET_ANGULAR_VELOCITY,
	I3_PHYSIX_CMD_SET_DIMENSION,
};

typedef union _tagPhysixCmdParam
{
	UINT32		m_i[12];
	REAL32		m_r[12];
} I3_PHYSIX_CMD_PARAM;

typedef struct _tagPhysixCmdInfo
{
	I3_PHYSIX_CMD			m_Command;
	i3PhysixShapeSet *		m_pShapeSet;
	void *					m_pObject;
	I3_PHYSIX_CMD_PARAM		m_Param;

	void Reset( void)
	{
		m_Command = I3_PHYSIX_CMD_UPDATE_DYNAMIC_STATE;
		m_pShapeSet = NULL;
		m_pObject = NULL;
		for( UINT32 i = 0; i < 12; i++ )
		{
			m_Param.m_i[i] = 0;
			m_Param.m_r[i] = 0.f;
		}
	}
} I3_PHYSIX_CMD_INFO;

typedef struct _tagPhysixReleaseActorInfo
{
	NxActor *		m_pActor;
	REAL32			m_time;
} I3_PHYSIX_RELEASE_ACTOR_INFO;

enum
{
	MPT_22 = 0,
	MPT_110,
	MPT_282,
	MPT_350,

	MPT_MAX
};

typedef struct _tagi3PhysixFilterCtx
{
	UINT32		mask0;
	UINT32		mask1;
	NxFilterOp	op0;
	NxFilterOp	op1;
	NxFilterOp	op2;
	bool		result;
} I3_PHYSIX_FILTER_CTX;

#define MAX_FILTER_STACK		2

#define PHF_FILTER				0x00000001

class i3PhysixContext;
extern I3_EXPORT_SCENE i3PhysixContext *	g_pPhysixCtx;

class I3_EXPORT_SCENE i3PhysixContext : public i3ElementBase
{
	I3_CLASS_DEFINE( i3PhysixContext);
protected:
	NxPhysicsSDK *			m_pSDK;
	NxScene *				m_pScene;
	NxActor *				m_pGroundPlaneActor;

	I3_PHYSIX_HIT_RESULT *	m_pResultList;
	UINT32					m_ResultCount;
	UINT32					m_ResultReserveCount;
	UINT32					m_ResultBuffSize;

	I3_PHYSIX_FILTER_CTX	m_FilterCtx[MAX_FILTER_STACK];
	INT32					m_idxFilter;
	
	UINT32					m_GroupMask[32];

	REAL32					m_rSimTimeSlop;
	REAL32					m_rFXtoForce;
	REAL32					m_rFXMax;

	BOOL					m_bSimulate;
	bool					m_bSimStarted;
	bool					m_bInSimulating;

	i3List					m_ConvexList;
	i3List					m_MeshList;

	i3List					m_RelActorList;

	// Ring Buffer
	I3_PHYSIX_CMD_INFO *	m_pCmdRing;
	INT32					m_idxCmdHead;
	INT32					m_idxCmdTail;

	void					_initCmd(void);
	I3_PHYSIX_CMD_INFO *	_allocCmd(void);
	void					_pushCmd( I3_PHYSIX_CMD cmd, i3PhysixShapeSet * pShapeSet, void * pObj, UINT32 param1 = 0, UINT32 param2 = 0);
	void					_pushCmd( I3_PHYSIX_CMD cmd, i3PhysixShapeSet * pShapeSet, void * pObj, MATRIX * pMtx);
	void					_pushCmd( I3_PHYSIX_CMD cmd, i3PhysixShapeSet * pShapeSet, void * pObj, VEC3D * pVec);
	void					_pushCmd( I3_PHYSIX_CMD cmd, i3PhysixShapeSet * pShapeSet, void * pObj, VEC3D * pVec1, VEC3D * pVec2);
	void					_resetCmd(void);
	void					_processCmd( REAL32 tm);

	void					_OnUpdateDynamicState( I3_PHYSIX_CMD_INFO * pInfo);
	void					_OnReleaseActor( NxActor * pActor);
	void					_OnReleaseJoint( I3_PHYSIX_CMD_INFO * pInfo);
	void					_OnReleaseController( I3_PHYSIX_CMD_INFO * pInfo);
	void					_OnUpdateGlobalPose( I3_PHYSIX_CMD_INFO * pInfo);
	void					_OnSetLinearVelocity( I3_PHYSIX_CMD_INFO * pInfo);
	void					_OnSetAngularVelocity( I3_PHYSIX_CMD_INFO * pInfo);
	void					_OnSetDimension( I3_PHYSIX_CMD_INFO * pInfo);
public:
	UINT32					m_nProfileTransCount;

	INT64					m_timeAllocMax;
	INT64					m_timeAllocTotalMax;
	INT64					m_timeFreeMax;
	
	UINT32					m_nProfileMemAllocMaxCount;
	UINT32					m_nProfileMemReAllocMaxCount;
	UINT32					m_nProfileMemFreeMaxCount;

	UINT32					m_nProfileMemAllocSize;

	INT64					m_timeRaycastMax;
	UINT32					m_nProfileRaycastMaxCount;

	// 추가 정보입니다.
	// 데칼에서 액터 패어 그룹에 사용됩니다.
	UINT16					m_nWorldHitCollisionGroup;
	UINT16					m_nObjectHitCollisionGroup;
	UINT16					m_nDecalCollisionGroup;

	void					_getProfileString( char * pszStr);
public:
	UINT32	getGroupMask( INT32 idx )	{ I3ASSERT(idx<32); return m_GroupMask[ idx ]; }

	REAL32	getGravity( void )
	{
		if( m_pScene == NULL )	return 0.f;
		NxVec3 vGravity;
		m_pScene->getGravity( vGravity );
		return vGravity.y;
	}
	void	setGravity( REAL32 rGravity )
	{
		if( m_pScene == NULL )	return;
		NxVec3 vGravity( 0.f, rGravity, 0.f );
		m_pScene->setGravity( vGravity );
	}

	void	setActorGroupPairFlags( UINT16 nGrp1, UINT16 nGrp2, UINT32 nFlag )
	{
		if( m_pScene == NULL )	return;
		m_pScene->setActorGroupPairFlags( nGrp1, nGrp2, nFlag );
	}

	void	setActorPairFlags( NxActor * pActor1, NxActor * pActor2, UINT32 nFlag )
	{
		if( m_pScene == NULL )	return;
		m_pScene->setActorPairFlags( *pActor1, *pActor2, nFlag );
	}

	void	setActorPairFlags( i3PhysixShapeSet * p1, i3PhysixShapeSet * p2, UINT32 nFlag)
	{
		if( m_pScene == NULL )	return;
		m_pScene->setActorPairFlags( *p1->getNxActor(), *p2->getNxActor(), nFlag );
	}

	void	setShapePairFlags( NxShape * pShape1, NxShape * pShape2, UINT32 nFlag)
	{
		if( m_pScene == NULL )	return;
		m_pScene->setShapePairFlags( *pShape1, *pShape2, nFlag );
	}

	REAL32	getSimTime( void )								{ return m_rSimTimeSlop; }
	void	setSimTime( REAL32 rSimTime )					{ m_rSimTimeSlop = rSimTime; }

	REAL32	getFxToForce( void )							{ return m_rFXtoForce; }
	void	setFxToForce( REAL32 rValue )					{ m_rFXtoForce = rValue; }

	REAL32	getFxMax( void)									{ return m_rFXMax; }
	void	setFxMax( REAL32 rValue)						{ m_rFXMax = rValue; }

	void	setSleepLinVelocity( REAL32 rLinVelocity )
	{
		if( m_pSDK == NULL )	return;
		m_pSDK->setParameter( NX_DEFAULT_SLEEP_LIN_VEL_SQUARED, rLinVelocity );
	}
	void	setSleepAngVelocity( REAL32 rAngVelocity )
	{
		if( m_pSDK == NULL )	return;
		m_pSDK->setParameter( NX_DEFAULT_SLEEP_ANG_VEL_SQUARED, rAngVelocity );
	}
	void	setBoundThresHold( REAL32 rValue )
	{
		if( m_pSDK == NULL )	return;
		m_pSDK->setParameter( NX_BOUNCE_THRESHOLD, rValue );
	}
	void	setMaxAngVelocity( REAL32 rLimit )
	{
		if( m_pSDK == NULL )	return;
		m_pSDK->setParameter( NX_MAX_ANGULAR_VELOCITY, rLimit );
	}

	void	setStaticFriction( REAL32 rStaticFriction )
	{
		if( m_pSDK == NULL )	return;
		m_pSDK->setParameter( NX_STA_FRICT_SCALING, rStaticFriction );
	}

	void	setDynamicFriction( REAL32 rDynamicFriction )
	{
		if( m_pSDK == NULL )	return;
		m_pSDK->setParameter( NX_DYN_FRICT_SCALING, rDynamicFriction );
	}

	void	setEnableSimulate( BOOL bValue )		{ m_bSimulate = bValue; }
	bool	isSimulating(void)						{ return m_bInSimulating; }

public:
	i3PhysixContext(void);
	virtual ~i3PhysixContext(void);

	// TimeStepMethod { NX_TIMESTEP_VARIABLE, NX_TIMESTEP_FIXED }
	// Result 카운트는 줄이면 안됩니다. -- 20080613 dfly79
	bool	Create( NxTimeStepMethod timeStepMethod = NX_TIMESTEP_VARIABLE, REAL32 rSimTimeSlop = 1.f, UINT32	numResult = 300);

	bool	IsHardwarePresent();
	
	void	simulate( REAL32 tm);
	void	fetchResults( REAL32 tm);
	void	reset(void);

	NxPhysicsSDK *			getSDK(void)						{ return m_pSDK; }
	NxScene *				getScene(void)						{ return m_pScene; }

	UINT32					getResultCount(void)				{ return m_ResultCount; }
	I3_PHYSIX_HIT_RESULT *	getResult(void)						{ I3ASSERT( m_ResultReserveCount < m_ResultBuffSize); return m_pResultList + m_ResultReserveCount; }
	void					resetResult(void)					{ m_ResultCount = m_ResultReserveCount = 0; }
	void					reserveResult( UINT32 cnt)			{ I3ASSERT( cnt < m_ResultBuffSize); m_ResultReserveCount = cnt; }

	NxActor *				createActor( NxActorDesc * pDesc)
	{ 
		if( m_pSDK == NULL)		return NULL;
		return m_pScene->createActor( *pDesc); 
	}

	void					CMD_releaseActor( NxActor * pActor);

	NxJoint *				createJoint( NxJointDesc * pDesc)		
	{ 
		if( m_pSDK == NULL)		return NULL;		
		return m_pScene->createJoint( *pDesc); 
	}

	void					CMD_releaseJoint( NxJoint * pJoint);

	I3_TERRAIN_MATERIAL *	getTerrainMaterialPreset( I3_TERRAIN_TYPE terrain);
	NxMaterial *			createMaterial( NxMaterialDesc * pDesc)	
	{ 
		if( m_pSDK == NULL)		return NULL;
		return m_pScene->createMaterial( (const NxMaterialDesc&)(*pDesc) ); 
	}

	void					releaseMaterial( NxMaterial * pMtl)		
	{ 
		if( m_pSDK == NULL)		return;
		m_pScene->releaseMaterial( *pMtl); 
	}
	NxMaterial *			validateMaterial( REAL32 restitution, REAL32 sFriction, REAL32 dFriction);
	void					RemoveAllMaterials(void);
	UINT32					getMaterialCount(void)					
	{ 
		if( m_pSDK == NULL)		return 0;
		return m_pScene->getNbMaterials();
	}

	NxMaterial *			getMaterial( INT32 idx)					
	{ 
		if( m_pSDK == NULL)		return NULL;
		return m_pScene->getMaterialFromIndex( (NxMaterialIndex)idx); 
	}
	

	NxController *			createController( NxControllerDesc * pDesc);
	void					CMD_releaseController( NxController * pCtrl);

	void					setGroupCollisionEnable( UINT16 GrpIdx1,	UINT16 GrpIdx2 = 0xFFFF, bool bFlag = true);
	bool					getGroupCollisionEnable( UINT16 GrpIdx1, UINT16 GrpIdx2)			
	{	
		if( m_pSDK == NULL)		return false;
		return m_pScene->getGroupCollisionFlag( GrpIdx1, GrpIdx2); 
	}
	UINT32					getCollidableDestMask( INT32 group);

	void					setFilterOps( NxFilterOp op0, NxFilterOp op1, NxFilterOp op2)
	{
		I3ASSERT( (m_idxFilter >= 0) && (m_idxFilter < MAX_FILTER_STACK));

		m_FilterCtx[ m_idxFilter].op0 = op0;
		m_FilterCtx[ m_idxFilter].op1 = op1;
		m_FilterCtx[ m_idxFilter].op2 = op2;

		m_pScene->setFilterOps( op0, op1, op2);
	}

	void					setFilterBool( bool flag)
	{ 
		I3ASSERT( (m_idxFilter >= 0) && (m_idxFilter < MAX_FILTER_STACK));

		m_FilterCtx[ m_idxFilter].result = flag;

		m_pScene->setFilterBool( flag); 
	}

	void					setFilterConstant( UINT32	mask1, UINT32 mask2)
	{
		NxGroupsMask m;

		if( m_pSDK == NULL)		return;

		I3ASSERT( (m_idxFilter >= 0) && (m_idxFilter < MAX_FILTER_STACK));

		m_FilterCtx[ m_idxFilter].mask0 = mask1;
		m_FilterCtx[ m_idxFilter].mask1 = mask2;

		m.bits1 = 0;
		m.bits2 = 0;
		m.bits3 = 0;

		m.bits0 = mask1;
		m_pScene->setFilterConstant0( m);

		m.bits0 = mask2;
		m_pScene->setFilterConstant1( m);
	}

	void					pushFilter(void)
	{
		m_idxFilter ++;

		I3ASSERT( (m_idxFilter >= 0) && (m_idxFilter < MAX_FILTER_STACK));
	}

	void					popFilter(void)
	{
		m_idxFilter--;

		I3ASSERT( (m_idxFilter >= 0) && (m_idxFilter < MAX_FILTER_STACK));

		NxGroupsMask m;
		I3_PHYSIX_FILTER_CTX * pCtx = & m_FilterCtx[ m_idxFilter];

		m_pScene->setFilterOps( pCtx->op0, pCtx->op1, pCtx->op2);
		m_pScene->setFilterBool( pCtx->result);

		m.bits1 = 0;
		m.bits2 = 0;
		m.bits3 = 0;

		m.bits0 = pCtx->mask0;
		m_pScene->setFilterConstant0( m);

		m.bits0 = pCtx->mask1;
		m_pScene->setFilterConstant1( m);
	}

	I3_PHYSIX_HIT_RESULT *	RaycastClosest( i3CollideeLine * pLine, INT32 grpIndex, UINT32 mask = 0xFFFFFFFF);
	UINT32					Raycast( i3CollideeLine * pLine, INT32 grpIndex, UINT32 mask = 0xFFFFFFFF);

	I3_PHYSIX_HIT_RESULT *	OverlapSphereClosest( i3CollideeSphere * pSphere, INT32 grpIndex);
	UINT32					OverlapSphere( i3CollideeSphere * pSphere, INT32 grpIndex);

	#if defined( I3_DEBUG)
		void				Dump( bool bDumpActor = false);
		void				DumpPairs(void);

		void				LogDebugRenderable(void);
	#endif

		void				PollForWork(void)				{ m_pScene->pollForWork( NX_WAIT_SIMULATION_END ); }

		void				CMD_updateDynamicState( i3PhysixShapeSet * pShapeSet);
		void				CMD_updateGlobalPose( i3PhysixShapeSet * pShapeSet, MATRIX * pMtx);
		void				CMD_setLinearVelocity( i3PhysixShapeSet * pShapeSet, VEC3D * pLinVel);
		void				CMD_setAngularVelocity( i3PhysixShapeSet * pShapeSet, VEC3D * pLinVel);
		void				CMD_setDimension( NxShape * pShape, VEC3D * pDimension);

		void CheckInvalidRealValue(I3_PHYSIX_CMD_INFO * pInfo, REAL32& rValue);
};

#endif

#endif
