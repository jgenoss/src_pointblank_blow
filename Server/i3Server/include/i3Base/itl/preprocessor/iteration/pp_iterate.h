#pragma once

// 작성 방법
// #ifndef pp_is_iterating
// #ifndef 인클루드가드
// #define 인클루드가드
//
// #include "pp_iterate.h"
//
// #define pp_iteration_limits( m, n )  : m~n범위
// #define pp_filename_1	"파일이름"	: 자기자신의 파일이름을 적음
// #include pp_iterate()
// #endif
//
// #else
// #define n pp_iteration()			    : n이 이제 템플릿파라미터의 갯수로 쓰임..
// 
// 사용..
//
// #undef n
// #endif

#include "../arithmetic/pp_dec.h"
#include "../arithmetic/pp_inc.h"
#include "../array/pp_array_elem.h"		// pp_array_size, pp_tuple_elem 포함
#include "../slot/pp_slot.h"			// pp_cat포함


#define pp_iteration_depth()	0 

#define pp_iteration()			pp_cat(pp_iteration_, pp_iteration_depth())
#define pp_iteration_start()	pp_cat(pp_iteration_start_, pp_iteration_depth())
#define pp_iteration_finish()	pp_cat(pp_iteration_finish_, pp_iteration_depth())
#define pp_iteration_flags() (pp_cat(pp_iteration_flags_, pp_iteration_depth()))

#define pp_frame_iteration(i) pp_cat(pp_iteration_, i)
#define pp_frame_start(i) pp_cat(pp_iteration_start_, i)
#define pp_frame_finish(i) pp_cat(pp_iteration_finish_, i)
#define pp_frame_flags(i) (pp_cat(pp_iteration_flags_, i))

#define pp_relative_iteration(i) pp_cat(pp_relative_, i)(pp_iteration_)

#define pp_relative_0(m) pp_cat(m, pp_iteration_depth())
#define pp_relative_1(m) pp_cat(m, pp_dec(pp_iteration_depth()))
#define pp_relative_2(m) pp_cat(m, pp_dec(pp_dec(pp_iteration_depth())))
#define pp_relative_3(m) pp_cat(m, pp_dec(pp_dec(pp_dec(pp_iteration_depth()))))
#define pp_relative_4(m) pp_cat(m, pp_dec(pp_dec(pp_dec(pp_dec(pp_iteration_depth())))))

#define pp_relative_start(i) pp_cat(pp_relative_, i)(pp_iteration_start_)
#define pp_relative_finish(i) pp_cat(pp_relative_, i)(pp_iteration_finish_)
#define pp_relative_flags(i) (pp_cat(pp_relative_, i)(pp_iteration_flags_))


#define pp_iterate()			pp_cat(pp_iterate_, pp_inc(pp_iteration_depth()))

# define pp_iterate_1			"i3Base/itl/preprocessor/iteration/detail/iter/pp_forward1.h"
# define pp_iterate_2			"i3Base/itl/preprocessor/iteration/detail/iter/pp_forward2.h"
# define pp_iterate_3			"i3Base/itl/preprocessor/iteration/detail/iter/pp_forward3.h"
# define pp_iterate_4			"i3Base/itl/preprocessor/iteration/detail/iter/pp_forward4.h"
# define pp_iterate_5			"i3Base/itl/preprocessor/iteration/detail/iter/pp_forward5.h"

