#pragma once

// 구버전 MakeRelativePath의 일반화버전... ( 구함수는 상당이 많이 쓰여졌고, 또 유용하다...)
#include "resolve_path_token.h"
#include "../../itl/range/generic_append_range.h"
#include "../../itl/range/generic_clear.h"
#include "../compare/generic_is_iequal.h"
#include "generic_string_copy.h"

namespace i3
{	
	namespace detail
	{
		template<class RngRefOrLit, class InRng1, class InRng2> 
		void make_relative_path_impl( const InRng1& base_path, const InRng2& abs_path, RngRefOrLit out_rel_path)
		{
			i3::generic_clear(out_rel_path);

			typedef typename i3::range_value<InRng1>::type					base_value_type;
			typedef typename i3::range_value<InRng2>::type					abs_value_type;
			compile_assert_pred( (i3::is_same< base_value_type, abs_value_type>) );
			
			typedef base_value_type											Ch;
			typedef i3::rc_basic_string<Ch>									rc_string_Ch;

			i3::vector<rc_string_Ch>		AbsToken;
			i3::resolve_path_token( abs_path, AbsToken);
			if( AbsToken.empty() )
				return;

			i3::vector<rc_string_Ch>		BaseToken;
			i3::resolve_path_token( base_path, BaseToken);
			if ( BaseToken.empty() )
			{
				i3::generic_string_copy( out_rel_path, abs_path );
				return;
			}

	#if defined( I3_WINDOWS)		// 첫번째토큰에는 드라이브 문자가 들어가는데, 다르다면 원본 카피 후 종료.
			if( i3::generic_is_iequal( BaseToken[0], AbsToken[0] ) == false ) 
			{
				i3::generic_string_copy( out_rel_path, abs_path);		
				return;
			}
			size_t idx = 1;
		
	#else
			size_t idx = 0;
	#endif	
			const size_t numAbsToken = AbsToken.size();
			const size_t numBaseToken = BaseToken.size();
		
			// 미스 매칭 된것부터 찾는다.... (구버전에서 떼서 먼저 구현)

			for (; idx < numAbsToken && idx < numBaseToken  ; ++idx)
			{
				if ( !i3::generic_is_iequal(AbsToken[idx] , BaseToken[idx]) )
				{
					break;
				}
			}
		
			// 남아있는 BaseToken이 있다면 그 갯수만큼 ".."를 붙인다..( 상대경로에서 미스된만큼 앞으로 전진)
			const i3::iterator_range<const Ch*> dotdot = i3::as_literal(i3::make_generic_literal(Ch, ".."));
			const i3::iterator_range<const Ch*> slash  = i3::as_literal(i3::make_generic_literal(Ch, "\\"));

			for (size_t i = idx ; i < numBaseToken ; ++i )
			{
				i3::generic_append_range( out_rel_path, dotdot);
				i3::generic_append_range( out_rel_path, slash);
			}
			
			// 남아있는 AbsToken을 주욱 복사하면 된다.. (맨마지막것만 슬래시를 쓰지 않는것에 유의하만 된다.)
			if ( idx < numAbsToken )
			{
				for ( ; idx + 1 < numAbsToken ; ++idx )
				{
					i3::generic_append_range( out_rel_path, AbsToken[idx]);
					i3::generic_append_range( out_rel_path, slash);
				}

				i3::generic_append_range(out_rel_path, AbsToken[idx]);
			}	
		}

	}

	template<class InRng1, class InRng2, class OutRng> inline typename i3::disable_if_iterator<OutRng>::type
	make_relative_path( const InRng1& base_path, const InRng2& abs_path, OutRng& out_rel_path)
	{
		detail::make_relative_path_impl<OutRng&>(i3::as_literal(base_path), i3::as_literal(abs_path), out_rel_path);	
	}

	template<class InRng1, class InRng2, class OutIt> inline typename i3::enable_if_iterator<OutIt>::type
	make_relative_path( const InRng1& base_path, const InRng2& abs_path, OutIt out_rel_path)
	{
		detail::make_relative_path_impl<OutIt>(i3::as_literal(base_path), i3::as_literal(abs_path), out_rel_path);			
	}
	
}