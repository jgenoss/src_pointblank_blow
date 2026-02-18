#ifndef _ADO_H__
#define _ADO_H__

#define __USE_NEW_DB__
#include <ATLComTime.h>
#include "S2DB.h"


//#pragma warning( disable : 4146 )  // Disable warning messages
//#import <msado15>  no_namespace no_implementation rename("EOF", "adoEOF")
//#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF", "adoEOF")
#import ".\msado60_Backcompat_x64.tlb" no_namespace rename("EOF", "adoEOF")

//#pragma warning(push)
//#pragma warning(disable:4146)
//#import "C:\Program Files\Common Files\System\ADO\msado15.dll" no_namespace rename("EOF", "EndOfFile")
//#pragma warning(pop)

/*-----------------------------------------------------------------------
Name : CADODatabase
2007 03 06 : 버그확인 : GetComError(_com_error& err, _ConnectionPtr pConn) <- 이함수에서 의문점 발생 잠시 막음

-----------------------------------------------------------------------*/

class CADODatabase : public i3ElementBase
{
	I3_CLASS_DEFINE( CADODatabase );
protected:
	_ConnectionPtr	m_pConnection;		// ADO Connetion 객체
	_RecordsetPtr	m_pRecordSet;		// ADO RecordSet 객체
    //_CommandPtr		m_pCommand;			// ADO Command 객체

	long			m_nRecordCount;

	BOOL			m_bConnection;		// Mess 크래쉬로 인한 땜빵코드입니다.
	//char 			m_strConnect[SERVER_STRING_COUNT];
	//char 			m_strError[SERVER_STRING_COUNT];

	BOOL			m_bCursorLocation_adUserClientFlag; // CursorLocation Type 설정에 대한 Flag.
	char			m_strLastError[ 1024 ];
	HRESULT			m_hrLastError;

public:
	CADODatabase();
	virtual ~CADODatabase();

	void Create(BOOL bCurserTypeFlag = FALSE);

	BOOL IsConnected(void)
	{
		if (m_pConnection)
		{
			if (m_pConnection->GetState() == adStateOpen) return TRUE;
		}
		return FALSE;
	}

	//LPCSTR			GetConnenctionVersion();

	BOOL			BeginTrans();
	BOOL			CommitTrans();
	BOOL			RollbackTrans();

	BOOL			Close(void);
	BOOL			IsEOF(void);
	BOOL			Connect(char * strConnectFile);
	BOOL			Connect( char* strDataSource, char* strCatalog, char* strID, char* strPW, char* pErrMsg = NULL );
	HRESULT			ReConnect(char * strConnectFile);
	HRESULT			ReConnect( char* strDataSource, char* strCatalog, char* strID, char* strPW, char* pErrMsg = NULL );
	BOOL			Execute(char * strSql);
	BOOL			ExecuteQuery(char * strSql, BOOL IsLock = TRUE);
	BOOL			ExecuteSP(char * strSql);
	BOOL			ExecuteSP(wchar_t * strSql);
	BOOL			ExecuteSP(wchar_t * strSql, long CommandTimeOut);

	BOOL			MoveFirst(void);
	BOOL			Move(long nMove);
	BOOL			MoveNext(void);

	long			GetRecordCount();	   
	BOOL			GetboolData(long nFieldIndex);

	INT8			GetINT8Data(long nFieldIndex); 
	UINT8			GetUINT8Data(long nFieldIndex); 
	INT16			GetINT16Data(long nFieldIndex);
	UINT16			GetUINT16Data(long nFieldIndex);
	INT32			GetINT32Data(long nFieldIndex);
	UINT32			GetUINT32Data(long nFieldIndex);
	INT64			GetINT64Data(long nFieldIndex);
	UINT64			GetUINT64Data(long nFieldIndex);

	long			GetLongData(long nFieldIndex);
	float			GetFloatData(long nFieldIndex);
	DATE			GetDateData(long nFieldIndex);
	double			GetDoubleData(long nFieldIndex);

	void			GetTextData(long nFieldIndex, char * pString, INT32 Maxlen);
	void			GetTextData(long nFieldIndex, WCHAR * pString, INT32 Maxlen);
	void			GetBinaryData(long nFieldIndex, void * pData, INT32 Maxlen);

	BOOL			SetINT32Data(long nFieldIndex, INT32 data);
	BOOL			SetINT64Data(long nFieldIndex, INT64 data);
	BOOL			SetBinaryData( long nFieldIndex, void * pData, INT32 Length ); 

	void			GetComError(_com_error& err, _ConnectionPtr pConn);

	_RecordsetPtr	GetRecordSet() { return m_pRecordSet; }

	char*			GetLastError()					{	return m_strLastError;	}
	HRESULT			GethrLastError()				{	return m_hrLastError;	}
	bool			IsNeedConnect();
};

#endif