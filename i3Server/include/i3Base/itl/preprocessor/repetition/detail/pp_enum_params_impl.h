#pragma once

#define pp_enum_params_impl(count, param)			pp_repeat_impl(count, pp_enum_params_m, param)
#define pp_enum_params_m(z, n, param)				pp_comma_if(n)	param ## n
#define pp_enum_params_z_impl(z, count, param)		pp_repeat_ ## z(count, pp_enum_params_m, param)
