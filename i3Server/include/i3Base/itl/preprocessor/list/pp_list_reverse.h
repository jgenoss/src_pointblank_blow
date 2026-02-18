#pragma once

#include "pp_list_fold_left.h"

#include "detail/pp_list_reverse_impl.h"

#define pp_list_reverse(list)				pp_list_reverse_impl(list)
#define pp_list_reverse_d(d, list)			pp_list_reverse_d_impl(d, list)
