#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "GameSessionManager.h"

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
		char notifyBuf[128];
		int nOffset = 0;

		uint16_t nSz = 0;
		uint16_t nProto = PROTOCOL_GMCHAT_START_CHAT_ACK;
		nOffset += sizeof(uint16_t);
		memcpy(notifyBuf + nOffset, &nProto, sizeof(uint16_t));		nOffset += sizeof(uint16_t);

		int32_t nResult = 0;
		memcpy(notifyBuf + nOffset, &nResult, sizeof(int32_t));		nOffset += sizeof(int32_t);

		// GM nickname
		memcpy(notifyBuf + nOffset, m_szNickname, 64);				nOffset += 64;

		nSz = (uint16_t)nOffset;
		memcpy(notifyBuf, &nSz, sizeof(uint16_t));

		notifyPacket.SetPacketData(notifyBuf, nOffset);
		pTarget->SendMessage(&notifyPacket);

		printf("[GMChat] GM '%s' started chat with '%s'\n", m_szNickname, targetNick);
	}

	// Send ACK to GM
	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_GMCHAT_START_CHAT_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, targetNick, 64);				offset += 64;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
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
		char recvBuf[512];
		int rOffset = 0;

		uint16_t rSz = 0;
		uint16_t rProto = PROTOCOL_GMCHAT_SEND_CHAT_ACK;
		rOffset += sizeof(uint16_t);
		memcpy(recvBuf + rOffset, &rProto, sizeof(uint16_t));		rOffset += sizeof(uint16_t);

		int32_t rResult = 0;
		memcpy(recvBuf + rOffset, &rResult, sizeof(int32_t));		rOffset += sizeof(int32_t);

		// Sender nickname
		memcpy(recvBuf + rOffset, m_szNickname, 64);				rOffset += 64;

		// Message
		memcpy(recvBuf + rOffset, &msgLen, sizeof(uint16_t));		rOffset += sizeof(uint16_t);
		memcpy(recvBuf + rOffset, pMsg, msgLen);					rOffset += msgLen;

		rSz = (uint16_t)rOffset;
		memcpy(recvBuf, &rSz, sizeof(uint16_t));

		recvPacket.SetPacketData(recvBuf, rOffset);
		pTarget->SendMessage(&recvPacket);
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
		char endBuf[128];
		int eOffset = 0;

		uint16_t eSz = 0;
		uint16_t eProto = PROTOCOL_GMCHAT_END_CHAT_ACK;
		eOffset += sizeof(uint16_t);
		memcpy(endBuf + eOffset, &eProto, sizeof(uint16_t));		eOffset += sizeof(uint16_t);

		int32_t eResult = 0;
		memcpy(endBuf + eOffset, &eResult, sizeof(int32_t));		eOffset += sizeof(int32_t);
		memcpy(endBuf + eOffset, m_szNickname, 64);				eOffset += 64;

		eSz = (uint16_t)eOffset;
		memcpy(endBuf, &eSz, sizeof(uint16_t));

		endPacket.SetPacketData(endBuf, eOffset);
		pTarget->SendMessage(&endPacket);
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

		case 1:	// Mute (TODO: implement chat mute timer)
			printf("[GMChat] GM '%s' muted '%s' for %u minutes: %s\n",
				m_szNickname, targetNick, duration, reason);
			break;

		case 2:	// Kick
			printf("[GMChat] GM '%s' kicked '%s': %s\n", m_szNickname, targetNick, reason);
			// Send kick message then disconnect
			{
				i3NetworkPacket kickPacket;
				char kickBuf[32];
				int kOffset = 0;

				uint16_t kSz = 0;
				uint16_t kProto = PROTOCOL_SERVER_MESSAGE_KICK_PLAYER;
				kOffset += sizeof(uint16_t);
				memcpy(kickBuf + kOffset, &kProto, sizeof(uint16_t));	kOffset += sizeof(uint16_t);

				int32_t kickResult = 0;
				memcpy(kickBuf + kOffset, &kickResult, sizeof(int32_t));	kOffset += sizeof(int32_t);

				kSz = (uint16_t)kOffset;
				memcpy(kickBuf, &kSz, sizeof(uint16_t));

				kickPacket.SetPacketData(kickBuf, kOffset);
				pTarget->SendMessage(&kickPacket);
			}
			break;

		case 3:	// Ban (TODO: persist in database)
			printf("[GMChat] GM '%s' banned '%s' for %u minutes: %s\n",
				m_szNickname, targetNick, duration, reason);
			break;

		default:
			result = 4;	// Invalid penalty type
			break;
		}

		// Notify target of penalty
		if (result == 0 && penaltyType > 0)
		{
			i3NetworkPacket penaltyNotify;
			char penBuf[128];
			int pOffset = 0;

			uint16_t pSz = 0;
			uint16_t pProto = PROTOCOL_GMCHAT_NOTI_USER_PENALTY_ACK;
			pOffset += sizeof(uint16_t);
			memcpy(penBuf + pOffset, &pProto, sizeof(uint16_t));		pOffset += sizeof(uint16_t);

			int32_t pResult = 0;
			memcpy(penBuf + pOffset, &pResult, sizeof(int32_t));		pOffset += sizeof(int32_t);
			memcpy(penBuf + pOffset, &penaltyType, 1);					pOffset += 1;
			memcpy(penBuf + pOffset, &duration, sizeof(uint32_t));		pOffset += sizeof(uint32_t);

			pSz = (uint16_t)pOffset;
			memcpy(penBuf, &pSz, sizeof(uint16_t));

			penaltyNotify.SetPacketData(penBuf, pOffset);
			pTarget->SendMessage(&penaltyNotify);
		}
	}

	// ACK to GM
	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_GMCHAT_APPLY_PENALTY_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, targetNick, 64);				offset += 64;
	memcpy(buffer + offset, &penaltyType, 1);				offset += 1;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}
