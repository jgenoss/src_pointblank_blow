#include "pch.h"
#include "GameCamera_FPS.h"
#include "GlobalVariables.h"
#include "GameCharaBoneContext.h"
#include "GlobalFunc.h"

I3_CLASS_INSTANCE( CGameCamera_FPS); //, CGameCamera);

CGameCamera_FPS::CGameCamera_FPS(void)
{
	i3Vector::Set( m_vBasePosition, 0.0f, 0.5f, 0.2f);

	m_CamFOV		= 34.5f;
	m_CamNear		= 0.1f;
	m_CamFar		= 1000.0f;
}


void CGameCamera_FPS::InitCamera(CGameCharaBase * pChara, REAL32 rInterpolateTime/*, i3Camera * pCamera*/)
{
	CGameCamera::InitCamera( pChara, rInterpolateTime/*, pCamera*/);

	if( m_pTargetChara != nullptr)
	{
		//m_pTargetChara->ChangeFPSView();
		m_pTargetChara->Cmd_ChangeView( CHARA_VIEW_1PV);


		if (m_pTargetChara->IsEnabledOccludee() == true)
		{
			m_pTargetChara->EnableOccludee(false);
			m_pTargetChara->EnableRenderAABB(false);

			if (m_pTargetChara->GetCurrentCharaWeapon())
			{
				m_pTargetChara->GetCurrentCharaWeapon()->EnableRenderAABB(false);
			}
		}

	}
	
	m_bNeckAnim = false;
}

void CGameCamera_FPS::AdjustTargetByCamera()
{
	if( m_pTargetChara != nullptr)
	{
		//m_pTargetChara->ChangeFPSView();
		if(m_pTargetChara->is1PV() == false)
		{
			m_pTargetChara->Cmd_ChangeView( CHARA_VIEW_1PV);
		}
	}
}

void CGameCamera_FPS::RespawnCamera()
{
	m_bNeckAnim = false;
}

void CGameCamera_FPS::OnUpdate( REAL32 rDeltaSeconds)
{
	CGameCamera::OnUpdate( rDeltaSeconds);
}

void CGameCamera_FPS::OnEndInterpolate()
{
	if( m_pTargetChara != nullptr)
	{
		//m_pTargetChara->ChangeFPSView();
		m_pTargetChara->Cmd_ChangeView( CHARA_VIEW_1PV);
	}
}

void CGameCamera_FPS::OnInputKeyboard( UINT64 nKeyPress, UINT64 nKeyStroke)
{
	CGameCamera::OnInputKeyboard( nKeyPress, nKeyStroke);
}

void CGameCamera_FPS::ReCalculateMatrix()
{
	if(m_bNeckAnim)
	{
		ReCalculateMatrixWithNeckAnim();
	}
	else
	{
		ReCalculateMatrixNormal();
	}
}

void CGameCamera_FPS::ReCalculateMatrixWithNeckAnim()
{
	if( nullptr == m_pTargetChara  ) return;
 	if( m_bNeedUpdate == false)	return;

	VEC3D	vAt, vPos, vUp, vRight;
	VEC3D offset;
	
	INT32 idxBone = 0;

	i3Vector::Zero(&vPos);

	if( m_pTargetChara->is1PV())
	{	//·¾ÅÍ½ºÆÃ
		if(m_pTargetChara->IsRaptorBase()) {
			if (m_pTargetChara->IsTank())
			{
				idxBone = RAPTOR_BONE_SPINE2;
			}
			else
				idxBone = RAPTOR_BONE_NECK;
		}
		else if(m_pTargetChara->IsRex()) {
			idxBone = TREX_BONE_PIVOTCAM;
		}
		else {
			idxBone = BONE_NECK;
		}
	}
	else
	{
		idxBone = BONE_IK;
	}

	i3Matrix::Identity( m_pCamMatrix);

	MATRIX matTemp;
	i3Matrix::Copy(&matTemp, m_pTargetChara->getBoneContext()->getBoneMatrix( idxBone));

	// GlobalFunc::PB_TRACE("ReCalculateMatrix At    %f %f %f\n", i3Matrix::GetUp(&matTemp)->x, i3Matrix::GetUp(&matTemp)->y, i3Matrix::GetUp(&matTemp)->z);
	// GlobalFunc::PB_TRACE("ReCalculateMatrix Right %f %f %f\n", i3Matrix::GetRight(m_pCamMatrix)->x, i3Matrix::GetRight(m_pCamMatrix)->y, i3Matrix::GetRight(m_pCamMatrix)->z);
	// GlobalFunc::PB_TRACE("ReCalculateMatrix Up    %f %f %f\n", i3Matrix::GetUp(m_pCamMatrix)->x, i3Matrix::GetUp(m_pCamMatrix)->y, i3Matrix::GetUp(m_pCamMatrix)->z);

	i3Vector::Copy( &vAt, i3Matrix::GetUp( m_pTargetChara->getBoneContext()->getBoneMatrix( idxBone)));
	i3Vector::Copy( &vUp, i3Matrix::GetRight( m_pTargetChara->getBoneContext()->getBoneMatrix( idxBone)));

	i3Vector::Normalize(&vAt, &vAt);
	i3Vector::Normalize(&vUp, &vUp);

	vAt.x = -vAt.x;
	vAt.y = -vAt.y;
	vAt.z = -vAt.z;

	i3Matrix::SetAt(m_pCamMatrix, &vAt);
	i3Matrix::SetUp(m_pCamMatrix, &vUp);
	i3Vector::Cross(&vRight, &vUp, &vAt);

	i3Matrix::SetRight(m_pCamMatrix, &vRight);

	m_pTargetChara->getDBInfo()->GetFPSCamPos( &offset);
	if( m_pTargetChara->isLeftHand())	offset.x = -offset.x;

	i3Vector::TransformNormal(&vPos, (VEC3D *) &offset, m_pCamMatrix);
	i3Vector::Add(&vPos, &vPos, i3Matrix::GetPos( m_pTargetChara->getBoneContext()->getBoneMatrix( idxBone)));
	
	
	i3Matrix::SetPos(m_pCamMatrix, &vPos);

	

	CGameCamera::ReCalculateMatrix();
}


void CGameCamera_FPS::ReCalculateMatrixNormal() 
{
 	if( m_bNeedUpdate == false)	return;
	if( m_pTargetChara == nullptr )	return;

	REAL32	Theta;
	VEC3D	vAt, vPos;
	INT32 idxBone = 0;
	VEC3D offset;
	
	if( m_pTargetChara->IsDino())
	{
		if( m_pTargetChara->IsRaptorBase())
		{
			if (m_pTargetChara->IsTank())
				idxBone = RAPTOR_BONE_SPINE2;
			else
				idxBone = RAPTOR_BONE_NECK;
		}
		else
			idxBone = TREX_BONE_PIVOTCAM;
	}
	else
	{
		if( m_pTargetChara->is1PV())
		{
			idxBone = BONE_NECK;
		}
		else
		{
			idxBone = BONE_IK;
		}
	}

	m_pTargetChara->getDBInfo()->GetFPSCamPos( &offset);

	if( m_pTargetChara->isLeftHand())
		offset.x = -offset.x;

	i3Matrix::SetTranslate( m_pCamMatrix, &offset);

	i3Vector::Copy( &vAt, i3Matrix::GetAt( m_pTargetMatrix));


	Theta = atan2f( getX( &vAt), getZ( &vAt));
	i3Matrix::PostRotateY( m_pCamMatrix, Theta + I3_DEG2RAD( 180.0f));

	//Rotate
	i3Matrix::PostRotateAxis( m_pCamMatrix, i3Matrix::GetRight( m_pTargetMatrix), - m_pTargetChara->getBoneContext()->getViewRho());// + 0.15f);

	i3Matrix::Mul( m_pCamMatrix, m_pCamMatrix, m_pTargetChara->getBoneContext()->getShoveMatrix());

	i3Vector::Copy( &vPos, i3Matrix::GetPos( m_pTargetChara->getBoneContext()->getBoneMatrix( idxBone)));
	i3Vector::Add( i3Matrix::GetPos(m_pCamMatrix), i3Matrix::GetPos(m_pCamMatrix), &vPos);

	CGameCamera::ReCalculateMatrix();
}

