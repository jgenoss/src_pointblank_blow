#pragma once

#include "../utility/bits_tag.h"
#include "detail/pp_ft_bit.h"

namespace i3
{
	namespace ft
	{
		typedef bits_tag< pp_ft_bit_non_variadic, pp_ft_mask_variadic>	non_variadic_tag;
		typedef non_variadic_tag										non_variadic;
		typedef bits_tag< pp_ft_bit_variadic, pp_ft_mask_variadic>		variadic_tag;
		typedef variadic_tag											variadic;

		typedef bits_tag<0, pp_ft_bit_const>							non_const_tag;
		typedef	non_const_tag											non_const;
		typedef bits_tag<pp_ft_bit_const, pp_ft_bit_const>				const_tag;
		typedef const_tag												const_qualified;

		typedef bits_tag<0, pp_ft_bit_volatile>							non_volatile_tag;
		typedef non_volatile_tag										non_volatile;
		typedef bits_tag<pp_ft_bit_volatile, pp_ft_bit_volatile>		volatile_tag;
		typedef volatile_tag											volatile_qualified;
	
		typedef bits_tag<0,  pp_ft_mask_cv >							non_cv_tag;
		typedef non_cv_tag												non_cv;

		typedef bits_tag<pp_ft_bit_const, pp_ft_mask_cv>				const_non_volatile_tag;
		typedef const_non_volatile_tag									const_non_volatile;
		typedef bits_tag<pp_ft_bit_volatile, pp_ft_mask_cv>				volatile_non_const_tag;
		typedef volatile_non_const_tag									volatile_non_const;

		typedef bits_tag<pp_ft_bit_cv, pp_ft_mask_cv>					cv_tag;
		typedef cv_tag													cv_qualified;

		
		typedef bits_tag< pp_ft_bit_callable_builtin, pp_ft_bit_callable_builtin>			callable_builtin_tag;
		typedef bits_tag< pp_ft_bit_callable_builtin|pp_ft_bit_non_member, pp_ft_bit_callable_builtin|pp_ft_bit_non_member> nonmember_callable_builtin_tag;
		

		typedef bits_tag< pp_ft_bit_naked_function, pp_ft_mask_type>		function_tag;
		typedef bits_tag< pp_ft_bit_function_ptr, pp_ft_mask_type>			function_ptr_tag;
		typedef function_ptr_tag											pointer_tag;

		typedef bits_tag< pp_ft_bit_function_ref, pp_ft_mask_type>			function_ref_tag;
		typedef function_ref_tag											reference_tag;

		typedef bits_tag< pp_ft_bit_member, pp_ft_bit_member>					member_ptr_tag;
		typedef member_ptr_tag													member_pointer_tag;

		typedef bits_tag< pp_ft_bit_member_function_ptr, pp_ft_mask_type>		member_function_ptr_tag;
		typedef member_function_ptr_tag											member_function_pointer_tag;

		typedef bits_tag< pp_ft_bit_member_object_ptr, pp_ft_mask_type>			member_object_ptr_tag;
		typedef	member_object_ptr_tag											member_object_pointer_tag;
		typedef bits_tag<  pp_ft_bit_member_object_ptr_base, pp_ft_mask_type>	member_object_ptr_base_tag;
		
		typedef bits_tag<	pp_ft_bit_naked_function|pp_ft_bit_non_variadic|pp_ft_bit_default_cc,
							pp_ft_mask_type|pp_ft_mask_variadic|pp_ft_mask_cc >		nv_dcc_func_tag;
		typedef bits_tag<	pp_ft_bit_member_function_ptr|pp_ft_bit_non_variadic|pp_ft_bit_default_cc,
							pp_ft_mask_type|pp_ft_mask_variadic|pp_ft_mask_cc >		nv_dcc_mfp_tag;

		typedef null_bits_tag							null_tag;

	}
	
	
}

#include "ft_tag_function.h"


