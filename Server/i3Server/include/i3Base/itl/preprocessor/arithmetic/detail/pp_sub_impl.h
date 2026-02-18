#pragma once


#define pp_sub_impl(x, y)				pp_tuple_elem(2, 0, pp_while(pp_sub_p, pp_sub_o, (x, y)))
#define pp_sub_p(d, xy)					pp_tuple_elem(2, 1, xy)
#define pp_sub_o(d, xy)					pp_sub_o_i xy
#define pp_sub_o_i(x, y)				(pp_dec(x), pp_dec(y))
#define pp_sub_d_impl(d, x, y)			pp_tuple_elem(2, 0, pp_while_ ## d(pp_sub_p, pp_sub_o, (x, y)))

