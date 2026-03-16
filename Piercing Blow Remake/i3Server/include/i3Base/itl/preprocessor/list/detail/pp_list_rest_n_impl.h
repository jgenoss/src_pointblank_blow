#pragma once

#define pp_list_rest_n_impl(count, list) pp_tuple_elem(2, 0, pp_while(pp_list_rest_n_p, pp_list_rest_n_o, (list, count)))

#define pp_list_rest_n_p(d, lc) pp_tuple_elem(2, 1, lc)

#define pp_list_rest_n_o(d, lc) (pp_list_rest(pp_tuple_elem(2, 0, lc)), pp_dec(pp_tuple_elem(2, 1, lc)))

#define pp_list_rest_n_d_impl(d, count, list) pp_tuple_elem(2, 0, pp_while_ ## d(pp_list_rest_n_p, pp_list_rest_n_o, (list, count)))

