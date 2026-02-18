#ifndef _StringFilter_h
#define _StringFilter_h

#include "FilterNode.h"

enum STR_FILTER
{
	STR_FILTER_INCLUDE_NICK = 0,	//
	STR_FILTER_MATCH_NICK,			//
	STR_FILTER_INCLUDE_CHAT_NICK,	//

#if defined( LOCALE_NORTHAMERICA )
	STR_FILTER_INCLUDE_CHAT_NICK2,
#endif

	STR_FILTER_URL,
	STR_FILTER_MAX
};

bool IsValidNicknameLetterSize(const i3::wliteral_range& nickname);

// 반복입력,수시입력 처리의 경우 필터링과 아무 상관없기 때문에 클래스를 분리한다...

namespace detail
{
	template<class T>
	struct is_wchar_str_if_iterator : i3::integral_constant<bool, sizeof(typename i3::iterator_value<T>::type) == sizeof(wchar_t)> {};
	template<class T>
	struct is_wchar_str_if_seq : i3::integral_constant<bool, sizeof(typename i3::range_value<T>::type) == sizeof(wchar_t)> {};
	template<class T>
	struct is_wchar_str : mpl::eval_if<i3::is_iterator<T>, is_wchar_str_if_iterator<T>, is_wchar_str_if_seq<T> > {};

	template<class T, class R>
	struct enable_if_wchar_str : i3::enable_if< is_wchar_str<T>, R> {};



}

#include "i3Base/smart_ptr/pscoped_ptr.h"

typedef i3::pscoped_ptr<class CChattingInputChecker>		CChattingInputCheckerPtr;

class CChattingInputChecker
{
public:
	
	CChattingInputChecker();
	~CChattingInputChecker();

	void					UpdateText(const i3::wliteral_range& wTextRng);
	void					UpdateText(const i3::rc_wstring& wstrText);
	
	template<class T>		typename detail::enable_if_wchar_str<T,
	void>::type				UpdateText(const T& wstr) { UpdateText( i3::wliteral_range(wstr) );   }

	bool					IsInputBlock() const { return m_bInputBlocked;  }
	bool					IsRepeatBlock() const { return m_bRepeatBlocked; }
	
private:
	bool					InputCheckImp(DWORD dwCurrTime);
	bool					RepeatCheckImp(const i3::wliteral_range& wTextRng, DWORD dwCurrTime);
	bool 					UpdateCheckImp(const i3::wliteral_range& wTextRng);

	DWORD					m_dwInputStamp;
	DWORD					m_dwRepeatStamp;
	INT32					m_nRepeatCount;
	i3::rc_wstring			m_wstrOldString;
	
	bool					m_bInputBlocked;
	bool					m_bRepeatBlocked;
};


typedef i3::pscoped_ptr<class CStrFilter>		CStrFilterPtr;

// IsFilterNick은 IsFilterOK가 양덕루틴을 따로 갖고 있는것을 제외하고는 IsFilterOK와 실행상 같은 함수이다..

// IsFilterMatchOK는 테스트문자열의 중간 ~ 끝까지 확실이 필터문자열과 일치한 경우 필터링시킨다....
//                    따라서, 테스트문자열이 필터문자열을 포함한 후 다른 문자열이 이어진다면 전혀
//                    필터링이 이루어지지 않는다..

class CStrFilter
{
public:
	CStrFilter();
	~CStrFilter();
	void					CreateFilter(STR_FILTER mode);
	bool					IsFilterOK( const i3::wliteral_range& wTextRng ) const;			// 채팅문자에 문자열이 포함되어있는지
	bool					IsFilterNickOK( const i3::wliteral_range& wTextRng ) const;		// 닉네임에 문자열이 포함되어있는지
	bool					IsFilterMatchOK( const i3::wliteral_range& wTextRng ) const;		// 문자열이 완전히 같은지를 비교
	
	bool					ChangeStringOK( i3::wstring& wstrText ) const;			// 판호버전용 스트링 함수 제작
	bool					ChangeStringOK( i3::rc_wstring& wstrText) const;
	bool					ChangeStringOK( i3::stack_wstring& wstrText) const;


private:
	CStrFilterRoot			m_root;
};


#endif //_StringFilter_h
