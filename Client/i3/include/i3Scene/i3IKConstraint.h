#if !defined( __I3_IKCONSTRAINT_H)
#define __I3_IKCONSTRAINT_H

/**
\file i3IKConstraint.h
\author Jung, SwoonGoo
\date 2009-2-19
*/

/** \brief TargetPoint에 대한 정보를 처리합니다. */
class I3_EXPORT_SCENE i3IKConstraint : public i3PersistantElement
{
	I3_EXPORT_CLASS_DEFINE( i3IKConstraint, i3PersistantElement);
protected:
	// Persistent members
	INT32				m_iIKBoneStarterIdx;		///< Starter bone
	INT32				m_iIKBoneEffectorIdx;		///< Effector bone

	REAL32				m_rDistanceThresh;			///< EndEffector와 Targetpoint의 distance threshold

	// Volatile members
	enum I3_IKCONSTRAINT_TYPE
	{
		I3_IKCONSTRAINT_TYPE_NONE,
		I3_IKCONSTRAINT_TYPE_ANIMATION,			///< not used
		I3_IKCONSTRAINT_TYPE_USERVALUE,			///< 외부 목표점으로 처리한다.
	};
	
	I3_IKCONSTRAINT_TYPE	m_Type;

	MATRIX				m_matTarget;				///< 내부 Matrix 계산에 사용, Worldmatrix를 기준으로 한다.
	MATRIX *			m_pTargetMat;				///< 외부 Matrix pointer

public:
	/** \brief IK Bone의 Starter Index를 반환한다.
		\return Bone Index	*/
	INT32		getIKBoneStarterIdx( void)					{ return m_iIKBoneStarterIdx; }

	/** \brief IK Bone의 Starter Index를 설정한다.
		\param[in] idx Bone index
		\return N/A */
	void		setIKBoneStarterIdx( INT32 idx)				{ m_iIKBoneStarterIdx = idx; }

	/** \brief IK Bone의 Effector Index를 반환한다.
		\note Effector Bone이 목표점에 근접하도록 IK가 계산이 된다.
		\return Bone Index */
	INT32		getIKBoneEffectorIdx( void)					{ return m_iIKBoneEffectorIdx; }

	/** \brief IK Bone의 Effector Index를 설정한다.
		\param[in] idx Bone Index
		\return N/A */
	void		setIKBoneEffectorIdx( INT32 idx)			{ m_iIKBoneEffectorIdx = idx; }

	/** \brief 목표점에 대한 접근 거리 정도값을 반환한다.
		\return 거리값	*/
	REAL32		getDistanceThresh( void)					{ return m_rDistanceThresh; }

	/** \brief 목표점에 대한 접근 거리 정도값을 설정한다.
		\param[in] rThreshHold 거리값
		\return N/A */
	void		setDistanceThresh( REAL32 rThreshHold)		{ m_rDistanceThresh = rThreshHold; }

	/** \brief 목표점에 대한 Matrix를 반환한다.
		\return MATRIX 구조체 포인터 */
	MATRIX *	getTargetMatrix( void)						{ return &m_matTarget; }

	/** \brief 목표점에 대한 Matrix를 설정한다.
		\param[in] pMat MATRIX 구조체 포인터
		\return N/A */
	void		setTargetMatrix( MATRIX * pMat)				{ i3Matrix::Copy( &m_matTarget, pMat); }

	/** \brief 목표점에 대한 Matrix pointer를 반환한다. (Not used!!)
		\return MATRIX 구조체 포인터 */
	MATRIX *	getTargetMatrixPtr( void)					{ return m_pTargetMat; }

	/** \brief 목표점에 대한 Matrix pointer를 설정한다. (Not used!!)
		\param[in] pMat MATRIX 구조체 포인터
		\return N/A */
	void		setTargetMatrixPtr( MATRIX * pMat)			{ m_pTargetMat = pMat; }


public:
	i3IKConstraint();
	virtual ~i3IKConstraint();

	/** \brief 목표점에 접근했는지 검사한다.
		\param[in] pVec VEC3D 구조체 포이터. 목표점
		\return true : 접근했다. false : 접근하지 않았다. */
	bool		CheckReachToTarget( VEC3D * pVec);

	/**
	\brief i3IKConstraint을 복사한다.
	\param[out] pObj 복사할 대상 i3IKConstraint 객체 포인터.
	\param[in] method 복사하는 방식
	\return N/A
	*/
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);

	/**
	\brief 파일로 저장한다.
	\param[in] pResFile i3ResourceFile 객체 포인터
	\return 파일 사이즈
	*/
	virtual UINT32	OnSave( i3ResourceFile * pResFile);

	/**
	\brief 파일에서 읽어온다.
	\param[in] pResFile i3ResourceFile 객체 포인터
	\return 파일 사이즈
	*/
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);

	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);
};

#endif
