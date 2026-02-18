#pragma once

#define pp_enum_trailing_binary_params_1_impl(count, p1, p2)	pp_repeat_impl(count, pp_enum_trailing_binary_params_1_m, (p1, p2))
#define pp_enum_trailing_binary_params_1_m(z, n, pp)			pp_enum_trailing_binary_params_1_m_i(z, n, pp_tuple_elem(2, 0, pp), pp_tuple_elem(2, 1, pp))
#define pp_enum_trailing_binary_params_1_m_i(z, n, p1, p2)		, pp_cat(p1, pp_inc(n)) pp_cat(p2, pp_inc(n))
