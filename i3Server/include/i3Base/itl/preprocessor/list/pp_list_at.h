#pragma once

#include "pp_list_rest_n.h"		// pp_list_adt Æ÷ÇÔ..

#include "detail/pp_list_at_impl.h"

#define pp_list_at(list, index)				pp_list_at_impl(list, index)
#define pp_list_at_d(d, list, index)		pp_list_at_d_impl(d, list, index)
