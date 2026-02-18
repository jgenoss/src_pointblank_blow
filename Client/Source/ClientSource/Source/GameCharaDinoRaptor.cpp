#include "pch.h"
#include "GameCharaDinoRaptor.h"

#include "GameCharaContexts.h"
#include "Character/GameDinoAnimContext.h"
#include "StageBattle.h"
#include "Crosshair_Dino_Raptor.h"
#include "WeaponDino.h"
#include "CrosshairMgr.h"

#include "ui/UIHudDefine.h"
#include "ui/UIHudUtil.h"

I3_CLASS_INSTANCE( CGameCharaDinoRaptor);

CGameCharaDinoRaptor::CGameCharaDinoRaptor()
{
	m_eNowDinoAttackType = DAT_RAPTOR_ATTACK_B;
}

void CGameCharaDinoRaptor::_CreateDino( void)
{
	CGameCharaDino::_CreateDino();
}

/*virtual*/ void CGameCharaDinoRaptor::_PlayDinoSound_Fire(WEAPON::FIRE_ORDER fireOrder)
{
	switch(fireOrder)
	{
	case WEAPON::PRIMARY_FIRE:
		{
			g_pSndMng->StartDinoActionSound( GTBD_ON_RAPTOR, GTBDA_ON_ATTACK_1_A, GetPos(), is1PV());
		}
		break;
	case WEAPON::SECONDARY_FIRE:
		{
			g_pSndMng->StartDinoActionSound( GTBD_ON_RAPTOR, GTBDA_ON_ATTACK_2, GetPos(), is1PV());
		}
		break;
	}
}

void CGameCharaDinoRaptor::_PlayDownfallSound( void)
{
	g_pSndMng->StartDinoActionSound( 1, GTBDA_ON_DAMAGE_DOWNFALL, GetPos(), is1PV());
}

ID_UPPER_DINO_AI CGameCharaDinoRaptor::DinoAttack(WEAPON::FIRE_ORDER fireOrder)
{
	ID_UPPER_DINO_AI dinoAI = ID_UPPER_DINO_UNKNOWN;
	((WeaponDino*)GetDinoWeapon())->SetFireOrder(fireOrder);
	ChangeDinoWeapon(fireOrder);

	dinoAI = _FindDinoUpperAttackAI(fireOrder);

	getHUDContext()->_ApplyCrosshair();
	addEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE );

	if(dinoAI != ID_UPPER_DINO_UNKNOWN)
	{
		_PlayDinoAnimUpper_Fire(fireOrder, dinoAI);
	}

	return dinoAI;
}

void CGameCharaDinoRaptor::OnDinoAttackRun(i3AIContext * pCtx, REAL32 tm)
{
	if(m_eNowDinoAttackType == DAT_RAPTOR_ATTACK_A)
	{
		m_arDinoWeapon[WEAPON::PRIMARY_FIRE]->DinoAttackProcess( pCtx->getLocalTime(), getAnimContext()->getUpperTimeScale() );		
	}
	else if(m_eNowDinoAttackType == DAT_RAPTOR_ATTACK_B)
	{
		m_arDinoWeapon[WEAPON::SECONDARY_FIRE]->DinoAttackProcess( pCtx->getLocalTime(), getAnimContext()->getUpperTimeScale() );
	}
}

WEAPON::FIRE_ORDER CGameCharaDinoRaptor::GetFireOrder()
{
	if(m_eNowDinoAttackType == DAT_RAPTOR_ATTACK_B)
	{
		return WEAPON::SECONDARY_FIRE;
	}
	else if(m_eNowDinoAttackType == DAT_RAPTOR_ATTACK_A)
	{
		return WEAPON::PRIMARY_FIRE;
	}

	I3ASSERT_0;
	return WEAPON::PRIMARY_FIRE;
}

void CGameCharaDinoRaptor::_PlayBreathSnd(void)
{
	INT32 nRand = i3Math::Rand()%4;
	
	g_pSndMng->StartDinoActionSound( 1, GTBDA_ON_BREATH_1 + nRand, GetPos(), is1PV());
}

void CGameCharaDinoRaptor::ResetChara()
{
	CGameCharaDino::ResetChara();

	Crosshair * pCurCH = CrosshairMgr::i()->getCrosshair();
	if(pCurCH != nullptr) {
		pCurCH->ResetCrosshair();
	}

	hu_dino::set_dino_attribute( GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_RAPTOR"), 
		DinoState::SCRATCH, DinoState::BITE );
}

/*virtual*/ void CGameCharaDinoRaptor::SetDinoFireStartPos(VEC3D * pSrc)
{
	if(is1PV())
	{
		MATRIX matCam;
		getCameraContext()->getVirtualCamPosition( pSrc, &matCam);
		VEC3D* dir = i3Matrix::GetAt(&matCam);
		i3Vector::Normalize(dir, dir);
		dir->x *= -0.15f; dir->y *= -0.15f; dir->z *= -0.15f;
		i3Vector::Add(pSrc, pSrc, dir);
	}
	else
		CGameCharaDino::SetDinoFireStartPos(pSrc);
}

/*virtual*/ EDinoAttackType CGameCharaDinoRaptor::GetDinoAttackType(INT32 idx)
{
	if( idx == WEAPON::PRIMARY_FIRE )
	{
		I3TRACE("RAPTOR : SCRATCH \n");
		return DAT_RAPTOR_ATTACK_A;
	}
	else if ( idx == WEAPON::SECONDARY_FIRE )
	{	
		I3TRACE("RAPTOR : BITE \n");
		return DAT_RAPTOR_ATTACK_B;
	}

	return m_eNowDinoAttackType;
}