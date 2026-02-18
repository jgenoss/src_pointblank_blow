#include "stdafx.h"
#include "i3TDKUtilFile.h"

////////////////////////////////////////////////////////////////////////////////////////
// 주어진 경로의 끝에 역슬레쉬(\)가 존재하는 경우, 역슬레쉬를 없앤다.
// 
// char * Path	: 경로명을 저장한 문자열
//
// Return		: None
//
// 작성자		: 이진균
// 작성일자		: 1999.6.1
/*
I3_EXPORT_TDK
void i3TDK::NormalizePath( char * Path)
{
	char *p;

    p = Path;
    while( *p) p++;

    do {
    	*p = 0;
        p--;
    } while( *p == '\\');
}
*/
