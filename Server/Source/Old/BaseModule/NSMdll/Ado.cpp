#include "stdafx.h"
#include "Ado.h"
#include "Memory.h"
#include "String.h"
#include <stdio.h>
#include <ATLComTime.h>

//#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF", "adoEOF")
#import ".\msado60_Backcompat_x64.tlb" no_namespace rename("EOF", "adoEOF")

NSM_cADODatabase::NSM_cADODatabase()
{
	m_pConnection	= NULL;
	m_pRecordSet	= NULL;
	m_nRecordCount	= 0;
	m_strError[0]	= '\0';

	m_bCursorLocation_adUserClientFlag = FALSE;
}

NSM_cADODatabase::~NSM_cADODatabase()
{
	Close();
}

void NSM_cADODatabase::Create(BOOL bCurserTypeFlag)
{
	m_bCursorLocation_adUserClientFlag = bCurserTypeFlag;
}

//////////////////////////////////////////////////////////////////////
// 연결 닫기

BOOL NSM_cADODatabase::Close(void)
{
	try
	{
		if (m_pRecordSet == NULL) return TRUE;

		if (m_pRecordSet->GetState() == adStateOpen)
			m_pRecordSet->Close();

		m_pRecordSet = NULL;

		if (m_pConnection == NULL) return TRUE;

		if (m_pConnection->State == adStateOpen)
			m_pConnection->Close();

		m_pConnection = NULL;
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 연결
BOOL NSM_cADODatabase::Connect(char * strConnect)
{	
	_bstr_t bstrConnectString( strConnect );

	try
	{
		m_pConnection = _ConnectionPtr(__uuidof(Connection));
		m_pConnection->Open(bstrConnectString, _bstr_t(""), _bstr_t(""), adCmdUnspecified);
		m_pConnection->CommandTimeout = DEF_COMMAND_TIME_OUT;	// 쿼리실행 제한시간(Timeout)

		m_pRecordSet = _RecordsetPtr(__uuidof(Recordset));
		m_pRecordSet->putref_ActiveConnection(m_pConnection);
	}
	catch (_com_error & err)
	{
		GetComError(err, m_pConnection);
		return FALSE;
	}

	return TRUE;
}

BOOL NSM_cADODatabase::Connect( char* strDataSource, char* strCatalog, char* strID, char* strPW )
{
	char strConnectInfo[ MAX_PATH ];

	sprintf( strConnectInfo, "Provider='SQLOLEDB.1';Data Source='%s';Initial Catalog='%s';Persist Security Info='True'", strDataSource, strCatalog );		

	//Initialize the COM Library
	CoInitializeEx(NULL, COINIT_MULTITHREADED );

	try
	{
		m_pConnection = _ConnectionPtr(__uuidof(Connection));
		m_pConnection->Open(_bstr_t(strConnectInfo), _bstr_t(strID), _bstr_t(strPW), adCmdUnspecified);

		m_pRecordSet = _RecordsetPtr(__uuidof(Recordset));
		m_pRecordSet->putref_ActiveConnection(m_pConnection);
	}
	catch (_com_error & err)
	{
		GetComError(err, m_pConnection);
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
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

//////////////////////////////////////////////////////////////////////////
// SQL 수행
BOOL NSM_cADODatabase::Execute(char * strSql)
{
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

	return TRUE;
} // Execute()

BOOL NSM_cADODatabase::ExecuteQuery(char * strSql, BOOL IsLock)
{
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

	return TRUE;
} // ExecuteQuery()

BOOL NSM_cADODatabase::ExecuteSP(char * strSql)
{
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

	return TRUE;
} // ExecuteSP()

//////////////////////////////////////////////////////////////////////////
BOOL NSM_cADODatabase::BeginTrans()
{
	long lRv ;

	try
	{
		lRv = m_pConnection->BeginTrans();
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);   	   
		return FALSE;
	}

	if( SUCCEEDED( lRv ) )	return TRUE;

	return FALSE ;
}

BOOL NSM_cADODatabase::CommitTrans()
{
	HRESULT hResult ;

	try
	{
		hResult = m_pConnection->CommitTrans();
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);   	   
		return FALSE;
	}

	if( SUCCEEDED( hResult ) )	return TRUE;

	return FALSE ;
}

BOOL NSM_cADODatabase::RollbackTrans()
{
	HRESULT hResult ;

	try
	{
		hResult = m_pConnection->RollbackTrans();
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);   	   
		return FALSE;
	}

	if( SUCCEEDED( hResult ) )	return TRUE;

	return FALSE ;
}


//////////////////////////////////////////////////////////////////////
// RecordSet 처음 위치로 이동
BOOL NSM_cADODatabase::MoveFirst(void)
{
	try
	{
		m_pRecordSet->MoveFirst();
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 레코드 이동
BOOL NSM_cADODatabase::Move(long nMove)
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
BOOL NSM_cADODatabase::MoveNext(void)
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
// 마지막 record 위치인지

BOOL NSM_cADODatabase::IsEOF(void)
{
	try
	{
		if (m_pRecordSet == NULL)			return TRUE;
		if (m_pRecordSet->adoEOF == FALSE)	return FALSE;
		//else	   	   	   	   	   	   		return TRUE;
	}
	catch (_com_error & err)
	{
		GetComError(err, m_pConnection);	   	   
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Record Count 요청

long NSM_cADODatabase::GetRecordCount()
{
	if (m_pRecordSet == NULL) return 0;
	return (long)m_pRecordSet->GetRecordCount();
}

BOOL NSM_cADODatabase::GetboolData(long nFieldIndex)
{
	BOOL cData = FALSE;
	_variant_t vtFieldName(nFieldIndex);

	try
	{
		cData = m_pRecordSet->Fields->GetItem(vtFieldName)->Value.boolVal;
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return FALSE;
	}	  

	if (cData) return TRUE;

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Int 타입 형식으로 요청
INT8 NSM_cADODatabase::GetINT8Data(long nFieldIndex)
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

UINT8 NSM_cADODatabase::GetUINT8Data(long nFieldIndex)
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

short int	NSM_cADODatabase::GetINT16Data(long nFieldIndex)
{
	short int nData = 0; 
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

UINT16	NSM_cADODatabase::GetUINT16Data(long nFieldIndex)
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

INT32	NSM_cADODatabase::GetINT32Data(long nFieldIndex)
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

UINT32	NSM_cADODatabase::GetUINT32Data(long nFieldIndex)
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

INT64	NSM_cADODatabase::GetINT64Data(long nFieldIndex)
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

UINT64	NSM_cADODatabase::GetUINT64Data(long nFieldIndex)
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
// long 형식으로 요청

long NSM_cADODatabase::GetLongData(long nFieldIndex)
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

float NSM_cADODatabase::GetFloatData(long nFieldIndex)
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
// 날짜 타입 형식으로 요청

DATE NSM_cADODatabase::GetDateData(long nFieldIndex)
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
// double 형식으로 요청

double NSM_cADODatabase::GetDoubleData(long nFieldIndex)
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

//////////////////////////////////////////////////////////////////////
// 문자열 형식으로 요청
void NSM_cADODatabase::GetTextData(long nFieldIndex, char * pString, int Maxlen)
{	
	_variant_t vtFieldName(nFieldIndex);

	try
	{
		_variant_t   variant(DISP_E_PARAMNOTFOUND, VT_ERROR);
		variant = m_pRecordSet->Fields->GetItem(vtFieldName)->Value;

		if (variant.vt != VT_NULL) 
		{
			NSMString::NCopy( pString, (LPCSTR) _bstr_t(variant.bstrVal), Maxlen );
			pString[Maxlen-1] = '\0';
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

//////////////////////////////////////////////////////////////////////
// Binary 형식으로 저장
BOOL NSM_cADODatabase::SetBinaryData( long nFieldIndex, void * pData, int Length )
{
	_variant_t vtNewName;
	_variant_t vtFieldName(nFieldIndex);
	try
	{
		SAFEARRAY * psa = ByteArrayToVar((BYTE*)pData, Length, vtNewName);
		m_pRecordSet->Fields->GetItem(vtFieldName)->Value = vtNewName; 
		m_pRecordSet->Update(); 
		SafeArrayDestroy(psa); 
		vtNewName.parray = NULL; 
		vtNewName.vt = 0;
	}
	catch (_com_error& err)
	{
		GetComError(err, m_pConnection);	   	   
		return FALSE;
	}

	return TRUE; 
}

//////////////////////////////////////////////////////////////////////
// Binary 형식으로 요청
void NSM_cADODatabase::GetBinaryData(long nFieldIndex, void * pData, int Maxlen)
{
	_variant_t vtFieldName(nFieldIndex);

	try
	{
		_variant_t   variant(DISP_E_PARAMNOTFOUND, VT_ERROR);
		variant = m_pRecordSet->Fields->GetItem(vtFieldName)->GetValue();

		if(variant.vt == (VT_ARRAY | VT_UI1)) //only process binary data
		{
			void HUGEP *pvData;
			SafeArrayAccessData(variant.parray, &pvData);
			NSM_nsMemory::Copy(pData, (void*)pvData, Maxlen); 
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

// int 저장
BOOL NSM_cADODatabase::SetintData(long nFieldIndex, int data)
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

//////////////////////////////////////////////////////////////////////////
// Error process
void NSM_cADODatabase::GetComError(_com_error &err, _ConnectionPtr pConn)
{
#ifdef _DEBUG
	char szFormat[1024] = {NULL,};
	char strMsg[5120] = {NULL,};
	_bstr_t bstrDescription;

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

				NSMString::Format( szFormat, 1024, "%s,%s", (LPCTSTR) bstrState, (LPCTSTR) bstrDescription);
				NSMString::NCopy( strMsg, szFormat, 1024 );
				NSMString::NCopy( m_strError, strMsg, 1024 );
			}

			pError->Release();
		}

		pErrors->Release();

		if (strlen(strMsg))
		{
			//TRACE(strMsg); 
			return; 
		}
	}

	bstrDescription = err.Description();

	char strTemp[256];
	_bstr_t bstrSource(err.Source());

	NSMString::Format(szFormat, 1024, "Error Code    : %08x\n", err.Error());
	NSMString::NConcat( strMsg, szFormat, 1024); 

	NSMString::Format(szFormat, 1024, "Error Content : %s\n", err.ErrorMessage());
	NSMString::NConcat( strMsg, szFormat, 1024); 

	NSMString::Format(szFormat, 1024, "Error Source  : %s\n", bstrSource.length() ? (LPCTSTR) bstrSource : "None");
	NSMString::NConcat( strMsg, szFormat, 1024); 

	NSMString::Format(szFormat, 1024, "Error Desc  : %s\n", bstrDescription.length() ? (LPCTSTR) bstrDescription : "None");
	NSMString::NConcat( strMsg, szFormat, 1024);

	NSMString::NCopy( m_strError, strMsg, 1024 );
#endif
	return;
}

void NSM_cADODatabase::ConvertString( char * pOutStr, INT32 i32Size, const char * pInStr, BOOL TYPE )
{	
	// DB 에서 ' 는 '' 로 입력해야 합니다.
	INT32 i32OutSize = 0;
	while( '\0' != *pInStr )
	{
		if( *pInStr == 39 )	// "'"
		{
			*pOutStr = *pInStr;
			pOutStr++;
			if( TRUE == TYPE)
			{
				*pOutStr = *pInStr;
				pOutStr++;
				*pOutStr = *pInStr;
				pOutStr++;
			}
		}
		*pOutStr = *pInStr;

		pOutStr++;
		pInStr++;
		i32OutSize++;

		if( i32Size <= i32OutSize )	break;
	}

	*pOutStr = '\0';
}

//////////////////////////////////////////////////////////////////////
// UINT32 형식을 날짜 형식의 문자열로 변경 (ex)[ 1110141212 -> 2011-10-14 12:12 ]
void NSM_cADODatabase::ConvertUINT32ToSmallDate( char* strSmallDateTime, INT32 i32Lenth, UINT32 ui32SmallDateTime)
{
	if( i32Lenth < STRING_SMALLDATE_SIZE )	// 시간을 날짜로 변환 했을때 16 + '\0' = 17 자리가 나옵니다. "2011-10-10 10:10"
	{
		strSmallDateTime[0] = '\0';
		return;
	}

	if(0 == ui32SmallDateTime)
	{
		// 아무값이 없으면 초기값을 2000-01-01 00:00 로 설정
		// 기본설정값을 가능하면 2000-01-01 00:00 로 설정하였으면 함
		ui32SmallDateTime = 1010000; 
	}
	sprintf_s( strSmallDateTime, i32Lenth, "%d-%d-%d %d:%d", 
		(2000+(ui32SmallDateTime / 100000000)), (ui32SmallDateTime % 100000000) / 1000000, (ui32SmallDateTime % 1000000) / 10000, (ui32SmallDateTime % 10000) / 100, (ui32SmallDateTime % 100) );
}
//////////////////////////////////////////////////////////////////////
// 날짜 타입을 UINT32 형식으로 변경 (((년 - 2000) * 100000000) + (월 * 1000000) + (일 * 10000) + (시* 100) + 분)
UINT32 NSM_cADODatabase::ConvertSmallDateToUINT32(DATE dateData)
{
	COleDateTime oleDate(dateData);
	int iYear	= oleDate.GetYear();
	int iMonth	= oleDate.GetMonth();
	int iDay	= oleDate.GetDay();
	int iHour	= oleDate.GetHour();
	int iMinute	= oleDate.GetMinute();

	if( iYear < 2000 )
	{
		return 0;
	}
	return ((iYear - 2000) * 100000000) + (iMonth * 1000000) + (iDay * 10000) + (iHour* 100) + iMinute;
}

//////////////////////////////////////////////////////////////////////
/*
TEST
*/
/*
void TestStoredProc() 
{ 
        try{ 
                // 객체 생성 
                _RecordsetPtr recordset; 
                _CommandPtr cmd; 
                _ConnectionPtr connection; 
                connection.CreateInstance(__uuidof(Connection)); 
                recordset.CreateInstance(__uuidof(Recordset)); 
                cmd.CreateInstance(__uuidof(Command)); 

                // 커넥션 객체 오픈 
                connection->ConnectionTimeOut=10; 
                connection->open("provider=...",NULL,NULL,adOptionUnspecified); 

                // 커맨트 객체 초기화 
                cmd->CommandType=adCmdStoredProc;        

                cmd->CommandText=_bstr_t("spBeginJob"); 

                cmd->ActiveConnection=connection; 

                // 저장프로시져의 리턴 값 및 파라메터값을 지정함. 
                _ParameterPtr  paramRet=cmd->CreateParameter("return",adInteger,adParamReturnValue,sizeof(int)) ; 
                _ParameterPtr  paramInput=cmd->CreateParameter("@inputData",adChar,adParamInput,4) ; 
                 
                // adParamInput에 해당하는 파라메터에 입력 값 대입 
                ptr1->Value=_variant_t("Test");         
                 
                // 파라미터값을 command객체에 추가( returnValue를 처음에 append해야 함.) 
                cmd->Parameters->Append(paramRet); 
                cmd->Parameters->Append(paramInput); 

                // 명령실행 
                recordset=cmd->Execute(NULL,NULL,adCmdStoredProc); 

                // Return값 읽어 옴. 방법#1 
                _variant_t retData(paramRet->Value); 
                // Return값 읽어 옴. 방법#2 
                cmd->Parameters->GetItem("ret")->Value; 
        }
		catch(_com_error &e)
		{ 
                //에러처리 
        } 
} 

*/




