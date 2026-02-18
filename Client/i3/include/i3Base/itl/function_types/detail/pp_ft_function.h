

# ifndef pp_ft_file_head
#    error pp_ft_error:  pp_ft_file_head is not defined
# endif


# ifndef pp_ft_function_condition
#    error pp_ft_error:  pp_ft_function_condition is not defined
# endif

#ifndef pp_ft_function_h
#define pp_ft_function_h

#include "../../preprocessor/pp_stringize.h"

#endif


// function
#if     (pp_ft_function_condition & pp_ft_bit_naked_function) == pp_ft_bit_naked_function

#define pp_ft_iterate_file		pp_stringize(pp_cat(pp_ft_file_head, _function.h))			
#define pp_ft_type_flags		pp_ft_bit_naked_function
#define pp_ft_cv_condition		0

#include "pp_ft_cc_tag.h"

#undef  pp_ft_cv_condition
#undef  pp_ft_type_flags
#undef  pp_ft_iterate_file

#endif

// function_ptr

#if     (pp_ft_function_condition & pp_ft_bit_function_ptr) == pp_ft_bit_function_ptr

#define pp_ft_iterate_file		pp_stringize(pp_cat(pp_ft_file_head, _function_ptr.h))			
#define pp_ft_type_flags		pp_ft_bit_function_ptr
#define pp_ft_cv_condition		0

#include "pp_ft_cc_tag.h"

#undef  pp_ft_cv_condition
#undef  pp_ft_type_flags
#undef  pp_ft_iterate_file

#endif



// function_ref
#if     (pp_ft_function_condition & pp_ft_bit_function_ref) == pp_ft_bit_function_ref

#define pp_ft_iterate_file		pp_stringize(pp_cat(pp_ft_file_head, _function_ref.h))			
#define pp_ft_type_flags		pp_ft_bit_function_ref
#define pp_ft_cv_condition		0

#include "pp_ft_cc_tag.h"

#undef  pp_ft_cv_condition
#undef  pp_ft_type_flags
#undef  pp_ft_iterate_file

#endif

// mem_fun_ptr
#if     (pp_ft_function_condition & pp_ft_bit_member_function_ptr) == pp_ft_bit_member_function_ptr

#define pp_ft_iterate_file		pp_stringize(pp_cat(pp_ft_file_head, _member_function_ptr.h))		
#define pp_ft_type_flags		pp_ft_bit_member_function_ptr
#define pp_ft_cv_condition		1

#include "pp_ft_cc_tag.h"

#undef  pp_ft_cv_condition
#undef  pp_ft_type_flags
#undef  pp_ft_iterate_file

#endif


