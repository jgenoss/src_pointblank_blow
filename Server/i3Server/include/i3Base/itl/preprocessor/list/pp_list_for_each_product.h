#pragma once

#include "../control/pp_if.h"
#include "pp_list_adt.h"
#include "pp_list_to_tuple.h"
#include "../tuple/pp_tuple_elem.h"
#include "../tuple/pp_tuple_to_list.h"
#include "../tuple/pp_tuple_reverse.h"

#include "detail/pp_list_for_each_product_impl.h"

#define pp_list_for_each_product(macro, size, tuple)		pp_list_for_each_product_impl(macro, size, tuple)
#define pp_list_for_each_product_r(r, macro, size, tuple)	pp_list_for_each_product_r_impl(r, macro, size, tuple)
