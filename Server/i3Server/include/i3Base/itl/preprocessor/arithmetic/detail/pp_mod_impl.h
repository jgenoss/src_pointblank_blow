#pragma once


#define pp_mod_impl(x, y)			pp_tuple_elem(3, 1, pp_div_base(x, y))
#define pp_mod_d_impl(d, x, y)		pp_tuple_elem(3, 1, pp_div_base_d(d, x, y))

