#if !defined( __I3_SKELETON_H)
#define __I3_SKELETON_H

#include "i3Bone.h"
#include "i3BoneMatrixListAttr.h"
#include "i3AnimationContext.h"
#include "i3PhysixShapeSet.h"

class i3SceneTracer;

class I3_EXPORT_SCENE i3Skeleton : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3Skeleton);
	
protected:
	UINT32						m_dynamicState;		// ´ëÇ¥
	i3BoneMatrixListAttr *		m_pBoneListAttr;
	UINT8 *						m_pInitializeForce;
#if defined (I3_PHYSX)
	i3PhysixShapeSet **			m_ppShapeSet;
#endif

	UINT32						m_lastFrameID;

protected:

#if defined (I3_PHYSX)
	void				_CalcTransformToPhysix( i3PhysixShapeSet * pShapeSet, INT32 idx );
#endif

public:
	INT32				getBoneCount(void)							{ return m_pBoneListAttr->getBoneCount(); }
	void				SetBoneCount( INT32 count);
	i3Bone *			getBone( INT32 idx)							{ return m_pBoneListAttr->getBone( idx); }

	INT32				getParentBoneIndex( INT32 idx)				{ return m_pBoneListAttr->getParentBoneIndex( idx); }
	void				setParentBoneIndex( INT32 idx, INT32 parentIdx)
	{
		m_pBoneListAttr->setParentBoneIndex( idx, parentIdx);
	}

	MATRIX *			getMatrix( INT32 idx)						{ return m_pBoneListAttr->getMatrix( idx); }
	void				setMatrix( INT32 idx, MATRIX * pMtx)		{ m_pBoneListAttr->setMatrix( idx, pMtx); }

	void				GetFullMatrix( INT32 idx, MATRIX * pMtx);

	MATRIX *			getInverseMatrix( INT32 idx)				{ return m_pBoneListAttr->getInverseMatrix( idx); }
	void				setInverseMatrix( INT32 idx, MATRIX * pMtx)	{ m_pBoneListAttr->setInverseMatrix( idx, pMtx); }

	UINT32				getDepth( UINT32 idx)						{ return m_pBoneListAttr->getDepth( idx); }
	void				setDepth( UINT32 idx, UINT32 depth)			{ m_pBoneListAttr->setDepth( idx, depth); }

	char *				getName( INT32 idx)							{ return m_pBoneListAttr->getBoneName( idx); }
	void				setName( INT32 idx, char * pszName)			{ m_pBoneListAttr->setBoneName( idx, pszName); }

#if defined (I3_PHYSX)
	void				setShapeSet( INT32 boneIdx, i3PhysixShapeSet * pShapeSet);
	i3PhysixShapeSet *	getShapeSet( INT32 boneIdx)					{ return m_ppShapeSet[ boneIdx]; }
	bool				isShapeSet( INT32 boneIdx)					{ return (m_ppShapeSet != NULL) && (m_ppShapeSet[boneIdx] != NULL); }
#endif

	i3BoneMatrixListAttr *	getBoneMatrixListAttr(void)				{ return m_pBoneListAttr; }
	void					setBoneMatrixListAttr( i3BoneMatrixListAttr * pAttr)
	{
		I3_REF_CHANGE( m_pBoneListAttr, pAttr);
	}

#if defined (I3_PHYSX)
	void				setDynamicState( UINT32 state, i3PhysixShapeSet * pShapeSet );
	void				setDynamicState( UINT32 state,  bool bShapeSet = true);
	void				modifyDynamicState( UINT32 addState, UINT32 removeState);
	bool				isDynamicState( UINT32 mask)				{ return (m_dynamicState & mask) == mask; }
	bool				isDynamic( void)							{ return (m_ppShapeSet != NULL) && (isDynamicState( I3_PHYSIX_STATE_KINEMATIC) == false); }

	void				setBoneLinearVelocity( INT32 boneIdx, VEC3D * pVec )
	{
		if( m_ppShapeSet == NULL ) return;
		I3ASSERT( boneIdx < getBoneCount() );
		m_ppShapeSet[boneIdx]->setFBLinearVelocity( pVec );
	}
	void				addBoneLinearVelocity( INT32 boneIdx, VEC3D * pVec )
	{	
		if( m_ppShapeSet == NULL ) return;
		I3ASSERT( boneIdx < getBoneCount() );
		m_ppShapeSet[boneIdx]->addFBLinearVelocity( pVec );
	}

	void				setBoneAngularVelocity( INT32 boneIdx, VEC3D * pVec )
	{
		if( m_ppShapeSet == NULL ) return;
		I3ASSERT( boneIdx < getBoneCount() );
		m_ppShapeSet[boneIdx]->setFBAngularVelocity( pVec );
	}
	void				addBoneAngularVelocity( INT32 boneIdx, VEC3D * pVec )
	{
		if( m_ppShapeSet == NULL ) return;
		I3ASSERT( boneIdx < getBoneCount() );
		m_ppShapeSet[boneIdx]->addFBAngularVelocity( pVec );
	}
#endif

	void				setInitForceFeed( INT32 boneidx, UINT8 nValue )
	{
		I3ASSERT( boneidx < getBoneCount() );
		m_pInitializeForce[ boneidx ] = nValue;
	}
	bool				checkInitForceFeed( INT32 boneidx )
	{
		I3ASSERT( boneidx < getBoneCount() );
		if( m_pInitializeForce[ boneidx ] )		return true;
		else									return false;
	}

	UINT32				getLastUpdateFrameID(void)						{ return m_lastFrameID; }
	void				setLastUpdateFrameID( UINT32 id)				{ m_lastFrameID = id; }

public:
	i3Skeleton(void);
	virtual ~i3Skeleton(void);

	INT32				FindBoneByName( char * pszName);

	void				Traverse( i3SceneTracer * pTracer, i3AnimationContext * pCtx, INT32 * pBoneMap);

	void				UpdateAnim( UINT32 idFrame, REAL32 tm, i3AnimationContext * pCtx, INT32 * pBoneMap);
	void				TransformWorldSpace( i3SceneTracer * pTracer, i3AnimationContext * pCtx, INT32 * pBoneMap);

	bool				IsSame( i3Skeleton * pSkel);

	void				CreateJoints(void);

	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual	void		OnBuildObjectList( i3List * pList);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
	void				OnInstancing( void * pUserDefault = NULL);
};

#endif
