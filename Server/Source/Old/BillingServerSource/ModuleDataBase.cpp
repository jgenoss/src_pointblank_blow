#include "pch.h"
#include "ModuleDataBase.h"
#include "RingBuffer.h"

I3_CLASS_INSTANCE( CModuleDataBase, i3Thread );

CModuleDataBase::CModuleDataBase(void)
{	
	m_bRunning			= FALSE;

	m_pDB				= NULL;

	m_pRingGetIn		= NULL;
	m_pRingGetOut		= NULL;
	m_pRingBuyIn		= NULL;
	m_pRingBuyOut		= NULL;
	m_pRingLinkIn		= NULL;
	m_pRingLinkOut		= NULL;

	m_i32LogThreadIdx	= LOG_THREAD_INVALID;
}

CModuleDataBase::~CModuleDataBase(void)
{
	OnDestroy(); 
}

BOOL CModuleDataBase::OnCreate( char* strFilePath, TYPE_RING_BUFFER_SET* pRingBufferSet )
{
	m_pRingGetIn		= pRingBufferSet->_pRingGetIn;
	m_pRingGetOut		= pRingBufferSet->_pRingGetOut;
	m_pRingBuyIn		= pRingBufferSet->_pRingBuyIn;
	m_pRingBuyOut		= pRingBufferSet->_pRingBuyOut;
	m_pRingLinkIn		= pRingBufferSet->_pRingLinkIn;
	m_pRingLinkOut		= pRingBufferSet->_pRingLinkOut;

	m_i32LogThreadIdx	= g_pLog->InsertLogBuffer();
	if( LOG_THREAD_INVALID == m_i32LogThreadIdx )						return FALSE;

	i3String::Format( m_strUdlFilePath, SERVER_STRING_COUNT, "File Name=%s\\%s;", g_pCCurrentPath, strFilePath );

	//TASK THREAD 
	m_pDB = CADODatabase::NewObject(); 
	if( FALSE == m_pDB->Connect( m_strUdlFilePath ) )					return FALSE;

	WriteLog( g_i32MainLogThreadIdx, "[INIT] DB UserCash Create Success" );
	
	if( FALSE == i3Thread::Create( "ModuleDataBase" ) )					return FALSE;
	return TRUE; 
}

void CModuleDataBase::OnDestroy(void)
{	
	m_bRunning = FALSE;
	I3_SAFE_RELEASE( m_pDB );
	return; 
}

UINT32 CModuleDataBase::OnRunning( void* pUserData )
{
	INT32 i32PopCount;
	TYPE_GET_CASH_IN*	pGetCashIn;
	TYPE_GET_CASH_OUT*	pGetCashOut;
	TYPE_ITEM_BUY_IN*	pItemBuyIn;
	TYPE_ITEM_BUY_OUT*	pItemBuyOut;
	TYPE_LINK_IN*		pLinkIn;

	m_bRunning = TRUE;
	while( m_bRunning )
	{
		// GetCash
		i32PopCount = m_pRingGetIn->GetBufferCount();
		if( 0 < i32PopCount )
		{
			pGetCashIn	= (TYPE_GET_CASH_IN*)m_pRingGetIn->Pop();
			pGetCashOut	= (TYPE_GET_CASH_OUT*)m_pRingGetOut->PushPointer();
			if( pGetCashIn && pGetCashOut )
			{
				_GetUserCash( pGetCashIn, pGetCashOut );

				m_pRingGetOut->PushPointerIdx();
				m_pRingGetIn->PopIdx();
			}
			else
			{
				if( NULL == pGetCashIn )	WriteLog( m_i32LogThreadIdx, "[DBAS] TYPE_GET_CASH_IN Buffer Full" );
				else						WriteLog( m_i32LogThreadIdx, "[DBAS] TYPE_GET_CASH_OUT Buffer Full" );
			}
		}
		// ItemBuy
		i32PopCount = m_pRingBuyIn->GetBufferCount();
		if( 0 < i32PopCount )
		{
			pItemBuyIn	= (TYPE_ITEM_BUY_IN*)m_pRingBuyIn->Pop();
			pItemBuyOut	= (TYPE_ITEM_BUY_OUT*)m_pRingBuyOut->PushPointer();
			if( pItemBuyIn && pItemBuyOut )
			{
				_ItemBuy( pItemBuyIn, pItemBuyOut );

				m_pRingBuyOut->PushPointerIdx();
				m_pRingBuyIn->PopIdx();
			}
			else
			{
				if( NULL == pItemBuyIn )	WriteLog( m_i32LogThreadIdx, "[DBAS] TYPE_ITEM_BUY_IN Buffer Full" );
				else						WriteLog( m_i32LogThreadIdx, "[DBAS] TYPE_ITEM_BUY_OUT Buffer Full" );
			}
		}
		// InsertCash
		i32PopCount = m_pRingLinkIn->GetBufferCount();
		if( 0 < i32PopCount )
		{
			pLinkIn		= (TYPE_LINK_IN*)m_pRingLinkIn->Pop();
			if( pLinkIn )
			{
				_InsertUserCash( pLinkIn );

				m_pRingLinkIn->PopIdx();
			}
			else
			{
				WriteLog( m_i32LogThreadIdx, "[DBAS] TYPE_LINK_IN Buffer Full" );
			}
		}

		if( 0 == i32PopCount)
		{
			Sleep(1);
		}		
	}
	return 0;
}

BOOL CModuleDataBase::_GetUserCash( TYPE_GET_CASH_IN* pRingIn, TYPE_GET_CASH_OUT* pRingOut )
{
	i3String::Copy( pRingOut->_strID, pRingIn->_strID, MAX_ID_SIZE );
	pRingOut->_i32ServerIdx		= pRingIn->_i32ServerIdx;
	pRingOut->_i64UID			= pRingIn->_i64UID;
	pRingOut->_i32ReturnCode	= RETURN_CODE_ERROR_SYSTEM;
	pRingOut->_i64Cash			= 0;

	char strQuery[ SQL_STRING_COUNT ];
	char strID[ SQL_STRING_COUNT ];
	ConvertString( strID, pRingIn->_strID );
	i3String::Format( strQuery, SQL_STRING_COUNT, "EXECUTE usp_GetCash '%s'", strID );
	if( FALSE == m_pDB->ExecuteSP( strQuery ) )
	{
		_ReConnectDataBase( strQuery );
		return FALSE;
	}
	if( 0 == m_pDB->GetRecordCount() )
	{	
		_ReConnectDataBase( strQuery );
		return FALSE;
	}

	pRingOut->_i32ReturnCode	= RETURN_CODE_SUCCESS;
	pRingOut->_i64Cash			= m_pDB->GetINT64Data(0);

	return TRUE;
}

BOOL CModuleDataBase::_ItemBuy( TYPE_ITEM_BUY_IN* pRingIn, TYPE_ITEM_BUY_OUT* pRingOut )
{
	i3String::Copy( pRingOut->_strID, pRingIn->_strID, MAX_ID_SIZE );
	pRingOut->_i32ServerIdx			= pRingIn->_i32ServerIdx;
	pRingOut->_ui32MainBufferIdx	= pRingIn->_ui32MainBufferIdx;
	pRingOut->_i64UID				= pRingIn->_i64UID;
	pRingOut->_i32ReturnCode		= RETURN_CODE_ERROR_SYSTEM;
	pRingOut->_i64Cash				= 0;

	char strQuery[ SQL_STRING_COUNT ];
	char strID[ SQL_STRING_COUNT ];
	ConvertString( strID, pRingIn->_strID );
	i3String::Format( strQuery, SQL_STRING_COUNT, "EXECUTE usp_ItemBuy '%s',%d", strID, pRingIn->_ui32Price );
	if( FALSE == m_pDB->ExecuteSP( strQuery ) )
	{
		_ReConnectDataBase( strQuery );
		return FALSE;
	}
	if( 0 == m_pDB->GetRecordCount() )
	{	
		_ReConnectDataBase( strQuery );
		return FALSE;
	}
	
	pRingOut->_i32ReturnCode = m_pDB->GetINT32Data(0);
	if( RETURN_CODE_SUCCESS != pRingOut->_i32ReturnCode )
	{	// 실패코드 추가
		pRingOut->_i32ReturnCode |= 0x80000000;
	}
	pRingOut->_i64Cash			= m_pDB->GetINT64Data(1);

	// CashUpdate
	_UpdateUserCash( pRingOut->_strID, pRingOut->_i64Cash );	

	return TRUE;
}

BOOL CModuleDataBase::_InsertUserCash( TYPE_LINK_IN* pRingIn )
{
	char strQuery[ SQL_STRING_COUNT ];
	char strID[ SQL_STRING_COUNT ];
	ConvertString( strID, pRingIn->_strID );
	i3String::Format( strQuery, SQL_STRING_COUNT, "EXECUTE usp_InsertCash '%s',%I64d", strID, pRingIn->_i64Cash );
	if( FALSE == m_pDB->ExecuteSP( strQuery ) )
	{
		_ReConnectDataBase( strQuery );
		return FALSE;
	}
	if( 0 == m_pDB->GetRecordCount() )
	{	
		_ReConnectDataBase( strQuery );
		return FALSE;
	}

	INT32 i32Result = m_pDB->GetINT32Data(0);
	if( RETURN_CODE_SUCCESS == i32Result )
	{
		INT64 i64Cash = m_pDB->GetINT64Data(1);

		_UpdateUserCash( pRingIn->_strID, i64Cash, pRingIn->_ui64Idx );
	}
	else
	{
		// 로그 남깁니다.
		
	}

	return TRUE;
}

BOOL CModuleDataBase::_UpdateUserCash( char* strID, INT64 i64Cash, UINT64 ui64Idx )
{
	// Update
	TYPE_LINK_OUT* pLinkOut	= (TYPE_LINK_OUT*)m_pRingLinkOut->PushPointer();
	if( pLinkOut )
	{
		i3String::Copy( pLinkOut->_strID, strID, MAX_ID_SIZE );
		pLinkOut->_ui64Idx	= ui64Idx;
		pLinkOut->_i64Cash	= i64Cash;
		m_pRingLinkOut->PushPointerIdx();
	}
	else
	{
		WriteLog( m_i32LogThreadIdx, "[DBAS] TYPE_LINK_OUT Buffer Full" );
		// 로그 남깁니다.
		// Update 용입니다.
	}
	return TRUE;
}

void CModuleDataBase::_ReConnectDataBase( char* strQuery )
{
	WriteLog( m_i32LogThreadIdx, "[DBAS] Query Fail : %s", strQuery );

	if( m_pDB->IsConnected() )
	{
		WriteLog( m_i32LogThreadIdx, "[DBAS] Connect" );
	}
	else
	{
		WriteLog( m_i32LogThreadIdx, "[DBAS] DisConnect" );
	}

	m_pDB->Close(); 
	if( m_pDB->Connect( m_strUdlFilePath ) )
	{
		WriteLog( m_i32LogThreadIdx, "[DBAS] Re Connect Success" );
	}
	else
	{
		WriteLog( m_i32LogThreadIdx, "[DBAS] Re Connect Fail" );
	}

	return; 
}
