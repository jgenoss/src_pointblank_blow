#pragma once

#define pp_min_impl(x, y) pp_iif(pp_less_equal(y, x), y, x)
#define pp_min_d_impl(d, x, y) pp_iif(pp_less_equal_d(d, y, x), y, x)
