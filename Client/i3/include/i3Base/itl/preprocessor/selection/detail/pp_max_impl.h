#pragma once

#define pp_max_impl(x, y) pp_iif(pp_less_equal(x, y), y, x)
#define pp_max_d_impl(d, x, y) pp_iif(pp_less_equal_d(d, x, y), y, x)
