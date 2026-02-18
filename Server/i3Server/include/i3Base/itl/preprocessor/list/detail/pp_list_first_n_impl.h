#pragma once

#define pp_list_first_n_impl(count, list) pp_list_reverse(pp_tuple_elem(3, 2, pp_while(pp_list_first_n_p, pp_list_first_n_o, (count, list, pp_nil))))

#define pp_list_first_n_p(d, data) pp_tuple_elem(3, 0, data)

#define pp_list_first_n_o(d, data) pp_list_first_n_o_d data

#define pp_list_first_n_o_d(c, l, nl) (pp_dec(c), pp_list_rest(l), (pp_list_first(l), nl))

#define pp_list_first_n_d_impl(d, count, list) pp_list_reverse_d(d, pp_tuple_elem(3, 2, pp_while_ ## d(pp_list_first_n_p, pp_list_first_n_o, (count, list, pp_nil))))
