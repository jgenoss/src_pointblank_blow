#include "pch.h"
#include "ModuleDBNick.h"
#include "SystemTimer.h"

static UINT32 _DefThreadProcNick(void * pPointer)
{
	((CModuleDBNick*)pPointer)->Working(); 
	return 0; 
}

CModuleDBNick::CModuleDBNick()
{
	m_hThread				= NULL;
	m_bIsRunning			= false;

	m_pSystemTimer			= NULL;
	m_pADONick				= NULL;
	m_pRingNickNameIn		= NULL;
	m_pRingNickNameOut		= NULL;
}

CModuleDBNick::~CModuleDBNick()
{
	Destroy();
}

bool CModuleDBNick::Create()
{	

	//--------------------------------------------------------------------------------------------------------
	m_pSystemTimer	= new CSystemTimer;
	if( NULL == m_pSystemTimer )														return false;
	if( false == m_pSystemTimer->Create() )												return false;

	m_pSIADBConfig = g_pConfig->GetDatabaseConfig("PBUser");
	if( !m_pSIADBConfig )
		return false;

	//For NickName 
	m_pADONick = CADODatabase::NewObject(); 
	if( !m_pADONick->Connect(m_pSIADBConfig->GetDataSource(), m_pSIADBConfig->GetCatalog(), m_pSIADBConfig->GetAccount(), m_pSIADBConfig->GetPassword()) )
		return false; 

	m_pRingNickNameIn	= new S2RingBuffer;
	if( NULL == m_pRingNickNameIn )														return false;
	if( false == m_pRingNickNameIn->Create( sizeof(NICK_IN), DB_NICKNAME_COUNT ) )		return false;

	m_pRingNickNameOut = new S2RingBuffer;
	if( NULL == m_pRingNickNameOut )													return false;
	if( false == m_pRingNickNameOut->Create( sizeof(NICK_OUT), DB_NICKNAME_COUNT ) )	return false; 

	DWORD dwThreadID;
	m_hThread = CreateThread( NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProcNick , this, 0, &dwThreadID );
	if( NULL == m_hThread )																return false;
	if( FALSE == SetThreadPriority( m_hThread, THREAD_PRIORITY_NORMAL) )				return false;
	
	g_pLog->WriteLog( L"[SUCC] CREATE DATABASE WORKER NICKNAME"  ); 	

	return true;
}

void CModuleDBNick::Destroy()
{
	m_bIsRunning = false;
	::WaitForSingleObject( m_hThread, INFINITE );

	I3_SAFE_RELEASE( m_pADONick );
	I3_SAFE_DELETE( m_pRingNickNameIn );
	I3_SAFE_DELETE( m_pRingNickNameOut );
	I3_SAFE_DELETE( m_pSystemTimer );
}

void CModuleDBNick::Working(void)
{
	bool		bWorked;
	wchar_t		wstrSQL[SERVER_STRING_COUNT];
	NICK_IN*	pNickIn; 
	NICK_OUT*	pNickOut; 

	m_i32LogIdx = g_pLog->AddLogThread(L"NICK", 0, GetCurrentThreadId());

	m_bIsRunning = true;

	while( m_bIsRunning )
	{
		// Thread Timer
		m_pSystemTimer->RenewElapsedTime();

		bWorked = false;

		if( 0 == m_pRingNickNameIn->GetBufferCount() )
		{
			// 처리할 일이 없습니다. Continue;
			goto ERROR_NICK_BUFFER;
		}

		pNickIn	= (NICK_IN *)	m_pRingNickNameIn->Pop(); //종혁
		pNickOut= (NICK_OUT *)	m_pRingNickNameOut->PushPointer(); 
		if( NULL == pNickIn )
		{	// Buffer Full
			g_pLog->WriteLog( L"[ModuleDBNick] Ring In Buffer Full" );
			goto ERROR_NICK_BUFFER; 
		}
		if( NULL == pNickOut )
		{	// Buffer Full
			g_pLog->WriteLog( L"[ModuleDBNick] Ring Out Buffer Full" );
			goto ERROR_NICK_BUFFER;
		}

		pNickOut->pNode			= pNickIn->pNode;
		pNickOut->ui8Type		= pNickIn->ui8Type;
		pNickOut->i32Rv			= EVENT_ERROR_SUCCESS;

		// DB 에서 사용 할 수 있는 닉네임으로 변경
		TTCHAR strNick[CONVERT_STRING_COUNT];
		S2DB::ConvertString( strNick, CONVERT_STRING_COUNT, pNickIn->strNick );

		switch( pNickIn->ui8Type )
		{
		case TYPE_NICK_T_FIND		:
			{	// 사용 할 수 있는 닉네임인지 확인
				wchar_t wstrDate[ SMALLDATE_STRING_SIZE ];
				i3ThreadTimer::LocalTime().GetSmallDate( wstrDate, SMALLDATE_STRING_SIZE );

				S2DB::MakeQueryW( wstrSQL, L"usp_LogNickName_sel", 
					DB_DATA_TYPE_SMALLDATETIME,	wstrDate,											//    @sdtNow_        SMALLDATETIME
					DB_DATA_TYPE_TTCHAR,		pNickIn->strNick,									//,   @strUNickname_  NVARCHAR(50)
					DB_DATA_TYPE_END );

				if( FALSE == m_pADONick->ExecuteSP( wstrSQL ) )
				{
					ErrorDBNick(MODULE_NICK_DATABASE_THREAD_IDX, wstrSQL); 
					pNickOut->i32Rv	= EVENT_ERROR_DATABASE;
					goto ERROR_NICK_DATA;
				}
				// 닉네임이 존재하며
				if( 0 < m_pADONick->GetRecordCount() )
				{
					// 닉네임이 존재하면 무조건 중복체크 합니다.
					//T_UID T_UID = m_pADONick->GetINT64Data(0);
					//if( T_UID != pNickIn->m_T_UID )
					{
						pNickOut->i32Rv	= EVENT_ERROR_EVENT_DUPLICATE_NICK;
						goto ERROR_NICK_DATA;
					}
				}
				goto ERROR_NICK_DATA;
			}
			break;  
		case TYPE_NICK_T_CHANGE	: 
			{
				i3String::Copy( pNickOut->strOldNick, pNickIn->strOldNick, NET_NICK_NAME_SIZE );

				S2DB::MakeQueryW( wstrSQL, L"usp_Basic_upd_Nick", 
					DB_DATA_TYPE_BIGINT,		pNickIn->m_T_UID,						//    @i64UserUID_                BIGINT
					DB_DATA_TYPE_TTCHAR,		strNick,								//,   @strUNickname_              NVARCHAR(50)
					DB_DATA_TYPE_END );
			}
			break;
		case TYPE_NICK_T_GIFT	:
			{
				S2DB::ConvertString( strNick, CONVERT_STRING_COUNT, pNickIn->strNick );

				// 선물 받는 사람 닉네임으로 UID를 검색합니다. 
				S2DB::MakeQueryW( wstrSQL, L"usp_Basic_sel_ReceverUID",  
					DB_DATA_TYPE_TTCHAR,			strNick,		//,   @strUNickname_              NVARCHAR(50)
					DB_DATA_TYPE_END );

				if( FALSE == m_pADONick->ExecuteSP( wstrSQL ) )
				{
					ErrorDBNick(MODULE_NICK_DATABASE_THREAD_IDX, wstrSQL); 
					pNickOut->i32Rv	= EVENT_ERROR_DATABASE;
					goto ERROR_NICK_DATA;
				}

				if( 1 != m_pADONick->GetRecordCount() )
				{
					_DataBaseResultFailLog(MODULE_NICK_DATABASE_THREAD_IDX, wstrSQL); 
					pNickOut->i32Rv = EVENT_ERROR_DATABASE; 
					goto ERROR_NICK_DATA;
				}
				
				if( 1 != m_pADONick->GetINT32Data( 0 ) )
				{
					pNickOut->i32Rv = EVENT_ERROR_FIND_USER_DB; 
					goto ERROR_NICK_DATA;
				}

				pNickOut->m_TUID_Receiver = m_pADONick->GetINT64Data( 1 );

				if( 0 == pNickOut->m_TUID_Receiver )
				{
					pNickOut->i32Rv = EVENT_ERROR_FIND_USER_DB; 
					goto ERROR_NICK_DATA;
				}

				i3mem::Copy( &pNickOut->m_SendGift, &pNickIn->m_SendGift, sizeof(SEND_GIFT) );
			}
			break;
		default:
			{
				pNickOut->i32Rv = EVENT_ERROR_EVENT_DUPLICATE_NICK; 
				goto ERROR_NICK_DATA;
			}
			break;
		}

		if( TYPE_NICK_T_GIFT != pNickIn->ui8Type )
		{
			if( FALSE == m_pADONick->ExecuteSP( wstrSQL ) )
			{
				ErrorDBNick(MODULE_NICK_DATABASE_THREAD_IDX, wstrSQL); 
				pNickOut->i32Rv = EVENT_ERROR_DATABASE; 
				goto ERROR_NICK_DATA; 
			}
			if( 1 != m_pADONick->GetRecordCount() )
			{
				_DataBaseResultFailLog(MODULE_NICK_DATABASE_THREAD_IDX, wstrSQL); 
				pNickOut->i32Rv = EVENT_ERROR_DATABASE; 
				goto ERROR_NICK_DATA;
			}

			switch(  m_pADONick->GetINT32Data( 0 ) )
			{
			case TYPE_NICK_SP_NONE:
				{
					pNickOut->TItemWareIdx	= pNickIn->TItemWareIdx;
					i3String::Copy( pNickOut->strNick, pNickIn->strNick, sizeof(pNickOut->strNick) );
					pNickOut->strNick[ NET_NICK_NAME_SIZE-1 ]	='\0';

					// 닉네임 생성 or 변경시 여기서 노드에 카피한다.
					if( TYPE_NICK_T_CHANGE == pNickIn->ui8Type )
					{
						i3String::Copy( pNickIn->pNode->_sBasic.m_strNick , pNickIn->strNick, NET_NICK_NAME_SIZE );
						pNickIn->pNode->_sBasic.m_strNick[ NET_NICK_NAME_SIZE-1 ]	='\0';
					}
				}
				break;
			case TYPE_NICK_SP_USE:
				{
					pNickOut->i32Rv = EVENT_ERROR_EVENT_DUPLICATE_NICK; 
				}
				break;
			case TYPE_NICK_SP_FAIL:
				{
					//_ReConnectDataBase( MODULE_NICK_DATABASE_THREAD_IDX, strSQL );
					pNickOut->i32Rv = EVENT_ERROR_DATABASE;
				}
				break;

			case TYPE_NICK_SP_NOT_EXIST :	// 매개변수로 던진 자신의 현재(strOldNick) 닉네임이 DB에 없다.
				{	
					pNickOut->i32Rv = EVENT_ERROR_FIND_USER_DB;
				}
				break;
			default:
				{
					pNickOut->i32Rv = EVENT_ERROR_DATABASE;
				}
				break;
			}
		}

ERROR_NICK_DATA :

		m_pRingNickNameIn->PopIdx();
		m_pRingNickNameOut->PushPointerIdx(); 
		bWorked = true;

ERROR_NICK_BUFFER : 
		REAL32 r32Elapsed = m_pSystemTimer->RenewElapsedTime();
		if(CHECK_TIMEOUT < r32Elapsed )
		{
			g_pLog->WriteLog( L"[ModuleDBNick] DB Time Over : %d(ms)", (UINT32)(100*r32Elapsed) );
		}

		if( false == bWorked )			::WaitForSingleObject( m_hThread, 1 );
	}

	return; 
}

//-------------------------------------------------------------------------------
//For Nickname
INT32 CModuleDBNick::PushNickName(INT8 i8Type, USER_NODE* pNode, TTCHAR * strNickname, TTCHAR * strOldNickName, T_ItemDBIdx TItemWareIdx, SEND_GIFT* pstSendGift )
{
	NICK_IN * pNickIn = (NICK_IN *)m_pRingNickNameIn->PushPointer();
	if(pNickIn == NULL)			return EVENT_RETURN_ERROR_BUFFER; 
	
	pNickIn->ui8Type			= i8Type; 
	pNickIn->pNode				= pNode;
	pNickIn->m_T_UID			= pNode->_i64UID;
	pNickIn->TItemWareIdx		= TItemWareIdx;

	if( NULL != strNickname ) //받는 닉넴
	{
		i3String::Copy( pNickIn->strNick, strNickname, sizeof(TTCHAR) * NET_NICK_NAME_SIZE); 
		pNickIn->strNick[ NET_NICK_NAME_SIZE-1 ]		='\0';
	}

	if( NULL != strOldNickName ) //보낸 닉넴
	{
		i3String::Copy( pNickIn->strOldNick, strOldNickName, sizeof(TTCHAR) * NET_NICK_NAME_SIZE );
		pNickIn->strOldNick[ NET_NICK_NAME_SIZE-1 ] ='\0';
	}

	if ( TYPE_NICK_T_GIFT == i8Type )
	{
		i3mem::Copy( &pNickIn->m_SendGift, pstSendGift, sizeof(SEND_GIFT) );
	}

	pNode->AddReference();
	m_pRingNickNameIn->PushPointerIdx();

	return EVENT_RETURN_OK;  
}

void CModuleDBNick::ErrorDBNick(INT32 i32ThreadIdx, wchar_t* wstrSQL)
{
	g_pLog->WriteLog( L"[ErrorDBNick] %s, Error Code : %08x", wstrSQL, m_pADONick->GethrLastError() );
	
	HRESULT EventType = m_pADONick->ReConnect(
																	m_pSIADBConfig->GetDataSource(),
																	m_pSIADBConfig->GetCatalog(),
																	m_pSIADBConfig->GetAccount(),
																	m_pSIADBConfig->GetPassword()
																);
	
	switch( EventType )
	{
	case EVENT_ERROR_EVENT_LOG_IN_NOT_CONNECT_DB:		// 리커넥을 시도하였으나 실패 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_DB, L"[ErrorDBNick] RECONNECT FAILE" );
		break;
	case EVENT_ERROR_SUCCESS:							// 리커넥을 시도하여 성공 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_DB, L"[ErrorDBNick] RECONNECT SUCCEED" );
		break;
	case EVENT_ERROR_FAIL:								// 리커넥을 시도하지 않았을 경우
		
		break;
	}
}

void CModuleDBNick::_DataBaseResultFailLog( INT32 i32ThreadIdx, wchar_t* wstrSQL )
{
	g_pLog->WriteLog( L"[DB_ERROR] %s", wstrSQL ); 
}
