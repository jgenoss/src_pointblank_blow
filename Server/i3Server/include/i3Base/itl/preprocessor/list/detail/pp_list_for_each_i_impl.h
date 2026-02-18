#pragma once

#define pp_list_for_each_i_impl(macro, data, list) pp_list_for_each_i_i(macro, data, list)
#define pp_list_for_each_i_i(macro, data, list) pp_for((macro, data, list, 0), pp_list_for_each_i_p, pp_list_for_each_i_o, pp_list_for_each_i_m)

#define pp_list_for_each_i_p(r, x) pp_list_is_cons(pp_tuple_elem(4, 2, x))

#define pp_list_for_each_i_o(r, x) pp_list_for_each_i_o_d x
#define pp_list_for_each_i_o_d(m, d, l, i) (m, d, pp_list_rest(l), pp_inc(i))

#define pp_list_for_each_i_m(r, x) pp_list_for_each_i_m_d(r, pp_tuple_elem(4, 0, x), pp_tuple_elem(4, 1, x), pp_tuple_elem(4, 2, x), pp_tuple_elem(4, 3, x))

#define pp_list_for_each_i_m_d(r, m, d, l, i) m(r, d, i, pp_list_first(l))

#define pp_list_for_each_i_r_impl(r, macro, data, list) pp_for_ ## r((macro, data, list, 0), pp_list_for_each_i_p, pp_list_for_each_i_o, pp_list_for_each_i_m)

