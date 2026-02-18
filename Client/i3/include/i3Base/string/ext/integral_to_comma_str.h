#pragma once

//
// i3String::ValueToStr 일반화..
//
#include "num_to_str.h"
#include "../../itl/type_traits/remove_reference.h"

namespace i3
{
	namespace detail_num_to_str
	{
		template<class Integral, class LitSeq> 
		void	integral_to_comma_str_impl(Integral val, LitSeq out)
		{
			static const int max_digit_buff_size = get_max_digit_buffer_size<Integral>::value;
			typedef typename i3::remove_reference<LitSeq>::type LitSeq2;
			typedef	typename i3::range_value<LitSeq2>::type	char_type;
		
			char_type	output_array[max_digit_buff_size];		
			int			dest_idx = max_digit_buff_size;
			output_array[--dest_idx] = 0;		// 미리 널문자 삽입.

			detail_num_to_str::process_num_to_str_if_comma(val, output_array, dest_idx);
		
			typedef typename mpl::if_<	is_signed<Integral>, 
				detail_num_to_str::add_sign_char, 
				detail_num_to_str::no_sign_char>::type 	dispatch_sign;

			dispatch_sign::negative(val, output_array, dest_idx);

			int length = max_digit_buff_size - dest_idx - 1;

			i3::generic_string_copy(out, output_array + dest_idx, output_array +dest_idx + length);
		}

		template<class Integral, class Lit> 
		size_t	integral_to_comma_str_impl(Integral val, Lit out, size_t out_size)
		{
			static const int max_digit_buff_size = get_max_digit_buffer_size<Integral>::value;
			typedef	typename i3::range_value<Lit>::type	char_type;
		
			char_type	output_array[max_digit_buff_size];		
			int			dest_idx = max_digit_buff_size;
			output_array[--dest_idx] = 0;		// 미리 널문자 삽입.

			detail_num_to_str::process_num_to_str_if_comma(val, output_array, dest_idx);
		
			typedef typename mpl::if_<	is_signed<Integral>, 
				detail_num_to_str::add_sign_char, 
				detail_num_to_str::no_sign_char>::type 	dispatch_sign;

			dispatch_sign::negative(val, output_array, dest_idx);

			int length = max_digit_buff_size - dest_idx - 1;

			// 윗 버전과의 차이는 여기 뿐이다..
			if ( length >= int(out_size) )
			{
				*out = char_type();
				return 0;
			}
			//
			i3::generic_string_copy(out, output_array + dest_idx, output_array +dest_idx + length);
			return length;
		}

	}

	template<class Integral, class Seq> inline
	typename i3::enable_if< mpl::and_< is_integral<Integral>, mpl::not_<is_iterator<Seq> > >,
	void>::type		integral_to_comma_str( Integral val, Seq& out)
	{
		detail_num_to_str::integral_to_comma_str_impl<Integral, Seq&>(val, out);
	}

	template<class Integral, class Lit> inline
	typename i3::enable_if< mpl::and_< is_integral<Integral>, is_iterator<Lit> >,
	void>::type		integral_to_comma_str( Integral val, Lit out)
	{
		detail_num_to_str::integral_to_comma_str_impl<Integral, Lit>(val, out);
	}

/////////////////////// Dest MAX값을 넘겨주는 오버로딩 버전..리터럴버전에서만 필요하기 때문에 
	// 시퀀스버전은 없다..

	template<class Integral, class Lit> inline
	typename i3::enable_if< mpl::and_< is_integral<Integral>, is_iterator<Lit> >,
	size_t>::type		integral_to_comma_str( Integral val, Lit out, size_t out_size)
	{
		return detail_num_to_str::integral_to_comma_str_impl<Integral, Lit>(val, out, out_size);
	}


}