#include "pch.h"
#include "ModuleDBShop.h"
#include "Shop.h"
#include "ModuleLog.h"

static UINT32 _DefThreadProcWebGift( void* pPoint )
{
	((CModuleDBShop*)pPoint)->OnWorkingWebGift();
	return 0; 
}


bool CModuleDBShop::OnCreateWebGift(void)
{
	// Create DB
	m_pADOdb_WEBGIFT = CADODatabase::NewObject(); 
	if( !m_pADOdb_WEBGIFT )
		return false;

	m_pGiftDBConfig = g_pConfig->GetDatabaseConfig("PBLink");
	if( !m_pGiftDBConfig )
		return false;

	if( !m_pADOdb_WEBGIFT->Connect(m_pGiftDBConfig->GetDataSource(), m_pGiftDBConfig->GetCatalog(), m_pGiftDBConfig->GetAccount(), m_pGiftDBConfig->GetPassword()) )
		return false; 
		
	
	//Make Web Thread 
	DWORD dwThreadID;
	m_hThreadWebGift = CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProcWebGift, this, 0, &dwThreadID);
	if( NULL == m_hThreadWebGift )												return false;
	if( FALSE == SetThreadPriority( m_hThreadWebGift, THREAD_PRIORITY_NORMAL) )	return false;

	g_pLog->AddLogThread( L"SHGF", 0, dwThreadID );

	return true;
}

void CModuleDBShop::OnWorkingWebGift(void)
{
	wchar_t wstrSQL[SERVER_STRING_COUNT]; 
	
	CADODatabase * pADO_DB = m_pADOdb_WEBGIFT;

	INT32		i32Count;
	TWebGiftDxIdx	i32Idx;
	T_UID		i64UID;
	UINT32		ui32GoodsID;
	UINT8		ui8InsertType;	// WEBGIFT_INSERT_TYPE
	INT32		i32BuyPrice;
	
	TTCHAR		strMessage[ MAX_GIFT_MESSAGE_SIZE ];
	
	DB_WEB_GIFT_IN	* pDBWebGift; 
	DB_WEB_GIFT_OUT	* pDBWebGiftOut; 
	SHOP_GOODS_LIST*	pGoods ;
	INT32	iTableIdx; 	
	UINT8	ui8IsInsert;	// TUserGift 테이블에 INSERT 여부.
	BOOL	bSPResult;		// SP 실행결과
	BOOL	bWorking;

	// 오래된 데이터 삭제(지급 완료된 것만)
	DATE32 dt32DeleteCondition = i3ThreadTimer::LocalTime();
	dt32DeleteCondition.AddTime( I3DATE_ADD_TYPE_DAY, (0 - g_pConfig->m_ui16DeliverStoragePeriod) );

	wchar_t wstrDeleteCondition[ SMALLDATE_STRING_SIZE ];
	dt32DeleteCondition.GetSmallDate(wstrDeleteCondition, SMALLDATE_STRING_SIZE);
	
	S2DB::MakeQueryW( wstrSQL, L"usp_GiftDeliver_del",
		DB_DATA_TYPE_SMALLDATETIME,			wstrDeleteCondition,
		DB_DATA_TYPE_END);
	UINT32 ui32OldTime = i3ThreadTimer::GetServerTime();
	BOOL bResult = pADO_DB->ExecuteSP( wstrSQL, 600 );
	UINT32 ui32NewTime = i3ThreadTimer::GetServerTime();

	if( FALSE == bResult )
	{
		ErrorDBShop( MODULE_WEBGIFT_THREAD_IDX,  wstrSQL); 
	}
	else
	{
		if( 1 != pADO_DB->GetRecordCount() )
		{
			_DataBaseResultFailLog( MODULE_WEBGIFT_THREAD_IDX, wstrSQL );
		}
		INT32 i32DeleteCount = pADO_DB->GetINT32Data(0);
		if( 0 > i32DeleteCount ) // DELETE 실패시 @Result = -1
		{
			_DataBaseResultFailLog( MODULE_WEBGIFT_THREAD_IDX, wstrSQL );
		}
		else
		{
			g_pLog->WriteLog( L"Delete GiftDeliver before [%s]! Delete Count : %d", wstrDeleteCondition, i32DeleteCount );
		}	
	}
	g_pLog->WriteLog( L"Delete GiftDeliver Time : %d", ui32NewTime-ui32OldTime );
	m_bIsRunningWebGift = TRUE;
	while( m_bIsRunningWebGift )
	{
		bWorking = FALSE;

		// 상점 정보 로드 대기.  
		if (GetVersion() <= 0 )
		{
			::WaitForSingleObject(m_hThreadWebGift, 1000);
			continue;
		}

		// 1. CModuleDBShop쓰레드에서 지급한 결과를 DB에 UPDATE 합니다.
		for( INT32 i = 0 ; i < DATABASE_THREAD_COUNT ; i++ ) 
		{
			i32Count = m_ppOutWebGiftBuffer[i]->GetBufferCount();
			for( INT32 k = 0 ; k < i32Count ; k++ )
			{
				pDBWebGiftOut = (DB_WEB_GIFT_OUT*)m_ppOutWebGiftBuffer[i]->Pop();
				
				if( EVENT_ERROR_SUCCESS == pDBWebGiftOut->_i32Rv )		ui8IsInsert = WEBGIFT_INSERT_STATE_COMPLETE;
				else								ui8IsInsert = WEBGIFT_INSERT_STATE_FAIL;
				
				S2DB::MakeQueryW( wstrSQL, L"usp_GiftDeliver_upd",
					DB_DATA_TYPE_INT,			pDBWebGiftOut->_i32BufferIdx,
					DB_DATA_TYPE_TINYINT,		ui8IsInsert,
					DB_DATA_TYPE_END);

				bSPResult	= TRUE;
				if( FALSE == pADO_DB->ExecuteSP( wstrSQL ) )
				{
					bSPResult = FALSE;
					ErrorDBShop( MODULE_WEBGIFT_THREAD_IDX,  wstrSQL); 
				}
				else
				{
					if( 1 != pADO_DB->GetRecordCount() )
					{
						bSPResult = FALSE;
						_DataBaseResultFailLog( MODULE_WEBGIFT_THREAD_IDX, wstrSQL );
					}
					if( 1 != pADO_DB->GetINT32Data(0) )
					{
						bSPResult = FALSE;
						_DataBaseResultFailLog( MODULE_WEBGIFT_THREAD_IDX, wstrSQL );
					}
				}

				if( FALSE == bSPResult )
				{	 // Rv가 성공이면, 중복지급 될 수 있습니다.
					g_pLog->WriteLog( L"WEBGIFT UPDATE ERROR : DBIdx = %d / UID : %I64d / GoodsID : %d / Rv : %x", 
						pDBWebGiftOut->_i32BufferIdx, pDBWebGiftOut->_i64UID, pDBWebGiftOut->m_TGoodsID, pDBWebGiftOut->_i32Rv );
				}

				m_ppOutWebGiftBuffer[i]->PopIdx();
			}
		}
		 
		UINT64 ui64Result = 0;	// 비트플래그(0:실패, 1:성공). WEBGIFT_MAX 가 64개 이상이 되면 수정되야 합니다.
		TWebGiftDxIdx i32ResultIdx[ WEBGIFT_MAX ];

		S2DB::MakeQueryW( wstrSQL, L"usp_GiftDeliver_sel_All",
			DB_DATA_TYPE_TINYINT,		WEBGIFT_MAX,
			DB_DATA_TYPE_END);

		if( !pADO_DB->ExecuteSP( wstrSQL ) )
		{
			ErrorDBShop( MODULE_WEBGIFT_THREAD_IDX, wstrSQL );
			::WaitForSingleObject( m_hThreadWebGift, 1000 );
			continue;
		}

		// 2. 선물함 or 인벤토리 지급을 위해 Push 합니다.
		i32Count = pADO_DB->GetRecordCount(); 
		for(INT32 i = 0; i < i32Count; i++)
		{
			i32Idx			= pADO_DB->GetINT32Data(0);
			i64UID			= pADO_DB->GetINT64Data(1);
			ui8InsertType	= pADO_DB->GetUINT8Data(2);
			ui32GoodsID		= pADO_DB->GetUINT32Data(3); 
			pADO_DB->GetTextData( 4, strMessage, MAX_GIFT_MESSAGE_SIZE);
			i32BuyPrice		= MAX(0, pADO_DB->GetINT32Data(5));


			i32ResultIdx[i] = i32Idx;
			

			pGoods = g_pShop->GetGoodsByID( ui32GoodsID );		// GoodsID 로 Goods 를 얻어온다.
			if(  NULL != pGoods )
			{
				iTableIdx = (INT32)(i64UID % DATABASE_THREAD_COUNT);
				pDBWebGift = (DB_WEB_GIFT_IN *)m_ppInWebGiftBuffer[iTableIdx]->PushPointer(); 
				if(pDBWebGift != NULL )
				{  
					bWorking = TRUE;
					pDBWebGift->_i32BufferIdx		= i32Idx; 
					pDBWebGift->_i64UID				= i64UID; 
					pDBWebGift->_ui32GoodsID		= ui32GoodsID;
					pDBWebGift->_eInsertType		= (WEBGIFT_INSERT_TYPE)ui8InsertType;
					i3String::Copy( pDBWebGift->_strMessage, strMessage, MAX_GIFT_MESSAGE_SIZE );

					m_ppInWebGiftBuffer[iTableIdx]->PushPointerIdx();

					ui64Result |= (UINT64)0x01 << i;
				}
				else   
				{
					g_pLog->WriteLog( L"WEBGIFT BUFFER FULL / DB IDX = %d / UID = %I64d / GOODS ID = %d ", i32Idx, i64UID, ui32GoodsID );  
				}
			}
			else
			{
				//g_pLog->WriteLog( m_i32LogGiftIdx, "WEBGIFT UNKNOWN GOODS / DB IDX = %d / UID = %I64d / GOODS ID = %d ", i32Idx, i64UID, ui32GoodsID );  
			}


			// ZLog 기록. 지급 결과와 상관없이 기록합니다.
			if( WEBGIFT_INSERT_TYPE_WEBSHOP == ui8InsertType && i32BuyPrice > 0)
			{
				if( g_pModuleLogD != NULL ) 
				{
					LOG_BUY	stLog;
					i3mem::FillZero( &stLog, sizeof( LOG_BUY ) );
					stLog.m_TUID			= i64UID; 
					stLog.m_i32BuyPrice		= i32BuyPrice; 
					stLog.m_TGoodsID		= ui32GoodsID; 
					g_pModuleLogD->SendLogMessage(LOG_MESSAGE_BUY_WEBSHOP, LOCALE_CODE_NONE, &stLog );
				}
			}

			pADO_DB->MoveNext(); 
		}				

		// 3. 2의 결과를 DB에 UPDATE합니다.
		for( INT32 i = 0 ; i < i32Count ; i++ )
		{
			UINT8 ui8Result = WEBGIFT_INSERT_STATE_FAIL;
			if( ui64Result & (UINT64)0x01 << i )
			{	// 이부분이 없으면 중복 지급 될 수 있습니다. 중요
				ui8Result = WEBGIFT_INSERT_STATE_ING;
			}

			S2DB::MakeQueryW( wstrSQL, L"usp_GiftDeliver_upd",
					DB_DATA_TYPE_INT,			i32ResultIdx[ i ],
					DB_DATA_TYPE_TINYINT,		ui8Result,
					DB_DATA_TYPE_END);
			if( !pADO_DB->ExecuteSP( wstrSQL ) )
			{
				ErrorDBShop( MODULE_WEBGIFT_THREAD_IDX, wstrSQL );
				continue;
			}
			if(1 != pADO_DB->GetRecordCount())
			{
				_DataBaseResultFailLog( MODULE_WEBGIFT_THREAD_IDX, wstrSQL );
			}
			if(1 != pADO_DB->GetINT32Data(0))
			{
				_DataBaseResultFailLog( MODULE_WEBGIFT_THREAD_IDX, wstrSQL );
			}
		}		
 
		if( FALSE == bWorking ) 
		{	// 한일이 없다면 1초간 휴식합니다.
			::WaitForSingleObject( m_hThreadWebGift, 1000 );
		}
		else 
		{
			::WaitForSingleObject( m_hThreadWebGift, 1 );
		}
	}

	return; 
}