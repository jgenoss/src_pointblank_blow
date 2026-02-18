#include "pch.h"
#include "GameCharaPlayer.h"
#include "GlobalVariables.h"
#include "StageBattle.h"

/*
void CGameCharaPlayer::_InitBones(void)
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
	};
	INT32 i;

	// Bones
	for( i = 0; i < BONE_MAX; i++)
	{
		m_idxBone[i] = m_pBody->FindBoneByName( s_szBones[i]);
		I3ASSERT( m_idxBone[i] != -1);

		i3Matrix::Identity( &m_Bone[i]);
	}

	i3Matrix::Identity( &m_mtxPivot);
	
	m_PivotYaw = 0.0f;
	m_PivotYawTarget = 0.0f;
	m_timeYawBlend = 0.0f;
	m_timeTargetYawBlend;
	m_bValidPivot = false;
}

void CGameCharaPlayer::_calcSpineBones(void)
{
	INT32 i;
	MATRIX worldTx;
	MATRIX * pLast = &worldTx;

	i3Matrix::Copy( &worldTx, GetMatrix());

	// Head Bone은 제외
	for( i = 0; i < BONE_MAX - 1; i++)
	{
		i3Matrix::Mul( getBoneMatrix( i), m_pBody->getCacheMatrix( m_idxBone[i]), pLast);

		pLast = getBoneMatrix( i);
	}
}

void CGameCharaPlayer::_UpdateAnim( REAL32 tm)
{
	i3SceneTracer * pTracer;

	pTracer = g_pFramework->GetViewer()->GetSceneTracer();

	pTracer->GetModelViewMatrixStack()->Reset();
	
	if( g_pGameContext->IsP2PActivate() )
	{
		pTracer->SetTime( tm);
		pTracer->SetViewMatrix( g_pCamera->getCamMatrix(), g_pCamera->getCamMatrix());
	}
	else
	{
		// GUI에서 character animation이 2배가 되는 것을 막습니다 - praptor
		pTracer->SetTime( 0.0f);
	}
	
	m_pBody->UpdateAnim( pTracer);
}


void CGameCharaPlayer::_TransformWorldSpace( REAL32 tm)
{
	i3SceneTracer * pTracer;

	pTracer = g_pFramework->GetViewer()->GetSceneTracer();

	pTracer->GetModelViewMatrixStack()->Reset();
	pTracer->SetTime( tm);
	pTracer->SetViewMatrix( g_pCamera->getCamMatrix(), g_pCamera->getCamMatrix());

	GetSceneObject()->OnUpdate( pTracer);
	
	i3Matrix::Copy( getBoneMatrix( BONE_IK), m_pBody->getCacheMatrix( m_idxBone[ BONE_IK]));
	i3Matrix::Copy( getBoneMatrix( BONE_NECK), m_pBody->getCacheMatrix( m_idxBone[ BONE_NECK]));
	i3Matrix::Copy( getBoneMatrix( BONE_HEAD), m_pBody->getCacheMatrix( m_idxBone[ BONE_HEAD]));
}

inline REAL32 _norm_angle( REAL32 rad)
{
	while( rad < 0.0f)		rad += I3_2PI;

	return rad;
}

void CGameCharaPlayer::_calcTheta( REAL32 tm)
{
	VEC3D vTempPos;
	REAL32 recoilHorz;

	if( m_RecoilData.m_RecoilHorzTime < m_RecoilData.m_RecoilHorzTimeTarget)
	{
		// 수평 반동의 적용
		if( m_RecoilData.m_HorzState != RECOIL_HORZ_FIRING)
		{
			// 새롭게 수평 반동이 적용되기 시작했다. (새로운 연사)
			m_RecoilData.m_HorzStartAngle = m_Theta;
			m_RecoilData.m_HorzState = RECOIL_HORZ_FIRING;			
		}

		recoilHorz = m_RecoilData.m_RecoilHorzAngle * (tm * m_RecoilData.m_rRecoilHorzTimeTarget);

		m_Theta += I3_DEG2RAD( recoilHorz);

		m_RecoilData.m_RecoilHorzTime += tm;
	}
	else
	{
		// 수평 반동이 적용되지 않는다.
		WeaponBase * pWeapon = getWeaponContext()->getCurrent();

		if( (pWeapon != NULL) && (m_RecoilData.m_HorzState == RECOIL_HORZ_FIRING) )//&& (STATE_IsUpper( CHARA_UPPER_FIRE) == false))
		{
			// 발사 중인데도 getAutoFireCount() == 0이면 연사를 멈춘 상황

			m_RecoilData.m_HorzState = RECOIL_HORZ_RESTORE;		// 원래의 Theta로 되돌아가는 모드로 변환.
			m_RecoilData.m_timeHorzRestore = 0.0f;
		}
		
		if( m_RecoilData.m_HorzState == RECOIL_HORZ_RESTORE)
		{
			// Theta의 Restore 상태라면...
			m_Theta = _norm_angle( m_Theta);
			REAL32 oldTheta = m_Theta;
			m_RecoilData.m_HorzStartAngle = _norm_angle( m_RecoilData.m_HorzStartAngle);

			REAL32 diff = m_RecoilData.m_HorzStartAngle - m_Theta;

			if( m_RecoilData.m_HorzStartAngle > oldTheta)
			{
				if( oldTheta >= m_RecoilData.m_HorzStartAngle)
				{
					m_RecoilData.m_timeHorzRestore = 2.0f;
					m_Theta = m_RecoilData.m_HorzStartAngle;
				}
			}
			else
			{
				if( oldTheta <= m_RecoilData.m_HorzStartAngle)
				{
					m_RecoilData.m_timeHorzRestore = 2.0f;
					m_Theta = m_RecoilData.m_HorzStartAngle;
				}
			}

			m_Theta += (diff) * 0.07f;

			m_RecoilData.m_timeHorzRestore += tm;

			if( m_RecoilData.m_timeHorzRestore > 2.0f)
			{
				m_RecoilData.m_timeHorzRestore = 0.0f;
				m_RecoilData.m_HorzState = RECOIL_HORZ_NA;
			}
		}
	}

	i3Vector::Copy( &vTempPos, GetPos());

	i3Matrix::SetRotateY( GetMatrix(), m_Theta + I3_DEG2RAD( m_ScopeWaveData.m_ScopeWaveTheta));
	i3Matrix::PostTranslate( GetMatrix(), &vTempPos);
}

void CGameCharaPlayer::_AdjustPivot( REAL32 tm, MATRIX * pInvMtx)
{
	MATRIX tmp, inv;
	REAL32 dot;
	VEC3D at1, at2;

	if( m_bValidPivot)
	{
		BOOL bTrue1PV = FALSE;
		if( g_pCamera && g_pCamera->IsExactTypeOf(CGameCamera_FPS::GetClassMeta()) )
		{	
			CGameCharaPlayer* pCharacter = g_pCamera->getCurTarget();
			if( pCharacter == this ) bTrue1PV = TRUE;
		}

		i3Matrix::Mul( &tmp, &m_mtxPivot, getBoneMatrix( BONE_IK));

		i3Matrix::Transpose3x3( &inv, GetMatrix());

		// 회전 축 구하기
		if( !bTrue1PV )
		{
			i3Vector::Normalize( &at1, i3Matrix::GetAt( &tmp));
			i3Vector::Normalize( &at2, i3Matrix::GetAt( GetMatrix()));

			i3Vector::Cross( &m_PivotAxis, &at1, &at2);
			i3Vector::TransformNormal( &m_PivotAxis, &m_PivotAxis, pInvMtx);
			i3Vector::Normalize( &m_PivotAxis, &m_PivotAxis);

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

		m_timeYawBlend += tm;
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

void CGameCharaPlayer::_calcRho( REAL32 tm)
{
	//Rho의 값에 따라 상체를 구부린다.
	MATRIX tmp, mod;
	VEC3D axis;
	REAL32 rate;

	// 3인칭일 때는 IK, Spine1, Spine2, Spine3 Bone들을 모두 조금씩 비율별로 회전시키지만
	// 1인칭일 때는 IK Bone만을 회전시킨다.

	BOOL bTrue1PV = FALSE;
	if( g_pCamera && g_pCamera->IsExactTypeOf(CGameCamera_FPS::GetClassMeta()) )
	{	
		CGameCharaPlayer* pCharacter = g_pCamera->getCurTarget();
		if( pCharacter == this ) bTrue1PV = TRUE;
	}

	if( bTrue1PV )
	{
		rate = 1.0f;
		i3Matrix::Inverse( &tmp, NULL, getBoneMatrix( BONE_NECK));
	}
	else
	{
		rate = RATE;
		i3Matrix::Inverse( &tmp, NULL, getBoneMatrix( BONE_IK));
	}

	//	통합 애니메이션 플레이중이 아닐때만 허리 보정을 해야한다.
	if( (getStateMask() & CHARA_STATE_USED_COMBINE_ANIM) == 0)
		_AdjustPivot( tm, &tmp);

	{
		REAL32 recoil = I3_DEG2RAD( m_RecoilData.m_RecoilAngle + m_ScopeWaveData.m_ScopeWaveRho);
		REAL32 rho = m_Rho;

		if( getActionContext()->isLower( CHARA_LOWER_DEATH))		//	죽는 동작때는 Rho값을 적용하지 않게 
		{
			rho = 0.f;
			recoil = 0.f;
		}

		i3Vector::Copy( &axis, i3Matrix::GetRight( GetMatrix() ) );
		i3Vector::TransformNormal( &axis, &axis, &tmp);
		i3Vector::Normalize( &axis, &axis);

		i3Matrix::Identity( &mod);
		i3Matrix::SetRotateAxis( &mod, &axis, (rho + recoil) * rate);
	}

	// 어깨가 흔들리는 것을 보정하기 위한 처리
	{
		i3Vector::Copy( &axis, i3Matrix::GetAt( GetMatrix() ) );
		i3Vector::TransformNormal( &axis, &axis, &tmp);
		i3Vector::Normalize( &axis, &axis);

		i3Matrix::PostRotateAxis( &mod, &axis, m_PivotRoll * rate);
	}

	if( !getActionContext()->isLower( CHARA_LOWER_DEATH))		//	죽는 동작 이외는 피봇 적용
	{		
		REAL32 yaw = m_PivotYaw * rate;

		if( m_bValidPivot && (bTrue1PV == false))
			i3Matrix::PostRotateAxis( &mod, &m_PivotAxis, yaw);
		else
			i3Matrix::PostRotateAxis( &mod, i3Matrix::GetRight( &mod), yaw);
	}

	if( bTrue1PV )
	{
		// 충격 흔들림.
		if( m_bShoveActive)
		{
			m_timeShove += tm;
			if( m_timeShove > 0.2f)
			{
				if( m_bShoveReturn == false)
				{
					m_bShoveReturn = true;
					// 인간의 반응속도...
					// 이 시간이 지나면서 부터는 충격에 대한 억지력을 가지고 원래 자세로
					// 되돌리려 한다.
					i3Quat::Copy( &m_quatShoveTarget, &m_quatShove);
				}

				//m_timeShove = 0.0f;

				m_acclShove = m_tShove * -0.1f;
			}
			else
			{
			}

			m_velShove += m_acclShove * tm;
			m_tShove += m_velShove;

			if((m_bShoveReturn) && ( i3Math::abs( m_acclShove) < 0.005f))
			{
				m_tShove = 0.0f;
				m_bShoveActive = false;
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

			I3TRACE( "%f %f %f\n", m_tShove, m_velShove, m_acclShove);
		}

		addX(i3Matrix::GetPos( &mod), m_pCameraContext->getIKHeight());

		MATRIX * pOut = m_pBody->getCacheMatrix( m_idxBone[ BONE_NECK]);

		i3Matrix::Mul( pOut, &mod, pOut);
	}
	else
	{
		MATRIX * pOut = m_pBody->getCacheMatrix( m_idxBone[ BONE_IK]);
		i3Matrix::Mul( pOut, &mod, pOut);

		pOut = m_pBody->getCacheMatrix( m_idxBone[ BONE_SPINE1]);
		i3Matrix::Mul( pOut, &mod, pOut);

		pOut = m_pBody->getCacheMatrix( m_idxBone[ BONE_SPINE2]);
		i3Matrix::Mul( pOut, &mod, pOut);

		pOut = m_pBody->getCacheMatrix( m_idxBone[ BONE_SPINE3]);
		i3Matrix::Mul( pOut, &mod, pOut);
	}	
}

void CGameCharaPlayer::Shove( VEC3D * pDir, REAL32 force)
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

	i3Matrix::Inverse( &tmp, NULL, getBoneMatrix( BONE_NECK));
	i3Vector::TransformNormal( &axis, &axis, &tmp);

	//i3Vector::Copy( &axis, i3Matrix::GetRight( getBoneMatrix( BONE_SPINE3)));

	// 현재 Shove Rotation에서 추가로 돌리기
	i3Quat::Copy( &m_quatShoveTarget, &m_quatShove);
	i3Quat::RotateAxis( &m_quatShoveTarget, &axis, force);

	m_acclShove = -1.2f;
	m_velShove = 0.2f;
	m_tShove = 0.0f;
	m_bShoveActive = true;
	m_bShoveReturn = false;
	m_timeShove = 0.0f;
}
*/

/*
REAL32 CGameCharaPlayer::getIKHeightForCam(void)
{
	BOOL bTrue1PV = FALSE;
	if( g_pCamera && g_pCamera->IsExactTypeOf(CGameCamera_FPS::GetClassMeta()) )
	{	
		CGameCharaPlayer* pCharacter = g_pCamera->getCurTarget();
		if( pCharacter == this ) bTrue1PV = TRUE;
	}	

	//if( is1PV())
	if( bTrue1PV )
	{
		return m_pDBInfo->m_vCamIKHeight.vec[0] + m_rIKHeight;
	}
	else
	{
		return m_pDBInfo->m_vCamIKHeight.vec[m_StateForCam];
	}
}
*/


/*
void CGameCharaPlayer::_calcDeviation( REAL32 tm)
{
	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL)
		return;

	CWeaponInfo * pInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pInfo != NULL);

	REAL32 base = pInfo->m_rCrossHairMin;
	REAL32 newDeviation;

	newDeviation = base;
	
	if( getActionContext()->isBody( CHARA_BODY_STAND))
	{
		// Stand
		newDeviation += (base * pInfo->m_rCrossHairSizeStand);
	}

	// xgkick
	if( isStateMask( CHARA_STATE_ZOOM))
	{
		newDeviation += (base * pInfo->m_rCrossHairSizeZoom);
	}

	switch( getActionContext()->getLower())
	{
		case CHARA_LOWER_MOVE :	
			if( getActionContext()->isBody( CHARA_BODY_CROUCH))
				newDeviation += (base * pInfo->m_rCrossHairSizeMoveSit);	
			else
				newDeviation += (base * pInfo->m_rCrossHairSizeMove);	
			break;

		case CHARA_LOWER_WALK :	
			if( getActionContext()->isBody( CHARA_BODY_CROUCH))
				newDeviation += (base * pInfo->m_rCrossHairSizeWalkSit);	
			else
				newDeviation += (base * pInfo->m_rCrossHairSizeWalk);	
			break;

		case CHARA_LOWER_JUMP : newDeviation += (base * pInfo->m_rCrossHairSizeJump);	break;
	}

	switch( getActionContext()->getUpper())
	{
		case CHARA_UPPER_DAMAGE :	newDeviation += (base * pInfo->m_rCrossHairSizeDamage);	break;
	}

	if( newDeviation < 0.0f)
		newDeviation = 0.0f;

	if( newDeviation > pInfo->m_rCrossHairMax)
		newDeviation = pInfo->m_rCrossHairMax;

	if( newDeviation != m_DeviationData.m_DeviationTargetAngle)
	{
		REAL32 speed = pWeapon->getCrosshairSpeedFactor() * pInfo->m_CrossHairContractTime;

		if( newDeviation > m_DeviationData.m_DeviationAngle)
		{
			// Crosshair가 커지는 것은 아주 빠르게 한다.
			speed = pInfo->m_CrossHairExpandTime;
		}

		m_DeviationData.m_DeviationTargetAngle = newDeviation;
		m_DeviationData.m_DeviationStartAngle = m_DeviationData.m_DeviationAngle;
		m_DeviationData.m_DeviationState = DEVIATION_CHANGE;
		m_DeviationData.m_DeviationTime = 0.0f;
		m_DeviationData.m_DeviationTargetTime = speed;

		//I3TRACE( "DEVIATION TARGET : %f\n", newDeviation);
	}
}

void CGameCharaPlayer::_calcRecoil( REAL32 tm)
{
	m_RecoilData._calcRecoil( tm );
	
	_calcDeviation( tm);

	m_DeviationData._calcDeviation( tm );

	//_calcScopeWave( tm);
}

void CGameCharaPlayer::_calcScopeWave( REAL32 tm)
{
	m_ScopeWaveData._calcScopeWave( tm );
}

*/

