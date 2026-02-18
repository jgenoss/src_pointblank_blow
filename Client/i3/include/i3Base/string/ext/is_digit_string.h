#pragma once

#include "../../itl/range/as_literal.h"
#include "../../itl/range/algorithm/find_if.h"
#include "../../itl/range/value_type.h"

namespace i3
{
	namespace detail
	{
		template<class Range>
		struct is_not_digit_string_functor : 
			i3::unary_function<typename range_value<Range>::type, bool>
		{
			typedef typename range_value<Range>::type	 value_type;
		
			bool operator()( value_type c) const
			{
				return ! ( isdigit(c) 
							|| c == value_type('.') 
							|| c== value_type('+')
							|| c == value_type(':') 
							|| c == value_type('-') );
			}
		};
	}

	template<class Range> inline
	bool is_digit_string( const Range& input)
	{
		typedef typename get_iterator_range_type<const Range>::type iter_range_type;
		typedef typename iter_range_type::iterator iterator;
		iter_range_type rng = as_literal(input);
		iterator it = i3::range::find_if(rng, 
				detail::is_not_digit_string_functor<iter_range_type>());
		return it == rng.end();
	}


}