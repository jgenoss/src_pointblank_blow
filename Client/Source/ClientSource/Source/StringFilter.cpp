#include "pch.h"
#include "StringFilter.h"
#include "i3Base/string/ext/ismb.h"
#include "i3Base/string/algorithm/to_lower.h"
#include "i3Base/string/ext/remove_all_space.h"
#include "i3Base/itl/range/algorithm/find.h"


// I3_CLASS_INSTANCE( CStringFilter);


#define FILTER_STRING_LENGTH		512 // 64  (64가 너무 적은듯 하여 512로 일단 변경) 2014.07.11.수빈.
#define STRING_INPUT_TIME			1

#define STRING_FILTER_LENGTH		512

#define	STRING_FILTERED_CHARACTER	'*'

CChattingInputChecker::CChattingInputChecker() : 	m_nRepeatCount(0), m_dwRepeatStamp(0), m_dwInputStamp(0), 
	m_bInputBlocked(false), m_bRepeatBlocked(false)
{
		
}

CChattingInputChecker::~CChattingInputChecker()
{
	
	
}

void	CChattingInputChecker::UpdateText(const i3::wliteral_range& wTextRng) 
{ 
	if (UpdateCheckImp(wTextRng))
		m_wstrOldString = wTextRng;  
}

void	CChattingInputChecker::UpdateText(const i3::rc_wstring& wstrText) 
{

	if (UpdateCheckImp(wstrText))
		m_wstrOldString = wstrText;  
}

bool	CChattingInputChecker::InputCheckImp(DWORD dwCurrTime)
{
	
#if defined( LOCALE_NORTHAMERICA )				// BUG : 조건매크로가 잘못 걸린 것은 실수버그로 보임...(! 제거)(2014.09.03.수빈)
	// 2011-11-22 이윤호
	// 북미는 똑같은 문자열을 3번 연속으로 쓰면 5분(300초)간 입력을 막는다
	// IsInput은 IsRepeat 보다 먼저 실행되어야 한다
	if( m_nRepeatCount >= 2)
	{
		DWORD dwCurrRepeatElapsed = dwCurrTime - m_dwRepeatStamp;
		
		if ( dwCurrRepeatElapsed < 300 * 1000 )	// 300초..
		{
			m_bInputBlocked = true;
			return false;	
		}
		m_nRepeatCount = 0;						// 300초 후엔 그래도 입력 가능하게 변경..
	}
#endif

	DWORD dwCurrInputElapsed = dwCurrTime - m_dwInputStamp;
	static const DWORD InputTime = DWORD(STRING_INPUT_TIME) * 1000;

	if( dwCurrInputElapsed < InputTime )		// 1초이내로 채팅 엔터키 반복시 인듯 하다..(항상 적용될듯)
	{
		m_bInputBlocked = true;
		return false;
	}

	m_dwInputStamp = dwCurrTime;

	return true;
}

bool	CChattingInputChecker::RepeatCheckImp(const i3::wliteral_range& wTextRng, DWORD dwCurrTime)
{
#if !defined( LOCALE_NORTHAMERICA )
	if( i3::generic_is_equal( m_wstrOldString, wTextRng )  ) 
	{
		DWORD dwCurrRepeatElapsed = dwCurrTime - m_dwRepeatStamp;

		if( dwCurrRepeatElapsed < 3000 )	
		{
			m_bRepeatBlocked = true;
		}
		else
		{
			m_dwRepeatStamp = dwCurrTime;
		}
		return false;		//      // 이곳의 false는 m_wstrOldString 대입을 막기 위한 것... 
	}
	
	m_dwRepeatStamp = dwCurrTime;	// 문자열이 다르면 반복타임값을 현재값으로 리셋..

#else
	if(i3::generic_is_equal( m_wstrOldString, wTextRng )  )
	{
		++m_nRepeatCount;
		if( m_nRepeatCount >= 2 )		// 2번째까지는 반복허용..2번째에 타임스탬프찍고, 3번째때 바로 블록되게..
			m_dwRepeatStamp = dwCurrTime;

		return false;				// 이곳의 false는 m_wstrOldString 대입을 막기 위한 것...
	}
	else
	{
		m_nRepeatCount	= 0;
	}

#endif
	return true;
}


bool 	CChattingInputChecker::UpdateCheckImp(const i3::wliteral_range& wTextRng)
{
	m_bInputBlocked = false;
	m_bRepeatBlocked = false;

	DWORD dwCurrTime = ::timeGetTime();

	if ( !InputCheckImp(dwCurrTime) )			// 인풋 블록킹..
		return false;
	
	return RepeatCheckImp(wTextRng, dwCurrTime);		// 동일문자열 반복 블록킹...
}



namespace
{
	void	FilterTextOptimize( wchar_t* wszText ) 
	{
		// 공백을 제거하고 대문자를 소문자로 바꿔줌
		const wchar_t* p = wszText;

		char cSub = L'a'- L'A';

		while( *p ) {
			if( *p != L' ' )
			{
				if( *p >= L'A' && *p <= L'Z' ) {
					*wszText = (*p) + cSub;
				}
				else
				{
					*wszText = (*p);
				}
				wszText++;
			}
			p++;
		}

		*wszText = L'\0';
	}
	
	template<class Seq>		typename i3::disable_if_iterator<Seq,
	void>::type FilterTextOptimize(Seq& inout)
	{
		i3::remove_all_space(inout);
		i3::to_lower(inout);
	}
	
	// 공백 지우고, 소문자화하는 루틴은 상동이지만, 변경된 스트링에서 원스트링으로 복구할수 있는 매핑구조 2개를 준비해둔다..
	
	void PrepareStringRemap(const i3::wliteral_range& rng, i3::vector<size_t>& outSpacePosMap, i3::vector<size_t>& outUpperPosMap)
	{
		const size_t num = rng.size();
		size_t index_count = 0;
		
		for (size_t i = 0 ; i < num ; ++i )
		{
			if ( rng[i] == L' ' )
			{
				outSpacePosMap.push_back(index_count);
			}
			else
			{
				if ( L'A' <= rng[i] && rng[i] <= L'Z' )		// 대문자인 경우 결과물 기준으로 위치기억해둔다..
					outUpperPosMap.push_back(index_count);
				++index_count;
			}
		}
	}
	
	template<class Seq>		typename i3::disable_if_iterator<Seq,
	void>::type RestoreStringFromOptimize(Seq& inout, const i3::vector<size_t>& SpacePosMap, const i3::vector<size_t>& UpperPosMap)
	{
		const size_t num_space = SpacePosMap.size();
		const size_t num_upper = UpperPosMap.size();
		
		if (num_space + num_upper == 0) return;

		static const wchar_t cSub = L'a'- L'A';

		// 먼저 대문자복귀부터 처리..
		for (size_t i = 0; i < num_upper ; ++i)
		{
			wchar_t val = inout[ UpperPosMap[i] ];
			
			if (val != STRING_FILTERED_CHARACTER)
				inout[UpperPosMap[i]] -= cSub;
		}
		
		if (num_space)
		{
			inout.insert(0u, num_space, 0);		// 아무거나 빈문자열을 일단 맨 앞에 더한다..
												// 맨 앞에 더하는 이유는 2회이상의 재할당을 막으면서...
												// 뒷쪽 부분을 복사한걸 앞에서 삽입하려는 목적이다..
													// num_space는 offset역할을 해줄 것이다...
			Seq::iterator begin_copy = inout.begin() + num_space;
			Seq::iterator end_copy = begin_copy; 
			Seq::iterator dest     = inout.begin();

			for (size_t i = 0 ; i < num_space ; ++i )
			{
				end_copy = inout.begin() + (SpacePosMap[i] + num_space); // 결과물의 공백 위치..
				dest = i3::copy( begin_copy, end_copy, dest);
				*dest = L' ';	++dest;				// 필요한 공백은 여기서 처리...
				begin_copy = end_copy;
			}
		}
	}
	
	
	const char * g_szFilterFile[STR_FILTER_MAX] =
	{
		"StrTbl_Filter_Include_Nick.txt",        //0번
		"StrTbl_Filter_Match_Nick.txt",			 //1번
		"StrTbl_Filter_Include_Chat_Nick.txt",	 //2번

#if defined( LOCALE_NORTHAMERICA )
		"StrTbl_Filter_Include_Chat_Nick2.txt",		//2-2번 (북미만)
#endif

		"StrTbl_Filter_URL.txt",				 //3번
	};

	void		AddStringImp(const i3::wliteral_range& wstr, CStrFilterRoot* root)
	{
		const std::size_t num_str = wstr.size();		
		if (!num_str) return;					

		bool is_end = (wstr.size() == 1);
		
		CStrFilterNode* child = root->InsertChild( wstr[0], is_end);
		
		i3::wliteral_range next_range = wstr;		next_range.advance_begin(1);

		while ( !next_range.empty() )
		{
			is_end = (next_range.size() == 1);
			
			child = child->InsertChild(next_range[0], is_end);

			next_range.advance_begin(1);
		}
	}

	//
	// TODO : UTF16 유니코드의 경우, 태국어를 위한 i3String::CharNext 함수 처리 없이도 
	// ::CharNextW()로 처리가 가능하다는 얘기를 들었지만, 확실치 않으므로 조사필요하다. (2014.07.11.수빈)
	//
	bool    IsSameTextIncludeImp( const i3::wliteral_range& test_rng, const CStrFilterRoot* root)
	{
		CStrFilterNode* child =	root->FindChild(test_rng[0]);		
		if (!child)
			return false;
		
		if (child->IsEnd() )
			return true;
		
		i3::wliteral_range next_rng = test_rng;		next_rng.advance_begin(1);

		while ( !next_rng.empty() )
		{
			child = child->FindChild(next_rng[0]);
			if (!child)
				return false;
			if ( child->IsEnd() )
				return true;

			next_rng.advance_begin(1);
		}

		return false;
	}
	
	// 이 함수는 리턴값등으로 판별하면 안된다..필터링될만한 단어는 모두 필터스트링 변경..
	bool   IsSameTextIncludeChangeImp(const i3::mutable_wliteral_range& inout_rng, const CStrFilterRoot* root, 
		i3::mutable_wliteral_range::iterator& out_first)
	{
		out_first = inout_rng.begin();

		// inout_rng는 advance되서는 안된다...
		CStrFilterNode* child =	root->FindChild(inout_rng[0]);		
		if (!child)
			return false;		// 필터링 안된것 확정..
		
		bool bFiltered = false;
		
		i3::mutable_wliteral_range::iterator fill_end = inout_rng.begin();

		if (child->IsEnd())
		{
			++fill_end;
			bFiltered = true;						// 리턴하면 안된다..
		}
		
		i3::mutable_wliteral_range next_rng = inout_rng;	next_rng.advance_begin(1);

		while( !next_rng.empty() )
		{
			child = child->FindChild(next_rng[0]);
			if (!child)
				break;
			if ( child->IsEnd() )
			{
				fill_end = next_rng.begin();	++fill_end;
				bFiltered = true;
			}

			next_rng.advance_begin(1);
		}
		
		if (bFiltered)
		{
			i3::fill( inout_rng.begin(), fill_end, STRING_FILTERED_CHARACTER);
			out_first = fill_end;
		}

		return bFiltered;
	}


	//  이경우는 test_rng 문자열이 필터문자열과 완벽히 일치해야한다...
	bool	IsSameTextMatchImp(const i3::wliteral_range& test_rng, const CStrFilterRoot* root)
	{
		CStrFilterNode* child =	root->FindChild(test_rng[0]);		
		if (!child)
			return false;

		i3::wliteral_range next_rng = test_rng;		next_rng.advance_begin(1);

		if ( child->IsEnd() && next_rng.empty() )
			return true;
		
		while ( !next_rng.empty() )
		{
			child = child->FindChild(next_rng[0]);
			if (!child)
				return false;

			next_rng.advance_begin(1);

			if (child->IsEnd() && next_rng.empty() )
				return true;
		}
		return false;
	}


	bool	CheckTextImp( const i3::wliteral_range& wTextRng, const CStrFilterRoot* root) 
	{
		const wchar_t* first = wTextRng.begin();
		const wchar_t* last  = wTextRng.end();

		while( first < last )
		{
			i3::wliteral_range test_rng(first, last);

			if( IsSameTextIncludeImp( test_rng, root) )		// test_rng 안에 필터문자열 포함될 경우..
				return false;
			
			first = ::CharNextW(first);
			//(char*) i3String::CharNext(GetCodePage(), pszText);
		}
		return true;
	}

	bool	CheckTextMatchImp(const i3::wliteral_range& wTextRng, const CStrFilterRoot* root )
	{
		const wchar_t* first = wTextRng.begin();
		const wchar_t* last  = wTextRng.end();


		while( first < last )
		{
			i3::wliteral_range test_rng(first, last);

			if( IsSameTextMatchImp( test_rng, root )) 
				return false;
			
			first = ::CharNextW(first);
			// (char*) i3String::CharNext(GetCodePage(), (const char*) pszText);		//	�ґЩ±№ѕоїл
		}

		return true;
	}


	// 필터링된 문자열을 STRING_FILTERED_CHARACTER 로 수정 
	// 리턴값을 활용하는게 좋겠다...필터링이 되었다면 true, 원본과 같다면 false...

	template<class OptimizedSeq>			typename i3::disable_if_iterator<OptimizedSeq,
	bool>::type	ChangeTextImp(const i3::wliteral_range& wOriginRng, OptimizedSeq& inoutOptimized, const CStrFilterRoot* root) //pszText : Copy from Optimized ChatText
	{
		wchar_t* first = inoutOptimized.begin();
		wchar_t* last  = inoutOptimized.end();
		
		bool bFiltered = false;
		i3::mutable_wliteral_range change_rng(first, last);

		i3::mutable_wliteral_range::iterator next_first;

		while( first < last )
		{
			change_rng.set_begin(first);

			if ( IsSameTextIncludeChangeImp(change_rng, root, next_first) )
			{
				bFiltered = true;
				first = next_first;
			}
			else
			{
				first = ::CharNextW(first);
			}
			// (char*) i3String::CharNext(GetCodePage(), (const char*)pAddr);
			// TODO : CharNext 코드 점검 해볼 것..(2014.07.11.수빈)
		}
		
		if (bFiltered == false)		// 필터링 된게 없으므로 그냥 리턴..
			return true;
		
		i3::vector<size_t>		SpacePosMap;		SpacePosMap.reserve(wOriginRng.size() - inoutOptimized.size());
		i3::vector<size_t>		UpperPosMap;		UpperPosMap.reserve(inoutOptimized.size() / 2); // 일단 문자열의 절반정도만..

		PrepareStringRemap(wOriginRng, SpacePosMap, UpperPosMap);
		RestoreStringFromOptimize(inoutOptimized, SpacePosMap, UpperPosMap);
		
		return false;	
	}

	bool	ChangeTextImp_NorthAmeria( const i3::mutable_wliteral_range& wTextRng, const CStrFilterRoot* root)
	{

		i3::mutable_wliteral_range change_rng = wTextRng;

		typedef i3::custom_stack_wstring<FILTER_STRING_LENGTH>::type		stack_wstring_type;
		stack_wstring_type wstrTemp;

		bool bFiltered = false;

		while (! change_rng.empty() )
		{
			wchar_t* blank_pos = i3::range::find(change_rng, L' ');

			wstrTemp.assign( change_rng.begin(), blank_pos );
			i3::to_lower(wstrTemp);

			if ( IsSameTextMatchImp(wstrTemp, root) )		// 검사단어가 필터링 단어와 완벽히 일치할때만 변경처리.
			{
				i3::fill( change_rng.begin(), blank_pos, STRING_FILTERED_CHARACTER);
				bFiltered = true;
			}

			if (blank_pos == change_rng.end() )
				break;

			++blank_pos;

			change_rng.set_begin( blank_pos );
		}

		return !bFiltered;
	}



}


CStrFilter::CStrFilter()
{

}

CStrFilter::~CStrFilter()
{

}

void	CStrFilter::CreateFilter(STR_FILTER mode)
{

	char szPath[MAX_PATH] = "";
	{
#if defined ( MULTYLANGUAGE)
		sprintf_s( szPath, "Locale%sString/%s", g_pStringFilePath.c_str(), g_szFilterFile[mode]);
#else
		char szStringPath[MAX_STRING_COUNT] = "";
		sprintf_s( szStringPath, "Script/String/%s", g_szFilterFile[mode]);
		//	각 언어에 맞는 파일로 설정한다.
		ConvertLocalePath( szPath, szStringPath, g_pStringFilePath.c_str());
#endif
	}

	i3FileStream file;

	if( false == file.Open( szPath, STREAM_READ | STREAM_SHAREREAD) ) return;

	//	유니코드 파일인지 검사
	{
		WCHAR16 test_word;
		file.Read( &test_word, sizeof( WCHAR16));

		//	여기에 걸리면 스크립트 파일이 유니코드가 아닙니다. 반드시 유니코드 파일이어야 합니다. komet
		if( !CHECK_UNICODE(test_word))
		{
			I3PRINTLOG(I3LOG_FATAL,  "String Filter have not unicode !! (%s)", szPath);
			return;
		}
	}

	typedef i3::custom_stack_wstring<FILTER_STRING_LENGTH>::type		stack_wstring_type;

	stack_wstring_type	wstrReadLine;

	for ( ; ; )
	{
		UINT32 nReadByte = file.ReadLine(wstrReadLine);
		if (nReadByte == STREAM_ERR)
			break;
		
		// 유니코드로 된 텍스트를 한줄씩 읽습니다.
		if( wstrReadLine.empty() )	continue;

		if( i3::generic_is_iequal( wstrReadLine, L"[StringTableEnd]" ) )
			break;
		
		FilterTextOptimize( wstrReadLine );		// 공백 압축 루틴일 것같다..

		AddStringImp( wstrReadLine, &m_root);
	}

	file.Close();
}

bool	CStrFilter::IsFilterOK( const i3::wliteral_range& wTextRng ) const		// 채팅문자에 문자열이 포함되어있는지
{
	typedef i3::custom_stack_wstring<FILTER_STRING_LENGTH>::type		stack_wstring_type;
	
#if !defined( LOCALE_NORTHAMERICA)
	stack_wstring_type wstrTemp = wTextRng;

	FilterTextOptimize( wstrTemp );
	return CheckTextImp( wstrTemp , &m_root);

#else
	// 양덕 루틴은 테스트문자열을 공백단위 단어로 끊고, 그걸로 필터문자열과 완벽히 일치하는지를 본다..

	i3::wliteral_range test_rng = wTextRng;
	
	stack_wstring_type wstrTemp;

	while (! test_rng.empty() )
	{
		const wchar_t* blank_pos = i3::range::find(test_rng, L' ');
		
		wstrTemp.assign( test_rng.begin(), blank_pos );
		i3::to_lower(wstrTemp);

		if ( IsSameTextMatchImp(wstrTemp, &m_root) )
			return false;

		if (blank_pos == test_rng.end() )
			break;

		++blank_pos;

		test_rng.set_begin( blank_pos );
	}

	return true;
#endif

}
	
bool	CStrFilter::IsFilterNickOK( const i3::wliteral_range& wTextRng ) const
{
	typedef i3::custom_stack_wstring<FILTER_STRING_LENGTH>::type		stack_wstring_type;
	stack_wstring_type wstrTemp = wTextRng;
	FilterTextOptimize( wstrTemp );
	return CheckTextImp( wstrTemp, &m_root );
}

bool	CStrFilter::IsFilterMatchOK( const i3::wliteral_range& wTextRng ) const	// 문자열이 완전히 같은지를 비교
{
	typedef i3::custom_stack_wstring<FILTER_STRING_LENGTH>::type		stack_wstring_type;
	stack_wstring_type wstrTemp = wTextRng;
	FilterTextOptimize( wstrTemp );
	return CheckTextMatchImp( wstrTemp, &m_root);
}

bool	CStrFilter::ChangeStringOK( i3::wstring& wstrText )	const		// 판호버전용 스트링 함수 제작
{
	
//#if !defined( LOCALE_NORTHAMERICA)
	typedef i3::custom_stack_wstring<FILTER_STRING_LENGTH>::type		stack_wstring_type;

	stack_wstring_type wstrTemp = wstrText;
	FilterTextOptimize( wstrTemp );

	if ( ChangeTextImp(wstrText, wstrTemp, &m_root) )
		return true;

	wstrText = wstrTemp;
	return false;

//#else 

//	return ChangeTextImp_NorthAmeria(wstrText);		// 미국버전만 쓰는듯..

//#endif
	
}

bool	CStrFilter::ChangeStringOK( i3::rc_wstring& wstrText) const
{
//#if !defined( LOCALE_NORTHAMERICA)
	typedef i3::custom_stack_wstring<FILTER_STRING_LENGTH>::type		stack_wstring_type;

	stack_wstring_type wstrTemp = wstrText;
	FilterTextOptimize( wstrTemp );

	if ( ChangeTextImp(wstrText, wstrTemp, &m_root) )
		return true;

	wstrText = wstrTemp;
	return false;

//#else
//	typedef i3::custom_stack_wstring<FILTER_STRING_LENGTH>::type		stack_wstring_type;
//	stack_wstring_type wstrTemp = wstrText;
//
//	bool bOK = ChangeTextImp_NorthAmeria(wstrTemp);	
//	if (!bOK)
//		wstrText = wstrTemp;
//	return bOK;		// 미국버전만 쓰는듯..
//#endif
	
}

bool	CStrFilter::ChangeStringOK( i3::stack_wstring& wstrText) const
{
//#if !defined( LOCALE_NORTHAMERICA)
	typedef i3::custom_stack_wstring<FILTER_STRING_LENGTH>::type		stack_wstring_type;

	stack_wstring_type wstrTemp( wstrText.c_str(), wstrText.size() );

	FilterTextOptimize( wstrTemp );

	if ( ChangeTextImp(wstrText, wstrTemp, &m_root) )
		return true;

	wstrText.assign(wstrTemp.c_str(), wstrTemp.size());
	return false;

//#else 
//
//	return ChangeTextImp_NorthAmeria(wstrText);		// 미국버전만 쓰는듯..
//
//#endif
	
}



void _parseLocaleWord(char* & word)
{
//1Byte 국가
//라틴어, 러시아어, 터키어, 아랍어, 태국어, 영어, 그리스어, 히브리어, 발트어, 중앙유럽어(동유럽어)
//베트남어, 서유럽어

//1 - 2Byte 국가
//한국, 중국(번체)

//1 - 3Byte 국가
//일본, 중국(간체)

// 참조 : http://arigaram-textcube.blogspot.com/2008/04/charset-%EA%B3%BC-codepage-%EB%AA%A9%EB%A1%9D.html
//				http://postgresql.kr/docs/8.4/multibyte.html


#if defined( LOCALE_KOREA ) && !defined( SALES_BUILD)
	for( int i = 0; i < 2; ++i )
	{
		if( *(++word) >= 0 )
			break;
	}
#else
	++word;
#endif
}

namespace
{
	inline bool is_1st_surrogate(wchar_t c) {	return 0xD800 <= c && c <= 0xDBFF;	}

#if defined ( LOCALE_RUSSIA)
	static const UINT32 ValidScoreLimit = 16;		// 러시아 닉네임 최대 길이
	static const UINT32 ValidScoreMin = 4;			// 러시아 닉네임 최소 길이
	static const UINT32 MaxNicknameLength = ValidScoreLimit;
#else
	static const UINT32 ValidScoreLimit = 16;		// 한계 점수
	static const UINT32 ValidScoreMin = 2;			// 최소 점수
	static const UINT32 MaxNicknameLength = NET_NICK_NAME_SIZE - 1;
#endif
}

bool IsValidNicknameLetterSize(const i3::wliteral_range& nickname)
{
	const size_t num = nickname.size();
	int score = 0;

	for (size_t i = 0; i < num ; ++i, ++score)
	{
		if ( is_1st_surrogate(nickname[i]) )
			++i;
	}
	
	// 평가
	if (ValidScoreMin <= score && ValidScoreLimit >= score)
		return true;
	
	I3PRINTLOG(I3LOG_DEFALUT, "NickName Length : %d ", score);
	return false;
}

/*
bool CStringFilter::IsValidNickname(const char* nickname)
{
	//이정우 : 닉네임 스트링 수 구하기(2011/04/18)
	char* ptrString = (char*)nickname;
	int score = 0;

	while( *ptrString )
	{
		//아스키 코드일 경우
		if( *ptrString > 0 )
		{
			++ptrString;
		}
		//지역 코드일 경우
		else
		{
			_parseLocaleWord(ptrString);
		}
		++score;
	}

	// 평가
	if (ValidScoreMin <= score && ValidScoreLimit >= score)
	{
		return true;
	}
	else
	{
		I3PRINTLOG(I3LOG_DEFALUT, "NickName Length : %d ", score);
		return false;
	}
}
*/
