#pragma once

/// bit..

/// 0번비트 (callable 비트)
/// 1번비트, 4번비트 ( non_member / member )
/// 0 ~ 7번비트  : 타입속성
/// 8 ~ 9번 비트   : variadic
/// 10~11번 비트 : 한정자

/// 15번비트 : default cc_id비트..

#define pp_ft_bit_callable_builtin			0x00000001
#define pp_ft_bit_non_member				0x00000002
#define pp_ft_bit_member					0x00000020


#define pp_ft_bit_naked_function			0x00000007	// 0x0000 0111	
#define pp_ft_bit_function_ptr				0x0000000B	// 0x0000 1011
#define pp_ft_bit_function_ref				0x00000013	// 0x0001 0011	
#define pp_ft_bit_member_function_ptr		0x00000061  // 0x0110 0001
#define pp_ft_bit_member_object_ptr			0x000000A3  // 0x1010 0001
#define pp_ft_bit_member_object_ptr_base	0x000002A3	// + non_variadic bit..
///
#define pp_ft_bit_variadic					0x00000100
#define pp_ft_bit_non_variadic				0x00000200
#define pp_ft_bit_const						0x00000400
#define pp_ft_bit_volatile					0x00000800
#define pp_ft_bit_cv						0x00000C00		
///
#define pp_ft_bit_default_cc				0x00008000		// cc_id영역전 1비트..


///
#define pp_ft_mask_type						0x000000FF		
#define pp_ft_mask_kind						0x000000FC
///
#define pp_ft_mask_member					0x00000022
#define pp_ft_mask_variadic					0x00000300
#define pp_ft_mask_cv						0x00000C00
///
#define pp_ft_mask_flags					0x00000FFF
#define pp_ft_mask_cc						0x00FF8000		// default_cc비트를 포함..

#define pp_ft_mask_full						0x00FF0FFF		// ccid + flags

#define pp_ft_shift_arity					24				// 태그에는 쓰지 않고, 다른 용도임..



