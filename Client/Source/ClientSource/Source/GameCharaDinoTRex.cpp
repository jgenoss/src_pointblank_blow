#include "pch.h"
#include "GameCharaDinoTRex.h"

#include "GameCharaContexts.h"
#include "Character/GameDinoAnimContext.h"
#include "StageBattle.h"
#include "GlobalVariables.h"
#include "WeaponDino.h"

#include "ui/UIHudDefine.h"
#include "ui/UIHudUtil.h"
#include "BattleServerContext.h"

I3_CLASS_INSTANCE( CGameCharaDinoTRex);

CGameCharaDinoTRex::CGameCharaDinoTRex(void)
{
	m_eNowDinoAttackType = DAT_REX_ATTACK2;
}

CGameCharaDinoTRex::~CGameCharaDinoTRex()
{
	I3_SAFE_RELEASE(m_pTrexCrash);
}

void CGameCharaDinoTRex::_CreateDino( void)
{
	CGameCharaDino::_CreateDino();

	I3_SAFE_RELEASE( m_pTrexCrash );
	T_ItemID ItemID = getDBInfo()->GetDinoWeaponID(2);
	m_pTrexCrash = i3::kind_cast<WeaponDino*>( g_pWeaponManager->_CreateDinoWeapon(ItemID, ItemID, this) );
	I3ASSERT(m_pTrexCrash != nullptr);
}

/*virtual*/ void CGameCharaDinoTRex::_PlayDinoSound_Fire(WEAPON::FIRE_ORDER fireOrder)
{
	switch(fireOrder)
	{
	case WEAPON::PRIMARY_FIRE:
		{
			g_pSndMng->StartDinoActionSound( GTBD_ON_TREX, GTBDA_ON_ATTACK_1_A, GetPos(), is1PV());
		}
		break;
	case WEAPON::SECONDARY_FIRE:
		{
			g_pSndMng->StartDinoActionSound( GTBD_ON_TREX, GTBDA_ON_ATTACK_2, GetPos(), is1PV());
		}
		break;
	}
}

//EscapeMission :
ID_UPPER_DINO_AI CGameCharaDinoTRex::DinoAttack(WEAPON::FIRE_ORDER fireOrder)
{
	ID_UPPER_DINO_AI dinoAI = ID_UPPER_DINO_UNKNOWN;
	((WeaponDino*)GetDinoWeapon())->SetFireOrder(fireOrder);
	ChangeDinoWeapon(fireOrder);

	if( isDeathblowState())
		return dinoAI;

	if( isCharaStateMask( CHARA_STATE_DISABLE_SHOT) == true)
		return dinoAI;

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

void CGameCharaDinoTRex::_PlayBreathSnd(void)
{
	INT32 nRand = i3Math::Rand()%4;

	g_pSndMng->StartDinoActionSound( 0, GTBDA_ON_BREATH_1 + nRand, GetPos(), is1PV());
}

void CGameCharaDinoTRex::OnDinoAttackRun(i3AIContext * pCtx, REAL32 tm)
{
	if(m_eNowDinoAttackType == DAT_REX_ATTACK1)
	{
		m_arDinoWeapon[WEAPON::PRIMARY_FIRE]->DinoAttackProcess( pCtx->getLocalTime(), getAnimContext()->getUpperTimeScale() );
	}
	else if(m_eNowDinoAttackType == DAT_REX_ATTACK2)
	{
		bool isAtkSuccess = m_arDinoWeapon[WEAPON::SECONDARY_FIRE]->DinoAttackProcess( pCtx->getLocalTime() );

		if( isAtkSuccess)
		{
			m_isAvailHowl = false;
			m_rElapsedHowlTime = GetDinoSkillCoolTime( WEAPON::SECONDARY_FIRE);
		}
	}
}

REAL32 CGameCharaDinoTRex::GetHowlCooldownRate(void)
{
	REAL32 max = MINMAX( 0.1f, GetDinoSkillCoolTime(WEAPON::SECONDARY_FIRE), 100.0f);
	REAL32 rate = MINMAX( 0.0f, m_rElapsedHowlTime / max, 1.0f);

	return ( 1.0f - rate);
}

void CGameCharaDinoTRex::OnUpdate( REAL32 rDeltaSeconds)
{
	CGameCharaDino::OnUpdate( rDeltaSeconds);

	if( isCharaStateMask( CHARA_STATE_DEATH) == true)
		return;


	if( getActionContext()->isLower( CHARA_LOWER_RUN) || getActionContext()->isLower( CHARA_LOWER_WALK))
	{
		m_pTrexCrash->DinoAttackProcess(rDeltaSeconds);
	}

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

void CGameCharaDinoTRex::UpdateHowlGuage()
{
	hu_dino::set_dino_raio( GetHowlCooldownRate() );
}

WEAPON::FIRE_ORDER CGameCharaDinoTRex::GetFireOrder()
{
	if(m_eNowDinoAttackType == DAT_REX_ATTACK1)
	{
		return WEAPON::PRIMARY_FIRE;
	}
	else if(m_eNowDinoAttackType == DAT_REX_ATTACK2)
	{
		return WEAPON::SECONDARY_FIRE;
	}

	I3ASSERT_0;
	return WEAPON::SECONDARY_FIRE;
}

void CGameCharaDinoTRex::ResetChara()
{
	CGameCharaDino::ResetChara();

	hu_dino::set_dino_attribute( GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_TREX"), 
		DinoState::BITE, DinoState::HOWL );
}

/*virtual*/ void CGameCharaDinoTRex::SetDinoFireStartPos(VEC3D * pSrc)
{
	if(is1PV())
	{
		CGameCharaDino::SetDinoFireStartPos(pSrc);
	}
	else
	{
		i3Vector::Sub( pSrc, i3Matrix::GetPos( getBoneContext()->getBoneMatrix( TREX_BONE_PIVOTCAM)), i3Matrix::GetPos( getBoneContext()->getIKMatrix()));
		i3Vector::Scale( pSrc, pSrc, 0.5f);
		i3Vector::Sub( pSrc, i3Matrix::GetPos( getBoneContext()->getBoneMatrix( TREX_BONE_PIVOTCAM)), pSrc);
	}
}

/*virtual*/ bool CGameCharaDinoTRex::isSkipAttack(INT32 idx)
{
	if( idx == WEAPON::SECONDARY_FIRE )
		return ( isLocal() && (m_isAvailHowl == false) );

	return false;
}

/*virtual*/ EDinoAttackType CGameCharaDinoTRex::GetDinoAttackType(INT32 idx)
{
	if( idx == WEAPON::PRIMARY_FIRE )
	{
		I3TRACE("TREX : BITE \n");
		return DAT_REX_ATTACK1;
	}
	else if ( idx == WEAPON::SECONDARY_FIRE )
	{
		I3TRACE("TREX : HOWL \n");
		return DAT_REX_ATTACK2;		
	}

	return m_eNowDinoAttackType;
}