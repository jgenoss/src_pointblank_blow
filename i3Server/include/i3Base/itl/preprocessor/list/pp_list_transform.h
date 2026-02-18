#pragma once

#include "pp_list_fold_right.h"
#include "../tuple/pp_tuple_elem.h"

#include "detail/pp_list_transform_impl.h"

#define pp_list_transform(op, data, list)		pp_list_transform_impl(op, data, list)
#define pp_list_transform_d(d, op, data, list)	pp_list_transform_d_impl(d, op, data, list)
