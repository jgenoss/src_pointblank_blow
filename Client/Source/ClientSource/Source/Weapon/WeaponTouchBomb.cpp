#include "pch.h"
#include "WeaponTouchBomb.h"
#include "MainWeapon_TouchBomb.h"

#include "WeaponFunction.h"

I3_CLASS_INSTANCE(WeaponTouchBomb);

MainWeapon * WeaponTouchBomb::CreateMainWeapon()
{
	MainWeapon_TouchBomb * p = new MainWeapon_TouchBomb(this);
	MEMDUMP_NEW(p, sizeof(MainWeapon_TouchBomb));
	return p;
}

WeaponTouchBomb::WeaponTouchBomb()
{
	for (INT32 i = 0; i < GRENADESEX_MAX; i++)
	{
		aiIdle[i] = -1;
		aiFire[i] = -1;
		aiLoadMag[i] = -1;
		aiLoadBullet[i] = -1;
	}
}

WeaponTouchBomb::~WeaponTouchBomb()
{
}

void WeaponTouchBomb::OnBindResource(void)
{
	WeaponGrenade::OnBindResource();

	// 남여를 나누는데 구분이 없는 무기라면 기본으로 돌립니다.
	aiIdle[GRENADESEX_MALE] = static_cast<INT8>(m_pAICtx->FindAIStateByName("Idle_Male"));
	aiIdle[GRENADESEX_FEMALE] = static_cast<INT8>(m_pAICtx->FindAIStateByName("Idle_Female"));

	aiFire[GRENADESEX_MALE] = static_cast<INT8>(m_pAICtx->FindAIStateByName("Fire_Male"));
	aiFire[GRENADESEX_FEMALE] = static_cast<INT8>(m_pAICtx->FindAIStateByName("Fire_Female"));

	aiLoadMag[GRENADESEX_MALE] = static_cast<INT8>(m_pAICtx->FindAIStateByName("LoadMag_Male"));
	aiLoadMag[GRENADESEX_FEMALE] = static_cast<INT8>(m_pAICtx->FindAIStateByName("LoadMag_Female"));

	aiLoadBullet[GRENADESEX_MALE] = static_cast<INT8>(m_pAICtx->FindAIStateByName("LoadBullet_Male"));
	aiLoadBullet[GRENADESEX_FEMALE] = static_cast<INT8>(m_pAICtx->FindAIStateByName("LoadBullet_Female"));

	if (aiIdle[GRENADESEX_MALE] == -1)
	{
		for (INT32 i = 0; i < GRENADESEX_MAX; i++)
		{
			aiIdle[i] = static_cast<INT8>(m_pAICtx->FindAIStateByName("Idle"));
		}
	}

	if (aiFire[GRENADESEX_MALE] == -1)
	{
		for (INT32 i = 0; i < GRENADESEX_MAX; i++)
		{
			aiFire[i] = static_cast<INT8>(m_pAICtx->FindAIStateByName("Fire"));
		}
	}

	if (aiLoadMag[GRENADESEX_MALE] == -1)
	{
		for (INT32 i = 0; i < GRENADESEX_MAX; i++)
		{
			aiLoadMag[i] = static_cast<INT8>(m_pAICtx->FindAIStateByName("LoadMag"));
		}
	}

	if (aiLoadBullet[GRENADESEX_MALE] == -1)
	{
		for (INT32 i = 0; i < GRENADESEX_MAX; i++)
		{
			aiLoadBullet[i] = static_cast<INT8>(m_pAICtx->FindAIStateByName("LoadBullet"));
		}
	}

	setAIIndex(WEAPON::AI_IDLE, aiIdle[GRENADESEX_MALE]);
}

void WeaponTouchBomb::FireReady(void)
{
	if (m_pOwner == nullptr) return;
	
	INT8 idx = -1;

	if (m_pOwner->isFemale())	idx = aiLoadBullet[GRENADESEX_FEMALE];
	else						idx = aiLoadBullet[GRENADESEX_MALE];

	WF::PlayWeaponAnim(this, idx);

	WeaponGrenade::FireReady();
}

void WeaponTouchBomb::_OnPlayWeaponIdleAnim(void)
{
	if (m_pOwner == nullptr) return;

	//1,3인칭 대기 애니메이션이 있는지 먼저 확인
	if ((getAIIndex(WEAPON::AI_IDLE1PV) != -1) && (getAIIndex(WEAPON::AI_IDLE3PV) != -1))
	{
		if (m_pOwner != NULL && m_pOwner->is1PV())
			WF::PlayWeaponAnim(this, getAIIndex(WEAPON::AI_IDLE1PV));
		else if (m_pOwner != NULL && m_pOwner->is3PV())
			WF::PlayWeaponAnim(this, getAIIndex(WEAPON::AI_IDLE3PV));
	}
	else
	{
		INT8 idx = -1;

		if (m_pOwner->isFemale())	idx = aiIdle[GRENADESEX_FEMALE];
		else						idx = aiIdle[GRENADESEX_MALE];

		setAIIndex(WEAPON::AI_IDLE, idx);
		WF::PlayWeaponAnim(this, idx);
	}
}

void WeaponTouchBomb::_OnPlayWeaponFireAnim(void)
{
	if (m_pOwner == nullptr) return;

	//1,3인칭 공격 애니메이션이 있는지 먼저 확인
	if ((getAIIndex(WEAPON::AI_FIRE1PV) != -1) && (getAIIndex(WEAPON::AI_FIRE3PV) != -1))
	{
		if (m_pOwner != NULL && m_pOwner->is1PV())
			WF::PlayWeaponAnim(this, getAIIndex(WEAPON::AI_FIRE1PV));
		else if (m_pOwner != NULL && m_pOwner->is3PV())
			WF::PlayWeaponAnim(this, getAIIndex(WEAPON::AI_FIRE3PV));
	}
	else
	{
		INT8 idx = -1;

		if (m_pOwner->isFemale())	idx = aiFire[GRENADESEX_FEMALE];
		else						idx = aiFire[GRENADESEX_MALE];

		setAIIndex(WEAPON::AI_FIRE, idx);
		WF::PlayWeaponAnim(this, idx);
	}
}
