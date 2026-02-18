#include "pch.h"
#include "GameCharaDinoCCSting.h"

#include "GameCharaContexts.h"
#include "Character/GameDinoAnimContext.h"
#include "StageBattle.h"
#include "Crosshair_Dino_Sting.h"
#include "WeaponDino.h"
#include "CrosshairMgr.h"
#include "Weapon/WeaponItemID.h"

#include "ui/UIHudDefine.h"
#include "ui/UIHudUtil.h"

#include "i3Base/string/ext/utf16_to_mb.h"


I3_CLASS_INSTANCE( CGameCharaDinoCCSting);//, CGameCharaDinoSting);

void CGameCharaDinoCCSting::_CreateDino( void)
{
	

	CGameCharaDino::_CreateDino();

	i3::rc_wstring wstrFireEffect;
	m_arDinoWeapon[WEAPON::SECONDARY_FIRE]->getWeaponInfo()->GetFireEffect(wstrFireEffect);
	
	i3::string strFireEffect;	i3::utf16_to_mb(wstrFireEffect, strFireEffect);

	m_idxSpitFireEffect = g_pEffectManager->FindEffect(strFireEffect.c_str());

	m_idxBlazeEffect = g_pEffectManager->FindEffect("DTown_Dino_Sting_Fir_A");
}

/*virtual*/ void CGameCharaDinoCCSting::_PlayDinoSound_Fire(WEAPON::FIRE_ORDER fireOrder)
{
	switch(fireOrder)
	{
	case WEAPON::PRIMARY_FIRE:
		{
			g_pSndMng->StartDinoActionSound( GTBD_ON_RAPTOR, GTBDA_ON_ATTACK_2, GetPos(), is1PV());
		}
		break;
	case WEAPON::SECONDARY_FIRE:
		{
			g_pSndMng->StartDinoActionSound( GTBD_ON_STING, GTBDA_ON_ATTACK_1_A, GetPos(), is1PV());
		}
		break;
	}
}

ID_UPPER_DINO_AI CGameCharaDinoCCSting::DinoAttack(WEAPON::FIRE_ORDER fireOrder)
{
	((WeaponDino*)GetDinoWeapon())->SetFireOrder(fireOrder);
	ChangeDinoWeapon(fireOrder);

	ID_UPPER_DINO_AI dinoAI = _FindDinoUpperAttackAI(fireOrder);

	getHUDContext()->_ApplyCrosshair();
	addEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE );

	if(dinoAI != ID_UPPER_DINO_UNKNOWN)
	{
		_PlayDinoAnimUpper_Fire(fireOrder, dinoAI);
	}

	return dinoAI;
}


void CGameCharaDinoCCSting::OnUpdate( REAL32 rDeltaSeconds)
{
	CGameCharaDino::OnUpdate( rDeltaSeconds);

	if( isCharaStateMask( CHARA_STATE_DEATH) == true)
		return;
}
void CGameCharaDinoCCSting::OnDinoAttackRun(i3AIContext * pCtx, REAL32 tm)
{
	CWeaponInfo * pWeaponInfo = GetDinoWeapon()->getWeaponInfo();
	if(m_eNowDinoAttackType == DAT_STING_ATTACK1)
	{
		if( pWeaponInfo->GetTypeFire() != WEAPON::FIRE_MELEE)
			return;

		m_arDinoWeapon[WEAPON::PRIMARY_FIRE]->DinoAttackProcess( pCtx->getLocalTime(), getAnimContext()->getUpperTimeScale() );

	}
	else if(m_eNowDinoAttackType == DAT_STING_ATTACK2)
	{
		if( pWeaponInfo->GetTypeFire() != WEAPON::FIRE_RANGE)
			return;

		m_arDinoWeapon[WEAPON::SECONDARY_FIRE]->DinoAttackProcess( pCtx->getLocalTime(), getAnimContext()->getUpperTimeScale() );
	}
}

void CGameCharaDinoCCSting::ResetChara()
{
	CGameCharaDino::ResetChara();

	Crosshair * pCurCH = CrosshairMgr::i()->getCrosshair();
	if(pCurCH != nullptr) {
		pCurCH->ResetCrosshair();
	}

	hu_dino::set_dino_attribute( GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_STING"), 
		DinoState::BITE, DinoState::POISON_PRICKLE );
}
