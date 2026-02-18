#include "stdafx.h"
/*
#include "GameStringTable.h"

#include "i3UIEditor.h"
#include "MainFrm.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//CGameStringTable theGameString;

CGameStringTable::CGameStringTable()
{

}

CGameStringTable::~CGameStringTable()
{
}

bool	CGameStringTable::Init( const char* szWorkDir )
{
	if( NULL == szWorkDir )
	{
		return false;
	}

	m_szWorkDir = szWorkDir;

	for( int i = 0; i < MAX_NATION_CNT; ++i )
	{
		m_NationStrings[i].clear();
	}

	//AddNation( ID_LANGUAGE_DEV );
//	AddNation( ID_BRAZIL );
//	AddNation( ID_CHINA );
//	AddNation( ID_INDONESIA );
//	AddNation( ID_DEBUG_EXPORTALL );
//	AddNation( ID_ITALY );
//	AddNation( ID_KOREA );
//	AddNation( ID_LATIN );
//	AddNation( ID_MIDDLEEAST );
//	AddNation( ID_PHILIPPINES );
//	AddNation( ID_RUSSIA );
//	AddNation( ID_TAILAND );
//	AddNation( ID_TURKEY );
	
	AddNation( ID_LANGUAGE_DEV );
	
	return true;
}

const char*		CGameStringTable::GetNationString( int id ) const
{
	switch( id )
	{
		case ID_LANGUAGE_MIDDLEEAST:		return "MiddleEast";	break;
		case ID_LANGUAGE_THAILAND:			return "Thailand";		break;
		case ID_LANGUAGE_ENGLISH:			return "English";		break;
		case ID_LANGUAGE_PHILIPPINES:		return "English/Philippines";	break;
		case ID_LANGUAGE_SINGMAL:			return "English/Singmal";		break;
		case ID_LANGUAGE_NORTHAMERICA:		return "English/USA";	break;
		case ID_LANGUAGE_INDONESIA:			return "Indonesia";		break;
		case ID_LANGUAGE_ITALY:				return "Italian";			break;
		case ID_LANGUAGE_KOREAN:			return "Korean";			break;
		case ID_LANGUAGE_BRAZIL:			return "Brazil";		break;
		case ID_LANGUAGE_RUSSIA:			return "Russia";		break;
		case ID_LANGUAGE_LATINAMERICA:		return "LatinAmerica";	break;
		case ID_LANGUAGE_THAIWAN:			return "Thaiwan";		break;
		case ID_LANGUAGE_TURKY:				return "Turky";			break;
		case ID_LANGUAGE_VIETNAM:			return "Vietnam";		break;
		case ID_LANGUAGE_JAPAN:				return "Japan";			break;
	}

	return "Dev";
}


void	CGameStringTable::AddNation( int nNationID )
{
	const char* szNation = GetNationString( nNationID );

	s_nation na;

	na.nID			= (int)m_Nations.size();
	na.nNationID	= nNationID;
	na.szNation		= szNation;

	m_Nations.insert( std::make_pair( nNationID, na ) );

	ResetNationString( nNationID );
}

void	CGameStringTable::ResetNationString( int nNationID )
{
	map_nation::iterator itr = m_Nations.find( nNationID );

	if( itr == m_Nations.end() )
	{
		assert( !"nation id error...." );
		return;
	}

	const s_nation& nation = itr->second;
	map_string& mapStr = m_NationStrings[ nation.nID ];

	if( false == mapStr.empty() )
	{
		return;
	}

//	const char* szWorkDir	= m_szWorkDir.c_str();		
	const char* szNation	= nation.szNation;


	std::string path = m_szWorkDir;
	size_t _npos1 = path.find("\\ui");
	path.assign(path, 0, _npos1);


	char szPath[ MAX_PATH ];
	i3::snprintf( szPath, MAX_PATH, "%s\\Script\\String\\%s\\String_%s.txt", path.c_str(), szNation, szNation );

	i3FileStream fs;

	//if ( i3::generic_string_size(szNation) > 0 )		// szNation 설정 사용되지 않고 있어 해당 필드값 존재하지 않으면 파일 로딩 안합니다.
		//fs.Open( szPath, STREAM_READ );

	wchar_t wch;

	std::wstring szKey, szText;

	bool bSetKey = true;

	fs.Read( &wch, sizeof(wchar_t) );

	if( 0xfeff == wch ) 
	{
		// 유니코드라는 소리
	}

	while( STREAM_ERR != fs.Read( &wch, sizeof(wchar_t) ) )
	{
		if( bSetKey )
		{
			if( wch == L'\t' )
			{
				bSetKey = false;
			}
			else
			{
				szKey += wch;
			}
		}
		else
		{
			if( wch == L'\n' )
			{
				mapStr.insert( std::make_pair( szKey, szText ) );
				bSetKey = true;

				szKey.clear();
				szText.clear();

			}
			else
			{
				szText += wch;
			}
		}
	}
}

const std::string&		CGameStringTable::GetString( int nNationID, const std::string& szKey ) const
{
	if( ID_LANGUAGE_DEV == nNationID )
		nNationID = ID_LANGUAGE_KOREAN;

	map_nation::const_iterator itr = m_Nations.find( nNationID );

	if( itr == m_Nations.end() )
	{
		return szKey;
	}

//	wchar_t wsz[ MAX_PATH ];
//	i3String::CopyASCIItoWCHAR( wsz, szKey.c_str() );
	i3::wstring wstr;
	i3::mb_to_utf16(szKey.c_str(), wstr);


	const s_nation& nation = itr->second;
	const map_string& mapStr = m_NationStrings[ nation.nID ];

	map_string::const_iterator itr_sz = mapStr.find( wstr.c_str() );

	if( mapStr.end() == itr_sz )
	{
		return szKey;
	}
	
	static std::string ret;
	
//	char sz[ 10000 ];
//	i3String::CopyWCHARtoASCII( sz, 10000, itr_sz->second.c_str(), itr_sz->second.length() +1 );
	i3::string str;
	i3::utf16_to_mb(itr_sz->second.c_str(), str);

//	ret = sz;
	ret.assign(str.c_str(), str.length());
	return ret;
}

const std::string&	CGameStringTable::GetString( const std::string& szKey ) const
{
	CMainFrame* pMain = static_cast< CMainFrame* >( AfxGetMainWnd() );

	if( NULL != pMain )
		return GetString( pMain->m_nNationSel, szKey );

	return szKey;
}

//
//std::wstring	CGameStringTable::GetString( const std::wstring& wszKey ) const
//{
//	CMainFrame* pMain = static_cast< CMainFrame* >( AfxGetMainWnd() );
//
//	if( NULL != pMain )
//	{
//		return GetString( pMain->m_nNationSel, wszKey );
//	}
//
//	return wszKey;
//}
//
//std::string		CGameStringTable::GetString( const std::string& szKey ) const
//{
//	char sz[ 10000 ];
//	wchar_t wszKey[ 1000 ];
//	i3String::CopyASCIItoWCHAR( wszKey, szKey.c_str() );
//
//	std::wstring wsz = GetString( wszKey );
//	i3String::CopyWCHARtoASCII( sz, 10000, wsz.c_str(), wsz.length()+1 );
//
//	return sz;
//}
*/
