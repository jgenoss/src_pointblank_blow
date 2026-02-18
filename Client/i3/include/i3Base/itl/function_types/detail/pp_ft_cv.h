
// no include guard


# ifndef pp_ft_variadic
#    error pp_ft_error:  pp_ft_variadic is not defined
# endif

# ifndef pp_ft_variadic_flags
#    error pp_ft_error:  pp_ft_variadic_flags is not defined
# endif


#define pp_ft_cv	
#define pp_value	pp_ft_type_flags|pp_ft_variadic_flags
#include pp_assign_slot(1)
#define pp_ft_flags pp_slot(1)

#include pp_ft_iterate_file

#undef pp_ft_flags
#undef pp_ft_cv

#if ( pp_ft_cv_condition == 1 )

// const

#	define pp_ft_cv	const 
#	define pp_value	pp_ft_type_flags|pp_ft_variadic_flags|pp_ft_bit_const
#	include pp_assign_slot(1)
#	define pp_ft_flags pp_slot(1)

#	include pp_ft_iterate_file

#	undef pp_ft_flags
#	undef pp_ft_cv


// volatile

#	define pp_ft_cv	volatile 
#	define pp_value	pp_ft_type_flags|pp_ft_variadic_flags|pp_ft_bit_volatile
#	include pp_assign_slot(1)
#	define pp_ft_flags pp_slot(1)

#	include pp_ft_iterate_file

#	undef pp_ft_flags
#	undef pp_ft_cv

// const volatile

#	define pp_ft_cv	const volatile 
#	define pp_value	pp_ft_type_flags|pp_ft_variadic_flags|pp_ft_bit_cv
#	include pp_assign_slot(1)
#	define pp_ft_flags pp_slot(1)

#	include pp_ft_iterate_file

#	undef pp_ft_flags
#	undef pp_ft_cv


#endif  // pp_ft_cv_condition == 1