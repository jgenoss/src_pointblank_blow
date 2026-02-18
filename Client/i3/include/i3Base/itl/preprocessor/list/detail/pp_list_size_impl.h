#pragma once

#define pp_list_size_impl(list) pp_tuple_elem(2, 0, pp_while(pp_list_size_p, pp_list_size_o, (0, list)))
#define pp_list_size_p(d, rl)	pp_list_is_cons(pp_tuple_elem(2, 1, rl))
#define pp_list_size_o(d, rl)	(pp_inc(pp_tuple_elem(2, 0, rl)), pp_list_rest(pp_tuple_elem(2, 1, rl)))

#define pp_list_size_d_impl(d, list) pp_tuple_elem(2, 0, pp_while_ ## d(pp_list_size_p, pp_list_size_o, (0, list)))
