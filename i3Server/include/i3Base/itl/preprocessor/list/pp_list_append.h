#pragma once

#include "pp_list_fold_right.h"

#include "detail/pp_list_append_impl.h"

#define pp_list_append(a, b)		pp_list_append_impl(a, b)
#define pp_list_append(d, a, b)		pp_list_append_d_impl(d, a, b)
