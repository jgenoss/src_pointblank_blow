#include "pch.h"
#include "GameCharaWeaponControl3PVLocal.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "BattleHud.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaWeaponControl3PVLocal, CGameCharaWeaponControl3PV);

CGameCharaWeaponControl3PVLocal::CGameCharaWeaponControl3PVLocal()
{
}

CGameCharaWeaponControl3PVLocal::~CGameCharaWeaponControl3PVLocal()
{
}

//void CGameCharaWeaponControl3PVLocal::OnUpdate( REAL32 rDeltatime)
//{
//	CGameCharaWeaponControl3PV::OnUpdate( rDeltatime);
//
//	//	무기 변경때 한프레임 랜더링 버그 막기위해...이전 프레임에 바뀐(바뀌기로 예약된) 무기를 지금 변경한다.
//	if( getSwapSlotNumber() != getCurrentSlotNumber())
//	{
//		if( getCurrentWeapon() != NULL)
//		{
//			// 이전 무기는 사용하지 않는 상태로 합니다.
//			getWeaponContext()->NoUseWeapon( getCurrentWeapon());
//		}
//		
//		//	애니메이션 하기 전에 먼저 무기 교체(모델링)을 해야 한다.		
//		OnSelectWeapon( getSwapSlotNumber());
//
//		if( (getCurrentWeapon() != NULL) && g_pFramework->IsBattleStage())
//		{
//			getCurrentWeapon()->WeaponChange();		//	무기 교체시 이후 무기는 Change 상태로
//
//			// HUD
//			{
//				CStageBattle * pStage = (CStageBattle*) g_pFramework->GetCurrentStage();
//				CHud * pHud = pStage->getHud();
//				if( pHud != NULL )	pHud->setOldLoadedBullet( 0 );
//			}
//		}
//		
//		// 이전에 들었던 무기를 해당 위치에 붙인다.
//		OnAttachPurchasePrimWeapon( getOldSlotNumber());
//		OnAttachPurchaseSideWeapon( getOldSlotNumber());
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
//	}
//}

//bool CGameCharaWeaponControl3PVLocal::OnSwapWeapon( INT32 nSlot)
//{
//	if( !CGameCharaWeaponControl3PV::OnSwapWeapon( nSlot) )
//		return false;
//	
//	//	Weapon Change
//	WeaponBase * pCurWeapon = getCurrentWeapon();
//
//	if( pCurWeapon != NULL)
//	{	
//		//	캐릭터 애니메이션을 변경하면 다음 프레임부터 적용되기 때문에 이번 프레임에 무기를 바꾸지 않고 다름 프레임에 
//		//	무기를 바꾸도록 한다.
//		setOldSlotNumber( getCurrentSlotNumber());
//		setSwapSlotNumber( nSlot);				//	다음 프레임에 바뀔 무기 번호 
//	}
//	else
//	{	//	현재 무기가 NULL이면 당장 바꾼다.		
//		//	애니메이션 하기 전에 먼저 무기 교체(모델링)을 해야 한다.
//		if( !OnSelectWeapon( nSlot))
//			return false;
//
//		WeaponBase * pChangeWeapon = getCurrentWeapon();
//		if( pChangeWeapon == NULL )
//			return false;
//
//		pChangeWeapon->WeaponChange();		//	무기 교체시 이후 무기는 Change 상태로
//	}
//
//	//	무기는 기본 오른손에 부착	
//	ChangeAttachHand( CHARA_WEAPON_GRIP_RIGHT_HAND);
//	return true;
//}

//bool CGameCharaWeaponControl3PVLocal::OnDropedWeaponSearching( REAL32 rDeltatime)
//{
//	return _ProcessDropedWeaponSearsing();
//}

//bool CGameCharaWeaponControl3PVLocal::OnFire( VEC3D * pStartPos, VEC3D * pTargetPos)
//{
//	return _ProcessFireForLocal();
//}
//
//bool CGameCharaWeaponControl3PVLocal::OnSecFire( VEC3D * pStartPos, VEC3D * pTargetPos)
//{
//	CGameCharaWeaponControl3PV::OnSecFire( pStartPos, pTargetPos);
//
//	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
//	if( pWeapon == NULL )
//		return false;
//
//	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
//	I3ASSERT( pWeaponInfo != NULL);
//
//	//	보조 공격
//	switch( pWeaponInfo->GetTypeClass())
//	{			
//	case WEAPON_CLASS_KNIFE:
//		{
//			if( pWeapon->IsExactTypeOf( WeaponKnife::GetClassMeta()))
//			{
//				WeaponKnife * pWeaponKnife = (WeaponKnife *) pWeapon;
//				pWeaponKnife->SetAttackType( WEAPON_KNIFE_SECONDARY_ATTACK );
//			}
//			else if( pWeapon->IsExactTypeOf( WeaponDualKnife::GetClassMeta()))
//			{
//				WeaponDualKnife * pWeaponKnife = (WeaponDualKnife *) pWeapon;
//				pWeaponKnife->SetAttackType( WEAPON_KNIFE_SECONDARY_ATTACK );
//			}
//		}
//		break;
//	}
//
//	if( !pWeapon->Fire( WEAPON_SECONDARY_FIRE) )
//		return false;
//	m_pOwner->ApplyCrosshair();
//
//	// 편차를 계산
//	getBoneContext()->_ProcessFire();
//
//	return true;
//}

//bool CGameCharaWeaponControl3PVLocal::OnExtActivate( void)
//{
//	CGameCharaWeaponControl3PV::OnExtActivate();
//
//	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
//	if( pWeapon == NULL )
//		return false;
//
//	if( !pWeapon->IsExtensionWeapon())		return false;
//
//	if( pWeapon->GetCurrentExtensionType() == WEAPON_EXT_DUAL_MAG)
//	{// 듀얼 탄창을 교체할 수 있는가?
//		if( pWeapon->getTotalBulletCount() <= 0 && pWeapon->getLoadedBulletDualCount() <= 0)
//			return false;
//	}
//
//	//	무기 Extension 활성화 / 비활성화 중에는 무기는 발사 못한다.	
//	if( pWeapon->NET_ExtensionActivate( true, false))
//	{
//		pWeapon->addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
//
//		return true;
//	}
//
//	return false;
//}

//bool CGameCharaWeaponControl3PVLocal::OnExtDeactivate( void)
//{
//	CGameCharaWeaponControl3PV::OnExtDeactivate();
//
//	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
//	if( pWeapon == NULL )
//		return false;
//
//	if( !pWeapon->IsExtensionWeapon())		return false;
//
//	if( pWeapon->GetCurrentExtensionType() == WEAPON_EXT_DUAL_MAG)
//	{// 듀얼 탄창을 교체할 수 있는가?
//		if( pWeapon->getTotalBulletCount() <= 0 && pWeapon->getLoadedBulletDualCount() <= 0)
//			return false;
//	}
//
//	//	무기 Extension 활성화 / 비활성화 중에는 무기는 발사 못한다.	
//	if( pWeapon->NET_ExtensionActivate( false, false))
//	{
//		pWeapon->addWeaponStateMask( WSM_FIRE_DISABLE | WSM_LOADMAG_DISABLE | WSM_LOADBULLET_DISABLE);
//		return true;
//	}
//
//	return false;
//}

//bool CGameCharaWeaponControl3PVLocal::OnLoadMagazine( void)
//{
//	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
//	if( pWeapon == NULL )
//		return false;
//
//	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
//	I3ASSERT( pWeaponInfo != NULL);
//
//	//	총기 계열만 가능
//	if(pWeaponInfo->isGun() == false)
//		return false;
//	
//	//	KEYWORD : WEAPON_SHOTGUN_LAUNCHER	
//	if( pWeapon->GetCurrentExtensionType() == WEAPON_EXT_SHOTGUN_LAUNCHER && pWeapon->isExtensionActivate() )
//	{
//		WeaponShotGunLauncher * pLauncher = (WeaponShotGunLauncher*) pWeapon;
//		if( getActionContext()->isUpper( CHARA_UPPER_SWAPWEAPON))	// 무기 스왑중에는 리로드 금지
//			return false;
//
//		// KEYWORD : WEAPON_DUMMY
//		if( pWeapon->IsLoadMagazineLeftBarrel() )
//		{
//			m_pOwner->ChangeWeaponAttachHand( CHARA_WEAPON_BARREL_LEFT_HAND);
//			pWeapon->ChangeGunDummy( WEAPON_ATTACH_BARREL);
//		}
//
//		if( pLauncher->getTotalShellCount() <= 0)
//		{
//			return false;
//		}
//		
//		//	LSS 장전
//		pLauncher->LoadShell();
//	}
//	else
//	{
//		//	소유 총알이 부족할 경우 장전을 할 수 없다.
//		if( pWeapon->getTotalBulletCount() <= 0)	
//			return false;
//
//		if( (pWeapon->getMagazineType() == WEAPON_DUAL_MAGAZINE_NONE) &&
//			pWeapon->getLoadedBulletCount() == pWeapon->GetActualLoadBullet() )
//			return false;
//
//		if( getActionContext()->isUpper( CHARA_UPPER_SWAPWEAPON ))	// 무기 스왑중에는 리로드 금지
//			return false;
//
//		// KEYWORD : WEAPON_DUMMY
//		if( pWeapon->IsLoadMagazineLeftBarrel() )
//		{
//			m_pOwner->ChangeWeaponAttachHand( CHARA_WEAPON_BARREL_LEFT_HAND);
//			pWeapon->ChangeGunDummy( WEAPON_ATTACH_BARREL);
//		}
//
//		pWeapon->SetEnable( true); //SetVisible( true);
//		pWeapon->LoadMagazine(CHARA_ANIMSET_3PV);
//	}
//
//	if( pWeaponInfo->GetTypeTrigger() != WEAPON_TRIGGER_RELOADING_SHOT )
//		m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);
//
//	if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
//		m_pOwner->Cmd_Idle_Camera();
//
//	return true;
//}

//bool CGameCharaWeaponControl3PVLocal::OnLoadMagazineDual( void)
//{
//	CGameCharaWeaponControl3PV::OnLoadMagazineDual();
//
//	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
//	if( pWeapon == NULL )
//		return false;
//	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
//	I3ASSERT( pWeaponInfo != NULL);
//
//	if(pWeaponInfo->isGun() == false)
//		return false;
//
//	if( pWeapon->GetCurrentExtensionType() != WEAPON_EXT_DUAL_MAG )
//		return false;
//
//	//	소유 총알이 부족할 경우 장전을 할 수 없다.
//	if( pWeapon->getTotalBulletCount() <= 0)	
//		return false;
//
//	pWeapon->SetEnable( true); //SetVisible( true);
//
//	//	무기 Extension 동작 
//	pWeapon->ExtensionActivate( true);
//	g_pFramework->getChatBox()->PutSystemChatting( "Extension On");
//
//	if( pWeaponInfo->GetTypeTrigger() != WEAPON_TRIGGER_RELOADING_SHOT )
//		m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);	//	발사 준비중에 발사하지 못하도록. 애니메이션이 끝나면 발사 가능
//
//	return true;
//}

//bool CGameCharaWeaponControl3PVLocal::OnLoadBullet( void)
//{
//	CGameCharaWeaponControl3PV::OnLoadBullet();
//
//	if( getCurrentWeapon() == NULL )	return false;
//
//	//	총기 계열만 가능
//	if( getCurrentWeapon()->getWeaponInfo()->isGun() == false)	return false;
//
//	if( !getCurrentWeapon()->isLoadBulletEnabled())	return false;
//
//	// KEYWORD : WEAPON_DUMMY
//	if( getCurrentWeapon()->IsLoadBulletLeftBarrel() )
//	{	// MG는 탄약 장전시에만 왼손 파지
//		m_pOwner->ChangeWeaponAttachHand( CHARA_WEAPON_BARREL_LEFT_HAND);
//		getCurrentWeapon()->ChangeGunDummy( WEAPON_ATTACH_BARREL);
//	}
//
//	getCurrentWeapon()->SetEnable( true);
//	getCurrentWeapon()->LoadBullet();
//
//	if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
//		m_pOwner->Cmd_Idle_Camera();
//
//	return true;
//}

//bool CGameCharaWeaponControl3PVLocal::OnLoadMagazine_Ready( void)
//{
//	bool bRv = CGameCharaWeaponControl::OnLoadMagazine_Ready();
//
//	if( bRv)
//	{
//		bRv = _ActionWeapon_LoadMagazine_Ready_ForLocal();
//	}
//
//	return bRv;
//}



