#pragma once

//
// WindowsAPI GetEnvirontmentVariable의 일반화 버전이 필요해서 만듬..
//
// 출력은 내부에서 i3::stack_string을 쓰고, 복사하도록 처리한다..
// API와 같은 출력 리터럴 버전은 만들지 않는다... 출력리터럴버전은 사용하기 불편하므로 아예 배제한다..
//

#include "../../itl/range/value_type.h"
#include "../../itl/range/copy_range.h"
#include "../stack_string.h"

namespace i3
{
	
	namespace detail 
	{
		template<bool is_multibyte> 
		struct get_env_var_api;

		template<>
		struct get_env_var_api<true>
		{
			template<class Ch> inline
			static DWORD call(const Ch* input, Ch* output, DWORD outsize) 
			{ return ::GetEnvironmentVariableA((const char*)input,(char*)output,outsize); }
		};

		template<>
		struct get_env_var_api<false>
		{
			template<class Ch> inline
			static DWORD call(const Ch* input, Ch* output, DWORD outsize) 
			{ return ::GetEnvironmentVariableW((const wchar_t*)input,(wchar_t*)output,outsize); }
		};
		
		template<class Ch, class StackString>  
		bool get_environment_variable_impl( const Ch* input, StackString& out)
		{
			static const bool	is_multibyte = (sizeof(Ch) == 1);
			static const DWORD	INIT_BUFF_SIZE = StackString::BuffSize;		// 디폴트로 잡힌 128을 그냥 쓴다..

			out.resize(INIT_BUFF_SIZE-1);
			DWORD res_size = 
				detail::get_env_var_api<is_multibyte>::call(input, &*out.begin(), INIT_BUFF_SIZE-1);	// 임시로 잡힌 128..

			if (res_size == 0 )
				return false;
			
			out.resize(res_size);		// 늘던 줄던 하게 된다..

			if (res_size > INIT_BUFF_SIZE-1)
			{
				detail::get_env_var_api<is_multibyte>::call(input, &*out.begin(), res_size);
			}
			
			return true;
		}
		
		template<bool IsStackString>
		struct get_env_var_dispatch;

		template<>
		struct get_env_var_dispatch<true>
		{
			template<class Ch, class StackString>
			static bool call( const Ch* input, StackString& out) { return get_environment_variable_impl(input, out); }
		};
		template<>
		struct get_env_var_dispatch<false>
		{
			template<class Ch, class String>
			static bool call( const Ch* input, String& out) 
			{
				static const DWORD	INIT_BUFF_SIZE = i3::stack_string::BuffSize;		// 디폴트로 잡힌 128을 그냥 쓴다..
				i3::stack_basic_string<mpl::size_t<INIT_BUFF_SIZE>, Ch>	buffer;	// 모자르면 한번더 호출해서 구할수 있음..
				
				bool res;
				if ( res = get_environment_variable_impl(input, buffer) )
				{
					i3::generic_string_copy(out, buffer);
				}
				else
				{
					out.clear();
				}
				return res;
			}
		};

		template<class Ch, class OutSeq>  inline
		typename disable_if_iterator<OutSeq, 
		bool>::type get_environment_variable_imp( const Ch* input, OutSeq& out)
		{
			static const bool is_stack_str = i3::is_template_same<i3::stack_basic_string<mpl_, mpl_, mpl_, mpl_>, OutSeq>::value;
			return get_env_var_dispatch<is_stack_str>::call( input, out );
		}

	}

	template<class Rng, class OutSeq>  inline
	typename disable_if_iterator<OutSeq, bool>::type 
	get_environment_variable( const Rng& input, OutSeq& out)
	{
		typedef typename i3::get_iterator_range_type<const Rng>::type	range_type;
		typedef typename i3::range_value<range_type>::type				Ch;
		range_type rng = i3::as_literal(input);
		if (*rng.end() == Ch(0))		// 널 종결문자 존재시..
			return detail::get_environment_variable_imp( &rng[0], out);
		
		static const DWORD	INIT_BUFF_SIZE = i3::stack_string::BuffSize;		// 디폴트로 잡힌 128을 그냥 쓴다..
		i3::stack_basic_string<mpl::size_t<INIT_BUFF_SIZE>, Ch>	input_tmp(rng.begin(), rng.end());	// 모자르면 한번더 호출해서 구할수 있음..
		
		return detail::get_environment_variable_imp(input_tmp.c_str(), out);
	}

}