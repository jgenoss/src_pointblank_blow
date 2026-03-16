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
	else if (m_i32GP < CLAN_CREATE_COST_GP)
	{
		result = 2;		// Not enough GP
	}
	else if (m_i32Level < 5)
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
			m_i32GP -= CLAN_CREATE_COST_GP;
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
	char buffer[64];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_CS_CREATE_CLAN_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &clanId, sizeof(int32_t));		offset += sizeof(int32_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
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
	char buffer[512];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_CS_DETAIL_INFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	if (reqClanId <= 0 || !g_pClanManager)
	{
		result = 1;
		memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
	}
	else
	{
		GameClanInfo* pClan = g_pClanManager->FindClan(reqClanId);
		if (!pClan)
		{
			result = 2;
			memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
		}
		else
		{
			memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

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
			memcpy(buffer + offset, &noticeLen, 2);				offset += 2;
			if (noticeLen > 0)
			{
				memcpy(buffer + offset, pClan->szNotice, noticeLen);
				offset += noticeLen;
			}
		}
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnClanMemberListReq(char* pData, INT32 i32Size)
{
	// PROTOCOL_CS_MEMBER_LIST_REQ -> ACK
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	int32_t result = 0;

	i3NetworkPacket packet;
	char buffer[4096];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_CS_MEMBER_LIST_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	if (m_i32ClanId <= 0 || !g_pClanManager)
	{
		result = 1;
		memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
	}
	else
	{
		GameClanInfo* pClan = g_pClanManager->FindClan(m_i32ClanId);
		if (!pClan)
		{
			result = 2;
			memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
		}
		else
		{
			memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

			uint16_t memberCount = (uint16_t)pClan->i32MemberCount;
			memcpy(buffer + offset, &memberCount, 2);				offset += 2;

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
				memcpy(buffer + offset, &online, 1);				offset += 1;
			}
		}
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

void GameSession::OnClanJoinReq(char* pData, INT32 i32Size)
{
	// Join request to a clan (simplified - auto-accept)
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
		else
		{
			if (pClan->AddMember(m_i64UID, m_szNickname, CLAN_MEMBER_REGULAR,
				m_i32Level, m_i32RankId))
			{
				m_i32ClanId = targetClanId;
				printf("[GameSession] Joined clan - UID=%lld, ClanId=%d, Name=%s\n",
					m_i64UID, targetClanId, pClan->szName);
			}
			else
			{
				result = 3;
			}
		}
	}

	// Send join ACK
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_CS_JOIN_REQUEST_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &targetClanId, sizeof(int32_t));offset += sizeof(int32_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
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
	char buffer[512];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_CS_CHATTING_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// Sender nickname
	memcpy(buffer + offset, m_szNickname, 64);				offset += 64;

	// Message
	memcpy(buffer + offset, &msgLen, sizeof(uint16_t));		offset += sizeof(uint16_t);
	memcpy(buffer + offset, szMsg, msgLen);					offset += msgLen;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);

	// Broadcast to all online members
	for (int i = 0; i < MAX_CLAN_MEMBERS; i++)
	{
		if (!pClan->members[i].IsActive())
			continue;

		GameSession* pMember = g_pGameSessionManager->FindSessionByUID(pClan->members[i].i64UID);
		if (pMember && pMember->GetTask() >= GAME_TASK_CHANNEL)
			pMember->SendMessage(&packet);
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
	char buffer[4096];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_CS_CLIENT_CLAN_LIST_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

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

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
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
			// Find the requester online to get their info
			GameSession* pRequester = g_pGameSessionManager->FindSessionByUID(requesterUID);
			if (pRequester)
			{
				if (pRequester->GetClanId() > 0)
					result = -6;	// Already in a clan
				else
				{
					pClan->AddMember(requesterUID, pRequester->GetNickname(),
						CLAN_MEMBER_REGULAR, pRequester->GetLevel(), pRequester->GetRankId());
					pRequester->SetClanId(m_i32ClanId);

					// Notify the requester
					pRequester->SendSimpleAck(PROTOCOL_CS_ACCEPT_REQUEST_RESULT_ACK, 0);
				}
			}
			else
			{
				result = -7;	// Requester offline
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
				i3NetworkPacket invitePacket;
				char invBuf[128];
				int invOff = 0;

				uint16_t invSz = 0;
				uint16_t invProto = PROTOCOL_CS_INVITE_ACCEPT_REQ;	// Client shows invite dialog
				invOff += sizeof(uint16_t);
				memcpy(invBuf + invOff, &invProto, sizeof(uint16_t));	invOff += sizeof(uint16_t);
				memcpy(invBuf + invOff, &m_i32ClanId, 4);				invOff += 4;
				memcpy(invBuf + invOff, m_szClanName, MAX_CLAN_NAME_LEN);	invOff += MAX_CLAN_NAME_LEN;
				memcpy(invBuf + invOff, m_szNickname, 64);				invOff += 64;

				invSz = (uint16_t)invOff;
				memcpy(invBuf, &invSz, sizeof(uint16_t));

				invitePacket.SetPacketData(invBuf, invOff);
				pTarget->SendMessage(&invitePacket);
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

	// In this simplified version, there's no pending request queue
	// Send empty list
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_CS_REQUEST_LIST_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	uint16_t requestCount = 0;
	memcpy(buffer + offset, &requestCount, sizeof(uint16_t));	offset += sizeof(uint16_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
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
	char buffer[256];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_CS_MEMBER_DETAIL_INFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

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
				memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
				memcpy(buffer + offset, &pMember->i64UID, 8);		offset += 8;
				memcpy(buffer + offset, pMember->szNickname, 64);	offset += 64;
				memcpy(buffer + offset, &pMember->ui8Level, 1);	offset += 1;
				memcpy(buffer + offset, &pMember->i32PlayerLevel, 4);	offset += 4;
				memcpy(buffer + offset, &pMember->i32RankId, 4);	offset += 4;

				uint8_t online = pMember->bOnline ? 1 : 0;
				memcpy(buffer + offset, &online, 1);				offset += 1;

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
				memcpy(buffer + offset, &kills, 4);		offset += 4;
				memcpy(buffer + offset, &deaths, 4);	offset += 4;
				memcpy(buffer + offset, &wins, 4);		offset += 4;
				memcpy(buffer + offset, &losses, 4);	offset += 4;
			}
		}
	}

	if (result != 0)
	{
		memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
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
	char buffer[512];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_CS_PAGE_CHATTING_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	// Sender info
	memcpy(buffer + offset, &m_i64UID, 8);				offset += 8;
	memcpy(buffer + offset, m_szNickname, 64);			offset += 64;

	// Message
	memcpy(buffer + offset, &msgLen, sizeof(uint16_t));	offset += sizeof(uint16_t);
	memcpy(buffer + offset, pData + 2, msgLen);			offset += msgLen;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);

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
				pMember->SendMessage(&packet);
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
	char buffer[512];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_CS_CLAN_LIST_DETAIL_INFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = -1;

	if (g_pClanManager)
	{
		GameClanInfo* pClan = g_pClanManager->FindClan(clanId);
		if (pClan)
		{
			result = 0;
			memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
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
			memcpy(buffer + offset, &noticeLen, 2);				offset += 2;
			if (noticeLen > 0)
			{
				memcpy(buffer + offset, pClan->szNotice, noticeLen);	offset += noticeLen;
			}

			// Intro
			uint16_t introLen = (uint16_t)strlen(pClan->szIntro);
			memcpy(buffer + offset, &introLen, 2);				offset += 2;
			if (introLen > 0)
			{
				memcpy(buffer + offset, pClan->szIntro, introLen);	offset += introLen;
			}
		}
	}

	if (result != 0)
	{
		memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
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

	i3NetworkPacket notePacket;
	char noteBuf[600];
	int noteOff = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_CS_NOTE_ACK;
	noteOff += sizeof(uint16_t);
	memcpy(noteBuf + noteOff, &proto, sizeof(uint16_t));	noteOff += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(noteBuf + noteOff, &result, sizeof(int32_t));	noteOff += sizeof(int32_t);

	char senderNick[64] = {};
	strncpy(senderNick, m_szNickname, 63);
	memcpy(noteBuf + noteOff, senderNick, 64);				noteOff += 64;

	uint16_t contentLen = (uint16_t)(i32Size > 480 ? 480 : i32Size);
	memcpy(noteBuf + noteOff, &contentLen, 2);				noteOff += 2;
	memcpy(noteBuf + noteOff, pData, contentLen);			noteOff += contentLen;

	sz = (uint16_t)noteOff;
	memcpy(noteBuf, &sz, sizeof(uint16_t));
	notePacket.SetPacketData(noteBuf, noteOff);

	if (g_pGameSessionManager)
	{
		for (int i = 0; i < MAX_CLAN_MEMBERS; i++)
		{
			if (pClan->members[i].i64UID > 0)
			{
				GameSession* pTarget = g_pGameSessionManager->FindSessionByUID(pClan->members[i].i64UID);
				if (pTarget && pTarget != this)
					pTarget->SendMessage(&notePacket);
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
	char buffer[512];
	int offset = 0;

	uint16_t sz = 0;
	offset += sizeof(uint16_t);
	uint16_t proto = PROTOCOL_CS_NEW_DETAIL_INFO_ACK;
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);
	memcpy(buffer + offset, &pClan->i32ClanId, sizeof(int32_t));	offset += sizeof(int32_t);

	// Clan name
	char name[32] = {};
	strncpy(name, pClan->szName, 31);
	memcpy(buffer + offset, name, 32);	offset += 32;

	// Master nickname
	char masterNick[64] = {};
	strncpy(masterNick, pClan->szMasterNickname, 63);
	memcpy(buffer + offset, masterNick, 64);	offset += 64;

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
	memcpy(buffer + offset, notice, 128);	offset += 128;	// Truncated to 128 for packet

	char intro[128] = {};
	strncpy(intro, pClan->szIntro, 127);
	memcpy(buffer + offset, intro, 64);		offset += 64;	// Truncated to 64

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
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

	// Check GP cost (TODO: move to config in Phase 8.8)
	int32_t createCost = 50000;

	if (m_i32GP < createCost)
	{
		SendSimpleAck(PROTOCOL_CS_NEW_CREATE_CLAN_ACK, 4);	// Not enough GP
		return;
	}

	// Check level requirement (TODO: move to config in Phase 8.8)
	int32_t minLevel = 5;

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

	// Send empty request list (request system not fully implemented)
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	offset += sizeof(uint16_t);
	uint16_t proto = PROTOCOL_CS_NEW_REQUEST_LIST_ACK;
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	uint8_t requestCount = 0;
	memcpy(buffer + offset, &requestCount, 1);	offset += 1;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}
