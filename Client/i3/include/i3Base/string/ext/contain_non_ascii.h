#pragma once

// wchar_t까지 고려한다면 단순이 부호없는 타입에서 128보다 큰 값이면 모두 해당시키게 만든다..
// contain_native의 일반화..

namespace i3
{
	namespace detail
	{
		struct non_ascii_pred_fun
		{
			template<class C>
			bool operator(C c) const
			{
				return unsigned(c) > unsigned(0x7F);
			}
		}

		template<class Range> inline
		int contain_non_ascii_imp( const Range& rng )
		{
			typedef typename range_iterator<Range>::type iterator;
			typedef typename get_iterator_range_type<Range>::type iterator_range_type;
			iterator it = i3::range::find_if(rng, detail::non_ascii_pred_fun() );
			return  ( it == rng.end() ) ? -1 : i3::distance(rng.begin(), it);
		}

	}


	template<class Range> inline
	int contain_non_ascii( const Range& rng )
	{
		return detail::contain_non_ascii_imp(i3::as_literal(rng));
	}


}
