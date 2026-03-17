#include "pch.h"
#include "GameObject.h"

GameObject::GameObject()
{
	Reset();
}

GameObject::~GameObject()
{
}

void GameObject::Reset()
{
	m_bActive = false;
	m_ui16SType = 0;
	m_i32NetIndex = -1;
	m_i32HP = 0;
	m_i32MaxHP = 0;
	m_fPos[0] = m_fPos[1] = m_fPos[2] = 0.0f;
	m_fRespawnTime = 0.0f;
	m_dwDestroyTime = 0;
	m_i8GroupIdx = -1;
	m_ui8WeaponType = 0;
	m_ui8UseItemCount = 0;
	memset(m_ui32UseItemID, 0, sizeof(m_ui32UseItemID));
	memset(m_ui32UseItemProbability, 0, sizeof(m_ui32UseItemProbability));
}

void GameObject::InitFromMapObject(const DS_GAME_OBJECT* pMapObj, int i32Index)
{
	if (!pMapObj)
		return;

	Reset();

	m_bActive = true;
	m_ui16SType = pMapObj->m_iSType;
	m_i32NetIndex = i32Index;
	m_i32HP = pMapObj->m_i16NowHP;
	m_i32MaxHP = pMapObj->m_i16MaxHP;
	m_fPos[0] = pMapObj->m_vCenterPos[0];
	m_fPos[1] = pMapObj->m_vCenterPos[1];
	m_fPos[2] = pMapObj->m_vCenterPos[2];
	m_fRespawnTime = pMapObj->m_r32RespawnTime;
	m_i8GroupIdx = pMapObj->m_i8GroupIdx;
	m_ui8WeaponType = pMapObj->m_eWeaponType;

	// Battle use items
	m_ui8UseItemCount = pMapObj->m_ui8UseItemCount;
	for (int i = 0; i < 5 && i < m_ui8UseItemCount; i++)
	{
		m_ui32UseItemID[i] = pMapObj->m_ui32UseItemID[i];
		m_ui32UseItemProbability[i] = pMapObj->m_ui32UseItemProbability[i];
	}
}

void GameObject::ApplyDamage(int i32Damage)
{
	if (!m_bActive || m_i32HP <= 0)
		return;

	m_i32HP -= i32Damage;
	if (m_i32HP <= 0)
	{
		m_i32HP = 0;
		Destroy();
	}
}

void GameObject::Destroy()
{
	m_i32HP = 0;
	m_dwDestroyTime = GetTickCount();
}

void GameObject::Respawn()
{
	m_i32HP = m_i32MaxHP;
	m_dwDestroyTime = 0;
}

bool GameObject::NeedsRespawn() const
{
	if (!m_bActive || m_i32HP > 0 || m_fRespawnTime <= 0.0f)
		return false;

	if (m_dwDestroyTime == 0)
		return false;

	DWORD dwElapsed = (GetTickCount() - m_dwDestroyTime) / 1000;
	return (dwElapsed >= (DWORD)m_fRespawnTime);
}

void GameObject::Update(DWORD dwNow)
{
	if (!m_bActive)
		return;

	// Check respawn timer for weapon boxes and item boxes
	if (IsWeaponBox() || IsUseItemBox())
	{
		if (NeedsRespawn())
		{
			Respawn();
		}
	}
}
