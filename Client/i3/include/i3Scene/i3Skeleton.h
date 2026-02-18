#if !defined( __I3_SKELETON_H)
#define __I3_SKELETON_H

#include "i3Bone.h"
#include "i3BoneMatrixListAttr.h"
#include "i3AnimationContext.h"
#include "i3PhysixShapeSet.h"

/** \defgroup I3_SKELFLAG*/
//@{
typedef UINT32 I3_SKELFLAG;

#define I3_SKELFLAG_IK_ENABLE		0x00000001		///< IK 사용
#define I3_SKELFLAG_IK_USELIMIT		0x00000002		///< IK의 Rotate Limit를 사용
#define I3_SKELFLAG_IK_BLEND		0x00000004		///< frame간에 blend를 한다. (animation과 blend할 시 필요)

#define I3_SKELFLAG_NO_TRANSLATION	0x00010000		///< Rotation값으로만 Animation을 한다. (translation은 제외시킨다)
#define I3_SKELFLAG_USE_ORIENTATION	0x00020000		///< Local 축을 맞춘다.
#define I3_SKELFLAG_USE_MOTION		0x00040000		///< 외부 Matrix로 Animation을 처리한다. (motion data에 buffering)
//@}

class i3SceneTracer;
class i3Skeleton;

/** \brief
	\desc leaf class */

class I3_EXPORT_SCENE i3Skeleton : public i3PersistantElement
{
	I3_EXPORT_CLASS_DEFINE( i3Skeleton, i3PersistantElement);
	
protected:
	UINT32					m_dynamicState = (I3_PHYSIX_STATE_DISABLE & (~I3_PHYSIX_STATE_KINEMATIC));		// 대표
	i3BoneMatrixListAttr *	m_pBoneListAttr = nullptr;

	I3_SKELFLAG				m_nSkelFlag = 0;

	// Volatile members
	UINT32					m_lastFrameID = 0xFFFFFFFF;
	i3::vector_map<i3::string, INT32>	m_BoneMap;


public:
	INT32			getBoneCount(void)							{ return m_pBoneListAttr->getBoneCount(); }
	i3Bone *		getBone( INT32 idx)							{ return m_pBoneListAttr->getBone( idx); }

	virtual INT32	getParentBoneIndex( INT32 idx)				{ return m_pBoneListAttr->getParentBoneIndex( idx); }
	virtual void	setParentBoneIndex( INT32 idx, INT32 parentIdx){ m_pBoneListAttr->setParentBoneIndex( idx, parentIdx);}

	virtual MATRIX *getMatrix( INT32 idx)						{ return m_pBoneListAttr->getMatrix( idx); }
	virtual void	setMatrix( INT32 idx, MATRIX * pMtx)		{ m_pBoneListAttr->setMatrix( idx, pMtx); }

	virtual void	GetFullMatrix( INT32 idx, MATRIX * pMtx);

	virtual MATRIX *getInverseMatrix( INT32 idx)				{ return m_pBoneListAttr->getInverseMatrix( idx); }
	virtual void	setInverseMatrix( INT32 idx, MATRIX * pMtx)	{ m_pBoneListAttr->setInverseMatrix( idx, pMtx); }

	virtual UINT32	getDepth( UINT32 idx) const					{ return m_pBoneListAttr->getDepth( idx); }
	virtual void	setDepth( UINT32 idx, UINT32 depth)			{ m_pBoneListAttr->setDepth( idx, depth); }

	const char *	getName( INT32 idx)	const					{ return m_pBoneListAttr->getBoneName( idx); }
	const i3::rc_string& getNameString(INT32 idx) const			{ return m_pBoneListAttr->getBoneNameString(idx); }

	i3::vector_map<i3::string, INT32> *	getBoneMapString( void)	{ return &m_BoneMap; }

	void			setName( INT32 idx, const i3::rc_string& strName)	{ m_pBoneListAttr->setBoneName( idx, strName); }

	i3BoneMatrixListAttr *	getBoneMatrixListAttr(void)			{ return m_pBoneListAttr; }
	void					setBoneMatrixListAttr( i3BoneMatrixListAttr * pAttr);

	UINT32			getLastUpdateFrameID(void)					{ return m_lastFrameID; }
	void			setLastUpdateFrameID( UINT32 id)			{ m_lastFrameID = id; }

	/** \brief Flag를 반환한다.
		\return I3_SKELFLAG */
	I3_SKELFLAG		getSkelFlag( void)							{ return m_nSkelFlag; }

	/** \brief Flag를 설정한다.
		\param[in] nFlag I3_SKELFLAG
		\return N/A */
	void			setSkelFlag( I3_SKELFLAG nFlag)				{ m_nSkelFlag = nFlag; }

	/** \brief Flag를 추가한다.
		\param[in] nFlag I3_SKELFLAG
		\return N/A */
	void			addSkelFlag( I3_SKELFLAG nFlag)				{ m_nSkelFlag |= nFlag; }

	/** \brief Flag를 제거한다.
		\param[in] nFlag I3_SKELFLAG
		\return N/A */
	void			removeSkelFlag( I3_SKELFLAG nFlag)			{ m_nSkelFlag &= ~nFlag; } 

public:
	virtual ~i3Skeleton(void);

	virtual void	SetBoneCount( INT32 count);

	INT32			FindBoneByName( const char * pszName);

	/** \brief Update를 한다.
		\param[in] pTracer i3SceneTracer 객체 포인터
		\param[in] pCtx i3AnimationContext 객체 포인터
		\param[in] pBoneMap Bone Map
		\return N/A */
	virtual void	Traverse( i3SceneTracer * pTracer, i3AnimationContext * pCtx, INT32 * pBoneMap);

	/** \brief Animation을 Update한다.
		\param[in] idFrame Frame ID
		\param[in] tm Delta time
		\param[in] pCtx i3AnimationContext 객체 포인터
		\param[in] pBoneMap Bone Map
		\return N/A */
	virtual void	UpdateAnim( UINT32 idFrame, REAL32 tm, i3AnimationContext * pCtx, INT32 * pBoneMap);

	/** \brief World좌표로 Transform한다.
		\param[in] pTracer i3SceneTracer 객체 포인터
		\param[in] pCtx i3AnimationContext 객체 포인터
		\param[in] pBoneMap Bone Map
		\return N/A */
	void			TransformWorldSpace( i3SceneTracer * pTracer, i3AnimationContext * pCtx, INT32 * pBoneMap);

	bool			IsSame( i3Skeleton * pSkel);

	void			CreateJoints(void);

	/** \brief Local Matrix를 만든다.
		\param[in] pTracer i3SceneTracer 객체 포인터
		\param[in] pAnimCtx i3AnimationContext 객체 포인터
		\param[in] pBoneMap Bone Map
		\return N/A */
	void			BuildTransformedMatrix( i3SceneTracer * pTracer, i3AnimationContext * pAnimCtx, INT32 * pBoneMap);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual	void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	void			OnInstancing( I3_ONINSTANCING_INFO * pInfo);

	void			UpdateBoneMatrixListAttrInfo();


#if defined (I3_PHYSX)
protected:
	i3PhysixShapeSet **		m_ppShapeSet = nullptr;
	UINT8 *					m_pInitializeForce = nullptr;

	void			_CalcTransformToPhysix( i3PhysixShapeSet * pShapeSet, INT32 idx );

public:
	void			setShapeSet( INT32 boneIdx, i3PhysixShapeSet * pShapeSet);
	i3PhysixShapeSet *	getShapeSet( INT32 boneIdx)				{ return m_ppShapeSet[ boneIdx]; }
	bool			isShapeSet( INT32 boneIdx)					{ return (m_ppShapeSet != nullptr) && (m_ppShapeSet[boneIdx] != nullptr); }

	void			setDynamicState( UINT32 state, i3PhysixShapeSet * pShapeSet );
	void			setDynamicState( UINT32 state,  bool bShapeSet = true);
	void			modifyDynamicState( UINT32 addState, UINT32 removeState);
	bool			isDynamicState( UINT32 mask)				{ return (m_dynamicState & mask) == mask; }
	bool			isDynamic( void)							{ return (m_ppShapeSet != nullptr) && (isDynamicState( I3_PHYSIX_STATE_KINEMATIC) == false); }

	void			setBoneLinearVelocity( INT32 boneIdx, VEC3D * pVec )
	{
		if( m_ppShapeSet == nullptr ) return;
		I3ASSERT( boneIdx < getBoneCount() );
		m_ppShapeSet[boneIdx]->setFBLinearVelocity( pVec );
	}
	void			addBoneLinearVelocity( INT32 boneIdx, VEC3D * pVec )
	{	
		if( m_ppShapeSet == nullptr ) return;
		I3ASSERT( boneIdx < getBoneCount() );
		m_ppShapeSet[boneIdx]->addFBLinearVelocity( pVec );
	}

	void			setBoneAngularVelocity( INT32 boneIdx, VEC3D * pVec )
	{
		if( m_ppShapeSet == nullptr ) return;
		I3ASSERT( boneIdx < getBoneCount() );
		m_ppShapeSet[boneIdx]->setFBAngularVelocity( pVec );
	}
	void			addBoneAngularVelocity( INT32 boneIdx, VEC3D * pVec )
	{
		if( m_ppShapeSet == nullptr ) return;
		I3ASSERT( boneIdx < getBoneCount() );
		m_ppShapeSet[boneIdx]->addFBAngularVelocity( pVec );
	}

	void			setInitForceFeed( INT32 boneidx, UINT8 nValue )
	{
		I3ASSERT( boneidx < getBoneCount() );
		m_pInitializeForce[ boneidx ] = nValue;
	}
	bool			checkInitForceFeed( INT32 boneidx )
	{
		I3ASSERT( boneidx < getBoneCount() );
		if( m_pInitializeForce[ boneidx ] )		return true;
		else									return false;
	}
#endif

#if defined( I3_DEBUG)
	void			Dump(void);
#endif
};

#endif
