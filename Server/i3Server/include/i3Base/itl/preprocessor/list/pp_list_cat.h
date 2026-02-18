#pragma once

#include "pp_list_adt.h"
#include "pp_list_fold_left.h"		// pp_catÆ÷ÇÔ..

#include "detail/pp_list_cat_impl.h"

#define pp_list_cat(list)		pp_list_cat_impl(list)
#define pp_list_cat_d(d, list)	pp_list_cat_d_impl(list)
