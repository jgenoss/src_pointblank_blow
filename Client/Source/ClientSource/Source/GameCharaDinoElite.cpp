#include "pch.h"
#include "GameCharaDinoElite.h"

#include "GameCharaContexts.h"
#include "Character/GameDinoAnimContext.h"
#include "StageBattle.h"
#include "Crosshair_Dino_Raptor.h"
#include "WeaponDino.h"
#include "CrosshairMgr.h"

#include "ui/UIHudDefine.h"
#include "ui/UIHudUtil.h"
#include "BattleServerContext.h"
#include "BattleSlotContext.h"

I3_CLASS_INSTANCE( CGameCharaDinoElite);

CGameCharaDinoElite::CGameCharaDinoElite()
{
	m_eNowDinoAttackType = DAT_ELITE_SCRATCH;
}


void CGameCharaDinoElite::_CreateDino( void)
{
	CGameCharaDino::_CreateDino();

	m_idxEliteScratchEffect = g_pEffectManager->FindEffect( "Dino_Elite_Fire_A");
	m_idxEliteBuffEffect = g_pEffectManager->FindEffect( "Dino_Elite_Fire_B_1");
}

/*virtual*/ void CGameCharaDinoElite::_PlayDinoSound_Fire(WEAPON::FIRE_ORDER fireOrder)
{
	switch(fireOrder)
	{
	case WEAPON::PRIMARY_FIRE:
		{
			g_pSndMng->StartDinoActionSound( GTBD_ON_ELITE, GTBDA_ON_ATTACK_1_A, GetPos(), is1PV());
		}
		break;
	case WEAPON::SECONDARY_FIRE:
		{
			g_pSndMng->StartDinoActionSound( GTBD_ON_ELITE, GTBDA_ON_ATTACK_2, GetPos(), is1PV());
		}
		break;
	}
}

void CGameCharaDinoElite::OnUpdate( REAL32 rDeltaSeconds)
{
	CGameCharaDino::OnUpdate( rDeltaSeconds);

	if( isCharaStateMask( CHARA_STATE_DEATH) == true)
		return;


	if( m_isAvailHowl == false)
	{
		m_rElapsedHowlTime -= rDeltaSeconds;
		if( m_rElapsedHowlTime < 0.0f)
		{
			m_isAvailHowl = true;
		}
	}
	if (isLocal())
		UpdateHowlGuage();
}

ID_UPPER_DINO_AI CGameCharaDinoElite::DinoAttack(WEAPON::FIRE_ORDER fireOrder)
{
	ID_UPPER_DINO_AI dinoAI = ID_UPPER_DINO_UNKNOWN;
	((WeaponDino*)GetDinoWeapon())->SetFireOrder(fireOrder);
	ChangeDinoWeapon(fireOrder);

	if( isSkipAttack(fireOrder) )
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

void CGameCharaDinoElite::OnDinoAttackRun(i3AIContext * pCtx, REAL32 tm)
{
	if(m_eNowDinoAttackType == DAT_ELITE_SCRATCH)
	{
		m_arDinoWeapon[WEAPON::PRIMARY_FIRE]->DinoAttackProcess( pCtx->getLocalTime(), getAnimContext()->getUpperTimeScale() );
	}
	else if(m_eNowDinoAttackType == DAT_ELITE_HOWL)
	{
		bool isAtkSuccess = m_arDinoWeapon[WEAPON::SECONDARY_FIRE]->DinoAttackProcess( pCtx->getLocalTime() );

		if( isAtkSuccess)
		{
			m_isAvailHowl = false;
			m_rElapsedHowlTime = GetDinoSkillCoolTime(WEAPON::SECONDARY_FIRE);
		}
	}
}

WEAPON::FIRE_ORDER CGameCharaDinoElite::GetFireOrder()
{
	if(m_eNowDinoAttackType == DAT_ELITE_HOWL)
	{
		return WEAPON::SECONDARY_FIRE;
	}
	else if(m_eNowDinoAttackType == DAT_ELITE_SCRATCH)
	{
		return WEAPON::PRIMARY_FIRE;
	}

	I3ASSERT_0;
	return WEAPON::PRIMARY_FIRE;
}

void CGameCharaDinoElite::_PlayBreathSnd(void)
{
	INT32 nRand = i3Math::Rand()%4;
	g_pSndMng->StartDinoActionSound( GTBD_ON_ELITE, GTBDA_ON_BREATH_1 + nRand, GetPos(), is1PV());
}

void CGameCharaDinoElite::ResetChara()
{
	CGameCharaDino::ResetChara();

	Crosshair * pCurCH = CrosshairMgr::i()->getCrosshair();
	if(pCurCH != nullptr) {
		pCurCH->ResetCrosshair();
	}

	hu_dino::set_dino_attribute( GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_ELITE"), 
		DinoState::SCRATCH, DinoState::SCRATCH );
}

void CGameCharaDinoElite::DoFireHitEffect( void)
{
	((WeaponDino*)GetDinoWeapon())->_FireEffect( getFXMatrix(), m_idxEliteScratchEffect, true);
}


void CGameCharaDinoElite::UpdateFlag()
{

	if(m_rAccuTimeForPenalty > 2.0f)
	{
		m_bExplosionPenalty = false;
	}
}

void CGameCharaDinoElite::UpdateHowlGuage()
{
	hu_dino::set_dino_raio( GetHowlCooldownRate() );
}

REAL32 CGameCharaDinoElite::GetHowlCooldownRate(void)
{
	REAL32 max = MINMAX( 0.1f, GetDinoSkillCoolTime(WEAPON::SECONDARY_FIRE), 100.0f);
	REAL32 rate = MINMAX( 0.0f, m_rElapsedHowlTime / max, 1.0f);

	return ( 1.0f - rate);
}

void CGameCharaDinoElite::HP_Recovery(REAL32 rRecoveryHP/* = 0.0f*/)
{
	REAL32 rHP = getCharaInfo()->GetHP();
	rHP += rRecoveryHP;

	if (rHP >= (REAL32) getFullHP() )
	{
		rHP = (REAL32) getFullHP();
	}

	if( BattleServerContext::i()->IsUseDedicationMode() == false)
	{
		if(BattleServerContext::i()->IsP2PHost())
		{
			m_pCharaInfo->SetHP(rHP);
			GAMEINFO_CHARACTER * pCharaInfo		= BattleSlotContext::i()->setGameInfo_Chara( getCharaNetIndex() );
			pCharaInfo->_tNetData._tHP.setHP( (UINT16) rHP);
			m_isAvailRecoveryHp = false;
			m_rElapsedRecoveryTime =0.0f;
		}
	}
}

/*virtual*/ EDinoAttackType CGameCharaDinoElite::GetDinoAttackType(INT32 idx)
{
	if( idx == WEAPON::PRIMARY_FIRE )
	{
		I3TRACE("ELITE : SCRATCH \n");
		return DAT_ELITE_SCRATCH;
	}
	else if ( idx == WEAPON::SECONDARY_FIRE )
	{
		I3TRACE("ELITE : HOWL \n");
		return DAT_ELITE_HOWL;
	}

	return m_eNowDinoAttackType;
}

/*virtual*/ bool CGameCharaDinoElite::isSkipAttack(INT32 idx)
{
	if( idx == WEAPON::SECONDARY_FIRE )
		return ( isLocal() && (m_isAvailHowl == false) );

	return false;
}