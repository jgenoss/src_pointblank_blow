#if !defined( __I3_SKELETON2_H)
#define __I3_SKELETON2_H

#include "i3BoneMatrixListAttr.h"
#include "i3Skeleton.h"
#include "Animation/i3AnimationContext2.h"
#include "i3PhysixShapeSet.h"
#include "i3IKBoneInfo.h"
#include "i3IKContext.h"

/**
\file i3Skeleton2.h
	\author Swoongoo
	\date 2009-2-19
	\note @li m_pIKContextList가 NULL이면 IK Animation을 하지 않는다.
		@li 여러개의 IKContext를 갖을 수 있다.
		@li 하나의 IKContext에는 여러 Branch(i3Constraint로 control)를 갖을 수 있다.
		@li IKContext간에는 영향을 주면 안된다.	따라서 parent bone에 IK가 있다면 단일 IKContext로 연결된 IK Bone으로 처리해야 한다.
		@li	연결된 IK Bone의 사이에 freezing하여 고정시키는 기능이 있다.

		@li flow : Animation Update -> IK Solver -> AfterTraverseIKUpdate( endeffector부터 child bone에 animation을 업데이트한다.)
*/

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

// i3Skeleton::Traverse 대신 호출하는 Callback, if NULL이면 i3Skeleton::Traverse를 정상적으로 호출 */
typedef void (*I3_SKELETON_TRAVERSEPROC)( i3SceneTracer * pTracer, i3AnimationContext2 * pCtx, i3Skeleton * pSkel, INT32 * pBoneMap);

/** \brief IK 및 Animation 공유를 할 수 있도록 하는 class */
class I3_EXPORT_SCENE i3Skeleton2 : public i3Skeleton
{
	I3_CLASS_DEFINE( i3Skeleton2);
protected :

	// Volatile members
	I3_SKELETON_TRAVERSEPROC	m_pTraverse;

public:

	i3Bone2 *			getBone2( INT32 idx)		{ return (i3Bone2*) getBone( idx); }

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \fn	void i3Skeleton::setTraverse( I3_SKELETON_TRAVERSEPROC traverse)
	///
	/// \brief	Sets a traverse.
	///
	/// \author	
	/// \date	2011-12-08
	///
	/// \param	traverse	The traverse.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void				setTraverse( I3_SKELETON_TRAVERSEPROC traverse)		{ m_pTraverse = traverse; }

public:
	i3Skeleton2();
	virtual ~i3Skeleton2();

	virtual void		SetBoneCount( INT32 count);

	/** \brief Update를 한다.
		\param[in] pTracer i3SceneTracer 객체 포인터
		\param[in] pCtx i3AnimationContext 객체 포인터
		\param[in] pBoneMap Bone Map
		\return N/A */
	virtual void		Traverse( i3SceneTracer * pTracer, i3AnimationContext * pCtx, INT32 * pBoneMap);

	/** \brief Animation을 Update한다.
		\param[in] idFrame Frame ID
		\param[in] tm Delta time
		\param[in] pCtx i3AnimationContext 객체 포인터
		\param[in] pBoneMap Bone Map
		\return N/A */
	virtual void		UpdateAnim( UINT32 idFrame, REAL32 tm, i3AnimationContext * pCtx, INT32 * pBoneMap);

	/** \brief World좌표로 Transform한다.
		\param[in] pTracer i3SceneTracer 객체 포인터
		\param[in] pCtx i3AnimationContext 객체 포인터
		\param[in] pBoneMap Bone Map
		\return N/A */
	virtual void		TransformWorldSpace( i3SceneTracer * pTracer, i3AnimationContext * pCtx, INT32 * pBoneMap);

	/** \brief Bone구조가 같은지 확인
		\param[in] pSkel i3Skeleton2 객체 포인터
		\return true : 같다. false : 같지 않다. */
	virtual bool		IsSame( i3Skeleton * pSkel);

	/**
	\brief i3Skeleton2을 복사한다.
	\param[out] pObj 복사할 대상 i3Skeleton2 객체 포인터.
	\param[in] method 복사하는 방식
	\return N/A
	*/
	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);

	/**
	\brief 파일로 저장한다.
	\param[in] pResFile i3ResourceFile 객체 포인터
	\return 파일 사이즈
	*/
	virtual UINT32		OnSave( i3ResourceFile * pResFile);

	/**
	\brief 파일에서 읽어온다.
	\param[in] pResFile i3ResourceFile 객체 포인터
	\return 파일 사이즈
	*/
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);

	/** \brief Instancing으로 생성될 시에 초기화 작업을 한다.
		\param[in] pInfo I3_ONINSTANCING_INFO 구조체 포인터. 초기화 정보를 담고 있다.
		\return N/A	*/
	void				OnInstancing( I3_ONINSTANCING_INFO * pInfo);

	//////////////////////////////////////////////////////////////////
	//						IK Animation							//
	//////////////////////////////////////////////////////////////////
private:
	i3List *			m_pIKContextList;

public:
	/** \brief IK Context List를 반환한다.
		\return i3List 객체 포인터	*/
	i3List *			getIKContextList( void)						{ return m_pIKContextList; }

	/** \brief IK Context List를 설정한다.
		\note pList의 RefCount를 증가시킨다.
		\param[in] pList i3List 객체 포인터
		\return N/A */
	void				setIKContextList( i3List * pList)			{ I3_REF_CHANGE( m_pIKContextList, pList); }

	/** \brief 등록된 IK Context 개수를 반환한다.
		\return IK Context 개수 */
	INT32				getIKContextCount( void)					{ if( m_pIKContextList != NULL) return m_pIKContextList->GetCount(); return 0; }
	
	/** \brief IK Context를 반환한다.
		\param[in] idx IK Context Index
		\return i3IKContext 객체 포인터 */
	i3IKContext *		getIKContext( INT32 idx)					{ if( m_pIKContextList != NULL) return (i3IKContext*)m_pIKContextList->GetItem( idx); return NULL;}

	/** \brief IK Context를 설정한다.
		\param[in] idx IK Context Index
		\param[in] pIKContext i3IKContext 객체 포인터
		\return N/A */
	void				setIKContext( INT32 idx, i3IKContext * pIKContext)		{ if( m_pIKContextList != NULL){I3ASSERT( idx < m_pIKContextList->GetCount()); m_pIKContextList->SetItem( idx, pIKContext);} }
	
	/** \brief IK Context에서 IK Bone Info를 반환한다.
		\param[in] pIKCtx i3IKContext 객체 포인터
		\param[in] idx Bone Index
		\return i3IKBoneInfo 구조체 포인터 */
	i3IKBoneInfo *		getIKBoneInfoByBoneIdx( i3IKContext * pIKCtx, INT32 idx)
	{
		INT32 iIKInfoIdx = getBone2( idx)->getIKBoneInfoIndex();
		if( iIKInfoIdx != -1)
			return pIKCtx->getIKBoneInfo( iIKInfoIdx);
		return NULL;
	}

protected :
	//bool				_CheckSharingBone( i3IKBoneInfo * pBoneInfo, i3IKContext * pSrcIKCtx);
	//bool				_CheckIncludeBone( i3IKBoneInfo * pBoneInfo, i3IKContext * pSrcIKCtx);
	// return true : is branch,
	//bool				_MergeBranch( i3IKContext * pIKCtx, INT32 * pBoneMap );
	void				_IncreaseIKJointCountProcess( INT32 iBoneIdx, i3IKContext * pPrevCtx, i3IKBoneInfo * pSetInfo);

	void				_SetDefaultIKBoneInfo( i3IKBoneInfo * pOut, INT32 iBoneIdx);

public:
	/** \brief IK Bone Info를 만든다.
		\param[in] iStartBoneIndex Starter Bone Index
		\param[in] iEndEffectBoneIndex End Effector Bone Index
		\param[in] pBoneMap Bone Map
		\return N/A */
	void				BuildIKBoneInfo( INT32 iStartBoneIndex, INT32 iEndEffectBoneIndex);
	
	/** \brief IKAnimation을 한 후 SceneG 에 업데이트
		\details 하위 본에 대해서 업데이트를 해준다.
		\param[in] pTracer i3SceneTracer 객체 포인터
		\param[in] pCtx i3AnimationContext 객체 포인터
		\param[in] pBoneMap Bone Map
		\return N/A */
	void				AfterTraverseIKUpdate( i3SceneTracer * pTracer, i3AnimationContext2 * pCtx, INT32 * pBoneMap);
	
	/** \brief i3Skeleton을 i3Skeleton2로 변환한다.
		\param[in] pSrc i3Skeleton 객체 포인터
		\return N/A */
	void				ConvertFromSkeleton( i3Skeleton * pSrc);

	/** \brief i3Skeleton2를 i3Skeleton으로 변환한다.
		\param[out] pDest i3Skeleton 객체 포인터
		\return N/A */
	void				ConvertToSkeleton( i3Skeleton * pDest);

	/** \brief 현재 IK node에 있는 본인지 검사
		\param[in] pIKContext i3IKContext 객체 포인터
		\param[in] iStartBoneIdx Starter Bone Index. @n Starter에서부터 검사를 한다. 그 이전은 무시.
		\param[in] iBoneIdx Bone Index
		\param[in] pBoneMap Bone Map
		\return true : 포함. false : 미포함 */
	bool				CheckIncludeIKContext( i3IKContext * pIKContext, INT32 iStartBoneIdx, INT32 iBoneIdx, INT32 * pBoneMap);

	/** \brief IK 사용 유무를 설정한다.
		\param[in] bEnable true : IK사용. false : IK 미사용
		\return N/A */
	void				SetEnableIKAnimation( bool bEnable);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//									Animation stream data for Motion									//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	i3MatrixArray *			m_pTxMotionBoneArray;

public:
	/** \brief Motion Matrix를 반환한다.
		\param[in] iBoneIdx Bone index
		\return MATRIX 구조체 포인터 */
	MATRIX *				getMotionBoneMatrix( INT32 iBoneIdx)		{ I3ASSERT( iBoneIdx >= 0 && iBoneIdx < getBoneCount()); return m_pTxMotionBoneArray->GetMatrix( iBoneIdx); }

	
};

#endif
