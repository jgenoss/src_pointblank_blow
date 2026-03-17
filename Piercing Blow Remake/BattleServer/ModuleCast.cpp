#include "pch.h"
#include "ModuleCast.h"

// ============================================================================
// ModuleCast - TCP communication to GameServer (stub implementation)
// Will be fully implemented when GameServer protocol integration is done
// ============================================================================

ModuleCast::ModuleCast()
	: m_bConnected(false)
	, m_ui32IP(0)
	, m_ui16Port(0)
	, m_ui32LastConnectAttempt(0)
	, m_ui32ReconnectInterval(5000)	// 5 seconds
{
}

ModuleCast::~ModuleCast()
{
	OnDestroy();
}

bool ModuleCast::OnCreate(uint32_t ui32IP, uint16_t ui16Port)
{
	m_ui32IP = ui32IP;
	m_ui16Port = ui16Port;
	// TODO: Create TCP socket and connect to GameServer
	return true;
}

void ModuleCast::OnDestroy()
{
	m_bConnected = false;
	// TODO: Close TCP socket
}

void ModuleCast::Update()
{
	// TODO: Check connection state, reconnect if needed
	// Process outgoing message queue
}

bool ModuleCast::Send_RoomCreateAck(int32_t i32WorkIdx, uint32_t ui32RoomIdx, int32_t i32Result)
{
	if (!m_bConnected) return false;
	// TODO: Build and send PROTOCOL_DSERVER_REGISTER_ROOM_ACK packet
	return true;
}

bool ModuleCast::Send_BattleKillAck(int32_t i32WorkIdx, const BattleRoomInfo* pRoomInfo,
									 const DeathInfoChara* pCharacters, uint8_t ui8Count,
									 uint8_t ui8RoundNum)
{
	if (!m_bConnected) return false;
	// TODO: Build and send PROTOCOL_BATTLE_KILL_ACK
	return true;
}

bool ModuleCast::Send_BattleRoundStart(int32_t i32WorkIdx, const BattleRoomInfo* pRoomInfo)
{
	if (!m_bConnected) return false;
	// TODO: Build and send PROTOCOL_BATTLE_RoundStart
	return true;
}

bool ModuleCast::Send_BattleRoundEnd(int32_t i32WorkIdx, const BattleRoomInfo* pRoomInfo,
									  uint8_t ui8RoundEndType, uint8_t ui8WinTeam)
{
	if (!m_bConnected) return false;
	// TODO: Build and send PROTOCOL_BATTLE_RoundEnd
	return true;
}

bool ModuleCast::Send_BombInstallAck(int32_t i32WorkIdx, const BattleRoomInfo* pRoomInfo,
									  uint32_t ui32SlotIdx)
{
	if (!m_bConnected) return false;
	// TODO: Build and send PROTOCOL_BATTLE_M_BOMB_INSTALL_ACK
	return true;
}

bool ModuleCast::Send_BombUninstallAck(int32_t i32WorkIdx, const BattleRoomInfo* pRoomInfo,
										uint32_t ui32SlotIdx)
{
	if (!m_bConnected) return false;
	// TODO: Build and send PROTOCOL_BATTLE_M_BOMB_UNINSTALL_ACK
	return true;
}

bool ModuleCast::Send_DestructionHPAck(int32_t i32WorkIdx, const BattleRoomInfo* pRoomInfo,
										uint32_t ui32ObjectIdx, int32_t i32HP)
{
	if (!m_bConnected) return false;
	// TODO: Build and send PROTOCOL_BATTLE_M_DESTRUCTION_HP_ACK
	return true;
}

bool ModuleCast::Send_DefenceAck(int32_t i32WorkIdx, const BattleRoomInfo* pRoomInfo,
								  uint32_t ui32ObjectIdx)
{
	if (!m_bConnected) return false;
	// TODO: Build and send PROTOCOL_BATTLE_M_DEFENCE_ACK
	return true;
}

bool ModuleCast::Send_TouchDown(int32_t i32WorkIdx, const BattleRoomInfo* pRoomInfo,
								 uint32_t ui32SlotIdx)
{
	if (!m_bConnected) return false;
	// TODO: Build and send PROTOCOL_BATTLE_TouchDown
	return true;
}

bool ModuleCast::Send_HackUserAck(int32_t i32WorkIdx, const BattleRoomInfo* pRoomInfo,
								   const BattleHackInfo* pUsers, uint8_t ui8Count)
{
	if (!m_bConnected) return false;
	// TODO: Build and send PROTOCOL_BATTLE_HACK_USER_ACK
	return true;
}

bool ModuleCast::Send_KickUserAck(int32_t i32WorkIdx, const BattleRoomInfo* pRoomInfo,
								   uint32_t ui32SlotIdx, int64_t i64UID)
{
	if (!m_bConnected) return false;
	// TODO: Build and send PROTOCOL_BATTLE_KICK_USER_BATTLE_ACK
	return true;
}

bool ModuleCast::Send_SlotEquipmentAck(int32_t i32WorkIdx, uint32_t ui32RoomIdx,
										uint32_t ui32SlotIdx, const uint32_t* pEquipIDs,
										uint8_t ui8Count)
{
	if (!m_bConnected) return false;
	// TODO: Build and send PROTOCOL_BATTLE_SLOT_EQUIPMENT_ACK
	return true;
}

bool ModuleCast::ServerInit(int32_t i32WorkIdx, int32_t i32Result)
{
	if (!m_bConnected) return false;
	// TODO: Build and send server init notification
	return true;
}
