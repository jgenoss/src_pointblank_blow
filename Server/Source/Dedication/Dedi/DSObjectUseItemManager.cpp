#include "pch.h"
#include "DSObjectUseItemManager.h"

CDSObjectUseItemManager::CDSObjectUseItemManager()
{
	m_i32ObjectCount = 0;
}

CDSObjectUseItemManager::~CDSObjectUseItemManager()
{

}

void CDSObjectUseItemManager::SetObjectList(CDSObject * objectList, UINT8* objectidxList)
{
	if (m_i32ObjectCount != 0)
	{
		// 오브젝트 넣기
		for (INT32 i = 0; i < m_i32ObjectCount; i++)
		{
			m_pObject[i] = &objectList[objectidxList[i]];
		}
	}
}

void CDSObjectUseItemManager::Reset()
{
	//m_i32ObjectCount = 0;

	CDSObject* pObject;

	// 모든 오브젝트 초기화
	for (INT32 i = 0; i < m_i32ObjectCount; i++)
	{
		// Timer를 작동합니다.
		m_pObject[i]->m_bTimerEnable			= TRUE;

		// Reset 시점은 라운드 시작 시점. StartTimer를 시작하며 스타트 타임을 설정합니다.
		m_pObject[i]->m_bItemStartTimerEnable	= FALSE;
		m_pObject[i]->m_ui32StartTime = (UINT32)GetTickCount();
		m_pObject[i]->setRespawnTime(0.f);
		m_pObject[i]->setState(OBJECT_USE_ITEM_STATE_OFF);
	}
}

void CDSObjectUseItemManager::Update()
{
	CDSObject* pObject;
	for (INT32 i = 0; i < m_i32ObjectCount; i++)
	{
		// 업데이트 시점
		pObject = m_pObject[i];

		if (FALSE == pObject->m_bTimerEnable)			continue;

		// 첫 리스폰인지 부터 체크합니다.
		REAL32 RespawnTime = 0.f;
		if (FALSE == pObject->m_bItemStartTimerEnable)
		{
			RespawnTime = pObject->m_r32UseItemStartTime;
		}
		else
		{
			RespawnTime = pObject->m_r32UseItemRespawnTime;
		}

		// 경과시간을 체크 합니다.
		pObject->setRespawnTime((REAL32)((GetTickCount() - pObject->m_ui32StartTime) * 0.001));

		// 시작 시간을 넘었는지 체크합니다.
		if (pObject->getRespawnTime() < RespawnTime) continue;

		// 시작 시간을 넘었을 경우 체크합니다.
		pObject->setEnterCharaIndex(0xFF);
		pObject->setState(OBJECT_USE_ITEM_STATE_ON);
		pObject->setRespawnTime(RespawnTime);

		pObject->m_bItemStartTimerEnable = TRUE;
		pObject->m_bTimerEnable = FALSE;
		pObject->m_bUDPSend = TRUE;
	}
}

bool CDSObjectUseItemManager::GetObjectInRPG7()
{
	CDSObject* pObject;
	for (INT32 i = 0; i < m_i32ObjectCount; i++)
	{
		// Timer를 작동합니다.
		INT32 count = m_pObject[i]->m_ui8UseItemCount;
		for (INT32 rpg = 0; rpg < count; rpg++)
		{
			if (m_pObject[i]->m_ui32UseItemID[rpg] == Weapon::GetItemID(WEAPON_CLASS_ROCKET_LAUNCHER, ROCKET_LAUNCHER_RPG7))
			{
				return true;
			}
		}
	}

	return false;
}
