#ifndef __BATTLEMEMBER_H__
#define __BATTLEMEMBER_H__

#pragma once
#include "BattleDef.h"

// Miembro de una battle room (port simplificado de CDediMember)
// Cada slot de la room tiene un BattleMember que representa a un jugador
class BattleMember
{
public:
	BattleMember();
	~BattleMember();

	// Lifecycle
	void		Init();
	bool		Join(int64_t i64UID, uint32_t ui32IP, uint16_t ui16Port, int i32Team, int i32Slot);
	void		Leave();

	// Queries
	bool		IsMember() const			{ return m_bActive; }
	bool		IsAlive() const				{ return m_bAlive; }
	bool		IsSameAddress(uint32_t ui32IP, uint16_t ui16Port) const;

	// Getters
	int64_t			GetUID() const			{ return m_i64UID; }
	uint32_t		GetIP() const			{ return m_ui32IP; }
	uint16_t		GetPort() const			{ return m_ui16Port; }
	int				GetTeam() const			{ return m_i32Team; }
	int				GetSlotIdx() const		{ return m_i32SlotIdx; }
	BattleMemberState GetState() const		{ return m_eState; }
	DWORD			GetLastPacketTime() const { return m_dwLastPacketTime; }

	// Stats
	int				GetKills() const		{ return m_i32Kills; }
	int				GetDeaths() const		{ return m_i32Deaths; }
	int				GetHeadshots() const	{ return m_i32Headshots; }

	// Setters
	void			SetState(BattleMemberState eState)	{ m_eState = eState; }
	void			SetAlive(bool bAlive)				{ m_bAlive = bAlive; }
	void			UpdateLastPacketTime()				{ m_dwLastPacketTime = GetTickCount(); }
	void			AddKill()							{ m_i32Kills++; }
	void			AddDeath()							{ m_i32Deaths++; }
	void			AddHeadshot()						{ m_i32Headshots++; }

	// Fill result
	void			FillPlayerResult(BattlePlayerResult* pResult) const;

private:
	bool				m_bActive;
	BattleMemberState	m_eState;
	int64_t				m_i64UID;
	uint32_t			m_ui32IP;
	uint16_t			m_ui16Port;
	int					m_i32Team;
	int					m_i32SlotIdx;

	// Stats de esta batalla
	int					m_i32Kills;
	int					m_i32Deaths;
	int					m_i32Headshots;

	// State
	bool				m_bAlive;
	DWORD				m_dwLastPacketTime;
};

#endif // __BATTLEMEMBER_H__
