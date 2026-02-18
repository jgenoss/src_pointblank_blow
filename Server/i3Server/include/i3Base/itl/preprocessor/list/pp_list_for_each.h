#pragma once

#include "pp_list_for_each_i.h"		// pp_tuple_elem, pp_tuple_rem Æ÷ÇÔ

#include "detail/pp_list_for_each_impl.h"

#define pp_list_for_each(macro, data, list)			pp_list_for_each_impl(macro, data, list)
#define pp_list_for_each_r(r, macro, data, list)	pp_list_for_each_r_impl(r, macro, data, list)
