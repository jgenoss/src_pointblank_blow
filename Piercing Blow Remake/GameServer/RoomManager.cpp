#include "pch.h"
#include "RoomManager.h"
#include "Room.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "GameContextMain.h"

I3_CLASS_INSTANCE(RoomManager);

RoomManager* g_pRoomManager = nullptr;

RoomManager::RoomManager()
	: m_pChannelResRoomList(nullptr)
	, m_pChannelRoomList(nullptr)
	, m_pcsChannelRoom(nullptr)
	, m_ui32EachChannelMaxCount(MAX_ROOM_COUNT_PER_CHANNEL)
	, m_ui32ChannelCount(0)
	, m_InfoSide(nullptr)
	, m_pRoomInfoIdx0(nullptr)
	, m_pRoomCount0(nullptr)
	, m_pRoomInfoIdx1(nullptr)
	, m_pRoomCount1(nullptr)
	, m_pChangeRoomListTime(nullptr)
	, m_i32UseRoomCount(0)
{
}

RoomManager::~RoomManager()
{
}

void RoomManager::OnCreate()
{
	if (!g_pContextMain)
		return;

	m_ui32ChannelCount = g_pContextMain->m_ui8ChannelCount;
	m_ui32EachChannelMaxCount = g_pContextMain->m_ui16MaxRoomsPerChannel;

	// Allocate per-channel structures
	m_pChannelResRoomList = new i3List*[m_ui32ChannelCount];
	m_pChannelRoomList = new i3List*[m_ui32ChannelCount];
	m_pcsChannelRoom = new i3Mutex*[m_ui32ChannelCount];

	// Double-buffered room info
	m_InfoSide = new int[m_ui32ChannelCount];
	m_pRoomInfoIdx0 = new int*[m_ui32ChannelCount];
	m_pRoomCount0 = new uint32_t[m_ui32ChannelCount];
	m_pRoomInfoIdx1 = new int*[m_ui32ChannelCount];
	m_pRoomCount1 = new uint32_t[m_ui32ChannelCount];
	m_pChangeRoomListTime = new uint32_t[m_ui32ChannelCount];

	for (uint32_t ch = 0; ch < m_ui32ChannelCount; ch++)
	{
		// Pre-allocate room pool per channel
		m_pChannelResRoomList[ch] = new i3List();
		m_pChannelRoomList[ch] = new i3List();
		m_pcsChannelRoom[ch] = new i3Mutex();

		for (uint32_t r = 0; r < m_ui32EachChannelMaxCount; r++)
		{
			Room* pRoom = new Room();
			pRoom->SetRoomIdx((int)r);
			m_pChannelResRoomList[ch]->AddTail(pRoom);
		}

		// Double-buffered index arrays
		m_InfoSide[ch] = 0;
		m_pRoomInfoIdx0[ch] = new int[m_ui32EachChannelMaxCount];
		m_pRoomInfoIdx1[ch] = new int[m_ui32EachChannelMaxCount];
		m_pRoomCount0[ch] = 0;
		m_pRoomCount1[ch] = 0;
		m_pChangeRoomListTime[ch] = 0;

		memset(m_pRoomInfoIdx0[ch], -1, sizeof(int) * m_ui32EachChannelMaxCount);
		memset(m_pRoomInfoIdx1[ch], -1, sizeof(int) * m_ui32EachChannelMaxCount);
	}

	printf("[RoomManager] Created - Channels=%d, MaxRoomsPerChannel=%d\n",
		m_ui32ChannelCount, m_ui32EachChannelMaxCount);
}

void RoomManager::OnUpdate()
{
	DWORD dwNow = GetTickCount();

	// Update room count stats and battle timers
	int totalRooms = 0;
	for (uint32_t ch = 0; ch < m_ui32ChannelCount; ch++)
	{
		int roomCount = m_pChannelRoomList[ch]->GetCount();
		totalRooms += roomCount;

		// Update room state machines for active rooms
		m_pcsChannelRoom[ch]->Lock();
		i3ListNode* pNode = m_pChannelRoomList[ch]->Begin();
		while (pNode)
		{
			Room* pRoom = (Room*)pNode->pData;
			i3ListNode* pNext = pNode->pNext;	// Cache next before potential removal
			if (pRoom && pRoom->IsCreated())
				pRoom->OnUpdateRoom(dwNow);
			pNode = pNext;
		}
		m_pcsChannelRoom[ch]->Unlock();

		// Periodic double-buffer update
		if (dwNow - m_pChangeRoomListTime[ch] > 1000)	// Update every 1 second
		{
			UpdateRoomInfo(ch);
			m_pChangeRoomListTime[ch] = dwNow;
		}
	}
	m_i32UseRoomCount = totalRooms;
}

void RoomManager::OnDestroy()
{
	for (uint32_t ch = 0; ch < m_ui32ChannelCount; ch++)
	{
		// Destroy all rooms
		if (m_pChannelResRoomList[ch])
		{
			for (int i = 0; i < (int)m_ui32EachChannelMaxCount; i++)
			{
				Room* pRoom = (Room*)m_pChannelResRoomList[ch]->GetItem(i);
				if (pRoom)
				{
					if (pRoom->IsCreated())
						pRoom->OnDestroy();
					delete pRoom;
				}
			}
			delete m_pChannelResRoomList[ch];
		}

		if (m_pChannelRoomList[ch])
			delete m_pChannelRoomList[ch];

		if (m_pcsChannelRoom[ch])
			delete m_pcsChannelRoom[ch];

		if (m_pRoomInfoIdx0[ch])
			delete[] m_pRoomInfoIdx0[ch];
		if (m_pRoomInfoIdx1[ch])
			delete[] m_pRoomInfoIdx1[ch];
	}

	delete[] m_pChannelResRoomList;		m_pChannelResRoomList = nullptr;
	delete[] m_pChannelRoomList;		m_pChannelRoomList = nullptr;
	delete[] m_pcsChannelRoom;			m_pcsChannelRoom = nullptr;
	delete[] m_InfoSide;				m_InfoSide = nullptr;
	delete[] m_pRoomInfoIdx0;			m_pRoomInfoIdx0 = nullptr;
	delete[] m_pRoomCount0;				m_pRoomCount0 = nullptr;
	delete[] m_pRoomInfoIdx1;			m_pRoomInfoIdx1 = nullptr;
	delete[] m_pRoomCount1;				m_pRoomCount1 = nullptr;
	delete[] m_pChangeRoomListTime;		m_pChangeRoomListTime = nullptr;
}

int RoomManager::OnCreateRoom(GameSession* pSession, GameRoomCreateInfo* pInfo)
{
	if (!pSession || !pInfo)
		return -1;

	int channel = pSession->GetChannelNum();
	if (channel < 0 || channel >= (int)m_ui32ChannelCount)
		return -1;

	m_pcsChannelRoom[channel]->Lock();

	// Find an empty room in the pool
	Room* pRoom = nullptr;
	int roomIdx = -1;

	for (int i = 0; i < (int)m_ui32EachChannelMaxCount; i++)
	{
		Room* pCandidate = (Room*)m_pChannelResRoomList[channel]->GetItem(i);
		if (pCandidate && !pCandidate->IsCreated())
		{
			pRoom = pCandidate;
			roomIdx = i;
			break;
		}
	}

	if (!pRoom)
	{
		m_pcsChannelRoom[channel]->Unlock();
		return -1;	// No empty rooms
	}

	if (!pRoom->OnCreate(pSession, pInfo, channel))
	{
		m_pcsChannelRoom[channel]->Unlock();
		return -1;
	}

	pRoom->SetRoomIdx(roomIdx);
	m_pChannelRoomList[channel]->AddTail(pRoom);

	m_pcsChannelRoom[channel]->Unlock();

	return roomIdx;
}

int RoomManager::OnJoinRoom(GameSession* pSession, int i32ChannelNum, int i32RoomIdx, int i32Team)
{
	if (!pSession)
		return -1;

	if (i32ChannelNum < 0 || i32ChannelNum >= (int)m_ui32ChannelCount)
		return -1;

	Room* pRoom = GetRoom(i32ChannelNum, i32RoomIdx);
	if (!pRoom || !pRoom->IsCreated())
		return -1;

	m_pcsChannelRoom[i32ChannelNum]->Lock();
	int slot = pRoom->OnJoin(pSession, i32Team);
	m_pcsChannelRoom[i32ChannelNum]->Unlock();

	return slot;
}

int RoomManager::OnLeaveRoom(GameSession* pSession, int i32ChannelNum)
{
	if (!pSession)
		return -1;

	if (i32ChannelNum < 0 || i32ChannelNum >= (int)m_ui32ChannelCount)
		return -1;

	int roomIdx = pSession->GetRoomIdx();
	if (roomIdx < 0)
		return -1;

	Room* pRoom = GetRoom(i32ChannelNum, roomIdx);
	if (!pRoom)
		return -1;

	m_pcsChannelRoom[i32ChannelNum]->Lock();

	pRoom->OnLeave(pSession);

	// If room was destroyed (empty), remove from active list
	if (!pRoom->IsCreated())
		m_pChannelRoomList[i32ChannelNum]->Remove(pRoom);

	m_pcsChannelRoom[i32ChannelNum]->Unlock();

	return 0;
}

int RoomManager::OnQuickJoinRoom(GameSession* pSession, int i32ChannelNum)
{
	if (!pSession)
		return -1;

	if (i32ChannelNum < 0 || i32ChannelNum >= (int)m_ui32ChannelCount)
		return -1;

	m_pcsChannelRoom[i32ChannelNum]->Lock();

	// Search for a room with available slots
	for (int i = 0; i < (int)m_ui32EachChannelMaxCount; i++)
	{
		Room* pRoom = (Room*)m_pChannelResRoomList[i32ChannelNum]->GetItem(i);
		if (pRoom && pRoom->IsCreated() && pRoom->GetRoomState() == ROOM_STATE_READY)
		{
			if (pRoom->GetPlayerCount() < pRoom->GetMaxPlayers())
			{
				int slot = pRoom->OnJoin(pSession, TEAM_RED);
				if (slot >= 0)
				{
					m_pcsChannelRoom[i32ChannelNum]->Unlock();
					return i;	// Return room index
				}
			}
		}
	}

	m_pcsChannelRoom[i32ChannelNum]->Unlock();
	return -1;
}

Room* RoomManager::GetRoom(int i32ChannelNum, int i32Idx)
{
	if (i32ChannelNum < 0 || i32ChannelNum >= (int)m_ui32ChannelCount)
		return nullptr;

	if (i32Idx < 0 || i32Idx >= (int)m_ui32EachChannelMaxCount)
		return nullptr;

	return (Room*)m_pChannelResRoomList[i32ChannelNum]->GetItem(i32Idx);
}

int RoomManager::GetChannelUseRoomCount(int i32ChannelNum)
{
	if (i32ChannelNum < 0 || i32ChannelNum >= (int)m_ui32ChannelCount)
		return 0;

	// Read from the active side of the double buffer
	if (m_InfoSide[i32ChannelNum] == 0)
		return m_pRoomCount0[i32ChannelNum];
	return m_pRoomCount1[i32ChannelNum];
}

void RoomManager::OnSendRoomList(GameSession* pSession, int i32Channel)
{
	if (!pSession)
		return;

	if (i32Channel < 0 || i32Channel >= (int)m_ui32ChannelCount)
		return;

	// Read from the active side
	int side = m_InfoSide[i32Channel];
	int* pIndices = (side == 0) ? m_pRoomInfoIdx0[i32Channel] : m_pRoomInfoIdx1[i32Channel];
	uint32_t count = (side == 0) ? m_pRoomCount0[i32Channel] : m_pRoomCount1[i32Channel];

	// Build room list packet
	i3NetworkPacket packet;
	char buffer[4096];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_LOBBY_GET_ROOMLIST_ACK;
	offset += sizeof(uint16_t);	// Reserve space for size
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	uint32_t roomCount = count;
	memcpy(buffer + offset, &roomCount, sizeof(uint32_t));	offset += sizeof(uint32_t);

	// Serialize each room's basic info
	for (uint32_t i = 0; i < count && i < 10; i++)	// Max 10 rooms per page (SEND_MAX_ROOM_COUNT)
	{
		int roomIdx = pIndices[i];
		Room* pRoom = GetRoom(i32Channel, roomIdx);
		if (pRoom && pRoom->IsCreated())
		{
			int roomInfoSize = 0;
			pRoom->FillRoomInfoBasic(buffer + offset, &roomInfoSize);
			offset += roomInfoSize;
		}
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	pSession->SendMessage(&packet);
}

void RoomManager::UpdateRoomInfo(int i32ChannelNum)
{
	// Double-buffer swap: write to inactive side, then swap
	int writeSide = (m_InfoSide[i32ChannelNum] == 0) ? 1 : 0;
	int* pWriteIndices = (writeSide == 0) ? m_pRoomInfoIdx0[i32ChannelNum] : m_pRoomInfoIdx1[i32ChannelNum];
	uint32_t* pWriteCount = (writeSide == 0) ? &m_pRoomCount0[i32ChannelNum] : &m_pRoomCount1[i32ChannelNum];

	uint32_t count = 0;

	for (int i = 0; i < (int)m_ui32EachChannelMaxCount && count < m_ui32EachChannelMaxCount; i++)
	{
		Room* pRoom = (Room*)m_pChannelResRoomList[i32ChannelNum]->GetItem(i);
		if (pRoom && pRoom->IsCreated())
		{
			pWriteIndices[count] = i;
			count++;
		}
	}

	*pWriteCount = count;

	// Atomic swap
	m_InfoSide[i32ChannelNum] = writeSide;
}
