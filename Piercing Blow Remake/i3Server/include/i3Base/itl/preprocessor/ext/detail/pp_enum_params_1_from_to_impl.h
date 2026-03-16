#pragma once

#define pp_enum_params_1_from_to_impl(first, last, param)		pp_repeat_from_to(first, last, pp_enum_params_1_from_to_m, (first, param) )
#define pp_enum_params_1_from_to_m(z, n, fp)			pp_enum_params_1_from_to_m_i(z, n, pp_tuple_elem(2, 0, fp), pp_tuple_elem(2, 1, fp))
#define pp_enum_params_1_from_to_m_i(z, n, f, p)		pp_comma_if(pp_not_equal(n, f)) pp_cat(p, pp_inc(n))

