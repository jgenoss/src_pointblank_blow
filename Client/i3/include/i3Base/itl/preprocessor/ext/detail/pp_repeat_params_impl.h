#pragma once

#define pp_repeat_params_impl(count, param)			pp_repeat_impl(count, pp_repeat_params_m, param)
#define pp_repeat_params_m(z, n, param)				pp_cat(param, n)
