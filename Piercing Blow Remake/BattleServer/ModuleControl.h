#ifndef __MODULECONTROL_H__
#define __MODULECONTROL_H__

#pragma once

// ============================================================================
// ModuleControl - Admin/control server communication
// Port of original CModuleControl from Dedication/Dedi/ModuleControl.h
// Receives admin commands: pause, kick, ban, server info requests
// Sends server status: player count, room count, performance stats
// ============================================================================

class ModuleControl
{
public:
	ModuleControl();
	~ModuleControl();

	bool OnCreate(uint32_t ui32IP, uint16_t ui16Port);
	void OnDestroy();

	bool IsConnected() const { return m_bConnected; }

	// Update (check connection, process commands)
	void Update();

	// Send server init status
	bool ServerInit(int32_t i32WorkIdx, int32_t i32Result);

	// Send system info (room count, player count, CPU, memory)
	bool SendSystemInfo(uint32_t ui32RoomCount, uint32_t ui32PlayerCount,
						 float fCPUUsage, uint32_t ui32MemoryMB);

private:
	bool		m_bConnected;
	uint32_t	m_ui32IP;
	uint16_t	m_ui16Port;
	uint32_t	m_ui32LastConnectAttempt;
	uint32_t	m_ui32LastSystemInfoSend;
};

#endif // __MODULECONTROL_H__
