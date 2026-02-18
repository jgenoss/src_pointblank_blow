#include "i3CommonType.h"
#include "i3IniParser.h"
#include "i3ErrorHandler.h"
#include "i3StreamFile.h"
#include "i3Macro.h"
#include "i3String.h"
#include "i3Lexer.h"
#include "i3Performance.h"
#include "i3FixedPoint.h"
#include "i3Language.h"

#include "string/compare/generic_is_niequal.h"
#include "string/compare/generic_is_iequal.h"
#include "i3Base/string/ext/safe_string_copy.h"
#include "i3Base/string/ext/generic_string_size.h"
#include "i3Base/string/ext/atoi.h"

#if defined( I3_PSP) 
#include <stdarg.h>	
#endif

I3_CLASS_INSTANCE( i3IniParser);

i3IniParser::~i3IniParser()
{
	Close();
}

// Create to save
bool i3IniParser::Create( const char *szFileName )
{
	Close();

	m_bFromFile = true;

	i3FileStream* pFileStream = i3FileStream::new_object();

	m_pStream = pFileStream;

	if( !pFileStream->Create( szFileName, STREAM_WRITE ) )
	{
		I3PRINTLOG(I3LOG_WARN, "Could not create file.");
		//I3TRACE( "%s : Could not create file. (%s)\n", __FUNCTION__, szFileName );
		return false;
	}

	return true;
}

// 파일에서 직접 읽는다.
bool i3IniParser::OpenFromFile( const char *szFileName, bool bAllowDuplicatedSection )
{
	Close();

	m_bFromFile = true;

	i3FileStream* pFileStream = i3FileStream::new_object();

	m_pStream = pFileStream;

	if( !pFileStream->Open( szFileName, STREAM_READ | STREAM_SHAREREAD ) )
	{
		I3PRINTLOG(I3LOG_WARN, "Could not open file.");
		//I3TRACE( "%s : Could not open file. (%s)\n", __FUNCTION__, szFileName );
		return false;
	}

	Open( m_pStream, bAllowDuplicatedSection, true);

	return true;
}

void i3IniParser::Close()
{
	if( m_bFromFile )
	{
		m_bFromFile = false;
		I3_SAFE_RELEASE( m_pStream );
	}
	else
	{
		m_pStream = nullptr;
	}


	for(size_t i = 0; i < m_SectionList.size(); i++)
	{
		void *p = m_SectionList[i];				// ????
		i3MemFree( p);
	}

	m_SectionList.clear();
	I3MEM_SAFE_FREE_POINTER( m_pValueTable );

	m_nNumValues = 0;
}

#define READBUF_SIZE 512

UINT32 i3IniParser::_ReadLine( char * szLine)
{
	UINT32 readsize = 0;
	WCHAR16 szReadLine[READBUF_SIZE] = L"";

	i3mem::FillZero( szLine, READBUF_SIZE * 2);
	if( m_bUnicode)
	{
		readsize = m_pStream->ReadLineW( szReadLine, READBUF_SIZE);

		i3String::WideCharToMultiByte( i3Language::GetCurrentCodePage(), 0, szReadLine, i3::generic_string_size(szReadLine), szLine, READBUF_SIZE * 2, nullptr, nullptr);

		I3ASSERT( i3::generic_string_size( szLine) < READBUF_SIZE * 2);
	}
	else
	{
		readsize = m_pStream->ReadLine( szLine, READBUF_SIZE * 2);
	}

	return readsize;
}

// 스트림에서 읽는다.
bool i3IniParser::Open( i3Stream *pStream, bool bAllowDuplicatedSection, bool bFromFile )
{
	if( ! bFromFile )
	{
		Close();
	}

	if( pStream == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN, "Stream is nullptr.");

		//I3TRACE( "%s : Stream is nullptr.\n", __FUNCTION__ );
		return false;
	}

	m_bAllowDuplicatedSection = bAllowDuplicatedSection;
	m_pStream = pStream;

	UINT32 nPos = m_pStream->GetPosition();

	WCHAR16 code;
	UINT32 rc = m_pStream->Read( &code, sizeof( WCHAR16));
	if( rc == STREAM_ERR)
	{
		return false;
	}

	m_bUnicode = false;
	if( code == 0xfeff)
	{
		m_bUnicode = true;
	}
	else
	{
		m_pStream->SetPosition( nPos, STREAM_BEGIN);
	}

	char szReadBuf[READBUF_SIZE*2] = "";
	char szBuf[READBUF_SIZE] = "";
	char *szCur = nullptr;

	i3IniSectionInfo *pPrevInfo = nullptr;
	UINT32 nLineCount = 0;

	// 한줄씩 읽는다.
	while( _ReadLine( szReadBuf))
	{
		// 앞뒤로 공백 문자 제거
		RemoveWhiteSpace( szBuf, szReadBuf );

		// '['문자로 시작하고
		if( szBuf[0] =='[' )
		{
			INT32 nLastPos = i3::generic_string_size( szBuf ) - 1;
			// ']' 문자로 끝나면
			if( szBuf[ nLastPos ] == ']' )
			{
				// szCur 에 '['와']'사이의 문자열을 설정하고.
				szBuf[ nLastPos ] = '\0';
				szCur = &szBuf[1];

				// 문자열의 길이를 확인하고,
				if( i3::generic_string_size( szCur ) <= 0 )
				{
					I3PRINTLOG(I3LOG_WARN, "Section name is nullptr.");
					//I3TRACE( "%s : Section name is nullptr.\n", __FUNCTION__ );
					I3ASSERT_0;
				}
				else
				{
					if( ! m_bAllowDuplicatedSection )
					{
						// 똑같은 이름이 있는지 확인하고
						if( FindSection( szCur ) != nullptr)
						{
							I3PRINTLOG(I3LOG_WARN, "Section name is duplicated");
							//I3TRACE( "%s : Section name is duplicated(%s).\n", __FUNCTION__, szCur );
							I3ASSERT_0;
							return false;
						}
					}

					// 섹션 하나 추가
					//I3HMEM hMem = i3MemAlloc( sizeof(i3IniSectionInfo));
					i3IniSectionInfo *pInfo = (i3IniSectionInfo *)i3MemAlloc( sizeof(i3IniSectionInfo));

					i3::safe_string_copy( pInfo->Name, szCur, I3_INI_NAME_SIZE);		// 섹션 이름 저장
					pInfo->Position = m_pStream->GetPosition(); // Stream 위치 저장
					m_SectionList.push_back( pInfo );				// 리스트에 추가

					// 이전 섹션의 라인수 설정
					if( pPrevInfo != nullptr)
					{
						pPrevInfo->NumLines = nLineCount - 1;
					}

					// 라인 카운트 초기화
					nLineCount = 0;
					pPrevInfo = pInfo;
				}
			}
		}
		nLineCount++;
	}

	// 이전 섹션의 라인수 설정
	if( pPrevInfo != nullptr)
	{
		pPrevInfo->NumLines = nLineCount - 1;
	}

	return true;
}

// 공백 문자 제거 하면서 복사한다. 
// 단 [] 혹은 ""으로 묶여 있는 문자열 내부의 공백은 그대로 복사한다.
void i3IniParser::RemoveWhiteSpace( char *szOut, const char *szInput )
{
   bool bString  = false, bSection = false;

   INT32 nLen = i3::generic_string_size( (char*)szInput );
   INT32 nCur = 0;

	for (INT32 i = 0 ; i < nLen ; i++)
	{
		bool bCopy = true;

		switch (szInput[i])
		{
		// 섹션 구분
		case '[':
			{
				I3ASSERT( !bSection );
				bSection = true;
			}
			break;
		case ']':
			{
				I3ASSERT( bSection );
				bSection = false;
			}
			break;

		// 제어문자. 문자열 내부에서 "나 \를 입력하고 싶을 때
		case '\\':
			{
				if( bString )
				{
					// 추가로 제어하고 싶은 문자가 잇으면 여기서 제어하면 됨.
					i++; // 다음 문자를 그대로 복사
				}
			}
			break;

		// 문자열 구분
		case '\"':
			{
				if( !bSection )
				{
					bString = bString ? false : true;
					bCopy = false;
				}
			}
			break;

		// White Space
		case ' ':
		case '\t':
		case '\n':
		case '\r':
			{
				if( !bSection && !bString )
				{
					bCopy = false;
				}
			}
			break;

		// 주석 문자
		case ';':
			{
				if( !bSection && !bString )
				{
					goto Exit;
				}
			}
			break;
		}

		if ( bCopy )
		{
			szOut[nCur++] = szInput[i];
		}
	}

Exit:

	szOut[nCur] = 0;
}

bool i3IniParser::ProcessString( char *szOut, const char *szInput, INT32 nOutBufLength )
{
	INT32 nLen = i3::generic_string_size( (char*)szInput );
	INT32 nCur = 0;

	I3ASSERT( nOutBufLength != 0 );

	for (INT32 i = 0 ; i < nLen ; i++)
	{
		switch (szInput[i])
		{
		case '\\':
		case '"':
			{
				if( nOutBufLength > 0 && nCur >= nOutBufLength ) return false;
				szOut[nCur++] = '\\';

				if( nOutBufLength > 0 && nCur >= nOutBufLength ) return false;
				szOut[nCur++] = szInput[i];
			}
			break;

		case '\r': continue;

		case '\n':
			{
				if( nOutBufLength > 0 && nCur >= nOutBufLength ) return false;
				szOut[nCur++] = '\\';

				if( nOutBufLength > 0 && nCur >= nOutBufLength ) return false;
				szOut[nCur++] = 'n';
			}
			break;

		default:
			{
				if( nOutBufLength > 0 && nCur >= nOutBufLength ) return false;
				szOut[nCur++] = szInput[i];
			}
			break;
		}
	}

	if( nOutBufLength > 0 && nCur >= nOutBufLength ) return false;
	szOut[nCur++] = '\0';

	return true;
}

i3IniSectionInfo* i3IniParser::FindSection( const char *szName )
{
	const size_t n = m_SectionList.size();
	for(size_t i = 0; i < n; i++ )
	{
		i3IniSectionInfo *pInfo = m_SectionList[i];
		if( i3::generic_is_iequal( pInfo->Name, szName ) )
		{ 
			return pInfo;
		}
	}
	return false;
}

INT32 i3IniParser::GetSectionIndex( const char *szName )
{
	size_t n = m_SectionList.size();

	for(size_t i = 0; i < n; i++ )
	{
		i3IniSectionInfo *pInfo = m_SectionList[i];
		if( i3::generic_is_iequal( pInfo->Name, szName )  )
		{
			return i;
		}
	}
	return -1;
}

void i3IniParser::DeleteAllValues()
{
}

// 지정된 이름의 섹션의 모든 값을 읽어서 테이블로 만든다.
bool i3IniParser::ReadSection( const char *szName )
{
	// 섹션 찾기
	i3IniSectionInfo *pInfo = FindSection( szName );
	return ReadSectionByInfo( pInfo );
}

// 지정된 인덱스의 섹션의 모든 값을 읽어서 테이블로 만든다.
bool i3IniParser::ReadSectionByIndex( INT32 nIndex )
{
	i3IniSectionInfo *pInfo = nullptr;
	if( nIndex < INT32(m_SectionList.size()) )
	{
		pInfo =	m_SectionList[nIndex];
	}
	return ReadSectionByInfo( pInfo );
}

// 지정된 섹션의 모든 값을 읽어서 테이블로 만든다.
bool i3IniParser::ReadSectionByInfo( i3IniSectionInfo *pInfo )
{
	m_szSectionName[0] = 0;

	if( m_pStream == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN, "m_pStream is nullptr.");
		//I3TRACE( "ERROR : [%s] m_pStream is nullptr.\n", __FUNCTION__ );
		return false;
	}

	if( pInfo == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN, "pInfo is nullptr.");
		//I3TRACE( "ERROR : [%s] pInfo is nullptr.\n", __FUNCTION__ );
		return false;
	}

	strncpy( m_szSectionName, pInfo->Name, sizeof( m_szSectionName) -1 );

	// Value Table 준비
	I3MEM_SAFE_FREE_POINTER( m_pValueTable );
	if( pInfo->NumLines <= 0 )
	{
		return true;
	}

	{
//		I3HMEM hMem = i3MemAlloc( sizeof(i3IniValue) * pInfo->NumLines);
		m_pValueTable = (i3IniValue *) i3MemAlloc( sizeof(i3IniValue) * pInfo->NumLines);
	}
    
	char szReadBuf[1024];
	char szBuf[1024];
    UINT32 i;

	// Section의 처음 위치로 이동
	m_pStream->SetPosition( pInfo->Position, STREAM_BEGIN );

	m_nNumValues = 0;

	for( i = 0; i < pInfo->NumLines; i++ )
	{
		// 한줄 읽음
		_ReadLine( szReadBuf);
		
		RemoveWhiteSpace( szBuf, szReadBuf );

		// 아무것도 없으면 스킵...
        if( szBuf[0] == '\0' || szBuf[0] == '=' )
		{
			continue;
		}

		// 구분자(=)를 찾아서 0을 넣어서 두개의 문자열로 만듬.
		char *pSeperator = strchr( szBuf, '=' );
		if( pSeperator == nullptr)
		{
			continue;
		}
		*pSeperator = 0;
		char *pValue = pSeperator + 1;

		// 라벨 길이 검사
		if( strlen( szBuf ) >= I3_INI_NAME_SIZE )
		{
			// 너무 길면 스킵
			I3PRINTLOG(I3LOG_WARN, "szLabel name is too long");
			//I3TRACE( "%s : szLabel name is too long (%s)\n", __FUNCTION__, szBuf );
			I3ASSERT_0;
			continue;
		}
		else
		{
			INT32 nValueLength = strlen( pValue );

			// 값의 길이 검사
			if( nValueLength >= I3_INI_VALUE_SIZE )
			{
				// 너무 길면 스킵
				I3PRINTLOG(I3LOG_WARN, "szLabel value string is too long ");
				//I3TRACE( "%s : szLabel value string is too long (%s)\n", __FUNCTION__, pValue );
				I3ASSERT_0;
				continue;
			}
			else
			{
				// 라벨과 값 문자열 복사
				strncpy( m_pValueTable[m_nNumValues].szLabel, szBuf, sizeof(m_pValueTable[m_nNumValues].szLabel) -1 );
				strncpy( m_pValueTable[m_nNumValues].szValue, pValue, sizeof(m_pValueTable[m_nNumValues].szValue) -1 );
				m_pValueTable[m_nNumValues].nLength = nValueLength;
			}
		}
		m_nNumValues++;
	}

	return true;
}

const char *i3IniParser::GetStringValue( const char *szLabel ) // Use internal buffer and return its pointer.
{
	if( m_pValueTable == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN, "Value Table is not exist. ");
		//I3TRACE( "%s : Value Table is not exist.\n", __FUNCTION__ );
		return nullptr;
	}

	UINT32 i;
	for( i = 0; i < m_nNumValues; i++ )
	{
		if(i3::generic_is_iequal( szLabel, m_pValueTable[i].szLabel ) )
		{
			return m_pValueTable[i].szValue;
		}
	}

	//I3TRACE( "%s : Could not find the label named '%s'.\n", __FUNCTION__, szLabel );
	return nullptr;
}

bool i3IniParser::GetString( const char *szLabel,	char *Value, INT32 nBufLength )
{	
	if( m_pValueTable == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN, "Value Table is not exist.");
		//I3TRACE( "%s : Value Table is not exist.\n", __FUNCTION__ );
		return false;
	}

	if( nBufLength <= 0 )
	{
		I3PRINTLOG(I3LOG_WARN, "nBufLength is zero.");
		return false;
	}

	UINT32 i;
	for( i = 0; i < m_nNumValues; i++ )
	{
		if( i3::generic_is_iequal( szLabel, m_pValueTable[i].szLabel ) )
		{
			if( ( nBufLength > 0 ) && ( nBufLength <= m_pValueTable[i].nLength ) )
			{
				I3PRINTLOG(I3LOG_WARN, "nBufLength is too short.");
				//I3TRACE( "%s : nBufLength is too short. (szLabel = %s)\n", __FUNCTION__, szLabel  );
				return false;
			}
			else
			{
				strncpy( Value, m_pValueTable[i].szValue, m_pValueTable[i].nLength + 1 );
				return true;
			}
		}
	}

	//I3TRACE( "%s : Could not find the label named '%s'.\n", __FUNCTION__, szLabel );
	return false;
}

bool i3IniParser::GetValue( const char *szLabel,	INT32 *nValue	)
{
	char *szBuf = m_szTemp;

	if( GetString( szLabel, szBuf, sizeof( m_szTemp) ) )
	{
		if ( i3::generic_is_niequal(szBuf, "T", 1) ||
			i3::generic_is_niequal(szBuf, "Y", 1) )
		{
			*nValue = 1;
		}
		else if (	i3::generic_is_niequal(szBuf, "F", 1) ||
					i3::generic_is_niequal(szBuf, "N", 1) )
		{
			*nValue = 0;
		}
		else
		{
			*nValue = i3::atoi( szBuf);
		}
		return true;
	}

	return false;
}

bool i3IniParser::GetValue( const char *szLabel, UINT32 *nValue	)
{
	char *szBuf = m_szTemp;

	if( GetString( szLabel, szBuf, sizeof( m_szTemp) ) )
	{
		if (i3::generic_is_niequal(szBuf, "T", 1) ||
			i3::generic_is_niequal(szBuf, "Y", 1) )
		{
			*nValue = 1;
		}
		else if( i3::generic_is_niequal( szBuf, "F", 1)  || 
				 i3::generic_is_niequal( szBuf, "N", 1) )
		{
			*nValue = 0;
		}
		else
		{ 
			*nValue = i3::atoi( szBuf);
		}
		return true;
	}

	return false;
}

bool i3IniParser::GetValue( const char *szLabel,	REAL32 *rValue	)
{
	char *szBuf = m_szTemp;

	if( GetString( szLabel, szBuf, sizeof( m_szTemp) ) )
	{
        *rValue = (REAL32)atof( szBuf );
		return true;
	}

	return false;
}

bool i3IniParser::GetValue( const char *szLabel,	REAL64 *rValue	)
{
	char *szBuf = m_szTemp;

	if( GetString( szLabel, szBuf, sizeof( m_szTemp) ) )
	{
        *rValue = atof( szBuf );
		return true;
	}

	return false;
}


bool i3IniParser::GetString( const char *szLabel, const char *szDefault, char *szValue, INT32 nBufLength )
{
    if( GetString( szLabel, szValue, nBufLength ) )
	{
		return true;
	}
	else
	{
		strncpy( szValue, szDefault, nBufLength  );
		return false;
	}
}

bool i3IniParser::GetValue( const char *szLabel, INT32 nDefault, INT32 *nValue	)
{	
    if( GetValue( szLabel, nValue ) )
	{
		return true;
	}
	else
	{
		*nValue = nDefault;
		return false;
	}
}

bool i3IniParser::GetValue( const char *szLabel, UINT32 nDefault, UINT32 *nValue )
{
    if( GetValue( szLabel, nValue ) )
	{
		return true;
	}
	else
	{
		*nValue = nDefault;
		return false;
	}
}

bool i3IniParser::GetValue( const char *szLabel, REAL32 rDefault, REAL32 *rValue )
{
    if( GetValue( szLabel, rValue ) )
	{
		return true;
	}
	else
	{
		*rValue = rDefault;
		return false;
	}
}

bool i3IniParser::GetValue( const char *szLabel, REAL64 rDefault, REAL64 *rValue )
{
    if( GetValue( szLabel, rValue ) )
	{
		return true;
	}
	else
	{
		*rValue = rDefault;
		return false;
	}
}

// ============================================================================
// Get Multiple Values
// ============================================================================
INT32 i3IniParser::GetValues( const char *szLabel, INT32 *nValues, INT32 NumValues, INT32 nDelimiter )
{
	char *szBuf = m_szTemp;

	if( GetString( szLabel, "", szBuf, sizeof( m_szTemp) ) )
	{
		i3SimpleLineLexer Lexer;
		INT32 nRead = Lexer.GetValuesFromString( nValues, NumValues, nDelimiter, szBuf );
		if( nRead != NumValues )
		{
			I3TRACE( "ERROR : '%s' must be specified with %d items.\n", szLabel, NumValues );
			I3ASSERT_0;
		}
		return nRead;
	}

	return 0;
}

INT32 i3IniParser::GetValues( const char *szLabel, REAL32 *rValues, INT32 NumValues, INT32 nDelimiter )
{
	char *szBuf = m_szTemp;

	if( GetString( szLabel, "", szBuf, sizeof( m_szTemp) ) )
	{
		i3SimpleLineLexer Lexer;
		INT32 nRead = Lexer.GetValuesFromString( rValues, NumValues, nDelimiter, szBuf );
		if( nRead != NumValues )
		{
			I3TRACE( "ERROR : '%s' must be specified with %d items.\n", szLabel, NumValues );
			I3ASSERT_0;
		}
		return nRead;
	}

	return 0;
}

// ----------------------------------------------------------------------------
// 기본값 하나 설정
// ----------------------------------------------------------------------------
INT32 i3IniParser::GetValues( const char *szLabel, INT32 nDefault, INT32 *nValues, INT32 NumValues, INT32 nDelimiter )
{
	INT32 nRead = GetValues( szLabel, nValues, NumValues, nDelimiter );

	INT32 i;
	for( i = nRead; i < NumValues; i++ )
	{
		nValues[i] = nDefault;
	}

	return nRead;
}

INT32 i3IniParser::GetValues( const char *szLabel, REAL32 rDefault, REAL32 *rValues, INT32 NumValues, INT32 nDelimiter )
{
	INT32 nRead = GetValues( szLabel, rValues, NumValues, nDelimiter );

	INT32 i;
	for( i = nRead; i < NumValues; i++ )
	{
		rValues[i] = rDefault;
	}

	return nRead;
}

// ----------------------------------------------------------------------------
// 기본값 여러개 설정
// ----------------------------------------------------------------------------
INT32 i3IniParser::GetValues( const char *szLabel, INT32 *nDefault, INT32 *nValues, INT32 NumValues, INT32 nDelimiter )
{
	INT32 nRead = GetValues( szLabel, nValues, NumValues, nDelimiter );

	INT32 i;
	for( i = nRead; i < NumValues; i++ )
	{
		nValues[i] = nDefault[i];
	}

	return nRead;
}

INT32 i3IniParser::GetValues( const char *szLabel, REAL32 *rDefault, REAL32 *rValues, INT32 NumValues, INT32 nDelimiter )
{
	INT32 nRead = GetValues( szLabel, rValues, NumValues, nDelimiter );

	INT32 i;
	for( i = nRead; i < NumValues; i++ )
	{
		rValues[i] = rDefault[i];
	}

	return nRead;
}

// 문자열 배열과 개수를 입력하면 해당되는 문자열과 일치하면 그 문자열의 배열상의 인덱스를 nValue에 설정하고 true를 리턴함.
// 라벨은 찾았으나 일치하는 문자열이 없을 경우 nValue에는 -1을 설정하고, false를 리턴.
// 찾고자 하는 라벨이 없으면 nDefault 값으로 설정해서 false를 리턴. 이 경우 어떤 문자열인지 알고 싶으면 GetTempBuffer()를 사용하면 됨.
bool i3IniParser::GetIndex( const char *szLabel, const char *szIndexString[], UINT32 nDefault, INT32 &nValue, UINT32 nCount )
{
	char *szBuf = m_szTemp;

	if( GetString( szLabel, szBuf, sizeof( m_szTemp) ) )
	{
		UINT32 i;
		for( i = 0; i < nCount; i++ )
		{
			if( i3::generic_is_iequal( szBuf, szIndexString[i] ) )
			{
				nValue = i;
				return true;
			}
		}
		//I3TRACE( "ERROR : [i3IniParser::GetIndex] Unknown string found. Label = '%s', Value = '%s'\n", szLabel, szBuf );
		nValue = -1;
	}
	else
	{
		nValue = nDefault;
	}

	return false;
}

#if !defined( I3_WIPI)
//  WIPI 환경에서는 Variant Argument를 지원하지 않기 때문에
//  제거함.
bool i3IniParser::Write( const char *szLabel, const char *szFmt, ... )
{
	I3ASSERT( m_pStream != nullptr);
	char *szBuf = m_szTemp;
	char szPrintBuf[ I3_INI_BUFFER_SIZE ];

	va_list marker;
	va_start( marker, szFmt );
	vsprintf( szBuf, szFmt, marker );

	if( szLabel == nullptr)
	{
		m_pStream->Printf( szPrintBuf, "%s\r\n", szBuf );
	}
	else
	{
		m_pStream->Printf( szPrintBuf, "%s = %s\r\n", szLabel, szBuf );
	}
	return true;
}

bool i3IniParser::WriteSectionName( const char *szName )
{
	return Write(nullptr, "[%s]", szName );
}

bool i3IniParser::WriteBool( const char *szLabel, bool bTrue )
{
	return Write( szLabel, "%s", bTrue ? "True" : "False" );
}

bool i3IniParser::WriteValue( const char *szLabel, const char *szValue )
{
	char szBuf[256];
	if( ! ProcessString( szBuf, szValue, 256 ) )
	{
		I3ASSERT_0;
		return false;
	}
	return Write( szLabel, "\"%s\"", szBuf );
}

bool i3IniParser::WriteValue( const char *szLabel, INT32 nValue )
{
	return Write( szLabel, "%d", nValue );
}

bool i3IniParser::WriteValue( const char *szLabel, UINT32 nValue )
{
	return Write( szLabel, "%d", nValue );
}

bool i3IniParser::WriteValue( const char *szLabel, REAL32 rValue )
{
	return Write( szLabel, "%f", rValue );
}

bool i3IniParser::WriteValue( const char *szLabel, REAL64 rValue )
{
	return Write( szLabel, "%lf", rValue );
}
#endif
