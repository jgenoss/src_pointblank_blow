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

#endif
