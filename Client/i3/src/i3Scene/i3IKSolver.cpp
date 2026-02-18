#include "i3SceneDef.h"
#include "i3IKSolver.h"
#include "i3IKConstraint.h"
#include "i3IKBoneInfo.h"
#include "Animation/i3AnimationContext2.h"
#include "i3IKContext.h"
#include "i3Skeleton.h"
#include "i3SceneTracer.h"
#include "i3BoneRef.h"

//I3_CLASS_INSTANCE( i3IKSolver, i3ElementBase);
I3_CLASS_INSTANCE( i3IKSolver);

i3IKSolver::i3IKSolver()
{
	m_nSolverType		= I3_IKSOLVER_TYPE_JACOBIAN;
	m_iLimitSolvCount	= 30;
	m_iSolvCount		= 0;

	m_rThresHoldFactor	= 0.f;
	m_rDampingLambdaSq	= 0.f;

	m_rMaxAngleSDLS		= 0.f;

	m_iCurrentCalcAxis	= 0;
	m_rThresHoldAngle				= 0.01f;
}

i3IKSolver::~i3IKSolver()
{	
}

void i3IKSolver::_SetAngleByLimitValue( VECND * pOut, i3IKContext * pIKCtx)
{
	INT32 i;

	I3ASSERT( pOut != NULL);

	REAL32 maxChange = 1.f / i3VectorN::MaxAbs( pOut);
	REAL32 * rValues = pOut->getValues();

	for( i = 0; i < pIKCtx->getIKBoneCount(); i++)
	{
		i3IKBoneInfo * pInfo = pIKCtx->getIKBoneInfo( i);

		I3ASSERT( pOut->getSize() > pInfo->getIKJointCount());
			
		// 회전축에 대한 리미트값까지 산출한다.
		if( maxChange > pInfo->getRotateLimit() )
		{	
			rValues[ pInfo->getIKJointCount()] *= pInfo->getRotateLimit() * maxChange;
		}

		//if( rValues[ pInfo->getIKJointCount()] < 0.f)
		//	rValues[ pInfo->getIKJointCount()] = 0.f;
	}
}

void i3IKSolver::_CalcdTClampedFromdS( void)
{
	INT32 i;
	INT32 len = m_vnDeltaS.getSize();
	INT32 j = 0;

	REAL32 * dS = m_vnDeltaS.getValues();
	REAL32 * dT = m_vnDeltaT.getValues();
	REAL32 * dSclamp = m_vnDeltaSclamp.getValues();

	for( i = 0; i < len; i+=3, j++ )
	{
		REAL32 normSq = (dS[i] * dS[i]) + (dS[i+1] * dS[i+1]) + (dS[i+2] * dS[i+2]);

		if( normSq > (dSclamp[j] * dSclamp[j]) )
		{
			REAL32 factor = dSclamp[j] / i3Math::Sqrt( normSq);

			dT[i] = dS[i]*factor;
			dT[i+1] = dS[i+1]*factor;
			dT[i+2] = dS[i+2]*factor;
		}
		else {
			dT[i] = dS[i];
			dT[i+1] = dS[i+1];
			dT[i+2] = dS[i+2];
		}
	}
}

void i3IKSolver::CreateBuffer( INT32 iIKBoneCount, INT32 iIKEffectorCount)
{
	INT32 iRowCount = iIKEffectorCount * 3;
	INT32 iColCount = iIKBoneCount;

	m_vnDeltaS.setSize( iRowCount );
	m_vnDeltaTheta.setSize( iIKBoneCount );
	m_vnDeltaT.setSize( iRowCount );

	m_matnJacobian.setSize( iRowCount, iIKBoneCount);

	m_vnSuperDiag.setSize( iRowCount);
	m_matnU.setSize( iRowCount, iRowCount);
	m_matnV.setSize( iIKBoneCount, iIKBoneCount);
	m_vnW.setSize( MIN( iRowCount, iColCount) );

	m_matnJacobianNorm.setSize(iIKEffectorCount, iIKBoneCount);
	m_vnDeltaSclamp.setSize( iIKEffectorCount);
	m_vnPrevTheta.setSize( iColCount);

	m_rThresHoldFactor = 0.1f;
	m_rDampingLambdaSq = 0.1f;
	m_rMaxAngleSDLS = 0.01f;
}

void i3IKSolver::Update( I3_ANIM_MEMORY_SEQUENCE_INFO * pOut, I3_ANIM_MEMORY_SEQUENCE_INFO * pOriginal, i3AnimationContext2 * pAnimCtx, i3IKContext * pIKCtx)
{
	I3ASSERT( pAnimCtx != NULL);
	I3ASSERT( pIKCtx != NULL);

	if( !(pIKCtx->getIKStyle() & I3_IKBONE_STYLE_ENABLE) )
		return;

	bool bLoof = true;
	m_iSolvCount = 0;

	m_iCurrentCalcAxis = 0;

	while( bLoof)
	{
		// 횟수 초과면 종료
		m_iSolvCount++;
		if( m_iSolvCount > m_iLimitSolvCount)
			break;
		
		// 내부에서 거리를 측정합니다. return true이면 도달한 것입니다.
		if( _SetJacobianMatrix( pAnimCtx, pIKCtx) == false)
		{
			switch( m_nSolverType)
			{
			case I3_IKSOLVER_TYPE_JACOBIAN :		_CalculateJacobianTranspose( pIKCtx);		break;
			case I3_IKSOLVER_TYPE_PSEUDOINVERSE :	_CalculatePseudoinverse( pIKCtx);			break;
			case I3_IKSOLVER_TYPE_DLS :				_CalculateDLS( pIKCtx);						break;
			case I3_IKSOLVER_TYPE_SDLS :			_CalculateSDLS( pIKCtx);						break;
			}

			_UpdateIKBone( pOut, pOriginal, pAnimCtx, pIKCtx);

			m_iCurrentCalcAxis = (m_iCurrentCalcAxis + 1) % 3;
		}
		else
		{
			bLoof = false;
		}
	}

	_UpdateFreezeBone( pOut, pAnimCtx, pIKCtx);
}

bool i3IKSolver::_CalculateRotateAxisByNoLimit( MATRIX * pMat, MATRIX * pEffectMat, MATRIX * pTargetMat, i3IKBoneInfo * pIKInfo)
{
	// 현재 IKBone의 Inverse를 구해서 Anchor와 Target에 대한 Direction을 구한다.
	i3Matrix::Inverse( pIKInfo->getInvMatrix(), 0, pMat );

	i3Vector::TransformCoord( pIKInfo->getAnchorDir(), i3Matrix::GetPos( pEffectMat), pIKInfo->getInvMatrix());
	i3Vector::TransformCoord( pIKInfo->getTargetDir(), i3Matrix::GetPos( pTargetMat), pIKInfo->getInvMatrix());

	i3Vector::Cross( pIKInfo->getRotateAxis(), pIKInfo->getAnchorDir(), pIKInfo->getTargetDir());

	i3Vector::Normalize( pIKInfo->getRotateAxis(), pIKInfo->getRotateAxis());

	return true;
}

void i3IKSolver::_PresetRotateAxis( i3IKBoneInfo * pIKInfo, VEC3D * pVecAnchor, VEC3D * pVecTarget)
{
	REAL32 rAngle;
	VEC3D vAnchor, vTarget;
	REAL32	rReverse = 1.f;

	bool bSkip = false;

	switch( m_iCurrentCalcAxis )
	{
	case 0:
		{
			if( (pIKInfo->getLimitSwing2Low() == 0.f) &&
				(pIKInfo->getLimitSwing2High() == 0.f) )
			{
				setY( pIKInfo->getRotateAxis(), 0.f);
				setZ( pIKInfo->getRotateAxis(), 0.f);

				bSkip = true;
			}
			else
			{
				i3Vector::Set( &vAnchor, 0.f, i3Vector::GetY( pVecAnchor), i3Vector::GetZ( pVecAnchor));
				i3Vector::Set( &vTarget, 0.f, i3Vector::GetY( pVecTarget), i3Vector::GetZ( pVecTarget));
				
				setY( pIKInfo->getRotateAxis(), 0.f);
				setZ( pIKInfo->getRotateAxis(), 0.f);

				if( getX( pIKInfo->getRotateAxis()) < 0.f)
					rReverse = -1.f;
			}
		}
		break;
	case 1:
		{
			if( (pIKInfo->getLimitSwing1Low() == 0.f) &&
				(pIKInfo->getLimitSwing1High() == 0.f) )
			{
				setX( pIKInfo->getRotateAxis(), 0.f);
				setZ( pIKInfo->getRotateAxis(), 0.f);

				bSkip = true;
			}
			else
			{
				i3Vector::Set( &vAnchor, i3Vector::GetX( pVecAnchor), 0.f, i3Vector::GetZ( pVecAnchor));
				i3Vector::Set( &vTarget, i3Vector::GetX( pVecTarget), 0.f, i3Vector::GetZ( pVecTarget));

				setX( pIKInfo->getRotateAxis(), 0.f);
				setZ( pIKInfo->getRotateAxis(), 0.f);

				if( getY( pIKInfo->getRotateAxis()) < 0.f)
					rReverse = -1.f;
			}
		}
		break;

	case 2:
		{
			if( (pIKInfo->getLimitTwistLow() == 0.f) &&
				(pIKInfo->getLimitTwistHigh() == 0.f) )
			{
				setX( pIKInfo->getRotateAxis(), 0.f);
				setY( pIKInfo->getRotateAxis(), 0.f);

				bSkip = true;
			}
			else
			{
				i3Vector::Set( &vAnchor, i3Vector::GetX( pVecAnchor), i3Vector::GetY( pVecAnchor), 0.f);
				i3Vector::Set( &vTarget, i3Vector::GetX( pVecTarget), i3Vector::GetY( pVecTarget), 0.f);

				setX( pIKInfo->getRotateAxis(), 0.f);
				setY( pIKInfo->getRotateAxis(), 0.f);

				if( getZ( pIKInfo->getRotateAxis()) < 0.f)
					rReverse = -1.f;
			}
		}
		break;
	default :
		I3FATAL( "invalid IK axis.");
		break;
	}

	if( !bSkip)
	{
		i3Vector::Normalize( &vAnchor, &vAnchor);
		i3Vector::Normalize( &vTarget, &vTarget);

		rAngle = (1.f - i3Vector::Dot( &vAnchor, &vTarget)) * I3_PI2 * rReverse;

		if( rAngle < m_rThresHoldAngle && rAngle > -m_rThresHoldAngle )
			rAngle = 0.f;

		pIKInfo->setRotationAngle( rAngle);
	}
	else
		pIKInfo->setRotationAngle( 0.f);

	i3Vector::Normalize( pIKInfo->getRotateAxis(), pIKInfo->getRotateAxis());
}

void i3IKSolver::_CalculateRotateAxisByLimit( MATRIX * pMat, MATRIX * pEffectMat, MATRIX * pTargetMat, i3IKBoneInfo * pIKInfo)
{
	VEC3D vCross;

	i3Matrix::Inverse( pIKInfo->getInvMatrix(), 0, pMat );

	i3Vector::TransformCoord( pIKInfo->getAnchorDir(), i3Matrix::GetPos( pEffectMat), pIKInfo->getInvMatrix());
	i3Vector::TransformCoord( pIKInfo->getTargetDir(), i3Matrix::GetPos( pTargetMat), pIKInfo->getInvMatrix());

	i3Vector::Cross( &vCross, pIKInfo->getAnchorDir(), pIKInfo->getTargetDir());

	//if( i3Vector::Length( &vCross) > I3_EPS)
	{
		i3Vector::Copy( pIKInfo->getRotateAxis(), &vCross);
	}

	_PresetRotateAxis( pIKInfo, pIKInfo->getAnchorDir(), pIKInfo->getTargetDir());
}

REAL32 i3IKSolver::_CalculateLimitAngleForce( i3Skeleton * pSkel, i3IKBoneInfo * pIKInfo,
											 MATRIX * pMat, MATRIX * pParentMat, i3IKContext * pIKCtx, REAL32 rMinLimit)
{	
	MATRIX matOriginal;
	MATRIX mtxLocalOriginal;
	VEC3D vTemp;

	REAL32 rLimit;
	REAL32 rTotalAngle;

	REAL32 rDot, rDir;
	REAL32 rCurrentAngle;

	I3ASSERT( pIKInfo != NULL);
	I3ASSERT( pParentMat != NULL);

	i3Bone * pBone = pSkel->getBone( pIKInfo->getBoneIndex());

	// Bone의 기본 Matrix
	i3Matrix::Mul( &matOriginal, pBone->getMatrix(), pParentMat);
	
	// 기본 Matrix의 Local
	i3Matrix::Mul( &mtxLocalOriginal, &matOriginal, pIKInfo->getInvMatrix());
	
	// X-Axis
	switch( m_iCurrentCalcAxis)
	{	
	case 0:
		if( pIKInfo->getLimitSwing2Low() == pIKInfo->getLimitSwing2High() )
		{
			rMinLimit = 0.f;
		}
		else
		{
			i3Vector::Set( &vTemp, 0.f, i3Vector::GetY( i3Matrix::GetUp( &mtxLocalOriginal)), i3Vector::GetZ( i3Matrix::GetUp( &mtxLocalOriginal)));
			i3Vector::Normalize( &vTemp, &vTemp);
			
			rDot = getY( &vTemp);
			rDir = getZ( &vTemp);

			rCurrentAngle = (1.f - rDot) * I3_PI2;

			if( rDir > 0.f)
				rCurrentAngle *= -1.f;

			rTotalAngle = pIKInfo->getRotationAngle() + rCurrentAngle;
			
			if( pIKInfo->getLimitSwing2Low() > rTotalAngle )
			{
				rLimit = (rCurrentAngle - pIKInfo->getLimitSwing2Low()) * pIKInfo->getTensionSwing2();
				if( rLimit < rMinLimit )
				{
					rMinLimit = rLimit;
				}
			}
			
			if( pIKInfo->getLimitSwing2High() < rTotalAngle)
			{
				rLimit = (pIKInfo->getLimitSwing2High() - rCurrentAngle) * pIKInfo->getTensionSwing2();

				if( rLimit < rMinLimit )
					rMinLimit = rLimit;
			}
		}
		break;

	case 1 :
		if( pIKInfo->getLimitSwing1Low() == pIKInfo->getLimitSwing1High() )
		{
			rMinLimit = 0.f;
		}
		else
		{
			i3Vector::Set( &vTemp, i3Vector::GetX( i3Matrix::GetAt( &mtxLocalOriginal)), 0.f, i3Vector::GetZ( i3Matrix::GetAt( &mtxLocalOriginal)));
			i3Vector::Normalize( &vTemp, &vTemp);

			rDot = getZ( &vTemp);
			rDir = getX( &vTemp);

			rCurrentAngle = (1.f - rDot) * I3_PI2;

			if( rDir > 0.f)
				rCurrentAngle *= -1.f;

			rTotalAngle = pIKInfo->getRotationAngle() + rCurrentAngle;
						
			if( pIKInfo->getLimitSwing1Low() > rTotalAngle)
			{
				rLimit = (rCurrentAngle - pIKInfo->getLimitSwing1Low()) * pIKInfo->getTensionSwing1();

				if( rLimit < rMinLimit )
					rMinLimit = rLimit;
			}

			if( pIKInfo->getLimitSwing1High() < rTotalAngle)
			{
				rLimit = (pIKInfo->getLimitSwing1High() - rCurrentAngle) * pIKInfo->getTensionSwing1();

				if( rLimit < rMinLimit )
					rMinLimit = rLimit;
			}
		}
		break;
	
	case 2 :
		if( pIKInfo->getLimitTwistLow() == pIKInfo->getLimitTwistHigh() )
		{
			rMinLimit = 0.f;
		}
		else
		{
			i3Vector::Set( &vTemp, getX( i3Matrix::GetRight( &mtxLocalOriginal)), getY( i3Matrix::GetRight( &mtxLocalOriginal)), 0.f);
			i3Vector::Normalize( &vTemp, &vTemp);
			
			rDot = getX( &vTemp);
			rDir = getY( &vTemp);

			rCurrentAngle = (1.f - rDot) * I3_PI2;

			if( rDir > 0.f)
				rCurrentAngle *= -1.f;

			rTotalAngle = pIKInfo->getRotationAngle() + rCurrentAngle;

			if( pIKInfo->getLimitTwistLow() > rTotalAngle )
			{	
				rLimit = (rCurrentAngle - pIKInfo->getLimitTwistLow()) * pIKInfo->getTensionTwist();

				if( rLimit < rMinLimit )
					rMinLimit = rLimit;
			}
			
			if( pIKInfo->getLimitTwistHigh() < rTotalAngle )
			{
				rLimit = (pIKInfo->getLimitTwistHigh() - rCurrentAngle) * pIKInfo->getTensionTwist();

				if( rLimit < rMinLimit )
					rMinLimit = rLimit;
			}
		}
		break;
	}

#if defined( I3_DEBUG)
	SetRenderableAxis( pIKCtx, pIKInfo, &mtxLocalOriginal, pBone->getMatrix(), 0.f);
#endif

	return rMinLimit;
}

void i3IKSolver::_CalucateRotationEffector( i3IKConstraint * pConstraint, i3IKBoneInfo * pEffectIKInfo, MATRIX * pEffectMat)
{
	VEC3D vTemp, vCurrentDir;

	// Rotation용으로 역행렬을 구한다.
	MATRIX vTempMtx;

	i3Matrix::Copy( &vTempMtx, pEffectMat);
	i3Matrix::SetPos( &vTempMtx, 0.f, 0.f, 0.f, 1.f);

	i3Matrix::Inverse( pEffectIKInfo->getInvMatrix(), 0, &vTempMtx);

	switch( m_iCurrentCalcAxis)
	{
	case 0:
		{
			i3Vector::Set( &vCurrentDir, 0.f, 1.f, 0.f);
			i3Vector::TransformNormal( &vTemp, i3Matrix::GetRight( pConstraint->getTargetMatrix()), pEffectIKInfo->getInvMatrix());
		}
		break;
	case 1:
		{
			i3Vector::Set( &vCurrentDir, 0.f, 0.f, 1.f);
			i3Vector::TransformNormal( &vTemp, i3Matrix::GetUp( pConstraint->getTargetMatrix()), pEffectIKInfo->getInvMatrix());
		}
		break;
	case 2:
		{
			i3Vector::Set( &vCurrentDir, 1.f, 0.f, 0.f);
			i3Vector::TransformNormal( &vTemp, i3Matrix::GetAt( pConstraint->getTargetMatrix()), pEffectIKInfo->getInvMatrix());
		}
		break;
	default :
		//I3FATAL( "invalid calculate axis.");
		break;
	}

	i3Vector::Cross( pEffectIKInfo->getRotateAxis(), &vCurrentDir, &vTemp);

	REAL32 rAngle = (1.f - i3Vector::Dot( &vCurrentDir, &vTemp)) * I3_PI2;

	if( rAngle > pEffectIKInfo->getRotateLimit() )
		rAngle = pEffectIKInfo->getRotateLimit();

	pEffectIKInfo->setRotationAngle( rAngle);

	i3Vector::Set( &vTemp, 0.f, 0.f, 0.f);
	m_matnJacobian.setTriple( 0, pEffectIKInfo->getIKJointCount(), &vTemp);
}

bool i3IKSolver::_SetJacobianMatrix( i3AnimationContext2 * pAnimCtx, i3IKContext * pIKCtx)
{
	// Find all ancestors (they will usually all be joints)
	// Set the corresponding entries in the Jacobians J, K.
	INT32 parentIdx;

	VEC3D vTemp;
	VEC3D vCross;
	VEC3D vZero;
	VEC3D vDir;
	VEC3D vRotate;

	bool bRv = true;

	I3ASSERT( pAnimCtx != NULL);
	I3ASSERT( pIKCtx != NULL);

	i3IKBoneInfo * pIKBoneInfo = pIKCtx->getIKBoneInfo( pIKCtx->getIKBoneCount()-1 );
	I3ASSERT( pIKBoneInfo->getBoneIndex() != -1 );
	I3ASSERT( pIKBoneInfo->getBoneIndex() == pIKCtx->getIKConstraint()->getIKBoneEffectorIdx());

	i3Skeleton * pSkel = pAnimCtx->getFrameSkeleton();

	I3ASSERT( pSkel != NULL);
	
	i3VectorN::FillZero( &m_vnDeltaTheta);
	i3MatrixN::Identity( &m_matnJacobian);

	i3Vector::Zero( &vZero);

	i3Bone * pBone = pSkel->getBone( pIKCtx->getIKConstraint()->getIKBoneEffectorIdx());

	// Worldtransformed matrix
	MATRIX * pEffectMat = pAnimCtx->getTempMatrix( pIKCtx->getIKConstraint()->getIKBoneEffectorIdx());

	i3IKConstraint * pConstraint = pIKCtx->getIKConstraint();
	i3Vector::Sub( &vDir, i3Matrix::GetPos( pConstraint->getTargetMatrix()), i3Matrix::GetPos( pEffectMat));

	REAL32 rLen = i3Vector::Length( &vDir);

	if( rLen > pConstraint->getDistanceThresh() )
	{
		// Effector와 Target의 방향
		m_vnDeltaS.setValue( 0, i3Vector::GetX( &vDir));
		m_vnDeltaS.setValue( 1, i3Vector::GetY( &vDir));
		m_vnDeltaS.setValue( 2, i3Vector::GetZ( &vDir));

		// Effector Bone의 회전량 계산
		_CalucateRotationEffector( pConstraint, pIKBoneInfo, pEffectMat);

		// Jacobian Matrix를 세팅
		// 부모를 찾아가며 세팅을 한다.
		while( pBone->getParentIndex() != -1)
		{
			i3IKBoneInfo * pParentIKBoneInfo = pSkel->getIKBoneInfoByBoneIdx( pIKCtx, pBone->getParentIndex());
			if( pParentIKBoneInfo == NULL)
				break;

			i3Bone * pParentBone = pSkel->getBone( pBone->getParentIndex());

			i3Bone * pStarterBone = pSkel->getBone( pConstraint->getIKBoneStarterIdx());
			if( pParentBone->getDepth() < pStarterBone->getDepth())
				break;

			parentIdx = pBone->getParentIndex();

			if( pParentIKBoneInfo->getFlag() & I3_IKBONE_FLAG_FREEZE)
			{	
				m_matnJacobian.setTriple( 0, pParentIKBoneInfo->getIKJointCount(), &vZero);
			}
			else
			{	
				MATRIX * pParentMat = pAnimCtx->getTempMatrix( parentIdx);
				
				// 회전축 구하기
				_CalculateRotateAxisByLimit( pParentMat, pEffectMat, pConstraint->getTargetMatrix(), pParentIKBoneInfo);
				
				// Jacobian Matrix를 세팅한다.
				// world matrix로 변환
				MATRIX mtxTemp;

				i3Matrix::Copy( &mtxTemp, pParentMat);

				i3Vector::TransformNormal( &vRotate, pParentIKBoneInfo->getRotateAxis(), &mtxTemp);
				
				// -(end effector pos. - joint pos.)
				// cross product with joint rotation axis
				i3Vector::Sub( &vTemp, i3Matrix::GetPos( pParentMat), i3Matrix::GetPos( pEffectMat) );
				i3Vector::Cross( &vCross, &vTemp, &vRotate);

				m_matnJacobian.setTriple( 0, pParentIKBoneInfo->getIKJointCount(), &vCross);
			}
			
			pBone = pParentBone;
		}

		bRv = false;
	}

	return bRv;
}

void i3IKSolver::_CalculateJacobianTranspose( i3IKContext * pIKCtx)
{
	INT32 i;
	REAL32 beta, rScale;

	i3MatrixN::MultiplyTranspose( &m_vnDeltaTheta, &m_matnJacobian, &m_vnDeltaS );
	// Scale back the dTheta values greedily 
	i3MatrixN::Multiply( &m_vnDeltaT, &m_matnJacobian, &m_vnDeltaTheta );						// dT = J * dTheta
	REAL32 alpha = i3VectorN::Dot( &m_vnDeltaS, &m_vnDeltaT) / i3VectorN::Length( &m_vnDeltaT);

	// Also scale back to be have max angle change less than MaxAngleJtranspose
	if( alpha > 0.f )
	{
		REAL32 maxChange = 1.f / (i3VectorN::MaxAbs( &m_vnDeltaTheta));
		alpha *= maxChange;

		for( i = 0; i < pIKCtx->getIKBoneCount(); i++)
		{
			i3IKBoneInfo * pInfo = pIKCtx->getIKBoneInfo( i); 
				
			// 회전축에 대한 리미트값까지 산출한다.
			beta = MAX( pInfo->getRotateLimit() * maxChange, 0.01f);
			rScale = MIN( alpha, beta);

			REAL32 Theta = m_vnDeltaTheta.getValue( pInfo->getIKJointCount()) * rScale;
 			
			m_vnDeltaTheta.setValue( pInfo->getIKJointCount(), Theta);
		}
	}
}

void i3IKSolver::_CalculatePseudoinverse( i3IKContext * pIKCtx)
{
	INT32 i;
	// Compute Singular Value Decomposition 
	//	This an inefficient way to do Pseudoinverse, but it is convenient since we need SVD anyway
	i3MatrixN::ComputeSVD( &m_matnJacobian, &m_matnU, &m_vnW, &m_matnV, &m_vnSuperDiag);
	
#if defined( I3_DEBUG)
	// Next line for debugging only
	//I3ASSERT( i3MatrixN::DebugCheckSVD( &m_matnJacobian, &m_matnU, &m_vnW, &m_matnV) == true);
#endif

	// Calculate response vector dTheta that is the DLS solution.
	//	Delta target values are the dS values
	//  We multiply by Moore-Penrose pseudo-inverse of the J matrix
	REAL32 pseudoInverseThreshold = m_rThresHoldFactor * i3VectorN::MaxAbs( &m_vnW);

	INT32 diagLength = m_vnW.getSize();
	REAL32 * wPtr = m_vnW.getValues();

	i3VectorN::FillZero( &m_vnDeltaTheta);

	for ( i = 0; i < diagLength; i++ )
	{		
		REAL32 dotProdCol = i3MatrixN::DotProductColumn( &m_matnU, &m_vnDeltaS, i);		// Dot product with i-th column of U
		REAL32 alpha = *(wPtr++);
		if( i3Math::abs( alpha) > pseudoInverseThreshold )
		{
			alpha = 1.f / alpha;
			i3MatrixN::AddArrayScale( m_matnV.getRowCount(), m_matnV.getColumnValues( i), 1, m_vnDeltaTheta.getValues(), 1, dotProdCol * alpha );
		}
	}

	// Scale back to not exceed maximum angle changes
	_SetAngleByLimitValue( &m_vnDeltaTheta, pIKCtx);
}

void i3IKSolver::_CalculateDLS( i3IKContext * pIKCtx)
{
	i3MatrixN::MultiplyTranspose( &m_matnU, &m_matnJacobian, &m_matnJacobian );		// U = J * (J^T)
	i3MatrixN::AddToDiagonal( &m_matnU, m_rDampingLambdaSq );
	
	// Use the next four lines instead of the succeeding two lines for the DLS method with clamped error vector e.
	// CalcdTClampedFromdS();
	// VectorRn dTextra(3*nEffector);
	// U.Solve( dT, &dTextra );
	// J.MultiplyTranspose( dTextra, dTheta );
	
	// Use these two lines for the traditional DLS method
	i3MatrixN::Solve( &m_matnU, &m_matnAugmented, &m_vnDeltaS, &m_vnDeltaT);
	i3MatrixN::MultiplyTranspose( &m_vnDeltaTheta, &m_matnJacobian, &m_vnDeltaT );

	// Scale back to not exceed maximum angle changes
	_SetAngleByLimitValue( &m_vnDeltaTheta, pIKCtx);
}

void i3IKSolver::_CalculateDLSwithSVD( i3IKContext * pIKCtx)
{
	INT32 i;
	// Compute Singular Value Decomposition 
	//	This an inefficient way to do DLS, but it is convenient since we need SVD anyway

	i3MatrixN::ComputeSVD( &m_matnJacobian, &m_matnU, &m_vnW, &m_matnV, &m_vnSuperDiag);
	
#if defined( I3_DEBUG)
	// Next line for debugging only
	I3ASSERT( i3MatrixN::DebugCheckSVD( &m_matnJacobian, &m_matnU, &m_vnW, &m_matnV) == true);
#endif

	// Calculate response vector dTheta that is the DLS solution.
	//	Delta target values are the dS values
	//  We multiply by DLS inverse of the J matrix
	INT32 diagLength = m_vnW.getSize();
	REAL32 * wPtr = m_vnW.getValues();

	i3VectorN::FillZero( &m_vnDeltaTheta);

	for( i = 0; i < diagLength; i++ )
	{		
		REAL32 dotProdCol = i3MatrixN::DotProductColumn( &m_matnU, &m_vnDeltaS, i);		// Dot product with i-th column of U
		REAL32 alpha = *(wPtr++);
		alpha = alpha / ((alpha*alpha) + m_rDampingLambdaSq);
		i3MatrixN::AddArrayScale( m_matnV.getRowCount(), m_matnV.getColumnValues( i), 1, m_vnDeltaTheta.getValues(), 1, dotProdCol * alpha );
	}

	// Scale back to not exceed maximum angle changes
	_SetAngleByLimitValue( &m_vnDeltaTheta, pIKCtx);
}

void i3IKSolver::_CalculateSDLS( i3IKContext * pIKCtx)
{
	INT32 i, j;
	// Compute Singular Value Decomposition 
	i3MatrixN::ComputeSVD( &m_matnJacobian, &m_matnU, &m_vnW, &m_matnV, &m_vnSuperDiag );

#if defined( I3_DEBUG)
	// Next line for debugging only
	//I3ASSERT( i3MatrixN::DebugCheckSVD( &m_matnJacobian, &m_matnU, &m_vnW, &m_matnV) == true);
#endif

	// Calculate response vector dTheta that is the SDLS solution.
	//	Delta target values are the dS values
	INT32 nRows = m_matnJacobian.getRowCount();
//	INT32 numEndEffectors = pIKCtx->getIKConstraintCount();		// Equals the number of rows of J divided by three
	INT32 nCols = m_matnJacobian.getColCount();

	i3VectorN::FillZero( &m_vnDeltaTheta);

	// Calculate the norms of the 3-vectors in the Jacobian
	const REAL32 *jx = m_matnJacobian.getValues();
	REAL32 *jnx = m_matnJacobianNorm.getValues();

	for( i = nCols; i > 0; i-- )
	{
		REAL32 accumSq = (*(jx)) * (*(jx));
		jx++;
		accumSq += (*(jx)) * (*(jx));
		jx++;
		accumSq += (*(jx)) * (*(jx));
		jx++;
		*(jnx++) = i3Math::Sqrt( accumSq);
	}

	// Clamp the dS values
	_CalcdTClampedFromdS();

	// Loop over each singular vector
	for (i = 0; i < nRows; i++ )
	{
		REAL32 wiInv = m_vnW.getValue( i);

		if( wiInv < 1.0e-10 )
		{
			continue;
		}

		wiInv = 1.f / wiInv;

		REAL32 N = 0.f;						// N is the quasi-1-norm of the i-th column of U
		REAL32 alpha = 0.f;					// alpha is the dot product of dT and the i-th column of U

		const REAL32 *dTx = m_vnDeltaT.getValues();
		const REAL32 *ux = m_matnU.getColumnValues( i);
		
		//for( j = 1; j > 0; j-- )
		{
			REAL32 tmp;

			alpha += (*ux)*(*(dTx++));
			tmp = ((*ux) * (*ux));
			ux++;
			alpha += (*ux)*(*(dTx++));
			tmp += ((*ux) * (*ux));
			ux++;
			alpha += (*ux)*(*(dTx++));
			tmp += ((*ux) * (*ux));
			ux++;
			N += i3Math::Sqrt( tmp);
		}

		// M is the quasi-1-norm of the response to angles changing according to the i-th column of V
		//		Then is multiplied by the wiInv value.
		REAL32 M = 0.f;
		REAL32 *vx = m_matnV.getColumnValues( i);
		jnx = m_matnJacobianNorm.getValues();

		for( j = nCols; j > 0; j-- )
		{
			REAL32 accum = 0.f;
			//for( k = numEndEffectors; k > 0; k-- )
			{
				accum += *(jnx++);
			}
			M += i3Math::abs( (*(vx++)) ) * accum;
		}
		M *= i3Math::abs( wiInv);
	
		REAL32 gamma = m_rMaxAngleSDLS;

		if( N < M )
		{
			gamma *= N/M;				// Scale back maximum permissable joint angle
		}

		// Calculate the dTheta from pure pseudoinverse considerations
		REAL32 scale = alpha * wiInv;			// This times i-th column of V is the psuedoinverse response

		i3VectorN::LoadScaled( &m_vnPrevTheta, m_matnV.getColumnValues( i), scale);

		// Now rescale the dTheta values.
		REAL32 max = i3VectorN::MaxAbs( &m_vnPrevTheta);
		REAL32 rescale = (gamma)/(gamma+max);

		i3VectorN::AddScaled( &m_vnDeltaTheta, &m_vnPrevTheta, rescale);
		/*if ( gamma<max) {
			dTheta.AddScaled( dPreTheta, gamma/max );
		}
		else {
			dTheta += dPreTheta;
		}*/
	}

	// Scale back to not exceed maximum angle changes

	REAL32 maxChange = i3VectorN::MaxAbs( &m_vnDeltaTheta);
	REAL32 * rValues = m_vnDeltaTheta.getValues();

	for( i = 0; i < pIKCtx->getIKBoneCount(); i++)
	{
		
		i3IKBoneInfo * pInfo = pIKCtx->getIKBoneInfo( i);
		
		// 회전축에 대한 리미트값까지 산출한다.
		if( maxChange > pInfo->getRotateLimit() )
		{
			*rValues *= pInfo->getRotateLimit() / (pInfo->getRotateLimit() + maxChange);
		}
	}
}

void i3IKSolver::_UpdateIKBone( I3_ANIM_MEMORY_SEQUENCE_INFO * pOut, I3_ANIM_MEMORY_SEQUENCE_INFO * pOriginal, i3AnimationContext2 * pAnimCtx, i3IKContext * pIKCtx)
{
	INT32 i;
	i3Bone * pBone;

	i3Skeleton * pSkel = pAnimCtx->getFrameSkeleton();

	for( i = 0; i < pIKCtx->getIKBoneCount(); i++)
	{
		i3IKBoneInfo * pInfo = pIKCtx->getIKBoneInfo( i);

		pBone = pSkel->getBone( pInfo->getBoneIndex());

		I3_ANIM_MEMORY_BONE_INFO * pOutInfo = pOut->getBoneInfo( pInfo->getBoneIndex());

		pOutInfo->addState( I3_ANIM_MEM_BONE_INFO_STATE_USED);

		if( pInfo->isFlag( I3_IKBONE_FLAG_FREEZE) == false)
		{
			I3ASSERT( pBone->getParentIndex() != 0);

			REAL32 rLimit = 0.f;

			if( pInfo->getFlag() & I3_IKBONE_FLAG_EFFECTOR)
			{
				// 이전 애니메이션에서 Rotation
				rLimit = pInfo->getRotationAngle();
			}
			else
			{
				// 변경하기 전의 Matrix를 저장한다. Child 본에서는 변경전의 위치에서 계산되어진다.
				rLimit = _CalculateLimitAngleForce( pSkel, pInfo, pAnimCtx->getTempMatrix( i),
					pAnimCtx->getTempMatrix( pBone->getParentIndex()), pIKCtx, m_vnDeltaTheta.getValue( pInfo->getIKJointCount()));
			}

			//if( rLimit > 0.f)
			{
				QUATERNION q, localQ;

				// 이전에 구한 WorldMatrix에서 Local Matrix를 구하자
				MATRIX invMtx;
				i3Matrix::Inverse( &invMtx, 0, pAnimCtx->getTempMatrix( pBone->getParentIndex()));

				MATRIX localMtx;
				i3Matrix::Mul( &localMtx, pAnimCtx->getTempMatrix( pInfo->getBoneIndex()), &invMtx);

				i3Matrix::GetRotateQuat( &localQ, &localMtx);

				i3Quat::RotateAxis( &q, pInfo->getRotateAxis(), rLimit);
				i3Quat::Mul( pOutInfo->getRotation(), &q, &localQ);
				pOutInfo->addState( I3MATH_KEY_ROTATE);

				i3Quat::Copy( pOriginal->getBoneInfo( pInfo->getBoneIndex())->getRotation(), pOutInfo->getRotation());
			}
		}
	}

	// WorldTransform to temp matrix
	i3MatrixStack stack;

	INT32 prevDepth;
	MATRIX * pMtx;
	MATRIX mtx;

	stack.SetBookmark();
	stack.PushIdentity();
	stack.Push( pAnimCtx->getTempMatrix( 0), pAnimCtx->getModelViewMatrixPreFrame());

	prevDepth = -1;

	for( i = 0; i < pIKCtx->getIKBoneCount(); i++)
	{
		i3IKBoneInfo * pInfo = pIKCtx->getIKBoneInfo( i);

		pBone = pAnimCtx->GetLocalBone( pInfo->getBoneIndex());

		I3ASSERT( pBone != NULL);

		INT32 iPopCount = (prevDepth - ((INT32)pBone->getDepth() - 1));
		prevDepth = pBone->getDepth();
		stack.Pop( iPopCount);

		I3_ANIM_MEMORY_BONE_INFO * pBoneInfo = pOut->getBoneInfo( pInfo->getBoneIndex());

		if( pBoneInfo->getState() & I3_ANIM_MEM_BONE_INFO_STATE_USED)
		{
			i3Matrix::Identity( &mtx);

			// Rotation Update
			i3Matrix::SetRotateQuat( &mtx, pBoneInfo->getRotation());

			pMtx = &mtx;
		}
		else
			pMtx = pAnimCtx->GetLocalMatrix( i);
		
		stack.PushPre( pAnimCtx->getTempMatrix( i), pMtx);
	}
}

void i3IKSolver::_UpdateFreezeBone( I3_ANIM_MEMORY_SEQUENCE_INFO * pOut, i3AnimationContext2 * pAnimCtx, i3IKContext * pIKCtx)
{
	INT32 i;

	for( i = 0; i < pIKCtx->getIKBoneCount(); i++)
	{
		i3IKBoneInfo * pInfo = pIKCtx->getIKBoneInfo( i);

		I3_ANIM_MEMORY_BONE_INFO * pOutInfo = pOut->getBoneInfo( pInfo->getBoneIndex());

		pOutInfo->addState( I3_ANIM_MEM_BONE_INFO_STATE_USED);
	
		if( pInfo->isFlag( I3_IKBONE_FLAG_FREEZE) )
		{
			if( pInfo->getPickedBoneIndex() != -1)
			{
				I3_ANIM_MEMORY_BONE_INFO *pPickedInfo = pOut->getBoneInfo( pInfo->getPickedBoneIndex());
				pPickedInfo->CopyTo( pOutInfo);
				pOutInfo->addState( I3MATH_KEY_ROTATE);
			}
		}
	}
}

void i3IKSolver::SetRenderableAxis( i3IKContext * pIKCtx, i3IKBoneInfo * pIKInfo, MATRIX * pLocalParentMat, MATRIX * pLocalMat, REAL32 rAngle)
{

	I3ASSERT( pIKCtx != NULL);

	MATRIX matDefault;

	if( pIKCtx->getIKRotateAxisList() != NULL)
	{
		i3Matrix::Copy( &matDefault, pLocalParentMat);
		
		for(size_t i = 0; i < pIKCtx->getIKRotateAxisList()->size(); i++)
		{	
			i3BoneRef * pBoneRef = (i3BoneRef*) (*pIKCtx->getIKRotateAxisList())[i];

			if( (pBoneRef != NULL) &&
				(pBoneRef->getBoneIndex() == pIKInfo->getBoneIndex()) )
			{
				i3Node * pChild;
				i3Transform * pTrans;

				pChild = pBoneRef->GetChild( 0);
				pTrans = (i3Transform*) pChild->GetChild( 0);
				if( pTrans != NULL)
				{
					pTrans->SetMatrix( &matDefault);
				}

				MATRIX preMat;
				i3Matrix::Copy( &preMat, &matDefault);

				// Z-Axis
				
				pChild = pBoneRef->GetChild( 1);
				pTrans = (i3Transform*) pChild->GetChild( 0);
				if( pTrans != NULL)
				{	
					i3Matrix::PreRotateZ( &matDefault, pIKInfo->getLimitTwistLow());
					pTrans->SetMatrix( &matDefault);
					
				}

				pChild = pBoneRef->GetChild( 2);
				pTrans = (i3Transform*) pChild->GetChild( 0);
				if( pTrans != NULL)
				{
					i3Matrix::PreRotateZ( &matDefault, (pIKInfo->getLimitTwistHigh() - pIKInfo->getLimitTwistLow()));
					pTrans->SetMatrix( &matDefault);
				}

				i3Matrix::Copy( &matDefault, &preMat);

				// Y-Axis
				pChild = pBoneRef->GetChild( 3);
				pTrans = (i3Transform*) pChild->GetChild( 0);
				if( pTrans != NULL)
				{
					i3Matrix::PreRotateY( &matDefault, pIKInfo->getLimitSwing2Low());
					pTrans->SetMatrix( &matDefault);
				}

				pChild = pBoneRef->GetChild( 4);
				pTrans = (i3Transform*) pChild->GetChild( 0);
				if( pTrans != NULL) 
				{
					i3Matrix::PreRotateY( &matDefault, (pIKInfo->getLimitSwing2High() - pIKInfo->getLimitSwing2Low()) );
					pTrans->SetMatrix( &matDefault);
				}

				i3Matrix::Copy( &matDefault, &preMat);

				// X-Axis
				pChild = pBoneRef->GetChild( 5);
				pTrans = (i3Transform*) pChild->GetChild( 0);
				if( pTrans != NULL)
				{
					i3Matrix::PreRotateX( &matDefault, pIKInfo->getLimitSwing1Low());
					pTrans->SetMatrix( &matDefault);
				}

				pChild = pBoneRef->GetChild( 6);
				pTrans = (i3Transform*) pChild->GetChild( 0);
				if( pTrans != NULL)
				{
					i3Matrix::PreRotateX( &matDefault, (pIKInfo->getLimitSwing1High() - pIKInfo->getLimitSwing1Low()) );
					pTrans->SetMatrix( &matDefault);
				}

				break;
			}
		}
	}
}

void i3IKSolver::_RestoreIKMatrix( i3AnimationContext2 * pAnimCtx, i3IKContext * pIKCtx)
{
	INT32 i, prevDepth;
	MATRIX mat;
	MATRIX * pIn;
	i3IKBoneInfo * pIKInfo;
	i3Bone * pBone;

	I3ASSERT( pAnimCtx != NULL);

	i3MatrixStack * pStack = pAnimCtx->getMatrixStack();
	
	i3Skeleton * pSkel = pAnimCtx->getFrameSkeleton();

	I3ASSERT( pSkel != NULL);

	pStack->SetBookmark();

	prevDepth = -1;

	for( i = 0; i < pSkel->getBoneCount(); i++)
	{
		pBone = pSkel->getBone( i);

		INT32 iPopCount = (prevDepth - ((INT32)pBone->getDepth() - 1));
		pStack->Pop( iPopCount);

		i3Matrix::Identity( &mat);

		// IKBone 이면 업데이트
		bool bIKChild = (bool)((pBone->getFlag() & I3_IKBONE_FLAG_CHILD) == I3_IKBONE_FLAG_CHILD);
		bool bIncludeIK = pSkel->CheckIncludeIKContext( pIKCtx, i, i, NULL);

		if( pBone->getFlag() & I3_BONEFLAG_IKBONE)
		{
			pIKInfo = pSkel->getIKBoneInfoByBoneIdx( pIKCtx, i);
			I3ASSERT( pIKInfo != NULL);

			if( pIKInfo->isFirstFrame() )
			{
				// 초기화하는 frame이면 이전 애니메이션 matrix를 그대로 쓴다.
				pStack->Push( pAnimCtx->getCacheMatrix( i), NULL);
				
				pIKInfo->setFirstFrameValue( false);
			}
			else
			{
				pIn = pBone->getMatrix();

				if( _GetAnimatedMatrix( &mat, pAnimCtx, i) == true)
				{
					pIn = &mat;
				}

				// Animation 후 이전 frame의 회전량을 더한다.
				i3Matrix::Mul( &mat, pIKInfo->getNextFrameRotate(), pIn);

				pStack->PushPre( pAnimCtx->getCacheMatrix( i), pIn);
			}
		}
		else if( bIKChild && bIncludeIK )
		{
			pIn = pBone->getMatrix();

			// Animation에서 Transform을 가져온다.
			if( _GetAnimatedMatrix( &mat, pAnimCtx, i) == true)
			{
				pIn = &mat;
			}

			// 상위 bone의 IK된 상태에서 Animation된 matrix를 더한다.
			pStack->PushPre( pAnimCtx->getCacheMatrix( i), pIn);
		}
		else
		{
			pStack->Push( pAnimCtx->getCacheMatrix( i), NULL);
		}

		prevDepth = pBone->getDepth();
	}

	pStack->RestoreBookmark();
	
}

bool i3IKSolver::_GetAnimatedMatrix( MATRIX * pOut, i3AnimationContext2 * pAnimCtx, INT32 iBoneIdx)
{
	I3ASSERT( pOut != NULL);
	I3ASSERT( pAnimCtx != NULL);

	i3TransformSourceCombiner * pCombiner;

	i3MatrixStack * pStack = pAnimCtx->getMatrixStack();

	if( pAnimCtx->getCombinerCount() > 0)
	{
		pCombiner = pAnimCtx->getCombiner( iBoneIdx);

		if( pCombiner->getAnimationCount() > 0)
		{
			I3MATRIXINFO Env;

			i3mem::FillZero( &Env, sizeof( Env));

			Env.m_FrameID = pAnimCtx->getLastUpdateFrameID();
			Env.m_Time = pAnimCtx->getLocalTime();
			Env.m_pIn = Env.m_pOut = pOut;
			Env.m_pModelViewMatrix = pStack->GetTop();

			pCombiner->GetMatrix( &Env);

			return true;
		}
	}

	return false;
}



