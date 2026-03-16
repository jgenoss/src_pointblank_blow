#pragma once

#include "../arithmetic/pp_inc.h"
#include "pp_list_adt.h"
#include "../repetition/pp_for.h"
#include "../tuple/pp_tuple_elem.h"

#include "detail/pp_list_for_each_i_impl.h"

#define pp_list_for_each_i(macro, data, list)			pp_list_for_each_i_impl(macro, data, list)
#define pp_list_for_each_i_r(r, macro, data, list)		pp_list_for_each_i_r_impl(r, macro, data, list)
