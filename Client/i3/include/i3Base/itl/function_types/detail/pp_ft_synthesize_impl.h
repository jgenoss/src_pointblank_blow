#pragma once


#define pp_ft_file_head		pp_ft_synthesize_impl
#define pp_ft_function_condition	(pp_ft_bit_naked_function|pp_ft_bit_member_function_ptr)

#include "pp_ft_function.h"

#undef  pp_ft_function_condition	
#undef  pp_ft_file_head
