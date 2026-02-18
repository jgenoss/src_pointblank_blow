#ifndef _ADO_H__
#define _ADO_H__

#pragma warning( disable : 4251 )  // Disable warning messages
//#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF", "adoEOF")
#import ".\msado60_Backcompat_x64.tlb" no_namespace rename("EOF", "adoEOF")

#define DEF_CONNECT_TIME_OUT	15		// DB Connect 제한시간(Timeout)(Default : 15)
#define DEF_COMMAND_TIME_OUT	5		// 쿼리실행 제한시간(Timeout)(Default :30)

/*-----------------------------------------------------------------------
Name : CADODatabase
2007 03 06 : 버그확인 : GetComError(_com_error& err, _ConnectionPtr pConn) <- 픽스됨

-----------------------------------------------------------------------*/

//////////////////////////////////////////////////////////////////////////
// Data Size
#define STRING_SMALLDATE_SIZE		17		// SmallDate의 문자열 자릿수

//////////////////////////////////////////////////////////////////////////

class EXPORT_BASE NSM_cADODatabase
{	
protected:
	   _ConnectionPtr	m_pConnection;			// ADO Connetion 객체
	  _RecordsetPtr		m_pRecordSet;			// ADO RecordSet 객체

	   long		m_nRecordCount;

	   BOOL			m_bCursorLocation_adUserClientFlag; // CursorLocation Type 설정에 대한 Flag.

public:
	   NSM_cADODatabase();
	   virtual ~NSM_cADODatabase();

	   char m_strError[1024];

	   void Create(BOOL bCurserTypeFlag = FALSE);

	   bool IsConnected(void)
	   {
			if (m_pConnection)
			{
				if (m_pConnection->GetState() == adStateOpen) return TRUE;
			}
			return FALSE;
	   }
		
		BOOL	Close(void);		
		BOOL	Connect(char * strPath);
		BOOL	Connect( char* strDataSource, char* strCatalog, char* strID, char* strPW );
		BOOL	Execute(char * strSql);
		BOOL	ExecuteQuery(char * strSql, BOOL IsLock = TRUE);
		BOOL	ExecuteSP(char * strSql);

		BOOL	BeginTrans();
		BOOL	CommitTrans();
		BOOL	RollbackTrans();

		BOOL	MoveFirst(void);
		BOOL	Move(long nMove);
		BOOL	MoveNext(void);
		BOOL	IsEOF(void);

		long	GetRecordCount();	   
		BOOL	GetboolData(long nFieldIndex);

		INT8	GetINT8Data(long nFieldIndex); 
		UINT8	GetUINT8Data(long nFieldIndex); 
		INT16	GetINT16Data(long nFieldIndex);
		UINT16	GetUINT16Data(long nFieldIndex);
		INT32	GetINT32Data(long nFieldIndex);
		UINT32	GetUINT32Data(long nFieldIndex);
		INT64	GetINT64Data(long nFieldIndex);
		UINT64	GetUINT64Data(long nFieldIndex);

		long	GetLongData(long nFieldIndex);
		float	GetFloatData(long nFieldIndex);
		DATE	GetDateData(long nFieldIndex);
		double	GetDoubleData(long nFieldIndex);
		void	GetTextData(long nFieldIndex, char * pString, int Maxlen);	

		BOOL	SetBinaryData( long nFieldIndex, void * pData, int Length );
		void	GetBinaryData( long nFieldIndex, void * pData, int Maxlen );

		BOOL	SetintData(long nFieldIndex, int data);

		void	GetComError(_com_error& err, _ConnectionPtr pConn);

		void	ConvertString( char * pOutStr, INT32 i32Size, const char * pInStr, BOOL TYPE = FALSE );

		void	ConvertUINT32ToSmallDate( char*	strSmallDateTime, INT32 i32Lenth, UINT32 ui32SmallDateTime );
		UINT32	ConvertSmallDateToUINT32(DATE dateData);		
};

#endif


//#pragma warning( disable : 4146 )  // Disable warning messages
//#import <msado15>  no_namespace no_implementation rename("EOF", "adoEOF")
//#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF", "adoEOF")

//#pragma warning(push)
//#pragma warning(disable:4146)
//#import "C:\Program Files\Common Files\System\ADO\msado15.dll" no_namespace rename("EOF", "EndOfFile")
//#pragma warning(pop)


