#include "pch.h"
#include "GameCamera.h"
#include "GlobalVariables.h"
#include "GameCharaBase.h"
#include "i3Scene/i3XrayScene.h"

I3_CLASS_INSTANCE( CGameCamera);

bool CGameCamera::m_bCameraInitForUSG = false;
VAR_NMLR(i3Camera*, 1)	CGameCamera::m_pCamera;
VAR_NMLR(MATRIX*, 2)	CGameCamera::m_pCamMatrix;

VEC3D		CGameCamera::m_vMouseDelta;
UINT32		CGameCamera::m_nMouseState	= 0;
UINT64		CGameCamera::m_nKeyPress	= 0;
UINT64		CGameCamera::m_nKeyStroke	= 0;
bool		CGameCamera::m_IsCamLeft	= false;

CGameCamera::CGameCamera()
{
	if (m_bCameraInitForUSG == false)	// USG 초기화 시점 관련 에러로 추가. 울산가드 글로벌 변수는 전역단에서 초기화 하면 안됨(루코셀 메인 보안 모듈 실행 후 초기화 가능).
	{
		m_pCamera = nullptr;
		m_pCamMatrix = nullptr;

		m_bCameraInitForUSG = true;
	}
}

CGameCamera::~CGameCamera(void)
{
	I3_SAFE_DELETE(m_vBasePosition);
}

void	CGameCamera::_ResetCamStatus()
{
	m_bNeedUpdate	= true;

	m_pTargetChara	=	nullptr;
	m_pTargetMatrix	=	nullptr;
	i3Vector::Zero( &m_vOldMouseDelta);

	//Zoom
	m_bEnabledZoom		= false;
	m_rZoomElapsedTime	= 0.0f;
	m_rTargetFOV		= m_CamFOV;
	m_rZoomTime			= 0.0f;
	m_rZoomRatio		= 1.0f;

	//Tremble
	InitTremble();

	//Interpolate
	m_bEnabledInterpolate	= false;
	m_InterAccTime		= 0.0f;
	m_InterpolateTerm	= 0.0f;
	i3Vector::Zero( &m_vStart);
	i3Vector::Zero( &m_vEnd);
	i3Vector::Zero( &m_vStartAt);
	i3Vector::Zero( &m_vEndAt);
}

void	CGameCamera::InitCamera(CGameCharaBase * pChar, REAL32 rInterpolateTime/*, i3Camera * m_UICamera*/)
{
	_ResetCamStatus();

	if( m_pCamera == nullptr)
	{
		m_pCamera = g_pFramework->GetDefaultCamera();
		m_pCamMatrix = m_pCamera->GetMatrix();
	}

	//캐릭터와의 연관 여부는 카메라에 따라 다를수 있음.
	m_pTargetChara = pChar;

	if( pChar != nullptr)
		m_pTargetMatrix = pChar->GetMatrix();
	else
		m_pTargetMatrix = m_pCamMatrix;

	_SetCameraProjection();
	
	{
		SetInterPolate( rInterpolateTime);
	}

	m_bNeedUpdate	= true;

	g_pCamera		= this;
	m_rZoomRatio	= (m_CamFOV * ( 1.0f/m_rDefaultFOV));
}

void	CGameCamera::_SetCameraProjection()
{
	i3RenderTarget	* pRT = g_pViewer->GetRenderContext()->GetRenderTarget(0);
	I3ASSERT( pRT != nullptr);
	
	m_pCamera->SetPerspective( I3_DEG2RAD( m_CamFOV), (REAL32)pRT->GetWidth(), (REAL32)pRT->GetHeight(), m_CamNear, m_CamFar);
}

void CGameCamera::SetCameraFOV(REAL32	rFov)
{
	m_CamFOV = rFov;

	if( m_pCamera != nullptr)
		m_pCamera->SetFOV( I3_DEG2RAD(m_CamFOV));
}
void CGameCamera::SetCameraNear(REAL32 rNear)
{	
	m_CamNear = rNear;
	if( m_pCamera != nullptr)
		m_pCamera->SetNear( m_CamNear);
}

void CGameCamera::SetCameraFar(REAL32	rFar)
{
	m_CamFar = rFar;
	if( m_pCamera != nullptr)
		m_pCamera->SetFar( m_CamFar);
}

void CGameCamera::SetDefaultFOV(REAL32 rTime)
{
	ZoomIn( m_rDefaultFOV, rTime);
}

void CGameCamera::ZoomIn( REAL32 rFov, REAL32 rTime)
{
	if( m_bEnabledZoom == true)
	{
		m_rZoomElapsedTime	= m_rZoomTime - m_rZoomElapsedTime;
	}
	else
	{
		m_rZoomElapsedTime	= 0.0f;
	}

	m_bEnabledZoom		= true;

	m_rTargetFOV		= rFov;
	m_rZoomTime			= rTime;

	m_rZoomRatio		= rFov/ m_rDefaultFOV;
}

void CGameCamera::UpdateZoom( REAL32 rDeltaSeconds)
{
	if (m_pTargetChara)
	{
		g_pFramework->getSgContext()->GetXrayScene()->SetXrayTargetNode(m_pTargetChara->GetNode());
		g_pFramework->getSgContext()->GetXrayScene()->SetXrayZoomRate(m_rTargetFOV);
	}

	if( m_bEnabledZoom == false)
		return ;

	m_rZoomElapsedTime += rDeltaSeconds;
	if( m_rZoomElapsedTime >= m_rZoomTime)
	{
		m_CamFOV = m_rTargetFOV;
		_SetCameraProjection();

		m_bEnabledZoom = false;
	}

	REAL32 rDelta	= m_rTargetFOV - m_CamFOV;
	REAL32 rDiv		= i3Math::div( m_rZoomElapsedTime, m_rZoomTime);
	REAL32 rRatio	= i3Math::sin_libm( I3_DEG2RAD(rDiv * 90.0f));
	REAL32 rFov		= m_CamFOV + (rDelta * rRatio);

	m_pCamera->SetFOV( I3_DEG2RAD( rFov));
}

void CGameCamera::PivotRotate(MATRIX *pMat, REAL32 rDeltaX, REAL32 rDeltaY)
{
	VEC3D  localPos, vecRight;
	i3Vector::Copy( &localPos, i3Matrix::GetPos( pMat));
	i3Matrix::SetPos( pMat, &localPos);

	i3Vector::Copy( &vecRight, i3Matrix::GetRight(pMat));
	i3Vector::Normalize( &vecRight, &vecRight);

	i3Matrix::PostRotateAxis( pMat, &vecRight, rDeltaY);
	i3Matrix::PostRotateAxis( pMat, &I3_YAXIS, rDeltaX);

	i3Matrix::SetPos( pMat, &localPos);
}

void CGameCamera::SlantWise( MATRIX * pMat, REAL32 rDelta)
{
	VEC3D localPos, vecAt;
	i3Vector::Copy( &localPos, i3Matrix::GetPos( pMat));

	i3Vector::Normalize( &vecAt, i3Matrix::GetAt( pMat));

	i3Matrix::PostRotateAxis( pMat, &vecAt, rDelta);

	i3Matrix::SetPos( pMat, &localPos);
}

void CGameCamera::InitTremble( void)
{
	m_bEnabledTremble	= false;
	m_TrembleAccTime	= 0.0f;
	m_TrembleTime		= 0.0f;
	m_rTrembAmp			= 0.0f;
	m_rTrembFreq		= 0.0f;
	m_rTrembPhase		= 0.0f;
}

void CGameCamera::SetTremble( REAL32 rAmp, REAL32 rFreq, REAL32 rPhase, REAL32 rTrembleTime)
{
	m_rTrembAmp			= rAmp;
	m_rTrembFreq		= rFreq;
	m_rTrembPhase		= rPhase;
	m_bEnabledTremble	= true;
	m_TrembleTime		= rTrembleTime;
	m_TrembleAccTime	= 0.0f;
}

void CGameCamera::UpdateTremble( REAL32 rDeltaSeconds)
{
	if( m_bEnabledTremble)
	{
		REAL32	T = I3_PI * ( m_TrembleAccTime * m_rTrembFreq) + m_rTrembPhase;
		REAL32	H = m_rTrembAmp * ((m_TrembleTime - m_TrembleAccTime)/m_TrembleTime);

		REAL32 w = sin( 5.0f * T) * H;
		REAL32 c = cos( 2.0f * T) * H;

		SlantWise( m_pCamMatrix, I3_DEG2RAD( w));

		i3Matrix::PreRotateX(m_pCamMatrix, I3_DEG2RAD( c));

		m_TrembleAccTime += rDeltaSeconds;
		if( m_TrembleAccTime >= m_TrembleTime)
		{
			m_bEnabledTremble = false;
		}
	}
}

void	CGameCamera::OnUpdate( REAL32 rDeltaSeconds)
{
	UpdateZoom( rDeltaSeconds);

	ReCalculateMatrix();
	//UpdateInterpolate( rDeltaSeconds);

	UpdateTremble( rDeltaSeconds);

	i3GameNode::OnUpdate( rDeltaSeconds);

	//현재 프레임의 마우스 회전량 및 위치 저장
	i3Vector::Copy( &m_vOldMouseDelta, &m_vMouseDelta) ;	//이전 프레임에서의 마우스 회전량
}

void	CGameCamera::ToggleCamSidePos()
{
	m_IsCamLeft = !m_IsCamLeft;
}

void CGameCamera::OnEndInterpolate()
{
	
}

void	CGameCamera::ReCalculateMatrix()
{
	//m_bNeedUpdate = false;
}

void	CGameCamera::OnInputKeyboard( UINT64 nKeyPress, UINT64 nKeyStroke)
{
	m_nKeyPress = nKeyPress;
	m_nKeyStroke = nKeyStroke;

	//m_bNeedUpdate = true;
}

void	CGameCamera::OnInputMouse( VEC3D * pvecDelta, UINT32 nState)
{
	i3Vector::Sub( &m_vMouseDelta, &m_vMouseDelta, pvecDelta);
	setY( &m_vMouseDelta, (REAL32) (((INT32) getY( &m_vMouseDelta)) % 180) );

	m_nMouseState	=	nState;

	//m_bNeedUpdate = true;
}

void CGameCamera::SetInterPolate( REAL32 rInterpolateTime)
{
	if( rInterpolateTime > 0.f)		m_bEnabledInterpolate	= true;
	else							m_bEnabledInterpolate	= false;

 	m_InterpolateTerm		= rInterpolateTime;
	m_InterAccTime			= 0.0f;

	i3Vector::Copy(&m_vStart, i3Matrix::GetPos( m_pCamMatrix));
	i3Vector::Copy(&m_vStartAt,	i3Matrix::GetAt( m_pCamMatrix));

	i3Vector::Normalize(&m_vStartAt, &m_vStartAt);
	i3Vector::Scale( &m_vStartAt, &m_vStartAt, ATPOSSCALE);
	i3Vector::Add(&m_vStartAt, &m_vStartAt, i3Matrix::GetPos( m_pCamMatrix));

	//I3TRACE( "Camera interpolate %.4f\n", rInterpolateTime);
}

void CGameCamera::UpdateInterpolate( REAL32 rDeltaSeconds)
{
	if( m_bEnabledInterpolate == false)	
		return;

	if( m_InterAccTime >= m_InterpolateTerm)
	{		
		m_bEnabledInterpolate = false;
		OnEndInterpolate();
		return;
	}

	MATRIX	Mat; 
	VEC3D	Pos, AtPos;

	i3Vector::Copy( &m_vEnd, i3Matrix::GetPos(m_pCamMatrix) );
	i3Vector::Copy( &m_vEndAt,  i3Matrix::GetAt(m_pCamMatrix) );

	i3Vector::Normalize( &m_vEndAt, &m_vEndAt);
	i3Vector::Scale( &m_vEndAt, &m_vEndAt, ATPOSSCALE);
	i3Vector::Add( &m_vEndAt, &m_vEndAt, i3Matrix::GetPos( m_pCamMatrix));

	//1. Set Time 			
	REAL32 rDiv		= i3Math::div( m_InterAccTime, m_InterpolateTerm);
	REAL32 rRatio	= i3Math::sin( I3_DEG2RAD(rDiv * 180.0f));
	REAL32 rInter	= rDiv + ((rDiv * rRatio) * 0.33f);
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

	m_InterAccTime += rDeltaSeconds;
	return;
}

