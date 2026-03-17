#include "pch.h"
#include "TaskProcessor.h"
#include "BattleRoom.h"
#include "GameCharacter.h"
#include "UDPChecker.h"
#include "ServerStatistics.h"

TaskProcessor::TaskProcessor()
	: m_ui32ThreadCount(1)
	, m_bRunning(false)
	, m_dwLastProcessTime(0)
	, m_dwAccumulator(0)
{
}

TaskProcessor::~TaskProcessor()
{
	Shutdown();
}

bool TaskProcessor::Initialize(uint32_t ui32ThreadCount)
{
	m_ui32ThreadCount = (ui32ThreadCount > 0) ? ui32ThreadCount : 1;
	m_bRunning = true;
	m_dwLastProcessTime = 0;
	m_dwAccumulator = 0;
	return true;
}

void TaskProcessor::Shutdown()
{
	m_bRunning = false;
}

void TaskProcessor::OnUpdate(uint32_t dwDelta)
{
	if (!m_bRunning)
		return;

	m_dwAccumulator += dwDelta;

	// Process at fixed interval (63ms = ~16 ticks/sec)
	if (m_dwAccumulator < TASK_PROCESS_INTERVAL_MS)
		return;

	uint32_t ui32Delta = m_dwAccumulator;
	m_dwAccumulator = 0;

	// Iterate all active battle rooms and process them
	// The BattleServerContext manages the room list - rooms are processed
	// in order, with relay first then game logic
	// Note: In production, each task thread would process a subset of rooms
	// For now, single-threaded processing

	// Room processing is driven by BattleServerContext calling
	// ProcessRoom_Relay and ProcessRoom for each active room
	(void)ui32Delta;
}

void TaskProcessor::ProcessRoom_Relay(int32_t i32TaskThreadIdx, BattleRoom* pRoom,
									   int32_t i32GroupIdx, uint32_t ui32Delta)
{
	if (!pRoom)
		return;

	// Relay phase: forward queued UDP packets to other room members
	// This is the hot path - must be fast
	// The UdpRelay system handles the actual packet forwarding
	// Here we just trigger it

	// Process any pending position updates
	for (uint32_t i = 0; i < 16; i++)
	{
		GameCharacter* pChara = pRoom->GetCharacter(i);
		if (!pChara || !pChara->IsAlive())
			continue;

		// Speed state check during relay
		SpeedState* pSpeed = pChara->GetSpeedState();
		if (pSpeed)
		{
			float fAvg = 0.0f;
			if (pSpeed->IsSpeedHack(&fAvg))
			{
				// Speed hack detected during relay - room should handle
				// In original: sends hack report via ModuleCast
			}
		}
	}
}

void TaskProcessor::ProcessRoom(int32_t i32TaskIdx, BattleRoom* pRoom,
								 int32_t i32GroupIdx)
{
	if (!pRoom)
		return;

	// Game logic phase: process timers, object respawns, round state

	// 1. Update game object timers (weapon box respawns)
	// The GameObjectManager handles this via BattleRoom::Update()

	// 2. Check round timer
	// If round time exceeded, trigger round end

	// 3. Process any pending damage calculations
	// Validate queued hits, apply damage, check for kills

	// 4. Check for mission completion
	// Bomb timer, destruction targets, defence objectives

	// 5. Check for round end conditions
	// All players dead on one team, objective completed, timer expired

	// 6. Update latency tracking for all characters
	for (uint32_t i = 0; i < 16; i++)
	{
		GameCharacter* pChara = pRoom->GetCharacter(i);
		if (!pChara)
			continue;

		// Latency calculation from original:
		// avg = total_latency / sample_count
		uint32_t samples = pChara->GetLatencySampleCount();
		if (samples > 0)
		{
			// Average latency is tracked per character for
			// latency compensation in hit validation
		}
	}
}
