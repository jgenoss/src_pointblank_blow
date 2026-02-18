#pragma once

#define pp_less_impl(x, y)				pp_iif(pp_not_equal(x, y), pp_less_equal, 0 pp_tuple_eat_2)(x, y)
#define pp_less_d_impl(d, x, y)			pp_iif(pp_not_equal(x, y), pp_less_equal_d, 0 pp_tuple_eat_3)(d, x, y)
