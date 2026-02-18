#pragma once

#include "pp_list_enum.h"

#include "detail/pp_list_to_tuple_impl.h"

#define pp_list_to_tuple(list)			pp_list_to_tuple_impl(list)
#define pp_list_to_tuple_r(r, list)		pp_list_to_tuple_r_impl(r, list)
