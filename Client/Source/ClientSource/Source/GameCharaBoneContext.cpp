#include "pch.h"
#include "GlobalVariables.h"
#include "GunInfo.h"
#include "GameCharaContexts.h"
#include "BattleSlotContext.h"
#include "Designation/DesigApplyInfo.h"


I3_CLASS_INSTANCE( CGameCharaBoneContext);//, CGameCharaContextBase);

CGameCharaBoneContext::CGameCharaBoneContext()
{
	setRho(0.f);
	setTheta(0.f);
}

CGameCharaBoneContext::~CGameCharaBoneContext()
{
	OnDestroy();

	I3MEM_SAFE_FREE(m_idxBone);
	I3MEM_SAFE_FREE(m_Bone);
	I3_SAFE_RELEASE( m_pEyeDummyLeft);
	I3_SAFE_RELEASE( m_pEyeDummyRight);
	I3_SAFE_DELETE(  m_mtxPivot);
}

REAL32 CGameCharaBoneContext::getViewRho()
{
	WeaponBase * pWeapon = getCurrentWeapon();
	if( pWeapon == nullptr )	return getRho();
	return getRhoValue() + I3_DEG2RAD((m_RecoilData.m_RecoilAngle * pWeapon->getCamSyncRate()) + m_ScopeWaveData.m_ScopeWaveRho);
}

void CGameCharaBoneContext::OnCreate( CGameCharaBase * pOwner, INT32 defaultView, INT32 defaultNetwork)
{
	m_pBody = pOwner->getSceneObject()->GetBody();

	CGameCharaContextBase::OnCreate( pOwner, defaultView, defaultNetwork);

	if( pOwner->getCharaInfo()->GetType() == CHARA::TYPE_NPC)
		return;

	I3ASSERT(m_idxBone == nullptr);
	I3ASSERT(m_Bone == nullptr);

	if(pOwner->IsDino())
	{	//렙터스팅
		if( pOwner->IsRaptorBase())					m_iBoneMax = RAPTOR_BONE_MAX;
		else if( pOwner->IsRex())					m_iBoneMax = TREX_BONE_MAX;
#ifdef DOMI_DINO
		else if (pOwner->IsDomiRaptorBoneBase())	m_iBoneMax = DOMI_RAPTOR_BONE_MAX;
		else if( pOwner->IsDomiMutantRex())			m_iBoneMax = DOMI_MUTANTREX_BONE_MAX;
#endif
		else	I3ASSERT_0;
	}
	else						m_iBoneMax = BONE_MAX;

	m_idxBone = (INT32*)i3MemAlloc(sizeof(INT32)*m_iBoneMax);
	m_Bone = (MATRIX*)i3MemAlloc(sizeof(MATRIX)*m_iBoneMax);

	__InitBone();

	if( (m_pBody->getLOD( 0) != nullptr) &&
		(m_pBody->getLOD( 1) != nullptr) )
	{
		if( m_pBody->getLOD( 0)->getBoneCount() != m_pBody->getLOD( 1)->getBoneCount())
		{
			addBoneFlag( CHARA_BONE_FLAG_DIFF_BONECOUNT);
		}
	}
}

void CGameCharaBoneContext::OnReset( void)
{
	CGameCharaContextBase::OnReset();

	m_CharaBoneFlag = 0;

	setTheta(0.0f);
	setRho(0.0f);
	
	m_RecoilData.Reset();

	//	Pivot
	
	m_PivotYaw				= 0.f;
	m_PivotYawTarget		= 0.f;
	m_timeYawBlend			= 0.f;
	m_timeTargetYawBlend	= 0.f;
	m_PivotRoll				= 0.f;
	i3Vector::Zero( &m_PivotAxis);
	i3Matrix::Identity( m_mtxPivot);
	
	m_velShove			= 0.f;
	m_acclShove			= 0.f;
	m_tShove			= 0.f;
	m_timeShove			= 0.f;
	i3Quat::Identity( &m_quatShoveTarget);
	i3Quat::Identity( &m_quatShoveStart);
	i3Quat::Identity( &m_quatShove);
	i3Matrix::Identity( &m_ShoveMatrix);

	m_HeadInterpolateTime	= 0.f;
	m_HeadTheta			= 0.f;
	m_HeadRho			= 0.f;
	m_HeadTargetTheta	= 0.f;
	m_HeadTargetRho		= 0.f;
	m_HeadWatchUserIdx	= -1;

	m_AimStartTheta		= 0.f;
	m_AimTheta			= 0.f;
	m_AimTime			= 0.f;
	m_AimInterpolate	= 0.f;
}

void CGameCharaBoneContext::OnChange1PV( void)
{
	if( m_pEyeDummyLeft != nullptr)
		m_pEyeDummyLeft->AddFlag(I3_NODEFLAG_DISABLE);
	if( m_pEyeDummyRight != nullptr)
		m_pEyeDummyRight->AddFlag(I3_NODEFLAG_DISABLE);
}

void CGameCharaBoneContext::OnChange3PV( void)
{
	if( m_pEyeDummyLeft != nullptr)
		m_pEyeDummyLeft->RemoveFlag(I3_NODEFLAG_DISABLE);
	if( m_pEyeDummyRight != nullptr)
		m_pEyeDummyRight->RemoveFlag(I3_NODEFLAG_DISABLE);
}

void CGameCharaBoneContext::__InitBone()
{
	static char s_szBones[][32] =
	{
		"Root",
		"Pelvis",
		"IK",
		"Spine1",
		"Spine2",
		"Spine3",
		"Neck",
		"Head",
		"L Foot",
		"R Foot",
		"L Forearm",
		"R Forearm",

		"L Thigh",
		"R Thigh",
		"L Calf",
		"R Calf",
	};

	INT32 i;

	// Bones
	for( i = 0; i < m_iBoneMax; i++)
	{
		m_idxBone[i] = m_pBody->FindBoneByName( s_szBones[i]);
		I3ASSERT( m_idxBone[i] != -1);
		i3Matrix::Identity( &m_Bone[i]);
	}

	i3Matrix::Identity( m_mtxPivot);
	
	m_PivotYaw = 0.0f;
	m_PivotYawTarget = 0.0f;
	m_timeYawBlend = 0.0f;
	m_timeTargetYawBlend = 0.f;
	removeBoneFlag( CHARA_BONE_FLAG_VALID_PIVOT);
}


void CGameCharaBoneContext::_CalcSpineBones()
{
	INT32 i;
	MATRIX worldTx;
	MATRIX * pLast = nullptr;
	INT32 calcCount = m_iBoneMax;
	if( m_pOwner->IsDino() )
		calcCount = m_iBoneMax - 1;
	
	i3Matrix::Copy( &worldTx, m_pOwner->GetMatrix());
	pLast = &worldTx;
	
	for( i = 0; i < calcCount; i++)
	{
		i3Matrix::Mul( getBoneMatrix( i), m_pBody->getCacheMatrix( m_idxBone[i]), pLast);

		pLast = getBoneMatrix( i);
	}
}

void CGameCharaBoneContext::UpdateAnim( REAL32 rDeltatime)
{
	i3SceneTracer * pTracer;

	pTracer = g_pFramework->GetViewer()->GetSceneTracer();
	pTracer->GetModelViewMatrixStack()->Reset();
	pTracer->SetTime( rDeltatime);
	if( g_pCamera != nullptr)
		pTracer->SetViewMatrix( g_pCamera->getCamMatrix(), g_pCamera->getCamMatrix());
	m_pBody->UpdateAnim( pTracer);
}


void CGameCharaBoneContext::TransformWorldSpace( REAL32 rDeltatime)
{
	i3SceneTracer * pTracer;

	if( g_pCamera==nullptr)return;

	pTracer = g_pFramework->GetViewer()->GetSceneTracer();

	pTracer->GetModelViewMatrixStack()->Reset();
	pTracer->SetTime( rDeltatime);
	pTracer->SetViewMatrix( g_pCamera->getCamMatrix(), g_pCamera->getCamMatrix());

	if( m_pOwner->is1PV() && m_pOwner->isLeftHand())
	{
		MATRIX * pDest = m_pBody->getCacheMatrix( m_idxBone[ BONE_IK]);

		MATRIX mtx;

		mtx.m22 = -1.0f;

		i3Matrix::Mul( pDest, &mtx, pDest);
	}

	m_pOwner->getSceneObject()->OnUpdate( pTracer);
	
	if( m_pOwner->isNPC() )
		return;

	// bug fix 2010.08.24 순구
	// 1인칭의 경우 하체 Bone이 없는 경우가 있다. 이경우 1LOD의 Shapeset을 Update해줘야한다.
	if( isBoneFlag( CHARA_BONE_FLAG_DIFF_BONECOUNT) && !m_pOwner->IsDino())
	{
		if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH)
			&& m_pBody->getCurrentLOD() == m_pBody->getLOD( 0) )
		{
			SetUpdateShapeSet(BONE_LLEGU);
			SetUpdateShapeSet(BONE_LLEGD);
			SetUpdateShapeSet(BONE_LFOOT);
			SetUpdateShapeSet(BONE_RLEGU);
			SetUpdateShapeSet(BONE_RLEGD);
			SetUpdateShapeSet(BONE_RFOOT);
		}
	}

	INT32 i;
	for( i = 0; i < m_iBoneMax; i++)
	{
		i3Matrix::Copy( getBoneMatrix( i), m_pBody->getCacheMatrix( m_idxBone[ i])); 
	}
}

void CGameCharaBoneContext::SetUpdateShapeSet(BONES eBone)
{
	i3Skeleton * pSkel = m_pBody->getFrameLOD()->getSkeleton();

	MATRIX * pMtx = pSkel->getBone( m_idxBone[ eBone])->getMatrix();
	i3Matrix::Mul( m_pBody->getCacheMatrix( m_idxBone[ eBone]), pMtx, m_pBody->getCacheMatrix( m_idxBone[ BONE_PELVIS]));

	if (pSkel->isShapeSet(m_idxBone[ eBone]))
	{
		i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( m_idxBone[ eBone]);
		pShapeSet->setShapeGlobalPose( m_pBody->getCacheMatrix( m_idxBone[ eBone]));
	}
	//else
		//TRACE("CGameCharaBoneContext::SetUpdateShapeSet ShapeSet = nullptr : index = %d\n",(int)eBone);

}

inline REAL32 _norm_angle( REAL32 rad)
{
	while( rad < 0.0f)		rad += I3_2PI;

	return rad;
}

void CGameCharaBoneContext::_CalcTheta( REAL32 rDeltatime)
{
	VEC3D vTempPos;
	REAL32 recoilHorz;

	if( m_RecoilData.m_RecoilHorzTime < m_RecoilData.m_RecoilHorzTimeTarget)
	{
		// 수평 반동의 적용
		if( m_RecoilData.m_HorzState != RECOIL_HORZ_FIRING)
		{
			// 새롭게 수평 반동이 적용되기 시작했다. (새로운 연사)
			m_RecoilData.m_HorzStartAngle = getTheta();
			m_RecoilData.m_HorzState = RECOIL_HORZ_FIRING;			
		}

		recoilHorz = m_RecoilData.m_RecoilHorzAngle * (rDeltatime * m_RecoilData.m_rRecoilHorzTimeTarget);

		setTheta( getTheta() + I3_DEG2RAD( recoilHorz) );

		m_RecoilData.m_RecoilHorzTime = rDeltatime + m_RecoilData.m_RecoilHorzTime;
	}
	else
	{
		if(m_pOwner->IsDino() == false)
		{
			// 수평 반동이 적용되지 않는다.
			WeaponBase * pWeapon = getCurrentWeapon();

			if( (pWeapon != nullptr) && (m_RecoilData.m_HorzState == RECOIL_HORZ_FIRING) )//&& (STATE_IsUpper( CHARA_UPPER_FIRE) == false))
			{
				// 발사 중인데도 getAutoFireCount() == 0이면 연사를 멈춘 상황
				if( pWeapon->getAutoFireCount() <= 0)
				{
					m_RecoilData.m_HorzState = RECOIL_HORZ_RESTORE;		// 원래의 Theta로 되돌아가는 모드로 변환.
					m_RecoilData.m_timeHorzRestore = 0.0f;
				}
			}
		}
		
		if( m_RecoilData.m_HorzState == RECOIL_HORZ_RESTORE)
		{
			// Theta의 Restore 상태라면...
			setTheta( _norm_angle( getTheta()) );
			REAL32 oldTheta = getTheta();
			m_RecoilData.m_HorzStartAngle = _norm_angle( m_RecoilData.m_HorzStartAngle);

			REAL32 diff = m_RecoilData.m_HorzStartAngle - getTheta();

			if( i3Math::abs( diff) >= I3_PI)
			{
				// 각도가 차가 180도를 넘어간다면 동일각에 대한 각도의 표현 차이(0 == 360) 때문에
				// 발생하는 현상이 있다.
				// 예를 들어, 왼쪽으로 10도만 돌면 될 것이 오른쪽으로 350도 돌아가는 현상이 그것이다.
				//
				// 아래는 그것을 방지하기 위해, 회전각이 180도를 넘어가는 경우에 보정을 한다.
				//
				if( diff > 0.0f)
					m_RecoilData.m_HorzStartAngle = m_RecoilData.m_HorzStartAngle - I3_2PI;
				else
					m_RecoilData.m_HorzStartAngle = m_RecoilData.m_HorzStartAngle + I3_2PI;

				diff = m_RecoilData.m_HorzStartAngle - getTheta();
			}

			if( m_RecoilData.m_HorzStartAngle > oldTheta)
			{
				if( oldTheta >= m_RecoilData.m_HorzStartAngle)
				{
					m_RecoilData.m_timeHorzRestore = 2.0f;
					setTheta( m_RecoilData.m_HorzStartAngle );
				}
			}
			else
			{
				if( oldTheta <= m_RecoilData.m_HorzStartAngle)
				{
					m_RecoilData.m_timeHorzRestore = 2.0f;
					setTheta( m_RecoilData.m_HorzStartAngle );
				}
			}

			setTheta( getTheta() + ((diff) * 0.07f * m_RecoilData.m_RecoilHorzAngle_Recovertime) );

			m_RecoilData.m_timeHorzRestore = m_RecoilData.m_timeHorzRestore + rDeltatime;

			if( m_RecoilData.m_timeHorzRestore > 2.0f)
			{
				cancelRecoilHorz();
			}
		}
	}

	setTheta( i3Math::NormalizeAngle( getTheta()) );

	{
		i3Vector::Copy( &vTempPos, m_pOwner->GetPos());
		i3Matrix::SetRotateY( m_pOwner->GetMatrix(), getTheta() + I3_DEG2RAD( m_ScopeWaveData.m_ScopeWaveTheta));
		i3Matrix::PostTranslate( m_pOwner->GetMatrix(), &vTempPos);
	}
}

void CGameCharaBoneContext::AdjustPivot( REAL32 rDeltatime, MATRIX * pInvMtx)
{
	MATRIX tmp, inv;
	REAL32 dot;
	VEC3D at1, at2;

	if( isBoneFlag( CHARA_BONE_FLAG_VALID_PIVOT))
	{
		bool bTrue1PV = false;
		if( i3::same_of<CGameCamera_FPS*>( g_pCamera))
		{	
			CGameCharaBase* pCharacter = g_pCamera->getCurTarget();
			if( pCharacter == m_pOwner ) bTrue1PV = true;
		}

		i3Matrix::Mul( &tmp, m_mtxPivot, getBoneMatrix( BONE_IK));

		i3Matrix::Transpose3x3( &inv, m_pOwner->GetMatrix());

		// 회전 축 구하기
		if( !bTrue1PV )
		{
			i3Vector::Normalize( &at1, i3Matrix::GetAt( &tmp));
			i3Vector::Normalize( &at2, i3Matrix::GetAt( m_pOwner->GetMatrix()));

			i3Vector::Cross( &m_PivotAxis, &at1, &at2);
			i3Vector::TransformNormal( &m_PivotAxis, &m_PivotAxis, pInvMtx);

			if( i3Vector::LengthSq( &m_PivotAxis) < I3_EPS)
			{
				i3Vector::Zero( &m_PivotAxis);
			}
			else
			{
				i3Vector::Normalize( &m_PivotAxis, &m_PivotAxis);
			}

			////////////////////////////////////////////////////////
			// Yaw 회전을 보정하기 위한 각도 계산
			{
				dot = i3Vector::Dot( &at1, &at2);
				if( dot > 1.0f)		dot = 1.0f;
				if( dot < -1.0f)	dot = -1.0f;

				m_PivotYawTarget = acosf( dot);
			}
		}
		else
		{
			i3Vector::Copy( &at1, i3Matrix::GetAt( &tmp));
			at1.y = 0.0f;

			i3Vector::Normalize( &at1, &at1);

			i3Vector::TransformNormal( &at1, &at1, &inv);

			////////////////////////////////////////////////////////
			// Yaw 회전을 보정하기 위한 각도 계산
			{
				dot = i3Vector::Dot( &at1, &I3_ZAXIS);
				if( dot > 1.0f)		dot = 1.0f;
				if( dot < -1.0f)	dot = -1.0f;

				m_PivotYawTarget = acos( dot);
			}

			// 캐릭터의 Local-Space로 변환
			

			// acos은 Minus 각도가 나오지 않으므로, Local-Space에서의 부호를
			// 비교하여 음의 각도인지, 양의 각도인지를 확인 
			if( at1.x > 0.0)
				m_PivotYawTarget = -m_PivotYawTarget;
		}

		//////////////////////////////////////////////////////////
		// Roll 회전을 보정하기 위한 각도 계산
		VEC3D up1, up2;

		i3Vector::Copy( &up1, i3Matrix::GetUp( &tmp));
		i3Vector::TransformNormal( &up2, &up1, &inv);

		//up1.z = 0.0f;
		up2.z = 0.0f;

		i3Vector::Normalize( &up2, &up2);
		dot = i3Vector::Dot( &up2, &I3_YAXIS);

		if( dot > 1.0f)		dot = 1.0f;
		if( dot < -1.0f)	dot = -1.0f;
		
		m_PivotRoll = -acos( dot);

		{
			if( up2.x > 0.0f)
				m_PivotRoll *= -1.0f;
		}
	}
	else
	{
		m_PivotYawTarget = 0.0f;
		m_PivotRoll = 0.0f;
	}

	if( m_PivotYawTarget != m_PivotYaw)
	{
		REAL32 t;

		m_timeYawBlend += rDeltatime;
		if( m_timeYawBlend >= m_timeTargetYawBlend)
		{
			m_timeYawBlend = m_timeTargetYawBlend;
			m_PivotYaw = m_PivotYawTarget;
		}
		else
		{
			if( m_timeTargetYawBlend > I3_EPS)
				t = m_timeYawBlend / m_timeTargetYawBlend;
			else
				t = 1.0f;

			m_PivotYaw = m_PivotYaw + ((m_PivotYawTarget - m_PivotYaw) * t);
		}
		//I3TRACE( "YAW : %f, %f, %f\n", I3_RAD2DEG( m_PivotYaw), m_PivotYawTarget, t);
	}
}

#define RATE		0.25f

void CGameCharaBoneContext::_DoUpdateRhoBone1PV( MATRIX * pMod)
{
	VEC3D vHeight, *pDir;
	pDir = i3Matrix::GetRight( m_pBody->getCacheMatrix( m_idxBone[ BONE_IK]));

	MATRIX mtxTemp;
	MATRIX * pMat = m_pBody->getCacheMatrix( m_idxBone[ BONE_SPINE1]);
	i3Matrix::Mul( &mtxTemp, m_pBody->getCacheMatrix( m_idxBone[ BONE_SPINE2]), pMat);
	i3Matrix::Mul( &mtxTemp, m_pBody->getCacheMatrix( m_idxBone[ BONE_SPINE3]), &mtxTemp);
	i3Matrix::Mul( &mtxTemp, m_pBody->getCacheMatrix( m_idxBone[ BONE_NECK]), &mtxTemp);
	i3Matrix::Transpose( &mtxTemp, &mtxTemp);

	i3Vector::TransformNormal( &vHeight, pDir, &mtxTemp);

	i3Vector::Normalize( &vHeight, &vHeight);
	i3Vector::Scale( &vHeight, &vHeight, getCameraContext()->getIKHeight());

	i3Vector::Add( i3Matrix::GetPos( pMod), i3Matrix::GetPos( pMod), &vHeight);

	MATRIX * pOut = m_pBody->getCacheMatrix( m_idxBone[ BONE_NECK]);
	i3Matrix::Mul( pOut, pMod, pOut);
}

void CGameCharaBoneContext::_CalcRho( REAL32 rDeltatime)
{
	//Rho의 값에 따라 상체를 구부린다.
	MATRIX tmp, mod;
	VEC3D axis;
	REAL32 rate;
	MATRIX * pOut = nullptr;

	// 3인칭일 때는 IK, Spine1, Spine2, Spine3 Bone들을 모두 조금씩 비율별로 회전시키지만
	// 1인칭일 때는 IK Bone만을 회전시킨다.

	bool bTrue1PV = false;
	if( i3::same_of<CGameCamera_FPS*>( g_pCamera))
	{	
		CGameCharaBase * pCharacter = g_pCamera->getCurTarget();
		if( pCharacter == m_pOwner ) bTrue1PV = true;
	}
	//렙터스팅
	if( bTrue1PV )
	{
		rate = 1.0f;
		i3Matrix::Inverse( &tmp, nullptr, getBoneMatrix( BONE_NECK));
	}
	else
	{
		rate = RATE;
		i3Matrix::Inverse( &tmp, nullptr, getBoneMatrix( BONE_IK));
	}
	
	//	통합 애니메이션 플레이중이 아닐때만 허리 보정을 해야한다.
	//if( !m_pOwner->isCharaStateMask( CHARA_STATE_USED_COMBINE_ANIM) && m_pOwner->is3PV())
	{
	//	AdjustPivot( rDeltatime, &tmp);
	}

	{
		REAL32 recoil = I3_DEG2RAD( m_RecoilData.m_RecoilAngle + m_ScopeWaveData.m_ScopeWaveRho);
		REAL32 rho = getRhoValue();
		
		i3Vector::Copy( &axis, i3Matrix::GetRight( m_pOwner->GetMatrix() ) );

		i3Vector::TransformNormal( &axis, &axis, &tmp);
		i3Vector::Normalize( &axis, &axis);

		i3Matrix::SetRotateAxis( &mod, &axis, (rho + recoil) * rate);
	}

	/*
	// 어깨가 흔들리는 것을 보정하기 위한 처리
	{
		i3Vector::Copy( &axis, i3Matrix::GetAt( m_pOwner->GetMatrix() ) );
		i3Vector::TransformNormal( &axis, &axis, &tmp);
		i3Vector::Normalize( &axis, &axis);

		i3Matrix::PostRotateAxis( &mod, &axis, m_PivotRoll * rate);
	}

	// 피봇 적용
	{		
		REAL32 yaw = m_PivotYaw * rate;

		if( isBoneFlag( CHARA_BONE_FLAG_VALID_PIVOT) && (bTrue1PV == false))
			i3Matrix::PostRotateAxis( &mod, &m_PivotAxis, yaw);
		else
			i3Matrix::PostRotateAxis( &mod, i3Matrix::GetRight( &mod), yaw);
	}
	*/

	if( bTrue1PV )
	{
		// 충격 흔들림.
		if( isBoneFlag( CHARA_BONE_FLAG_SHOVE_ACTIVE) )
		{
			m_timeShove += rDeltatime;
			if( m_timeShove > 0.2f)
			{
				if( isBoneFlag( CHARA_BONE_FLAG_SHOVE_RETURN) == false)
				{
					addBoneFlag( CHARA_BONE_FLAG_SHOVE_RETURN);
					// 인간의 반응속도...
					// 이 시간이 지나면서 부터는 충격에 대한 억지력을 가지고 원래 자세로
					// 되돌리려 한다.
					i3Quat::Copy( &m_quatShoveTarget, &m_quatShove);
				}

				//m_timeShove = 0.0f;
				m_acclShove = m_tShove * -0.1f;
			}

			m_velShove += m_acclShove * rDeltatime;
			m_tShove += m_velShove;

			if( isBoneFlag( CHARA_BONE_FLAG_SHOVE_RETURN) && ( i3Math::abs( m_acclShove) < 0.005f))
			{
				m_tShove = 0.0f;
				removeBoneFlag( CHARA_BONE_FLAG_SHOVE_ACTIVE);
				i3Matrix::Identity( &m_ShoveMatrix);
			}
			else
			{
				//REAL32 t = min( m_tShove, 1.0f);
				REAL32 t = m_tShove;

				i3Quat::Slerp( &m_quatShove, &m_quatShoveStart, &m_quatShoveTarget, t);
				//i3Quat::Dump( "", &m_quatShove);
				i3Matrix::SetRotateQuat( &m_ShoveMatrix, &m_quatShove);
			}

			i3Matrix::Mul( &mod, &mod, &m_ShoveMatrix);

			//I3TRACE( "%f %f %f\n", m_tShove, m_velShove, m_acclShove);
		}

		_DoUpdateRhoBone1PV( &mod);
	}
	else
	{
		pOut = m_pBody->getCacheMatrix( m_idxBone[ BONE_IK]);
		i3Matrix::Mul( pOut, &mod, pOut);

		pOut = m_pBody->getCacheMatrix( m_idxBone[ BONE_SPINE1]);
		i3Matrix::Mul( pOut, &mod, pOut);

		pOut = m_pBody->getCacheMatrix( m_idxBone[ BONE_SPINE2]);
		i3Matrix::Mul( pOut, &mod, pOut);

		pOut = m_pBody->getCacheMatrix( m_idxBone[ BONE_SPINE3]);
		i3Matrix::Mul( pOut, &mod, pOut);
	}
}

void CGameCharaBoneContext::Shove( VEC3D * pDir, REAL32 force)
{
	VEC3D dir, axis;

	i3Vector::Normalize( &dir, pDir);

	REAL32 dot = 1.0f - i3Vector::Dot( &dir, &I3_YAXIS);
	if( i3Math::abs( dot) <= I3_EPS)
	{
		return;
	}

	i3Vector::Cross( &axis, &dir, &I3_YAXIS);
	i3Vector::Normalize( &axis, &axis);		// World-Space에서의 회전축 (Local-Space로 변환되어야 한다.)

	// World-Space의 회전축을 Local Space로 바꾼다.
	MATRIX tmp;

	i3Matrix::Inverse( &tmp, nullptr, getBoneMatrix( BONE_NECK));
	i3Vector::TransformNormal( &axis, &axis, &tmp);

	//i3Vector::Copy( &axis, i3Matrix::GetRight( getBoneMatrix( BONE_SPINE3)));

	// 현재 Shove Rotation에서 추가로 돌리기
	i3Quat::Copy( &m_quatShoveTarget, &m_quatShove);
	i3Quat::RotateAxis( &m_quatShoveTarget, &axis, force);

	m_acclShove = -1.2f;
	m_velShove = 0.2f;
	m_tShove = 0.0f;
	addBoneFlag( CHARA_BONE_FLAG_SHOVE_ACTIVE);
	removeBoneFlag( CHARA_BONE_FLAG_SHOVE_RETURN);
	m_timeShove = 0.0f;
}

void CGameCharaBoneContext::CalcDeviation( REAL32 rDeltatime)
{
	WeaponBase * pWeapon = getCurrentWeapon();
	if( pWeapon == nullptr)		return;

	CWeaponInfo * pInfo = pWeapon->GetCurrentWeaponInfo();
	
	// 호칭 효과 적용 (초탄 정확도)
	REAL32 addDeviation = 0.f;

	const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());
	if (pNetInfo->GetDesigApplyInfo())
		addDeviation = pNetInfo->GetDesigApplyInfo()->desigWeapon.weaponFirstShotRate;

	REAL32 newDeviation = pInfo->CalcDeviationByAction(m_pOwner, pWeapon, addDeviation);
	
	/*I3TRACE("%f newDeviation %f m_DeviationTargetAngle %f \n", g_pFramework->GetAccTime(), newDeviation, m_DeviationData.m_DeviationTargetAngle);*/

	if( newDeviation != m_DeviationData.m_DeviationTargetAngle)
	{
		REAL32 speed = pWeapon->getCrossHairDeviationTargetTime(pInfo);

		if( newDeviation > m_DeviationData.m_DeviationAngle)
		{
			// Crosshair가 커지는 것은 아주 빠르게 한다.
			speed = pInfo->GetCrossHairExpandTime();
		}

		m_DeviationData.m_DeviationTargetAngle = newDeviation;
		m_DeviationData.m_DeviationStartAngle = m_DeviationData.m_DeviationAngle;
		m_DeviationData.m_DeviationState = DEVIATION_CHANGE;
		m_DeviationData.m_DeviationTime = 0.0f;
		m_DeviationData.m_DeviationTargetTime = speed;

		//I3TRACE( "DEVIATION TARGET : %f\n", newDeviation);
	}
}

void CGameCharaBoneContext::CalcRecoil( REAL32 rDeltatime)
{
	m_RecoilData._calcRecoil( rDeltatime );
	
	CalcDeviation( rDeltatime);

	m_DeviationData._calcDeviation( rDeltatime );

	//_calcScopeWave( tm);
}

void CGameCharaBoneContext::CalcScopeWave( REAL32 rDeltatime)
{
	m_ScopeWaveData._calcScopeWave( rDeltatime );
}

void CGameCharaBoneContext::_ProcessFire( void)
{
	bool bApplyExt;

	WeaponBase * pWeapon = getUsedWeapon();
	if( pWeapon == nullptr )	return;

	// 탄환 발사형이 아니라면 스킵
	if( !pWeapon->isBulletFireWeapon() )
		return;

	CWeaponInfo * pWeaponInfo = pWeapon->getRealWeaponInfo();

	I3ASSERT( pWeaponInfo != nullptr);

	if( pWeapon->GetCurrentExtensionType() != WEAPON::EXT_NONE && (pWeapon->GetCurrentExtensionType() & WEAPON_EXT_MASK_FIRE))
	{
		bApplyExt = true;

		if( pWeapon->GetCurrentExtensionType() & WEAPON_EXT_MASK_NEED_TO_ACTIVATE)
		{
			//if( pWeapon->isExtensionActivate() == false)
			// SCOPE를 사용시에는 Extension 반동으로 사용합니다.
			// Jinsik.kim(2015/04/23)
			if (pWeapon->isWeaponStateMask(WSM_ACTIVATE_SCOPE) == false)
			{
				bApplyExt = false;
			}
		}
		else if( !(pWeapon->GetCurrentExtensionType() & WEAPON_EXT_MASK_DIRECT_USE))
		{
			if( !m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
				bApplyExt = false;
		}
	}
	else
		bApplyExt = false;
	
	// 총기반동 제어
	REAL32 fRateVert = 0.0f;
	REAL32 fRateHorz = 0.f;

	const NET_CHARA_INFO * pNetInfo = BattleSlotContext::i()->getNetCharaInfo( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());
	
	// 총기 반동 본 적용에 호칭이 들어간다..
	if ( pNetInfo->GetDesigApplyInfo() )
	{
		fRateVert  = pNetInfo->GetDesigApplyInfo()->desigWeapon.weaponReaction * 0.01f;
		fRateHorz  = fRateVert;

		fRateVert += pNetInfo->GetDesigApplyInfo()->desigWeapon.weaponVertReaction;
		fRateHorz += pNetInfo->GetDesigApplyInfo()->desigWeapon.weaponHorzReaction;
	}
	
	// 수직 반동
 	if( m_pOwner->isLocal())
 		pWeaponInfo->SetRecoilVert(bApplyExt, m_RecoilData, pWeapon, fRateVert);

	// 수평 반동
	pWeaponInfo->SetRecoilHorz(bApplyExt, m_RecoilData, pWeapon, getTheta(), fRateHorz);

	// 편차
	__SetDeviation(bApplyExt, pWeaponInfo, pWeapon);
	
}

void CGameCharaBoneContext::__SetDeviation(bool bApplyExt, CWeaponInfo* pWeaponInfo, WeaponBase* pWeapon)
{
	REAL32 dev, factor = 1.0f;

	if( bApplyExt)	factor = pWeaponInfo->GetExtDeviationFactor();
	
	if( i3::same_of<WeaponDualCIC*>(pWeapon) )
	{
		if( ((WeaponDualCIC*)pWeapon)->getCurrentUseWeapon() ) // KEYWORD : 호칭
			dev = pWeapon->getRealWeaponInfo()->GetDeviation() * factor;
		else
			dev = pWeapon->GetDeviation() * factor;
	}else
	{
		dev = pWeapon->GetDeviation() * factor;		// KEYWORD : 호칭
	}

	m_DeviationData.m_FireDeviation = m_DeviationData.m_DeviationAngle;
	m_DeviationData.m_DeviationAngle = m_DeviationData.m_DeviationTargetAngle = m_DeviationData.m_DeviationAngle + dev;
	m_DeviationData.m_DeviationStartAngle = m_DeviationData.m_DeviationAngle;

	if( m_DeviationData.m_DeviationTargetAngle > pWeaponInfo->GetCrossHairMax())
		m_DeviationData.m_DeviationTargetAngle = pWeaponInfo->GetCrossHairMax();

	m_DeviationData.m_DeviationTargetTime = pWeapon->getCrossHairDeviationTargetTime(pWeaponInfo);

	m_DeviationData.m_DeviationTime = 0.0f;
	m_DeviationData.m_DeviationState = DEVIATION_CHANGE;

	// I3TRACE("%f m_DeviationTargetAngle %f m_FireDeviation %f\n", g_pFramework->GetAccTime(), m_DeviationData.m_DeviationTargetAngle, m_DeviationData.m_FireDeviation);
}

void CGameCharaBoneContext::_DoUpdateRhoBone3PV( MATRIX * pMod)
{
	MATRIX * pOut = m_pBody->getCacheMatrix( m_idxBone[ BONE_IK]);
	i3Matrix::Mul( pOut, pMod, pOut);
	pOut = m_pBody->getCacheMatrix( m_idxBone[ BONE_SPINE1]);
	i3Matrix::Mul( pOut, pMod, pOut);
	pOut = m_pBody->getCacheMatrix( m_idxBone[ BONE_SPINE2]);
	i3Matrix::Mul( pOut, pMod, pOut);
	pOut = m_pBody->getCacheMatrix( m_idxBone[ BONE_SPINE3]);
	i3Matrix::Mul( pOut, pMod, pOut);
}

void CGameCharaBoneContext::__CalcRho3PVCommon( REAL32 rDeltatime)
{
	//Rho의 값에 따라 상체를 구부린다.
	MATRIX tmp, mod;
	VEC3D axis;
	REAL32 rate;

	// 3인칭일 때는 IK, Spine1, Spine2, Spine3 Bone들을 모두 조금씩 비율별로 회전시키지만
	// 1인칭일 때는 IK Bone만을 회전시킨다.

	rate = RATE;
	if( m_pOwner->IsRex())
	{
		if( getRhoValue() >= 0.0f)
		{
			rate = 0.125f;
		}
		else
		{
			rate = 0.2f;
		}
	}
	else if( m_pOwner->IsRaptorBase())
	{
		if( getRhoValue() >= 0.0f)
		{
			rate = 0.142857f;
		}
		else
		{
			rate = 0.2f;
		}
	}

	i3Matrix::Inverse( &tmp, nullptr, getBoneMatrix( BONE_IK));

	//	통합 애니메이션 플레이중이 아닐때만 허리 보정을 해야한다.
	if( !m_pOwner->isCharaStateMask( CHARA_STATE_USED_COMBINE_ANIM))
	{
		AdjustPivot( rDeltatime, &tmp);
	}

	{
		REAL32 recoil = I3_DEG2RAD( m_RecoilData.m_RecoilAngle + m_ScopeWaveData.m_ScopeWaveRho);
		REAL32 rho = getRhoValue();

		if( m_pOwner->IsRex())
		{
			if( getRhoValue() < - I3_DEG2RAD( 50.0f))
			{
				rho = - I3_DEG2RAD( 50.0f);
			}
		}

		i3Vector::Copy( &axis, i3Matrix::GetRight( m_pOwner->GetMatrix() ) );
		i3Vector::TransformNormal( &axis, &axis, &tmp);
		i3Vector::Normalize( &axis, &axis);

		i3Matrix::SetRotateAxis( &mod, &axis, (rho + recoil) * rate);
	}

	// 어깨가 흔들리는 것을 보정하기 위한 처리
	{
		i3Vector::Copy( &axis, i3Matrix::GetAt( m_pOwner->GetMatrix() ) );
		i3Vector::TransformNormal( &axis, &axis, &tmp);
		i3Vector::Normalize( &axis, &axis);

		i3Matrix::PostRotateAxis( &mod, &axis, m_PivotRoll * rate);
	}
	
	{		
		REAL32 yaw = m_PivotYaw * rate;
		i3Matrix::PostRotateAxis( &mod, &m_PivotAxis, yaw);
	}
	
	_DoUpdateRhoBone3PV( &mod);
}

void CGameCharaBoneContext::__CalcRho3PVRaptor( REAL32 rDeltatime)
{
	//Rho의 값에 따라 상체를 구부린다.
	MATRIX tmp[7], mod[7];
	VEC3D axis;
	REAL32 rate;

	// 3인칭일 때는 IK, Spine1, Spine2, Spine3 Bone들을 모두 조금씩 비율별로 회전시키지만
	// 1인칭일 때는 IK Bone만을 회전시킨다.

	rate = 0.142857f;
	i3Matrix::Transpose3x3( &tmp[0], getBoneMatrix( RAPTOR_BONE_IK));
	i3Matrix::Transpose3x3( &tmp[1], getBoneMatrix( RAPTOR_BONE_SPINE));
	i3Matrix::Transpose3x3( &tmp[2], getBoneMatrix( RAPTOR_BONE_SPINE1));
	i3Matrix::Transpose3x3( &tmp[3], getBoneMatrix( RAPTOR_BONE_SPINE2));
	i3Matrix::Transpose3x3( &tmp[4], getBoneMatrix( RAPTOR_BONE_NECK));
	i3Matrix::Transpose3x3( &tmp[5], getBoneMatrix( RAPTOR_BONE_NECK1));
	i3Matrix::Transpose3x3( &tmp[6], getBoneMatrix( RAPTOR_BONE_NECK2));

	//	통합 애니메이션 플레이중이 아닐때만 허리 보정을 해야한다.
	if( !m_pOwner->isCharaStateMask( CHARA_STATE_USED_COMBINE_ANIM))
	{
		AdjustPivot( rDeltatime, &tmp[0]);
	}

	{
		REAL32 recoil = I3_DEG2RAD( m_RecoilData.m_RecoilAngle + m_ScopeWaveData.m_ScopeWaveRho);
		REAL32 rho = getRhoValue();

		i3Vector::TransformNormal( &axis, i3Matrix::GetRight( m_pOwner->GetMatrix()), &tmp[0]);
		i3Vector::Normalize( &axis, &axis);
		i3Matrix::SetRotateAxis( &mod[0], &axis, (rho + recoil) * rate);

		i3Vector::TransformNormal( &axis, i3Matrix::GetRight( m_pOwner->GetMatrix()), &tmp[1]);
		i3Vector::Normalize( &axis, &axis);
		i3Matrix::SetRotateAxis( &mod[1], &axis, (rho + recoil) * rate);

		i3Vector::TransformNormal( &axis, i3Matrix::GetRight( m_pOwner->GetMatrix()), &tmp[2]);
		i3Vector::Normalize( &axis, &axis);
		i3Matrix::SetRotateAxis( &mod[2], &axis, (rho + recoil) * rate);

		i3Vector::TransformNormal( &axis, i3Matrix::GetRight( m_pOwner->GetMatrix()), &tmp[3]);
		i3Vector::Normalize( &axis, &axis);
		i3Matrix::SetRotateAxis( &mod[3], &axis, (rho + recoil) * rate);

		i3Vector::TransformNormal( &axis, i3Matrix::GetRight( m_pOwner->GetMatrix()), &tmp[4]);
		i3Vector::Normalize( &axis, &axis);
		i3Matrix::SetRotateAxis( &mod[4], &axis, (rho + recoil) * rate);

		i3Vector::TransformNormal( &axis, i3Matrix::GetRight( m_pOwner->GetMatrix()), &tmp[5]);
		i3Vector::Normalize( &axis, &axis);
		i3Matrix::SetRotateAxis( &mod[5], &axis, (rho + recoil) * rate);

		i3Vector::TransformNormal( &axis, i3Matrix::GetRight( m_pOwner->GetMatrix()), &tmp[6]);
		i3Vector::Normalize( &axis, &axis);
		i3Matrix::SetRotateAxis( &mod[6], &axis, (rho + recoil) * rate);
	}

	// 어깨가 흔들리는 것을 보정하기 위한 처리
	{
		i3Vector::TransformNormal( &axis, i3Matrix::GetAt( m_pOwner->GetMatrix()), &tmp[0]);
		i3Vector::Normalize( &axis, &axis);
		i3Matrix::PostRotateAxis( &mod[0], &axis, m_PivotRoll * rate);

		i3Vector::TransformNormal( &axis, i3Matrix::GetAt( m_pOwner->GetMatrix()), &tmp[1]);
		i3Vector::Normalize( &axis, &axis);
		i3Matrix::PostRotateAxis( &mod[1], &axis, m_PivotRoll * rate);

		i3Vector::TransformNormal( &axis, i3Matrix::GetAt( m_pOwner->GetMatrix()), &tmp[2]);
		i3Vector::Normalize( &axis, &axis);
		i3Matrix::PostRotateAxis( &mod[2], &axis, m_PivotRoll * rate);

		i3Vector::TransformNormal( &axis, i3Matrix::GetAt( m_pOwner->GetMatrix()), &tmp[3]);
		i3Vector::Normalize( &axis, &axis);
		i3Matrix::PostRotateAxis( &mod[3], &axis, m_PivotRoll * rate);

		i3Vector::TransformNormal( &axis, i3Matrix::GetAt( m_pOwner->GetMatrix()), &tmp[4]);
		i3Vector::Normalize( &axis, &axis);
		i3Matrix::PostRotateAxis( &mod[4], &axis, m_PivotRoll * rate);

		i3Vector::TransformNormal( &axis, i3Matrix::GetAt( m_pOwner->GetMatrix()), &tmp[5]);
		i3Vector::Normalize( &axis, &axis);
		i3Matrix::PostRotateAxis( &mod[5], &axis, m_PivotRoll * rate);

		i3Vector::TransformNormal( &axis, i3Matrix::GetAt( m_pOwner->GetMatrix()), &tmp[6]);
		i3Vector::Normalize( &axis, &axis);
		i3Matrix::PostRotateAxis( &mod[6], &axis, m_PivotRoll * rate);
	}
	
	{		
		REAL32 yaw = m_PivotYaw * rate;
		i3Matrix::PostRotateAxis( &mod[0], &m_PivotAxis, yaw);
	}
	
	{
		MATRIX * pOut = m_pBody->getCacheMatrix( m_idxBone[ RAPTOR_BONE_IK]);
		i3Matrix::Mul( pOut, &mod[0], pOut);

		pOut = m_pBody->getCacheMatrix( m_idxBone[ RAPTOR_BONE_SPINE]);
		i3Matrix::Mul( pOut, &mod[1], pOut);

		pOut = m_pBody->getCacheMatrix( m_idxBone[ RAPTOR_BONE_SPINE1]);
		i3Matrix::Mul( pOut, &mod[2], pOut);

		pOut = m_pBody->getCacheMatrix( m_idxBone[ RAPTOR_BONE_SPINE2]);
		i3Matrix::Mul( pOut, &mod[3], pOut);

		pOut = m_pBody->getCacheMatrix( m_idxBone[ RAPTOR_BONE_NECK]);
		i3Matrix::Mul( pOut, &mod[4], pOut);

		pOut = m_pBody->getCacheMatrix( m_idxBone[ RAPTOR_BONE_NECK1]);
		i3Matrix::Mul( pOut, &mod[5], pOut);

		pOut = m_pBody->getCacheMatrix( m_idxBone[ RAPTOR_BONE_NECK2]);
		i3Matrix::Mul( pOut, &mod[6], pOut);
	}
}


void CGameCharaBoneContext::_CalcRho3PV( REAL32 rDeltatime)
{
	if(m_pOwner->IsRaptorBase()) {
		__CalcRho3PVRaptor(rDeltatime);
	}
	else {
		__CalcRho3PVCommon(rDeltatime);
	}
}

void CGameCharaBoneContext::_CalcThetaNetwork( REAL32 rDeltatime)
{
	VEC3D vTempPos;
	setTheta( i3Math::NormalizeAngle( getTheta()) );
	i3Vector::Copy( &vTempPos, m_pOwner->GetPos());

	i3Matrix::SetRotateY( m_pOwner->GetMatrix(), getTheta() + I3_DEG2RAD( m_ScopeWaveData.m_ScopeWaveTheta));
	i3Matrix::PostTranslate( m_pOwner->GetMatrix(), &vTempPos);
}

#define HEADINTERPOLATETIME	0.15f
#define HEADWATCHCHECKTIME 0.5f

void CGameCharaBoneContext::setHeadWatchUserIdx( INT32 idx)
{
	I3ASSERT( idx < SLOT_MAX_COUNT);
	m_HeadWatchUserIdx = idx;
	m_HeadInterpolateTime = HEADWATCHCHECKTIME;
}

void CGameCharaBoneContext::_CalcHeadBone( REAL32 rDeltatime)
{
	REAL32 rLen;
	VEC3D vDir;
	VEC3D vSub;
	VEC3D axis;
	MATRIX rot;
	MATRIX tmp, mod;
	MATRIX * pOut = m_pBody->getCacheMatrix( getBoneIndex( BONE_HEAD));

	m_HeadInterpolateTime += rDeltatime;

	if( (getActionContext()->isUpper( CHARA_UPPER_IDLE) || getActionContext()->isUpper( CHARA_UPPER_MOVE) ) &&
		!m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM) )
	{
		if( m_HeadInterpolateTime > HEADWATCHCHECKTIME)
		{	
			INT32 closeChara = -1;
			REAL32 rLength = 10.f;

			if( m_HeadWatchUserIdx == -1 )
			{	// 주시하는 캐릭터가 없는 경우 가장 가까운 캐릭터를 검색
				INT32 charaCnt = g_pCharaManager->getCharaCount();
				for( INT32 i = 0; i < charaCnt; i++)
				{
					CGameCharaBase * pChara = g_pCharaManager->getChara( i );
					if( pChara == nullptr)
						continue;
					if( pChara->isCharaStateMask( CHARA_STATE_DEATH) )
						continue;

					if( pChara != m_pOwner )
					{	
						i3Vector::Sub( &vSub, pChara->GetPos(), m_pOwner->GetPos());
						rLen = i3Vector::LengthSq( &vSub);
						if( rLength > rLen)
						{
							rLength = rLen;
							closeChara = i;
							i3Vector::Copy( &vDir, &vSub);
						}
					}
				}
			}
			else
			{	// 주시하는 캐릭터가 있는 경우
				CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( m_HeadWatchUserIdx);
				if( pChara != nullptr && pChara != m_pOwner && !pChara->isCharaStateMask( CHARA_STATE_DEATH) )
				{
					i3Vector::Sub( &vSub, pChara->GetPos(), m_pOwner->GetPos());
					i3Vector::Copy( &vDir, &vSub);
					closeChara = m_HeadWatchUserIdx;
				}
				m_HeadWatchUserIdx = -1;
			}

			if( closeChara != -1 )
			{
				m_HeadTheta = m_HeadTargetTheta;
				i3Vector::Normalize( &vDir, &vDir);
				
				i3Matrix::SetRotateY( &rot, -getTheta());
				i3Vector::TransformCoord( &vDir, &vDir, &rot);

				m_HeadTargetTheta	= i3Math::atan2( getX( &vDir), getZ( &vDir));

				if( m_HeadTargetTheta < -1.3f )
					m_HeadTargetTheta = -1.3f;

				if( m_HeadTargetTheta > 1.f )
					m_HeadTargetTheta = 1.f;

				m_HeadInterpolateTime = 0.f;
			}
			else
			{
				m_HeadTheta = m_HeadTargetTheta;
				m_HeadTargetTheta = 0.f;
				m_HeadInterpolateTime = 0.f;
			}
		}
	}
	else
	{
		m_HeadTheta = m_HeadTargetTheta;
		m_HeadTargetTheta = 0.f;
	}
	
	REAL32 rInterpolate = m_HeadInterpolateTime / HEADINTERPOLATETIME;

	if( rInterpolate > 1.f)
		rInterpolate = 1.f;

	REAL32 rTheta = m_HeadTheta + (m_HeadTargetTheta - m_HeadTheta) * rInterpolate;

	i3Matrix::Inverse( &tmp, nullptr, getBoneMatrix( BONE_IK));

	i3Vector::Copy( &axis, i3Matrix::GetUp( m_pOwner->GetMatrix() ) );
	i3Vector::TransformNormal( &axis, &axis, &tmp);
	i3Vector::Normalize( &axis, &axis);

	i3Matrix::SetRotateAxis( &mod, &axis, rTheta);

	i3Matrix::Mul( pOut, &mod, pOut);
}

void CGameCharaBoneContext::_CalcAimBone( REAL32 rDeltatime)
{	
	MATRIX * pOut;
	MATRIX tmp, mod;
	VEC3D axis;
	REAL32 rTheta;

	I3ASSERT( m_pOwner->IsDino() == false);
	
	if( getActionContext() && getActionContext()->isLower( CHARA_LOWER_IDLE))
	{
		rTheta = i3Math::NormalizeAngle( getTheta() - m_AimTheta);
	
		if( (rTheta > 0.4f || rTheta < -0.9f))
		{
			m_AimStartTheta = i3Math::NormalizeAngle( m_AimTheta);
			m_AimTime = 0.f;
			m_AimInterpolate = rTheta;

			if( !isBoneFlag( CHARA_BONE_FLAG_AIM_ROTATE) )
			{
				if( !getActionContext()->isLower( CHARA_LOWER_RUN) )
				{
					if( rTheta > 0.f)
						getAnimContext()->_Play_Anim_Lower( ID_LOWER_MOVE_LEFT);
					else
						getAnimContext()->_Play_Anim_Lower( ID_LOWER_MOVE_RIGHT);
				}

				if( getActionContext()->isLower( CHARA_LOWER_RUN))	removeBoneFlag( CHARA_BONE_FLAG_AIM_ROTATE);
				else												addBoneFlag( CHARA_BONE_FLAG_AIM_ROTATE);
			}
		}

		{	
			m_AimTime += rDeltatime;

			if( m_AimTime >= 0.2f)
			{
				m_AimTime = 0.2f;

				if( isBoneFlag( CHARA_BONE_FLAG_AIM_ROTATE) )
				{
					getAnimContext()->_Play_Anim_Lower( ID_LOWER_IDLE);
					removeBoneFlag( CHARA_BONE_FLAG_AIM_ROTATE);
				}
			}
			
			REAL32 rSlope = m_AimInterpolate * m_AimTime * 5.f;

			m_AimTheta = i3Math::NormalizeAngle( m_AimStartTheta + rSlope);
		}
	}
	else
	{
		m_AimInterpolate = rTheta = 0.f;
		m_AimTheta = m_AimStartTheta = getTheta();
	}

	if( m_pBody->getAnimationContext() != nullptr)
	{
		i3Matrix::Inverse( &tmp, nullptr, getBoneMatrix( BONE_IK));
		i3Vector::Copy( &axis, i3Matrix::GetRight( getBoneMatrix( BONE_IK)));
		
		i3Vector::TransformNormal( &axis, &axis, &tmp);

		i3Matrix::SetRotateAxis( &mod, &axis, -rTheta );
	
		pOut = m_pBody->getCacheMatrix( m_idxBone[ BONE_PELVIS]);

		i3Matrix::Mul( pOut, &mod, pOut);
	}
}

void CGameCharaBoneContext::SetCharaDir(REAL32 rX, REAL32 rY, REAL32 rZ)
{
	VEC3D v3Temp0;
	i3Vector::Set(&v3Temp0, 0.f, 0.f, 1.f);
	VEC3D v3Temp1;
	
	i3Vector::Set(&v3Temp1, rX, 0.f, rZ);
	i3Vector::Normalize(&v3Temp1, &v3Temp1);

	setTheta( acos(i3Vector::Dot(&v3Temp0, &v3Temp1)) );

	i3Vector::Set(&v3Temp1, 0.f, rY, rZ);
	i3Vector::Normalize(&v3Temp1, &v3Temp1);

	if(rZ < 0) 
	{
		i3Vector::Set(&v3Temp0, 0.f, 0.f, -1.f);
	}

	setRho(-acos(i3Vector::Dot(&v3Temp0, &v3Temp1)));
}
