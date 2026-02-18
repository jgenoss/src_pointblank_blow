#pragma once

#include "fixed_string.h"
#include "../itl/preprocessor/pp_cat.h"
#include "../itl/preprocessor/pp_stringize.h"
#include "../itl/preprocessor/pp_wstringize.h"

namespace i3
{
	template<class Fixed, class F>
	class make_static_fixed_string
	{
	public:
		operator const Fixed& () const { return ms_str; }
	private:
		static const Fixed	ms_str;
	};
	template<class Fixed, class F>
	const Fixed	make_static_fixed_string<Fixed,F>::ms_str = F()() ;
}

#define declare_fixed_literal(x) \
struct pp_cat(fn_c_, x) {  const char* operator()() const { return pp_stringize(x); } }; \
	i3::fixed_string x = i3::make_static_fixed_string<i3::fixed_string, pp_cat(fn_c_, x)>() ;

#define declare_fixed_wliteral(x) \
struct pp_cat(fn_w_, x) {  const wchar_t* operator()() const { return pp_wstringize(x); } }; \
	i3::fixed_wstring x = i3::make_static_fixed_string<i3::fixed_wstring, pp_cat(fn_w_, x)>() ;


#define copy_fixed_literal(dest, src) \
struct pp_cat(fn_c_, src) {  const char* operator()() const { return pp_stringize(src); } }; \
	i3::fixed_string dest = i3::make_static_fixed_string<i3::fixed_string, pp_cat(fn_c_, src)>() ;

#define copy_fixed_wliteral(dest, src) \
struct pp_cat(fn_w_, src) {  const wchar_t* operator()() const { return pp_wstringize(src); } }; \
	i3::fixed_wstring dest = i3::make_static_fixed_string<i3::fixed_wstring, pp_cat(fn_w_, src)>() ;
