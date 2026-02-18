#include "pch.h"
#include "MainWeapon_FryingPan.h"
#include "WeaponFunction.h"

MainWeapon_FryingPan::MainWeapon_FryingPan(WeaponBase* p) : MainWeapon_MiniAxe(p),
	m_ai_IdleFemale(-1), m_ai_FireFemale(-1), m_ai_SecondaryFireFemale(-1)
{

}

#ifdef max	// std::numeric_limits<type>::max()를 사용하기 위해.
#undef max
#endif

void MainWeapon_FryingPan::OnBindResource()
{
	MainWeapon_MiniAxe::OnBindResource();

	i3AIContext* pAICtx = m_pWeaponBase->GetAIContext();

	if (pAICtx)
	{
		INT32 id = std::numeric_limits<INT32>::quiet_NaN();

		id = pAICtx->FindAIStateByName("Idle_Female");
		I3ASSERT(id < std::numeric_limits<INT8>::max());
		m_ai_IdleFemale = static_cast<INT8>(id);

		id = pAICtx->FindAIStateByName("Fire_Female");
		I3ASSERT(id < std::numeric_limits<INT8>::max());
		m_ai_FireFemale = static_cast<INT8>(id);

		id = pAICtx->FindAIStateByName("SecondaryFire_Female");
		I3ASSERT(id < std::numeric_limits<INT8>::max());
		m_ai_SecondaryFireFemale = static_cast<INT8>(id);
	}
}

void MainWeapon_FryingPan::OnIdleAnim()
{
	CGameCharaBase* pOwner = m_pWeaponBase->getOwner();
	if (pOwner)
	{
		INT8 ai; // = std::numeric_limits<INT8>::quiet_NaN();
		if ( pOwner->isFemale() )
			ai = m_ai_IdleFemale;
		else
			ai = m_pWeaponBase->getAIIndex(WEAPON::AI_IDLE);

		WF::PlayWeaponAnim(m_pWeaponBase, ai);
	}
}

void MainWeapon_FryingPan::OnFireAnim()
{
	CGameCharaBase* pOwner = m_pWeaponBase->getOwner();
	if (pOwner)
	{
		INT8 ai; // = std::numeric_limits<INT8>::quiet_NaN();
		if ( pOwner->isFemale() )
			ai = m_ai_FireFemale;
		else
			ai = m_pWeaponBase->getAIIndex(WEAPON::AI_FIRE);

		WF::PlayWeaponAnim(m_pWeaponBase, ai);
	}
}

void MainWeapon_FryingPan::OnSecondaryFireAnim()
{
	CGameCharaBase* pOwner = m_pWeaponBase->getOwner();
	if (pOwner)
	{
		INT8 ai; // = std::numeric_limits<INT8>::quiet_NaN();
		if (pOwner->isFemale() )
			ai = m_ai_SecondaryFireFemale;
		else
			ai = m_pWeaponBase->getAIIndex(WEAPON::AI_SECFIRE);

		WF::PlayWeaponAnim(m_pWeaponBase, ai);
	}
}