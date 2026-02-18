#include "pch.h"
#include "WeaponNunchaku.h"
#include "WeaponFunction.h"

#include "MainWeapon_Nunchaku.h"

I3_CLASS_INSTANCE( WeaponNunchaku);

WeaponNunchaku::WeaponNunchaku()
{
	for(INT32 sexType = 0; sexType < ANIMATION_SEX_MAX; sexType++)
	{
		for(INT32 personType = 0; personType < ANIMATION_PERSON_MAX; personType++)
		{
			m_ai_Change[sexType][personType]	= -1;
			m_ai_Idle[sexType][personType]		= -1;

			m_ai_AttackA[sexType][personType]	= -1;
			m_ai_AttackB[sexType][personType]	= -1;
		}

		m_ai_IdleA[sexType] = -1;
		m_ai_IdleB[sexType] = -1;
	}
}

WeaponNunchaku::~WeaponNunchaku()
{

}

MainWeapon * WeaponNunchaku::CreateMainWeapon()
{
	MainWeapon_Nunchaku * p = new MainWeapon_Nunchaku(this);
	MEMDUMP_NEW(p, sizeof(MainWeapon_Nunchaku));
	return p;
}

void WeaponNunchaku::OnBindResource( void )
{
	WeaponKnife::OnBindResource();

	m_ai_Change[ANIMATION_MALE][ANIMATION_FIRST_PERSON] = (INT8) m_pAICtx->FindAIStateByName( "Change_Male_1PV");
	m_ai_Change[ANIMATION_MALE][ANIMATION_THIRD_PERSON] = (INT8) m_pAICtx->FindAIStateByName( "Change_Male_3PV");

	m_ai_Change[ANIMATION_FEMALE][ANIMATION_FIRST_PERSON] = (INT8) m_pAICtx->FindAIStateByName( "Change_FeMale_1PV");
	m_ai_Change[ANIMATION_FEMALE][ANIMATION_THIRD_PERSON] = (INT8) m_pAICtx->FindAIStateByName( "Change_FeMale_3PV");

	m_ai_Idle[ANIMATION_MALE][ANIMATION_FIRST_PERSON] = (INT8) m_pAICtx->FindAIStateByName( "Idle_Male_1PV");
	m_ai_Idle[ANIMATION_MALE][ANIMATION_THIRD_PERSON] = (INT8) m_pAICtx->FindAIStateByName( "Idle_Male_3PV");

	m_ai_Idle[ANIMATION_FEMALE][ANIMATION_FIRST_PERSON] = (INT8) m_pAICtx->FindAIStateByName( "Idle_FeMale_1PV");
	m_ai_Idle[ANIMATION_FEMALE][ANIMATION_THIRD_PERSON] = (INT8) m_pAICtx->FindAIStateByName( "Idle_FeMale_3PV");

	m_ai_AttackA[ANIMATION_MALE][ANIMATION_FIRST_PERSON] = (INT8) m_pAICtx->FindAIStateByName( "Attack_A_Male_1PV");
	m_ai_AttackA[ANIMATION_MALE][ANIMATION_THIRD_PERSON] = (INT8) m_pAICtx->FindAIStateByName( "Attack_A_Male_3PV");

	m_ai_AttackA[ANIMATION_FEMALE][ANIMATION_FIRST_PERSON] = (INT8) m_pAICtx->FindAIStateByName( "Attack_A_FeMale_1PV");
	m_ai_AttackA[ANIMATION_FEMALE][ANIMATION_THIRD_PERSON] = (INT8) m_pAICtx->FindAIStateByName( "Attack_A_FeMale_3PV");

	m_ai_AttackB[ANIMATION_MALE][ANIMATION_FIRST_PERSON] = (INT8) m_pAICtx->FindAIStateByName( "Attack_B_Male_1PV");
	m_ai_AttackB[ANIMATION_MALE][ANIMATION_THIRD_PERSON] = (INT8) m_pAICtx->FindAIStateByName( "Attack_B_Male_3PV");

	m_ai_AttackB[ANIMATION_FEMALE][ANIMATION_FIRST_PERSON] = (INT8) m_pAICtx->FindAIStateByName( "Attack_B_FeMale_1PV");
	m_ai_AttackB[ANIMATION_FEMALE][ANIMATION_THIRD_PERSON] = (INT8) m_pAICtx->FindAIStateByName( "Attack_B_FeMale_3PV");

	m_ai_IdleA[ANIMATION_MALE] = (INT8) m_pAICtx->FindAIStateByName( "UI_IdleA_Male");
	m_ai_IdleA[ANIMATION_FEMALE] = (INT8) m_pAICtx->FindAIStateByName( "UI_IdleA_FeMale");

	m_ai_IdleB[ANIMATION_MALE] = (INT8) m_pAICtx->FindAIStateByName( "UI_IdleB_Male");
	m_ai_IdleB[ANIMATION_FEMALE] = (INT8) m_pAICtx->FindAIStateByName( "UI_IdleB_FeMale");
}

void WeaponNunchaku::_OnPlayWeaponIdleAnim( void )
{
	if( m_pOwner != nullptr)
	{
		// 스키니 캐릭터는 본이 여자이므로 여자랑 동일하게 가져갑니다.
		INT32 sexType = m_pOwner->getDBInfo()->GetCharaSexType();
		if (sexType == CHARA::eSEX_TYPE_SKINY_MALE) sexType = CHARA::eSEX_TYPE_FEMALE;
		INT32 personType = static_cast<INT32>(m_pOwner->is3PV());

		setAIIndex(WEAPON::AI_IDLE, m_ai_Idle[sexType][ personType]);

		WF::PlayWeaponAnim(this, m_ai_Idle[sexType][ personType] );
	}
}

void WeaponNunchaku::_OnPlayWeaponFireAnim( void )
{
	if( m_pOwner != nullptr)
	{
		// 스키니 캐릭터는 본이 여자이므로 여자랑 동일하게 가져갑니다.
		INT32 sexType = m_pOwner->getDBInfo()->GetCharaSexType();
		if (sexType == CHARA::eSEX_TYPE_SKINY_MALE) sexType = CHARA::eSEX_TYPE_FEMALE;
		INT32 personType = static_cast<INT32>(m_pOwner->is3PV());

		setAIIndex(WEAPON::AI_FIRE, m_ai_AttackA[sexType][ personType]);

		WF::PlayWeaponAnim(this, m_ai_AttackA[sexType][ personType] );
	}
}

void WeaponNunchaku::_OnPlayWeaponSecondFireAnim( void )
{
	if( m_pOwner != nullptr)
	{
		INT32 sexType = m_pOwner->getDBInfo()->GetCharaSexType();
		if (sexType == CHARA::eSEX_TYPE_SKINY_MALE) sexType = CHARA::eSEX_TYPE_FEMALE;
		INT32 personType = static_cast<INT32>(m_pOwner->is3PV());

		setAIIndex(WEAPON::AI_SECFIRE, m_ai_AttackB[sexType][ personType]);

		WF::PlayWeaponAnim(this, m_ai_AttackB[sexType][ personType] );
	}
}

void WeaponNunchaku::PlayAnim_UI_IdleA( void )
{
	// V3는 Idle로 돌립니다.
	_OnPlayWeaponIdleAnim();
	//if( m_pOwner != nullptr)
	//{
	//	INT32 sexType = m_pOwner->getDBInfo()->GetCharaSexType();

	//	WF::PlayWeaponAnim(this, m_ai_IdleA[sexType] );
	//}
}

void WeaponNunchaku::PlayAnim_UI_IdleB( void )
{
	//if( m_pOwner != nullptr)
	//{
	//	INT32 sexType = m_pOwner->getDBInfo()->GetCharaSexType();

	//	WF::PlayWeaponAnim(this, m_ai_IdleB[sexType] );
	//}
	// V3는 Idle로 돌립니다.
	_OnPlayWeaponIdleAnim();
}

void WeaponNunchaku::OnPlayChangeAniImpl( REAL32 rTimeScale /*= 1.f*/ )
{
	if( m_pOwner != nullptr)
	{
		INT32 sexType = m_pOwner->getDBInfo()->GetCharaSexType();
		if (sexType == CHARA::eSEX_TYPE_SKINY_MALE) sexType = CHARA::eSEX_TYPE_FEMALE;
		INT32 personType = static_cast<INT32>(m_pOwner->is3PV());

		setAIIndex(WEAPON::AI_CHANGE, m_ai_Change[sexType][ personType]);

		WF::PlayWeaponAnim(this, m_ai_Change[sexType][ personType], rTimeScale);
	}
}

void WeaponNunchaku::OnEndFire(i3AIContext * pCtx, REAL32 tm)
{
}