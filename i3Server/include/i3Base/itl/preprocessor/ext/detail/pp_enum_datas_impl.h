#pragma once

#define pp_enum_datas_impl(count, data)				pp_repeat_impl(count, pp_enum_datas_m, data)
#define pp_enum_datas_m(z, n, data)					pp_comma_if(n)	data
