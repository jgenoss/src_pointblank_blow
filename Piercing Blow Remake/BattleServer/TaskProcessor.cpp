#include "pch.h"
#include "TaskProcessor.h"
#include "BattleRoom.h"
#include "BattleRoomManager.h"
#include "GameCharacter.h"
#include "UDPChecker.h"
#include "HMSParser.h"
#include "ServerStatistics.h"
#include "BattleMember.h"
#include "BattleSession.h"
#include "BattleSessionManager.h"
#include "GameObjectManager.h"

TaskProcessor::TaskProcessor()
	: m_ui32ThreadCount(1)
	, m_bRunning(false)
	, m_dwAccumulator(0)
	, m_pRoomManager(nullptr)
	, m_pStatistics(nullptr)
	, m_UdpSocket(INVALID_SOCKET)
{
	memset(m_Workers, 0, sizeof(m_Workers));
	QueryPerformanceFrequency(&m_PerfFrequency);
}

TaskProcessor::~TaskProcessor()
{
	Shutdown();
}

bool TaskProcessor::Initialize(uint32_t ui32ThreadCount)
{
	if (m_bRunning)
		return true;

	if (ui32ThreadCount < 1) ui32ThreadCount = 1;
	if (ui32ThreadCount > TASK_MAX_WORKER_THREADS) ui32ThreadCount = TASK_MAX_WORKER_THREADS;

	m_ui32ThreadCount = ui32ThreadCount;

	// Verify PacketQueue is available
	if (!g_pPacketQueue)
	{
		printf("[TaskProcessor] ERROR: PacketQueue not initialized\n");
		return false;
	}

	int32_t totalTracks = g_pPacketQueue->GetTrackCount();

	// Distribute tracks across worker threads
	int32_t tracksPerThread = totalTracks / (int32_t)ui32ThreadCount;
	int32_t remainder = totalTracks % (int32_t)ui32ThreadCount;

	int32_t trackStart = 0;
	for (uint32_t i = 0; i < ui32ThreadCount; i++)
	{
		TaskWorkerParam& w = m_Workers[i];
		w.i32ThreadIdx = (int32_t)i;
		w.i32TrackStart = trackStart;
		w.i32TrackEnd = trackStart + tracksPerThread + (((int32_t)i < remainder) ? 1 : 0);
		w.bRunning = false;
		w.bShutdown = false;
		w.ui64PacketsProcessed = 0;
		w.ui64TicksProcessed = 0;
		w.r64TotalProcessTime = 0.0;
		w.r64PeakProcessTime = 0.0;

		// Create synchronization events
		w.hWakeEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		w.hDoneEvent = CreateEvent(nullptr, FALSE, TRUE, nullptr);	// Initially signaled (done)

		if (!w.hWakeEvent || !w.hDoneEvent)
		{
			printf("[TaskProcessor] ERROR: CreateEvent failed for thread %u\n", i);
			Shutdown();
			return false;
		}

		// Create worker thread
		w.hThread = CreateThread(nullptr, 0, WorkerThreadProc, &m_Workers[i], 0, nullptr);
		if (!w.hThread)
		{
			printf("[TaskProcessor] ERROR: CreateThread failed for thread %u\n", i);
			Shutdown();
			return false;
		}

		trackStart = w.i32TrackEnd;
	}

	m_bRunning = true;
	m_dwAccumulator = 0;

	printf("[TaskProcessor] Initialized: %u worker threads, %d tracks total\n",
		ui32ThreadCount, totalTracks);
	for (uint32_t i = 0; i < ui32ThreadCount; i++)
	{
		printf("[TaskProcessor]   Thread %u: tracks [%d, %d)\n",
			i, m_Workers[i].i32TrackStart, m_Workers[i].i32TrackEnd);
	}

	return true;
}

void TaskProcessor::Shutdown()
{
	m_bRunning = false;

	// Signal all workers to shutdown
	for (uint32_t i = 0; i < m_ui32ThreadCount; i++)
	{
		m_Workers[i].bShutdown = true;
		if (m_Workers[i].hWakeEvent)
			SetEvent(m_Workers[i].hWakeEvent);
	}

	// Wait for all workers to finish (with timeout)
	for (uint32_t i = 0; i < m_ui32ThreadCount; i++)
	{
		if (m_Workers[i].hThread)
		{
			WaitForSingleObject(m_Workers[i].hThread, 5000);
			CloseHandle(m_Workers[i].hThread);
			m_Workers[i].hThread = nullptr;
		}
		if (m_Workers[i].hWakeEvent)
		{
			CloseHandle(m_Workers[i].hWakeEvent);
			m_Workers[i].hWakeEvent = nullptr;
		}
		if (m_Workers[i].hDoneEvent)
		{
			CloseHandle(m_Workers[i].hDoneEvent);
			m_Workers[i].hDoneEvent = nullptr;
		}
	}

	// Print final stats
	for (uint32_t i = 0; i < m_ui32ThreadCount; i++)
	{
		printf("[TaskProcessor] Thread %u stats: %llu packets, %llu ticks, avg=%.3fms, peak=%.3fms\n",
			i,
			(unsigned long long)m_Workers[i].ui64PacketsProcessed,
			(unsigned long long)m_Workers[i].ui64TicksProcessed,
			m_Workers[i].ui64TicksProcessed > 0
				? (m_Workers[i].r64TotalProcessTime / m_Workers[i].ui64TicksProcessed) * 1000.0
				: 0.0,
			m_Workers[i].r64PeakProcessTime * 1000.0);
	}
}

void TaskProcessor::OnUpdate(uint32_t dwDelta)
{
	if (!m_bRunning)
		return;

	m_dwAccumulator += dwDelta;

	// Process at fixed interval (63ms = ~16 ticks/sec)
	if (m_dwAccumulator < TASK_PROCESS_INTERVAL_MS)
		return;

	m_dwAccumulator -= TASK_PROCESS_INTERVAL_MS;

	// Cap accumulator to prevent spiral-of-death if processing is slow
	if (m_dwAccumulator > TASK_PROCESS_INTERVAL_MS * 4)
		m_dwAccumulator = TASK_PROCESS_INTERVAL_MS;

	// Wait for all workers to be done from previous tick
	HANDLE doneHandles[TASK_MAX_WORKER_THREADS];
	for (uint32_t i = 0; i < m_ui32ThreadCount; i++)
		doneHandles[i] = m_Workers[i].hDoneEvent;

	WaitForMultipleObjects(m_ui32ThreadCount, doneHandles, TRUE, 100);

	// Wake all workers for this tick
	for (uint32_t i = 0; i < m_ui32ThreadCount; i++)
		SetEvent(m_Workers[i].hWakeEvent);
}

uint64_t TaskProcessor::GetTotalPacketsProcessed() const
{
	uint64_t total = 0;
	for (uint32_t i = 0; i < m_ui32ThreadCount; i++)
		total += m_Workers[i].ui64PacketsProcessed;
	return total;
}

uint64_t TaskProcessor::GetTotalTicksProcessed() const
{
	uint64_t total = 0;
	for (uint32_t i = 0; i < m_ui32ThreadCount; i++)
		total += m_Workers[i].ui64TicksProcessed;
	return total;
}

// ============================================================================
// Worker Thread
// ============================================================================

DWORD WINAPI TaskProcessor::WorkerThreadProc(LPVOID lpParam)
{
	TaskWorkerParam* pWorker = (TaskWorkerParam*)lpParam;
	if (!pWorker)
		return 1;

	pWorker->bRunning = true;

	printf("[TaskProcessor] Worker thread %d started\n", pWorker->i32ThreadIdx);

	while (!pWorker->bShutdown)
	{
		// Wait for wake signal from main thread
		DWORD dwWait = WaitForSingleObject(pWorker->hWakeEvent, 100);
		if (pWorker->bShutdown)
			break;

		if (dwWait != WAIT_OBJECT_0)
			continue;	// Timeout, check shutdown flag and re-wait

		// Performance timing
		LARGE_INTEGER perfStart, perfEnd, perfFreq;
		QueryPerformanceFrequency(&perfFreq);
		QueryPerformanceCounter(&perfStart);

		// Process all tracks assigned to this worker
		if (g_pPacketQueue)
		{
			PacketEntry entry;

			for (int32_t trackIdx = pWorker->i32TrackStart; trackIdx < pWorker->i32TrackEnd; trackIdx++)
			{
				int32_t packetsInTrack = g_pPacketQueue->GetTrackCount(trackIdx);
				int32_t processed = 0;

				// Pop and process all packets in this track
				while (processed < TASK_MAX_PACKETS_PER_ROOM && g_pPacketQueue->Pop(trackIdx, &entry))
				{
					// Find the target room
					if (!g_pBattleRoomManager)
						continue;

					BattleRoom* pRoom = g_pBattleRoomManager->GetRoom(entry.ui16RoomIdx);
					if (!pRoom || !pRoom->IsActive())
						continue;

					// Validate slot index
					if (entry.ui16SlotIdx >= BATTLE_SLOT_MAX)
						continue;

					// ============================================
					// PIPELINE: UDPChecker → HMSParser → state update
					// (matches original TaskProcessor_Room.cpp)
					// ============================================

					// Step 1: UDPChecker - Parse and validate packet
					int32_t consumed = UDPChecker::GamePacketParsing(
						pRoom, entry.aData, entry.ui16DataSize, entry.ui16SlotIdx);

					if (consumed < 0)
					{
						// Invalid packet - skip
						continue;
					}

					// Step 2: Track member activity
					BattleMember* pMember = pRoom->GetMember(entry.ui16SlotIdx);
					if (pMember && pMember->IsMember())
						pMember->UpdateLastPacketTime();

					processed++;
					pWorker->ui64PacketsProcessed++;
				}
			}

			// After processing all packets for all tracks, do game logic for active rooms
			// Each track maps to a set of rooms. Process room logic for rooms with activity.
			if (g_pBattleRoomManager)
			{
				int32_t maxRooms = g_pBattleRoomManager->GetMaxRoomCount();
				for (int32_t roomIdx = 0; roomIdx < maxRooms; roomIdx++)
				{
					// Only process rooms that belong to this worker's tracks
					int32_t trackIdx = roomIdx % g_pPacketQueue->GetTrackCount();
					if (trackIdx < pWorker->i32TrackStart || trackIdx >= pWorker->i32TrackEnd)
						continue;

					BattleRoom* pRoom = g_pBattleRoomManager->GetRoom(roomIdx);
					if (!pRoom || !pRoom->IsActive())
						continue;

					ProcessRoomLogic(pWorker->i32ThreadIdx, pRoom);
				}
			}
		}

		// Performance measurement
		QueryPerformanceCounter(&perfEnd);
		double elapsedSec = (double)(perfEnd.QuadPart - perfStart.QuadPart) / (double)perfFreq.QuadPart;

		pWorker->r64TotalProcessTime += elapsedSec;
		pWorker->ui64TicksProcessed++;
		if (elapsedSec > pWorker->r64PeakProcessTime)
			pWorker->r64PeakProcessTime = elapsedSec;

		// Track in global statistics
		if (g_pStatistics)
			g_pStatistics->AddPacketProcess(pWorker->i32ThreadIdx, elapsedSec);

		// Signal done
		SetEvent(pWorker->hDoneEvent);
	}

	pWorker->bRunning = false;
	printf("[TaskProcessor] Worker thread %d stopped\n", pWorker->i32ThreadIdx);
	return 0;
}

// ============================================================================
// Room packet processing (static, called by workers)
// ============================================================================

void TaskProcessor::ProcessRoomPackets(int32_t i32TaskIdx, BattleRoom* pRoom,
									   int32_t i32TrackIdx, SOCKET udpSocket,
									   ServerStatistics* pStats)
{
	if (!pRoom || !g_pPacketQueue)
		return;

	PacketEntry entry;
	int32_t processed = 0;

	// Build multicast buffer for this room
	// (accumulate all outgoing data, then send once to each member)
	char multicastBuf[TASK_MULTICAST_BUFFER_SIZE];
	int32_t multicastSize = 0;

	while (processed < TASK_MAX_PACKETS_PER_ROOM && g_pPacketQueue->Pop(i32TrackIdx, &entry))
	{
		if (entry.ui16SlotIdx >= BATTLE_SLOT_MAX)
			continue;

		// Validate and parse packet through the pipeline
		int32_t consumed = UDPChecker::GamePacketParsing(
			pRoom, entry.aData, entry.ui16DataSize, entry.ui16SlotIdx);

		if (consumed < 0)
			continue;

		// Append to multicast buffer (relay to other members)
		if (multicastSize + entry.ui16DataSize < TASK_MULTICAST_BUFFER_SIZE)
		{
			memcpy(multicastBuf + multicastSize, entry.aData, entry.ui16DataSize);
			multicastSize += entry.ui16DataSize;
		}

		processed++;
	}

	// Send accumulated multicast to all room members
	if (multicastSize > 0 && udpSocket != INVALID_SOCKET)
	{
		MulticastToRoom(pRoom, multicastBuf, multicastSize, 0xFFFF, udpSocket);
	}
}

// ============================================================================
// Room game logic processing (static, called by workers)
// ============================================================================

void TaskProcessor::ProcessRoomLogic(int32_t i32TaskIdx, BattleRoom* pRoom)
{
	if (!pRoom || pRoom->GetState() != BATTLE_ROOM_USE)
		return;

	// 1. Update game object timers (weapon box respawns)
	GameObjectManager* pObjMgr = pRoom->GetObjectManager();
	if (pObjMgr)
		pObjMgr->Update(GetTickCount());

	// 2. Update dropped weapon timers
	DroppedWeaponMgr* pDropMgr = pRoom->GetDroppedWeaponMgr();
	if (pDropMgr)
	{
		DWORD dwNow = GetTickCount();
		float fGameTime = (float)(dwNow - 0) / 1000.0f;	// Approximate game time
		pDropMgr->Update(fGameTime);
	}

	// 3. Check round timer
	pRoom->Update();

	// 4. Speed hack enforcement pass
	// Check all alive characters for accumulated speed violations
	for (uint32_t i = 0; i < BATTLE_SLOT_MAX; i++)
	{
		GameCharacter* pChara = pRoom->GetCharacter(i);
		if (!pChara || !pChara->IsAlive())
			continue;

		SpeedState* pSpeed = pChara->GetSpeedState();
		if (!pSpeed)
			continue;

		float fAvgSpeed = 0.0f;
		if (pSpeed->IsSpeedHack(&fAvgSpeed))
		{
			// Speed hack confirmed via averaging - report to GameServer
			BattleMember* pMember = pRoom->GetMember(i);
			if (pMember && pMember->IsMember())
			{
				// Find the owning BattleSession and send hack notification
				extern BattleSessionManager* g_pBattleSessionManager;
				if (g_pBattleSessionManager)
				{
					BattleSession* pSession = (BattleSession*)g_pBattleSessionManager->GetSession(pRoom->GetOwnerSessionIdx());
					if (pSession)
					{
						char desc[128];
						snprintf(desc, sizeof(desc), "SpeedHack avg=%.0f slot=%u", fAvgSpeed, i);
						pSession->SendHackNotify(
							pRoom->GetGameServerRoomIdx(), pRoom->GetChannelNum(),
							i, pMember->GetUID(),
							(uint8_t)HACK_TYPE_SPEED_HACK, (uint8_t)HACK_SEVERITY_HIGH, desc);
					}
				}

				// Reset speed state to avoid repeated reports
				pSpeed->Reset();
			}
		}
	}

	// 5. Latency tracking update
	for (uint32_t i = 0; i < BATTLE_SLOT_MAX; i++)
	{
		GameCharacter* pChara = pRoom->GetCharacter(i);
		if (!pChara)
			continue;

		uint32_t samples = pChara->GetLatencySampleCount();
		if (samples > 100)
		{
			// Average has accumulated enough - compute and reset
			// This is used for latency compensation in hit validation
			float avg = pChara->GetAverageLatency() / (float)samples;
			pChara->SetLatencyTime(avg);
		}
	}
}

// ============================================================================
// Multicast UDP to all room members
// ============================================================================

void TaskProcessor::MulticastToRoom(BattleRoom* pRoom, const char* pData, int32_t i32Size,
									uint32_t ui32ExcludeSlot, SOCKET udpSocket)
{
	if (!pRoom || !pData || i32Size <= 0 || udpSocket == INVALID_SOCKET)
		return;

	for (int i = 0; i < BATTLE_SLOT_MAX; i++)
	{
		if ((uint32_t)i == ui32ExcludeSlot)
			continue;

		BattleMember* pMember = pRoom->GetMember(i);
		if (!pMember || !pMember->IsMember())
			continue;

		uint32_t destIP = pMember->GetIP();
		uint16_t destPort = pMember->GetPort();
		if (destIP == 0 || destPort == 0)
			continue;

		struct sockaddr_in destAddr;
		memset(&destAddr, 0, sizeof(destAddr));
		destAddr.sin_family = AF_INET;
		destAddr.sin_addr.s_addr = htonl(destIP);
		destAddr.sin_port = htons(destPort);

		sendto(udpSocket, pData, i32Size, 0,
			   (struct sockaddr*)&destAddr, sizeof(destAddr));
	}
}

// Global statistics pointer (extern'd from BattleServerContext)
extern ServerStatistics* g_pStatistics;
