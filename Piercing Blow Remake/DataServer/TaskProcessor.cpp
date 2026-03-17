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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TaskProcessor - Production-grade async DB worker thread pool
// Pattern: Request queue -> Worker threads (DB connections from pool) -> Response queue -> Main thread sends
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TaskProcessor::TaskProcessor(DataServerContext* pContext)
	: m_pContext(pContext)
	, m_lTaskHead(0)
	, m_lTaskTail(0)
	, m_lRespHead(0)
	, m_lRespTail(0)
	, m_i32WorkerCount(0)
	, m_lShutdown(0)
	, m_hTaskSemaphore(NULL)
	, m_lTasksQueued(0)
	, m_lTasksProcessed(0)
	, m_lTasksDropped(0)
	, m_lResponsesPending(0)
	, m_lActiveWorkers(0)
	, m_lPeakQueueDepth(0)
	, m_l64TotalProcessTimeUs(0)
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
		printf("[TaskProcessor] ERROR: Failed to create semaphore (err=%d)\n", GetLastError());
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
			printf("[TaskProcessor] ERROR: Failed to create worker thread %d (err=%d)\n",
				i, GetLastError());
			m_i32WorkerCount = i;
			break;
		}
	}

	m_dwLastStatTime = GetTickCount();

	printf("[TaskProcessor] Initialized with %d worker threads (queue=%d, response=%d)\n",
		m_i32WorkerCount, TASK_QUEUE_SIZE, TASK_RESPONSE_SIZE);

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

	// Wait for all worker threads to finish
	HANDLE handles[TASK_MAX_WORKERS];
	int handleCount = 0;
	for (int i = 0; i < m_i32WorkerCount; i++)
	{
		if (m_Workers[i].hThread)
			handles[handleCount++] = m_Workers[i].hThread;
	}

	if (handleCount > 0)
	{
		DWORD dwWait = WaitForMultipleObjects(handleCount, handles, TRUE, 10000);
		if (dwWait == WAIT_TIMEOUT)
			printf("[TaskProcessor] WARNING: Worker threads did not exit within timeout\n");
	}

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

	printf("[TaskProcessor] Shutdown complete. Stats: queued=%ld, processed=%ld, dropped=%ld, peakDepth=%ld\n",
		m_lTasksQueued, m_lTasksProcessed, m_lTasksDropped, m_lPeakQueueDepth);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// QueueTask - Called from IOCP network threads, must be thread-safe
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool TaskProcessor::QueueTask(TaskType eType, int i32SessionIdx, uint16_t ui16ProtocolId,
							   const void* pData, int i32DataSize)
{
	if (m_lShutdown)
		return false;

	if (i32DataSize > (int)sizeof(TaskEntry::Data))
	{
		printf("[TaskProcessor] WARNING: Task data too large (%d bytes), dropping\n", i32DataSize);
		InterlockedIncrement(&m_lTasksDropped);
		return false;
	}

	EnterCriticalSection(&m_csTaskQueue);

	// Check if queue is full
	LONG lNextHead = (m_lTaskHead + 1) % TASK_QUEUE_SIZE;
	if (lNextHead == m_lTaskTail)
	{
		LeaveCriticalSection(&m_csTaskQueue);
		InterlockedIncrement(&m_lTasksDropped);
		printf("[TaskProcessor] WARNING: Queue full, dropping task (type=%d, session=%d)\n",
			eType, i32SessionIdx);
		return false;
	}

	// Write to ring buffer
	TaskEntry* pEntry = &m_TaskQueue[m_lTaskHead];
	pEntry->eType = eType;
	pEntry->i32SessionIdx = i32SessionIdx;
	pEntry->ui16ProtocolId = ui16ProtocolId;
	pEntry->i32DataSize = i32DataSize;
	pEntry->dwQueueTime = GetTickCount();

	if (pData && i32DataSize > 0)
		memcpy(pEntry->Data, pData, i32DataSize);

	// Advance head
	InterlockedExchange(&m_lTaskHead, lNextHead);

	// Track peak queue depth
	LONG lDepth = (m_lTaskHead - m_lTaskTail + TASK_QUEUE_SIZE) % TASK_QUEUE_SIZE;
	LONG lPeak = m_lPeakQueueDepth;
	while (lDepth > lPeak)
	{
		LONG lOld = InterlockedCompareExchange(&m_lPeakQueueDepth, lDepth, lPeak);
		if (lOld == lPeak)
			break;
		lPeak = lOld;
	}

	LeaveCriticalSection(&m_csTaskQueue);

	InterlockedIncrement(&m_lTasksQueued);

	// Signal a worker thread
	ReleaseSemaphore(m_hTaskSemaphore, 1, NULL);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Worker Thread
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
			if (pThis->m_lTaskTail != pThis->m_lTaskHead)
			{
				memcpy(&task, &pThis->m_TaskQueue[pThis->m_lTaskTail], sizeof(TaskEntry));
				InterlockedExchange(&pThis->m_lTaskTail,
					(pThis->m_lTaskTail + 1) % TASK_QUEUE_SIZE);
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
		InterlockedExchangeAdd64(&pParam->l64TotalProcessTimeUs, elapsedUs);
		InterlockedExchangeAdd64(&pThis->m_l64TotalProcessTimeUs, elapsedUs);
	}

	InterlockedDecrement(&pThis->m_lActiveWorkers);
	printf("[TaskProcessor] Worker %d stopped (processed=%ld)\n", workerIdx, pParam->lTasksProcessed);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ProcessTask - Execute DB operation on worker thread, push response to queue
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TaskProcessor::ProcessTask(TaskEntry* pTask, int i32WorkerIdx)
{
	if (!m_pContext)
		return;

	// Acquire a DB connection from the pool (blocking wait)
	DBConnectionPool* pPool = m_pContext->GetDBPool();
	if (!pPool)
		return;

	DBConnection* pConn = pPool->AcquireConnectionWait(5000);
	if (!pConn)
	{
		printf("[TaskProcessor:W%d] Failed to acquire DB connection for task type=%d\n",
			i32WorkerIdx, pTask->eType);
		return;
	}

	// Prepare response
	TaskResponse response;
	memset(&response, 0, sizeof(response));
	response.i32SessionIdx = pTask->i32SessionIdx;

	switch (pTask->eType)
	{
	case TASK_AUTH_LOGIN:
	{
		IS_AUTH_LOGIN_REQ* pReq = (IS_AUTH_LOGIN_REQ*)pTask->Data;

		IS_AUTH_LOGIN_ACK ack;
		memset(&ack, 0, sizeof(ack));
		ack.i32SessionIdx = pReq->i32SessionIdx;

		if (m_pContext->GetModuleDBAuth())
		{
			m_pContext->GetModuleDBAuth()->AuthenticateUser(
				pReq->szUsername, pReq->szPassword, &ack);
		}

		printf("[TaskProcessor:W%d] AUTH_LOGIN: user='%s' result=%d uid=%lld\n",
			i32WorkerIdx, pReq->szUsername, ack.i32Result, ack.i64UID);

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_AUTH_LOGIN_ACK;
		memcpy(response.Data, &ui16Proto, sizeof(ui16Proto));
		memcpy(response.Data + sizeof(ui16Proto), &ack, sizeof(ack));
		response.i32DataSize = sizeof(ui16Proto) + sizeof(ack);
		break;
	}

	case TASK_PLAYER_LOAD:
	{
		IS_PLAYER_LOAD_REQ* pReq = (IS_PLAYER_LOAD_REQ*)pTask->Data;

		IS_PLAYER_LOAD_ACK ack;
		memset(&ack, 0, sizeof(ack));
		ack.i64UID = pReq->i64UID;
		ack.i32SessionIdx = pReq->i32SessionIdx;

		char szPayload[7168];	// Leave room for header in response buffer
		int i32PayloadSize = 0;

		if (m_pContext->GetModuleDBUserLoad())
		{
			m_pContext->GetModuleDBUserLoad()->LoadPlayerData(
				pReq->i64UID, &ack, szPayload, &i32PayloadSize);
		}

		printf("[TaskProcessor:W%d] PLAYER_LOAD: uid=%lld result=%d payloadSize=%d\n",
			i32WorkerIdx, pReq->i64UID, ack.i32Result, i32PayloadSize);

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_PLAYER_LOAD_ACK;
		int i32Offset = 0;
		memcpy(response.Data + i32Offset, &ui16Proto, sizeof(ui16Proto));
		i32Offset += sizeof(ui16Proto);
		memcpy(response.Data + i32Offset, &ack, sizeof(ack));
		i32Offset += sizeof(ack);
		if (ack.i32Result == 0 && i32PayloadSize > 0)
		{
			memcpy(response.Data + i32Offset, szPayload, i32PayloadSize);
			i32Offset += i32PayloadSize;
		}
		response.i32DataSize = i32Offset;
		break;
	}

	case TASK_PLAYER_SAVE:
	{
		IS_PLAYER_SAVE_REQ* pReq = (IS_PLAYER_SAVE_REQ*)pTask->Data;

		bool bResult = false;
		if (m_pContext->GetModuleDBUserSave())
		{
			bResult = m_pContext->GetModuleDBUserSave()->SavePlayerData(
				pReq->i64UID, pReq->i32Level, pReq->i64Exp, pReq->i32Cash, pReq->i32GP);
		}

		printf("[TaskProcessor:W%d] PLAYER_SAVE: uid=%lld result=%d\n",
			i32WorkerIdx, pReq->i64UID, bResult ? 0 : 1);

		IS_PLAYER_SAVE_ACK ack;
		memset(&ack, 0, sizeof(ack));
		ack.i64UID = pReq->i64UID;
		ack.i32Result = bResult ? 0 : 1;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_PLAYER_SAVE_ACK;
		memcpy(response.Data, &ui16Proto, sizeof(ui16Proto));
		memcpy(response.Data + sizeof(ui16Proto), &ack, sizeof(ack));
		response.i32DataSize = sizeof(ui16Proto) + sizeof(ack);
		break;
	}

	case TASK_PLAYER_CREATE_NICK:
	{
		IS_PLAYER_CREATE_NICK_REQ* pReq = (IS_PLAYER_CREATE_NICK_REQ*)pTask->Data;

		int i32Result = 2;	// default error
		if (m_pContext->GetModuleDBAuth())
		{
			i32Result = m_pContext->GetModuleDBAuth()->CreateNickname(
				pReq->i64UID, pReq->szNickname);
		}

		printf("[TaskProcessor:W%d] CREATE_NICK: uid=%lld nick='%s' result=%d\n",
			i32WorkerIdx, pReq->i64UID, pReq->szNickname, i32Result);

		IS_PLAYER_CREATE_NICK_ACK ack;
		memset(&ack, 0, sizeof(ack));
		ack.i64UID = pReq->i64UID;
		ack.i32SessionIdx = pReq->i32SessionIdx;
		ack.i32Result = i32Result;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_PLAYER_CREATE_NICK_ACK;
		memcpy(response.Data, &ui16Proto, sizeof(ui16Proto));
		memcpy(response.Data + sizeof(ui16Proto), &ack, sizeof(ack));
		response.i32DataSize = sizeof(ui16Proto) + sizeof(ack);
		break;
	}

	case TASK_PLAYER_CHECK_NICK:
	{
		IS_PLAYER_CHECK_NICK_REQ* pReq = (IS_PLAYER_CHECK_NICK_REQ*)pTask->Data;

		bool bExists = false;
		if (m_pContext->GetModuleDBAuth())
		{
			bExists = m_pContext->GetModuleDBAuth()->CheckNicknameExists(pReq->szNickname);
		}

		printf("[TaskProcessor:W%d] CHECK_NICK: nick='%s' exists=%d\n",
			i32WorkerIdx, pReq->szNickname, bExists);

		IS_PLAYER_CHECK_NICK_ACK ack;
		memset(&ack, 0, sizeof(ack));
		ack.i32SessionIdx = pReq->i32SessionIdx;
		ack.i32Result = bExists ? 1 : 0;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_PLAYER_CHECK_NICK_ACK;
		memcpy(response.Data, &ui16Proto, sizeof(ui16Proto));
		memcpy(response.Data + sizeof(ui16Proto), &ack, sizeof(ack));
		response.i32DataSize = sizeof(ui16Proto) + sizeof(ack);
		break;
	}

	case TASK_STATS_SAVE:
	{
		IS_STATS_SAVE_REQ* pReq = (IS_STATS_SAVE_REQ*)pTask->Data;

		bool bResult = false;
		if (m_pContext->GetModuleDBUserSave())
		{
			bResult = m_pContext->GetModuleDBUserSave()->SaveBattleStats(
				pReq->i64UID, pReq->i32Kills, pReq->i32Deaths,
				pReq->i32Headshots, pReq->i32Wins, pReq->i32Losses);
		}

		printf("[TaskProcessor:W%d] STATS_SAVE: uid=%lld result=%d\n",
			i32WorkerIdx, pReq->i64UID, bResult ? 0 : 1);

		IS_STATS_SAVE_ACK ack;
		memset(&ack, 0, sizeof(ack));
		ack.i64UID = pReq->i64UID;
		ack.i32Result = bResult ? 0 : 1;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_STATS_SAVE_ACK;
		memcpy(response.Data, &ui16Proto, sizeof(ui16Proto));
		memcpy(response.Data + sizeof(ui16Proto), &ack, sizeof(ack));
		response.i32DataSize = sizeof(ui16Proto) + sizeof(ack);
		break;
	}

	case TASK_GAME_DATA:
	{
		ProcessGameDataTask(pTask, i32WorkerIdx, &response);
		break;
	}

	case TASK_SOCIAL:
	{
		ProcessSocialTask(pTask, i32WorkerIdx, &response);
		break;
	}

	default:
		printf("[TaskProcessor:W%d] Unknown task type %d\n", i32WorkerIdx, pTask->eType);
		break;
	}

	// Release DB connection
	pPool->ReleaseConnection(pConn);

	// Push response to response queue (main thread will send it)
	if (response.i32DataSize > 0)
	{
		EnterCriticalSection(&m_csRespQueue);

		LONG lNextHead = (m_lRespHead + 1) % TASK_RESPONSE_SIZE;
		if (lNextHead != m_lRespTail)
		{
			memcpy(&m_ResponseQueue[m_lRespHead], &response, sizeof(TaskResponse));
			InterlockedExchange(&m_lRespHead, lNextHead);
			InterlockedIncrement(&m_lResponsesPending);
		}
		else
		{
			printf("[TaskProcessor:W%d] WARNING: Response queue full, dropping response for session %d\n",
				i32WorkerIdx, response.i32SessionIdx);
		}

		LeaveCriticalSection(&m_csRespQueue);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ProcessResponses - Main thread drains the response queue and sends packets
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TaskProcessor::ProcessResponses()
{
	// Drain response queue
	EnterCriticalSection(&m_csRespQueue);

	while (m_lRespTail != m_lRespHead)
	{
		TaskResponse resp;
		memcpy(&resp, &m_ResponseQueue[m_lRespTail], sizeof(TaskResponse));
		InterlockedExchange(&m_lRespTail, (m_lRespTail + 1) % TASK_RESPONSE_SIZE);
		InterlockedDecrement(&m_lResponsesPending);

		// We release the lock while sending so other workers can push responses
		LeaveCriticalSection(&m_csRespQueue);

		// Find the DataSession by index and send the packet
		DataSessionManager* pSessMgr = m_pContext->GetDataSessionManager();
		if (pSessMgr && resp.i32DataSize > (int)sizeof(uint16_t))
		{
			DataSession* pSession = pSessMgr->GetSession(resp.i32SessionIdx);
			if (pSession && pSession->GetIsActive())
			{
				// Extract protocol ID from serialized data
				uint16_t ui16Proto;
				memcpy(&ui16Proto, resp.Data, sizeof(ui16Proto));

				i3NetworkPacket packet((PROTOCOL)ui16Proto);
				packet.WriteData(resp.Data + sizeof(ui16Proto),
					resp.i32DataSize - sizeof(ui16Proto));
				pSession->SendPacketMessage(&packet);
			}
		}

		// Re-acquire lock for next iteration
		EnterCriticalSection(&m_csRespQueue);
	}

	LeaveCriticalSection(&m_csRespQueue);

	// Log statistics periodically (every 60 seconds)
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
		"Workers=%ld/%d QueuePeak=%ld RespPending=%ld AvgTime=%.2fms\n",
		m_lTasksQueued, m_lTasksProcessed, m_lTasksDropped,
		m_lActiveWorkers, m_i32WorkerCount,
		m_lPeakQueueDepth, m_lResponsesPending, avgMs);

	for (int i = 0; i < m_i32WorkerCount; i++)
	{
		LONG workerProcessed = m_Workers[i].lTasksProcessed;
		LONG64 workerUs = m_Workers[i].l64TotalProcessTimeUs;
		double workerAvg = (workerProcessed > 0) ? ((double)workerUs / (double)workerProcessed / 1000.0) : 0.0;
		printf("  Worker[%d]: Processed=%ld AvgTime=%.2fms\n", i, workerProcessed, workerAvg);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Game Data task processing (worker thread)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TaskProcessor::ProcessGameDataTask(TaskEntry* pTask, int i32WorkerIdx, TaskResponse* pResponse)
{
	ModuleDBGameData* pModule = m_pContext->GetModuleDBGameData();
	if (!pModule)
		return;

	switch (pTask->ui16ProtocolId)
	{
	case PROTOCOL_IS_EQUIPMENT_SAVE_REQ:
	{
		IS_EQUIPMENT_SAVE_REQ* pReq = (IS_EQUIPMENT_SAVE_REQ*)pTask->Data;
		bool bResult = pModule->SaveEquipment(pReq->i64UID, pReq);

		IS_EQUIPMENT_SAVE_ACK ack;
		ack.i64UID = pReq->i64UID;
		ack.i32Result = bResult ? 0 : 1;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_EQUIPMENT_SAVE_ACK;
		memcpy(pResponse->Data, &ui16Proto, sizeof(ui16Proto));
		memcpy(pResponse->Data + sizeof(ui16Proto), &ack, sizeof(ack));
		pResponse->i32DataSize = sizeof(ui16Proto) + sizeof(ack);
		break;
	}

	case PROTOCOL_IS_MEDAL_SAVE_REQ:
	{
		IS_MEDAL_SAVE_REQ* pReq = (IS_MEDAL_SAVE_REQ*)pTask->Data;
		bool bResult = pModule->SaveMedal(pReq->i64UID, pReq);

		IS_MEDAL_SAVE_ACK ack;
		ack.i64UID = pReq->i64UID;
		ack.i32Result = bResult ? 0 : 1;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_MEDAL_SAVE_ACK;
		memcpy(pResponse->Data, &ui16Proto, sizeof(ui16Proto));
		memcpy(pResponse->Data + sizeof(ui16Proto), &ack, sizeof(ack));
		pResponse->i32DataSize = sizeof(ui16Proto) + sizeof(ack);
		break;
	}

	case PROTOCOL_IS_ATTENDANCE_SAVE_REQ:
	{
		IS_ATTENDANCE_SAVE_REQ* pReq = (IS_ATTENDANCE_SAVE_REQ*)pTask->Data;
		bool bResult = pModule->SaveAttendance(pReq->i64UID, pReq);

		IS_ATTENDANCE_SAVE_ACK ack;
		ack.i64UID = pReq->i64UID;
		ack.i32Result = bResult ? 0 : 1;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_ATTENDANCE_SAVE_ACK;
		memcpy(pResponse->Data, &ui16Proto, sizeof(ui16Proto));
		memcpy(pResponse->Data + sizeof(ui16Proto), &ack, sizeof(ack));
		pResponse->i32DataSize = sizeof(ui16Proto) + sizeof(ack);
		break;
	}

	case PROTOCOL_IS_SKILL_SAVE_REQ:
	{
		IS_SKILL_SAVE_REQ* pReq = (IS_SKILL_SAVE_REQ*)pTask->Data;
		bool bResult = pModule->SaveSkill(pReq->i64UID, pReq);

		IS_SKILL_SAVE_ACK ack;
		ack.i64UID = pReq->i64UID;
		ack.i32Result = bResult ? 0 : 1;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_SKILL_SAVE_ACK;
		memcpy(pResponse->Data, &ui16Proto, sizeof(ui16Proto));
		memcpy(pResponse->Data + sizeof(ui16Proto), &ack, sizeof(ack));
		pResponse->i32DataSize = sizeof(ui16Proto) + sizeof(ack);
		break;
	}

	case PROTOCOL_IS_QUEST_SAVE_REQ:
	{
		IS_QUEST_SAVE_REQ* pReq = (IS_QUEST_SAVE_REQ*)pTask->Data;
		const char* pQuestData = pTask->Data + sizeof(IS_QUEST_SAVE_REQ);
		int i32QuestDataSize = (int)pReq->ui16DataSize;

		if (pTask->i32DataSize < (int)(sizeof(IS_QUEST_SAVE_REQ) + i32QuestDataSize))
			break;

		bool bResult = pModule->SaveQuest(pReq->i64UID, pReq, pQuestData, i32QuestDataSize);

		IS_QUEST_SAVE_ACK ack;
		ack.i64UID = pReq->i64UID;
		ack.i32Result = bResult ? 0 : 1;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_QUEST_SAVE_ACK;
		memcpy(pResponse->Data, &ui16Proto, sizeof(ui16Proto));
		memcpy(pResponse->Data + sizeof(ui16Proto), &ack, sizeof(ack));
		pResponse->i32DataSize = sizeof(ui16Proto) + sizeof(ack);
		break;
	}

	case PROTOCOL_IS_OPTION_SAVE_REQ:
	{
		IS_OPTION_SAVE_REQ* pReq = (IS_OPTION_SAVE_REQ*)pTask->Data;
		const char* pOptionsData = pTask->Data + sizeof(IS_OPTION_SAVE_REQ);
		int i32OptionsDataSize = (int)pReq->ui16DataSize;

		if (pTask->i32DataSize < (int)(sizeof(IS_OPTION_SAVE_REQ) + i32OptionsDataSize))
			break;

		bool bResult = pModule->SaveOptions(pReq->i64UID, pOptionsData, i32OptionsDataSize);

		IS_OPTION_SAVE_ACK ack;
		ack.i64UID = pReq->i64UID;
		ack.i32Result = bResult ? 0 : 1;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_OPTION_SAVE_ACK;
		memcpy(pResponse->Data, &ui16Proto, sizeof(ui16Proto));
		memcpy(pResponse->Data + sizeof(ui16Proto), &ack, sizeof(ack));
		pResponse->i32DataSize = sizeof(ui16Proto) + sizeof(ack);
		break;
	}

	case PROTOCOL_IS_OPTION_LOAD_REQ:
	{
		IS_OPTION_LOAD_REQ* pReq = (IS_OPTION_LOAD_REQ*)pTask->Data;

		const int MAX_OPTIONS_SIZE = 2048;
		char optBuf[MAX_OPTIONS_SIZE];
		int i32Read = pModule->LoadOptions(pReq->i64UID, optBuf, MAX_OPTIONS_SIZE);

		IS_OPTION_LOAD_ACK ack;
		ack.i64UID       = pReq->i64UID;
		ack.i32SessionIdx = pReq->i32SessionIdx;
		ack.i32Result    = (i32Read > 0) ? 0 : 1;
		ack.ui16DataSize = (i32Read > 0) ? (uint16_t)i32Read : 0;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_OPTION_LOAD_ACK;
		int i32Offset = 0;
		memcpy(pResponse->Data + i32Offset, &ui16Proto, sizeof(ui16Proto));		i32Offset += sizeof(ui16Proto);
		memcpy(pResponse->Data + i32Offset, &ack, sizeof(ack));				i32Offset += sizeof(ack);
		if (ack.ui16DataSize > 0 && i32Offset + ack.ui16DataSize <= (int)sizeof(pResponse->Data))
		{
			memcpy(pResponse->Data + i32Offset, optBuf, ack.ui16DataSize);
			i32Offset += ack.ui16DataSize;
		}
		pResponse->i32DataSize = i32Offset;
		break;
	}

	case PROTOCOL_IS_MEDAL_SET_LOAD_REQ:
	{
		IS_MEDAL_SET_LOAD_REQ* pReq = (IS_MEDAL_SET_LOAD_REQ*)pTask->Data;

		const int MAX_SETS = 20;
		IS_MEDAL_SET_CURRENT_ENTRY sets[MAX_SETS];
		int i32Count = pModule->LoadMedalSets(pReq->i64UID, sets, MAX_SETS);

		IS_MEDAL_SET_LOAD_ACK ack;
		ack.i64UID        = pReq->i64UID;
		ack.i32SessionIdx = pReq->i32SessionIdx;
		ack.i32Result     = 0;
		ack.ui16Count     = (uint16_t)i32Count;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_MEDAL_SET_LOAD_ACK;
		int i32Offset = 0;
		memcpy(pResponse->Data + i32Offset, &ui16Proto, sizeof(ui16Proto));		i32Offset += sizeof(ui16Proto);
		memcpy(pResponse->Data + i32Offset, &ack, sizeof(ack));				i32Offset += sizeof(ack);
		if (i32Count > 0)
		{
			int i32SetSize = i32Count * (int)sizeof(IS_MEDAL_SET_CURRENT_ENTRY);
			if (i32Offset + i32SetSize <= (int)sizeof(pResponse->Data))
			{
				memcpy(pResponse->Data + i32Offset, sets, i32SetSize);
				i32Offset += i32SetSize;
			}
		}
		pResponse->i32DataSize = i32Offset;
		break;
	}

	case PROTOCOL_IS_SHOP_LIST_REQ:
	{
		IS_SHOP_ITEM_ENTRY items[500];
		memset(items, 0, sizeof(items));
		int i32Count = pModule->LoadShopItems(items, 500);

		IS_SHOP_LIST_ACK ack;
		ack.i32Result = 0;
		ack.i32ItemCount = i32Count;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_SHOP_LIST_ACK;
		int i32Offset = 0;
		memcpy(pResponse->Data + i32Offset, &ui16Proto, sizeof(ui16Proto));
		i32Offset += sizeof(ui16Proto);
		memcpy(pResponse->Data + i32Offset, &ack, sizeof(ack));
		i32Offset += sizeof(ack);
		if (i32Count > 0)
		{
			int i32ItemsSize = i32Count * (int)sizeof(IS_SHOP_ITEM_ENTRY);
			if (i32Offset + i32ItemsSize <= (int)sizeof(pResponse->Data))
			{
				memcpy(pResponse->Data + i32Offset, items, i32ItemsSize);
				i32Offset += i32ItemsSize;
			}
		}
		pResponse->i32DataSize = i32Offset;

		printf("[TaskProcessor:W%d] SHOP_LIST_ACK: sent %d items\n", i32WorkerIdx, i32Count);
		break;
	}

	case PROTOCOL_IS_SHOP_BUY_REQ:
	{
		IS_SHOP_BUY_REQ* pReq = (IS_SHOP_BUY_REQ*)pTask->Data;

		printf("[TaskProcessor:W%d] SHOP_BUY_REQ: UID=%lld, GoodsId=%u, PayType=%d, Price=%d\n",
			i32WorkerIdx, pReq->i64UID, pReq->ui32GoodsId, pReq->ui8PayType, pReq->i32Price);

		IS_SHOP_BUY_ACK ack;
		memset(&ack, 0, sizeof(ack));
		ack.i64UID = pReq->i64UID;
		ack.i32SessionIdx = pReq->i32SessionIdx;
		ack.ui32ItemId = pReq->ui32ItemId;

		pModule->BuyShopItem(pReq, &ack);

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_SHOP_BUY_ACK;
		memcpy(pResponse->Data, &ui16Proto, sizeof(ui16Proto));
		memcpy(pResponse->Data + sizeof(ui16Proto), &ack, sizeof(ack));
		pResponse->i32DataSize = sizeof(ui16Proto) + sizeof(ack);
		break;
	}

	case PROTOCOL_IS_SHOP_COUPON_REQ:
	{
		IS_SHOP_COUPON_REQ* pReq = (IS_SHOP_COUPON_REQ*)pTask->Data;

		IS_SHOP_COUPON_ACK ack;
		memset(&ack, 0, sizeof(ack));
		ack.i64UID       = pReq->i64UID;
		ack.i32SessionIdx = pReq->i32SessionIdx;

		pModule->RedeemCoupon(pReq, &ack);

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_SHOP_COUPON_ACK;
		memcpy(pResponse->Data, &ui16Proto, sizeof(ui16Proto));
		memcpy(pResponse->Data + sizeof(ui16Proto), &ack, sizeof(ack));
		pResponse->i32DataSize = sizeof(ui16Proto) + sizeof(ack);
		break;
	}

	case PROTOCOL_IS_INVEN_UPDATE_REQ:
	{
		IS_INVEN_UPDATE_REQ* pReq = (IS_INVEN_UPDATE_REQ*)pTask->Data;
		bool bResult = pModule->UpdateInventory(pReq);

		IS_INVEN_UPDATE_ACK ack;
		ack.i64UID = pReq->i64UID;
		ack.i32Result = bResult ? 0 : 3;
		ack.ui32ItemId = pReq->ui32ItemId;
		ack.i32SlotIdx = pReq->i32SlotIdx;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_INVEN_UPDATE_ACK;
		memcpy(pResponse->Data, &ui16Proto, sizeof(ui16Proto));
		memcpy(pResponse->Data + sizeof(ui16Proto), &ack, sizeof(ack));
		pResponse->i32DataSize = sizeof(ui16Proto) + sizeof(ack);
		break;
	}

	default:
		printf("[TaskProcessor:W%d] Unknown game data protocol 0x%04X\n",
			i32WorkerIdx, pTask->ui16ProtocolId);
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Social task processing (worker thread)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TaskProcessor::ProcessSocialTask(TaskEntry* pTask, int i32WorkerIdx, TaskResponse* pResponse)
{
	ModuleDBSocial* pModule = m_pContext->GetModuleDBSocial();
	if (!pModule)
		return;

	switch (pTask->ui16ProtocolId)
	{
	case PROTOCOL_IS_CLAN_CREATE_REQ:
	{
		IS_CLAN_CREATE_REQ* pReq = (IS_CLAN_CREATE_REQ*)pTask->Data;
		int i32ClanId = pModule->CreateClan(pReq);

		IS_CLAN_CREATE_ACK ack;
		memset(&ack, 0, sizeof(ack));
		ack.i64MasterUID = pReq->i64MasterUID;
		ack.i32SessionIdx = pReq->i32SessionIdx;
		ack.i32ClanId = i32ClanId;
		ack.i32Result = (i32ClanId > 0) ? 0 : 1;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_CLAN_CREATE_ACK;
		memcpy(pResponse->Data, &ui16Proto, sizeof(ui16Proto));
		memcpy(pResponse->Data + sizeof(ui16Proto), &ack, sizeof(ack));
		pResponse->i32DataSize = sizeof(ui16Proto) + sizeof(ack);
		break;
	}

	case PROTOCOL_IS_CLAN_DISBAND_REQ:
	{
		IS_CLAN_DISBAND_REQ* pReq = (IS_CLAN_DISBAND_REQ*)pTask->Data;
		bool bResult = pModule->DisbandClan(pReq->i32ClanId, pReq->i64MasterUID);

		IS_CLAN_DISBAND_ACK ack;
		ack.i32ClanId = pReq->i32ClanId;
		ack.i32Result = bResult ? 0 : 1;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_CLAN_DISBAND_ACK;
		memcpy(pResponse->Data, &ui16Proto, sizeof(ui16Proto));
		memcpy(pResponse->Data + sizeof(ui16Proto), &ack, sizeof(ack));
		pResponse->i32DataSize = sizeof(ui16Proto) + sizeof(ack);
		break;
	}

	case PROTOCOL_IS_CLAN_JOIN_REQ:
	{
		IS_CLAN_JOIN_REQ* pReq = (IS_CLAN_JOIN_REQ*)pTask->Data;
		bool bResult = pModule->JoinClan(pReq);

		IS_CLAN_JOIN_ACK ack;
		ack.i32ClanId = pReq->i32ClanId;
		ack.i64UID = pReq->i64UID;
		ack.i32SessionIdx = pReq->i32SessionIdx;
		ack.i32Result = bResult ? 0 : 1;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_CLAN_JOIN_ACK;
		memcpy(pResponse->Data, &ui16Proto, sizeof(ui16Proto));
		memcpy(pResponse->Data + sizeof(ui16Proto), &ack, sizeof(ack));
		pResponse->i32DataSize = sizeof(ui16Proto) + sizeof(ack);
		break;
	}

	case PROTOCOL_IS_CLAN_LEAVE_REQ:
	{
		IS_CLAN_LEAVE_REQ* pReq = (IS_CLAN_LEAVE_REQ*)pTask->Data;
		bool bResult = pModule->LeaveClan(pReq->i32ClanId, pReq->i64UID);

		IS_CLAN_LEAVE_ACK ack;
		ack.i32ClanId = pReq->i32ClanId;
		ack.i64UID = pReq->i64UID;
		ack.i32Result = bResult ? 0 : 1;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_CLAN_LEAVE_ACK;
		memcpy(pResponse->Data, &ui16Proto, sizeof(ui16Proto));
		memcpy(pResponse->Data + sizeof(ui16Proto), &ack, sizeof(ack));
		pResponse->i32DataSize = sizeof(ui16Proto) + sizeof(ack);
		break;
	}

	case PROTOCOL_IS_CLAN_LOAD_REQ:
	{
		IS_CLAN_LOAD_REQ* pReq = (IS_CLAN_LOAD_REQ*)pTask->Data;

		printf("[TaskProcessor:W%d] CLAN_LOAD_REQ: ClanId=%d\n", i32WorkerIdx, pReq->i32ClanId);

		IS_CLAN_LOAD_ACK ack;
		memset(&ack, 0, sizeof(ack));
		ack.i32ClanId = pReq->i32ClanId;

		IS_CLAN_MEMBER_INFO members[50];
		memset(members, 0, sizeof(members));
		int memberCount = 0;

		bool bResult = pModule->LoadClan(
			pReq->i32ClanId, &ack, members, 50, &memberCount);

		ack.i32Result = bResult ? 0 : 1;
		ack.i32MemberCount = memberCount;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_CLAN_LOAD_ACK;
		int i32Offset = 0;
		memcpy(pResponse->Data + i32Offset, &ui16Proto, sizeof(ui16Proto));
		i32Offset += sizeof(ui16Proto);
		memcpy(pResponse->Data + i32Offset, &ack, sizeof(ack));
		i32Offset += sizeof(ack);
		if (memberCount > 0)
		{
			int i32MembersSize = memberCount * (int)sizeof(IS_CLAN_MEMBER_INFO);
			if (i32Offset + i32MembersSize <= (int)sizeof(pResponse->Data))
			{
				memcpy(pResponse->Data + i32Offset, members, i32MembersSize);
				i32Offset += i32MembersSize;
			}
		}
		pResponse->i32DataSize = i32Offset;
		break;
	}

	case PROTOCOL_IS_FRIEND_ADD_REQ:
	{
		IS_FRIEND_ADD_REQ* pReq = (IS_FRIEND_ADD_REQ*)pTask->Data;
		int i32Result = pModule->AddFriend(pReq->i64UID, pReq->i64FriendUID);

		IS_FRIEND_ADD_ACK ack;
		ack.i64UID = pReq->i64UID;
		ack.i64FriendUID = pReq->i64FriendUID;
		ack.i32SessionIdx = pReq->i32SessionIdx;
		ack.i32Result = i32Result;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_FRIEND_ADD_ACK;
		memcpy(pResponse->Data, &ui16Proto, sizeof(ui16Proto));
		memcpy(pResponse->Data + sizeof(ui16Proto), &ack, sizeof(ack));
		pResponse->i32DataSize = sizeof(ui16Proto) + sizeof(ack);
		break;
	}

	case PROTOCOL_IS_FRIEND_REMOVE_REQ:
	{
		IS_FRIEND_REMOVE_REQ* pReq = (IS_FRIEND_REMOVE_REQ*)pTask->Data;
		bool bResult = pModule->RemoveFriend(pReq->i64UID, pReq->i64FriendUID);

		IS_FRIEND_REMOVE_ACK ack;
		ack.i64UID = pReq->i64UID;
		ack.i64FriendUID = pReq->i64FriendUID;
		ack.i32Result = bResult ? 0 : 1;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_FRIEND_REMOVE_ACK;
		memcpy(pResponse->Data, &ui16Proto, sizeof(ui16Proto));
		memcpy(pResponse->Data + sizeof(ui16Proto), &ack, sizeof(ack));
		pResponse->i32DataSize = sizeof(ui16Proto) + sizeof(ack);
		break;
	}

	case PROTOCOL_IS_FRIEND_LIST_REQ:
	{
		IS_FRIEND_LIST_REQ* pReq = (IS_FRIEND_LIST_REQ*)pTask->Data;

		IS_FRIEND_ENTRY friends[100];
		memset(friends, 0, sizeof(friends));
		int i32Count = pModule->LoadFriendList(pReq->i64UID, friends, 100);

		IS_FRIEND_LIST_ACK ack;
		ack.i64UID = pReq->i64UID;
		ack.i32SessionIdx = pReq->i32SessionIdx;
		ack.i32Count = i32Count;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_FRIEND_LIST_ACK;
		int i32Offset = 0;
		memcpy(pResponse->Data + i32Offset, &ui16Proto, sizeof(ui16Proto));
		i32Offset += sizeof(ui16Proto);
		memcpy(pResponse->Data + i32Offset, &ack, sizeof(ack));
		i32Offset += sizeof(ack);
		if (i32Count > 0)
		{
			int i32FriendsSize = i32Count * (int)sizeof(IS_FRIEND_ENTRY);
			if (i32Offset + i32FriendsSize <= (int)sizeof(pResponse->Data))
			{
				memcpy(pResponse->Data + i32Offset, friends, i32FriendsSize);
				i32Offset += i32FriendsSize;
			}
		}
		pResponse->i32DataSize = i32Offset;
		break;
	}

	case PROTOCOL_IS_BLOCK_ADD_REQ:
	{
		IS_BLOCK_ADD_REQ* pReq = (IS_BLOCK_ADD_REQ*)pTask->Data;
		int i32Result = pModule->AddBlock(pReq->i64UID, pReq->i64BlockedUID);

		IS_BLOCK_ADD_ACK ack;
		ack.i64UID = pReq->i64UID;
		ack.i64BlockedUID = pReq->i64BlockedUID;
		ack.i32SessionIdx = pReq->i32SessionIdx;
		ack.i32Result = i32Result;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_BLOCK_ADD_ACK;
		memcpy(pResponse->Data, &ui16Proto, sizeof(ui16Proto));
		memcpy(pResponse->Data + sizeof(ui16Proto), &ack, sizeof(ack));
		pResponse->i32DataSize = sizeof(ui16Proto) + sizeof(ack);
		break;
	}

	case PROTOCOL_IS_BLOCK_REMOVE_REQ:
	{
		IS_BLOCK_REMOVE_REQ* pReq = (IS_BLOCK_REMOVE_REQ*)pTask->Data;
		bool bResult = pModule->RemoveBlock(pReq->i64UID, pReq->i64BlockedUID);

		IS_BLOCK_REMOVE_ACK ack;
		ack.i64UID = pReq->i64UID;
		ack.i64BlockedUID = pReq->i64BlockedUID;
		ack.i32Result = bResult ? 0 : 1;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_BLOCK_REMOVE_ACK;
		memcpy(pResponse->Data, &ui16Proto, sizeof(ui16Proto));
		memcpy(pResponse->Data + sizeof(ui16Proto), &ack, sizeof(ack));
		pResponse->i32DataSize = sizeof(ui16Proto) + sizeof(ack);
		break;
	}

	case PROTOCOL_IS_BLOCK_LIST_REQ:
	{
		IS_BLOCK_LIST_REQ* pReq = (IS_BLOCK_LIST_REQ*)pTask->Data;

		IS_BLOCK_ENTRY blocks[100];
		memset(blocks, 0, sizeof(blocks));
		int i32Count = pModule->LoadBlockList(pReq->i64UID, blocks, 100);

		IS_BLOCK_LIST_ACK ack;
		ack.i64UID = pReq->i64UID;
		ack.i32SessionIdx = pReq->i32SessionIdx;
		ack.i32Count = i32Count;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_BLOCK_LIST_ACK;
		int i32Offset = 0;
		memcpy(pResponse->Data + i32Offset, &ui16Proto, sizeof(ui16Proto));
		i32Offset += sizeof(ui16Proto);
		memcpy(pResponse->Data + i32Offset, &ack, sizeof(ack));
		i32Offset += sizeof(ack);
		if (i32Count > 0)
		{
			int i32BlocksSize = i32Count * (int)sizeof(IS_BLOCK_ENTRY);
			if (i32Offset + i32BlocksSize <= (int)sizeof(pResponse->Data))
			{
				memcpy(pResponse->Data + i32Offset, blocks, i32BlocksSize);
				i32Offset += i32BlocksSize;
			}
		}
		pResponse->i32DataSize = i32Offset;
		break;
	}

	case PROTOCOL_IS_NOTE_SEND_REQ:
	{
		IS_NOTE_SEND_REQ* pReq = (IS_NOTE_SEND_REQ*)pTask->Data;

		IS_NOTE_SEND_ACK ack;
		memset(&ack, 0, sizeof(ack));
		ack.i32SessionIdx = pReq->i32SessionIdx;
		ack.i32Result = pModule->SaveNote(pReq) ? 0 : 3;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_NOTE_SEND_ACK;
		memcpy(pResponse->Data, &ui16Proto, sizeof(ui16Proto));
		memcpy(pResponse->Data + sizeof(ui16Proto), &ack, sizeof(ack));
		pResponse->i32DataSize = sizeof(ui16Proto) + sizeof(ack);
		break;
	}

	case PROTOCOL_IS_NOTE_LIST_REQ:
	{
		IS_NOTE_LIST_REQ* pReq = (IS_NOTE_LIST_REQ*)pTask->Data;

		IS_NOTE_LIST_ACK ack;
		memset(&ack, 0, sizeof(ack));
		ack.i64UID = pReq->i64UID;
		ack.i32SessionIdx = pReq->i32SessionIdx;

		IS_NOTE_ENTRY entries[50];
		memset(entries, 0, sizeof(entries));
		int noteCount = pModule->LoadNotes(pReq->i64UID, entries, 50);
		ack.i32Count = noteCount;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_NOTE_LIST_ACK;
		int i32Offset = 0;
		memcpy(pResponse->Data + i32Offset, &ui16Proto, sizeof(ui16Proto));
		i32Offset += sizeof(ui16Proto);
		memcpy(pResponse->Data + i32Offset, &ack, sizeof(ack));
		i32Offset += sizeof(ack);
		if (noteCount > 0)
		{
			int i32NotesSize = noteCount * (int)sizeof(IS_NOTE_ENTRY);
			if (i32Offset + i32NotesSize <= (int)sizeof(pResponse->Data))
			{
				memcpy(pResponse->Data + i32Offset, entries, i32NotesSize);
				i32Offset += i32NotesSize;
			}
		}
		pResponse->i32DataSize = i32Offset;
		break;
	}

	case PROTOCOL_IS_NOTE_DELETE_REQ:
	{
		IS_NOTE_DELETE_REQ* pReq = (IS_NOTE_DELETE_REQ*)pTask->Data;

		IS_NOTE_DELETE_ACK ack;
		memset(&ack, 0, sizeof(ack));
		ack.i64UID = pReq->i64UID;
		ack.i64NoteId = pReq->i64NoteId;
		ack.i32Result = pModule->DeleteNote(pReq->i64UID, pReq->i64NoteId) ? 0 : 1;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_NOTE_DELETE_ACK;
		memcpy(pResponse->Data, &ui16Proto, sizeof(ui16Proto));
		memcpy(pResponse->Data + sizeof(ui16Proto), &ack, sizeof(ack));
		pResponse->i32DataSize = sizeof(ui16Proto) + sizeof(ack);
		break;
	}

	case PROTOCOL_IS_PLAYER_BAN_REQ:
	{
		IS_PLAYER_BAN_REQ* pReq = (IS_PLAYER_BAN_REQ*)pTask->Data;

		IS_PLAYER_BAN_ACK ack;
		memset(&ack, 0, sizeof(ack));
		ack.i64UID = pReq->i64UID;
		ack.i32Result = pModule->BanPlayer(pReq) ? 0 : 2;

		uint16_t ui16Proto = (uint16_t)PROTOCOL_IS_PLAYER_BAN_ACK;
		memcpy(pResponse->Data, &ui16Proto, sizeof(ui16Proto));
		memcpy(pResponse->Data + sizeof(ui16Proto), &ack, sizeof(ack));
		pResponse->i32DataSize = sizeof(ui16Proto) + sizeof(ack);
		break;
	}

	default:
		printf("[TaskProcessor:W%d] Unknown social protocol 0x%04X\n",
			i32WorkerIdx, pTask->ui16ProtocolId);
		break;
	}
}
