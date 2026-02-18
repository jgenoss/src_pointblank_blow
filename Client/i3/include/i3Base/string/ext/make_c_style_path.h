#pragma once

#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/range/as_literal.h"
#include "../../itl/range/generic_push_back.h"
#include "../../itl/range/generic_clear.h"
#include "../../itl/range/generic_append_range.h"

//
// 전혀 쓰이진 않는것 같은데...MakeCStylePath()의 일반화 버전..
// 함수는 역슬래시 1개를 2개로 바꿔주는 기능을 한다..
// replace_all()함수를 이용해도 좋은데, 이 경우 그 함수인수가 문자열이고 너무 일반화된 상태..
//

namespace i3
{
	namespace detail
	{

		template<class OutRng, class InRng> 
		void make_c_style_path_if_seq( OutRng& out, const InRng& in)	
		{
			i3::generic_clear(out);
			
			typedef typename InRng::iterator						in_iterator_type;
			typedef typename InRng::value_type						in_value_type;
				
			in_iterator_type f = in.begin();
			in_iterator_type l = in.end();

			in_iterator_type found = i3::find( f, l, in_value_type('\\') );

			while(found != l)
			{
				i3::generic_append_range( out, f, ++found);
				i3::generic_push_back(out, in_value_type('\\'));
							
				f = found;
				found = i3::find( f, l, in_value_type('\\') );
			}

			i3::generic_append_range( out, f, found);
		}

		template<class OutIt, class InRng> 
		void make_c_style_path_if_lit( OutIt out, const InRng& in)
		{
			typedef typename InRng::iterator						in_iterator_type;
			typedef typename InRng::value_type						in_value_type;
			
			in_iterator_type f = in.begin();
			in_iterator_type l = in.end();
					
			while( f != l)
			{
				*out = *f;

				if( *f == in_value_type('\\') )
				{
					++out;
					*out = in_value_type('\\');
				}
				++out;
				++f;
			}
			*out = 0;
		}
	}
	
	template<class OutRng, class InRng> inline typename i3::disable_if_iterator<OutRng>::type
	make_c_style_path( OutRng& out, const InRng& in)	
	{
		detail::make_c_style_path_if_seq(out, i3::as_literal(in));
	}
	
	template<class OutIt, class InRng> inline typename i3::enable_if_iterator<OutIt>::type
	make_c_style_path( OutIt out, const InRng& in)
	{
		detail::make_c_style_path_if_lit(out, i3::as_literal(in));
	}

}