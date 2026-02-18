#if !defined(pp_is_iterating)

#ifndef i3_snprint_f_h
#define i3_snprint_f_h

#include "detail/sprint_object.h"
#include "sprintf_formatter.h"
#include "detail/snprint_io.h"
#include "detail/nformatter_io.h"

#include "../../itl/preprocessor/ext/pp_enum_params_cat_params.h"
#include "../../itl/preprocessor/ext/pp_repeat_params.h"

#define i3_snprintf_arity_limit		20

namespace i3
{
	template<class OutIt, class FmtRange> inline 
	typename i3::enable_if_iterator<OutIt,	
	size_t>::type snprintf( OutIt out, std::size_t out_num, const FmtRange& rng)
	{
		using namespace i3::detail_sprintf;
		typedef typename i3::range_value<const FmtRange>::type Ch;
		return  snprint_io<Ch>(sprint_object< Ch >(as_literal(rng)), out_num).result(out);
	}

	template<class OutSeq, class FmtRange> inline 
	typename i3::disable_if_iterator<OutSeq,	
	size_t>::type snprintf( OutSeq& out, std::size_t out_num, const FmtRange& rng)
	{	
		using namespace i3::detail_sprintf;
		typedef typename i3::range_value<const FmtRange>::type Ch;
		return snprint_io<Ch>( sprint_object< Ch >(as_literal(rng)), out_num).result(out);
	}	
	
	template<class OutIt, class FSeq> inline 
	typename i3::enable_if_iterator<OutIt,	
	size_t>::type snprintf( OutIt out, std::size_t out_num, const sprintf_formatter<FSeq>& fmter)
	{
		using namespace i3::detail_sprintf;
		return	nformatter_io<sprintf_formatter<FSeq> >(fmter, out_num).result(out);
	}

	template<class OutSeq, class FSeq> inline 
	typename i3::disable_if_iterator<OutSeq,	
	size_t>::type snprintf( OutSeq& out, std::size_t out_num, const sprintf_formatter<FSeq>& fmter)
	{	
		using namespace i3::detail_sprintf;
		return nformatter_io<sprintf_formatter<FSeq> >(fmter, out_num).result(out);
	}
	
	template<class OutIt, class FmtRange, class A0> inline 
	typename i3::enable_if_iterator<OutIt,	
	size_t>::type snprintf( OutIt out, std::size_t out_num, const FmtRange& rng, const A0& a0)
	{
		using namespace i3::detail_sprintf;
		typedef typename i3::range_value<const FmtRange>::type Ch;
		return (snprint_io<Ch>( sprint_object< Ch >(as_literal(rng)) , out_num) % a0).result(out);
	}

	template<class OutSeq, class FmtRange, class A0> inline 
	typename i3::disable_if_iterator<OutSeq,	
	size_t>::type snprintf( OutSeq& out, std::size_t out_num, const FmtRange& rng, const A0& a0)
	{	
		using namespace i3::detail_sprintf;
		typedef typename i3::range_value<const FmtRange>::type Ch;
		return (snprint_io<Ch>( sprint_object< Ch >(as_literal(rng)), out_num) % a0).result(out);
	}	
	
	template<class OutIt, class FSeq, class A0> inline 
	typename i3::enable_if_iterator<OutIt,	
	size_t>::type snprintf( OutIt out, std::size_t out_num, const sprintf_formatter<FSeq>& fmter, const A0& a0)
	{
		using namespace i3::detail_sprintf;
		return	(nformatter_io<sprintf_formatter<FSeq> >(fmter, out_num) % a0).result(out);
	}

	template<class OutSeq, class FSeq, class A0> inline 
	typename i3::disable_if_iterator<OutSeq,	
	size_t>::type snprintf( OutSeq& out, std::size_t out_num, const sprintf_formatter<FSeq>& fmter, const A0& a0)
	{	
		using namespace i3::detail_sprintf;
		return (nformatter_io<sprintf_formatter<FSeq> >(fmter, out_num) % a0).result(out);
	}

	
}

#define		pp_iteration_params_1	(3, (2, i3_snprintf_arity_limit, "snprintf.h"))		// 0 ~ 5까지 특화 / 자체재귀..(양개구간임에 유의할것)
#include	pp_iterate()


#endif // i3_snprint_f_h

#else // pp_is_iterating

#define i_				pp_frame_iteration(1)

namespace i3
{
	template<class OutIt, class FmtRange, pp_enum_params(i_, class A) > inline 
	typename i3::enable_if_iterator<OutIt,	
	size_t>::type snprintf( OutIt out, std::size_t out_num, const FmtRange& rng, pp_enum_params_cat_params(i_, const A, &, a) )
	{
		using namespace i3::detail_sprintf;
		typedef typename i3::range_value<const FmtRange>::type Ch;
		return (snprint_io<Ch>(sprint_object< Ch >(as_literal(rng)), out_num) pp_repeat_params(i_, %a ) ).result(out);
	}

	template<class OutSeq, class FmtRange, pp_enum_params(i_, class A) > inline 
	typename i3::disable_if_iterator<OutSeq,	
	size_t>::type snprintf( OutSeq& out, std::size_t out_num, const FmtRange& rng, pp_enum_params_cat_params(i_, const A, &, a) )
	{	
		using namespace i3::detail_sprintf;
		typedef typename i3::range_value<const FmtRange>::type Ch;
		return (snprint_io<Ch>(sprint_object< Ch >(as_literal(rng)), out_num) pp_repeat_params(i_, %a ) ).result(out);
	}	
	
	template<class OutIt, class FSeq, pp_enum_params(i_, class A)> inline 
	typename i3::enable_if_iterator<OutIt,	
	size_t>::type snprintf( OutIt out, std::size_t out_num, const sprintf_formatter<FSeq>& fmter, pp_enum_params_cat_params(i_, const A, &, a))
	{
		using namespace i3::detail_sprintf;
		return	(nformatter_io<sprintf_formatter<FSeq> >(fmter, out_num) pp_repeat_params(i_, %a ) ).result(out);
	}

	template<class OutSeq, class FSeq, pp_enum_params(i_, class A)> inline 
	typename i3::disable_if_iterator<OutSeq,	
	size_t>::type snprintf( OutSeq& out, std::size_t out_num, const sprintf_formatter<FSeq>& fmter, pp_enum_params_cat_params(i_, const A, &, a))
	{	
		using namespace i3::detail_sprintf;
		return (nformatter_io<sprintf_formatter<FSeq> >(fmter, out_num) pp_repeat_params(i_, %a ) ).result(out);
	}

}

#undef i_

#endif // pp_is_iterating

