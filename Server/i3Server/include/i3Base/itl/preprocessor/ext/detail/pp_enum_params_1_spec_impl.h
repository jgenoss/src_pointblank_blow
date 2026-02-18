#pragma once

#define pp_enum_params_1_spec_impl(total_count, param_count, param, def_param) pp_enum_params_1(param_count, param) pp_comma_if(pp_and(param_count, pp_sub(total_count, param_count))) pp_enum_datas(pp_sub(total_count, param_count), def_param)
