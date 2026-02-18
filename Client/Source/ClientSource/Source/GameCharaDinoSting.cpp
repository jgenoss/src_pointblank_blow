#include "pch.h"
#include "GameCharaDinoSting.h"

#include "GameCharaContexts.h"
#include "Character/GameDinoAnimContext.h"
#include "StageBattle.h"
#include "Crosshair_Dino_Sting.h"
#include "WeaponDino.h"
#include "CrosshairMgr.h"
#include "Weapon/WeaponItemID.h"
#include "ui/UIHudDefine.h"
#include "ui/UIHudUtil.h"
#include "BattleSlotContext.h"
#include "MainWeapon_GrenadeShell.h"
#include "i3Base/string/ext/utf16_to_mb.h"

I3_CLASS_INSTANCE( CGameCharaDinoSting);

CGameCharaDinoSting::CGameCharaDinoSting()
{	
	m_eNowDinoAttackType = DAT_STING_ATTACK2;
}

void CGameCharaDinoSting::_CreateDino( void)
{
//	char szFireEffect[256];

	CGameCharaDino::_CreateDino();
	
	i3::rc_wstring wstrFireEffect;

	m_arDinoWeapon[WEAPON::PRIMARY_FIRE]->getWeaponInfo()->GetFireEffect(wstrFireEffect);

	i3::string strFireEffect;		i3::utf16_to_mb(wstrFireEffect, strFireEffect);

	m_idxSpitFireEffect = g_pEffectManager->FindEffect( strFireEffect.c_str() );
	m_idxPoisonFireEffect = g_pEffectManager->FindEffect( "Dino_Sting_Fir_B");
}

/*virtual*/ void CGameCharaDinoSting::_PlayDinoSound_Fire(WEAPON::FIRE_ORDER fireOrder)
{
	switch(fireOrder)
	{
	case WEAPON::PRIMARY_FIRE:
		{
			g_pSndMng->StartDinoActionSound( GTBD_ON_STING, GTBDA_ON_ATTACK_1_A, GetPos(), is1PV());
		}
		break;
	case WEAPON::SECONDARY_FIRE:
		{
			g_pSndMng->StartDinoActionSound( GTBD_ON_STING, GTBDA_ON_ATTACK_2, GetPos(), is1PV());
		}
		break;
	}
}

void CGameCharaDinoSting::_PlayDownfallSound( void)
{
	g_pSndMng->StartDinoActionSound( 2, GTBDA_ON_DAMAGE_DOWNFALL, GetPos(), is1PV());
}

void CGameCharaDinoSting::ThrowPoisonGrenade()
{
	WeaponDinoBreath * pWeapon = i3::same_cast<WeaponDinoBreath*>( m_arDinoWeapon[WEAPON::SECONDARY_FIRE] );
	I3ASSERT(pWeapon != nullptr);

	pWeapon->ThrowShell();
}

ID_UPPER_DINO_AI CGameCharaDinoSting::DinoAttack(WEAPON::FIRE_ORDER fireOrder)
{
	ID_UPPER_DINO_AI dinoAI = ID_UPPER_DINO_UNKNOWN;
	((WeaponDino*)GetDinoWeapon())->SetFireOrder(fireOrder);
	ChangeDinoWeapon(fireOrder);

	if( isSkipAttack(fireOrder) == true )
		return dinoAI;

	dinoAI = _FindDinoUpperAttackAI(fireOrder);

	getHUDContext()->_ApplyCrosshair();
	addEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE );

	if(dinoAI != ID_UPPER_DINO_UNKNOWN)
	{
		_PlayDinoAnimUpper_Fire(fireOrder, dinoAI);
	}

	return dinoAI;
}


WEAPON::FIRE_ORDER CGameCharaDinoSting::GetFireOrder()
{
	if(m_eNowDinoAttackType == DAT_STING_ATTACK2)
	{
		return WEAPON::SECONDARY_FIRE;
	}
	else if(m_eNowDinoAttackType == DAT_STING_ATTACK1)
	{
		return WEAPON::PRIMARY_FIRE;
	}
	I3ASSERT_0;

	//빌드 경고 때문에 넣는다.
	return WEAPON::FIRE_ORDER_COUNT;
}

void CGameCharaDinoSting::OnUpdate( REAL32 rDeltaSeconds)
{
	CGameCharaDino::OnUpdate( rDeltaSeconds);

	if( isCharaStateMask( CHARA_STATE_DEATH) == true)
		return;


	if( m_isAvailPoison == false)
	{
		m_rElapsedPoisonTime -= rDeltaSeconds;
		if( m_rElapsedPoisonTime < 0.0f)
		{
			m_isAvailPoison = true;
		}
	}

	if (isLocal())
		UpdatePoisonGuage();
}

void CGameCharaDinoSting::UpdatePoisonGuage()
{
	hu_dino::set_dino_raio( GetPoisonCooldownRate() );
}

void CGameCharaDinoSting::OnDinoAttackRun(i3AIContext * pCtx, REAL32 tm)
{
	CWeaponInfo * pWeaponInfo = GetDinoWeapon()->getWeaponInfo();

	if(m_eNowDinoAttackType == DAT_STING_ATTACK1)
	{
		if( pWeaponInfo->GetTypeFire() != WEAPON::FIRE_RANGE)
			return;

		m_arDinoWeapon[WEAPON::PRIMARY_FIRE]->DinoAttackProcess( pCtx->getLocalTime(), getAnimContext()->getUpperTimeScale() );
	}
	else if(m_eNowDinoAttackType == DAT_STING_ATTACK2)
	{
		if( pWeaponInfo->GetTypeDamage() != WEAPON::DAMAGE_POISON )
		{
			return;
		}

		bool isAtkSuccess = m_arDinoWeapon[WEAPON::SECONDARY_FIRE]->DinoAttackProcess( pCtx->getLocalTime(), getAnimContext()->getUpperTimeScale() );

		if( isAtkSuccess)
		{
			m_isAvailPoison = false;
			m_rElapsedPoisonTime = GetDinoSkillCoolTime( WEAPON::SECONDARY_FIRE);
		}
	}
}
REAL32 CGameCharaDinoSting::GetPoisonCooldownRate(void)
{
	REAL32 max = MINMAX( 0.1f, GetDinoSkillCoolTime(WEAPON::SECONDARY_FIRE), 100.0f);
	REAL32 rate = MINMAX( 0.0f, m_rElapsedPoisonTime / max, 1.0f);

	return ( 1.0f - rate);
}

void CGameCharaDinoSting::_PlayBreathSnd(void)
{
	INT32 nRand = i3Math::Rand()%4;

	g_pSndMng->StartDinoActionSound( 2, GTBDA_ON_BREATH_1 + nRand, GetPos(), is1PV());
}

void CGameCharaDinoSting::ResetChara()
{
	CGameCharaDino::ResetChara();

	Crosshair * pCurCH = CrosshairMgr::i()->getCrosshair();
	if(pCurCH != nullptr) {
		pCurCH->ResetCrosshair();
	}

	hu_dino::set_dino_attribute( GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_STING"), 
		DinoState::POISON_PRICKLE, DinoState::POISON_GAS );
}

/*virtual*/ void CGameCharaDinoSting::SetDinoFireStartPos(VEC3D * pSrc)
{
	if(is1PV())
	{
		MATRIX matCam;
		getCameraContext()->getVirtualCamPosition( pSrc, &matCam);
		VEC3D* dir = i3Matrix::GetAt(&matCam);
		i3Vector::Normalize(dir, dir);
		dir->x *= -0.25f; dir->y *= -0.25f; dir->z *= -0.25f;
		i3Vector::Add(pSrc, pSrc, dir);
	}
	else
		CGameCharaDino::SetDinoFireStartPos(pSrc);
}

/*virtual*/ EDinoAttackType CGameCharaDinoSting::GetDinoAttackType(INT32 idx)
{
	if( idx == WEAPON::PRIMARY_FIRE )
	{
		I3TRACE("STING : PRIMARY \n");
		return DAT_STING_ATTACK1;
	}
	else if ( idx == WEAPON::SECONDARY_FIRE )
	{
		I3TRACE("STING : SECONDARY \n");
		return DAT_STING_ATTACK2;
	}

	return m_eNowDinoAttackType;
}

/*virtual*/ bool CGameCharaDinoSting::isSkipAttack(INT32 idx)
{
	if( idx == WEAPON::SECONDARY_FIRE )
		return ( isLocal() && (m_isAvailPoison == false) );

	return false;
}
