#ifndef __TASKPROCESSOR_H__
#define __TASKPROCESSOR_H__

#pragma once
#include <windows.h>

class DataServerContext;
class DataSession;

// Task types for the async queue
enum TaskType : uint8_t
{
	TASK_NONE = 0,
	TASK_AUTH_LOGIN,
	TASK_PLAYER_LOAD,
	TASK_PLAYER_SAVE,
	TASK_PLAYER_CREATE_NICK,
	TASK_PLAYER_CHECK_NICK,
	TASK_STATS_SAVE,
	TASK_GAME_DATA,			// Generic game data operation
	TASK_SOCIAL,			// Generic social operation
	TASK_MAX
};

// Task entry in the queue
struct TaskEntry
{
	TaskType		eType;
	int				i32SessionIdx;		// Which DataSession sent this
	uint16_t		ui16ProtocolId;		// Original protocol ID (for game data/social routing)
	int				i32DataSize;		// Payload size
	char			Data[8192];			// Payload buffer (copy of request data)
	DWORD			dwQueueTime;		// When queued (for latency tracking)
};

// Response entry (result back to main thread for sending)
struct TaskResponse
{
	int				i32SessionIdx;
	int				i32DataSize;
	char			Data[8192];			// Serialized response packet data
	DWORD			dwProcessTime;		// Processing duration in microseconds
};

#define TASK_QUEUE_SIZE			4096
#define TASK_RESPONSE_SIZE		4096
#define TASK_MAX_WORKERS		8
#define TASK_PROCESS_INTERVAL	1		// 1ms check interval

// Production-grade async task processor for DataServer
// Pattern: Request queue -> Worker threads (each with own DB connection) -> Response queue -> Main thread sends
class TaskProcessor
{
public:
	TaskProcessor(DataServerContext* pContext);
	~TaskProcessor();

	// Initialize worker threads
	bool		Initialize(int i32WorkerCount);
	void		Shutdown();

	// Queue a task (called from IOCP network threads - must be thread-safe)
	bool		QueueTask(TaskType eType, int i32SessionIdx, uint16_t ui16ProtocolId,
						  const void* pData, int i32DataSize);

	// Process responses on main thread (called from OnUpdate)
	void		ProcessResponses();

	// Statistics
	volatile LONG		m_lTasksQueued;
	volatile LONG		m_lTasksProcessed;
	volatile LONG		m_lTasksDropped;
	volatile LONG		m_lResponsesPending;
	volatile LONG		m_lActiveWorkers;
	volatile LONG		m_lPeakQueueDepth;
	volatile LONG64		m_l64TotalProcessTimeUs;	// Microseconds

private:
	static DWORD WINAPI WorkerThreadProc(LPVOID lpParam);
	void		ProcessTask(TaskEntry* pTask, int i32WorkerIdx);

	// Ring buffer queues with CRITICAL_SECTION
	TaskEntry			m_TaskQueue[TASK_QUEUE_SIZE];
	volatile LONG		m_lTaskHead;		// Producer writes here
	volatile LONG		m_lTaskTail;		// Consumers read from here
	CRITICAL_SECTION	m_csTaskQueue;
	HANDLE				m_hTaskSemaphore;	// Signals workers when tasks available

	TaskResponse		m_ResponseQueue[TASK_RESPONSE_SIZE];
	volatile LONG		m_lRespHead;
	volatile LONG		m_lRespTail;
	CRITICAL_SECTION	m_csRespQueue;

	// Worker threads
	struct WorkerParam
	{
		TaskProcessor*	pProcessor;
		int				i32WorkerIdx;
		HANDLE			hThread;
		volatile LONG	lTasksProcessed;
		volatile LONG64	l64TotalProcessTimeUs;
	};

	WorkerParam			m_Workers[TASK_MAX_WORKERS];
	int					m_i32WorkerCount;
	volatile LONG		m_lShutdown;

	DataServerContext*	m_pContext;
};

#endif // __TASKPROCESSOR_H__
