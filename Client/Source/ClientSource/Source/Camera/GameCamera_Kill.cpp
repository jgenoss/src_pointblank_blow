#include "pch.h"
#include "GameCamera_Kill.h"
#include "GlobalVariables.h"
#include "GameCharaBoneContext.h"
#include "GameCharaCollisionContext.h"

#include "GameCharaContexts.h"
#include "BattleSlotContext.h"

I3_CLASS_INSTANCE( CGameCamera_Kill);

//#define	KILLCAM_ROLL
#define	KILLCAM_SUDDEN
#define CAM_FREEZETIME	3.f	// 2.f

void CGameCamera_Kill::InitCamera(CGameCharaBase * pChara, REAL32 rInterpolateTime)
{
	CGameCamera::InitCamera(pChara, rInterpolateTime);

	m_bFPS = true;

	if( m_pTargetChara != nullptr)
	{
		m_pTargetChara->Cmd_ChangeView( CHARA_VIEW_3PV, false);

		if (m_pTargetChara->IsEnabledOccludee() == false)
		{
			m_pTargetChara->EnableOccludee(true);
			m_pTargetChara->EnableRenderAABB(true);

			if (m_pTargetChara->GetCurrentCharaWeapon())
			{
				m_pTargetChara->GetCurrentCharaWeapon()->EnableRenderAABB(true);
			}
		}

		//m_pTargetChara->ChangeObserverView( false);
	}

	m_bEnabledAutoCam		= false;
	m_rRotation				= 0.0f;
	m_rElapsedNoCollision	= 0.0f;
	i3Vector::Zero( &m_vDamagedDir);

	m_rFreezeTime = 0.0f;	// 데쓰켐 멈춤 타이머(캐릭터따라 떨리는거 방지용)
	m_bFreeze			= false;

	m_rCollisionTimer = 0.f;

/*
#if defined ( KILLCAM_SUDDEN)
	{
		MATRIX * pMat = m_pCamMatrix;

		REAL32	Theta;
		VEC3D	vAt, vPos;

		i3Vector::Copy( &vPos, i3Matrix::GetPos( pMat));
		i3Vector::Copy( &vAt, i3Matrix::GetAt( m_pTargetMatrix));

		Theta = atan2f( getX( &vAt), getZ( &vAt));
		i3Matrix::SetRotateY( pMat, Theta + I3_DEG2RAD( 180.0f));
		i3Matrix::SetPos( pMat, &vPos);
	}

	i3Matrix::Copy( &m_mtxInterpolateTarget, m_pCamMatrix);

	SetInterPolate( rInterpolateTime);
#endif*/
}

void CGameCamera_Kill::OnInputKeyboard( UINT64 nKeyPress, UINT64 nKeyStroke)
{
#if defined ( KILLCAM_ROLL)
#elif defined( KILLCAM_SUDDEN)
	if( m_bEnabledInterpolate == false)
	{
		CGameCamera::OnInputKeyboard( nKeyPress, nKeyStroke);

		if (m_bEnabledAutoCam && (i3Vector::Length( &m_vMouseDelta) > I3_EPS))
		{
			SetEnableAutoCam( false);
		}
	}
#endif
}

void CGameCamera_Kill::SetEnemyIdx(INT32 nIdx)
{
	m_nEnemyIdx = nIdx;
}

INT32 CGameCamera_Kill::GetEnemyIdx(void)
{
	return m_nEnemyIdx;
}

void CGameCamera_Kill::SetEnableAutoCam( bool bEnable)
{
	m_bEnabledAutoCam = bEnable;

	i3Vector::Zero( &m_vMouseDelta);
	m_nMouseState	= 0;
}

void CGameCamera_Kill::CalcDirection()
{
	if( g_pCharaManager != nullptr)
	{	
		const GLOBAL_CHARA_INFO * pInfo = g_pCharaManager->getCharaInfo( m_nEnemyIdx);
		if( pInfo->GetType() != CHARA::TYPE_NA)
		{
			if( m_pTargetChara != nullptr)
			{
				CGameCharaBase * pChara = m_pTargetChara;//g_pCharaManager->m_pChara[m_nEnemyIdx];
				if( pChara != nullptr)
				{
					i3Skeleton * pSkel = pChara->getSceneObject()->GetBody()->getFrameLOD()->getSkeleton();
					I3ASSERT( pSkel != nullptr);

					INT32 boneCnt = pSkel->getBoneCount();
					for( INT32 i = 0; i < boneCnt; i++ )
					{
						i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i );

						if( pShapeSet == nullptr )		continue;
						if( pChara->getCollisionContext()->GetHitPart( pShapeSet ) != CHARA_HIT_ROOT )	continue;

						VEC3D vDir;
						MATRIX mtx;

						pShapeSet->getShapeGlobalPose( &mtx);

						i3Vector::Sub( &vDir, i3Matrix::GetPos( m_pCamMatrix), i3Matrix::GetPos( &mtx));
						addY( &vDir, - 1.3f);

						SetDamagedDirection( &vDir);
						return;
					}
					//	로컬 좌표로 변경하여 방향을 구합니다.
				}
			}
		}
		else
		{
			
		}
	}
	else
	{
		I3TRACE("INVALID:\n");
		i3Matrix::Copy( &m_mtxInterpolateTarget, m_pCamMatrix);
		return;
	}
}

void CGameCamera_Kill::SetDamagedDirection( VEC3D * pDamagedDir)
{
	i3Vector::Copy( &m_vDamagedDir, pDamagedDir);
	i3Vector::Normalize( &m_vDamagedDir, &m_vDamagedDir);

	MATRIX * pMat = &m_mtxInterpolateTarget;

	REAL32	Theta;
	VEC3D	vAt;//, vPos;
	//	MATRIX	* pIKMatrix;
	Theta = atan2f( getX( &m_vDamagedDir), getZ( &m_vDamagedDir));

	i3Matrix::SetRotateY( pMat, Theta);

	//카메라의 위치를 케릭터의 뒷방향으로 이동
	REAL32 fLen;
	fLen = i3Math::Sqrt( (getX(&m_vDamagedDir) * getX(&m_vDamagedDir)) + (getZ(&m_vDamagedDir) * getZ(&m_vDamagedDir)) );
	fLen = atan2f(getY(&m_vDamagedDir), fLen);

	i3Vector::Normalize( &vAt, i3Matrix::GetRight( pMat));
	i3Matrix::PostRotateAxis( pMat, &vAt, - fLen);

	m_rRotation = fLen;
	i3Matrix::SetPos( pMat, i3Matrix::GetPos(m_pCamMatrix));
}

void CGameCamera_Kill::OnEndInterpolate()
{
#if defined( KILLCAM_ROLL)
#elif defined( KILLCAM_SUDDEN)
	i3Vector::Zero( &m_vMouseDelta);	//현재 프레임에서의 마우스 회전량 
	m_nMouseState = 0;
#endif
}

#define KILLCAM_ROTATION 60.0f

void CGameCamera_Kill::OnUpdate( REAL32 rDeltaSeconds)
{
	CGameCamera::OnUpdate( rDeltaSeconds);

	UpdateInterpolate( rDeltaSeconds);
	
#if defined ( KILLCAM_ROLL)
	if( m_bCollision)
	{
		m_rElapsedNoCollision = 0.0f;
		m_rRotation += ( KILLCAM_ROTATION * rDeltaSeconds);
		if( m_rRotation >= 360.0f)
		{
			m_rRotation -= 360.0f;
		}
	}
	else
	{
		m_rElapsedNoCollision += rDeltaSeconds;
		if( m_rElapsedNoCollision < 0.5f)
		{
			m_rRotation += ( KILLCAM_ROTATION * rDeltaSeconds);
		}
	}
#endif

	//if( m_rFreezeTime == 0.f )
	if( m_bFPS )
	{
		if( getCurTarget() != nullptr )//&& getCurTarget()->isContact() )
			m_rFreezeTime += rDeltaSeconds;
	}

	m_rCollisionTimer += rDeltaSeconds;
}

void CGameCamera_Kill::ReCalculateMatrix()
{
	if( m_bNeedUpdate == false)	return;

	//GlobalFunc::PB_TRACE("CGameCamera_Kill::ReCalculateMatrix m_rFreezeTime %f m_bFPS %d \n", m_rFreezeTime, m_bFPS);
	// KILLCAM_FPS
	if( m_rFreezeTime < CAM_FREEZETIME )
	{
		MATRIX mTarget;
		//MATRIX mTarget1;
		
		if( (m_pTargetChara != nullptr))
		{
			// BONE_HEAD는 너무 흔들린다
			i3Matrix::Copy(&mTarget, m_pTargetChara->getBoneContext()->getBoneMatrix(BONE_IK));	
			i3Vector::Copy(&m_vLastTargetPos, i3Matrix::GetPos( m_pTargetChara->getBoneContext()->getIKMatrix()));
			
			/*i3Matrix::Copy( &mTarget, m_pTargetChara->GetMatrix() );
			mTarget.m30 = m_vLastTargetPos.x;
			mTarget.m31 = m_vLastTargetPos.y;
			mTarget.m32 = m_vLastTargetPos.z;*/

			// 월드 축에 맞춤
			i3Matrix::PreRotateX(&mTarget, I3_PI2);
			i3Matrix::PreRotateZ(&mTarget, -I3_PI2);	

			// 일단 적용
			i3Matrix::Copy(m_pCamMatrix, &mTarget);

			if( m_pTargetChara->isCharaStateMask( CHARA_STATE_CAMFREEZE) )
			{
				m_rFreezeTime = CAM_FREEZETIME;
			}
			//GlobalFunc::PB_TRACE("Right %f %f %f", m_pCamMatrix->m00, m_pCamMatrix->m01, m_pCamMatrix->m02);
			//GlobalFunc::PB_TRACE("Up    %f %f %f", m_pCamMatrix->m10, m_pCamMatrix->m11, m_pCamMatrix->m12);
			//GlobalFunc::PB_TRACE("At	%f %f %f", m_pCamMatrix->m20, m_pCamMatrix->m21, m_pCamMatrix->m22);
			//GlobalFunc::PB_TRACE("Pos   %f %f %f", m_pCamMatrix->m30, m_pCamMatrix->m31, m_pCamMatrix->m32);
		}
	}
	else
	{
		m_bFreeze = true;

		if (m_bFPS)
		{
			SetEnableAutoCam( true);
			SetInterPolate( 5.0f);

            i3Matrix::Copy(&m_mtxInterpolateTarget,m_pCamMatrix);		

			if( m_pTargetChara != nullptr )
			{
				i3Vector::Copy(&m_vLastTargetPos, i3Matrix::GetPos( m_pTargetChara->getBoneContext()->getIKMatrix()));
			}

			if( m_pTargetChara != nullptr )
			{	// 캐릭터를 보이게 하고
				m_pTargetChara->SetEnable();

				// 물리값을 유지합니다.
				m_pTargetChara->Cmd_SetPhysics();
			}

			m_bFPS = false;
		}
		else
		{
			m_bEnabledInterpolate = false;

			REAL32 rDistance = m_vBasePosition->z;
			//REAL32 rTheta = I3_PI4/2;			// 각도: pef에서 참조

			// 자신을 죽인 캐릭터의 위치를 구한다
			VEC3D vEnemyPos;

			if (m_bEnabledAutoCam)
			{				
				CGameCharaBase* pChara = g_pCharaManager->getCharaByNetIdx( m_nEnemyIdx );
				
				if (pChara && (m_nEnemyIdx != BattleSlotContext::i()->getMySlotIdx()) )
				{
					i3Vector::Copy(&vEnemyPos, i3Matrix::GetPos( pChara->getBoneContext()->getIKMatrix()));
				}
				else
				{
					i3Vector::Copy(&vEnemyPos, &m_vLastTargetPos);
				}

				i3Vector::Copy(&m_vLastEnemyPos, &vEnemyPos);

				// 기획팀 요청으로 자동 추적하는 기능을 끕니다. 2008.11.11
				//SetEnableAutoCam( false);
			}
			else
			{
				i3Vector::Copy(&vEnemyPos, &m_vLastEnemyPos);
			}

			// 자신과 자신을 죽인 캐릭터가 모두 보이는 쪽으로 카메라 이동
			MATRIX mTarget;
			VEC3D vAt;

			// EnemyDummy
			if (i3Vector::IsSame(&vEnemyPos, &m_vLastTargetPos))
			{
				vEnemyPos.x = 0.0f;
				vEnemyPos.y = 0.0f;
				vEnemyPos.z = 0.0f;
			}

			// 배후 방향은 평면(x,z)에서 계산
			i3Vector::Sub(&vAt, &vEnemyPos, &m_vLastTargetPos);
							
			// rDistance와 rTheta 적용
			VEC3D vPlaneAt;
			i3Vector::Copy(&vPlaneAt, &vAt);
			i3Vector::SetY(&vPlaneAt, 0.0f);
			i3Vector::Normalize(&vAt, &vAt);
			i3Vector::Normalize(&vPlaneAt, &vPlaneAt);
			
			VEC3D vAxis;
			if (vAt.y >= 0.0f)
			{
				i3Vector::Cross(&vAxis, &vAt, &vPlaneAt);									
			}
			else
			{
				i3Vector::Cross(&vAxis, &vPlaneAt, &vAt);
			}	
            
            i3Vector::Scale(&vPlaneAt, &vPlaneAt, rDistance);
			
			i3Matrix::SetPos(&mTarget, &vPlaneAt);
			
			VEC3D vPos;
			i3Vector::Sub(&vPos, &m_vLastTargetPos, i3Matrix::GetPos(&mTarget));
			
			i3Matrix::Identity(&mTarget);
			i3Matrix::SetPos(&mTarget, &vPos);
			i3Vector::AddY(i3Matrix::GetPos(&mTarget), m_vBasePosition->y);

			// 카메라 At방향은 Enemy
			if (m_bEnabledAutoCam)
			{
				m_rXTheta = atan2(vAt.x, vAt.z);
				//m_rYTheta = atan2(vAt.y, vAt.x);
				REAL32 fLen;
				fLen = i3Math::Sqrt( (vAt.x * vAt.x) + (vAt.z * vAt.z));
				m_rYTheta = atan2f(vAt.y, fLen);
				
				// 예각 계산
				if (m_rYTheta > I3_PI2)
				{
					m_rYTheta = I3_PI - m_rYTheta;
				}
				else if (m_rYTheta < -I3_PI2)
				{
					m_rYTheta = I3_PI + m_rYTheta;
				}
			}
			else
			{					
				// 마우스 조작이 필요한 GUI가 활성화 상태이면, 카메라 조작을 막는다.
				if (GAMEKEY_MODE_GAME == g_pFramework->getGameKeyMode())
				{
					m_rXTheta -= I3_DEG2RAD(i3Vector::GetX(g_pFramework->getMouseMoveDelta()) * g_pConfig->m_rMouseSensivity);
					m_rYTheta -= I3_DEG2RAD(i3Vector::GetY(g_pFramework->getMouseMoveDelta()) * g_pConfig->m_rMouseSensivity);
				}
			}

			m_rYTheta = i3Math::Min(m_rYTheta, I3_PI/3);
			m_rYTheta = i3Math::Max(m_rYTheta, -I3_PI4);

			i3Matrix::PreRotateY(&mTarget, -(I3_PI - m_rXTheta));
			i3Matrix::PreRotateX(&mTarget, m_rYTheta);

			//if( m_rCollisionTimer > 0.1f )
			{
				i3CollideeLine line;
				
			//	VEC3D vPos;
				i3Vector::Copy(&vPos, i3Matrix::GetPos(&mTarget));

				line.SetStart( &m_vLastTargetPos);
				line.SetEnd( &vPos);
				
				I3_PHYSIX_HIT_RESULT* pResult;

 				pResult = g_pFramework->raycastClosest(&line, CGRP_MASKGROUP_ATTACK);
				
				if( pResult != nullptr)
				{
					VEC3D vecInter;
					
					i3Vector::Sub( &vecInter, &pResult->m_Intersect, line.GetStart());
					
					VEC3D vecLine;
					
					i3Vector::Sub(&vecLine, &vPos, &m_vLastTargetPos);

					/*VEC3D vecFull;

					i3Vector::Sub( &vecFull, line.GetStart(), &pResult->m_Intersect);*/
									
					if( pResult->m_T < i3Vector::Length(&vecLine))
					{
						vecInter.x = i3Math::Mul(vecInter.x, 0.5f);
						vecInter.y = i3Math::Mul(vecInter.y, 0.5f);
						vecInter.z = i3Math::Mul(vecInter.z, 0.5f);

						i3Vector::Add( &vPos, &m_vLastTargetPos, &vecInter);

						if( vecInter.y < 0.5f)
						{	// 너무 낮다. 위로 올리자
							VEC3D vDown;
							i3Vector::Set( &vDown, getX( &vPos), -1.f, getZ( &vPos));
							line.SetStart( &vPos);
							line.SetEnd( &vDown);

							pResult = g_pFramework->raycastClosest(&line, CGRP_MASKGROUP_ATTACK);
							if( pResult != nullptr)
							{
								VEC3D vecLength;
								i3Vector::Sub(&vecLength, &vPos, &pResult->m_Intersect);
								REAL32 length = i3Vector::Length(&vecLength);
								if(length <= 2.0f)
									setY( &vPos, getY( &pResult->m_Intersect) + 1.f);
								else
									setY( &vPos, getY(&vPos) + 1.f);
							}
						}
					}

					i3Matrix::SetPos(&mTarget, &vPos);
				}

				m_rCollisionTimer = 0.f;
			}

			// 최종 카메라 위치 설정
			i3Matrix::Copy(m_pCamMatrix, &mTarget);
		}


        // SeeKiller
		//if( m_bEnabledAutoCam )
		//{
		//	CalcDirection();

		//	i3Matrix::Copy( m_pCamMatrix, &m_mtxInterpolateTarget);
		//}
		//else
		//{
		//	i3Matrix::Copy( m_pCamMatrix, &m_mtxInterpolateTarget);

		//	if( m_bEnabledInterpolate == false)
		//	{
		//		//임시
		//		REAL32 rRho = I3_DEG2RAD(getY( &m_vMouseDelta) * g_pConfig->m_rMouseSensivity);
		//		if( rRho > I3_DEG2RAD( 60.0f))
		//		{
		//			rRho = I3_DEG2RAD( 60.0f);
		//			setY( &m_vMouseDelta, 60.0f / g_pConfig->m_rMouseSensivity );
		//		}
		//		else if( rRho < I3_DEG2RAD( -45.0f))
		//		{
		//			rRho = I3_DEG2RAD( -45.0f);
		//			setY( &m_vMouseDelta, -45.0f / g_pConfig->m_rMouseSensivity );
		//		}

		//		PivotRotate( m_pCamMatrix, I3_DEG2RAD( getX( &m_vMouseDelta) * g_pConfig->m_rMouseSensivity), rRho);
		//	}
		//}
	}

//#if defined ( KILLCAM_ROLL)
//	m_bCollision = false;
//	//////////////////////////////////////////////////////////////////////////////
//	MATRIX mtx;
//	VEC3D  vAt, vTemp;
//	REAL32 Theta;
//
//	MATRIX	*pIKMtx		= m_pTargetChara->getIKMatrix();
//	VEC3D	*vPointPos	= i3Matrix::GetPos( pIKMtx);
//
//	//Calculate EndPos
//	MATRIX mtxTemp;
//	i3Matrix::Copy( &mtxTemp, m_pTargetMatrix);
//	i3Matrix::PreRotateYDeg( &mtxTemp, m_rRotation);
//
//	i3Vector::Normalize( &vAt, i3Matrix::GetAt( &mtxTemp));
//
//	Theta = atan2f( getX( &vAt), getZ( &vAt));
//	i3Matrix::SetRotateY( &mtx, Theta + I3_DEG2RAD( 180.0f));
//
//	i3Vector::TransformCoord( &vTemp, &m_vBasePosition, &mtx);
//
//	//카메라의 위치를 케릭터의 뒷방향으로 이동
//	REAL32 fLen;
//	fLen = i3Math::Sqrt( (getX(&m_vBasePosition) * getX(&m_vBasePosition)) + (getZ(&m_vBasePosition) * getZ(&m_vBasePosition)) );
//	fLen = atan2f(getY(&m_vBasePosition), fLen);
//
//	//카메라의 위치를 케릭터의 뒷방향으로 이동
//	i3Matrix::Copy( m_pCamMatrix, &mtx);
//	i3Matrix::PostRotateAxis( m_pCamMatrix, i3Matrix::GetRight( &mtxTemp), fLen);
//	i3Vector::Add( i3Matrix::GetPos( m_pCamMatrix), &vTemp, vPointPos);
//
//	/////////////////////////////////////////////////////////////////////////////
//	//CheckCollision
//	i3CollideeLine Col;
//	VEC3D ve, vOriginal;
//
//	i3Vector::Add( &ve, &vTemp, vPointPos);
//
//	Col.SetStart( vPointPos);
//	Col.SetEnd( &ve);
//
//	i3CollisionContext * pCtx = g_pFramework->getCollisionContext();
//	if( pCtx != nullptr)
//	{
//		if( pCtx->FAKE_CollisionTest( &Col, CGRP_CHARA_LINE, I3_COLLISION_STYLE_NEAREST_ONE) > 0)
//		{
//			FAKE_I3_COLLISION_RESULT * pResult = pCtx->GetResult();
//
//			VEC3D vecInter;
//			i3Vector::Sub( &vecInter, Col.GetEnd(), &pResult->m_Intersect);
//
//			i3Vector::Copy( &vOriginal, &m_vBasePosition);
//			REAL32 rLength = i3Vector::Length( &m_vBasePosition);
//			REAL32 rLength2= i3Vector::Length( &vecInter);
//
//			i3Vector::Normalize( &vOriginal, &m_vBasePosition);
//			i3Vector::Scale( &vOriginal, &vOriginal, rLength - (rLength2 + 0.1f));
//
//			i3Vector::TransformCoord( &vTemp, &vOriginal, &mtx);
//
//			//카메라의 위치를 케릭터의 뒷방향으로 이동
//			REAL32 fLen;
//			fLen = i3Math::Sqrt( (getX(&vOriginal) * getX(&vOriginal)) + (getZ(&vOriginal) * getZ(&vOriginal)) );
//			fLen = atan2f(getY(&vOriginal), fLen);
//
//			//카메라의 위치를 케릭터의 뒷방향으로 이동
//			i3Matrix::Copy( m_pCamMatrix, &mtx);
//			i3Matrix::PostRotateAxis( m_pCamMatrix, i3Matrix::GetRight( &mtxTemp), fLen);
//			i3Vector::Add( i3Matrix::GetPos( m_pCamMatrix), &vTemp, vPointPos);
//
//			m_bCollision = true;
//		}
//	}
//#elif defined( KILLCAM_SUDDEN)

   
	//// 죽는 모습 위에서 바라보는 카메라
	//{		
	//	m_bEnabledInterpolate = false;

	//	i3Matrix::SetRotateX(m_pCamMatrix, -I3_PI2);
	//	i3Matrix::SetPos(m_pCamMatrix, i3Matrix::GetPos(m_pTargetChara->getBoneMatrix(BONE_IK)));	

	//	i3Vector::AddY(i3Matrix::GetPos(m_pCamMatrix), 4.0f);

	//	VEC3D vBone;
	//	i3Vector::Copy(&vBone, i3Matrix::GetPos(m_pTargetChara->getBoneMatrix(BONE_IK)));
	//	VEC3D vTarget;
	//	i3Vector::Copy(&vTarget, m_pTargetChara->GetPos());

	//	//I3PRINTF("bone(%f, %f, %f),  chara(%f, %f, %f)\n", vBone.x, vBone.y, vBone.z, vTarget.x, vTarget.y, vTarget.z);
	//}

//#endif
	
 	CGameCamera::ReCalculateMatrix();	
}
