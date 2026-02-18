#include "pch.h"
#include "Shop.h"
#include "ModuleDBShop.h"
#include "ModuleServiceLogFile.h"
#include "TaskProcessor.h"
#include "ModuleLog.h"

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

I3_CLASS_INSTANCE(CModuleDBShop, i3ElementBase);

CModuleDBShop * g_pModuleDBShop = NULL; 

#define DATABASE_EXECUTE_SP_ITEM		if( !pDataBase->ExecuteSP( wstrSQL ) )\
{\
	ErrorDBShop( i32ThreadIdx, wstrSQL);\
	return EVENT_ERROR_DATABASE_EXECUTE_SP;\
}\

#define DATABASE_EXECUTE_SP_GOODS		if( !pDataBase->ExecuteSP( wstrSQL ) )\
{\
	ErrorDBShop( i32ThreadIdx, wstrSQL);\
	pGoods->m_i32Rv = EVENT_ERROR_DATABASE_EXECUTE_SP;\
	goto NEXT_GOODS;\
}\

#define DATABASE_EXECUTE_SP		if( !pDataBase->ExecuteSP( wstrSQL ) )\
{\
	ErrorDBShop( i32ThreadIdx, wstrSQL);\
	pOut->_i32Rv = EVENT_ERROR_DATABASE_EXECUTE_SP;\
	return;\
}\

#define START_ELEPSETIME	{ QueryPerformanceFrequency(&stQP_Freq_Begin);\
							QueryPerformanceCounter(&stQP_Count_Begin); } 

#define END_ELEPSETIME		{ QueryPerformanceCounter(&stQP_Count_End); \
							stQP_Result.QuadPart = (LONGLONG)stQP_Count_End.QuadPart - stQP_Count_Begin.QuadPart; \
							r64ElepseTime = (REAL64)stQP_Result.QuadPart / (REAL64)stQP_Freq_Begin.QuadPart; \
							g_pStatistics->AddStat(STAT_TYPE_DB_SHOP, i32ThreadIdx, r64ElepseTime); }
								
					
static char	s_InvenTable[DATABASE_THREAD_COUNT][SERVER_STRING_COUNT] = { "TUserWare00", "TUserWare01", "TUserWare02", "TUserWare03", "TUserWare04","TUserWare05", "TUserWare06", "TUserWare07", "TUserWare08",	"TUserWare09"};
static char	s_GiftTable[DATABASE_THREAD_COUNT][SERVER_STRING_COUNT] = { "TUserGift00", "TUserGift01", "TUserGift02", "TUserGift03", "TUserGift04","TUserGift05", "TUserGift06", "TUserGift07", "TUserGift08",	"TUserGift09"};

//Thread Proc

static UINT32 _DefThreadProc( INT32 * i32TheadIdx )
{
	g_pModuleDBShop->OnWorking( *i32TheadIdx );	
	return 0;
}

static UINT32 _DefThreadProcMain( void )
{
	g_pModuleDBShop->OnMainWorking( );	
	return 0;
}


CModuleDBShop::CModuleDBShop()
{
	m_HDBThread				= NULL;
	m_pHThread				= NULL;
	m_pi32ThreadIdx			= NULL;

	m_bIsRunningDB			= FALSE;
	m_bIsRunningWebGift		= FALSE;
	m_pbIsRunning			= NULL;

	m_pADOdb_DBMAIN_Shop	= NULL; 
	m_ppADOdb_DATABASE		= NULL; 
	m_pADOdb_WEBGIFT		= NULL;

	m_ppInShopItemBuffer	= NULL; 
	m_ppOutShopItemBuffer	= NULL;
	m_ui32OutShopItemThreadIdx= 0;

	m_ppInShopBuyBuffer		= NULL; 
	m_ppOutShopBuyBuffer	= NULL;
	m_ui32OutShopBuyThreadIdx= 0;

	m_ppInShopBuffer		= NULL; 
	m_ppOutShopBuffer		= NULL;
	m_ui32OutShopThreadIdx	= 0;

	m_ppInInventoryBuffer	= NULL;	
	m_ppOutInventorytBuffer	= NULL;
	m_ui32OutInventoryThreadIdx	= 0;

	m_ppInGiftBuffer		= NULL;
	m_ppOutGiftBuffer		= NULL;
	m_ui32OutGiftThreadIdx	= 0;

	m_ppInGiftListBuffer	= NULL;
	m_ppOutGiftListBuffer	= NULL;
	m_ui32OutGiftListThreadIdx= 0;

	m_ppInWebGiftBuffer		= NULL;
	m_ppOutWebGiftBuffer	= NULL;
	m_ui32OutWebGiftThreadIdx = 0;

	m_ppInRepairBuffer		= NULL;
	m_ppOutRepairBuffer		= NULL;
	m_ui32OutRepairThreadIdx	= 0;

	m_ppInCouponBuffer		= NULL;
	m_ppOutCouponBuffer		= NULL;
	m_ui32OutCouponThreadIdx	= 0;

	m_ppInGachaBuffer	= NULL; 
	m_ppOutGachaBuffer	= NULL;
	m_ui32OutGachaThreadIdx= 0;

	m_bUpdateShopInfo		= FALSE; 
}

CModuleDBShop::~CModuleDBShop()
{
	OnDestroy();  
}

INT32 CModuleDBShop::OnCreate(void)
{	
	DWORD	dwThreadID;

	//For Thead 
	m_pi32ThreadIdx		= (INT32 *)i3MemAlloc( sizeof(INT32)	* DATABASE_THREAD_COUNT ); 
	m_pHThread			= (HANDLE*)i3MemAlloc( sizeof(HANDLE)	* DATABASE_THREAD_COUNT );
	m_pbIsRunning		= (BOOL*)i3MemAlloc( sizeof(BOOL)		* DATABASE_THREAD_COUNT );

	//ADO Database
	m_ppADOdb_DATABASE		= (CADODatabase **)i3MemAlloc( sizeof(CADODatabase *) * DATABASE_THREAD_COUNT); 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Buffer

	//Shop Buy Buffer
	m_ppInShopItemBuffer	= (S2RingBuffer **)i3MemAlloc( sizeof(S2RingBuffer *) * DATABASE_THREAD_COUNT); 
	m_ppOutShopItemBuffer	= (S2RingBuffer **)i3MemAlloc( sizeof(S2RingBuffer *) * DATABASE_THREAD_COUNT); 	

	//Shop Buy Buffer
	m_ppInShopBuyBuffer		= (S2RingBuffer **)i3MemAlloc( sizeof(S2RingBuffer *) * DATABASE_THREAD_COUNT); 
	m_ppOutShopBuyBuffer	= (S2RingBuffer **)i3MemAlloc( sizeof(S2RingBuffer *) * DATABASE_THREAD_COUNT); 	

	//Shop Buffer
	m_ppInShopBuffer		= (S2RingBuffer **)i3MemAlloc( sizeof(S2RingBuffer *) * DATABASE_THREAD_COUNT); 
	m_ppOutShopBuffer		= (S2RingBuffer **)i3MemAlloc( sizeof(S2RingBuffer *) * DATABASE_THREAD_COUNT); 	

	//Inventory Buffer 
	m_ppInInventoryBuffer	= (S2RingBuffer **)i3MemAlloc( sizeof(S2RingBuffer *) * DATABASE_THREAD_COUNT); 
	m_ppOutInventorytBuffer	= (S2RingBuffer **)i3MemAlloc( sizeof(S2RingBuffer *) * DATABASE_THREAD_COUNT); 

	//Gift Buffer
	m_ppInGiftBuffer		= (S2RingBuffer **)i3MemAlloc( sizeof(S2RingBuffer *) * DATABASE_THREAD_COUNT); 
	m_ppOutGiftBuffer		= (S2RingBuffer **)i3MemAlloc( sizeof(S2RingBuffer *) * DATABASE_THREAD_COUNT);

	//Gift List Buffer
	m_ppInGiftListBuffer	= (S2RingBuffer **)i3MemAlloc( sizeof(S2RingBuffer *) * DATABASE_THREAD_COUNT); 
	m_ppOutGiftListBuffer	= (S2RingBuffer **)i3MemAlloc( sizeof(S2RingBuffer *) * DATABASE_THREAD_COUNT);

	//Web Gift Buffer 
	m_ppInWebGiftBuffer		= (S2RingBuffer **)i3MemAlloc( sizeof(S2RingBuffer *) * DATABASE_THREAD_COUNT); 
	m_ppOutWebGiftBuffer	= (S2RingBuffer **)i3MemAlloc( sizeof(S2RingBuffer *) * DATABASE_THREAD_COUNT); 

	// Repair
	m_ppInRepairBuffer		= (S2RingBuffer **)i3MemAlloc( sizeof(S2RingBuffer *) * DATABASE_THREAD_COUNT); 
	m_ppOutRepairBuffer		= (S2RingBuffer **)i3MemAlloc( sizeof(S2RingBuffer *) * DATABASE_THREAD_COUNT); 	

	// GiftCoupon
	m_ppInCouponBuffer		= (S2RingBuffer **)i3MemAlloc( sizeof(S2RingBuffer *) * DATABASE_THREAD_COUNT);
	m_ppOutCouponBuffer		= (S2RingBuffer **)i3MemAlloc( sizeof(S2RingBuffer *) * DATABASE_THREAD_COUNT);

	// Gacha
	m_ppInGachaBuffer		= (S2RingBuffer **)i3MemAlloc( sizeof(S2RingBuffer *) * DATABASE_THREAD_COUNT); 
	m_ppOutGachaBuffer		= (S2RingBuffer **)i3MemAlloc( sizeof(S2RingBuffer *) * DATABASE_THREAD_COUNT); 	


	m_ui32OutShopBuyThreadIdx	= 0;
	m_ui32OutShopThreadIdx		= 0;
	m_ui32OutInventoryThreadIdx	= 0;
	m_ui32OutGiftThreadIdx		= 0;
	m_ui32OutWebGiftThreadIdx	= 0;
	m_ui32OutRepairThreadIdx	= 0;
	m_ui32OutCouponThreadIdx	= 0;
	m_ui32OutGachaThreadIdx	= 0;

	for(INT32 i = 0; i < DATABASE_THREAD_COUNT; i++)
	{
		//Create Database
		m_ppADOdb_DATABASE[i] = CADODatabase::NewObject();
		if( !m_ppADOdb_DATABASE[i] )
			return -1;

		char szDBName[DB_STRING_LENGTH] = {0, };
		i3String::Format(szDBName, DB_STRING_LENGTH, "PBUser%02d", i);
		m_pSIAUserDBConfig[i] = g_pConfig->GetDatabaseConfig(szDBName);
		if( !m_pSIAUserDBConfig[i] )
			return -2;

		if( !m_ppADOdb_DATABASE[i]->Connect(m_pSIAUserDBConfig[i]->GetDataSource(), m_pSIAUserDBConfig[i]->GetCatalog(), m_pSIAUserDBConfig[i]->GetAccount(), m_pSIAUserDBConfig[i]->GetPassword()) )
			return -3;

		//Create Shop Item Buffer 
		m_ppInShopItemBuffer[i]	= new S2RingBuffer;
		if( NULL == m_ppInShopItemBuffer[i] ) return -4;
		if( false == m_ppInShopItemBuffer[i]->Create( sizeof( DB_SHOP_ITEM ), DATABASE_BUFFER_COUNT) ) return -5;
		m_ppOutShopItemBuffer[i] = new S2RingBuffer;
		if( NULL == m_ppOutShopItemBuffer[i] ) return -6;
		if( false == m_ppOutShopItemBuffer[i]->Create( sizeof( DB_SHOP_ITEM ), DATABASE_BUFFER_COUNT) ) return -7;
 
		//Create Shop Buy Buffer 
		m_ppInShopBuyBuffer[i]	= new S2RingBuffer;
		if( NULL == m_ppInShopBuyBuffer[i] ) return -8;
		if( false == m_ppInShopBuyBuffer[i]->Create( sizeof( DB_SHOP_GOODS ), DATABASE_BUFFER_COUNT) ) return -9;
		m_ppOutShopBuyBuffer[i] = new S2RingBuffer;
		if( NULL == m_ppOutShopBuyBuffer[i] ) return -10;
		if( false == m_ppOutShopBuyBuffer[i]->Create( sizeof( DB_SHOP_GOODS ), DATABASE_BUFFER_COUNT) ) return -11;

		//Create Shop Buffer 
		m_ppInShopBuffer[i]	= new S2RingBuffer;
		if( NULL == m_ppInShopBuffer[i] ) return -12;
		if( false == m_ppInShopBuffer[i]->Create( sizeof( DB_SHOP_IN ), DATABASE_BUFFER_COUNT) ) return -13;
		m_ppOutShopBuffer[i] = new S2RingBuffer;
		if( NULL == m_ppOutShopBuffer[i] ) return -14;
		if( false == m_ppOutShopBuffer[i]->Create( sizeof( DB_SHOP_OUT ), DATABASE_BUFFER_COUNT) ) return -15;

		//Create Inventory Buffer 
		m_ppInInventoryBuffer[i] = new S2RingBuffer; 
		if( NULL == m_ppInInventoryBuffer[i] ) return -16;
		if( false == m_ppInInventoryBuffer[i]->Create( sizeof(DB_INVENTORY_IN) , DATABASE_BUFFER_COUNT) ) return -17;	
		m_ppOutInventorytBuffer[i] = new S2RingBuffer; 
		if( NULL == m_ppOutInventorytBuffer[i] ) return -18;
		if( false == m_ppOutInventorytBuffer[i]->Create( sizeof(DB_INVENTORY_OUT) , DATABASE_BUFFER_COUNT) ) return -19;

		//Create Gift Buffer
		m_ppInGiftBuffer[i]	= new S2RingBuffer; 
		if( NULL == m_ppInGiftBuffer[i] ) return -20;
		if( false == m_ppInGiftBuffer[i]->Create( sizeof(SEND_GIFT), DATABASE_BUFFER_COUNT) ) return -21;
		m_ppOutGiftBuffer[i] = new S2RingBuffer; 
		if( NULL == m_ppOutGiftBuffer[i] ) return -22;
		if( false == m_ppOutGiftBuffer[i]->Create( sizeof(SEND_GIFT), DATABASE_BUFFER_COUNT) ) return -23;

		//Create Gift List Buffer
		m_ppInGiftListBuffer[i]	= new S2RingBuffer; 
		if( NULL == m_ppInGiftListBuffer[i] ) return -24;
		if( false == m_ppInGiftListBuffer[i]->Create( sizeof(DB_GIFT_IN) , DATABASE_BUFFER_COUNT) ) return -25;
		m_ppOutGiftListBuffer[i] = new S2RingBuffer; 
		if( NULL == m_ppOutGiftListBuffer[i] ) return -26;
		if( false == m_ppOutGiftListBuffer[i]->Create( sizeof(DB_GIFT_OUT) , DATABASE_BUFFER_COUNT) ) return -27;

		//Create Web Gift Buffer 
		m_ppInWebGiftBuffer[i]	= new S2RingBuffer; 
		if( NULL == m_ppInWebGiftBuffer[i] ) return -28;
		if( false == m_ppInWebGiftBuffer[i]->Create( sizeof(DB_WEB_GIFT_IN), DATABASE_BUFFER_WEB_GIFT ) ) return -29;

		m_ppOutWebGiftBuffer[i] = new S2RingBuffer; 
		if( NULL == m_ppOutWebGiftBuffer[i] ) return -30;
		if( false == m_ppOutWebGiftBuffer[i]->Create( sizeof(DB_WEB_GIFT_OUT), DATABASE_BUFFER_WEB_GIFT ) ) return -31;

		//Create Repair Buffer 
		m_ppInRepairBuffer[i]	= new S2RingBuffer;
		if( NULL == m_ppInRepairBuffer[i] ) return -32;
		if( false == m_ppInRepairBuffer[i]->Create( sizeof( DB_REPAIR ), DATABASE_BUFFER_COUNT) ) return -33;
		m_ppOutRepairBuffer[i] = new S2RingBuffer;
		if( NULL == m_ppOutRepairBuffer[i] ) return -34;
		if( false == m_ppOutRepairBuffer[i]->Create( sizeof( DB_REPAIR ), DATABASE_BUFFER_COUNT) ) return -35;

		//Create Coupon Buffer 
		m_ppInCouponBuffer[i]	= new S2RingBuffer;
		if( NULL == m_ppInCouponBuffer[i] ) return -36;
		if( false == m_ppInCouponBuffer[i]->Create( sizeof( DB_COUPON ), DATABASE_BUFFER_COUNT) ) return -37;
		m_ppOutCouponBuffer[i] = new S2RingBuffer;
		if( NULL == m_ppOutCouponBuffer[i] ) return -38;
		if( false == m_ppOutCouponBuffer[i]->Create( sizeof( DB_COUPON ), DATABASE_BUFFER_COUNT) ) return -39;

		//Create Gacha Buffer 
		m_ppInGachaBuffer[i]	= new S2RingBuffer;
		if( NULL == m_ppInGachaBuffer[i] ) return -40;
		if( false == m_ppInGachaBuffer[i]->Create( sizeof( DB_GACHA ), DATABASE_BUFFER_COUNT) ) return -41;
		m_ppOutGachaBuffer[i] = new S2RingBuffer;
		if( NULL == m_ppOutGachaBuffer[i] ) return -42;
		if( false == m_ppOutGachaBuffer[i]->Create( sizeof( DB_GACHA ), DATABASE_BUFFER_COUNT) ) return -43;
				
		//Create Thread 		
		m_pi32ThreadIdx[i] = i;
		m_pHThread[i] = CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProc , &m_pi32ThreadIdx[i] , 0, &dwThreadID);
		if(m_pHThread[i] == NULL)
		{
			return -44;
		}
		
		//Set Thread Priority
		if( !SetThreadPriority( m_pHThread[i], THREAD_PRIORITY_DATABASE) )
		{
			return -45;
		}
	}

	//For Main 
	{

		m_pControlDBConfig = g_pConfig->GetDatabaseConfig("PBOption");

		if( FALSE == m_pControlDBConfig )			return -46;


		//Sia 
		m_pADOdb_DBMAIN_Shop = CADODatabase::NewObject();
		if( FALSE == m_pADOdb_DBMAIN_Shop->Connect(m_pControlDBConfig->GetDataSource(), m_pControlDBConfig->GetCatalog(), m_pControlDBConfig->GetAccount(), m_pControlDBConfig->GetPassword()) )
		{
			return -47;
		}

		//Make Main Thread 		
		m_HDBThread = CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProcMain, NULL, 0, &dwThreadID);
		if(m_HDBThread == NULL)return -50;
		if( !SetThreadPriority( m_HDBThread, THREAD_PRIORITY_DATABASE) )return -51;
		
		m_bUpdateShopInfo	= TRUE; 
	}

	if( false == OnCreateWebGift() )		
		{
			return -52;
	}

	return 1;
}

bool CModuleDBShop::OnDestroy(void)
{
	m_bIsRunningDB		= FALSE;
	m_bIsRunningWebGift		= FALSE;
	for( INT32 i = 0 ; i < DATABASE_THREAD_COUNT ; i++ )	m_pbIsRunning[i]	= FALSE;

	::WaitForSingleObject( m_HDBThread, INFINITE );
	::WaitForSingleObject( m_hThreadWebGift, INFINITE );
	::WaitForMultipleObjects( DATABASE_THREAD_COUNT, m_pHThread, TRUE, INFINITE );

	for(INT32 i = 0; i < DATABASE_THREAD_COUNT; i++)
	{ 
		I3_SAFE_DELETE( m_ppInShopItemBuffer[i] ); 
		I3_SAFE_DELETE( m_ppOutShopItemBuffer[i] ); 
		I3_SAFE_DELETE( m_ppInShopBuyBuffer[i] ); 
		I3_SAFE_DELETE( m_ppOutShopBuyBuffer[i] );
		I3_SAFE_DELETE( m_ppInShopBuffer[i] ); 
		I3_SAFE_DELETE( m_ppOutShopBuffer[i] ); 
		I3_SAFE_DELETE( m_ppInInventoryBuffer[i] ); 
		I3_SAFE_DELETE( m_ppOutInventorytBuffer[i] ); 
		I3_SAFE_DELETE( m_ppInGiftBuffer[i] ); 
		I3_SAFE_DELETE( m_ppOutGiftBuffer[i] ); 
		I3_SAFE_DELETE( m_ppInGiftListBuffer[i] ); 
		I3_SAFE_DELETE( m_ppOutGiftListBuffer[i] ); 
		I3_SAFE_DELETE( m_ppInWebGiftBuffer[i] ); 
		I3_SAFE_DELETE( m_ppOutWebGiftBuffer[i] ); 
		I3_SAFE_DELETE( m_ppInRepairBuffer[i] ); 
		I3_SAFE_DELETE( m_ppOutRepairBuffer[i] );
		I3_SAFE_DELETE( m_ppInCouponBuffer[i] );
		I3_SAFE_DELETE( m_ppOutCouponBuffer[i] );
		I3_SAFE_DELETE( m_ppInCouponBuffer[i] ); 
		I3_SAFE_DELETE( m_ppInGachaBuffer[i] ); 
		I3_SAFE_DELETE( m_ppOutGachaBuffer[i] ); 

		I3_SAFE_RELEASE( m_ppADOdb_DATABASE[i] ); 
	}

	I3MEM_SAFE_FREE(m_pi32ThreadIdx);
	I3MEM_SAFE_FREE(m_pHThread);
	I3MEM_SAFE_FREE(m_pbIsRunning);
	I3MEM_SAFE_FREE(m_ppADOdb_DATABASE); 
	I3MEM_SAFE_FREE(m_ppInShopItemBuffer); 
	I3MEM_SAFE_FREE(m_ppOutShopItemBuffer); 
	I3MEM_SAFE_FREE(m_ppInShopBuyBuffer); 
	I3MEM_SAFE_FREE(m_ppOutShopBuyBuffer); 
	I3MEM_SAFE_FREE(m_ppInShopBuffer); 
	I3MEM_SAFE_FREE(m_ppOutShopBuffer); 
	I3MEM_SAFE_FREE(m_ppInInventoryBuffer); 
	I3MEM_SAFE_FREE(m_ppOutInventorytBuffer); 
	I3MEM_SAFE_FREE(m_ppInGiftBuffer); 
	I3MEM_SAFE_FREE(m_ppOutGiftBuffer); 
	I3MEM_SAFE_FREE(m_ppInGiftListBuffer); 
	I3MEM_SAFE_FREE(m_ppOutGiftListBuffer);
	I3MEM_SAFE_FREE(m_ppInWebGiftBuffer); 
	I3MEM_SAFE_FREE(m_ppOutWebGiftBuffer);
	I3MEM_SAFE_FREE(m_ppInRepairBuffer); 
	I3MEM_SAFE_FREE(m_ppOutRepairBuffer); 
	I3MEM_SAFE_FREE(m_ppInCouponBuffer);
	I3MEM_SAFE_FREE(m_ppOutCouponBuffer);
	I3MEM_SAFE_FREE(m_ppInGachaBuffer); 
	I3MEM_SAFE_FREE(m_ppOutGachaBuffer); 

	I3_SAFE_RELEASE( m_pADOdb_DBMAIN_Shop );
	I3_SAFE_RELEASE( m_pADOdb_WEBGIFT );

	return true; 
}

void CModuleDBShop::OnMainWorking(void)
{
	g_pLog->AddLogThread(L"SHMA", 0, GetCurrentThreadId());

	m_bIsRunningDB = TRUE;

	while( m_bIsRunningDB )
	{
		//Update ShopInfo - 실시간 로드 허용 O. 노준영
		if(TRUE == m_bUpdateShopInfo && g_pShop != NULL)
		{
			g_pLog->WriteLog( L"Check Shop Infomation" );

			INT32 i32Rv = g_pShop->Update_DB();
			if( EV_FAILED(i32Rv) )
			{
				g_pLog->WriteLog( L"[CModuleDBShop::OnMainWorking] Error Update_DB() / Reason : %x ", i32Rv );
			}
			
			//g_pShop->UpdateField_DB();
			m_bUpdateShopInfo = FALSE;
		}

		::WaitForSingleObject( m_HDBThread, 1000 );

	}

	return; 
}

void CModuleDBShop::OnWorking(INT32 i32ThreadIdx)
{ 
	
	DB_SHOP_GOODS * pBuyIn; 
	DB_SHOP_ITEM  *pItemIn;

	LARGE_INTEGER stQP_Freq_Begin, stQP_Count_Begin, stQP_Count_End, stQP_Result;
	REAL64 r64ElepseTime;

	BOOL bWorking; 

	wchar_t wstrLog[MAX_PATH];
	i3String::Format(wstrLog, MAX_PATH, L"SHDB%02d", i32ThreadIdx );
	g_pLog->AddLogThread(wstrLog, 0, GetCurrentThreadId());

	m_pbIsRunning[i32ThreadIdx] = TRUE;
	while( m_pbIsRunning[i32ThreadIdx]  )
	{
		bWorking = FALSE; 

		//-----------------------------------------------------------------------------------
		//SHOP ITEM
		//-----------------------------------------------------------------------------------
		if( m_ppInShopItemBuffer[i32ThreadIdx]->GetBufferCount() > 0)
		{
			pItemIn = (DB_SHOP_ITEM *)m_ppInShopItemBuffer[i32ThreadIdx]->Pop();

			START_ELEPSETIME;

			pItemIn->m_i32Rv = _EXEC_ItemInsert(i32ThreadIdx, pItemIn->m_TUID,  &pItemIn->m_stDBItem  );		

			END_ELEPSETIME; 

			if (r64ElepseTime > CHECK_TIMEOUT )
			{	// DB TIMEOVER
				g_pLog->WriteLog(L"[ModuleDBShop] DB Time Over ItenInsert (Sec) : %.6lf / UID : %I64d ", r64ElepseTime, pItemIn->m_TUID );
			}

			m_ppInShopItemBuffer[i32ThreadIdx]->PopIdx(); 
			m_ppOutShopItemBuffer[i32ThreadIdx]->Push(pItemIn);

			bWorking = TRUE; 
		}


		//-----------------------------------------------------------------------------------
		//SHOP BUY
		//-----------------------------------------------------------------------------------
		if( m_ppInShopBuyBuffer[i32ThreadIdx]->GetBufferCount() > 0)
		{
			pBuyIn = (DB_SHOP_GOODS *)m_ppInShopBuyBuffer[i32ThreadIdx]->Pop();

			BOOL bAck = TRUE;		// Server 에 Ack 를 보낼지를 결정한다.

			START_ELEPSETIME;

			switch( pBuyIn->m_ui8DBAction )
			{
			case TYPE_SHOP_GOODS_BUY		: _EXEC_ShopBuy(i32ThreadIdx, pBuyIn);		break;
			case TYPE_SHOP_GOODS_BUY_LINK	: _EXEC_ShopBuyLink(i32ThreadIdx, pBuyIn);	break;
			case TYPE_SHOP_GOODS_INSERT		: _EXEC_GoodsInsert(i32ThreadIdx, pBuyIn );	break;
		
			default: bAck = FALSE;		break;
			}

			END_ELEPSETIME;

			if (r64ElepseTime > CHECK_TIMEOUT)
			{	// DB TIMEOVER
				g_pLog->WriteLog(L"[ModuleDBShop] DB Time Over BuyGoods (Sec) : %.6lf / UID : %I64d / Action : %d , GoodsCount : %d ", 
					r64ElepseTime, pBuyIn->m_TUID, pBuyIn->m_ui8DBAction, pBuyIn->m_ui8GoodsCount);
			}
		
			m_ppInShopBuyBuffer[i32ThreadIdx]->PopIdx(); 

			if(bAck) m_ppOutShopBuyBuffer[i32ThreadIdx]->Push(pBuyIn); 

			bWorking = TRUE; 
		}

		//-----------------------------------------------------------------------------------
		//SHOP : AUTH, INSERT, DELETE, 
		//-----------------------------------------------------------------------------------
		if( m_ppInShopBuffer[i32ThreadIdx]->GetBufferCount() > 0)
		{
			DB_SHOP_IN * pIn;
			DB_SHOP_OUT ShopOut; 
			pIn = (DB_SHOP_IN *)m_ppInShopBuffer[i32ThreadIdx]->Pop();
			ShopOut._ui8Type			= pIn->_ui8InsertType; 			
			
			BOOL bAck = TRUE;		// Server 에 Ack 를 보낼지를 결정한다.

			START_ELEPSETIME;

			switch( pIn->_ui8InsertType )
			{
			case TYPE_SHOP_ITEM_AUTH		:	_EXEC_ItemAuth(i32ThreadIdx,	pIn, &ShopOut);						break; 
			case TYPE_SHOP_ITEM_DELETE		:	_EXEC_ItemDelete(i32ThreadIdx,	pIn, &ShopOut);						break; 
			case TYPE_SHOP_ITEM_DELETE_NOACK:	_EXEC_ItemDelete(i32ThreadIdx,  pIn, &ShopOut);	bAck = FALSE;		break;
			case TYPE_SHOP_ITEM_AUTH_NOACK	:	_EXEC_ItemAuth(i32ThreadIdx,	pIn, &ShopOut);	bAck = FALSE;		break; 
			case TYPE_SHOP_ITEM_LOOP_AUTH   :   _EXEC_ItemLoopAuth(i32ThreadIdx, pIn, &ShopOut);					break;
			default							:	bAck = FALSE;														break;
			}
			// DB End Time

			END_ELEPSETIME;

			if (r64ElepseTime > CHECK_TIMEOUT)
			{	// DB TIMEOVER
				g_pLog->WriteLog(L"[ModuleDBShop] DB Time Over Shop (Sec) : %.6lf / UID : %I64d / Type : %d", r64ElepseTime, pIn->_i64UID, pIn->_ui8InsertType);
			}

			ShopOut._i64UID			= pIn->_i64UID; 
			m_ppInShopBuffer[i32ThreadIdx]->PopIdx(); 

			//SetOut
			if( bAck )		m_ppOutShopBuffer[i32ThreadIdx]->Push(&ShopOut); 

			bWorking = TRUE; 
		}

		//-----------------------------------------------------------------------------------
		//INVENTORY : GIVE INVENTORY INFO : LOGIN TIME
		if( m_ppInInventoryBuffer[i32ThreadIdx]->GetBufferCount() > 0)
		{
			DB_INVENTORY_OUT InventoryOut; 			
			DB_INVENTORY_IN * pIn = (DB_INVENTORY_IN*)m_ppInInventoryBuffer[i32ThreadIdx]->Pop();

			START_ELEPSETIME;

			INT32 i32EnableGift = 0;	// 로그용입니다.

			bool bGiftAutoAuth = false;
			// 선물함에 만료되거나 갯수 초과한 선물이 있다면 인벤토리로 지급합니다.
			if( g_pConfig->IsContentsEnabled( CONTENTS_GIFT ) )
			{
				i32EnableGift = 1;
				_EXEC_CheckGiftList( i32ThreadIdx, pIn->_i64UID, bGiftAutoAuth );
			}
	
			_EXEC_GetItem(i32ThreadIdx, pIn->_i64UID, &InventoryOut);

			END_ELEPSETIME;

			if (r64ElepseTime > CHECK_TIMEOUT)
			{	// DB TIMEOVER
				g_pLog->WriteLog(L"[ModuleDBShop] DB Time Over GetInven (Sec) : %.6lf / UID : %I64d / InvenCount : %d / GiftAutoAuth : %d", 
					r64ElepseTime, pIn->_i64UID, InventoryOut._ui16Count, bGiftAutoAuth);
			}

			InventoryOut._i64UID		= pIn->_i64UID;
			InventoryOut._bGiftAutoAuth	= bGiftAutoAuth;
			m_ppInInventoryBuffer[i32ThreadIdx]->PopIdx(); 

			//SetOut 
			m_ppOutInventorytBuffer[i32ThreadIdx]->Push(&InventoryOut); 
			bWorking = TRUE; 
		}

		//-----------------------------------------------------------------------------------
		//GIFT
		//-----------------------------------------------------------------------------------
		if( m_ppInGiftBuffer[i32ThreadIdx]->GetBufferCount() > 0)
		{
			START_ELEPSETIME;

			SEND_GIFT * pGift = (SEND_GIFT*)m_ppInGiftBuffer[i32ThreadIdx]->Pop(); 			
			if (g_pConfig->IsContentsEnabled(CONTENTS_GIFT))
			{
				_EXEC_GiftInsert(i32ThreadIdx, pGift);				// 선물하기
			}
			else
			{
				_EXEC_GiftShop_Insert(i32ThreadIdx, pGift);				// 선물하기
			}
			m_ppInGiftBuffer[i32ThreadIdx]->PopIdx();

			END_ELEPSETIME;

			if (r64ElepseTime > CHECK_TIMEOUT)
			{	// DB TIMEOVER
				g_pLog->WriteLog(L"[ModuleDBShop] DB Time Over Gift (Sec) : %.6lf / UID : %I64d / GiftCount : %d",
					r64ElepseTime, pGift->m_TReceiverUID , pGift->m_ui8GiftCount );
			}

			//SetOut 
			m_ppOutGiftBuffer[i32ThreadIdx]->Push(pGift); 
			bWorking = TRUE; 
		}

		//-----------------------------------------------------------------------------------
		//GIFT LIST. 선물함 조회, 선물 인증(지급,삭제), 선물 UPDATE( 링크후 )
		//-----------------------------------------------------------------------------------
		if( m_ppInGiftListBuffer[i32ThreadIdx]->GetBufferCount() > 0)
		{
			DB_GIFT_OUT GiftListOut = {0,};		
			GiftListOut._i32Rv = EVENT_ERROR_FAIL;

			DB_GIFT_IN * pIn = (DB_GIFT_IN*)m_ppInGiftListBuffer[i32ThreadIdx]->Pop(); 			

			// switch문 아래로 내리지 마세요.
			GiftListOut.m_ui8Type			= pIn->m_ui8Type;
			GiftListOut.m_TUID				= pIn->m_TUID;
			GiftListOut.m_ui8GiftCount		= pIn->m_ui8GiftCount;

			for( UINT8 i = 0; i < GiftListOut.m_ui8GiftCount ; ++i )
			{
				GiftListOut.m_aAuthGift[i].m_TDBIdx = pIn->m_aAuthGift[i].m_TDBIdx;
			}
			
			START_ELEPSETIME;

			switch( pIn->m_ui8Type )
			{
			case TYPE_SHOP_GIFT_LIST:	// 선물함 조회
				_EXEC_GetGiftList( i32ThreadIdx, &GiftListOut);	
				break;
			case TYPE_SHOP_GIFT_BUY:	// 선물 구매(링크결제)후 처리.
				{
					i3mem::Copy(&GiftListOut.m_SendGift, &pIn->m_SendGift, sizeof(SEND_GIFT));	//SIA에서 넘어온 결과를 처리할때만 Buffer 정보를 복사합니다.

					if (!g_pConfig->IsContentsEnabled(CONTENTS_GIFT))
					{						
						_EXEC_GiftBuyLink_Inven(i32ThreadIdx, &GiftListOut);						
					}
					else
					{
						_EXEC_GiftBuyLink(i32ThreadIdx, &GiftListOut);
					}					
				}
				break;
			case TYPE_SHOP_GIFT_READ:	// 선물 메시지 읽기.
				_EXEC_GiftRead( i32ThreadIdx, &GiftListOut );	
				break;
			case TYPE_SHOP_GIFT_DELETE:	// 선물 삭제.
				_EXEC_GiftDelete( i32ThreadIdx, &GiftListOut );	
				break;
			case TYPE_SHOP_GIFT_AUTH:	// 선물 인증.(인벤토리 지급)
				_EXEC_GiftAuth( i32ThreadIdx, &GiftListOut );	
				break;
			}

			END_ELEPSETIME;

			if (r64ElepseTime > CHECK_TIMEOUT)
			{	// DB TIMEOVER
				g_pLog->WriteLog(L"[ModuleDBShop] DB Time Over GiftList (Sec) : %.6lf / Type : %d / GiftCount : %d",
					r64ElepseTime, pIn->m_ui8Type, GiftListOut.m_ui8GiftCount);
			}
			 
			m_ppInGiftListBuffer[i32ThreadIdx]->PopIdx(); 

			//SetOut 
			m_ppOutGiftListBuffer[i32ThreadIdx]->Push(&GiftListOut); 
			bWorking = TRUE; 
		}		

		//-----------------------------------------------------------------------------------
		//REPAIR
		if( m_ppInRepairBuffer[i32ThreadIdx]->GetBufferCount() > 0)
		{
			START_ELEPSETIME;

			DB_REPAIR * pRepair = (DB_REPAIR*)m_ppInRepairBuffer[i32ThreadIdx]->Pop(); 
			switch( pRepair->m_eType )
			{
			case DB_REPAIR_TYPE_BUY : 
				_EXEC_Repair(i32ThreadIdx, pRepair);  
				break;
			case DB_REPAIR_TYPE_LINK : 
				_EXEC_RepairLink(i32ThreadIdx, pRepair);  
				break;
			}
			m_ppOutRepairBuffer[i32ThreadIdx]->Push(pRepair); 
			m_ppInRepairBuffer[i32ThreadIdx]->PopIdx(); 

			END_ELEPSETIME;

			if (r64ElepseTime > CHECK_TIMEOUT)
			{	// DB TIMEOVER
				g_pLog->WriteLog(L"[ModuleDBShop] DB Time Over Repair (Sec) : %.6lf / Type : %d , RepairCount : %d ",
					r64ElepseTime, pRepair->m_eType, pRepair->m_stRepairInfo.m_ui8ItemCount);
			}

			//SetOut 
			
			bWorking = TRUE; 
		}

		//-----------------------------------------------------------------------------------
		//GIFTCOUPON
		if( m_ppInCouponBuffer[i32ThreadIdx]->GetBufferCount() > 0)
		{
			START_ELEPSETIME;

			DB_COUPON* pCoupon = (DB_COUPON*)m_ppInCouponBuffer[i32ThreadIdx]->Pop();
			switch( pCoupon->m_eType )
			{
			case DB_COUPON_TYPE_USE:
				break;
			case DB_COUPON_TYPE_LINK:
				{
#ifdef I3_DEBUG
					if ( EV_FAILED( pCoupon->m_i32Ret ) ) break;

					// 페이레터에서 직접 기프트딜리버를 통해 지급하기때문에 하기의 루틴은 사용하지 않습니다만, 추후 변경될 여지있습니다. 2014.6.3
					// 1. 유저 선물DB에 삽입
					SEND_GIFT SendGift;
					SendGift.m_eGiftType = GIFT_TYPE_COUPON;
					SendGift.m_TReceiverUID = pCoupon->m_TUID;
					SendGift.m_ui8GiftCount = pCoupon->m_i8GoodsCount;
					for( UINT8 i = 0; i < SendGift.m_ui8GiftCount; i++ )
						SendGift.m_aGift[i].m_ui32GoodsID = pCoupon->m_ai32GoodsID[i];
					i3String::Copy( SendGift.m_strSenderNick, _TT("GM"), NET_NICK_NAME_SIZE );

					char strCoupon[GIFTCOUPON_LENGTH + 1];
					i3mem::Copy( strCoupon, pCoupon->m_strCoupon, GIFTCOUPON_LENGTH + 1);
					strCoupon[GIFTCOUPON_LENGTH] = '\0';
					i3String::Format( SendGift.m_strMessage, MAX_GIFT_MESSAGE_SIZE, _TT("Coupon:%s Used"), strCoupon );

					_EXEC_GiftInsert(i32ThreadIdx, &SendGift);

					// 2. 유저 선물리스트 갱신 요청
					DB_GIFT_IN DBGiftIn;
					DBGiftIn.m_TUID			= pCoupon->m_TUID;
					DBGiftIn.m_ui8Type		= TYPE_SHOP_GIFT_LIST;
					DBGiftIn.m_ui8GiftCount = 0;

					m_ppInGiftListBuffer[i32ThreadIdx]->Push( &DBGiftIn );
#endif
				}
				break;
			}
			m_ppOutCouponBuffer[i32ThreadIdx]->Push(pCoupon);
			m_ppInCouponBuffer[i32ThreadIdx]->PopIdx();

			END_ELEPSETIME;

			if (r64ElepseTime > CHECK_TIMEOUT)
			{	// DB TIMEOVER
				char strCouponCode[GIFTCOUPON_LENGTH + 1];
				i3mem::Copy(strCouponCode, pCoupon->m_strCoupon, GIFTCOUPON_LENGTH + 1);
				strCouponCode[GIFTCOUPON_LENGTH] = '\0';
				g_pLog->WriteLog(L"[ModuleDBShop] DB Time Over Coupon (Sec) : %.6lf / CouponUseType : %d , CouponCode : %s  ",
					r64ElepseTime, pCoupon->m_eType, strCouponCode);
			}

			bWorking = TRUE;
		}

		//-----------------------------------------------------------------------------------
		// GACHA
		//-----------------------------------------------------------------------------------
		if( m_ppInGachaBuffer[i32ThreadIdx]->GetBufferCount() > 0)
		{
			DB_GACHA* pGacha= (DB_GACHA*)m_ppInGachaBuffer[i32ThreadIdx]->Pop();

			START_ELEPSETIME;

			switch ( pGacha->m_ui8Action )
			{
			case TYPE_GACHA_PURCHASE :
				pGacha->m_i32Rv = _EXEC_GachaBuy(i32ThreadIdx, pGacha );		
				break;
			case TYPE_GACHA_PROVIDE :
				pGacha->m_i32Rv = _EXEC_ItemInsert(i32ThreadIdx, pGacha->m_TUID,  &pGacha->m_stDBItem  );		
				break;
			case TYPE_GACHA_TICKET_DELETE :
				{
					DB_SHOP_IN stShopIn;
					DB_SHOP_OUT stShopOut;
					stShopIn._i64UID = pGacha->m_TUID;
					stShopIn._TWareDBIdx = pGacha->m_TTicketDBIdx;
					_EXEC_ItemDelete(i32ThreadIdx,	&stShopIn, &stShopOut);
					pGacha->m_i32Rv = stShopOut._i32Rv;
				}
				break;
			default:
				break;
			}

			END_ELEPSETIME;

			g_pStatistics->AddStat(STAT_TYPE_DB_SHOP, i32ThreadIdx, r64ElepseTime);

			if (r64ElepseTime > CHECK_TIMEOUT)
			{	// DB TIMEOVER
				g_pLog->WriteLog(L"[ModuleDBShop] DB Time Over ItenInsert (Sec) : %.6lf / UID : %I64d / Action : %d ", r64ElepseTime, pGacha->m_TUID, pGacha->m_ui8Action);
			}

			m_ppInGachaBuffer[i32ThreadIdx]->PopIdx(); 
			m_ppOutGachaBuffer[i32ThreadIdx]->Push(pGacha); 

			bWorking = TRUE; 
		}

		//-----------------------------------------------------------------------------------
		//WEB Gift
		//-----------------------------------------------------------------------------------
		if( m_ppInWebGiftBuffer[i32ThreadIdx]->GetBufferCount() > 0)
		{
			DB_WEB_GIFT_IN	* pIn	= (DB_WEB_GIFT_IN *)m_ppInWebGiftBuffer[i32ThreadIdx]->Pop();	
			DB_WEB_GIFT_OUT	pOut;	 
			if( NULL != pIn )
			{
				pOut._i64UID			= pIn->_i64UID;
				pOut._i32BufferIdx		= pIn->_i32BufferIdx; 
				pOut.m_TGoodsID			= pIn->_ui32GoodsID; 

				if ( !g_pConfig->IsContentsEnabled( CONTENTS_GIFT ) )				 
				{	// 선물 시스템 비활성화시 인벤토리로 지급 
					DB_ITEM DBItem; 
					DBItem.m_ui8ItemCount = 0;
					DBItem.m_ui8InsertType = ITEM_INSERT_REQUEST_WEB_GIFT;

					SHOP_GOODS_LIST* pGoodsList = g_pShop->GetGoodsByID( pIn->_ui32GoodsID );		// GoodsID 로 Goods 를 얻어온다.

					for( UINT16 i = 0 ; i < pGoodsList->_ui16ItemCount; i++ )	
					{	 
						SHOP_ITEM_DATA* pItemData = pGoodsList->_ppItemData[i];
						if( NULL == pItemData ) 
						{	 
							//g_pLog->WriteLog( m_pi32LogShopIdx[ i32ThreadIdx ], "WebGift Inven Not Find Item / UID : %I64d / DBIdx : %d / GoodsID : %d /ItemIdx:%d ",
							//	pIn->_i64UID, pIn->_i32BufferIdx, pIn->_ui32GoodsID, i ); 
							continue; 
						}
						DBItem.m_aItem[DBItem.m_ui8ItemCount]._ui32ItemID		= pItemData->_ui32ItemID;
						DBItem.m_aItem[DBItem.m_ui8ItemCount]._ui32AddItemArg	= pGoodsList->_pui32ItemArg[i]; 
						++DBItem.m_ui8ItemCount;
					}


					START_ELEPSETIME;

					pOut._i32Rv = _EXEC_ItemInsert( i32ThreadIdx, pIn->_i64UID, &DBItem, TYPE_SHOP_WEB_GIFT );

					END_ELEPSETIME;

					if (r64ElepseTime > CHECK_TIMEOUT)
					{	// DB TIMEOVER
						g_pLog->WriteLog(L"[ModuleDBShop] DB Time Over Web Gift Gift (Sec) : %.6lf / UID : %I64d ", r64ElepseTime, pIn->_i64UID);
					}

					if( EV_FAILED( pOut._i32Rv ) )
					{	// Error	 
						//g_pLog->WriteLog( m_pi32LogShopIdx[ i32ThreadIdx ], "WebGift Inven DB ItemInsert Fail / Reason : %x / UID : %I64d / DBIdx : %d / GoodsID : %d / ItemCount : %d / ItemId:%d / ItemArg : %d",
						//	pOut._i32Rv, pIn->_i64UID, pIn->_i32BufferIdx, pIn->_ui32GoodsID, DBItem.m_ui8ItemCount, DBItem.m_aItem[0]._ui32ItemID, DBItem.m_aItem[0]._ui32AddItemArg );	

					}
					else
					{
						// Trans - Game 인벤토리 갱신.
						DB_SHOP_ITEM DBShopItem;
						DBShopItem.m_i32Rv = pOut._i32Rv;
						DBShopItem.m_TUID = pIn->_i64UID;
						i3mem::Copy( &DBShopItem.m_stDBItem, &DBItem, sizeof( DB_ITEM) );
						i3String::Copy(DBShopItem.m_strSenderNick, _TT("GM"), NET_NICK_NAME_SIZE);

						if( DBItem.m_ui8InsertType == ITEM_INSERT_REQUEST_WEB_GIFT)
							i3String::Copy(DBShopItem.m_strMessage, pIn->_strMessage, MAX_GIFT_MESSAGE_SIZE);

						m_ppOutShopItemBuffer[i32ThreadIdx]->Push(&DBShopItem); 
					}
				}
				else	// 선물 시스템 활성화시 선물함으로 지급
				{
					// 선물 받는 유저에게 Send.
					SEND_GIFT SendGift; 
					switch( pIn->_eInsertType )
					{
					default:
					case WEBGIFT_INSERT_TYPE_WEBTOOL :
						SendGift.m_eRequestType = ITEM_INSERT_REQUEST_WEB_GIFT;
						break;
					case WEBGIFT_INSERT_TYPE_WEBSHOP :
						SendGift.m_eRequestType = ITEM_INSERT_REQUEST_WEB_SHOP;
						break;
					}
					
					SendGift.m_TSenderUID	= 0;
					SendGift.m_eGiftType	= GIFT_TYPE_GM;
					SendGift.m_TReceiverUID = pIn->_i64UID;
					SendGift.m_ui8GiftCount = 1;
					SendGift.m_aGift[0].m_ui32GoodsID= pIn->_ui32GoodsID;  
					i3String::Copy( SendGift.m_strSenderNick, _TT("GM"),  NET_NICK_NAME_SIZE  );
					i3String::Copy( SendGift.m_strMessage, pIn->_strMessage, MAX_GIFT_MESSAGE_SIZE );

					START_ELEPSETIME;
					_EXEC_GiftInsert(i32ThreadIdx, &SendGift);
					END_ELEPSETIME;

					if (r64ElepseTime > CHECK_TIMEOUT)
					{	// DB TIMEOVER
						g_pLog->WriteLog(L"[ModuleDBShop] DB Time Over Web Gift Inven (Sec) : %.6lf / UID : %I64d ", r64ElepseTime, pIn->_i64UID);
					}

					pOut._i32Rv				= SendGift.m_aGift[0].m_i32Rv;
					pOut._i32GiftWareDBIdx	= SendGift.m_aGift[0].m_TGiftDBIdx ;

					if( EV_FAILED( pOut._i32Rv ) )
					{	// Error	 
					//	g_pLog->WriteLog( m_pi32LogShopIdx[ i32ThreadIdx ], "WebGift Gift DB Insert Fail / Reason : %x / UID : %I64d / DBIdx : %d / GoodsID : %d ",
					//		pOut._i32Rv, pIn->_i64UID, pIn->_i32BufferIdx, pIn->_ui32GoodsID );	 
					}
					else
					{
						// 선물 받는 유저에게 Send
						m_ppOutGiftBuffer[i32ThreadIdx]->Push(&SendGift); 
					}
				}

				// ModuleDBShop_WebGift에서 결과값을 UPDATE 시킨다.  
				if( RINGBUFFER_SUCCESS != m_ppOutWebGiftBuffer[i32ThreadIdx]->Push(&pOut) )
				{	// 이 작업이 실패하면 중복지급 될 수 있습니다.
					g_pLog->WriteLog( L"WebGiftTr Buffer Push Fail / UID : %I64D / DBIdx : %d / GoodsID : %d  ",
						pIn->_i64UID, pIn->_i32BufferIdx, pIn->_ui32GoodsID );
				}

			}

			bWorking = TRUE; 
			
			m_ppInWebGiftBuffer[i32ThreadIdx]->PopIdx();
		}

		//아무것도 한일이 없다면 잠시 휴식을..
		if(!bWorking)	::WaitForSingleObject( m_pHThread[i32ThreadIdx], 1 );
	}
	return; 
}

////////////////////////////////////////////////////////////////////////////
INT32 CModuleDBShop::GetOutShopItemCount_T(void)
{
	INT32 i32Count = 0; 
	for(INT32 i = 0 ; i < DATABASE_THREAD_COUNT; i++)
	{
		i32Count += m_ppOutShopItemBuffer[i]->GetBufferCount();
	}

	return i32Count; 
}

DB_SHOP_ITEM * CModuleDBShop::GetOutShopItemData_T(void)
{
	DB_SHOP_ITEM * pOut = NULL;

	for(INT32 i = 0; i < DATABASE_THREAD_COUNT; i++)
	{
		m_ui32OutShopItemThreadIdx++; 
		if(m_ui32OutShopItemThreadIdx >= DATABASE_THREAD_COUNT) m_ui32OutShopItemThreadIdx = 0; 

		if(m_ppOutShopItemBuffer[m_ui32OutShopItemThreadIdx]->GetBufferCount() > 0)
		{
			pOut = (DB_SHOP_ITEM *)m_ppOutShopItemBuffer[m_ui32OutShopItemThreadIdx]->Pop(); 
			break;
		}
	}
	return pOut; 
}

void CModuleDBShop::GetOutShopItemIdx_T(void)
{
	m_ppOutShopItemBuffer[m_ui32OutShopItemThreadIdx]->PopIdx(); 
	return; 
}

////////////////////////////////////////////////////////////////////////////
INT32 CModuleDBShop::GetOutShopBuyCount_T(void)
{
	INT32 i32Count = 0;
	for(INT32 i = 0 ; i < DATABASE_THREAD_COUNT; i++)
	{
		i32Count += m_ppOutShopBuyBuffer[i]->GetBufferCount();
	}

	return i32Count;
}

DB_SHOP_GOODS * CModuleDBShop::GetOutShopBuyData_T(void)
{
	DB_SHOP_GOODS * pOut = NULL;

	for(INT32 i = 0; i < DATABASE_THREAD_COUNT; i++)
	{
		m_ui32OutShopBuyThreadIdx++; 
		if(m_ui32OutShopBuyThreadIdx >= DATABASE_THREAD_COUNT) m_ui32OutShopBuyThreadIdx = 0; 

		if(m_ppOutShopBuyBuffer[m_ui32OutShopBuyThreadIdx]->GetBufferCount() > 0)
		{
			pOut = (DB_SHOP_GOODS *)m_ppOutShopBuyBuffer[m_ui32OutShopBuyThreadIdx]->Pop(); 
			break;
		}
	}
	return pOut; 
}

void CModuleDBShop::GetOutShopBuyIdx_T(void)
{
	m_ppOutShopBuyBuffer[m_ui32OutShopBuyThreadIdx]->PopIdx(); 
	return; 
}

////////////////////////////////////////////////////////////////////////////
INT32 CModuleDBShop::GetOutShopCount_T(void)
{
	INT32 i32Count = 0;
	for(INT32 i = 0 ; i < DATABASE_THREAD_COUNT; i++)
	{
		i32Count += m_ppOutShopBuffer[i]->GetBufferCount();
	}

	return i32Count;
}

DB_SHOP_OUT * CModuleDBShop::GetOutShopData_T(void)
{
	DB_SHOP_OUT * pOut = NULL;

	for(INT32 i = 0; i < DATABASE_THREAD_COUNT; i++)
	{
		m_ui32OutShopThreadIdx++; 
		if(m_ui32OutShopThreadIdx >= DATABASE_THREAD_COUNT) m_ui32OutShopThreadIdx = 0; 

		if(m_ppOutShopBuffer[m_ui32OutShopThreadIdx]->GetBufferCount() > 0)
		{
			pOut = (DB_SHOP_OUT *)m_ppOutShopBuffer[m_ui32OutShopThreadIdx]->Pop(); 
			break;
		}
	}
	return pOut; 
}

void CModuleDBShop::GetOutShopIdx_T(void)
{
	m_ppOutShopBuffer[m_ui32OutShopThreadIdx]->PopIdx(); 
	return; 
}

BOOL CModuleDBShop::InShopItem( DB_SHOP_ITEM * pIn )
{
	INT32 i32ThreadIdx = (INT32)(pIn->m_TUID % DATABASE_THREAD_COUNT);
	if(0 > i32ThreadIdx || DATABASE_THREAD_COUNT <= i32ThreadIdx) return FALSE;

	//Push
	if( m_ppInShopItemBuffer[i32ThreadIdx]->Push( pIn ) != RINGBUFFER_SUCCESS )
	{
		return FALSE; 
	}
	return TRUE;

}
BOOL CModuleDBShop::InShopBuy( DB_SHOP_GOODS * pIn )
{
	INT32 i32ThreadIdx = (INT32)(pIn->m_TUID % DATABASE_THREAD_COUNT);
	if(0 > i32ThreadIdx || DATABASE_THREAD_COUNT <= i32ThreadIdx) return FALSE;

	//Push
	if( m_ppInShopBuyBuffer[i32ThreadIdx]->Push( pIn ) != RINGBUFFER_SUCCESS )
	{
		return FALSE; 
	}
	return TRUE;
}

BOOL CModuleDBShop::InInsertItem( DB_SHOP_IN * pIn )
{
	INT32 i32ThreadIdx = (INT32)(pIn->_i64UID % DATABASE_THREAD_COUNT);
	if(0 > i32ThreadIdx || DATABASE_THREAD_COUNT <= i32ThreadIdx) return FALSE;

	//Push
	if( m_ppInShopBuffer[i32ThreadIdx]->Push( pIn ) != RINGBUFFER_SUCCESS )
	{
		return FALSE; 
	}
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//INVENTRORY 용
BOOL CModuleDBShop::InInventory( DB_INVENTORY_IN* pIn )
{
	INT32 i32ThreadIdx = (INT32)(pIn->_i64UID % DATABASE_THREAD_COUNT);
	if(0 > i32ThreadIdx || DATABASE_THREAD_COUNT <= i32ThreadIdx) return FALSE;

	//Push
	if( m_ppInInventoryBuffer[i32ThreadIdx]->Push( pIn ) != RINGBUFFER_SUCCESS )
	{
		return FALSE; 
	}
	return TRUE; 
}

INT32 CModuleDBShop::GetOutInventoryCount_T(void)
{
	INT32 i32Count = 0; 
	for(INT32 i = 0 ; i < DATABASE_THREAD_COUNT; i++)
	{
		i32Count += m_ppOutInventorytBuffer[i]->GetBufferCount();
	}
	return i32Count;
}

DB_INVENTORY_OUT *	CModuleDBShop::GetOutInventoryData_T(void)
{
	DB_INVENTORY_OUT * pOut = NULL;

	for(INT32 i = 0; i < DATABASE_THREAD_COUNT; i++)
	{
		m_ui32OutInventoryThreadIdx++; 
		if(m_ui32OutInventoryThreadIdx >= DATABASE_THREAD_COUNT) m_ui32OutInventoryThreadIdx = 0; 

		if(m_ppOutInventorytBuffer[m_ui32OutInventoryThreadIdx]->GetBufferCount() > 0)
		{
			pOut = (DB_INVENTORY_OUT *)m_ppOutInventorytBuffer[m_ui32OutInventoryThreadIdx]->Pop(); 
			break;
		}
	}

	return pOut; 
}

void CModuleDBShop::GetOutInventoryIdx_T(void)
{
	m_ppOutInventorytBuffer[m_ui32OutInventoryThreadIdx]->PopIdx(); 
}


////////////////////////////////////////////////////////////////////////////
//Gift 용
BOOL CModuleDBShop::InGift( DB_GIFT_IN* pIn )
{
	INT32 i32ThreadIdx = (INT32)(pIn->m_TUID % DATABASE_THREAD_COUNT);
	if(0 > i32ThreadIdx || DATABASE_THREAD_COUNT <= i32ThreadIdx) return FALSE;

	//Push
	if( m_ppInGiftListBuffer[i32ThreadIdx]->Push( pIn ) != RINGBUFFER_SUCCESS )
	{
		return FALSE; 
	}
	return TRUE; 
}

BOOL CModuleDBShop::InInsertGift( SEND_GIFT * pIn)
{
	INT32 i32ThreadIdx = (INT32)(pIn->m_TReceiverUID % DATABASE_THREAD_COUNT);
	if(0 > i32ThreadIdx || DATABASE_THREAD_COUNT <= i32ThreadIdx) return FALSE;

	//Push
	if( m_ppInGiftBuffer[i32ThreadIdx]->Push( pIn ) != RINGBUFFER_SUCCESS )
	{
		return FALSE; 
	}
	return TRUE; 
}

INT32 CModuleDBShop::GetOutGiftCount_T(void)
{
	INT32 i32Count = 0; 
	for(INT32 i = 0 ; i < DATABASE_THREAD_COUNT; i++)
	{
		i32Count += m_ppOutGiftBuffer[i]->GetBufferCount();
	}
	return i32Count;
}

void CModuleDBShop::GetOutGiftIdx_T(void)
{
	m_ppOutGiftBuffer[m_ui32OutGiftThreadIdx]->PopIdx(); 
}

SEND_GIFT * CModuleDBShop::GetOutGiftData_T(void)
{
	SEND_GIFT * pOut = NULL;

	for(INT32 i = 0; i < DATABASE_THREAD_COUNT; i++)
	{
		m_ui32OutGiftThreadIdx++; 
		if(m_ui32OutGiftThreadIdx >= DATABASE_THREAD_COUNT) m_ui32OutGiftThreadIdx = 0; 

		if(m_ppOutGiftBuffer[m_ui32OutGiftThreadIdx]->GetBufferCount() > 0)
		{
			pOut = (SEND_GIFT *)m_ppOutGiftBuffer[m_ui32OutGiftThreadIdx]->Pop(); 
			break;
		}
	}

	return pOut; 
}


INT32 CModuleDBShop::GetOutGiftListCount_T(void)
{
	INT32 i32Count = 0; 
	for(INT32 i = 0 ; i < DATABASE_THREAD_COUNT; i++)
	{
		i32Count += m_ppOutGiftListBuffer[i]->GetBufferCount();
	}
	return i32Count;
}

void CModuleDBShop::GetOutGiftListIdx_T(void)
{
	m_ppOutGiftListBuffer[m_ui32OutGiftListThreadIdx]->PopIdx(); 
}

DB_GIFT_OUT * CModuleDBShop::GetOutGiftListData_T(void)
{
	DB_GIFT_OUT * pOut = NULL;

	for(INT32 i = 0; i < DATABASE_THREAD_COUNT; i++)
	{
		m_ui32OutGiftListThreadIdx++; 
		if(m_ui32OutGiftListThreadIdx >= DATABASE_THREAD_COUNT) m_ui32OutGiftListThreadIdx = 0; 

		if(m_ppOutGiftListBuffer[m_ui32OutGiftListThreadIdx]->GetBufferCount() > 0)
		{
			pOut = (DB_GIFT_OUT *)m_ppOutGiftListBuffer[m_ui32OutGiftListThreadIdx]->Pop(); 
			break; 
		}
	}

	return pOut; 
}

// WebGift
INT32 CModuleDBShop::GetOutWebGiftCount_T(void)
{
	INT32 i32Count = 0; 
	for(INT32 i = 0 ; i < DATABASE_THREAD_COUNT; i++)
	{
		i32Count += m_ppOutWebGiftBuffer[i]->GetBufferCount();
	}
	return i32Count;
}

void CModuleDBShop::GetOutWebGiftIdx_T(void)
{
	m_ppOutWebGiftBuffer[m_ui32OutWebGiftThreadIdx]->PopIdx(); 
}

DB_WEB_GIFT_OUT * CModuleDBShop::GetOutWebGiftData_T(void)
{
	DB_WEB_GIFT_OUT * pOut = NULL;

	for(INT32 i = 0; i < DATABASE_THREAD_COUNT; i++)
	{
		m_ui32OutWebGiftThreadIdx++; 
		if(m_ui32OutWebGiftThreadIdx >= DATABASE_THREAD_COUNT) m_ui32OutWebGiftThreadIdx = 0; 

		if(m_ppOutWebGiftBuffer[m_ui32OutWebGiftThreadIdx]->GetBufferCount() > 0)
		{
			pOut = (DB_WEB_GIFT_OUT *)m_ppOutWebGiftBuffer[m_ui32OutWebGiftThreadIdx]->Pop(); 
			break;
		}
	}

	return pOut; 
}

// GiftCoupon
INT32 CModuleDBShop::GetOutCouponCount_T(void)
{
	INT32 i32Count = 0;
	for( INT32 i = 0; i < DATABASE_THREAD_COUNT; i++)
	{
		i32Count += m_ppOutCouponBuffer[i]->GetBufferCount();
	}
	return i32Count;
}

void CModuleDBShop::GetOutCouponIdx_T(void)
{
	m_ppOutCouponBuffer[m_ui32OutCouponThreadIdx]->PopIdx();
}

DB_COUPON * CModuleDBShop::GetOutCouponData_T(void)
{
	DB_COUPON * pOut = NULL;

	for( INT32 i = 0; i < DATABASE_THREAD_COUNT; i++ )
	{
		m_ui32OutCouponThreadIdx++;
		if( m_ui32OutCouponThreadIdx >= DATABASE_THREAD_COUNT ) m_ui32OutCouponThreadIdx = 0;

		if( m_ppOutCouponBuffer[m_ui32OutCouponThreadIdx]->GetBufferCount() > 0 )
		{
			pOut = (DB_COUPON*)m_ppOutCouponBuffer[m_ui32OutCouponThreadIdx]->Pop();
			break;
		}
	}

	return pOut;
}

BOOL CModuleDBShop::InCoupon( DB_COUPON * pIn )
{
	INT32 i32ThreadIdx = (INT32)(pIn->m_TUID % DATABASE_THREAD_COUNT);
	if(0 > i32ThreadIdx || DATABASE_THREAD_COUNT <= i32ThreadIdx) return FALSE;

	if( m_ppInCouponBuffer[i32ThreadIdx]->Push( pIn ) != RINGBUFFER_SUCCESS )
	{
		return FALSE;
	}
	return TRUE;
}

// Repair
INT32 CModuleDBShop::GetOutRepairCount_T(void)
{
	INT32 i32Count = 0; 
	for(INT32 i = 0 ; i < DATABASE_THREAD_COUNT; i++)
	{
		i32Count += m_ppOutRepairBuffer[i]->GetBufferCount();
	}
	return i32Count;
}

void CModuleDBShop::GetOutRepairIdx_T(void)
{
	m_ppOutRepairBuffer[m_ui32OutRepairThreadIdx]->PopIdx(); 
}

DB_REPAIR * CModuleDBShop::GetOutRepairData_T(void)
{
	DB_REPAIR * pOut = NULL;

	for(INT32 i = 0; i < DATABASE_THREAD_COUNT; i++)
	{
		m_ui32OutRepairThreadIdx++; 
		if(m_ui32OutRepairThreadIdx >= DATABASE_THREAD_COUNT) m_ui32OutRepairThreadIdx = 0; 

		if(m_ppOutRepairBuffer[m_ui32OutRepairThreadIdx]->GetBufferCount() > 0)
		{
			pOut = (DB_REPAIR*)m_ppOutRepairBuffer[m_ui32OutRepairThreadIdx]->Pop(); 
			break; 
		}
	}

	return pOut; 
}

// Gacha
INT32 CModuleDBShop::GetOutGachaCount_T(void)
{
	INT32 i32Count = 0;
	for( INT32 i = 0; i < DATABASE_THREAD_COUNT; i++)
	{
		i32Count += m_ppOutGachaBuffer[i]->GetBufferCount();
	}
	return i32Count;
}

void CModuleDBShop::GetOutGachaIdx_T(void)
{
	m_ppOutGachaBuffer[m_ui32OutGachaThreadIdx]->PopIdx();
}

DB_GACHA * CModuleDBShop::GetOutGachaData_T(void)
{
	DB_GACHA * pOut = NULL;

	for( INT32 i = 0; i < DATABASE_THREAD_COUNT; i++ )
	{
		m_ui32OutGachaThreadIdx++;
		if( m_ui32OutGachaThreadIdx >= DATABASE_THREAD_COUNT ) m_ui32OutGachaThreadIdx = 0;

		if( m_ppOutGachaBuffer[m_ui32OutGachaThreadIdx]->GetBufferCount() > 0 )
		{
			pOut = (DB_GACHA*)m_ppOutGachaBuffer[m_ui32OutGachaThreadIdx]->Pop();
			break;
		}
	}

	return pOut;
}

BOOL CModuleDBShop::InGacha( DB_GACHA * pIn )
{
	INT32 i32ThreadIdx = (INT32)(pIn->m_TUID % DATABASE_THREAD_COUNT);
	if(0 > i32ThreadIdx || DATABASE_THREAD_COUNT <= i32ThreadIdx) return FALSE;

	if( m_ppInGachaBuffer[i32ThreadIdx]->Push( pIn ) != RINGBUFFER_SUCCESS )
	{
		return FALSE;
	}
	return TRUE;
}

bool	CModuleDBShop::GetFieldShopInfo_M( FieldShopInfo* pFieldShopInfo )
{
	wchar_t wstrSQL[SERVER_STRING_COUNT]; 

	////////////////////////////////////////////////////////////////////////////////
	// Version 체크 후 Category 얻어오기
	S2DB::MakeQueryW( wstrSQL, L"usp_FieldShopVersion_scan",
		DB_DATA_TYPE_INT,		pFieldShopInfo->m_ui32Version,
		DB_DATA_TYPE_END );
	CADODatabase * pDataBase = m_pADOdb_DBMAIN_Shop; 

	if( !pDataBase->ExecuteSP( wstrSQL ) )
	{
		ErrorDBShop( MODULE_DB_SHOP_MAIN_THREAD_IDX, wstrSQL );
		return false;
	}

	INT32 i32Count = pDataBase->GetRecordCount();
	if( 0 == i32Count )
	{
		_DataBaseResultFailLog( MODULE_DB_SHOP_MAIN_THREAD_IDX, wstrSQL );
		return false;
	}
	// 구버전 또는 Select 실패
	if( 2 != pDataBase->GetINT32Data(0) )	return false;	
	INT32 i32Version = pDataBase->GetINT32Data(1);

	// 버전을 제외한 나머지 부분을 초기화 해 줍니다.
	memset( pFieldShopInfo->m_CategoryList, 0, sizeof(Category)*MAX_FIELDSHOP_CATEGORYCOUNT );

	for( INT32 i = 0 ; i < i32Count ; i++ )
	{
		INT32 i32Idx = pDataBase->GetINT8Data(2);
		pDataBase->GetTextData( 3, pFieldShopInfo->m_CategoryList[ i32Idx-1 ].m_strCategoryName, MAX_FIELDSHOP_CATEGORYNAMESIZE );
		pDataBase->MoveNext();
	}

	////////////////////////////////////////////////////////////////////////////////
	// GoodsList 부분 얻어오기
	S2DB::MakeQueryW( wstrSQL, L"usp_FieldShopGoods_scan", 
		DB_DATA_TYPE_END );
	if( !pDataBase->ExecuteSP( wstrSQL ) )
	{
		ErrorDBShop( MODULE_DB_SHOP_MAIN_THREAD_IDX, wstrSQL );
		return false;
	}
	if( 1 != pDataBase->GetINT32Data(0) )	return false;

	i32Count = pDataBase->GetRecordCount();
	for( INT32 i = 0 ; i < i32Count ; i++ )
	{
		INT8 i8Use			= pDataBase->GetINT8Data(1);	// 적용유무.
		if ( 1 == i8Use )
		{
			INT32 i32Idx			= pDataBase->GetINT8Data(2) - 1;
			INT32 i32GoodsIdx		= pDataBase->GetINT8Data(3) - 1;
			INT32 i32GoodsDayIdx	= pDataBase->GetINT8Data(4) - 1;

			if( MAX_FIELDSHOP_CATEGORYCOUNT > i32Idx && 
				MAX_FIELDSHOP_GOODSLISTCOUNT > i32GoodsIdx &&
				MAX_FIELDSHOP_GOODSPERIODCOUNT > i32GoodsDayIdx )
			{
				pFieldShopInfo->m_CategoryList[ i32Idx ].m_ui32GoodsIDList[ i32GoodsIdx ][ i32GoodsDayIdx ] = pDataBase->GetUINT32Data( 5 );
			}
		}
		pDataBase->MoveNext();
	}	

	pFieldShopInfo->m_ui32Version = i32Version;

	return true;
}

////////////////////////////////////////////////////////////////////////////
INT32 CModuleDBShop::GetShopVersion_M(void)
{
	wchar_t wstrSQL[SERVER_STRING_COUNT]; 

	S2DB::MakeQueryW( wstrSQL, L"usp_ShopVersin_scan",
		DB_DATA_TYPE_END );

	CADODatabase * pDataBase = m_pADOdb_DBMAIN_Shop; 

	if( !pDataBase->ExecuteSP( wstrSQL ) )
	{
		ErrorDBShop( MODULE_DB_SHOP_MAIN_THREAD_IDX, wstrSQL );
		return 0;
	}
	if( 1 != pDataBase->GetRecordCount() )
	{
		_DataBaseResultFailLog( MODULE_DB_SHOP_MAIN_THREAD_IDX, wstrSQL );
		return 0;
	}

	return pDataBase->GetINT32Data(0); 
}


INT32	CModuleDBShop::GetShopData_M(void)
{	
	SHOP_DATA* pShopData = g_pShop->GetNotUseShopData();

	i3mem::FillZero( pShopData,	sizeof(SHOP_DATA) );
	
	wchar_t wstrSQL[SERVER_STRING_COUNT];
	wchar_t	wstrLog[SERVER_STRING_COUNT];
	INT32 i32RealCount = 0;
	INT32 i32Count = 0;

	// 1. Get Item 
	S2DB::MakeQueryW( wstrSQL, L"usp_ShopItem_scan", 
		DB_DATA_TYPE_END );
	CADODatabase * pDataBase = m_pADOdb_DBMAIN_Shop; 

	if( FALSE == pDataBase->ExecuteSP( wstrSQL ) )
	{
		ErrorDBShop( MODULE_DB_SHOP_MAIN_THREAD_IDX, wstrSQL );
		return EVENT_ERROR_DATABASE_SP_RESULT;
	}

	i32Count = pDataBase->GetRecordCount();
	if( MAX_SHOP_ITEM_COUNT < i32Count )
	{
		i3String::Format( wstrLog, SERVER_STRING_COUNT, L"[ERROR] MAX_SHOP_ITEM_COUNT(%d) < %d ",MAX_SHOP_ITEM_COUNT,i32Count);
		_DataBaseResultFailLog( MODULE_DB_SHOP_MAIN_THREAD_IDX, wstrLog );
		i32Count = MAX_SHOP_ITEM_COUNT;
		//return EVENT_ERROR_DATABASE_SP_RESULT;
	}

	SHOP_ITEM_DATA	pTempShopData[MAX_SHOP_ITEM_COUNT];
	i3mem::FillZero(pTempShopData,	sizeof(SHOP_ITEM_DATA)*MAX_SHOP_ITEM_COUNT); 

	for(INT32 i = 0; i < i32Count; i++)
	{
		pTempShopData[i]._ui32ItemID	= pDataBase->GetUINT32Data(0);
		pTempShopData[i]._ui8AuthType		= pDataBase->GetUINT8Data(1); 
		pTempShopData[i]._ui8AuthExpireType	= pDataBase->GetUINT8Data(2);
		pTempShopData[i]._ui32Expire		= pDataBase->GetUINT32Data(3);
		pTempShopData[i]._ui8AutoStart	= pDataBase->GetUINT8Data(4);

		// AuthType, AuthExpireType, AutoStart 가 모두 0보다 크면 셋팅합니다.( 온전한 데이터 )
		if ( pTempShopData[i]._ui8AuthType > 0 && pTempShopData[i]._ui8AuthExpireType > 0 && pTempShopData[i]._ui8AutoStart > 0 )
		{
			pDataBase->GetTextData( 5, pTempShopData[i]._strName, MAX_ITEM_NAME );
			pTempShopData[i]._strName[ MAX_ITEM_NAME-1 ] = '\0';
			pTempShopData[i]._ui8Restriction	= pDataBase->GetUINT8Data(6);
			pTempShopData[i]._ui8RestrictionType= pDataBase->GetUINT8Data(7);
			pTempShopData[i]._ui16Condition		= pDataBase->GetUINT16Data(8);

			i3mem::Copy( &pShopData->m_pItem[i32RealCount], &pTempShopData[i], sizeof(SHOP_ITEM_DATA) );	

			pShopData->m_pItem[i32RealCount]._ui32RepairCash = 0;
			pShopData->m_pItem[i32RealCount]._ui32RepairPoint = 0;
			pShopData->m_pItem[i32RealCount]._ui32TermItemID = 0;
			pShopData->m_pItem[i32RealCount]._ui32TermAuthSec = 0;
			pShopData->m_pItem[i32RealCount]._ui32Durability = 0;

			i32RealCount++;
		}

		pDataBase->MoveNext();
	}

	pShopData->m_ui32ItemCount = i32RealCount;



	// 2. Get ItemAuth and Sync 
	i32RealCount = 0;
	i32Count = 0;

	S2DB::MakeQueryW( wstrSQL, L"usp_ShopTermAuth_scan", 
		DB_DATA_TYPE_END );
	if( FALSE == pDataBase->ExecuteSP( wstrSQL ) )
	{
		ErrorDBShop( MODULE_DB_SHOP_MAIN_THREAD_IDX, wstrSQL );
		return EVENT_ERROR_DATABASE_SP_RESULT;
	}

	i32Count = pDataBase->GetRecordCount();
	if( MAX_SHOP_ITEM_COUNT < i32Count )
	{
		i3String::Format( wstrLog, SERVER_STRING_COUNT, L"[ERROR] MAX_SHOP_ITEM_AUTH_COUNT(%d) < %d ",MAX_SHOP_ITEM_COUNT,i32Count);
		_DataBaseResultFailLog( MODULE_DB_SHOP_MAIN_THREAD_IDX, wstrLog );
		i32Count = MAX_SHOP_ITEM_COUNT;
		//return EVENT_ERROR_DATABASE_SP_RESULT;
	}

	UINT32 ui32TempItemID = 0;
	for(INT32 i = 0; i < i32Count; i++)
	{		
		ui32TempItemID		= pDataBase->GetUINT32Data(0);

		for( UINT32 k = 0 ; k < pShopData->m_ui32ItemCount ; k++ )
		{
			if( pShopData->m_pItem[k]._ui32ItemID != ui32TempItemID ) continue;

			pShopData->m_pItem[k]._ui32TermItemID  = pDataBase->GetUINT32Data(1);
			pShopData->m_pItem[k]._ui32TermAuthSec = pDataBase->GetUINT32Data(2);

			break;
		}
		pDataBase->MoveNext();
	}



	// 3. Get Repair and Sunc
	i32RealCount = 0;
	i32Count = 0;

	S2DB::MakeQueryW( wstrSQL, L"usp_ShopItemRepair_scan", 
		DB_DATA_TYPE_END );
	if( FALSE == pDataBase->ExecuteSP( wstrSQL ) )
	{
		ErrorDBShop( MODULE_DB_SHOP_MAIN_THREAD_IDX, wstrSQL);
		return EVENT_ERROR_DATABASE_SP_RESULT;
	}

	i32Count = pDataBase->GetRecordCount();
	if( MAX_SHOP_ITEM_COUNT < i32Count )
	{
		i3String::Format( wstrLog, SERVER_STRING_COUNT, L"[ERROR] MAX_SHOP_REPAIR_COUNT(%d) < %d ",MAX_SHOP_ITEM_COUNT,i32Count);
		_DataBaseResultFailLog( MODULE_DB_SHOP_MAIN_THREAD_IDX, wstrLog );
		i32Count = MAX_SHOP_ITEM_COUNT;
		//return EVENT_ERROR_DATABASE_SP_RESULT;
	}

	ui32TempItemID = 0;
	for(INT32 i = 0; i < i32Count; i++)
	{		
		pShopData->m_pRepair[i]._ui32ItemID		= pDataBase->GetUINT32Data(0);
		pShopData->m_pRepair[i]._ui32PricePoint	= pDataBase->GetUINT32Data(1);
		pShopData->m_pRepair[i]._ui32PriceCash	= pDataBase->GetUINT32Data(2);
		pShopData->m_pRepair[i]._ui32Durability	= pDataBase->GetUINT32Data(3);

		for( UINT32 k = 0 ; k < pShopData->m_ui32ItemCount ; k++ )
		{
			if( pShopData->m_pItem[k]._ui32ItemID != pShopData->m_pRepair[i]._ui32ItemID ) continue;

			pShopData->m_pItem[k]._ui32RepairPoint  = pShopData->m_pRepair[i]._ui32PricePoint;
			pShopData->m_pItem[k]._ui32RepairCash	= pShopData->m_pRepair[i]._ui32PriceCash;	
			pShopData->m_pItem[k]._ui32Durability	= pShopData->m_pRepair[i]._ui32Durability;

			break;
		}
		pDataBase->MoveNext();
	}

	pShopData->m_ui32RepairCount = i32Count;


	// 4. Get Goods
	i32RealCount = 0;
	i32Count = 0;

	S2DB::MakeQueryW( wstrSQL, L"usp_ShopGoods_scan", 
		DB_DATA_TYPE_END );
	if( FALSE == pDataBase->ExecuteSP( wstrSQL ) )
	{
		ErrorDBShop( MODULE_DB_SHOP_MAIN_THREAD_IDX, wstrSQL );
		return EVENT_ERROR_DATABASE_SP_RESULT;
	}

	i32Count = pDataBase->GetRecordCount();

	if( MAX_SHOP_GOODS_COUNT < i32Count )
	{
		i3String::Format( wstrLog, SERVER_STRING_COUNT, L"[ERROR] MAX_SHOP_GOODS_COUNT(%d) < %d ",MAX_SHOP_GOODS_COUNT,i32Count);
		_DataBaseResultFailLog( MODULE_DB_SHOP_MAIN_THREAD_IDX, wstrLog );
		i32Count = MAX_SHOP_GOODS_COUNT;
		//return EVENT_ERROR_DATABASE_SP_RESULT;
	}	

	SHOP_GOODS_DATA	pTempGoods[MAX_SHOP_GOODS_COUNT];
	i3mem::FillZero(pTempGoods,	sizeof(SHOP_GOODS_DATA)*MAX_SHOP_GOODS_COUNT); 
	for(INT32 i= 0; i < i32Count; i++)
	{
		pTempGoods[i]._ui32GoodsID				= pDataBase->GetUINT32Data(0);
		pTempGoods[i]._ui8Enable				= pDataBase->GetUINT8Data(1);

		if( GOODS_USE_ENABLE ==  pTempGoods[i]._ui8Enable )
		{
			pTempGoods[i]._ui8SaleType				= pDataBase->GetUINT8Data(2);
			
			pDataBase->GetTextData(3, pTempGoods[i]._strName, MAX_ITEM_NAME);
			pTempGoods[i]._strName[ MAX_ITEM_NAME-1 ] = '\0';
			pTempGoods[i]._ui32SaleCount			= 0;
			pTempGoods[i]._ui32GiftCount			= 0;

			i3mem::Copy( &pShopData->m_pGoods[i32RealCount], &pTempGoods[i] ,sizeof(SHOP_GOODS_DATA) );	

			i32RealCount++;
		}

		pDataBase->MoveNext();
	}

	pShopData->m_ui32GoodsCount = i32RealCount;



	// 5. Get Price
	i32Count = 0;

	S2DB::MakeQueryW( wstrSQL, L"usp_ShopGoodsPrice_scan", 
		DB_DATA_TYPE_END );
	if( FALSE == pDataBase->ExecuteSP( wstrSQL ) )
	{
		ErrorDBShop( MODULE_DB_SHOP_MAIN_THREAD_IDX, wstrSQL );
		return EVENT_ERROR_DATABASE_SP_RESULT;
	}

	i32Count = pDataBase->GetRecordCount();

	if( MAX_SHOP_PRICE_COUNT < i32Count )
	{
		i3String::Format( wstrLog, SERVER_STRING_COUNT, L"[ERROR] MAX_SHOP_PRICE_COUNT(%d) < %d ",MAX_SHOP_PRICE_COUNT,i32Count);
		_DataBaseResultFailLog( MODULE_DB_SHOP_MAIN_THREAD_IDX, wstrLog );
		i32Count = MAX_SHOP_PRICE_COUNT;
	}	

	for(INT32 i= 0; i < i32Count; i++)
	{
		pShopData->m_pPrice[i].m_ui32GoodsID	= pDataBase->GetUINT32Data(0);
		pShopData->m_pPrice[i].m_ui8PriceType	= pDataBase->GetUINT8Data(1);	
		pShopData->m_pPrice[i].m_ui32Point		= pDataBase->GetUINT32Data(2);	
		pShopData->m_pPrice[i].m_ui32Cash		= pDataBase->GetUINT32Data(3);	
		pShopData->m_pPrice[i].m_ui32Coin		= pDataBase->GetUINT32Data(4);	
		pShopData->m_pPrice[i].m_ui8Mark		= pDataBase->GetUINT8Data(5);	
		
 
		pDataBase->MoveNext();
	}

	pShopData->m_ui32PriceCount = i32Count;

	// 6. Get Matching
	i32RealCount = 0;
	i32Count = 0;

	S2DB::MakeQueryW( wstrSQL, L"usp_ShopMatching_scan", 
		DB_DATA_TYPE_END );
	if( FALSE == pDataBase->ExecuteSP( wstrSQL ) )
	{
		ErrorDBShop( MODULE_DB_SHOP_MAIN_THREAD_IDX, wstrSQL);
		return EVENT_ERROR_DATABASE_SP_RESULT;
	}

	i32Count = pDataBase->GetRecordCount();
	if( MAX_SHOP_MATCHING_COUNT < i32Count )
	{
		i3String::Format( wstrLog, SERVER_STRING_COUNT, L"[ERROR] MAX_SHOP_MATCHING_COUNT(%d) < %d ",MAX_SHOP_MATCHING_COUNT,i32Count);
		_DataBaseResultFailLog( MODULE_DB_SHOP_MAIN_THREAD_IDX, wstrLog );
		i32Count = MAX_SHOP_MATCHING_COUNT;
		//return EVENT_ERROR_DATABASE_SP_RESULT;
	}

	for(INT32 i = 0; i < i32Count; i++)
	{		
		pShopData->m_pMatching[i]._ui32GoodsID	= pDataBase->GetUINT32Data(0);
		pShopData->m_pMatching[i]._ui32ItemID	= pDataBase->GetUINT32Data(1);
		pShopData->m_pMatching[i]._ui32ItemArg	= pDataBase->GetUINT32Data(2);
		pShopData->m_pMatching[i]._ui32BonusArg	= pDataBase->GetUINT32Data(3);
		pDataBase->MoveNext();
	}

	pShopData->m_ui32MatchingCount = i32Count;

	return EVENT_ERROR_SUCCESS; 
}

void CModuleDBShop::_EXEC_GetItem(INT32 i32ThreadIdx, T_UID i64UID, DB_INVENTORY_OUT * pOut)
{
	CADODatabase * pDataBase = m_ppADOdb_DATABASE[i32ThreadIdx]; 
	wchar_t	wstrSQL[SERVER_STRING_COUNT];

	char strLog[MAX_PATH];

	INT32		i32DeleteItemCount = 0; 
	INVEN_BUFFER aDeleteItem[MAX_INVEN_COUNT];

	pOut->_ui16Count = 0; 

	S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_sel_All",
		DB_DATA_TYPE_INT,			i32ThreadIdx,
		DB_DATA_TYPE_BIGINT,		i64UID,
		DB_DATA_TYPE_END);
	DATABASE_EXECUTE_SP;

	INT32 i32Count = pDataBase->GetRecordCount();

	for(INT32 i = 0; i < i32Count ; i++)
	{
		pOut->_Inven[pOut->_ui16Count]._TItemWareDBIdx		= pDataBase->GetINT32Data(0);
		pOut->_Inven[pOut->_ui16Count]._ui32ItemID			= pDataBase->GetUINT32Data(1);
		pOut->_Inven[pOut->_ui16Count]._ui8ItemType			= pDataBase->GetUINT8Data(2);
		pOut->_Inven[pOut->_ui16Count]._ui32ItemArg			= pDataBase->GetUINT32Data(3);
		//20121015//pOut->_Inven[pOut->_ui16Count]._ui8ItemEquipSlotNum	= INIT_UINT8;

		pDataBase->MoveNext();

		SHOP_ITEM_DATA * pItem = g_pShop->GetItemByID( pOut->_Inven[pOut->_ui16Count]._ui32ItemID ); 
		if( NULL == pItem  )
		{
			continue; 
		}

		// 캐쉬 결제후 미지급된 아이템은 인벤토리 리스트에서 제외시킵니다.
		if( ITEM_ATTR_TYPE_STAND_BY == pOut->_Inven[pOut->_ui16Count]._ui8ItemType ) continue;

		if( ((pItem->_ui8AuthType == ITEM_TYPE_TIME) &&									//기간제 인지
			(pItem->_ui8AuthExpireType == ITEM_EXPIRE_DELETE) &&						//지워야 하는 속성인지
			(ITEM_ATTR_TYPE_AUTH == pOut->_Inven[pOut->_ui16Count]._ui8ItemType) &&		//인증이 된건지
			(i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI() > pOut->_Inven[pOut->_ui16Count]._ui32ItemArg)	) ||		//시간이 다 된건지
			((0 != pItem->_ui32Expire) &&												//아이템 강제 만료 시간이 되었는지
			(i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI() > pItem->_ui32Expire )))
		{//삭제한다.
			if( i32DeleteItemCount >= MAX_INVEN_COUNT)
			{
				char strLog[MAX_PATH];
				i3String::Format(strLog, MAX_PATH, "Error DeleteItmCount Over %I64d, %d", i64UID, pOut->_Inven[pOut->_ui16Count]._TItemWareDBIdx); 
				WRITE_SERVICE_LOG_DB(i32ThreadIdx, strLog); 
				continue;
			}
			aDeleteItem[i32DeleteItemCount]._TItemWareDBIdx = pOut->_Inven[pOut->_ui16Count]._TItemWareDBIdx;
			aDeleteItem[i32DeleteItemCount]._ui32ItemID		= pOut->_Inven[pOut->_ui16Count]._ui32ItemID;
			i32DeleteItemCount++;
			continue; 
		}

		pOut->_ui16Count++; 
		if(MAX_INVEN_COUNT <= pOut->_ui16Count ) break;
	}

	//Delete Item 
	pOut->_ui16DeleteCharaCount = 0;
	for(INT32 i = 0; i < i32DeleteItemCount; i++)
	{
		S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_del",
			DB_DATA_TYPE_INT,			i32ThreadIdx,
			DB_DATA_TYPE_BIGINT,		i64UID,
			DB_DATA_TYPE_INT,			aDeleteItem[i]._TItemWareDBIdx,
			DB_DATA_TYPE_END);
		DATABASE_EXECUTE_SP;	
		if( 1 != pDataBase->GetRecordCount() )
		{
			_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
		}

		if ( ITEM_TYPE_CHARA == GET_ITEM_TYPE(aDeleteItem[i]._ui32ItemID) )
		{
			pOut->_DeleteCharaDBIdx[pOut->_ui16DeleteCharaCount++] = aDeleteItem[i]._TItemWareDBIdx; 
			i3String::Format(strLog, MAX_PATH, "DELETE CHARA UID:%I64d ItemDBIdx:%d ItemID:%d", i64UID, aDeleteItem[i]._TItemWareDBIdx, aDeleteItem[i]._ui32ItemID); 
		}
		else 
		{
			i3String::Format(strLog, MAX_PATH, "DELETE UID:%I64d ItemDBIdx:%d ItemID:%d", i64UID, aDeleteItem[i]._TItemWareDBIdx, aDeleteItem[i]._ui32ItemID);
		}
		WRITE_SERVICE_LOG_DB(i32ThreadIdx, strLog);
	}

	pOut->_i32Rv = EVENT_ERROR_SUCCESS; 

	return; 
}

void CModuleDBShop::_EXEC_GiftInsert(INT32 i32ThreadIdx, SEND_GIFT * pGift )
{
	CADODatabase * pDataBase = m_ppADOdb_DATABASE[i32ThreadIdx]; 
	wchar_t wstrSQL[SERVER_STRING_COUNT];
	char strLog[SERVER_STRING_COUNT];
	TTCHAR strNewNick[CONVERT_STRING_COUNT];
	TTCHAR strNewMessage[CONVERT_STRING_COUNT];

	S2DB::ConvertString( strNewNick, CONVERT_STRING_COUNT, pGift->m_strSenderNick );
	S2DB::ConvertString( strNewMessage, CONVERT_STRING_COUNT, pGift->m_strMessage );
 
	for( UINT8 i= 0 ; i < pGift->m_ui8GiftCount ; ++i )
	{
		ENUM_TYPE_GIFT_DELIVER	eDeliver = TYPE_GIFT_DELIVER_NORMAL;
		if( GIFT_TYPE_USER == pGift->m_eGiftType  && GOODS_BUY_CASH == pGift->m_aGift[i].m_ui8BuyType ) 
		{	// 유저 선물 구매하기시 캐쉬 결제는 결제 대기상태로 INSERT 합니다. LINK 결제 성공후 UPDATE.
			eDeliver = TYPE_GIFT_DELIVER_STAND_BY;
		}

		// 선물 받는 사람의 Gift DB에 내용 Insert.
		S2DB::MakeQueryW( wstrSQL, L"usp_GiftBox_ins", 
			DB_DATA_TYPE_INT,		i32ThreadIdx, 
			DB_DATA_TYPE_BIGINT,	pGift->m_TReceiverUID, 
			DB_DATA_TYPE_INT,		pGift->m_aGift[i].m_ui32GoodsID, 
			DB_DATA_TYPE_INT,		eDeliver, 
			DB_DATA_TYPE_TTCHAR,	strNewNick, 
			DB_DATA_TYPE_TTCHAR,	strNewMessage,
			DB_DATA_TYPE_END);
		if( !pDataBase->ExecuteSP( wstrSQL ) )
		{
			ErrorDBShop( i32ThreadIdx, wstrSQL);
			pGift->m_aGift[i].m_i32Rv = EVENT_ERROR_DATABASE_EXECUTE_SP;
			continue;
		}
		if( 1 != pDataBase->GetRecordCount() )
		{
			_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
			pGift->m_aGift[i].m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT; 
			continue;
		}
		if( 1 != pDataBase->GetINT32Data(0))
		{
			_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
			pGift->m_aGift[i].m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
			continue;
		}

		pGift->m_aGift[i].m_i32Rv		= EVENT_ERROR_SUCCESS;
		pGift->m_aGift[i].m_TGiftDBIdx	= pDataBase->GetINT32Data(1);

		// Log 기록
		i3String::Format( strLog, SERVER_STRING_COUNT, "BUY GIFT BuyType:%d, SenderUID:%I64d, ReciverUID:%I64d, GoodsID:%d, GiftDBIdx:%d", 
		pGift->m_aGift[i].m_ui8BuyType,	pGift->m_TSenderUID, pGift->m_TReceiverUID, pGift->m_aGift[i].m_ui32GoodsID, pGift->m_aGift[i].m_TGiftDBIdx );
		WRITE_SERVICE_LOG_DB(i32ThreadIdx, strLog); 
	}
	return; 
}

INT32 CModuleDBShop::_EXEC_CheckGiftList( INT32 i32ThreadIdx, T_UID i64UID, bool& bGiftAutoAuth )
{
	CADODatabase * pDataBase = m_ppADOdb_DATABASE[i32ThreadIdx]; 
	wchar_t	wstrSQL[SERVER_STRING_COUNT];

	// 임시 선물 정보
	S2MO_GIFT		GiftBuffer[ MAX_SHOP_USER_GIFT_COUNT_TEMP ] = { 0,};	// DB에 100개 초과할 경우 생각해서
	INT32			i32TotalGiftCount	=  0;	// 총 선물 갯수
	UINT32			m_ui32NowTime = i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();

	// 1. DB SELECT
	S2DB::MakeQueryW( wstrSQL, L"usp_GiftBox_sel_All",
		DB_DATA_TYPE_INT,			i32ThreadIdx,
		DB_DATA_TYPE_BIGINT,		i64UID,
		DB_DATA_TYPE_END);
	if( !pDataBase->ExecuteSP( wstrSQL ) )
	{
		ErrorDBShop( i32ThreadIdx, wstrSQL );
		return EVENT_ERROR_DATABASE_EXECUTE_SP;
	};

	INT32 i32DBCount = pDataBase->GetRecordCount(); 
	i32DBCount = MIN( i32DBCount, MAX_SHOP_USER_GIFT_COUNT_TEMP );	// 최대값 설정.

	// 최근에 받은 선물부터 조회됩니다. ORDER BY i32Idx DESC
	for(INT32 i = 0; i < i32DBCount ; ++i )
	{
		GiftBuffer[i32TotalGiftCount].m_TDBIdx		=	pDataBase->GetINT32Data(0);
		GiftBuffer[i32TotalGiftCount].m_TGoodsID	=	pDataBase->GetUINT32Data(1);
		GiftBuffer[i32TotalGiftCount].m_ui8Deliver	=	pDataBase->GetUINT8Data(2);

		if( TYPE_GIFT_DELIVER_STAND_BY == (ENUM_TYPE_GIFT_DELIVER) GiftBuffer[i32TotalGiftCount].m_ui8Deliver )	
		{	// 결제 대기 상태의 선물은 리스트에서 제외 시킵니다.
			pDataBase->MoveNext();
			continue;
		}

		DATE dtExpire										=	pDataBase->GetDateData(3);
		GiftBuffer[i32TotalGiftCount].m_ui32Expire			=	S2DB::ConvertSmallDateToUINT32(dtExpire);

		pDataBase->GetTextData( 4, GiftBuffer[i32TotalGiftCount].m_strSenderNick, MAX_USER_NICK);
		GiftBuffer[i32TotalGiftCount].m_strSenderNick[MAX_USER_NICK-1] ='\0';
		pDataBase->GetTextData( 5, GiftBuffer[i32TotalGiftCount].m_strMessage, MAX_GIFT_MESSAGE_SIZE);
		GiftBuffer[i32TotalGiftCount].m_strMessage[MAX_GIFT_MESSAGE_SIZE-1] ='\0';

		++i32TotalGiftCount;

		pDataBase->MoveNext();
	}

	DB_ITEM stDBItem; 
	stDBItem.m_ui8InsertType = ITEM_INSERT_REQUEST_GIFT_AUTH; 

	// 2. 100개 초과시 지급된것은 삭제, 지급안된것은 지급후 삭제. 
	for( INT32 i32OverIdx = MAX_SHOP_USER_GIFT_COUNT ; i32OverIdx < i32TotalGiftCount   ; ++i32OverIdx )
	{
		INT32 i32Rv = _GiftAutoAuth(i32ThreadIdx, i64UID, &stDBItem, &GiftBuffer[i32OverIdx], bGiftAutoAuth, TYPE_GIFT_DELIVER_OVERFLOW );
		if( EV_FAILED( i32Rv ) )
		{
			g_pLog->WriteLog( L"Fail Gift Auto Auth 1 / UID : %I64d / GiftDBIdx : %d / GoodsID : %d / Reason : %x ",
				i64UID, GiftBuffer[i32OverIdx].m_TDBIdx , GiftBuffer[i32OverIdx].m_TGoodsID, i32Rv ); 
			continue;
		}
	}

	i32TotalGiftCount = MIN( i32TotalGiftCount, MAX_SHOP_USER_GIFT_COUNT );	// 최대값 재설정.

	// 3. 만료기간이 지난 선물중 지급된것은 삭제, 지급안된것은 지급후 삭제. 
	for( INT32 i32GiftIdx = 0 ; i32GiftIdx < i32TotalGiftCount ; ++i32GiftIdx )
	{
		// 만료기간이 지나지 않았다면 continue.
		if( m_ui32NowTime < GiftBuffer[i32GiftIdx].m_ui32Expire ) continue;	

		INT32 i32Rv = _GiftAutoAuth(i32ThreadIdx, i64UID, &stDBItem, &GiftBuffer[i32GiftIdx], bGiftAutoAuth, TYPE_GIFT_DELIVER_EXPIRE );
		if( EV_FAILED( i32Rv ) )
		{
			g_pLog->WriteLog( L"Fail Gift Auto Auth 2 / UID : %I64d / GiftDBIdx : %d / GoodsID : %d / Reason : %x ",
				i64UID, GiftBuffer[i32GiftIdx].m_TDBIdx , GiftBuffer[i32GiftIdx].m_TGoodsID, i32Rv ); 
			continue;
		}
	}

	return EVENT_ERROR_SUCCESS; 
}

void CModuleDBShop::_EXEC_GetGiftList( INT32 i32ThreadIdx,  DB_GIFT_OUT * pOut)
{
	CADODatabase * pDataBase = m_ppADOdb_DATABASE[i32ThreadIdx]; 
	wchar_t	wstrSQL[SERVER_STRING_COUNT];

	// 1. DB SELECT
	S2DB::MakeQueryW( wstrSQL, L"usp_GiftBox_sel_All",
		DB_DATA_TYPE_INT,			i32ThreadIdx,
		DB_DATA_TYPE_BIGINT,		 pOut->m_TUID,
		DB_DATA_TYPE_END);
	DATABASE_EXECUTE_SP;

	INT32 i32DBCount = pDataBase->GetRecordCount(); 
	i32DBCount = MIN( i32DBCount, MAX_SHOP_USER_GIFT_COUNT );	// 최대값 설정.

	pOut->m_ui8GiftCount = 0;	// 클라이언트에 내려줄 실제 선물 갯수

	// 최근에 받은 선물부터 조회됩니다. ORDER BY i32Idx DESC
	for(INT32 i = 0; i < i32DBCount ; ++i )
	{
		pOut->m_Gift[pOut->m_ui8GiftCount].m_TDBIdx		=	pDataBase->GetINT32Data(0);
		pOut->m_Gift[pOut->m_ui8GiftCount].m_TGoodsID	=	pDataBase->GetUINT32Data(1);

		SHOP_GOODS_LIST*	pGoods = g_pShop->GetGoodsByID( pOut->m_Gift[pOut->m_ui8GiftCount].m_TGoodsID );	
		if(	NULL == pGoods )
		{	// 유효하지 않은 GoodsID라면 리스트에서 제외.
			goto NEXT_GIFT; 
		}

		pOut->m_Gift[pOut->m_ui8GiftCount].m_ui8Deliver	=	(ENUM_TYPE_GIFT_DELIVER)pDataBase->GetUINT8Data(2);

		if( TYPE_GIFT_DELIVER_STAND_BY == pOut->m_Gift[pOut->m_ui8GiftCount].m_ui8Deliver )	
		{	// 결제 대기 상태면 리스트에서 제외.
			goto NEXT_GIFT; 
		}

		DATE dtExpire										=	pDataBase->GetDateData(3);
		pOut->m_Gift[pOut->m_ui8GiftCount].m_ui32Expire			=	S2DB::ConvertSmallDateToUINT32(dtExpire);

		pDataBase->GetTextData( 4, pOut->m_Gift[pOut->m_ui8GiftCount].m_strSenderNick, MAX_USER_NICK);
		pOut->m_Gift[pOut->m_ui8GiftCount].m_strSenderNick[MAX_USER_NICK-1] ='\0';
		pDataBase->GetTextData( 5, pOut->m_Gift[pOut->m_ui8GiftCount].m_strMessage, MAX_GIFT_MESSAGE_SIZE);
		pOut->m_Gift[pOut->m_ui8GiftCount].m_strMessage[MAX_GIFT_MESSAGE_SIZE-1] ='\0';

		++pOut->m_ui8GiftCount;

NEXT_GIFT:

		pDataBase->MoveNext();
	}

	pOut->_i32Rv = EVENT_ERROR_SUCCESS; 

	return; 
}

// 캐쉬 구매시 DB 체크용으로 STAND BY 로우를 무조건 INSERT합니다.
void CModuleDBShop::_EXEC_ShopBuy(INT32 i32ThreadIdx, DB_SHOP_GOODS * pIn)
{	
	SHOP_ITEM_DATA	*pItem		= NULL;
	CADODatabase	* pDataBase	= m_ppADOdb_DATABASE[i32ThreadIdx];
	wchar_t wstrSQL[SERVER_STRING_COUNT];
	char strLog[MAX_PATH];

	pIn->m_ui8GoodsCount = MIN( pIn->m_ui8GoodsCount, MAX_SHOP_GOODS_BUY_COUNT ); 

	for( UINT8 GoodsLoop = 0 ; GoodsLoop < pIn->m_ui8GoodsCount ; GoodsLoop++ )
	{
		DB_GOODS *pGoods = &pIn->m_stDBGoods[GoodsLoop];
		pGoods->m_i32Rv				= EVENT_ERROR_DATABASE;
		for( UINT8 ItemLoop = 0 ; ItemLoop < pGoods->m_stDBItem.m_ui8ItemCount ; ItemLoop++ )	
		{
			INVEN_BUFFER_DB *pstDBItem = &pIn->m_stDBGoods[GoodsLoop].m_stDBItem.m_aItem[ItemLoop];
			pItem		= g_pShop->GetItemByID( pstDBItem->_ui32ItemID );
			if( NULL == pItem )
			{	 
				pGoods->m_i32Rv = EVENT_ERROR_EVENT_INSERT_ITEM_VALID_ITEM_ID;
				goto NEXT_GOODS;
			}

			pstDBItem->_ui8ItemType = ITEM_ATTR_TYPE_STAND_BY;

			S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_ins",
				DB_DATA_TYPE_INT,		i32ThreadIdx,
				DB_DATA_TYPE_BIGINT,	pIn->m_TUID,
				DB_DATA_TYPE_INT,		pstDBItem->_ui32ItemID,
				DB_DATA_TYPE_TINYINT,	pstDBItem->_ui8ItemType,
				DB_DATA_TYPE_INT,		pstDBItem->_ui32AddItemArg,
				DB_DATA_TYPE_END );
			DATABASE_EXECUTE_SP_GOODS;
			if( 1 != pDataBase->GetRecordCount() )
			{
				_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
				pGoods->m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
				goto NEXT_GOODS;
			}
			if( 1 != pDataBase->GetINT32Data(0))
			{
				_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
				pGoods->m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
				goto NEXT_GOODS;
			}

			S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_sel_ItemIdx",
				DB_DATA_TYPE_INT,			i32ThreadIdx,
				DB_DATA_TYPE_BIGINT,		pIn->m_TUID,
				DB_DATA_TYPE_INT,			pstDBItem->_ui32ItemID,
				DB_DATA_TYPE_END);
			DATABASE_EXECUTE_SP_GOODS;

			INT32 i32Count = pDataBase->GetRecordCount();
			if( 1 > i32Count )
			{
				_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
				pGoods->m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
				goto NEXT_GOODS;
			}

			pstDBItem->_TItemWareDBIdx = pDataBase->GetINT32Data(0);
			
			i3String::Format( strLog, MAX_PATH, "SHOP BUY INSERT STAND BY / CASH /  UID:%I64d GoodsID:%d ItemID:%d ItemArg:%d DBIdx:%d", 
				pIn->m_TUID, pGoods->m_TGoodsID, pstDBItem->_ui32ItemID, pstDBItem->_ui32AddItemArg, pstDBItem->_TItemWareDBIdx );
			WRITE_SERVICE_LOG_DB(i32ThreadIdx, strLog);

		}	// ItemEnd

		pGoods->m_i32Rv				= EVENT_ERROR_SUCCESS;

NEXT_GOODS:
		;
	}	// Goods End


	return; 
}

void CModuleDBShop::_EXEC_ShopBuyLink(INT32 i32ThreadIdx, DB_SHOP_GOODS * pIn)
{
	T_ItemDBIdx	_TItemDBIdx;
	UINT8		_ItemState;
	UINT32		_AuthArg;
	UINT32		_NewAuthArg;
	INT32		i32DeleteRowRv;
	SHOP_ITEM_DATA	*pItem		= NULL;

	pIn->m_stChara.m_ui8SlotState = MULTI_SLOT_STATE_NORMAL;
	pIn->m_stChara.m_bAddCharaSlot	= false;
		
	CADODatabase	* pDataBase	= m_ppADOdb_DATABASE[i32ThreadIdx]; 
	wchar_t wstrSQL[SERVER_STRING_COUNT];
	char strLog[MAX_PATH];

	pIn->m_ui8GoodsCount = MIN( pIn->m_ui8GoodsCount, MAX_SHOP_GOODS_BUY_COUNT );  

	for( UINT8 GoodsLoop = 0 ; GoodsLoop < pIn->m_ui8GoodsCount ; GoodsLoop++ ) 
	{
		DB_GOODS *pGoods = &pIn->m_stDBGoods[GoodsLoop];
		if( NULL == pGoods ) continue;

		for( UINT8 ItemLoop = 0 ; ItemLoop < pGoods->m_stDBItem.m_ui8ItemCount ; ItemLoop++ ) 
		{
			INVEN_BUFFER_DB *pstDBItem = &pIn->m_stDBGoods[GoodsLoop].m_stDBItem.m_aItem[ItemLoop];
			if( NULL == pstDBItem ) continue;

			// Link 결제 실패라면 STAND BY 로우를 DELETE 합니다.
			if( EV_FAILED(pGoods->m_i32Rv) )
			{ 	
				i32DeleteRowRv = _EXEC_DeleteRow( i32ThreadIdx, pIn->m_TUID, pstDBItem->_TItemWareDBIdx );
				if( EV_FAILED( i32DeleteRowRv ) )
				{	// 여기서 DB 작업이 실패하면, 추후 보상 대상에서 제외시켜야 하므로 로그 남깁니다.
					i3String::Format( strLog, MAX_PATH, "SHOP BUY DELETE FAIL(LINK FAIL) / NO COMPENSATION / UID:%I64d Action:%d ItemID:%d ItemArg%d DBIdx:%d ", 
						pIn->m_TUID, pIn->m_ui8JobAction, pstDBItem->_ui32ItemID , pstDBItem->_ui32AddItemArg, pstDBItem->_TItemWareDBIdx );
					WRITE_SERVICE_LOG_DB(i32ThreadIdx, strLog);
				} 
				continue;
			}

			pItem		= g_pShop->GetItemByID( pstDBItem->_ui32ItemID );
			if( NULL == pItem )
			{	 
				pGoods->m_i32Rv = EVENT_ERROR_EVENT_INSERT_ITEM_VALID_ITEM_ID;
				goto NEXT_GOODS;
			}

			// 파츠 예외처리.
			BOOL bPartsExtend = FALSE;
			if( g_pConfig->IsPartsByItemID( pstDBItem->_ui32ItemID ) )
			{
				switch( pIn->m_ui8JobAction ) 
				{
				case TYPE_SHOP_ITEM_EXTEND :
					// 파츠기간 연장은 기존 로우 UPDATE 후 STAND BY 로우 DELETE 합니다.
					bPartsExtend = TRUE;
					break;
				default:	
					// 파츠구매는 중복소지가 가능하므로 무조건 STAND BY 로우를 UPDATE 합니다.
					goto INSERT_FLAG;	 
					break;
				}
			}

			// 기간 연장 요청한 Parts 라면 최초 요청했던 인벤토리 DB Idx를 검색합니다.
			if( bPartsExtend )
			{
				S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_sel_ItemExtend",
					DB_DATA_TYPE_INT,			i32ThreadIdx,
					DB_DATA_TYPE_BIGINT,		pIn->m_TUID,
					DB_DATA_TYPE_INT,			pIn->m_TItemDBIdx,
					DB_DATA_TYPE_INT,			pstDBItem->_ui32ItemID,
					DB_DATA_TYPE_END);
			}			 
			//  있는지 검색. 단, ItemState가  ITEM_ATTR_TYPE_STAND_BY인 로우는 제외.
			else
			{
				S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_sel_checkItem",
				DB_DATA_TYPE_INT,			i32ThreadIdx,
				DB_DATA_TYPE_BIGINT,		pIn->m_TUID,
				DB_DATA_TYPE_INT,			pstDBItem->_ui32ItemID,
				DB_DATA_TYPE_TINYINT,		ITEM_ATTR_TYPE_STAND_BY,
				DB_DATA_TYPE_END);
				
			}
			
			DATABASE_EXECUTE_SP_GOODS;
	
			if( pDataBase->GetRecordCount() > 0) 
			{//있음. 
				_TItemDBIdx	= pDataBase->GetINT32Data(0);	
				_ItemState  = pDataBase->GetUINT8Data(1);
				_AuthArg	= pDataBase->GetUINT32Data(2); 

				if (ITEM_TYPE_CHARA == GET_ITEM_TYPE(pItem->_ui32ItemID) && ITEM_ATTR_TYPE_BASIC == _ItemState && ITEM_ATTR_TYPE_BASIC == pItem->_ui8AuthType)
				{	// 기존에 소지한 캐릭터가 영구제이면 INSERT
					goto INSERT_FLAG;	// 기존에 있으나 없으나 상관없이 INSERT.
				}
				
				// 기존 로우 UPDATE. 
				if( ITEM_EXPIRE_LEAVE == pItem->_ui8AuthExpireType  ||  ITEM_ATTR_TYPE_BASIC == _ItemState || ITEM_ATTR_TYPE_BASIC == pItem->_ui8AuthType )
				{	// 기존에 소지한 아이템이 내구도 이거나 영구제이면 실패 처리 한다.
					pGoods->m_i32Rv	= EVENT_ERROR_SHOP_ITEM_TYPE_ERROR;
					goto NEXT_GOODS;
				}

				switch( pItem->_ui8AuthType )
				{
				case ITEM_TYPE_COUNT: //카운트 
					_NewAuthArg = CALCULATE_ITEM_COUNT(_AuthArg, pstDBItem->_ui32AddItemArg);
					break;
				case ITEM_TYPE_TIME: //기간
					if(_ItemState == ITEM_ATTR_TYPE_BUY)			_NewAuthArg = CALCULATE_ITEM_COUNT( _AuthArg, pstDBItem->_ui32AddItemArg);							// 아직 인증이 안된 상태
					else if( i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI() > _AuthArg )	_NewAuthArg = g_pConfig->GetAuthTime( pstDBItem->_ui32AddItemArg );				// 인증은 되었지만 기간이 지난상태
					else											_NewAuthArg = g_pConfig->GetAddAuthTime(_AuthArg, pstDBItem->_ui32AddItemArg );	// 현재 인증중
					break;
				default				: //Error
					pGoods->m_i32Rv	= EVENT_ERROR_SHOP_ITEM_TYPE_ERROR;
					goto NEXT_GOODS;
					break; 
				}

				S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_upd_ItemAuth",
					DB_DATA_TYPE_INT,			i32ThreadIdx,
					DB_DATA_TYPE_INT,			_NewAuthArg,
					DB_DATA_TYPE_BIGINT,		pIn->m_TUID,
					DB_DATA_TYPE_INT,			_TItemDBIdx,
					DB_DATA_TYPE_END);
				DATABASE_EXECUTE_SP_GOODS;
				if( 1 != pDataBase->GetRecordCount() )
				{
					_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
					pGoods->m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
					goto NEXT_GOODS;
				}
				if( 1 != pDataBase->GetINT32Data(0))
				{
					_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
					pGoods->m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
					goto NEXT_GOODS;
				}	
				
				// STAND BY 로우 DELETE, 
				_EXEC_DeleteRow( i32ThreadIdx,  pIn->m_TUID, pstDBItem->_TItemWareDBIdx ) ;
			}
			else	// 파츠구매 or 기존에 없으면 STAND BY 로우 UPDATE.
			{	
INSERT_FLAG:	
				_ItemState = ITEM_ATTR_TYPE_BUY;
				if( ITEM_TYPE_ETERNITY == pItem->_ui8AuthType	)
				{	// 영구제 아이템일 경우 타입을 변경해 줍니다.
					_ItemState = ITEM_ATTR_TYPE_BASIC;
					pstDBItem->_ui32AddItemArg = 1;
				}

				S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_upd_ItemState",
					DB_DATA_TYPE_INT,			i32ThreadIdx,
					DB_DATA_TYPE_BIGINT,		pIn->m_TUID,
					DB_DATA_TYPE_INT,			pstDBItem->_TItemWareDBIdx,
					DB_DATA_TYPE_TINYINT,		_ItemState,
					DB_DATA_TYPE_END);
				DATABASE_EXECUTE_SP_GOODS;
				if( 1 != pDataBase->GetRecordCount() )
				{
					_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
					pGoods->m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
					goto NEXT_GOODS;
				}
				if( 1 != pDataBase->GetINT32Data(0))
				{
					_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
					pGoods->m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
					goto NEXT_GOODS;
				}

				S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_sel_ItemIdx",
				DB_DATA_TYPE_INT,			i32ThreadIdx,
				DB_DATA_TYPE_BIGINT,		pIn->m_TUID,
				DB_DATA_TYPE_INT,			pstDBItem->_ui32ItemID,
				DB_DATA_TYPE_END);
				DATABASE_EXECUTE_SP_GOODS;
				if( 1 > pDataBase->GetRecordCount() )
				{
					_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
					pGoods->m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
					goto NEXT_GOODS;
				}
				_TItemDBIdx	= pstDBItem->_TItemWareDBIdx;	
				_NewAuthArg = pstDBItem->_ui32AddItemArg;

				if (ITEM_TYPE_TIME == pItem->_ui8AuthType)
				{	// 기간제 캐릭터 구매시에는 캐릭터를 미사용 상태로 전달합니다.
					pIn->m_stChara.m_ui8SlotState = MULTI_SLOT_STATE_UNUSE;
				}

				pIn->m_stChara.m_bAddCharaSlot = true;
			}

			if( TYPE_SHOP_ITEM_EXTEND == pItem->_ui8AuthType ) i3String::Format( strLog, MAX_PATH, "ITEM EXTEND / BUY CASH / UID:%I64d GoodsID:%d ItemID:%d ItemArg:%d DBIdx:%d",  pIn->m_TUID, pGoods->m_TGoodsID, pstDBItem->_ui32ItemID, _NewAuthArg, _TItemDBIdx);
			else i3String::Format( strLog, MAX_PATH, "ITEM INSERT / BUY CASH / UID:%I64d GoodsID:%d ItemID:%d ItemArg:%d DBIdx:%d",  pIn->m_TUID, pGoods->m_TGoodsID, pstDBItem->_ui32ItemID, _NewAuthArg, _TItemDBIdx);
			WRITE_SERVICE_LOG_DB(i32ThreadIdx, strLog);

			// pIn에 결과값을 담습니다.
			pstDBItem->_TItemWareDBIdx		= _TItemDBIdx;
			pstDBItem->_ui8ItemType			= _ItemState;
			pstDBItem->_ui32ItemArg			= _NewAuthArg;
		}	// ItemEnd

		// 주석이 맞음
		//pGoods->m_i32Rv				= EVENT_ERROR_SUCCESS;

NEXT_GOODS:
		;
	}	// Goods End

	

	return; 
}

void CModuleDBShop::_EXEC_ItemDelete(INT32 i32ThreadIdx, DB_SHOP_IN * pIn, DB_SHOP_OUT * pOut)
{	
	CADODatabase * pDataBase = m_ppADOdb_DATABASE[i32ThreadIdx]; 
	wchar_t wstrSQL[SERVER_STRING_COUNT]; 

	char strLog[MAX_PATH];

	pOut->_i32Rv			= EVENT_ERROR_SUCCESS;
	pOut->_i64UID			= pIn->_i64UID;
	pOut->_TItemWareDBIdx	= pIn->_TWareDBIdx;
	pOut->_i8SlotIdx		= pIn->_i8SlotIdx;

	S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_del",
			DB_DATA_TYPE_INT,			i32ThreadIdx,
			DB_DATA_TYPE_BIGINT,		pIn->_i64UID,
			DB_DATA_TYPE_INT,			pIn->_TWareDBIdx,
			DB_DATA_TYPE_END);
	DATABASE_EXECUTE_SP;
	if( 1 != pDataBase->GetRecordCount() )
	{
		_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
		pOut->_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
		return;
	}
	if( 1 != pDataBase->GetINT32Data(0))
	{
		_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
		pOut->_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
		return;
	}

	if (pIn->_ui8InsertType != TYPE_SHOP_ITEM_DELETE_NOACK)
	{
		i3String::Format(strLog, MAX_PATH, "DELETE %I64d %d", pIn->_i64UID, pIn->_TWareDBIdx);
		WRITE_SERVICE_LOG_DB(i32ThreadIdx, strLog);
	}

	return; 
}
 
void CModuleDBShop::_EXEC_ItemAuth(INT32 i32ThreadIdx, DB_SHOP_IN * pIn, DB_SHOP_OUT * pOut)
{
	CADODatabase * pDataBase = m_ppADOdb_DATABASE[i32ThreadIdx]; 
	wchar_t wstrSQL[SERVER_STRING_COUNT]; 

	S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_sel_AuthArg",
		DB_DATA_TYPE_INT,			i32ThreadIdx,
		DB_DATA_TYPE_BIGINT,		pIn->_i64UID,
		DB_DATA_TYPE_INT,			pIn->_TWareDBIdx,
		DB_DATA_TYPE_END);
	DATABASE_EXECUTE_SP;

	T_UID	_UID		= 0;
	UINT32	_ItemID		= 0;
	UINT8	_ItemState	= 0; 
	UINT32	_AuthArg	= 0;

	UINT32	ui32NewArg	= 0;

	pOut->_TItemWareDBIdx	= pIn->_TWareDBIdx;		//Set DB IDX
	pOut->_i8SlotIdx		= pIn->_i8SlotIdx;			

	if( pDataBase->GetRecordCount() != 1)			//지금 구조에서 하나가 아니면 버그 입니다. 
	{
		pOut->_i32Rv = EVENT_FAIL_DATABASE_AUTH; 
		return; 
	}

	_UID		= pDataBase->GetINT64Data(0);
	_ItemID		= pDataBase->GetINT32Data(1);
	_ItemState	= pDataBase->GetINT8Data(2);
	_AuthArg	= pDataBase->GetUINT32Data(3);

	// 이곳에 기존 아이디 정보를 담아둡니다.
	pOut->_ui32TempItem = _ItemID;

	if( ( _UID != pIn->_i64UID ) || (0 == _ItemID) || ( 0 == _ItemState ) || ( 0 == _AuthArg ) )		// 몇가지 에러들입니다. 실패처리 합니다.
	{
		pOut->_i32Rv = EVENT_ERROR_EVENT_AUTH_FAIL; 
		return; 
	}
	// 이미 인증이 된 아이템으므로 인증 할 수 없습니다.
	if( ITEM_ATTR_TYPE_AUTH == _ItemState )
	{
		pOut->_i32Rv = EVENT_ERROR_EVENT_AUTH_START_ALREADY_AUTH; 
		return; 
	}

	SHOP_ITEM_DATA * pItem = g_pShop->GetItemByID( _ItemID );
	if( NULL == pItem )
	{	// 나올 수 없습니다.
		pOut->_i32Rv = EVENT_ERROR_EVENT_INSERT_ITEM_VALID_ITEM_ID; 
		return;
	}

	// 수동인증이 아니면 인증 할 수 없습니다.
	if( ITEM_AUTH_HAND != pItem->_ui8AutoStart )
	{
		pOut->_i32Rv = EVENT_ERROR_EVENT_AUTH_FAIL; 
		return;
	}

	switch( pItem->_ui8AuthType )
	{
	case ITEM_TYPE_COUNT: //카운트 
		{//갯수 만큼 증감 한다.
			ui32NewArg = 0;
			if( _AuthArg < pIn->_ui32ItemArg )
			{
				ui32NewArg = 0;

				// 이런경우가 나오는지 로그한번 봅시다.
				g_pLog->WriteLog( L"[DB_SHOP_ERROR]_EXEC_ItemAuth( Arg: %d, Use: %d )", _AuthArg, pIn->_ui32ItemArg );
			}
			else
			{
				ui32NewArg = _AuthArg - pIn->_ui32ItemArg;
			}

			if( ui32NewArg <= 0 && ITEM_EXPIRE_DELETE == pItem->_ui8AuthExpireType )
			{//Delete 
				S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_del",
				DB_DATA_TYPE_INT,			i32ThreadIdx,
				DB_DATA_TYPE_BIGINT,		pIn->_i64UID,
				DB_DATA_TYPE_INT,			pIn->_TWareDBIdx,
				DB_DATA_TYPE_END);
				pOut->_ui32ItemID	= 0;
			}
			else 
			{//Update 
				S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_upd_ItemAuth",
					DB_DATA_TYPE_INT,			i32ThreadIdx,
					DB_DATA_TYPE_INT,			ui32NewArg,
					DB_DATA_TYPE_BIGINT,		pIn->_i64UID,
					DB_DATA_TYPE_INT,			pIn->_TWareDBIdx,
					DB_DATA_TYPE_END);
				pOut->_ui32ItemID = _ItemID;
			}
			DATABASE_EXECUTE_SP;
			if( 1 != pDataBase->GetRecordCount() )
			{
				_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
				pOut->_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
				return;
			}
			if( 1 != pDataBase->GetINT32Data(0))
			{
				_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
				pOut->_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
				return;
			}

			pOut->_ui8ItemState	= ITEM_ATTR_TYPE_BUY; 
			pOut->_ui32AuthSec	= ui32NewArg;
		}
		break; 
	case ITEM_TYPE_TIME: //기간 
		{//DB에 만료 시간으로 AuthSec을 만료 시간으로 한다.
			ui32NewArg = g_pConfig->GetAuthTime( _AuthArg ) ;//타임 변환 
			S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_upd_ItemExpireTime",
				DB_DATA_TYPE_INT,			i32ThreadIdx,
				DB_DATA_TYPE_TINYINT,		ITEM_ATTR_TYPE_AUTH,
				DB_DATA_TYPE_INT,			ui32NewArg,
				DB_DATA_TYPE_BIGINT,		pIn->_i64UID,
				DB_DATA_TYPE_INT,			pIn->_TWareDBIdx,
				DB_DATA_TYPE_END);
			DATABASE_EXECUTE_SP;
			if( 1 != pDataBase->GetRecordCount() )
			{
				_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
				pOut->_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
				return;
			}
			if( 1 != pDataBase->GetINT32Data(0))
			{
				_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
				pOut->_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
				return;
			}

			pOut->_ui32ItemID	= _ItemID;
			pOut->_ui8ItemState = ITEM_ATTR_TYPE_AUTH; 
			pOut->_ui32AuthSec	= ui32NewArg;
		}
		break; 
	case ITEM_TYPE_ETERNITY:	// 영구
		{	// 영구 아이템이므로 인증 할 수 없습니다.
			pOut->_i32Rv = EVENT_ERROR_EVENT_AUTH_FAIL;
			return;
		}
		break;
	default: break; 
	}

	//자동으로 연산된거 말고 유저가 요청한것만 처리합니다. 
	if(pIn->_ui8InsertType != TYPE_SHOP_ITEM_AUTH_NOACK )
	{
		char strLog[MAX_PATH];
		i3String::Format( strLog, MAX_PATH, "AUTH %I64d %d, %d (StandardTime)", pIn->_i64UID, pIn->_TWareDBIdx, ui32NewArg);//int32->int64		
		WRITE_SERVICE_LOG_DB(i32ThreadIdx, strLog); 
	}

	pOut->_i32Rv = EVENT_ERROR_SUCCESS;
	return; 
}

void CModuleDBShop::_EXEC_ItemLoopAuth(INT32 i32ThreadIdx, DB_SHOP_IN * pIn, DB_SHOP_OUT * pOut)
{
	CADODatabase * pDataBase = m_ppADOdb_DATABASE[i32ThreadIdx];
	wchar_t wstrSQL[SERVER_STRING_COUNT];

	S2DB::MakeQueryW(wstrSQL, L"usp_Inventory_sel_AuthArg",
		DB_DATA_TYPE_INT,		i32ThreadIdx,
		DB_DATA_TYPE_BIGINT,	pIn->_i64UID,
		DB_DATA_TYPE_INT,		pIn->_TWareDBIdx,
		DB_DATA_TYPE_END);
	DATABASE_EXECUTE_SP;

	T_UID	_UID			= 0;
	UINT32	_ItemID			= 0;
	UINT8	_ItemState		= 0;
	UINT32	_AuthArg		= 0;

	UINT32	ui32NewArg		= 0;

	pOut->_TItemWareDBIdx = pIn->_TWareDBIdx;		//Set DB IDX
	pOut->_i8SlotIdx = pIn->_i8SlotIdx;

	if (pDataBase->GetRecordCount() != 1)			//지금 구조에서 하나가 아니면 버그 입니다. 
	{
		pOut->_i32Rv = EVENT_FAIL_DATABASE_AUTH;
		return;
	}

	_UID		= pDataBase->GetINT64Data(0);
	_ItemID		= pDataBase->GetINT32Data(1);
	_ItemState	= pDataBase->GetINT8Data(2);
	_AuthArg	= pDataBase->GetUINT32Data(3);

	// 이곳에 기존 아이디 정보를 담아둡니다.
	pOut->_ui32TempItem = _ItemID;

	if ((_UID != pIn->_i64UID) || (0 == _ItemID) || (0 == _ItemState) || (0 == _AuthArg))		// 몇가지 에러들입니다. 실패처리 합니다.
	{
		pOut->_i32Rv = EVENT_ERROR_EVENT_AUTH_FAIL;
		return;
	}
	// 이미 인증이 된 아이템으므로 인증 할 수 없습니다.
	if (ITEM_ATTR_TYPE_AUTH == _ItemState)
	{
		pOut->_i32Rv = EVENT_ERROR_EVENT_AUTH_START_ALREADY_AUTH;
		return;
	}

	SHOP_ITEM_DATA * pItem = g_pShop->GetItemByID(_ItemID);
	if (NULL == pItem)
	{	// 나올 수 없습니다.
		pOut->_i32Rv = EVENT_ERROR_EVENT_INSERT_ITEM_VALID_ITEM_ID;
		return;
	}

	// 수동인증이 아니면 인증 할 수 없습니다.
	if (ITEM_AUTH_HAND != pItem->_ui8AutoStart)
	{
		pOut->_i32Rv = EVENT_ERROR_EVENT_AUTH_FAIL;
		return;
	}

	switch (pItem->_ui8AuthType)
	{
	case ITEM_TYPE_COUNT: //카운트 
	{//갯수 만큼 증감 한다.
		UINT32 ui32InArg = pIn->_ui32ItemArg * static_cast<UINT32>(pIn->_32Arg1.TLoopCount);
		ui32NewArg = 0;
		if (_AuthArg < ui32InArg)
		{
			ui32NewArg = 0;
			// 이런경우가 나오는지 로그한번 봅시다.
			g_pLog->WriteLog(L"[DB_SHOP_ERROR]_EXEC_ItemAuth( Arg: %d, Use: %d )", _AuthArg, ui32InArg);
		}
		else
		{
			ui32NewArg = _AuthArg - ui32InArg;
		}
		//g_pLog->WriteLog(L"[DB_COUNT] : (NewArg : %d, Auth : %d, In :%d)", ui32NewArg, _AuthArg, ui32InArg);

		if (ui32NewArg <= 0 && ITEM_EXPIRE_DELETE == pItem->_ui8AuthExpireType)
		{//Delete 
			S2DB::MakeQueryW(wstrSQL, L"usp_Inventory_del",
				DB_DATA_TYPE_INT, i32ThreadIdx,
				DB_DATA_TYPE_BIGINT, pIn->_i64UID,
				DB_DATA_TYPE_INT, pIn->_TWareDBIdx,
				DB_DATA_TYPE_END);
			pOut->_ui32ItemID = 0;
		}
		else
		{//Update 
			S2DB::MakeQueryW(wstrSQL, L"usp_Inventory_upd_ItemAuth",
				DB_DATA_TYPE_INT, i32ThreadIdx,
				DB_DATA_TYPE_INT, ui32NewArg,
				DB_DATA_TYPE_BIGINT, pIn->_i64UID,
				DB_DATA_TYPE_INT, pIn->_TWareDBIdx,
				DB_DATA_TYPE_END);
			pOut->_ui32ItemID = _ItemID;
		}
		DATABASE_EXECUTE_SP;
		if (1 != pDataBase->GetRecordCount())
		{
			_DataBaseResultFailLog(i32ThreadIdx, wstrSQL);
			pOut->_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
			return;
		}
		if (1 != pDataBase->GetINT32Data(0))
		{
			_DataBaseResultFailLog(i32ThreadIdx, wstrSQL);
			pOut->_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
			return;
		}

		pOut->_ui8ItemState = ITEM_ATTR_TYPE_BUY;
		pOut->_ui32AuthSec = ui32NewArg;
	}
	break;
	// 기간제, 영구제는 아직 사용하지 않는다.
	default: 
		break;
	}

	pOut->_i32Rv = EVENT_ERROR_SUCCESS;
	// Out에 Loop Count를 적용 해줍니다. Out 값을 이용해 묶어서 처리하기 위함
	pOut->_32Arg1.TLoopCount = pIn->_32Arg1.TLoopCount;
	return;
}

void CModuleDBShop::_EXEC_GiftAuth( INT32 i32ThreadIdx,	DB_GIFT_OUT * pOut)
{
	CADODatabase	* pDataBase	= m_ppADOdb_DATABASE[i32ThreadIdx]; 
	wchar_t wstrSQL[SERVER_STRING_COUNT];
	char	strLog[SERVER_STRING_COUNT];
	
	for( UINT8 i = 0 ; i < pOut->m_ui8GiftCount ; i++ )
	{
		AUTH_GIFT_INVEN* pGift		= &pOut->m_aAuthGift[i];
		pGift->m_ui8ItemCount	= 0;
		
		// 선물 번호로 GoodsID를 찾아 온다.
		S2DB::MakeQueryW( wstrSQL, L"usp_GiftBox_sel_GiftState",
			DB_DATA_TYPE_INT,			i32ThreadIdx,
			DB_DATA_TYPE_BIGINT,		pOut->m_TUID,
			DB_DATA_TYPE_INT,			pGift->m_TDBIdx,
			DB_DATA_TYPE_END);
		if( !pDataBase->ExecuteSP( wstrSQL ) )
		{
			ErrorDBShop( i32ThreadIdx, wstrSQL );
			pGift->m_i32Rv	= EVENT_ERROR_DATABASE_EXECUTE_SP; 
			continue;
		}

		if( 1 != pDataBase->GetRecordCount() )
		{
			//_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
			pGift->m_i32Rv	= EVENT_ERROR_DATABASE_SP_RESULT; 
			continue;
		}

		T_UID		TReceiverUID	= pDataBase->GetINT64Data(0);
		T_GoodsID	TGoodsID		= pDataBase->GetUINT32Data(1);	// 지급할 선물 ID 
		UINT8		ui8Deliver		= pDataBase->GetUINT8Data(2);	// 지급 여부
		
		// 선물 상태 체크
		if( ui8Deliver == TYPE_GIFT_DELIVER_RECEIVED || ui8Deliver == TYPE_GIFT_DELIVER_STAND_BY || pOut->m_TUID != TReceiverUID  ) 
		{	
			pGift->m_i32Rv	= EVENT_ERROR_SHOP_GIFT_AUTH_FAIL_HACK; 
			continue;
		}

		// 상품 인벤토리 지급 
		SHOP_GOODS_LIST *pGoods = g_pShop->GetGoodsByID( TGoodsID );		// GoodsID 로 Goods 를 얻어온다.
		if( NULL ==  pGoods ) // 해당 Goods가 없다. 
		{
			pGift->m_i32Rv	= EVENT_ERROR_EVENT_BUY_GOODS_PARAMETER_GOODS; 
			continue;
		}

		DB_ITEM DBItem;
		DBItem.m_ui8InsertType = ITEM_INSERT_REQUEST_GIFT_AUTH;
		pGift->m_ui8ItemCount = DBItem.m_ui8ItemCount = (UINT8)pGoods->_ui16ItemCount;

		for( UINT8 k = 0 ; k < DBItem.m_ui8ItemCount ; ++k )	
		{	
			DBItem.m_aItem[k]._ui32ItemID		= pGoods->_ppItemData[k]->_ui32ItemID;
			DBItem.m_aItem[k]._ui32AddItemArg = pGoods->_pui32ItemArg[k]; 
		} 

		INT32 i32Rv = _EXEC_ItemInsert( i32ThreadIdx, pOut->m_TUID, &DBItem ); 
		if( EV_FAILED( i32Rv ) )
		{	
			pGift->m_i32Rv	= i32Rv; 
			continue;
		}

		for( UINT8 k = 0 ; k < DBItem.m_ui8ItemCount ; ++k )	
		{	// 결과 Update	 
			i3mem::Copy( &pGift->m_Inven[k], &DBItem.m_aItem[k], sizeof( INVEN_BUFFER ) );
		} 

		i3String::Format( strLog, SERVER_STRING_COUNT, "GIFT AUTH / UID : %I64d / GiftDBIdx : %d / GoodsID : %d / Deliver : %d", pOut->m_TUID, pGift->m_TDBIdx, TGoodsID, ui8Deliver );
		WRITE_SERVICE_LOG_DB(i32ThreadIdx, strLog); 

		S2DB::MakeQueryW( wstrSQL, L"usp_GiftBox_upd_DeliverState",
			DB_DATA_TYPE_INT,			i32ThreadIdx,
			DB_DATA_TYPE_BIGINT,		pOut->m_TUID,
			DB_DATA_TYPE_INT,			pGift->m_TDBIdx,
			DB_DATA_TYPE_TINYINT,		TYPE_GIFT_DELIVER_RECEIVED,
			DB_DATA_TYPE_END);

		if( !pDataBase->ExecuteSP( wstrSQL ) )
		{
			ErrorDBShop( i32ThreadIdx, wstrSQL );
			pGift->m_i32Rv = EVENT_ERROR_DATABASE_EXECUTE_SP;
			continue;
		}

		if( 1 != pDataBase->GetRecordCount() )	
		{
			pGift->m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
			continue;
		}

		if( 1 != pDataBase->GetINT32Data(0))	
		{
			pGift->m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
			continue;
		}

		pGift->m_ui32Expire = g_pConfig->GetAddAuthTime( i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI(), SECOND_PER_DAY * MAX_AUTH_GIFT_EXPIRE_DAY ) ;
		pGift->m_ui8Deliver = TYPE_GIFT_DELIVER_RECEIVED;
		pGift->m_i32Rv		= EVENT_ERROR_SUCCESS;
		pGift->m_TGoodsID	= TGoodsID;

	}

	return ; 
}

void CModuleDBShop::_EXEC_GiftBuyLink( INT32 i32ThreadIdx,	DB_GIFT_OUT * pOut )
{
	CADODatabase * pDataBase = m_ppADOdb_DATABASE[i32ThreadIdx]; 
	wchar_t	wstrSQL[SERVER_STRING_COUNT];

	for( UINT8 i = 0 ; i < pOut->m_ui8GiftCount; ++i )
	{
		switch( pOut->m_aAuthGift[i].m_i32Rv ) 
		{
		case EVENT_ERROR_SUCCESS:	// 링크  결제 성공시 Deliver를 UPDATE
			S2DB::MakeQueryW( wstrSQL, L"usp_GiftBox_upd_DeliverState",
			DB_DATA_TYPE_INT,			i32ThreadIdx,
			DB_DATA_TYPE_BIGINT,		pOut->m_TUID,
			DB_DATA_TYPE_INT,			pOut->m_aAuthGift[i].m_TDBIdx,
			DB_DATA_TYPE_TINYINT,		TYPE_GIFT_DELIVER_NORMAL,
			DB_DATA_TYPE_END);
			pOut->m_aAuthGift[i].m_ui32Expire = g_pConfig->GetAddAuthTime( i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI(), SECOND_PER_DAY * MAX_GET_GIFT_EXPIRE_DAY ) ;
			pOut->m_aAuthGift[i].m_ui8Deliver = TYPE_GIFT_DELIVER_NORMAL;
			break;
		default:	// 링크 결제 실패시 Row Delete
			S2DB::MakeQueryW( wstrSQL, L"usp_GiftBox_del",
				DB_DATA_TYPE_INT,			i32ThreadIdx,
				DB_DATA_TYPE_BIGINT,		pOut->m_TUID,
				DB_DATA_TYPE_INT,			pOut->m_aAuthGift[i].m_TDBIdx,
				DB_DATA_TYPE_END);
			break;
		}

		if( !pDataBase->ExecuteSP( wstrSQL ) )
		{
			ErrorDBShop( i32ThreadIdx, wstrSQL );
			pOut->m_aAuthGift[i].m_i32Rv = EVENT_ERROR_DATABASE_EXECUTE_SP;
			continue;
		}

		if( 1 != pDataBase->GetRecordCount() )	
		{
			pOut->m_aAuthGift[i].m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
			continue;
		}

		if( 1 != pDataBase->GetINT32Data(0))	
		{
			pOut->m_aAuthGift[i].m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
			continue;
		}

		// 주석이 맞음. 링크 결제 실패시에는 최종 결과가 실패로 내려가야 하므로
		// pGift[i].m_i32Rv = EVENT_ERROR_SUCCESS;
		
	}

	return ;
}

void CModuleDBShop::_EXEC_GiftRead( INT32 i32ThreadIdx, DB_GIFT_OUT * pOut )
{
	CADODatabase * pDataBase = m_ppADOdb_DATABASE[i32ThreadIdx]; 
	wchar_t	wstrSQL[SERVER_STRING_COUNT];
	
	for( UINT8 i = 0 ; i < pOut->m_ui8GiftCount; ++i )
	{
		//  1. 현재 선물 전달 상태를 확인 합니다.
		S2DB::MakeQueryW( wstrSQL, L"usp_GiftBox_sel_GiftState",
			DB_DATA_TYPE_INT,			i32ThreadIdx,
			DB_DATA_TYPE_BIGINT,		pOut->m_TUID,
			DB_DATA_TYPE_INT,			pOut->m_aAuthGift[i].m_TDBIdx,
			DB_DATA_TYPE_END);
		if( !pDataBase->ExecuteSP( wstrSQL ) )
		{
			ErrorDBShop( i32ThreadIdx, wstrSQL);
			pOut->m_aAuthGift[i].m_i32Rv = EVENT_ERROR_DATABASE_EXECUTE_SP;
			continue;
		}

		if( 1 != pDataBase->GetRecordCount() )
		{
			//_DataBaseResultFailLog( i32ThreadIdx, wstrSQL ); 
			pOut->m_aAuthGift[i].m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
			continue;
		}

		T_UID		TReceiverUID	= pDataBase->GetINT64Data(0); 
		//T_GoodsID	TGoodsID		= pDataBase->GetUINT32Data(1);	// 지급할 선물 ID 
		UINT8		ui8Deliver		= pDataBase->GetUINT8Data(2);	// 지급 여부
		DATE dtExpire				=	pDataBase->GetDateData(3);
		UINT32 ui32Expire			=	S2DB::ConvertSmallDateToUINT32(dtExpire);

		// 선물 상태 체크
		if( ui8Deliver == TYPE_GIFT_DELIVER_RECEIVED || ui8Deliver == TYPE_GIFT_DELIVER_STAND_BY || pOut->m_TUID != TReceiverUID  ) 
		{ 	// 결제 완료되지 않은 선물이거나 지급된 선물이 READ상태가 되면 안됩니다.
			pOut->m_aAuthGift[i].m_i32Rv = EVENT_ERROR_SHOP_GIFT_AUTH_FAIL_HACK;
			continue;
		}

		S2DB::MakeQueryW( wstrSQL, L"usp_GiftBox_upd_DeliverState",
			DB_DATA_TYPE_INT,			i32ThreadIdx,
			DB_DATA_TYPE_BIGINT,		pOut->m_TUID,
			DB_DATA_TYPE_INT,			pOut->m_aAuthGift[i].m_TDBIdx,
			DB_DATA_TYPE_TINYINT,		TYPE_GIFT_DELIVER_READ,
			DB_DATA_TYPE_END);

		if( !pDataBase->ExecuteSP( wstrSQL ) )
		{
			ErrorDBShop( i32ThreadIdx, wstrSQL );
			pOut->m_aAuthGift[i].m_i32Rv = EVENT_ERROR_DATABASE_EXECUTE_SP;
			continue;
		}

		if( 1 != pDataBase->GetRecordCount() )	
		{
			pOut->m_aAuthGift[i].m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
			continue;
		}

		if( 1 != pDataBase->GetINT32Data(0))	
		{
			pOut->m_aAuthGift[i].m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
			continue;
		}
	
		pOut->m_aAuthGift[i].m_ui8Deliver					= TYPE_GIFT_DELIVER_READ;
		pOut->m_aAuthGift[i].m_ui32Expire					= ui32Expire; 
		pOut->m_aAuthGift[i].m_i32Rv						= EVENT_ERROR_SUCCESS;
	}
	return ;
}

void CModuleDBShop::_EXEC_GiftDelete(	INT32 i32ThreadIdx,	DB_GIFT_OUT * pOut )
{
	CADODatabase * pDataBase = m_ppADOdb_DATABASE[i32ThreadIdx]; 
	wchar_t	wstrSQL[SERVER_STRING_COUNT];

	for( UINT8 i = 0 ; i < pOut->m_ui8GiftCount; ++i )
	{
		AUTH_GIFT_INVEN	*pGift = &pOut->m_aAuthGift[i];

		// 선물 번호로 GoodsID를 찾아 온다.
		S2DB::MakeQueryW(wstrSQL, L"usp_GiftBox_sel_GiftState",
			DB_DATA_TYPE_INT,			i32ThreadIdx,
			DB_DATA_TYPE_BIGINT,		pOut->m_TUID,
			DB_DATA_TYPE_INT,			pGift->m_TDBIdx,
			DB_DATA_TYPE_END);
		if( !pDataBase->ExecuteSP( wstrSQL ) )
		{
			ErrorDBShop( i32ThreadIdx, wstrSQL);
			pGift->m_i32Rv	= EVENT_ERROR_DATABASE_EXECUTE_SP; 
			continue;
		}

		if( 1 != pDataBase->GetRecordCount() )
		{
			//_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
			pGift->m_i32Rv	= EVENT_ERROR_DATABASE_SP_RESULT; 
			continue;
		}

		//T_UID		TReceiverUID	= pDataBase->GetINT64Data(0);
		pGift->m_TGoodsID			= pDataBase->GetUINT32Data(1);	
		//UINT8		ui8Deliver		= pDataBase->GetUINT8Data(2);	


		S2DB::MakeQueryW( wstrSQL, L"usp_GiftBox_del",
				DB_DATA_TYPE_INT,			i32ThreadIdx,
				DB_DATA_TYPE_BIGINT,		pOut->m_TUID,
				DB_DATA_TYPE_INT,			pGift->m_TDBIdx,
				DB_DATA_TYPE_END);

		if( !pDataBase->ExecuteSP( wstrSQL ) )
		{
			ErrorDBShop( i32ThreadIdx, wstrSQL);
			pGift->m_i32Rv = EVENT_ERROR_DATABASE_EXECUTE_SP;
			continue;
		}

		if( 1 != pDataBase->GetRecordCount() )	
		{
			pGift->m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
			continue;
		}

		if( 1 != pDataBase->GetINT32Data(0))	
		{
			pGift->m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
			continue;
		}

		pGift->m_i32Rv		= EVENT_ERROR_SUCCESS;
	}

	return ;
}

void CModuleDBShop::ErrorDBShop(INT32 i32ThreadIdx, wchar_t * wstrSql )
{
	CADODatabase*	pDataBase = NULL; 
	CDBConfig*		pDBConfig = NULL;

	switch(i32ThreadIdx)
	{
	case MODULE_WEBGIFT_THREAD_IDX			: 
		{
			pDataBase = m_pADOdb_WEBGIFT; 
			pDBConfig = m_pGiftDBConfig;
		}
		break; 
	case MODULE_DB_SHOP_MAIN_THREAD_IDX		: 
		{
			pDataBase = m_pADOdb_DBMAIN_Shop;
			pDBConfig = m_pControlDBConfig;
		}
		break; 
	case MAIN_THREAD_IDX :
		return;
	default				 : 
		{
			if( DATABASE_THREAD_COUNT <= i32ThreadIdx ) return;

			pDataBase = m_ppADOdb_DATABASE[i32ThreadIdx];
			pDBConfig = m_pSIAUserDBConfig[i32ThreadIdx];
		}
		break; 
	}

	if( !pDBConfig )
		return;

	g_pLog->WriteLog( L"[ErrorDBShop] %s, Error Code : %08x", wstrSql, pDataBase->GethrLastError() );
	
	HRESULT EventType = pDataBase->ReConnect(
																	pDBConfig->GetDataSource(),
																	pDBConfig->GetCatalog(),
																	pDBConfig->GetAccount(),
																	pDBConfig->GetPassword()
																);
	
	switch( EventType )
	{
	case EVENT_ERROR_EVENT_LOG_IN_NOT_CONNECT_DB:		// 리커넥을 시도하였으나 실패 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_DB, L"[ErrorDBShop] RECONNECT FAILE" );
		break;
	case EVENT_ERROR_SUCCESS:							// 리커넥을 시도하여 성공 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_DB, L"[ErrorDBShop] RECONNECT SUCCEED" );
		break;
	case EVENT_ERROR_FAIL:								// 리커넥을 시도하지 않았을 경우
		
		break;
	}
}

void CModuleDBShop::_DataBaseResultFailLog(INT32 i32ThreadIdx, wchar_t * wstrSql )
{
	g_pLog->WriteLog( L"[DB_ERROR] %s", wstrSql );
}

BOOL CModuleDBShop::InRepair( DB_REPAIR * pIn )
{
	INT32 i32ThreadIdx = (INT32)(pIn->m_TUID % DATABASE_THREAD_COUNT);
	if(0 > i32ThreadIdx || DATABASE_THREAD_COUNT <= i32ThreadIdx) return FALSE;

	//Push
	if( m_ppInRepairBuffer[i32ThreadIdx]->Push( pIn ) != RINGBUFFER_SUCCESS )
	{
		return FALSE; 
	}
	return TRUE;
}

void CModuleDBShop::_EXEC_Repair( INT32 i32ThreadIdx, DB_REPAIR * pIn )
{
	CADODatabase	* pDataBase	= m_ppADOdb_DATABASE[i32ThreadIdx]; 
	wchar_t wstrSQL[SERVER_STRING_COUNT];
	char strLog[MAX_PATH];
		
	for( UINT8 i = 0 ; i < pIn->m_stRepairInfo.m_ui8ItemCount ; i++ )
	{
		S2MO_REPAIR_ITEM *pItem = &pIn->m_stRepairInfo.m_stRepairItem[i];
		switch( pItem->m_ui8BuyType ) 
		{
		case GOODS_BUY_POINT:	// 포인트 수리는 바로 수리.
			{				
				S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_upd_RepairAuth",
					DB_DATA_TYPE_INT,			i32ThreadIdx,
					DB_DATA_TYPE_BIGINT,		pIn->m_TUID,
					DB_DATA_TYPE_INT,			pItem->m_TItemDBIdx,
					DB_DATA_TYPE_INT,			pItem->m_ui32AuthArg,
					DB_DATA_TYPE_END);
				if( !pDataBase->ExecuteSP( wstrSQL ) )
				{
					pItem->m_i32Rv = EVENT_ERROR_DATABASE_EXECUTE_SP;
					ErrorDBShop( i32ThreadIdx, wstrSQL);
					break;
				}
				if( 1 != pDataBase->GetRecordCount() )
				{
					pItem->m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
					_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
					break;
				}
				if( 1 != pDataBase->GetINT32Data(0))
				{
					pItem->m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
					_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
					break;
				}

				pItem->m_ui32AuthArg = pDataBase->GetUINT32Data(1);		// 새로 업데이트된 Arg를 담아둡니다.

				i3String::Format( strLog, MAX_PATH, "REPAIR / POINT /  UID:%I64d ItemID:%d ItemArg:%d DBIdx:%d", 
					pIn->m_TUID, pItem->m_TItemID, pItem->m_ui32AuthArg, pItem->m_TItemDBIdx );
				WRITE_SERVICE_LOG_DB(i32ThreadIdx, strLog);

			}
			break;
		case GOODS_BUY_CASH:	// 캐쉬 결제는 기존에 아이템 소지여부와 관계없이 STAND BY 로우를 INSERT한다.
			{
				S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_ins",
					DB_DATA_TYPE_INT,		i32ThreadIdx,
					DB_DATA_TYPE_BIGINT,	pIn->m_TUID,
					DB_DATA_TYPE_INT,		pItem->m_TItemID,
					DB_DATA_TYPE_TINYINT,	ITEM_ATTR_TYPE_STAND_BY,
					DB_DATA_TYPE_INT,		pItem->m_ui32AuthArg,
					DB_DATA_TYPE_END );
				if( !pDataBase->ExecuteSP( wstrSQL ) )
				{
					pItem->m_i32Rv = EVENT_ERROR_DATABASE_EXECUTE_SP;
					ErrorDBShop( i32ThreadIdx, wstrSQL);
					break;
				}
				if( 1 != pDataBase->GetRecordCount() )
				{
					pItem->m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
					_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
					break;
				}
				if( 1 != pDataBase->GetINT32Data(0))
				{
					pItem->m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
					_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
					break;
				}

				S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_sel_ItemIdx",
				DB_DATA_TYPE_INT,			i32ThreadIdx,
				DB_DATA_TYPE_BIGINT,		pIn->m_TUID,
				DB_DATA_TYPE_INT,			pItem->m_TItemID,
				DB_DATA_TYPE_END);
				if( !pDataBase->ExecuteSP( wstrSQL ) )
				{
					pItem->m_i32Rv = EVENT_ERROR_DATABASE_EXECUTE_SP;
					ErrorDBShop( i32ThreadIdx, wstrSQL);
					break;
				}

				if( 1 > pDataBase->GetRecordCount() )
				{
					pItem->m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
					_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
					break;
				}

				pItem->m_TItemDBIdx = pDataBase->GetINT32Data(0);	// DB Idx를 담아둡니다.
				pItem->m_i32Rv = EVENT_ERROR_SUCCESS;

				i3String::Format( strLog, MAX_PATH, "REPAIR INSERT STAND BY / CASH /  UID:%I64d ItemID:%d ItemArg:%d DBIdx:%d", 
					pIn->m_TUID, pItem->m_TItemID, pItem->m_ui32AuthArg, pItem->m_TItemDBIdx );
				WRITE_SERVICE_LOG_DB(i32ThreadIdx, strLog);
			}
			break;
		default:
			break;
		}
	}
	return;
}

void CModuleDBShop::_EXEC_RepairLink(		INT32 i32ThreadIdx, DB_REPAIR * pIn )
{
	CADODatabase	* pDataBase	= m_ppADOdb_DATABASE[i32ThreadIdx]; 
	wchar_t wstrSQL[SERVER_STRING_COUNT];
	char	strLog[MAX_PATH];
	INT32	i32DeleteRowRv;

	for( UINT8 i = 0 ; i < pIn->m_stRepairInfo.m_ui8ItemCount ; i++ )
	{
		S2MO_REPAIR_ITEM *pItem = &pIn->m_stRepairInfo.m_stRepairItem[i];
		if( NULL == pItem ) continue;

		if( EV_FAILED(pItem->m_i32Rv) ) 
		{	// Link 결제 실패라면 STAND BY 로우를 DELETE 합니다.
			i32DeleteRowRv = _EXEC_DeleteRow( i32ThreadIdx,  pIn->m_TUID, pItem->m_TItemDBIdx  ) ;
			if( EV_FAILED( i32DeleteRowRv ) )	
			{	// 여기서 DB 작업이 실패하면, 추후 보상 대상에서 제외시켜야 하므로 로그 남깁니다.
				i3String::Format( strLog, MAX_PATH, "REPAIR DELETE FAIL(LINK FAIL) / NO COMPENSATION / UID:%I64d  ItemID:%d ItemArg%d DBIdx:%d ", 
					pIn->m_TUID, pItem->m_TItemID, pItem->m_ui32AuthArg, pItem->m_TItemDBIdx );
				WRITE_SERVICE_LOG_DB(i32ThreadIdx, strLog);
			} 
			continue;
		}

		// 2. LINK 결제 성공  
		
		// 기존 로우 검색
		S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_sel_checkItem",
			DB_DATA_TYPE_INT,			i32ThreadIdx,
			DB_DATA_TYPE_BIGINT,		pIn->m_TUID,
			DB_DATA_TYPE_INT,			pItem->m_TItemID,
			DB_DATA_TYPE_TINYINT,		ITEM_ATTR_TYPE_STAND_BY,
			DB_DATA_TYPE_END);
		if( !pDataBase->ExecuteSP( wstrSQL ) )
		{
			ErrorDBShop( i32ThreadIdx, wstrSQL);
			pItem->m_i32Rv = EVENT_ERROR_DATABASE_EXECUTE_SP;
			continue;
		}
		T_ItemDBIdx TItemDBIdx = pDataBase->GetINT32Data(0);

		// 수리. 기존 로우 UPDATE.
		S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_upd_RepairAuth",
					DB_DATA_TYPE_INT,			i32ThreadIdx,
					DB_DATA_TYPE_BIGINT,		pIn->m_TUID,
					DB_DATA_TYPE_INT,			TItemDBIdx,
					DB_DATA_TYPE_INT,			pItem->m_ui32AuthArg,
					DB_DATA_TYPE_END);
		if( !pDataBase->ExecuteSP( wstrSQL ) )
		{
			pItem->m_i32Rv = EVENT_ERROR_DATABASE_EXECUTE_SP;
			ErrorDBShop( i32ThreadIdx, wstrSQL);
			continue;
		}
		if( 1 != pDataBase->GetRecordCount() )
		{
			pItem->m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
			_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
			continue;
		}
		if( 1 != pDataBase->GetINT32Data(0))
		{
			pItem->m_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
			_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
			continue;
		}

		UINT32 ui32NewArg = pDataBase->GetUINT32Data(1);

		// STAND BY 로우 DELETE, 
		_EXEC_DeleteRow( i32ThreadIdx,  pIn->m_TUID, pItem->m_TItemDBIdx  ) ;
		
		// 새로 업데이트된 DBIdx, Arg를 담아둡니다.
		pItem->m_TItemDBIdx = TItemDBIdx;
		pItem->m_ui32AuthArg = ui32NewArg;

		i3String::Format( strLog, MAX_PATH, "REPAIR / CASH /  UID:%I64d ItemID:%d ItemArg:%d DBIdx:%d", 
			pIn->m_TUID, pItem->m_TItemID, pItem->m_ui32AuthArg, pItem->m_TItemDBIdx );
		WRITE_SERVICE_LOG_DB(i32ThreadIdx, strLog);
	}
	return;
}


void CModuleDBShop::_EXEC_GoodsInsert(INT32 i32ThreadIdx, DB_SHOP_GOODS * pIn )
{
	pIn->m_ui8GoodsCount = MIN( pIn->m_ui8GoodsCount, MAX_SHOP_GOODS_BUY_COUNT ); 
	pIn->m_stChara.m_ui8SlotState = MULTI_SLOT_STATE_NORMAL;
	
	for( UINT8 GoodsLoop = 0 ; GoodsLoop < pIn->m_ui8GoodsCount ; GoodsLoop++ )	
	{
		DB_GOODS *pGoods = &pIn->m_stDBGoods[GoodsLoop];
		if( NULL == pGoods ) continue;
 
		pGoods->m_i32Rv = _EXEC_ItemInsert( i32ThreadIdx, pIn->m_TUID,  (DB_ITEM*)&pGoods->m_stDBItem, pIn->m_ui8JobAction, pIn->m_TItemDBIdx, &pIn->m_stChara.m_ui8SlotState, &pIn->m_stChara.m_bAddCharaSlot );
		if( EV_FAILED( pGoods->m_i32Rv ) )
		{
			g_pLog->WriteLog( L"[DB_SHOP_ERROR]_EXEC_GoodsInsert / Rv : %x / UID : %I64d / Type : %d / GoodsID : %d", 
				pGoods->m_i32Rv, pIn->m_TUID, pIn->m_ui8JobAction, pGoods->m_TGoodsID );
			continue;
		}


	}	// Goods End

	return; 
}

INT32 CModuleDBShop::_EXEC_ItemInsert( INT32 i32ThreadIdx, T_UID TUID, DB_ITEM* pDBItem, UINT8 ui8JobAction, T_ItemDBIdx TItemDBIdx, UINT8 * pui8MultiSlotState, bool* pbAddCharaSlot )
{
	T_ItemDBIdx	_TItemDBIdx;
	UINT8		_ItemState;
	UINT32		_AuthArg;
	UINT32		_NewAuthArg = 0;
		
	SHOP_ITEM_DATA	*pItem		= NULL; 
	CADODatabase	* pDataBase	= m_ppADOdb_DATABASE[i32ThreadIdx]; 
	wchar_t wstrSQL[SERVER_STRING_COUNT];
	char strLog[MAX_PATH];

	for( UINT8 i = 0 ; i < pDBItem->m_ui8ItemCount ; i++ )	
	{
		INVEN_BUFFER_DB* pItemInfo = &pDBItem->m_aItem[i];
		if( NULL == pItemInfo ) return EVENT_ERROR_EVENT_INSERT_ITEM_FAIL;

		pItem	= g_pShop->GetItemByID( pItemInfo->_ui32ItemID );
		if( NULL == pItem )	return EVENT_ERROR_EVENT_INSERT_ITEM_VALID_ITEM_ID;
	
		// 기간 연장
		if( TYPE_SHOP_ITEM_EXTEND == ui8JobAction ) 
		{
			//1. 있는지 검색 
			S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_sel_ItemExtend",
					DB_DATA_TYPE_INT,			i32ThreadIdx,
					DB_DATA_TYPE_BIGINT,		TUID,
					DB_DATA_TYPE_INT,			TItemDBIdx,
					DB_DATA_TYPE_INT,			pItemInfo->_ui32ItemID,
					DB_DATA_TYPE_END);
			DATABASE_EXECUTE_SP_ITEM;

			// 없으면 에러.
			if( pDataBase->GetRecordCount() == 0)  return EVENT_ERROR_SHOP_NOT_FIND_ITEM;

			// 아이템타입 에러.
			if( ITEM_EXPIRE_LEAVE == pItem->_ui8AuthExpireType ) return EVENT_ERROR_SHOP_ITEM_TYPE_ERROR; 
			
			_TItemDBIdx	= pDataBase->GetINT32Data(0);	
			_ItemState  = pDataBase->GetUINT8Data(1);
			_AuthArg	= pDataBase->GetUINT32Data(2);

			// 소지한 아이템이 영구제이면 에러.
			if( _ItemState == INVENTORY_STATE_ETERNITY ) return EVENT_ERROR_SHOP_ITEM_TYPE_ERROR;
			
			switch( pItem->_ui8AuthType )
			{
			case ITEM_TYPE_COUNT: //카운트 
				_NewAuthArg = CALCULATE_ITEM_COUNT(_AuthArg, pItemInfo->_ui32AddItemArg);
				break; 		
			case ITEM_TYPE_TIME: //기간
				if(_ItemState == INVENTORY_STATE_BUY)			_NewAuthArg = CALCULATE_ITEM_COUNT( _AuthArg, pItemInfo->_ui32AddItemArg);							// 아직 인증이 안된 상태
				else if( i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI() > _AuthArg )	_NewAuthArg = g_pConfig->GetAuthTime( pItemInfo->_ui32AddItemArg );				// 인증은 되었지만 기간이 지난상태
				else											_NewAuthArg = g_pConfig->GetAddAuthTime(_AuthArg, pItemInfo->_ui32AddItemArg);	// 현재 인증중
				break;
			default : 
				return EVENT_ERROR_SHOP_ITEM_TYPE_ERROR;
				break;
			}

			S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_upd_ItemAuth",
					DB_DATA_TYPE_INT,			i32ThreadIdx,
					DB_DATA_TYPE_INT,			_NewAuthArg,
					DB_DATA_TYPE_BIGINT,		TUID,
					DB_DATA_TYPE_INT,			TItemDBIdx,
					DB_DATA_TYPE_END);
			DATABASE_EXECUTE_SP_ITEM;
			if( 1 != pDataBase->GetRecordCount() )
			{
				_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
				return  EVENT_ERROR_DATABASE_SP_RESULT;
			}
			if( 1 != pDataBase->GetINT32Data(0))
			{
				_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
				return  EVENT_ERROR_DATABASE_SP_RESULT;
			}

			i3String::Format( strLog, MAX_PATH, "ITEM EXTEND / Action:%d UID:%I64d ItemID:%d State:%d Arg:%d DBIdx:%d", 
				ui8JobAction, TUID, pItemInfo->_ui32ItemID, _ItemState, _NewAuthArg, TItemDBIdx );
			WRITE_SERVICE_LOG_DB(i32ThreadIdx, strLog);

			pItemInfo->_TItemWareDBIdx		= TItemDBIdx;
			pItemInfo->_ui8ItemType			= _ItemState;
			pItemInfo->_ui32ItemArg			= _NewAuthArg;
			continue;
		}

		// 파츠라면 무조건 새로 INSERT.
		if( g_pConfig->IsPartsByItemID( pItemInfo->_ui32ItemID ) )
		{
			goto INSERT_FLAG;	// 기존에 있으나 없으나 상관없이 INSERT.
		}
		
		//1. 있는지 검색 
		S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_sel_checkItem",
			DB_DATA_TYPE_INT,			i32ThreadIdx,
			DB_DATA_TYPE_BIGINT,		TUID,
			DB_DATA_TYPE_INT,			pItemInfo->_ui32ItemID,
			DB_DATA_TYPE_TINYINT,		ITEM_ATTR_TYPE_STAND_BY,
			DB_DATA_TYPE_END);
		DATABASE_EXECUTE_SP_ITEM;

		if (pDataBase->GetRecordCount() > 0)
		{//있음
			_TItemDBIdx = pDataBase->GetINT32Data(0);
			_ItemState = pDataBase->GetUINT8Data(1);
			_AuthArg = pDataBase->GetUINT32Data(2);

			
			if (ITEM_TYPE_CHARA == GET_ITEM_TYPE(pItem->_ui32ItemID) && ITEM_ATTR_TYPE_BASIC == _ItemState && ITEM_ATTR_TYPE_BASIC == pItem->_ui8AuthType)
			{	// 기존에 소지한 캐릭터가 영구제이면 INSERT
				goto INSERT_FLAG;	// 기존에 있으나 없으나 상관없이 INSERT.
			}

			// 기존에 소지한 아이템이 내구도 이거나 영구제이면 실패 처리 한다.
			if (ITEM_EXPIRE_LEAVE == pItem->_ui8AuthExpireType || ITEM_ATTR_TYPE_BASIC == _ItemState || ITEM_ATTR_TYPE_BASIC == pItem->_ui8AuthType)
			{
				return EVENT_ERROR_SHOP_ITEM_TYPE_ERROR;
			}

			switch( pItem->_ui8AuthType )
			{
			case ITEM_TYPE_COUNT: //카운트 
				_NewAuthArg = CALCULATE_ITEM_COUNT(_AuthArg, pItemInfo->_ui32AddItemArg);
				break; 		
			case ITEM_TYPE_TIME: //기간
				if(_ItemState == ITEM_ATTR_TYPE_BUY)			_NewAuthArg = CALCULATE_ITEM_COUNT( _AuthArg, pItemInfo->_ui32AddItemArg);								// 아직 인증이 안된 상태
				else if( i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI() > _AuthArg )	_NewAuthArg = g_pConfig->GetAuthTime( pItemInfo->_ui32AddItemArg );				// 인증은 되었지만 기간이 지난상태
				else											_NewAuthArg = g_pConfig->GetAddAuthTime(_AuthArg, pItemInfo->_ui32AddItemArg );	// 현재 인증중
				break;
			default				: //Error
				return EVENT_ERROR_SHOP_ITEM_TYPE_ERROR;
				break; 
			}

			S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_upd_ItemAuth",
					DB_DATA_TYPE_INT,			i32ThreadIdx,
					DB_DATA_TYPE_INT,			_NewAuthArg,
					DB_DATA_TYPE_BIGINT,		TUID,
					DB_DATA_TYPE_INT,			_TItemDBIdx,
					DB_DATA_TYPE_END);

			DATABASE_EXECUTE_SP_ITEM;
			if( 1 != pDataBase->GetRecordCount() )
			{
				_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
				return  EVENT_ERROR_DATABASE_SP_RESULT;
			}
			if( 1 != pDataBase->GetINT32Data(0))
			{
				_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
				return  EVENT_ERROR_DATABASE_SP_RESULT;
			}

			
		}
		else	// 없으면 INSERT.
		{	
INSERT_FLAG:  
			switch( pItem->_ui8AuthType )
			{
			case ITEM_TYPE_COUNT: //카운트
				_ItemState	= ITEM_ATTR_TYPE_BUY; 
				_NewAuthArg	= pItemInfo->_ui32AddItemArg;
				break;
			case ITEM_TYPE_TIME: //기간 
				if( pui8MultiSlotState  )
				{
					*pui8MultiSlotState = MULTI_SLOT_STATE_UNUSE;
				}

				if( pItem->_ui8AutoStart == ITEM_AUTH_AUTO )
				{	// 자동 인증
					_ItemState	= ITEM_ATTR_TYPE_AUTH;
					_NewAuthArg = g_pConfig->GetAuthTime( pItemInfo->_ui32AddItemArg); //기간 넣기
				}
				else
				{	// 수동 인증, 인증 못함
					_ItemState	= ITEM_ATTR_TYPE_BUY; 
					_NewAuthArg = pItemInfo->_ui32AddItemArg;
				}
				break;
			case ITEM_TYPE_ETERNITY:	// 영구
				_ItemState		= ITEM_ATTR_TYPE_BASIC; 
				_NewAuthArg		= 1;
				break;
			default				: //Error
				return  EVENT_ERROR_SHOP_ITEM_TYPE_ERROR; 
				break; 
			}

			S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_ins",
				DB_DATA_TYPE_INT,		i32ThreadIdx,
				DB_DATA_TYPE_BIGINT,	TUID,
				DB_DATA_TYPE_INT,		pItemInfo->_ui32ItemID,
				DB_DATA_TYPE_TINYINT,	_ItemState,
				DB_DATA_TYPE_INT,		_NewAuthArg,
				DB_DATA_TYPE_END );
			DATABASE_EXECUTE_SP_ITEM;
			if( 1 != pDataBase->GetRecordCount() )
			{
				_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
				return EVENT_ERROR_DATABASE_SP_RESULT;
			}
			if( 1 != pDataBase->GetINT32Data(0))
			{
				_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
				return EVENT_ERROR_DATABASE_SP_RESULT;
			}

			S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_sel_ItemIdx",
				DB_DATA_TYPE_INT,			i32ThreadIdx,
				DB_DATA_TYPE_BIGINT,		TUID,
				DB_DATA_TYPE_INT,			pItemInfo->_ui32ItemID,
				DB_DATA_TYPE_END);

			DATABASE_EXECUTE_SP_ITEM;

			_TItemDBIdx		= pDataBase->GetINT32Data(0);

			if( pbAddCharaSlot ) 
			{
				*pbAddCharaSlot = true;
			}
		}

		i3String::Format( strLog, MAX_PATH, "ITEM INSERT / Action:%d UID:%I64d ItemID:%d State:%d Arg:%d DBIdx:%d", 
			ui8JobAction, TUID, pItemInfo->_ui32ItemID, _ItemState, _NewAuthArg, _TItemDBIdx );
		WRITE_SERVICE_LOG_DB(i32ThreadIdx, strLog);

		pItemInfo->_TItemWareDBIdx	= _TItemDBIdx;
		pItemInfo->_ui8ItemType		= _ItemState;
		pItemInfo->_ui32ItemArg		= _NewAuthArg;
	}	// ItemEnd

	return EVENT_ERROR_SUCCESS;
}

INT32 CModuleDBShop::_EXEC_DeleteRow( INT32 i32ThreadIdx, T_UID TUID, T_ItemDBIdx	TItemDBIdx )
{ 
	CADODatabase	* pDataBase	= m_ppADOdb_DATABASE[i32ThreadIdx]; 
	wchar_t wstrSQL[SERVER_STRING_COUNT];

	S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_del",
			DB_DATA_TYPE_INT,			i32ThreadIdx,
			DB_DATA_TYPE_BIGINT,		TUID,
			DB_DATA_TYPE_INT,			TItemDBIdx,
			DB_DATA_TYPE_END);
	if( !pDataBase->ExecuteSP( wstrSQL ) )
	{	
		ErrorDBShop( i32ThreadIdx, wstrSQL );
		return EVENT_ERROR_DATABASE_EXECUTE_SP;	
	}
	if( 1 != pDataBase->GetRecordCount() )
	{
		_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
		return  EVENT_ERROR_DATABASE_SP_RESULT;
	}
	if( 1 != pDataBase->GetINT32Data(0))
	{
		_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
		return EVENT_ERROR_DATABASE_SP_RESULT;
	}

	return EVENT_ERROR_SUCCESS;
}

INT32 CModuleDBShop::_GiftAutoAuth( INT32 i32ThreadIdx, T_UID TUID, DB_ITEM *pItem, S2MO_GIFT *pGift, bool& bGiftAutoAuth, ENUM_TYPE_GIFT_DELIVER eDeliver )
{
	if( NULL == pItem || NULL == pGift ) return EVENT_ERROR_FAIL;

	CADODatabase * pDataBase = m_ppADOdb_DATABASE[i32ThreadIdx]; 
	wchar_t wstrSQL[SERVER_STRING_COUNT];
	char	strLog[ SERVER_STRING_COUNT];

	switch( pGift->m_ui8Deliver ) 
	{
	case TYPE_GIFT_DELIVER_NORMAL : 
	case TYPE_GIFT_DELIVER_READ : 
		{	// 선물을 인벤토리로 지급 
			SHOP_GOODS_LIST*	pGoods = g_pShop->GetGoodsByID( pGift->m_TGoodsID );// GoodsID 로 Goods 를 얻어온다.
			if(	NULL == pGoods ) return EVENT_ERROR_EVENT_BUY_GOODS_PARAMETER_GOODS; // 해당 Goods가 없다

			pItem->m_ui8ItemCount = (UINT8)pGoods->_ui16ItemCount;

			for( UINT8 i = 0 ; i < pItem->m_ui8ItemCount ; ++i )	
			{	
				pItem->m_aItem[i]._ui32ItemID		= pGoods->_ppItemData[i]->_ui32ItemID;
				pItem->m_aItem[i]._ui32AddItemArg	= pGoods->_pui32ItemArg[i];
			}

			INT32 i32Rv = _EXEC_ItemInsert( i32ThreadIdx, TUID, pItem ); 
			if( EV_FAILED( i32Rv ) )
			{
				i3String::Format( strLog, SERVER_STRING_COUNT, "Fail Gift Auto Auth / UID : %I64d / GoodsID : %d / Reason : %x ", TUID, pGift->m_TGoodsID , i32Rv );
				WRITE_SERVICE_LOG_DB( i32ThreadIdx, strLog ); 
				return i32Rv;  
			}

			bGiftAutoAuth = true;

			// ZLog 기록
			if( g_pModuleLogD != NULL ) 
			{
				LOG_GIFT	stLog; 
				stLog.m_TSenderUID		= 0;	// 알 수 없음.
				stLog.m_TReceiverUID	= TUID;
				stLog.m_eRequestType	= ITEM_INSERT_REQUEST_UNKNOWN;	// 알 수 없음.
				stLog.m_TGoodsID		= pGift->m_TGoodsID;
				stLog.m_eState	= eDeliver;
				stLog.m_ui32ExpireDate =  i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();

				LOG_MESSAGE eLogMessage = (LOG_MESSAGE)(LOG_MESSAGE_GIFT_DBSHOP_0 + i32ThreadIdx);
				g_pModuleLogD->SendLogMessage( eLogMessage , LOCALE_CODE_NONE, &stLog   );
			}
		}
		//break; 없는게 맞음.
	case TYPE_GIFT_DELIVER_RECEIVED : 
		{	// 선물을 DB에서 삭제.
			S2DB::MakeQueryW( wstrSQL, L"usp_GiftBox_del",
				DB_DATA_TYPE_INT,			i32ThreadIdx,
				DB_DATA_TYPE_BIGINT,		TUID,
				DB_DATA_TYPE_INT,			pGift->m_TDBIdx,
				DB_DATA_TYPE_END);
			if( !pDataBase->ExecuteSP( wstrSQL ) )
			{
				ErrorDBShop( i32ThreadIdx, wstrSQL);
				return EVENT_ERROR_DATABASE_EXECUTE_SP;

			}
			if( 1 != pDataBase->GetRecordCount() )
			{
				_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
				return EVENT_ERROR_DATABASE_SP_RESULT;
			}
			if( 1 != pDataBase->GetINT32Data(0))
			{
				_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
				return EVENT_ERROR_DATABASE_SP_RESULT;
			}
		}
		break;
	}

	return EVENT_ERROR_SUCCESS;
}

INT32 CModuleDBShop::_EXEC_GachaBuy(INT32 i32ThreadIdx, DB_GACHA * pIn)
{	
	CADODatabase	* pDataBase	= m_ppADOdb_DATABASE[i32ThreadIdx];
	wchar_t wstrSQL[SERVER_STRING_COUNT];
	char strLog[MAX_PATH];

	UINT32 ui32ItemID = pIn->m_ui32GachaGoodsID / 100;
	SHOP_GOODS_LIST *pGoods = g_pShop->GetGoodsByID( pIn->m_ui32GachaGoodsID );

	S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_ins",
		DB_DATA_TYPE_INT,		i32ThreadIdx,
		DB_DATA_TYPE_BIGINT,	pIn->m_TUID,
		DB_DATA_TYPE_INT,		ui32ItemID,
		DB_DATA_TYPE_TINYINT,	ITEM_ATTR_TYPE_STAND_BY,
		DB_DATA_TYPE_INT,		pGoods->_pui32ItemArg[0],
		DB_DATA_TYPE_END );

	if( !pDataBase->ExecuteSP( wstrSQL ) )
	{
		ErrorDBShop( i32ThreadIdx, wstrSQL);
		return EVENT_ERROR_DATABASE_EXECUTE_SP;
	} 
	if( 1 != pDataBase->GetRecordCount() )
	{
		_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
		return EVENT_ERROR_DATABASE_SP_RESULT;
	}
	if( 1 != pDataBase->GetINT32Data(0))
	{
		_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
		return  EVENT_ERROR_DATABASE_SP_RESULT; 
	}
	
	S2DB::MakeQueryW( wstrSQL, L"usp_Inventory_sel_ItemIdx",
		DB_DATA_TYPE_INT,		i32ThreadIdx,
		DB_DATA_TYPE_BIGINT,	pIn->m_TUID,
		DB_DATA_TYPE_INT,		ui32ItemID,
		DB_DATA_TYPE_END );

	if( !pDataBase->ExecuteSP( wstrSQL ) )
	{
		ErrorDBShop( i32ThreadIdx, wstrSQL);
		return EVENT_ERROR_DATABASE_EXECUTE_SP;
	} 

	INT32 i32Count = pDataBase->GetRecordCount();
	if(  i32Count < 1 )
	{
		_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
		return EVENT_ERROR_DATABASE_SP_RESULT; 
	}

	pIn->m_TTicketDBIdx = pDataBase->GetINT32Data(0);

	i3String::Format( strLog, MAX_PATH, "GACHA BUY INSERT STAND BY / CASH /  UID:%I64d ItemID:%d ItemArg:%d DBIdx:%d", 
		pIn->m_TUID, ui32ItemID, pGoods->_pui32ItemArg[0], pIn->m_TTicketDBIdx );
	WRITE_SERVICE_LOG_DB(i32ThreadIdx, strLog);

	return EVENT_ERROR_SUCCESS; 
}

void	CModuleDBShop::_EXEC_GiftShop_Insert(INT32 i32ThreadIdx, SEND_GIFT * pGift)
{
	CADODatabase * pDataBase = m_ppADOdb_DATABASE[i32ThreadIdx];
	wchar_t wstrSQL[SERVER_STRING_COUNT];
	char strLog[MAX_PATH];

	for (INT32 i = 0; i < pGift->m_ui8GiftCount; i++)
	{
		if (GET_GOODS_TYPE(pGift->m_aGift[i].m_ui32GoodsID) == ITEM_TYPE_SET)
		{
			continue; // 세트아이템은 패스
		}		

		g_pShop->GetItemInfo_byGoodsID(pGift->m_aGift[i].m_ui32GoodsID, &pGift->m_InvenItem[i]._ui32ItemID, &pGift->m_InvenItem[i]._ui32AddItemArg);// GoodsID 로 Goods 를 얻어온다.

		if ( 0 == pGift->m_InvenItem[i]._ui32ItemID)
		{
			i3String::Format(strLog, MAX_PATH, "GoodsID Not Exist GoodsID :%d ", pGift->m_aGift[i].m_ui32GoodsID);
			pGift->m_aGift[i].m_i32Rv = EVENT_ERROR_FAIL;
			return; // 해당 Goods가 없다
		}		

		if( GOODS_BUY_CASH == pGift->m_aGift[i].m_ui8BuyType)
		{
			
				S2DB::MakeQueryW(wstrSQL, L"usp_Inventory_ins",
					DB_DATA_TYPE_INT,		i32ThreadIdx,
					DB_DATA_TYPE_BIGINT,	pGift->m_TReceiverUID,
					DB_DATA_TYPE_INT,		pGift->m_InvenItem[i]._ui32ItemID,
					DB_DATA_TYPE_TINYINT,	ITEM_ATTR_TYPE_STAND_BY,
					DB_DATA_TYPE_INT,		pGift->m_InvenItem[i]._ui32AddItemArg,
					DB_DATA_TYPE_END);

				if (!pDataBase->ExecuteSP(wstrSQL))
				{
					_DataBaseResultFailLog(i32ThreadIdx, wstrSQL);
					pGift->m_aGift[i].m_i32Rv = EVENT_ERROR_DATABASE_EXECUTE_SP;
					continue;
				}
				if (1 != pDataBase->GetRecordCount())
				{
					_DataBaseResultFailLog(i32ThreadIdx, wstrSQL);	
					continue;
				}
				if (1 != pDataBase->GetINT32Data(0))
				{
					_DataBaseResultFailLog(i32ThreadIdx, wstrSQL);
					continue;
				}

				S2DB::MakeQueryW(wstrSQL, L"usp_Inventory_sel_ItemIdx",
					DB_DATA_TYPE_INT,		i32ThreadIdx,
					DB_DATA_TYPE_BIGINT,	pGift->m_TReceiverUID,
					DB_DATA_TYPE_INT,		pGift->m_InvenItem[i]._ui32ItemID,
					DB_DATA_TYPE_END);

				if (!pDataBase->ExecuteSP(wstrSQL))
				{
					_DataBaseResultFailLog(i32ThreadIdx, wstrSQL);
					pGift->m_aGift[i].m_i32Rv = EVENT_ERROR_DATABASE_EXECUTE_SP;
					continue;
				}
				INT32 i32Count = pDataBase->GetRecordCount();
				if (1 > i32Count)
				{
					_DataBaseResultFailLog(i32ThreadIdx, wstrSQL);	
					continue;
				}

				pGift->m_aGift[i].m_TGiftDBIdx	= pDataBase->GetINT32Data(0);
				pGift->m_aGift[i].m_i32Rv		= EVENT_ERROR_SUCCESS;

				i3String::Format(strLog, MAX_PATH, "Gift BUY INSERT STAND BY / CASH /  SendUID:%I64d recvUID : %i64d  GoodsID:%d",
					pGift->m_TSenderUID, pGift->m_TReceiverUID, pGift->m_aGift[i].m_ui32GoodsID);
				WRITE_SERVICE_LOG_DB(i32ThreadIdx, strLog);
			
		}
		else
		{
			pGift->m_aGift[i].m_i32Rv = _EXEC_ItemInsert(i32ThreadIdx, pGift->m_TReceiverUID, pGift);
		}
	}

}
INT32 CModuleDBShop::_EXEC_GiftBuyLink_Inven(INT32 i32ThreadIdx, DB_GIFT_OUT * pOut)
{
	T_ItemDBIdx	_TItemDBIdx;
	UINT8		_ItemState;
	UINT32		_AuthArg;
	UINT32		_NewAuthArg = 0;
	INT32		i32DeleteRowRv;
	SHOP_ITEM_DATA	*pItem = NULL;	

	CADODatabase	* pDataBase = m_ppADOdb_DATABASE[i32ThreadIdx];
	wchar_t wstrSQL[SERVER_STRING_COUNT];
	char strLog[MAX_PATH];	

	for (INT32 i = 0; i < pOut->m_ui8GiftCount; i++)
	{
		INVEN_BUFFER_DB* DB_Item = &pOut->m_SendGift.m_InvenItem[i];
		if (DB_Item == NULL) return EVENT_ERROR_EVENT_INSERT_ITEM_FAIL;

		if (EV_FAILED(pOut->m_aAuthGift[i].m_i32Rv))
		{
			i32DeleteRowRv = _EXEC_DeleteRow(i32ThreadIdx, pOut->m_SendGift.m_TReceiverUID, pOut->m_aAuthGift[i].m_TDBIdx);		
			if (EV_FAILED(i32DeleteRowRv))
			{	// 여기서 DB 작업이 실패하면, 추후 보상 대상에서 제외시켜야 하므로 로그 남깁니다.
				i3String::Format(strLog, MAX_PATH, "Gift BUY DELETE Link FAIL(LINK FAIL) / NO COMPENSATION / SenderUID:%I64d  RecverUID : %64d\n",
					pOut->m_SendGift.m_TSenderUID, pOut->m_SendGift.m_TReceiverUID);
				WRITE_SERVICE_LOG_DB(i32ThreadIdx, strLog);
			}
			pOut->m_aAuthGift[i].m_i32Rv = EVENT_ERROR_FAIL;
			continue;
		}
		
		g_pShop->GetItemInfo_byGoodsID(pOut->m_SendGift.m_aGift[i].m_ui32GoodsID, &DB_Item->_ui32ItemID, &DB_Item->_ui32AddItemArg);// GoodsID 로 Goods 를 얻어온다.

		pItem = g_pShop->GetItemByID(DB_Item->_ui32ItemID);
		if (NULL == pItem)
		{
			pOut->_i32Rv = EVENT_ERROR_EVENT_INSERT_ITEM_VALID_ITEM_ID;
			continue;
		}

	
		S2DB::MakeQueryW(wstrSQL, L"usp_Inventory_sel_checkItem",
			DB_DATA_TYPE_INT, i32ThreadIdx,
			DB_DATA_TYPE_BIGINT, pOut->m_SendGift.m_TReceiverUID,
			DB_DATA_TYPE_INT, DB_Item->_ui32ItemID,
			DB_DATA_TYPE_TINYINT, ITEM_ATTR_TYPE_STAND_BY,
			DB_DATA_TYPE_END);

		if (!pDataBase->ExecuteSP(wstrSQL))
		{
			ErrorDBShop(i32ThreadIdx, wstrSQL);
			pOut->_i32Rv = EVENT_ERROR_DATABASE_EXECUTE_SP;
			continue;
		}

		if (pDataBase->GetRecordCount() > 0)
		{//있음. 
			_TItemDBIdx = pDataBase->GetINT32Data(0);
			_ItemState = pDataBase->GetUINT8Data(1);
			_AuthArg = pDataBase->GetUINT32Data(2);

			// 기존 로우 UPDATE. 
			if (ITEM_EXPIRE_LEAVE == pItem->_ui8AuthExpireType || ITEM_ATTR_TYPE_BASIC == _ItemState || ITEM_ATTR_TYPE_BASIC == pItem->_ui8AuthType)
			{	// 기존에 소지한 아이템이 내구도 이거나 영구제이면 실패 처리 한다.
				pOut->_i32Rv = EVENT_ERROR_SHOP_ITEM_TYPE_ERROR;
			}

			switch (pItem->_ui8AuthType)
			{
			case ITEM_TYPE_COUNT: //카운트 
				_NewAuthArg = _AuthArg + DB_Item->_ui32AddItemArg;
				break;
			case ITEM_TYPE_TIME: //기간
				if (_ItemState == ITEM_ATTR_TYPE_BUY)			_NewAuthArg = _AuthArg + DB_Item->_ui32AddItemArg;							// 아직 인증이 안된 상태
				else if (i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI() > _AuthArg)	_NewAuthArg = g_pConfig->GetAuthTime(DB_Item->_ui32AddItemArg);				// 인증은 되었지만 기간이 지난상태
				else											_NewAuthArg = g_pConfig->GetAddAuthTime(_AuthArg, DB_Item->_ui32AddItemArg);	// 현재 인증중
				break;
			default: //Error
				pOut->_i32Rv = EVENT_ERROR_SHOP_ITEM_TYPE_ERROR;
				break;
			}
			if (EV_FAILED(pOut->_i32Rv)) continue;

			S2DB::MakeQueryW(wstrSQL, L"usp_Inventory_upd_ItemAuth",
				DB_DATA_TYPE_INT, i32ThreadIdx,
				DB_DATA_TYPE_INT, _NewAuthArg,
				DB_DATA_TYPE_BIGINT, pOut->m_SendGift.m_TReceiverUID,
				DB_DATA_TYPE_INT, _TItemDBIdx,
				DB_DATA_TYPE_END);

			if (!pDataBase->ExecuteSP(wstrSQL))
			{
				ErrorDBShop(i32ThreadIdx, wstrSQL);
				pOut->_i32Rv = EVENT_ERROR_DATABASE_EXECUTE_SP;
				continue;
			}

			if (1 != pDataBase->GetRecordCount())
			{
				_DataBaseResultFailLog(i32ThreadIdx, wstrSQL);
				pOut->_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
				continue;

			}
			if (1 != pDataBase->GetINT32Data(0))
			{
				_DataBaseResultFailLog(i32ThreadIdx, wstrSQL);
				pOut->_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
				continue;
			}

			// STAND BY 로우 DELETE, 
			_EXEC_DeleteRow(i32ThreadIdx, pOut->m_SendGift.m_TReceiverUID, pOut->m_aAuthGift[i].m_TDBIdx);
		}
		else	// 파츠구매 or 기존에 없으면 STAND BY 로우 UPDATE.
		{		
			_ItemState = ITEM_ATTR_TYPE_BUY;			
			if (NULL != pItem)
			{
				if (ITEM_TYPE_ETERNITY == pItem->_ui8AuthType)
				{	// 영구제 아이템일 경우 타입을 변경해 줍니다.
					_ItemState = ITEM_ATTR_TYPE_BASIC;					
				}
			}

			S2DB::MakeQueryW(wstrSQL, L"usp_Inventory_upd_ItemState",
				DB_DATA_TYPE_INT, i32ThreadIdx,
				DB_DATA_TYPE_BIGINT, pOut->m_SendGift.m_TReceiverUID,
				DB_DATA_TYPE_INT, pOut->m_aAuthGift[i].m_TDBIdx,
				DB_DATA_TYPE_TINYINT, _ItemState,
				DB_DATA_TYPE_END);

			if (!pDataBase->ExecuteSP(wstrSQL))
			{
				ErrorDBShop(i32ThreadIdx, wstrSQL);
				pOut->_i32Rv = EVENT_ERROR_DATABASE_EXECUTE_SP;
				continue;
			}

			if (1 != pDataBase->GetRecordCount())
			{
				_DataBaseResultFailLog(i32ThreadIdx, wstrSQL);
				pOut->_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
				continue;
			}
			if (1 != pDataBase->GetINT32Data(0))
			{
				_DataBaseResultFailLog(i32ThreadIdx, wstrSQL);
				pOut->_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
				continue;
			}

			S2DB::MakeQueryW(wstrSQL, L"usp_Inventory_sel_ItemIdx",
				DB_DATA_TYPE_INT, i32ThreadIdx,
				DB_DATA_TYPE_BIGINT, pOut->m_SendGift.m_TReceiverUID,
				DB_DATA_TYPE_INT, DB_Item->_ui32ItemID,
				DB_DATA_TYPE_END);
		
			if (1 > pDataBase->GetRecordCount())
			{
				_DataBaseResultFailLog(i32ThreadIdx, wstrSQL);
				pOut->_i32Rv = EVENT_ERROR_DATABASE_SP_RESULT;
				continue;
			}
			_TItemDBIdx = pOut->m_aAuthGift[i].m_TDBIdx;
			_NewAuthArg = DB_Item->_ui32AddItemArg;
		}	

		// pIn에 결과값을 담습니다.
		DB_Item->_TItemWareDBIdx = _TItemDBIdx;
		DB_Item->_ui8ItemType	= _ItemState;
		DB_Item->_ui32ItemArg	= _NewAuthArg;
		// ItemEnd	
	}	// Goods End
	return EVENT_ERROR_SUCCESS;
}

INT32 CModuleDBShop::_EXEC_ItemInsert(INT32 i32ThreadIdx, T_UID TUID, SEND_GIFT* pSendItem, UINT8 ui8JobAction, T_ItemDBIdx TItemDBIdx, UINT8 * pui8MultiSlotState, bool* pbAddCharaSlot)
{
	T_ItemDBIdx	_TItemDBIdx;
	UINT8		_ItemState;
	UINT32		_AuthArg;
	UINT32		_NewAuthArg = 0;

	SHOP_ITEM_DATA	*pItem = NULL;
	CADODatabase	* pDataBase = m_ppADOdb_DATABASE[i32ThreadIdx];
	wchar_t wstrSQL[SERVER_STRING_COUNT];
	char strLog[MAX_PATH];

	for (UINT8 i = 0; i < pSendItem->m_ui8GiftCount; i++)
	{
		INVEN_BUFFER_DB* pItemInfo = &pSendItem->m_InvenItem[i];
		if (NULL == pItemInfo) return EVENT_ERROR_EVENT_INSERT_ITEM_FAIL;

		pItem = g_pShop->GetItemByID(pItemInfo->_ui32ItemID);
		if (NULL == pItem)	return EVENT_ERROR_EVENT_INSERT_ITEM_VALID_ITEM_ID;		

		// 파츠라면 무조건 새로 INSERT.
		if (g_pConfig->IsPartsByItemID(pItemInfo->_ui32ItemID))
		{
			goto INSERT_FLAG;	// 기존에 있으나 없으나 상관없이 INSERT.
		}

		//1. 있는지 검색 
		S2DB::MakeQueryW(wstrSQL, L"usp_Inventory_sel_checkItem",
			DB_DATA_TYPE_INT, i32ThreadIdx,
			DB_DATA_TYPE_BIGINT, TUID,
			DB_DATA_TYPE_INT, pItemInfo->_ui32ItemID,
			DB_DATA_TYPE_TINYINT, ITEM_ATTR_TYPE_STAND_BY,
			DB_DATA_TYPE_END);
		DATABASE_EXECUTE_SP_ITEM;

		if (pDataBase->GetRecordCount() > 0)
		{//있음
			_TItemDBIdx = pDataBase->GetINT32Data(0);
			_ItemState = pDataBase->GetUINT8Data(1);
			_AuthArg = pDataBase->GetUINT32Data(2);


			if (ITEM_TYPE_CHARA == GET_ITEM_TYPE(pItem->_ui32ItemID) && ITEM_ATTR_TYPE_BASIC == _ItemState && ITEM_ATTR_TYPE_BASIC == pItem->_ui8AuthType)
			{	// 기존에 소지한 캐릭터가 영구제이면 INSERT
				goto INSERT_FLAG;	// 기존에 있으나 없으나 상관없이 INSERT.
			}

			// 기존에 소지한 아이템이 내구도 이거나 영구제이면 실패 처리 한다.
			if (ITEM_EXPIRE_LEAVE == pItem->_ui8AuthExpireType || ITEM_ATTR_TYPE_BASIC == _ItemState || ITEM_ATTR_TYPE_BASIC == pItem->_ui8AuthType)
			{
				return EVENT_ERROR_SHOP_ITEM_TYPE_ERROR;
			}

			switch (pItem->_ui8AuthType)
			{
			case ITEM_TYPE_COUNT: //카운트 
				_NewAuthArg = _AuthArg + pItemInfo->_ui32AddItemArg;
				break;
			case ITEM_TYPE_TIME: //기간
				if (_ItemState == ITEM_ATTR_TYPE_BUY)			_NewAuthArg = _AuthArg + pItemInfo->_ui32AddItemArg;								// 아직 인증이 안된 상태
				else if (i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI() > _AuthArg)	_NewAuthArg = g_pConfig->GetAuthTime(pItemInfo->_ui32AddItemArg);				// 인증은 되었지만 기간이 지난상태
				else											_NewAuthArg = g_pConfig->GetAddAuthTime(_AuthArg, pItemInfo->_ui32AddItemArg);	// 현재 인증중
				break;
			default: //Error
				return EVENT_ERROR_SHOP_ITEM_TYPE_ERROR;
				break;
			}

			S2DB::MakeQueryW(wstrSQL, L"usp_Inventory_upd_ItemAuth",
				DB_DATA_TYPE_INT, i32ThreadIdx,
				DB_DATA_TYPE_INT, _NewAuthArg,
				DB_DATA_TYPE_BIGINT, TUID,
				DB_DATA_TYPE_INT, _TItemDBIdx,
				DB_DATA_TYPE_END);

			DATABASE_EXECUTE_SP_ITEM;
			if (1 != pDataBase->GetRecordCount())
			{
				_DataBaseResultFailLog(i32ThreadIdx, wstrSQL);
				return  EVENT_ERROR_DATABASE_SP_RESULT;
			}
			if (1 != pDataBase->GetINT32Data(0))
			{
				_DataBaseResultFailLog(i32ThreadIdx, wstrSQL);
				return  EVENT_ERROR_DATABASE_SP_RESULT;
			}


		}
		else	// 없으면 INSERT.
		{
		INSERT_FLAG:
			switch (pItem->_ui8AuthType)
			{
			case ITEM_TYPE_COUNT: //카운트
				_ItemState = ITEM_ATTR_TYPE_BUY;
				_NewAuthArg = pItemInfo->_ui32AddItemArg;
				break;
			case ITEM_TYPE_TIME: //기간 
				if (pui8MultiSlotState)
				{
					*pui8MultiSlotState = MULTI_SLOT_STATE_UNUSE;
				}

				if (pItem->_ui8AutoStart == ITEM_AUTH_AUTO)
				{	// 자동 인증
					_ItemState = ITEM_ATTR_TYPE_AUTH;
					_NewAuthArg = g_pConfig->GetAuthTime(pItemInfo->_ui32AddItemArg); //기간 넣기
				}
				else
				{	// 수동 인증, 인증 못함
					_ItemState = ITEM_ATTR_TYPE_BUY;
					_NewAuthArg = pItemInfo->_ui32AddItemArg;
				}
				break;
			case ITEM_TYPE_ETERNITY:	// 영구
				_ItemState = ITEM_ATTR_TYPE_BASIC;
				_NewAuthArg = 1;
				break;
			default: //Error
				return  EVENT_ERROR_SHOP_ITEM_TYPE_ERROR;
				break;
			}

			S2DB::MakeQueryW(wstrSQL, L"usp_Inventory_ins",
				DB_DATA_TYPE_INT, i32ThreadIdx,
				DB_DATA_TYPE_BIGINT, TUID,
				DB_DATA_TYPE_INT, pItemInfo->_ui32ItemID,
				DB_DATA_TYPE_TINYINT, _ItemState,
				DB_DATA_TYPE_INT, _NewAuthArg,
				DB_DATA_TYPE_END);
			DATABASE_EXECUTE_SP_ITEM;
			if (1 != pDataBase->GetRecordCount())
			{
				_DataBaseResultFailLog(i32ThreadIdx, wstrSQL);
				return EVENT_ERROR_DATABASE_SP_RESULT;
			}
			if (1 != pDataBase->GetINT32Data(0))
			{
				_DataBaseResultFailLog(i32ThreadIdx, wstrSQL);
				return EVENT_ERROR_DATABASE_SP_RESULT;
			}

			S2DB::MakeQueryW(wstrSQL, L"usp_Inventory_sel_ItemIdx",
				DB_DATA_TYPE_INT, i32ThreadIdx,
				DB_DATA_TYPE_BIGINT, TUID,
				DB_DATA_TYPE_INT, pItemInfo->_ui32ItemID,
				DB_DATA_TYPE_END);

			DATABASE_EXECUTE_SP_ITEM;

			_TItemDBIdx = pDataBase->GetINT32Data(0);

			if (pbAddCharaSlot)
			{
				*pbAddCharaSlot = true;
			}
		}

		i3String::Format(strLog, MAX_PATH, "ITEM INSERT / Action:%d UID:%I64d ItemID:%d State:%d Arg:%d DBIdx:%d",
			ui8JobAction, TUID, pItemInfo->_ui32ItemID, _ItemState, _NewAuthArg, _TItemDBIdx);
		WRITE_SERVICE_LOG_DB(i32ThreadIdx, strLog);

		pItemInfo->_TItemWareDBIdx = _TItemDBIdx;
		pItemInfo->_ui8ItemType = _ItemState;
		pItemInfo->_ui32ItemArg = _NewAuthArg;
	}	// ItemEnd

	return EVENT_ERROR_SUCCESS;
}
