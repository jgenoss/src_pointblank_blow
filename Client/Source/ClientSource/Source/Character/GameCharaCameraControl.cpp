#include "pch.h"
#include "GameCharaCameraControl.h"
#include "GlobalVariables.h"
#include "GameCharaCameraContext.h"
#include "GameCharaBoneContext.h"
#include "GameCharaWeaponContext.h"

I3_CLASS_INSTANCE( CGameCharaCameraControl);

bool CGameCharaCameraControl::OnChangeFPSCamera( void)
{
	if (CGameCameraManager::i() == nullptr) return false;
	if( g_pCamera == nullptr )				return false;

	CGameCharaBase* pTargetCharacter = CGameCameraManager::i()->getTargetCharacter();
	if( pTargetCharacter != nullptr && pTargetCharacter != m_pOwner)
	{
		//이전에 TargetCharacter를 3PV로 설정.(제압 미션 옵저버 모드 -> 리스폰 할 경우) 
		CHARA_VIEW_TYPE eView_Type = CGameCameraManager::i()->getTargetCharacter()->getViewType();
		if( eView_Type == CHARA_VIEW_1PV) CGameCameraManager::i()->getTargetCharacter()->Cmd_Observer( false);
	}

	//	Default 카메라로
	CGameCameraManager::i()->setTargetCharacter(m_pOwner);
	CGameCameraManager::i()->ChangeCamera(CAMERA_TYPE_FPS);
	
	//I3PRINTF("FPS idx = %d, Nick = %s\n", getCharaInfo()->_NetIdx, BattleSlotContext::i()->getNickForSlot( getCharaInfo()->_NetIdx));

	//	일반 카메라 상태
	getCameraContext()->_Idle_Camera();

	return true;
}

bool CGameCharaCameraControl::OnChangeObserverCamera( void)
{
	return false;
}

bool CGameCharaCameraControl::OnSetObserveZoom(void)
{
	bool	bCheckZoom = false;

	if(m_pOwner->IsDino() == false)
	{
		m_pOwner->Cmd_SyncWeapon();

		WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();

		if( pWeapon == nullptr )
			return false;

		I3ASSERT( pWeapon->getWeaponInfo() != nullptr);

		WEAPON::ZOOM_TYPE type = pWeapon->getWeaponInfo()->GetTypeZoom();
		

		if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
		{
			if( type == WEAPON::ZOOM_ZOOM || type == WEAPON::ZOOM_SCOPE )
			{	
				if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
				{
					if( type == WEAPON::ZOOM_ZOOM)
					{
						getCameraContext()->SetZoom();
						bCheckZoom = true;
					}
					else if( type == WEAPON::ZOOM_SCOPE)
					{	
						getCameraContext()->SetScopeLevel( getBoneContext()->getScopeWaveData()->m_nCurScopeLevel);
						bCheckZoom = true;
					}
				}
			}
		}
	}

	if( !bCheckZoom )
		getCameraContext()->_Idle_Camera();
	else
	{
		//상대방 crosshair에 맞춰 업데이트가 되어야함
		if (m_pOwner->isCharaStateMask(CHARA_STATE_OBSERVER))	
			m_pOwner->Cmd_ApplyCrosshair();
	}

	return bCheckZoom;
}

bool CGameCharaCameraControl::OnSetZoom( void)
{
	return true;
}

bool CGameCharaCameraControl::OnProcessRespawn( void)
{
	if( m_pOwner->isLocal())//!m_pOwner->isNetwork())
	{
		OnChangeFPSCamera();
		// 1인칭 모드에 대한 뷰 처리
		//m_pOwner->ChangeFPSView( false);
		m_pOwner->Cmd_ChangeView( CHARA_VIEW_1PV, false);
	}
	else
	{
		m_pOwner->Cmd_ChangeView( CHARA_VIEW_3PV, false);
		//m_pOwner->ChangeObserverView( false);
	}

	return true;
}


