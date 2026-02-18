#pragma once

#define pp_enum_shifted_binary_params_impl(count, p1, p2)	pp_repeat_impl(pp_dec(count), pp_enum_shifted_binary_params_m, (p1, p2))
#define pp_enum_shifted_binary_params_m(z, n, pp)			pp_enum_shifted_binary_params_m_i(z, n, pp_tuple_elem(2, 0, pp), pp_tuple_elem(2, 1, pp))
#define pp_enum_shifted_binary_params_m_i(z, n, p1, p2)		pp_comma_if(n) pp_cat(p1, pp_inc(n)) pp_cat(p2, pp_inc(n))
#define pp_enum_shifted_binary_params_z_impl(z, count, p1, p2)	pp_repeat_ ## z(pp_dec(count), pp_enum_shifted_binary_params_m, (p1, p2))
