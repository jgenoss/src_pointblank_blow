#ifndef __I3_NETWORK_ODBC_H
#define __I3_NETWORK_ODBC_H

///////////////////////////////////////////////////////////////////
// SQL ODBC  by 정순구 2006. 5. 3
// ODBC 접속 관리
#include <sql.h>
#include <sqlext.h>

class I3_EXPORT_NETWORK i3NetworkDataBase : public i3ElementBase
{
	I3_CLASS_DEFINE( i3NetworkDataBase );
protected:	
	SQLHENV		m_hEnv;
	SQLHDBC		m_hDbc;
	SQLHSTMT	m_hStmt;

	char		m_pDsn[32];
	char		m_pUserID[32];
	char		m_pPasswd[32];
	char		m_szTime[32];
	BOOL		m_IsClean; 

public:
	i3NetworkDataBase();
	virtual ~i3NetworkDataBase();

	//Connect , DisConnect 
	BOOL	ReConnect(void); 
	BOOL	Connect(char* pDSN, char* pUserID, char* pPasswd);
	BOOL	Disconnect(void); 

	//Query 
	BOOL	ExecDirect(char * pQuery); 
	BOOL	Clean(void);
	INT32	FetchData(void);

	//Get Result 
	INT32	GetData(INT16 iColNo, INT16 iType, long lDataSize, void* pData); 
	INT16	ResultColCount(void);		
	long	ResultRowCount(void);		

	char *	GetCurrentDate( void );		//날자 가지고 오기 ..여기에 꼬옥 필요없다. 

	// Option
	BOOL	SetStmtOption( INT32 Option, UINT32 Value );

	// Debug
	void	GetError( INT16 nHandleType, SQLHANDLE sqlhandle, const char * pPosition );
};

#endif	// __I3_ODBC_H
