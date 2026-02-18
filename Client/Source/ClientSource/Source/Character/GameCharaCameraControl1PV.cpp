#include "pch.h"
#include "GameCharaCameraControl1PV.h"
#include "GlobalVariables.h"

I3_CLASS_INSTANCE( CGameCharaCameraControl1PV);

bool CGameCharaCameraControl1PV::OnChangeKillCamera( INT32 idx)
{
//	if( g_pCameraManager == nullptr )
//		return false;
	if (CGameCameraManager::i() == nullptr)
		return false;

	if( g_pCamera == nullptr )
		return false;

	CGameCameraManager::i()->setTargetCharacter(m_pOwner);
//	g_pCameraManager->setTargetCharacter( m_pOwner);
	CGameCameraManager::i()->ChangeCamera(CAMERA_TYPE_KILL);
//	g_pCameraManager->ChangeCamera( CAMERA_TYPE_KILL);
	m_pOwner->Cmd_ChangeView( CHARA_VIEW_3PV);
	
	// 킬 카메라 시작시 1인칭 캐릭터는 안보이도록 합니다.
	m_pOwner->SetVisible( false );	
	
	if( i3::same_of<CGameCamera_Kill*>(g_pCamera))
	{
		CGameCamera_Kill * pKillCam = (CGameCamera_Kill*)g_pCamera;
		pKillCam->SetEnemyIdx( idx);
	}

	return true;
}



