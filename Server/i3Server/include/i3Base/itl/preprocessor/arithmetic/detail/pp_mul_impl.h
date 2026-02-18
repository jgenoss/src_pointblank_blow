#pragma once

#define pp_mul_impl(x, y)				pp_tuple_elem(3, 0, pp_while(pp_mul_p, pp_mul_o, (0, x, y)))
#define pp_mul_p(d, rxy)				pp_tuple_elem(3, 2, rxy)
#define pp_mul_o(d, rxy)				pp_mul_o_i(d, pp_tuple_elem(3, 0, rxy), pp_tuple_elem(3, 1, rxy), pp_tuple_elem(3, 2, rxy))
#define pp_mul_o_i(d, r, x, y)			(pp_add_d(d, r, x), x, pp_dec(y))
#define pp_mul_d_impl(d, x, y)			pp_tuple_elem(3, 0, pp_while_ ## d(pp_mul_p, pp_mul_o, (0, x, y)))
