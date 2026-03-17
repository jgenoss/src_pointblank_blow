#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "GameSessionManager.h"
#include "ModuleDataServer.h"

// ============================================================================
// GM Chat Handlers (Protocol_GMCHAT 0x1A00)
// GM 1:1 chat with players and penalty system
// ============================================================================

// GM auth level required for GM chat features
#define GM_AUTH_LEVEL_CHAT		1		// Level 1+ can start GM chat
#define GM_AUTH_LEVEL_PENALTY	2		// Level 2+ can apply penalties

void GameSession::OnGMChatStartReq(char* pData, INT32 i32Size)
{
	// GM starts 1:1 chat with a player
	// Parse: targetNick(64)
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (m_ui8AuthLevel < GM_AUTH_LEVEL_CHAT)
	{
		SendSimpleAck(PROTOCOL_GMCHAT_START_CHAT_ACK, 1);	// Not authorized
		return;
	}

	if (i32Size < 64)
	{
		SendSimpleAck(PROTOCOL_GMCHAT_START_CHAT_ACK, 2);	// Invalid data
		return;
	}

	char targetNick[64];
	memcpy(targetNick, pData, 64);
	targetNick[63] = '\0';

	int32_t result = 0;

	// Find target player
	GameSession* pTarget = nullptr;
	if (g_pGameSessionManager)
		pTarget = g_pGameSessionManager->FindSessionByNickname(targetNick);

	if (!pTarget)
	{
		result = 3;		// Player not found/offline
	}
	else
	{
		// Notify target that GM wants to chat
		i3NetworkPacket notifyPacket;
		nOffset += sizeof(uint16_t);

		int32_t nResult = 0;

		// GM nickname
		i3NetworkPacket notifyPacket(PROTOCOL_GMCHAT_START_CHAT_ACK);
		notifyPacket.WriteData(&nResult, sizeof(int32_t));
		notifyPacket.WriteData(m_szNickname, 64);
		pTarget->SendPacketMessage(&notifyPacket);

		printf("[GMChat] GM '%s' started chat with '%s'\n", m_szNickname, targetNick);
	}

	// Send ACK to GM
	i3NetworkPacket packet;
	offset += sizeof(uint16_t);
	i3NetworkPacket packet(PROTOCOL_GMCHAT_START_CHAT_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(targetNick, 64);
	SendPacketMessage(&packet);
}

void GameSession::OnGMChatSendReq(char* pData, INT32 i32Size)
{
	// GM sends message to player (or player replies to GM)
	// Parse: targetNick(64) + msgLen(2) + message(variable)
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 66)
		return;

	char targetNick[64];
	memcpy(targetNick, pData, 64);
	targetNick[63] = '\0';

	uint16_t msgLen = *(uint16_t*)(pData + 64);
	if (msgLen > 256) msgLen = 256;
	if (i32Size < (int)(66 + msgLen))
		return;

	char* pMsg = pData + 66;
	int32_t result = 0;

	// Find target
	GameSession* pTarget = nullptr;
	if (g_pGameSessionManager)
		pTarget = g_pGameSessionManager->FindSessionByNickname(targetNick);

	if (!pTarget)
	{
		result = 1;		// Target offline
	}
	else
	{
		// Forward message to target
		i3NetworkPacket recvPacket;
		rOffset += sizeof(uint16_t);

		int32_t rResult = 0;

		// Sender nickname

		// Message
		memcpy(recvBuf + rOffset, pMsg, msgLen);					rOffset += msgLen;
		i3NetworkPacket recvPacket(PROTOCOL_GMCHAT_SEND_CHAT_ACK);
		recvPacket.WriteData(&rResult, sizeof(int32_t));
		recvPacket.WriteData(m_szNickname, 64);
		recvPacket.WriteData(&msgLen, sizeof(uint16_t));
		pTarget->SendPacketMessage(&recvPacket);
	}

	// ACK to sender
	SendSimpleAck(PROTOCOL_GMCHAT_SEND_CHAT_ACK, result);
}

void GameSession::OnGMChatEndReq(char* pData, INT32 i32Size)
{
	// End 1:1 GM chat session
	// Parse: targetNick(64)
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (i32Size < 64)
	{
		SendSimpleAck(PROTOCOL_GMCHAT_END_CHAT_ACK, 1);
		return;
	}

	char targetNick[64];
	memcpy(targetNick, pData, 64);
	targetNick[63] = '\0';

	// Notify target that chat ended
	GameSession* pTarget = nullptr;
	if (g_pGameSessionManager)
		pTarget = g_pGameSessionManager->FindSessionByNickname(targetNick);

	if (pTarget)
	{
		i3NetworkPacket endPacket;
		eOffset += sizeof(uint16_t);

		int32_t eResult = 0;
		i3NetworkPacket endPacket(PROTOCOL_GMCHAT_END_CHAT_ACK);
		endPacket.WriteData(&eResult, sizeof(int32_t));
		endPacket.WriteData(m_szNickname, 64);
		pTarget->SendPacketMessage(&endPacket);
	}

	printf("[GMChat] GM '%s' ended chat with '%s'\n", m_szNickname, targetNick);
	SendSimpleAck(PROTOCOL_GMCHAT_END_CHAT_ACK, 0);
}

void GameSession::OnGMChatPenaltyReq(char* pData, INT32 i32Size)
{
	// GM applies penalty to a player
	// Parse: targetNick(64) + penaltyType(1) + duration(4) + reason(128)
	if (m_eMainTask < GAME_TASK_CHANNEL)
		return;

	if (m_ui8AuthLevel < GM_AUTH_LEVEL_PENALTY)
	{
		SendSimpleAck(PROTOCOL_GMCHAT_APPLY_PENALTY_ACK, 1);	// Not authorized
		return;
	}

	if (i32Size < 69)
	{
		SendSimpleAck(PROTOCOL_GMCHAT_APPLY_PENALTY_ACK, 2);	// Invalid data
		return;
	}

	char targetNick[64];
	memcpy(targetNick, pData, 64);
	targetNick[63] = '\0';

	uint8_t penaltyType = *(uint8_t*)(pData + 64);		// 0=warn, 1=mute, 2=kick, 3=ban
	uint32_t duration = *(uint32_t*)(pData + 65);		// Duration in minutes (0=permanent for ban)

	char reason[128] = {0};
	if (i32Size >= 197)
	{
		memcpy(reason, pData + 69, 128);
		reason[127] = '\0';
	}

	int32_t result = 0;

	GameSession* pTarget = nullptr;
	if (g_pGameSessionManager)
		pTarget = g_pGameSessionManager->FindSessionByNickname(targetNick);

	if (!pTarget)
	{
		result = 3;		// Player not found
	}
	else
	{
		switch (penaltyType)
		{
		case 0:	// Warning
			{
				// Send warning message to target
				char warnMsg[256];
				int len = sprintf_s(warnMsg, "[GM Warning] %s", reason[0] ? reason : "You have been warned by a GM.");
				pTarget->SendServerAnnounce(warnMsg, (uint16_t)len);
				printf("[GMChat] GM '%s' warned '%s': %s\n", m_szNickname, targetNick, reason);
			}
			break;

		case 1:	// Mute
			{
				DWORD dwDurationMs = (duration > 0) ? (DWORD)(duration * 60000u) : (DWORD)(60000u);
				pTarget->SetMuteExpireTime(GetTickCount() + dwDurationMs);
				printf("[GMChat] GM '%s' muted '%s' for %u minutes: %s\n",
					m_szNickname, targetNick, duration, reason);
				// Notify target
				char muteMsg[128];
				int mLen = sprintf_s(muteMsg, "[GM] You have been muted for %u minute(s).", duration > 0 ? duration : 1u);
				pTarget->SendServerAnnounce(muteMsg, (uint16_t)mLen);
			}
			break;

		case 2:	// Kick
			printf("[GMChat] GM '%s' kicked '%s': %s\n", m_szNickname, targetNick, reason);
			{
				i3NetworkPacket kickPacket;
				kOffset += sizeof(uint16_t);

				int32_t kickResult = 0;
				i3NetworkPacket kickPacket(PROTOCOL_SERVER_MESSAGE_KICK_PLAYER);
				kickPacket.WriteData(&kickResult, sizeof(int32_t));
				pTarget->SendPacketMessage(&kickPacket);
				pTarget->OnDisconnect(TRUE);
			}
			break;

		case 3:	// Ban
			printf("[GMChat] GM '%s' banned '%s' for %u minutes: %s\n",
				m_szNickname, targetNick, duration, reason);
			if (g_pModuleDataServer && g_pModuleDataServer->IsConnected())
				g_pModuleDataServer->RequestPlayerBan(pTarget->GetUID(), m_i64UID, (int)(duration * 60), reason);
			break;

		default:
			result = 4;	// Invalid penalty type
			break;
		}

		// Notify target of penalty
		if (result == 0 && penaltyType > 0)
		{
			i3NetworkPacket penaltyNotify;
			pOffset += sizeof(uint16_t);

			int32_t pResult = 0;
			i3NetworkPacket penaltyNotify(PROTOCOL_GMCHAT_NOTI_USER_PENALTY_ACK);
			penaltyNotify.WriteData(&pResult, sizeof(int32_t));
			penaltyNotify.WriteData(&penaltyType, 1);
			penaltyNotify.WriteData(&duration, sizeof(uint32_t));
			pTarget->SendPacketMessage(&penaltyNotify);
		}
	}

	// ACK to GM
	i3NetworkPacket packet;
	offset += sizeof(uint16_t);
	i3NetworkPacket packet(PROTOCOL_GMCHAT_APPLY_PENALTY_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(targetNick, 64);
	packet.WriteData(&penaltyType, 1);
	SendPacketMessage(&packet);
}

// ============================================================================
// Mute helpers
// ============================================================================

bool GameSession::IsMuted() const
{
	if (m_dwMuteExpireTime == 0)
		return false;
	return (GetTickCount() < m_dwMuteExpireTime);
}

void GameSession::SetMuteExpireTime(DWORD dwExpireTime)
{
	m_dwMuteExpireTime = dwExpireTime;
}
