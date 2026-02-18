///////////////////////////////////////////////////////////////////////////////
//
// i3MapFileParser.cpp
//
// Created by KimHyunwoo, 20070411
//
///////////////////////////////////////////////////////////////////////////////
#include "i3CommonType.h"

#if defined (I3_WINDOWS)
#include "i3String.h"
#include "i3MapFileParser.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/ext/generic_string_size.h"
#include "i3Base/string/ext/contain_string.h"

// ------------------------------------------------------------------------- //
// NAME: CheckExpection()
// DESC: 예외처리 시작
// ------------------------------------------------------------------------- //
bool	i3MapFileParser::CheckExpection( const char *pszMapFile, const char* pszExceptionAddress)
{
	DWORD dwAddress = GetHexValue( pszExceptionAddress );

	// 오류를 뽑아낸다.
	if(SetCheckExpection( pszMapFile, dwAddress )) return true;

	return false;
}

bool	i3MapFileParser::CheckExpection( const char *pszMapFile, DWORD dwAddress)
{
	// 오류를 뽑아낸다.
	if(SetCheckExpection( pszMapFile, dwAddress )) return true;

	return false;
}

bool	i3MapFileParser::CheckExpection( const char *pszMapFile, PVOID pAddress)
{
	char szAddress[260];
	sprintf(szAddress, "%X", pAddress);

	DWORD dwAddress = GetHexValue( szAddress );

	if(SetCheckExpection( pszMapFile, dwAddress )) return true;

	return false;
}
// ------------------------------------------------------------------------- //
// NAME: SetCheckExpection()
// DESC: 예외처리 시작
// ------------------------------------------------------------------------- //
bool	i3MapFileParser::SetCheckExpection( const char *pszMapFile, DWORD dwExceptionAddress )
{
	bool	bError = false;
	char	szAddress[128] = {0, };
	char	szObjName[128] = {0, };

	m_nSourceLine = 0;
	memset( m_szClassName, 0, 128 );
	memset( m_szFunctionName, 0, 128);	

	FILE *fp = nullptr;
	fp = fopen( pszMapFile , "rt" );
	if(nullptr == fp )		return false;

	// Preferred load address is
	DWORD dwStartAddress = 0;

	// 시작주소 얻어오기.
	if( false == GetStartAddress( fp, &dwStartAddress ) )	{
		goto go_Exit;
	}

	// 체크하기	
	if( false == GetAddressInfo( fp, dwExceptionAddress, m_szFunctionName, szAddress, szObjName ))	{
		goto go_Exit;
	}
			
	// 충돌 라인은 충돌 주소 - 시작주소 - 0x00001000
 	DWORD dwLine = dwExceptionAddress - dwStartAddress - 4096;

	if( false == GetLineInfo( fp, dwLine, szObjName, szAddress, &m_nSourceLine, m_szClassName ) )  {
		goto go_Exit;
	}	

	bError = true;


go_Exit:
	
	fclose(fp);

	return bError;
}

// ------------------------------------------------------------------------- //
// NAME: GetStartAddress()
// DESC: 시작주소 구하기
// ------------------------------------------------------------------------- //
bool	i3MapFileParser::GetStartAddress( FILE *fp, DWORD* pdwStartAddress )
{
	char szLine[1024];
	while ( !feof(fp) )
	{
		fgets(szLine, 1024, fp);
		if( -1 < i3::contain_string( szLine, "Preferred load address is ") )
		{
			char szValue[128] = {0, };
			INT32 nStringLen = i3::generic_string_size("Preferred load address is  ");
			i3::string_ncopy_nullpad( szValue, szLine + nStringLen, 8 );
			szValue[8] = '\0';

			*pdwStartAddress = GetHexValue(szValue);
			return true;
		}
	}
	return false;
}

// ------------------------------------------------------------------------- //
// NAME: GetAddressInfo()
// DESC: 함수와 클래스 이름 구하기
// ------------------------------------------------------------------------- //
bool	i3MapFileParser::GetAddressInfo(FILE *fp, DWORD dwExceptionAddress, char *pszFunctionName, char* pszAddress, char *pszClassName )
{
	char szLine[1024] = { 0 };

	while ( !feof(fp) )	{
		fgets(szLine, 1024, fp);

		if( -1 < i3::contain_string( szLine, "Rva+Base" ) ) {		// "Rva+Base" 있는곳 까지 우선 찾는다.

			if(GetAddressInfoReal( fp, dwExceptionAddress, pszFunctionName, pszAddress, pszClassName )) return true;

			return false;
		}
	}
	return false;
}

// ------------------------------------------------------------------------- //
// NAME: GetAddressInfoReal()
// DESC: 함수와 클래스 이름 구하기
// ------------------------------------------------------------------------- //
bool	i3MapFileParser::GetAddressInfoReal(FILE *fp, DWORD dwExceptionAddress, char *pszFunctionName, char* pszAddress, char *pszClassName )
{	
	char szLine[1024] = { 0 };
	char szPreLineSave[1024] = { 0, };
	char szAddress[1024 ];
	char szValue[1024];
	char szRva[1024];

	while ( !feof(fp) )	{

		fgets(szLine, 1024, fp);

		if( -1 < i3::contain_string( szLine, "entry point at" ) ) 
			return false;		// entry point at 부분에서 자동 리턴 

		if( i3::generic_string_size(szLine) > 1 ) {

			// 라인의 정보를 뽑아낸다.
			sscanf(szLine, "%s %s %s", szAddress, szValue, szRva );

			DWORD dwAddress = GetHexValue( szRva );

			if( dwAddress > dwExceptionAddress ) {

				sscanf( szPreLineSave, "%s %s", szAddress, szValue );

				GetFunctionName( szValue, pszFunctionName );

				GetObjectName( szPreLineSave, pszClassName );

				sscanf(szPreLineSave, "%s", pszAddress );

				return true;					
			}
		}
		// 주소를 넘어가면 지난줄의 주소값이 해당주소가 되기때문에 지난번 값을 저장
		strncpy( szPreLineSave, szLine, sizeof(szPreLineSave) -1 );
	}

	return false;
}

// ------------------------------------------------------------------------- //
// NAME: GetLineInfo()
// DESC: 함수와 클래스 이름 구하기
// ------------------------------------------------------------------------- //
bool	i3MapFileParser::GetLineInfo(FILE *fp, DWORD dwAddress, const char* pObeName, const char* pszAddress, INT32* pnLine, char* pszClassName )
{
	char szLine[1024] = { 0 };
	// Line numbers for까지 포인터 이동
	while ( !feof(fp) )
	{
		fgets(szLine, 1024, fp);

		if( -1 < i3::contain_string( szLine, "Line numbers for" ) ) {								// "Line numbers for"  단까지 뽑아낸다.

			if( -1 < i3::contain_string( szLine, pObeName ) ) {									// 클래스 명이 같은것이 있는지를 체크

				GetLineClassName( szLine, pszClassName );								// 클래스 명을 얻어온다.

				if( GetLineReal( fp, dwAddress, pszAddress, pnLine ) ) {	
					return true;														// 라인을 얻어온다.
				}				
				
			}
		}
	}	
	return false;
}

// ------------------------------------------------------------------------- //
// NAME: GetLineClassName()
// DESC: 클래스 이름 구하기
// ------------------------------------------------------------------------- //
void	i3MapFileParser::GetLineClassName( const char* szLine, char* pszClassName )
{
	INT32 i = 0, nStart = 0, nEnd = 0;

	while( szLine[i] != '\0') {		

		if( szLine[i] == '\\' ) {
			nStart = i + 1;
		}
		else if( szLine[i] == ')' ) {
			nEnd = i;
			break;
		}
		i++;
	}

	i3::string_ncopy_nullpad( pszClassName, szLine + nStart, nEnd - nStart);
	while( *pszClassName ) pszClassName++;
	*pszClassName = '\0';
}

// ------------------------------------------------------------------------- //
// NAME: GetLineReal()
// DESC: 함수와 클래스 이름 구하기
// ------------------------------------------------------------------------- //

// 이부분 정말 수정해야함.. ㅜㅜ 코드가 너저분..
bool	i3MapFileParser::GetLineReal(FILE *fp, DWORD dwAddress, const char* pszAddress, INT32* pnLine )
{
	char	szLine[1024] = { 0 };
	INT32	nLine[4] = {-1, };
	char	szAddress[4][64] = {0, };
	bool	bLink = false;
	INT32	nTemp = 0;
	
	while ( !feof(fp) )
	{
		fgets(szLine, 1024, fp);

		if( i3::generic_string_size(szLine) > 1) {
			// 라인씩 검사하는 방법으로..
			if( -1 < i3::contain_string( szLine, "Line numbers for" ) )	return false;

			if( -1 < i3::contain_string( szLine, pszAddress ))			bLink = true;

			sscanf( szLine, "%d %s %d %s %d %s %d %s", 
				&nLine[0], szAddress[0], &nLine[1], szAddress[1], &nLine[2], szAddress[2], &nLine[3], szAddress[3] );

			for( INT32 i = 0 ; i < 4 ; i++ ) {
				if(nLine[i] > -1 ) {
					if( bLink) {
						char	szAddressTemp[4][64] = {0, };
						i3::string_ncopy_nullpad( szAddressTemp[i], szAddress[i] + 5, 8 );
						szAddressTemp[i][8] = '\0';

						DWORD dwHex = GetHexValue( szAddressTemp[i] );

						if( dwAddress <= dwHex ) {	// 이부분이 라인부분				
							return true;
						}
					}
					*pnLine = nLine[i];
				}
			}
		}
		else nTemp++;

		if(nTemp >= 2) return false;
	}
	return false;
}
// ------------------------------------------------------------------------- //
// NAME: GetFunctionName()
// DESC: 함수와 클래스 이름 구하기
// ------------------------------------------------------------------------- //
void	i3MapFileParser::GetFunctionName( char *pszLine, char* pszFunctionName )
{
	INT32 i = 0;
	while( *pszLine != '\0' ) {
		if( *pszLine == '@' || *pszLine == ' ' ) {
			i3::string_ncopy_nullpad( pszFunctionName, pszLine, i );
			while( *pszFunctionName ) pszFunctionName++;
			*pszFunctionName = '\0';
			return;
		}
		pszLine++;
	}
	strncpy( m_szFunctionName, pszLine, sizeof(m_szFunctionName) -1 );
}

// ------------------------------------------------------------------------- //
// NAME: GetFunctionName()
// DESC: 함수와 클래스 이름 구하기
// ------------------------------------------------------------------------- //
void	i3MapFileParser::GetObjectName( char* pszPreLineSave, char* pszClassName )
{
	INT32 nLen = i3::generic_string_size( pszPreLineSave );
	INT32 nStart;

	nStart = nLen - 1;
	while(pszPreLineSave[ nStart ] != ' ') {
		nStart--;
	}

	nStart++;

	i3::string_ncopy_nullpad( pszClassName, pszPreLineSave + nStart, nLen - 1 - nStart );

	while( *pszClassName ) pszClassName++;
	*pszClassName = '\0';
}

// ------------------------------------------------------------------------- //
// NAME: GetHexValue()
// DESC: 헥사값을 10진수로 바꾸기
// ------------------------------------------------------------------------- //
DWORD	i3MapFileParser::GetHexValue( const char *pszHex )
{
	INT32	i = 0, nHexSum = 0;
 
	while( *(pszHex+1) != '\0' && *(pszHex+1) ) pszHex++;
	
	do {
		nHexSum += GetHexConvert( *pszHex ) * GetHexMult( i );		
		i++;
	} while( *(--pszHex) );

	return nHexSum;
}

// ------------------------------------------------------------------------- //
// NAME: GetHexConvert()
// DESC: 헥사값을 10진수로
// ------------------------------------------------------------------------- //
INT32	i3MapFileParser::GetHexConvert( const char cHex )
{
	if('a' <= cHex && cHex <= 'f') 
		return (cHex - 'a') + 10;

	else if('A' <= cHex && cHex <= 'F') 
		return (cHex - 'A') + 10;

	else if('0' <= cHex && cHex <= '9')
		return cHex - '0';

	return 0;
}

// ------------------------------------------------------------------------- //
// NAME: GetHexMult()
// DESC: 10진수로 바꾸기 위해 16씩 곱하기
// ------------------------------------------------------------------------- //
INT32	i3MapFileParser::GetHexMult( INT32 nMult )
{
	if(nMult == 0)		return 1;

	return 16 * GetHexMult( nMult - 1 );
}
#endif