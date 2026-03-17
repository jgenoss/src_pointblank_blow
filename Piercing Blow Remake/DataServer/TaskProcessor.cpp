#include "TaskProcessor.h"
#include "DataServerContext.h"
#include "DataSession.h"
#include "DataSessionManager.h"
#include "ModuleDBAuth.h"
#include "ModuleDBUserLoad.h"
#include "ModuleDBUserSave.h"
#include "ModuleDBGameData.h"
#include "ModuleDBSocial.h"
#include "DBConnectionPool.h"
#include "InterServerProtocol.h"
#include "i3NetworkPacket.h"
#include <cstdio>
#include <cstring>

// ============================================================================
// TaskProcessor - Production-grade async DB worker thread pool
// ============================================================================

TaskProcessor::TaskProcessor(DataServerContext* pContext)
	: m_pContext(pContext)
	, m_lHead(0)
	, m_lTail(0)
	, m_lCount(0)
	, m_lRespHead(0)
	, m_lRespTail(0)
	, m_i32WorkerCount(0)
	, m_lShutdown(0)
	, m_lTasksQueued(0)
	, m_lTasksProcessed(0)
	, m_lTasksDropped(0)
	, m_lResponsesPending(0)
	, m_lActiveWorkers(0)
	, m_lPeakQueueDepth(0)
	, m_l64TotalProcessTimeUs(0)
	, m_hTaskSemaphore(NULL)
	, m_dwLastStatTime(0)
{
	InitializeCriticalSectionAndSpinCount(&m_csTaskQueue, 4000);
	InitializeCriticalSectionAndSpinCount(&m_csRespQueue, 4000);
	QueryPerformanceFrequency(&m_liFrequency);
	memset(m_Workers, 0, sizeof(m_Workers));
}

TaskProcessor::~TaskProcessor()
{
	Shutdown();
	DeleteCriticalSection(&m_csTaskQueue);
	DeleteCriticalSection(&m_csRespQueue);
}

bool TaskProcessor::Initialize(int i32WorkerCount)
{
	if (i32WorkerCount <= 0)
		i32WorkerCount = 4;
	if (i32WorkerCount > TASK_MAX_WORKERS)
		i32WorkerCount = TASK_MAX_WORKERS;

	m_i32WorkerCount = i32WorkerCount;

	// Create semaphore for task notification
	m_hTaskSemaphore = CreateSemaphore(NULL, 0, TASK_QUEUE_SIZE, NULL);
	if (!m_hTaskSemaphore)
	{
		printf("[TaskProcessor] ERROR: Failed to create semaphore\n");
		return false;
	}

	// Create worker threads
	for (int i = 0; i < m_i32WorkerCount; i++)
	{
		m_Workers[i].pProcessor = this;
		m_Workers[i].i32WorkerIdx = i;
		m_Workers[i].lTasksProcessed = 0;
		m_Workers[i].l64TotalProcessTimeUs = 0;

		m_Workers[i].hThread = CreateThread(NULL, 0, WorkerThreadProc, &m_Workers[i], 0, NULL);
		if (!m_Workers[i].hThread)
		{
			printf("[TaskProcessor] ERROR: Failed to create worker thread %d\n", i);
			m_i32WorkerCount = i;
			break;
		}
	}

	m_dwLastStatTime = GetTickCount();

	printf("[TaskProcessor] Initialized with %d worker threads (queue capacity=%d)\n",
		m_i32WorkerCount, TASK_QUEUE_SIZE);
	return m_i32WorkerCount > 0;
}

void TaskProcessor::Shutdown()
{
	if (m_lShutdown)
		return;

	InterlockedExchange(&m_lShutdown, 1);

	// Wake all workers so they can exit
	if (m_hTaskSemaphore)
		ReleaseSemaphore(m_hTaskSemaphore, m_i32WorkerCount, NULL);

	// Wait for all worker threads to finish (5 second timeout)
	HANDLE handles[TASK_MAX_WORKERS];
	int handleCount = 0;
	for (int i = 0; i < m_i32WorkerCount; i++)
	{
		if (m_Workers[i].hThread)
			handles[handleCount++] = m_Workers[i].hThread;
	}

	if (handleCount > 0)
		WaitForMultipleObjects(handleCount, handles, TRUE, 5000);

	// Cleanup thread handles
	for (int i = 0; i < m_i32WorkerCount; i++)
	{
		if (m_Workers[i].hThread)
		{
			CloseHandle(m_Workers[i].hThread);
			m_Workers[i].hThread = NULL;
		}
	}

	if (m_hTaskSemaphore)
	{
		CloseHandle(m_hTaskSemaphore);
		m_hTaskSemaphore = NULL;
	}

	printf("[TaskProcessor] Shutdown - Processed=%ld, Dropped=%ld\n",
		m_lTasksProcessed, m_lTasksDropped);
}

bool TaskProcessor::QueueTask(TaskType eType, int i32SessionIdx, uint16_t ui16ProtocolId,
							   const void* pData, int i32DataSize)
{
	if (m_lShutdown || !pData || i32DataSize <= 0)
		return false;

	if (i32DataSize > (int)sizeof(TaskEntry::Data))
	{
		printf("[TaskProcessor] WARNING: Task data too large (%d bytes), dropping\n", i32DataSize);
		InterlockedIncrement(&m_lTasksDropped);
		return false;
	}

	EnterCriticalSection(&m_csTaskQueue);

	LONG count = m_lCount;
	if (count >= TASK_QUEUE_SIZE)
	{
		LeaveCriticalSection(&m_csTaskQueue);
		InterlockedIncrement(&m_lTasksDropped);
		printf("[TaskProcessor] WARNING: Queue full, dropping task (type=%d)\n", eType);
		return false;
	}

	// Write to ring buffer
	LONG head = m_lHead;
	TaskEntry* pEntry = &m_Queue[head % TASK_QUEUE_SIZE];
	pEntry->eType = eType;
	pEntry->ui16ProtocolId = ui16ProtocolId;
	pEntry->i32SessionIdx = i32SessionIdx;
	pEntry->i32DataSize = i32DataSize;
	pEntry->dwQueueTime = GetTickCount();
	memcpy(pEntry->Data, pData, i32DataSize);

	m_lHead = head + 1;
	LONG newCount = InterlockedIncrement(&m_lCount);

	LeaveCriticalSection(&m_csTaskQueue);

	// Track peak queue depth
	LONG peak = m_lPeakQueueDepth;
	while (newCount > peak)
	{
		if (InterlockedCompareExchange(&m_lPeakQueueDepth, newCount, peak) == peak)
			break;
		peak = m_lPeakQueueDepth;
	}

	InterlockedIncrement(&m_lTasksQueued);

	// Signal a worker
	ReleaseSemaphore(m_hTaskSemaphore, 1, NULL);
	return true;
}

// Legacy synchronous methods - redirect to QueueTask for backward compatibility
void TaskProcessor::ProcessAuthLogin(DataSession* pSession, IS_AUTH_LOGIN_REQ* pReq)
{
	QueueTask(TASK_AUTH_LOGIN, pSession->GetIndex(), PROTOCOL_IS_AUTH_LOGIN_REQ, pReq, sizeof(IS_AUTH_LOGIN_REQ));
}

void TaskProcessor::ProcessPlayerLoad(DataSession* pSession, IS_PLAYER_LOAD_REQ* pReq)
{
	QueueTask(TASK_PLAYER_LOAD, pSession->GetIndex(), PROTOCOL_IS_PLAYER_LOAD_REQ, pReq, sizeof(IS_PLAYER_LOAD_REQ));
}

void TaskProcessor::ProcessPlayerSave(DataSession* pSession, IS_PLAYER_SAVE_REQ* pReq)
{
	QueueTask(TASK_PLAYER_SAVE, pSession->GetIndex(), PROTOCOL_IS_PLAYER_SAVE_REQ, pReq, sizeof(IS_PLAYER_SAVE_REQ));
}

void TaskProcessor::ProcessPlayerCreateNick(DataSession* pSession, IS_PLAYER_CREATE_NICK_REQ* pReq)
{
	QueueTask(TASK_PLAYER_CREATE_NICK, pSession->GetIndex(), PROTOCOL_IS_PLAYER_CREATE_NICK_REQ, pReq, sizeof(IS_PLAYER_CREATE_NICK_REQ));
}

void TaskProcessor::ProcessPlayerCheckNick(DataSession* pSession, IS_PLAYER_CHECK_NICK_REQ* pReq)
{
	QueueTask(TASK_PLAYER_CHECK_NICK, pSession->GetIndex(), PROTOCOL_IS_PLAYER_CHECK_NICK_REQ, pReq, sizeof(IS_PLAYER_CHECK_NICK_REQ));
}

void TaskProcessor::ProcessStatsSave(DataSession* pSession, IS_STATS_SAVE_REQ* pReq)
{
	QueueTask(TASK_STATS_SAVE, pSession->GetIndex(), PROTOCOL_IS_STATS_SAVE_REQ, pReq, sizeof(IS_STATS_SAVE_REQ));
}

// ============================================================================
// Worker Thread
// ============================================================================

DWORD WINAPI TaskProcessor::WorkerThreadProc(LPVOID lpParam)
{
	WorkerParam* pParam = (WorkerParam*)lpParam;
	TaskProcessor* pThis = pParam->pProcessor;
	int workerIdx = pParam->i32WorkerIdx;

	printf("[TaskProcessor] Worker %d started\n", workerIdx);

	InterlockedIncrement(&pThis->m_lActiveWorkers);

	while (!pThis->m_lShutdown)
	{
		// Wait for a task signal
		DWORD dwWait = WaitForSingleObject(pThis->m_hTaskSemaphore, 1000);

		if (pThis->m_lShutdown)
			break;

		if (dwWait == WAIT_TIMEOUT)
			continue;

		if (dwWait != WAIT_OBJECT_0)
			continue;

		// Pop a task from the queue
		TaskEntry task;
		bool bGotTask = false;

		EnterCriticalSection(&pThis->m_csTaskQueue);
		{
			if (pThis->m_lCount > 0)
			{
				LONG tail = pThis->m_lTail;
				memcpy(&task, &pThis->m_Queue[tail % TASK_QUEUE_SIZE], sizeof(TaskEntry));
				pThis->m_lTail = tail + 1;
				InterlockedDecrement(&pThis->m_lCount);
				bGotTask = true;
			}
		}
		LeaveCriticalSection(&pThis->m_csTaskQueue);

		if (!bGotTask)
			continue;

		// Process the task with timing
		LARGE_INTEGER liStart, liEnd;
		QueryPerformanceCounter(&liStart);

		pThis->ProcessTask(&task, workerIdx);

		QueryPerformanceCounter(&liEnd);
		LONG64 elapsedUs = ((liEnd.QuadPart - liStart.QuadPart) * 1000000) / pThis->m_liFrequency.QuadPart;

		InterlockedIncrement(&pParam->lTasksProcessed);
		InterlockedIncrement(&pThis->m_lTasksProcessed);
		InterlockedAdd64(&pParam->l64TotalProcessTimeUs, elapsedUs);
		InterlockedAdd64(&pThis->m_l64TotalProcessTimeUs, elapsedUs);
	}

	InterlockedDecrement(&pThis->m_lActiveWorkers);
	printf("[TaskProcessor] Worker %d stopped (processed=%ld)\n", workerIdx, pParam->lTasksProcessed);

	return 0;
}

// ============================================================================
// ProcessTask - Execute DB operation and send response from worker thread
// Since i3NetworkSession::SendMessage posts to IOCP, it's safe from any thread.
// ============================================================================

void TaskProcessor::ProcessTask(TaskEntry* pTask, int i32WorkerIdx)
{
	if (!m_pContext)
		return;

	// Get the DataSession by index
	DataSessionManager* pSessMgr = (DataSessionManager*)m_pContext->GetSessionManager();
	if (!pSessMgr)
		return;

	DataSession* pSession = pSessMgr->GetSession(pTask->i32SessionIdx);
	if (!pSession)
		return;

	switch (pTask->eType)
	{
	case TASK_AUTH_LOGIN:
	{
		if (!m_pContext->GetModuleDBAuth())
			return;

		IS_AUTH_LOGIN_REQ* pReq = (IS_AUTH_LOGIN_REQ*)pTask->Data;
		IS_AUTH_LOGIN_ACK ack;
		memset(&ack, 0, sizeof(ack));
		ack.i32SessionIdx = pReq->i32SessionIdx;

		m_pContext->GetModuleDBAuth()->AuthenticateUser(pReq->szUsername, pReq->szPassword, &ack);

		i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_AUTH_LOGIN_ACK);
		packet.WriteData(&ack, sizeof(ack));
		pSession->SendMessage(&packet);
		break;
	}

	case TASK_PLAYER_LOAD:
	{
		if (!m_pContext->GetModuleDBUserLoad())
			return;

		IS_PLAYER_LOAD_REQ* pReq = (IS_PLAYER_LOAD_REQ*)pTask->Data;
		IS_PLAYER_LOAD_ACK ack;
		memset(&ack, 0, sizeof(ack));
		ack.i64UID = pReq->i64UID;
		ack.i32SessionIdx = pReq->i32SessionIdx;

		char szPayload[8192];
		int i32PayloadSize = 0;

		m_pContext->GetModuleDBUserLoad()->LoadPlayerData(
			pReq->i64UID, &ack, szPayload, &i32PayloadSize);

		i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_PLAYER_LOAD_ACK);
		packet.WriteData(&ack, sizeof(ack));
		if (ack.i32Result == 0 && i32PayloadSize > 0)
			packet.WriteData(szPayload, i32PayloadSize);
		pSession->SendMessage(&packet);
		break;
	}

	case TASK_PLAYER_SAVE:
	{
		if (!m_pContext->GetModuleDBUserSave())
			return;

		IS_PLAYER_SAVE_REQ* pReq = (IS_PLAYER_SAVE_REQ*)pTask->Data;
		bool bResult = m_pContext->GetModuleDBUserSave()->SavePlayerData(
			pReq->i64UID, pReq->i32Level, pReq->i64Exp, pReq->i32Cash, pReq->i32GP);

		IS_PLAYER_SAVE_ACK ack;
		memset(&ack, 0, sizeof(ack));
		ack.i64UID = pReq->i64UID;
		ack.i32Result = bResult ? 0 : 1;

		i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_PLAYER_SAVE_ACK);
		packet.WriteData(&ack, sizeof(ack));
		pSession->SendMessage(&packet);
		break;
	}

	case TASK_PLAYER_CREATE_NICK:
	{
		if (!m_pContext->GetModuleDBAuth())
			return;

		IS_PLAYER_CREATE_NICK_REQ* pReq = (IS_PLAYER_CREATE_NICK_REQ*)pTask->Data;
		int i32Result = m_pContext->GetModuleDBAuth()->CreateNickname(pReq->i64UID, pReq->szNickname);

		IS_PLAYER_CREATE_NICK_ACK ack;
		memset(&ack, 0, sizeof(ack));
		ack.i64UID = pReq->i64UID;
		ack.i32SessionIdx = pReq->i32SessionIdx;
		ack.i32Result = i32Result;

		i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_PLAYER_CREATE_NICK_ACK);
		packet.WriteData(&ack, sizeof(ack));
		pSession->SendMessage(&packet);
		break;
	}

	case TASK_PLAYER_CHECK_NICK:
	{
		if (!m_pContext->GetModuleDBAuth())
			return;

		IS_PLAYER_CHECK_NICK_REQ* pReq = (IS_PLAYER_CHECK_NICK_REQ*)pTask->Data;
		bool bExists = m_pContext->GetModuleDBAuth()->CheckNicknameExists(pReq->szNickname);

		IS_PLAYER_CHECK_NICK_ACK ack;
		memset(&ack, 0, sizeof(ack));
		ack.i32SessionIdx = pReq->i32SessionIdx;
		ack.i32Result = bExists ? 1 : 0;

		i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_PLAYER_CHECK_NICK_ACK);
		packet.WriteData(&ack, sizeof(ack));
		pSession->SendMessage(&packet);
		break;
	}

	case TASK_STATS_SAVE:
	{
		if (!m_pContext->GetModuleDBUserSave())
			return;

		IS_STATS_SAVE_REQ* pReq = (IS_STATS_SAVE_REQ*)pTask->Data;
		bool bResult = m_pContext->GetModuleDBUserSave()->SaveBattleStats(
			pReq->i64UID, pReq->i32Kills, pReq->i32Deaths,
			pReq->i32Headshots, pReq->i32Wins, pReq->i32Losses);

		IS_STATS_SAVE_ACK ack;
		memset(&ack, 0, sizeof(ack));
		ack.i64UID = pReq->i64UID;
		ack.i32Result = bResult ? 0 : 1;

		i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_STATS_SAVE_ACK);
		packet.WriteData(&ack, sizeof(ack));
		pSession->SendMessage(&packet);
		break;
	}

	case TASK_GAME_DATA:
	{
		// Game data operations dispatched by protocol ID
		// These are handled directly from DataSession for now since they
		// need complex per-handler response building
		break;
	}

	case TASK_SOCIAL:
	{
		// Social operations dispatched by protocol ID
		break;
	}

	default:
		break;
	}
}

// ============================================================================
// Response processing (main thread) - currently unused since workers send directly
// Kept for future use if response queueing is needed
// ============================================================================

void TaskProcessor::ProcessResponses()
{
	// Workers send responses directly via i3NetworkSession::SendMessage (IOCP-safe)
	// This method is a placeholder for future response-queue pattern if needed

	// Log statistics periodically
	DWORD dwNow = GetTickCount();
	if (dwNow - m_dwLastStatTime >= 60000)
	{
		m_dwLastStatTime = dwNow;
		LogStatistics();
	}
}

void TaskProcessor::LogStatistics()
{
	LONG64 totalUs = m_l64TotalProcessTimeUs;
	LONG totalProcessed = m_lTasksProcessed;
	double avgMs = (totalProcessed > 0) ? ((double)totalUs / (double)totalProcessed / 1000.0) : 0.0;

	printf("[TaskProcessor] Stats: Queued=%ld Processed=%ld Dropped=%ld "
		"Workers=%ld/%d QueuePeak=%ld AvgTime=%.2fms\n",
		m_lTasksQueued, m_lTasksProcessed, m_lTasksDropped,
		m_lActiveWorkers, m_i32WorkerCount,
		m_lPeakQueueDepth, avgMs);

	// Per-worker stats
	for (int i = 0; i < m_i32WorkerCount; i++)
	{
		LONG workerProcessed = m_Workers[i].lTasksProcessed;
		LONG64 workerUs = m_Workers[i].l64TotalProcessTimeUs;
		double workerAvg = (workerProcessed > 0) ? ((double)workerUs / (double)workerProcessed / 1000.0) : 0.0;
		printf("  Worker[%d]: Processed=%ld AvgTime=%.2fms\n", i, workerProcessed, workerAvg);
	}
}
