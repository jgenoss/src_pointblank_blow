#pragma once

#include "detail/static_log2.h"
#include "functional.h"
#include <typeinfo>
#include <math.h>
#include <string>

namespace i3
{
	template <class T> struct hash;

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

		template <class Ch> inline
		std::size_t	string_hash_value( const Ch* pCh, std::size_t num )
		{
			const Ch* pChEnd = pCh + num;
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


		inline std::size_t	hash_value( int v ) 
		{ 
			return static_cast<std::size_t>(v); 
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

	}

	

	template<> struct hash<bool> : public i3::unary_function< bool, std::size_t >
	{	std::size_t	operator()( bool v ) const { return static_cast<std::size_t>(v); }	};

	template<> struct hash<char> : public i3::unary_function< char, std::size_t >
	{	std::size_t	operator()( char v ) const { return static_cast<std::size_t>(v); }	};

	template<> struct hash<unsigned char> : public i3::unary_function<unsigned char, std::size_t >
	{	std::size_t	operator()(unsigned char v ) const { return static_cast<std::size_t>(v); }	};

	template<> struct hash<signed char> : public i3::unary_function<signed char, std::size_t >
	{	std::size_t	operator()(signed char v ) const { return static_cast<std::size_t>(v); }	};
	
	template<> struct hash<short> : public i3::unary_function< short, std::size_t >
	{	std::size_t	operator()( short v ) const { return static_cast<std::size_t>(v); }	};

	template<> struct hash<unsigned short> : public i3::unary_function<unsigned short, std::size_t >
	{	std::size_t	operator()( unsigned short v ) const { return static_cast<std::size_t>(v); }	};

	template<> struct hash<int> : public i3::unary_function<int, std::size_t >
	{	std::size_t	operator()( int v ) const { return static_cast<std::size_t>(v); }	};

	template<> struct hash<unsigned int> : public i3::unary_function<unsigned int, std::size_t >
	{	std::size_t	operator()( unsigned  int v ) const { return static_cast<std::size_t>(v); }	};

	template<> struct hash<long> : public i3::unary_function<long, std::size_t >
	{	std::size_t	operator()( long v ) const { return static_cast<std::size_t>(v); }	};

	template<> struct hash<unsigned long> : public i3::unary_function<unsigned long, std::size_t >
	{	std::size_t	operator()( unsigned  long v ) const { return static_cast<std::size_t>(v); }	};

#ifdef _NATIVE_WCHAR_T_DEFINED
	template<> struct hash<wchar_t> : public i3::unary_function<wchar_t, std::size_t >
	{	std::size_t	operator()( wchar_t v ) const { return static_cast<std::size_t>(v); }	};
#endif

	template<> struct hash<float> : public i3::unary_function<float, std::size_t >
	{	std::size_t	operator()( float v ) const { return detail::float_hash_value(v); }	};
	
	template<> struct hash<double> : public i3::unary_function<double, std::size_t >
	{	std::size_t	operator()( double v ) const { return detail::float_hash_value(v); }	};
	
	template<> struct hash<long double> : public i3::unary_function<long double, std::size_t >
	{	std::size_t	operator()( long double v ) const { return detail::float_hash_value(v); }	};
	
//	template<> struct hash<std::string> : public std::unary_function<std::string, std::size_t >
//	{	std::size_t	operator()(const std::string& v ) const { return detail::hash_range(v.begin(), v.end() ); }  };

//	template<> struct hash<std::wstring> : public std::unary_function<std::wstring, std::size_t >
//	{	std::size_t	operator()(const std::wstring& v ) const { return detail::hash_range(v.begin(), v.end() ); }  };
	
//	template<> struct hash<std::string> : public i3::unary_function<std::string, std::size_t >
//	{	std::size_t	operator()(const std::string& v ) const { return detail::string_hash_value(v.data(), v.size() ); }  };

//	template<> struct hash<std::wstring> : public i3::unary_function<std::wstring, std::size_t >
//	{	std::size_t	operator()(const std::wstring& v ) const { return detail::string_hash_value(v.data(), v.size() ); }  };

	template<class Ch, class T, class A> 
	struct hash<std::basic_string<Ch, T, A> > : public i3::unary_function<std::basic_string<Ch, T, A>, std::size_t >
	{	
		std::size_t	operator()(const std::basic_string<Ch, T, A>& v ) const 
		{ 
			return detail::string_hash_value(v.data(), v.size() ); 
		}  
	};

	template<class Ch, class T, class A> class basic_string;
	
	template<class Ch, class T, class A> 
	struct hash<i3::basic_string<Ch, T, A> > : public i3::unary_function<i3::basic_string<Ch, T, A>, std::size_t >
	{	
		std::size_t	operator()(const i3::basic_string<Ch, T, A>& v ) const 
		{ 
			return detail::string_hash_value(v.data(), v.size() ); 
		}  
	};

//	template<class Ch, class T, class A> 
//	struct hash<i3::wstring> : public std::unary_function<i3::wstring, std::size_t >
//	{	
//		std::size_t	operator()(const i3::wstring& v ) const 
//		{ 
//			return detail::string_hash_value(v.data(), v.size() ); 
//		}  
//	};

	template<class Ch, class T, class A> class rc_basic_string;

	template<class Ch, class T, class A> 
	struct hash<i3::rc_basic_string<Ch, T, A> > : public i3::unary_function<i3::rc_basic_string<Ch, T, A>, std::size_t >
	{	
		std::size_t	operator()(const i3::rc_basic_string<Ch, T, A>& v ) const 
		{ 
			return detail::string_hash_value(v.data(), v.size() ); 
		}  
	};

//	template<> struct hash<i3::rc_wstring> : public std::unary_function<i3::rc_wstring, std::size_t >
//	{	
//		std::size_t	operator()(const i3::rc_wstring& v ) const 
//		{ 
//			return detail::string_hash_value(v.data(), v.size() ); 
//		}  
//	};

	template<class Ch, class T, class A> class fixed_basic_string;

	template<class Ch, class T, class A> 
	struct hash<i3::fixed_basic_string<Ch, T, A> > : public i3::unary_function<i3::fixed_basic_string<Ch, T, A>, std::size_t> 
	{
		__forceinline std::size_t	operator()(const i3::fixed_basic_string<Ch, T, A>& v ) const 
		{
			std::size_t x = static_cast<std::size_t>( reinterpret_cast<std::ptrdiff_t>(v.c_str() ) );
			return x + (x >> 3);
		}
	};


	template<class T> struct hash<T*> : public i3::unary_function<T*, std::size_t>
	{
		__forceinline std::size_t	operator()(T* v) const 
		{       
			std::size_t x = static_cast<std::size_t>( reinterpret_cast<std::ptrdiff_t>(v) );
			return x + (x >> 3);
		}
	};
	
	//
	// type_info의 해시함수버전..
	// 
	template<> struct hash<const std::type_info*> :  public i3::unary_function<const std::type_info*, std::size_t>
	{
		__forceinline std::size_t	operator()(const std::type_info* v) const 
		{
			// 문자열처리로 대신할것..
			return detail::string_hash_value( v->raw_name());
		}
	};

//	type_info는 값복사생성,복사대입 모두 안됨. 또한 참조의 경우, 컨테이너에 들어갈수 없어, 가능한 것은 오직 상수 포인터뿐이다..
//	template<> struct hash<const std::type_info&> :  public i3::unary_function<const std::type_info&, std::size_t>
//	{
//		__forceinline std::size_t	operator()(const std::type_info& v) const 
//		{
//			// 문자열처리로 대신할것..
//			return detail::string_hash_value(v.raw_name());
//		}
//	};
//

	template<class T> struct hash : public i3::unary_function<T, std::size_t>
	{
		__forceinline std::size_t operator()(const T& val) const
		{
			return detail::hash_value(val);
		}
	};
		

}