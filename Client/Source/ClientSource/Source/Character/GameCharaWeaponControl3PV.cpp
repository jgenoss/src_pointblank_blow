#include "pch.h"
#include "GameCharaWeaponControl3PV.h"
#include "GlobalVariables.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaWeaponControl3PV);

bool CGameCharaWeaponControl3PV::OnChangeViewWeapon( void)
{
	for( UINT32 i = 0; i < WEAPON_SLOT_COUNT; i++)
	{
		WEAPON_SLOT_TYPE slot = static_cast<WEAPON_SLOT_TYPE>(i);
		if( getWeapon( slot) != nullptr )
		{
			getWeapon( slot)->OnChangeObserverView();
			if( getWeapon( slot) != getCurrentWeapon() && slot < WEAPON_SLOT_MELEE)
			{	// 현재 들고 있는 무기와 다른 경우 해당 부착위치에 붙인다.
				OnAttachPurchasePrimWeapon( slot);
				OnAttachPurchaseSideWeapon( slot);
			}
		}
	}

	return true;
}

/*virtual*/ bool CGameCharaWeaponControl3PV::OnSelectWeapon( WEAPON_SLOT_TYPE nSlot, bool bPlaySound /*= true*/)
{
	if( CGameCharaWeaponControl::OnSelectWeapon( nSlot, bPlaySound))
	{
		if( g_pFramework->IsBattleStage())
		{	// InGame에서만 등에 붙입니다.
			// 이전에 들었던 무기를 해당 위치에 붙인다.
			OnAttachPurchasePrimWeapon( (WEAPON_SLOT_TYPE) getOldSlotNumber());
			OnAttachPurchaseSideWeapon( (WEAPON_SLOT_TYPE) getOldSlotNumber());
		}
		return true;
	}

	return false;
}


bool CGameCharaWeaponControl3PV::OnZoom( void)
{
	if( getCurrentWeapon() == nullptr )
		return false;

	CWeaponInfo * pWeaponInfo = getCurrentWeapon()->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr );

	WEAPON::ZOOM_TYPE type = pWeaponInfo->GetTypeZoom();

	//	견착 기능이 있는 무기
	switch( type)
	{
		case  WEAPON::ZOOM_ZOOM:
			if( ! m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
			{
				m_pOwner->Cmd_ApplyCrosshair();
				return true;
			}
			break;

		case WEAPON::ZOOM_SCOPE :
			//	스코프 기능이 있는 무기
			if( !m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
			{
				INT32 nScopeLevel = getBoneContext()->getScopeWaveData()->m_nCurScopeLevel + 1;
				getBoneContext()->getScopeWaveData()->m_nCurScopeLevel = nScopeLevel;
				m_pOwner->Cmd_ApplyCrosshair();
				
				return true;
			}
			break;

		case WEAPON::ZOOM_STEADY_AIM :
			// 다른 어떤 Animation 및 Crosshair의 변경없이 FOV만을 조절
			if( ! m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
			{
				m_pOwner->Cmd_ApplyCrosshair();
				return true;
			}
			break;
	}

	m_pOwner->Cmd_ApplyCrosshair();
	return false;
}


bool CGameCharaWeaponControl3PV::OnProcessRespawn( void)
{
	if( !CGameCharaWeaponControl::OnProcessRespawn())
		return false;
	
	
	OnAttachPurchasePrimWeapon( WEAPON_SLOT_PRIMARY);
	OnAttachPurchaseSideWeapon( WEAPON_SLOT_SECONDARY);

	if( !m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_PRIMARY))
	{
		// 주무기 제한시 보조무기로 변경
		m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_SECONDARY);
	}

	return true;
}



