#include "pch.h"
#include "Ado.h"
#include <ATLComTime.h>

//#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF", "adoEOF")
#import ".\msado60_Backcompat_x64.tlb" no_namespace rename("EOF", "adoEOF")

CADODatabase::CADODatabase()
{
	m_pConnection						= nullptr;
	m_pRecordSet						= nullptr;
	m_nRecordCount						= 0;
	m_bConnection						= FALSE;
	m_strLastError[0]					= '\0';
	m_hrLastError						= 0;

	m_bCursorLocation_adUserClientFlag = FALSE;
}

CADODatabase::~CADODatabase()
{
	Close();
}

void CADODatabase::Create(BOOL bCurserTypeFlag)
{
	m_bCursorLocation_adUserClientFlag = bCurserTypeFlag;
}

//LPCSTR CADODatabase::GetConnenctionVersion()
//{
//	char	strTemp[SQL_STRING_COUNT];
//	memset(strTemp, 0, SQL_STRING_COUNT);
//	_bstr_t strConnenetionVersion = m_pConnection->GetVersion();		
//	
//	INT32 Len = (INT32)strlen(_bstr_t(strConnenetionVersion)); 
//	if(Len < 0)
//	{
//		I3TRACE("[CADODatabase::GetConnenctionVersion] negative Value\n"); 
//		strTemp[SQL_STRING_COUNT] = 0; 
//	}
//	else
//	{
//		if(Len < SQL_STRING_COUNT)
//		{
//			strcpy(strTemp, (LPCSTR) _bstr_t(strConnenetionVersion));
//		}
//		else
//		{
//			i3mem::Copy((void *)strTemp, (void*)(LPCSTR)_bstr_t(strConnenetionVersion), SQL_STRING_COUNT); 
//			strTemp[SQL_STRING_COUNT] = 0; 
//		}
//	}
//
//	return (LPCSTR)strTemp;
//}

BOOL CADODatabase::Connect(char * strConnectFile)
{
	//Initialize the COM Library
	::CoInitializeEx(NULL, COINIT_MULTITHREADED );

	try
	{
		m_pConnection = _ConnectionPtr(__uuidof(Connection));
		//m_pCommand = _CommandPtr(__uuidof(Command));		
		m_pConnection->Open(_bstr_t(strConnectFile), _bstr_t(""), _bstr_t(""), adCmdUnspecified);
		m_pRecordSet = _RecordsetPtr(__uuidof(Recordset));
		m_pRecordSet->putref_ActiveConnection(m_pConnection);	

		m_bConnection	= TRUE;
	}
	catch (_com_error & err)
	{
		GetComError(err, m_pConnection);
		return FALSE;
	}

	return TRUE;
}

BOOL CADODatabase::Connect( char* strDataSource, char* strCatalog, char* strID, char* strPW )
{
	char strConnectInfo[ MAX_PATH ];

	sprintf_s( strConnectInfo, MAX_PATH, "Provider='SQLOLEDB.1';Data Source='%s';Initial Catalog='%s';Persist Security Info='True'", strDataSource, strCatalog );		

	//Initialize the COM Library
	::CoInitializeEx(NULL, COINIT_MULTITHREADED );

	try
	{
		m_pConnection = _ConnectionPtr(__uuidof(Connection));
		//m_pCommand = _CommandPtr(__uuidof(Command));		
		m_pConnection->Open(_bstr_t(strConnectInfo), _bstr_t(strID), _bstr_t(strPW), adCmdUnspecified);
		m_pRecordSet = _RecordsetPtr(__uuidof(Recordset));
		m_pRecordSet->putref_ActiveConnection(m_pConnection);	

		m_bConnection	= TRUE;
	}
	catch (_com_error & err)
	{
		GetComError(err, m_pConnection);
		return FALSE;
	}

	return TRUE;
}

BOOL CADODatabase::ReConnect(char * strConnectFile)
{
	if( this->IsNeedConnect() )
	{
		this->Close();

		if( this->Connect(strConnectFile) )
		{
			return TRUE;
		}

		return FALSE;
	}

	return FALSE;
}

BOOL CADODatabase::ReConnect( char* strDataSource, char* strCatalog, char* strID, char* strPW )
{
	if( this->IsNeedConnect() )
	{
		this->Close();

		if( this->Connect(strDataSource, strCatalog, strID, strPW ) )
		{
			return TRUE;
		}

		return FALSE;
	}

	return FALSE;
}

BOOL CADODatabase::BeginTrans()
{
	long lRv = m_pConnection->BeginTrans();
	if( SUCCEEDED( lRv ) )		return TRUE;

	return FALSE;
}

BOOL CADODatabase::CommitTrans()
{
	HRESULT hResult = m_pConnection->CommitTrans();

	if( SUCCEEDED( hResult ) )	return TRUE;

	return FALSE;
}

BOOL CADODatabase::RollbackTrans()
{
	HRESULT hResult = m_pConnection->RollbackTrans();

	if( SUCCEEDED( hResult ) )	return TRUE;

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// SQL 수행
BOOL CADODatabase::Execute(char * strSql)
{
	if( FALSE == m_bConnection )	return FALSE;

	_variant_t vtConnection((IDispatch*)m_pConnection);
	_variant_t vtQry(strSql);

	try
	{
		if (TRUE == m_bCursorLocation_adUserClientFlag)
		{
			if (m_pRecordSet->GetState() == adStateOpen)
			{
				m_pRecordSet->Close();
				m_pRecordSet->Release();
			}

			m_pRecordSet->CursorLocation = adUseClient; // Don't use 'adUseServer' option.
			m_pRecordSet->Open(vtQry, vtConnection, adOpenStatic, adLockOptimistic, adCmdText);
			m_pRecordSet->AddRef(); // release it externally.
		}
		else
		{
			if (m_pRecordSet->GetState() == adStateOpen) { m_pRecordSet->Close(); }
			m_pRecordSet->Open(vtQry, vtConnection, adOpenStatic, adLockOptimistic, adCmdText);
		}
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return FALSE;
	}

	return true;
}

BOOL CADODatabase::ExecuteQuery(char * strSql, BOOL IsLock)
{
	if( FALSE == m_bConnection )	return FALSE;

	_variant_t vtConnection((IDispatch*)m_pConnection);
	_variant_t vtQry(strSql);

	try
	{
		if (TRUE == m_bCursorLocation_adUserClientFlag)
		{
			if (m_pRecordSet->GetState() == adStateOpen)
			{
				m_pRecordSet->Close();
				m_pRecordSet->Release();
			}
			m_pRecordSet->CursorLocation = adUseClient; // Don't use 'adUseServer' option.
			if (TRUE == IsLock)	
			{
				m_pRecordSet->Open(vtQry, vtConnection, adOpenStatic, adLockOptimistic, adCmdText);			
			}
			else
			{
				m_pRecordSet->Open(vtQry, vtConnection, adOpenStatic, adLockUnspecified, adCmdText);			
			}
			m_pRecordSet->AddRef(); // release it externally.

		}
		else
		{
			if (m_pRecordSet->GetState() == adStateOpen)
			{
				m_pRecordSet->Close();
			}
			if (TRUE == IsLock)	
			{
				m_pRecordSet->Open(vtQry, vtConnection, adOpenStatic, adLockOptimistic, adCmdText);			
			}
			else
			{
				m_pRecordSet->Open(vtQry, vtConnection, adOpenStatic, adLockUnspecified, adCmdText);			
			}
		}
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return FALSE;
	}

	return true;
} // ExecuteQuery()

BOOL CADODatabase::ExecuteSP(char * strSql)
{
	if( FALSE == m_bConnection )	return FALSE;

	_variant_t vtConnection((IDispatch*)m_pConnection);
	_variant_t vtQry(strSql);

	try
	{		
		if (m_pRecordSet->GetState() == adStateOpen)
		{
			m_pRecordSet->Close();			
			m_pRecordSet->Release();
			//m_pRecordSet = _RecordsetPtr(__uuidof(Recordset));
			//m_pRecordSet->putref_ActiveConnection(m_pConnection);	
		}		

		m_pRecordSet->CursorLocation = adUseClient; // Don't use 'adUseServer' option.
		m_pRecordSet->Open(vtQry, vtConnection, adOpenStatic, adLockUnspecified, adCmdUnspecified );	
		m_pRecordSet->AddRef(); // release it externally.		
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return FALSE;
	}

	return true;
} // ExecuteSP()

BOOL CADODatabase::ExecuteSP(wchar_t * strSql)
{
	if( FALSE == m_bConnection )	return FALSE;

	_variant_t vtConnection((IDispatch*)m_pConnection);
	_variant_t vtQry(strSql);

	try
	{		
		if (m_pRecordSet->GetState() == adStateOpen)
		{
			m_pRecordSet->Close();			
			m_pRecordSet->Release();
			//m_pRecordSet = _RecordsetPtr(__uuidof(Recordset));
			//m_pRecordSet->putref_ActiveConnection(m_pConnection);	
		}		

		m_pRecordSet->CursorLocation = adUseClient; // Don't use 'adUseServer' option.
		m_pRecordSet->Open(vtQry, vtConnection, adOpenStatic, adLockUnspecified, adCmdUnspecified );	
		m_pRecordSet->AddRef(); // release it externally.		
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return FALSE;
	}

	return true;
} // ExecuteSP()

//////////////////////////////////////////////////////////////////////
// 날짜 타입 형식으로 요청

DATE CADODatabase::GetDateData(long nFieldIndex)
{
	DATE sData = 0;
	_variant_t vtFieldName(nFieldIndex);

	try
	{
		sData = m_pRecordSet->Fields->GetItem(vtFieldName)->Value.date;
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return 0;
	}

	return sData;
}

//////////////////////////////////////////////////////////////////////
// Int 타입 형식으로 요청
INT8 CADODatabase::GetINT8Data(long nFieldIndex)
{
	INT8 nData = 0; 
	_variant_t vtFieldName(nFieldIndex);

	try
	{
		nData = m_pRecordSet->Fields->GetItem(vtFieldName)->Value.bVal;
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return 0;
	}

	return nData; 
}

UINT8 CADODatabase::GetUINT8Data(long nFieldIndex)
{
	UINT8 nData = 0; 
	_variant_t vtFieldName(nFieldIndex);

	try
	{
		nData = m_pRecordSet->Fields->GetItem(vtFieldName)->Value.bVal;
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return 0;
	}

	return nData; 
}

INT16	CADODatabase::GetINT16Data(long nFieldIndex)
{
	INT16 nData = 0; 
	_variant_t vtFieldName(nFieldIndex);

	try
	{
		nData = m_pRecordSet->Fields->GetItem(vtFieldName)->Value.iVal;
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return 0;
	}

	return nData; 
}

UINT16	CADODatabase::GetUINT16Data(long nFieldIndex)
{
	UINT16 nData = 0; 
	_variant_t vtFieldName(nFieldIndex);

	try
	{
		nData = m_pRecordSet->Fields->GetItem(vtFieldName)->Value.uiVal;
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return 0;
	}

	return nData; 
}

INT32	CADODatabase::GetINT32Data(long nFieldIndex)
{
	INT32 nData = 0; 
	_variant_t vtFieldName(nFieldIndex);

	try
	{
		nData = m_pRecordSet->Fields->GetItem(vtFieldName)->Value.intVal;
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return 0;
	}

	return nData; 
}

UINT32	CADODatabase::GetUINT32Data(long nFieldIndex)
{
	UINT32 nData = 0; 
	_variant_t vtFieldName(nFieldIndex);

	try
	{
		nData = m_pRecordSet->Fields->GetItem(vtFieldName)->Value.uintVal;
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return 0;
	}

	return nData; 
}

INT64	CADODatabase::GetINT64Data(long nFieldIndex)
{
	INT64 nData = 0; 
	_variant_t vtFieldName(nFieldIndex);

	try
	{
		nData = m_pRecordSet->Fields->GetItem(vtFieldName)->Value.llVal;
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return 0;
	}

	return nData; 
}

UINT64	CADODatabase::GetUINT64Data(long nFieldIndex)
{
	UINT64 nData = 0; 
	_variant_t vtFieldName(nFieldIndex);

	try
	{
		nData = m_pRecordSet->Fields->GetItem(vtFieldName)->Value.ullVal;
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return 0;
	}

	return nData; 
}

//////////////////////////////////////////////////////////////////////
// 문자열 형식으로 요청
void CADODatabase::GetTextData(long nFieldIndex, char * pString, INT32 Maxlen)
{	
	_variant_t vtFieldName(nFieldIndex);

	try
	{
		_variant_t   variant(DISP_E_PARAMNOTFOUND, VT_ERROR);
		variant = m_pRecordSet->Fields->GetItem(vtFieldName)->Value;

		if (variant.vt != VT_NULL) 
		{
			INT32 Len = (INT32)strlen(_bstr_t(variant.bstrVal)); 
			if(Len < 0)
			{
				pString[Maxlen-1] = 0; 
			}
			else
			{
				if(Len < Maxlen)
				{
					strcpy_s(pString, Maxlen, (LPCSTR) _bstr_t(variant.bstrVal));
				}
				else
				{
					memcpy((void *)pString, (void*)(LPCSTR)_bstr_t(variant.bstrVal), Maxlen); 
					pString[Maxlen-1] = 0; 
				}
			}
		}
		else  
		{
			*pString = '\0';
			return;
		}
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return;
	}	

	return;
}

//////////////////////////////////////////////////////////////////////
// 문자열 형식으로 요청
void CADODatabase::GetTextData(long nFieldIndex, WCHAR * pString, INT32 Maxlen)
{	
	_variant_t vtFieldName(nFieldIndex);

	try
	{
		_variant_t   variant(DISP_E_PARAMNOTFOUND, VT_ERROR);
		variant = m_pRecordSet->Fields->GetItem(vtFieldName)->Value;

		if (variant.vt != VT_NULL) 
		{
			INT32 Len = (INT32)wcslen((LPWSTR)_bstr_t(variant.bstrVal)); 
			if(Len < 0)
			{
				pString[Maxlen-1] = 0; 
			}
			else
			{
				if(Len < Maxlen)
				{
					wcscpy_s(pString, Maxlen, (LPCWSTR) _bstr_t(variant.bstrVal));
				}
				else
				{
					memcpy((void *)pString, (void*)(LPCWSTR)_bstr_t(variant.bstrVal), sizeof(WCHAR) * Maxlen); 
					pString[Maxlen-1] = 0; 
				}
			}
		}
		else  
		{
			*pString = '\0';
			return;
		}
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return;
	}	

	return;
}

//////////////////////////////////////////////////////////////////////
// Binary 형식으로 요청
void CADODatabase::GetBinaryData(long nFieldIndex, void * pData, INT32 Maxlen)
{
	_variant_t vtFieldName(nFieldIndex);

	try
	{
		_variant_t   variant(DISP_E_PARAMNOTFOUND, VT_ERROR);
		variant = m_pRecordSet->Fields->GetItem(vtFieldName)->GetValue();

		if(variant.vt == (VT_ARRAY | VT_UI1)) //only process binary data
		{			
			//variant.parray->rgsabound[0].cElements;

			void HUGEP *pvData;
			SafeArrayAccessData(variant.parray, &pvData);
			memcpy(pData, (void*)pvData, Maxlen); 
			SafeArrayUnaccessData(variant.parray);
		}
		else  
		{
			return;
		}
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return;
	}

	return;
}

SAFEARRAY* CreateSafeArrayU1(int size)
{
	//Helper function for creating a one dimensional byte array
	SAFEARRAYBOUND rgb[1];		// one-dimensional array
	rgb[0].cElements=size;		// eight elements
	rgb[0].lLbound=0;			// lower bound of array
	return SafeArrayCreate(VT_UI1,1, rgb);
}

SAFEARRAY* ByteArrayToVar(BYTE* pBytes, int size, VARIANT& v)
{
	SAFEARRAY* psa = CreateSafeArrayU1(size);

	void HUGEP *pvData;
	HRESULT hr = SafeArrayAccessData(psa, &pvData);

	for (int i=0; i < size; i++)
	{
		((BYTE*) (pvData)) [i] = pBytes[i];
	}

	hr = SafeArrayUnaccessData(psa);

	v.parray = psa;
	v.vt = VT_ARRAY | VT_UI1;

	return psa; 
}

//////////////////////////////////////////////////////////////////////
// Binary 형식으로 저장
BOOL CADODatabase::SetBinaryData( long nFieldIndex, void * pData, INT32 Length )
{
	_variant_t vtNewName;
	_variant_t vtFieldName(nFieldIndex);
	try
	{
		SAFEARRAY * psa = ByteArrayToVar((BYTE*)pData, Length, vtNewName);
		m_pRecordSet->Fields->GetItem(vtFieldName)->Value = vtNewName; 
		m_pRecordSet->Update(); 
		SafeArrayDestroy(psa); 
		vtNewName.parray = nullptr;
		vtNewName.vt = 0;
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return FALSE;
	}

	return TRUE; 
}

// INT32 저장
BOOL CADODatabase::SetINT32Data(long nFieldIndex, INT32 data)
{
	_variant_t vtNewName;
	_variant_t vtFieldName(nFieldIndex);

	try
	{
		vtNewName.vt = VT_I4;
		vtNewName.intVal = data;

		m_pRecordSet->Fields->GetItem(vtFieldName)->Value = vtNewName;
		m_pRecordSet->Update();
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return FALSE;
	}

	return TRUE; 
}

// INT32 저장
BOOL CADODatabase::SetINT64Data(long nFieldIndex, INT64 data)
{
	_variant_t vtNewName;
	_variant_t vtFieldName(nFieldIndex);

	try
	{
		vtNewName.vt = VT_UI8;
		vtNewName.ullVal = data;

		m_pRecordSet->Fields->GetItem(vtFieldName)->Value = vtNewName;
		m_pRecordSet->Update();
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return FALSE;
	}

	return TRUE; 
}

//////////////////////////////////////////////////////////////////////
// long 형식으로 요청

long CADODatabase::GetLongData(long nFieldIndex)
{
	long nData = 0;
	_variant_t vtFieldName(nFieldIndex);

	try
	{
		nData = m_pRecordSet->Fields->GetItem(vtFieldName)->Value.lVal;
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return 0;
	}

	return nData;
}

//////////////////////////////////////////////////////////////////////
// float 형식으로 요청

float CADODatabase::GetFloatData(long nFieldIndex)
{
	float fData = 0;
	_variant_t vtFieldName(nFieldIndex);

	try
	{
		fData = m_pRecordSet->Fields->GetItem(vtFieldName)->Value.fltVal;
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return 0.0f;
	}

	return fData;
}

//////////////////////////////////////////////////////////////////////
// double 형식으로 요청

double CADODatabase::GetDoubleData(long nFieldIndex)
{
	double dData = 0;
	_variant_t vtFieldName(nFieldIndex);

	try
	{
		dData = m_pRecordSet->Fields->GetItem(vtFieldName)->Value.dblVal;
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return 0.0;
	}	  

	return dData;
}

BOOL CADODatabase::GetboolData(long nFieldIndex)
{
	char cData = 0;
	_variant_t vtFieldName(nFieldIndex);

	try
	{
		cData = m_pRecordSet->Fields->GetItem(vtFieldName)->Value.bVal;
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return false;
	}	  

	if (cData) return TRUE;

	return FALSE;
}

BOOL CADODatabase::Move(long nMove)
{
	try
	{
		m_pRecordSet->Move(nMove);
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// RecordSet 다음 record으로 이동
BOOL CADODatabase::MoveNext(void)
{
	try
	{
		m_pRecordSet->MoveNext();
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// RecordSet 처음 위치로 이동
BOOL CADODatabase::MoveFirst(void)
{
	try
	{
		m_pRecordSet->MoveFirst();
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// 마지막 record 위치인지

BOOL CADODatabase::IsEOF(void)
{
	try
	{
		if (m_pRecordSet == nullptr)			return TRUE;
		if (m_pRecordSet->adoEOF == FALSE)	return FALSE;
		else	   	   	   	   	   	   		return TRUE;
	}
	catch (_com_error & err)
	{
		GetComError(err, m_pConnection);	   	   
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////
// 연결 닫기

BOOL CADODatabase::Close(void)
{
	try
	{
		m_bConnection = FALSE;
		if (m_pRecordSet == nullptr) return TRUE;

		if (m_pRecordSet->GetState() == adStateOpen)
		{
			m_pRecordSet->Close();	
			if (TRUE == m_bCursorLocation_adUserClientFlag)
			{
				m_pRecordSet->Release();
			}
		}

		if (m_pConnection == nullptr) return TRUE;

		if (m_pConnection->State == adStateOpen)
		{
			m_pConnection->Close();
		}

		m_pConnection = nullptr;
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return FALSE;
	}

	CoUninitialize();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Record Count 요청

long CADODatabase::GetRecordCount()
{
	try
	{
		if (m_pRecordSet == nullptr) return 0;
		return( m_pRecordSet->GetRecordCount() & 0xFF);
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);
		return 0;
	}
}


void CADODatabase::GetComError(_com_error &err, _ConnectionPtr pConn)
{

	char szFormat[ 1024 ];
	_bstr_t bstrDescription;

	m_hrLastError = err.Error();

	if (pConn)
	{
		ErrorPtr pError;
		ErrorsPtr pErrors = pConn->GetErrors();

		pErrors->get_Item((_variant_t)((long) 0), &pError);

		if (pError)
		{
			_bstr_t bstrState(pError->GetSQLState());

			if (bstrState.length())
			{
				bstrDescription = pError->GetDescription();

				sprintf_s( szFormat, 1024, "%s,%s", (LPCTSTR) bstrState, (LPCTSTR) bstrDescription);
				strcpy_s(m_strLastError, 1024, szFormat); 	   	   	   	   
			}

			pError->Release();
		}

		pErrors->Release();

		if (strlen(m_strLastError))
		{
			return; 
		}
	}

	bstrDescription = err.Description();

	_bstr_t bstrSource(err.Source());

	sprintf_s( szFormat, 1024, "Error Code : %08x\n", err.Error());
	strcat_s( m_strLastError, 1024, szFormat );

	sprintf_s( szFormat, 1024, "Error Content : %s\n", err.ErrorMessage());
	strcat_s( m_strLastError, 1024, szFormat );

#ifndef UNICODE
	sprintf_s( szFormat, 1024, "Error Source  : %s\n", bstrSource.length() ? (LPCTSTR) bstrSource : "None");
	strcat_s( m_strLastError, 1024, szFormat );

	sprintf_s( szFormat, 1024, "Error Desc  : %s\n", bstrDescription.length() ? (LPCTSTR) bstrDescription : "None");
	strcat_s( m_strLastError, 1024, szFormat );
#endif

	return;
}

bool			CADODatabase::IsNeedConnect()
{
	switch( m_hrLastError )
	{
	case 0x00000000L	:		//	DB_E_SUCCED							쿼리 실행 문제가 없었지만 리커넥을 콜하는 경우가있습니다.
	case 0x80040E07L	:		//	DB_E_CANTCONVERTVALUE				데이터 오버플로 이외의 이유로 인해 명령의 리터럴 값을 올바른 형식으로 변환할 수 없습니다.
	case 0x80040E14L	:		//	DB_E_ERRORSINCOMMAND				명령에 하나 이상의 오류가 있습니다.

		return FALSE;
	}

	return TRUE;
}
