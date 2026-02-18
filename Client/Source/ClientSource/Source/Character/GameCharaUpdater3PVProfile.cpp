#include "pch.h"
#include "GameCharaUpdater3PVProfile.h"
#include "GlobalVariables.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaUpdater3PVProfile);

void CGameCharaUpdater3PVProfile::OnFirstUpdate( REAL32 rDeltatime)
{
	//if( m_pOwner->is3PV())
	//{
	//	// Animation과 World-Space Transform 사이에 LOD 계산이 달라질 수 있으며,
	//	// 그것은 Visual 상에 여러 문제가 될 수 있기 때문에
	//	// 수동으로 LOD를 계산시킨다.
	//	//	Zoom시 FOV 비율 계산
	//	REAL32 fRatio = 1.f;
	//	VEC3D CamPos;
	//	
	//	if( g_pCamera != nullptr)
	//	{
	//		fRatio = g_pCamera->GetCameraDefaultFOV() / g_pCamera->GetCameraFOV();
	//		i3Vector::Copy( &CamPos, i3Matrix::GetPos( g_pCamera->getCamMatrix()));
	//	}
	//	else
	//	{
	//		i3Vector::Set( &CamPos, 0.f, 0.5f, 1.f);
	//	}


	//	//	모델링 LOD 계산
	//	m_pOwner->calcLOD( m_pOwner->GetPos(), &CamPos, fRatio);
	//}

	getBoneContext()->UpdateAnim( rDeltatime);				// Animation의 Key 값을 적용 (각 Bone의 Cache Matrix에 저장)
	getBoneContext()->_CalcTheta( rDeltatime);				// Position
	getBoneContext()->_CalcSpineBones();
}

void CGameCharaUpdater3PVProfile::OnUpdate( REAL32 rDeltatime)
{
	CGameCharaUpdater3PV::OnUpdate( rDeltatime);
		
	//	캐릭터 모델링 알파 처리
	getMaterialContext()->ProcessMaterial( rDeltatime);
}

