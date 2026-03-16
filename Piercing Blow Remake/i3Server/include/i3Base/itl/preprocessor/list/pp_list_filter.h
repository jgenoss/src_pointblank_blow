#pragma once

#include "../control/pp_if.h"
#include "pp_list_fold_right.h"
#include "../tuple/pp_tuple_elem.h"

#include "detail/pp_list_filter_impl.h"

#define pp_list_filter(pred, data, list)		pp_list_filter_impl(pred, data, list)
#define pp_list_filter_d(d, pred, data, list)	pp_list_filter_d_impl(d, pred, data, list)
