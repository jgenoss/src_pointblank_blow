#pragma once

#include "../../itl/undef_minmax_begin.h"
#include "../../itl/range/iterator.h"
#include "../../itl/range/value_type.h"
#include "../../itl/range/as_literal.h"
#include "is_white_space.h"
#include "char_to_digit.h"

namespace i3
{
	
	namespace detail
	{

		enum strtol_flag
		{
			strtol_flag_neg = 1,
			strtol_flag_overflow = 2,
		};

		template<class Range>  
		long strtol_impl(const Range& rng, typename i3::range_const_iterator<Range>::type* pout_it, int base)
		{
			typedef typename i3::range_value<Range>::type	value_type;
			typedef typename i3::range_const_iterator<Range>::type const_iterator;

			const_iterator	nptr	= i3::const_begin(rng);
			const_iterator	end		= i3::const_end(rng);
			int flags = 0;
			
			if (pout_it != 0)
				*pout_it = nptr;
			
			const_iterator	p = nptr;
		
			if ( p == end ) 
				return 0;
		
			value_type		c = *p;
		
			while ( is_white_space(c) )
			{
				if (++p == end) return 0;
				c = *p;
			}

			if ( c == value_type('-') ) 
			{
				if ( ++p == end) return 0;
				flags |= strtol_flag_neg;		c = *p;
			}
			else if ( c == value_type('+'))
			{
				if ( ++p == end) return 0;
				c = *p;
			}
			
			if ( base == 0)
			{
				if ( c != value_type('0') )
					base = 10;
				else 
				{
					if (++p == end)
					{
						if (pout_it) *pout_it = p; 
						return 0;
					}

					c = *p;

					if ( c == value_type('x') || c == value_type('X'))
					{
						base = 16;
						if ( ++p == end) return 0;
						
						c = *p;
					}
					else
					{
						base = 8;
					}
				}
			}
			else
			if (base == 16)
			{
				if (  c == value_type('0') )
				{
					if (++p == end)
					{
						if (pout_it) *pout_it = p; 
						return 0;
					}
					
					c = *p;

					if (  c == value_type('x') || c == value_type('X') ) 
					{
						if ( ++p == end) return 0;
						c = *p;
					}
				}
			}
			
			long	maxval = std::numeric_limits<long>::max() / base;

			long	digval;
			long	number = 0;

			for (;;)
			{
				digval = char_to_digit(c);
					
				if ( digval == -1)
				{
					enum { ALPHABET_INTERVAL = 'a' - 'A', };

					value_type up;
					if ( value_type('a') <= c && c <= value_type('z') )
						up = value_type(c - ALPHABET_INTERVAL );
					else if ( value_type('A') <= c && c <= value_type('Z') )
						up = c;
					else
						break;

					digval = up - value_type('A') + 10;
				}

				if ( digval >= (long)base) 
					break;

				if (number < maxval)
				{
					number = number * base + digval;		// 이게 기본루틴..
				}
				else
				{
					flags |= strtol_flag_overflow;
					if ( pout_it == nullptr)
						break;
				}
				
				if (++p == end)
					break;
				c = *p;
			}
						
			if ( flags & strtol_flag_overflow )
			{
				if ( flags & strtol_flag_neg)
					number = std::numeric_limits<long>::min();
				else
					number = std::numeric_limits<long>::max();
			}
			else
			{
				if (flags & strtol_flag_neg)
					number = -number;
			}
			
			if (pout_it != 0)
				*pout_it = p;

			return number;
		}
	}
	
	template<class Range> inline 
	long strtol(const Range& rng, typename i3::range_const_iterator<Range>::type& out_it, int base)
	{
		return detail::strtol_impl(i3::as_literal(rng), &out_it, base);
	}
	
	template<class Range> inline 
	long strtol(const Range& rng, int base)
	{
		return detail::strtol_impl(i3::as_literal(rng), nullptr, base);
	}
	
	template<class Range> inline 
	long strtol(const Range& rng)
	{
		return strtol(rng, 10);
	}
	
}

#include "../../itl/undef_minmax_end.h"
