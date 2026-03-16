#include "pch.h"
#include "GameSessionManager.h"
#include "GameSession.h"
#include "GameContextMain.h"

#define SESSION_CHECK_COUNT		300		// Check N sessions per update

I3_CLASS_INSTANCE(GameSessionManager);

GameSessionManager::GameSessionManager()
	: m_pSessions(nullptr)
	, m_pChannelUserList(nullptr)
	, m_pcsChannelUser(nullptr)
	, m_pLobbyUserList(nullptr)
	, m_pcsLobbyUser(nullptr)
	, m_ui32ChannelCount(0)
	, m_i32ActiveCount(0)
	, m_i32SessionCheckIdx(0)
	, m_dwLastTimeoutCheck(0)
{
}

GameSessionManager::~GameSessionManager()
{
}

BOOL GameSessionManager::OnCreate(i3List* pSessionList)
{
	if (!g_pContextMain)
		return FALSE;

	int sessionCount = g_pContextMain->m_i32SessionCount;
	m_ui32ChannelCount = g_pContextMain->m_ui8ChannelCount;

	// Pre-allocate session pool
	m_pSessions = new GameSession[sessionCount];

	for (int i = 0; i < sessionCount; i++)
	{
		m_pSessions[i].Create();
		m_SessionList.AddTail(&m_pSessions[i]);
	}

	// Allocate per-channel structures
	m_pChannelUserList = new i3List*[m_ui32ChannelCount];
	m_pcsChannelUser = new i3Mutex*[m_ui32ChannelCount];
	m_pLobbyUserList = new i3List*[m_ui32ChannelCount];
	m_pcsLobbyUser = new i3Mutex*[m_ui32ChannelCount];

	for (uint32_t i = 0; i < m_ui32ChannelCount; i++)
	{
		m_pChannelUserList[i] = new i3List();
		m_pcsChannelUser[i] = new i3Mutex();
		m_pLobbyUserList[i] = new i3List();
		m_pcsLobbyUser[i] = new i3Mutex();
	}

	// Call parent with our session list
	i3NetworkSessionManager::OnCreate(&m_SessionList);

	printf("[GameSessionManager] Created - Sessions=%d, Channels=%d\n",
		sessionCount, m_ui32ChannelCount);

	return TRUE;
}

void GameSessionManager::OnUpdate()
{
	i3NetworkSessionManager::OnUpdate();

	// Periodic timeout check
	DWORD dwNow = GetTickCount();
	if (dwNow - m_dwLastTimeoutCheck > 5000)	// Every 5 seconds
	{
		CheckTimeouts();
		m_dwLastTimeoutCheck = dwNow;
	}
}

BOOL GameSessionManager::OnDestroy()
{
	// Cleanup per-channel structures
	for (uint32_t i = 0; i < m_ui32ChannelCount; i++)
	{
		if (m_pChannelUserList[i])	delete m_pChannelUserList[i];
		if (m_pcsChannelUser[i])	delete m_pcsChannelUser[i];
		if (m_pLobbyUserList[i])	delete m_pLobbyUserList[i];
		if (m_pcsLobbyUser[i])		delete m_pcsLobbyUser[i];
	}

	delete[] m_pChannelUserList;	m_pChannelUserList = nullptr;
	delete[] m_pcsChannelUser;		m_pcsChannelUser = nullptr;
	delete[] m_pLobbyUserList;		m_pLobbyUserList = nullptr;
	delete[] m_pcsLobbyUser;		m_pcsLobbyUser = nullptr;

	if (m_pSessions)
	{
		delete[] m_pSessions;
		m_pSessions = nullptr;
	}

	return i3NetworkSessionManager::OnDestroy();
}

ULONG_PTR GameSessionManager::ConnectSession_v(SOCKET Socket, struct sockaddr_in* pAddr)
{
	m_i32ActiveCount++;
	return i3NetworkSessionManager::ConnectSession_v(Socket, pAddr);
}

GameSession* GameSessionManager::GetSession(int i32Idx)
{
	if (!g_pContextMain || i32Idx < 0 || i32Idx >= g_pContextMain->m_i32SessionCount)
		return nullptr;

	return &m_pSessions[i32Idx];
}

GameSession* GameSessionManager::GetSession(int i32Idx, int64_t i64UID)
{
	GameSession* pSession = GetSession(i32Idx);
	if (!pSession)
		return nullptr;

	if (pSession->GetUID() != i64UID)
		return nullptr;

	return pSession;
}

int GameSessionManager::OnEnterChannel(GameSession* pSession, uint32_t ui32Channel)
{
	if (!pSession || ui32Channel >= m_ui32ChannelCount)
		return -1;

	uint16_t maxUsers = g_pContextMain ? g_pContextMain->m_ui16MaxUsersPerChannel : 200;

	m_pcsChannelUser[ui32Channel]->Lock();

	if (m_pChannelUserList[ui32Channel]->GetCount() >= maxUsers)
	{
		m_pcsChannelUser[ui32Channel]->Unlock();
		return -1;	// Channel full
	}

	m_pChannelUserList[ui32Channel]->AddTail(pSession);
	m_pcsChannelUser[ui32Channel]->Unlock();

	return 0;
}

void GameSessionManager::OnLeaveChannel(GameSession* pSession, uint32_t ui32Channel)
{
	if (!pSession || ui32Channel >= m_ui32ChannelCount)
		return;

	// Also leave lobby if in it
	OnLeaveLobby(pSession, ui32Channel);

	m_pcsChannelUser[ui32Channel]->Lock();
	m_pChannelUserList[ui32Channel]->Remove(pSession);
	m_pcsChannelUser[ui32Channel]->Unlock();
}

int GameSessionManager::GetChannelUserCount(uint32_t ui32Channel)
{
	if (ui32Channel >= m_ui32ChannelCount)
		return 0;

	return m_pChannelUserList[ui32Channel]->GetCount();
}

void GameSessionManager::OnEnterLobby(GameSession* pSession, uint32_t ui32Channel)
{
	if (!pSession || ui32Channel >= m_ui32ChannelCount)
		return;

	m_pcsLobbyUser[ui32Channel]->Lock();
	m_pLobbyUserList[ui32Channel]->AddTail(pSession);
	m_pcsLobbyUser[ui32Channel]->Unlock();
}

void GameSessionManager::OnLeaveLobby(GameSession* pSession, uint32_t ui32Channel)
{
	if (!pSession || ui32Channel >= m_ui32ChannelCount)
		return;

	m_pcsLobbyUser[ui32Channel]->Lock();
	m_pLobbyUserList[ui32Channel]->Remove(pSession);
	m_pcsLobbyUser[ui32Channel]->Unlock();
}

void GameSessionManager::OnSendLobbyChatting(GameSession* pSender, char* pMessage, uint16_t ui16Size)
{
	if (!pSender || !pMessage || ui16Size == 0)
		return;

	uint32_t channel = pSender->GetChannelNum();
	if (channel >= m_ui32ChannelCount)
		return;

	// Build chat packet
	i3NetworkPacket packet;
	char buffer[512];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = 0x0507;	// PROTOCOL_LOBBY_CHAT_ACK
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int senderIdx = pSender->GetIndex();
	memcpy(buffer + offset, &senderIdx, sizeof(int));		offset += sizeof(int);

	uint16_t msgSize = (uint16_t)min((int)ui16Size, 256);
	memcpy(buffer + offset, &msgSize, sizeof(uint16_t));	offset += sizeof(uint16_t);
	memcpy(buffer + offset, pMessage, msgSize);				offset += msgSize;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);

	// Broadcast to all lobby users in this channel
	m_pcsLobbyUser[channel]->Lock();

	// Iterate via index since i3List doesn't have proper iterators
	int count = m_pLobbyUserList[channel]->GetCount();
	for (int i = 0; i < count; i++)
	{
		GameSession* pSession = (GameSession*)m_pLobbyUserList[channel]->GetItem(i);
		if (pSession)
			pSession->SendMessage(&packet);
	}

	m_pcsLobbyUser[channel]->Unlock();
}

void GameSessionManager::OnSendChannelUser(int i32ChannelNum, i3NetworkPacket* pPacket)
{
	if (i32ChannelNum < 0 || i32ChannelNum >= (int)m_ui32ChannelCount || !pPacket)
		return;

	m_pcsChannelUser[i32ChannelNum]->Lock();

	int count = m_pChannelUserList[i32ChannelNum]->GetCount();
	for (int i = 0; i < count; i++)
	{
		GameSession* pSession = (GameSession*)m_pChannelUserList[i32ChannelNum]->GetItem(i);
		if (pSession)
			pSession->SendMessage(pPacket);
	}

	m_pcsChannelUser[i32ChannelNum]->Unlock();
}

void GameSessionManager::CheckTimeouts()
{
	if (!g_pContextMain)
		return;

	int sessionCount = g_pContextMain->m_i32SessionCount;
	int checkCount = min(SESSION_CHECK_COUNT, sessionCount);

	for (int i = 0; i < checkCount; i++)
	{
		int idx = (m_i32SessionCheckIdx + i) % sessionCount;
		GameSession* pSession = &m_pSessions[idx];

		if (pSession->GetTask() != GAME_TASK_NONE && pSession->IsTimedOut())
		{
			printf("[GameSessionManager] Session timeout - Index=%d, Task=%d\n",
				idx, pSession->GetTask());

			// Disconnect timed out session
			pSession->OnDisconnect(TRUE);
			m_i32ActiveCount--;
		}
	}

	m_i32SessionCheckIdx = (m_i32SessionCheckIdx + checkCount) % sessionCount;
}
