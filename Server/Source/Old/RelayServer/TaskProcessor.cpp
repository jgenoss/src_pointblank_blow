#include "pch.h"
#include "TaskProcessor.h"
#include "PacketLocker.h"
#include "Gateway.h"
#include "LogFile.h"
#include "Receiver.h"
#include "GroupMaker.h"

I3_CLASS_INSTANCE(CTaskProcessor, i3Thread);

CTaskProcessor* g_pTaskProcessor = NULL;

CTaskProcessor::CTaskProcessor()
{
	m_bIsRunning = FALSE;
	m_pRecvBuffer = NULL;
	m_pSendBuffer = NULL;
	m_nDataSize = 0;
	m_nReadedLength = 0;
	m_pPacketLocker = NULL;
}

CTaskProcessor::~CTaskProcessor()
{
	I3MEM_SAFE_FREE(m_pRecvBuffer);
	I3MEM_SAFE_FREE(m_pSendBuffer);
}

BOOL CTaskProcessor::Create(UINT32 nDataSize,CPacketLocker* pPacketLocker,CGateway* pGateway)
{
	I3ASSERT(NULL == m_pRecvBuffer);
	I3ASSERT(NULL == m_pSendBuffer);
	I3ASSERT(nDataSize);
	I3ASSERT(pPacketLocker);
	I3ASSERT(pGateway);

	m_pRecvBuffer = (UINT8*)i3MemAlloc(nDataSize);
	m_pSendBuffer = (UINT8*)i3MemAlloc(nDataSize);
	m_nDataSize = nDataSize;
	m_pPacketLocker = pPacketLocker;
	m_pGateway = pGateway;

	if (FALSE == i3Thread::Create("TaskProcessor", 0, 4096, NULL, PRIORITY_HIGHEST))
	{
		I3MEM_SAFE_FREE(m_pRecvBuffer);
		I3MEM_SAFE_FREE(m_pSendBuffer);
		m_nDataSize = 0;
		m_pPacketLocker = NULL;

		return FALSE;
	}

	return TRUE;
}

UINT32 CTaskProcessor::OnRunning(void* pUserData)
{
	m_bIsRunning = TRUE;

	while(m_bIsRunning)
	{
		UINT32 idx = 0;
		UINT32 nEmpty = 0;

		// 최근 읽은 버퍼로 부터 1주 하는 동안 모두 비어 있으면 잠시 쉼
		while(nEmpty < m_pPacketLocker->getCount())
		{
			m_nReadedLength = m_pPacketLocker->Pop(idx, m_pRecvBuffer, m_nDataSize);
			
			if (m_nReadedLength)
			{
				switch(CReceiver::GetReceiverType(idx))
				{
				case GAME_SERVER:		_DoTaskForGame(idx);	break;				
				default:				/* Do nothing */		break;
				}

				nEmpty = 0;
			}
			else
			{
				nEmpty++;
			}

			idx = (idx + 1) % m_pPacketLocker->getCount();
		}

		::Sleep(1);
	}

	return 0;
}

BOOL CTaskProcessor::SendPacketMessage(UINT32 idx,i3NetworkPacket* pPacket)
{
	return m_pGateway->SendPacket(idx, pPacket);
}

void CTaskProcessor::_DoTaskForGame(UINT32 idxBuffer)
{
	i3NetworkPacket packet;
	packet.CopyToBuffer((char*)m_pRecvBuffer, m_nReadedLength);

	switch(packet.GetProtocolID())
	{
	case PROTOCOL_BASE_HEART_BIT_REQ		: 
		{
			i3NetworkPacket	sendPacket(PROTOCOL_BASE_HEART_BIT_ACK);
			SendPacketMessage(idxBuffer, &sendPacket);
		}
		break;
	default: 
		{
#if 0
			char message[MAX_STRING_COUNT];
			sprintf(message, "[WARNING] CTaskProcessor::_DoTaskForGame() - Unkown protocol id(%d)", packet.GetProtocolID());
			COMMON_LOG(message);
#endif
		}
		break; 
	}

	return; 
}
