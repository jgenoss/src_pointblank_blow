#include "pch.h"
#include "ModuleDataBase.h"
#include "ModuleLogFile.h"
#include "DataBase.h"

I3_CLASS_INSTANCE(CModuleDataBase, i3Thread); 

CModuleDataBase * g_pModuleDataBase = NULL; 

#define	DB_QUERY_BUFFER_COUNT	100

CModuleDataBase::CModuleDataBase(void)
{
	m_hKillEvent	= ::CreateEvent(NULL, false, false, NULL);

	m_pMainQuery		= NULL;
	m_iMainReadIdx		= 0;
	m_iMainWriteIdx		= 0;

	m_pModuleZPQuery	= NULL;
	m_iModuleZPReadIdx	= 0;
	m_iModuleZPWriteIdx = 0;

	m_ppUserQuery		= NULL;
	m_pUserReadIdx		= NULL;
	m_pUserWriteIdx		= NULL;

	m_bIsRunning		= FALSE;
}

CModuleDataBase::~CModuleDataBase(void)
{	
	OnDestroy_M(); 
}

BOOL CModuleDataBase::OnCreate_M(INT32 iThreadCount)
{
	BOOL bRv = TRUE; 
	bRv;
	m_iThreadCount = iThreadCount; 

	//////////////////////////////////////////////////////////////////////////
	//Make Buffer 
	m_pMainQuery		= (DB_QUERY *)i3MemAlloc(sizeof(DB_QUERY) * DB_QUERY_BUFFER_COUNT);
	m_iMainReadIdx		= 0;
	m_iMainWriteIdx		= 0;

	m_pModuleZPQuery	= (DB_QUERY *)i3MemAlloc(sizeof(DB_QUERY) * DB_QUERY_BUFFER_COUNT);
	m_iModuleZPReadIdx	= 0;
	m_iModuleZPWriteIdx = 0;

	m_ppUserQuery		= (DB_QUERY **)i3MemAlloc(sizeof(DB_QUERY *) * m_iThreadCount);
	for(INT32 i= 0 ; i < m_iThreadCount; i++)
	{
		m_ppUserQuery[i] = (DB_QUERY *)i3MemAlloc( sizeof(DB_QUERY) * DB_QUERY_BUFFER_COUNT );
	}

	m_pUserReadIdx		= (INT32*)i3MemAlloc( sizeof(INT32) * m_iThreadCount );	
	m_pUserWriteIdx		= (INT32*)i3MemAlloc( sizeof(INT32) * m_iThreadCount );
	i3mem::FillZero( m_pUserReadIdx,	sizeof( m_pUserReadIdx ));
	i3mem::FillZero( m_pUserWriteIdx,	sizeof( m_pUserWriteIdx ));

	//////////////////////////////////////////////////////////////////////////
	//Create Thread
	m_bIsRunning = TRUE;
	if( !i3Thread::Create("CModuleDataBase", 0, 4096, NULL, PRIORITY_NORMAL) )return FALSE; 	

	WRITE_M_LOG(MAIN_THREAD_IDX , NULL, "START MODULE DATABASE"); 
	return TRUE; 
}

void CModuleDataBase::OnDestroy_M(void)
{
	::SetEvent( m_hKillEvent );
	::Sleep(100);
	::CloseHandle( m_hKillEvent ); 	

	i3Thread::Terminate();

	I3MEM_SAFE_FREE(m_pMainQuery); 
	I3MEM_SAFE_FREE(m_pModuleZPQuery); 

	for(INT32 i = 0; i < m_iThreadCount; i++)
	{
		I3MEM_SAFE_FREE( m_ppUserQuery[i] );
	}
	I3MEM_SAFE_FREE(m_ppUserQuery); 
	I3MEM_SAFE_FREE(m_pUserReadIdx); 
	I3MEM_SAFE_FREE(m_pUserWriteIdx); 

	return; 
}

UINT32 CModuleDataBase::OnRunning( void * pUserData)
{
	BOOL bRv = TRUE; 
	bRv;
	INT32	iSendCount; 
	INT32	iReadIdx;

	while(m_bIsRunning)
	{
		if( ::WaitForSingleObject( m_hKillEvent, 1) == WAIT_OBJECT_0 )break;

		//Main Thread
		iSendCount = m_iMainWriteIdx - m_iMainReadIdx; 
		for(INT32 i = 0; i < iSendCount; i++)
		{
			iReadIdx = m_iMainReadIdx % DB_QUERY_BUFFER_COUNT; 
			g_pDataBase->ExecuteDataBaseQuery_D( m_pMainQuery[iReadIdx]._pQuery ); 
			m_iMainReadIdx++; 			
		}        

		////Nc Module not support 

		//Module Zp Thread
		iSendCount = m_iModuleZPWriteIdx - m_iModuleZPReadIdx; 
		for(INT32 i = 0; i < iSendCount; i++)
		{
			iReadIdx = m_iModuleZPReadIdx % DB_QUERY_BUFFER_COUNT; 	
			g_pDataBase->ExecuteDataBaseQuery_D( m_pModuleZPQuery[iReadIdx]._pQuery ); 
			m_iModuleZPReadIdx++; 			
		}

		//User Thread
		for(INT32 i = 0; i < m_iThreadCount; i++)
		{
			DB_QUERY	* pDbBuffer = m_ppUserQuery[i]; 
			iSendCount = m_pUserWriteIdx[i] - m_pUserReadIdx[i];
			
			for(INT32 j = 0; j < iSendCount; j++)
			{
				iReadIdx = m_pUserReadIdx[i] % DB_QUERY_BUFFER_COUNT; 	
				g_pDataBase->ExecuteDataBaseQuery_D( pDbBuffer[iReadIdx]._pQuery ); 
				m_pUserReadIdx[i]++; 				
			}
		}
	}

	return 0; 
}

void CModuleDataBase::WriteUpdateQuery_MAU(INT32 iWorkThreadIdx, char * pQuery)
{
	DB_QUERY	*	pQueryBuffer	= NULL;
	INT32		*	pWritePos		= NULL;	
	INT32		*	pReadPos		= NULL;

	switch( iWorkThreadIdx )
	{
	case MAIN_THREAD_IDX					: 
		pQueryBuffer	= m_pMainQuery; 
		pReadPos		= &m_iMainReadIdx;
		pWritePos		= &m_iMainWriteIdx;
		break; 		
	case MODULE_ZP_THREAD_IDX				: 
		pQueryBuffer	= m_pModuleZPQuery; 
		pReadPos		= &m_iModuleZPReadIdx;
		pWritePos		= &m_iModuleZPWriteIdx;
		break;
		//Not support
	case MODULE_NC_THREAD_IDX				: 
	case MODULE_DATABASE_THREAD_IDX			: 
		break;	
	default									: 
		pQueryBuffer	= m_ppUserQuery[iWorkThreadIdx]; 
		pReadPos		= &m_pUserReadIdx[iWorkThreadIdx];
		pWritePos		= &m_pUserWriteIdx[iWorkThreadIdx];
		break; 
	}	

	if(pQueryBuffer == NULL)
	{
		WRITE_M_LOG(iWorkThreadIdx , NULL, "Not Support DataBase Module"); 
		return; 
	}

	if(*pWritePos == *pReadPos)
	{
		*pWritePos	= 0; 
		*pReadPos	= 0; 
	}

	if( *pReadPos > 2000000000 )
	{
		WRITE_M_LOG(iWorkThreadIdx , NULL, "FULL1 DataBase Module"); 
		return; 
	}
	if( (*pWritePos - *pReadPos) >= DB_QUERY_BUFFER_COUNT )
	{
		WRITE_M_LOG(iWorkThreadIdx , NULL, "FULL2 DataBase Module"); 
		return; 
	}
	INT32 iBufferIdx = *pWritePos % DB_QUERY_BUFFER_COUNT; 

	strcpy(pQueryBuffer[iBufferIdx]._pQuery, pQuery); 

	*pWritePos = *pWritePos + 1;

	return; 
}
