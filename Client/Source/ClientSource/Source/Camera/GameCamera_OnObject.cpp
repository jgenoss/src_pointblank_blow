#include "pch.h"
#include "GameCamera_OnObject.h"
#include "GlobalVariables.h"
#include "GameCharaBoneContext.h"
#include "GlobalFunc.h"
#include "GameCharaInputContext.h"

I3_CLASS_INSTANCE( CGameCamera_OnObject);//, CGameCamera);

CGameCamera_OnObject::CGameCamera_OnObject(void)
{
	m_CamFOV		= 34.5f;
	m_CamNear		= 0.1f;
	m_CamFar		= 1000.0f;
}


void CGameCamera_OnObject::InitCamera(CGameCharaBase * pChar, REAL32 rInterpolateTime)
{
	// BugFix - M197 사용중 헬기 폭파로 사망후 다음 M197을 사용시 Tremble이 작동되는 문제. 2011.05.26. swoongoo
	InitTremble();

	if(pChar == nullptr)
	{
		m_pi3Obj = nullptr;
		m_pTargetMatrix = nullptr;
	}

	m_pTargetChara = pChar;

	if (m_pTargetChara)
	{
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

void CGameCamera_OnObject::SetCameraFromTrans(i3Body* pBody, INT32 idx, REAL32 rInterpolateTerm, i3Object* pObj)
{
	I3ASSERT(pBody);
	I3ASSERT(pObj);

	m_pTargetMatrix = pBody->getCacheMatrix(idx);
	m_pi3Obj = pObj;

	SetInterPolate( rInterpolateTerm);
}


void CGameCamera_OnObject::OnUpdate( REAL32 rDeltaSeconds)
{
	CGameCamera::OnUpdate( rDeltaSeconds);
}

void CGameCamera_OnObject::OnEndInterpolate()
{
}

void CGameCamera_OnObject::OnInputKeyboard( UINT64 nKeyPress, UINT64 nKeyStroke)
{
	CGameCamera::OnInputKeyboard( nKeyPress, nKeyStroke);
}

void CGameCamera_OnObject::ReCalculateMatrix() 
{
 	if( m_bNeedUpdate == false)	return;
	if( m_pTargetMatrix == nullptr) return;
	if( m_pCamMatrix == nullptr) return;
	

	MATRIX matResult;
	MATRIX matTemp;
	
	i3Matrix::PostRotateX( &matResult, I3_DEG2RAD( 90.0f));
	i3Matrix::PostRotateZ( &matTemp, I3_DEG2RAD( 90.0f));
	i3Matrix::Mul(&matResult, &matResult, &matTemp);
	
	matResult.m30 = -getZ( m_vBasePosition);

	i3Matrix::Mul(&matResult, &matResult, m_pTargetMatrix);

	i3Matrix::Copy(m_pCamMatrix, &matResult);

	CGameCamera::ReCalculateMatrix();
}
