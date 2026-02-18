#if !defined( __I3_IKBONEINFO_H)
#define __I3_IKBONEINFO_H

typedef UINT32 I3_IKBONE_FLAG;
#define I3_IKBONE_FLAG_DISABLE		0x00000001
#define I3_IKBONE_FLAG_STARTER		0x00000002	///< IK 의 Root Bone
#define	I3_IKBONE_FLAG_EFFECTOR		0x00000004	///< IK 의 끝 Bone
#define I3_IKBONE_FLAG_FREEZE		0x00000008	///< 움직이지 않는 IK Bone 
#define I3_IKBONE_FLAG_CHILD		0x00000010	///< IKBone의 Child, IKBone의 하위에 있는 경우 애니메이션을 하기 위한 flag
#define I3_IKBONE_FLAG_ANIMATE		0x00000020	///< IKBone의 Animation을 사용하여 최종 IK를 한다. @n Animation된 축을 유지하도록 설정(character의 foot 부분)
#define I3_IKBONE_FLAG_FIXEDTRANS	0x00010000	///< Bone Animation에 Position이 아닌 Distance에 의한 계산을 사용한다.(Skeleton의 Translation을 사용 Animation Key는 사용하지 않음)
#define I3_IKBONE_FLAG_MOTIONDATA	0x00020000	///< Motion Array에 세팅되는 본 정보를 사용한다.
#define I3_IKBONE_FLAG_FKANIM		0x00040000	///< KeyAnimation을 사용 (Off라면 TPose 상태의 기본 모델로 한다)

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

/** \brief IK용 Bone의 정보 */
struct ALIGN4 i3IKBoneInfo
{	
	//////////////////////////////////////////////////////////////////////////////////////////////
	// Persistent members
	I3_IKBONE_FLAG			m_BoneFlag;
	INT32					m_BoneIndex;			///< 매칭되는 Bone의 Index

	INT32					m_iIKJointCount;		///< Starter 까지의 Joint의 갯수
	
	REAL32					m_rSwing1_Low;			///< LocalAxis에서의 제한 각도 (radian 값)
	REAL32					m_rSwing1_High;			///< LocalAxis에서의 제한 각도 (radian 값)
	REAL32					m_rSwing1Tension;		///< 현재 frame에 회전각의 양을 조절한다. (0.f ~ 1.f)

	REAL32					m_rSwing2_Low;			///< LocalAxis에서의 제한 각도 (radian 값)
	REAL32					m_rSwing2_High;			///< LocalAxis에서의 제한 각도 (radian 값)
	REAL32					m_rSwing2Tension;		///< 현재 frame에 회전각의 양을 조절한다. (0.f ~ 1.f)

	REAL32					m_rTwist_Low;			///< LocalAxis에서의 제한 각도 (radian 값)
	REAL32					m_rTwist_High;			///< LocalAxis에서의 제한 각도 (radian 값)
	REAL32					m_rTwistTension;		///< 현재 frame에 회전각의 양을 조절한다. (0.f ~ 1.f)

	REAL32					m_rRotateLimit;			///< 최대 회전량 (이 각도를 기준으로 회전을 한다)

	INT32					m_PickedBoneIndex;		///< 해당 본과 동일하게 동작할때 사용

	//////////////////////////////////////////////////////////////////////////////////////////////
	// Volatile members
	VEC3D					m_vRotateAxis;		///< 회전축		(LocalAxis)
	REAL32					m_rRotationAngle;	///< 목표에 대한 회전각 

	MATRIX					m_mtxPreTemp;		///< 변경 전의 Matrix

	MATRIX					m_matInv;			///< 현재 Bone의 InvMat
	VEC3D					m_vAnchorDir;		///< Bone Center에서 Effector까지의 방향
	VEC3D					m_vTargetLocalDir;	///< Bone Center에서 TargetPoint까지의 방향
	
	MATRIX					m_mtxNextFrameRotate;		///< 다음 frame에 회전될 matrix
	bool					m_bFirstFrame;				///< 처음 frame에 대해서 PreRotate를 하지 않도록 하기 위함..

public:
	I3_IKBONE_FLAG			getFlag( void)						{ return m_BoneFlag; }
	void					addFlag( I3_IKBONE_FLAG flag)		{ m_BoneFlag |= flag; }
	void					setFlag( I3_IKBONE_FLAG flag)		{ m_BoneFlag = flag; }
	void					removeFlag( I3_IKBONE_FLAG flag)	{ m_BoneFlag &= ~flag; }
	bool					isFlag( I3_IKBONE_FLAG flag)		{ return (bool)((m_BoneFlag & flag) == flag); }



	/** \brief Bone index를 반환한다.
		\return Bone Index */
	INT32					getBoneIndex( void)					{ return m_BoneIndex;	}

	/** \brief Bone Index를 설정한다.
		\param[in] idx Bone Index
		\return N/A */
	void					setBoneIndex( UINT32 idx)			{ m_BoneIndex = idx;	}

	/** \brief IK Joint Count를 반환한다.
		\return IK Joint Count */
	INT32					getIKJointCount( void)				{ return m_iIKJointCount; }

	/** \brief IK Joint Count를 설정한다. */
	void					setIKJointCount( INT32 iCount)		{ m_iIKJointCount = iCount; }

	/** \brief Swing1 High Limit값을 반환한다.
		\return Limit값 */
	REAL32					getLimitSwing1High( void)			{ return m_rSwing1_High; }

	/** \brief Swing1 High Limit값을 설정한다.
		\param[in] rVal Limit값
		\return N/A */
	void					setLimitSwing1High( REAL32 rVal)	{ m_rSwing1_High = rVal; }

	/** \brief Swing1 Low Limit값을 반환한다.
		\return Limit값 */
	REAL32					getLimitSwing1Low( void)			{ return m_rSwing1_Low; }

	/** \brief Swing1 Low Limit값을 설정한다.
		\param[in] rVal Limit값
		\return N/A */
	void					setLimitSwing1Low( REAL32 rVal)		{ m_rSwing1_Low = rVal; }

	/** \brief Swing1 Tension값을 반환한다.
		\return Tension값 */
	REAL32					getTensionSwing1( void)				{ return m_rSwing1Tension; }

	/** \brief Swing1 Tension값을 설정한다.
		\param[in] rVal Tension값
		\return N/A */
	void					setTensionSwing1( REAL32 rVal)		{ m_rSwing1Tension = rVal; }

	/** \brief Swing2 High Limit값을 반환한다.
		\return Limit값 */
	REAL32					getLimitSwing2High( void)			{ return m_rSwing2_High; }

	/** \brief Swing2 High Limit값을 설정한다.
		\param[in] rVal Limit값
		\return N/A */
	void					setLimitSwing2High( REAL32 rVal)	{ m_rSwing2_High = rVal; }

	/** \brief Swing2 Low Limit값을 반환한다.
		\return Limit값 */
	REAL32					getLimitSwing2Low( void)			{ return m_rSwing2_Low; }

	/** \brief Swing2 Low Limit값을 설정한다.
		\param[in] rVal Limit값
		\return N/A */
	void					setLimitSwing2Low( REAL32 rVal)		{ m_rSwing2_Low = rVal; }

	/** \brief Swing2 Tension값을 반환한다.
		\return N/A */
	REAL32					getTensionSwing2( void)				{ return m_rSwing2Tension; }

	/** \brief Swing2 Tension값을 설정한다.
		\param[in] rVal Tension값
		\return N/A */
	void					setTensionSwing2( REAL32 rVal)		{ m_rSwing2Tension = rVal; }

	/** \brief Twist High Limit값을 반환한다.
		\return Limit값 */
	REAL32					getLimitTwistHigh( void)			{ return m_rTwist_High; }

	/** \brief Twist High Limit값을 설정한다.
		\param[in] rVal Limit값
		\return N/A */
	void					setLimitTwistHigh( REAL32 rVal)		{ m_rTwist_High = rVal; }

	/** \brief Twist Low Limit값을 반환한다.
		\return Limit값 */
	REAL32					getLimitTwistLow( void)				{ return m_rTwist_Low; }

	/** \brief Twist Low Limit값을 설정한다.
		\param[in] rVal Limit값
		\return N/A */
	void					setLimitTwistLow( REAL32 rVal)		{ m_rTwist_Low = rVal; }

	/** \brief Twist Tension값을 반환한다.
		\return Tension값 */
	REAL32					getTensionTwist( void)				{ return m_rTwistTension; }

	/** \brief Twist Tension값을 설정한다.
		\param[in] rVal Tension값
		\return N/A */
	void					setTensionTwist( REAL32 rVal)		{ m_rTwistTension = rVal; }

	/** \brief 전체적인 Rotation Limit값을 반환한다.
		\return Rotation (Radian) */
	REAL32					getRotateLimit( void)				{ return m_rRotateLimit; }
	void					setRotateLimit( REAL32 rAng)		{ m_rRotateLimit = rAng; }


	INT32					getPickedBoneIndex( void)			{ return m_PickedBoneIndex; }
	void					setPickedBoneIndex( INT32 idx)		{ m_PickedBoneIndex = idx; }

	/////////////////////////////////////////////////////////////////////////////////////////
	// in calculate fuction

	/** \brief 회전축을 반환한다.
		\return VEC3D 구조체 포인터 */
	VEC3D*					getRotateAxis( void)				{ return &m_vRotateAxis; }

	/** \brief 회전축을 설정한다.
		\param[in] pVec VEC3D 구조체 포인터
		\return N/A */
	void					setRotateAxis( VEC3D * pVec)		{ I3ASSERT( pVec); i3Vector::Copy( &m_vRotateAxis, pVec); }

	/** \brief 회전값을 반환한다.
		\return 회전값. (radian) */
	REAL32					getRotationAngle( void)				{ return m_rRotationAngle; }

	/** \brief 회전값을 설정한다.
		\param[in] rVal 회전값. (radian)
		\return N/A */
	void					setRotationAngle( REAL32 rVal)		{ m_rRotationAngle = rVal; }

	/** \brief 회전하기 전의 Matrix를 반환한다.
		\return MATRIX 구조체 포인터 */
	MATRIX *				getPreTempMatrix( void)				{ return &m_mtxPreTemp; }

	/** \brief 회전하기 전의 Matrix를 설정한다.
		\param[in] pMat MATRIX 구조체 포인터
		\return N/A */
	void					setPreTempMatrix( MATRIX * pMat)	{ i3Matrix::Copy( &m_mtxPreTemp, pMat); }

	/** \brief Inverse Matrix를 반환한다.
		\return MATRIX 구조체 포인터 */
	MATRIX *				getInvMatrix( void)					{ return &m_matInv; }

	/** \brief Inverse Matrix를 설정한다.
		\param[in] pMat MATRIX 구조체 포인터
		\return N/A */
	void					setInvMatrix( MATRIX * pMat)		{ i3Matrix::Copy( &m_matInv, pMat); }

	/** \brief Bone이 바라보는 방향을 반환한다.
		\return VEC3D 구조체 포인터 */
	VEC3D *					getAnchorDir( void)					{ return &m_vAnchorDir; }

	/** \brief Bone이 바라보는 방향을 설정한다.
		\param[in] pVec VEC3D 구조체 포인터
		\return N/A */
	void					setAnchorDir( VEC3D * pVec)			{ i3Vector::Copy( &m_vAnchorDir, pVec); }

	/** \brief 목표점에 대한 방향을 반환한다.
		\return VEC3D 구조체 포인터 */
	VEC3D *					getTargetDir( void)					{ return &m_vTargetLocalDir; }

	/** \brief 목표점에 대한 방향을 설정한다.
		\param[in] pVec VEC3D 구조체 포인터
		\return N/A */
	void					setTargetDir( VEC3D * pVec)			{ i3Vector::Copy( &m_vTargetLocalDir, pVec); }

	/** \brief 다음 Frame에 Update되는 matrix를 반환한다.
		\return MATRIX 구조체 포인터 */
	MATRIX *				getNextFrameRotate( void)			{ return &m_mtxNextFrameRotate; }

	/** \brief 다음 Frame에 Update되는 Matrix를 설정한다.
		\param[in] pMat MATRIX 구조체 포인터
		\return N/A */
	void					setNextFrameRotate( MATRIX * pMat)	{ i3Matrix::Copy( &m_mtxNextFrameRotate, pMat); }

	/** \brief 맨 첫 frame인지 확인한다.
		\return true : 첫 frame. false : 첫 frame이 아니다. */
	bool					isFirstFrame( void)					{ return m_bFirstFrame; }

	/** \brief 첫 frame 유무를 설정한다.
		\param[in] bVal true : 첫 frame. false : 첫 frame이 아니다.
		\return N/A */
	void					setFirstFrameValue( bool bVal)		{ m_bFirstFrame = bVal; }
	

	bool					SaveToXML( const char * pszField, i3XMLElement * pXML);
	bool					LoadFromXML( const char * pszField, i3XMLElement * pXML);
} ;

#if defined( I3_COMPILER_VC)
#pragma pack( pop )
#endif

#endif
