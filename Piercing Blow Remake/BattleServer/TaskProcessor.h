#ifndef __TASKPROCESSOR_H__
#define __TASKPROCESSOR_H__

#pragma once
#include "PacketQueue.h"
#include "UdpBufferPool.h"

// ============================================================================
// TaskProcessor - Multi-threaded room packet processor
// Production port of original CTaskProcessor from Dedication/Dedi/TaskProcessor.h
//
// Architecture (matches original pattern):
// 1. Worker threads wake every TASK_PROCESS_INTERVAL_MS (63ms = ~16 ticks/sec)
// 2. Each thread owns a set of PacketQueue tracks
// 3. Per-tick processing per room group:
//    a. Pop packets from PacketQueue track
//    b. For each packet: UDPChecker → HMSParser → game state update
//    c. Build multicast buffer (UDPCSBuilder equivalent)
//    d. Send multicast to all room members via UDP
//    e. Check round end conditions
//    f. Track performance via QueryPerformanceCounter
//
// Threading model:
// - Main thread calls OnUpdate() which wakes worker threads
// - Each worker processes its assigned tracks independently
// - No lock contention between workers (tracks partition rooms)
// ============================================================================

class BattleRoom;
class BattleRoomManager;
class ServerStatistics;

// Task processing constants
#define TASK_PROCESS_INTERVAL_MS	63		// ~16 ticks/sec (original: 63ms)
#define TASK_MAX_ROOMS_PER_TICK		32		// Max rooms per tick per thread
#define TASK_MAX_PACKETS_PER_ROOM	256		// Max packets to process per room per tick
#define TASK_MAX_WORKER_THREADS		4		// Max worker threads
#define TASK_MULTICAST_BUFFER_SIZE	65536	// Multicast build buffer

// Worker thread parameter (equivalent to TASK_PROCESSOR_PARAM)
struct TaskWorkerParam
{
	int32_t		i32ThreadIdx;			// Worker thread index
	int32_t		i32TrackStart;			// First track this worker owns
	int32_t		i32TrackEnd;			// Last track (exclusive)
	HANDLE		hThread;				// Thread handle
	HANDLE		hWakeEvent;				// Event to wake worker
	HANDLE		hDoneEvent;				// Event signaled when done
	bool		bRunning;				// Thread running flag
	bool		bShutdown;				// Shutdown signal

	// Performance counters per worker
	uint64_t	ui64PacketsProcessed;
	uint64_t	ui64TicksProcessed;
	double		r64TotalProcessTime;	// Accumulated processing time (seconds)
	double		r64PeakProcessTime;		// Peak single-tick time (seconds)
};

class TaskProcessor
{
public:
	TaskProcessor();
	~TaskProcessor();

	// Initialize with thread count (1-4)
	bool Initialize(uint32_t ui32ThreadCount = 2);
	void Shutdown();

	// Main update - called from server main loop at high frequency
	// Manages tick timing and wakes worker threads
	void OnUpdate(uint32_t dwDelta);

	// Set external dependencies
	void SetRoomManager(BattleRoomManager* pMgr)		{ m_pRoomManager = pMgr; }
	void SetStatistics(ServerStatistics* pStats)		{ m_pStatistics = pStats; }
	void SetUdpSocket(SOCKET sock)						{ m_UdpSocket = sock; }

	// Statistics
	uint32_t GetThreadCount() const						{ return m_ui32ThreadCount; }
	uint64_t GetTotalPacketsProcessed() const;
	uint64_t GetTotalTicksProcessed() const;

	// Process a single room's packets from the queue (called by worker threads)
	static void ProcessRoomPackets(int32_t i32TaskIdx, BattleRoom* pRoom,
								   int32_t i32TrackIdx, SOCKET udpSocket,
								   ServerStatistics* pStats);

	// Process a single room's game logic (timers, objects, round checks)
	static void ProcessRoomLogic(int32_t i32TaskIdx, BattleRoom* pRoom);

	// Send multicast UDP to all room members
	static void MulticastToRoom(BattleRoom* pRoom, const char* pData, int32_t i32Size,
								uint32_t ui32ExcludeSlot, SOCKET udpSocket);

private:
	// Worker thread entry point
	static DWORD WINAPI WorkerThreadProc(LPVOID lpParam);

	// Process all rooms in assigned tracks
	void ProcessTracks(TaskWorkerParam* pWorker);

	uint32_t			m_ui32ThreadCount;
	bool				m_bRunning;
	uint32_t			m_dwAccumulator;		// Time accumulator for fixed-step

	// Worker threads
	TaskWorkerParam		m_Workers[TASK_MAX_WORKER_THREADS];

	// External dependencies (not owned)
	BattleRoomManager*	m_pRoomManager;
	ServerStatistics*	m_pStatistics;
	SOCKET				m_UdpSocket;

	// Performance frequency for QueryPerformanceCounter
	LARGE_INTEGER		m_PerfFrequency;
};

#endif // __TASKPROCESSOR_H__
