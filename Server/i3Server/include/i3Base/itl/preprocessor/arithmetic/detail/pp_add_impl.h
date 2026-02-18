#pragma once


#define pp_add_impl(x, y)				pp_tuple_elem(2, 0, pp_while(pp_add_p, pp_add_o, (x, y)))
#define pp_add_p(d, xy)					pp_tuple_elem(2, 1, xy)
#define pp_add_o(d, xy)					pp_add_o_i xy
#define pp_add_o_i(x, y)				(pp_inc(x), pp_dec(y))
#define pp_add_d_impl(d, x, y)			pp_tuple_elem(2, 0, pp_while_ ## d(pp_add_p, pp_add_o, (x, y)))

