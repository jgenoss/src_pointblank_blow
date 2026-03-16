#ifndef __BATTLEROOM_H__
#define __BATTLEROOM_H__

#pragma once
#include "i3ElementBase.h"
#include "BattleDef.h"
#include "BattleMember.h"
#include "InterServerProtocol.h"

// Battle Room (port simplificado de CDediRoom)
// Gestiona una batalla: miembros por slot, state machine, timer, resultados
class BattleRoom : public i3ElementBase
{
	I3_CLASS_DEFINE(BattleRoom);

public:
	BattleRoom();
	virtual ~BattleRoom();

	// Lifecycle
	bool			Create(IS_BATTLE_CREATE_REQ* pReq, int i32RoomIdx, uint16_t ui16UdpPort, int i32SessionIdx);
	void			Reset();

	// Member management
	bool			AddMember(int64_t i64UID, uint32_t ui32IP, uint16_t ui16Port, int i32Team, int i32Slot);
	void			RemoveMember(int i32Slot);
	int				GetActiveMemberCount() const;
	BattleMember*	GetMember(int i32Slot);
	BattleMember*	FindMemberByUID(int64_t i64UID);
	BattleMember*	FindMemberByAddress(uint32_t ui32IP, uint16_t ui16Port);

	// State
	bool			IsActive() const				{ return m_eState != BATTLE_ROOM_INVALID; }
	BattleRoomState	GetState() const				{ return m_eState; }
	bool			ShouldDestroy() const;

	// Battle flow
	void			StartBattle();
	void			EndBattle(int i32WinnerTeam);
	void			Update();

	// Result
	void			FillBattleResult(BattleResult* pResult) const;

	// Getters
	int				GetRoomIdx() const				{ return m_i32RoomIdx; }
	int				GetGameServerRoomIdx() const		{ return m_i32GameServerRoomIdx; }
	int				GetChannelNum() const			{ return m_i32ChannelNum; }
	uint8_t			GetGameMode() const				{ return m_ui8GameMode; }
	uint8_t			GetMapIndex() const				{ return m_ui8MapIndex; }
	uint8_t			GetMaxPlayers() const			{ return m_ui8MaxPlayers; }
	uint16_t		GetUdpPort() const				{ return m_ui16UdpPort; }
	int				GetOwnerSessionIdx() const		{ return m_i32OwnerSessionIdx; }
	int				GetRedScore() const				{ return m_i32RedScore; }
	int				GetBlueScore() const			{ return m_i32BlueScore; }
	bool			IsEnding() const				{ return m_eState == BATTLE_ROOM_ENDING; }

	// Setters
	void			AddRedScore()					{ m_i32RedScore++; }
	void			AddBlueScore()					{ m_i32BlueScore++; }

private:
	BattleRoomState		m_eState;
	BattleMember		m_Members[BATTLE_SLOT_MAX];

	int					m_i32RoomIdx;				// Index en BattleRoomManager
	int					m_i32GameServerRoomIdx;		// Room index en el GameServer
	int					m_i32ChannelNum;			// Canal del GameServer
	uint8_t				m_ui8GameMode;				// STAGE_MODE
	uint8_t				m_ui8MapIndex;				// Map
	uint8_t				m_ui8MaxPlayers;			// Max jugadores
	int					m_i32PlayerCount;			// Jugadores esperados
	uint16_t			m_ui16UdpPort;				// Puerto UDP asignado

	DWORD				m_dwCreateTime;
	DWORD				m_dwBattleStartTime;
	DWORD				m_dwLastActivityTime;

	int					m_i32RedScore;
	int					m_i32BlueScore;
	int					m_i32OwnerSessionIdx;		// Session index del GameServer dueno
};

#endif // __BATTLEROOM_H__
