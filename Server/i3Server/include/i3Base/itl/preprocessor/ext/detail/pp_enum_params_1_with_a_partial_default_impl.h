#pragma once

#define pp_enum_params_1_with_a_partial_default_impl(total_count, no_def_count, param, def) pp_enum_params_1(no_def_count, param) pp_comma_if(pp_and(no_def_count, pp_sub(total_count, no_def_count))) \
	pp_enum_params_1_from_to_with_a_default(no_def_count, total_count, param, def)
