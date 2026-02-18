#pragma once
#include "DB/MapPlayInfo.h"

struct KILLERUSER
{
	i3::vector<PLAY_KILL_LIST> veckill;
	UINT64					   ui64KillerUID = 0;
};

class CHitMapManager
{
	CMapPlayInfo*				m_pMapInfo;

	INT32						m_i32RoomListCount;
	PLAY_ROOM_LIST*				m_pRoomList;

	COleDateTime m_tmStart;
	COleDateTime m_tmEnd;

	std::map<UINT64, KILLERUSER>		m_vecKillerUser;

	i3RegistrySet					m_WeaponRegistry;
public:
	CHitMapManager();
	~CHitMapManager();

	void				Init();
	void				Build(COleDateTime tmStart, COleDateTime tmEnd);
	void				GetKillList(UINT64 ui64BattleUID, WEAPON_SLOT_TYPE slotType = WEAPON_SLOT_UNKNOWN);

	INT32				GetRoomListCount() {return m_i32RoomListCount;}
	PLAY_ROOM_LIST*		GetRoomList() { return m_pRoomList; }
	std::map<UINT64, KILLERUSER> GetKillerUserList() {
		return m_vecKillerUser; 
	}
};

