#ifndef __TASKPROCESSOR_H__
#define __TASKPROCESSOR_H__

#pragma once

// ============================================================================
// TaskProcessor - Per-room game logic processor
// Port of original CTaskProcessor from Dedication/Dedi/TaskProcessor.h
// Processes room game logic at 63ms intervals (16 ticks/sec)
// Handles packet relay, position updates, timer ticks, object respawns
// ============================================================================

class BattleRoom;

// Task processing interval (from original: 63ms minimum)
#define TASK_PROCESS_INTERVAL_MS	63
#define TASK_MAX_ROOMS_PER_TICK		32

class TaskProcessor
{
public:
	TaskProcessor();
	~TaskProcessor();

	// Initialize task processor
	bool Initialize(uint32_t ui32ThreadCount = 1);
	void Shutdown();

	// Main update - called from server main loop
	// Iterates all active rooms and processes them
	void OnUpdate(uint32_t dwDelta);

	// Process a single room's relay (forward UDP packets to other members)
	static void ProcessRoom_Relay(int32_t i32TaskThreadIdx, BattleRoom* pRoom,
								   int32_t i32GroupIdx, uint32_t ui32Delta);

	// Process a single room's game logic (timers, objects, validation)
	static void ProcessRoom(int32_t i32TaskIdx, BattleRoom* pRoom,
							 int32_t i32GroupIdx);

private:
	uint32_t	m_ui32ThreadCount;
	bool		m_bRunning;
	uint32_t	m_dwLastProcessTime;
	uint32_t	m_dwAccumulator;		// Time accumulator for fixed-step
};

#endif // __TASKPROCESSOR_H__
