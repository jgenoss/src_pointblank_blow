#include "pch.h"
#include "ModuleDBUserSave.h"

BOOL CModuleDBUserSave::_WorkingCreateNick( INT32 i32ThreadIdx )
{
	if( 0 == m_ppCreateNickIn[ i32ThreadIdx ]->GetBufferCount() )	return FALSE;

	RING_CREATE_NICK_IN* pRingIn	= (RING_CREATE_NICK_IN *)m_ppCreateNickIn[ i32ThreadIdx ]->Pop();
	RING_CREATE_NICK_OUT* pRingOut	= (RING_CREATE_NICK_OUT *)m_ppCreateNickOut[ i32ThreadIdx ]->PushPointer();
	if( NULL == pRingOut )
	{
		g_pLog->WriteLog( L"Create Nick Out Buffer Full" );
		return FALSE;
	}
	else if( NULL == pRingIn )
	{
		g_pLog->WriteLog( L"Create Nick In Buffer Not Find" );
		return FALSE;
	}

	pRingOut->m_TUID = pRingIn->m_TUID; 

	if( FALSE == _SaveCreateNick( i32ThreadIdx, pRingIn, pRingOut ) )
	{
		g_pLog->WriteLog( L"[Error]Create Nick Not Save" );
	}

	m_ppCreateNickIn[ i32ThreadIdx ]->PopIdx();
	m_ppCreateNickOut[ i32ThreadIdx ]->PushPointerIdx();

	return TRUE;
}

BOOL CModuleDBUserSave::_SaveCreateNick( INT32 i32Threadidx, RING_CREATE_NICK_IN * pIn, RING_CREATE_NICK_OUT* pOut )
{
	wchar_t wstrSQL[ SERVER_STRING_COUNT ];

	wchar_t wstrDate[ SMALLDATE_STRING_SIZE ];
	i3ThreadTimer::LocalTime().GetSmallDate( wstrDate, SMALLDATE_STRING_SIZE );

	// 닉네임 유무 검사
	S2DB::MakeQueryW( wstrSQL, L"usp_LogNickName_sel", 
		DB_DATA_TYPE_SMALLDATETIME,	wstrDate,												//    @sdtNow_        SMALLDATETIME
		DB_DATA_TYPE_TTCHAR,		pIn->m_strNick,											//,   @strUNickname_  NVARCHAR(50)
		DB_DATA_TYPE_END );

	if( FALSE == m_ppNickDB[i32Threadidx]->ExecuteSP( wstrSQL ) )
	{
		ErrorDBUserNick( i32Threadidx, wstrSQL );
		pOut->m_i32Result	= EVENT_ERROR_DATABASE;
		return FALSE;
	}
	// 닉네임이 존재하며
	if( 0 < m_ppNickDB[i32Threadidx]->GetRecordCount() )
	{
		// 내가 만들었던 닉네임이 아닐 경우는 중복 체크
		T_UID TUID = m_ppNickDB[i32Threadidx]->GetINT64Data(0);
		if( TUID != pIn->m_TUID )
		{
			pOut->m_i32Result	= EVENT_ERROR_EVENT_DUPLICATE_NICK;
			return FALSE;
		}
	}

	UINT32 ui32CreateDate	= i3ThreadTimer::LocalTime().GetDateTimeYYMMDDHHMI();
	UINT32 ui32Point		= g_pConfig->m_ui32NewUserPoint;

	// 유저 추가
	S2DB::MakeQueryW( wstrSQL, L"usp_Basic_ins", 
		DB_DATA_TYPE_BIGINT,		pIn->m_TUID,										//    @i64UserUID_        BIGINT
		DB_DATA_TYPE_TTCHAR,		pIn->m_strNick,										//,   @strUNickname_      NVARCHAR(50)
		DB_DATA_TYPE_SMALLDATETIME,	wstrDate,											//,   @sdtCreat_          SMALLDATETIME
		DB_DATA_TYPE_INT,			ui32Point,											//,   @i32Point_          INT
		DB_DATA_TYPE_END );

	if( FALSE == m_ppNickDB[i32Threadidx]->ExecuteSP( wstrSQL ) )
	{
		ErrorDBUserNick( i32Threadidx, wstrSQL ); 
		pOut->m_i32Result	= EVENT_ERROR_DATABASE; 
		return FALSE;
	}
	if( 1 != m_ppNickDB[i32Threadidx]->GetRecordCount() )
	{
		ErrorDBUserNick( i32Threadidx, wstrSQL ); 
		pOut->m_i32Result	= EVENT_ERROR_DATABASE; 
		return FALSE;
	}
	INT32 i32Result = (INT32)m_ppNickDB[i32Threadidx]->GetINT16Data(0);
	switch( i32Result )
	{
	case -1:
		{
			pOut->m_i32Result	= EVENT_ERROR_EVENT_DUPLICATE_NICK;	// 이미 생성된 계정입니다.
			return TRUE;
		}
		break;
	case -2:
		{
			pOut->m_i32Result	= EVENT_ERROR_EVENT_DUPLICATE_NICK;	// 중복된 닉네임 입니다.
			return TRUE;
		}
		break;
	}

	// 생성 완료.
	pOut->m_i32Result				= EVENT_ERROR_SUCCESS;
	pOut->m_ui32CreateDate			= ui32CreateDate;
	pOut->m_ui32CreatePoint			= ui32Point;
	i3String::Copy( pOut->m_strNick, pIn->m_strNick, NET_NICK_NAME_SIZE );

	return TRUE;
}

INT32 CModuleDBUserSave::PushCreateNick( RING_CREATE_NICK_IN* pCreateNick )
{		
	UINT32 i32WriteIdx = (UINT32)( pCreateNick->m_TUID % DATABASE_THREAD_COUNT );
	return m_ppCreateNickIn[ i32WriteIdx ]->Push( pCreateNick );
}

INT32 CModuleDBUserSave::GetCreateNickCount(void)
{
	INT32 i32Rv = 0; 
	for(INT32 i = 0; i < DATABASE_THREAD_COUNT; i++)
	{
		i32Rv += m_ppCreateNickOut[i]->GetBufferCount(); 
	}

	return i32Rv; 
}

RING_CREATE_NICK_OUT * CModuleDBUserSave::GetCreateNick(void)
{
	for(INT32 i = 0; i < DATABASE_THREAD_COUNT; i++)
	{
		m_i32CreateNickOutIdx++; 
		if( m_i32CreateNickOutIdx >= DATABASE_THREAD_COUNT )	m_i32CreateNickOutIdx = 0; 

		if( m_ppCreateNickOut[ m_i32CreateNickOutIdx ]->GetBufferCount() )
		{
			return ( RING_CREATE_NICK_OUT* )m_ppCreateNickOut[ m_i32CreateNickOutIdx ]->Pop(); 
		}
	}

	return NULL; 
}

void CModuleDBUserSave::GetCreateNickIdx(void)
{
	m_ppCreateNickOut[ m_i32CreateNickOutIdx ]->PopIdx(); 
}