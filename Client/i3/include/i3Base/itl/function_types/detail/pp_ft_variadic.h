
// no include guard


# ifndef pp_ft_cc
#    error pp_ft_error:  pp_ft_cc is not defined
# endif

# ifndef pp_ft_cc_id
#    error pp_ft_error:  pp_ft_cc_id is not defined
# endif

# ifndef pp_ft_cc_condition
#    error pp_ft_error:  pp_ft_cc_condition is not defined
# endif

#if  ( ( pp_ft_type_flags & pp_ft_bit_non_member) && ( pp_ft_cc_condition & pp_ft_bit_member) )	// 멤버함수를 요구하는데, 일반함수라면..	

#elif ( ( pp_ft_type_flags & pp_ft_bit_member) && ( pp_ft_cc_condition & pp_ft_bit_non_member) )	// 일반함수를 요구하는데, 멤버함수라면..

#else

// 먼저 일반함수부터
#define pp_ft_variadic		
#define pp_ft_variadic_flags	pp_ft_bit_non_variadic
#include "pp_ft_cv.h"

// 그다음 variadic condition을 볼것..
#undef pp_ft_variadic_flags
#undef pp_ft_variadic

#if !( pp_ft_cc_condition & pp_ft_bit_non_variadic )				
	
#define pp_ft_variadic	...
#define pp_ft_variadic_flags	pp_ft_bit_variadic
#include "pp_ft_cv.h"

#undef pp_ft_variadic_flags
#undef pp_ft_variadic
	
#endif // 가변인자조건..

#endif // 함수타입조건..
