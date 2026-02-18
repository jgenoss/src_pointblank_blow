#pragma once

#include "hash_fwd.h"
#include "detail/static_log2.h"
#include "functional.h"
#include <typeinfo>
#include <math.h>
#include <string>
#include <limits>

#include "type_traits/is_integral.h"
#include "type_traits/call_traits.h"
#include "enable_if.h"
#include "../string/algorithm/detail/case_conv_fast_functor.h"
//#include "type_traits/is_scalar.h"

namespace i3
{

	template <class T> inline
	void hash_combine(std::size_t& seed, const T& v)
	{
		seed ^= hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed>>2);
	}

	template <class It> inline
	std::size_t hash_range(It first, It last)
	{
		std::size_t seed = 0; 
		for ( ; first != last ; ++first)
			hash_combine(seed, *first);
		return seed;
	}

	template<class It> 
	void hash_range(std::size_t& out, It f, It l)
	{
		for ( ; f != l ; ++f)
			hash_combine(out, *f);	
	}

	template<class T1, class T2>
	std::size_t hash_value(const std::pair<T1,T2>& v) 
	{
		std::size_t seed = 0; 
		hash_combine(seed, v.first);
		hash_combine(seed, v.second);
		return seed;		
	}
	
	template<class, class> struct pair;
	
	template<class T1, class T2>
	std::size_t hash_value(const i3::pair<T1,T2>& v) 
	{
		std::size_t seed = 0; 
		hash_combine(seed, v.first);
		hash_combine(seed, v.second);
		return seed;		
	}

	template< class T, unsigned N > inline 
	std::size_t hash_value(const T (&x)[N])
	{
		return hash_range(x, x + N);
	}

	template< class T, unsigned N >	inline 
	std::size_t hash_value(T (&x)[N])
	{
		return hash_range(x, x + N);
	}

	template<class T> __forceinline
	std::size_t hash_value(T* v)
	{
		std::size_t x = static_cast<std::size_t>( reinterpret_cast<std::ptrdiff_t>(v) );
		return x + (x >> 3);		
	}


	namespace detail
	{
		inline float		call_ldexp(float v, int exp){	return ldexpf(v, exp);	}
		inline double		call_ldexp(double v, int exp) { return ldexp(v, exp); }
		inline long double	call_ldexp(long double v, int exp) { return ldexpl(v, exp); }
		
		
		inline float		call_frexp(float v, int* exp) {	return frexpf(v, exp);	}
		inline double		call_frexp(double v, int* exp) { return frexp(v, exp); }
		inline long double	call_frexp(long double v, int* exp) { return frexpl(v, exp); }

		inline void			hash_float_combine(std::size_t& seed, std::size_t value)
		{
			seed ^= value + (seed<<6) + (seed>>2);
		}

		template <class T> inline 
		std::size_t	float_hash_value( T v)
		{
			if ( v == 0 ) return 0;

			int exp = 0;

			v = call_frexp(v, &exp);

			// A postive value is easier to hash, so combine the sign with the exponent.
			if(v < 0) 
			{
				v = -v;		exp += std::numeric_limits<T>::max_exponent - std::numeric_limits<T>::min_exponent;
			}

			// The result of frexp is always between 0.5 and 1, so its
			// top bit will always be 1. Subtract by 0.5 to remove that.
			v -= T(0.5);
			v =  call_ldexp(v, std::numeric_limits<std::size_t>::digits + 1);

			std::size_t seed = static_cast<std::size_t>(v);
			v -= seed;

			// ceiling(digits(T) * log2(radix(T))/ digits(size_t)) - 1;
			std::size_t const length = (std::numeric_limits<T>::digits * static_log2<std::numeric_limits<T>::radix>::value - 1)
				/ std::numeric_limits<std::size_t>::digits;

			for(std::size_t i = 0; i != length; ++i)
			{
				v = call_ldexp(v, std::numeric_limits<std::size_t>::digits);
				std::size_t part = static_cast<std::size_t>(v);
				v -= part;
				hash_float_combine(seed, part);
			}

			hash_float_combine(seed, exp);

			return seed;
		}

		template <class Ch> inline
		std::size_t	string_hash_value( const Ch* pCh, std::size_t num )
		{
			const Ch* const pChEnd = pCh + num;
			std::size_t nHash = 0;
			for ( ; pCh != pChEnd ; ++pCh )
				nHash = (nHash<<5) + nHash + (*pCh);
			return nHash;
		}
		
		template <class Ch> inline
		std::size_t string_hash_value( const Ch* pCh )
		{
			std::size_t nHash = 0;
			for ( ; *pCh != 0 ; ++pCh )
				nHash = (nHash<<5) + nHash + (*pCh);
			return nHash;
		}


		template <class T>
		inline std::size_t hash_value_signed(T val)
		{
			const int size_t_bits = std::numeric_limits<std::size_t>::digits;
			// ceiling(std::numeric_limits<T>::digits / size_t_bits) - 1
			const int length = (std::numeric_limits<T>::digits - 1)
				/ size_t_bits;

			std::size_t seed = 0;
			T positive = val < 0 ? -1 - val : val;

			// Hopefully, this loop can be unrolled.
			for(unsigned int i = length * size_t_bits; i > 0; i -= size_t_bits)
			{
				seed ^= (std::size_t) (positive >> i) + (seed<<6) + (seed>>2);
			}
			seed ^= (std::size_t) val + (seed<<6) + (seed>>2);

			return seed;
		}

		template <class T>
		inline std::size_t hash_value_unsigned(T val)
		{
			const int size_t_bits = std::numeric_limits<std::size_t>::digits;
			// ceiling(std::numeric_limits<T>::digits / size_t_bits) - 1
			const int length = (std::numeric_limits<T>::digits - 1)
				/ size_t_bits;

			std::size_t seed = 0;

			// Hopefully, this loop can be unrolled.
			for(unsigned int i = length * size_t_bits; i > 0; i -= size_t_bits)
			{
				seed ^= (std::size_t) (val >> i) + (seed<<6) + (seed>>2);
			}
			seed ^= (std::size_t) val + (seed<<6) + (seed>>2);

			return seed;
		}


		template<class T>
		struct is_normal_integral_impl : integral_constant<bool, (sizeof(T) < sizeof(long long)) > {};
		
		template<class T>
		struct is_normal_integral 
		{
			typedef typename mpl::eval_if< is_integral<T>, is_normal_integral_impl<T>, false_type>::type type;
			static const bool value = type::value;
		};
		
	
	}
	
	template<class T> __forceinline typename i3::enable_if< detail::is_normal_integral<T>,
	std::size_t>::type hash_value( T v )
	{
		return static_cast<std::size_t>(v);
	}
	
	template<class T> __forceinline typename i3::enable_if< is_enum<T>,
	std::size_t>::type hash_value( T v )
	{
		return static_cast<std::size_t>(v);
	}

	__forceinline std::size_t hash_value(float v) {	return detail::float_hash_value(v); }
	__forceinline std::size_t hash_value(double v) { return detail::float_hash_value(v); }
	__forceinline std::size_t hash_value(long double v) { return detail::float_hash_value(v); }
	__forceinline std::size_t hash_value(long long v) { return detail::hash_value_signed(v); }
	__forceinline std::size_t hash_value(unsigned long long v) { return detail::hash_value_unsigned(v); }


	template<class Ch, class T, class A> __forceinline
	std::size_t hash_value(const std::basic_string<Ch, T, A>& v )
	{
		return detail::string_hash_value(v.data(), v.size() ); 
	}

	template<class Ch, class T, class A> class basic_string;
	
	template<class Ch, class T, class A> __forceinline
	std::size_t hash_value(const i3::basic_string<Ch, T, A>& v )
	{
		return detail::string_hash_value(v.data(), v.size() ); 
	}

	template<class Ch, class T, class A> class rc_basic_string;

	template<class Ch, class T, class A> __forceinline
	std::size_t hash_value(const i3::rc_basic_string<Ch, T, A>& v )
	{
		return detail::string_hash_value(v.data(), v.size() ); 
	}

	template<class Ch, class T, class A> class fixed_basic_string;
	
	template<class Ch, class T, class A> __forceinline
	std::size_t hash_value(const i3::fixed_basic_string<Ch, T, A>& v )
	{
		std::size_t x = static_cast<std::size_t>( reinterpret_cast<std::ptrdiff_t>(v.c_str() ) );
		return x + (x >> 3);
	}

	__forceinline std::size_t hash_value(const std::type_info* v)
	{
		return detail::string_hash_value( v->raw_name());
	}

	//
	// type_info의 해시함수버전..
	//	type_info는 값복사생성,복사대입 모두 안됨. 또한 참조의 경우, 컨테이너에 들어갈수 없어, 가능한 것은 오직 상수 포인터뿐이다..
	template<> struct hash<const std::type_info*> :  public i3::unary_function<const std::type_info*, std::size_t>
	{
		__forceinline std::size_t	operator()(const std::type_info* v) const 
		{
			// 문자열처리로 대신할것..
			return detail::string_hash_value( v->raw_name());
		}
	};

	template<class T> struct hash : public i3::unary_function<T, std::size_t>
	{
		typedef typename i3::make_parameter<T>::type		parameter_type;

		__forceinline std::size_t operator()(parameter_type val) const
		{
			return i3::hash_value(val);
		}
	};
	
	template <class Ch> inline
	std::size_t	istring_hash_value(const Ch* pCh, std::size_t num)
	{
		const Ch* const pChEnd = pCh + num;
		std::size_t nHash = 0;
		for (; pCh != pChEnd; ++pCh)
			nHash = (nHash << 5) + nHash + to_lower_functor_fast<Ch>()(*pCh);
		return nHash;
	}

	template <class Ch> inline
	std::size_t istring_hash_value(const Ch* pCh)
	{
		std::size_t nHash = 0;
		for (; *pCh != 0; ++pCh)
			nHash = (nHash << 5) + nHash + to_lower_functor_fast<Ch>()(*pCh);
		return nHash;
	}



}