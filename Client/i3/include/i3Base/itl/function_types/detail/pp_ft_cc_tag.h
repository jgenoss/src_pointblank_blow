
// no include guard ..

# ifndef pp_ft_iterate_file
#    error pp_ft_error:  pp_ft_iterate_file is not defined
# endif

# ifndef pp_ft_type_flags
#    error pp_ft_error:  pp_ft_type_flags is not defined
# endif

# ifndef pp_ft_cv_condition
#    error pp_ft_error:  pp_ft_cv_condition is not defined
# endif


#ifndef _M_IX86

#define pp_ft_cc 
#define pp_ft_cc_id				pp_ft_cc_id_implicit
#define pp_ft_cc_condition		pp_ft_cc_condition_x64

#include "pp_ft_variadic.h"

#undef  pp_ft_cc_condition
#undef  pp_ft_cc_id 
#undef  pp_ft_cc 

#else // _M_IX86


#if defined(pp_ft_cc_implicit)
#define pp_ft_cc			
#define pp_ft_cc_id				pp_ft_cc_id_implicit
#define pp_ft_cc_condition		pp_ft_cc_condition_implicit			// 정해지지 않음.

#include "pp_ft_variadic.h"

#undef  pp_ft_cc_condition
#undef  pp_ft_cc_id 
#undef  pp_ft_cc 
#endif

#if defined(pp_ft_cc_cdecl)
#define pp_ft_cc				pp_ft_cc_cdecl
#define pp_ft_cc_id				pp_ft_cc_id_cdecl
#define pp_ft_cc_condition		pp_ft_cc_condition_cdecl

#include "pp_ft_variadic.h"

#undef  pp_ft_cc_condition
#undef  pp_ft_cc_id 
#undef  pp_ft_cc 
#endif


#if defined(pp_ft_cc_stdcall)
#define pp_ft_cc				pp_ft_cc_stdcall
#define pp_ft_cc_id				pp_ft_cc_id_stdcall
#define pp_ft_cc_condition		pp_ft_cc_condition_stdcall

#include "pp_ft_variadic.h"

#undef  pp_ft_cc_condition
#undef  pp_ft_cc_id 
#undef  pp_ft_cc 
#endif


#if defined(pp_ft_cc_pascal)
#define pp_ft_cc				pp_ft_cc_pascal
#define pp_ft_cc_id				pp_ft_cc_id_pascal
#define pp_ft_cc_condition		pp_ft_cc_condition_pascal

#include "pp_ft_variadic.h"

#undef  pp_ft_cc_condition
#undef  pp_ft_cc_id 
#undef  pp_ft_cc 
#endif


#if defined(pp_ft_cc_fastcall)
#define pp_ft_cc				pp_ft_cc_fastcall
#define pp_ft_cc_id				pp_ft_cc_id_fastcall
#define pp_ft_cc_condition		pp_ft_cc_condition_fastcall

#include "pp_ft_variadic.h"

#undef  pp_ft_cc_condition
#undef  pp_ft_cc_id 
#undef  pp_ft_cc 
#endif


#if defined(pp_ft_cc_clrcall)
#define pp_ft_cc				pp_ft_cc_clrcall
#define pp_ft_cc_id				pp_ft_cc_id_clrcall
#define pp_ft_cc_condition		pp_ft_cc_condition_clrcall

#include "pp_ft_variadic.h"

#undef  pp_ft_cc_condition
#undef  pp_ft_cc_id 
#undef  pp_ft_cc 
#endif


#if defined(pp_ft_cc_thiscall)
#define pp_ft_cc				pp_ft_cc_thiscall
#define pp_ft_cc_id				pp_ft_cc_id_thiscall
#define pp_ft_cc_condition		pp_ft_cc_condition_thiscall

#include "pp_ft_variadic.h"

#undef  pp_ft_cc_condition
#undef  pp_ft_cc_id 
#undef  pp_ft_cc 
#endif


#if defined(pp_ft_cc_implicit_thiscall)
#define pp_ft_cc			
#define pp_ft_cc_id				pp_ft_cc_id_implicit_thiscall
#define pp_ft_cc_condition		pp_ft_cc_condition_implicit_thiscall

#include "pp_ft_variadic.h"

#undef  pp_ft_cc_condition
#undef  pp_ft_cc_id 
#undef  pp_ft_cc 
#endif



#endif // _M_IX86
