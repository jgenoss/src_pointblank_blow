#pragma once

#define pp_repeat_params_1_impl(count, param)		pp_repeat_impl(count, pp_repeat_params_1_m, param)
#define pp_repeat_params_1_m(z, n, param)			pp_cat(param, pp_inc(n))
