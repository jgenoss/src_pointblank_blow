#pragma once

#define pp_div_base(x, y)			pp_while(pp_div_base_p, pp_div_base_o, (0, x, y))
#define pp_div_base_p(d, rxy)		pp_div_base_p_i(d, pp_tuple_elem(3, 0, rxy), pp_tuple_elem(3, 1, rxy), pp_tuple_elem(3, 2, rxy))
#define pp_div_base_p_i(d, r, x, y) pp_less_equal_d(d, y, x)
#define pp_div_base_o(d, rxy)		pp_div_base_o_i(d, pp_tuple_elem(3, 0, rxy), pp_tuple_elem(3, 1, rxy), pp_tuple_elem(3, 2, rxy))
#define pp_div_base_o_i(d, r, x, y) (pp_inc(r), pp_sub_d(d, x, y), y)
#define pp_div_base_d(d, x, y)		pp_while_ ## d(pp_div_base_p, pp_div_base_o, (0, x, y))

#define pp_div_impl(x, y)			pp_tuple_elem(3, 0, pp_div_base(x, y))
#define pp_div_d_impl(d, x, y)		pp_tuple_elem(3, 0, pp_div_base_d(d, x, y))
