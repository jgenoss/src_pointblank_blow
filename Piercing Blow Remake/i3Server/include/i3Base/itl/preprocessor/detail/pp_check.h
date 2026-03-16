#pragma once

#include "../pp_cat.h"

#define pp_check(x, type)	pp_check_d(x, type)

#define pp_check_d(x, type) pp_check_1(type x)
#define pp_check_1(chk)		pp_check_2(chk)
#define pp_check_2(chk)		pp_check_3((pp_check_result_ ## chk))
#define pp_check_3(im)		pp_check_5(pp_check_4 im)
#define pp_check_4(res, _)	res
#define pp_check_5(res)		res

#define pp_check_result_1	1, pp_nil
