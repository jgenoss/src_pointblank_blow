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

private:
	void			ProcessReceivedPacket(const char* pData, int i32Size,
										 uint32_t ui32SenderIP, uint16_t ui16SenderPort);
	void			RelayPacket(BattleRoom* pRoom, const char* pData, int i32Size,
							   uint32_t ui32SenderIP, uint16_t ui16SenderPort);

private:
	bool			m_bInitialized;
	SOCKET			m_Socket;
	uint16_t		m_ui16BasePort;
	char			m_RecvBuffer[BATTLE_UDP_RECV_BUFFER];
};

#endif // __UDPRELAY_H__
