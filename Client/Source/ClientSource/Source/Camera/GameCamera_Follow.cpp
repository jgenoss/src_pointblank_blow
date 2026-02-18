#include "pch.h"
#include "GameCamera_Follow.h"
#include "GlobalVariables.h"
#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"

I3_CLASS_INSTANCE( CGameCamera_Follow);

CGameCamera_Follow::CGameCamera_Follow(void)
{
	i3Vector::Set( m_vBasePosition, 0.0f, 2.0f, 2.0f);

	m_IsCamLeft	= false;
}

void CGameCamera_Follow::InitCamera( CGameCharaBase * pChar, REAL32 rInterpolateTime)
{
	CGameCamera::InitCamera(pChar, rInterpolateTime);

	if( m_pTargetChara != nullptr)
	{
		m_pTargetChara->Cmd_ChangeView( CHARA_VIEW_3PV);

		if (m_pTargetChara->IsEnabledOccludee() == false)
		{
			m_pTargetChara->EnableOccludee(true);
			m_pTargetChara->EnableRenderAABB(true);

			if (m_pTargetChara->GetCurrentCharaWeapon())
			{
				m_pTargetChara->GetCurrentCharaWeapon()->EnableRenderAABB(true);
			}
		}

		//m_pTargetChara->ChangeObserverView();
	}
}

void CGameCamera_Follow::OnUpdate( REAL32 rDeltaSeconds)
{
	CGameCamera::OnUpdate( rDeltaSeconds);
}

void CGameCamera_Follow::ToggleCamSidePos()
{
	SetInterPolate( 0.2f);

	CGameCamera::ToggleCamSidePos();
}

void CGameCamera_Follow::ReCalculateMatrix()
{
	if( m_bNeedUpdate == false )	return;
	
	VEC3D	*vCamPos, vPointPos, vAt, vTemp, vOriginal;
	MATRIX  *pMat;
	REAL32	Theta, Distance;

	// Chara의 자세에따른 위치
	if( m_pTargetChara == nullptr )
		return;

	i3Vector::Copy( &vPointPos, m_pTargetChara->GetPos());
	addY( &vPointPos, m_pTargetChara->getCameraContext()->getIKHeightForCam());

	pMat		= m_pCamMatrix;
	vCamPos		= i3Matrix::GetPos( pMat);

	Distance = getZ( m_vBasePosition);

#if 0
	Distance += getZ( &m_vMouseDelta) * 0.01f;
	if( Distance <= getZ( m_vBasePosition))
	{
		setX( &m_vMouseDelta, 0.0f);
		Distance = getZ( m_vBasePosition);
	}
#endif

	{
		i3Vector::Normalize( &vAt, i3Matrix::GetAt( m_pTargetMatrix));

		Theta = atan2f( getX( &vAt), getZ( &vAt));
		i3Matrix::SetRotateY( pMat, Theta + I3_DEG2RAD( 180.0f));	//카메라는 월드상에서 y축을 기준으로 180도 만큼 회전되어있다.

		//카메라의 위치를 케릭터의 뒷방향으로 이동
		i3Matrix::PostRotateAxis( pMat, i3Matrix::GetRight( pMat), m_pTargetChara->getBoneContext()->getRho());

		i3Vector::Copy( &vOriginal, m_vBasePosition);

		addZ( &vOriginal, Distance);

		if( m_IsCamLeft)
		{
			setX( &vOriginal, -getX( &vOriginal));
		}

		i3Vector::TransformCoord( &vTemp, &vOriginal, pMat);

		/////////////////////////////////////////////////////////////////////////////
		//CheckCollision
		i3CollideeLine Col;
		VEC3D ve;

		i3Vector::Add( &ve, &vTemp, &vPointPos);

		Col.SetStart( &vPointPos);
		Col.SetEnd( &ve);

		I3_PHYSIX_HIT_RESULT * pResult;

		pResult = g_pFramework->raycastClosest( &Col, CGRP_MASKGROUP_ATTACK);
		if( pResult != nullptr)
		{
			VEC3D vecInter;
			i3Vector::Sub( &vecInter, Col.GetEnd(), &pResult->m_Intersect);

			addZ( &vOriginal, - i3Vector::Length(&vecInter));

			i3Vector::TransformCoord( &vTemp, &vOriginal, pMat);
		}

		i3Vector::Add( vCamPos, &vTemp, &vPointPos);
	}

	CGameCamera::ReCalculateMatrix();
}
