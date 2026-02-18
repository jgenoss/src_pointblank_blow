#pragma once

#define pp_enum_params_1_impl(count, param)			pp_repeat_impl(count, pp_enum_params_1_m, param)
#define pp_enum_params_1_m(z, n, param)				pp_comma_if(n)  pp_cat(param, pp_inc(n))
