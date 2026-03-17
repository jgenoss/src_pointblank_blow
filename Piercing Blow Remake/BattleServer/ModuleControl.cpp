#include "pch.h"
#include "ModuleControl.h"

// ============================================================================
// ModuleControl - Admin/control server communication (stub implementation)
// Will be fully implemented when control server protocol is defined
// ============================================================================

ModuleControl::ModuleControl()
	: m_bConnected(false)
	, m_ui32IP(0)
	, m_ui16Port(0)
	, m_ui32LastConnectAttempt(0)
	, m_ui32LastSystemInfoSend(0)
{
}

ModuleControl::~ModuleControl()
{
	OnDestroy();
}

bool ModuleControl::OnCreate(uint32_t ui32IP, uint16_t ui16Port)
{
	m_ui32IP = ui32IP;
	m_ui16Port = ui16Port;
	// TODO: Create TCP socket and connect to control server
	return true;
}

void ModuleControl::OnDestroy()
{
	m_bConnected = false;
}

void ModuleControl::Update()
{
	// TODO: Check connection, reconnect if needed
	// Process incoming admin commands
	// Send periodic system info updates
}

bool ModuleControl::ServerInit(int32_t i32WorkIdx, int32_t i32Result)
{
	if (!m_bConnected) return false;
	// TODO: Send server init notification to control server
	return true;
}

bool ModuleControl::SendSystemInfo(uint32_t ui32RoomCount, uint32_t ui32PlayerCount,
									float fCPUUsage, uint32_t ui32MemoryMB)
{
	if (!m_bConnected) return false;
	// TODO: Build and send system info packet
	return true;
}
