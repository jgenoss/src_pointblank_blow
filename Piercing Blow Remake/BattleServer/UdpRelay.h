#ifndef __UDPRELAY_H__
#define __UDPRELAY_H__

#pragma once
#include "BattleDef.h"

class BattleRoom;
class BattleRoomManager;

// Subsistema UDP simplificado para relay de paquetes de juego
// v1: Un solo socket UDP en basePort. Los clientes envian paquetes con
// un UdpRelayHeader que indica roomIdx y slotIdx.
// El relay retransmite a todos los demas miembros del mismo BattleRoom.
//
// Referencia: Server/Source/Dedication/Dedi/DediUdpParser.h, DediUdpBuilder.h
class UdpRelay
{
public:
	UdpRelay();
	~UdpRelay();

	// Lifecycle
	bool			Initialize(uint16_t ui16BasePort);
	void			Update();
	void			Shutdown();

	// State
	bool			IsInitialized() const		{ return m_bInitialized; }
	uint16_t		GetPort() const				{ return m_ui16BasePort; }

	// UDP packet type IDs (from CommonDediCli.h)
	// These are the first 2 bytes after UdpRelayHeader in game packets
	enum UdpGamePacketType
	{
		UDP_PKT_CYCLEINFO		= 0x0001,	// Position/rotation update
		UDP_PKT_FIRE			= 0x0002,	// Weapon fire event
		UDP_PKT_HIT				= 0x0003,	// Hit notification
		UDP_PKT_DEATH			= 0x0004,	// Death event
		UDP_PKT_RESPAWN			= 0x0005,	// Respawn event
		UDP_PKT_ACTIONKEY		= 0x0006,	// Action key (C4, use object, etc.)
		UDP_PKT_WEAPON_CHANGE	= 0x0007,	// Weapon change
	};

private:
	void			ProcessReceivedPacket(const char* pData, int i32Size,
										 uint32_t ui32SenderIP, uint16_t ui16SenderPort);
	void			ParseGamePacket(BattleRoom* pRoom, BattleMember* pSender,
								   const char* pGameData, int i32GameDataSize);
	void			RelayPacket(BattleRoom* pRoom, const char* pData, int i32Size,
							   uint32_t ui32SenderIP, uint16_t ui16SenderPort);

private:
	bool			m_bInitialized;
	SOCKET			m_Socket;
	uint16_t		m_ui16BasePort;
	char			m_RecvBuffer[BATTLE_UDP_RECV_BUFFER];
};

#endif // __UDPRELAY_H__
