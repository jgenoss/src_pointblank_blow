#include "i3CommonType.h"
#include "i3ErrorHandler.h"
#include "i3Lexer.h"
#include "i3Macro.h"

I3_CLASS_INSTANCE( i3LineLexer, i3ElementBase);

static char _DefaultWhiteCode[] = { 0x20, '\n', '\r', '\t', '\b', 0};	// 0x20: Space

i3LineLexer::i3LineLexer(void)
{
	m_pszMaster = NULL;
	m_pszCur = NULL;
	strcpy( (char *)m_WhiteCodes, _DefaultWhiteCode);
}

i3LineLexer::~i3LineLexer(void)
{
}

bool i3LineLexer::IsSeparator( char Code, char *SepArr)
{
	while(*SepArr != 0) {
		if( *SepArr == Code)
			return true;

		SepArr++;
	}

	return false;
}

bool i3LineLexer::IsWhiteCode( char Code)
{
	char *pTemp = m_WhiteCodes;

	while( *pTemp) {
		if( *pTemp == Code)
			return true;

		pTemp++;
	}

	return false;
}

void i3LineLexer::SetMasterString( char *str)
{
	m_pszCur = m_pszMaster = str;

	strcpy( (char *)m_WhiteCodes, (char *)_DefaultWhiteCode);
}

INT32 i3LineLexer::GetNextToken( char *Buf, char * pchSep, char *Seperator)
{
		int Count = 0;
	unsigned short code;

	// Skip white space
	strcpy( m_WhiteCodes, _DefaultWhiteCode);
	while(( *m_pszCur != 0))
	{
		code = *m_pszCur;

		if( code & 0x80)
		{
			break;
		}
		else
		{
			if( IsWhiteCode( (char )code) != true)
				break;
		}

		m_pszCur++;
	}

	if( *m_pszCur == 0)
	{
		*pchSep = 0;
		return -1;
	}


	sprintf( (PSTR)m_WhiteCodes, (PSTR)"%s%s", _DefaultWhiteCode, Seperator);
	while( *m_pszCur != 0)
	{
		if( *m_pszCur & 0x80)
		{
			*Buf = *m_pszCur;

			m_pszCur++;
			Buf++;
			Count++;
		}
		else
		{
			if( IsSeparator( *m_pszCur, Seperator) == true)
				break;
		}

		*Buf = *m_pszCur;

		Buf++;
		m_pszCur++;
		Count++;
	}

	*Buf = 0;

	if( IsSeparator( *m_pszCur, Seperator) == true)
	{
		*pchSep = *m_pszCur;
		m_pszCur++;
		Count ++;
	}
	else
	{
		*pchSep = 0;
	}

	return Count;
}

INT32  i3LineLexer::GetNextTokenWithoutWhite( char *Buf, char *Seperator)
{
	INT32 Count = 0;

	// Skip white space
	while((*m_pszCur) && (IsWhiteCode( *m_pszCur) == true)) {
		m_pszCur++;
	}

	if( *m_pszCur == 0)
		return 0;

	sprintf( (PSTR)m_WhiteCodes, (PSTR)"%s",Seperator);
	while(( *m_pszCur != 0) && (IsSeparator( *m_pszCur, m_WhiteCodes) == false)) 
	{
		*Buf = *m_pszCur;

		Buf++;
		m_pszCur++;
		Count++;
	}

	*Buf = 0;

	return Count;
}

// ============================================================================
// i3SimpleLineLexer
// ============================================================================
I3_CLASS_INSTANCE( i3SimpleLineLexer, i3ElementBase);

i3SimpleLineLexer::i3SimpleLineLexer(){}
i3SimpleLineLexer::~i3SimpleLineLexer(){}

void i3SimpleLineLexer::SetText( char *szText )
{
	m_szNextText = ( strlen( szText ) > 0 ) ? szText : NULL;
}

char *i3SimpleLineLexer::GetNextToken( INT32 nDelimiter )
{
	I3ASSERT( nDelimiter != 0 );
	if( m_szNextText == NULL )
	{
		return NULL;
	}

	m_szText = m_szNextText;
	m_szNextText = strchr( m_szText, nDelimiter );

	if( m_szNextText != NULL )
	{
		*m_szNextText = '\0';
		m_szNextText++;
		if ( m_szNextText[0] == 0)
		{
			m_szNextText = NULL;
		}
		else
		{
			while( nDelimiter == (INT32)m_szNextText[0] )
			{
				m_szNextText++;
			}
		}
	}

   return m_szText;
}

INT32 i3SimpleLineLexer::GetValuesFromString( INT32 *pInt, INT32 nNum, INT32 nDelimiter, char *szBuf )
{
	if( szBuf == NULL )
	{
		return FALSE;
	}

	SetText( szBuf );

	INT32 i;
	for( i = 0; i < nNum; i ++ )
	{
		char *pToken = GetNextToken( nDelimiter );
		if( pToken == NULL )
		{
			return i;
		}
		else
		{
			char *endptr;
			pInt[i] = (INT32)strtol( pToken, &endptr, 0 );
		}		
	}
	return i;
}

INT32 i3SimpleLineLexer::GetValuesFromString( REAL32 *pReal, INT32 nNum, INT32 nDelimiter, char *szBuf )
{
	if( szBuf == NULL )
	{
		return FALSE;
	}

	SetText( szBuf );

	INT32 i;
	for( i = 0; i < nNum; i ++ )
	{
		char *pToken = GetNextToken( nDelimiter );
		if( pToken == NULL )
		{
			return i;
		}
		else
		{
			pReal[i] = (REAL32)atof( pToken );
		}		
	}
	return i;
}
