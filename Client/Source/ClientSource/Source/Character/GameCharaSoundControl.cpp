#include "pch.h"
#include "GameCharaSoundControl.h"
#include "GameCharaBase.h"
#include "../Weapon/WeaponItemID.h"
#include "../Weapon/WeaponInfo/WeaponDinoInfo.h"

I3_CLASS_INSTANCE( CGameCharaSoundControl);

/*virtual*/ void CGameCharaSoundControl::OnHit( CHARA_DAMAGEINFO * pDamageInfo, VEC3D * pPos, bool bListener)
{
	CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( pDamageInfo->_ui32FireWeaponID);
	WeaponBase *  pWeaponBase = pDamageInfo->_pWeapon;

	if( pDamageInfo->isDamageTypeMelee() && pWeaponBase)
	{
		bool bRand = false;
		WEAPON_CLASS_TYPE weaponClass = pDamageInfo->GetFireWeaponClass();

		//2014-09-16 투척무기류(미니액스,중식칼등)의 피격사운드 문제로,
		//weaponinfo를 현재 상태의 무기 정보로 바꿔줍니다. ENG_김대영
		WeaponBase * pCurrentShapeWeapon = pWeaponBase->getUsedWeapon();
		if( pWeaponBase != pCurrentShapeWeapon)
			pWeaponInfo = pCurrentShapeWeapon->getWeaponInfo();

		if( weaponClass == WEAPON_CLASS_KNIFE &&
			(pDamageInfo->isFireWeaponNumber( WEAPON::SHOVEL) || pDamageInfo->isFireWeaponNumber( WEAPON::SHOVEL_ROYAL)))
			bRand = true;

		if( weaponClass == WEAPON_CLASS_KNUCKLE)
		{
			//	Brass Knuckle hit
			INT32 nRnd = i3Math::Rand() % 3;

			// 3가지 사운드중에서 랜덤.
			if( pDamageInfo->_FireOrder == WEAPON::SECONDARY_FIRE)
			{
				g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_KNUCKLE_HUMAN_B, nRnd, pPos, bListener);
			}
			else
			{
				g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_KNUCKLE_HUMAN_A, nRnd, pPos, bListener);	
			}
		}
		else if( weaponClass == WEAPON_CLASS_DUALKNIFE)
		{
			if( pWeaponInfo->GetCharaHitSoundIndex() > -1 )
				g_pSndMng->StartWeaponStateSnd( pWeaponInfo->GetCharaHitSoundIndex(), 0, pPos, bListener);
			else
				g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_KNIFE_HUMAN, 0, pPos, bListener);
		}
		else
		{
			if( pDamageInfo->_FireOrder == WEAPON::SECONDARY_FIRE)
			{
				if( pDamageInfo->isRealWeaponNumber( WEAPON::GH5007))
				{
					if( pDamageInfo->_pWeapon != nullptr && pDamageInfo->_pWeapon->getAutoFireCount() == 0)
						g_pSndMng->StartWeaponStateSnd( pWeaponInfo->GetCharaHitSoundIndex(), 0, pPos, bListener);
					else 
						g_pSndMng->StartWeaponStateSnd( pWeaponInfo->GetCharaHitSoundIndex2(), 0, pPos, bListener);
				}
				else if( pWeaponInfo->GetCharaHitSoundIndex2() > -1)
					g_pSndMng->StartWeaponStateSnd( pWeaponInfo->GetCharaHitSoundIndex2(), bRand?(i3Math::Rand()%2):0, pPos, bListener);
				else
					g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_KNIFE_HUMAN, 0, pPos, bListener);
			}
			else
			{
				if( !pDamageInfo->isRealWeaponNumber( WEAPON::GH5007) &&
					(pWeaponInfo->GetCharaHitSoundIndex() > -1) )
				{
					g_pSndMng->StartWeaponStateSnd( pWeaponInfo->GetCharaHitSoundIndex(), bRand?(i3Math::Rand()%2):0, pPos, bListener);
				}
				else
					g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_KNIFE_HUMAN, 0, pPos, bListener);
			}
		}
		
	}
	else
	{
		if( bListener)
		{
			// 기획팀 요청으로 사운드를 고정합니다. 2008.11.10
			if( pDamageInfo->isFireWeaponType(WEAPON_CLASS_DINO) )
			{
				INT32 nType = ((CWeaponDinoInfo*)pWeaponInfo)->GetHitSoundType();
				INT32 nIndex = ((CWeaponDinoInfo*)pWeaponInfo)->GetHitSoundIndex();
				if( nType > -1 && nIndex > -1)
					g_pSndMng->StartDinoActionSound( nType, nIndex, pPos, bListener);
			}
			else
			{
				bool bPlayed = false;
				if( pDamageInfo->_FireOrder == WEAPON::SECONDARY_FIRE)
				{
					if( pWeaponInfo->GetCharaHitSoundIndex2() > -1)
					{
						g_pSndMng->StartWeaponStateSnd( pWeaponInfo->GetCharaHitSoundIndex2(), 0, pPos, bListener);
						bPlayed = true;
					}
				}
				
				if( bPlayed == false)
				{
					if( m_pOwner->is1PV())
						g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_HUMAN, 2, pPos, true);
					else
						g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_HUMAN, i3Math::Rand() %2, pPos, true);
				}
			}
		}
		else
		{
			g_pSndMng->ReserveSoundPlay( IT_WEAPON_STATE, GTBWS_ON_HIT_HUMAN, i3Math::Rand()%2, pPos, false);
		}
	}
}

/*virtual*/ void CGameCharaSoundControl::OnJumpStart( void)
{
	g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, getMoveContext()->getFloorTerrainType(), GTBT_JUMP_START, m_pOwner->GetPos(), m_pOwner->is1PV());
}

/*virtual*/ void CGameCharaSoundControl::OnJumpEnd( void)
{
	g_pSndMng->ReserveSoundPlay( IT_TERRAIN_TYPE, getMoveContext()->getFloorTerrainType(), GTBT_JUMP_END, m_pOwner->GetPos(), m_pOwner->is1PV());
}

