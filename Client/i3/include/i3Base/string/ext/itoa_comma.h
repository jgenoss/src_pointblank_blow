#pragma once

#include "../../itl/type_traits/make_unsigned.h"
#include "../../itl/type_traits/is_signed.h"
#include "../../itl/type_traits/is_iterator.h"

#include "generic_string_copy.h"
#include "safe_string_copy.h"

// integral_to_comma_str 함수보다 정리된 버전을 따로 만듬..
// 10진수만 처리한다.  
//
namespace i3
{
	
	namespace detail_itoa_comma
	{
		struct num_to_str_unsigned
		{
			template<class Integral, class char_type> 
			static void	call( Integral val, char_type* inoutbuff, int& pos)
			{
				static const char_type zero_char_value = char_type(0x0030);
				static const int comma_sep_interval = 3;

				Integral v = val;
				int dest_idx = pos;
				int sep_count = 0;

				while( v > 0 )
				{
					if ( sep_count == comma_sep_interval )
					{
						inoutbuff[--dest_idx] = char_type(',');	sep_count = 0;
					}

					inoutbuff[--dest_idx] = char_type( (int)( v % 10) + zero_char_value );
					v /= 10;
					++sep_count;
				}
				pos = dest_idx;
			}
		};

		struct num_to_str_signed
		{
			template<class Integral, class char_type> 
			static void	call( Integral val, char_type* inoutbuff, int& pos)
			{
				static const char_type zero_char_value = char_type(0x0030);
				static const int comma_sep_interval = 3;

				Integral v = val;
				int dest_idx = pos;
				int sep_count = 0;

				if ( val < 1 )
				{
					inoutbuff[--dest_idx] = char_type( -(v % 10) + zero_char_value );
					v /= -10;
					++sep_count;
				}

				while( v > 0 )
				{
					if ( sep_count == comma_sep_interval )
					{
						inoutbuff[--dest_idx] = char_type(',');	sep_count = 0;
					}

					inoutbuff[--dest_idx] = char_type( ( v % 10) + zero_char_value );
					v /= 10;
					++sep_count;
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
		void>::type			itoa_comma( Integral val, Range& out)
	{
		static const int max_digit_buff_size = _CVTBUFSIZE;
		typedef typename i3::range_value<Range>::type  Ch;
		Ch	output_array[max_digit_buff_size];
		int	dest_idx = max_digit_buff_size;
		output_array[--dest_idx] = 0;		// 미리 널문자 삽입.

		typedef typename mpl::if_< is_signed<Integral>, 
			detail_itoa_comma::num_to_str_signed, 
			detail_itoa_comma::num_to_str_unsigned>::type dispatch_type;

		dispatch_type::call(val, output_array, dest_idx);

		i3::generic_string_copy( out, output_array + dest_idx, output_array + max_digit_buff_size - 1);
	}

	template<class Integral, class Lit> inline typename i3::enable_if<
		mpl::and_<is_integral<Integral>, 
		is_iterator<Lit> >,
		void>::type			itoa_comma( Integral val, Lit out, size_t buff_size)
	{
		static const int max_digit_buff_size = _CVTBUFSIZE;
		typedef typename i3::iterator_value<Lit>::type  Ch;

		Ch	output_array[max_digit_buff_size];
		int	dest_idx = max_digit_buff_size;
		output_array[--dest_idx] = 0;		// 미리 널문자 삽입.

		typedef typename mpl::if_< is_signed<Integral>, 
			detail_itoa_comma::num_to_str_signed, 
			detail_itoa_comma::num_to_str_unsigned>::type dispatch_type;

		dispatch_type::call(val, output_array, dest_idx);

		i3::iterator_range<const Ch*> res_rng( output_array + dest_idx, output_array + max_digit_buff_size - 1);

		i3::safe_string_copy( out, res_rng, buff_size);
	}


}