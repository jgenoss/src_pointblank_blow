#include "i3CommonType.h"
#include "i3NetworkDB.h"
#include "i3String.h"

#define I3NETWORK_DB_STMTOPTION

I3_CLASS_INSTANCE( i3NetworkDataBase, i3ElementBase );

i3NetworkDataBase::i3NetworkDataBase()
{
	m_hEnv			= SQL_NULL_HENV;
	m_hDbc			= SQL_NULL_HDBC;  
	m_hStmt			= SQL_NULL_HSTMT;
	m_IsClean		= TRUE; 

	m_pDsn[0]		= '\0';
	m_pPasswd[0]	= '\0';
	m_pUserID[0]	= '\0';
	m_szTime[0]		= '\0';
}

i3NetworkDataBase::~i3NetworkDataBase()
{
	Disconnect(); 
}

char * i3NetworkDataBase::GetCurrentDate( void )
{
	SYSTEMTIME	st;
	::GetLocalTime( &st );
	sprintf( m_szTime, "%d-%d-%d %d:%d:%d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
	return m_szTime; 
}

BOOL i3NetworkDataBase::ReConnect(void)
{
	Disconnect(); 
	return Connect(m_pDsn, m_pUserID, m_pPasswd); 
}

BOOL i3NetworkDataBase::Connect(char* pDSN, char* pUserID, char* pPasswd)
{
	RETCODE ret;
	i3String::Copy(m_pDsn,	pDSN, 32);
	i3String::Copy(m_pUserID, pUserID, 32);
	i3String::Copy(m_pPasswd, pPasswd, 32);

	// Allocate the ODBC environment and save handle.
	ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &m_hEnv);
	if( (ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO ) )
	{
		GetError(SQL_HANDLE_ENV, m_hEnv, "SQLAllocHandle:m_hEnv" ); return FALSE;
	}

	// Notify ODBC that this is an ODBC 3.0 app.
	ret = SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, SQL_IS_INTEGER);
	if( (ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO ) )
	{
		GetError(SQL_HANDLE_ENV, m_hEnv, "SQLSetEnvAttr" ); return FALSE;
	}

	// Allocate ODBC connection handle and connect.
	ret = SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hDbc);
	if( (ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO ) )
	{
		GetError(SQL_HANDLE_DBC, m_hDbc, "SQLAllocHandle:m_hDbc" ); return FALSE;
	}
	
	//Connect
	ret = SQLConnect(m_hDbc, (SQLCHAR*)m_pDsn, SQL_NTS, (SQLCHAR*)m_pUserID, SQL_NTS, (SQLCHAR*)m_pPasswd, SQL_NTS);
	if( (ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO ) )
	{
		GetError(SQL_HANDLE_DBC, m_hDbc, "SQLConnect" ); return FALSE;
	}
	
	// Allocate statement handle.
	ret = SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hStmt);
	if( (ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO ) )
	{
		GetError(SQL_HANDLE_STMT, m_hStmt, "SQLAllocHandle:m_hStmt" ); return FALSE;
	}

	return TRUE; 
}

BOOL i3NetworkDataBase::Disconnect(void)
{	
	RETCODE ret;
	ret = SQLCloseCursor( &m_hStmt );
	if( (ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO ) )
	{
		GetError(SQL_HANDLE_STMT, m_hStmt, "SQLCloseCursor" ); return FALSE;
	}

	ret = SQLFreeHandle( SQL_HANDLE_STMT, &m_hStmt );
	if( (ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO ) )
	{	
		GetError(SQL_HANDLE_STMT, m_hStmt, "SQLFreeHandle:m_hStmt" ); return FALSE;
	}

	ret = SQLDisconnect( m_hDbc );
	if( (ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO ) )
	{	
		GetError(SQL_HANDLE_DBC, m_hDbc, "SQLDisconnect" ); return FALSE;
	}

	ret = SQLFreeHandle( SQL_HANDLE_DBC, &m_hDbc );
	if( (ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO ) )
	{	
		GetError(SQL_HANDLE_DBC, m_hDbc, "SQLFreeHandle:m_hDbc" ); return FALSE;
	}

	ret = SQLFreeHandle( SQL_HANDLE_ENV, &m_hEnv );
	if( (ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO ) )
	{
		GetError(SQL_HANDLE_ENV, m_hEnv, "SQLFreeHandle:m_hEnv" ); return FALSE;
	}

	return TRUE; 
}

INT32 i3NetworkDataBase::FetchData(void)
{	
	RETCODE ret = SQLFetch(m_hStmt);
	if( (ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO ) )
	{	
		GetError(SQL_HANDLE_STMT, m_hStmt, "SQLFetch" );
		return -1;
	}

	return 1;
}

INT32 i3NetworkDataBase::GetData(INT16 iColNo, INT16 iType, long lDataSize, void* pData)
{
	RETCODE ret;	
	SQLLEN ntd;
	ret = SQLGetData(m_hStmt, iColNo, iType, pData, lDataSize, &ntd);	
	if(  (ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO )  )
	{	
		GetError(SQL_HANDLE_STMT, m_hStmt, "SQLGetData" );
		return -1;
	}

	return 1;
}

INT16 i3NetworkDataBase::ResultColCount(void)
{
	RETCODE ret;
	INT16 ResultColCount;
	ret = SQLNumResultCols(m_hStmt, &ResultColCount); 
	if(  (ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO )  )
	{	
		GetError(SQL_HANDLE_STMT, m_hStmt, "SQLNumResultCols" );
		return -1;
	}
	return ResultColCount;  
}

SQLLEN i3NetworkDataBase::ResultRowCount(void)
{
	RETCODE ret;
	SQLLEN ResultRowCount; 	
	ret = SQLRowCount(m_hStmt, &ResultRowCount); 
	if(  (ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO )  )
	{	
		GetError(SQL_HANDLE_STMT, m_hStmt, "SQLRowCount" );
		return -1;
	}
	return ResultRowCount; 
}

BOOL i3NetworkDataBase::ExecDirect(char * pQuery)
{
	RETCODE ret;	

	if( FALSE == m_IsClean )	Clean(); 

	SetStmtOption( SQL_ATTR_QUERY_TIMEOUT, 1 );
	
	ret = SQLExecDirect(m_hStmt, (SQLCHAR*)(LPSTR)pQuery, SQL_NTS);
	if( (ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO ) )
	{	
		GetError(SQL_HANDLE_STMT, m_hStmt, "SQLExecDirect" ); return FALSE;
	}

	m_IsClean = FALSE; 
	return TRUE;
}

BOOL i3NetworkDataBase::Clean(void)
{
	RETCODE ret;	
	ret = SQLCloseCursor( m_hStmt );
	if( (ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO ) )
	{	
		GetError(SQL_HANDLE_STMT, m_hStmt, "SQLCloseCursor" ); return FALSE;
	}
	m_IsClean = TRUE; 
	return TRUE; 
}

/*이함수의 용도는? 
*/ 
BOOL i3NetworkDataBase::SetStmtOption( INT32 Option, UINT32 Value )
{
#if defined( I3NETWORK_DB_STMTOPTION )
	RETCODE	ret;

	ret = ::SQLSetStmtAttr( m_hStmt, (SQLINTEGER) Option, (SQLPOINTER) &Value, (SQLINTEGER) sizeof( UINT32 ) );
	if( (ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO ) )
	{	
		GetError(SQL_HANDLE_STMT, m_hStmt, "SetStmtOption" ); return FALSE;
	}
#endif
	return TRUE;
}


void i3NetworkDataBase::GetError( INT16 nHandleType, SQLHANDLE sqlhandle, const char * pPosition )
{	
	char	szSqlState[ 6 ];
	char	szErrorMsg[ SQL_MAX_MESSAGE_LENGTH ];
	INT32	NativeError;
	INT16	nMsgLen;

	INT32	iDiag;
	RETCODE	ret;

	GetCurrentDate();
	
	::SQLGetDiagField( nHandleType, sqlhandle, 0, SQL_DIAG_NUMBER,
		(SQLPOINTER) &iDiag, 0, (SQLSMALLINT*) &nMsgLen );

	if( pPosition != NULL )
		I3TRACE( "[i3NetworkDataBase::GetError] (%s) Posistion( %s ), Diagnostic count = %d, \n", m_szTime, pPosition, iDiag );
	else
		I3TRACE( "[i3NetworkDataBase::GetError] (%s) Diagnostic count = %d, \n", m_szTime, iDiag );
 
	for( INT32	i = 1; ; i++ )
	{
		ret = ::SQLGetDiagRec( nHandleType, sqlhandle, (SQLSMALLINT) i, (SQLCHAR*) szSqlState, (SQLINTEGER*) &NativeError,
			(SQLCHAR*) szErrorMsg, (SQLSMALLINT) sizeof( szErrorMsg ), (SQLSMALLINT*) &nMsgLen );

		if( ret == SQL_NO_DATA )		break;
		if( i > 10)						break;		//아무리 많아도 10개만 출력합니다. 

		I3TRACE( "\t%d) SQLSTATE:%s, NativeError:%d, Message:%s\n", i, szSqlState, NativeError, szErrorMsg );			
	}

	return;
}


