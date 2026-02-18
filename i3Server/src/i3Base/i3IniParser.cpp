#include "i3CommonType.h"
#include "i3IniParser.h"
#include "i3ErrorHandler.h"
#include "i3List.h"
#include "i3Macro.h"
#include "i3String.h"
#include "i3Lexer.h"
#include "i3Performance.h"
#include "i3FixedPoint.h"
#include "i3Language.h"

I3_CLASS_INSTANCE( i3IniParser, i3ElementBase );

i3IniParser::i3IniParser()
{
	m_pStream = NULL;
	m_pSectionList = i3List::NewObject();
	m_pValueTable = NULL;
	m_nNumValues = 0;
	m_szSectionName[0] = 0;
	m_bFromFile = FALSE;
	m_bAllowDuplicatedSection = FALSE;

	m_bUnicode = false;
}

i3IniParser::~i3IniParser()
{
	Close();
	I3_SAFE_RELEASE( m_pSectionList );
}

// Create to save
BOOL i3IniParser::Create( const char *szFileName )
{
	Close();

	m_bFromFile = TRUE;

	m_pStream = i3FileStream::NewObject();

	if( ! m_pStream->Create( szFileName, STREAM_WRITE ) )
	{
		I3WARN("Could not create file.\n");
		//I3TRACE( "%s : Could not create file. (%s)\n", __FUNCTION__, szFileName );
		return FALSE;
	}

	return TRUE;
}

// 파일에서 직접 읽는다.
BOOL i3IniParser::OpenFromFile( const char *szFileName, BOOL bAllowDuplicatedSection )
{
	Close();

	m_bFromFile = TRUE;

	m_pStream = i3FileStream::NewObject();

	if( ! m_pStream->Open( szFileName, STREAM_READ | STREAM_SHAREREAD ) )
	{
		I3WARN("Could not open file.\n");
		//I3TRACE( "%s : Could not open file. (%s)\n", __FUNCTION__, szFileName );
		return FALSE;
	}

	Open( m_pStream, bAllowDuplicatedSection, TRUE );

	char szDrive[MAX_PATH];
	char szPath[MAX_PATH];
	char szExt[MAX_PATH];
	_splitpath( szFileName, szDrive, szPath, m_szi3INIFileName, szExt );
	i3String::Copy( m_szi3INIFileNameExt, m_szi3INIFileName, MAX_PATH );
	i3String::Concat( m_szi3INIFileNameExt, szExt );

	return TRUE;
}

void i3IniParser::Close()
{
	if( m_bFromFile )
	{
		m_bFromFile = FALSE;
		I3_SAFE_RELEASE( m_pStream );
	}
	else
	{
		m_pStream = NULL;
	}


	INT32 i;

	for( i = 0; i < m_pSectionList->GetCount(); i++)
	{
		void *p = m_pSectionList->Items[i];

		i3MemFree( p);
	}

	m_pSectionList->Clear();
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

		WideCharToMultiByte( i3Language::GetCurrentCodePage(), 0, szReadLine, i3String::Length(szReadLine), szLine, READBUF_SIZE * 2, NULL, NULL);

		I3ASSERT( i3String::Length( szLine) < READBUF_SIZE * 2);
	}
	else
	{
		readsize = m_pStream->ReadLine( szLine, READBUF_SIZE * 2);
	}

	return readsize;
}

// 스트림에서 읽는다.
BOOL i3IniParser::Open( i3Stream *pStream, BOOL bAllowDuplicatedSection, BOOL bFromFile )
{
	if (pStream->IsExactTypeOf(i3FileStream::GetClassMeta()))
	{
		I3TRACE("i3IniParser::Open() FileName:%s\n",((i3FileStream*)pStream)->GetFileName());
	}
	if( ! bFromFile )
	{
		Close();
	}

	if( pStream == NULL )
	{
		I3WARN("Stream is NULL.\n");

		//I3TRACE( "%s : Stream is NULL.\n", __FUNCTION__ );
		return FALSE;
	}

	m_bAllowDuplicatedSection = bAllowDuplicatedSection;
	m_pStream = pStream;

	UINT32 nPos = m_pStream->GetPosition();

	WCHAR16 code;
	UINT32 rc = m_pStream->Read( &code, sizeof( WCHAR16));
	if( rc == STREAM_ERR)
	{
		return FALSE;
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
	char *szCur = NULL;

	i3IniSectionInfo *pPrevInfo = NULL;
	UINT32 nLineCount = 0;

	// 한줄씩 읽는다.
	while( _ReadLine( szReadBuf))
	{
		// 앞뒤로 공백 문자 제거
		RemoveWhiteSpace( szBuf, szReadBuf );

		// '['문자로 시작하고
		if( szBuf[0] =='[' )
		{
			INT32 nLastPos = i3String::Length( szBuf ) - 1;
			// ']' 문자로 끝나면
			if( szBuf[ nLastPos ] == ']' )
			{
				// szCur 에 '['와']'사이의 문자열을 설정하고.
				szBuf[ nLastPos ] = '\0';
				szCur = &szBuf[1];

				// 문자열의 길이를 확인하고,
				if( i3String::Length( szCur ) <= 0 )
				{
					I3WARN("Section name is NULL.\n");
					//I3TRACE( "%s : Section name is NULL.\n", __FUNCTION__ );
					I3ASSERT_0;
				}
				else
				{
					if( ! m_bAllowDuplicatedSection )
					{
						// 똑같은 이름이 있는지 확인하고
						if( FindSection( szCur ) != NULL )
						{
							I3WARN("Section name is duplicated\n");
							//I3TRACE( "%s : Section name is duplicated(%s).\n", __FUNCTION__, szCur );
							I3ASSERT_0;
							return FALSE;
						}
					}

					// 섹션 하나 추가
					//I3HMEM hMem = i3MemAlloc( sizeof(i3IniSectionInfo));
					i3IniSectionInfo *pInfo = (i3IniSectionInfo *)i3MemAlloc( sizeof(i3IniSectionInfo));

					i3String::Copy( pInfo->Name, szCur, I3_INI_NAME_SIZE);		// 섹션 이름 저장
					pInfo->Position = m_pStream->GetPosition(); // Stream 위치 저장
					m_pSectionList->Add( pInfo );				// 리스트에 추가

					// 이전 섹션의 라인수 설정
					if( pPrevInfo != NULL )
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
	if( pPrevInfo != NULL )
	{
		pPrevInfo->NumLines = nLineCount - 1;
	}

	return TRUE;
}

// 공백 문자 제거 하면서 복사한다. 
// 단 [] 혹은 ""으로 묶여 있는 문자열 내부의 공백은 그대로 복사한다.
void i3IniParser::RemoveWhiteSpace( char *szOut, const char *szInput )
{
   BOOL bString  = FALSE, bSection = FALSE;

   INT32 nLen = i3String::Length( (char*)szInput );
   INT32 nCur = 0;

	for (INT32 i = 0 ; i < nLen ; i++)
	{
		BOOL bCopy = TRUE;

		switch (szInput[i])
		{
		// 섹션 구분
		case '[':
			{
				I3ASSERT( !bSection );
				bSection = TRUE;
			}
			break;
		case ']':
			{
				I3ASSERT( bSection );
				bSection = FALSE;
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
					bString = bString ? FALSE : TRUE;
					bCopy = FALSE;
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
					bCopy = FALSE;
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

BOOL i3IniParser::ProcessString( char *szOut, const char *szInput, INT32 nOutBufLength )
{
	INT32 nLen = i3String::Length( (char*)szInput );
	INT32 nCur = 0;

	I3ASSERT( nOutBufLength != 0 );

	for (INT32 i = 0 ; i < nLen ; i++)
	{
		//BOOL bCopy = TRUE;

		switch (szInput[i])
		{
		case '\\':
		case '"':
			{
				if( nOutBufLength > 0 && nCur >= nOutBufLength ) return FALSE;
				szOut[nCur++] = '\\';

				if( nOutBufLength > 0 && nCur >= nOutBufLength ) return FALSE;
				szOut[nCur++] = szInput[i];
			}
			break;

		case '\r': continue;

		case '\n':
			{
				if( nOutBufLength > 0 && nCur >= nOutBufLength ) return FALSE;
				szOut[nCur++] = '\\';

				if( nOutBufLength > 0 && nCur >= nOutBufLength ) return FALSE;
				szOut[nCur++] = 'n';
			}
			break;

		default:
			{
				if( nOutBufLength > 0 && nCur >= nOutBufLength ) return FALSE;
				szOut[nCur++] = szInput[i];
			}
			break;
		}
	}

	if( nOutBufLength > 0 && nCur >= nOutBufLength ) return FALSE;
	szOut[nCur++] = '\0';

	return TRUE;
}

i3IniSectionInfo* i3IniParser::FindSection( const char *szName )
{
	INT32 i, n = m_pSectionList->GetCount();
	for( i = 0; i < n; i++ )
	{
		i3IniSectionInfo *pInfo = (i3IniSectionInfo*)m_pSectionList->GetItem( i );
		if( i3String::Compare( pInfo->Name, szName ) == 0 )
		{ 
			return pInfo;
		}
	}
	return FALSE;
}

INT32 i3IniParser::GetSectionIndex( const char *szName )
{
	INT32 i, n = m_pSectionList->GetCount();
	for( i = 0; i < n; i++ )
	{
		i3IniSectionInfo *pInfo = (i3IniSectionInfo*)m_pSectionList->GetItem( i );
		if( i3String::Compare( pInfo->Name, szName ) == 0 )
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
BOOL i3IniParser::ReadSection( const char *szName )
{
	// 섹션 찾기
	i3IniSectionInfo *pInfo = FindSection( szName );
	return ReadSectionByInfo( pInfo );
}

// 지정된 인덱스의 섹션의 모든 값을 읽어서 테이블로 만든다.
BOOL i3IniParser::ReadSectionByIndex( INT32 nIndex )
{
	i3IniSectionInfo *pInfo = NULL;
	if( nIndex < m_pSectionList->GetCount() )
	{
		pInfo =	(i3IniSectionInfo*)m_pSectionList->GetItem( nIndex );
	}
	return ReadSectionByInfo( pInfo );
}

// 지정된 섹션의 모든 값을 읽어서 테이블로 만든다.
BOOL i3IniParser::ReadSectionByInfo( i3IniSectionInfo *pInfo )
{
	m_szSectionName[0] = 0;

	if( m_pStream == NULL )
	{
		I3WARN("m_pStream is NULL.\n");
		//I3TRACE( "ERROR : [%s] m_pStream is NULL.\n", __FUNCTION__ );
		return FALSE;
	}

	if( pInfo == NULL )
	{
		I3WARN("pInfo is NULL.\n");
		//I3TRACE( "ERROR : [%s] pInfo is NULL.\n", __FUNCTION__ );
		return FALSE;
	}

	strcpy( m_szSectionName, pInfo->Name );

	// Value Table 준비
	I3MEM_SAFE_FREE_POINTER( m_pValueTable );
	if( pInfo->NumLines <= 0 )
	{
		return TRUE;
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
		if( pSeperator == NULL )
		{
			continue;
		}
		*pSeperator = 0;
		char *pValue = pSeperator + 1;

		// 라벨 길이 검사
		if( strlen( szBuf ) >= I3_INI_NAME_SIZE )
		{
			// 너무 길면 스킵
			I3WARN("szLabel name is too long\n");
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
				I3WARN("szLabel value string is too long \n");
				//I3TRACE( "%s : szLabel value string is too long (%s)\n", __FUNCTION__, pValue );
				I3ASSERT_0;
				continue;
			}
			else
			{
				// 라벨과 값 문자열 복사
				strcpy( m_pValueTable[m_nNumValues].szLabel, szBuf );
				strcpy( m_pValueTable[m_nNumValues].szValue, pValue );
				m_pValueTable[m_nNumValues].nLength = nValueLength;
			}
		}
		m_nNumValues++;
	}

	return TRUE;
}

const char *i3IniParser::GetStringValue( const char *szLabel ) // Use internal buffer and return its pointer.
{
	if( m_pValueTable == NULL )
	{
		I3WARN("Value Table is not exist. \n");
		//I3TRACE( "%s : Value Table is not exist.\n", __FUNCTION__ );
		return NULL;
	}

	UINT32 i;
	for( i = 0; i < m_nNumValues; i++ )
	{
		if( i3String::Compare( szLabel, m_pValueTable[i].szLabel ) == 0 )
		{
			return m_pValueTable[i].szValue;
		}
	}

	//I3TRACE( "%s : Could not find the label named '%s'.\n", __FUNCTION__, szLabel );
	return NULL;
}

BOOL i3IniParser::GetValue( const char *szLabel,	char *Value, INT32 nBufLength )
{	
	if( m_pValueTable == NULL )
	{
		I3WARN("Value Table is not exist.\n");
		//I3TRACE( "%s : Value Table is not exist.\n", __FUNCTION__ );
		return FALSE;
	}

	if( nBufLength == 0 )
	{
		I3WARN("nBufLength is zero. \n");
		//I3TRACE( "%s : nBufLength is zero.\n", __FUNCTION__ );
		return FALSE;
	}

	UINT32 i;
	for( i = 0; i < m_nNumValues; i++ )
	{
		if( i3String::Compare( szLabel, m_pValueTable[i].szLabel ) == 0 )
		{
			if( ( nBufLength > 0 ) && ( nBufLength <= m_pValueTable[i].nLength ) )
			{
				I3WARN("nBufLength is too short. \n");
				//I3TRACE( "%s : nBufLength is too short. (szLabel = %s)\n", __FUNCTION__, szLabel  );
				return FALSE;
			}
			else
			{
				strcpy( Value, m_pValueTable[i].szValue );
				return TRUE;
			}
		}
	}

	//I3TRACE( "%s : Could not find the label named '%s'.\n", __FUNCTION__, szLabel );
	return FALSE;
}

BOOL i3IniParser::GetValue( const char *szLabel,	INT32 *nValue	)
{
	char *szBuf = m_szTemp;

	if( GetValue( szLabel, szBuf, I3_INI_VALUE_SIZE ) )
	{
		if( ( i3String::NCompare( szBuf, "T", 1 ) == 0 ) || ( i3String::NCompare( szBuf, "Y", 1 ) == 0 ) )
		{
			*nValue = TRUE;
		}
		else if ( ( i3String::NCompare( szBuf, "F", 1 ) == 0 ) || ( i3String::NCompare( szBuf, "N", 1 ) == 0 ) )
		{
			*nValue = FALSE;
		}
		else
		{
			*nValue = i3String::ToInt( szBuf);
		}
		return TRUE;
	}

	return FALSE;
}

BOOL i3IniParser::GetValue( const char *szLabel, UINT32 *nValue	)
{
	char *szBuf = m_szTemp;

	if( GetValue( szLabel, szBuf, I3_INI_VALUE_SIZE ) )
	{
		if( ( i3String::NCompare( szBuf, "T", 1) == 0 ) || ( i3String::NCompare( szBuf, "Y", 1) == 0 ) )
		{
			*nValue = TRUE;
		}
		else if( ( i3String::NCompare( szBuf, "F", 1) == 0 ) || ( i3String::NCompare( szBuf, "N", 1) == 0 ) )
		{
			*nValue = FALSE;
		}
		else
		{ 
			*nValue = i3String::ToInt( szBuf);
		}
		return TRUE;
	}

	return FALSE;
}

BOOL i3IniParser::GetValue( const char *szLabel,	REAL32 *rValue	)
{
	char *szBuf = m_szTemp;

	if( GetValue( szLabel, szBuf, I3_INI_VALUE_SIZE ) )
	{
        *rValue = (REAL32)atof( szBuf );
		return TRUE;
	}

	return FALSE;
}

BOOL i3IniParser::GetValue( const char *szLabel,	REAL64 *rValue	)
{
	char *szBuf = m_szTemp;

	if( GetValue( szLabel, szBuf, I3_INI_VALUE_SIZE ) )
	{
        *rValue = atof( szBuf );
		return TRUE;
	}

	return FALSE;
}


BOOL i3IniParser::GetValue( const char *szLabel, const char *szDefault, char *szValue, INT32 nBufLength )
{
    if( GetValue( szLabel, szValue, nBufLength ) )
	{
		return TRUE;
	}
	else
	{
		strcpy( szValue, szDefault );
		return FALSE;
	}
}

BOOL i3IniParser::GetValue( const char *szLabel, INT32 nDefault, INT32 *nValue	)
{	
    if( GetValue( szLabel, nValue ) )
	{
		return TRUE;
	}
	else
	{
		*nValue = nDefault;
		return FALSE;
	}
}

BOOL i3IniParser::GetValue( const char *szLabel, UINT32 nDefault, UINT32 *nValue )
{
    if( GetValue( szLabel, nValue ) )
	{
		return TRUE;
	}
	else
	{
		*nValue = nDefault;
		return FALSE;
	}
}

BOOL i3IniParser::GetValue( const char *szLabel, REAL32 rDefault, REAL32 *rValue )
{
    if( GetValue( szLabel, rValue ) )
	{
		return TRUE;
	}
	else
	{
		*rValue = rDefault;
		return FALSE;
	}
}

BOOL i3IniParser::GetValue( const char *szLabel, REAL64 rDefault, REAL64 *rValue )
{
    if( GetValue( szLabel, rValue ) )
	{
		return TRUE;
	}
	else
	{
		*rValue = rDefault;
		return FALSE;
	}
}

// ============================================================================
// Get Multiple Values
// ============================================================================
INT32 i3IniParser::GetValues( const char *szLabel, INT32 *nValues, INT32 NumValues, INT32 nDelimiter )
{
	char *szBuf = m_szTemp;

	if( GetValue( szLabel, "", szBuf, I3_INI_VALUE_SIZE ) )
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

	if( GetValue( szLabel, "", szBuf, I3_INI_VALUE_SIZE ) )
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

// 문자열 배열과 개수를 입력하면 해당되는 문자열과 일치하면 그 문자열의 배열상의 인덱스를 nValue에 설정하고 TRUE를 리턴함.
// 라벨은 찾았으나 일치하는 문자열이 없을 경우 nValue에는 -1을 설정하고, FALSE를 리턴.
// 찾고자 하는 라벨이 없으면 nDefault 값으로 설정해서 FALSE를 리턴. 이 경우 어떤 문자열인지 알고 싶으면 GetTempBuffer()를 사용하면 됨.
BOOL i3IniParser::GetIndex( const char *szLabel, const char *szIndexString[], UINT32 nDefault, INT32 &nValue, UINT32 nCount )
{
	char *szBuf = m_szTemp;

	if( GetValue( szLabel, szBuf, I3_INI_VALUE_SIZE ) )
	{
		UINT32 i;
		for( i = 0; i < nCount; i++ )
		{
			if( i3String::Compare( szBuf, szIndexString[i] ) == 0 )
			{
				nValue = i;
				return TRUE;
			}
		}
		//I3TRACE( "ERROR : [i3IniParser::GetIndex] Unknown string found. Label = '%s', Value = '%s'\n", szLabel, szBuf );
		nValue = -1;
	}
	else
	{
		nValue = nDefault;
	}

	return FALSE;
}

#if !defined( I3_WIPI)
//  WIPI 환경에서는 Variant Argument를 지원하지 않기 때문에
//  제거함.
BOOL i3IniParser::Write( const char *szLabel, const char *szFmt, ... )
{
	I3ASSERT( m_pStream != NULL );
	char *szBuf = m_szTemp;
	char szPrintBuf[ I3_INI_BUFFER_SIZE ];

	va_list marker;
	va_start( marker, szFmt );
	vsprintf( szBuf, szFmt, marker );

	if( szLabel == NULL )
	{
		m_pStream->Printf( szPrintBuf, "%s\r\n", szBuf );
	}
	else
	{
		m_pStream->Printf( szPrintBuf, "%s = %s\r\n", szLabel, szBuf );
	}
	return TRUE;
}

BOOL i3IniParser::WriteSectionName( const char *szName )
{
	return Write( NULL, "[%s]", szName );
}

BOOL i3IniParser::WriteBool( const char *szLabel, BOOL bTrue )
{
	return Write( szLabel, "%s", bTrue ? "True" : "False" );
}

BOOL i3IniParser::WriteValue( const char *szLabel, const char *szValue )
{
	char szBuf[256];
	if( ! ProcessString( szBuf, szValue, 256 ) )
	{
		I3ASSERT_0;
		return FALSE;
	}
	return Write( szLabel, "\"%s\"", szBuf );
}

BOOL i3IniParser::WriteValue( const char *szLabel, INT32 nValue )
{
	return Write( szLabel, "%d", nValue );
}

BOOL i3IniParser::WriteValue( const char *szLabel, UINT32 nValue )
{
	return Write( szLabel, "%d", nValue );
}

BOOL i3IniParser::WriteValue( const char *szLabel, REAL32 rValue )
{
	return Write( szLabel, "%f", rValue );
}

BOOL i3IniParser::WriteValue( const char *szLabel, REAL64 rValue )
{
	return Write( szLabel, "%lf", rValue );
}
#endif
