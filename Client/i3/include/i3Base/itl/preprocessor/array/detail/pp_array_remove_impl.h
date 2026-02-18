#pragma once

#define pp_array_remove_impl(array, i)		pp_array_remove_i(pp_deduce_d(), array, i)
#define pp_array_remove_i(d, array, i)		pp_array_remove_d_impl(d, array, i)

#define pp_array_remove_d_impl(d, array, i) pp_tuple_elem(4, 2, pp_while_ ## d(pp_array_remove_p, pp_array_remove_o, (0, i, (0, ()), array)))
#define pp_array_remove_p(d, st)			pp_not_equal(pp_tuple_elem(4, 0, st), pp_array_size(pp_tuple_elem(4, 3, st)))

#define pp_array_remove_o(d, st) pp_array_remove_o_i(pp_tuple_elem(4, 0, st), pp_tuple_elem(4, 1, st), pp_tuple_elem(4, 2, st), pp_tuple_elem(4, 3, st))
#define pp_array_remove_o_i(n, i, res, arr) (pp_inc(n), i, pp_iif(pp_not_equal(n, i), pp_array_push_back, res pp_tuple_eat_2)(res, pp_array_elem(n, arr)), arr)
