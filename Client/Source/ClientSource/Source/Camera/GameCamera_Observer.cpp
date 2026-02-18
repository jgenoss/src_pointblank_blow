#include "pch.h"
#include "GameCamera_Observer.h"
#include "GlobalVariables.h"
#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"

#include "GameMaster.h"

I3_CLASS_INSTANCE(CGameCamera_Observer);//, CGameCamera);


void CGameCamera_Observer::InitCamera(CGameCharaBase* pChar,REAL32 rInterpolateTime)
{
	CGameCamera::InitCamera(pChar, rInterpolateTime);	

	if( m_pTargetChara )
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

void CGameCamera_Observer::OnUpdate(REAL32 rDeltaSeconds)
{
	CGameCamera::OnUpdate(rDeltaSeconds);

	if (isEnabledInterpolate())
	{
		CGameCamera::UpdateInterpolate(rDeltaSeconds);
	}
	else
	{
		_UpdateInterpolateObserver(rDeltaSeconds);
	}
}

void CGameCamera_Observer::ReCalculateMatrix(void)
{
	if( nullptr == m_pTargetChara ) return;

	if( m_bNeedUpdate == false)	return;	

	// 캐릭터와 방향 맞춤
	VEC3D vecAt;
	REAL32	rTheta;

	i3Vector::Copy(&vecAt, i3Matrix::GetAt(m_pTargetMatrix));
	rTheta = i3Math::atan2( i3Vector::GetX(&vecAt), i3Vector::GetZ(&vecAt));
	i3Matrix::SetRotateY( m_pCamMatrix, rTheta + I3_DEG2RAD( 180.0f));

	// 캐릭터 시점을 따라 카메라 회전
	REAL32 rRhoTheta = m_pTargetChara->getBoneContext()->getRho();
	//if( m_pTargetChara->IsRex() == false)
	{
		rRhoTheta = i3Math::div(rRhoTheta, 10.0f);		// 회전 각 제한 1/10 - 각도제한 풀리면서 수정
	}

	i3Matrix::PostRotateAxis(m_pCamMatrix, i3Matrix::GetRight(m_pTargetMatrix), -rRhoTheta);
	
	// 캐릭터 허리각에 따라 카메라 위치 이동
	REAL32 rSittingHeight = m_pTargetChara->getCameraContext()->getIKHeightForCam();	// 허리기준 회전을 위한 허리 높이 
	VEC3D vBasePosition;
	VEC3D vecPos;

	i3Vector::Copy(&vBasePosition, m_vBasePosition);
	i3Vector::AddY(&vBasePosition, -rSittingHeight);
	if( m_pTargetChara->IsRex()  ||m_pTargetChara->IsDomiMutantRex())
	{
		i3Vector::AddZ( &vBasePosition, 4.5f);
		rRhoTheta = rRhoTheta - I3_DEG2RAD( 15.0f);
	}
#ifdef I3_DEBUG
	i3Vector::AddZ( &vBasePosition, 1.f);
	rRhoTheta = rRhoTheta - I3_DEG2RAD( 15.0f);
	
	if( m_pTargetChara->IsRex()  ||m_pTargetChara->IsDomiMutantRex())
	{
		i3Vector::AddZ( &vBasePosition, 4.5f);
		rRhoTheta = rRhoTheta - I3_DEG2RAD( 15.0f);
	}
#endif

	i3Vector::TransformCoord(&vecPos, &vBasePosition, m_pCamMatrix);
	i3Matrix::SetPos(m_pCamMatrix, &vecPos);
 	i3Matrix::PostRotateAxis(m_pCamMatrix, i3Matrix::GetRight(m_pTargetMatrix), -rRhoTheta);
	
	// 캐릭터 뒤로 이동
	i3Vector::Add(&vecPos, i3Matrix::GetPos( m_pCamMatrix), i3Matrix::GetPos(m_pTargetMatrix));
	i3Vector::AddY(&vecPos, rSittingHeight);	// 회전을 위해 뺀 높이를 더한다
	if( m_pTargetChara->IsRex() ||m_pTargetChara->IsDomiMutantRex() )
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

void CGameCamera_Observer::OnEndInterpolate(void)
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

void CGameCamera_Observer::_UpdateInterpolateObserver( REAL32 rDeltaSeconds)
{
	if ( CGameMaster::i()->IsPaused() || rDeltaSeconds <= 0.0f )
	{
		return;
	}

	MATRIX	Mat; 
	VEC3D	Pos, AtPos;

	if( m_pTargetChara == nullptr )
		return;
	
	i3Vector::Copy( &m_vEnd, i3Matrix::GetPos(m_pCamMatrix) );
	i3Vector::Copy( &m_vEndAt,  i3Matrix::GetAt(m_pCamMatrix) );

	i3Vector::Normalize( &m_vEndAt, &m_vEndAt);
	i3Vector::Scale( &m_vEndAt, &m_vEndAt, ATPOSSCALE);
	i3Vector::Add( &m_vEndAt, &m_vEndAt, i3Matrix::GetPos( m_pCamMatrix));

	//1. Set Time 			
	REAL32 rDiv	= 0.f;
	REAL32 rRatio = 0.f;
	REAL32 rInter = 1.f;
	if( m_InterpolateTerm > I3_EPS)
	{
		//rDiv = i3Math::div(	rDeltaSeconds, 0.16666666f);
		REAL32 rDelay = m_InterpolateTerm * rDeltaSeconds * 100.f ;
		rDiv = i3Math::div(	rDeltaSeconds, rDelay);
		rRatio = i3Math::sin( I3_DEG2RAD(rDiv * 180.0f));
		//rInter = rDiv + ((rDiv * rRatio) * 0.333f);
		rInter = rDiv + ((rDiv * rRatio) * rDelay * 2.f);
	}

	REAL32 Inverse  = 1.0f - rInter;

	//Position
	setX( &Pos, ( Inverse * getX(&m_vStart) ) + (rInter * getX(&m_vEnd))); 
	setY( &Pos, ( Inverse * getY(&m_vStart) ) + (rInter * getY(&m_vEnd))); 
	setZ( &Pos, ( Inverse * getZ(&m_vStart) ) + (rInter * getZ(&m_vEnd))); 

	//Rotation
	setX( &AtPos, ( Inverse * getX(&m_vStartAt) ) + (rInter * getX(&m_vEndAt))); 
	setY( &AtPos, ( Inverse * getY(&m_vStartAt) ) + (rInter * getY(&m_vEndAt))); 
	setZ( &AtPos, ( Inverse * getZ(&m_vStartAt) ) + (rInter * getZ(&m_vEndAt))); 

	//4. Set Camera At

	REAL32 Theta, Rho;
	i3Vector::Sub(&AtPos,  &Pos, &AtPos); 
	Theta = atan2f( i3Vector::GetX(&AtPos), i3Vector::GetZ(&AtPos) );
	i3Matrix::PostRotateY(&Mat, Theta);

	REAL32 fLen;
	fLen = i3Math::Sqrt( (getX(&AtPos) * getX(&AtPos)) + (getZ(&AtPos) * getZ(&AtPos)) );
	Rho = atan2f(getY(&AtPos), fLen);
	i3Matrix::PreRotateX(&Mat, - Rho);	
	i3Matrix::SetPos(&Mat, &Pos); 

	i3Matrix::Copy(m_pCamMatrix, &Mat);


	// Set Start
	i3Vector::Copy(&m_vStart, i3Matrix::GetPos( m_pCamMatrix));
	i3Vector::Copy(&m_vStartAt,	i3Matrix::GetAt( m_pCamMatrix));

	i3Vector::Normalize(&m_vStartAt, &m_vStartAt);
	i3Vector::Scale( &m_vStartAt, &m_vStartAt, ATPOSSCALE);
	i3Vector::Add(&m_vStartAt, &m_vStartAt, i3Matrix::GetPos( m_pCamMatrix));

	//i3Vector::Dump(&m_vStartAt);
}

void CGameCamera_Observer::CalculateNoneTargetPosition( CGameCharaBase* pPreviousChara)
{
	m_bEnabledInterpolate = false;

	if( pPreviousChara == nullptr) return;

	VEC3D vTargetPos, vCameraPos;
	i3Vector::Copy( &vTargetPos, i3Matrix::GetPos( pPreviousChara->getBoneContext()->getIKMatrix()));

	MATRIX mat;
	VEC3D vDir;
	i3Vector::Set( &vDir, 1.f, 1.f, 1.f);
	i3Vector::Normalize( &vDir, &vDir);
	i3Vector::Scale( &vDir, &vDir, 3.5f);

	i3Matrix::PreRotateAxis( &mat, &I3_YAXIS, I3_DEG2RAD(rand()%360));
	i3Vector::TransformNormal( &vDir, &vDir, &mat);

	i3Vector::Add( &vCameraPos, &vTargetPos, &vDir);
	
	//I3TRACE( "vDir		x : %.3f		y : %.3f		z : %.3f\n", vDir.x,vDir.y, vDir.z);


	i3Matrix::Identity( &mat);
	
	VEC3D vAt, vxzDir;
	i3Vector::Copy( &vAt, i3Matrix::GetAt( &mat));
	i3Vector::Copy( &vxzDir, &vDir);

	i3Vector::SetY( &vxzDir, 0.f); //,,,
	
	REAL32 rDot = i3Vector::Dot( &vxzDir, &vAt);
	REAL32 rTemp = i3Vector::Length( &vxzDir) * i3Vector::Length( &vAt);
	REAL32 rTheta = acosf( rDot/rTemp);

	if( vxzDir.x < I3_EPS) rTheta = rTheta * -1.f;

	i3Matrix::PreRotateY( &mat, rTheta);

	//I3TRACE("Y Theta : %.3f\n", rTheta);
	//I3TRACE("Y Theta(Degree) : %.3f\n", I3_RAD2DEG(rTheta));

	VEC3D vTemp;
	i3Vector::Copy( &vTemp, &vDir);
	i3Vector::Normalize( &vTemp, &vTemp);
	REAL32 fLen, Rho;
	fLen = i3Math::Sqrt( (getX(&vTemp) * getX(&vTemp)) + (getZ(&vTemp) * getZ(&vTemp)) );
	Rho = atan2f( vTemp.y, fLen);
	i3Matrix::PreRotateX(&mat, - Rho);	

	
	//
	i3CollideeLine ColLine;
	I3_PHYSIX_HIT_RESULT* pResult = nullptr;

	REAL32 len = i3Vector::Normalize( &vDir, &vDir);

	ColLine.SetStart( &vTargetPos);
	ColLine.SetEnd( &vCameraPos);
	ColLine.SetDir( &vDir);
	
	pResult = g_pFramework->raycastClosest( &ColLine, CGRP_MASKGROUP_ONLYWORLDHIT);
	if( pResult != nullptr) 
	{
		VEC3D	vecNew;
		i3Vector::Sub( &vecNew, &pResult->m_Intersect, &vTargetPos);
		REAL32 testlen = i3Vector::Normalize( &vecNew, &vecNew);

		if( testlen < len)
		{
			VEC3D vecRevert;
			i3Vector::Scale( &vecRevert, &vecNew, - 0.05f);

			VEC3D vSize;
			i3Vector::Scale( &vSize, &pResult->m_Normal, 0.05f);

			i3Vector::Add( &vecRevert, &vecRevert, &vSize);
			i3Vector::Add( &vCameraPos, &pResult->m_Intersect, &vecRevert);
		}
	}
	g_pFramework->resetRaycastResult();
	

	i3Matrix::SetPos( &mat, &vCameraPos);

	i3Matrix::Copy(m_pCamMatrix, &mat);
}