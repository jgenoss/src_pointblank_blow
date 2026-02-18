#pragma once

#define pp_enum_shifted_params_impl(count, param)	pp_repeat_impl(pp_dec(count), pp_enum_shifted_params_m, param)
#define pp_enum_shifted_params_m(z, n, param)		pp_comma_if(n) pp_cat(param, pp_inc(n))
#define pp_enum_shifted_params_z_impl(z, count, param) pp_repeat_ ## z(pp_dec(count), pp_enum_shifted_params_m, param)
