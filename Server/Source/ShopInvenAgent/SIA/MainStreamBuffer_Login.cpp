#include "pch.h"
#include "ModuleNet.h"
#include "MainStreamBuffer_Login.h"
#include "HashTable.h"
#include "ModuleTask.h"

CMainStreamBuffer_Login::CMainStreamBuffer_Login()
{
	m_i32TotalBufferCount	= 0;
	m_i32UseBufferCount		= 0;	
	m_i32BlankIdx			= 0;
	m_i32PushIdx			= 0;
	m_i32UpdateIdx			= 0;

	m_pi32BufferIndex		= NULL;	
	m_pBuffer				= NULL;
}

CMainStreamBuffer_Login::~CMainStreamBuffer_Login()
{
	I3_SAFE_DELETE_ARRAY( m_pi32BufferIndex );
	I3_SAFE_DELETE_ARRAY( m_pBuffer );
	delete m_pUIDTable;
}

static INT32 _UID_CompareProc( void* p1, void* p2 )
{
	T_UID i64UID1 = ((MAIN_STREAM_BUFFER_LOGIN*)p1)->m_LoginAck.m_TUID;
	T_UID i64UID2 = ((MAIN_STREAM_BUFFER_LOGIN*)p2)->m_LoginAck.m_TUID;

	if ( i64UID1 > i64UID2 )			return 1;
	else if ( i64UID1 < i64UID2 )		return -1;

	return 0;
}

INT32 CMainStreamBuffer_Login::CreateBuffer( INT32 i32Count )
{
	m_pi32BufferIndex = new INT32[ i32Count ];

	if( NULL == m_pi32BufferIndex )
		return -1;

	m_pBuffer = new MAIN_STREAM_BUFFER_LOGIN[ i32Count ];

	if( NULL == m_pBuffer )
		return -2;

	i3mem::FillZero( m_pBuffer, sizeof(MAIN_STREAM_BUFFER_LOGIN)*i32Count );

	for( INT32 i = 0; i < i32Count; i++ )
	{
		m_pi32BufferIndex[i] = i;
		m_pBuffer[i].m_i8Status = MAIN_STREAM_BUFFER_STATUS_EMPTY;
	}

	m_i32TotalBufferCount	= i32Count;
	m_i32UseBufferCount		= 0;

	// Make Hash Table 
	m_pUIDTable	= new CHashTable;
	if( NULL == m_pUIDTable )
		return -3;
	if( FALSE == m_pUIDTable->Create( MAX_CONNECT_USER_DIVISOR, _UID_CompareProc ) )
		return -4;

	return EVENT_ERROR_SUCCESS;
}

INT32 CMainStreamBuffer_Login::Push( UINT32 ui32ServerIdx, LOGIN_INPUT* pInput )
{
	if ( m_i32TotalBufferCount == m_i32UseBufferCount )
		return EVENT_FAIL_MAINSTREAMBUFFER_FULL;

	INT32 i32idx = _GetNextBufferIdx();

	MAIN_STREAM_BUFFER_LOGIN* pLogin = &m_pBuffer[i32idx];
	pLogin->m_i8Status			= MAIN_STREAM_BUFFER_STATUS_LOGINWORKER_SEND;
	pLogin->m_ui32ServerST		= i3ThreadTimer::GetServerTime() + TIME_OUT_SECOND_LOGINWORKER;
	pLogin->m_ui32ServerIdx		= ui32ServerIdx;

	ST_PACKET_LOGIN_ACK* pBuffer = &pLogin->m_LoginAck;
	pBuffer->m_i32Result		= EVENT_ERROR_EVENT_BUFFER_FULL;
	// vv3 - check
	pBuffer->m_ui8PBVer			= pInput->m_ui8PBVer;
	pBuffer->m_i32ServerIdx		= pInput->m_i32ServerIdx;
	pBuffer->m_i32SessionIdx	= pInput->m_i32SessionIdx;
	pBuffer->m_ui32AuthKey		= pInput->m_ui32AuthKey;
	pBuffer->m_ui32IP			= pInput->m_ui32IP;
	pBuffer->m_ui64UniqueKey	= pInput->m_ui64UniqueKey;
	pBuffer->m_ui8LinkThreadIdx = pInput->m_ui8LinkThreadIdx;
	pBuffer->m_ui8LocaleCode	= pInput->m_ui8LocaleCode;
	pBuffer->m_ui8Country		= pInput->m_ui8Country;
	i3mem::Copy( pBuffer->m_strID, pInput->m_strID, NET_ID_SIZE );

	return i32idx;
}
	
// 외부 연동 결과(로그인워커)를 버퍼에 반영
BOOL CMainStreamBuffer_Login::SetRecvResult( LOGIN_OUTPUT* pInput )
{
	INT32 i32BufferIdx = pInput->m_i32MainLoginBufferIdx;

	MAIN_STREAM_BUFFER_LOGIN* pMainBuffer = &m_pBuffer[i32BufferIdx];

	if( pMainBuffer->m_i8Status != MAIN_STREAM_BUFFER_STATUS_LOGINWORKER_SEND )
	{
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CMainStreamBuffer_Login::SetRecvResult_LOGIN_OUTPUT]Status Error Idx : %d, Status: %d",
			i32BufferIdx, pMainBuffer->m_i8Status );

		return FALSE;
	}
	
	// 1. 정보삽입
	ST_PACKET_LOGIN_ACK* pBuffer = &pMainBuffer->m_LoginAck;
	pBuffer->m_i32Result		= pInput->m_i32Result;
	pBuffer->m_ui8PBVer			= pInput->m_ui8PBVer;
	pBuffer->m_i8UIDCreateType	= pInput->m_i8UIDCreateType;
	pBuffer->m_i8LogOutSendType	= pInput->m_i8LogOutSendType;
	pBuffer->m_TUID				= pInput->m_TUID;
	pBuffer->m_i32Cash			= pInput->m_i32Cash;
	pBuffer->m_ui32Birthday		= pInput->m_ui32Birthday;
	pBuffer->m_ui32LoginTime	= pInput->m_ui32LoginTime;
	pBuffer->m_ui8LinkMethod	= pInput->m_ui8LinkMethod;
	pBuffer->m_ui64BillingIdx	= pInput->m_ui64BillingIdx;
	i3mem::Copy( pBuffer->m_strID, pInput->m_strID, NET_ID_SIZE );
	i3mem::Copy( pBuffer->m_strBillingID, pInput->m_strBillingID, BILLING_ID_SIZE );
	i3mem::Copy( pBuffer->m_aui8ShutDownTime, pInput->m_aui8ShutDownTime, SHUTDOWN_TIME_SIZE );
	i3mem::Copy( pBuffer->m_strExtUID, pInput->m_strExtUID, EXT_UID_SIZE );
	i3mem::Copy( pBuffer->m_strWebLoginKey, pInput->m_strWebLoginKey, WEB_LOGINKEY_SIZE );

	if ( LOCALE_CODE_NONE < pInput->m_ui8LocaleCode && pInput->m_ui8LocaleCode < LOCALE_CODE_MAX )
		pBuffer->m_ui8LocaleCode	= pInput->m_ui8LocaleCode;

	switch( pBuffer->m_i8UIDCreateType )
	{
	case TYPE_UID_CREATE_INTER:
		{
		}
		break;
	case TYPE_UID_CREATE_EXTER:
		{
			//로그인 결과가 성공일때만 HashTable에 넣어줍니다. (실패라면 UID도 모를뿐더러, PCCafe에 Push하지 않기때문에 필요없음.)
			if ( EV_SUCCESSED(pBuffer->m_i32Result) )
			{
				// 3. UID해쉬테이블에 UID추가
				if (FALSE == m_pUIDTable->Insert(pBuffer->m_TUID, &m_pBuffer[i32BufferIdx]))
				{
					return FALSE;
				}
			}
		}
		break;
	}

	if ( EV_SUCCESSED( pBuffer->m_i32Result ) )
	{
		// 2 - 1. PC카페워커에 인풋
		pMainBuffer->m_ui32ServerST = i3ThreadTimer::GetServerTime() + TIME_OUT_SECOND_PCCAFEWORKER;
		
		PCCAFE_INPUT PCCafeInput;
		PCCafeInput.m_eInputType	= TYPE_PCCAFE_INPUT_FIRSTLOGIN;
		PCCafeInput.m_ui32IP		= pBuffer->m_ui32IP;
		PCCafeInput.m_ui16GcaType	= pInput->m_ui16GcaType;

		switch( pBuffer->m_i8UIDCreateType )
		{
		case TYPE_UID_CREATE_INTER:
			{
				PCCafeInput.m_i32LoginBufferIdx = i32BufferIdx;
			}
			break;
		case TYPE_UID_CREATE_EXTER:
			{
				PCCafeInput.m_i32LoginBufferIdx	= -1;
				PCCafeInput.m_TUID				= pBuffer->m_TUID;
			}
			break;
		}
		g_pModuleTask->GetModulePCCafe()->PushData( &PCCafeInput );

		pMainBuffer->m_i8Status = MAIN_STREAM_BUFFER_STATUS_PCCAFEWORKER_SEND;
	}
	else
	{
		// 2 - 2. LoginAcK 전달
		SendResultData( i32BufferIdx );
	}

	return TRUE;
}

// 외부 연동 결과(PC카페워커)를 버퍼에 반영
BOOL CMainStreamBuffer_Login::SetRecvResult( PCCAFE_OUTPUT* pInput )
{
	//변경
	MAIN_STREAM_BUFFER_LOGIN* pBuffer = NULL;
	if ( -1 == pInput->m_i32LoginBufferIdx )
	{
		// TYPE_UID_CREATE_EXTER
		pBuffer = _FindByUID( pInput->m_TUID );
	}
	else
	{
		// TYPE_UID_CREATE_INTER
		pBuffer = &m_pBuffer[ pInput->m_i32LoginBufferIdx ];
	}

	if( NULL == pBuffer )
	{
		// write log 	
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CMainStreamBuffer_Login::SetRecvResult_PCCAFE_OUTPUT]pBuffer is null UID: %I64d / UseBufferCount : %d / LoginBufferIdx : %d / TotalBufferCount : %d ", 
		pInput->m_TUID, m_i32UseBufferCount, pInput->m_i32LoginBufferIdx, m_i32TotalBufferCount );

		return FALSE;
	}

	if( pBuffer->m_i8Status != MAIN_STREAM_BUFFER_STATUS_PCCAFEWORKER_SEND )
	{
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CMainStreamBuffer_Login::SetRecvResult_PCCAFE_OUTPUT]Status Error Status: %d/ UID: %I64d / UseBufferCount : %d / LoginBufferIdx : %d / TotalBufferCount : %d ", 
			pBuffer->m_i8Status, pInput->m_TUID, m_i32UseBufferCount, pInput->m_i32LoginBufferIdx, m_i32TotalBufferCount );

		return FALSE;
	}	
	
	// 2. 정보삽입
	pBuffer->m_LoginAck.m_ui8PCCafe				= pInput->m_ui8PCCafe;
	pBuffer->m_LoginAck.m_ui32PCCafeGUID		= pInput->m_ui32PCCafeGUID;
	pBuffer->m_LoginAck.m_ui32PCCafeBillingUID	= pInput->m_ui32PCCafeBillingUID;

	// 3. 로그인성공ACK 전달
	SendResultData( pBuffer - m_pBuffer );

	return TRUE;
}
// 타임아웃 체크용
BOOL CMainStreamBuffer_Login::UpdateBuffer()
{
	BOOL bWorking = FALSE;

	//변경
	INT32 i = m_i32UpdateIdx;
	for( INT32 i32Loop = 0; i32Loop < MAX_COUNT_PROCESS_LOGIN_BUFFER_IN_FRAME; i32Loop++ )
	{
		switch( m_pBuffer[i].m_i8Status )
		{
		// 로그인워커에 요청한 상태. 타임아웃 체크
		case MAIN_STREAM_BUFFER_STATUS_LOGINWORKER_SEND:
			{
				if ( m_pBuffer[i].m_ui32ServerST < i3ThreadTimer::GetServerTime() )
				{
					g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CMainStreamBuffer_Login::UpdateBuffer]Time out of Process by LoginWorker. Idx : %d", i );

					// 로그인실패ACK을 보낸다.
					bWorking = SendResultData( i );
				}
			}
			break;
		// PC카페워커에 요청한 상태. 타임아웃 체크
		case MAIN_STREAM_BUFFER_STATUS_PCCAFEWORKER_SEND:
			{
				if ( m_pBuffer[i].m_ui32ServerST < i3ThreadTimer::GetServerTime() )
				{
					g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CMainStreamBuffer_Login::UpdateBuffer]Time out of Process by PCCafeWorker. Idx : %d", i );

					// 비PC방인 상태로 로그인성공ACK을 보낸다.
					m_pBuffer[i].m_LoginAck.m_ui8PCCafe = PC_NOT_CAFE;
					bWorking = SendResultData( i );
				}
			}
			break;
		}
		if ( ++i >= m_i32TotalBufferCount )
			i = 0;
	}
	m_i32UpdateIdx = i;

	return bWorking;
}

BOOL CMainStreamBuffer_Login::SendResultData( INT32 i32BufferIdx )
{
	if (i32BufferIdx < 0 || i32BufferIdx > m_i32TotalBufferCount)
	{
		return FALSE;
	}

	MAIN_STREAM_BUFFER_LOGIN * pLogin = &m_pBuffer[i32BufferIdx];
	if( !pLogin ) return FALSE;
	
	// 로그인 결과를 트랜스서버에 전송한다.
	NET_PACKET_BUFFER pBuffer( PROTOCOL_MASK_LOGIN );
	pBuffer.WriteData( &pLogin->m_LoginAck, sizeof( ST_PACKET_LOGIN_ACK ) );
	g_pModuleNet->SendPush( pLogin->m_ui32ServerIdx, &pBuffer );

	// 버퍼를 비운다.
	_SetBlankBuffer( i32BufferIdx );

	return TRUE;
}

INT32 CMainStreamBuffer_Login::_GetNextBufferIdx()
{
	INT32 i32Idx = m_pi32BufferIndex[ m_i32PushIdx ];

	m_i32PushIdx++;
	m_i32PushIdx %= m_i32TotalBufferCount;

	m_i32UseBufferCount++;

	return i32Idx;
}

void CMainStreamBuffer_Login::_SetBlankBuffer( INT32 i32BufferIdx )
{
	//변경
	// UID가 바이너리리스트에 들어있다면, 제거합니다.
	if ( TYPE_UID_CREATE_EXTER == m_pBuffer[i32BufferIdx].m_LoginAck.m_i8UIDCreateType )
	{
		T_UID i64UID = m_pBuffer[i32BufferIdx].m_LoginAck.m_TUID;
		MAIN_STREAM_BUFFER_LOGIN* pBuffer = _FindByUID( i64UID );
		if( pBuffer )
			m_pUIDTable->Delete( i64UID, pBuffer );
	}

	m_pi32BufferIndex[m_i32BlankIdx] = i32BufferIdx;
	i3mem::FillZero( &m_pBuffer[i32BufferIdx], sizeof( MAIN_STREAM_BUFFER_LOGIN ) );

	m_i32BlankIdx++;
	m_i32BlankIdx %= m_i32TotalBufferCount;
	m_i32UseBufferCount--;
}

//변경
MAIN_STREAM_BUFFER_LOGIN* CMainStreamBuffer_Login::_FindByUID( T_UID i64UID )
{
	if( 0 == m_i32UseBufferCount )		return NULL;
	if( 0 > i64UID )					return NULL;
	
	MAIN_STREAM_BUFFER_LOGIN Tmp;
	Tmp.m_LoginAck.m_TUID = i64UID;
	i3BinList* pList = m_pUIDTable->Retrieve( i64UID );
	if( NULL == pList )					return NULL;

	INT32 i32Idx = pList->FindItem( &Tmp );
	if( 0 > i32Idx )		return NULL;

	return (MAIN_STREAM_BUFFER_LOGIN*)pList->GetItem( i32Idx );
}