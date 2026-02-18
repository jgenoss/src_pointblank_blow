#pragma once

#define pp_greater_equal_impl(x, y)			pp_less_equal(y, x)
#define pp_greater_equal_d_impl(d, x, y)	pp_less_equal_d(d, y, x)
