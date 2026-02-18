#include "pch.h"
#include "HtmlParser.h"

void CHtmlParser::addTag( char* pszTag )
{
	//i3String::Copy( m_pszTag[ m_i8numTag++ ], pszTag );
	i3String::Copy( m_pszTag[ m_i8numTag++ ], pszTag, MAX_TAG_LENGTH_HTMLPARSER );
}

BOOL CHtmlParser::Compare( char* pszTag, char* pszValue )
{
	if ( m_bParsed )
	{
		for( INT8 i = 0; i < m_i8numTag; i++ )
		{
			//if ( 0 == i3String::Compare( m_pszTag[ i ], pszTag ) )
			if ( 0 == i3String::Compare( m_pszTag[ i ], pszTag ) )
			{
				//if ( 0 == i3String::Compare( m_pszValue[ i ], pszValue ) ) return TRUE;
				if ( 0 == i3String::Compare( m_pszValue[ i ], pszValue ) ) return TRUE;
				else return FALSE;
			}
		}
	}
	return FALSE;
}

BOOL CHtmlParser::CompareByIdx( INT8 i8Index, char* pszValue )
{
	if ( m_bParsed )
	{
		//if ( 0 == i3String::Compare( m_pszValue[ i8Index ], pszValue ) ) return TRUE;
		if ( 0 == i3String::Compare( m_pszValue[ i8Index ], pszValue ) ) return TRUE;
		else return FALSE;
	}
	return FALSE;
}

// 2011.06.16
INT32 CHtmlParser::GetINT32( INT8 i8Index )
{
	if ( m_bParsed )
	{
		return atoi( m_pszValue[i8Index] );
	}
	return -1;
}

// Target과 String이 같은 문자열인지 비교하되, String 문자열의 길이 만큼만 비교한다. Target은 String내용보다 더 길어도 관계없다.
// runParse에서만 사용된다.
static bool	compareSp( char* Target, char* String )
{
	while( *String != '\0' )
	{
		if ( *Target != *String || *Target == '\0' ) return false; // 간혹 Target이 String보다 짧을 경우가 있으므로 || *Target == '\0' 코드가 없다면 Address: Target + a 의 메모리침범이 일어날 수 있음.
		Target++;
		String++;
	}

	return true;
}

// src에서 dest로 문자열 복사를 실행하되 src의 '\0'을 끝이 아닌 chEndCase를 끝으로 인식한다.
static INT32 copyEndCase( char* dest, char* src, char chEndCase )
{
	INT32 i32len = 0;

	while( *src != chEndCase )
	{
		*dest = *src;
		src++;
		dest++;
		i32len++;
	}

	*dest = '\0';
	return i32len;
}

void CHtmlParser::RunParse( char* pszText )
{
	// 이미 파싱한 기록이 있을시 이전의 Value값을 전부 삭제한다.
	if ( m_bParsed )
	{
		for( INT32 i = 0; i < MAX_TAG_COUNT_HTMLPARSER; i++ )
		{
			m_pszValue[ i ][ 0 ] = '\0';
		}
	}
	else
	{
		m_bParsed	= true;
	}

	INT32	i32CurrentTag	= 0;
	char*	pstrTemp		= pszText;
	char*	pszCurrentTag	= m_pszTag[ 0 ];
	
	while( *pstrTemp != '\0' )
	{
		// 파싱구문 <Tag>Value</Tag>에서 <Tag>태그를 찾는 부분
		if ( compareSp( pstrTemp, pszCurrentTag ) )
		{
			pstrTemp += strlen( pszCurrentTag );		// Tag 길이 만큼 증가

			// 태그가 정상으로 닫혀있으면
			if ( *pstrTemp == '>')
			{
				pstrTemp++;
				pstrTemp += copyEndCase( m_pszValue[ i32CurrentTag ], pstrTemp, '<' );
				pstrTemp += strlen( pszCurrentTag ) + 3;	// </Tag> 길이 만큼 증가
			}
			else
			{
				// 태그가 비정상적으로 닫혀있다면 그냥 패스
			}

			i32CurrentTag++;
			if ( i32CurrentTag == m_i8numTag ) return;
			pszCurrentTag = m_pszTag[ i32CurrentTag ];
		}
		else
		{
			pstrTemp++;
		}
	}
}

/*
void CHtmlParser::Run( char* pszText )
{
	INT8	i8CurrentTag	= 0;
	m_bRun	= TRUE;
	//char* pstrTemp;
	
	for( INT32 iCount = 0; pszText[ iCount ] != '\0' ; iCount++ )
	{
		INT8 i8p;

		// 파싱구문 <Tag>Value</Tag>에서 <Tag>태그를 찾는 부분
		if ( pszText[ iCount ] != '<' ) continue;

		for( i8p = 0; m_pszTag[ i8CurrentTag ][ i8p ] != '\0'; i8p++ )
		{
			if ( pszText[ iCount + i8p + 1 ] != m_pszTag[ i8CurrentTag ][ i8p ] ) continue;
		}

		if ( pszText[ iCount + i8p + 1 ] != '>' ) continue;

		iCount += i8p + 2;
		INT32	i32Ptr	= 0;

		// 파싱구문<Tag>Value</Tag>을 발견해서 실제 값(Value)을 추출하는 부분
		for(;; iCount++ )
		{
			// <Example>의 파싱이 끝나고 </Example>의 첫부분 '<'가 나왔을시 해당 단위 파싱 종료
			if ( pszText[ iCount ] == '<' )
			{
				m_pszValue[ i8CurrentTag++ ][ i32Ptr ] = '\0';
	
				// 주어진 태그갯수만큼 전부 파싱완료시 파싱작업종료
				if ( i8CurrentTag == m_i8numTag ) return;
				// 아직 미달시 계속해서 파싱
				else break;
			}
			else m_pszValue[ i8CurrentTag ][ i32Ptr++ ] = pszText[ iCount ];
		}
	}
}*/