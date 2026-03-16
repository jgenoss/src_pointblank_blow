#ifndef __GAMECONTEXTMAIN_H__
#define __GAMECONTEXTMAIN_H__

#pragma once
#include "i3ElementBase.h"

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

public:
	// Server identity
	int			m_i32ServerId;
	char		m_szServerName[64];

	// Session/channel config
	int			m_i32SessionCount;
	uint8_t		m_ui8ChannelCount;
	uint16_t	m_ui16MaxUsersPerChannel;
	uint16_t	m_ui16MaxRoomsPerChannel;

	// RSA key (from S2MO)
	int16_t		m_i16RSAKeySize;
	int8_t		m_pRSAKey[512];

	// Timers
	int			m_i32UpdateTimeSS;
	int			m_i32LastUpdateTime;
};

extern GameContextMain* g_pContextMain;

#endif // __GAMECONTEXTMAIN_H__
