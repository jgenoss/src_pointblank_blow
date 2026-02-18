#pragma once

#define pp_less_equal_impl(x, y)		pp_not(pp_sub(x, y))
#define pp_less_equal_d_impl(d, x, y)	pp_not(pp_sub_d(d, x, y))
