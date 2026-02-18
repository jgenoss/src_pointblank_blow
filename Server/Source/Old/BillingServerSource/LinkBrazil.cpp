#include "pch.h"
#include "LinkBrazil.h"
#include "TaskProcessor.h"

I3_CLASS_INSTANCE( CLinkBrazil, i3Thread );

CLinkBrazil::CLinkBrazil()
{
	m_bRunning = FALSE;
}

CLinkBrazil::~CLinkBrazil()
{
	OnDestroy();
}

BOOL CLinkBrazil::OnCreate( CRingBuffer* pRingLinkIn, CRingBuffer* pRingLinkOut )
{
	if( FALSE == CLinkDB::OnCreate( pRingLinkIn, pRingLinkOut, "BillingLink.udl" ) )	return FALSE;

	if( FALSE == i3Thread::Create( "Link_Brazil" ) )									return FALSE;

	return TRUE;
}

void CLinkBrazil::OnDestroy()
{
}

UINT32 CLinkBrazil::OnRunning( void* pUserData )
{
	BOOL bWorking;
	m_bRunning = TRUE;
	while( m_bRunning )
	{
		bWorking = FALSE;

		bWorking = _GetUserCash();
		bWorking = _SetUserCash();
		
		if( FALSE == bWorking )		::WaitForSingleObject( m_Handle, 100 );
	}
	return 0;
}

#define GET_COUNT	10
BOOL CLinkBrazil::_GetUserCash()
{
	char strQuery[ SQL_STRING_COUNT ];
	INT32	i32IdxCount = 0;
	TYPE_LINK_IN_BRAZIL	pLinkInBuffer[ GET_COUNT ];

	// Read Sql
	i3String::Format( strQuery, SQL_STRING_COUNT, " EXECUTE usp_tGetUserBuffer %d", GET_COUNT );
	if( FALSE == m_pDB->ExecuteSP( strQuery ) )
	{
		ReconnectDB( strQuery );
		return FALSE;
	}
	if( 0 == m_pDB->GetRecordCount() )
	{
		return FALSE;
	}

	for( INT32 i = 0 ; i < m_pDB->GetRecordCount() ; i++ )
	{
		pLinkInBuffer[ i32IdxCount ]._ui64Idx	= m_pDB->GetUINT64Data(0);
		pLinkInBuffer[ i32IdxCount ]._i64Cash	= m_pDB->GetINT64Data(1);		
		m_pDB->GetTextData( 2, pLinkInBuffer[ i32IdxCount ]._strID, MAX_ID_SIZE );
		pLinkInBuffer[ i32IdxCount ]._i8Type	= m_pDB->GetINT8Data(3);
		if(1 == pLinkInBuffer[ i32IdxCount ]._i8Type) { pLinkInBuffer[ i32IdxCount ]._i64Cash *= -1;  }	// 음수타입(type : 0(양수), 1(음수)) 이면 충전캐쉬(양수)를 차감캐쉬(음수)로 변경

		i32IdxCount++;
		m_pDB->MoveNext();
	}

	// Write Sql
	for( INT32 i = 0 ; i < i32IdxCount ; i++ )
	{
		TYPE_LINK_IN* pLinkIn = (TYPE_LINK_IN*)m_pRingLinkIn->PushPointer();
		if( pLinkIn )
		{
			i3String::Format( strQuery, SQL_STRING_COUNT, " EXECUTE usp_tSetUserBufferRead %I64d", pLinkInBuffer[i]._ui64Idx );
			if( FALSE == m_pDB->ExecuteSP( strQuery ) )
			{
				ReconnectDB( strQuery );
				continue;
			}
		
			pLinkIn->_ui64Idx		= pLinkInBuffer[i]._ui64Idx;
			pLinkIn->_i64Cash		= pLinkInBuffer[i]._i64Cash;
			i3String::Copy( pLinkIn->_strID, pLinkInBuffer[i]._strID, MAX_ID_SIZE );

			m_pRingLinkIn->PushPointerIdx();
		}
		else
		{
			WriteLog( m_i32LogThreadIdx, "[Link] LinkInsertCash Buffer Full" );
		}
	}

	return TRUE;
}

BOOL CLinkBrazil::_SetUserCash()
{
	INT32 i32PopCount = m_pRingLinkOut->GetBufferCount();
	if( 0 == i32PopCount )	return FALSE;
	
	TYPE_LINK_OUT* pLinkOut = (TYPE_LINK_OUT*)m_pRingLinkOut->Pop();
	if( pLinkOut )
	{
		char strQuery[ SQL_STRING_COUNT ];
		char strID[ SQL_STRING_COUNT ];
		ConvertString( strID, pLinkOut->_strID );

		i3String::Format( strQuery, SQL_STRING_COUNT, " EXECUTE usp_tsetusercash '%s',%I64d,%I64d", strID, pLinkOut->_i64Cash, pLinkOut->_ui64Idx );
		if( FALSE == m_pDB->ExecuteSP( strQuery ) )
		{
			ReconnectDB( strQuery );
		}

		m_pRingLinkOut->PopIdx();
	}
	else
	{
		WriteLog( m_i32LogThreadIdx, "[Link] LinkUpdateCash Buffer Crash" );
	}
		
	return TRUE;
}