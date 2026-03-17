#include "pch.h"
#include "UDPChecker.h"
#include "BattleRoom.h"
#include "GameCharacter.h"
#include "HMSParser.h"

// ============================================================================
// Main packet parsing entry point
// ============================================================================

int32_t UDPChecker::GamePacketParsing(BattleRoom* pRoom, const char* pPacket,
									   int32_t nSize, uint32_t ui32SlotIdx)
{
	if (!pRoom || !pPacket || nSize < (int32_t)sizeof(UDPPacketHeader))
		return -1;

	const UDPPacketHeader* pHeader = reinterpret_cast<const UDPPacketHeader*>(pPacket);
	const char* pData = pPacket + sizeof(UDPPacketHeader);
	int32_t nDataSize = nSize - (int32_t)sizeof(UDPPacketHeader);

	// Validate slot index
	if (ui32SlotIdx >= 16)
		return -1;

	PacketCheckResult eResult = PACKET_CHECK_OK;

	switch (pHeader->ui16Type)
	{
	case UDP_CYCLEINFO_POSITION:
		if (nDataSize >= (int32_t)sizeof(UDPPositionPacket))
			eResult = _ParseCharaPosRotPacket(pRoom, ui32SlotIdx,
				reinterpret_cast<const UDPPositionPacket*>(pData));
		else
			eResult = PACKET_CHECK_INVALID;
		break;

	case UDP_CYCLEINFO_CHARA_STATE:
		eResult = _ParseCharaState(pRoom, ui32SlotIdx, pData, nDataSize);
		break;

	case UDP_CYCLEINFO_FIRE:
		if (nDataSize >= (int32_t)sizeof(UDPFirePacket))
			eResult = _ParseFirePacket(pRoom, ui32SlotIdx,
				reinterpret_cast<const UDPFirePacket*>(pData));
		else
			eResult = PACKET_CHECK_INVALID;
		break;

	case UDP_CYCLEINFO_HIT_CHARA:
		if (nDataSize >= (int32_t)sizeof(UDPHitCharaPacket))
			eResult = _ParseHitPacketByChara(pRoom, ui32SlotIdx,
				reinterpret_cast<const UDPHitCharaPacket*>(pData));
		else
			eResult = PACKET_CHECK_INVALID;
		break;

	case UDP_CYCLEINFO_HIT_EXPLOSION:
		if (nDataSize >= (int32_t)sizeof(UDPHitExplosionPacket))
			eResult = _ParseHitPacketByExplosion(pRoom, ui32SlotIdx,
				reinterpret_cast<const UDPHitExplosionPacket*>(pData));
		else
			eResult = PACKET_CHECK_INVALID;
		break;

	case UDP_CYCLEINFO_HIT_OBJECT:
		if (nDataSize >= (int32_t)sizeof(UDPHitObjectPacket))
			eResult = _ParseHitPacketByObject(pRoom, ui32SlotIdx,
				reinterpret_cast<const UDPHitObjectPacket*>(pData));
		else
			eResult = PACKET_CHECK_INVALID;
		break;

	case UDP_CYCLEINFO_WEAPON_INFO:
		if (nDataSize >= (int32_t)sizeof(UDPWeaponInfoPacket))
			eResult = _ParseCharaWeaponInfo(pRoom, ui32SlotIdx,
				reinterpret_cast<const UDPWeaponInfoPacket*>(pData));
		else
			eResult = PACKET_CHECK_INVALID;
		break;

	case UDP_CYCLEINFO_WEAPON_PARAM:
		if (nDataSize >= (int32_t)sizeof(UDPWeaponParamPacket))
			eResult = _ParseCharaWeaponParam(pRoom, ui32SlotIdx,
				reinterpret_cast<const UDPWeaponParamPacket*>(pData));
		else
			eResult = PACKET_CHECK_INVALID;
		break;

	case UDP_CYCLEINFO_DROP_WEAPON:
		if (nDataSize >= (int32_t)sizeof(UDPDropWeaponPacket))
			eResult = _ParseDropWeaponPacket(pRoom, ui32SlotIdx,
				reinterpret_cast<const UDPDropWeaponPacket*>(pData));
		else
			eResult = PACKET_CHECK_INVALID;
		break;

	case UDP_CYCLEINFO_GET_WEAPON:
		if (nDataSize >= (int32_t)sizeof(UDPGetWeaponPacket))
			eResult = _ParseGetWeaponPacket(pRoom, ui32SlotIdx,
				reinterpret_cast<const UDPGetWeaponPacket*>(pData));
		else
			eResult = PACKET_CHECK_INVALID;
		break;

	case UDP_CYCLEINFO_DIRECT_PICKUP:
		if (nDataSize >= (int32_t)sizeof(UDPGetWeaponPacket))
			eResult = _ParseDirectPickUpPacket(pRoom, ui32SlotIdx,
				reinterpret_cast<const UDPGetWeaponPacket*>(pData));
		else
			eResult = PACKET_CHECK_INVALID;
		break;

	case UDP_CYCLEINFO_DROPPED_WEAPON:
		if (nDataSize >= (int32_t)sizeof(UDPDroppedWeaponPacket))
			eResult = _ParseDroppedWeaponPacket(pRoom, ui32SlotIdx,
				reinterpret_cast<const UDPDroppedWeaponPacket*>(pData));
		else
			eResult = PACKET_CHECK_INVALID;
		break;

	case UDP_CYCLEINFO_MISSION:
		if (nDataSize >= (int32_t)sizeof(UDPMissionPacket))
			eResult = _ParseMissionPacket(pRoom, ui32SlotIdx,
				reinterpret_cast<const UDPMissionPacket*>(pData));
		else
			eResult = PACKET_CHECK_INVALID;
		break;

	case UDP_CYCLEINFO_TAKING_OBJECT:
		eResult = _ParseCharaTakingObject(pRoom, ui32SlotIdx, pData, nDataSize);
		break;

	case UDP_CYCLEINFO_SYNC_OBJECT:
		eResult = _ParseCharaSyncObj(pRoom, ui32SlotIdx, pData, nDataSize);
		break;

	case UDP_CYCLEINFO_RADIO_CHAT:
		if (nDataSize >= (int32_t)sizeof(UDPRadioChatPacket))
			eResult = _ParseCharaRadioChat(pRoom, ui32SlotIdx,
				reinterpret_cast<const UDPRadioChatPacket*>(pData));
		else
			eResult = PACKET_CHECK_INVALID;
		break;

	case UDP_CYCLEINFO_SUICIDE_DMG:
		if (nDataSize >= (int32_t)sizeof(UDPSuicideDamagePacket))
			eResult = _ParseCharaSuicideDmg(pRoom, ui32SlotIdx,
				reinterpret_cast<const UDPSuicideDamagePacket*>(pData));
		else
			eResult = PACKET_CHECK_INVALID;
		break;

	default:
		// Unknown packet type - relay as-is
		break;
	}

	if (eResult == PACKET_CHECK_INVALID)
		return -1;

	return nSize;
}

// ============================================================================
// Individual packet parsers
// ============================================================================

PacketCheckResult UDPChecker::_ParseCharaState(BattleRoom* pRoom, uint32_t ui32SlotIdx,
												const char* pData, int32_t nSize)
{
	// Character state update (alive status, HP sync)
	GameCharacter* pChara = pRoom->GetCharacter(ui32SlotIdx);
	if (!pChara)
		return PACKET_CHECK_INVALID;

	// State packets are informational - just validate the slot exists
	return PACKET_CHECK_OK;
}

PacketCheckResult UDPChecker::_ParseCharaPosRotPacket(BattleRoom* pRoom, uint32_t ui32SlotIdx,
													   const UDPPositionPacket* pPosRot)
{
	GameCharacter* pChara = pRoom->GetCharacter(ui32SlotIdx);
	if (!pChara)
		return PACKET_CHECK_INVALID;

	if (!pChara->IsAlive())
		return PACKET_CHECK_IGNORE;

	// Validate position with HMSParser
	HackCheckResult hackResult = HMSParser::ValidatePosition(pRoom, ui32SlotIdx,
															  pPosRot->fPos, pPosRot->fTime);

	if (hackResult.eType != HACK_TYPE_NONE)
	{
		// Log hack but still update position (let the room decide action)
		// Ghost/speed hack detection is tracked per-character
	}

	// Update character position
	pChara->SetPosition(pPosRot->fPos[0], pPosRot->fPos[1], pPosRot->fPos[2]);
	pChara->SetRotation(pPosRot->fRot[0], pPosRot->fRot[1], pPosRot->fRot[2]);
	pChara->SetPosRecvTime(pPosRot->fTime);

	return PACKET_CHECK_OK;
}

PacketCheckResult UDPChecker::_ParseFirePacket(BattleRoom* pRoom, uint32_t ui32SlotIdx,
												const UDPFirePacket* pFire)
{
	GameCharacter* pChara = pRoom->GetCharacter(ui32SlotIdx);
	if (!pChara)
		return PACKET_CHECK_INVALID;

	if (!pChara->IsAlive())
		return PACKET_CHECK_IGNORE;

	// Check weapon sync
	HackType eHack = HMSParser::CheckWeaponSync(pRoom, ui32SlotIdx, pFire->ui32WeaponID);
	if (eHack != HACK_TYPE_NONE)
		return PACKET_CHECK_HACK;

	// Check fire speed
	FireInfo fireInfo;
	fireInfo.ui32SlotIdx = ui32SlotIdx;
	fireInfo.ui32WeaponID = pFire->ui32WeaponID;
	fireInfo.ui8AttackType = pFire->ui8AttackType;
	fireInfo.fPacketTime = pFire->fTime;

	eHack = HMSParser::CheckFireSpeed(pRoom, ui32SlotIdx, &fireInfo);

	// Update fire tracking
	pChara->SetLastFireInfo(pFire->fTime, pFire->ui32WeaponID,
							static_cast<AttackType>(pFire->ui8AttackType));

	return (eHack != HACK_TYPE_NONE) ? PACKET_CHECK_HACK : PACKET_CHECK_OK;
}

PacketCheckResult UDPChecker::_ParseHitPacketByChara(BattleRoom* pRoom, uint32_t ui32SlotIdx,
													  const UDPHitCharaPacket* pHit)
{
	GameCharacter* pAttacker = pRoom->GetCharacter(ui32SlotIdx);
	if (!pAttacker)
		return PACKET_CHECK_INVALID;

	if (!pAttacker->IsAlive())
		return PACKET_CHECK_IGNORE;

	// Build HitInfo for validation
	HitInfo hitInfo;
	hitInfo.ui32AttackerSlot = ui32SlotIdx;
	hitInfo.ui32TargetSlot = pHit->ui32TargetSlot;
	hitInfo.ui32WeaponID = pHit->ui32WeaponID;
	hitInfo.ui8AttackType = pHit->ui8AttackType;
	hitInfo.ui8HitPart = pHit->ui8HitPart;
	hitInfo.bExtension = (pHit->ui8Extension != 0);
	hitInfo.bHeadshot = (pHit->ui8Headshot != 0);
	memcpy(hitInfo.fAttackerPos, pHit->fAttackerPos, sizeof(float) * 3);
	memcpy(hitInfo.fTargetPos, pHit->fTargetPos, sizeof(float) * 3);
	memcpy(hitInfo.fDeathPos, pHit->fDeathPos, sizeof(float) * 3);

	// Full hit validation
	HackCheckResult hackResult = HMSParser::ValidateHit(pRoom, &hitInfo, 0.0f);
	if (hackResult.eType != HACK_TYPE_NONE)
		return PACKET_CHECK_HACK;

	// Update hit tracking
	pAttacker->IncHitCount();
	if (hitInfo.bHeadshot)
		pAttacker->IncCriticalHitCount();

	return PACKET_CHECK_OK;
}

PacketCheckResult UDPChecker::_ParseHitPacketByExplosion(BattleRoom* pRoom, uint32_t ui32SlotIdx,
														  const UDPHitExplosionPacket* pHit)
{
	GameCharacter* pAttacker = pRoom->GetCharacter(ui32SlotIdx);
	if (!pAttacker)
		return PACKET_CHECK_INVALID;

	// Validate target slot
	if (pHit->ui32TargetSlot >= 16)
		return PACKET_CHECK_INVALID;

	// Check weapon exists and has explosion range
	const WeaponDamageInfo* pInfo = WeaponTable::GetInstance().GetWeaponInfo(pHit->ui32WeaponID);
	if (!pInfo)
		return PACKET_CHECK_HACK;

	if (pInfo->fExplosionRange <= 0.0f)
		return PACKET_CHECK_HACK;	// Non-explosive weapon claiming explosion

	return PACKET_CHECK_OK;
}

PacketCheckResult UDPChecker::_ParseHitPacketByObject(BattleRoom* pRoom, uint32_t ui32SlotIdx,
													   const UDPHitObjectPacket* pHit)
{
	GameCharacter* pChara = pRoom->GetCharacter(ui32SlotIdx);
	if (!pChara)
		return PACKET_CHECK_INVALID;

	if (!pChara->IsAlive())
		return PACKET_CHECK_IGNORE;

	// Validate object index is within bounds
	if (pHit->ui32ObjectIdx >= 256)	// DS_MAX_OBJECT
		return PACKET_CHECK_INVALID;

	// Validate weapon
	HackType eHack = HMSParser::CheckWeaponSync(pRoom, ui32SlotIdx, pHit->ui32WeaponID);
	if (eHack != HACK_TYPE_NONE)
		return PACKET_CHECK_HACK;

	return PACKET_CHECK_OK;
}

PacketCheckResult UDPChecker::_ParseCharaWeaponInfo(BattleRoom* pRoom, uint32_t ui32SlotIdx,
													 const UDPWeaponInfoPacket* pWeapon)
{
	GameCharacter* pChara = pRoom->GetCharacter(ui32SlotIdx);
	if (!pChara)
		return PACKET_CHECK_INVALID;

	// Validate slot
	if (pWeapon->ui8Slot >= WEAPON_SLOT_COUNT)
		return PACKET_CHECK_INVALID;

	// Update equipped weapon tracking
	pChara->SetEquipWeapon(pWeapon->ui32WeaponID, pWeapon->ui8Slot);

	return PACKET_CHECK_OK;
}

PacketCheckResult UDPChecker::_ParseCharaWeaponParam(BattleRoom* pRoom, uint32_t ui32SlotIdx,
													  const UDPWeaponParamPacket* pParam)
{
	// Validate weapon parameters
	HackType eHack = HMSParser::CheckWeaponParam(pRoom, ui32SlotIdx,
												   pParam->ui32WeaponID, pParam->ui8AttackType);
	return (eHack != HACK_TYPE_NONE) ? PACKET_CHECK_HACK : PACKET_CHECK_OK;
}

PacketCheckResult UDPChecker::_ParseDropWeaponPacket(BattleRoom* pRoom, uint32_t ui32SlotIdx,
													  const UDPDropWeaponPacket* pDrop)
{
	GameCharacter* pChara = pRoom->GetCharacter(ui32SlotIdx);
	if (!pChara)
		return PACKET_CHECK_INVALID;

	if (pDrop->ui8Slot >= WEAPON_SLOT_COUNT)
		return PACKET_CHECK_INVALID;

	// Drop the weapon from character
	pChara->DropWeapon(pDrop->ui8Slot);

	return PACKET_CHECK_OK;
}

PacketCheckResult UDPChecker::_ParseGetWeaponPacket(BattleRoom* pRoom, uint32_t ui32SlotIdx,
													 const UDPGetWeaponPacket* pGet)
{
	GameCharacter* pChara = pRoom->GetCharacter(ui32SlotIdx);
	if (!pChara)
		return PACKET_CHECK_INVALID;

	if (pGet->ui8Slot >= WEAPON_SLOT_COUNT)
		return PACKET_CHECK_INVALID;

	// Check for RPG exploit
	uint8_t ui8Type = GET_ITEM_TYPE(pGet->ui32WeaponID);
	uint16_t ui16Number = GET_ITEM_NUMBER(pGet->ui32WeaponID);
	HackType eHack = HMSParser::CheckNaturalRPGGet(pRoom, ui32SlotIdx, ui8Type, ui16Number);
	if (eHack != HACK_TYPE_NONE)
		return PACKET_CHECK_HACK;

	// Pick up weapon
	pChara->PickUpWeapon(pGet->ui32WeaponID, pGet->ui8Slot);

	return PACKET_CHECK_OK;
}

PacketCheckResult UDPChecker::_ParseDirectPickUpPacket(BattleRoom* pRoom, uint32_t ui32SlotIdx,
														const UDPGetWeaponPacket* pPickup)
{
	// Same validation as regular pickup
	return _ParseGetWeaponPacket(pRoom, ui32SlotIdx, pPickup);
}

PacketCheckResult UDPChecker::_ParseDroppedWeaponPacket(BattleRoom* pRoom, uint32_t ui32SlotIdx,
														 const UDPDroppedWeaponPacket* pDropInfo)
{
	// Validate weapon exists
	const WeaponDamageInfo* pInfo = WeaponTable::GetInstance().GetWeaponInfo(pDropInfo->ui32WeaponID);
	if (!pInfo)
		return PACKET_CHECK_INVALID;

	// Bullet count sanity check
	if (pDropInfo->ui16LoadedBullets > pInfo->ui16LoadBullet * 2 ||
		pDropInfo->ui16ReserveBullets > pInfo->ui16MaxBullet * 2)
		return PACKET_CHECK_HACK;	// Suspicious bullet count

	return PACKET_CHECK_OK;
}

PacketCheckResult UDPChecker::_ParseMissionPacket(BattleRoom* pRoom, uint32_t ui32SlotIdx,
												   const UDPMissionPacket* pMission)
{
	GameCharacter* pChara = pRoom->GetCharacter(ui32SlotIdx);
	if (!pChara)
		return PACKET_CHECK_INVALID;

	if (!pChara->IsAlive())
		return PACKET_CHECK_IGNORE;

	// Mission packets (bomb install/uninstall, etc.) are processed by the room
	return PACKET_CHECK_OK;
}

PacketCheckResult UDPChecker::_ParseCharaTakingObject(BattleRoom* pRoom, uint32_t ui32SlotIdx,
													   const char* pData, int32_t nSize)
{
	GameCharacter* pChara = pRoom->GetCharacter(ui32SlotIdx);
	if (!pChara)
		return PACKET_CHECK_INVALID;

	if (!pChara->IsAlive())
		return PACKET_CHECK_IGNORE;

	// Object taking is validated by the room's object manager
	return PACKET_CHECK_OK;
}

PacketCheckResult UDPChecker::_ParseCharaSyncObj(BattleRoom* pRoom, uint32_t ui32SlotIdx,
												  const char* pData, int32_t nSize)
{
	// Object sync packets - validate slot exists
	GameCharacter* pChara = pRoom->GetCharacter(ui32SlotIdx);
	if (!pChara)
		return PACKET_CHECK_INVALID;

	return PACKET_CHECK_OK;
}

PacketCheckResult UDPChecker::_ParseCharaRadioChat(BattleRoom* pRoom, uint32_t ui32SlotIdx,
													const UDPRadioChatPacket* pRadio)
{
	// Radio chat - just validate slot
	GameCharacter* pChara = pRoom->GetCharacter(ui32SlotIdx);
	if (!pChara)
		return PACKET_CHECK_INVALID;

	// Validate radio type range
	if (pRadio->ui8RadioType > 10 || pRadio->ui8RadioIndex > 20)
		return PACKET_CHECK_INVALID;

	return PACKET_CHECK_OK;
}

PacketCheckResult UDPChecker::_ParseCharaSuicideDmg(BattleRoom* pRoom, uint32_t ui32SlotIdx,
													 const UDPSuicideDamagePacket* pSuicide)
{
	GameCharacter* pChara = pRoom->GetCharacter(ui32SlotIdx);
	if (!pChara)
		return PACKET_CHECK_INVALID;

	if (!pChara->IsAlive())
		return PACKET_CHECK_IGNORE;

	// Validate damage is reasonable
	if (pSuicide->i16Damage < 0 || pSuicide->i16Damage > 1000)
		return PACKET_CHECK_INVALID;

	return PACKET_CHECK_OK;
}
