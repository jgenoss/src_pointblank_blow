#pragma once

#include "../../../itl/range/value_type.h"
#include "../../../itl/range/iterator_range.h"
#include "../../../itl/type_traits/add_pointer.h"
#include "../generic_string_copy.h"
#include "../../../itl/type_traits/is_signed.h"

namespace i3
{
	// 옵션으로 유효자리 갯수와 소수점 자름 갯수로 고려해볼수 있다... 
	//(유효값 자체를 잘 계산하려면 유효자리 갯수가 유일한 방법이지만, 가독/직관적으로는 소수점 자름이 편하긴 하다..)
	
	//
	// 옵션때문에 함수 인수를 불필요하게 증가시키는 방법 대신에 tls_ptr을 사용한 전역 상태를 보존하도록 처리 변경..
	//
	static const int comma_sep_interval = 3;
	
	namespace detail
	{
		typedef errno_t (*fn_real_cvt)(char*, size_t, double, int, int*, int*);

		struct string_conversion_option_data
		{
			string_conversion_option_data() : 
				comma_enable(false), rts_mode(precision_mode::underpoint), 
				real_cvt_fun(&::_fcvt_s), force_fill_zero(true), precision_digit(3) {}
			fn_real_cvt				real_cvt_fun;
			int						precision_digit;
			precision_mode::type	rts_mode;
			bool					comma_enable;
			bool					force_fill_zero;
		};

		I3_EXPORT_BASE extern i3::tls_ptr<string_conversion_option_data>	g_string_conversion_option;
	}

	
	namespace str_conv_option
	{
		inline void	set_enable_comma(bool b) { detail::g_string_conversion_option->comma_enable = b; }
		inline bool	is_enable_comma() {  return detail::g_string_conversion_option->comma_enable; }
		inline precision_mode::type get_precision_mode() { return detail::g_string_conversion_option->rts_mode; }
		inline void set_precision_mode(precision_mode::type t) 
		{ 
			detail::g_string_conversion_option->rts_mode = t; 
			switch(t)
			{
			case precision_mode::significant_digit:	
				detail::g_string_conversion_option->real_cvt_fun = &::_ecvt_s; break;
			case precision_mode::underpoint:
			default:		
				detail::g_string_conversion_option->real_cvt_fun = &::_fcvt_s; break;
			}
		}

		inline bool	is_fill_zero() { return detail::g_string_conversion_option->force_fill_zero; }
		inline void	set_fill_zero(bool b) { detail::g_string_conversion_option->force_fill_zero = b; }


		inline int	get_precision() { return detail::g_string_conversion_option->precision_digit; }
		inline void	set_precision(int num) { detail::g_string_conversion_option->precision_digit = num; }
		
	}

	namespace detail_num_to_str
	{
		struct add_sign_char
		{
			template<class Integral, class char_type> inline static
				void negative(Integral v, char_type* buff, int& pos) {  if (v < 0) buff[--pos] = char_type('-');  }
		};

		struct no_sign_char
		{
			template<class Integral, class char_type> inline static void negative(Integral, char_type* , int& ) { }
		};
		
		template<class Integral, class char_type> 
		void	process_num_to_str( Integral val, char_type* inoutbuff, int& pos)
		{
			static const char_type zero_char_value = char_type(0x0030);

			Integral v = val;
			int dest_idx = pos;

#pragma warning(push)
#pragma warning(disable : 4146 4245)
			if ( val < 1 )
			{
				inoutbuff[--dest_idx] = -(v % 10) + zero_char_value;
				v /= -10;
			}
#pragma warning(pop)

			while( v > 0 )
			{
				inoutbuff[--dest_idx] = ( v % 10) + zero_char_value;
				v /= 10;
			}

			pos = dest_idx;
		}
		
		template<class Integral, class char_type> 
		void	process_num_to_str_if_comma( Integral val, char_type* inoutbuff, int& pos)
		{
			static const char_type zero_char_value = char_type(0x0030);

			Integral v = val;
			int dest_idx = pos;
			int sep_count = 0;

#pragma warning(push)
#pragma warning(disable : 4146 4245)
			if ( val < 1 )
			{
				inoutbuff[--dest_idx] = -(v % 10) + zero_char_value;
				v /= -10;
				++sep_count;
			}
#pragma warning(pop)

			while( v > 0 )
			{
				if ( sep_count == comma_sep_interval )
				{
					inoutbuff[--dest_idx] = char_type(',');	sep_count = 0;
				}

				inoutbuff[--dest_idx] = ( v % 10) + zero_char_value;
				v /= 10;
				++sep_count;
			}

			pos = dest_idx;
		}
	}


	template<class Integral, class Range> typename i3::enable_if< is_integral<Integral>,
	void>::type		integral_to_str( Integral val, Range& out)
	{
		static const int max_digit_buff_size = get_max_digit_buffer_size<Integral>::value;
		typedef	i3::range_value<Range>::type	char_type;
		
		char_type	output_array[max_digit_buff_size];		
		int			dest_idx = max_digit_buff_size;
		output_array[--dest_idx] = 0;		// 미리 널문자 삽입.

		if ( str_conv_option::is_enable_comma() )
			detail_num_to_str::process_num_to_str_if_comma(val, output_array, dest_idx);
		else
			detail_num_to_str::process_num_to_str(val, output_array, dest_idx);

		typedef typename mpl::if_<	is_signed<Integral>, 
			detail_num_to_str::add_sign_char, 
			detail_num_to_str::no_sign_char>::type 	dispatch_sign;

		dispatch_sign::negative(val, output_array, dest_idx);

		i3::generic_string_copy(out, output_array + dest_idx, output_array + max_digit_buff_size - 1);
	}


	//
	//  그냥 fcvt를 쓴다... (내부에서 ss가 쓰임)
	//
	template<class Real, class Range> typename i3::enable_if< is_floating_point<Real>,
		void>::type		real_to_str( Real val, Range& out)
	{
		// 
		int precision_num = str_conv_option::get_precision();

		typedef	i3::range_value<Range>::type	char_type;
		char_type	result_buff[_CVTBUFSIZE + 2];
		char_type* result_buff_last = result_buff;

		{
			char	fcvt_buff[ _CVTBUFSIZE ];		// _fcvt_s버전은 wchar_t버전이 없다...

			int dec_pos, sign;
			(*detail::g_string_conversion_option->real_cvt_fun)(fcvt_buff, 
				_CVTBUFSIZE, static_cast<double>(val), precision_num, &dec_pos, &sign);
			int fcvt_len = strlen(fcvt_buff);
			
			// 옵션에 따라 끝 '0' 제거처리..
			if ( !str_conv_option::is_fill_zero() )
			{	
				const int cond_value = (dec_pos > 0) ? dec_pos : 0;
				for ( ; fcvt_len > cond_value ; --fcvt_len )
				{
					if ( *(fcvt_buff+fcvt_len-1) != '0' )
						break;
				}
			}

			if (sign != 0)
				*(result_buff_last++) = char_type('-');	

			// 아래 경우엔 유효숫자를 늘린다..(셋째인수가 음수인경우 들어옴)
			if ( dec_pos > fcvt_len )	
			{
				int num_fill_zero = dec_pos - fcvt_len;
				i3::fill_n(fcvt_buff + fcvt_len, num_fill_zero, char('0'));
				fcvt_len += num_fill_zero;
			}

			if (dec_pos > 0)		// 1이상의 자릿수 존재한다면..
			{
				
				if ( str_conv_option::is_enable_comma() )
				{
					// 이 경우엔 소수점이전것은 세자리씩 끊어서 콤마 표시해야한다..
					// 첫번째 콤마 위치를 합리적으로 찾을 것..
					// dec_pos > 0인 것은 상위 조건에서 보증된다..
					int start_pos = 0;
					int num_comma = (dec_pos - 1) / comma_sep_interval;
					int comma_pos = (dec_pos - 1) % comma_sep_interval + 1;
					
					for ( ; num_comma > 0 ; --num_comma)
					{
						result_buff_last = i3::copy(	fcvt_buff + start_pos, 
											fcvt_buff + comma_pos, result_buff_last);
						*(result_buff_last++) = ',';
						start_pos = comma_pos;
						comma_pos += comma_sep_interval;
					}
					
					result_buff_last = i3::copy( fcvt_buff + start_pos, 
						fcvt_buff + dec_pos, result_buff_last);
				}
				else
				{
					result_buff_last = i3::copy(fcvt_buff, fcvt_buff + dec_pos, result_buff_last);
				}

				if ( dec_pos < fcvt_len )		// 소수점이하자리가 존재하면 이쪽 처리..
				{
					*(result_buff_last++) = char_type('.');
					result_buff_last = i3::copy(fcvt_buff + dec_pos, fcvt_buff + fcvt_len, result_buff_last);
				}
			}
			else
			{
				*(result_buff_last++) = char_type('0');	// 1보다 작은 경우 0을 우선찍음..

				if (fcvt_len > 0 )
				{
					*(result_buff_last++) = char_type('.');
					int num_fill_zero = -dec_pos;
					i3::fill_n(result_buff_last, num_fill_zero, char_type('0'));
					result_buff_last += num_fill_zero;
					result_buff_last = i3::copy(fcvt_buff, fcvt_buff + fcvt_len, result_buff_last);
				}
				else if ( precision_num > 0 )	// fcvt_len == 0인 경우, 찍어야할 유효숫자가 더 없음을 의미..
				{									// 자릿수를 강제로 찍으려면 precision_num수를 그대로 적용..	
					if ( str_conv_option::is_fill_zero() )
					{
						*(result_buff_last++) = char_type('.');
						i3::fill_n(result_buff_last, precision_num, char_type('0'));
						result_buff_last += precision_num;
					}
				}
			
			}

		}

		//
		i3::generic_string_copy(out, result_buff, result_buff_last);
		//
	}




}