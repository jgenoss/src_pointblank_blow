#ifndef __GAMECONTEXTMAIN_H__
#define __GAMECONTEXTMAIN_H__

#pragma once
#include "i3ElementBase.h"

// Channel type enum (from CommonDef_System.h ChannelType)
enum GameChannelType
{
	GAME_CHANNEL_TYPE_NONE = 0,
	GAME_CHANNEL_TYPE_NORMAL,			// Normal channel
	GAME_CHANNEL_TYPE_BEGIN1,			// Beginner - Kill/Death restriction
	GAME_CHANNEL_TYPE_BEGIN2,			// Beginner - Ranking restriction
	GAME_CHANNEL_TYPE_CLAN,				// Clan channel
	GAME_CHANNEL_TYPE_EXPERT,			// Expert - Min level
	GAME_CHANNEL_TYPE_EXPERT2,			// Expert - Min rank
	GAME_CHANNEL_TYPE_CHAMPIONSHIP,		// Championship
	GAME_CHANNEL_TYPE_NORMAL2,			// Normal 2
	GAME_CHANNEL_TYPE_NATIONAL,			// National
	GAME_CHANNEL_TYPE_MIDDLE,			// Middle
	GAME_CHANNEL_TYPE_PCCAFE_SILVER,	// PC Cafe Silver
	GAME_CHANNEL_TYPE_PCCAFE_GOLD,		// PC Cafe Gold
	GAME_CHANNEL_TYPE_PCCAFE_CLAN,		// PC Cafe Clan
	GAME_CHANNEL_TYPE_MAX,
};

#define MAX_GAME_CHANNELS	20

// Per-channel configuration
struct GameChannelInfo
{
	GameChannelType	eType;
	char			szName[32];
	uint16_t		ui16MaxUsers;
	uint16_t		ui16CurrentUsers;
	int				i32MinLevel;		// Min level for expert channels
	int				i32MaxLevel;		// Max level for beginner channels

	void Reset()
	{
		eType = GAME_CHANNEL_TYPE_NORMAL;
		szName[0] = '\0';
		ui16MaxUsers = 200;
		ui16CurrentUsers = 0;
		i32MinLevel = 0;
		i32MaxLevel = 0;
	}
};

// Version simplificada de CContextMain del original
// Solo contiene datos globales necesarios para el GameServer remake
class GameContextMain : public i3ElementBase
{
	I3_CLASS_DEFINE(GameContextMain);

public:
	GameContextMain();
	~GameContextMain();

	BOOL	Create();
	void	OnUpdate();

	// Channel helpers
	const GameChannelInfo&	GetChannelInfo(int idx) const	{ return m_Channels[idx]; }
	GameChannelInfo&		GetChannelInfoMut(int idx)		{ return m_Channels[idx]; }

public:
	// Server identity
	int			m_i32ServerId;
	char		m_szServerName[64];

	// Session/channel config
	int			m_i32SessionCount;
	uint8_t		m_ui8ChannelCount;
	uint16_t	m_ui16MaxUsersPerChannel;
	uint16_t	m_ui16MaxRoomsPerChannel;

	// Per-channel info
	GameChannelInfo	m_Channels[MAX_GAME_CHANNELS];

	// RSA key (from S2MO)
	int16_t		m_i16RSAKeySize;
	int8_t		m_pRSAKey[512];

	// Timers
	int			m_i32UpdateTimeSS;
	int			m_i32LastUpdateTime;
};

extern GameContextMain* g_pContextMain;

#endif // __GAMECONTEXTMAIN_H__
