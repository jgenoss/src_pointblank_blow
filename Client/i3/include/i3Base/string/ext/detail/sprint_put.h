#pragma once

#include "sprint_flags.h"
#include "../../../itl/type_traits/is_signed.h"
#include "../../../itl/type_traits/remove_reference.h"
#include "../../../itl/type_traits/is_template_same.h"
#include "../../../itl/type_traits/is_template_base_of.h"

namespace i3
{
	namespace detail_sprintf
	{
		I3_EXPORT_BASE size_t	double_to_fformat(double val, int precision, bool force_point, char* out, int* sign);
		I3_EXPORT_BASE size_t	double_to_eformat(double val, int precision, bool force_point, bool cap, char* out, int* sign);
		I3_EXPORT_BASE size_t	double_to_gformat(double val, int precision, bool force_point, bool cap, bool del_zero, char* out, int* sign);
		
		// enable_if처리필요..
		struct string_fmt_tag {};
		struct integral_fmt_tag {};
		struct floating_point_fmt_tag {};
		struct error_fmt_tag {};
		struct pointer_fmt_tag {};

		template<class T>
		struct find_fmt_tag
		{
			typedef typename mpl::multi_if< 
				mpl::or_<	i3::is_template_same<i3::basic_string<mpl_,mpl_,mpl_>, T>,
							i3::is_template_same<i3::stack_basic_string<mpl_,mpl_,mpl_,mpl_>, T>,
							i3::is_template_same<i3::rc_basic_string<mpl_,mpl_,mpl_>, T>,
							i3::is_template_same<i3::fixed_basic_string<mpl_,mpl_,mpl_>, T>,
							i3::is_template_base_of<i3::iterator_range<mpl_>, T>,
							i3::is_template_base_of<i3::literal_base_range<mpl_>, T>,
							i3::is_convertible<T, const char*>,
							i3::is_convertible<T, const wchar_t*> >, string_fmt_tag,
							i3::is_integral<T>, integral_fmt_tag,
							i3::is_enum<T>, integral_fmt_tag,
							i3::is_floating_point<T>, floating_point_fmt_tag,
							i3::is_pointer<T>, pointer_fmt_tag,
							error_fmt_tag>::type			type;
		};
		
		template<class T, class Else> 
		struct convert_type_for_fmt 
		{
			typedef typename mpl::multi_if< i3::is_enum<T>, int,
											mpl::or_< is_same<T, long long>, is_same<T, unsigned long long> >, Else,
											i3::is_signed<T>, int,
											i3::is_unsigned<T>, unsigned int,
											i3::is_convertible<T, char*>, const char*,
											i3::is_convertible<T, wchar_t*>, const wchar_t*,
											Else>::type				type;
		};
		
		template<class T, class ItemType, class SString> inline
		void put( T x, ItemType* item, SString& temp_storage)
		{
			typedef typename remove_const<typename remove_reference<T>::type>::type	non_cv;
			typedef typename find_fmt_tag<non_cv>::type			tag;
			typedef typename convert_type_for_fmt<non_cv, T>::type  converted_type;
			put_fmt<converted_type, ItemType, SString>(x, item, temp_storage, tag());
		}
		
		template<class T, class ItemType, class SString>
		void put_fmt( T x, ItemType* item, SString& temp_storage, string_fmt_tag )
		{
			typedef typename remove_reference<T>::type	non_ref;
			typedef typename i3::get_const_iterator_range_type<non_ref>::type	src_range_type;
			typedef typename i3::range_value<src_range_type>::type				src_value_type;

			src_range_type r = i3::as_literal(x);
			int orig_size = int(r.size());
			if (item->m_precision != -1 )
			{
				if ( item->m_precision < orig_size )
				{
					r.advance_end(item->m_precision - orig_size);
				}
			}
			typedef typename SString::value_type			dest_value_type;

			typedef typename get_conv_tag<src_value_type, dest_value_type>::type	conv_tag;
			process_convert_str_to_fmt(r, item, temp_storage, conv_tag());
		}
		
		
	// 
		// 문자열처리.. // 포맷인수와 매개인수의 문자원소타입이 다를때는 다국어 처리가 요구될수 있어서 주의해야한다..
		//
		struct no_str_conv_needed_tag {};
		struct mb_to_wc_conv_needed_tag {};
		struct wc_to_mb_conv_needed_tag {};
		
		template<class SrcVal, class DestVal>
		struct get_conv_tag
		{
			static const std::size_t SrcSize = sizeof(SrcVal);
			static const std::size_t DestSize = sizeof(DestVal);

			typedef typename mpl::if_c< (SrcSize == DestSize), 
										no_str_conv_needed_tag, 
										typename mpl::if_c< (SrcSize == 1 && DestSize == 2), 
															mb_to_wc_conv_needed_tag, 
															typename mpl::if_c< (SrcSize == 2 && DestSize == 1), 
																				wc_to_mb_conv_needed_tag, 
																				no_str_conv_needed_tag
																				>::type 
															>::type
										>::type			type;			

		};

		template<class SrcIt, class ItemType, class SString> inline
		void process_convert_str_to_fmt(
			const i3::iterator_range<SrcIt>& src, ItemType* item, SString& temp_storage, no_str_conv_needed_tag)
		{
			typedef typename SString::value_type							Ch;
			typedef i3::iterator_range<const Ch*>							result_range_type;
			put_result(src, result_range_type(nullptr, nullptr), item->m_flags, item, temp_storage);
		}
		
		template<class SrcIt, class ItemType, class SString>
		void process_convert_str_to_fmt(
			const i3::iterator_range<SrcIt>& src, ItemType* item, SString& temp_storage, mb_to_wc_conv_needed_tag)
		{
			i3::stack_wstring conv_s;	i3::mb_to_utf16(src, conv_s);
			typedef typename SString::value_type							Ch;
			typedef i3::iterator_range<const Ch*>							result_range_type;
			
		//	std::size_t tmp_start = temp_storage.size();
			const std::size_t conv_len = conv_s.size();
			temp_storage.append( conv_s.c_str(), conv_len );		

			result_range_type res = result_range_type( temp_storage.prev_pos(conv_len), temp_storage.curr_pos()); // &temp_storage[tmp_start], &temp_storage[tmp_start + conv_len] );
			put_result(res, result_range_type(nullptr, nullptr), item->m_flags, item, temp_storage);
		}
		
		template<class SrcIt, class ItemType, class SString>
		void process_convert_str_to_fmt(
			const i3::iterator_range<SrcIt>& src, ItemType* item, SString& temp_storage, wc_to_mb_conv_needed_tag)
		{
			i3::stack_string conv_s;	i3::utf16_to_mb(src, conv_s);
			typedef typename SString::value_type			Ch;
			typedef i3::iterator_range<const Ch*>			result_range_type;
			
		//	std::size_t tmp_start = temp_storage.size();
			const std::size_t conv_len = conv_s.size();
			temp_storage.append( conv_s.c_str(), conv_len );		

			result_range_type res = result_range_type( temp_storage.prev_pos(conv_len), temp_storage.curr_pos());	// &temp_storage[tmp_start], &temp_storage[tmp_start + conv_len] );
			put_result(res, result_range_type(nullptr, nullptr), item->m_flags, item, temp_storage);
		}
				
		
		struct unsigned_tag {};
		struct signed_tag {};

		//
		// 실수형의 경우 대부분 double로 처리한다..		(CRT내부 함수 의존인데, double과 char만 쓰인다..)
		//
		template<class T, class ItemType, class SString> 
		void put_fmt( T x, ItemType* item, SString& temp_storage, floating_point_fmt_tag )
		{
			
			if ( item->m_flags & SPRINTF_FL_INTEGRAL )		// 정수형으로 강제변환후 그곳에서 처리하도록 한다..
			{
				if (item->m_flags & SPRINTF_FL_FORCEUNSIGNED)
					put_fmt<unsigned int, ItemType, SString>(unsigned int(x), item, temp_storage, integral_fmt_tag());
				else
					put_fmt<int, ItemType, SString>(int(x), item, temp_storage, integral_fmt_tag());

				return;
			}

			// 감춰진 함수.. _cfltcvt_l()을 호출해야하는데, 이것은 CRT내부함수로, crt_dll의 경우엔 도저이 쓸수 없다.
			// 그러므로, 따로 제작한 함수로 대신해서 처리한다..
			// 별도 제작함수는 crt의 ecvt/fcvt함수가 쓰이며 모두 char타입 전용이다...
			// wchar_t버전의 경우 char배열로 결과를 내고, 카피한다..

			double val = x;
			int flags = item->m_flags;
			int	precision = (item->m_precision > -1 ) ? item->m_precision : 6;
			bool forcepoint = ( flags & SPRINTF_FL_ALTERNATE ) != 0;

			int sign;

			char   buff[_CVTBUFSIZE];		// 고정..
			size_t buff_writes = 0;

			if (flags & SPRINTF_FL_SCI)
			{
				buff_writes = double_to_eformat(val, precision, forcepoint, (flags & SPRINTF_FL_UPPER) != 0, 
												buff, &sign);
			}
			else if (flags & SPRINTF_FL_GFLOAT)
			{
				buff_writes = double_to_gformat(val, precision, forcepoint, (flags & SPRINTF_FL_UPPER) != 0, 
												!(flags & SPRINTF_FL_ALTERNATE), buff, &sign);
			}
			else
			{
				buff_writes = double_to_fformat(val, precision, forcepoint, buff, &sign);
			}
		
			if ( sign != 0 )		// 음수는 나중에 처리해야한다..
				flags |= SPRINTF_FL_NEGATIVE;	
						
			typedef typename SString::value_type							Ch;
			typedef i3::iterator_range<const Ch*>							range_type;

		//	std::size_t tmp_start = temp_storage.size();
			temp_storage.append( buff, buff + buff_writes);		// 반복자 구간으로 복사..

			range_type res = range_type( temp_storage.prev_pos(buff_writes), temp_storage.curr_pos());	// &temp_storage[tmp_start], &temp_storage[tmp_start + buff_writes] );
			
			range_type prefix;
			prefix_process(prefix, temp_storage, flags, signed_tag() );
			
			put_result(res, prefix, flags, item, temp_storage);

		}
		
		template<class It, class SString> 
		void prefix_process( i3::iterator_range<It>& prefix, SString& temp_storage, int flags, signed_tag)
		{
			typedef typename SString::value_type						Ch;
			typedef i3::iterator_range<It>								range_type;
			
			if (flags & SPRINTF_FL_NEGATIVE )
			{
			//	size_t tmp_start = temp_storage.size();
				temp_storage.append(Ch('-'));
				prefix = range_type( temp_storage.prev_pos(1), temp_storage.curr_pos());  // &temp_storage[tmp_start] , &temp_storage[tmp_start+1] );
			}
			else 
			if ( flags & SPRINTF_FL_SIGN )
			{
			//	size_t tmp_start = temp_storage.size();
				temp_storage.append(Ch('+'));
				prefix = range_type( temp_storage.prev_pos(1), temp_storage.curr_pos());  // &temp_storage[tmp_start] , &temp_storage[tmp_start+1] );
			}
			else
			if ( flags & SPRINTF_FL_SIGNSP)
			{
			//	size_t tmp_start = temp_storage.size();
				temp_storage.append(Ch(' '));
				prefix = range_type( temp_storage.prev_pos(1), temp_storage.curr_pos());  // &temp_storage[tmp_start] , &temp_storage[tmp_start+1] );
			}
			else
			{
				prefix = range_type(nullptr, nullptr);
			}
		}		

		template<class It, class SString> 
		void prefix_process( i3::iterator_range<It>& prefix, SString& temp_storage, int flags, unsigned_tag) 
		{ 
			typedef typename SString::value_type						Ch;
			typedef i3::iterator_range<It>								range_type;
			
			if ( (flags & SPRINTF_FL_ALTERNATE) && (flags & SPRINTF_FL_HEX) )
			{
			//	size_t tmp_start = temp_storage.size();
				temp_storage.append(Ch('0'));
				temp_storage.append( (flags & SPRINTF_FL_LOWER) ? Ch('x') : Ch('X') );
				prefix = range_type( temp_storage.prev_pos(2), temp_storage.curr_pos());	// &temp_storage[tmp_start] , &temp_storage[tmp_start+2] );
			}
			else
			{
				prefix = range_type(nullptr, nullptr);
			}
		}
		
		template<class T, class ItemType, class SString> 
		void put_fmt( T x, ItemType* item, SString& temp_storage, integral_fmt_tag )
		{
			if (item->m_flags & SPRINTF_FL_SINGLECHAR )		// 1개짜리 문자열로 취급해야한다..
			{
				typedef typename remove_const<typename remove_reference<T>::type>::type	non_cv;
				typedef typename SString::value_type							Ch;
				typedef i3::iterator_range<const Ch*>							range_type;

				static const non_cv mask = ( sizeof(Ch) == 1 ) ? 0xFF : 0xFFFF;
			//	std::size_t tmp_start = temp_storage.size();
				temp_storage.append( Ch( x & mask ) );
				range_type r = range_type( temp_storage.prev_pos(1), temp_storage.curr_pos());	//  &temp_storage[tmp_start] , &temp_storage[tmp_start+1] );
				put_fmt<const range_type&, ItemType, SString>(r, item, temp_storage, string_fmt_tag());
				return;
			}

			typedef typename remove_const<typename remove_reference<T>::type>::type	non_cv;
			typedef typename mpl::if_< is_signed<non_cv>, signed_tag, unsigned_tag>::type sign_tag;
			put_fmt_integral<T, ItemType, SString>(x, item, temp_storage, sign_tag() );
		}

		template<class T, class ItemType, class SString> inline
		void put_fmt_integral( T x, ItemType* item, SString& temp_storage, unsigned_tag tag)
		{
			put_fmt_integral_impl<T, ItemType, SString>(x, item, item->m_flags, item->m_precision, temp_storage, tag);
		}

		template<class T, class ItemType, class SString> 
		void put_fmt_integral( T x, ItemType* item, SString& temp_storage, signed_tag tag)
		{
			if (item->m_flags & SPRINTF_FL_FORCEUNSIGNED)
			{
				static const bool is_ref = i3::is_reference<T>::value;
				static const bool is_con = i3::is_const< typename remove_reference<T>::type>::value;

				typedef typename remove_const<typename remove_reference<T>::type>::type	non_cv;
				typedef typename i3::make_unsigned<non_cv>::type						us_non_cv;
				typedef typename mpl::eval_if_c<is_con, i3::add_const<us_non_cv>, i3::identity<us_non_cv> >::type us_cv;
				typedef typename mpl::eval_if_c<is_ref, i3::add_reference<us_cv>, i3::identity<us_cv> >::type		us_cv_ref;
				put_fmt_integral_impl<us_cv_ref, ItemType, SString>( us_cv_ref(x), item, item->m_flags, item->m_precision, temp_storage, unsigned_tag() );
			}
			else
			{
				put_fmt_integral_impl<T, ItemType, SString>(x, item, item->m_flags, item->m_precision, temp_storage, tag );
			}
		}

		template<class T, class ItemType, class SString, class SignTag>
		void put_fmt_integral_impl( T x, ItemType* item, int flags, int precision, SString& temp_storage, SignTag tag)
		{
		//
		// 정수형인데, 좀더 과격한 특화가 필요할수 있다..(precision고려하고, temp_storage에 append처리 별도로 해야함..)	
		// sprintf의 정수precision은 정밀도라기보다는 0채우기 의미가 된다...(그렇기 때문에, LEADZERO플래그가 제거될수 있다.)
		//
			typedef typename remove_const<typename remove_reference<T>::type>::type	non_cv;
		
			if (precision < 0)
				precision = 1;
			else
			{
				flags &=~SPRINTF_FL_LEADZERO;		// 제거..
				if ( precision > std::numeric_limits<non_cv>::digits10 )
					precision = std::numeric_limits<non_cv>::digits10;
			}
			
			typedef typename SString::value_type	Ch;
			static const std::size_t temp_size = std::numeric_limits<non_cv>::digits10 + 2;
			Ch	temp[ temp_size ];					// 이건 그냥 배열이 좋다..(특히, 정수형은 작게 유지된다..)
			Ch* out_temp_start;
			std::size_t out_len;
			
			integral_process(x, flags, precision, temp, out_temp_start, out_len, tag);
					
			typedef i3::iterator_range<const Ch*>	range_type;
			
		//	std::size_t tmp_start = temp_storage.size();
			temp_storage.append( out_temp_start, out_len );
			range_type res = range_type( temp_storage.prev_pos(out_len), temp_storage.curr_pos());	// &temp_storage[tmp_start], &temp_storage[tmp_start + out_len] );
						
			range_type prefix;
			prefix_process(prefix, temp_storage, flags, tag);
			put_result(res, prefix, flags, item, temp_storage);
		}
				

		template<class T, class Ch, std::size_t temp_size> 
		void integral_process(T x, int& flags, int precision, Ch (&temp)[temp_size], Ch*& out_start, std::size_t& out_len, signed_tag)
		{
			if ( x < 0 )
			{
				x = -x;
				flags |= SPRINTF_FL_NEGATIVE;
			}

			Ch* temp_pos = &temp[temp_size - 1];
		
			int digit;			
			
			while ( precision-- > 0 || x != 0 )
			{
				digit = (int)(x % 10 ) + Ch('0');
				x /= 10;
				*temp_pos-- = Ch(digit);
			}
			
			++temp_pos;

			std::size_t conv_len = std::size_t( &temp[temp_size] - temp_pos);
						
			out_start = temp_pos;
			out_len   = conv_len;

		}

		template<class T, class Ch, std::size_t temp_size> 
		void integral_process(T x, int flags, int precision, Ch (&temp)[temp_size], Ch*& out_start, std::size_t& out_len, unsigned_tag) 
		{ 
			Ch* temp_pos = &temp[temp_size - 1];

			T radix = 10;			// 디폴트 10..
			int hexadd = 0;

			if (flags  & (SPRINTF_FL_HEX | SPRINTF_FL_OCT) )
			{
				if ( flags & SPRINTF_FL_OCT )
				{
					radix = 8;
				}
				else 
				{
					radix = 16; 
					hexadd = ( flags & SPRINTF_FL_LOWER )	?  ('a') - ('9') - 1 
															:  ('A') - ('9') - 1;
				}
			}		
				
			int digit;			
			
			while ( precision-- > 0 || x != 0 )
			{
				digit = (int)(x % radix ) + Ch('0');
				x /= radix;
				if ( digit > Ch('9') ) digit += hexadd;
				*temp_pos-- = Ch(digit);
			}
			
			++temp_pos;

			std::size_t conv_len = std::size_t( &temp[temp_size] - temp_pos);
						
			if ( flags & SPRINTF_FL_FORCEOCTAL)
			{
				if (conv_len == 0 || temp_pos[0] != Ch('0') )
				{
					*--temp_pos = Ch('0');
					++conv_len;
				}
			}
			
			out_start = temp_pos;
			out_len   = conv_len;
		}

		template<class T, class ItemType, class SString> inline
		void put_fmt(T x, ItemType* item, SString& temp_storage, pointer_fmt_tag)
		{
			// 문자열 아닌 포인터의 경우, %p형식을 강제하도록 조정한다..
			int flags = item->m_flags|SPRINTF_FL_INTEGRAL|SPRINTF_FL_HEX|SPRINTF_FL_UPPER|SPRINTF_FL_FORCEUNSIGNED;
			int precision = sizeof(void*)*2;		// 소수점 정밀도를 무시하고 강제로 8 (32비트)로 정한다.
			put_fmt_integral_impl<uintptr_t, ItemType, SString>(uintptr_t(x), item, flags, precision, temp_storage, unsigned_tag());	
		}

		template<class It, class ItemType, class SString>
		void put_result( const i3::iterator_range<It>& res, const i3::iterator_range<It>& prefix, int flags, ItemType* item, SString& temp_storage)
		{		
			typedef typename SString::value_type	Ch;
			typedef i3::iterator_range<It>	range_type;

			int padding  = item->m_width - int(res.size() ) - int(prefix.size());
			
			if ( padding > 0 )
			{
			//	size_t tmp_start = temp_storage.size();

				if (flags & SPRINTF_FL_LEFT)
				{
					temp_storage.append(padding,  Ch(' ') );

					item->m_conv_range1 = prefix;
					item->m_conv_range2 = res;
					item->m_conv_range3 = range_type(temp_storage.prev_pos(padding), temp_storage.curr_pos());	//	&temp_storage[tmp_start], &temp_storage[tmp_start+padding]);

				}
				else
				{
					if ( flags & SPRINTF_FL_LEADZERO )
					{
						temp_storage.append(padding, Ch('0') );
						item->m_conv_range1 = prefix;
						item->m_conv_range2 = range_type(temp_storage.prev_pos(padding), temp_storage.curr_pos());	//	&temp_storage[tmp_start], &temp_storage[tmp_start+padding]);
						item->m_conv_range3 = res;
					}
					else
					{
						temp_storage.append(padding, Ch(' ') );
						item->m_conv_range1 = range_type(temp_storage.prev_pos(padding), temp_storage.curr_pos()); // &temp_storage[tmp_start], &temp_storage[tmp_start+padding]);
						item->m_conv_range2 = prefix;
						item->m_conv_range3 = res;
					}
				}
			}
			else
			{
				item->m_conv_range1 = prefix;
				item->m_conv_range2 = res;
				item->m_conv_range3 = range_type(nullptr, nullptr);
			}
		}

	}
}