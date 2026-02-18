#include "pch.h"
#include "TaskProcessor.h"
#include "Config.h"
#include "RingBuffer.h"
#include "ModuleDataBase.h"
#include "ModuleGateway.h"
#include "LinkManager.h"

#define USER_LAST_RECVTIME			(60 * 30)
#define USER_ANTI_GAME_SEND_TIME	(60 * 15)

I3_CLASS_INSTANCE( CTaskProcessor, i3Thread );

CTaskProcessor* g_pTaskProcessor = NULL;

CTaskProcessor::CTaskProcessor()
{
	m_bIsRunning				= FALSE;
	m_i32LogThreadIdx			= LOG_THREAD_INVALID;

	m_i32RingReceiver			= 0;
	m_ppRingReceiver			= NULL;

	m_pRingDBUserCashGetIn		= NULL;
	m_pRingDBUserCashGetOut		= NULL;
	m_pRingDBUserCashBuyIn		= NULL;
	m_pRingDBUserCashBuyOut		= NULL;

	m_pRingDBLinkIn				= NULL;
	m_pRingDBLinkOut			= NULL;

	m_pDBUserCash				= NULL;
	m_pLinkManager				= NULL;
	m_pGateway					= NULL;
}

CTaskProcessor::~CTaskProcessor()
{
	OnDestroy();
}

BOOL CTaskProcessor::OnCreate()
{
	m_i32LogThreadIdx		= g_pLog->InsertLogBuffer();
	if( LOG_THREAD_INVALID == m_i32LogThreadIdx )													return FALSE;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create Ring Buffer
	// Receiver
	m_i32RingReceiver = g_pConfig->m_i32ServerCount;
	m_ppRingReceiver = (CRingBuffer**)i3MemAlloc( sizeof(CRingBuffer*) * m_i32RingReceiver );
	if( NULL == m_ppRingReceiver )																	return FALSE;
	for( INT32 i = 0 ; i < m_i32RingReceiver ; i++ )
	{
		m_ppRingReceiver[i] = CRingBuffer::NewObject();
		if( NULL == m_ppRingReceiver[i] )															return FALSE;
		if( FALSE == m_ppRingReceiver[i]->OnCreate( PACKETBUFFERSIZE, RING_RECIVE_COUNT ) )			return FALSE;
	}
	// DBUserCashGetIn	
	m_pRingDBUserCashGetIn = CRingBuffer::NewObject();
	if( FALSE == m_pRingDBUserCashGetIn->OnCreate( 
		sizeof(TYPE_GET_CASH_IN), RING_DBUSERCASH_GET_IN_COUNT ) )									return FALSE;

	// DBUserCashGetOut	
	m_pRingDBUserCashGetOut = CRingBuffer::NewObject();
	if( FALSE == m_pRingDBUserCashGetOut->OnCreate( 
		sizeof(TYPE_GET_CASH_OUT), RING_DBUSERCASH_GET_OUT_COUNT ) )								return FALSE;

	// DBUserCashBuyIn	
	m_pRingDBUserCashBuyIn = CRingBuffer::NewObject();
	if( FALSE == m_pRingDBUserCashBuyIn->OnCreate( 
		sizeof(TYPE_ITEM_BUY_IN), RING_DBUSERCASH_BUY_IN_COUNT ) )									return FALSE;

	// DBUserCashBuyOut	
	m_pRingDBUserCashBuyOut = CRingBuffer::NewObject();
	if( FALSE == m_pRingDBUserCashBuyOut->OnCreate( 
		sizeof(TYPE_ITEM_BUY_OUT), RING_DBUSERCASH_BUY_OUT_COUNT ) )								return FALSE;

	// DBUserCashLinkIn	
	m_pRingDBLinkIn = CRingBuffer::NewObject();
	if( FALSE == m_pRingDBLinkIn->OnCreate( 
		sizeof(TYPE_LINK_IN), RING_DBUSERCASH_LINK_IN_COUNT ) )										return FALSE;

	// DBUserCashLinkOut	
	m_pRingDBLinkOut = CRingBuffer::NewObject();
	if( FALSE == m_pRingDBLinkOut->OnCreate( 
		sizeof(TYPE_LINK_OUT), RING_DBUSERCASH_LINK_OUT_COUNT ) )									return FALSE;

	TYPE_RING_BUFFER_SET RingBufferSet;
	RingBufferSet._pRingGetIn	= m_pRingDBUserCashGetIn;
	RingBufferSet._pRingGetOut	= m_pRingDBUserCashGetOut;
	RingBufferSet._pRingBuyIn	= m_pRingDBUserCashBuyIn;
	RingBufferSet._pRingBuyOut	= m_pRingDBUserCashBuyOut;
	RingBufferSet._pRingLinkIn	= m_pRingDBLinkIn;
	RingBufferSet._pRingLinkOut	= m_pRingDBLinkOut;
	WriteLog( g_i32MainLogThreadIdx, "[Task] Ring Buffer Create Success");
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create Database
	m_pDBUserCash = CModuleDataBase::NewObject();
	if( NULL == m_pDBUserCash )																		return FALSE;
	if( FALSE == m_pDBUserCash->OnCreate( "Billing.udl", &RingBufferSet ) )							return FALSE;

	// Create Link
	m_pLinkManager = CLinkManager::NewObject();
	if( NULL == m_pLinkManager )																	return FALSE;
	if( FALSE == m_pLinkManager->OnCreate( m_pRingDBLinkIn, m_pRingDBLinkOut ) )					return FALSE;

	// Create Gateway
	m_pGateway = CModuleGateway::NewObject();
	if( NULL == m_pGateway )																		return FALSE;
	if( FALSE == m_pGateway->OnCreate( g_pConfig->m_ui32IP, g_pConfig->m_ui16Port, 
		g_pConfig->m_i32ServerCount, g_pConfig->m_pui32ServerIp, m_ppRingReceiver ) )				return FALSE;

	WriteLog( g_i32MainLogThreadIdx, "[Task] Task Member Create Success");

	if( FALSE == i3Thread::Create( "TaskProcessor" ) )												return FALSE;

	return TRUE;
}

void CTaskProcessor::OnDestroy()
{
	m_bIsRunning = FALSE;
	i3Thread::WaitForFinish();

	I3_SAFE_RELEASE( m_pDBUserCash );
	I3_SAFE_RELEASE( m_pLinkManager );
	I3_SAFE_RELEASE( m_pGateway );
	
	// Destroy Ring Buffer
	I3_SAFE_RELEASE( m_pRingDBUserCashGetIn );
	I3_SAFE_RELEASE( m_pRingDBUserCashGetOut );
	I3_SAFE_RELEASE( m_pRingDBUserCashBuyIn );
	I3_SAFE_RELEASE( m_pRingDBUserCashBuyOut );
	I3_SAFE_RELEASE( m_pRingDBLinkIn );
	I3_SAFE_RELEASE( m_pRingDBLinkOut );
	
	if( m_ppRingReceiver )
	{
		for( INT32 i = 0 ; i < m_i32RingReceiver ; i++ )
		{
			I3_SAFE_RELEASE( m_ppRingReceiver[i] );
		}
		I3MEM_SAFE_FREE( m_ppRingReceiver );
	}
}

UINT32 CTaskProcessor::OnRunning(void* pUserData)
{
	INT32 i;
	INT32 i32PopCount;

	m_bIsRunning = TRUE;
	while( m_bIsRunning )
	{
		// Receiver
		for( i = 0 ; i < m_i32RingReceiver ; i++ )
		{
			i32PopCount = m_ppRingReceiver[i]->GetBufferCount();
			if( 0 < i32PopCount )
			{
				void* pBuffer = m_ppRingReceiver[i]->Pop();
				if( pBuffer )
				{
					_ParsingServer( i, pBuffer );
					m_ppRingReceiver[i]->PopIdx();
				}
				else
				{
					WriteLog( m_i32LogThreadIdx, "[Task] RingPop Error : m_ppRingReceiver");
				}
			}
		}
		// DBUserCashGetOut
		i32PopCount = m_pRingDBUserCashGetOut->GetBufferCount();
		if( 0 < i32PopCount )
		{
			TYPE_GET_CASH_OUT* pGetCashOut = (TYPE_GET_CASH_OUT*)m_pRingDBUserCashGetOut->Pop();
			if( pGetCashOut )
			{
				UINT8 ui8IDSize = (UINT8)i3String::Length( pGetCashOut->_strID );

				i3NetworkPacket SendPacket( PROTOCOL_BILLING_GET_CASH_ACK );
				SendPacket.WriteData( &ui8IDSize,					sizeof(UINT8)	);
				SendPacket.WriteData( pGetCashOut->_strID,			ui8IDSize		);
				SendPacket.WriteData( &pGetCashOut->_i64UID,		sizeof(INT64)	);
				SendPacket.WriteData( &pGetCashOut->_i32ReturnCode, sizeof(INT32)	);
				SendPacket.WriteData( &pGetCashOut->_i64Cash,		sizeof(INT64)	);
				ServerSendPacket( pGetCashOut->_i32ServerIdx, &SendPacket );

				m_pRingDBUserCashGetOut->PopIdx();
			}
			else
			{
				WriteLog( m_i32LogThreadIdx, "[Task] RingPop Error : m_pRingDBUserCashGetOut");
			}
		}
		// DBUserCashBuyOut
		i32PopCount = m_pRingDBUserCashBuyOut->GetBufferCount();
		if( 0 < i32PopCount )
		{
			TYPE_ITEM_BUY_OUT* pItemBuyOut = (TYPE_ITEM_BUY_OUT*)m_pRingDBUserCashBuyOut->Pop();
			if( pItemBuyOut )
			{
				UINT8 ui8IDSize = (UINT8)i3String::Length( pItemBuyOut->_strID );

				i3NetworkPacket SendPacket( PROTOCOL_BILLING_ITEM_BUY_ACK );
				SendPacket.WriteData( &ui8IDSize,							sizeof(UINT8)	);
				SendPacket.WriteData( pItemBuyOut->_strID,					ui8IDSize		);
				SendPacket.WriteData( &pItemBuyOut->_i64UID,				sizeof(INT64)	);
				SendPacket.WriteData( &pItemBuyOut->_i32ReturnCode,			sizeof(INT32)	);
				SendPacket.WriteData( &pItemBuyOut->_i64Cash,				sizeof(INT64)	);
				SendPacket.WriteData( &pItemBuyOut->_ui32MainBufferIdx,		sizeof(UINT32)	);
				ServerSendPacket( pItemBuyOut->_i32ServerIdx, &SendPacket );

				m_pRingDBUserCashBuyOut->PopIdx();
			}
			else
			{
				WriteLog( m_i32LogThreadIdx, "[Task] RingPop Error : m_pRingDBUserCashBuyOut");
			}
		}

		if(0 == i32PopCount)
		{
			Sleep(1);
		}
	}

	return 0;
}

BOOL CTaskProcessor::ServerSendPacket( INT32 i32Idx, i3NetworkPacket* pPacket )
{
	return m_pGateway->SendPacket( i32Idx, pPacket );
}
