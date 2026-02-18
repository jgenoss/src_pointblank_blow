#if !defined( __I3_PHYSIX_SHAPE_SET_H)
#define __I3_PHYSIX_SHAPE_SET_H

#if defined ( I3_PHYSX)

#include "i3Node.h"
#include "i3Collidee.h"
#include "i3PhysixDefine.h"
#include "i3PhysixJointInfo.h"

#define I3PHYSIX_CONTACT_FLAG_NOTUSE			1
#define I3PHYSIX_CONTACT_FLAG_NONORMAL			2
#define I3PHYSIX_CONTACT_FLAG_NOINTERSECT		4

#define I3PHYSIX_CONTACT_REPORT_COUNT			32

class I3_EXPORT_SCENE i3PhysixShapeSet : public i3EventReceiver
{
	I3_CLASS_DEFINE( i3PhysixShapeSet);
 
	friend class i3PhysixContext;

protected:
	// Persistent Members
	I3_PHYSIX_SHAPE_TYPE		m_ShapeType;
	UINT32						m_dynamicState;
	i3EventReceiver *			m_pReceiver;

	INT32						m_Usage;

	NxActor *					m_pNxActor;
	NxJoint *					m_pNxJoint;

	i3MemoryBuffer *			m_pTerrainArray;
	INT32						m_TerrainCount;

	i3MemoryBuffer *			m_pFaceArray;
	UINT32						m_FaceCount;

	i3Vector3Array *			m_pPosArray;

	i3PhysixJointInfo *			m_pJointInfo;

	UINT16						m_ShapeGroup;

	REAL32						m_Restitution;
	REAL32						m_StaticFriction;
	REAL32						m_DynamicFriction;

	VEC3D						m_vFBLinVelocity;			// 추가 힘에 대한 값
	VEC3D						m_vFBAngVelocity;			//
	REAL32						m_rFxForce;					// Animation to PhysixForce variable

	VEC3D						m_vInertia;

	i3MemoryBuffer *			m_pNxData;
	INT32						m_NxDataSize;

	INT32						m_nVersion;

	// Collection data를 버퍼에 넣도록 합니다. 실제 게임에서 인스턴스를 복사하는 경우 여기에서 복사를 합니다.
	i3MemoryBuffer *			m_pCollectionData;
	INT32						m_nCollectionSize;

	// Contact report
	UINT32						m_ContactReportFlags;
	I3_PHYSIX_CONTACT_REPORT	m_ContactReports[ I3PHYSIX_CONTACT_REPORT_COUNT ];
	i3List						m_ContactList;
	INT32						m_nContactCount;

	VEC3D						m_vOriginalSize;

	void *						m_pUserData;

public:
	VEC3D *						m_pvInitialPos;

protected:
	virtual void _GetWrappingBound( I3_GETBOUND_INFO * pInfo);

	void	*				_BuildTriMeshActor(void);
	void	*				_BuildConvexMeshActor(void);

	void					_UpdateDynamicState(void);

public:

	I3_PHYSIX_SHAPE_TYPE	getShapeType(void)								{ return m_ShapeType; }
	void					setShapeType( I3_PHYSIX_SHAPE_TYPE type)		{ m_ShapeType = type; }

	INT32					getUsage( void )								{ return m_Usage; }
	void					setUsage( INT32 usage)							{ m_Usage = usage; }
	
	INT32					getContactCount( void )							{ return m_nContactCount; }
	I3_PHYSIX_CONTACT_REPORT * getContactReport( INT32 idx )				{ return &m_ContactReports[ idx ]; }
	BOOL					addContactReport( I3_PHYSIX_CONTACT_REPORT * pReport )
	{
		if( m_nContactCount >= I3PHYSIX_CONTACT_REPORT_COUNT )	return FALSE;
		i3Physix::ContactReportCopyTo( &m_ContactReports[ m_nContactCount ], pReport );
		m_nContactCount++;
		return TRUE;
	}
	void					clearContactReport( void )						{ m_nContactCount = 0; }

	REAL32					getRestitution(void)							{ return m_Restitution; }
	void					setRestitution( REAL32 val)						{ m_Restitution = val; }

	REAL32					getStaticFriction(void)							{ return m_StaticFriction; }
	REAL32					getDynamicFriction(void)						{ return m_DynamicFriction; }
	void					setFriction( REAL32 sf, REAL32 df)
	{
		m_StaticFriction = sf;
		m_DynamicFriction = df;
	}

	bool					isDynamicActor(void)							{ if( m_pNxActor == NULL ) return false; return m_pNxActor->isDynamic(); }

	NxActor *				getNxActor(void)								{ return m_pNxActor; }
	void					setNxActor( NxActor * pActor)					{ m_pNxActor = pActor; }

	void					setGroup( UINT16 grp)							{ if( m_pNxActor == NULL ) return; m_pNxActor->setGroup( grp); }
	UINT16					getGroup(void)									{ if( m_pNxActor == NULL ) return 0xFFFF; return m_pNxActor->getGroup(); }

	NxJoint *				getNxJoint(void)								{ return m_pNxJoint; }

	INT32					getNxShapeCount(void)							{ if( m_pNxActor == NULL ) return 0; return m_pNxActor->getNbShapes(); }
	NxShape *				getNxShape( INT32 idx)							{ if( m_pNxActor == NULL ) return NULL; return m_pNxActor->getShapes()[ idx]; }

	UINT32					getFaceCount(void)								{ return m_FaceCount; }
	void					setFaceCount( UINT32 cnt)						{ m_FaceCount = cnt; }

	i3MemoryBuffer *		getFaceArray(void)								{ return m_pFaceArray; }
	void					setFaceArray( i3MemoryBuffer * pArray)			{ I3_REF_CHANGE( m_pFaceArray, pArray); }
	void					CopyFaceArray( I3_PHYSIX_TRI * pMaterialIndex, UINT32 count);

	I3_PHYSIX_TRI *			getFace( INT32 faceID)							{ if(m_pFaceArray == NULL) return NULL; return (I3_PHYSIX_TRI *) (((I3_PHYSIX_TRI *) m_pFaceArray->getBuffer()) + faceID); }

	INT32					getTerrainCount(void)							{ return m_TerrainCount; }
	void					setTerrainCount( UINT32 cnt)					{ m_TerrainCount = cnt; }

	I3_PHYSIX_TERRAIN *		getTerrain( INT32 idx)							{ I3ASSERT( idx < (INT32)m_TerrainCount); return ((I3_PHYSIX_TERRAIN *) m_pTerrainArray->getBuffer()) + idx; }
	void					setTerrainArray( i3MemoryBuffer * pArray)		{ I3_REF_CHANGE( m_pTerrainArray, pArray); }
	i3MemoryBuffer *		getTerrainArray(void)							{ return m_pTerrainArray; }

	i3Vector3Array*			getPositionArray(void)							{ return m_pPosArray; }
	void					setPositionArray( i3Vector3Array * pVec)		{ I3_REF_CHANGE( m_pPosArray, pVec); }

	i3PhysixJointInfo *		getJointInfo(void)								{ return m_pJointInfo; }
	void					setJointInfo( i3PhysixJointInfo * pInfo)		{ I3_REF_CHANGE( m_pJointInfo, pInfo); }

	i3EventReceiver	*		getEventReceiver(void)							{ return m_pReceiver; }
	void					setEventReceiver( i3EventReceiver * pObj);

	void					setShapeGroup( UINT16 grp);
	void					setShapeGroupMask( UINT32 mask);
	UINT16					getShapeGroup(void)								{ return m_ShapeGroup; }

	void					getShapeGlobalPose( MATRIX * pMtx);
	void					setShapeGlobalPose( MATRIX * pMtx);

	void					getShapeMassCenterPos( VEC3D * pPos )
	{
		if( m_pNxActor == NULL )	return;
		NxVec3 vec = m_pNxActor->getCMassGlobalPosition();
		i3Vector::Set( pPos, vec.x, vec.y, vec.z );
	}

	void					getMassLocalPose( VEC3D * pOut )
	{
		if( m_pNxActor == NULL )	return;
		NxVec3 vec = m_pNxActor->getCMassLocalPosition();
		i3Vector::Set( pOut, vec.x, vec.y, vec.z );
	}
	void					setMassLocalPose( VEC3D * pPos )
	{
		if( m_pNxActor == NULL )	return;
		NxVec3 vec( getX(pPos), getY(pPos), getZ(pPos) );
		m_pNxActor->setCMassOffsetLocalPosition( vec );
	}

	void					getMassLocalPose( MATRIX * pMtx )
	{
		if( m_pNxActor != NULL )
		{
			NxMat34 mat = m_pNxActor->getCMassGlobalPose();
			NXU::Copy( pMtx, &mat );
		}
		else
			i3Matrix::Identity( pMtx );
	}
	void					setMassLocalPose( MATRIX * pMtx )
	{
		if( m_pNxActor != NULL )
		{	
			m_pNxActor->setCMassOffsetLocalPose( (NxMat34 &) pMtx );
		}
	}

	void					setSleeping( BOOL bValue )
	{
		if( m_pNxActor == NULL )	return;
		if( m_pNxActor->isDynamic() )
		{
			if( bValue )
			{
				m_pNxActor->putToSleep();
			}
			else
				m_pNxActor->wakeUp();
		}
	}

	REAL32					getEnergy()
	{
		if( m_pNxActor == NULL )	return 0.f;
		return (REAL32) m_pNxActor->computeKineticEnergy();
	}

	void					getInertiaTensor( VEC3D *pVecOut )
	{
		if( m_pNxActor == NULL )	return;
		NxVec3 vec = m_pNxActor->getMassSpaceInertiaTensor();
		i3Vector::Set( pVecOut, vec.x, vec.y, vec.z );
	}
	void					setInertiaTensor( VEC3D *pVec )
	{
		if( m_pNxActor == NULL )	return;
		NxVec3 vec( getX( pVec ), getY( pVec ), getZ( pVec ) );
		m_pNxActor->setMassSpaceInertiaTensor( vec );
	}

	REAL32					getLinearDamping( void )
	{
		if( m_pNxActor == NULL )	return 0.f;
		return (REAL32) m_pNxActor->getLinearDamping();
	}
	void					setLinearDamping( REAL32 linDamp )
	{
		if( m_pNxActor == NULL )	return;
		m_pNxActor->setLinearDamping( linDamp );
	}

	void					getLinearMomentum( VEC3D * pVecOut )
	{
		if( m_pNxActor == NULL )	return;
		NxVec3 vec = m_pNxActor->getLinearMomentum();
		i3Vector::Set( pVecOut, vec.x, vec.y, vec.z );
	}
	void					setLinearMomentum( VEC3D * pVec )
	{
		if( m_pNxActor == NULL )	return;
		if( m_pNxActor->isDynamic() && !m_pNxActor->readBodyFlag( NX_BF_KINEMATIC ) )
		{
			NxVec3 vec( getX( pVec ), getY( pVec ), getZ( pVec ) );
			m_pNxActor->setLinearMomentum( vec );
		}
	}

	void					getLinearVelocity( VEC3D * pVecOut )
	{
		if( m_pNxActor == NULL )	return;
		if( m_pNxActor->isDynamic() )// && !m_pNxActor->readBodyFlag( NX_BF_KINEMATIC ) )
		{
			NxVec3 vec = m_pNxActor->getLinearVelocity();
			i3Vector::Set( pVecOut, vec.x, vec.y, vec.z );
		}
	}
	void					setLinearVelocity( VEC3D * pVec );

	VEC3D *					getFBLinearVelocity( void )						{ return &m_vFBLinVelocity; }
	void					setFBLinearVelocity( VEC3D * pVec )				{ i3Vector::Copy( &m_vFBLinVelocity, pVec ); }
	void					addFBLinearVelocity( VEC3D * pVec )				{ i3Vector::Add( &m_vFBLinVelocity, &m_vFBLinVelocity, pVec ); }

	REAL32					getAngularDamping( void )
	{
		if( m_pNxActor == NULL )	return 0.f;
		return (REAL32) m_pNxActor->getAngularDamping();
	}
	void					setAngularDamping( REAL32 angDamp )
	{
		if( m_pNxActor == NULL )	return;
		m_pNxActor->setAngularDamping( angDamp );
	}

	void					getAngularMomentum( VEC3D * pVecOut )
	{
		if( m_pNxActor == NULL )	return;
		NxVec3 vec = m_pNxActor->getAngularMomentum();
		i3Vector::Set( pVecOut, vec.x, vec.y, vec.z );
	}
	void					setAngularMomentum( VEC3D * pVec )
	{
		if( m_pNxActor == NULL )	return;
		if( m_pNxActor->isDynamic() && !m_pNxActor->readBodyFlag( NX_BF_KINEMATIC ) )
		{
			NxVec3 vec( getX( pVec ), getY( pVec ), getZ( pVec ) );
			m_pNxActor->setAngularMomentum( vec );
		}
	}

	void					getAngularVelocity( VEC3D * pVecOut )
	{
		if( m_pNxActor == NULL )	return;
		if( m_pNxActor->isDynamic() )
		{
			NxVec3 vec = m_pNxActor->getAngularVelocity();
			i3Vector::Set( pVecOut, vec.x, vec.y, vec.z );
		}
	}
	void					setAngularVelocity( VEC3D * pVec );

	VEC3D *					getFBAngularVelocity( void )					{ return &m_vFBAngVelocity; }
	void					setFBAngularVelocity( VEC3D * pVec )			{ i3Vector::Copy( &m_vFBAngVelocity, pVec );	}
	void					addFBAngularVelocity( VEC3D * pVec )			{ i3Vector::Add( &m_vFBAngVelocity, &m_vFBAngVelocity, pVec );	}

	REAL32					getSleepLinearVelocity( void )
	{
		if( m_pNxActor == NULL )	return 0.f;
		return m_pNxActor->getSleepLinearVelocity();
	}
	void					setSleepLinearVelocity( REAL32 rVel )
	{
		if( m_pNxActor == NULL )	return;
		m_pNxActor->setSleepLinearVelocity( rVel);
	}

	REAL32					getSleepAngularVelocity( void )
	{
		if( m_pNxActor == NULL )	return 0.f;
		return m_pNxActor->getSleepAngularVelocity();
	}
	void					setSleepAngularVelocity( REAL32 rVel )
	{
		if( m_pNxActor == NULL )	return;
		m_pNxActor->setSleepAngularVelocity( rVel);
	}

	REAL32					getMass( void )
	{
		if( m_pNxActor == NULL )	return 0.f;
		return (REAL32) m_pNxActor->getMass();
	}
	void					setMass( REAL32 fMass )
	{
		if( m_pNxActor == NULL )	return ;
		m_pNxActor->setMass( fMass );
	}

	REAL32					getSkinWidth( INT32 idx )
	{
		if( idx < getNxShapeCount() )
		{
			return getNxShape( idx)->getSkinWidth();
		}
		else
			return 0.f;
	}
	void					setSkinWidth( REAL32 rSize)
	{
		for( INT32 i = 0; i < getNxShapeCount(); i++ )
		{
			getNxShape( i )->setSkinWidth( rSize);
		}
	}

	void					addTorque( VEC3D * pVec )
	{
		if( m_pNxActor == NULL )	return;
		if( m_pNxActor->isDynamic() && !m_pNxActor->readBodyFlag( NX_BF_KINEMATIC ) )
		{
			NxVec3 vec( getX( pVec ) + getX( &m_vFBAngVelocity ), getY( pVec ) + getY( &m_vFBAngVelocity ), getZ( pVec ) + getZ( &m_vFBAngVelocity ) );
			m_pNxActor->addTorque( vec );
		}
	}

	void					addForce( VEC3D * pVec)
	{
		if( m_pNxActor == NULL)	return;
		if( m_pNxActor->isDynamic() && !m_pNxActor->readBodyFlag( NX_BF_KINEMATIC))
		{
			NxVec3 vec( getX( pVec ), getY( pVec ), getZ( pVec ));
			m_pNxActor->addForce( vec );
		}
	}

	REAL32					getFxForce( void )								{ return m_rFxForce; }
	void					setFxForce( REAL32 rForce )						{ m_rFxForce = rForce; }

	UINT32					getDynamicState(void)							{ return m_dynamicState; }
	void					setDynamicState( UINT32 state);
	bool					isDynamicState( UINT32 state)					{ return (m_dynamicState & state) == state; }
	void					modifyDynamicState( UINT32 addState, UINT32 removeState);

	BOOL					IsUsingContactReport( void )					{ return (BOOL) !(m_ContactReportFlags & I3PHYSIX_CONTACT_FLAG_NOTUSE); }
	void					EnableUsingContactReport( BOOL bVal )
	{
		if( bVal )
			m_ContactReportFlags &= ~I3PHYSIX_CONTACT_FLAG_NOTUSE;
		else
			m_ContactReportFlags |= I3PHYSIX_CONTACT_FLAG_NOTUSE;
	}

	VEC3D *					getOriginalSize( void)							{ return &m_vOriginalSize; }
	void					setOriginalSize( VEC3D * pSize)					{ i3Vector::Copy( &m_vOriginalSize, pSize); }

public:
	i3PhysixShapeSet(void);
	virtual ~i3PhysixShapeSet(void);

	void *			getUserData(void)										{ return m_pUserData; }
	void			setUserData( void * pObj)								{ m_pUserData = pObj; }

	void			Create( NxActorDesc * pDesc = NULL, bool bUseTool = false );
	void			CreateJoint( i3PhysixShapeSet * pParent);

	void			DestroyActor( void );
	void			DestroyJoint( void );

	void			CreateSphereShape( VEC3D * pCenterOff, REAL32 radius, INT32 group);
	void			CreateBoxShape( VEC3D * pCenter, VEC3D * pSize, INT32 group, bool bBody, MATRIX * pGlobalPose = NULL, I3_TERRAIN_TYPE terrain = I3_TERRAIN_CONCRETE);
	void			CreateBoxShapeCCD( VEC3D * pCenter, VEC3D * pSize, INT32 group, bool bBody, MATRIX * pGlobalPose = NULL, I3_TERRAIN_TYPE terrain = I3_TERRAIN_CONCRETE);
	void			CreatePlaneShape( VEC3D * pNormal, REAL32 rDistance, INT32 group, bool bBody );

	void			Reset(void);
	
	// 물량을 다른 Shape에 설정합니다.
	void			PhysixCopyToShape( i3PhysixShapeSet * pDestShape );

	static void		Dump( NxActorDesc * pActorDesc);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
	void	OnInstancing( VEC3D * pInitPos = NULL);

	// Convex용 TerrainArray 작성
	// Shape의 갯수 Terrain 갯수가 같게 됩니다.
	void			PrepareTerrainArrayForConvex(void);

	NxCCDSkeleton* CreateCCDSkeleton(REAL32 rX, REAL32 rY, REAL32 rZ);
};

#endif

#endif
