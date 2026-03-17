#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "ClanDef.h"
#include "GameSessionManager.h"

// ============================================================================
// Clan Handlers (Protocol_Clan 0x0700)
// ============================================================================

void GameSession::OnClanCreateReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_CS_CREATE_CLAN_REQ -> ACK
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < (int)(MAX_CLAN_NAME_LEN + 2))
		return;

	char szClanName[MAX_CLAN_NAME_LEN];
	memcpy(szClanName, pData, MAX_CLAN_NAME_LEN);
	szClanName[MAX_CLAN_NAME_LEN - 1] = '\0';

	uint16_t markId = *(uint16_t*)(pData + MAX_CLAN_NAME_LEN);

	int32_t result = 0;
	int32_t clanId = 0;

	if (m_i32ClanId > 0)
	{
		result = 1;		// Already in a clan
	}
	else if (m_i32GP < (g_pContextMain ? g_pContextMain->m_i32ClanCreateCostGP : CLAN_CREATE_COST_GP))
	{
		result = 2;		// Not enough GP
	}
	else if (m_i32Level < (g_pContextMain ? g_pContextMain->m_i32ClanCreateMinLevel : 5))
	{
		result = 3;		// Level too low
	}
	else if (!g_pClanManager)
	{
		result = 4;
	}
	else
	{
		clanId = g_pClanManager->CreateClan(szClanName, m_i64UID, m_szNickname,
			m_i32Level, m_i32RankId);

		if (clanId == -1)
		{
			result = 5;	// Name taken
		}
		else if (clanId < 0)
		{
			result = 4;	// No slots
		}
		else
		{
			m_i32GP -= (g_pContextMain ? g_pContextMain->m_i32ClanCreateCostGP : CLAN_CREATE_COST_GP);
			m_i32ClanId = clanId;

			// Set mark
			GameClanInfo* pClan = g_pClanManager->FindClan(clanId);
			if (pClan)
				pClan->ui16MarkId = markId;

			printf("[GameSession] Clan created - UID=%lld, Name=%s, ClanId=%d\n",
				m_i64UID, szClanName, clanId);
		}
	}

	// Send ACK
	i3NetworkPacket packet;
	offset += sizeof(uint16_t);
	i3NetworkPacket packet(PROTOCOL_CS_CREATE_CLAN_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&clanId, sizeof(int32_t));
	SendPacketMessage(&packet);
}

void GameSession::OnClanCloseReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_CS_CLOSE_CLAN_REQ -> ACK
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	int32_t result = 0;

	if (m_i32ClanId <= 0 || !g_pClanManager)
	{
		result = 1;		// Not in clan
	}
	else
	{
		GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
		if (!pClan)
		{
			result = 2;
		}
		else if (!pClan->IsMaster(m_i64UID))
		{
			result = 3;	// Not master
		}
		else
		{
			printf("[GameSession] Clan disbanded - UID=%lld, ClanId=%d, Name=%s\n",
				m_i64UID, m_i32ClanId, pClan->szName);

			// Clear clan ID for all online members
			if (g_pGameSessionManager)
			{
				for (int i = 0; i < MAX_CLAN_MEMBERS; i++)
				{
					if (pClan->members[i].IsActive())
					{
						GameSession* pMember = g_pGameSessionManager->FindSessionByUID(
							pClan->members[i].i64UID);
						if (pMember)
							pMember->m_i32ClanId = 0;
					}
				}
			}

			g_pClanManager->DisbandClan(m_i32ClanId);
			m_i32ClanId = 0;
		}
	}

	SendSimpleAck(PROTOCOL_CS_CLOSE_CLAN_ACK, result);
}

void GameSession::OnClanDetailInfoReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_CS_DETAIL_INFO_REQ -> ACK
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	int32_t reqClanId = m_i32ClanId;
	if (i32Size >= 4)
		reqClanId = *(int32_t*)pData;

	int32_t result = 0;

	i3NetworkPacket packet;
	offset += sizeof(uint16_t);

	if (reqClanId <= 0 || !g_pClanManager)
	{
		result = 1;
	}
	else
	{
		GameClanInfo* pClan = g_pClanManager->FindClan(reqClanId);
		if (!pClan)
		{
			result = 2;
		}
		else
		{

			// Clan detail info
			memcpy(buffer + offset, &pClan->i32ClanId, 4);			offset += 4;
			memcpy(buffer + offset, pClan->szName, MAX_CLAN_NAME_LEN);	offset += MAX_CLAN_NAME_LEN;
			memcpy(buffer + offset, &pClan->i32ClanExp, 4);		offset += 4;
			memcpy(buffer + offset, &pClan->i32ClanRank, 4);		offset += 4;
			memcpy(buffer + offset, &pClan->i32MemberCount, 4);	offset += 4;
			memcpy(buffer + offset, &pClan->i32MaxMembers, 4);		offset += 4;
			memcpy(buffer + offset, &pClan->ui16MarkId, 2);		offset += 2;
			memcpy(buffer + offset, &pClan->ui8MarkColor, 1);		offset += 1;
			memcpy(buffer + offset, &pClan->i32Wins, 4);			offset += 4;
			memcpy(buffer + offset, &pClan->i32Losses, 4);			offset += 4;

			// Master info
			memcpy(buffer + offset, &pClan->i64MasterUID, 8);		offset += 8;
			memcpy(buffer + offset, pClan->szMasterNickname, 64);	offset += 64;

			// Notice
			uint16_t noticeLen = (uint16_t)strlen(pClan->szNotice);
			if (noticeLen > 0)
			{
				memcpy(buffer + offset, pClan->szNotice, noticeLen);
				offset += noticeLen;
			}
		}
	}
	i3NetworkPacket packet(PROTOCOL_CS_DETAIL_INFO_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&noticeLen, 2);
	SendPacketMessage(&packet);
}

void GameSession::OnClanMemberListReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_CS_MEMBER_LIST_REQ -> ACK
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	int32_t result = 0;

	i3NetworkPacket packet;
	offset += sizeof(uint16_t);

	if (m_i32ClanId <= 0 || !g_pClanManager)
	{
		result = 1;
	}
	else
	{
		GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
		if (!pClan)
		{
			result = 2;
		}
		else
		{

			uint16_t memberCount = (uint16_t)pClan->i32MemberCount;

			// Per-member: uid(8) + nickname(64) + level(1) + playerLevel(4) + rankId(4) + online(1) = 82 bytes
			for (int i = 0; i < MAX_CLAN_MEMBERS; i++)
			{
				const GameClanMember& m = pClan->members[i];
				if (!m.IsActive())
					continue;

				if (offset + 82 > (int)sizeof(buffer))
					break;

				// Check online status
				bool isOnline = false;
				if (g_pGameSessionManager)
				{
					GameSession* pSess = g_pGameSessionManager->FindSessionByUID(m.i64UID);
					isOnline = (pSess != nullptr && pSess->GetTask() >= GAME_TASK_CHANNEL);
				}

				memcpy(buffer + offset, &m.i64UID, 8);				offset += 8;
				memcpy(buffer + offset, m.szNickname, 64);			offset += 64;
				memcpy(buffer + offset, &m.ui8Level, 1);			offset += 1;
				memcpy(buffer + offset, &m.i32PlayerLevel, 4);		offset += 4;
				memcpy(buffer + offset, &m.i32RankId, 4);			offset += 4;
				uint8_t online = isOnline ? 1 : 0;
			}
		}
	}
	i3NetworkPacket packet(PROTOCOL_CS_MEMBER_LIST_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&memberCount, 2);
	packet.WriteData(&online, 1);
	SendPacketMessage(&packet);
}

void GameSession::OnClanJoinReq(char* pData, INT32 i32Size)
{
	// Submit a join request — queued until master/staff accepts or denies
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 4)
		return;

	int32_t targetClanId = *(int32_t*)pData;
	int32_t result = 0;

	if (m_i32ClanId > 0)
	{
		result = 1;		// Already in a clan
	}
	else if (!g_pClanManager)
	{
		result = 4;
	}
	else
	{
		GameClanInfo* pClan = g_pClanManager->FindClan(targetClanId);
		if (!pClan)
		{
			result = 2;	// Clan not found
		}
		else if (pClan->i32MemberCount >= pClan->i32MaxMembers)
		{
			result = 3;	// Clan full
		}
		else if (pClan->HasJoinRequest(m_i64UID))
		{
			result = 5;	// Already has a pending request
		}
		else
		{
			if (pClan->AddJoinRequest(m_i64UID, m_szNickname, m_i32Level, m_i32RankId))
			{
				printf("[GameSession] Clan join request queued - UID=%lld, ClanId=%d, Name=%s\n",
					m_i64UID, targetClanId, pClan->szName);
			}
			else
			{
				result = 6;	// Request queue full
			}
		}
	}

	// Send join ACK
	i3NetworkPacket packet;
	offset += sizeof(uint16_t);
	i3NetworkPacket packet(PROTOCOL_CS_JOIN_REQUEST_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&targetClanId, sizeof(int32_t));
	SendPacketMessage(&packet);
}

void GameSession::OnClanLeaveReq(char* pData, INT32 i32Size)
{
	// Leave current clan
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	int32_t result = 0;

	if (m_i32ClanId <= 0 || !g_pClanManager)
	{
		result = 1;
	}
	else
	{
		GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
		if (!pClan)
		{
			result = 2;
		}
		else if (pClan->IsMaster(m_i64UID))
		{
			result = 3;	// Master cannot leave, must disband
		}
		else
		{
			printf("[GameSession] Left clan - UID=%lld, ClanId=%d\n", m_i64UID, m_i32ClanId);
			pClan->RemoveMember(m_i64UID);
			m_i32ClanId = 0;
		}
	}

	SendSimpleAck(PROTOCOL_CS_CLIENT_LEAVE_ACK, result);
}

void GameSession::OnClanEnterReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_CS_CLIENT_ENTER_REQ -> ACK
	// Enter clan UI
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	int32_t result = (m_i32ClanId > 0) ? 0 : 1;
	SendSimpleAck(PROTOCOL_CS_CLIENT_ENTER_ACK, result);
}

void GameSession::OnClanChatReq(char* pData, INT32 i32Size)
{
	// Clan chat broadcast to all online clan members
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (m_i32ClanId <= 0 || !g_pClanManager || !g_pGameSessionManager)
		return;

	if (i32Size < 2)
		return;

	uint16_t msgLen = *(uint16_t*)pData;
	if (msgLen == 0 || msgLen > 200 || i32Size < (int)(2 + msgLen))
		return;

	char szMsg[202];
	memcpy(szMsg, pData + 2, msgLen);
	szMsg[msgLen] = '\0';

	GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
	if (!pClan)
		return;

	// Build chat packet
	i3NetworkPacket packet;
	offset += sizeof(uint16_t);

	int32_t result = 0;

	// Sender nickname

	// Message
	memcpy(buffer + offset, szMsg, msgLen);					offset += msgLen;
	i3NetworkPacket packet(PROTOCOL_CS_CHATTING_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(m_szNickname, 64);
	packet.WriteData(&msgLen, sizeof(uint16_t));
	// Broadcast to all online members
	for (int i = 0; i < MAX_CLAN_MEMBERS; i++)
	{
		if (!pClan->members[i].IsActive())
			continue;

		GameSession* pMember = g_pGameSessionManager->FindSessionByUID(pClan->members[i].i64UID);
		if (pMember && pMember->GetTask() >= GAME_TASK_CHANNEL)
			pMember->SendPacketMessage(&packet);
	}
}

// ============================================================================
// Clan Member Management (Deportation, Commission)
// ============================================================================

void GameSession::OnClanDeportationReq(char* pData, INT32 i32Size)
{
	// Kick a member from the clan (master/staff only)
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 8)
		return;

	int64_t targetUID = *(int64_t*)pData;
	int32_t result = 0;

	if (m_i32ClanId <= 0 || !g_pClanManager)
	{
		result = 1;
	}
	else
	{
		GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
		if (!pClan)
		{
			result = 2;
		}
		else if (!pClan->IsStaffOrMaster(m_i64UID))
		{
			result = 3;	// Not authorized
		}
		else if (pClan->IsMaster(targetUID))
		{
			result = 4;	// Cannot kick master
		}
		else
		{
			GameClanMember* pMember = pClan->FindMember(targetUID);
			if (!pMember)
			{
				result = 5;	// Not a member
			}
			else
			{
				printf("[Clan] Deportation - ClanId=%d, GM_UID=%lld kicked UID=%lld (%s)\n",
					m_i32ClanId, m_i64UID, targetUID, pMember->szNickname);

				pClan->RemoveMember(targetUID);

				// Clear clan ID if target is online
				if (g_pGameSessionManager)
				{
					GameSession* pTarget = g_pGameSessionManager->FindSessionByUID(targetUID);
					if (pTarget)
					{
						pTarget->m_i32ClanId = 0;
						// Notify target
						pTarget->SendSimpleAck(PROTOCOL_CS_DEPORTATION_RESULT_ACK, 0);
					}
				}
			}
		}
	}

	SendSimpleAck(PROTOCOL_CS_DEPORTATION_ACK, result);
}

void GameSession::OnClanCommissionMasterReq(char* pData, INT32 i32Size)
{
	// Transfer master role to another member
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 8)
		return;

	int64_t targetUID = *(int64_t*)pData;
	int32_t result = 0;

	if (m_i32ClanId <= 0 || !g_pClanManager)
	{
		result = 1;
	}
	else
	{
		GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
		if (!pClan)
		{
			result = 2;
		}
		else if (!pClan->IsMaster(m_i64UID))
		{
			result = 3;	// Only master can transfer
		}
		else
		{
			GameClanMember* pNewMaster = pClan->FindMember(targetUID);
			GameClanMember* pOldMaster = pClan->FindMember(m_i64UID);
			if (!pNewMaster)
			{
				result = 4;	// Target not a member
			}
			else
			{
				printf("[Clan] Commission Master - ClanId=%d, Old=%lld -> New=%lld (%s)\n",
					m_i32ClanId, m_i64UID, targetUID, pNewMaster->szNickname);

				pNewMaster->ui8Level = CLAN_MEMBER_MASTER;
				if (pOldMaster) pOldMaster->ui8Level = CLAN_MEMBER_REGULAR;
				pClan->i64MasterUID = targetUID;
				strncpy_s(pClan->szMasterNickname, pNewMaster->szNickname, _TRUNCATE);

				// Notify new master if online
				if (g_pGameSessionManager)
				{
					GameSession* pTarget = g_pGameSessionManager->FindSessionByUID(targetUID);
					if (pTarget)
						pTarget->SendSimpleAck(PROTOCOL_CS_COMMISSION_MASTER_RESULT_ACK, 0);
				}
			}
		}
	}

	SendSimpleAck(PROTOCOL_CS_COMMISSION_MASTER_ACK, result);
}

void GameSession::OnClanCommissionStaffReq(char* pData, INT32 i32Size)
{
	// Promote a member to staff
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 8)
		return;

	int64_t targetUID = *(int64_t*)pData;
	int32_t result = 0;

	if (m_i32ClanId <= 0 || !g_pClanManager)
	{
		result = 1;
	}
	else
	{
		GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
		if (!pClan)
			result = 2;
		else if (!pClan->IsMaster(m_i64UID))
			result = 3;	// Only master can promote
		else
		{
			GameClanMember* pMember = pClan->FindMember(targetUID);
			if (!pMember)
				result = 4;
			else if (pMember->ui8Level <= CLAN_MEMBER_STAFF)
				result = 5;	// Already staff or master
			else
			{
				pMember->ui8Level = CLAN_MEMBER_STAFF;
				printf("[Clan] Commission Staff - ClanId=%d, UID=%lld promoted (%s)\n",
					m_i32ClanId, targetUID, pMember->szNickname);

				if (g_pGameSessionManager)
				{
					GameSession* pTarget = g_pGameSessionManager->FindSessionByUID(targetUID);
					if (pTarget)
						pTarget->SendSimpleAck(PROTOCOL_CS_COMMISSION_STAFF_RESULT_ACK, 0);
				}
			}
		}
	}

	SendSimpleAck(PROTOCOL_CS_COMMISSION_STAFF_ACK, result);
}

void GameSession::OnClanCommissionRegularReq(char* pData, INT32 i32Size)
{
	// Demote a staff member to regular
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 8)
		return;

	int64_t targetUID = *(int64_t*)pData;
	int32_t result = 0;

	if (m_i32ClanId <= 0 || !g_pClanManager)
	{
		result = 1;
	}
	else
	{
		GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
		if (!pClan)
			result = 2;
		else if (!pClan->IsMaster(m_i64UID))
			result = 3;
		else
		{
			GameClanMember* pMember = pClan->FindMember(targetUID);
			if (!pMember)
				result = 4;
			else if (pMember->ui8Level != CLAN_MEMBER_STAFF)
				result = 5;	// Not staff
			else
			{
				pMember->ui8Level = CLAN_MEMBER_REGULAR;
				printf("[Clan] Commission Regular - ClanId=%d, UID=%lld demoted (%s)\n",
					m_i32ClanId, targetUID, pMember->szNickname);

				if (g_pGameSessionManager)
				{
					GameSession* pTarget = g_pGameSessionManager->FindSessionByUID(targetUID);
					if (pTarget)
						pTarget->SendSimpleAck(PROTOCOL_CS_COMMISSION_REGULAR_RESULT_ACK, 0);
				}
			}
		}
	}

	SendSimpleAck(PROTOCOL_CS_COMMISSION_REGULAR_ACK, result);
}

// ============================================================================
// Clan Info Updates (Notice, Intro, Mark)
// ============================================================================

void GameSession::OnClanReplaceNoticeReq(char* pData, INT32 i32Size)
{
	// Update clan notice (master/staff only)
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 2)
		return;

	uint16_t noticeLen = *(uint16_t*)pData;
	if (noticeLen > MAX_CLAN_NOTICE_LEN - 1)
		noticeLen = MAX_CLAN_NOTICE_LEN - 1;
	if (i32Size < (int)(2 + noticeLen))
		return;

	int32_t result = 0;

	if (m_i32ClanId <= 0 || !g_pClanManager)
	{
		result = 1;
	}
	else
	{
		GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
		if (!pClan)
			result = 2;
		else if (!pClan->IsStaffOrMaster(m_i64UID))
			result = 3;
		else
		{
			memcpy(pClan->szNotice, pData + 2, noticeLen);
			pClan->szNotice[noticeLen] = '\0';
			printf("[Clan] Notice updated - ClanId=%d, by UID=%lld\n", m_i32ClanId, m_i64UID);
		}
	}

	SendSimpleAck(PROTOCOL_CS_REPLACE_NOTICE_ACK, result);
}

void GameSession::OnClanReplaceIntroReq(char* pData, INT32 i32Size)
{
	// Update clan intro (master/staff only)
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 2)
		return;

	uint16_t introLen = *(uint16_t*)pData;
	if (introLen > MAX_CLAN_INTRO_LEN - 1)
		introLen = MAX_CLAN_INTRO_LEN - 1;
	if (i32Size < (int)(2 + introLen))
		return;

	int32_t result = 0;

	if (m_i32ClanId <= 0 || !g_pClanManager)
	{
		result = 1;
	}
	else
	{
		GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
		if (!pClan)
			result = 2;
		else if (!pClan->IsStaffOrMaster(m_i64UID))
			result = 3;
		else
		{
			memcpy(pClan->szIntro, pData + 2, introLen);
			pClan->szIntro[introLen] = '\0';
			printf("[Clan] Intro updated - ClanId=%d, by UID=%lld\n", m_i32ClanId, m_i64UID);
		}
	}

	SendSimpleAck(PROTOCOL_CS_REPLACE_INTRO_ACK, result);
}

void GameSession::OnClanReplaceMarkReq(char* pData, INT32 i32Size)
{
	// Update clan mark/emblem (master only)
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 3)	// markId(2) + markColor(1)
		return;

	uint16_t markId = *(uint16_t*)pData;
	uint8_t markColor = *(uint8_t*)(pData + 2);

	int32_t result = 0;

	if (m_i32ClanId <= 0 || !g_pClanManager)
	{
		result = 1;
	}
	else
	{
		GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
		if (!pClan)
			result = 2;
		else if (!pClan->IsMaster(m_i64UID))
			result = 3;
		else
		{
			pClan->ui16MarkId = markId;
			pClan->ui8MarkColor = markColor;
			printf("[Clan] Mark updated - ClanId=%d, Mark=%d, Color=%d\n",
				m_i32ClanId, markId, markColor);
		}
	}

	SendSimpleAck(PROTOCOL_CS_REPLACE_MARK_ACK, result);
}

void GameSession::OnClanListReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_CS_CLIENT_CLAN_LIST_REQ -> ACK
	// Send list of all clans
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (!g_pClanManager)
	{
		SendSimpleAck(PROTOCOL_CS_CLIENT_CLAN_LIST_ACK, 1);
		return;
	}

	i3NetworkPacket packet;
	offset += sizeof(uint16_t);

	int32_t result = 0;

	// Count active clans
	const GameClanInfo* pClans = g_pClanManager->GetClans();
	uint16_t clanCount = 0;
	int countPos = offset;
	offset += sizeof(uint16_t);		// Reserve for count

	// Per-clan: id(4) + name(32) + memberCount(4) + maxMembers(4) + markId(2) + rank(4) = 50 bytes
	for (int i = 0; i < MAX_CLANS; i++)
	{
		if (!pClans[i].bActive)
			continue;

		if (offset + 50 > (int)sizeof(buffer))
			break;

		memcpy(buffer + offset, &pClans[i].i32ClanId, 4);			offset += 4;
		memcpy(buffer + offset, pClans[i].szName, MAX_CLAN_NAME_LEN);offset += MAX_CLAN_NAME_LEN;
		memcpy(buffer + offset, &pClans[i].i32MemberCount, 4);		offset += 4;
		memcpy(buffer + offset, &pClans[i].i32MaxMembers, 4);		offset += 4;
		memcpy(buffer + offset, &pClans[i].ui16MarkId, 2);			offset += 2;
		memcpy(buffer + offset, &pClans[i].i32ClanRank, 4);		offset += 4;
		clanCount++;
	}

	memcpy(buffer + countPos, &clanCount, sizeof(uint16_t));
	i3NetworkPacket packet(PROTOCOL_CS_CLIENT_CLAN_LIST_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	SendPacketMessage(&packet);
}

void GameSession::OnClanAcceptRequestReq(char* pData, INT32 i32Size)
{
	// Accept a clan join request - master/staff accepts pending member
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 8)
	{
		SendSimpleAck(PROTOCOL_CS_ACCEPT_REQUEST_ACK, -1);
		return;
	}

	int64_t requesterUID = 0;
	memcpy(&requesterUID, pData, 8);

	int32_t result = 0;

	if (m_i32ClanId <= 0 || !g_pClanManager)
	{
		result = -2;
	}
	else
	{
		GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
		if (!pClan)
			result = -3;
		else if (!pClan->IsStaffOrMaster(m_i64UID))
			result = -4;	// No permission
		else if (pClan->i32MemberCount >= pClan->i32MaxMembers)
			result = -5;	// Clan full
		else
		{
			// Verify a pending request exists
			ClanJoinRequest* pReq = pClan->FindJoinRequest(requesterUID);
			if (!pReq)
			{
				result = -8;	// No pending request from this player
			}
			else
			{
				// Find the requester online to get their current info
				GameSession* pRequester = g_pGameSessionManager->FindSessionByUID(requesterUID);
				if (pRequester)
				{
					if (pRequester->GetClanId() > 0)
						result = -6;	// Already joined another clan
					else
					{
						pClan->AddMember(requesterUID, pRequester->GetNickname(),
							CLAN_MEMBER_REGULAR, pRequester->GetLevel(), pRequester->GetRankId());
						pRequester->SetClanId(m_i32ClanId);
						pClan->RemoveJoinRequest(requesterUID);

						// Notify the requester
						pRequester->SendSimpleAck(PROTOCOL_CS_ACCEPT_REQUEST_RESULT_ACK, 0);
					}
				}
				else
				{
					// Requester offline — accept using queued info, they'll see the clan on next login
					pClan->AddMember(requesterUID, pReq->szNickname,
						CLAN_MEMBER_REGULAR, pReq->i32Level, pReq->i32RankId);
					pClan->RemoveJoinRequest(requesterUID);
				}
			}
		}
	}

	SendSimpleAck(PROTOCOL_CS_ACCEPT_REQUEST_ACK, result);
}

void GameSession::OnClanDenialRequestReq(char* pData, INT32 i32Size)
{
	// Deny a clan join request
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 8)
	{
		SendSimpleAck(PROTOCOL_CS_DENIAL_REQUEST_ACK, -1);
		return;
	}

	int64_t requesterUID = 0;
	memcpy(&requesterUID, pData, 8);

	int32_t result = 0;

	if (m_i32ClanId <= 0 || !g_pClanManager)
	{
		result = -2;
	}
	else
	{
		GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
		if (!pClan)
			result = -3;
		else if (!pClan->IsStaffOrMaster(m_i64UID))
			result = -4;
		else
		{
			// Remove from queue regardless of whether requester is online
			pClan->RemoveJoinRequest(requesterUID);

			// Notify the requester if online
			GameSession* pRequester = g_pGameSessionManager->FindSessionByUID(requesterUID);
			if (pRequester)
				pRequester->SendSimpleAck(PROTOCOL_CS_DENIAL_REQUEST_RESULT_ACK, 0);
		}
	}

	SendSimpleAck(PROTOCOL_CS_DENIAL_REQUEST_ACK, result);
}

void GameSession::OnClanInviteReq(char* pData, INT32 i32Size)
{
	// Master/staff invites a player to join the clan
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 8)
	{
		SendSimpleAck(PROTOCOL_CS_INVITE_ACK, -1);
		return;
	}

	int64_t targetUID = 0;
	memcpy(&targetUID, pData, 8);

	int32_t result = 0;

	if (m_i32ClanId <= 0 || !g_pClanManager)
	{
		result = -2;
	}
	else
	{
		GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
		if (!pClan)
			result = -3;
		else if (!pClan->IsStaffOrMaster(m_i64UID))
			result = -4;
		else if (pClan->i32MemberCount >= pClan->i32MaxMembers)
			result = -5;
		else
		{
			GameSession* pTarget = g_pGameSessionManager->FindSessionByUID(targetUID);
			if (!pTarget)
				result = -6;	// Target offline
			else if (pTarget->GetClanId() > 0)
				result = -7;	// Already in a clan
			else
			{
				// Send invite notification to target
				i3NetworkPacket invitePacket(PROTOCOL_CS_INVITE_ACCEPT_REQ);
				invitePacket.WriteData(&m_i32ClanId, sizeof(int32_t));
				invitePacket.WriteData(m_szClanName, MAX_CLAN_NAME_LEN);
				invitePacket.WriteData(m_szNickname, 64);
				pTarget->SendPacketMessage(&invitePacket);
			}
		}
	}

	SendSimpleAck(PROTOCOL_CS_INVITE_ACK, result);
}

void GameSession::OnClanRequestListReq(char* pData, INT32 i32Size)
{
	// Get list of pending join requests for this clan
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (m_i32ClanId <= 0 || !g_pClanManager)
	{
		SendSimpleAck(PROTOCOL_CS_REQUEST_LIST_ACK, -1);
		return;
	}

	GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
	if (!pClan || !pClan->IsStaffOrMaster(m_i64UID))
	{
		SendSimpleAck(PROTOCOL_CS_REQUEST_LIST_ACK, -2);
		return;
	}

	int32_t result = 0;
	uint16_t requestCount = (uint16_t)pClan->i32JoinRequestCount;

	i3NetworkPacket packet(PROTOCOL_CS_REQUEST_LIST_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&requestCount, sizeof(uint16_t));

	for (int i = 0; i < MAX_CLAN_JOIN_REQUESTS; i++)
	{
		if (!pClan->joinRequests[i].bActive)
			continue;
		const ClanJoinRequest& req = pClan->joinRequests[i];
		packet.WriteData(&req.i64UID, sizeof(int64_t));
		packet.WriteData(req.szNickname, 64);
		packet.WriteData(&req.i32Level, sizeof(int32_t));
		packet.WriteData(&req.i32RankId, sizeof(int32_t));
	}

	SendPacketMessage(&packet);
}

void GameSession::OnClanCheckDuplicateReq(char* pData, INT32 i32Size)
{
	// Check if clan name is already taken
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 2)
	{
		SendSimpleAck(PROTOCOL_CS_CHECK_DUPLICATE_ACK, -1);
		return;
	}

	char clanName[MAX_CLAN_NAME_LEN + 1] = {};
	int copyLen = (i32Size > MAX_CLAN_NAME_LEN) ? MAX_CLAN_NAME_LEN : i32Size;
	memcpy(clanName, pData, copyLen);
	clanName[MAX_CLAN_NAME_LEN] = '\0';

	int32_t result = 0;	// 0 = available
	if (g_pClanManager)
	{
		GameClanInfo* pExisting = g_pClanManager->FindClanByName(clanName);
		if (pExisting)
			result = 1;	// Name taken
	}

	SendSimpleAck(PROTOCOL_CS_CHECK_DUPLICATE_ACK, result);
}

void GameSession::OnClanMemberDetailInfoReq(char* pData, INT32 i32Size)
{
	// Get detailed info about a specific clan member
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 8)
	{
		SendSimpleAck(PROTOCOL_CS_MEMBER_DETAIL_INFO_ACK, -1);
		return;
	}

	int64_t memberUID = 0;
	memcpy(&memberUID, pData, 8);

	i3NetworkPacket packet;
	offset += sizeof(uint16_t);

	int32_t result = -1;

	if (m_i32ClanId > 0 && g_pClanManager)
	{
		GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
		if (pClan)
		{
			GameClanMember* pMember = pClan->FindMember(memberUID);
			if (pMember)
			{
				result = 0;
				memcpy(buffer + offset, &pMember->i64UID, 8);		offset += 8;
				memcpy(buffer + offset, pMember->szNickname, 64);	offset += 64;
				memcpy(buffer + offset, &pMember->ui8Level, 1);	offset += 1;
				memcpy(buffer + offset, &pMember->i32PlayerLevel, 4);	offset += 4;
				memcpy(buffer + offset, &pMember->i32RankId, 4);	offset += 4;

				uint8_t online = pMember->bOnline ? 1 : 0;

				// Stats from online session if available
				GameSession* pOnline = g_pGameSessionManager->FindSessionByUID(memberUID);
				int32_t kills = 0, deaths = 0, wins = 0, losses = 0;
				if (pOnline)
				{
					kills = pOnline->GetKills();
					deaths = pOnline->GetDeaths();
					wins = pOnline->GetWins();
					losses = pOnline->GetLosses();
				}
			}
		}
	}

	if (result != 0)
	{
	}
	i3NetworkPacket packet(PROTOCOL_CS_MEMBER_DETAIL_INFO_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&online, 1);
	packet.WriteData(&kills, 4);
	packet.WriteData(&deaths, 4);
	packet.WriteData(&wins, 4);
	packet.WriteData(&losses, 4);
	packet.WriteData(&result, sizeof(int32_t));
	SendPacketMessage(&packet);
}

void GameSession::OnClanPageChattingReq(char* pData, INT32 i32Size)
{
	// Clan page chatting - broadcast chat to all online clan members
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 2 || m_i32ClanId <= 0 || !g_pClanManager)
	{
		SendSimpleAck(PROTOCOL_CS_PAGE_CHATTING_ACK, -1);
		return;
	}

	uint16_t msgLen = *(uint16_t*)pData;
	if (msgLen > 256) msgLen = 256;
	if (i32Size < (int)(2 + msgLen))
		return;

	// Build chat broadcast packet
	i3NetworkPacket packet;
	offset += sizeof(uint16_t);

	int32_t result = 0;

	// Sender info

	// Message
	memcpy(buffer + offset, pData + 2, msgLen);			offset += msgLen;
	i3NetworkPacket packet(PROTOCOL_CS_PAGE_CHATTING_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&m_i64UID, 8);
	packet.WriteData(m_szNickname, 64);
	packet.WriteData(&msgLen, sizeof(uint16_t));
	// Send to all online clan members
	GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
	if (pClan)
	{
		for (int i = 0; i < MAX_CLAN_MEMBERS; i++)
		{
			if (!pClan->members[i].IsActive())
				continue;

			GameSession* pMember = g_pGameSessionManager->FindSessionByUID(pClan->members[i].i64UID);
			if (pMember)
				pMember->SendPacketMessage(&packet);
		}
	}
}

void GameSession::OnClanListDetailInfoReq(char* pData, INT32 i32Size)
{
	// Detailed info about a specific clan (from clan list UI)
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 4)
	{
		SendSimpleAck(PROTOCOL_CS_CLAN_LIST_DETAIL_INFO_ACK, -1);
		return;
	}

	int32_t clanId = 0;
	memcpy(&clanId, pData, 4);

	i3NetworkPacket packet;
	offset += sizeof(uint16_t);

	int32_t result = -1;

	if (g_pClanManager)
	{
		GameClanInfo* pClan = g_pClanManager->FindClan(clanId);
		if (pClan)
		{
			result = 0;
			memcpy(buffer + offset, &pClan->i32ClanId, 4);			offset += 4;
			memcpy(buffer + offset, pClan->szName, MAX_CLAN_NAME_LEN);	offset += MAX_CLAN_NAME_LEN;
			memcpy(buffer + offset, pClan->szMasterNickname, 64);	offset += 64;
			memcpy(buffer + offset, &pClan->i32MemberCount, 4);	offset += 4;
			memcpy(buffer + offset, &pClan->i32MaxMembers, 4);		offset += 4;
			memcpy(buffer + offset, &pClan->ui16MarkId, 2);		offset += 2;
			memcpy(buffer + offset, &pClan->ui8MarkColor, 1);		offset += 1;
			memcpy(buffer + offset, &pClan->i32ClanRank, 4);		offset += 4;
			memcpy(buffer + offset, &pClan->i32ClanExp, 4);		offset += 4;
			memcpy(buffer + offset, &pClan->i32Wins, 4);			offset += 4;
			memcpy(buffer + offset, &pClan->i32Losses, 4);			offset += 4;

			// Notice
			uint16_t noticeLen = (uint16_t)strlen(pClan->szNotice);
			if (noticeLen > 0)
			{
				memcpy(buffer + offset, pClan->szNotice, noticeLen);	offset += noticeLen;
			}

			// Intro
			uint16_t introLen = (uint16_t)strlen(pClan->szIntro);
			if (introLen > 0)
			{
				memcpy(buffer + offset, pClan->szIntro, introLen);	offset += introLen;
			}
		}
	}

	if (result != 0)
	{
	}
	i3NetworkPacket packet(PROTOCOL_CS_CLAN_LIST_DETAIL_INFO_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&noticeLen, 2);
	packet.WriteData(&introLen, 2);
	packet.WriteData(&result, sizeof(int32_t));
	SendPacketMessage(&packet);
}

// ============================================================================
// Batch 17 - Clan extras
// ============================================================================

void GameSession::OnClanInviteAcceptReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_CHANNEL)
	{
		SendSimpleAck(PROTOCOL_CS_INVITE_ACCEPT_ACK, -1);
		return;
	}

	if (m_i32ClanId > 0)
	{
		SendSimpleAck(PROTOCOL_CS_INVITE_ACCEPT_ACK, -2);
		return;
	}

	if (i32Size < (int)(sizeof(int) + sizeof(int64_t)))
	{
		SendSimpleAck(PROTOCOL_CS_INVITE_ACCEPT_ACK, -1);
		return;
	}

	int offset = 0;
	int clanId = 0;
	int64_t inviterUID = 0;
	memcpy(&clanId, pData + offset, sizeof(int));		offset += sizeof(int);
	memcpy(&inviterUID, pData + offset, sizeof(int64_t));

	if (!g_pClanManager)
	{
		SendSimpleAck(PROTOCOL_CS_INVITE_ACCEPT_ACK, -1);
		return;
	}

	GameClanInfo* pClan = g_pClanManager->FindClan(clanId);
	if (!pClan)
	{
		SendSimpleAck(PROTOCOL_CS_INVITE_ACCEPT_ACK, -3);
		return;
	}

	int memberSlot = pClan->FindEmptyMemberSlot();
	if (memberSlot < 0)
	{
		SendSimpleAck(PROTOCOL_CS_INVITE_ACCEPT_ACK, -4);
		return;
	}

	pClan->AddMember(m_i64UID, m_szNickname, 2);
	m_i32ClanId = clanId;

	if (g_pModuleDataServer)
		g_pModuleDataServer->RequestClanJoin(clanId, m_i64UID, GetIndex(), m_szNickname, (uint8_t)m_i32Level);

	SendSimpleAck(PROTOCOL_CS_INVITE_ACCEPT_ACK, 0);
}

void GameSession::OnClanCancelRequestReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_CHANNEL)
	{
		SendSimpleAck(PROTOCOL_CS_CANCEL_REQUEST_ACK, -1);
		return;
	}

	if (i32Size < (int)sizeof(int))
	{
		SendSimpleAck(PROTOCOL_CS_CANCEL_REQUEST_ACK, -1);
		return;
	}

	int clanId = 0;
	memcpy(&clanId, pData, sizeof(int));

	printf("[GameSession] Cancel clan request: UID=%lld, ClanId=%d\n", m_i64UID, clanId);
	SendSimpleAck(PROTOCOL_CS_CANCEL_REQUEST_ACK, 0);
}

void GameSession::OnClanReplaceNameReq(char* pData, INT32 i32Size)
{
	if (m_i32ClanId <= 0)
	{
		SendSimpleAck(PROTOCOL_CS_REPLACE_NAME_ACK, -1);
		return;
	}

	if (i32Size < (int)(sizeof(int) + 4))
	{
		SendSimpleAck(PROTOCOL_CS_REPLACE_NAME_ACK, -1);
		return;
	}

	if (!g_pClanManager)
	{
		SendSimpleAck(PROTOCOL_CS_REPLACE_NAME_ACK, -1);
		return;
	}

	GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
	if (!pClan || !pClan->IsMaster(m_i64UID))
	{
		SendSimpleAck(PROTOCOL_CS_REPLACE_NAME_ACK, -2);
		return;
	}

	int offset = 0;
	int clanId = 0;
	memcpy(&clanId, pData + offset, sizeof(int));	offset += sizeof(int);

	char newName[MAX_CLAN_NAME_LEN] = {};
	int nameLen = i32Size - offset;
	if (nameLen <= 0 || nameLen >= MAX_CLAN_NAME_LEN)
	{
		SendSimpleAck(PROTOCOL_CS_REPLACE_NAME_ACK, -3);
		return;
	}
	memcpy(newName, pData + offset, nameLen);

	GameClanInfo* pExisting = g_pClanManager->FindClanByName(newName);
	if (pExisting)
	{
		SendSimpleAck(PROTOCOL_CS_REPLACE_NAME_ACK, -4);
		return;
	}

	strncpy(pClan->szName, newName, MAX_CLAN_NAME_LEN - 1);
	SendSimpleAck(PROTOCOL_CS_REPLACE_NAME_ACK, 0);
}

void GameSession::OnClanNoteReq(char* pData, INT32 i32Size)
{
	if (m_i32ClanId <= 0)
	{
		SendSimpleAck(PROTOCOL_CS_NOTE_ACK, -1);
		return;
	}

	if (!g_pClanManager)
	{
		SendSimpleAck(PROTOCOL_CS_NOTE_ACK, -1);
		return;
	}

	GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
	if (!pClan || !pClan->IsStaffOrMaster(m_i64UID))
	{
		SendSimpleAck(PROTOCOL_CS_NOTE_ACK, -2);
		return;
	}

	if (i32Size < 4 || i32Size > 512)
	{
		SendSimpleAck(PROTOCOL_CS_NOTE_ACK, -1);
		return;
	}

	int32_t result = 0;
	char senderNick[64] = {};
	strncpy(senderNick, m_szNickname, 63);
	uint16_t contentLen = (uint16_t)(i32Size > 480 ? 480 : i32Size);

	i3NetworkPacket notePacket(PROTOCOL_CS_NOTE_ACK);
	notePacket.WriteData(&result, sizeof(int32_t));
	notePacket.WriteData(senderNick, 64);
	notePacket.WriteData(&contentLen, sizeof(uint16_t));
	notePacket.WriteData(pData, contentLen);

	if (g_pGameSessionManager)
	{
		for (int i = 0; i < MAX_CLAN_MEMBERS; i++)
		{
			if (pClan->members[i].i64UID > 0)
			{
				GameSession* pTarget = g_pGameSessionManager->FindSessionByUID(pClan->members[i].i64UID);
				if (pTarget && pTarget != this)
					pTarget->SendPacketMessage(&notePacket);
			}
		}
	}

	SendSimpleAck(PROTOCOL_CS_NOTE_ACK, 0);
}

void GameSession::OnClanReplacePersonmaxReq(char* pData, INT32 i32Size)
{
	if (m_i32ClanId <= 0)
	{
		SendSimpleAck(PROTOCOL_CS_REPLACE_PERSONMAX_ACK, -1);
		return;
	}

	if (!g_pClanManager)
	{
		SendSimpleAck(PROTOCOL_CS_REPLACE_PERSONMAX_ACK, -1);
		return;
	}

	GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
	if (!pClan || !pClan->IsMaster(m_i64UID))
	{
		SendSimpleAck(PROTOCOL_CS_REPLACE_PERSONMAX_ACK, -2);
		return;
	}

	SendSimpleAck(PROTOCOL_CS_REPLACE_PERSONMAX_ACK, 0);
}

void GameSession::OnClanCheckJoinAuthorityReq(char* pData, INT32 i32Size)
{
	int32_t result = 0;

	if (m_i32ClanId > 0)
		result = -1;
	else if (m_i32Level < 5)
		result = -2;

	SendSimpleAck(PROTOCOL_CS_CHECK_JOIN_AUTHORITY_ACK, result);
}

void GameSession::OnClanCheckMarkReq(char* pData, INT32 i32Size)
{
	if (i32Size < (int)sizeof(int))
	{
		SendSimpleAck(PROTOCOL_CS_CHECK_MARK_ACK, -1);
		return;
	}

	int markId = 0;
	memcpy(&markId, pData, sizeof(int));

	SendSimpleAck(PROTOCOL_CS_CHECK_MARK_ACK, 0);
}

// ============================================================================
// Batch 19 - Clan CS_NEW handlers
// ============================================================================

void GameSession::OnClanNewDetailInfoReq(char* pData, INT32 i32Size)
{
	// New version of clan detail info request
	if (i32Size < (int)sizeof(int32_t))
	{
		SendSimpleAck(PROTOCOL_CS_NEW_DETAIL_INFO_ACK, 1);
		return;
	}

	int32_t clanId = 0;
	memcpy(&clanId, pData, sizeof(int32_t));

	if (!g_pClanManager)
	{
		SendSimpleAck(PROTOCOL_CS_NEW_DETAIL_INFO_ACK, 2);
		return;
	}

	GameClanInfo* pClan = g_pClanManager->FindClan(clanId);
	if (!pClan)
	{
		SendSimpleAck(PROTOCOL_CS_NEW_DETAIL_INFO_ACK, 3);
		return;
	}

	// Send detailed clan info
	i3NetworkPacket packet;
	int32_t result = 0;
	memcpy(buffer + offset, &pClan->i32ClanId, sizeof(int32_t));	offset += sizeof(int32_t);

	// Clan name
	char name[32] = {};
	strncpy(name, pClan->szName, 31);

	// Master nickname
	char masterNick[64] = {};
	strncpy(masterNick, pClan->szMasterNickname, 63);

	// Clan stats
	memcpy(buffer + offset, &pClan->i32MemberCount, 4);	offset += 4;
	memcpy(buffer + offset, &pClan->i32MaxMembers, 4);		offset += 4;
	memcpy(buffer + offset, &pClan->i32ClanRank, 4);		offset += 4;
	memcpy(buffer + offset, &pClan->i32ClanExp, 4);		offset += 4;
	memcpy(buffer + offset, &pClan->i32Wins, 4);			offset += 4;
	memcpy(buffer + offset, &pClan->i32Losses, 4);			offset += 4;
	memcpy(buffer + offset, &pClan->ui16MarkId, 2);		offset += 2;
	memcpy(buffer + offset, &pClan->ui8MarkColor, 1);		offset += 1;

	// Notice and intro
	char notice[256] = {};
	strncpy(notice, pClan->szNotice, 255);

	char intro[128] = {};
	strncpy(intro, pClan->szIntro, 127);
	i3NetworkPacket packet(PROTOCOL_CS_NEW_DETAIL_INFO_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(name, 32);
	packet.WriteData(masterNick, 64);
	packet.WriteData(notice, 128);
	packet.WriteData(intro, 64);
	SendPacketMessage(&packet);
}

void GameSession::OnClanNewCreateClanReq(char* pData, INT32 i32Size)
{
	// New version of clan creation request
	// Delegates to existing create logic with additional fields
	if (i32Size < 32)
	{
		SendSimpleAck(PROTOCOL_CS_NEW_CREATE_CLAN_ACK, 1);
		return;
	}

	if (m_i32ClanId > 0)
	{
		SendSimpleAck(PROTOCOL_CS_NEW_CREATE_CLAN_ACK, 2);	// Already in a clan
		return;
	}

	// Parse clan name
	char clanName[32] = {};
	memcpy(clanName, pData, 32);
	clanName[31] = '\0';

	// Validate name length
	int nameLen = (int)strlen(clanName);
	if (nameLen < 2 || nameLen > 16)
	{
		SendSimpleAck(PROTOCOL_CS_NEW_CREATE_CLAN_ACK, 3);
		return;
	}

	int32_t createCost  = g_pContextMain ? g_pContextMain->m_i32ClanCreateCostGP   : 50000;
	int32_t minLevel    = g_pContextMain ? g_pContextMain->m_i32ClanCreateMinLevel : 5;

	if (m_i32GP < createCost)
	{
		SendSimpleAck(PROTOCOL_CS_NEW_CREATE_CLAN_ACK, 4);	// Not enough GP
		return;
	}

	if (m_i32Level < minLevel)
	{
		SendSimpleAck(PROTOCOL_CS_NEW_CREATE_CLAN_ACK, 5);	// Level too low
		return;
	}

	// Delegate to DataServer for creation
	if (g_pModuleDataServer)
	{
		g_pModuleDataServer->RequestClanCreate(m_i64UID, GetIndex(), clanName, m_szNickname);
		// Response comes async
	}
	else
	{
		// Offline mode: create locally
		if (g_pClanManager)
		{
			int newClanId = g_pClanManager->CreateClan(clanName, m_i64UID, m_szNickname, m_i32Level, m_i32RankId);
			if (newClanId > 0)
			{
				m_i32ClanId = newClanId;
				m_i32GP -= createCost;
				SendSimpleAck(PROTOCOL_CS_NEW_CREATE_CLAN_ACK, 0);
			}
			else
			{
				SendSimpleAck(PROTOCOL_CS_NEW_CREATE_CLAN_ACK, 6);	// Name taken
			}
		}
	}
}

void GameSession::OnClanNewRequestListReq(char* pData, INT32 i32Size)
{
	// View list of pending join requests for the clan
	if (m_i32ClanId <= 0)
	{
		SendSimpleAck(PROTOCOL_CS_NEW_REQUEST_LIST_ACK, 1);
		return;
	}

	if (!g_pClanManager)
	{
		SendSimpleAck(PROTOCOL_CS_NEW_REQUEST_LIST_ACK, 2);
		return;
	}

	GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
	if (!pClan)
	{
		SendSimpleAck(PROTOCOL_CS_NEW_REQUEST_LIST_ACK, 3);
		return;
	}

	// Only master/staff can view requests
	if (!pClan->IsStaffOrMaster(m_i64UID))
	{
		SendSimpleAck(PROTOCOL_CS_NEW_REQUEST_LIST_ACK, 4);
		return;
	}

	int32_t result = 0;
	uint8_t requestCount = (uint8_t)pClan->i32JoinRequestCount;

	i3NetworkPacket packet(PROTOCOL_CS_NEW_REQUEST_LIST_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&requestCount, sizeof(uint8_t));

	for (int i = 0; i < MAX_CLAN_JOIN_REQUESTS; i++)
	{
		if (!pClan->joinRequests[i].bActive)
			continue;
		const ClanJoinRequest& req = pClan->joinRequests[i];
		packet.WriteData(&req.i64UID, sizeof(int64_t));
		packet.WriteData(req.szNickname, 64);
		packet.WriteData(&req.i32Level, sizeof(int32_t));
		packet.WriteData(&req.i32RankId, sizeof(int32_t));
	}

	SendPacketMessage(&packet);
}

// ============================================================================
// Batch 20 - CS_* Clan context/operations handlers
// ============================================================================

void GameSession::OnClanContextReq(char* pData, INT32 i32Size)
{
	// Clan list context info (clan directory)
	OnClanListReq(pData, i32Size);	// Delegate to existing handler
}

void GameSession::OnClanClientContextReq(char* pData, INT32 i32Size)
{
	// Client's own clan context - enter clan page
	if (m_i32ClanId <= 0)
	{
		SendSimpleAck(PROTOCOL_CS_CLIENT_CLAN_CONTEXT_ACK, 1);
		return;
	}

	// Delegate to detail info with own clan
	OnClanDetailInfoReq(pData, i32Size);
}

void GameSession::OnClanCreateConditionReq(char* pData, INT32 i32Size)
{
	// Check clan creation conditions
	i3NetworkPacket packet;
	int32_t result = 0;
	// Check conditions
	if (m_i32ClanId > 0)
		result = 1;		// Already in clan
	else if (m_i32Level < (g_pContextMain ? g_pContextMain->m_i32ClanCreateMinLevel : 5))
		result = 2;		// Level too low
	else if (m_i32GP < (g_pContextMain ? g_pContextMain->m_i32ClanCreateCostGP : 50000))
		result = 3;		// Not enough GP
	i3NetworkPacket packet(PROTOCOL_CS_CREATE_CLAN_CONDITION_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	SendPacketMessage(&packet);
}

void GameSession::OnClanRequestContextReq(char* pData, INT32 i32Size)
{
	// Clan join request list context - delegate to request list
	OnClanRequestListReq(pData, i32Size);
}

void GameSession::OnClanRequestInfoReq(char* pData, INT32 i32Size)
{
	// Clan join request detail info
	if (i32Size < (int)sizeof(int64_t))
	{
		SendSimpleAck(PROTOCOL_CS_REQUEST_INFO_ACK, 1);
		return;
	}

	int64_t requestUID = 0;
	memcpy(&requestUID, pData, sizeof(int64_t));

	// Look up the requesting user's info
	GameSession* pRequester = g_pGameSessionManager ? g_pGameSessionManager->FindSessionByUID(requestUID) : nullptr;

	i3NetworkPacket packet;
	int32_t result = pRequester ? 0 : 1;

	if (pRequester)
	{
		char nick[64] = {};
		strncpy(nick, pRequester->GetNickname(), 63);
		int32_t level = pRequester->GetLevel();
		int32_t rankId = pRequester->GetRankId();
	}
	i3NetworkPacket packet(PROTOCOL_CS_REQUEST_INFO_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&requestUID, sizeof(int64_t));
	packet.WriteData(nick, 64);
	packet.WriteData(&level, 4);
	packet.WriteData(&rankId, 4);
	SendPacketMessage(&packet);
}

void GameSession::OnClanMemberContextReq(char* pData, INT32 i32Size)
{
	// Clan member list context - delegate to existing member list
	OnClanMemberListReq(pData, i32Size);
}

void GameSession::OnClanStaffAuthorityReq(char* pData, INT32 i32Size)
{
	// Manage staff authority (promote/demote)
	if (m_i32ClanId <= 0 || !g_pClanManager)
	{
		SendSimpleAck(PROTOCOL_CS_STAFF_AUTHORITY_ACK, 1);
		return;
	}

	if (i32Size < (int)(sizeof(int64_t) + 1))
	{
		SendSimpleAck(PROTOCOL_CS_STAFF_AUTHORITY_ACK, 2);
		return;
	}

	int64_t targetUID = 0;
	uint8_t newRole = 0;
	memcpy(&targetUID, pData, sizeof(int64_t));
	memcpy(&newRole, pData + sizeof(int64_t), 1);

	GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
	if (!pClan || !pClan->IsMaster(m_i64UID))
	{
		SendSimpleAck(PROTOCOL_CS_STAFF_AUTHORITY_ACK, 3);	// Not master
		return;
	}

	// Validate role
	if (newRole > CLAN_MEMBER_REGULAR)
	{
		SendSimpleAck(PROTOCOL_CS_STAFF_AUTHORITY_ACK, 4);
		return;
	}

	// Find and update member role
	GameClanMember* pMember = pClan->FindMember(targetUID);
	if (!pMember)
	{
		SendSimpleAck(PROTOCOL_CS_STAFF_AUTHORITY_ACK, 5);
		return;
	}

	pMember->ui8Role = newRole;
	SendSimpleAck(PROTOCOL_CS_STAFF_AUTHORITY_ACK, 0);
}

void GameSession::OnClanRoomInvitedReq(char* pData, INT32 i32Size)
{
	// Invite clan member to room
	if (i32Size < (int)sizeof(int64_t))
	{
		SendSimpleAck(PROTOCOL_CS_ROOM_INVITED_ACK, 1);
		return;
	}

	int64_t targetUID = 0;
	memcpy(&targetUID, pData, sizeof(int64_t));

	if (m_i32RoomIdx < 0)
	{
		SendSimpleAck(PROTOCOL_CS_ROOM_INVITED_ACK, 2);	// Not in a room
		return;
	}

	GameSession* pTarget = g_pGameSessionManager ? g_pGameSessionManager->FindSessionByUID(targetUID) : nullptr;
	if (!pTarget)
	{
		SendSimpleAck(PROTOCOL_CS_ROOM_INVITED_ACK, 3);
		return;
	}

	// Send invite notification to target
	i3NetworkPacket packet;
	char nick[64] = {};
	strncpy(nick, m_szNickname, 63);
	i3NetworkPacket packet(PROTOCOL_CS_ROOM_INVITED_RESULT_ACK);
	packet.WriteData(&m_i64UID, sizeof(int64_t));
	packet.WriteData(nick, 64);
	packet.WriteData(&m_i32ChannelNum, sizeof(int32_t));
	packet.WriteData(&m_i32RoomIdx, sizeof(int32_t));
	pTarget->SendPacketMessage(&packet);

	SendSimpleAck(PROTOCOL_CS_ROOM_INVITED_ACK, 0);
}

void GameSession::OnClanExpUpReq(char* pData, INT32 i32Size)
{
	// Clan EXP increase (via item)
	if (m_i32ClanId <= 0 || !g_pClanManager)
	{
		SendSimpleAck(PROTOCOL_CS_EXPUP_ACK, 1);
		return;
	}

	GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
	if (!pClan)
	{
		SendSimpleAck(PROTOCOL_CS_EXPUP_ACK, 2);
		return;
	}

	// Add EXP to clan (amount from item, default 1000)
	int32_t expAmount = 1000;
	if (i32Size >= (int)sizeof(int32_t))
		memcpy(&expAmount, pData, sizeof(int32_t));

	pClan->i32ClanExp += expAmount;
	SendSimpleAck(PROTOCOL_CS_EXPUP_ACK, 0);
}

void GameSession::OnClanReplaceMemberNickReq(char* pData, INT32 i32Size)
{
	// Update clan member's nickname (after nickname change)
	SendSimpleAck(PROTOCOL_CS_REPLACE_MEMBER_NICK_ACK, 0);
}

void GameSession::OnClanReplaceMemberRankReq(char* pData, INT32 i32Size)
{
	// Update clan member's rank display
	SendSimpleAck(PROTOCOL_CS_REPLACE_MEMBER_RANK_ACK, 0);
}

void GameSession::OnClanReplaceMemberColorNickReq(char* pData, INT32 i32Size)
{
	// Update clan member's color nickname
	SendSimpleAck(PROTOCOL_CS_REPLACE_MEMBER_COLOR_NICK_ACK, 0);
}

void GameSession::OnClanReplaceManagementReq(char* pData, INT32 i32Size)
{
	// Clan management settings change
	if (m_i32ClanId <= 0 || !g_pClanManager)
	{
		SendSimpleAck(PROTOCOL_CS_REPLACE_MANAGEMENT_ACK, 1);
		return;
	}

	GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
	if (!pClan || !pClan->IsMaster(m_i64UID))
	{
		SendSimpleAck(PROTOCOL_CS_REPLACE_MANAGEMENT_ACK, 2);
		return;
	}

	SendSimpleAck(PROTOCOL_CS_REPLACE_MANAGEMENT_ACK, 0);
}

void GameSession::OnClanListStreamReq(char* pData, INT32 i32Size)
{
	// Clan list streaming (continued pagination)
	OnClanListReq(pData, i32Size);	// Delegate
}

void GameSession::OnClanListGsReq(char* pData, INT32 i32Size)
{
	// Individual clan list refresh from GameServer
	OnClanListReq(pData, i32Size);	// Delegate
}

void GameSession::OnClanInoRankUpdateReq(char* pData, INT32 i32Size)
{
	// Update own rank in clan list display - fire and forget
}

void GameSession::OnClanItemDeleteReq(char* pData, INT32 i32Size)
{
	// Delete clan item (exp boost, color name item)
	// Fire and forget - item removal from clan
}

void GameSession::OnClanCheckDuplicateAzitUrlReq(char* pData, INT32 i32Size)
{
	// Check duplicate Azit (clan homepage) URL
	SendSimpleAck(PROTOCOL_CS_CHECK_DUPLICATE_AZIT_URL_ACK, 0);	// Always available
}

void GameSession::OnClanMatchResultContextReq(char* pData, INT32 i32Size)
{
	// Clan match result history context
	i3NetworkPacket packet;
	int32_t result = 0;
	uint8_t matchCount = 0;	// No match history yet
	i3NetworkPacket packet(PROTOCOL_CS_CLAN_MATCH_RESULT_CONTEXT_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&matchCount, 1);
	SendPacketMessage(&packet);
}

void GameSession::OnClanMatchResultListReq(char* pData, INT32 i32Size)
{
	// Clan match result detailed list
	i3NetworkPacket packet;
	int32_t result = 0;
	uint8_t count = 0;
	i3NetworkPacket packet(PROTOCOL_CS_CLAN_MATCH_RESULT_LIST_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&count, 1);
	SendPacketMessage(&packet);
}

void GameSession::OnClanMatchResultEmotionSaveReq(char* pData, INT32 i32Size)
{
	// Save emotion/reaction on clan match result (like button)
	// Fire and forget
}

void GameSession::OnClanRequestAutoAcceptReq(char* pData, INT32 i32Size)
{
	// Toggle auto-accept join requests
	if (m_i32ClanId <= 0)
	{
		SendSimpleAck(PROTOCOL_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_REQ + 1, 1);
		return;
	}
	SendSimpleAck(PROTOCOL_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_REQ + 1, 0);
}
