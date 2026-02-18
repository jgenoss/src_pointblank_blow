#include "pch.h"
#include "GameStringMap.h"

#include "i3Base/string/algorithm/replace_all.h"
#include "i3Base/string/stack_string.h"
#include "i3Base/string/ext/sprintf.h"
#include "i3Base/string/algorithm/find.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/algorithm/to_lower.h"


namespace
{
	const char* GameStringFileName[CGameStringMap::GSI_MAX] = 
	{
		"String",
		"AICharacter",
		"Basic",
		"Character",
		"Equipment",
		"Goods",
		"Quest",
		"Stage",
		"Weapon",
		"World"
	};
			
}

//--------------------------------------------------------------------------------
// 싱글턴의 인스턴스를 리턴한다.
CGameStringMap*	CGameStringMap::Get()
{
	static CGameStringMap	s_Instance;
	return &s_Instance;
}

//--------------------------------------------------------------------------------
CGameStringMap::CGameStringMap() //: m_TempMemPool(20,64) // 임시 버퍼의 개수와 사이즈
{
}

//--------------------------------------------------------------------------------
CGameStringMap::~CGameStringMap()
{
	Destroy();
}

//--------------------------------------------------------------------------------
// 로딩했던 내용들을 모두 지운다.
void CGameStringMap::Destroy()
{
	for(size_t i=0;i<GSI_MAX;++i)
	{
		m_StringTable[i].string_list.clear();
		m_StringTable[i].label_map.clear();
	}
}

//--------------------------------------------------------------------------------
bool CGameStringMap::_IsCaseSensitive(GameStringIndex idx )
{
	return idx == CGameStringMap::GSI_STRING;
}

bool CGameStringMap::CreateForPointBlank()
{
	return _Create(GetLocalePath(), GlobalFunc::GetLocaleFolderName(g_pEnvSet->m_nLanguage) );
}

bool CGameStringMap::LoadingKoreanLanguage()
{
	return _Create("/korea/", "korea");
}

bool CGameStringMap::LoadingEnglishLanguage()
{
	return _Create("/NorthAmerica/" , "NorthAmerica" );
}

bool CGameStringMap::_Create( const char* local_path, const char* local_name)
{
	Destroy();

	i3::stack_string strDirectory;
#if defined( MULTYLANGUAGE)
	i3::sprintf(strDirectory, "Locale%sString/", local_path );
#else
	i3::sprintf(strDirectory, "Script/String%s", local_path );
#endif
	
	i3::literal_formatter formatter_string_path("%s%s_%s.txt");
	
	i3::string strStringPath;
	
	for(size_t i=0;i<GSI_MAX;++i)
	{
		i3::sprintf( strStringPath, formatter_string_path, strDirectory, GameStringFileName[i], local_name );

		if( !_AddFromTextFilesImpl( m_StringTable[i], strStringPath, _IsCaseSensitive(GameStringIndex(i) ) ) )
		{
			Destroy();
			return false;
		}
	}

	return true;
	
} 



//--------------------------------------------------------------------------------
// 라벨과 지역화된 문자열 파일을 읽어 들인다. (텍스트 버전)
bool CGameStringMap::_AddFromTextFilesImpl( StringTableContext & table, const i3::string& strLocalStringPath, bool case_sensitive )
{
	bool bRet = false;

	i3FileStream	kStringStream;

		if (!kStringStream.Open(strLocalStringPath.c_str(), STREAM_READ | STREAM_SHAREREAD))
		{
#ifdef NC_BUILD
			I3PRINTLOG(I3LOG_FATAL, "Stream IO Error! %s", strLocalStringPath);
#else
			I3PRINTLOG(I3LOG_FATAL, "%s Could not open file. Check working directory.", strLocalStringPath);
#endif

			goto ExitLabel;
		}

	int nLineCount = 1;
	bool bContinue = true;

	// 라벨 파일을 끝까지 읽자.
	while( bContinue )
	{
		wchar_t wsString[2048];			// while 안에 잇거나 밖에 있거나 컴파일러는 같게 취급한다..(2k단위로 강제로 읽는 루틴으로 놔둔다..)

		//////////////////////////////////////////////////////////////////////////
		// 라벨 & 지역화 문자열 읽기
		//////////////////////////////////////////////////////////////////////////
		UINT32 uStringReaded = kStringStream.ReadLineW( wsString, _countof(wsString) );

		if( uStringReaded == 0 )
		{
			if( kStringStream.GetLastError() != i3Stream::STREAM_EOS )
			{
				// ERROR
				I3PRINTLOG(I3LOG_FATAL,  "Failed to read localized string from file. (line = %d) ( file = %s )", nLineCount,  strLocalStringPath );
				goto ExitLabel;
			}
			else
			{
				bContinue = false;
			}
		}
		else
		{
			if (wsString[0] == L'\0' )					// 길이가 0일려면, 그냥 첫원소가 0이면 0이다..  if (wcslen(wsString) == 0)은 불필요...
				continue;
		
			if( !bContinue )
			{
				// ERROR
				// 라벨은 다 읽었는데, 문자열은 아직 남아 있으면 에러.
				I3PRINTLOG(I3LOG_FATAL,  "Label file reached the end of the file, but the localized string file still has something. (file = %s )",  strLocalStringPath );
				goto ExitLabel;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// 읽어들인 라벨 & 문자열 처리
		//////////////////////////////////////////////////////////////////////////
		i3::string strLabel;
	
		if( bContinue )
		{
			// String 구문 변환
			i3::const_wchar_range	rng = i3::as_literal(wsString);
			const wchar_t*  tab_pos = i3::find(rng.begin(), rng.end(), L'\t');	// '\t'으로 분리 한다.
			
			i3::const_wchar_range labelRange(rng.begin(), tab_pos);
			i3::wstring			  localString(tab_pos + 1, rng.end() );		// 탭 문자 자체는 빼는것 같다..
			
			// 단어 치환
			i3::replace_all(localString, L"\\n", L"\n");
			i3::replace_all(localString, L"\\\\", L"\\");
			
			// Uni -> Ansi
			i3::utf16_to_mb(labelRange, strLabel, GetCodePage());

			//대소문자 구분을 없애기 위해 소문자로 변환
			if ( !case_sensitive )
				i3::to_lower(strLabel);

			// Label은 중복되면 안된다.
			StringHashMap::iterator it = table.label_map.find( strLabel );
			if( !strLabel.empty() && it != table.label_map.end() )
			{
				I3PRINTLOG(I3LOG_FATAL,  "Duplicated label is found. (%s)", strLabel );
				goto ExitLabel;
			}

			// String은 벡터에 순서대로 넣는다.
		
			table.string_list.push_back( localString );

			// 라벨은 String의 위치와 함께 맵에 넣는다.
			table.label_map.insert( StringHashMap::value_type( strLabel, table.string_list.size() - 1 ) );

			// 라인 카운터 증가
			nLineCount++;
		}
	}

	// 데이터 크기 검증
	if ( table.label_map.size() != table.string_list.size() )
	{
		I3PRINTLOG(I3LOG_FATAL,  "Invalid string table size error. (file = %s)", strLocalStringPath );
		goto ExitLabel;
	}

	bRet = true;

ExitLabel:
	return bRet;
}

namespace
{
	struct GameStringHashFun
	{
		std::size_t operator()( const i3::literal_range& val) const
		{
			return i3::detail::string_hash_value(val.c_str(), val.size());
		}
		std::size_t operator()( const i3::stack_string& val) const
		{
			return i3::detail::string_hash_value(val.c_str(), val.size());
		}
	};

	struct GameStringEqualFun 
	{
		bool operator()(const i3::string& l, const i3::literal_range& r) const
		{
			return i3::generic_is_equal(l, r);
		}
		bool operator()(const i3::literal_range& l, const i3::string& r) const
		{
			return i3::generic_is_equal(l, r);
		}

		bool operator()(const i3::string& l, const i3::stack_string& r) const
		{
			return i3::generic_is_equal(l, r);
		}
		bool operator()(const i3::stack_string& l, const i3::string& r) const
		{
			return i3::generic_is_equal(l, r);
		}
	};

}
//--------------------------------------------------------------------------------
const i3::rc_wstring& CGameStringMap::_GetStringImpl(StringTableContext& table,  const i3::string& strLabel, bool case_sensitive )
{
	StringHashMap::iterator it;

	if (case_sensitive)
	{
		it = table.label_map.find( strLabel );
	}
	else
	{
		i3::stack_string strLabel2(strLabel);
		i3::to_lower(strLabel2);
		it = table.label_map.find_other( strLabel2, GameStringHashFun(), GameStringEqualFun() );
	}
	
	if( it == table.label_map.end() )
	{
		i3::rc_wstring tmp;		i3::mb_to_utf16(strLabel, tmp);		
		table.string_list.push_back( tmp );
		table.label_map.insert( StringHashMap::value_type( strLabel, table.string_list.size() - 1 ) );

		return table.string_list.back(); 
	}

	return table.string_list[ it->second ];
}




const i3::rc_wstring& CGameStringMap::_GetStringImpl(StringTableContext& table,  const char* strLabel, bool case_sensitive )
{
	StringHashMap::iterator it;
	
	i3::literal_range strTest(strLabel);

	if (case_sensitive)
	{
		it = table.label_map.find_other( strTest, GameStringHashFun(), GameStringEqualFun() );
	}
	else
	{
		i3::stack_string strLabel2(strTest);
		i3::to_lower(strLabel2);
		it = table.label_map.find_other( strLabel2, GameStringHashFun(), GameStringEqualFun() );
	}

	if( it == table.label_map.end() )
	{
		i3::rc_wstring tmp;		i3::mb_to_utf16(strTest, tmp);		
		table.string_list.push_back( tmp );
		table.label_map.insert( StringHashMap::value_type( i3::string(strTest), table.string_list.size() - 1 ) );

		return table.string_list.back(); 
	}

	return table.string_list[ it->second ];
}


const i3::rc_wstring& CGameStringMap::GetString( const i3::string& strLabel, GameStringIndex idx )
{
	return _GetStringImpl(m_StringTable[idx], strLabel, _IsCaseSensitive(idx));
}

const i3::rc_wstring&	CGameStringMap::GetString( const char* strLabel, GameStringIndex idx)
{
	return _GetStringImpl(m_StringTable[idx], strLabel, _IsCaseSensitive(idx));
}


//--------------------------------------------------------------------------------
int CGameStringMap::_GetStringHandleImpl(StringTableContext& table, const i3::string& strLabel, bool case_sensitive)
{
	StringHashMap::iterator it;
	if (case_sensitive)
	{
		it = table.label_map.find( strLabel );
	}
	else
	{
		i3::string strLabel2(strLabel);
		i3::to_lower(strLabel2);
		it = table.label_map.find( strLabel2 );
	}

	if( it == table.label_map.end() )
		return -1;

	return it->second;
}

int CGameStringMap::GetStringHandle(const i3::string& strLabel, GameStringIndex idx )
{
	return _GetStringHandleImpl(m_StringTable[idx], strLabel, _IsCaseSensitive(idx));
}

//--------------------------------------------------------------------------------
// 핸들로 지역화된 문자열를 얻는다.(빠른 검색)
// 만약, 존재하지 않는 핸들이라면 핸들의 숫자값을 문자열로 만들어 반환한다.
const i3::rc_wstring& CGameStringMap::_GetStringByHandleImpl( StringTableContext& table, int nHandle )
{
	if( (int)table.string_list.size() < nHandle )
	{
		i3::string buff;	
		i3::sprintf(buff, "Excel Text _UNKNOWN(%d)", nHandle);
		
		StringHashMap::iterator it = table.label_map.find(buff);			
		if ( it != table.label_map.end() )
			return table.string_list[it->second];				

		i3::wstring tmp;		i3::mb_to_utf16(buff, tmp);
		table.string_list.push_back( tmp );

		table.label_map.insert( StringHashMap::value_type( buff, table.string_list.size() - 1 ) );
		return table.string_list.back();
	}

	return table.string_list[ nHandle ];
}

const i3::rc_wstring& CGameStringMap::GetStringByHandle( int nHandle, GameStringIndex idx )
{
	return _GetStringByHandleImpl(m_StringTable[idx], nHandle);
}


//--------------------------------------------------------------------------------
// 데이터 개수 반환
int CGameStringMap::GetCount(GameStringIndex idx)
{
	return m_StringTable[idx].string_list.size();
}

const CGameStringMap::StringVector& CGameStringMap::GetStringList(GameStringIndex idx) const
{
	return m_StringTable[idx].string_list;
}

const CGameStringMap::StringHashMap& CGameStringMap::GetLabelMap(GameStringIndex idx) const
{
	return m_StringTable[idx].label_map;
}


//
// GameStringFmtW
//
/*
void GameStringFmtW(i3::wstring& rst, const char* gameStr, ...)
{	
	const char* str = GAME_RCSTRING(gameStr);

	if(!str)
		return;

	char buf[1024]; buf[0] = nullptr;

	va_list arg; 
	va_start(arg, gameStr);
	vsprintf_s(buf, 1024, str, arg);
	va_end(arg);
	
	WCHAR16 outWide[2048]; outWide[0] = nullptr;

	INT32 len = strlen(buf);
	INT32 lenWide = ::MultiByteToWideChar(i3Language::GetCurrentCodePage(), 0, buf, len, nullptr, 0);
	::MultiByteToWideChar(i3Language::GetCurrentCodePage(), 0, buf, len, outWide, lenWide);

	outWide[lenWide < 2047 ? lenWide : 2047] = nullptr; 
	rst = outWide;
}
*/
