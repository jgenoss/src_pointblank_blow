#if !defined(pp_is_iterating)

#ifndef i3_format_rcstring_h
#define i3_format_rcstring_h

// 단순이, i3::string/i3::wstring을 대상으로 i3::sprintf의 출력값을 리턴타입으로 돌리게 해준다.  snprintf버전은 필요가 없을것으로 보인다..
#include "sprintf.h"

#define i3_format_rcstring_arity_limit		20

namespace i3
{
	namespace detail
	{
		template<class T>
		struct lazy_rcstring 
		{
			typedef i3::rc_basic_string< typename i3::range_value<T>::type >		type;
		};
	}

	template<class FmtRange> inline typename i3::lazy_disable_if< i3::is_base_of<sprintf_formatter_base, FmtRange>, 
		detail::lazy_rcstring<FmtRange> >::type format_rcstring(const FmtRange& rng)
	{
		i3::rc_basic_string<typename i3::range_value<FmtRange>::type > res;
		i3::sprintf(res, rng);
		return res;
	}

	template<class FSeq> inline 
		i3::rc_basic_string<typename i3::range_value<FSeq>::type > format_rcstring( const sprintf_formatter<FSeq>& fmter)
	{
		i3::rc_basic_string<typename i3::range_value<FSeq>::type > res;
		i3::sprintf(res, fmter);
		return res;
	}

	template<class FmtRange, class A0> inline typename i3::lazy_disable_if< i3::is_base_of<sprintf_formatter_base, FmtRange>, 
		detail::lazy_rcstring<FmtRange> >::type format_rcstring( const FmtRange& rng, const A0& a0)
	{
		i3::rc_basic_string<typename i3::range_value<FmtRange>::type > res;
		i3::sprintf(res, rng, a0);
		return res;
	}

	template<class FSeq, class A0> inline 
		i3::rc_basic_string<typename i3::range_value<FSeq>::type > format_rcstring( const sprintf_formatter<FSeq>& fmter, const A0& a0)
	{
		i3::rc_basic_string<typename i3::range_value<FSeq>::type > res;
		i3::sprintf(res, fmter, a0);
		return res;
	}
}

#define		pp_iteration_params_1	(3, (2, i3_format_rcstring_arity_limit, "format_rcstring.h"))		// 0 ~ 5까지 특화 / 자체재귀..(양개구간임에 유의할것)
#include	pp_iterate()

#endif //i3_format_rcstring_h

#else // pp_is_iterating

#define i_				pp_frame_iteration(1)

namespace i3
{
	template<class FmtRange, pp_enum_params(i_, class A) > inline typename i3::lazy_disable_if< i3::is_base_of<sprintf_formatter_base, FmtRange>, 
		detail::lazy_rcstring<FmtRange> >::type
		format_rcstring( const FmtRange& rng, pp_enum_params_cat_params(i_, const A, &, a) )
	{
		i3::rc_basic_string<typename i3::range_value<FmtRange>::type > res;
		i3::sprintf(res, rng, pp_enum_params(i_, a) );
		return res;
	}

	template<class FSeq, pp_enum_params(i_, class A)> inline 
		i3::rc_basic_string<typename i3::range_value<FSeq>::type > 
		format_rcstring( const sprintf_formatter<FSeq>& fmter, pp_enum_params_cat_params(i_, const A, &, a))
	{
		i3::rc_basic_string<typename i3::range_value<FSeq>::type > res;
		i3::sprintf(res, fmter, pp_enum_params(i_, a) );
		return res;
	}
}

#undef i_

#endif // pp_is_iterating
