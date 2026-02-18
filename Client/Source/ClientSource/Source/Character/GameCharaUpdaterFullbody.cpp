#include "pch.h"
#include "GameCharaUpdaterFullbody.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE(CGameCharaUpdaterFullbody);

void CGameCharaUpdaterFullbody::OnFirstUpdate(REAL32 tm)
{
	REAL32 ratio = g_pCamera->GetCameraDefaultFOV() / g_pCamera->GetCameraFOV();
	m_pOwner->calcLOD(m_pOwner->GetPos(), i3Matrix::GetPos(g_pCamera->getCamMatrix()), ratio);

	getBoneContext()->UpdateAnim(tm);
	getBoneContext()->TransformWorldSpace(tm);
}

void CGameCharaUpdaterFullbody::OnUpdate(REAL32 tm)
{
	if (m_pOwner->isCharaStateMask(CHARA_STATE_DEATH))
		getUpdateContext()->UpdateDeathTime(tm);

	getMaterialContext()->DriveLightVolume(tm);
	getCameraContext()->OnUpdate(tm);
	getMaterialContext()->ProcessMaterial(tm);
}

void CGameCharaUpdaterFullbody::OnLastUpdate(REAL32 tm)
{
}