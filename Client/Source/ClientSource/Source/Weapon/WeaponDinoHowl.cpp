#include "pch.h"
#include "WeaponDinoHowl.h"
#include "GameCharaDino.h"
#include "BattleServerContext.h"
#include "BattleSlotContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaBoneContext.h"

I3_CLASS_INSTANCE( WeaponDinoHowl);

WeaponDinoHowl::WeaponDinoHowl()
{
}

WeaponDinoHowl::~WeaponDinoHowl()
{
}

/*virtual*/ CHARA_HIT_PART WeaponDinoHowl::GetHitRagdollPart(void)
{
	if( m_pOwner->IsRex() )
		return CHARA_HIT_DEATH_HOWL;

	return CHARA_HIT_ROOT;
}

bool WeaponDinoHowl::_IsValidChara(CGameCharaBase * pChara, WEAPON::DAMAGE_TYPE damageType)
{
	if(pChara == nullptr)
		return false;

	if( pChara->getCharaNetIndex() == -1 )
		return false;

	if( pChara->IsRex() )
		return false;

	if( pChara->isCharaStateMask( CHARA_STATE_DEATH) == true)
		return false;

	// BUFF는 공룡에게 적용, 그 외는 인간에게 적용
	if( damageType == WEAPON::DAMAGE_BUFF )
	{
		if( !pChara->IsDino() )
			return false;
	}
	else
	{
		if( pChara->IsDino() )
			return false;
	}

	return true;
}

void WeaponDinoHowl::_SetTrexTrembleCamera(REAL32 rRange, REAL32 rSlope, REAL32 rEffectiveTime )
{
	CGameCharaBase * pTarget = g_pCamera->getCurTarget();

	if( pTarget == m_pOwner)
	{
		g_pCamera->SetTremble( 0.3f, 30.0, 0.1f, 3.f);
	}
	else if(pTarget != nullptr) 
	{

		VEC3D vLength, vDir;
		i3Vector::Sub( &vLength, pTarget->GetPos(), m_pOwner->GetPos() );
		REAL32 rLength = i3Vector::Normalize( &vDir, &vLength);

		REAL32 TrembAmpRate = 1.0f - ((rLength / rRange) * rSlope);
		TrembAmpRate *= 1.0f; //pInfo->getFootStepTrembleAmp();
		if( TrembAmpRate < 0.3f)
		{
			TrembAmpRate = 0.3f;
		}

		g_pCamera->SetTremble( 1.5f * TrembAmpRate * TrembAmpRate, 20.0f * TrembAmpRate, 0.1f, rEffectiveTime);
	}

	//	if( rLength < pInfo->getHowlRange() && pTarget != this)
	//	{
	//		pTarget->EFFECT_HOWL( pInfo->getHowlEffectiveTime());
	//	}
}

void WeaponDinoHowl::ProcessDinoHit(CHARA_DAMAGEINFO * pDamageInfo)
{
	// 데미지가 없는 경우 리턴(Raptor Howl)
	if(pDamageInfo->_rDamage <= 0.0f)
		return;

	const REAL32 rDamage = pDamageInfo->_rDamage;
	const REAL32 rEffectiveTime = pDamageInfo->_rEffectiveTime;
	const REAL32 rRange =  pDamageInfo->_rExplosionRange;
	const REAL32 rSlope = pDamageInfo->_rExplosionSlope;

	WEAPON::DAMAGE_TYPE dmgType = pDamageInfo->_DamageType;

	for(INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)
	{
		CGameCharaBase * pChara = g_pCharaManager->getChara( i);
		
		if( _IsValidChara(pChara, dmgType) == false)	continue;

		VEC3D vLength, vDir;
		i3Vector::Sub( &vLength, pChara->GetPos(), m_pOwner->GetPos() );

		REAL32 length = i3Vector::Normalize( &vDir, &vLength);

		if( length > rRange )
			continue;

		{
			REAL32 rate = MINMAX( 0.0f, 1.0f - (length / rRange ), 1.0f);

			pDamageInfo->_nKillerIdx		= m_pOwner->getCharaNetIndex();
			pDamageInfo->_nVictimIdx		= pChara->getCharaNetIndex();
			pDamageInfo->_pVecDir			= &vDir;
			pDamageInfo->_pVecPos			= m_pOwner->GetPos();
			pDamageInfo->_rDistance			= length;

			pDamageInfo->_rDamage			= rDamage * rate;
			pDamageInfo->_rEffectiveTime	= rEffectiveTime * rate;

			i3Vector::Copy( &pDamageInfo->_vStartPos, m_pOwner->GetPos() );
			i3Vector::Copy( &pDamageInfo->_vHitPos, pChara->GetPos() );
			pDamageInfo->_nVictimIdx		= pChara->getCharaNetIndex();

			pChara->OnHit( pDamageInfo );
		}

		if(dmgType == WEAPON::DAMAGE_BUFF)
			m_pOwner->HP_Recovery( GetDamage(false) );
	}

	if( m_pOwner->IsRex() )
		_SetTrexTrembleCamera(rRange, rSlope, rEffectiveTime);
}

/*virtual*/ bool WeaponDinoHowl::DinoAttackProcess(REAL32 rAnimTime, REAL32 rTimeScale)
{
	INT32 autoFireCount = getAutoFireCount();

	if( autoFireCount >= m_pWeaponInfo->GetAutoFireCount() )
		return false;

	bool isAttack = false;

	if( rAnimTime > GetDinoAttackTime(autoFireCount) )
	{
		ProcessAreaHitDino();
		_PlayFireEffectDino();
		autoFireCount++;

		isAttack = true;
	}
	
	setAutoFireCount(autoFireCount);

	return isAttack;
}