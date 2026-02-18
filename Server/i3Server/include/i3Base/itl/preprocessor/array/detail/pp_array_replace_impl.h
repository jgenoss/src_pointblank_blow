#pragma once

#define pp_array_replace_impl(array, i, elem)	pp_array_replace_i(pp_deduce_d(), array, i, elem)
#define pp_array_replace_i(d, array, i, elem) pp_array_replace_d_impl(d, array, i, elem)

#define pp_array_replace_d_impl(d, array, i, elem) pp_tuple_elem(5, 3, pp_while_ ## d(pp_array_replace_p, pp_array_replace_o, (0, i, elem, (0, ()), array)))
#define pp_array_replace_p(d, state) pp_not_equal(pp_tuple_elem(5, 0, state), pp_array_size(pp_tuple_elem(5, 4, state)))
#define pp_array_replace_o(d, state) pp_array_replace_o_i(pp_tuple_elem(5, 0, state), pp_tuple_elem(5, 1, state), pp_tuple_elem(5, 2, state), pp_tuple_elem(5, 3, state), pp_tuple_elem(5, 4, state))
#define pp_array_replace_o_i(n, i, elem, res, arr) (pp_inc(n), i, elem, pp_array_push_back(res, pp_iif(pp_not_equal(n, i), pp_array_elem(n, arr), elem)), arr)

