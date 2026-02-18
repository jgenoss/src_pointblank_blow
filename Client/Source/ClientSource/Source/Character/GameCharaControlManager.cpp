#include "pch.h"
#include "GameCharaControlManager.h"
#include "GlobalVariables.h"

I3_CLASS_INSTANCE( CGameCharaControlManager, i3ElementBase);

CGameCharaControlManager::CGameCharaControlManager()
{
	m_n1PV = 0;
	m_nLocal = 0;

	m_pOwner = NULL;
	
	
}

CGameCharaControlManager::~CGameCharaControlManager()
{
	OnDestroy();
}

void CGameCharaControlManager::OnCreate( CGameCharaBase * pChara, INT32 defaultView, INT32 defaultNetwork )
{
	m_pOwner = pChara;

	
<<<<<<< .mine
	
=======
	// CameraControl
	m_pCurrentCameraControl = NULL;
	m_p1PVCameraControl = CGameCharaCameraControl1PV::NewObject();
	m_p3PVCameraControl = CGameCharaCameraControl3PV::NewObject();

	// Commander
	m_pCommander = CGameCharaCommander::NewObject();
	m_pCommander->BindOwner( (CGameCharaPlayer*) pChara);

	// EquipControl
	m_pEquipControl = CGameCharaEquipControl::NewObject();

	// InputControl
	m_pCurrentInputControl = NULL;
	m_pLocalInputControl = CGameCharaInputControlLocal::NewObject();
	m_pLocalInputControl->BindOwner( (CGameCharaPlayer*)pChara);
	m_pNetworkInputControl = CGameCharaInputControlNetwork::NewObject();
	m_pNetworkInputControl->BindOwner( (CGameCharaPlayer*)pChara);

	// MoveControl
	m_pCurrentMoveControl = NULL;
	m_pLocalMoveControl = CGameCharaMoveControlLocal::NewObject();
	m_pLocalMoveControl->BindOwner( (CGameCharaPlayer*)pChara);
	m_pNetworkMoveControl = CGameCharaMoveControlNetwork::NewObject();
	m_pNetworkMoveControl->BindOwner( (CGameCharaPlayer*)pChara);

	// SoundControl
	m_pCurrentSoundControl = NULL;
	m_p1PVSoundControl = CGameCharaSoundControl1PV::NewObject();
	m_p3PVSoundControl = CGameCharaSoundControl3PV::NewObject();

>>>>>>> .r10809
	m_nLocal = !defaultNetwork;
	m_n1PV = !defaultView;

	// 
	if( defaultView == 0)
		OnChange1PV();
	else
		OnChange3PV();

	if( defaultNetwork == 0 )
		OnChangeLocal();
	else
		OnChangeNetwork();
}

void CGameCharaControlManager::OnDestroy()
{	
	
}


void CGameCharaControlManager::OnChange1PV()
{	
	m_n1PV = 1;
}

void CGameCharaControlManager::OnChange3PV()
{
	m_n1PV = 0;
}

void CGameCharaControlManager::OnChangeLocal()
{	
}

void CGameCharaControlManager::OnChangeNetwork()
{
	
}

void CGameCharaControlManager::OnUpdate( REAL32 rDeltatime)
{
	
}
