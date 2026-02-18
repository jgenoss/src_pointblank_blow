#include "pch.h"
#include "ModuleDBUID.h"

static UINT32 _DefThreadProcUID(void * pPointer)
{
	((CModuleDBUID*)pPointer)->OnWorking(); 
	return 0; 
}

CModuleDBUID::CModuleDBUID()
{
	m_pDB						= NULL;
	m_ppCommandIn_UID			= NULL;
	m_ppCommandOut_UID			= NULL;

	m_bIsRunning				= FALSE;
}

CModuleDBUID::~CModuleDBUID()
{
	OnDestroy();
}

BOOL CModuleDBUID::OnCreate()
{	

	m_pUserDBConfig = g_pConfig->GetDatabaseConfig("PBUser");
	if( !m_pUserDBConfig )
		return FALSE;

	//--------------------------------------------------------------------------------------------------------
	//For UID
	m_pDB = CADODatabase::NewObject(); 
	if( !m_pDB->Connect(m_pUserDBConfig->GetDataSource(), m_pUserDBConfig->GetCatalog(), m_pUserDBConfig->GetAccount(), m_pUserDBConfig->GetPassword()) )
		return FALSE;

	m_ppCommandIn_UID	= new S2RingBuffer;
	m_ppCommandIn_UID->Create( sizeof(UID_IN), DB_UID_BUFFER_COUNT ); 

	m_ppCommandOut_UID	= new S2RingBuffer;
	m_ppCommandOut_UID->Create( sizeof(UID_OUT), DB_UID_BUFFER_COUNT ); 

	DWORD dwThreadID;
	m_hThread = CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProcUID, this, 0, &dwThreadID);
	if( !SetThreadPriority(m_hThread, THREAD_PRIORITY_NORMAL) )
		return FALSE;
	
	g_pLog->WriteLog( L"[SUCC] CREATE DATABASE WORKER UID"  );

	return TRUE;
}

void CModuleDBUID::OnDestroy()
{
	m_bIsRunning = FALSE;
	::WaitForSingleObject( m_hThread, INFINITE );

	I3_SAFE_RELEASE( m_pDB ); 
	I3_SAFE_DELETE( m_ppCommandIn_UID ); 
	I3_SAFE_DELETE( m_ppCommandOut_UID ); 
}

void CModuleDBUID::OnWorking(void)
{
	BOOL		bWorked;
	wchar_t		wstrSQL[SERVER_STRING_COUNT];
	UID_IN*		pUidIn;
	UID_OUT*	pUidOut;
	UINT32		ui32Count;

	m_i32LogIdx = g_pLog->AddLogThread(L"UIDB", 0, GetCurrentThreadId());

	m_bIsRunning = TRUE; 

	while( m_bIsRunning )
	{
		bWorked = FALSE; 

		ui32Count = m_ppCommandIn_UID->GetBufferCount();
		//if(m_ppCommandIn_UID->GetBufferCount() > 0)
		for( UINT32 i = 0 ; i < ui32Count ; i++ )
		{
			pUidIn	= (UID_IN *)m_ppCommandIn_UID->Pop();
			pUidOut = (UID_OUT *)m_ppCommandOut_UID->PushPointer(); 
			if(pUidIn == NULL || pUidOut == NULL)
			{
				goto ERROR_UID_BUFFER; 
			}

			pUidOut->_i32Rv					= EVENT_ERROR_SUCCESS;
			pUidOut->_ui8Type				= pUidIn->_ui8Type;
			pUidOut->m_ui8PBVer				= pUidIn->m_ui8PBVer;
			pUidOut->_i64UID				= pUidIn->_i64UID;
			pUidOut->_ui64UserBillingIdx	= pUidIn->_ui64UserBillingIdx;
			pUidOut->m_ui32LoginTime		= pUidIn->m_ui32LoginTime;
			pUidOut->m_ui8LinkThreadIdx		= pUidIn->m_ui8LinkThreadIdx;
			//북미 필요
			pUidOut->_UniqueKey	= pUidIn->_UniqueKey;

			DWORD dwOldTime = GetTickCount();

			switch( pUidIn->_ui8Type )
			{			
			case TYPE_UID_LOAD:
				{
					pUidOut->_ui32ServerIdx		= pUidIn->_ui32ServerIdx; 
					pUidOut->_ui32SessionIdx	= pUidIn->_ui32SessionIdx;
					pUidOut->_i32AuthKey		= pUidIn->_i32AuthKey;
					pUidOut->m_ui8LocaleCode	= pUidIn->m_ui8LocaleCode;
					pUidOut->m_ui8LinkMethod	= pUidIn->m_ui8LinkMethod;

					LOGIN_IN_DB	LoginInDB;
					LoginInDB.m_TUID			= pUidIn->_i64UID;
					LoginInDB.m_i8UIDCreateType	= pUidIn->m_i8UIDCreateType;
					LoginInDB.m_ui32Birthday	= pUidIn->_birthday;
					i3String::Copy( LoginInDB.m_strId,	pUidIn->_strID,	NET_ID_SIZE );

					//Get Idx 
					pUidOut->_i32Rv = _GetUID_UIDTHREAD( &LoginInDB );

					if(pUidOut->_i32Rv < EVENT_ERROR_SUCCESS)
					{
						//계정블럭인 경우 블럭종료 일자를 보낸다.
						if (EVENT_ERROR_EVENT_LOG_IN_BLOCK_ACCOUNT == pUidOut->_i32Rv)
						{
							pUidOut->m_ui32BlockDate = LoginInDB.m_ui32BlockDate;
							pUidOut->m_i32PenaltyAmount = LoginInDB.m_i32PenaltyAmount;
							i3String::Copy(pUidOut->m_strUserNoticeComment, LoginInDB.m_strUserNoticeComment, BLOCK_COMMENT_SIZE);
						}

						//_ReConnectDataBase(MODULE_UID_DATA_THREAD_IDX, strSQL);
						goto ERROR_UID_DATA;
					}

					// 성공일 경우 필요한 값을 Out Buffer에 설정한다.
					pUidOut->_i64UID		= LoginInDB.m_TUID;
					pUidOut->_birthday		= LoginInDB.m_ui32Birthday;
					
					//Get Inventory;
					S2DB::MakeQueryW( wstrSQL, L"usp_AccountItemData_sel", 
					DB_DATA_TYPE_BIGINT,	pUidOut->_i64UID,							//    @iUSN_          BIGINT
					DB_DATA_TYPE_END );
					if( !m_pDB->ExecuteSP( wstrSQL ) )
					{
						ErrorDBUID( wstrSQL );
						pUidOut->_i32Rv = EVENT_ERROR_DATABASE;
						goto ERROR_UID_DATA;
					}

					pUidOut->_sInvenData.Reset();

					if( m_pDB->GetRecordCount() > 0 ) 
					{
						pUidOut->_sInvenData._ui8NickColor		= m_pDB->GetUINT8Data(0);
						pUidOut->_sInvenData._wRank				= m_pDB->GetUINT32Data(1);
						pUidOut->_sInvenData._wAprilFoolRank	= m_pDB->GetUINT32Data(2);
						m_pDB->GetTextData(3, pUidOut->_sInvenData._cNick, NET_NICK_NAME_SIZE);;
						pUidOut->_sInvenData._ui8CrossHairColor	= m_pDB->GetUINT8Data(4);
						pUidOut->_sInvenData._ui8ChattingColor	= m_pDB->GetUINT8Data(5);
						pUidOut->_sInvenData._ui16MuzzleFlashColor = m_pDB->GetUINT16Data(6);
					}

					pUidOut->_ui8Gender				= pUidIn->_ui8Gender;
					pUidOut->_pcCafe				= pUidIn->_pcCafe;
					pUidOut->_ui32PCCafeGUID		= pUidIn->_ui32PCCafeGUID;
					pUidOut->_ui32PCCafeBillingUID	= pUidIn->_ui32PCCafeBillingUID;
					pUidOut->_ui32Cash				= pUidIn->_ncCoin;

					//북미 필요
					pUidOut->_UniqueKey				= pUidIn->_UniqueKey;
					pUidOut->m_ui8Country			= pUidIn->m_ui8Country;
					pUidOut->m_i8LogOutSendType		= pUidIn->m_i8LogOutSendType;


					i3mem::Copy( pUidOut->m_strBillingID, pUidIn->m_strBillingID, BILLING_ID_SIZE );
					i3mem::Copy( pUidOut->m_strExtUID, pUidIn->m_strExtUID, EXT_UID_SIZE );
					i3mem::Copy( pUidOut->m_strWebLoginKey, pUidIn->m_strWebLoginKey, WEB_LOGINKEY_SIZE);
					i3String::Copy( pUidOut->_strID, pUidIn->_strID, sizeof(pUidOut->_strID) ); 
					i3String::Copy( pUidOut->_Custom, pUidIn->_Custom, sizeof(pUidOut->_Custom) );
					i3mem::Copy( pUidOut->m_aui8ShutDownTime, pUidIn->m_aui8ShutDownTime, sizeof(UINT8) * SHUTDOWN_TIME_SIZE );
				}
				break; 
			case TYPE_UID_BLOCK_USER:
				{
					TTCHAR strTempComment[CONVERT_STRING_COUNT];
					S2DB::ConvertString( strTempComment, CONVERT_STRING_COUNT, pUidIn->_strComment );

					S2DB::MakeQueryW( wstrSQL, L"usp_Account_upd_BlockMarking", 
						DB_DATA_TYPE_INT,		1,
						DB_DATA_TYPE_TTCHAR,	strTempComment,
						DB_DATA_TYPE_BIGINT,	pUidIn->_i64UID,
						DB_DATA_TYPE_END);
					if( !m_pDB->ExecuteSP(wstrSQL) )
					{
						ErrorDBUID( wstrSQL );
						pUidOut->_i32Rv = EVENT_ERROR_DATABASE;
						goto ERROR_UID_DATA; 
					}
					if( 1 != m_pDB->GetRecordCount() )
					{
						_DataBaseResultFailLog( wstrSQL );
						pUidOut->_i32Rv = EVENT_ERROR_DATABASE;
						goto ERROR_UID_DATA; 
					}
					if( 1 != m_pDB->GetINT32Data(0))
					{
						_DataBaseResultFailLog( wstrSQL );
						pUidOut->_i32Rv = EVENT_ERROR_DATABASE;
						goto ERROR_UID_DATA; 
					}
				}
				break;
			case TYPE_UID_BIRTHDAY_SET:
				{
					pUidOut->_ui32ServerIdx		= pUidIn->_ui32ServerIdx;
					pUidOut->_ui32SessionIdx	= pUidIn->_ui32SessionIdx;
					pUidOut->_i64UID			= pUidIn->_i64UID;

					S2DB::MakeQueryW( wstrSQL, L"usp_Account_upd_Birthday",
						DB_DATA_TYPE_BIGINT,		pUidIn->_i64UID,
						DB_DATA_TYPE_INT,			pUidIn->_birthday,
						DB_DATA_TYPE_END);
					if( !m_pDB->ExecuteSP( wstrSQL ) )
					{
						ErrorDBUID( wstrSQL );
						pUidOut->_i32Rv = EVENT_ERROR_DATABASE;
						goto ERROR_UID_DATA;
					}

					if( 1 != m_pDB->GetRecordCount() )
					{
						_DataBaseResultFailLog( wstrSQL );
						pUidOut->_i32Rv = EVENT_ERROR_DATABASE;
						goto ERROR_UID_DATA; 
					}
					if( 1 != m_pDB->GetINT32Data(0))
					{
						_DataBaseResultFailLog( wstrSQL );
						pUidOut->_i32Rv = EVENT_ERROR_DATABASE;
						goto ERROR_UID_DATA; 
					}

					pUidOut->_i32Rv				= EVENT_ERROR_SUCCESS;

					pUidOut->_birthday			= pUidIn->_birthday;
				}
				break;
			case TYPE_UID_DIRECT_PENALTY:
				{
					pUidOut->_ui32ServerIdx		= pUidIn->_ui32ServerIdx;
					pUidOut->m_i32SessionIdx	= pUidIn->m_i32SessionIdx;
					pUidOut->m_i8PenaltyType	= pUidIn->m_i8PenaltyType;
					pUidOut->m_i32PenaltyAmount	= pUidIn->m_i32PenaltyAmount;

					switch( pUidIn->m_i8PenaltyType )
					{
					case GMCHAT_PENALTY_CHAT:
						{
						S2DB::MakeQueryW( wstrSQL, L"usp_Basic_upd_ChatBlock",
								DB_DATA_TYPE_BIGINT,		pUidIn->_i64UID,
								DB_DATA_TYPE_INT,			pUidIn->m_i32PenaltyAmount,
								DB_DATA_TYPE_END);
						}
						break;
					case GMCHAT_PENALTY_BLOCK:
						{
						S2DB::MakeQueryW( wstrSQL, L"usp_Account_upd_DirectBlock",
								DB_DATA_TYPE_BIGINT,		pUidIn->_i64UID,
								DB_DATA_TYPE_INT,			pUidIn->m_i32PenaltyAmount,
							 // Block 관련 테이블 컬럼 변경으로 마지막 0이 Block으로 설정되면 주석 해제하기
							 // DB_DATA_TYPE_INT,  GMBlockType::GMCHAT_BLOCK_TYPE_BLOCKON,
								DB_DATA_TYPE_END);
						}
						break;
					default:
						goto ERROR_UID_DATA;
					}

					if( !m_pDB->ExecuteSP( wstrSQL ) )
					{
						ErrorDBUID( wstrSQL );
						pUidOut->_i32Rv = EVENT_ERROR_DATABASE;
						goto ERROR_UID_DATA;
					}

					if( 1 != m_pDB->GetRecordCount() )
					{
						_DataBaseResultFailLog( wstrSQL );
						pUidOut->_i32Rv = EVENT_ERROR_DATABASE;
						goto ERROR_UID_DATA; 
					}
					if( 1 != m_pDB->GetINT32Data(0))
					{
						_DataBaseResultFailLog( wstrSQL );
						pUidOut->_i32Rv = EVENT_ERROR_DATABASE;
						goto ERROR_UID_DATA; 
					}

					pUidOut->_i32Rv				= EVENT_ERROR_SUCCESS;
				}
				break;
			default: break; 
			}


ERROR_UID_DATA :
			DWORD dwNewTime = GetTickCount() - dwOldTime;
			if( dwNewTime > DEF_DB_LIMIT_TIME )
			{	// DB TIMEOVER
				g_pLog->WriteLog( L"[ModuleDBUID] DB Time Over : %d,  pUidIn->_ui8Type : %d ", dwNewTime, pUidIn->_ui8Type );
			}

			m_ppCommandIn_UID->PopIdx();
			m_ppCommandOut_UID->PushPointerIdx(); 
			bWorked = TRUE;
		}
ERROR_UID_BUFFER : 
		if( !bWorked ) Sleep(1);
	}

	return; 
}

INT32 CModuleDBUID::_GetUID_UIDTHREAD( LOGIN_IN_DB* pLoginInDB )
{
	wchar_t wstrSQL[SERVER_STRING_COUNT]; 
	
	wchar_t wstrID[ NET_ID_SIZE]; 

	i3String::CopyASCIItoWCHAR(wstrID, pLoginInDB->m_strId);
	

	switch( pLoginInDB->m_i8UIDCreateType )
	{
	case TYPE_UID_CREATE_INTER:
		{
			S2DB::MakeQueryW( wstrSQL, L"usp_Account_ins_UIDCreateInter_BirthDay", 
				DB_DATA_TYPE_TTCHAR,			wstrID,
				DB_DATA_TYPE_END );
		}
		break;
	case TYPE_UID_CREATE_EXTER:
		{
			S2DB::MakeQueryW( wstrSQL, L"usp_Account_ins_UIDCreateExter_BirthDay", 
				DB_DATA_TYPE_BIGINT,		pLoginInDB->m_TUID,
				DB_DATA_TYPE_TTCHAR,			wstrID,
				DB_DATA_TYPE_END );
		}
		break;
	}

	if( FALSE == m_pDB->ExecuteSP( wstrSQL ))
	{
		ErrorDBUID( wstrSQL );
		return EVENT_ERROR_DATABASE; 
	}
	if( 1 != m_pDB->GetRecordCount() )
	{
		_DataBaseResultFailLog( wstrSQL );
		return EVENT_ERROR_DATABASE; 
	}
	INT32 i32Error = m_pDB->GetINT32Data(0);
	switch( i32Error )
	{
	case 1:
	case 2:
		{
			pLoginInDB->m_TUID = m_pDB->GetINT64Data(1);
			DATE stBlockDate = m_pDB->GetDateData(2);
			UINT32 ui32BlockDate = S2DB::ConvertSmallDateToUINT32( stBlockDate );
			if( ui32BlockDate > i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI() )
			{
				m_pDB->GetTextData(3, pLoginInDB->m_strUserNoticeComment, BLOCK_COMMENT_SIZE);
				pLoginInDB->m_i32PenaltyAmount = m_pDB->GetINT32Data(4);

				//아직 BlockDate가 지나지 않은 경우 남은 시간으로 변환하여 Client에 전달합니다.
				//(Client와 서버시간이 다른 국가들이 있기때문)
				DATE32 stBlockDate32;
				stBlockDate32 = stBlockDate;
				ui32BlockDate = stBlockDate32.DiffTime( i3ThreadTimer::LocalTime() );
				pLoginInDB->m_ui32BlockDate = ui32BlockDate;
				return EVENT_ERROR_EVENT_LOG_IN_BLOCK_ACCOUNT;
			}		

			if (0 == pLoginInDB->m_ui32Birthday)
			{   // 로그인 프로세스( SIA LOGIN )에서 생일을 가져오지 않은 경우에 실행
				pLoginInDB->m_ui32Birthday = m_pDB->GetINT32Data(5);
			}
		}
		break;
	default:
		{
			_DataBaseResultFailLog( wstrSQL );
			return EVENT_ERROR_DATABASE;
		}
		break;
	}

	return EVENT_RETURN_OK; 
}

void CModuleDBUID::ErrorDBUID( wchar_t * wstrSQL)
{
	g_pLog->WriteLog( L"[ErrorDBUID] %s, Error Code : %08x", wstrSQL, m_pDB->GethrLastError() );
	
	HRESULT EventType = m_pDB->ReConnect(
																	m_pUserDBConfig->GetDataSource(),
																	m_pUserDBConfig->GetCatalog(),
																	m_pUserDBConfig->GetAccount(),
																	m_pUserDBConfig->GetPassword()
																);
	
	switch( EventType )
	{
	case EVENT_ERROR_EVENT_LOG_IN_NOT_CONNECT_DB:		// 리커넥을 시도하였으나 실패 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_DB, L"[ErrorDBUID] RECONNECT FAILE" );
		break;
	case EVENT_ERROR_SUCCESS:							// 리커넥을 시도하여 성공 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_DB, L"[ErrorDBUID] RECONNECT SUCCEED" );
		break;
	case EVENT_ERROR_FAIL:								// 리커넥을 시도하지 않았을 경우
		
		break;
	}
}

void CModuleDBUID::_DataBaseResultFailLog( wchar_t* wstrSQL)
{
	g_pLog->WriteLog( L"[DB_ERROR] %s", wstrSQL );  
}

INT32 CModuleDBUID::PushBirthDaySet(UINT32 iServerIdx, INT32 iSessionIdx, T_UID i64DbIdx, UINT32 ui32BirthDay)
{
	UID_IN * pUIDIn = (UID_IN *)PushUIDPT();

	if (NULL == pUIDIn) return EVENT_ERROR_FAIL;

	pUIDIn->_ui8Type		= TYPE_UID_BIRTHDAY_SET;
	pUIDIn->_ui32ServerIdx	= iServerIdx;
	pUIDIn->_ui32SessionIdx	= iSessionIdx;
	pUIDIn->_i64UID			= i64DbIdx;
	pUIDIn->_birthday		= ui32BirthDay;

	PushUIDPTIdx();

	return EVENT_ERROR_SUCCESS;
}