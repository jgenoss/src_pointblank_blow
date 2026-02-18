#pragma once

// 해당 내용은
// https://kloudio.sharepoint.com/PBENG/_layouts/15/WopiFrame.aspx?sourcedoc={FB5AE69D-DE8A-4DE4-BE40-948695199B07}&file=[PB]%20ZLogFormat.xlsx&action=default
// 여길 참조하세요

enum BATTLE_TEAM
{
	BATTLE_TEAM_RED				= 0,
	BATTLE_TEAM_BLUE,
	BATTLE_TEAM_DRAW,
};

enum IS_CLAN
{
	IS_CLAN_NOT					= 0,
	IS_CLAN_CLAN,
	IS_CLAN_CLANMATCH,
};

enum ROUND_WIN_REASON
{
	ROUND_WIN_REASON_WRONG		= 0,		// 비정상 종료
	ROUND_WIN_REASON_TIME,					// Time
	ROUND_WIN_REASON_EXPLOSION,				// 폭파
	ROUND_WIN_REASON_CLEAR,					// 해체
	ROUND_WIN_REASON_ALLDEATH,				// All Death
	ROUND_WIN_REASON_DESTROY,				// 파괴미션 파괴
	ROUND_WIN_REASON_DEFENCE,				// 방어미션 파괴
	ROUND_WIN_REASON_TUTORIAL,				// 튜토리얼 완료
	ROUND_WIN_REASON_KILL,					// 데스매치 킬수 달성
};

enum DEATH_TYPE
{
	DEATH_TYPE_NORMAL			= 1,		// 일반 적인 죽음 ( 총 )
	DEATH_TYPE_C4,
	DEATH_TYPE_HEADSHOT,
	DEATH_TYPE_SUICIDE,
	DEATH_TYPE_GRENADE,
};

namespace RoomSetting
{
	inline
	UINT32					GetBattleTimeToSec( UINT32 ui32Set )
	{
		switch ((ui32Set & 0xF0) >> 4)
		{
		case 0:		return 180;
		case 1:		return 300;
		case 2:		return 420;
		case 3:		return 300;
		case 4:		return 600;
		case 5:		return 900;
		case 6:		return 1200;
		case 7:		return 1500;
		case 8:		return 1800;
		case 9:		return 0x7FFF;			// 무제한
		}
		return 0x80000000;
	};

	inline
	UINT32					GetBattleKillCount( UINT32 ui32Set )
	{
		switch( ui32Set & 0x0F )
		{
		case 0:		return 60;
		case 1:		return 80;
		case 2:		return 100;
		case 3:		return 120;
		case 4:		return 140;
		case 5:		return 160;
		case 6:		return 0x7FFF;			// 무제한
		}
		return 0x80000000;
	};

	inline
	UINT32					GetBattleRoundCount( UINT32 ui32Set )
	{
		switch( ui32Set & 0x0F )
		{
		case 0:		return 3;
		case 1:		return 2;
		case 2:		return 3;
		case 3:		return 5;
		case 4:		return 7;
		case 5:		return 9;
		case 6:		return 0x7FFF;			// 무제한
		}
		return 0x80000000;
	};
};

// 현재 위치
struct LOCATION_INFO
{
	UINT8					m_ui8ServerIdx = 0;
	UINT8					m_ui8ChannelIdx = 0;
	UINT16					m_ui16RoomIdx = 0;
};

struct PLAY_ROOM_LIST
{
	DATE					m_dtTime;
	UINT64					m_ui64BattleUID = 0;
	UINT32					m_ui32MapID = 0;
	BATTLE_TEAM				m_eWinner;
	//STAGE_TYPE				m_eStageType;
	//STAGE_MASK_TYPE			m_eStageMask;
	IS_CLAN					m_eIsClan;
	COleDateTime			m_dtBattleStartDate;
	UINT32					m_ui32BattlePlayTimeToSec = 0;
	ROUND_WIN_REASON		m_eRoundWinReason;
	UINT32					m_ui32RoomSetting = 0;				// RoomSetting 값을 이용해서 얻어오세요
	LOCATION_INFO			m_Location;						// 방의 서버/채널/방 정보
};

struct PLAY_KILL_LIST
{
	DATE					m_dtTime;
	UINT64					m_ui64BattleUID = 0;
	UINT64					m_ui64KillerUID = 0;
	UINT64					m_ui64DeathUID = 0;
	UINT32					m_ui32MapID = 0;
	UINT32					m_ui32KillerWeaponID = 0;
	UINT16					m_ui16KillerRank = 0;
	UINT16					m_ui16DeathRank = 0;
	BATTLE_TEAM				m_eKillerTeam;
	BATTLE_TEAM				m_eDeathTeam;
	UINT32					m_ui32PlayTime = 0;
	DEATH_TYPE				m_eDeathType;
	D3DVECTOR				m_vec3KillPos = { 0, 0, 0 };
	D3DVECTOR				m_vec3DeathPos = { 0, 0, 0 };
};