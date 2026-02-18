#pragma once

#define pp_list_for_each_impl(macro, data, list) pp_list_for_each_i(pp_list_for_each_o, (macro, data), list)

#define pp_list_for_each_o(r, md, i, elem) pp_list_for_each_o_d(r, pp_tuple_elem(2, 0, md), pp_tuple_elem(2, 1, md), elem)

#define pp_list_for_each_o_d(r, m, d, elem) m(r, d, elem)

#define pp_list_for_each_r_impl(r, macro, data, list) pp_list_for_each_i_r(r, pp_list_for_each_o, (macro, data), list)
