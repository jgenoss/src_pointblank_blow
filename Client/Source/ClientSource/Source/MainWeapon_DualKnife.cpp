#include "pch.h"
#include "MainWeapon_DualKnife.h"
#include "WeaponFunction.h"
#include "Weapon/WeaponItemID.h"

MainWeapon_DualKnife::MainWeapon_DualKnife(WeaponBase* p) : MainWeapon_Dual(p)
{
	for (INT32 i = 0; i < WEAPON_DUAL_HANDTYPE_MAX; i++)
	{
		m_aiiDualChange[i] = -1;

		m_aiiDualAttackA[i] = -1;
		m_aiiDualAttackB[i] = -1;

		m_aiiUI_RebelMale[i] = -1;
		m_aiiUI_RebelFemale[i] = -1;

		m_aiiUI_RebelMale2[i] = -1;
		m_aiiUI_RebelFemale2[i] = -1;
	}
	
	m_nAttackType		= WEAPON_KNIFE_ATTACK;
	m_HandType			= WEAPON_DUAL_HANDTYPE_RIGHT;
	m_iAttackSoundPlayCount	= 0;
}

void	MainWeapon_DualKnife::OnCreate( bool bCreateInWeapon)
{
	MainWeapon_Dual::OnCreate( bCreateInWeapon);
	
	// 쌍칼 사운드는 하나만 있으므로 다른 사운드 나는 것을 막는다.
	if (m_pLeftHandWeapon != nullptr)
	{
		m_pLeftHandWeapon->setEnablePlayFireSound(false);
		MainWeapon_DualKnife * mainWeapon_Left = static_cast<MainWeapon_DualKnife*>(m_pLeftHandWeapon->GetMainWeapon());

		if (mainWeapon_Left != nullptr)
		{
			mainWeapon_Left->SetHandType(WEAPON_DUAL_HANDTYPE_LEFT);
		}
	}
}

void	MainWeapon_DualKnife::OnInitVariable()
{
	MainWeapon_Dual::OnInitVariable();

	m_nAttackType	= WEAPON_KNIFE_ATTACK;	// 이 멤버변수는 일반 나이프클래스의 것과 같은것인데, 상속구조문제가 좋지 않아 복사코드가 되었음..
}

void	MainWeapon_DualKnife::OnBindResource() 
{
	MainWeapon_Dual::OnBindResource();

	m_aiiDualChange[WEAPON_DUAL_HANDTYPE_RIGHT]				= (INT8) m_pWeaponBase->GetAIContext()->FindAIStateByName( "Change_Right");
	m_aiiDualChange[WEAPON_DUAL_HANDTYPE_LEFT]				= (INT8) m_pWeaponBase->GetAIContext()->FindAIStateByName( "Change_Left");

	m_aiiDualAttackA[WEAPON_DUAL_HANDTYPE_RIGHT]			= (INT8) m_pWeaponBase->GetAIContext()->FindAIStateByName( "AttackA_Right");
	m_aiiDualAttackA[WEAPON_DUAL_HANDTYPE_LEFT]				= (INT8) m_pWeaponBase->GetAIContext()->FindAIStateByName( "AttackA_Left");

	m_aiiDualAttackB[WEAPON_DUAL_HANDTYPE_RIGHT]			= (INT8) m_pWeaponBase->GetAIContext()->FindAIStateByName( "AttackB_Right");
	m_aiiDualAttackB[WEAPON_DUAL_HANDTYPE_LEFT]				= (INT8) m_pWeaponBase->GetAIContext()->FindAIStateByName( "AttackB_Left");

	m_aiiUI_RebelMale[WEAPON_DUAL_HANDTYPE_RIGHT]		= (INT8) m_pWeaponBase->GetAIContext()->FindAIStateByName( "UI_RebelMale_Right");
	m_aiiUI_RebelMale[WEAPON_DUAL_HANDTYPE_LEFT]		= (INT8) m_pWeaponBase->GetAIContext()->FindAIStateByName( "UI_RebelMale_Left");

	m_aiiUI_RebelFemale[WEAPON_DUAL_HANDTYPE_RIGHT]		= (INT8) m_pWeaponBase->GetAIContext()->FindAIStateByName( "UI_RebelFemale_Right");
	m_aiiUI_RebelFemale[WEAPON_DUAL_HANDTYPE_LEFT]		= (INT8) m_pWeaponBase->GetAIContext()->FindAIStateByName( "UI_RebelFemale_Left");

	m_aiiUI_RebelMale2[WEAPON_DUAL_HANDTYPE_RIGHT]		= (INT8) m_pWeaponBase->GetAIContext()->FindAIStateByName( "UI_RebelMale_Right2");
	m_aiiUI_RebelMale2[WEAPON_DUAL_HANDTYPE_LEFT]		= (INT8) m_pWeaponBase->GetAIContext()->FindAIStateByName( "UI_RebelMale_Left2");

	m_aiiUI_RebelFemale2[WEAPON_DUAL_HANDTYPE_RIGHT]	= (INT8) m_pWeaponBase->GetAIContext()->FindAIStateByName( "UI_RebelFemale_Right2");
	m_aiiUI_RebelFemale2[WEAPON_DUAL_HANDTYPE_LEFT]		= (INT8) m_pWeaponBase->GetAIContext()->FindAIStateByName( "UI_RebelFemale_Left2");

}

void	MainWeapon_DualKnife::ReturnToReady()
{
	MainWeapon_Dual::ReturnToReady();
	
	m_nAttackType		= WEAPON_KNIFE_ATTACK;
}

void	MainWeapon_DualKnife::OnPlayChangeAni( REAL32 rTimeScale)
{
	if( g_pFramework->IsBattleStage() )
	{
		WF::PlayWeaponAnim(m_pWeaponBase, m_aiiDualChange[this->GetHandType()], rTimeScale );

		if( m_pLeftHandWeapon)
			m_pLeftHandWeapon->OnPlayChangeAni( rTimeScale);
	}
}

void	MainWeapon_DualKnife::NET_Fire_DualKnife( VEC3D *pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type)
{
	m_pWeaponBase->setWeaponState( WEAPON::STATE_FIRE);		//	공격!!
	
	m_pWeaponBase->SetFireOrder(type);

	//WeaponBase::NET_Fire( nullptr, nullptr, type );

	if( pStartPos != nullptr )
	{
		i3Vector::Copy( m_pWeaponBase->GetStartPos(), pStartPos );
		m_pWeaponBase->m_bSetStart	= true;
	}
	else
	{
		m_pWeaponBase->m_bSetStart	= false;
	}
	
	if( pTargetPos != nullptr )
		i3Vector::Copy( m_pWeaponBase->GetTargetPos(), pTargetPos);

	if( m_pWeaponBase->getOwner() != nullptr && m_pWeaponBase->getOwner()->isLocal() )
		m_pWeaponBase->addWeaponStateMask( WSM_FIRING | WSM_FIRE_DISABLE);
	m_pWeaponBase->removeWeaponStateMask( WSM_WAIT_AUTOFIREDELAY);

	if( (m_pWeaponBase->getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_KNUCKLE) &&
		(m_pWeaponBase->getWeaponInfo()->GetRealNumber() == WEAPON::getItemIndex(WEAPON::BRASS_KNUCKLE)) &&
		(m_pWeaponBase->getFireOrder() == WEAPON::SECONDARY_FIRE))
	{
		// 오른쪽 클릭 공격 사운드는 시작시 텀이 있다. OnFire 중에서 호출함.
	}
	else if (m_pWeaponBase->getWeaponInfo()->GetRealNumber() == WEAPON::getItemIndex(WEAPON::DUAL_KUNAI))
	{
		// Dual Kunai는 사운드 처리를 Dual Throw Knife에서 처리합니다.
	}
	else
	{
		m_pWeaponBase->_OnPlayFireSound();
	}

	if( type == WEAPON::PRIMARY_FIRE)
	{
		WF::PlayWeaponAnim(m_pWeaponBase, m_aiiDualAttackA[this->GetHandType()]);
	}
	else if( type == WEAPON::SECONDARY_FIRE)
	{
		WF::PlayWeaponAnim(m_pWeaponBase, m_aiiDualAttackB[this->GetHandType()]);
	}

	m_iAttackSoundPlayCount = 0;

	if(	m_pLeftHandWeapon)
		m_pLeftHandWeapon->NET_Fire( pStartPos, pTargetPos, type);

}
