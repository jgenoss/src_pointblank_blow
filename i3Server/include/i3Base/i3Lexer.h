#if !defined( __i3LEXER_H)
#define __i3LEXER_H

#include "i3CommonType.h"
#include "i3ElementBase.h"
#include "i3MetaTemplate.h"

class I3_EXPORT_BASE i3LineLexer : public i3ElementBase
{
	I3_CLASS_DEFINE( i3LineLexer);

	char *	m_pszMaster;
	char *	m_pszCur;
	char	m_WhiteCodes[256];

	bool IsWhiteCode( char Code);
	bool IsSeparator( char Code, char *SepArr);
public:
	i3LineLexer(void);
	virtual ~i3LineLexer(void);

	void SetMasterString( char *str);
	char *	GetMasterString(void)				{ return m_pszMaster; }

	char *	getCurrentPosition(void)			{ return m_pszCur; }

	INT32  GetNextToken( char *Buf, char * pLastSep, char *Separator = " ");
	INT32  GetNextTokenWithoutWhite( char *Buf, char *Separator);
};


class I3_EXPORT_BASE i3SimpleLineLexer : public i3ElementBase
{
	I3_CLASS_DEFINE( i3SimpleLineLexer);

	char	*m_szText, *m_szNextText;
public:
	i3SimpleLineLexer();
	~i3SimpleLineLexer();

	// szText will be modified.
	void SetText( char *szText );
	char* GetNextToken( INT32 nChar );

	// Get Values
	INT32 GetValuesFromString( INT32 *pInt, INT32 nNum, INT32 nDelimiter, char *szBuf );
	INT32 GetValuesFromString( REAL32 *pReal, INT32 nNum, INT32 nDelimiter, char *szBuf );
};



// ** template **
template <class T>
class i3TLineLexer : public i3ElementBase
{
	T*		m_pszMaster;
	INT32	m_nCur;
	T		m_WhiteCodes[256];

	static const INT32 nDefWhiteCodes = 5;
	T _DefWhiteCodes[nDefWhiteCodes + 1];// = { (T)0x20, (T)'\n', (T)'\r', (T)'\t', (T)'\b', 0};	// 0x20: Space

	bool	IsWhiteCode( T Code);
	bool	IsSeparator( T Code, T* SepArr);
	
public:
	i3TLineLexer( void)
	{
		_DefWhiteCodes[0] = ' ';
		_DefWhiteCodes[1] = '\n';
		_DefWhiteCodes[2] = '\r';
		_DefWhiteCodes[3] = '\t';
		_DefWhiteCodes[4] = '\n';
		_DefWhiteCodes[5] = 0;

		SetMasterString( NULL);
	}
	
	virtual ~i3TLineLexer( void)	{};

	void	SetMasterString( T* str)
	{
		m_pszMaster = str;
		m_nCur = 0;
		memcpy( m_WhiteCodes, _DefWhiteCodes, sizeof(T)*(nDefWhiteCodes+1));
		//I3TRACE("i3TLineLexer::%d\n", sizeof(T));
	}

	T*		GetMasterString( void)		{	return m_pszMaster;		}
	
	INT32	GetCurrentPosition( void)	{	return m_nCur;	}

	INT32	GetNextToken( T* Buf, T* pLastSep, T* Separator);
};

template <class T>
bool i3TLineLexer<T>::IsSeparator( T Code, T* SepArr)
{
	while( *SepArr != 0) {
		if( *SepArr == Code)
			return true;
		SepArr++;
	}
	return false;
}

template <class T>
bool i3TLineLexer<T>::IsWhiteCode( T Code)
{
	T* pTemp = m_WhiteCodes;

	while( *pTemp) {
		if( *pTemp == Code)
			return true;
		pTemp++;
	}
	return false;
}

template <class T>
INT32 i3TLineLexer<T>::GetNextToken( T* Buf, T* pchSep, T* Seperator)
{
	int Count = 0;

	// Skip white space
	while(( m_pszMaster[m_nCur] != 0) && (IsWhiteCode( m_pszMaster[m_nCur]) == true))
	{
		m_nCur++;
	}

	// End of string
	if( m_pszMaster[m_nCur] == 0)
	{
		*pchSep = 0;
		return -1;
	}


	while( m_pszMaster[m_nCur] != 0)
	{
		if( IsSeparator(  m_pszMaster[m_nCur], Seperator) == true)
			break;

		*Buf = m_pszMaster[m_nCur];

		Buf++;
		m_nCur++;
		Count++;
	}

	*Buf = 0;

	if( IsSeparator( m_pszMaster[m_nCur], Seperator) == true)
	{
		*pchSep = m_pszMaster[m_nCur];
		m_nCur++;
		Count ++;
	}
	else
	{
		*pchSep = 0;
	}

	return Count;
}

#endif
