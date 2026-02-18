#include "stdafx.h"
#include "Memory.h"
#include <stdio.h>
#include "String.h"

void NSMString::Copy( char * pDest, const char * pSrc)
{
	while( *pSrc)
	{
		*pDest = *pSrc;

		pDest++;
		pSrc++;
	}

	*pDest = 0;
}

void NSMString::CopyW(WCHAR16 * pDest, WCHAR16 * pSrc)
{
	while( *pSrc)
	{
		*pDest = *pSrc;

		pDest++;
		pSrc++;
	}

	*pDest = 0;
}

void NSMString::CopyASCIItoWCHAR( WCHAR16 * pDest, const char * pSrc )
{
	while( *pSrc )
	{
		*pDest = WCHAR16( *pSrc ); 
		pDest++;
		pSrc++;
	}
	*pDest = 0;
}

INT32 NSMString::Length( const char * pStr1)
{
	INT32 Count = 0;

	while( *pStr1)
	{
		Count++;
		pStr1++;
	}

	return Count;
}

void NSMString::Concat( char * pDest, const char * pSrc)
{
	while( *pDest)
	{
		pDest++;
	}
	
	while( *pSrc)
	{
		*pDest = *pSrc;
		
		pDest++;
		pSrc++;
	}
	
	*pDest = 0;
}

INT32 NSMString::Compare( const char* str1, const char* str2)
{
#ifdef USE_C_LIBRARY_FUNCS

	return _stricmp( str1, str2 );

#else
	INT32 C1, C2;

	if ( str1 == NULL )
	{
		if ( str2 == NULL )
			return 0;
		else
			return -1;
	}
	else if ( str2 == NULL )
	{
		return 1;
	}

	do
	{
		C1 = *str1++;
		C2 = *str2++;

		if( C1 != C2)
		{		
			//	특수 문자 때문에 대문자를 소문자로 바꿔서 연산하는 방식으로 수정함. 다른 함수도 테스트후 수정 필요. by 2005.09.30 (KOMET)
			if((C1 >= 'A') && (C1 <= 'Z'))		
				C1 += 'a' - 'A';

			if((C2 >= 'A') && (C2 <= 'Z'))
				C2 += 'a' - 'A';
			
			if( C1 != C2)
				return C1 < C2 ? -1 : 1;
		}
	}
	while( C1 );

	return 0;
#endif
}

INT32 NSMString::CompareCase( const char* str1, const char* str2)
{
#ifdef USE_C_LIBRARY_FUNCS

	return strcmp( str1, str2 );

#else
	char C1, C2;

	do
	{
		C1 = *str1;
		C2 = *str2;

		if( C1 != C2)
			return C1 - C2;

		str1++;
		str2++;
	} while((*str1 != 0) && (*str2 != 0));

	C1 = *str1;
	C2 = *str2;

	return C1 - C2;
#endif
}

INT32 NSMString::NCompare( const char* str1, const char* str2, INT32 size)
{
#ifdef USE_C_LIBRARY_FUNCS

	return _strnicmp( str1, str2, size );

#else
	INT32	C1, C2;
	INT32	nSize = 0;

	if ( str1 == NULL )
	{
		if ( str2 == NULL )
			return 0;
		else
			return -1;
	}
	else if ( str2 == NULL )
	{
		return 1;
	}

	do 
	{
		C1 = *str1;
		C2 = *str2;

		if((C1 >= 'a') && (C1 <= 'z'))
			C1 += 'A' - 'a';

		if((C2 >= 'a') && (C2 <= 'z'))
			C2 += 'A' - 'a';

		if( C1 != C2)
			return C1 - C2;

		str1++;
		str2++;
		nSize++;
	} 
	while((*str1 != 0) && (*str2 != 0) && (nSize < size));

	if(*str1 == 0)
		return *str1 - *str2;

	return 0;
#endif
}

void NSMString::NCopy( char * pDest, const char *pSrc, INT32 Length)
{
	INT32 Count = 0;

	while( *pSrc)
	{
		*pDest = *pSrc;

		pDest++;
		pSrc++;
		Count++;

		if( Count >= Length)
			break;
	}

	*pDest = 0;
}

void NSMString::NConcat( char * pDest, const char * pSrc, INT32 Length )
{
	INT32 Count = 0;

	while( *pDest)
	{
		pDest++;
		Count++;
	}
	if( Count >= Length ) return;
	
	while( *pSrc)
	{
		*pDest = *pSrc;
		
		pDest++;
		pSrc++;
		Count++;

		if( Count >= Length ) break;;
	}
	
	*pDest = 0;
}

enum ATOI_MODE
{
	ATOI_DECIMAL = 0,
	ATOI_HEXA,
};

INT32 NSMString::ToInt( const char * pszStr)
{
	ATOI_MODE mode = ATOI_DECIMAL;
	INT32 rv = 0, sign = 1;
	char ch;

	if((pszStr[0] == '0') && ((pszStr[1] == 'x') || (pszStr[1] == 'X')))
	{
		mode = ATOI_HEXA;
		pszStr += 2;
	}

	while( * pszStr)
	{
		switch( *pszStr)
		{
			case '-' :	sign = -1;	break;
			case '+' :				break;
			default :

				if( mode == ATOI_DECIMAL)
				{
					if( (*pszStr >= '0') && (*pszStr <= '9'))
					{
						rv *= 10;
						rv += *pszStr - '0';
					}
					else
						return 0;
				}
				else
				{
					rv <<= 4;

					if( (*pszStr >= '0') && (*pszStr <= '9'))
					{
						rv += *pszStr - '0';
					}
					else 
					{
						if( (*pszStr >= 'a') && (*pszStr <= 'f'))
							ch = *pszStr - 'a';
						else if((*pszStr >= 'A') && (*pszStr <= 'F'))
							ch = *pszStr - 'A';
						else
							return 0;

						rv += ch + 10;
					}
				}
				break;
		}

		pszStr++;
	}

	return rv * sign;
}

void NSMString::Format( char * pszDest, INT32 len, const char * pszFormat, ...)
{
	va_list marker;

	va_start( marker, pszFormat);

	int Max = len - 1;
	int Rv = _vsnprintf_s( pszDest, len, _TRUNCATE, pszFormat, marker);

	if ((Rv < 0) || (Rv >= Max))
	{
		// need to null terminate the string
		pszDest[Max] = '\0';
	}

	va_end( marker);
}

void NSMString::CopyWCHARtoASCII( char * pDest, INT32 nDestLength, const WCHAR16 * pSrc, INT32 nSrcLength)
{
	WideCharToMultiByte( GetACP(), 0, pSrc, nSrcLength, pDest, nDestLength, NULL, NULL);
}

/*
void NSMString::MakePointFix( REAL64 Value, char *Result, INT32 UnderCount, bool FillZero)
{ 
	char conv[128];
    int i, j, FractCount;
    bool UnderFlag, fFindNull;

    sprintf( (PSTR)conv, (PSTR) "%f", Value);

    FractCount = 0;
    UnderFlag = false;		// 소숫점이 발견되었으며, 현재 소숫점 이하 자리를 처리하고 있음을 나타낸다.
    fFindNull = false;

    for( i = 0, j = 0; (UnderFlag != true) || (FractCount < UnderCount);) 
	{
    	switch( conv[i]) {
        	case '.' :
            	UnderFlag = true;
                Result[j] = '.';
                break;

            case 0 :
            	if( FillZero == false) {
                	UnderFlag = true;
                    FractCount = UnderCount;
                    fFindNull = true;
                }
                break;

            default :
            	if( UnderFlag == false) {
                	Result[j] = conv[i];
                }
                else {
                   	if( fFindNull == true)
                       	Result[j] = '0';
                    else
                       	Result[j] = conv[i];

                    FractCount ++;

                    if( FractCount >= UnderCount) {
                    	if( fFindNull != true) {
                        	if( conv[i+1] >= '5') {
                            	bool Borrow;
                            	int Saved = j;

                                do {
                                	Borrow = false;

                                    if( Result[j] == '.') {
                                    	Borrow = true;		// for cheat...
                                        j--;
                                    	continue;
                                    }

                                	if( Result[j] == '9') {
                                    	Result[j] = '0';

                                        j--;
                                        if( j < 0) {
                                        	// 반올림이 계속되어 정수자리를 넘어 단위자체가 올라가야 하는 경우...
                                            // 전체 문자열을 한칸 뒤로 밀고 첫자리에 '1'을 삽입한다.
                                            // ex) 999.99 에서 소숫점 2자리에서 반올림하여 1자리로 만드는 경우 1000.0 이 되어야 한다.
                                            for( i = Saved; i >= 0; i--) {
                                            	Result[i+1] = Result[i];
                                            }
                                            j = 0;
                                            Result[j] = '1';
                                            Saved ++;
                                        }
                                        else
                                        	Borrow = true;
                                    }
                                    else
                                		Result[j] ++;
                                } while(Borrow == true);

                                j = Saved;
                            }
                        }
                    }
                }
            	break;
        }

        if( conv[i] != 0)
        	i++;
        j++;
    }

    Result[j] = 0;
}

INT32 NSMString::ValueToStr( INT32 Value, char * szOut, INT32 Length)
{
	int i, MaxLen, StrLength;
	char conv[128];

	sprintf( (PSTR) conv, (PSTR) "%ld", Value);
	StrLength = (int) strlen( conv);

	if( StrLength <= 0)
	{
		return 0;
	}

	MaxLen = StrLength + ( StrLength - 1 ) / 3;
	if( MaxLen >= Length )
	{
		return 0;
	}

	*(szOut + MaxLen) = 0;

	char *pIn = conv + StrLength - 1;
	char *pOut = szOut + MaxLen - 1;
	for( i = 0; i < MaxLen; i++ )
	{
		if( (i + 1) % 4 == 0 )
		{
			*pOut = ',';
		}
		else
		{
			*pOut = *pIn;
			pIn--;
		}
		pOut--;
	}

	return MaxLen;
}

char * NSMString::GetFirstValidCharater( char * lpszStr)
{
	char *p = lpszStr;

	while( (*p == ' ') || (*p == '\t') || (*p == '\r') || (*p == '\n'))
		p++;

	return p;
}

INT32 NSMString::WhatIsCode( char * str, INT32 index)
{
	int i, rv;

	rv = 0;

	for( i = 0; i <= index; i++) {
		if( rv == 1)
			rv = 2;
		else if( *str & 0x80) 
			rv = 1;
		else 
			rv = 0;

		str++;
	}

	return rv;
}

char * NSMString::InsertStr( char * Dest, char * Src, INT32 Pos, INT32 Len)
{
	char *p, *q;
	INT32 Count, i;

	Count = (INT32)strlen( Dest) - Pos;

	p = Dest;
	while( *p) p++;

	q = p + Len;

	for( i = 0; i < Count+1; i++) {
		*q = *p;
		p--;
		q--;
	}

	p = Dest + Pos;
	q = Src;

	for( i = 0; i < Len; i++) {
		*p = *q;
		p++;
		q++;
	}

	return Dest; 
}	

WCHAR16 * NSMString::InsertStrW( WCHAR16 * Dest, WCHAR16 * Src, INT32 Pos, INT32 Len)
{
	WCHAR16 *p, *q;
	int Count, i;

	Count = (int)LengthW( Dest) - Pos;

	p = Dest;
	while( *p) p++;

	q = p + Len;

	for( i = 0; i < Count+1; i++) {
		*q = *p;
		p--;
		q--;
	}

	p = Dest + Pos;
	q = Src;

	for( i = 0; i < Len; i++) {
		*p = *q;
		p++;
		q++;
	}

	return Dest; 
}	

void NSMString::RemoveBlankFromHead( char * str)
{
	unsigned char *sp, *tp;

	sp = (unsigned char *) str;
	while((*sp <= 32) && (*sp != 0))
	{
		sp ++;
	}

	if( *sp == 0)
		return;

	tp = (unsigned char *) str;

	while( *sp) {
		*tp = *sp;
		tp++;
		sp++;
	}
	*tp = 0;
}

void NSMString::RemoveBlankFromTail( char * str)
{
	unsigned char *sp;
	int len;

	len = (int)strlen(str);
	sp = (unsigned char *)(str + (len - 1));

	while(len && *sp < 0x21)
	{
		sp--;
		len--;	// 길이 만큼만 처리하여 index underflow를 막는다. - praptor
	}

	sp++;
	*sp = 0;
}

void NSMString::RemoveAllBlank( char *str)
{
	unsigned char *p;

	p = (unsigned char *)str;

	while( *p) {
		if( !isspace( *p) || (*p > 127)) {
			*str = *p;
			str++;
		}
		p++;
	}
	*str = 0;
}

void NSMString::RemoveAllChar( char *str, char RemoveChar)
{
	unsigned char *p = (unsigned char *) str;

	while( *p) {
		if( *p != RemoveChar) {
			*str = *p;
			str++;
		}
		p++;
	}
	*str = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
// 파일명을 담은 문자열에서 뒷부부분의 확장자를 제거한 문자열로 만든다.
//
// char *fname	: 문자열의 포인터
//
// Return		: none
//
// 작성자		: 이진균
// 작성일자		: 1999.6.1
void NSMString::RemoveExt( char *fname)
{
	char *p;

	p = fname + strlen(fname);

	while((p > fname) && (*p != '.')) {
		p--;
	}

	if( *p == '.') {
		*p = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
// 파일명을 담은 문자열에서 뒷부부분의 확장자를 새로운 확장자로 교체한다.
///
/// char *szPath			: 문자열의 포인터
/// const char *szNewExt	: 새로운 확장자		예) "txt"
///
/// \Return		: none
//
// 작성자		: 차재천
// 작성일자		: 2005.8.19
void NSMString::SwapExt( char *szOut, const char *szIn, const char *szNewExt )
{
	if( szOut != szIn )
	{
		Copy( szOut, szIn );
	}
	RemoveExt( szOut );
	Concat( szOut, "." );
    Concat( szOut, szNewExt );
}

char * NSMString::SplitFileName( const char *fname, char *Buf, BOOL WithExt)
{
	// considerable cases
	// 1. C:\\Directory\\FileName.EXT
	// 2. ..\Directory\\FileName.EXT
	// 3. FileName.EXT
	// 4. FileName
	INT32 len = (INT32)strlen( fname);
	char *pStart, *pResult;

	if( len <= 0)
		return (char *) fname;

	pStart = (char *)(fname + len - 1);
	while((pStart > fname) && (*pStart != '\\') && (*pStart != '/')) 
		pStart --;

	if((*pStart == '\\') || (*pStart == '/'))
		pStart++;

	pResult = pStart;

	while( *pStart) {
		if( Buf != NULL) {
			*Buf = *pStart;

			Buf++;
		}

		pStart++;

		if( WithExt == FALSE) 
			if( *pStart == '.') {

				if( Buf != NULL)
					*Buf = 0;
				return pResult;
			}
	}

	if( Buf != NULL)
		*Buf = 0;

	return pResult;
}

void NSMString::SplitPath( const char * pszSrc, char * pszDest)
{
	char * pszTemp, *pszEnd;

	pszEnd = (char *) pszSrc + strlen( pszSrc);

	// Path의 끝점을 찾는다.
	while( (pszEnd > pszSrc))
	{
		if( (*pszEnd == '\\') || (*pszEnd == '/'))
			break;

		pszEnd --;
	}

	// 앞쪽에서부터 pszEnd 위치까지 복사
	pszTemp = (char *) pszSrc;

	while( pszTemp < pszEnd)
	{
		*pszDest = *pszTemp;
		pszDest++;
		pszTemp++;
	}

	*pszDest = 0;
}

INT32 NSMString::SplitFileExt( const char *fname, char *BufExt, int  )
{
	int Count = 0;

	for (int i = (int)strlen(fname) - 1; i >= 0; i--) {

		if ( fname[i] == '.' ) {

			if (BufExt != NULL)
			{
				NSM_nsMemory::Copy( BufExt, (void *)(fname + i + 1), Count);

				BufExt[ Count] = 0;
			}

			break;
		}
		else
			Count++;
	}

	return Count;
}

void NSMString::RemoveDriveChar( char *path)
{
	char *pTemp;

	pTemp = path;

	while( *pTemp != 0) {
		if( *pTemp == ':')
			break;

		pTemp++;
	}

	if( *pTemp == 0)
		return;

	pTemp++;

	while( *pTemp != 0) {
		*path = *pTemp;

		path++;
		pTemp++;
	}

	*path = 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
// 문자열에 있는 모든 공백을 제거한다.
//
// char *str	: 문자열의 포인터
// char *Buf    : 제거한 문자열을 저장할 버퍼(NULL일 경우 제외)
//
// Return		: 공백을 제거한 문자열의 문자 개수
//
// 작성자		: 경주현
// 작성일자		: 1999.7.19
INT32 NSMString::EraseSpace(const char *str, char *Buf)
{
	unsigned int Count = 0;

	while( *str )
	{
		if( *str != ' ' )
		{
			*Buf = *str;
			Buf++;
			Count++;
		}
		str++;
	}

	*Buf = 0;

	return Count;
}

bool NSMString::IsDigitString( const char *str)
{
	while( *str) {
		if( ! (isdigit( *str) || (*str == '.') || (*str == '+') || (*str == ':') || (*str == '-'))) {
			return false;
		}

		str++;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////
// 파일명에 접미사를 붙인다.
//
// 예) "Test.txt" + "_KOR" => "Test_KOR.txt"
//
void NSMString::AddPostfix( char *szOut, const char *szIn, const char *szPostfix, char chDot )
{
	char szTemp[MAX_PATH];

	INT32 nDotPos;
	nDotPos = ContainCharRev( szIn, chDot );
	if( nDotPos < 0 )
	{	// chDot 이 없는 경우, 그냥 맨 뒤에다가 붙인다.
		Copy( szTemp, szIn );
		Concat( szTemp, szPostfix );
	}
	else if( nDotPos == 0 )
	{	// chDot 이 맨처음에 하나만 있는 경우, 접미사를 복사하고 그 뒤에다가 나머지를 붙인다.
		Copy( szTemp, szPostfix );
		Concat( szTemp, szIn );
	}
	else
	{	// chDot 이전의 내용을 복사
		NCopy( szTemp, szIn, nDotPos );
		// 접미사 붙이기
		Concat( szTemp, szPostfix );
		// 나머지 부분 붙이기
		Concat( szTemp, &szIn[nDotPos] );
	}

	Copy( szOut, szTemp );
}

////////////////////////////////////////////////////////////////////////////////////////
// 주어진 경로의 끝에 역슬레쉬(\)가 존재하는 경우, 역슬레쉬를 없앤다.
// 
// LPSTR Path	: 경로명을 저장한 문자열
//
// Return		: None
//
// 작성자		: 이진균
// 작성일자		: 1999.6.1
void NSMString::NormalizePath( char* Path)
{
	char* p;

    p = Path;
    while( *p) p++;

    do {
    	*p = 0;
        p--;
    } while( ( *p == '\\' ) || ( *p == '/' ) );
}

void NSMString::MakeUnixPath( char *szPath )
{
	//I3ASSERT( szPath != NULL );
	char *p = szPath;
	while( *p != 0 )
	{
		if( *p == '\\' )
		{
			*p = '/';
		}
		p++;
	}
}

/// 파일명을 담은 문자열에서 파일명을 제거하고 디렉토리 부분만을 문자열로 만든다.
void NSMString::RemoveFileName( char *Path )
{	
	char* p;

	p = Path;
	if( *p == 0 ) return;

	while( *p) p++;

	do
	{
		*p = 0;
		p--;
	}
	while( (*p != '\\') && (*p != '/') && (p > Path) );
	*p = 0;
}

INT32 NSMString::CompareW( const WCHAR16* str1, const WCHAR16* str2)
{
	WCHAR16 C1, C2;
	do
	{
		C1 = *str1;
		C2 = *str2;

		if((C1 >= WCHAR16('a') ) && (C1 <= WCHAR16('z')))
			C1 += WCHAR16('A' - 'a');

		if((C2 >= WCHAR16('a')) && (C2 <= WCHAR16('z')))
			C2 += WCHAR16('A' - 'a');

		if( C1 != C2)
			return C1 - C2;

		str1++;
		str2++;
	}
	while((*str1 != 0) && (*str2 != 0));

	C1 = *str1;
	C2 = *str2;

	return C1 - C2;
}

INT32 NSMString::NCompareW( const char* str1, const char* str2, INT32 size)
{
	INT32	C1, C2;
	INT32	nSize = 0;

	do 
	{
		C1 = *str1;
		C2 = *str2;

		if((C1 >= WCHAR16('a')) && (C1 <= WCHAR16('z')))
			C1 += 'A' - 'a';

		if((C2 >= WCHAR16('a')) && (C2 <= WCHAR16('z')))
			C2 += WCHAR16('A') - WCHAR16('a');

		if( C1 != C2)
			return C1 - C2;

		str1++;
		str2++;
		nSize++;
	}
	while((*str1 != 0) && (nSize < size));

	if(*str1 == 0)
		return *str1 - *str2;

	return 0;
}

INT32 NSMString::CompareCaseW( const WCHAR16 * str1, const WCHAR16 * str2 )
{
	WCHAR16 C1, C2;

	do
	{
		C1 = *str1;
		C2 = *str2;

		if( C1 != C2)
		{
			return C1 - C2;
		}

		str1++;
		str2++;
	}
	while( (*str1 != 0) && (*str2 != 0) );

	C1 = *str1;
	C2 = *str2;

	return C1 - C2;
}

INT32 NSMString::LengthW( const WCHAR16 * pStr )
{
	INT32 Count = 0;

	while( *pStr )
	{
		Count++;
		pStr++;
	}

	return Count;
}

// pStr 문자열 안에 ch가 있으면 그 위치를 반환한다.
// 앞에서 부터 찾는다.
// 없으면 -1 반환
INT32 NSMString::ContainChar( const char * pStr, char ch )
{
	INT32 idx = 0;

	const char *pCur = pStr;

	for( pCur = pStr, idx = 0; *pCur != 0; pCur++, idx++ )
	{
		if( *pCur == ch )
		{
			return idx;
		}
	}

	return -1;
}

// pStr 문자열 안에 ch가 있으면 그 위치를 반환한다.
// 뒤에서 부터 찾는다.
// 없으면 -1 반환
INT32 NSMString::ContainCharRev( const char * pStr, char ch )
{
	INT32 nLen = Length( pStr );
	if( nLen <= 0 ) return -1;

	INT32 i;
	for( i = nLen - 1; i >= 0; i-- )
	{
		if( pStr[i] == ch )
		{
			return i;
		}
	}

	return -1;
}

INT32 NSMString::Contain( const char * pStr1, const char * pStr2)
{
	INT32 Len;
	INT32 Idx = 0, StartIdx = 0;

	Len = Length( pStr2);

	while( *pStr1)
	{
		if( *pStr1 == pStr2[Idx])
		{
			if( Idx == 0)
				StartIdx = Idx;

			Idx++;

			if( Idx >= Len)
				return StartIdx;
		}
		else
		{
			if( Idx != 0 )
			{
				pStr1--;
			}
			Idx = 0;
		}

		pStr1++;
	}

	return -1;
}

INT32 NSMString::ContainNative( const char * pStr)
{
	INT32 Idx = 0;

	while( *pStr)
	{
		if( *pStr & 0x80)
		{
			return Idx;
		}

		Idx++;
		pStr++;
	}

	return -1;
}

INT32 NSMString::ContainW( const WCHAR16 * pStr1, const WCHAR16 * pStr2)
{
	INT32 Len;
	INT32 Idx = 0, StartIdx = 0;

	Len = LengthW( pStr2);

	while( *pStr1)
	{
		if( *pStr1 == pStr2[Idx])
		{
			if( Idx == 0)
				StartIdx = Idx;

			Idx++;

			if( Idx >= Len)
				return StartIdx;
		}
		else
		{
			if( Idx != 0 )
			{
				pStr1--;
			}
			Idx = 0;
		}

		pStr1++;
	}

	return -1;
}

void NSMString::ToUpper( char * pStr)
{
	while( *pStr)
	{
		if((*pStr >= 'a') && (*pStr <= 'z'))
		{
			*pStr = *pStr - ('a' - 'A');
		}

		pStr++;
	}
}

// nCount이후부터 대문자로 바꾸는 함수
void NSMString::ToUpperAfterN(char* pStr, INT32 nCount)
{
	INT32 i = 0;

	while(*pStr)
	{
		i++;
		if(i >= nCount)
		{
			break;
		}
	}
	
	ToUpper(pStr);
}

// nCount전까지 대문자로 바꾸는 함수
void NSMString::ToUpperBeforeN(char* pStr, INT32 nCount)
{
	INT32 i = 0;
	
	while( *pStr)
	{
		if(i >= nCount)
		{
			break;
		}
		
		if((*pStr >= 'a') && (*pStr <= 'z'))
		{
			*pStr = *pStr - ('a' - 'A');
		}

		pStr++;
	}
}

void NSMString::NCopyW( WCHAR16 * pDest, const WCHAR16 *pSrc, INT32 Length )
{
	INT32 Count = 0;

	while( *pSrc)
	{
		*pDest = *pSrc;

		pDest++;
		pSrc++;
		Count++;

		if( Count >= Length)
			break;
	}

	*pDest = 0;
}

void NSMString::ConcatW(WCHAR16 * pDest, const WCHAR16 * pSrc)
{
	while( *pDest)
	{
		pDest++;
	}
	
	while( *pSrc)
	{
		*pDest = *pSrc;
		
		pDest++;
		pSrc++;
	}
	
	*pDest = 0;
}

// UCS2 -> UTF8 encode
//
// UTF-8로 변환된 내용을 파일로 저장할려면, 해더로 3바이트를 써 주어야 됨.
// 0xEF 0xBB 0xBF
// 
int NSMString::UCS2toUTF8( char *pDest, const WCHAR16 *pSrc, int nDestBufSize )
{
	if( pSrc == NULL ) return 0;

	int count = (int)( NSMString::LengthW( pSrc ) );
	WCHAR16 unicode;
	unsigned char bytes[4];
	int nbytes;
	int len = 0;

	for( int i = 0; i < count; i++ )
	{
		unicode = pSrc[i];
		if ( unicode < 0x80 )
		{
			nbytes = 1;
			bytes[0] = (unsigned char) unicode;							//  0xxxxxxx
		}
		else if ( unicode < 0x0800 )
		{
			nbytes = 2;
			bytes[1] = (unsigned char) ((unicode & 0x3f) | 0x80);		// 10xxxxxx
			bytes[0] = (unsigned char) ((unicode >> 6) & 0x1f | 0xc0) ;	// 110xxxxx
		}
		else
		{
			nbytes = 3;
			bytes[2] = (unsigned char) ((unicode & 0x3f) | 0x80);		// 10xxxxxx
			bytes[1] = (unsigned char) ((unicode >> 6) & 0x3f | 0x80);	// 10xxxxxx
			bytes[0] = (unsigned char) ((unicode >> 12) & 0x0f | 0xe0);	// 1110xxxx
		}

		if( (len + nbytes) >= nDestBufSize )
		{
			//I3WARN("Need more buffer size.\n");
			////I3TRACE( "[%s] : Need more buffer size.\n", __FUNCTION__ );
			//I3ASSERT_0;
			break;
		}

		for( int j = 0; j < nbytes; j++ )
		{
			pDest[len] = bytes[j];
			len++;
		}
	}

	pDest[len] = 0x00;

	return len;
}

// UTF-8포맷을 유니코드 스트링으로 디코딩
//
// UTF-8로 변환된 내용을 파일로 저장할려면, 해더에 다음의 2바이트를 써 주어야 됨.
// 0xFF 0xFE
//
int NSMString::UTF8toUCS2( WCHAR16 *pDest, const char *_pSrc, int nDestBufSize )
{
	unsigned char *pSrc = ( unsigned char * )_pSrc;
	unsigned char c;
	WCHAR16 unicode;
	int count = 0;
	int i = 0;
	//int len = Length( pSrc );
	int len = (int)strlen( _pSrc );

	for( i = 0; i < len; )
	{
		if( ( count + 1 ) >= nDestBufSize )
		{
			//I3WARN("Need more buffer size.\n");
			////I3TRACE( "[%s] : Need more buffer size.\n", __FUNCTION__ );
			//I3ASSERT_0;
			break;
		}

		c = pSrc[i] & 0xe0;

		if( c < 0x80 )
		{
			unicode = WCHAR16( pSrc[i] );
			i++;
		}
		else if ( c < 0xe0 )
		{
			unicode =	WCHAR16( pSrc[i] & 0x1f );
			i++;
			unicode =	unicode << 6;
			unicode |=	WCHAR16( pSrc[i] & 0x3f );
			i++;
		}
		else if ( c < 0xf0 )
		{
			unicode =	WCHAR16( pSrc[i] & 0x0f );
			i++;
			unicode =	unicode << 6;
			unicode |=	WCHAR16( pSrc[i] & 0x3f );
			i++;
			unicode =	unicode << 6;
			unicode |=	WCHAR16( pSrc[i] & 0x3f );
			i++;
		}
		else
		{
			//I3WARN("Invalid character.\n");
			////I3TRACE( "[%s] : Invalid character.\n", __FUNCTION__ );
			//I3ASSERT_0;
			break;
		}

		pDest[count] = unicode;
		count++;
	}

	pDest[count] = 0x0000;

	return count;
}

void NSMString::ftoa( REAL32 val, char * pszStr, INT32 fractLen)
{
	char * pszTemp;

	if( fractLen == -1)
	{
		sprintf( (PSTR)pszStr, (PSTR)"%f", val);
	}
	else
	{
		char format[64];

		sprintf( format, "%%.%df", fractLen);
		sprintf( pszStr, format, val);
	}

	pszTemp = pszStr;
	while( *pszTemp)
		pszTemp++;

	do
	{
		pszTemp--;

		if( *pszTemp == '0')
			*pszTemp = 0;

	} while( *pszTemp == 0);

	if( *pszTemp == '.')
		*pszTemp = 0;
}


/// szIn과 같은 내용의 문자열을 생성해서 리턴한다.
char* NSMString::NewString( const char* szIn )
{
	//I3ASSERT( szIn != NULL );
	UINT32 nLen = Length( szIn );
	//I3ASSERT( nLen >= 0 );
	
	char *szOut = (char *)NSM_nsMemory::Alloc( nLen + 1 );
	//I3ASSERT( szOut != NULL );

#if 0
	{
		// 메모리 누수 탐지
		void *pCheck = szOut;
		static int nCount = 0;
		static int nCheck = -1;
		//I3ASSERT( nCheck != nCount );
		//I3WARN("%d, %p\n", nCount, pCheck );
		////I3TRACE( __FUNCTION__ " : %d, %p\n", nCount, pCheck );
		nCount++;
	}
#endif

	Copy( szOut, szIn );

	return szOut;
}

void NSMString::MakeCStylePath( char * pszDest, char * pszSrc)
{
	//I3ASSERT( pszDest != NULL);
	//I3ASSERT( pszSrc != NULL);

	while( *pszSrc != 0)
	{
		*pszDest = *pszSrc;

		if( *pszSrc == '\\')
		{
			pszDest++;
			*pszDest = '\\';
		}

		pszDest++;
		pszSrc++;
	}

	*pszDest = 0;
}

enum ATOI_MODE
{
	ATOI_DECIMAL = 0,
	ATOI_HEXA,
};

static SYMBOLTABLE_INFO *	_FindSymbol( SYMBOLTABLE_INFO * pTable, INT32 symbolCount, char * pszStr)
{
	INT32 i;

	for( i = 0; i < symbolCount; i++)
	{
		if( NSMString::Compare( pTable[i].m_szSymbol, pszStr) == 0)
		{
			return & pTable[i];
		}
	}

	return NULL;
}

void	NSMString::PreprocessMacro( char * pszDest, INT32 Length, const char * pszSrc, SYMBOLTABLE_INFO * pTable, INT32 symbolCount)
{
	char * pszTemp = (char *) pszSrc;
	char szSym[65];
	INT32 symAccm = 0;
	bool bInSymbol = false;
	SYMBOLTABLE_INFO * pSymbol;

	while( *pszTemp != 0)
	{
		switch( *pszTemp)
		{
			case '%' :
				if( bInSymbol)
				{
					if( symAccm == 0)
					{
						// %% 의 경우
						*pszDest = '%';
						pszDest++;
					}
					else
					{
						szSym[ symAccm] = 0;
						symAccm = 0;

						pSymbol = _FindSymbol( pTable, symbolCount, szSym);

						if( pSymbol != NULL)
						{
							INT32 len = (INT32)strlen( pSymbol->m_szValue);

							memcpy( pszDest, pSymbol->m_szValue, len);
							pszDest += len;
						}
						else
						{
							//I3TRACE( "올바르지 않은 Symbol : %s\n", szSym);
						}
					}

					bInSymbol = false;
				}
				else
				{
					bInSymbol = true;
					symAccm = 0;
				}
				break;

			default :
				if( bInSymbol)
				{
					szSym[ symAccm ] = *pszTemp;
					symAccm ++;
				}
				else
				{
					*pszDest = *pszTemp;
					pszDest++;
				}
				break;
		}

		pszTemp++;
	}

	*pszDest = 0;
}

bool _scanWildcard( char * pszName, char * pszRull)
{
	while( (*pszRull != 0) && (*pszName != 0))
	{
		switch( *pszRull)
		{
			case '*' :			return true;

			case '?' :
				break;

			default :
				if( toupper( *pszName) != toupper( *pszRull))
					return false;
				break;
		}

		pszRull++;
		pszName++;
	}

	if( *pszRull != *pszName)
		return false;

	return true;
}

bool NSMString::IsMatchedWildcard( char * pszFileName, char * pszRull)
{
	char	szRullName[ MAX_PATH], szRullExt[ MAX_PATH];
	char	szName[ MAX_PATH], szExt[ MAX_PATH];

	NSMString::SplitFileName( pszRull, szRullName, FALSE);
	NSMString::SplitFileExt( pszRull, szRullExt, sizeof(szRullExt));

	NSMString::SplitFileName( pszFileName, szName, FALSE);
	NSMString::SplitFileExt(	 pszFileName, szExt, sizeof(szExt) - 1);

	if( szRullName[0] != 0)
	{
		if( _scanWildcard( szName, szRullName) == false)
			return false;
	}

	if( szRullExt[0] != 0)
	{
		if( _scanWildcard( szExt, szRullExt) == false)
			return false;
	}

	return true;
}

void NSMString::GetPrefixName( char * pszDest, const  char * pszStr)
{
	INT32 i;

	// 우선 문자열의 복사
	for( i = 0; pszStr[i] != 0; i++)
	{
		pszDest[i] = pszStr[i];
	}

	while( isdigit( pszDest[i - 1]))
	{
		// 이전 문자가 digit 문자라면 앞으로 이동한다.
		i--;
	}

	// 현재 i의 이전 문자는 digit 문자가 아니기때문에
	// 끝을 표시한다.
	pszDest[i] = 0;
}

BOOL NSMString::Replace(char * str, INT32 bufLen, const char * before, const char * affer)
{
	const cbufMax = 4096;
	char buf[cbufMax] = "", * pbuf = buf;
	unsigned char * p = (unsigned char*) str;
	unsigned char * bf = (unsigned char*) before;
	unsigned char * af = (unsigned char*) affer;
	INT32 len = 0;

	if( str == NULL || *str == 0) return FALSE;
	if( before == NULL || *before == 0) return FALSE;
	if( affer == NULL || *affer == 0) return FALSE;

	INT32 maxLen = MIN( bufLen, cbufMax);

	while( *p) 
	{
		//	같은 문자열을 찾는다
		if( *p == *bf)
		{
			INT32 count = 0;
			unsigned char * c = p;

			//	문자열을 비교해본다.
			while( *c != 0 && *c == *bf)
			{
				bf++;
				c++;
				count++;
			}		

			//	찾아야할 모든 문자열 검색이 끝났다.
			if( *bf == 0)
			{				
				af = (unsigned char*) affer;

				//	변경할 문자열로 교체한다.
				while( *af)
				{
					*(pbuf++) = *(af++);

					len++;

					//	버퍼가 가득찼으므로 진행 종료
					if( len >= maxLen-1)
					{
						*pbuf = 0;
						goto LABEL_EXIT;
					}
				}

				p = c;	//	교체후 포인터 점프.	
			}
			//	비교 실패. 원본 그대도 복사
			else
			{
				for(INT32 i=0; i < count; i++)
				{
					*(pbuf++) = *(p++);
					len++;

					//	버퍼가 가득찼으므로 진행 종료
					if( len >= maxLen-1)
					{
						*pbuf = 0;
						goto LABEL_EXIT;
					}
				}
			}

			//	다음으로 계속 진행되도록
			bf = (unsigned char*) before;
		}
		//	다른 문자다
		else
		{
			*(pbuf++) = *(p++);
			len++;

			//	버퍼가 가득찼으므로 진행 종료
			if( len >= maxLen-1)
			{
				*pbuf = 0;
				goto LABEL_EXIT;
			}		
		}
	}

LABEL_EXIT:

	//	최종 변경된것 복사
	if( buf[0] != 0)
	{
		len = MIN( len, maxLen-1);

        NSMString::NCopy( str, buf, len);

		str[len] = 0;

		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//	THAI language -- 소스는 gpgstudy에서 가져옴 (2008.7.31 komet)
/////////////////////////////////////////////////////////////////////////////////////////////

// Unicode of thai : 0x0E00~0x0E7F (Direct Mapping)
// Thai Character -> Unicode    (char+0x0E00)-0xA0 =    char+0x0D60
// Unicode -> Thai Character (wchar_t-0x0E00)+0xA0 = wchar_t-0x0D60

enum THAI_CLASS		//	Thai character classification
{	
	CTRL= 0,		//	Control characters
	NON= 1,			//	Non-composibles
	CONS= 2,		//	Consonants
	LV= 3,			//	Leading Vowels
	FV1= 4,			//	Following Vowels
	FV2= 5,
	FV3= 6,
	BV1= 7,			//	Below Vowels
	BV2= 8,
	BD= 9,			//	Below Diacritic
	TONE=10,		//	Tonemarks
	AD1=11,			//	Above Diacritics
	AD2=12,
	AD3=13,
	AV1=14,			//	Above Vowels
	AV2=15,
	AV3=16,
};

THAI_CLASS 
THAI_CLASS_TABLE[256] = 
{
	CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, 
	CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, 
	NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
	NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
	NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
	NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
	NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
	NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, CTRL, 
	CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, 
	CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, 
	NON, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, 
	CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, 
	CONS, CONS, CONS, CONS,  FV3, CONS,  FV3, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS,  NON, 
	FV1,  AV2,  FV1,  FV1,  AV1,  AV3,  AV2,  AV3,  BV1,  BV2,   BD,  NON,  NON,  NON,  NON,  NON, 
	LV,   LV,   LV,   LV,   LV,  FV2,  NON,  AD2, TONE, TONE, TONE, TONE,  AD1,  AD1,  AD3,  NON, 
	NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, CTRL, 
};

enum THAI_INPUT_TYPE { IA, IC, IS, IR, IX };

BOOL THAI_INPUT_COMPOSIBLE[3][5] = {
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ TRUE, TRUE, TRUE,FALSE, TRUE },
	{ TRUE, TRUE,FALSE,FALSE, TRUE },
};

THAI_INPUT_TYPE 
THAI_INPUT_STATE_CHECK[17][17] = {
	{ IX,IA,IA,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IA,IS,IA,IC,IC,IC,IC,IC,IC,IC,IC,IC,IC },
	{ IX,IS,IA,IS,IS,IS,IS,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IS,IA,IS,IA,IS,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IA,IS,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IA,IS,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IA,IS,IA,IR,IR,IR,IC,IC,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IS,IA,IR,IR,IR,IC,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IS,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IA,IA,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IS,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IS,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IS,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IS,IA,IR,IR,IR,IC,IC,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IS,IA,IR,IR,IR,IC,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IS,IA,IR,IR,IR,IC,IR,IC,IR,IR,IR,IR },
};

enum THAI_OUTPUT_TYPE { ON, OC, OX };

THAI_OUTPUT_TYPE 
THAI_OUTPUT_STATE_CHECK[17][17] = {
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,OC,OC,OC,OC,OC,OC,OC,OC,OC,OC,OC,OC,OC },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,OC,OC,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,OC,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,OC,OC,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,OC,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,OC,ON,OC,ON,ON,ON,ON },
};

BOOL NSMString::IsComposite(INT32 nCodePage, const char prev, const char curr, INT32 mode)
{
	switch( nCodePage)
	{
	case 874:	//	THAI
		{
			if( mode > 2)	
				return FALSE;
			else 
			{
				BYTE prev_stream = (BYTE)prev;
				//I3ASSERT( prev_stream >= 0 && prev_stream < 256);

				BYTE curr_stream = (BYTE)curr;
				//I3ASSERT( curr_stream >= 0 && curr_stream < 256);

				THAI_CLASS prev_class = THAI_CLASS_TABLE[prev_stream];
				//I3ASSERT( prev_class >= 0 && prev_class < 17);

				THAI_CLASS curr_class = THAI_CLASS_TABLE[curr_stream];
				//I3ASSERT( curr_class >= 0 && curr_class < 17);

				THAI_INPUT_TYPE state_type = THAI_INPUT_STATE_CHECK[prev_class][curr_class];

				return THAI_INPUT_COMPOSIBLE[mode][state_type];
			}
		}
		break;
	default:
		//I3ASSERT_0;
		break;
	}

	return FALSE;
}

INT32 NSMString::LengthChar(INT32 nCodePage, const char * pszStr)
{
	//I3ASSERT( pszStr != NULL && nCodePage != 0 && nCodePage != 1);

	INT32 nCount = 0;
	const char * pszCurr = pszStr;
	while( *pszCurr)
	{
		pszCurr = CharNext(nCodePage, pszCurr);
		nCount++;
	}	

	return nCount;
}

const char * NSMString::CharNext(INT32 nCodePage, const char * pszStr)
{
	//I3ASSERT( pszStr != NULL && nCodePage != 0 && nCodePage != 1);

	switch( nCodePage)
	{
	case 874:		//	THAI			태국은 조합형 MBCS 체계 (1~4바이트 가변 바이트 ) 주의! 태국어는 2바이트가 아니다!
		{
			const BYTE * stream = (const BYTE*) pszStr;
			
			for(;;)
			{
				//I3ASSERT( stream[0] >= 0 && stream[0] < 256);
				//I3ASSERT( stream[1] >= 0 && stream[1] < 256);

				THAI_CLASS c1 = THAI_CLASS_TABLE[ stream[0]];
				//I3ASSERT( c1 >= 0 && c1 < 17);

				THAI_CLASS c2 = THAI_CLASS_TABLE[ stream[1]];			
				//I3ASSERT( c2 >= 0 && c2 < 17);

				if( THAI_OUTPUT_STATE_CHECK[c1][c2] == OC)
					++stream;
				else
					break;
			}

			return (const char*)(stream+1);
		}
		break;
	case 949:		//	HANGUL			한글 
	case 932:		//	SHIFTJIS		일본어
	case 936:		//	GB2312			중국어 간체
	case 950:		//	CHINESEBIG5		중국어 번체
		{
			return ::CharNextExA((WORD) nCodePage, pszStr, 0);
		}
		break;
	default:
		{
			return ::CharNextExA((WORD) nCodePage, pszStr, 0);
		}
		break;
	}
}

const char * NSMString::CharPrev(INT32 nCodePage, const char * pszStart, const char * pszCurrent)
{
	//I3ASSERT( pszStart != NULL && pszCurrent != NULL && nCodePage != 0 && nCodePage != 1);

	switch( nCodePage)
	{
	case 874:		//	THAI			태국은 조합형 MBCS 체계 (1~4바이트 가변 바이트 )
		{
			while( pszStart < pszCurrent)
			{
				const char * pszNext = CharNext(nCodePage, pszStart);

				if( pszNext == pszCurrent)
					return pszStart;
				else if( pszNext < pszCurrent)
					pszStart = pszNext;
				else
					break;
			}

			return pszStart;
		}
		break;
	case 949:		//	HANGUL			한글 
	case 932:		//	SHIFTJIS		일본어
	case 936:		//	GB2312			중국어 간체
	case 950:		//	CHINESEBIG5		중국어 번체
		{
			return ::CharPrevExA((WORD) nCodePage, pszStart, pszCurrent, 0);
		}
		break;
	default:
		{
			return ::CharPrevExA((WORD) nCodePage, pszStart, pszCurrent, 0);
		}
		break;
	}
}

void NSMString::Format( char * pszDest, INT32 len, const char * pszFormat, ...)
{
	va_list marker;

	va_start( marker, pszFormat);

	int Max = len - 1;
	int Rv = _vsnprintf( pszDest, len, pszFormat, marker);

	if ((Rv < 0) || (Rv >= Max))
	{
		// need to null terminate the string
		pszDest[Max] = '\0';
	}

	va_end( marker);
}

INT32 BF_search(char *dest, char *src)
{
	INT32 i = 0,j = 0;
	INT32 srcLen=NSMString::Length(src), destLen=NSMString::Length(dest);

	if( dest == NULL || src == NULL || srcLen == 0 || destLen == 0)
		return -1;

	for(j=0; j<=destLen-srcLen; j++)
	{
		for(i=0; i<srcLen && src[i] == dest[i+j]; i++);

		if( i >= srcLen)
			return j;
	}

	return -1;
}

INT32 * KMP_init(char *src)
{
	INT32 i=0, j=0;
	INT32 srcLen=NSMString::Length(src);

	INT32 * next = (INT32*) NSM_nsMemory::Alloc( sizeof(INT32) * (srcLen+1));

	next[0] = -1;
	
	for(i=0, j=-1; i<srcLen; i++, j++, next[i] = j)
	{
		while((j>=0) && (src[i] != src[j]))
			j= next[j];
	}	
	
	return next;
}

INT32 KMP_search(char *dest, char *src)
{
	INT32 i=0, j=0;
	INT32 destLen = NSMString::Length(dest), srcLen=NSMString::Length(src);

	if( dest == NULL || src == NULL || srcLen == 0 || destLen == 0)
		return -1;

	//	Preprocessing
	INT32 * next = KMP_init( src);
	//I3ASSERT( next != NULL);

	//	searching
	for( i=0, j=0; j<srcLen && i<destLen; i++, j++)
	{
		while((j >= 0) && (dest[i] != src[j])) 
			j = next[j];
	}

	if( next )
	{
		MEM_SAFE_FREE( next);
	}

	if( j == srcLen)	return i-srcLen;
	else				return -1;
}

#define IS_ASCII_CHAR(ch)		(ch>=0 && ch<256)

// bad-character shift array
void preBmBc( char *src, INT32 srcLen, INT32 * bmBc, INT32 bmBcLen)
{
	INT32 i=0;

	for( i=0; i<bmBcLen; i++)
	{
		bmBc[i] = srcLen;
	}

	for( i=0; i<srcLen-1; i++)
	{
		// ascii code 내에 있는 것만 처리
        // multi byte 등은 그대로 둠
		if( IS_ASCII_CHAR( src[i]))
			bmBc[ src[i]] = srcLen-1-i;
	}
}

//	brief make suffix for good-suffix shift array
void suffixes( char * src, INT32 srcLen, INT32 *suff)
{
	INT32 f=0,g=0,i=0;

	suff[ srcLen-1] = srcLen;

	g = srcLen-1;

	for(i=srcLen-2; i>=0; i--)
	{
		if( i>g && suff[ i+srcLen-1-f] < i-g)
		{
			suff[i] = suff[ i+srcLen-1-f];
		}
		else
		{
			if( i<g)	g = i;

			f=i;
		
			while( g>=0 && src[g] == src[ g+srcLen-1-f])
				--g;

			suff[i] = f-g;
		}
	}
}

// make good-suffix shift array
void preBmGs( char* src, INT32 srcLen, INT32 * bmGs, INT32 bmGsLen)
{
	INT32 i=0,j=0;
	INT32 * suff = (INT32*) NSM_nsMemory::Alloc( sizeof(INT32) * (srcLen+1));
	
	// make suffix
	suffixes( src, srcLen, suff);

	for( i=0; i<srcLen; i++)
	{
		bmGs[ i] = srcLen;
	}

	for( i=srcLen-1; i >= 0; i--)
	{
		if( suff[ i] == i+1)
		{
			for( ; j<srcLen-1-i; j++)
			{
				if( bmGs[ j] == srcLen)
				{
					bmGs[ j] = srcLen-1-i;
				}
			}
		}
	}

	for( i=0; i<=srcLen-2; i++)
	{
		bmGs[ srcLen-1-suff[i]] = srcLen-1-i;
	}

	if( suff)
	{
		MEM_SAFE_FREE( suff);
	}
}

INT32 BM_search( char * dest, char* src)
{
	INT32 i=0,j=0;
	BOOL find = FALSE;
	INT32 srcLen = NSMString::Length( src);
	INT32 destLen = NSMString::Length( dest);

	if( dest == NULL || src == NULL || srcLen == 0 || destLen == 0)
		return -1;

	if( src == NULL || dest == NULL)		return -1;

	INT32 * bmGs = (INT32*) NSM_nsMemory::Alloc( sizeof( INT32) * (srcLen+1));
	INT32 bmBc[256];	//	 ascii code 크기인 256이다. 

	//	Preprocessing	
	preBmGs( src, srcLen, bmGs, (srcLen+1));	// make good-suffix shift array	
	preBmBc( src, srcLen, bmBc, 256);			// make bad-character shift

	//	Searching
	while( j<= destLen - srcLen)
	{
		for( i=srcLen-1; i>=0 && src[i] == dest[i+j]; i--);

		if(i<0)
		{
			find = TRUE;

			goto _EXIT;
			//j+=bmGs[0];
		}
		else
		{
			INT32 skip = 1;

			// ascii code 내에 있는 것만 처리
			if( IS_ASCII_CHAR( dest[i+j]))
			{
				skip = bmBc[ dest[i+j]];
			}

			j+=MAX( bmGs[i], skip - srcLen + 1 + i);
		}
	}


_EXIT:

	if( bmGs)
	{
		MEM_SAFE_FREE( bmGs);
	}

	if( find)
		return j;
	else
		return -1;
}

void preQsBc( char* src, INT32 srcLen, INT32 * qsBc, INT32 qsBcLen)
{
	INT32 i=0;

	for( i=0; i<qsBcLen; i++)
	{
		qsBc[i] = srcLen + 1;
	}

	for( i=0; i<srcLen; i++)
	{
		// ascii code 내에 있는 것만 처리
        // multi byte 등은 그대로 둠
		if( IS_ASCII_CHAR( src[i]))
			qsBc[ src[i]] = srcLen - i;
	}
}

INT32 Q_search( char* dest, char* src)
{
	INT32 j=0;
	INT32 qsBc[256];	//	 ascii code 크기인 256이다.
	INT32 srcLen=NSMString::Length(src), destLen=NSMString::Length(dest);

	if( dest == NULL || src == NULL || srcLen == 0 || destLen == 0)
		return -1;

	//	Preprocessing
	preQsBc( src, srcLen, qsBc, 256);

	//	Searching
	while( j <= destLen-srcLen)
	{
		//	find
		if( memcmp( src, dest+j, srcLen) == 0)return j;			
		
		INT32 skip = 1;

		// ascii code 내에 있는 것만 처리
		if( IS_ASCII_CHAR( dest[j+srcLen]))
		{
			skip = qsBc[ dest[j+srcLen]];
		}

		//	skip string
		j+=skip;
	}

	return -1;
}

#define REHASH(a, b, h)		((((h)-(a)*d)<<1)+(b))

INT32 RK_search( char* dest, char* src)
{
	INT32 d=0, hx=0, hy=0, i=0, j=0;
	INT32 srcLen=NSMString::Length(src), destLen=NSMString::Length(dest);

	if( dest == NULL || src == NULL || srcLen == 0 || destLen == 0)
		return -1;

	//	Preprocessing
	for(d=i=1; i<srcLen; i++)
		d = (d<<1);

	for( hy=hx=i=0; i<srcLen; i++)
	{
		hx = ((hx<<1) + src[i]);
		hy = ((hy<<1) + dest[i]);
	}

	//	Searching
	while( j <= destLen-srcLen)
	{
		if( hx==hy && memcmp(src, dest+j, srcLen) == 0)return j;
		hy = REHASH( dest[j], dest[j+srcLen], hy);
		j++;
	}

	return -1;
}

INT32 NSMString::Search(char * pszDest, char * pszSrc, ALGORITHM type)
{
	switch( type)
	{
	case BF:	return BF_search( pszDest, pszSrc);	
	case KMP:	return KMP_search( pszDest, pszSrc);
	case BM:	return BM_search( pszDest, pszSrc);
	case QUICK:	return Q_search( pszDest, pszSrc);
	case RK:	return RK_search( pszDest, pszSrc);
	}

	return -1;
}
*/