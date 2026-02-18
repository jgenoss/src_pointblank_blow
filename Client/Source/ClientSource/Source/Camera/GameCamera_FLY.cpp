#include "pch.h"
#include "GameCamera_FLY.h"
#include "GameCharaBase.h"
#include "GameCamera_LocationChange.h"

I3_CLASS_INSTANCE( CGameCamera_FLY);

CGameCamera_FLY::CGameCamera_FLY(void)
{
	m_bNeedUpdate	= false;
	m_rMoveSpeedMax	= m_MoveSpeed;
}


void CGameCamera_FLY::InitCamera( CGameCharaBase * pChar, REAL32 rInterpolateTime)
{
	CGameCamera::InitCamera(pChar, rInterpolateTime);

	if( pChar != nullptr)
	{	// 관전 모드에 대한 캐릭터 뷰를 세팅
		pChar->Cmd_ChangeView( CHARA_VIEW_3PV);

		if (m_pTargetChara->IsEnabledOccludee() == false)
		{
			m_pTargetChara->EnableOccludee(true);
			m_pTargetChara->EnableRenderAABB(true);

			if (m_pTargetChara->GetCurrentCharaWeapon())
			{
				m_pTargetChara->GetCurrentCharaWeapon()->EnableRenderAABB(true);
			}
		}

		//pChar->ChangeObserverView();
	}
}

void CGameCamera_FLY::OnInputKeyboard( UINT64 nKeyPress, UINT64 nKeyStroke)
{
	if( m_pCamera == nullptr)
		return;

	MATRIX * pMat = m_pCamMatrix;

	VEC3D vecAt;
	VEC3D vecRight;

	i3Vector::Copy( &vecAt, i3Matrix::GetAt( pMat));
	i3Vector::Copy( &vecRight, i3Matrix::GetRight( pMat));

	i3Vector::Normalize(&vecAt, &vecAt);
	i3Vector::Normalize(&vecRight, &vecRight);

	i3Vector::Scale(&vecAt, &vecAt, m_MoveSpeed * m_MoveDelta);
	i3Vector::Scale(&vecRight, &vecRight, m_MoveSpeed * m_MoveDelta);

	if( nKeyPress & GAME_KEY_MOVE_LEFT)
	{
		addX( &m_vMouseDelta, m_RotateSpeed);
	}

	if( nKeyPress & GAME_KEY_CMD_CAMPIVOT_LEFT)
	{
		addX( &m_vMouseDelta, m_RotateSpeed);
	}

	if( nKeyPress & GAME_KEY_CMD_CAMPIVOT_RIGHT)
	{
		addX( &m_vMouseDelta, - m_RotateSpeed);
	}

	if( nKeyPress & GAME_KEY_CMD_CAMPIVOT_UP)
	{
		addY( &m_vMouseDelta, m_RotateSpeed);
	}

	if( nKeyPress & GAME_KEY_CMD_CAMPIVOT_DOWN)
	{
		addY( &m_vMouseDelta, - m_RotateSpeed);
	}


	if( nKeyPress & GAME_KEY_MOVE_RIGHT)
	{
		PivotRotate( pMat, - I3_DEG2RAD( m_MoveSpeed * m_MoveDelta), 0.0f);
	}


	if( nKeyPress & GAME_KEY_MOVE_LEFT )
	{
		i3Vector::Sub(i3Matrix::GetPos(pMat), i3Matrix::GetPos(pMat), &vecRight);	
	}
	
	if( nKeyPress & GAME_KEY_MOVE_RIGHT )
	{
		i3Vector::Add(i3Matrix::GetPos(pMat), i3Matrix::GetPos(pMat), &vecRight);	
	}

	if( nKeyPress & GAME_KEY_MOVE_FORWARD )
	{
		i3Vector::Sub(i3Matrix::GetPos(pMat), i3Matrix::GetPos(pMat), &vecAt);	
	}

	if( nKeyPress & GAME_KEY_MOVE_BACKWARD )
	{
		i3Vector::Add(i3Matrix::GetPos(pMat), i3Matrix::GetPos(pMat), &vecAt);	
	}

	if( g_pFramework->getKeyboard()->GetPressState( I3I_KEY_R))
	{
		i3Vector::AddY( i3Matrix::GetPos(pMat), m_MoveSpeed * m_MoveDelta);
	}

	if( g_pFramework->getKeyboard()->GetPressState( I3I_KEY_F))
	{
		i3Vector::AddY( i3Matrix::GetPos(pMat), -(m_MoveSpeed * m_MoveDelta));
	}

	// 카메라 이동 속도 수정, - -> WHEELDOWN, = -> WHEELUP
	REAL32 mouseWheelVal = g_pFramework->getMouse()->GetDeltaZ();

	if (mouseWheelVal < 0.0f)
	{
		if (nKeyPress & GAME_KEY_ACTION_CROUCH)
		{
			m_RotateSpeed -= 0.05f;
			if( m_RotateSpeed <= 0.1f) m_RotateSpeed = 0.1f;
		}
		else
		{
			m_MoveSpeed -= 0.1f;
			if( m_MoveSpeed <= 0.1f) m_MoveSpeed = 0.1f;
		}
	}

	if (mouseWheelVal > 0.0f)
	{
		if( nKeyPress & GAME_KEY_ACTION_CROUCH)
		{
			m_RotateSpeed += 0.05f;
			if( m_RotateSpeed > 30.0f) m_RotateSpeed = 30.0f;
		}
		else
		{
			m_MoveSpeed += 0.1f;
			if( m_MoveSpeed > 30.0f) m_MoveSpeed = 30.0f;
		}
	}

	CGameCamera::OnInputKeyboard( nKeyPress, nKeyStroke);

	m_bNeedUpdate = true;
}

void CGameCamera_FLY::OnInputMouse( VEC3D * pVecDelta, UINT32 nState)
{
	m_bNeedUpdate = true;

	CGameCamera::OnInputMouse( pVecDelta, nState);
}

void CGameCamera_FLY::InitTopViewCamera()
{
	VEC3D v3Temp;

	m_rRotateY = -90.f;
	m_rPreRotateY = 0.f;
	m_rBaseRotateY	= I3_DEG2RAD(180.f);
	i3Vector::Set(&v3Temp, 0.f, 0.f, 0.f);
	SetMouseDelta(v3Temp);
}

void CGameCamera_FLY::ReCalculateMatrix()
{
	if( m_bNeedUpdate )
	{
		//임시
		REAL32	rSensitivity = 0.5f;

		VEC3D	vecPos;
		MATRIX	*pMat = m_pCamMatrix;
		
		i3Vector::Copy( &vecPos, i3Matrix::GetPos(pMat));
		i3Matrix::SetRotateY( pMat, m_rBaseRotateY);

		REAL32 rRotY = getY( &m_vMouseDelta) - m_rPreRotateY;
		m_rPreRotateY = getY( &m_vMouseDelta);
		if( i3Math::abs( rRotY ) < 120.f)
		{
			m_rRotateY += (rRotY * rSensitivity);
			m_rRotateY = MINMAX( -89.f, m_rRotateY, 89.f);
		}

		PivotRotate(pMat, I3_DEG2RAD(getX( &m_vMouseDelta) * rSensitivity),
						I3_DEG2RAD( m_rRotateY));

		i3Matrix::SetPos( pMat, &vecPos);
		//GlobalFunc::PB_TRACE("m_vMouseDelta %f m_rRotateY %f", getX( &m_vMouseDelta), m_rRotateY);
		
	}

	CGameCamera::ReCalculateMatrix();
}

void CGameCamera_FLY::OnUpdate( REAL32 rDeltaSeconds)
{
	m_MoveDelta = rDeltaSeconds;

	CGameCamera::OnUpdate( rDeltaSeconds);
}

void CGameCamera_FLY::SetBaseRotationY(const REAL32 y)
{
	m_rBaseRotateY = y;
}

void FlyCameraUtil::SaveFlyCameraLocation(I3I_KEY inputKey)
{
	CGameCamera_FLY* flyCam = static_cast<CGameCamera_FLY*>(g_pCamera);

	CameraLocationChange::Key key(g_pFramework->GetCurrentStageIndex(), inputKey);
	CameraLocationChange::Val val(flyCam->getCamMatrix(), flyCam->GetMouseDelta(), flyCam->GetRotateY(), flyCam->GetPreRotateY() );
	CameraLocationChange::TheStorage().Add(key, val);

	I3TRACE("Saving Cam Info [%d, %d]\n", g_pFramework->GetCurrentStageIndex(), inputKey);
}

void FlyCameraUtil::MoveFlyCameraLocation(I3I_KEY inputKey)
{
	CameraLocationChange::Key key(g_pFramework->GetCurrentStageIndex(), inputKey);
	CameraLocationChange::MoveLocation* f = CameraLocationChange::TheStorage().Find(key);
	if (f)
	{
		(*f)();

		I3TRACE("Locate Cam [%d, %d]\n", g_pFramework->GetCurrentStageIndex(), inputKey);
	}
}