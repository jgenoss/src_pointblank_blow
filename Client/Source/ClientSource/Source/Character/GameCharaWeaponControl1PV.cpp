#include "pch.h"
#include "GameCharaWeaponControl1PV.h"
#include "GlobalVariables.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaWeaponControl1PV);

bool CGameCharaWeaponControl1PV::OnCreateWeapon( WEAPON_SLOT_TYPE nSlot, T_ItemID Itemid, UINT8 nMultiWeaponSync)
{
	bool bRv = CGameCharaWeaponControl::OnCreateWeapon( nSlot, Itemid, nMultiWeaponSync);

	// 1인칭용은 바운드 박스 제거
	WeaponBase* pWeapon = nullptr;

	pWeapon = getWeapon( nSlot);
	if (pWeapon == nullptr) return false;

	pWeapon->OnChangeFPSView();

	return bRv;
}

WeaponBase * CGameCharaWeaponControl1PV::OnAttachWeapon( WEAPON_SLOT_TYPE nSlot, i3Node * pSceneNode, i3GameNode * pGameNode)
{
	WeaponBase * pWeapon = CGameCharaWeaponControl::OnAttachWeapon( nSlot, pSceneNode, pGameNode);

	// 1인칭용 무기는 Shadow가 나오지 않도록 설정되어 있다.
	if( pWeapon != nullptr)
		pWeapon->getSceneObject()->GetBody()->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);

	if (pWeapon)
	{
		CGameCharaBase* pOwner = pWeapon->getOwner();
		if (pOwner)
			pWeapon->EnableRenderAABB(pOwner->IsEnabledRenderAABB());
		else
			pWeapon->EnableRenderAABB(true);
	}

	return pWeapon;
}

WeaponBase * CGameCharaWeaponControl1PV::OnDetachWeapon( WEAPON_SLOT_TYPE nSlot)
{
	WeaponBase * pWeapon = CGameCharaWeaponControl::OnDetachWeapon( nSlot);

	if( pWeapon != nullptr )
	{
		// 1인칭용 무기는 Shadow가 나오지 않도록 설정되어 있다.
		pWeapon->getSceneObject()->GetBody()->RemoveFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);
	}

	if (pWeapon)
	{
		pWeapon->EnableRenderAABB(false);
	}
	
	return pWeapon;
}

bool CGameCharaWeaponControl1PV::OnChangeViewWeapon( void)
{
	CGameCharaWeaponControl::OnChangeViewWeapon();
	// Add code here

	for( UINT32 i = 0; i < WEAPON_SLOT_COUNT; i++)
	{
		WeaponBase * pWeapon = getWeapon( static_cast<WEAPON_SLOT_TYPE>(i) );
		if( pWeapon != nullptr )
		{
			pWeapon->OnChangeFPSView();
			if( pWeapon != getCurrentWeapon() )
			{	// ageia 현재 들고 있는 무기와 다르다면 모두 월드 밖으로 옮깁니다.
				_NoUseWeapon( pWeapon);
			}
		}
	}

	return true;
}

/*virtual*/ bool CGameCharaWeaponControl1PV::OnAttachPurchasePrimWeapon( WEAPON_SLOT_TYPE nSlot)
{
	if( CGameCharaWeaponControl::OnAttachPurchasePrimWeapon( nSlot))
	{
		WeaponBase * pWeapon = getWeapon( nSlot);
		if( pWeapon != nullptr)
			pWeapon->SetEnable( false);
		return true;
	}

	return false;
}

/*virtual*/ bool CGameCharaWeaponControl1PV::OnAttachPurchaseSideWeapon( WEAPON_SLOT_TYPE nSlot)
{	
	if( CGameCharaWeaponControl::OnAttachPurchaseSideWeapon( nSlot))
	{
		WeaponBase * pWeapon = getWeapon( nSlot);
		if( pWeapon != nullptr)
			pWeapon->SetEnable( false);
		return true;
	}

	return false;
}

/*virtual*/ bool CGameCharaWeaponControl1PV::OnSelectWeapon( WEAPON_SLOT_TYPE nSlot, bool bPlaySound /*= true*/)
{
	if( CGameCharaWeaponControl::OnSelectWeapon( nSlot, bPlaySound))
	{
		// Animation과 World-Space Transform 사이에 LOD 계산이 달라질 수 있으며,
		// 그것은 Visual 상에 여러 문제가 될 수 있기 때문에
		// 수동으로 LOD를 계산시킨다.
		getCurrentWeapon()->setCurrentLOD( 0);
		return true;
	}

	return false;
}

bool CGameCharaWeaponControl1PV::OnZoom( void)
{
	if( getCurrentWeapon() == nullptr )
		return false;

	CWeaponInfo * pWeaponInfo = getCurrentWeapon()->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr );

	m_pOwner->Cmd_ApplyCrosshair();

	WEAPON::ZOOM_TYPE type = pWeaponInfo->GetTypeZoom();

	//	견착 기능이 있는 무기
	switch( type)
	{
		case  WEAPON::ZOOM_ZOOM:
			if( ! m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
			{
				//	견착 카메라 상태
				getCameraContext()->SetZoom();
				return true;
			}
			break;

		case WEAPON::ZOOM_SCOPE :
			//	스코프 기능이 있는 무기
			{
				INT32 nLvl = getBoneContext()->getScopeWaveData()->m_nCurScopeLevel + 1;

				//	스나이퍼 카메라 상태	
				if( getCameraContext()->SetScopeLevel( nLvl))
				{
					return true;
				}
			}
			break;

		case WEAPON::ZOOM_STEADY_AIM :
			// 다른 어떤 Animation 및 Crosshair의 변경없이 FOV만을 조절
			if( ! m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
			{
				//	견착 카메라 상태
				getCameraContext()->SetZoom();
				return true;
			}
			break;
		default :
			return false;
	}
	
	return false;
}

bool CGameCharaWeaponControl1PV::OnExtActivate( void)
{
	m_pOwner->Cmd_ApplyCrosshair();

	return CGameCharaWeaponControl::OnExtActivate();
}

bool CGameCharaWeaponControl1PV::OnExtDeactivate( void)
{
	m_pOwner->Cmd_ApplyCrosshair();

	return CGameCharaWeaponControl::OnExtDeactivate();
}

bool CGameCharaWeaponControl1PV::OnRemoveShell( void)
{
	if( getCurrentWeapon() == nullptr )							return false;
	if( getCurrentWeapon()->GetCurrentExtensionType() != WEAPON::EXT_GLAUNCHER)
		return false;

	WeaponGrenadeLauncher * pGLauncher = (WeaponGrenadeLauncher*) getCurrentWeapon();

	if( !m_pOwner->isNetwork())
	{
		if( !pGLauncher->isLoadBulletEnabled())
			return false;
	}

	pGLauncher->SetEnable( true);
	pGLauncher->RemoveShell();
	
	if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
		m_pOwner->Cmd_Idle_Camera();

	return true;
}


bool CGameCharaWeaponControl1PV::OnProcessRespawn( void)
{	
	if( !CGameCharaWeaponControl::OnProcessRespawn())
		return false;
	
	if( !m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_PRIMARY))
	{
		// 주무기 제한시 보조무기로 변경
		m_pOwner->Cmd_ChangeWeapon( WEAPON_SLOT_SECONDARY);
	}

	return true;
}


