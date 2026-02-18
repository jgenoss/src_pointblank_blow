#include "pch.h"
#include "ModuleThaiPc.h"

I3_CLASS_INSTANCE(CModulePcList, i3Thread); 

CModulePcList * g_pModulePcList = NULL; 

CModulePcList::CModulePcList(void)
{
	m_bIsRunning	= FALSE;

	i3mem::FillZero( m_pIpList,			sizeof(TYPE_IPLIST)*PC_BUFFER_COUNT*PCCAFE_MAX_COUNT );
	i3mem::FillZero( m_iIpListCount,	sizeof(UINT32)*PC_BUFFER_COUNT );
	
	m_i32UsePCCafeIdx	= 0;
	m_i32UseIDPlusIdx	= 0;

	m_iUpdateTime		= 0;
}

CModulePcList::~CModulePcList(void)
{	
	OnDestroy(); 
}

BOOL CModulePcList::OnCreate(void)
{

	m_bPCCafeActive			= g_pConfig->m_bPCCafeActive;
	m_bThaiIDPlusActive		= g_pConfig->m_bThaiIDPlusActive;

	// PCCafe  가 활성화 되어있지 않으면 생성을 하지 않습니다.
	if( (FALSE == m_bPCCafeActive) && (FALSE == m_bThaiIDPlusActive) )		return TRUE;

	// ThaiPCRoom
	if( m_bPCCafeActive )
	{
		m_pIPListDBConfig = g_pConfig->GetDatabaseConfig("PBLink");
		if( !m_pIPListDBConfig )
			return FALSE;

		//For Thai PC
		m_pDBPCCafeInfo._pDB = CADODatabase::NewObject();
		if( FALSE == m_pDBPCCafeInfo._pDB->Connect( m_pIPListDBConfig->GetDataSource(), m_pIPListDBConfig->GetCatalog(), m_pIPListDBConfig->GetAccount(), m_pIPListDBConfig->GetPassword() ) )
		{
			g_pLog->WriteLog( L"[CDataBase::OnCreate] Thai PcCafe Connect Fail"  );
			return FALSE;
		}

		m_i32UsePCCafeIdx = 0;
		if( FALSE == LoadPcCafeList_PcThread( g_i32MainLogIdx, m_pIpList[ m_i32UsePCCafeIdx ],  &m_iIpListCount[ m_i32UsePCCafeIdx ]) )
		{
			g_pLog->WriteLog( L"[ERROR]CModulePcList DataBase Load Fail"  );
			return FALSE;
		}
	}

	// Thai IDPlus
	if( m_bThaiIDPlusActive )
	{
		//For Thai IDPlus
		m_pThaiIDPlus._pDB = CADODatabase::NewObject();
		if( FALSE == m_pThaiIDPlus._pDB->Connect(m_pIPListDBConfig->GetDataSource(), m_pIPListDBConfig->GetCatalog(), m_pIPListDBConfig->GetAccount(), m_pIPListDBConfig->GetPassword()) )
		{
			g_pLog->WriteLog( L"[CDataBase::OnCreate] Thai Thai IDPlus Connect Fail" );
			return FALSE;
		}

		//Read Thai E- ID
		m_i32UseIDPlusIdx = 0; 
		if( FALSE == LoadThaiIDPlusThread( g_i32MainLogIdx, m_pIDPlusList[ m_i32UseIDPlusIdx ],  &m_iIDPlusListCount[ m_i32UseIDPlusIdx ]) )
		{
			g_pLog->WriteLog( L"[ERROR]CModulePcList DataBase Load Fail"  );
			return FALSE;
		}
	}

	//Set Time
	m_iUpdateTime = i3ThreadTimer::GetServerTime();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Create Thread
	if( !i3Thread::Create("CModulePcList", 0, 4096, NULL, PRIORITY_NORMAL) )return FALSE; 
	
	return TRUE; 
}

void CModulePcList::OnDestroy(void)
{
	m_bIsRunning = FALSE;
	i3Thread::WaitForFinish();

	I3_SAFE_RELEASE( m_pDBPCCafeInfo._pDB );
	I3_SAFE_RELEASE( m_pThaiIDPlus._pDB );
	return; 
}

UINT32 CModulePcList::OnRunning( void * pUserData)
{
	m_i32LogIdx = g_pLog->AddLogThread(L"PCCA", 0, m_ThreadID);

	m_bIsRunning = TRUE;

	INT32 i32NextIdx;
	while(m_bIsRunning)
	{
		if( (m_iUpdateTime + (7 * 60)) < i3ThreadTimer::GetServerTime() )	//7분 마다 갱신한다. 
		{
			if( m_bPCCafeActive )
			{
				i32NextIdx = m_i32UsePCCafeIdx+1;
				if( PC_BUFFER_COUNT <= i32NextIdx ) i32NextIdx = 0;

				if( LoadPcCafeList_PcThread( m_i32LogIdx, m_pIpList[ i32NextIdx ],  &m_iIpListCount[ i32NextIdx ]) )
				{
					m_i32UsePCCafeIdx = i32NextIdx;
				}
				else
				{
					I3TRACE("[ERROR]CModulePcList DataBase Load Fail"); 
				}
			}

			if( m_bThaiIDPlusActive )
			{
				i32NextIdx = m_i32UseIDPlusIdx+1;
				if( PC_BUFFER_COUNT <= i32NextIdx ) i32NextIdx = 0;

				if( LoadThaiIDPlusThread( m_i32LogIdx, m_pIDPlusList[ i32NextIdx ],  &m_iIDPlusListCount[ i32NextIdx ] ) )
				{
					m_i32UseIDPlusIdx = i32NextIdx;
				}
				else
				{
					I3TRACE("[ERROR]CModuleThaiIDPlus DataBase Load Fail"); 
				}
			}

			m_iUpdateTime = i3ThreadTimer::GetServerTime();
		}
		::WaitForSingleObject( m_Handle, 1 );					//10초 동안 대기 합니다. 
	}

	return 0; 
}

BOOL CModulePcList::LoadPcCafeList_PcThread( INT32 i32ThreadIdx, TYPE_IPLIST * pList, UINT32 * Count)
{
	DB_QUERY_INFO * pDataBaseInfo = &m_pDBPCCafeInfo;

	UINT32 ui32NewCount = 0;
	S2DB::MakeQueryW( pDataBaseInfo->_pQuery, L"usp_80100_iplist_loadpccafelist",
		DB_DATA_TYPE_END); 
	if( !pDataBaseInfo->_pDB->ExecuteSP( pDataBaseInfo->_pQuery ) )	
	{
		ErrorDBPcList( i32ThreadIdx,  MODULE_THAI_PCCAFE_DB_THREAD_IDX );
		return NET_ERROR_DB_QUERY; 
	}

	//Set PCCafe Count 
	ui32NewCount = (UINT32)pDataBaseInfo->_pDB->GetRecordCount(); 
	if( ui32NewCount > PCCAFE_MAX_COUNT)
	{
		g_pLog->WriteLog( L"[OnCreate-CDataBase::LoadPcCafeList_PcThread] NewCount > PCCAFE_MAX_COUNT"  );
		
		return FALSE; 
	}

	*Count = ui32NewCount;

	//Load Ip List 
	char	strPCIP[16]; 
	strPCIP[15] = 0;
	UINT32	ui32IP;
	UINT8	ui8Rank;
	UINT8	ui8CIDR;
	for( UINT32 i = 0 ; i < ui32NewCount ; i++ )
	{
		pDataBaseInfo->_pDB->GetTextData(0, strPCIP, 16);
		ui32IP	= ::htonl( ::inet_addr(strPCIP) );
		ui8Rank	= (UINT8)pDataBaseInfo->_pDB->GetUINT16Data( 1 );
		ui8CIDR = pDataBaseInfo->_pDB->GetUINT8Data( 2 );

		// 순차 정렬을 합니다.
		INT32 i32InsertIdx = 0;
		for( UINT32 j = 0 ; j <= i ; j++ )
		{
			i32InsertIdx = j;

			if( pList[ j ].m_ui32IP == ui32IP )
			{
				i32InsertIdx = -1;
				break;
			}
			else if( pList[ j ].m_ui32IP == 0 || pList[ j ].m_ui32IP > ui32IP )
			{
				break;
			}
		}
		// -1 이면 이미 같은 IP 가 존재할 때입니다. 더이상 추가 할 필요가 없습니다.
		if( -1 != i32InsertIdx )
		{
			INT32 i32MoveCount = i - i32InsertIdx;
			if( 0 < i32MoveCount )
			{
				memmove( &pList[ i32InsertIdx+1 ], &pList[ i32InsertIdx ], sizeof( TYPE_IPLIST )*i32MoveCount );
			}
			
			// 값을 세팅해 줍니다.
			pList[ i32InsertIdx ].m_ui32IP		= ui32IP;
			pList[ i32InsertIdx ].m_ui8Rank		= ui8Rank;
			pList[ i32InsertIdx ].m_ui32CIDR	= 0;
			for( INT32 j = 0 ; j < ui8CIDR ; j++ )
			{
				// CIDR 을 설정합니다. 추후 이 값으로 & 연산해서 속해있는 IP 인지 확인 할 예정입니다.
				pList[ i32InsertIdx ].m_ui32CIDR |= 1 << (31-j);
			}
		}

		if( FALSE == pDataBaseInfo->_pDB->MoveNext() )
		{
			return FALSE;
		}
	}
		
	return TRUE; 
}

BOOL CModulePcList::LoadThaiIDPlusThread(INT32 i32ThreadIdx, TYPE_IDPLUS_LIST * pList, UINT32 * pui32Count)
{
	DB_QUERY_INFO * pDataBaseInfo = &m_pThaiIDPlus;

	UINT32 ui32NewCount = 0;
	S2DB::MakeQueryW( pDataBaseInfo->_pQuery, L"usp_80200_iplist_loadthaiidplus",
		DB_DATA_TYPE_END);
	if( !pDataBaseInfo->_pDB->ExecuteSP(pDataBaseInfo->_pQuery) )	
	{
		ErrorDBPcList( i32ThreadIdx,  MODULE_THAI_IDPLUS_DB_THREAD_IDX );
		return NET_ERROR_DB_QUERY; 
	}

	//Set E-SportID Count 
	ui32NewCount = (UINT32)pDataBaseInfo->_pDB->GetRecordCount(); 
	if( ui32NewCount > THAI_IDPLUS_MAX_COUNT)
	{
		g_pLog->WriteLog( L"[OnCreate-CDataBase::LoadThaiIDPlusThread] NewCount > THAI_IDPLUS_MAX_COUNT" );
		return FALSE; 
	}

	*pui32Count = ui32NewCount;

	//Load Thai IDPlus List 	
	for(UINT32 i = 0; i < ui32NewCount; i++)
	{
		pList[i]._i64uid	= pDataBaseInfo->_pDB->GetINT64Data( 0 );
		pList[i]._Level		= (UINT8)pDataBaseInfo->_pDB->GetUINT32Data( 1 ); 

		if(!pDataBaseInfo->_pDB->MoveNext())
		{
			return FALSE;
		}
	}

	return TRUE; 
}

// 순차검색
BOOL CModulePcList::IsPcCafe(UINT32 iIP, UINT8& ui8Rank)
{
	if( FALSE == m_bPCCafeActive )	return FALSE;

	UINT32			ui32Count	= m_iIpListCount[ m_i32UsePCCafeIdx ];
	TYPE_IPLIST *	pList		= m_pIpList[ m_i32UsePCCafeIdx ];
	UINT32			ui32IP		= iIP;

	if( 0 == ui32Count )	return FALSE;

	INT32 i32Start	= 0;
	INT32 i32End	= ui32Count;
	INT32 i32Center = 0;
	while( i32Start < i32End )
	{
		i32Center = (i32Start + i32End) / 2;

		if( pList[ i32Center ].m_ui32IP		> ui32IP )	
		{
			i32End = i32Center;
			i32Center = i32End;
		}
		else if( pList[ i32Center ].m_ui32IP	< ui32IP )
		{
			i32Start = i32Center;
			i32Center = i32Start;
		}
		else 
		{
			break;
		}
		if( 1 == (i32End - i32Start) )
		{
			i32Center = i32Start;
			break;
		}
	} 

	if( (ui32IP & pList[i32Center].m_ui32CIDR) == pList[i32Center].m_ui32IP )
	{
		ui8Rank = pList[i32Center].m_ui8Rank;
		return TRUE; 
	}

	return FALSE; 
}

// 이분검색
INT32 CModulePcList::IsThaiIDPlus(T_UID i64UID)
{
	if( FALSE == m_bThaiIDPlusActive )	return PC_NOT_CAFE;

	UINT32				ui32Count	= m_iIDPlusListCount[ m_i32UseIDPlusIdx ];
	TYPE_IDPLUS_LIST *	pList		= m_pIDPlusList[ m_i32UseIDPlusIdx ];

	INT32 left = 0;
	INT32 right = ui32Count;
	INT32 mid = 0;

	while(right >= left)
	{
		mid = (left+right)/2;
		if(pList[mid]._i64uid == i64UID)
		{
			if(1011120000 > pList[mid]._ExpireDate)
			{
				return pList[mid]._Level;
			}
		}
		if(i64UID > pList[mid]._i64uid) left = mid+1;
		else right = mid - 1;
	}

	return PC_NOT_CAFE;
};

void CModulePcList::ErrorDBPcList( INT32 i32WorkThreadIdx , INT32 i32DBType )
{
	CADODatabase	*	pDataBase = NULL; 
	wchar_t			*	pStrQuery = NULL;
		
	switch( i32DBType )
	{
	case MODULE_THAI_PCCAFE_DB_THREAD_IDX	:
		pDataBase		= m_pDBPCCafeInfo._pDB; 
		pStrQuery		= m_pDBPCCafeInfo._pQuery; 
		break;
	case MODULE_THAI_IDPLUS_DB_THREAD_IDX:
		pDataBase		= m_pThaiIDPlus._pDB; 
		pStrQuery		= m_pThaiIDPlus._pQuery; 
		break;
	default :
		return;
	}

	g_pLog->WriteLog( L"[ErrorDBPcList] %s, Error Code : %08x", pStrQuery, pDataBase->GethrLastError() );
	
	HRESULT EventType = pDataBase->ReConnect(
												m_pIPListDBConfig->GetDataSource(),
												m_pIPListDBConfig->GetCatalog(),
												m_pIPListDBConfig->GetAccount(),
												m_pIPListDBConfig->GetPassword()
											);
	
	switch( EventType )
	{
	case EVENT_ERROR_EVENT_LOG_IN_NOT_CONNECT_DB:		// 리커넥을 시도하였으나 실패 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_DB, L"[ErrorDBPcList] RECONNECT FAILE" );
		break;

	case EVENT_ERROR_SUCCESS:							// 리커넥을 시도하여 성공 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_DB, L"[ErrorDBPcList] RECONNECT SUCCEED" );
		break;
	case EVENT_ERROR_FAIL:								// 리커넥을 시도하지 않았을 경우
		
		break;
	}
}