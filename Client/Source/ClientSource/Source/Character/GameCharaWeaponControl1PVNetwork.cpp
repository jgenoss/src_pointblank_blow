#include "pch.h"
#include "GameCharaWeaponControl1PVNetwork.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "BattleHud.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaWeaponControl1PVNetwork, CGameCharaWeaponControl1PV);

CGameCharaWeaponControl1PVNetwork::CGameCharaWeaponControl1PVNetwork()
{
}

CGameCharaWeaponControl1PVNetwork::~CGameCharaWeaponControl1PVNetwork()
{
}

//void CGameCharaWeaponControl1PVNetwork::OnUpdate( REAL32 rDeltatime)
//{
//	CGameCharaWeaponControl1PV::OnUpdate( rDeltatime);
//
//	//	무기 변경때 한프레임 랜더링 버그 막기위해...이전 프레임에 바뀐(바뀌기로 예약된) 무기를 지금 변경한다.
//	if( getSwapSlotNumber() != getCurrentSlotNumber())
//	{
//		if( getCurrentWeapon() != NULL)
//		{
//			// 이전 무기는 사용하지 않는 무기
//			getWeaponContext()->NoUseWeapon( getCurrentWeapon());
//		}
//		
//		//	애니메이션 하기 전에 먼저 무기 교체(모델링)을 해야 한다.		
//		OnSelectWeapon( getSwapSlotNumber());
//
//		if( (getCurrentWeapon() != NULL) && g_pFramework->IsBattleStage())
//		{
//			getCurrentWeapon()->NET_WeaponChange( CHARA_ANIMSET_1PV, true);		//	무기 교체시 이후 무기는 Change 상태로
//			// HUD
//			{
//				CStageBattle * pStage = (CStageBattle*) g_pFramework->GetCurrentStage();
//				CHud * pHud = pStage->getHud();
//				if( pHud != NULL )	pHud->setOldLoadedBullet( 0 );
//			}
//		}
//	}
//
//	if( getCurrentWeapon() != NULL)
//	{
//		//	왼손 장전이 끝난 후에도 무기가 왼쪽에 붙어 있는것을 막는다.
//		if( getWeaponContext()->getAttachHand() == CHARA_WEAPON_BARREL_LEFT_HAND)
//		{
//			// 오른손 그립 무기
//			//	왼손 장전이 끝난 후에도 무기가 왼쪽에 붙어 있는것을 막는다.
//			if( !getActionContext()->isUpper( CHARA_UPPER_LOADMAGAZINE) && 
//				!getActionContext()->isUpper( CHARA_UPPER_LOADBULLET))
//			{	
//				//	오른손 그립이면 다시 오른손에 붙이도록 되돌린다.
//				m_pOwner->ChangeWeaponAttachHand( CHARA_WEAPON_GRIP_RIGHT_HAND);
//				getCurrentWeapon()->ChangeGunDummy( WEAPON_ATTACH_GRIP);
//			}
//		}
//		else if( getWeaponContext()->getAttachHand() == CHARA_WEAPON_BARREL_RIGHT_HAND)
//		{
//			// 왼손 그립 무기
//			// 장전이 끝나고 무기가 오른쪽에 붙어 있는 것을 막는다.
//			if( !getActionContext()->isUpper( CHARA_UPPER_LOADMAGAZINE) &&
//				!getActionContext()->isUpper( CHARA_UPPER_LOADBULLET))
//			{
//				//
//				m_pOwner->ChangeWeaponAttachHand( CHARA_WEAPON_GRIP_LEFT_HAND);
//				getCurrentWeapon()->ChangeGunDummy( WEAPON_ATTACH_GRIP);
//			}
//		}
//
//		// Animation과 World-Space Transform 사이에 LOD 계산이 달라질 수 있으며,
//		// 그것은 Visual 상에 여러 문제가 될 수 있기 때문에
//		// 수동으로 LOD를 계산시킨다.
//		getCurrentWeapon()->setCurrentLOD( 0);
//	}
//}

//bool CGameCharaWeaponControl1PVNetwork::OnSwapWeapon( INT32 nSlot)
//{
//	if( !CGameCharaWeaponControl1PV::OnSwapWeapon( nSlot) )
//		return false;
//
//	//	Weapon Change
//	if( getCurrentWeapon() != NULL)
//	{	
//		//	캐릭터 애니메이션을 변경하면 다음 프레임부터 적용되기 때문에 이번 프레임에 무기를 바꾸지 않고 다름 프레임에 
//		//	무기를 바꾸도록 한다.
//		setOldSlotNumber( getCurrentSlotNumber());
//		setSwapSlotNumber( nSlot);		//	다음 프레임에 바뀔 무기 번호 
//	}
//	else
//	{	//	현재 무기가 NULL이면 당장 바꾼다.		
//		//	애니메이션 하기 전에 먼저 무기 교체(모델링)을 해야 한다.
//		if( !OnSelectWeapon( nSlot))	return false;
//		if( getCurrentWeapon() == NULL )	return false;
//		if( g_pGameContext->IsP2PActivate() )
//			getCurrentWeapon()->NET_WeaponChange( CHARA_ANIMSET_1PV, true);	//	무기 교체시 이후 무기는 Change 상태로
//	}
//
//	//	무기는 기본 오른손에 부착	
//	ChangeAttachHand( CHARA_WEAPON_GRIP_RIGHT_HAND);
//
//	return true;
//}
//
//bool CGameCharaWeaponControl1PVNetwork::OnFire( VEC3D * pStartPos, VEC3D * pTargetPos)
//{
//	return _ProcessFireForNetwork( pStartPos, pTargetPos);
//}
//
//bool CGameCharaWeaponControl1PVNetwork::OnSecFire( VEC3D * pStartPos, VEC3D * pTargetPos)
//{
//	return _ProcessFireForNetwork( pStartPos, pTargetPos, WEAPON_SECONDARY_FIRE);
//}

//bool CGameCharaWeaponControl1PVNetwork::OnExtActivate( void)
//{
//	CGameCharaWeaponControl1PV::OnExtActivate();
//
//	if( getCurrentWeapon() == NULL )				return false;
//	if( !getCurrentWeapon()->IsExtensionWeapon())	return false;
//
//	//	무기 Extension 활성화 / 비활성화 중에는 무기는 발사 못한다.	
//	getCurrentWeapon()->NET_ExtensionActivate( true);
//
//	return true;
//}

//bool CGameCharaWeaponControl1PVNetwork::OnExtDeactivate( void)
//{
//	CGameCharaWeaponControl1PV::OnExtDeactivate();
//
//	if( getCurrentWeapon() == NULL )				return false;
//	if( !getCurrentWeapon()->IsExtensionWeapon())	return false;
//
//	//	무기 Extension 활성화 / 비활성화 중에는 무기는 발사 못한다.	
//	getCurrentWeapon()->NET_ExtensionActivate( false);
//
//	return true;
//}
//
//bool CGameCharaWeaponControl1PVNetwork::OnLoadMagazine( void)
//{
//	if( getCurrentWeapon() == NULL )		return false;
//	
//	I3ASSERT( getCurrentWeapon()->getWeaponInfo() != NULL);
//	//	총기 계열만 가능
//	if( getCurrentWeapon()->getWeaponInfo()->isGun() == false)	return false;
//
//	//	KEYWORD : WEAPON_SHOTGUN_LAUNCHER
//	if( getCurrentWeapon()->GetCurrentExtensionType() == WEAPON_EXT_SHOTGUN_LAUNCHER && getCurrentWeapon()->isExtensionActivate() )
//	{		
//		WeaponShotGunLauncher * pLauncher = (WeaponShotGunLauncher*) getCurrentWeapon();
//		if( getActionContext()->isUpper( CHARA_UPPER_SWAPWEAPON))
//			return false;
//
//		if( pLauncher->getTotalShellCount() <= 0)
//		{	
//			return false;
//		}
//		
//		pLauncher->LoadShell();
//	}
//	else
//	{
//		// KEYWORD : WEAPON_DUMMY
//		if( getCurrentWeapon()->IsLoadMagazineLeftBarrel() )
//		{
//			m_pOwner->ChangeWeaponAttachHand( CHARA_WEAPON_BARREL_LEFT_HAND);
//			getCurrentWeapon()->ChangeGunDummy( WEAPON_ATTACH_BARREL);
//		}
//
//		getCurrentWeapon()->SetEnable( true);
//		getCurrentWeapon()->NET_LoadMagazine(CHARA_ANIMSET_1PV);
//	}
//
//	if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
//		m_pOwner->Cmd_Idle_Camera();
//
//	return true;
//}

//bool CGameCharaWeaponControl1PVNetwork::OnLoadMagazineDual( void)
//{
//	CGameCharaWeaponControl1PV::OnLoadMagazineDual();
//
//	if( getCurrentWeapon() == NULL )		return false;
//	I3ASSERT( getCurrentWeapon()->getWeaponInfo() != NULL);
//	if( getCurrentWeapon()->getWeaponInfo()->isGun() == false)	return false;
//	if( getCurrentWeapon()->GetCurrentExtensionType() != WEAPON_EXT_DUAL_MAG )	return false;
//
//	getCurrentWeapon()->SetEnable( true);
//	getCurrentWeapon()->ExtensionActivate( true);	//	무기 Extension 동작 
//
//	return true;
//}

//bool CGameCharaWeaponControl1PVNetwork::OnLoadBullet( void)
//{
//	CGameCharaWeaponControl1PV::OnLoadBullet();
//
//	if( getCurrentWeapon() == NULL )	return false;
//
//	//	총기 계열만 가능
//	if( getCurrentWeapon()->getWeaponInfo()->isGun() == false)	return false;
//
//	// KEYWORD : WEAPON_DUMMY
//	if( getCurrentWeapon()->IsLoadBulletLeftBarrel() )
//	{	// MG는 탄약 장전시에만 왼손 파지
//		m_pOwner->ChangeWeaponAttachHand( CHARA_WEAPON_BARREL_LEFT_HAND);
//		getCurrentWeapon()->ChangeGunDummy( WEAPON_ATTACH_BARREL);
//	}
//
//	getCurrentWeapon()->SetEnable( true);
//
//	// 2006. 9. 23 by 정순구
//	// desc : 네트워크 부분 추가
//	getCurrentWeapon()->NET_LoadBullet( CHARA_ANIMSET_1PV);
//
//	if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
//		m_pOwner->Cmd_Idle_Camera();
//	
//	return true;
//}

//bool CGameCharaWeaponControl1PVNetwork::OnLoadMagazine_Ready( void)
//{	
//	bool bRv = CGameCharaWeaponControl::OnLoadMagazine_Ready();
//
//	if( bRv)
//	{
//		bRv = _ActionWeapon_LoadMagazine_Ready_ForNetwork();
//	}
//
//	return bRv;
//}

