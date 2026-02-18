#pragma once

// 메타함수가 아님.. 함수 오버로딩 사용
#include "arg_true_false_type.h"

namespace i3
{
	inline arg_true_type is_arg_c_str( char* ) { return arg_true; }
	inline arg_true_type is_arg_c_str( const char* ) { return arg_true; }
	inline arg_true_type is_arg_c_str( wchar_t*) { return arg_true; }
	inline arg_true_type is_arg_c_str( const wchar_t* ) { return arg_true; }
	template<class T> inline
	arg_false_type		 is_arg_c_str( const T& ) { return arg_false; }
}