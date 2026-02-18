#pragma once

//  cc = calling convention..
//  MSVC80이상 기준..
//	64비트 OS의 경우 clrcall을 제외하고는 별도의 calling convention을 갖고 있지 않다...(유의)

#define pp_ft_cc_id_implicit					1
#define pp_ft_cc_id_cdecl						2
#define pp_ft_cc_id_stdcall						3
#define pp_ft_cc_id_pascal						4
#define pp_ft_cc_id_fastcall					5
#define pp_ft_cc_id_clrcall						6
#define pp_ft_cc_id_thiscall					7
#define pp_ft_cc_id_implicit_thiscall			8

#ifndef _M_IX86
namespace i3
{
	namespace ft
	{
		struct implicit_cc_tag : bits_tag< encode_bits<0, pp_ft_cc_id_implicit>::value, pp_ft_mask_cc> {};
		typedef implicit_cc_tag			implicit_cc;
	}
}

#define pp_ft_cc_condition_x64					pp_ft_bit_callable_builtin

#else // _M_IX86

#if defined (__cplusplus_cli)
#define pp_ft_cc_clrcall		__clrcall	// ft_callable_builtin
#define pp_ft_cc_condition_clrcall				pp_ft_bit_callable_builtin
#endif

#define pp_ft_cc_thiscall		__thiscall	//ft_non_variadic| ft_member_pointer | ft_callable_builtin
#define pp_ft_cc_cdecl			__cdecl		//ft_callable_builtin
#define pp_ft_cc_stdcall		__stdcall	//ft_non_variadic|ft_callable_builtin
#define	pp_ft_cc_fastcall		__fastcall	//ft_non_variadic|ft_callable_builtin

#define pp_ft_cc_condition_thiscall				(pp_ft_bit_callable_builtin|pp_ft_bit_non_variadic|pp_ft_bit_member)
#define pp_ft_cc_condition_cdecl				(pp_ft_bit_callable_builtin)
#define pp_ft_cc_condition_stdcall				(pp_ft_bit_callable_builtin|pp_ft_bit_non_variadic)
#define pp_ft_cc_condition_fastcall				(pp_ft_bit_callable_builtin|pp_ft_bit_non_variadic)

namespace i3
{
	namespace ft
	{
		typedef bits_tag<pp_ft_bit_default_cc, pp_ft_mask_cc>	default_cc_tag;
		typedef default_cc_tag			default_cc;


#if defined(pp_ft_cc_implicit)
		struct implicit_cc_tag : bits_tag< encode_bits<0, pp_ft_cc_id_implicit>::value, pp_ft_mask_cc > {};
		typedef implicit_cc_tag			implicit_cc;
#endif

#if defined(pp_ft_cc_cdecl)
		struct cdecl_cc_tag : bits_tag< encode_bits<0, pp_ft_cc_id_cdecl>::value, pp_ft_mask_cc> {};
		typedef cdecl_cc_tag			cdecl_cc;
#endif

#if defined(pp_ft_cc_stdcall)
		struct stdcall_cc_tag : bits_tag< encode_bits<0, pp_ft_cc_id_stdcall>::value, pp_ft_mask_cc> {};
		typedef stdcall_cc_tag			stdcall_cc;
#endif

#if defined(pp_ft_cc_pascal)
		struct pascal_cc_tag : bits_tag< encode_bits<0, pp_ft_cc_id_pascal>::value, pp_ft_mask_cc> {};
		typedef pascal_cc_tag			pascal_cc;
#endif

#if defined(pp_ft_cc_fastcall)
		struct fastcall_cc_tag : bits_tag< encode_bits<0, pp_ft_cc_id_fastcall>::value, pp_ft_mask_cc> {};
		typedef fastcall_cc_tag			fastcall_cc;
#endif

#if defined(pp_ft_cc_clrcall)
		struct clrcall_cc_tag : bits_tag< encode_bits<0, pp_ft_cc_id_clrcall>::value, pp_ft_mask_cc> {};
		typedef clrcall_cc_tag			clrcall_cc;
#endif

#if defined(pp_ft_cc_thiscall)
		struct thiscall_cc_tag : bits_tag< encode_bits<0, pp_ft_cc_id_thiscall>::value, pp_ft_mask_cc> {};
		typedef thiscall_cc_tag			thiscall_cc;
#endif

#if defined(pp_ft_cc_implicit_thiscall)
		struct thiscall_cc_tag : bits_tag< encode_bits<0, pp_ft_cc_id_implicit_thiscall>::value, pp_ft_mask_cc> {};
		typedef thiscall_cc_tag			thiscall_cc;
#endif
	

	
	}
}

#endif // _M_IX86