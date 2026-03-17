#include "pch.h"
#include "UDPChecker.h"
#include "BattleRoom.h"
#include "BattleMember.h"
#include "GameCharacter.h"
#include "HMSParser.h"
#include "GameObjectManager.h"
#include "PhysicsEngine.h"

// Static rate limiters
SlotRateLimit UDPChecker::s_RateLimits[BATTLE_SLOT_MAX];

void UDPChecker::Initialize()
{
	for (int i = 0; i < BATTLE_SLOT_MAX; i++)
		s_RateLimits[i].Reset();
}

void UDPChecker::ResetSlot(uint32_t ui32SlotIdx)
{
	if (ui32SlotIdx < BATTLE_SLOT_MAX)
		s_RateLimits[ui32SlotIdx].Reset();
}

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
	if (ui32SlotIdx >= BATTLE_SLOT_MAX)
		return -1;

	// Rate limiting check
	DWORD dwNow = GetTickCount();
	UDPPacketType eType = (UDPPacketType)pHeader->ui16Type;

	if (!s_RateLimits[ui32SlotIdx].CheckAndUpdate(eType, dwNow))
	{
		// Rate limited - log and drop
		// Only log occasionally to avoid log spam
		if (s_RateLimits[ui32SlotIdx].i32TotalPackets == RATE_LIMIT_MAX_PACKETS + 1)
		{
			printf("[UDPChecker] Rate limit hit: slot %u type=0x%04X count=%d\n",
				ui32SlotIdx, pHeader->ui16Type, s_RateLimits[ui32SlotIdx].i32TotalPackets);

			// Report as potential hack if severely over limit
			if (s_RateLimits[ui32SlotIdx].i32TotalPackets > RATE_LIMIT_MAX_PACKETS * 2)
			{
				HackCheckResult hackResult;
				hackResult.eType = HACK_TYPE_BULLET_HACK;	// Repurpose as flood detection
				hackResult.eSeverity = HACK_SEVERITY_MEDIUM;
				hackResult.ui32SlotIdx = ui32SlotIdx;
				snprintf(hackResult.szDescription, sizeof(hackResult.szDescription),
						 "PacketFlood: slot %u rate=%d/s", ui32SlotIdx,
						 s_RateLimits[ui32SlotIdx].i32TotalPackets);

				BattleMember* pMember = pRoom->GetMember(ui32SlotIdx);
				int64_t uid = pMember ? pMember->GetUID() : 0;
				HMSParser::EnforceHack(pRoom, &hackResult, uid);
			}
		}
		return nSize;	// Consume but don't process
	}

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
	GameCharacter* pChara = pRoom->GetCharacter(ui32SlotIdx);
	if (!pChara)
		return PACKET_CHECK_INVALID;

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
		// Enforce the hack
		BattleMember* pMember = pRoom->GetMember(ui32SlotIdx);
		int64_t uid = pMember ? pMember->GetUID() : 0;
		HMSParser::EnforceHack(pRoom, &hackResult, uid);

		// Still update position even on hack (prevents desync)
		// The enforcement system will remove the player if severity is HIGH+
	}

	// Update character position and member position
	pChara->SetPosition(pPosRot->fPos[0], pPosRot->fPos[1], pPosRot->fPos[2]);
	pChara->SetRotation(pPosRot->fRot[0], pPosRot->fRot[1], pPosRot->fRot[2]);
	pChara->SetPosRecvTime(pPosRot->fTime);

	// Also update BattleMember position for relay system
	BattleMember* pMember = pRoom->GetMember(ui32SlotIdx);
	if (pMember)
		pMember->SetPosition(pPosRot->fPos[0], pPosRot->fPos[1], pPosRot->fPos[2]);

	// Update physics capsule position for accurate hit detection
	PhysicsEngine* pPhysics = pRoom->GetPhysicsEngine();
	if (pPhysics)
		pPhysics->UpdateCharacterPosition(ui32SlotIdx, pPosRot->fPos);

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
	{
		HackCheckResult result;
		result.eType = eHack;
		result.eSeverity = HACK_SEVERITY_HIGH;
		result.ui32SlotIdx = ui32SlotIdx;
		snprintf(result.szDescription, sizeof(result.szDescription),
				 "Fire: weapon sync fail weapon=%u", pFire->ui32WeaponID);
		BattleMember* pMember = pRoom->GetMember(ui32SlotIdx);
		HMSParser::EnforceHack(pRoom, &result, pMember ? pMember->GetUID() : 0);
		return PACKET_CHECK_HACK;
	}

	// Check fire speed
	FireInfo fireInfo;
	fireInfo.ui32SlotIdx = ui32SlotIdx;
	fireInfo.ui32WeaponID = pFire->ui32WeaponID;
	fireInfo.ui8AttackType = pFire->ui8AttackType;
	fireInfo.fPacketTime = pFire->fTime;

	eHack = HMSParser::CheckFireSpeed(pRoom, ui32SlotIdx, &fireInfo);
	if (eHack != HACK_TYPE_NONE)
	{
		HackCheckResult result;
		result.eType = eHack;
		result.eSeverity = HACK_SEVERITY_MEDIUM;
		result.ui32SlotIdx = ui32SlotIdx;
		snprintf(result.szDescription, sizeof(result.szDescription),
				 "FireSpeed: weapon=%u count=%u", pFire->ui32WeaponID, pChara->GetFireCount());
		BattleMember* pMember = pRoom->GetMember(ui32SlotIdx);
		HMSParser::EnforceHack(pRoom, &result, pMember ? pMember->GetUID() : 0);
	}

	// Decrease bullets
	WeaponSlotType eSlot = WeaponTable::GetSlotFromItemID(pFire->ui32WeaponID);
	const WeaponDamageInfo* pWeaponInfo = WeaponTable::GetInstance().GetWeaponInfo(pFire->ui32WeaponID);
	if (pWeaponInfo && eSlot != WEAPON_SLOT_MELEE)
	{
		if (!pChara->DecreaseBullet((uint8_t)eSlot, pWeaponInfo->ui8FireDecBulletCount))
		{
			// Out of ammo - potential bullet hack if they keep firing
			// Track but don't immediately flag (could be lag)
		}
	}

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

	// Validate target
	if (pHit->ui32TargetSlot >= BATTLE_SLOT_MAX)
		return PACKET_CHECK_INVALID;

	GameCharacter* pTarget = pRoom->GetCharacter(pHit->ui32TargetSlot);
	if (!pTarget || !pTarget->IsAlive())
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
	{
		BattleMember* pMember = pRoom->GetMember(ui32SlotIdx);
		HMSParser::EnforceHack(pRoom, &hackResult, pMember ? pMember->GetUID() : 0);
		return PACKET_CHECK_HACK;
	}

	// === VALID HIT - Apply damage ===
	const WeaponDamageInfo* pWeaponInfo = WeaponTable::GetInstance().GetWeaponInfo(pHit->ui32WeaponID);
	if (pWeaponInfo)
	{
		// Calculate damage
		bool bCritical = hitInfo.bHeadshot;
		float fPartRate = WeaponDamageInfo::GetDamageRateByHitPart((CharaHitPart)hitInfo.ui8HitPart);
		float fDefRate = pTarget->GetDefenceRate((CharaHitPart)hitInfo.ui8HitPart);

		int baseDmg = pWeaponInfo->CalculateDamage(
			(AttackType)hitInfo.ui8AttackType, bCritical, 0.0f);
		int finalDmg = (int)(baseDmg * fPartRate * fDefRate);
		if (finalDmg < 1) finalDmg = 1;

		// Apply damage
		int32_t remainHP = pTarget->SubHP((int16_t)finalDmg);

		if (remainHP <= 0)
		{
			// Target killed
			pTarget->SetAlive(false);
			pTarget->SetHP(0);

			// Update member state
			BattleMember* pTargetMember = pRoom->GetMember(pHit->ui32TargetSlot);
			if (pTargetMember)
			{
				pTargetMember->OnDeath();
			}

			// Update attacker stats
			BattleMember* pAttackerMember = pRoom->GetMember(ui32SlotIdx);
			if (pAttackerMember)
			{
				pAttackerMember->AddKill();
				if (hitInfo.bHeadshot)
					pAttackerMember->AddHeadshot();

				// Update team scores
				if (pAttackerMember->GetTeam() == BATTLE_TEAM_RED)
					pRoom->AddRedScore();
				else
					pRoom->AddBlueScore();
			}

			// Send kill notification to GameServer
			extern BattleSessionManager* g_pBattleSessionManager;
			if (g_pBattleSessionManager)
			{
				BattleSession* pSession = (BattleSession*)g_pBattleSessionManager->GetSession(
					pRoom->GetOwnerSessionIdx());
				if (pSession)
				{
					IS_BATTLE_KILL_INFO killInfo;
					killInfo.ui32KillerSlot = ui32SlotIdx;
					killInfo.ui32VictimSlot = pHit->ui32TargetSlot;
					killInfo.i64KillerUID = pAttackerMember ? pAttackerMember->GetUID() : 0;
					killInfo.i64VictimUID = pTargetMember ? pTargetMember->GetUID() : 0;
					killInfo.ui32WeaponID = pHit->ui32WeaponID;
					killInfo.ui8HitPart = pHit->ui8HitPart;
					killInfo.ui8Headshot = pHit->ui8Headshot;
					killInfo.ui8Pad[0] = 0;
					killInfo.ui8Pad[1] = 0;

					pSession->SendKillNotify(
						pRoom->GetGameServerRoomIdx(), pRoom->GetChannelNum(),
						0, &killInfo, 1);
				}
			}
		}
	}

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

	if (pHit->ui32TargetSlot >= BATTLE_SLOT_MAX)
		return PACKET_CHECK_INVALID;

	const WeaponDamageInfo* pInfo = WeaponTable::GetInstance().GetWeaponInfo(pHit->ui32WeaponID);
	if (!pInfo)
		return PACKET_CHECK_HACK;

	if (pInfo->fExplosionRange <= 0.0f)
		return PACKET_CHECK_HACK;

	// Calculate explosion damage based on distance
	GameCharacter* pTarget = pRoom->GetCharacter(pHit->ui32TargetSlot);
	if (pTarget && pTarget->IsAlive())
	{
		float dx = pHit->fExplosionPos[0] - pHit->fTargetPos[0];
		float dy = pHit->fExplosionPos[1] - pHit->fTargetPos[1];
		float dz = pHit->fExplosionPos[2] - pHit->fTargetPos[2];
		float dist = sqrtf(dx * dx + dy * dy + dz * dz);

		float falloff = 1.0f - (dist / pInfo->fExplosionRange);
		if (falloff < 0.0f) falloff = 0.0f;
		if (falloff > 1.0f) falloff = 1.0f;

		int baseDmg = pInfo->CalculateDamage(ATTACK_TYPE_L1, false, 0.0f);
		int finalDmg = (int)(baseDmg * falloff);
		if (finalDmg > 0)
		{
			int32_t remainHP = pTarget->SubHP((int16_t)finalDmg);
			if (remainHP <= 0)
			{
				pTarget->SetAlive(false);
				pTarget->SetHP(0);
				BattleMember* pTargetMember = pRoom->GetMember(pHit->ui32TargetSlot);
				if (pTargetMember)
					pTargetMember->OnDeath();

				BattleMember* pAttackerMember = pRoom->GetMember(ui32SlotIdx);
				if (pAttackerMember)
				{
					pAttackerMember->AddKill();
					if (pAttackerMember->GetTeam() == BATTLE_TEAM_RED)
						pRoom->AddRedScore();
					else
						pRoom->AddBlueScore();
				}
			}
		}
	}

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

	if (pHit->ui32ObjectIdx >= 256)
		return PACKET_CHECK_INVALID;

	HackType eHack = HMSParser::CheckWeaponSync(pRoom, ui32SlotIdx, pHit->ui32WeaponID);
	if (eHack != HACK_TYPE_NONE)
	{
		HackCheckResult result;
		result.eType = eHack;
		result.eSeverity = HACK_SEVERITY_HIGH;
		result.ui32SlotIdx = ui32SlotIdx;
		snprintf(result.szDescription, sizeof(result.szDescription),
				 "ObjHit: weapon sync fail weapon=%u", pHit->ui32WeaponID);
		BattleMember* pMember = pRoom->GetMember(ui32SlotIdx);
		HMSParser::EnforceHack(pRoom, &result, pMember ? pMember->GetUID() : 0);
		return PACKET_CHECK_HACK;
	}

	// Apply damage to game object
	GameObjectManager* pObjMgr = pRoom->GetObjectManager();
	if (pObjMgr)
		pObjMgr->ApplyDamageToObject(pHit->ui32ObjectIdx, pHit->i32Damage);

	return PACKET_CHECK_OK;
}

PacketCheckResult UDPChecker::_ParseCharaWeaponInfo(BattleRoom* pRoom, uint32_t ui32SlotIdx,
													 const UDPWeaponInfoPacket* pWeapon)
{
	GameCharacter* pChara = pRoom->GetCharacter(ui32SlotIdx);
	if (!pChara)
		return PACKET_CHECK_INVALID;

	if (pWeapon->ui8Slot >= WEAPON_SLOT_COUNT)
		return PACKET_CHECK_INVALID;

	pChara->SetEquipWeapon(pWeapon->ui32WeaponID, pWeapon->ui8Slot);

	// Also update member weapon tracking
	BattleMember* pMember = pRoom->GetMember(ui32SlotIdx);
	if (pMember)
		pMember->SetWeaponID(pWeapon->ui32WeaponID);

	return PACKET_CHECK_OK;
}

PacketCheckResult UDPChecker::_ParseCharaWeaponParam(BattleRoom* pRoom, uint32_t ui32SlotIdx,
													  const UDPWeaponParamPacket* pParam)
{
	HackType eHack = HMSParser::CheckWeaponParam(pRoom, ui32SlotIdx,
												   pParam->ui32WeaponID, pParam->ui8AttackType);
	if (eHack != HACK_TYPE_NONE)
	{
		HackCheckResult result;
		result.eType = eHack;
		result.eSeverity = HACK_SEVERITY_MEDIUM;
		result.ui32SlotIdx = ui32SlotIdx;
		snprintf(result.szDescription, sizeof(result.szDescription),
				 "WeaponParam: weapon=%u attack=%u", pParam->ui32WeaponID, pParam->ui8AttackType);
		BattleMember* pMember = pRoom->GetMember(ui32SlotIdx);
		HMSParser::EnforceHack(pRoom, &result, pMember ? pMember->GetUID() : 0);
		return PACKET_CHECK_HACK;
	}
	return PACKET_CHECK_OK;
}

PacketCheckResult UDPChecker::_ParseDropWeaponPacket(BattleRoom* pRoom, uint32_t ui32SlotIdx,
													  const UDPDropWeaponPacket* pDrop)
{
	GameCharacter* pChara = pRoom->GetCharacter(ui32SlotIdx);
	if (!pChara)
		return PACKET_CHECK_INVALID;

	if (pDrop->ui8Slot >= WEAPON_SLOT_COUNT)
		return PACKET_CHECK_INVALID;

	// Add to dropped weapon manager with current bullet state
	const WeaponInstance* pWeapon = pChara->GetWeapon(pDrop->ui8Slot);
	if (pWeapon && pWeapon->IsValid())
	{
		DroppedWeaponMgr* pDropMgr = pRoom->GetDroppedWeaponMgr();
		if (pDropMgr)
		{
			float fGameTime = (float)GetTickCount() / 1000.0f;
			pDropMgr->AddDroppedWeapon(ui32SlotIdx, pWeapon->ui32ItemID,
									   pDrop->fPos, pWeapon->ui16LoadedBullets,
									   pWeapon->ui16ReserveBullets, fGameTime);
		}
	}

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

	uint8_t ui8Type = GET_ITEM_TYPE(pGet->ui32WeaponID);
	uint16_t ui16Number = GET_ITEM_NUMBER(pGet->ui32WeaponID);
	HackType eHack = HMSParser::CheckNaturalRPGGet(pRoom, ui32SlotIdx, ui8Type, ui16Number);
	if (eHack != HACK_TYPE_NONE)
	{
		HackCheckResult result;
		result.eType = eHack;
		result.eSeverity = HACK_SEVERITY_HIGH;
		result.ui32SlotIdx = ui32SlotIdx;
		snprintf(result.szDescription, sizeof(result.szDescription),
				 "RPGExploit: weapon=%u", pGet->ui32WeaponID);
		BattleMember* pMember = pRoom->GetMember(ui32SlotIdx);
		HMSParser::EnforceHack(pRoom, &result, pMember ? pMember->GetUID() : 0);
		return PACKET_CHECK_HACK;
	}

	pChara->PickUpWeapon(pGet->ui32WeaponID, pGet->ui8Slot);
	return PACKET_CHECK_OK;
}

PacketCheckResult UDPChecker::_ParseDirectPickUpPacket(BattleRoom* pRoom, uint32_t ui32SlotIdx,
														const UDPGetWeaponPacket* pPickup)
{
	return _ParseGetWeaponPacket(pRoom, ui32SlotIdx, pPickup);
}

PacketCheckResult UDPChecker::_ParseDroppedWeaponPacket(BattleRoom* pRoom, uint32_t ui32SlotIdx,
														 const UDPDroppedWeaponPacket* pDropInfo)
{
	const WeaponDamageInfo* pInfo = WeaponTable::GetInstance().GetWeaponInfo(pDropInfo->ui32WeaponID);
	if (!pInfo)
		return PACKET_CHECK_INVALID;

	if (pDropInfo->ui16LoadedBullets > pInfo->ui16LoadBullet * 2 ||
		pDropInfo->ui16ReserveBullets > pInfo->ui16MaxBullet * 2)
	{
		HackCheckResult result;
		result.eType = HACK_TYPE_BULLET_HACK;
		result.eSeverity = HACK_SEVERITY_HIGH;
		result.ui32SlotIdx = ui32SlotIdx;
		snprintf(result.szDescription, sizeof(result.szDescription),
				 "BulletHack: weapon=%u loaded=%u reserve=%u",
				 pDropInfo->ui32WeaponID, pDropInfo->ui16LoadedBullets,
				 pDropInfo->ui16ReserveBullets);
		BattleMember* pMember = pRoom->GetMember(ui32SlotIdx);
		HMSParser::EnforceHack(pRoom, &result, pMember ? pMember->GetUID() : 0);
		return PACKET_CHECK_HACK;
	}

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

	// Send mission notification to GameServer
	extern BattleSessionManager* g_pBattleSessionManager;
	if (g_pBattleSessionManager)
	{
		BattleSession* pSession = (BattleSession*)g_pBattleSessionManager->GetSession(
			pRoom->GetOwnerSessionIdx());
		BattleMember* pMember = pRoom->GetMember(ui32SlotIdx);
		if (pSession && pMember)
		{
			pSession->SendMissionNotify(
				pRoom->GetGameServerRoomIdx(), pRoom->GetChannelNum(),
				pMission->ui8MissionType, ui32SlotIdx,
				pMember->GetUID(),
				pMission->ui8Param1, pMission->ui8Param2);
		}
	}

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

	// Mark character as carrying object
	pChara->SetOnLoad(true);
	return PACKET_CHECK_OK;
}

PacketCheckResult UDPChecker::_ParseCharaSyncObj(BattleRoom* pRoom, uint32_t ui32SlotIdx,
												  const char* pData, int32_t nSize)
{
	GameCharacter* pChara = pRoom->GetCharacter(ui32SlotIdx);
	if (!pChara)
		return PACKET_CHECK_INVALID;

	return PACKET_CHECK_OK;
}

PacketCheckResult UDPChecker::_ParseCharaRadioChat(BattleRoom* pRoom, uint32_t ui32SlotIdx,
													const UDPRadioChatPacket* pRadio)
{
	GameCharacter* pChara = pRoom->GetCharacter(ui32SlotIdx);
	if (!pChara)
		return PACKET_CHECK_INVALID;

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

	if (pSuicide->i16Damage < 0 || pSuicide->i16Damage > 1000)
		return PACKET_CHECK_INVALID;

	// Apply self-damage
	int32_t remainHP = pChara->SubHP(pSuicide->i16Damage);
	if (remainHP <= 0)
	{
		pChara->SetAlive(false);
		pChara->SetHP(0);

		BattleMember* pMember = pRoom->GetMember(ui32SlotIdx);
		if (pMember)
		{
			pMember->OnDeath();
			pMember->AddDeath();
		}
	}

	return PACKET_CHECK_OK;
}
