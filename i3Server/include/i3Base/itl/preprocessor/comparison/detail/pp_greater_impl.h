#pragma once

#define pp_greater_impl(x, y)			pp_less(y, x)
#define pp_greater_d_impl(d, x, y)		pp_less_d(d, y, x)
