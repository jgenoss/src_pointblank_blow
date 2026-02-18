// i3String.h

#if !defined( __I3_STRINGTOOLS_H)
#define __I3_STRINGTOOLS_H

#include "i3CommonType.h"
#include "i3Memory.h"

class i3StringList;

typedef struct i3SymbolTableInfo
{
	char	m_szSymbol[64];
	char	m_szValue[256];
} I3_SYMBOLTABLE_INFO;

namespace i3String
{
	//////////////////////////////////////////////////////////////////////////////////////
	/// 실수를 문자열로 변환한다.
	///
	/// REAL64 Value		: 변환하려는 실수값
	/// char *Result		: 문자열 결과를 받기 위한 버퍼의 포인터
	/// INT32 UnderCount	: 문자열로 변환할 때, 남길 소숫점 아래의 개수
	/// bool FillZero	: 소숫점 아래자리가 UnderCount에 미치지 않을 경우, 나머지 자리를
	///					  0으로 채울 것인지 말것인지 결정하는 인자
	///					  ex.) 1.2를 소숫점 3자리까지 만들때
	///						   FillZero = false -> 1.2
	///						   FillZero = true  -> 1.200
	//
	// 작성자			: 이진균
	// 작성일자			: 1999.6.1
	I3_EXPORT_BASE void MakePointFix( REAL64 Value, char *Result, INT32 UnderCount, bool FillZero);

	/////////////////////////////////////////////////////////////////////////////////////////
	/// 값을 Comma(,)를 삽입한 포맷으로 문자열 변환한다.
	/// 
	/// LONG Value		: 문자열로 변환할 값
	/// LPTSTR StrBuf	: 결과 문자열을 반환할 버퍼의 포인터
	/// INT32 Length		: 제한할 최대 길이
	/// 
	/// return 
	///			= 0		: Error
	///			> 0		: 변환된 문자열의 총 길이(Bytes)
	///
	/// ex)	123456780 => 1,234,567,890
	//
	// 작성자			: 이진균
	// 작성일자			: 1999.6.1
	I3_EXPORT_BASE INT32 ValueToStr( INT32 Value, char * szOut, INT32 Length);

	////////////////////////////////////////////////////////////////////////////////////////
	/// 문자열에서 처음 발견되는 valid character(출력가능한 문자)의 위치를 포인터로 반환한다.
	///
	/// return		: 문자열에서 처음 발견되는 문자의 포인터
	/// 
	/// note) white character로 판별하는 경우
	///		 space, tab, carrige return(\r), new line(\n)
	///		
	///		만약 문자열이 빈 문자열일 경우 올바르게 동작하지 않을 수 있다.
	//
	// 작성자		: 이진균
	// 작성일자		: 1999.6.1
	I3_EXPORT_BASE char *GetFirstValidCharater( char * lpszStr);

	////////////////////////////////////////////////////////////////////////////////////////
	/// 문자열에서 특정 인덱스의 문자가 영문, 한글 첫번째 Byte, 또는 한굴 두번째 Byte인지를
	/// 판별하여 반환한다.
	/// 
	/// char *str	: 문자열의 포인터
	/// INT32 index	: 판별하려는 문자의 인덱스 (0 based)
	///
	/// return
	///			영문			: 0
	///			한글 1st byte	: 1
	///			한글 2nd byte	: 2
	//
	// 작성자		: 이진균
	// 작성일자		: 1999.6.1
	I3_EXPORT_BASE INT32 WhatIsCode( char * str, INT32 index);

	///////////////////////////////////////////////////////////////////////////////////////
	/// 문자열의 특정 위치에 다른 문자열을 삽입한다.
	///
	/// char * Dest	: 문자열을 삽입할 문자열
	/// char * Src	: 삽입될 문자열
	/// INT32 Pos		: 문자열을 삽입할 인덱스 (0 based)
	/// INT32 Len		: 삽입할 문자열의 길이
	///
	/// \Return		: 삽입된 문자열의 선두 포인터
	//
	// 작성자		: 이진균
	// 작성일자		: 1999.6.1
	I3_EXPORT_BASE char * InsertStr( char * Dest, char * Src, INT32 Pos, INT32 Len);
	I3_EXPORT_BASE WCHAR16 * InsertStrW( WCHAR16 * Dest, WCHAR16 * Src, INT32 Pos, INT32 Len);

	//////////////////////////////////////////////////////////////////////////////////////
	/// 문자열의 앞부분에서부터 공백 문자를 제거한다.
	/// 출력-가능 문자가 발견되면 처리를 종료한다. (뒷부분의 공백문자를 처리하지 않음)
	///
	/// char *str	: 공백문자를 삭제할 문자열
	/// 
	/// \Return		: None
	//
	// 작성자		: 이진균
	// 작성일자		: 1999.6.1
	I3_EXPORT_BASE void RemoveBlankFromHead( char * str);

	/////////////////////////////////////////////////////////////////////////////////////////
	/// 문자열의 뒷부분에서부터 공백 문자를 삭제한다.
	/// 출력-가능한 문자가 발견되면 처리를 종료한다. (앞부분의 공백문자를 처리하지 않음)
	///
	/// char *str	: 공백문자를 제거할 문자열의 포인터
	///
	/// \Return		: none
	//
	// 작성자		: 이진균
	// 작성일자		: 1999.6.1
	I3_EXPORT_BASE void RemoveBlankFromTail( char * str);

	/////////////////////////////////////////////////////////////////////////////////////////
	/// 문자열 전체에서 모든 공백문자를 제거한다.
	/// 출력-공백문자(Space, Tab, Newline, ...)등을 모두 제거한 문자열
	///
	/// char *str	: 공백문자를 제거할 문자열의 포인터
	///
	/// \Return		: none
	//
	// 작성자		: 이진균
	// 작성일자		: 1999.6.1
	I3_EXPORT_BASE void RemoveAllBlack( char *str);

	/////////////////////////////////////////////////////////////////////////////////////////
	// 파일명을 담은 문자열에서 뒷부부분의 확장자를 제거한 문자열로 만든다.
	///
	/// char *fname	: 문자열의 포인터
	///
	/// \Return		: none
	//
	// 작성자		: 이진균
	// 작성일자		: 1999.6.1
	I3_EXPORT_BASE void RemoveExt( char *fname);

	/////////////////////////////////////////////////////////////////////////////////////////
	// 파일명을 담은 문자열에서 뒷부부분의 확장자를 새로운 확장자로 교체한다.
	///
	/// char *szOut				: 저장될 문자열
	/// char *szIn				: 원본 문자열
	/// const char *szNewExt	: 새로운 확장자
	///
	/// \Return		: none
	//
	// 작성자		: 차재천
	// 작성일자		: 2005.8.19
	I3_EXPORT_BASE void SwapExt( char *szOut, const char *szIn, const char *szNewExt );


	I3_EXPORT_BASE char * SplitFileName( const char *fname, char *Buf, BOOL WithExt);

	I3_EXPORT_BASE void	  SplitPath( const char * pszSrc, char * pszDest);

	/////////////////////////////////////////////////////////////////////////////////////////
	/// 파일명을 담은 문자열에서 뒷부부분의 확장자만을 문자열로 만든다.
	///
	/// char *fname	: 문자열의 포인터
	/// char *ExtBuf : 확장자를 담을 문자열 포인터(NULL일 경우 개수만 리턴)
	///
	/// \Return		: 확장자 문자열의 문자 개수
	//
	// 작성자		: 경주현
	// 작성일자		: 1999.6.21
	I3_EXPORT_BASE INT32 SplitFileExt( const char *fname, char *BufExt = NULL, int BufExtSize = 0 );

	I3_EXPORT_BASE void RemoveDriveChar( char *path);

	I3_EXPORT_BASE void ResolvePathToken( const char * pszPath, i3StringList * pList);

	/////////////////////////////////////////////////////////////////////////////////////////
	/// 주어진 경로를 기준으로 상대 경로로 변환하여 반환한다.
	///
	///
	/// 작성자		: 이진균
	I3_EXPORT_BASE void MakeRelativePath( const char * pszBase, const char * pszAbsPath, char * pszRelPath);

	I3_EXPORT_BASE void MakeCStylePath( char * pszDest, char * pszSrc);

	/////////////////////////////////////////////////////////////////////////////////////////
	/// 문자열에 있는 모든 공백을 제거한다.
	///
	/// char *str	: 문자열의 포인터
	/// char *Buf    : 제거한 문자열을 저장할 버퍼(NULL일 경우 제외)
	///
	/// \Return		: 공백을 제거한 문자열의 문자 개수
	//
	// 작성자		: 경주현
	// 작성일자		: 1999.7.19
	I3_EXPORT_BASE INT32 EraseSpace(const char *str, char *Buf);

	I3_EXPORT_BASE bool IsDigitString( const char *str);

	I3_EXPORT_BASE void ParseMultiSelectFileName( char *FileName, i3StringList *List);

	////////////////////////////////////////////////////////////////////////////////////////
	// 파일명에 접미사를 붙인다.
	//
	// 예) "Test.txt" + "_KOR" => "Test_KOR.txt"
	//
	I3_EXPORT_BASE void AddPostfix( char *szOut, const char *szIn, const char *szPostfix, char chDot = '.' );

	////////////////////////////////////////////////////////////////////////////////////////
	/// 주어진 경로의 끝에 역슬레쉬(\)가 존재하는 경우, 역슬레쉬를 없앤다.
	/// 
	/// LPSTR Path	: 경로명을 저장한 문자열
	///
	/// \Return		: None
	//
	// 작성자		: 이진균
	// 작성일자		: 1999.6.1
	I3_EXPORT_BASE void NormalizePath( char* Path);

	////////////////////////////////////////////////////////////////////////////////////////
	/// 주어진 경로 문자열에 역슬래쉬(\)가 존재하는 경우, 슬래쉬(/)로 바꾼다.
	I3_EXPORT_BASE void MakeUnixPath( char *szPath );

	////////////////////////////////////////////////////////////////////////////////////////
	I3_EXPORT_BASE void	PreprocessMacro( char * pszDest, INT32 Length, const char * pszSrc, I3_SYMBOLTABLE_INFO * pTable, INT32 symbolCount);

	/////////////////////////////////////////////////////////////////////////////////////////
	/// 파일명을 담은 문자열에서 파일명을 제거하고 디렉토리 부분만을 문자열로 만든다.
	///
	/// char *Path	: 문자열의 포인터
	///
	//
	// 작성자		: 차재천
	// 작성일자		: 2005.01.24
	I3_EXPORT_BASE void RemoveFileName( char *Path );

	I3_EXPORT_BASE bool	IsMatchedWildcard( char * pszFileName, char * pszRull);

	/// 2개의 문자열 비교 (대소문자 안 가림.)
	/// 
	/// \Return		: 차이값 (같으면 0 )
	I3_EXPORT_BASE INT32 Compare( const char* str1, const char* str2);
	I3_EXPORT_BASE INT32 CompareW( const WCHAR16* str1, const WCHAR16* str2);
	I3_EXPORT_BASE INT32 NCompare( const char* str1, const char* str2, INT32 size);		//	by KOMET (2005.09.29)
	I3_EXPORT_BASE INT32 NCompareW( const char* str1, const char* str2, INT32 size);		//	by KOMET (2005.09.29)

	/// 2개의 ASCII 문자열 비교 (대소문자 가림.)
	/// 
	/// \Return		: 차이값 (같으면 0 )
	I3_EXPORT_BASE INT32 CompareCase( const char* str1, const char* str2);
	I3_EXPORT_BASE INT32 CompareCaseW( const WCHAR16* str1, const WCHAR16* str2);

	I3_EXPORT_BASE void ToUpper( char * pStr);
	I3_EXPORT_BASE void ToUpperAfterN ( char * pStr, INT32 nCount);
	I3_EXPORT_BASE void ToUpperBeforeN( char * pStr, INT32 nCount);

	/// pDest에 pSrc의 내용을 복사한다.
	I3_EXPORT_BASE void Copy( char * pDest, const char * pSrc);
	I3_EXPORT_BASE void CopyW( WCHAR16 * pDest, const WCHAR16 * pSrc);
	I3_EXPORT_BASE void NCopy( char * pDest, const char *pSrc, INT32 Length);
	I3_EXPORT_BASE void NCopyW( WCHAR16 * pDest, const WCHAR16 *pSrc, INT32 Length);

	I3_EXPORT_BASE void CopyASCIItoWCHAR( WCHAR16 * pDest, const char * pSrc );

	// 유니코드 스트링을 UTF-8포맷으로 인코딩
	I3_EXPORT_BASE int UCS2toUTF8( char *pDest, const WCHAR16 *pSrc, int nDestBufSize );

	// UTF-8포맷을 유니코드 스트링으로 디코딩
	I3_EXPORT_BASE int UTF8toUCS2( WCHAR16 *pDest, const char *pSrc, int nDestBufSize );

	/// 문자열의 길이를 구한다.
	I3_EXPORT_BASE INT32 Length( const char * pStr1 );
	I3_EXPORT_BASE INT32 LengthW( const WCHAR16 * pStr );

	// pStr 문자열 안에 ch가 있으면 그 위치를 반환한다.
	// 앞에서 부터 찾는다.
	// 없으면 -1 반환
	I3_EXPORT_BASE INT32 ContainChar( const char * pStr, char ch );

	// pStr 문자열 안에 ch가 있으면 그 위치를 반환한다.
	// 뒤에서 부터 찾는다.
	// 없으면 -1 반환
	I3_EXPORT_BASE INT32 ContainCharRev( const char * pStr, char ch );

	/// pStr1 문자열 안에 pStr2가 들어 있으면 그 위치를 인덱스로 돌려준다.
	// 없으면 -1 반환
	I3_EXPORT_BASE INT32 Contain( const char * pStr1, const char * pStr2);
	I3_EXPORT_BASE INT32 ContainW( const WCHAR16 * pStr1, const WCHAR16 * pStr2);

	/// pDest 문자열에 pSrc를 덧붙인다.
	I3_EXPORT_BASE void Concat( char * pDest, const char * pSrc);
	I3_EXPORT_BASE void ConcatW(WCHAR16 * pDest, const WCHAR16 * pSrc); 

	/// szIn과 같은 내용의 문자열을 생성해서 리턴한다.
	//I3_EXPORT_BASE char* NewString( const char* szIn, I3HMEM *pMemID = NULL );
	I3_EXPORT_BASE char* NewString( const char* szIn );

	I3_EXPORT_BASE void	ftoa( REAL32 fval, char * pszStr, INT32 fractLen = -1);

	I3_EXPORT_BASE INT32	ToInt( const char * pszStr);

	I3_EXPORT_BASE void		GetPrefixName( char * pszDest, const  char * pszStr);
}

#endif
