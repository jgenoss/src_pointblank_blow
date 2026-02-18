#include "pch.h"

#include "GameCharaDinoCCRaptor.h"
#include "GameCharaContexts.h"
#include "Character/GameDinoAnimContext.h"
#include "StageBattle.h"
#include "Crosshair_Dino_Raptor.h"
#include "WeaponDino.h"
#include "CrosshairMgr.h"
#include "GameCharaContexts.h"
#include "UI/UIHudManager.h"
#include "UI/UIHudDinoState.h"
#include "ui/UIHudDefine.h"
#include "ui/UIHudUtil.h"


I3_CLASS_INSTANCE( CGameCharaDinoCCRaptor);//, CGameCharaDinoRaptor);

CGameCharaDinoCCRaptor::~CGameCharaDinoCCRaptor()
{
	I3_SAFE_DELETE( m_buff);
}

void CGameCharaDinoCCRaptor::_CreateDino( void)
{
	CGameCharaDino::_CreateDino();

	m_idxRaptorBuffEffect = g_pEffectManager->FindEffect( "TANK_ATTACK_B");
}

void CGameCharaDinoCCRaptor::Create(GLOBAL_CHARA_INFO* pCharaInfo, const CCharaInfo* pDBInfo)
{
	CGameCharaDino::Create(pCharaInfo, pDBInfo);

	DinoSkill::BuffInfo info;
	info.owner		 = this;
	info.effectIdx	 = g_pEffectManager->FindEffect("TANK_ATTACK_B");
	info.speedRate	 = GetDinoSkillMovementRate(WEAPON::SECONDARY_FIRE);
	info.cooldown	 = GetDinoSkillCoolTime(WEAPON::SECONDARY_FIRE);
	info.buffSustain = GetDinoSkillDuration(WEAPON::SECONDARY_FIRE);

	m_buff = new DinoSkill::Buff(info);
	MEMDUMP_NEW( m_buff, sizeof( DinoSkill::Buff));
}

/*virtual*/ void CGameCharaDinoCCRaptor::Destroy( void)
{
	CGameCharaBase::Destroy();

	I3_SAFE_DELETE( m_buff);
}

ID_UPPER_DINO_AI CGameCharaDinoCCRaptor::DinoAttack(WEAPON::FIRE_ORDER fireOrder)
{
	// Howl Buff 쿨타임 대기중이면 사용 공격 모션 안함
	if(fireOrder == WEAPON::SECONDARY_FIRE)
	{
		if(m_buff->CooldownRate() < 1.0f)
			return ID_UPPER_DINO_UNKNOWN;
	}

	return CGameCharaDinoRaptor::DinoAttack(fireOrder);
}

void CGameCharaDinoCCRaptor::OnUpdate( REAL32 rDeltaSeconds)
{
	CGameCharaDino::OnUpdate( rDeltaSeconds);

	if( isCharaStateMask( CHARA_STATE_DEATH) == true)
		return;

	m_buff->Update(rDeltaSeconds);
}

void CGameCharaDinoCCRaptor::OnDinoAttackRun(i3AIContext * pCtx, REAL32 tm)
{
	if(m_eNowDinoAttackType == DAT_RAPTOR_ATTACK_A)
	{
		m_arDinoWeapon[WEAPON::PRIMARY_FIRE]->DinoAttackProcess( pCtx->getLocalTime(), getAnimContext()->getUpperTimeScale() );		
	}
	else if (m_eNowDinoAttackType == DAT_RAPTOR_ATTACK_B)
	{
		bool isAtkSuccess = m_arDinoWeapon[WEAPON::SECONDARY_FIRE]->DinoAttackProcess( pCtx->getLocalTime() );		

		if(isAtkSuccess)
			m_buff->Start();
	}
}

void CGameCharaDinoCCRaptor::ResetChara()
{
	CGameCharaDinoRaptor::ResetChara();

	hu_dino::set_dino_attribute( GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_RAPTOR"), 
		DinoState::SCRATCH, DinoState::RAPTOR_BUFF );

	if (m_buff) // Create()에서 ResetChara()를 호출할 때는 생성이 안되어 있다. Respawn() 호출 시에는 필요하다.
		m_buff->Stop();
}
