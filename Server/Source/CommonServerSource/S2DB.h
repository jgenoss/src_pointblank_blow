#ifndef __S2DB_H__
#define __S2DB_H__

enum DB_DATA_TYPE
{
	DB_DATA_TYPE_BIT,
	DB_DATA_TYPE_TINYINT,
	DB_DATA_TYPE_SMALLINT,
	DB_DATA_TYPE_INT,
	DB_DATA_TYPE_BIGINT,
	DB_DATA_TYPE_CHAR,
	DB_DATA_TYPE_TTCHAR,
	DB_DATA_TYPE_BINARY,
	DB_DATA_TYPE_SMALLDATETIME,
	DB_DATA_TYPE_DATETIME,
	DB_DATA_TYPE_REAL,
	DB_DATA_TYPE_END,
};

#define S2_STRCPY( buffer, val ) \
{\
	*buffer = '\'';\
	buffer++;\
	while( *val )\
	{\
		if( '\'' == *val )\
		{\
			*buffer = '\'';\
			buffer++;\
		}\
		*buffer = *val;\
		buffer++;\
		val++;\
	}\
	*buffer = '\'';\
	buffer++;\
}

#define S2_ITOA( buffer, val ) \
{\
	pPtr = strNum;\
	INT32 i32Count = 0;\
	BOOL bSign = FALSE;\
	if( 0 > val )\
	{\
		val		= ~val + 1;\
		bSign	= TRUE;\
	}\
	do\
	{\
		*pPtr	= (val%10)+'0';\
		val		/= 10;\
		pPtr++;\
		i32Count++;\
	}\
	while( val );\
	if( bSign )\
	{\
		*pPtr	= '-';\
		pPtr++;\
		i32Count++;\
	}\
	for( INT32 i = 0 ; i < i32Count ; i++ )\
	{\
		pPtr--;\
		*buffer	= *pPtr;\
		buffer++;\
	}\
}

#define S2_FCVT( buffer, val ) \
{\
	pPtr = val;\
	while( *pPtr !='\0' )\
	{\
		*buffer = *pPtr;\
		buffer++;\
		pPtr++;\
	}\
}

namespace S2DB
{
	inline void MakeQueryA( char* strDest, char* strSPName, ... )
	{
		char strNum[ 32 ];
		char* pPtr;

		va_list marker;
		va_start( marker, strSPName );

		// EXEC 삽입
		strcpy( strDest, "EXEC " );
		strDest += 5;
		// SP 이름 삽입
		while( '\0' != *strSPName )
		{
			*strDest = *strSPName;
			strDest++;
			strSPName++;
		}
		// SP 이후 스페이스 삽입
		*strDest = ' ';	strDest++;

		BOOL bContinue	= TRUE;
		while( bContinue )
		{
			// DataType 을 얻어온다.
			DB_DATA_TYPE eDataType = va_arg( marker, DB_DATA_TYPE );
			switch( eDataType )
			{
			case DB_DATA_TYPE_BIT:
			case DB_DATA_TYPE_TINYINT:
			case DB_DATA_TYPE_SMALLINT:
			case DB_DATA_TYPE_INT:
				{
					INT32 i32Value = va_arg( marker, INT32 );
					S2_ITOA( strDest, i32Value );
					*strDest = ',';
					strDest++;
				}
				break;
			case DB_DATA_TYPE_BIGINT:
				{
					INT64 i64Value = va_arg( marker, INT64 );
					S2_ITOA( strDest, i64Value );
					*strDest = ',';
					strDest++;
				}
				break;
			case DB_DATA_TYPE_CHAR:
				{
					*strDest = 'N';
					strDest++;
				}
				//break; 주석이 맞습니다.
			case DB_DATA_TYPE_SMALLDATETIME:
			case DB_DATA_TYPE_DATETIME:
				{
					char* strValue = va_arg( marker, char* );
					S2_STRCPY( strDest, strValue );
					*strDest = ',';
					strDest++;
				}
				break;
			case DB_DATA_TYPE_TTCHAR:
				{				
					I3ASSERT(0);
				}
				break;
			case DB_DATA_TYPE_END:
				{
					strDest--;
					bContinue = FALSE;
				}
				break;
			case DB_DATA_TYPE_REAL:
				{
					double  dVal = va_arg(marker, double);
					char pDoubleBuf[32];
					i3String::MakePointFix(dVal, pDoubleBuf, 6, false);

					S2_FCVT( strDest, pDoubleBuf);
					*strDest = ',';
					strDest++;
				}
				break;
			case DB_DATA_TYPE_BINARY:
				{
					char * strValue = va_arg(marker, char*);
					S2_FCVT(strDest, strValue);
					*strDest = ',';
					strDest++;
				}
				break;
			default:
				{
					I3ASSERT(0);
				}
				break;
			}
		}
		
		*strDest = '\0';

		va_end( marker);
	}

	inline void MakeQueryW( wchar_t* wstrDest, wchar_t* wstrSPName, ... )
	{
		wchar_t  strNum[ 32 ];
		wchar_t* pPtr;

		va_list marker;
		va_start( marker, wstrSPName );

		// EXEC 삽입
		wcscpy( wstrDest, L"EXEC " );
		wstrDest += 5;
		// SP 이름 삽입
		while( '\0' != *wstrSPName )
		{
			*wstrDest = *wstrSPName;
			wstrDest++;
			wstrSPName++;
		}
		// SP 이후 스페이스 삽입
		*wstrDest = ' ';	wstrDest++;

		BOOL bContinue	= TRUE;
		while( bContinue )
		{
			// DataType 을 얻어온다.
			DB_DATA_TYPE eDataType = va_arg( marker, DB_DATA_TYPE );
			switch( eDataType )
			{
			case DB_DATA_TYPE_BIT:
			case DB_DATA_TYPE_TINYINT:
			case DB_DATA_TYPE_SMALLINT:
			case DB_DATA_TYPE_INT:
				{
					INT32 i32Value = va_arg( marker, INT32 );
					S2_ITOA( wstrDest, i32Value );
					*wstrDest = ',';
					wstrDest++;
				}
				break;
			case DB_DATA_TYPE_BIGINT:
				{
					INT64 i64Value = va_arg( marker, INT64 );
					S2_ITOA( wstrDest, i64Value );
					*wstrDest = ',';
					wstrDest++;
				}
				break;
			case DB_DATA_TYPE_CHAR:
				{
					I3ASSERT(0);
				}
				break;
			case DB_DATA_TYPE_SMALLDATETIME:
			case DB_DATA_TYPE_DATETIME:
				{
					*wstrDest = 'N';
					wstrDest++;

					wchar_t* wstrValue = va_arg( marker, wchar_t* );
					S2_STRCPY( wstrDest, wstrValue );

					*wstrDest = ',';
					wstrDest++;
				}
				break;
			case DB_DATA_TYPE_TTCHAR:
				{				
					*wstrDest = 'N';
					wstrDest++;

					wchar_t* wstrValue = va_arg( marker, wchar_t* );
					S2_STRCPY( wstrDest, wstrValue );

					*wstrDest = ',';
					wstrDest++;
				}
				break;
			case DB_DATA_TYPE_END:
				{
					wstrDest--;
					bContinue = FALSE;
				}
				break;
			case DB_DATA_TYPE_REAL:
				{
					double  dVal = va_arg(marker, double);
					wchar_t pDoubleBuf[32];
					i3String::MakePointFix(dVal, pDoubleBuf, 6, false);

					S2_FCVT( wstrDest, pDoubleBuf);
					*wstrDest = ',';
					wstrDest++;
				}
				break;
			case DB_DATA_TYPE_BINARY:
				{
					wchar_t * strValue = va_arg(marker, wchar_t*);
					S2_FCVT( wstrDest, strValue );
					*wstrDest = ',';
					wstrDest++;
				}
				break;
			default:
				{
					I3ASSERT(0);
				}
				break;
			}
		}
		
		*wstrDest = '\0';

		va_end( marker);
	}

	/////////////////////////////////////////////////////////////////////
	// UINT32 형식을 날짜 형식의 문자열로 변경 (ex)[ 20120224 -> 2012-02-24 00:00 ]
	inline void ConvertUINT32DateTypeToSmallDate( char* strSmallDateTime, INT32 i32Lenth, UINT32 ui32SmallDate)
	{
		if( i32Lenth < SMALLDATE_STRING_SIZE )	// 시간을 날짜로 변환 했을때 16 + '\0' = 17 자리가 나옵니다. "2011-10-10 10:10"
		{
			strSmallDateTime[0] = '\0';
			return;
		}

		if(0 == ui32SmallDate)
		{
			// 아무값이 없으면 초기값을 2000-01-01 00:00 로 설정
			// 기본설정값을 가능하면 2000-01-01 00:00 로 설정하였으면 함
			ui32SmallDate = 1010000;
		}
		sprintf_s( strSmallDateTime, i32Lenth, "%04d-%02d-%02d", 
			(ui32SmallDate / 10000), (ui32SmallDate % 10000) / 100, (ui32SmallDate % 100) );
	};
	//////////////////////////////////////////////////////////////////////
	// 날짜 타입을 UINT32 형식으로 변경 (((년 - 2000) * 100000000) + (월 * 1000000) + (일 * 10000) + (시* 100) + 분)
	inline UINT32 ConvertSmallDateToUINT32( DATE dateData )
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
		if( iYear > 2042 )
		{
			iYear = 2042;
		}

		return ((iYear - 2000) * 100000000) + (iMonth * 1000000) + (iDay * 10000) + (iHour* 100) + iMinute;
	};
	
	// UINT32 형식을 날짜 형식의 문자열로 변경 (ex)[ 1110141212 -> 2011-10-14 12:12 ]
	inline void ConvertUINT32ToSmallDate( char* strSmallDateTime, INT32 i32Lenth, UINT32 ui32SmallDateTime)
	{
		if( i32Lenth < SMALLDATE_STRING_SIZE )	// 시간을 날짜로 변환 했을때 16 + '\0' = 17 자리가 나옵니다. "2011-10-10 10:10"
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

		UINT32 ui32Year		= ui32SmallDateTime / 100000000;
		UINT32 ui32Month	= (ui32SmallDateTime % 100000000) / 1000000;
		UINT32 ui32Day		= (ui32SmallDateTime % 1000000) / 10000;
		UINT32 ui32Hour		= (ui32SmallDateTime % 10000) / 100;
		UINT32 ui32Minute	= (ui32SmallDateTime % 100);

		// 범위 초과 체크
		ui32Year = MIN(42, ui32Year);
		if (0 == ui32Month || ui32Month > 12 || 0 == ui32Day || ui32Day > 31 || ui32Hour > 23 || ui32Minute > 59)
		{// 기본설정값을 가능하면 2000-01-01 00:00 로 설정하였으면 함
			ui32Year	= 00;
			ui32Month	= 01;
			ui32Day		= 01;
			ui32Hour	= 00;
			ui32Minute	= 00;
		}

		sprintf_s( strSmallDateTime, i32Lenth, "%04d-%02d-%02d %02d:%02d", 
			(2000+ ui32Year), ui32Month, ui32Day, ui32Hour, ui32Minute);
	};

	inline void ConvertUINT32ToSmallDate( wchar_t* wstrSmallDateTime, INT32 i32Lenth, UINT32 ui32SmallDateTime)
	{
		if( i32Lenth < SMALLDATE_STRING_SIZE )	// 시간을 날짜로 변환 했을때 16 + '\0' = 17 자리가 나옵니다. "2011-10-10 10:10"
		{
			wstrSmallDateTime[0] = '\0';
			return;
		}

		if(0 == ui32SmallDateTime)
		{
			// 아무값이 없으면 초기값을 2000-01-01 00:00 로 설정
			// 기본설정값을 가능하면 2000-01-01 00:00 로 설정하였으면 함
			ui32SmallDateTime = 1010000;
		}

		UINT32 ui32Year = ui32SmallDateTime / 100000000;
		UINT32 ui32Month = (ui32SmallDateTime % 100000000) / 1000000;
		UINT32 ui32Day = (ui32SmallDateTime % 1000000) / 10000;
		UINT32 ui32Hour = (ui32SmallDateTime % 10000) / 100;
		UINT32 ui32Minute = (ui32SmallDateTime % 100);

		// 범위 초과 체크
		ui32Year = MIN(42, ui32Year);
		if (0 == ui32Month || ui32Month > 12 || 0 == ui32Day || ui32Day > 31 || ui32Hour > 23 || ui32Minute > 59)
		{// 기본설정값을 가능하면 2000-01-01 00:00 로 설정하였으면 함
			ui32Year = 00;
			ui32Month = 01;
			ui32Day = 01;
			ui32Hour = 00;
			ui32Minute = 00;
		}

		swprintf_s( wstrSmallDateTime, i32Lenth, L"%04d-%02d-%02d %02d:%02d", 
			(2000 + ui32Year), ui32Month, ui32Day, ui32Hour, ui32Minute);
	};

	//////////////////////////////////////////////////////////////////////////
	// 날짜 타입을 UINT64 형식으로 변경 (((년 - 2000) * 10000000000) + (월 * 100000000) + (일 * 1000000) + (시* 10000) + (분*100)+초)
	inline UINT64 ConvertSmallDateToUINT64(DATE dateData)
	{
		COleDateTime oleDate(dateData);
		INT64 iYear	= oleDate.GetYear();
		INT64 iMonth	= oleDate.GetMonth();
		INT64 iDay	= oleDate.GetDay();
		INT64 iHour	= oleDate.GetHour();
		INT64 iMinute	= oleDate.GetMinute();
		INT64 iSecond	= oleDate.GetSecond();	

		if( iYear < 2000 )
		{
			return 0;
		}
		return ((iYear - 2000) * 10000000000) + (iMonth * 100000000) + (iDay * 1000000) + (iHour* 10000) + (iMinute* 100) + iSecond;
	};
	//
	////////////////////////////////////////////////////////////////////////////
	//// 날짜 타입을 UINT64 형식으로 변경 
	//// ( ((년 - 2000) * 10000000000000) + (월 * 100000000000) + (일 * 1000000000) + (시 * 10000000) + (분 * 100000) + (초 * 1000) + 밀리초 )
	inline UINT64 ConvertDateToUINT64( DATE dateData )
	{
		COleDateTime oleDate(dateData);
		UINT64 iYear		= oleDate.GetYear();
		UINT64 iMonth		= oleDate.GetMonth();
		UINT64 iDay		= oleDate.GetDay();
		UINT64 iHour		= oleDate.GetHour();
		UINT64 iMinute		= oleDate.GetMinute();
		UINT64 iSecond		= oleDate.GetSecond();
		// COleDateTime 형식이 milliSecond를 지원하지 않음.
		int iMilliSec	= 0;

		if( iYear < 2000 )
		{
			return 0;
		}
		return (UINT64)(((iYear - 2000) * 10000000000000) + (iMonth * 100000000000) + (iDay * 1000000000) + (iHour * 10000000) + (iMinute * 100000) + (iSecond * 1000) + iMilliSec);
	}

	//
	//// UINT64 형식을 날짜 형식의 문자열로 변경 (ex)[ 1110141212 -> 2011-10-14 12:12:12:12 ]
	//void CADODatabase::ConvertUINT64ToDateTime( char* strDateTime, INT32 i32Lenth, UINT64 ui64DateTime)
	//{
	//	if( i32Lenth < STRING_DATETIME_SIZE64 )
	//	{
	//		strDateTime[0] = '\0';
	//		return;
	//	}
	//
	//	if(0 == ui64DateTime)
	//	{
	//		// 아무값이 없으면 초기값을 2000-01-01 00:00.000 로 설정
	//		// 기본설정값을 가능하면 2000-01-01 00:00.000 로 설정하였으면 함
	//		ui64DateTime = 101000000;
	//	}
	//	sprintf_s( strDateTime, i32Lenth, "%I64d-%I64d-%I64d %I64d:%I64d:%I64d.%64d", 
	//		(2000+(ui64DateTime / 10000000000000)), (ui64DateTime % 10000000000000) / 100000000000, (ui64DateTime % 100000000000) / 1000000000, (ui64DateTime % 1000000000) / 10000000, (ui64DateTime % 10000000) / 100000, (ui64DateTime % 100000) / 1000, (ui64DateTime % 100) );
	//}

	//////////////////////////////////////////////////////////////////////
	// 데이타를 바이너리 형식으로 변경
	inline bool ConvertStringToBinary( char* pOut, INT32 i32OutSize, char* pIn, INT32 i32InSize )
	{
		// 크기 비교 - 쓸수 없으면 flase 를 리턴합니다.
		if( ((i32InSize * 2) + 3) > i32OutSize ) return false;

		*pOut = '0';	pOut++;
		*pOut = 'x';	pOut++;

		for( INT32 i = 0 ; i < i32InSize ; ++i )
		{
			*pOut = (pIn[ i ] & 0xF0) >> 4 ;
			if( *pOut < 10 )	*pOut += '0';
			else				*pOut += 0x37;	// ('A'-10)
			++pOut;

			*pOut = (pIn[ i ] & 0x0F) ;
			if( *pOut < 10 )	*pOut += '0';
			else				*pOut += 0x37;	// ('A'-10)
			++pOut;
		}
		*pOut = '\0';

		return true;
	};

	inline bool ConvertStringToBinary( wchar_t* pOut, INT32 i32OutSize, char* pIn, INT32 i32InSize )
	{
		// 크기 비교 - 쓸수 없으면 flase 를 리턴합니다.
		if( ((i32InSize * 2) + 3) > i32OutSize ) return false;

		*pOut = '0';	pOut++;
		*pOut = 'x';	pOut++;

		for( INT32 i = 0 ; i < i32InSize ; ++i )
		{
			*pOut = (pIn[ i ] & 0xF0) >> 4 ;
			if( *pOut < 10 )	*pOut += '0';
			else				*pOut += 0x37;	// ('A'-10)
			++pOut;

			*pOut = (pIn[ i ] & 0x0F) ;
			if( *pOut < 10 )	*pOut += '0';
			else				*pOut += 0x37;	// ('A'-10)
			++pOut;
		}
		*pOut = '\0';

		return true;
	};

	//////////////////////////////////////////////////////////////////////
	// 스트링을 DB 에 쓸수 있는 상태로 변경
	inline void ConvertString( char * pOutStr, INT32 i32Size, const char * pInStr, BOOL bUseSP = FALSE )
	{	
		// DB 에서 ' 는 '' 로 입력해야 합니다.
		INT32 i32OutSize = 0;
		while( '\0' != *pInStr )
		{
			if( *pInStr == 39 )	// "'"
			{
				*pOutStr = *pInStr;
				pOutStr++;
				if(TRUE == bUseSP)
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
	};

	inline void ConvertString( wchar_t * pOutStr, INT32 i32Size, const wchar_t * pInStr, BOOL bUseSP = FALSE )
	{	
		// DB 에서 ' 는 '' 로 입력해야 합니다.
		INT32 i32OutSize = 0;
		while( '\0' != *pInStr )
		{
			if( *pInStr == 39 )	// "'"
			{
				*pOutStr = *pInStr;
				pOutStr++;
				if(TRUE == bUseSP)
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
	};

	//////////////////////////////////////////////////////////////////////
	// UINT8 을 bool 형태로 변환
	inline bool ConvertUINT8Tobool( UINT8 ui8In )
	{
		if( ui8In == 0 )	return false;
		return true;
	}
};

#endif