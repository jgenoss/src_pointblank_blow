#include "LuacOption.h"
#include "i3Base.h"
#include "i3Base/string/algorithm/to_lower.h"
#include "i3Base/string/ext/generic_string_size.h"

CLuacOption::CLuacOption(void)
{
	m_LogType			= LT_NEW;
	m_bDump				= true;
	m_DebugInfoType		= DT_EXCLUDE;
	m_bPrintLoadError	= true;
	m_bPrintSrcError	= false;
	m_bExcludeFile		= false;

	m_vecstrExFileList.clear();
}

CLuacOption::~CLuacOption(void)
{
}



void CLuacOption::_setData( LOG_TYPE logtype, bool bDump, DEBUGINFO_TYPE debuginfotype, 
					bool bPrintLoadError, bool bPrintSrcError, bool bExcludeFile )
{
	m_LogType			= logtype;
	m_bDump				= bDump;
	m_DebugInfoType		= debuginfotype;
	m_bPrintLoadError	= bPrintLoadError;
	m_bPrintSrcError	= bPrintSrcError;
	m_bExcludeFile		= bExcludeFile;
}


// arg에서 옵션을 설정한다
bool CLuacOption::parseOption( char* pszOption )
{
	int		nLen = i3::generic_string_size( pszOption );

	if( nLen == 1 )
	{
		switch( pszOption[ 0 ] )
		{
		case	'1' :
			_setData( LT_NEW, true, DT_EXCLUDE, true, false, false );
			break;

		case	'2' :
			_setData( LT_ATTACH, true, DT_EXCLUDE, true, false, false );
			break;

		case	'3' :
			_setData( LT_NEW, true, DT_EXCLUDE, true, false, true );
			break;

		case	'4' :
			_setData( LT_ATTACH, true, DT_EXCLUDE, true, false, true );
			break;

		case	'5' :
			_setData( LT_NEW, false, DT_EXCLUDE, true, false, false );
			break;

		case	'6' :
			_setData( LT_ATTACH, false, DT_EXCLUDE, true, false, false );
			break;

		default :
			return false;
		}

		return true;
	}

	if( nLen != OT_COUNT ) return false;

	i3::to_lower( pszOption );

	// 로그 타입
	switch( pszOption[ OT_LOG ] )
	{
	case	'n' :	m_LogType	= LT_NEW;		break;
	case	'a' :	m_LogType	= LT_ATTACH;	break;
	case	'x' :	m_LogType	= LT_NONE;		break;
	default :		m_LogType	= LT_ATTACH;	break;
	}

	// 덤프
	if( pszOption[ OT_DUMP ] == 'o' )	m_bDump	= true;
	else								m_bDump	= false;

	// 디버그 정보
	if( pszOption[ OT_DEBUGINFO ] == 'o')	m_DebugInfoType	= DT_INCLUDE;
	else									m_DebugInfoType	= DT_EXCLUDE;

	// 루아 로드에러 출력
	if( pszOption[ OT_PRINTLOADERROR ] == 'o' )	m_bPrintLoadError	= true;
	else										m_bPrintLoadError	= false;

	// 루아 문법에러 출력
	if( pszOption[ OT_PRINTSRCERROR ] == 'o' )	m_bPrintSrcError	= true;
	else										m_bPrintSrcError	= false;

	// 제외 파일 있음
	if( pszOption[ OT_EXCLUDEFILE ] == 'o' )	m_bExcludeFile	= true;
	else										m_bExcludeFile	= false;


	return true;
}


// 제외 파일 목록을 생성한다
bool CLuacOption::setExcludeFileList( char* pszFileList )
{
	char	szTok[]		= ";";
	char*	pszFile		= NULL;
	char*	pszFileNext	= NULL;
	
	
	pszFile	= strtok_s( pszFileList, szTok, &pszFileNext );

	while( pszFile != NULL )
	{
		m_vecstrExFileList.push_back( pszFile );

		pszFile	= strtok_s( NULL, szTok, &pszFileNext );
	}

	return true;
}


// 제외 파일 목록과 비교한다
// return - true : 있음, false : 없음
bool CLuacOption::compareExcludeFile( const char* pszFileName )
{
	if( m_vecstrExFileList.size() <= 0 ) return false;

	vector<string>::iterator	iterExFile;

	for( iterExFile = m_vecstrExFileList.begin(); iterExFile != m_vecstrExFileList.end(); iterExFile++ )
	{
		if( strcmp( iterExFile->data(), pszFileName ) == 0 ) return true;
	}

	return false;
}