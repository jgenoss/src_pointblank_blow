#ifndef __NSM_STRING_H__
#define __NSM_STRING_H__

#include "NSMHeader.h"
/*
typedef struct i3SymbolTableInfo
{
	char	m_szSymbol[64];
	char	m_szValue[256];
} SYMBOLTABLE_INFO;
*/

namespace NSMString
{
	EXPORT_BASE void 	Copy( char * pDest, const char * pSrc);
	EXPORT_BASE void 	CopyW( WCHAR16 * pDest, WCHAR16 * pSrc);
	EXPORT_BASE void 	CopyASCIItoWCHAR( WCHAR16 * pDest, const char * pSrc );
	EXPORT_BASE void	CopyWCHARtoASCII( char * pDest, INT32 nDestLength, const WCHAR16 * pSrc, INT32 nSrcLength);

	EXPORT_BASE INT32	Length( const char * pStr1 );

	EXPORT_BASE void	Concat( char * pDest, const char * pSrc);
	EXPORT_BASE INT32	Compare( const char* str1, const char* str2);
	EXPORT_BASE INT32	CompareCase( const char* str1, const char* str2);
	EXPORT_BASE INT32	NCompare( const char* str1, const char* str2, INT32 size);		//	by KOMET (2005.09.29)
	EXPORT_BASE void	NCopy( char * pDest, const char *pSrc, INT32 Length);
	EXPORT_BASE void	NConcat( char * pDest, const char * pSrc, INT32 Length );
	EXPORT_BASE INT32	ToInt( const char * pszStr);
	EXPORT_BASE void	Format( char * pszDest, INT32 len, const char * pszFormat, ...);

	/* 필요한것만 풀어 봅시다. 필요없는거 풀지 말고요. 
	EXPORT_BASE void MakePointFix( REAL64 Value, char *Result, INT32 UnderCount, bool FillZero);
	EXPORT_BASE INT32 ValueToStr( INT32 Value, char * szOut, INT32 Length);
	EXPORT_BASE char *GetFirstValidCharater( char * lpszStr);
	EXPORT_BASE INT32 WhatIsCode( char * str, INT32 index);
	EXPORT_BASE char * InsertStr( char * Dest, char * Src, INT32 Pos, INT32 Len);
	EXPORT_BASE WCHAR16 * InsertStrW( WCHAR16 * Dest, WCHAR16 * Src, INT32 Pos, INT32 Len);
	EXPORT_BASE void RemoveBlankFromHead( char * str);
	EXPORT_BASE void RemoveBlankFromTail( char * str);
	EXPORT_BASE void RemoveAllBlank( char *str);	
	EXPORT_BASE void RemoveAllChar( char *str, char RemoveChar);
	EXPORT_BASE void RemoveExt( char *fname);
	EXPORT_BASE void SwapExt( char *szOut, const char *szIn, const char *szNewExt );
	EXPORT_BASE char * SplitFileName( const char *fname, char *Buf, BOOL WithExt);
	EXPORT_BASE void SplitPath( const char * pszSrc, char * pszDest);
	EXPORT_BASE INT32 SplitFileExt( const char *fname, char *BufExt = NULL, int BufExtSize = 0 );
	EXPORT_BASE void RemoveDriveChar( char *path);		
	EXPORT_BASE void MakeCStylePath( char * pszDest, char * pszSrc);
	EXPORT_BASE INT32 EraseSpace(const char *str, char *Buf);
	EXPORT_BASE bool IsDigitString( const char *str);	
	EXPORT_BASE void AddPostfix( char *szOut, const char *szIn, const char *szPostfix, char chDot = '.' );
	EXPORT_BASE void NormalizePath( char* Path);
	EXPORT_BASE void MakeUnixPath( char *szPath );
	EXPORT_BASE void PreprocessMacro( char * pszDest, INT32 Length, const char * pszSrc, SYMBOLTABLE_INFO * pTable, INT32 symbolCount);
	EXPORT_BASE void RemoveFileName( char *Path );
	EXPORT_BASE bool IsMatchedWildcard( char * pszFileName, char * pszRull);	
	EXPORT_BASE INT32 CompareW( const WCHAR16* str1, const WCHAR16* str2);	
	EXPORT_BASE INT32 NCompareW( const char* str1, const char* str2, INT32 size);		//	by KOMET (2005.09.29)
	
	EXPORT_BASE INT32 CompareCaseW( const WCHAR16* str1, const WCHAR16* str2);
	EXPORT_BASE void ToUpper( char * pStr);
	EXPORT_BASE void ToUpperAfterN ( char * pStr, INT32 nCount);
	EXPORT_BASE void ToUpperBeforeN( char * pStr, INT32 nCount);		
	
	EXPORT_BASE void NCopyW( WCHAR16 * pDest, const WCHAR16 *pSrc, INT32 Length);		

	EXPORT_BASE int UCS2toUTF8( char *pDest, const WCHAR16 *pSrc, int nDestBufSize );

	EXPORT_BASE int UTF8toUCS2( WCHAR16 *pDest, const char *pSrc, int nDestBufSize );
	
	EXPORT_BASE INT32 LengthW( const WCHAR16 * pStr );

	EXPORT_BASE INT32 ContainChar( const char * pStr, char ch );

	EXPORT_BASE INT32 ContainCharRev( const char * pStr, char ch );

	EXPORT_BASE INT32 Contain( const char * pStr1, const char * pStr2);
	EXPORT_BASE INT32 ContainW( const WCHAR16 * pStr1, const WCHAR16 * pStr2);
	EXPORT_BASE INT32 ContainNative( const char * pStr);	

	EXPORT_BASE void ConcatW(WCHAR16 * pDest, const WCHAR16 * pSrc); 

	EXPORT_BASE char* NewString( const char* szIn );

	EXPORT_BASE void	ftoa( REAL32 fval, char * pszStr, INT32 fractLen = -1);

	EXPORT_BASE INT32	ToInt( const char * pszStr);

	EXPORT_BASE void		GetPrefixName( char * pszDest, const  char * pszStr);

	EXPORT_BASE BOOL Replace(char * str, INT32 bufLen, const char * before, const char * affer);

	EXPORT_BASE const char *	CharNext(INT32 nCodePage, const char * pszStr);
	EXPORT_BASE const char *	CharPrev(INT32 nCodePage, const char * pszStart, const char * pszCurrent);
	EXPORT_BASE INT32 LengthChar(INT32 nCodePage, const char * pszStr);
	EXPORT_BASE BOOL IsComposite(INT32 nCodePage, const char prev, const char curr, INT32 mode);

	EXPORT_BASE void Format( char * pszDest, INT32 len, const char * pszFormat, ...);

	enum ALGORITHM
	{
		BF = 0,		//	Brute Force			(가장 단순함)
		KMP,		//	Knuth-Morris-Pratt	(내부에서 동적 메모리할당이 발생)
		BM,			//	Boyer-Moore			(내부에서 동적 메모리할당이 발생)
		QUICK,		//	Quick Search		(Boyer-Moore 경량판)
		RK,			//	Rabin-Karp			(해시를 이용)
	};

	//	string searching algorithm
	EXPORT_BASE INT32 Search(char * pszDest, char * pszSrc, ALGORITHM type);
	*/
}

#endif
