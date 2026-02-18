#include "pch.h"
#include "GameCharaHUDControl.h"
#include "GlobalVariables.h"
#include "GameCharaHUDContext.h"
#include "Camera/GameCamera_OnObject.h"
#include "UI/UIBattleFrame.h"
#include "UI/UIBattlePopupBase.h"
#include "CrosshairMgr.h"
#include "BattleHud.h"
#include "UserInfoContext.h"
#include "UI/UIUtil.h"
#include "MyRoomInfoContext.h"

I3_CLASS_INSTANCE( CGameCharaHUDControl);

namespace
{
	bool Is1PVObserver(const CGameCharaBase* pOwner)
	{
		if (!pOwner) return false;

		if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT))
			return false;

		return pOwner->isCharaStateMask(CHARA_STATE_OBSERVER) && pOwner->is1PV() ? true : false;
	}
}



/*virtual*/ void CGameCharaHUDControl::OnChangeWeapon( void)
{
}

/*virtual*/ void CGameCharaHUDControl::OnDamage( VEC3D * pDir /*= nullptr*/, REAL32 rHitTheta /*= 0.f*/, bool bDirectionView /*= false*/, INT32 filter_index /*=-1*/ )
{
	if( CHud::i() == nullptr)
		return;

	if( bDirectionView && pDir != nullptr)
	{
		REAL32 Rotation = atan2f( i3Vector::GetX( pDir ), i3Vector::GetZ( pDir ) ); // XZ 평면
		REAL32 rTargetAngle = i3Math::NormalizeAngle( Rotation );
		REAL32 fAngleGap = i3Math::NormalizeAngle( rTargetAngle - rHitTheta );
	
		CHud::i()->SetDamageDirection( fAngleGap);
	}

	// 낙하 데미지 이펙트 미출력 현상 수정
	//if( m_pOwner->IsDino() || m_pOwner->IsHelmProtection() == false)
	//{
		if( UserInfoContext::i()->IsGM_Observer() == false)
		{
			CHud::i()->OnDamageFilter(filter_index);
		}
	//}
}

/*virtual*/ bool CGameCharaHUDControl::OnApplyCrosshair( void)
{
 	if( m_pOwner->isLocal() || Is1PVObserver(m_pOwner))
	{
		if (g_pCamera == NULL)						return false;
		if (g_pCamera->getCurTarget() != m_pOwner)	return false;
		if (!g_pFramework->IsBattleStage())			return false;

		CROSSHAIR_MODE type = CH_MODE_NORMAL;

		CStageBattle * pStage = (CStageBattle*)g_pFramework->GetCurrentStage();

	if( pStage == nullptr)
		return false;

	if (CrosshairMgr::i()->getCrosshair() == nullptr) return false;
	if (CHud::i() == nullptr) return false;

	bool bEnableCross = true;			// CH가 나타날지 말지를 결정
	WeaponBase * pWeapon = nullptr;
	WEAPON_CLASS_TYPE weaponClass = WEAPON_CLASS_UNKNOWN;

	if( m_pOwner->IsDino() == false)
		pWeapon = getUsedWeapon();
	

		CHud::i()->SetEnableGrenadeLauncherCrossHair(false);

		if (pWeapon != NULL)
		{
			CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
			I3ASSERT(pWeaponInfo != NULL);

			weaponClass = pWeaponInfo->GetTypeClass();

			//	Crosshair
			if (!m_pOwner->isCharaStateMask(CHARA_STATE_DEATH))
			{
				if (i3::kind_of<WeaponDotLaser*>(pWeapon))
				{
					BOOL bEnable = TRUE;

					if (!Is1PVObserver(m_pOwner))
						bEnable = ((WeaponDotLaser*)pWeapon)->getEnableDotLaser();

					if (bEnable || pWeapon->isWeaponStateMask(WSM_ZOOM) ||
						pWeapon->isPossibleTurnOffWithDotLaser() == FALSE ||
						(pWeapon->GetCurrentExtensionType() == WEAPON::EXT_ZOOM && m_pOwner->isCharaStateMask(CHARA_STATE_ZOOM)))
					{
						bEnableCross = FALSE;
						CHud::i()->SetEnableGrenadeLauncherCrossHair(FALSE);
					}
					else // 도트 포인터를 끄면 ''?
					{
						CHud::i()->SetEnableGrenadeLauncherCrossHair(FALSE);
					}
				}
				//	레이저 사이트 달린 유탄 발사기일 경우
				else if (i3::kind_of<WeaponGrenadeLauncherWithLaser*>(pWeapon))
				{
					bEnableCross = (pWeapon->GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER);

					CHud::i()->SetEnableGrenadeLauncherCrossHair(bEnableCross);

					bEnableCross = FALSE;
				}
				// 익스텐션 없이 유탄 발사 가능한 경우
				else if (i3::kind_of<WeaponHandGrenade*>(pWeapon))
				{
					CHud::i()->SetEnableGrenadeLauncherCrossHair(TRUE);
					bEnableCross = FALSE;

				}
				//	레이저 사이트 달린 산탄 발사기일 경우
				else if (i3::kind_of<WeaponShotGunLauncherWithLaser*>(pWeapon))
				{
					CHud::i()->SetEnableGrenadeLauncherCrossHair(FALSE);

					bEnableCross = FALSE;
				}
				// 2014-06-18 탈취모드 수박을 들었을 경우 // ENG_김대영
				else if (i3::kind_of<WeaponWatermelon*>(pWeapon))
				{
					bEnableCross = FALSE;
				}
				else
				{
					if ((weaponClass == WEAPON_CLASS_SNIPER) ||
						(weaponClass == WEAPON_SUBCLASS_SNIPER))
					{	// 익스텐션없는 저격총
						bEnableCross = m_pOwner->isCharaStateMask(CHARA_STATE_ZOOM);

						if (bEnableCross)
							type = CH_MODE_SCOPE;						// Scope
					}

					switch (pWeapon->GetCurrentExtensionType())
					{
					case WEAPON::EXT_SCOPE:
					{
						bEnableCross = TRUE;
						if (m_pOwner->isCharaStateMask(CHARA_STATE_ZOOM))
							type = CH_MODE_SCOPE;						// Scope
						CHud::i()->SetEnableGrenadeLauncherCrossHair( false);
					}
					break;

				case WEAPON::EXT_ZOOM :
				case WEAPON::EXT_DOT_SIGHT :
					bEnableCross = ! m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM);
					CHud::i()->SetEnableGrenadeLauncherCrossHair( false);
					break;

					case WEAPON::EXT_GLAUNCHER:
						bEnableCross = !pWeapon->isExtensionActivate();
						CHud::i()->SetEnableGrenadeLauncherCrossHair(!bEnableCross);
						break;

					default:
						CHud::i()->SetEnableGrenadeLauncherCrossHair(FALSE);
						break;
					}

					if (pWeaponInfo->GetTypeClass() == WEAPON_CLASS_ROCKET_LAUNCHER)
					{	// RPG7
						bEnableCross = FALSE;
						type = CH_MODE_NORMAL;
						CHud::i()->SetEnableGrenadeLauncherCrossHair(FALSE);
					}
				}
			}
			else
			{
				bEnableCross = FALSE;	//	캐릭터가 죽은 경우
				CHud::i()->SetEnableGrenadeLauncherCrossHair(FALSE);
			}

			if (pWeaponInfo->GetCrossHairInvisible() & 0x00000002)
			{
				// 이 후에 강제로 Scope CH가 사라진다고 하더라도, 현재 ScopeCH가 ON이라는 것은
				// Scope 모드이기 때문에 일반 CH는 나타나지 않아야 한다.
				if (type == CH_MODE_SCOPE)
					bEnableCross = false;

				type = CH_MODE_NORMAL;
			}

			if (pWeaponInfo->GetCrossHairInvisible() & 0x00000001)
			{
				// Normal Crosshair
				if (type == CH_MODE_NORMAL)
					bEnableCross = FALSE;
			}

			if (((weaponClass == WEAPON_CLASS_SNIPER) ||
				(weaponClass == WEAPON_SUBCLASS_SNIPER)) &&
				(pWeaponInfo->GetCrossHairExpand() == 0.0f && !m_pOwner->isCharaStateMask(CHARA_STATE_ZOOM)))
			{
				bEnableCross = FALSE;
			}
		}

		// 관전 또는 헬기에 탑승시 Crosshair를 항상 Disable 시킨다.
		if (i3::same_of<CGameCamera_Observer*>(g_pCamera)
			|| i3::same_of<CGameCamera_Observer2*>(g_pCamera)
			|| i3::same_of<CGameCamera_FLY*>(g_pCamera)
			|| (i3::kind_of<CGameCamera_OnObject*>(g_pCamera)))
		{
			// 관전 시에는 Crosshair를 항상 Disable 시킨다.
			bEnableCross = FALSE;
		}

		if (m_pOwner->isCharaStateMask(CHARA_STATE_DEATH))
			bEnableCross = FALSE;

		UIBattlePopupBase * p = GameUI::GetBattlePopup(UBP_ACCUSE_CRIME);
		if (p != NULL && p->isEnable() == TRUE)
			bEnableCross = FALSE;
		// Dragonov와 SSG69와 같이 순줌이 되는 무기의 경우엔 Scope_CH를 나타내지 않는다.
		if ((type == CH_MODE_SCOPE) && (pWeapon->getScopeCrosshairEnable() == FALSE))
			type = CH_MODE_NORMAL;

		//옵져버는 해당 유저의 크로스헤어를 출력해야한다!
		if (Is1PVObserver(m_pOwner) && bEnableCross)
		{
			CrosshairMgr::i()->ApplyTargetCrosshair(m_pOwner);
		}
		else
		{
			CrosshairMgr::i()->setCrosshairType(type);
			CrosshairMgr::i()->enableCrossHair(bEnableCross ? true : false);
			CrosshairMgr::i()->setTargetChara(NULL);
		}

		if (MyRoomInfoContext::i()->getStage()->IsHudOff())
		{
			CrosshairMgr::i()->enableCrossHair(false);// != FALSE);
			if (weaponClass == WEAPON_CLASS_HANDGUN) CHud::i()->SetEnableGrenadeLauncherCrossHair(FALSE);
		}

		return true;
	}

	return false;
}

