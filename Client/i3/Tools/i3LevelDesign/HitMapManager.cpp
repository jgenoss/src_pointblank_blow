#include "pch.h"
#include "HitMapManager.h"
#include "GlobalVariable.h"
#include "i3Base/string/ext/itoa.h"

CHitMapManager::CHitMapManager()
{
	m_pMapInfo = NULL;
	m_pRoomList = NULL;
	m_i32RoomListCount = 0;
}


CHitMapManager::~CHitMapManager()
{
	if (m_pRoomList)
	{
		delete[] m_pRoomList;
		m_pRoomList = NULL;
	}

	delete m_pMapInfo;
	m_pMapInfo = NULL;

	std::map<UINT64, KILLERUSER>::iterator it;
	for (it = m_vecKillerUser.begin(); it != m_vecKillerUser.end(); ++it)
	{
		it->second.veckill.clear();
	}
	m_vecKillerUser.clear();
}

void CHitMapManager::Init()
{
	if (!m_pMapInfo)
	{
		m_pMapInfo = new CMapPlayInfo;

		char szFull[MAX_PATH], szBack[MAX_PATH];
		::GetCurrentDirectory(sizeof(szBack) - 1, szBack);
		sprintf(szFull, "File Name=%s\\HItmap.udl;", szBack);
		if (!m_pMapInfo->Create(szFull))
			AfxMessageBox( "HitMapDBConnect 실패");
	}

// 	m_WeaponRegistry.SetCodePage(I3_LANG_CODE_KOREAN);
// 
// 	char pchfildName[MAX_PATH];
// 	sprintf_s(pchfildName, "%s\\config\\Weapon.pef", g_pProfile->m_szResPath);
// 	INT32 nReadBytes = m_WeaponRegistry.LoadRegistrySet(pchfildName, REGISTRY_FILE_BINARY);
// 	if (nReadBytes == STREAM_ERR)
// 		AfxMessageBox("Weapon.pef 로드실패");

}

void CHitMapManager::Build(COleDateTime tmStart, COleDateTime tmEnd)
{
	m_tmStart = tmStart;
	m_tmEnd = tmEnd;
	if (m_pRoomList)
	{
		delete[] m_pRoomList;
		m_pRoomList = NULL;
	}

	m_i32RoomListCount = 0;

	m_pMapInfo->GetRoomList(&m_tmStart, &m_tmEnd, &m_i32RoomListCount, &m_pRoomList);
}

void CHitMapManager::GetKillList(UINT64 ui64BattleUID, WEAPON_SLOT_TYPE slotType)
{
	PLAY_KILL_LIST* pKillList;

	INT32 i32KillListCount = 0;
	std::map<UINT64, KILLERUSER>::iterator it;
	for (it = m_vecKillerUser.begin(); it != m_vecKillerUser.end(); ++it)
	{
		it->second.veckill.clear();
	}
	m_vecKillerUser.clear();

	m_pMapInfo->GetKillList(&m_tmStart, &m_tmEnd, ui64BattleUID, &i32KillListCount, &pKillList);

	for (int i = 0; i < i32KillListCount; i++)
	{
		int weapontype = GET_ITEM_TYPE(pKillList[i].m_ui32KillerWeaponID);
		weapontype -= 1;

		if (WEAPON_SLOT_COUNT == slotType)	//주무기, 보조무기, 근접무기, 던지는거 제외
		{
			if (weapontype >= WEAPON_SLOT_PRIMARY && weapontype <= WEAPON_SLOT_THROWING1)
				continue;
		}
		else if (WEAPON_SLOT_ALL == slotType)
		{

		}
		else
		{
			if (weapontype != slotType )
				continue;
		}

		std::map<UINT64, KILLERUSER>::iterator it = m_vecKillerUser.find(pKillList[i].m_ui64KillerUID);
		if (it == m_vecKillerUser.end())
		{
			KILLERUSER user;
			user.ui64KillerUID = pKillList[i].m_ui64KillerUID;
			user.veckill.push_back(pKillList[i]);
			m_vecKillerUser.insert(std::make_pair(pKillList[i].m_ui64KillerUID, user));
		}
		else
		{
			it->second.veckill.push_back(pKillList[i]);
		}
	}

	delete[] pKillList;
	
}