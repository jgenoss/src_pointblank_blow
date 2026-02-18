#if !defined( __I3_IK_SOLVER_H__)
#define __I3_IK_SOLVER_H__

#include "i3IKBoneInfo.h"
#include "Animation/i3AnimDef.h"

/** \brief IK 계산 방식 */
enum I3_IKSOLVER_TYPE
{	
	I3_IKSOLVER_TYPE_JACOBIAN,				///< Jacobian 방식
	I3_IKSOLVER_TYPE_PSEUDOINVERSE,			///< Pseudo Inverse 방식
	I3_IKSOLVER_TYPE_DLS,					///< DLS 방식
	I3_IKSOLVER_TYPE_SDLS,					///< SDLS 방식
};

class i3IKContext;
class i3IKConstraint;
class i3Skeleton;
class i3AnimationContext2;
class i3SceneTracer;

/** \brief IK 계산을 하는 class */
class I3_EXPORT_SCENE i3IKSolver : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3IKSolver, i3ElementBase);
protected:

	// Volatile members
	I3_IKSOLVER_TYPE	m_nSolverType;
	
	INT32			m_iLimitSolvCount;
	INT32			m_iSolvCount;

	// Jacobian variables
	MATRIX_N		m_matnJacobian;
	VECND			m_vnDeltaS;
	VECND			m_vnDeltaTheta;
	VECND			m_vnDeltaT;
	
	VECND			m_vnSuperDiag;			// temp member
	MATRIX_N		m_matnAugmented;		// temp member
	
	// Singular Value Decomposition
	MATRIX_N		m_matnU;
	MATRIX_N		m_matnV;
	VECND			m_vnW;

	REAL32			m_rThresHoldFactor;

	// DLS
	REAL32			m_rDampingLambdaSq;

	// SDLS
	MATRIX_N		m_matnJacobianNorm;
	VECND			m_vnDeltaSclamp;
	VECND			m_vnPrevTheta;
	REAL32			m_rMaxAngleSDLS;

	INT32			m_iCurrentCalcAxis;		///< 현재 계산되고 있는 축 번호 (x:0, y:1, z:2)
	REAL32			m_rThresHoldAngle;		///< 회전각이 이보다 작으면 회전하지 않습니다.

protected:
	// 각 bone의 limit값에 맞춰 회전량을 세팅함
	void	_SetAngleByLimitValue( VECND * pOut, i3IKContext * pIKCtx);

	// only SDLS
	// Damping값에 의한 절삭처리 함수
	void	_CalcdTClampedFromdS( void);

	// 목표점으로 회전할 수 있는 회전축을 구한다.(LocalAxis)
	bool	_CalculateRotateAxisByNoLimit( MATRIX * pMat, MATRIX * pEffectMat, MATRIX * pTargetMat, i3IKBoneInfo * pIKInfo);
	void	_CalculateRotateAxisByLimit( MATRIX * pMat, MATRIX * pEffectMat, MATRIX * pTargetMat, i3IKBoneInfo * pIKInfo);
	
	// 회전량에 대한 제한각을 적용합니다.
	REAL32	_CalculateLimitAngleForce( i3Skeleton * pSkel, i3IKBoneInfo * pIKInfo, MATRIX * pMat, MATRIX * pParentMat, i3IKContext * pIKCtx, REAL32 rMinLimit);
	
	// 회전되어야 할 전체 회전각을 구합니다.
	void	_PresetRotateAxis( i3IKBoneInfo * pIKInfo, VEC3D * pVecAnchor, VEC3D * pVecTarget);

	// EffectorBone의 회전량 계산
	void	_CalucateRotationEffector( i3IKConstraint * pConstraint, i3IKBoneInfo * pEffectIKInfo, MATRIX * pEffectMat);

	// 회전량 계산
	bool	_SetJacobianMatrix( i3AnimationContext2 * pAnimCtx, i3IKContext * pIKCtx);
	void	_CalculateJacobianTranspose( i3IKContext * pIKCtx);
	void	_CalculatePseudoinverse( i3IKContext * pIKCtx);
	void	_CalculateDLS( i3IKContext * pIKCtx);
	void	_CalculateDLSwithSVD( i3IKContext * pIKCtx);
	void	_CalculateSDLS( i3IKContext * pIKCtx);

	// 계산되어진 deltaTheta를 bone에 업데이트
	void	_UpdateIKBone( I3_ANIM_MEMORY_SEQUENCE_INFO * pOut, I3_ANIM_MEMORY_SEQUENCE_INFO * pOriginal, i3AnimationContext2 * pAnimCtx, i3IKContext * pIKCtx);

	// Freeze된 Bone을 업데이트
	void	_UpdateFreezeBone( I3_ANIM_MEMORY_SEQUENCE_INFO * pOut, i3AnimationContext2 * pAnimCtx, i3IKContext * pIKCtx);

	// 이전 frame에 계산되어진 IK Angle을 업데이트 합니다.
	void	_RestoreIKMatrix( i3AnimationContext2 * pAnimCtx, i3IKContext * pIKCtx);

	// 해당 bone의 animation 값을 가져 옵니다.
	bool	_GetAnimatedMatrix( MATRIX * pOut, i3AnimationContext2 * pAnimCtx, INT32 iBoneIdx);

public:
	/** \brief IK 계산 방식을 반환한다.
		\return I3_IKSOLVER_TYPE */
	I3_IKSOLVER_TYPE	getSolverType( void)						{ return m_nSolverType; }

	/** \brief IK 계산 방식을 설정한다.
		\param[in] nType I3_IKSOLVER_TYPE
		\return N/A */
	void			setSolverType( I3_IKSOLVER_TYPE nType)			{ m_nSolverType = nType; }

	/** \brief 최대 처리할 수 있는 횟수를 반환한다.
		\note 축을 분할해서 처리하므로 3배수로 증가합니다. (x, y, z)
		\return IK 계산 횟수 */
	INT32			getSolvThreshCount( void)						{ return m_iLimitSolvCount; }

	/** \brief IK 계산 횟수를 설정한다.
		\param[in] iCount 횟수 X 3 으로 설정된다.
		\return N/A */
	void			setSolvThreshCount( INT32 iCount)				{ m_iLimitSolvCount = iCount * 3; }

	/** \brief 최대 회전각을 반환한다.
		\return 회전각 (radian) */
	REAL32			getThresHoldAngle( void)						{ return m_rThresHoldAngle; }

	/** \brief 최대 회전각을 설정한다.
		\param[in] rVal 회전각 (radian)
		\return N/A */
	void			setThresHoldAngle( REAL32 rVal)					{ m_rThresHoldAngle = rVal; }
	

public:
	i3IKSolver();
	virtual ~i3IKSolver();

	/** \brief 계산용 Buffer를 생성한다.
		\param[in] iIKBoneCount IK Bone Count
		\param[in] iIKEffectorCount Effector count
		\return N/A */
	void	CreateBuffer( INT32 iIKBoneCount, INT32 iIKEffectorCount);

	void	Update( I3_ANIM_MEMORY_SEQUENCE_INFO * pOut, I3_ANIM_MEMORY_SEQUENCE_INFO * pOriginal, i3AnimationContext2 * pAnimCtx, i3IKContext * pIKCtx);

	/** \brief Update
		\param[in] pTracer i3SceneTracer 객체 포인터
		\param[in] pSkel i3Skeleton 객체 포인터
		\param[in] pAnimCtx i3AnimationContext 객체 포인터
		\param[in] pBoneMap Bone Map
		\return N/A */
	//void	Update( i3SceneTracer * pTracer, i3Skeleton2 * pSkel, i3AnimationContext2 * pAnimCtx, INT32 * pBoneMap);

	/** \brief Renderable Axis를 설정한다. (for Debug)
		\param[in] pIKCtx i3IKContext 객체 포인터
		\param[in] pIKInfo i3IKBoneInfo 구조체 포인터
		\param[in] pLocalParentMat 부모의 Local Matrix
		\param[in] pLocalMat Local Matrix
		\param[in] rAngle 회전값 (radian)
		\return N/A */
	void	SetRenderableAxis( i3IKContext * pIKCtx, i3IKBoneInfo * pIKInfo, MATRIX * pLocalParentMat, MATRIX * pLocalMat, REAL32 rAngle);
};

extern I3_EXPORT_SCENE i3IKSolver * g_pIKSolver;

#endif
