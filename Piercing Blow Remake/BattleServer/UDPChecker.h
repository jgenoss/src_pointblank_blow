#ifndef __UDPCHECKER_H__
#define __UDPCHECKER_H__

#pragma once

// ============================================================================
// UDPChecker - UDP game packet validation with rate limiting
// Production port of original DediUdpChecker from Dedication/Dedi/DediUdpChecker.h
//
// Parses and validates all incoming UDP game packets before processing.
// Now includes:
// - Per-slot packet rate limiting (prevents flood attacks)
// - Enforcement integration (calls HMSParser::EnforceHack on violations)
// - Actual game state updates on valid packets (not just validation)
// - Damage calculation and application for hit packets
// ============================================================================

class BattleRoom;
class GameCharacter;

// Packet check result
enum PacketCheckResult
{
	PACKET_CHECK_OK = 0,
	PACKET_CHECK_INVALID,		// Malformed packet
	PACKET_CHECK_HACK,			// Hack detected
	PACKET_CHECK_IGNORE,		// Packet should be ignored (e.g., dead player firing)
	PACKET_CHECK_RATE_LIMITED,	// Rate limited - drop silently
};

// UDP game packet types (from CommonDediCli.h)
enum UDPPacketType
{
	UDP_CYCLEINFO_POSITION		= 0x0001,	// Position + rotation update
	UDP_CYCLEINFO_CHARA_STATE	= 0x0002,	// Character state (alive, HP)
	UDP_CYCLEINFO_FIRE			= 0x0003,	// Weapon fire event
	UDP_CYCLEINFO_HIT_CHARA		= 0x0004,	// Hit by character weapon
	UDP_CYCLEINFO_HIT_EXPLOSION	= 0x0005,	// Hit by explosion
	UDP_CYCLEINFO_HIT_OBJECT	= 0x0006,	// Hit by object
	UDP_CYCLEINFO_WEAPON_INFO	= 0x0007,	// Weapon change/equip
	UDP_CYCLEINFO_WEAPON_PARAM	= 0x0008,	// Weapon parameters
	UDP_CYCLEINFO_DROP_WEAPON	= 0x0009,	// Drop weapon
	UDP_CYCLEINFO_GET_WEAPON	= 0x000A,	// Pick up weapon
	UDP_CYCLEINFO_DIRECT_PICKUP	= 0x000B,	// Direct weapon pickup
	UDP_CYCLEINFO_DROPPED_WEAPON= 0x000C,	// Dropped weapon data
	UDP_CYCLEINFO_MISSION		= 0x000D,	// Mission event (bomb, etc.)
	UDP_CYCLEINFO_TAKING_OBJECT	= 0x000E,	// Taking/carrying object
	UDP_CYCLEINFO_SYNC_OBJECT	= 0x000F,	// Object sync
	UDP_CYCLEINFO_RADIO_CHAT	= 0x0010,	// Radio chat command
	UDP_CYCLEINFO_SUICIDE_DMG	= 0x0011,	// Self-damage (fall, etc.)
};

// Simplified packet structures for parsing
#pragma pack(push, 1)

struct UDPPacketHeader
{
	uint16_t	ui16Type;
	uint16_t	ui16Size;
	uint32_t	ui32SlotIdx;
};

struct UDPPositionPacket
{
	float		fPos[3];
	float		fRot[3];
	float		fTime;
};

struct UDPFirePacket
{
	uint32_t	ui32WeaponID;
	uint8_t		ui8AttackType;
	uint8_t		ui8Pad[3];
	float		fTime;
};

struct UDPHitCharaPacket
{
	uint32_t	ui32TargetSlot;
	uint32_t	ui32WeaponID;
	uint8_t		ui8AttackType;
	uint8_t		ui8HitPart;
	uint8_t		ui8Extension;
	uint8_t		ui8Headshot;
	float		fAttackerPos[3];
	float		fTargetPos[3];
	float		fDeathPos[3];
};

struct UDPHitExplosionPacket
{
	uint32_t	ui32TargetSlot;
	uint32_t	ui32WeaponID;
	float		fExplosionPos[3];
	float		fTargetPos[3];
};

struct UDPHitObjectPacket
{
	uint32_t	ui32ObjectIdx;
	uint32_t	ui32WeaponID;
	uint8_t		ui8AttackType;
	uint8_t		ui8Pad[3];
	int32_t		i32Damage;
};

struct UDPWeaponInfoPacket
{
	uint32_t	ui32WeaponID;
	uint8_t		ui8Slot;
	uint8_t		ui8Pad[3];
};

struct UDPWeaponParamPacket
{
	uint32_t	ui32WeaponID;
	uint8_t		ui8AttackType;
	uint8_t		ui8Pad[3];
};

struct UDPDropWeaponPacket
{
	uint8_t		ui8Slot;
	uint8_t		ui8Pad[3];
	float		fPos[3];
};

struct UDPGetWeaponPacket
{
	uint32_t	ui32WeaponID;
	uint8_t		ui8Slot;
	uint8_t		ui8Pad[3];
};

struct UDPDroppedWeaponPacket
{
	uint32_t	ui32WeaponID;
	uint16_t	ui16LoadedBullets;
	uint16_t	ui16ReserveBullets;
	float		fPos[3];
};

struct UDPMissionPacket
{
	uint8_t		ui8MissionType;
	uint8_t		ui8Param1;
	uint8_t		ui8Param2;
	uint8_t		ui8Pad;
	float		fPos[3];
};

struct UDPSuicideDamagePacket
{
	int16_t		i16Damage;
	uint8_t		ui8DamageType;	// 0=fall, 1=self-grenade, etc.
	uint8_t		ui8Pad;
};

struct UDPRadioChatPacket
{
	uint8_t		ui8RadioType;
	uint8_t		ui8RadioIndex;
	uint8_t		ui8Pad[2];
};

#pragma pack(pop)

// ============================================================================
// Rate limiter per-slot
// ============================================================================

#define RATE_LIMIT_WINDOW_MS		1000	// 1 second window
#define RATE_LIMIT_MAX_PACKETS		200		// Max packets per slot per second
#define RATE_LIMIT_MAX_POSITION		64		// Max position updates per second
#define RATE_LIMIT_MAX_FIRE			30		// Max fire packets per second
#define RATE_LIMIT_MAX_HIT			20		// Max hit packets per second

struct SlotRateLimit
{
	DWORD		dwWindowStart;
	int32_t		i32TotalPackets;
	int32_t		i32PositionPackets;
	int32_t		i32FirePackets;
	int32_t		i32HitPackets;

	void Reset()
	{
		dwWindowStart = 0;
		i32TotalPackets = 0;
		i32PositionPackets = 0;
		i32FirePackets = 0;
		i32HitPackets = 0;
	}

	// Check and update rate limit. Returns true if within limits.
	bool CheckAndUpdate(UDPPacketType eType, DWORD dwNow)
	{
		// Reset window if expired
		if (dwNow - dwWindowStart > RATE_LIMIT_WINDOW_MS)
		{
			dwWindowStart = dwNow;
			i32TotalPackets = 0;
			i32PositionPackets = 0;
			i32FirePackets = 0;
			i32HitPackets = 0;
		}

		i32TotalPackets++;
		if (i32TotalPackets > RATE_LIMIT_MAX_PACKETS)
			return false;

		switch (eType)
		{
		case UDP_CYCLEINFO_POSITION:
			i32PositionPackets++;
			return (i32PositionPackets <= RATE_LIMIT_MAX_POSITION);

		case UDP_CYCLEINFO_FIRE:
			i32FirePackets++;
			return (i32FirePackets <= RATE_LIMIT_MAX_FIRE);

		case UDP_CYCLEINFO_HIT_CHARA:
		case UDP_CYCLEINFO_HIT_EXPLOSION:
		case UDP_CYCLEINFO_HIT_OBJECT:
			i32HitPackets++;
			return (i32HitPackets <= RATE_LIMIT_MAX_HIT);

		default:
			return true;	// No specific limit for other types
		}
	}
};

class UDPChecker
{
public:
	// Main entry: parse and validate a UDP game packet
	// Returns bytes consumed, or -1 on error
	static int32_t GamePacketParsing(BattleRoom* pRoom, const char* pPacket,
									  int32_t nSize, uint32_t ui32SlotIdx);

	// Initialize rate limiters (call once at startup)
	static void Initialize();

	// Reset rate limiters for a slot (call when player leaves)
	static void ResetSlot(uint32_t ui32SlotIdx);

private:
	// Individual packet parsers
	static PacketCheckResult _ParseCharaState(BattleRoom* pRoom, uint32_t ui32SlotIdx,
											   const char* pData, int32_t nSize);

	static PacketCheckResult _ParseCharaPosRotPacket(BattleRoom* pRoom, uint32_t ui32SlotIdx,
													  const UDPPositionPacket* pPosRot);

	static PacketCheckResult _ParseFirePacket(BattleRoom* pRoom, uint32_t ui32SlotIdx,
											   const UDPFirePacket* pFire);

	static PacketCheckResult _ParseHitPacketByChara(BattleRoom* pRoom, uint32_t ui32SlotIdx,
													 const UDPHitCharaPacket* pHit);

	static PacketCheckResult _ParseHitPacketByExplosion(BattleRoom* pRoom, uint32_t ui32SlotIdx,
														 const UDPHitExplosionPacket* pHit);

	static PacketCheckResult _ParseHitPacketByObject(BattleRoom* pRoom, uint32_t ui32SlotIdx,
													  const UDPHitObjectPacket* pHit);

	static PacketCheckResult _ParseCharaWeaponInfo(BattleRoom* pRoom, uint32_t ui32SlotIdx,
												    const UDPWeaponInfoPacket* pWeapon);

	static PacketCheckResult _ParseCharaWeaponParam(BattleRoom* pRoom, uint32_t ui32SlotIdx,
													 const UDPWeaponParamPacket* pParam);

	static PacketCheckResult _ParseDropWeaponPacket(BattleRoom* pRoom, uint32_t ui32SlotIdx,
													 const UDPDropWeaponPacket* pDrop);

	static PacketCheckResult _ParseGetWeaponPacket(BattleRoom* pRoom, uint32_t ui32SlotIdx,
													const UDPGetWeaponPacket* pGet);

	static PacketCheckResult _ParseDirectPickUpPacket(BattleRoom* pRoom, uint32_t ui32SlotIdx,
													   const UDPGetWeaponPacket* pPickup);

	static PacketCheckResult _ParseDroppedWeaponPacket(BattleRoom* pRoom, uint32_t ui32SlotIdx,
														const UDPDroppedWeaponPacket* pDropInfo);

	static PacketCheckResult _ParseMissionPacket(BattleRoom* pRoom, uint32_t ui32SlotIdx,
												  const UDPMissionPacket* pMission);

	static PacketCheckResult _ParseCharaTakingObject(BattleRoom* pRoom, uint32_t ui32SlotIdx,
													  const char* pData, int32_t nSize);

	static PacketCheckResult _ParseCharaSyncObj(BattleRoom* pRoom, uint32_t ui32SlotIdx,
												 const char* pData, int32_t nSize);

	static PacketCheckResult _ParseCharaRadioChat(BattleRoom* pRoom, uint32_t ui32SlotIdx,
												   const UDPRadioChatPacket* pRadio);

	static PacketCheckResult _ParseCharaSuicideDmg(BattleRoom* pRoom, uint32_t ui32SlotIdx,
													const UDPSuicideDamagePacket* pSuicide);

	// Rate limiters per slot
	static SlotRateLimit s_RateLimits[BATTLE_SLOT_MAX];
};

#endif // __UDPCHECKER_H__
