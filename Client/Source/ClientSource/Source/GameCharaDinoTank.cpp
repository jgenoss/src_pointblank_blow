#include "pch.h"
#include "GameCharaDinoTank.h"
#include "GameCharaContexts.h"
#include "Character/GameDinoAnimContext.h"
#include "StageBattle.h"
#include "Crosshair_Dino_Raptor.h"
#include "WeaponDino.h"
#include "CrosshairMgr.h"

#include "ui/UIHudDefine.h"
#include "ui/UIHudUtil.h"
#include "BattleServerContext.h"

#include "i3Base/string/ext/utf16_to_mb.h"

I3_CLASS_INSTANCE( CGameCharaDinoTank);//, CGameCharaDino);

CGameCharaDinoTank::CGameCharaDinoTank()
{
	m_eNowDinoAttackType = DAT_TANK_BUTT;
}

void CGameCharaDinoTank::_CreateDino( void)
{
	
	CGameCharaDino::_CreateDino();

	i3::rc_wstring wstrFireEffect;		i3::string strFireEffect;
	
	m_arDinoWeapon[WEAPON::PRIMARY_FIRE]->getWeaponInfo()->GetFireEffect(wstrFireEffect);
	i3::utf16_to_mb(wstrFireEffect, strFireEffect);

	m_idxButtFireEffect =  g_pEffectManager->FindEffect( strFireEffect.c_str() );

	m_arDinoWeapon[WEAPON::SECONDARY_FIRE]->getWeaponInfo()->GetFireEffect(wstrFireEffect);
	i3::utf16_to_mb(wstrFireEffect, strFireEffect);

	m_idxDashEffect =  g_pEffectManager->FindEffect( strFireEffect.c_str());

	m_idxHitDashEffect =  g_pEffectManager->FindEffect( "Hit_Tank_Dash");
}

/*virtual*/ void CGameCharaDinoTank::_PlayDinoSound_Fire(WEAPON::FIRE_ORDER fireOrder)
{
	switch(fireOrder)
	{
	case WEAPON::PRIMARY_FIRE:
		{
			g_pSndMng->StartDinoActionSound( GTBD_ON_TANK, GTBDA_ON_ATTACK_1_A, GetPos(), is1PV());
		}
		break;
	case WEAPON::SECONDARY_FIRE:
		{
			g_pSndMng->StartDinoActionSound( GTBD_ON_TANK, GTBDA_ON_ATTACK_2, GetPos(), is1PV());
			g_pSndMng->StartDinoActionSound( GTBD_ON_TANK, GTBDA_ON_BITE_HIT, GetPos(), is1PV());
		}
		break;
	}
}

void CGameCharaDinoTank::OnUpdate( REAL32 rDeltaSeconds)
{
	CGameCharaDino::OnUpdate( rDeltaSeconds);

	if( isCharaStateMask( CHARA_STATE_DEATH) == true)
		return;
	
	
		if( m_rDashSpeedGauge > 0.f && m_bDashPenalty == false)
		{
			m_isAvailDash = true;
		}


		UINT64 keyPress	= g_pFramework->getKeyPress();	
		UINT64 keyStroke = g_pFramework->getKeyStroke();	

		if( getActionContext()->isLower( CHARA_LOWER_RUN) || getActionContext()->isLower( CHARA_LOWER_WALK) )
		{
			m_arDinoWeapon[WEAPON::SECONDARY_FIRE]->DinoAttackProcess(rDeltaSeconds);
		}

		if( keyPress & GAME_KEY_ACTION_SUBFUNC || keyStroke & GAME_KEY_ACTION_SUBFUNC)
		{	
			
		
		}
		else 
		{
			m_isAvailCrash =false;
			m_rPressAccuTime = 0;
			m_rNoPressAccuTime += g_pViewer->getDeltaTime() ;

			m_rDashSpeedGauge += g_pViewer->getDeltaTime() *1.5f;
			if( m_rDashSpeedGauge > 8.0f )
			{
				m_rDashSpeedGauge = 8.0f;
			}
			
		}

		m_rAccuTimeForPenalty += g_pViewer->getDeltaTime();

		m_rOldDashSpeedGauge = m_rDashSpeedGauge;

	UpdateFlag();

	if (isLocal())
		UpdateDashGuage();
}

ID_UPPER_DINO_AI CGameCharaDinoTank::DinoAttack(WEAPON::FIRE_ORDER fireOrder)
{
	ID_UPPER_DINO_AI dinoAI = ID_UPPER_DINO_UNKNOWN;
	((WeaponDino*)GetDinoWeapon())->SetFireOrder(fireOrder);
	ChangeDinoWeapon(fireOrder);

	if ( isSkipAttack(fireOrder) == false)
	{
		dinoAI = _FindDinoUpperAttackAI(fireOrder);
	}

	getHUDContext()->_ApplyCrosshair();
	addEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE );

	if(dinoAI != ID_UPPER_DINO_UNKNOWN)
	{
		_PlayDinoAnimUpper_Fire(fireOrder, dinoAI);
	}

	return dinoAI;
}

void CGameCharaDinoTank::OnDinoAttackRun(i3AIContext * pCtx, REAL32 tm)
{
	if(m_eNowDinoAttackType == DAT_TANK_BUTT)
	{
		m_arDinoWeapon[WEAPON::PRIMARY_FIRE]->DinoAttackProcess( pCtx->getLocalTime(), getAnimContext()->getUpperTimeScale() );
	}
	else if(m_eNowDinoAttackType == DAT_TANK_DASH)
	{
		if( m_rDashSpeedGauge > 0.f && m_bDashPenalty == false)
		{		
			if( getActionContext()->isLower( CHARA_LOWER_RUN) && getActionContext()->isUpper( CHARA_UPPER_SECONDARY_FIRE) )
			{//
				m_isAvailCrash = true;

				CGameCharaBoneContext * pBoneCtx = getBoneContext();
				MATRIX * pMtx = pBoneCtx->getBoneMatrix( RAPTOR_BONE_IK);

				//PEF
				getMoveContext()->EnableSpeedReduction( 2.0f,0.1f);
				((WeaponDino*)GetDinoWeapon())->_FireEffect( pMtx, m_idxDashEffect, true);

			}

		}
		m_rNoPressAccuTime = 0;
		m_rPressAccuTime += g_pViewer->getDeltaTime();

		if(m_bDashPenalty == false)
		{
			m_rDashSpeedGauge -= g_pViewer->getDeltaTime() *2;
		}

		if(m_rDashSpeedGauge < 0)
		{
			/*			
			if( (m_rDashSpeedGauge * m_rOldFireSpeedGauge) < 0.f )
			{
			_StopFireSound();

			g_pSndMng->StartWeaponStateSnd( GTBWS_M197, 0, GetMuzzlePos(), (bool)((m_pOwner != nullptr) && ( m_pOwner->is1PV())));
			//GlobalFunc::PB_TRACE("WM_M197_OverHeat %f %f \n", (m_rPressAccuTime - m_rFireSpeedGauge), 5.5f + g_pViewer->getDeltaTime());
			m_bFired = false;
			}
			*/
			m_isAvailDash = false;

			m_bDashPenalty = true;
			m_rDashSpeedGauge = -0.000000001f;
			m_rAccuTimeForPenalty = 0;
		}
	}
}



WEAPON::FIRE_ORDER CGameCharaDinoTank::GetFireOrder()
{
	if(m_eNowDinoAttackType == DAT_TANK_DASH)
	{
		return WEAPON::SECONDARY_FIRE;
	}
	else if(m_eNowDinoAttackType == DAT_TANK_BUTT)
	{
		return WEAPON::PRIMARY_FIRE;
	}

	I3ASSERT_0;
	return WEAPON::PRIMARY_FIRE;
}

void CGameCharaDinoTank::_PlayBreathSnd(void)
{
	INT32 nRand = i3Math::Rand()%4;
	
	g_pSndMng->StartDinoActionSound( 3, GTBDA_ON_BREATH_1 + nRand, GetPos(), is1PV());
}

void CGameCharaDinoTank::ResetChara()
{
	CGameCharaDino::ResetChara();

	Crosshair * pCurCH = CrosshairMgr::i()->getCrosshair();
	if(pCurCH != nullptr) {
		pCurCH->ResetCrosshair();
	}

	hu_dino::set_dino_attribute( GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_TANK"), 
		DinoState::BUTT, DinoState::RUSH );
}

void CGameCharaDinoTank::DoFireHitEffect( void)
{
	((WeaponDino*)GetDinoWeapon())->_FireEffect( getFXMatrix(), m_idxButtFireEffect, true);
}

void CGameCharaDinoTank::UpdateFlag()
{

	if(m_rAccuTimeForPenalty > 3.0f)
	{
		m_bDashPenalty = false;
	}
}

void CGameCharaDinoTank::UpdateDashGuage()
{
	const REAL32 max = 8.0f;
	hu_dino::set_dino_raio( getDashGauge()/max );
}

/*virtual*/ bool CGameCharaDinoTank::isWalk(INT32 idx)
{
	if( idx == WEAPON::SECONDARY_FIRE )
		return false;

	return CGameCharaDino::isWalk(idx);
}

/*virtual*/ bool CGameCharaDinoTank::isSkipAttack(INT32 idx)
{
	if( idx == WEAPON::SECONDARY_FIRE )
		return (m_isAvailDash == false);

	return false;
}

/*virtual*/ EDinoAttackType CGameCharaDinoTank::GetDinoAttackType(INT32 idx)
{
	if( idx == WEAPON::PRIMARY_FIRE )
	{
		I3TRACE("TANK : BUTT \n");
		return DAT_TANK_BUTT;
	}
	else if ( idx == WEAPON::SECONDARY_FIRE )
	{
		I3TRACE("TANK : DASH \n");
		return DAT_TANK_DASH;		
	}

	return m_eNowDinoAttackType;
}
