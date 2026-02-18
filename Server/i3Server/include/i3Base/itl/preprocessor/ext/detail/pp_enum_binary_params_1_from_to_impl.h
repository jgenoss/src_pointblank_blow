#pragma once

#define pp_enum_binary_params_1_from_to_impl(first, last, p1, p2)	pp_repeat_from_to(first, last, pp_enum_binary_params_1_from_to_m, (first, p1, p2) )
#define pp_enum_binary_params_1_from_to_m(z, n, fpp) pp_enum_binary_params_1_from_to_m_i(z, n, pp_tuple_elem(3, 0, fpp), pp_tuple_elem(3, 1, fpp), pp_tuple_elem(3, 2, fpp))
#define pp_enum_binary_params_1_from_to_m_i(z, n, f, p1, p2) pp_comma_if(pp_not_equal(n, f)) pp_cat(p1, pp_inc(n))	pp_cat(p2, pp_inc(n))
