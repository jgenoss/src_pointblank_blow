#pragma once

#include "../../itl/type_traits/make_unsigned.h"
#include "../../itl/type_traits/is_signed.h"
#include "../../itl/type_traits/is_iterator.h"

#include "generic_string_copy.h"
#include "safe_string_copy.h"

namespace i3
{
	// 양, 음 부호는 오로지 10진수에만 쓰이고, 그이외 모든 진수는 양수로 간주해야한다.

	namespace detail_itoa
	{
		struct num_to_str_unsigned
		{
			template<class Integral, class char_type> 
			static void	call( Integral val, int radix, char_type* inoutbuff, int& pos)
			{
				static const char_type zero_char_value = char_type(0x0030);
				Integral v = val;
				int dest_idx = pos;
				int hexadd = (radix > 10) ?  ('a') - ('9') - 1 : 0;		//여기서는 10보다큰 진수라면 소문자 알파벳을 사용한다.

				int digit;
				while( v > 0 )
				{
					digit = (int)( v % radix) + zero_char_value;
					if (digit > char_type('9') ) digit += hexadd;
					inoutbuff[--dest_idx] = char_type(digit);
					v /= radix;
				}
				pos = dest_idx;
			}
			
			template<class Integral, class char_type> 
			static void	call( Integral val, char_type* inoutbuff, int& pos)
			{
				static const char_type zero_char_value = char_type(0x0030);
				Integral v = val;
				int dest_idx = pos;
			
				while( v > 0 )
				{
					inoutbuff[--dest_idx] = char_type( (int)( v % 10) + zero_char_value );
					v /= 10;
				}
				pos = dest_idx;
			}

		};
		
		struct num_to_str_signed
		{
			template<class Integral, class char_type> 
			static void	call( Integral val, int radix, char_type* inoutbuff, int& pos)
			{
				if ( radix != 10 )
				{
					typedef typename i3::make_unsigned<Integral>::type  unsigned_type;
					num_to_str_unsigned::call(static_cast<unsigned_type>(val), radix, inoutbuff, pos);
					return;
				}
				call(val, inoutbuff, pos);
			}

			template<class Integral, class char_type> 
			static void	call( Integral val, char_type* inoutbuff, int& pos)
			{
				static const char_type zero_char_value = char_type(0x0030);
				Integral v = val;
				int dest_idx = pos;
				if ( val < 1 )
				{
					inoutbuff[--dest_idx] = char_type( -(v % 10) + zero_char_value );
					v /= -10;
				}

				while( v > 0 )
				{
					inoutbuff[--dest_idx] = char_type( ( v % 10) + zero_char_value );
					v /= 10;
				}

				if (val < 0)
					inoutbuff[--dest_idx] = char_type('-');

				pos = dest_idx;
			}
		};
		
	}
	
	template<class Integral, class Range> inline typename i3::enable_if<
														mpl::and_<is_integral<Integral>, 
														mpl::not_<is_iterator<Range> > >,
	void>::type			itoa( Integral val, Range& out)
	{
		static const int max_digit_buff_size = _CVTBUFSIZE;
		typedef typename i3::range_value<Range>::type  Ch;
		Ch	output_array[max_digit_buff_size];
		int	dest_idx = max_digit_buff_size;
		output_array[--dest_idx] = 0;		// 미리 널문자 삽입.
		
		typedef typename mpl::if_< is_signed<Integral>, 
									detail_itoa::num_to_str_signed, 
									detail_itoa::num_to_str_unsigned>::type dispatch_type;

		dispatch_type::call(val, output_array, dest_idx);
		
		i3::generic_string_copy( out, output_array + dest_idx, output_array + max_digit_buff_size - 1);
	}
		
	template<class Integral, class Range> inline typename i3::enable_if<
		mpl::and_<is_integral<Integral>, 
		mpl::not_<is_iterator<Range> > >,
		void>::type		itoa( Integral val, int radix, Range& out)
	{
		static const int max_digit_buff_size = _CVTBUFSIZE;
		typedef typename i3::range_value<Range>::type  Ch;
		Ch	output_array[max_digit_buff_size];
		int	dest_idx = max_digit_buff_size;
		output_array[--dest_idx] = 0;		// 미리 널문자 삽입.

		typedef typename mpl::if_< is_signed<Integral>, 
			detail_itoa::num_to_str_signed, 
			detail_itoa::num_to_str_unsigned>::type dispatch_type;

		dispatch_type::call(val, radix, output_array, dest_idx);

		i3::generic_string_copy( out, output_array + dest_idx, output_array + max_digit_buff_size - 1);
	}

		
	template<class Integral, class Lit> inline typename i3::enable_if<
		mpl::and_<is_integral<Integral>, 
		is_iterator<Lit> >,
		void>::type			itoa( Integral val, Lit out, size_t buff_size)
	{
		static const int max_digit_buff_size = _CVTBUFSIZE;
		typedef typename i3::iterator_value<Lit>::type  Ch;

		Ch	output_array[max_digit_buff_size];
		int	dest_idx = max_digit_buff_size;
		output_array[--dest_idx] = 0;		// 미리 널문자 삽입.

		typedef typename mpl::if_< is_signed<Integral>, 
			detail_itoa::num_to_str_signed, 
			detail_itoa::num_to_str_unsigned>::type dispatch_type;

		dispatch_type::call(val, output_array, dest_idx);
		
		i3::iterator_range<const Ch*> res_rng( output_array + dest_idx, output_array + max_digit_buff_size - 1);

		i3::safe_string_copy( out, res_rng, buff_size);
	}

	template<class Integral, class Lit> inline typename i3::enable_if<
		mpl::and_<is_integral<Integral>, 
		is_iterator<Lit> >,
		void>::type		itoa( Integral val, int radix, Lit out, size_t buff_size)
	{
		static const int max_digit_buff_size = _CVTBUFSIZE;
		typedef typename i3::iterator_value<Lit>::type  Ch;

		Ch	output_array[max_digit_buff_size];
		int	dest_idx = max_digit_buff_size;
		output_array[--dest_idx] = 0;		// 미리 널문자 삽입.

		typedef typename mpl::if_< is_signed<Integral>, 
			detail_itoa::num_to_str_signed, 
			detail_itoa::num_to_str_unsigned>::type dispatch_type;

		dispatch_type::call(val, radix, output_array, dest_idx);

		i3::iterator_range<const Ch*> res_rng( output_array + dest_idx, output_array + max_digit_buff_size - 1);

		i3::safe_string_copy( out, res_rng, buff_size);
	}

	
}