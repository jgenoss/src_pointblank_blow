#include "pch.h"
#include "GameCamera_Observer2.h"
#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"
 
I3_CLASS_INSTANCE(CGameCamera_Observer2);

void CGameCamera_Observer2::InitCamera(CGameCharaBase* pChar,REAL32 rInterpolateTime)
{
	CGameCamera::InitCamera(pChar, rInterpolateTime);	

	if( m_pTargetChara )
	{
		m_pTargetChara->Cmd_ChangeView( CHARA_VIEW_3PV);
		//m_pTargetChara->ChangeObserverView();
		m_rRotateY = I3_RAD2DEG( m_pTargetChara->getBoneContext()->getRho());

		if (m_pTargetChara->IsEnabledOccludee() == false)
		{
			m_pTargetChara->EnableOccludee(true);
			m_pTargetChara->EnableRenderAABB(true);

			if (m_pTargetChara->GetCurrentCharaWeapon())
			{
				m_pTargetChara->GetCurrentCharaWeapon()->EnableRenderAABB(true);
			}
		}

	}
}

void CGameCamera_Observer2::OnUpdate(REAL32 rDeltaSeconds)
{
	CGameCamera::OnUpdate(rDeltaSeconds);

	CGameCamera::UpdateInterpolate(rDeltaSeconds);
}

void CGameCamera_Observer2::ReCalculateMatrix(void)
{
	if( nullptr == m_pTargetChara ) return;

	if( m_bNeedUpdate == false)	return;	

	i3Matrix::SetRotateY( m_pCamMatrix, I3_DEG2RAD(180.f));
	REAL32 rRotX = getX( &m_vMouseDelta);
	REAL32 rRotY = getY( &m_vMouseDelta) - m_rPreRotateY;
	m_rPreRotateY = getY( &m_vMouseDelta);
	if( i3Math::abs( rRotY ) < 120.f)
	{
		m_rRotateY += rRotY;
		m_rRotateY = MINMAX( -80.f, m_rRotateY, 80.f);
	}
	PivotRotate( m_pCamMatrix, I3_DEG2RAD( rRotX), I3_DEG2RAD( m_rRotateY) );
	//rRotY = MINMAX( -80.f, rRotY, 80.f);
	//PivotRotate( m_pCamMatrix, I3_DEG2RAD( rRotX), I3_DEG2RAD( rRotY) );
	
	// 캐릭터 허리각에 따라 카메라 위치 이동
	REAL32 rSittingHeight = m_pTargetChara->getCameraContext()->getIKHeightForCam();	// 허리기준 회전을 위한 허리 높이 
	VEC3D vBasePosition;
	VEC3D vecPos;

	i3Vector::Copy(&vBasePosition, m_vBasePosition);
	i3Vector::AddY(&vBasePosition, -rSittingHeight);
	if( m_pTargetChara->IsRex()  ||m_pTargetChara->IsDomiMutantRex())
	{
		i3Vector::AddZ( &vBasePosition, 4.5f);
	}

	i3Vector::TransformCoord(&vecPos, &vBasePosition, m_pCamMatrix);
	i3Matrix::SetPos(m_pCamMatrix, &vecPos);
	
	// 캐릭터 뒤로 이동
	i3Vector::Add(&vecPos, i3Matrix::GetPos( m_pCamMatrix), i3Matrix::GetPos(m_pTargetMatrix));
	i3Vector::AddY(&vecPos, rSittingHeight);	// 회전을 위해 뺀 높이를 더한다
	if( m_pTargetChara->IsRex()  ||m_pTargetChara->IsDomiMutantRex())
	{
		i3Vector::AddY(&vecPos, rSittingHeight * 2.0f);	// 회전을 위해 뺀 높이를 더한다
	}

	{
		i3CollideeLine line;
		
		MATRIX* mtHead = m_pTargetChara->getBoneContext()->getHeadMatrix();

		line.SetStart( i3Matrix::GetPos(mtHead) );
		line.SetEnd( &vecPos );
		
		I3_PHYSIX_HIT_RESULT * pResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK);
		
		if( pResult != nullptr)
		{
			VEC3D vecInter, vecLine;
			
			i3Vector::Sub( &vecInter, &pResult->m_Intersect, line.GetStart());
			i3Vector::Sub(&vecLine, &vecPos, i3Matrix::GetPos(mtHead));

			REAL32 rOriginalDistance = i3Vector::Length( &vecLine);
			REAL32 rCollisionDistance = i3Vector::Length( &vecInter);
							
			if( rCollisionDistance < rOriginalDistance)
			{
				i3Vector::Scale( &vecInter, &vecInter, 0.9f);
				i3Vector::Add( &vecPos, i3Matrix::GetPos(mtHead), &vecInter);
			}
		}
	}

	i3Matrix::SetPos( m_pCamMatrix, &vecPos);
    
	CGameCamera::ReCalculateMatrix();	
}

void CGameCamera_Observer2::OnEndInterpolate(void)
{
	if( m_pTargetChara != nullptr)
	{
		m_pTargetChara->Cmd_ChangeView( CHARA_VIEW_3PV);
		//m_pTargetChara->ChangeObserverView();
	}

	// Set Start
	i3Vector::Copy(&m_vStart, i3Matrix::GetPos( m_pCamMatrix));
	i3Vector::Copy(&m_vStartAt,	i3Matrix::GetAt( m_pCamMatrix));

	i3Vector::Normalize(&m_vStartAt, &m_vStartAt);
	i3Vector::Scale( &m_vStartAt, &m_vStartAt, ATPOSSCALE);
	i3Vector::Add(&m_vStartAt, &m_vStartAt, i3Matrix::GetPos( m_pCamMatrix));
}

